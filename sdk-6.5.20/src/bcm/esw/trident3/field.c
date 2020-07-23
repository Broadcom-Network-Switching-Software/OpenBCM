/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * File:       field.c
 * Purpose:    BCM56870 Field Processor functions.
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT3_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/tomahawk.h>
#include <bcm/field.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/udf.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident3.h>
#include "include/soc/esw/cancun.h"

#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif

#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
#include <bcm_int/common/xflow_macsec_cmn.h>
#endif

/* IFP Entry/Policy max width */
#define _BCM_FP_MEM_FIELD_BYTES  100

#if defined(BCM_HURRICANE4_SUPPORT)
extern uint8 ifp_hr4_action_profile_bitmap[48];
#endif
extern uint8 ifp_td3_action_profile_bitmap[44];

static soc_reg_t outer_tpid_r[4] = {
    PARSER1_ING_OUTER_TPID_0r,
    PARSER1_ING_OUTER_TPID_1r,
    PARSER1_ING_OUTER_TPID_2r,
    PARSER1_ING_OUTER_TPID_3r
};
static soc_reg_t inner_tpid_r[4] = {
    PARSER1_ING_INNER_TPID_0r,
    PARSER1_ING_INNER_TPID_1r,
    PARSER1_ING_INNER_TPID_2r,
    PARSER1_ING_INNER_TPID_3r
};
static soc_field_t classfldtbl[][7] = {
                       {SLICE_0_F1f, SLICE_0_F2f, SLICE_0_F3f, SLICE_0_F4f,
                                     SLICE_0_F6f, SLICE_0_F7f, SLICE_0_F8f},
                       {SLICE_1_F1f, SLICE_1_F2f, SLICE_1_F3f, SLICE_1_F4f,
                                     SLICE_1_F6f, SLICE_1_F7f, SLICE_1_F8f},
                       {SLICE_2_F1f, SLICE_2_F2f, SLICE_2_F3f, SLICE_2_F4f,
                                     SLICE_2_F6f, SLICE_2_F7f, SLICE_2_F8f},
                       {SLICE_3_F1f, SLICE_3_F2f, SLICE_3_F3f, SLICE_3_F4f,
                                     SLICE_3_F6f, SLICE_3_F7f, SLICE_3_F8f}
};
static soc_field_t classIdBfldtbl[4] =
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };
static soc_field_t dvpfldtbl[4] =
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };


#define _FP_TD3_ZONE_0_MATCH_ID_WIDTH 6
#define _FP_TD3_ZONE_1_MATCH_ID_WIDTH 11
#define _FP_TD3_ZONE_2_MATCH_ID_WIDTH 11
#define _FP_TD3_ZONE_3_MATCH_ID_WIDTH 11
#define _FP_TD3_ZONE_4_MATCH_ID_WIDTH 11

#define _FP_FLEX_OBJ_MAX         2
#define _FP_FLEX_OBJ_STR_SZ     40
#define _FP_FLEX_FLD_STR_SZ     40

/* Flex Qualifiers CEH data */
typedef struct _bcm_field_flex_qual_ceh_data_s {
    bcm_field_qualify_t         qual;
    uint8                       thirtytwo_bit_ext_valid;
    uint8                       sixteen_bit_ext_valid;
    uint8                       eight_bit_ext_valid;
    uint8                       four_bit_ext_valid;
    uint8                       presel_valid; /* (1 << 0) for IFP and EM
                                                 (1 << 1)  for FT */
    int8                        num_obj;
    char                        obj_str_arr[_FP_FLEX_OBJ_STR_SZ][_FP_FLEX_FLD_STR_SZ];
    char                        field_str_arr[_FP_FLEX_OBJ_STR_SZ][_FP_FLEX_FLD_STR_SZ];
    soc_cancun_ceh_field_info_t ceh_info[_FP_FLEX_OBJ_MAX];
    struct _bcm_field_flex_qual_ceh_data_s *next;
} _bcm_field_flex_qual_ceh_data_t;

typedef struct _bcm_field_flex_qual_ceh_db_s {
    _bcm_field_flex_qual_ceh_data_t *flex_data;
} _bcm_field_flex_qual_ceh_db_t;

_bcm_field_flex_qual_ceh_db_t fp_flex_db[BCM_MAX_NUM_UNITS];

typedef struct _flex_ceh_field_type_s {
    char  field_str[40];
} _flex_ceh_field_type_t;



int _field_ifp_base_offset_get(int unit,
                               soc_format_t fmt,
                               soc_field_t field,
                               uint32 *base_offset,
                               uint32 *field_pos)
{
    soc_field_info_t *field_info;

    field_info = soc_format_fieldinfo_get(unit, fmt, field);
    if (field_info == NULL) {
       return BCM_E_PARAM;
    }

    *base_offset = field_info->bp;

    switch (field) {
       case ZONE_0_MATCH_IDf:
           *field_pos = (fmt == IFP_L0_BUS_SECTION_16BITfmt) ? 36 : 22;
            break;
       case ZONE_1_MATCH_IDf:
           *field_pos = 35;
            break;
       case ZONE_2_MATCH_IDf:
           *field_pos = 34;
            break;
       case ZONE_3_MATCH_IDf:
           *field_pos = 33;
            break;
       case ZONE_4_MATCH_IDf:
           *field_pos = 32;
            break;
       case OPAQUE_1f:
           *field_pos = 42;
            break;
       case OPAQUE_2f:
           *field_pos = 43;
            break;
       case OPAQUE_3f:
           *field_pos = 44;
            break;
       case OPAQUE_4f:
           *field_pos = 45;
            break;
       case PARSER_1_FLEX_HVE_CHECK_RESULT_15_0f:
           *field_pos = 49;
            break;
       case PARSER_2_FLEX_HVE_CHECK_RESULT_15_0f:
           *field_pos = 48;
            break;
       case ZONE_1_MATCH_ID_7_0f:
           *field_pos = 21;
            break;
       case ZONE_2_MATCH_ID_7_0f:
           *field_pos = 20;
            break;
       case ZONE_3_MATCH_ID_7_0f:
           *field_pos = 19;
            break;
       case ZONE_4_MATCH_ID_7_0f:
           *field_pos = 18;
            break;
       case ZONE_2_MATCH_ID_11_8f:
           *field_pos = 36;
            break;
       case ZONE_4_MATCH_ID_11_8f:
           *field_pos = 34;
            break;
       case PKT_FLOW_ID_2f:
           *field_pos = 37;
            break;
       case PKT_FLOW_ID_1f:
           *field_pos = 38;
            break;
       case PKT_FLOW_ID_0f:
           *field_pos = 39;
            break;
       case UDF_1_CHUNK_5_4f:
           *field_pos = 10;
            break;
       case UDF_1_CHUNK_3_2f:
           *field_pos = 9;
            break;
       case UDF_2_CHUNK_5_4f:
           *field_pos = 7;
            break;
       case UDF_2_CHUNK_3_2f:
           *field_pos = 6;
            break;
       case UDF_2_CHUNK_1_0f:
           *field_pos = 5;
            break;
       default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_fp_flex_qual_ceh_db_insert(int unit, _bcm_field_flex_qual_ceh_data_t *flex_data)
{
    int rv = BCM_E_NONE;
    int i = 0;
    static _bcm_field_flex_qual_ceh_data_t *curr_flex_data = NULL;
    soc_cancun_ceh_field_info_t ceh_info[2];


    for (i = 0; i < flex_data->num_obj; i++) {
        rv = soc_cancun_ceh_obj_field_get(unit, flex_data->obj_str_arr[i],
                                          flex_data->field_str_arr[i], &ceh_info[i]);
        if (BCM_E_NOT_FOUND == rv) {
            return BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            return BCM_E_INTERNAL;
        }
    }
    if (fp_flex_db[unit].flex_data == NULL) {
        curr_flex_data = NULL;
    }

    if (curr_flex_data == NULL) {
        fp_flex_db[unit].flex_data = flex_data;
    } else {
        curr_flex_data->next = flex_data;
    }

    curr_flex_data = flex_data;

    return rv;
}

int
_bcm_field_td3_flex_qual_ceh_db_clear(int unit)
{
   _bcm_field_flex_qual_ceh_data_t *flex_data;

   flex_data = fp_flex_db[unit].flex_data;

   while (flex_data != NULL) {
      fp_flex_db[unit].flex_data = flex_data->next;
      sal_free(flex_data);
      flex_data = fp_flex_db[unit].flex_data;
   }

   fp_flex_db[unit].flex_data = NULL;

   return BCM_E_NONE;
}

#define _FP_QUAL_FLEX_QUAL_CEH_DATA_DECL                                          \
        _bcm_field_flex_qual_ceh_data_t *_flex_data_= NULL;


#define _FP_QUAL_FLEX_DB_ADD(_u_, _qual_, _32bit_ext_, _16bit_ext_, _8bit_ext_, _4bit_ext_,    \
                             _presel_valid_, _num_objs_, _obj1_str_, _obj2_str_,  \
                              _fld1_str_, _fld2_str_)                             \
        {                                                                         \
             /* Allocated stage qualifiers configuration array. */                \
             _FP_XGS3_ALLOC(_flex_data_, sizeof(_bcm_field_flex_qual_ceh_data_t), \
                             "Flex DB data Alloc");                               \
             if (_flex_data_ == NULL) {                                           \
                return BCM_E_MEMORY;                                              \
             }                                                                    \
                                                                                  \
             _flex_data_->qual = _qual_;                                          \
             _flex_data_->thirtytwo_bit_ext_valid = _32bit_ext_;                    \
             _flex_data_->sixteen_bit_ext_valid = _16bit_ext_;                    \
             _flex_data_->eight_bit_ext_valid = _8bit_ext_;                       \
             _flex_data_->four_bit_ext_valid = _4bit_ext_;                        \
             _flex_data_->presel_valid = _presel_valid_;                          \
             _flex_data_->num_obj = _num_objs_;                                   \
             sal_strcpy(_flex_data_->obj_str_arr[0], _obj1_str_);                 \
             sal_strcpy(_flex_data_->obj_str_arr[1], _obj2_str_);                 \
             sal_strcpy(_flex_data_->field_str_arr[0], _fld1_str_);               \
             sal_strcpy(_flex_data_->field_str_arr[1], _fld2_str_);               \
                                                                                  \
             _fp_flex_qual_ceh_db_insert(_u_, _flex_data_);                       \
             _flex_data_ = NULL;                                                  \
       }

int
_bcm_field_td3_flex_qual_ceh_db_init(int unit)
{
    int rv;
    int obj_ct;
    _bcm_field_flex_qual_ceh_data_t *flex_db;

    _FP_QUAL_FLEX_QUAL_CEH_DATA_DECL;

    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpIdentifier,             1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_4",   "\0",        "IPV4_IDENTIFIER",       "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyMacSecSecTagCbit,         1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_4",   "\0",        "SECTAG_C_BIT",       "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyMacSecSecTagEbit,  1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_4",   "\0",        "SECTAG_E_BIT",       "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyHiGigGbpSrcId,            1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_5",   "\0",        "GBP_SID",               "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifySysHdrLocalDstport,       1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_5",   "\0",        "SOBMH_LOCAL_DEST_PORT", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyTimeStampTxPktType,       1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_5",   "\0",        "SOBMH_TIMESTAMP_HDR_OFFSET", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyLoopbackIfaSwitchCopyDrop,1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_5",   "\0",        "IFA_SW_COPY_DROP_INDICATOR", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaGlobalNameSpace,       1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_5",   "\0",        "IFA_GNS", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaLocalNameSpace,        1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_2",   "\0",        "IFA_LNS", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaCurrentLength,        1, 0, 0, 0, 0, 1, "UDF_1_CHUNK_2",   "\0",        "IFA_CURRENT_LENGTH", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaRequestVector,        1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_3",   "\0",        "IFA_REQUEST_VECTOR", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaFlags,                1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_4",   "\0",        "IFA_FLAGS", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaMaxLength,             1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_4",   "\0",       "IFA_MAX_LENGTH", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaActionVector,          1, 0, 0, 0, 0, 1, "UDF_2_CHUNK_3",   "\0",       "IFA_ACTION_VECTOR", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifySysHdrType,               0, 1, 1, 0, 2, 1, "Z0_MATCH_ID",     "\0",        "COE",               "\0");
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching)))) {
        _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifySubportTagPresent,        0, 1, 1, 0, 3, 1, "Z1_MATCH_ID", "\0",          "SUBPORT_TAG",    "\0");
    }
#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_hx5_flex_gbp_support)) {
        /*
         * Presel Valid bitmap representation is like 0th bit for TD3 and 1st bit for HX5.
         * Presel Valid bitmap is set to 0x2 as they are not supported in TD3 currently.
         * When supported need to mark it as 0x3.
         */
        _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGbpPresent,               0, 1, 1, 0, 2, 1, "Z1_MATCH_ID", "\0",          "GBP",            "\0");
    }
#endif /* BCM_HELIX5_SUPPORT */
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyNetworkTagType,           0, 1, 1, 0, 3, 1, "Z1_MATCH_ID", "\0",          "VNTAG",          "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIncomingVlanFormat,       0, 1, 1, 0, 3, 1, "Z1_MATCH_ID", "\0",          "OTAG_ITAG",      "\0");
    if (soc_feature(unit, soc_feature_CnTag)) {
        _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyCnTagPresent,             0, 1, 1, 0, 3, 1, "Z1_MATCH_ID", "\0",          "CNTAG",          "\0");
    }
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyL2PktType,                0, 1, 1, 0, 3, 1, "Z1_MATCH_ID", "\0",          "ETH",            "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyPktType,                  0, 1, 1, 0, 3, 1, "Z2_MATCH_ID", "\0",          "MPLS_1_LABEL_WITH_CW",    "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyMplsCwPresent,            0, 1, 1, 0, 1, 2, "Z2_MATCH_ID", "Z2_MATCH_ID", "MPLS",           "MPLS_CW");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyMplsMcastEthertypePresent, 0, 1, 0, 1, 1, 2, "Z2_MATCH_ID", "Z2_MATCH_ID", "MPLS",           "MPLS_WITH_MCAST_ETYPE");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpv4OptionHdrType,        0, 1, 1, 0, 3, 2, "Z2_MATCH_ID", "Z2_MATCH_ID", "IPV4",           "AH");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpv6ExtensionHdrType,     0, 1, 1, 0, 3, 2, "Z2_MATCH_ID", "Z2_MATCH_ID", "IPV6",           "AH");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOverlayVlanFormat,        0, 1, 1, 0, 3, 1, "Z3_MATCH_ID", "\0",          "OTAG_ITAG",      "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOverlayL2PktType,         0, 1, 1, 0, 3, 1, "Z3_MATCH_ID", "\0",          "ETH",            "\0");
#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_hx5_flex_gbp_support)) {
        _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyHiGigGbpPresent,        0, 1, 1, 0, 2, 1, "Z3_MATCH_ID", "\0",          "GBP",            "\0");
    }
#endif /* BCM_HELIX5_SUPPORT */
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGbpSrcId,                 0, 1, 1, 0, 2, 1, "OPAQUE_1",    "\0",          "GBP_SID",        "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGbpDstId,                 0, 1, 1, 0, 2, 1, "OPAQUE_2",    "\0",          "GBP_DID",        "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOverlayPktType,           0, 1, 1, 0, 2, 1, "Z4_MATCH_ID", "\0",          "ARP",            "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOverlayIpv4OptionHdrType, 0, 1, 0, 0, 2, 2, "Z4_MATCH_ID", "Z4_MATCH_ID", "IPV4",           "AH");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOverlayIpv6ExtensionHdrType, 0, 1, 0, 0, 2, 2, "Z4_MATCH_ID", "Z4_MATCH_ID", "IPV6",           "AH");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyNshServiceIndex,          0, 1, 0, 0, 0, 1, "OPAQUE_2",    "\0",          "SI",             "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyNshServicePathId,         0, 1, 0, 0, 0, 2, "OPAQUE_2",    "OPAQUE_1",    "SPID_7_0",       "SPID_23_8");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaCurrentLengthEqualToZero, 0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IFA_CURRENT_LEN_EQ_TO_ZERO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaHopLimitEqualToZero,   0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IFA_HOP_LIMIT_EQ_TO_ZERO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIfaVersion2,              0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IFA_VERSION_EQ_TO_TWO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpFragPkt,                0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IP_FRAGMENTS", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyOuterPriTaggedPkt,        0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "OUTER_PRI_TAGGED", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpInIpPkt,                0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IP_IN_IP", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIntPktVersionOne,         0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "INT_VERSION_EQ_TO_ONE", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIOAMNameSpaceIdMatch,     0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IOAM_NAMESPACE_ID_MATCHED", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIntPktOverflowed,         0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "INT_OVERFLOW_FLAG_EQ_TO_ZERO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIOAMPktOverflowed,        0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IOAM_OVERFLOW_FLAG_EQ_TO_ZERO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIntPktFinalHop,           0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "INT_HOP_LIMIT_MORE_THAN_HOP_COUNT", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIOAMRemainingLenValid,    0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IOAM_REMAINING_LEN_GT_OR_EQ_EIGHT", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIntPktLengthValid,        0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "INT_MAX_LEN_MORE_THAN_CUR_LEN", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIntPktType,               0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "INT_MSG_TYPE_EQ_TO_REQUEST", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpmcV4Pkt,                0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "MACDA_V4_IPMC", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpmcV6Pkt,                0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "MACDA_V6_IPMC", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpChecksumOkPkt,          0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "CHECKSUMOK", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyNetworkTagMulticastPkt,   0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "VNTAG_ETAG_MCAST", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyIpTtlZeroPkt,             0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "IP_PKT_TTL_ZERO", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyBfdPktVersionOne,         0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "BFD_VERSION", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGenevePktVersionZero,     0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "GENEVE_VERSION_CHECK", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGpePktVersionZero,        0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "GPE_VERSION_CHECK", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGenevePktWithOam,         0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "GENEVE_OAM_BIT", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyGpePktWithOam,            0, 1, 0, 0, 0, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "GPE_OAM_FLAG", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyVxlanGbpPresent,          0, 1, 0, 0, 2, 1, "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0", "\0", "VXLAN_GBP", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyInnerBfdPktVersionOne,    0, 1, 0, 0, 0, 1, "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0", "\0", "BFD_VERSION", "\0");
    _FP_QUAL_FLEX_DB_ADD(unit, bcmFieldQualifyTunnelTerminated,         0, 1, 0, 0, 1, 1, "PKT_FLOW_ID_2",                       "\0", "TUNNEL_TERMINATED", "\0");

    flex_db = fp_flex_db[unit].flex_data;

    for (; flex_db != NULL; flex_db = flex_db->next) {
       for (obj_ct = 0; obj_ct < flex_db->num_obj; obj_ct++) {
           rv = soc_cancun_ceh_obj_field_get(unit, flex_db->obj_str_arr[obj_ct],
                                             flex_db->field_str_arr[obj_ct],
                                             &flex_db->ceh_info[obj_ct]);
           if (SOC_FAILURE(rv)) {
               if (rv == SOC_E_NOT_FOUND) {
                  LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META("CEH - OBJ:%s FIELD:%s for"
                                        " qual[%d] not Found!\n\r"),
                                        flex_db->obj_str_arr[obj_ct],
                                        flex_db->field_str_arr[obj_ct],
                                        flex_db->qual));
                  continue;
               }
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("CEH read error for OBJ:%s FIELD:%s \n\r"),
                                        flex_db->obj_str_arr[obj_ct],
                                        flex_db->field_str_arr[obj_ct]));
               return rv;
           }
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "OBJ:%s FIELD:%s offset:%d width:%d value:%d\n\r"),
                                        flex_db->obj_str_arr[obj_ct],
                                        flex_db->field_str_arr[obj_ct],
                                        flex_db->ceh_info[obj_ct].offset,
                                        flex_db->ceh_info[obj_ct].width,
                                        flex_db->ceh_info[obj_ct].value));
       }
    }

    return BCM_E_NONE;
}

/*
 * Function: _field_td3_flex_presel_qual_init
 * Purpose:  Initialize Field Flex Presel Stage Qualifiers.
 */
STATIC int
_field_td3_flex_presel_qual_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;
    uint8 valid_flags = 0;
    uint8 ct, num_flows = 0;
    uint32 offset0, width0;
    uint32 offset1, width1;
    uint32 z0_offset = 0;
    uint32 key_start_offset = 0;
    uint32 match_id_cont_0_offset = 0;
    uint32 match_id_cont_1_offset = 0;
    uint32 match_id_cont_2_offset = 0;
    uint32 pkt_flow_id_2_offset = 0;
    uint32 flow_type[4] = {0};
    uint32 flow_offset[4] = {0};
    soc_field_info_t *field_info = NULL;
    _bcm_field_flex_qual_ceh_data_t *flex_db = NULL;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    int rv;
    uint32 match_id_offset = 0, z4_offset = 0;
    soc_cancun_ceh_field_info_t ceh_info;
#endif

    _FP_QUAL_DECL;

    if (fp_flex_db[unit].flex_data == NULL) {
       return BCM_E_INTERNAL;
    }

    switch(stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_EXACTMATCH:
        case _BCM_FIELD_STAGE_INGRESS:
            valid_flags = (1 << 0);
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                mem = EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
            } else {
                mem = IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
            }
            field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            key_start_offset = field_info->bp;

            field_info = soc_mem_fieldinfo_get(unit, mem, MATCH_ID_CONT_0f);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            match_id_cont_0_offset = field_info->bp - key_start_offset;

            field_info = soc_mem_fieldinfo_get(unit, mem, MATCH_ID_CONT_1f);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            match_id_cont_1_offset = field_info->bp - key_start_offset;

            field_info = soc_mem_fieldinfo_get(unit, mem, MATCH_ID_CONT_2f);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            match_id_cont_2_offset = field_info->bp - key_start_offset;

            field_info = soc_mem_fieldinfo_get(unit, mem, PKT_FLOW_ID_2f);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            pkt_flow_id_2_offset = field_info->bp - key_start_offset;
            break;
#if defined(BCM_FLOWTRACKER_SUPPORT)
        case _BCM_FIELD_STAGE_FLOWTRACKER:
            valid_flags = (1 << 1);
            mem = BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm;

            field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            key_start_offset = field_info->bp;

            field_info = soc_mem_fieldinfo_get(unit, mem, MATCH_IDf);
            if (field_info == NULL) {
                return BCM_E_PARAM;
            }
            match_id_offset = field_info->bp - key_start_offset;

            /* Get Offsets for Zone0/1/2/3/4 */
            sal_memset(&ceh_info, 0, sizeof(soc_cancun_ceh_field_info_t));
            rv = soc_cancun_ceh_obj_field_get(unit, "MATCH_ID",
                    "Z0_MATCH_ID",
                    &ceh_info);
            BCM_IF_ERROR_RETURN(rv);
            z0_offset = match_id_offset + ceh_info.offset;

            sal_memset(&ceh_info, 0, sizeof(soc_cancun_ceh_field_info_t));
            rv = soc_cancun_ceh_obj_field_get(unit, "MATCH_ID",
                    "Z1_MATCH_ID",
                    &ceh_info);
            BCM_IF_ERROR_RETURN(rv);
            match_id_cont_0_offset = match_id_offset + ceh_info.offset;

            sal_memset(&ceh_info, 0, sizeof(soc_cancun_ceh_field_info_t));
            rv = soc_cancun_ceh_obj_field_get(unit, "MATCH_ID",
                    "Z2_MATCH_ID",
                    &ceh_info);
            BCM_IF_ERROR_RETURN(rv);
            match_id_cont_1_offset = match_id_offset + ceh_info.offset;

            sal_memset(&ceh_info, 0, sizeof(soc_cancun_ceh_field_info_t));
            rv = soc_cancun_ceh_obj_field_get(unit, "MATCH_ID",
                    "Z3_MATCH_ID",
                    &ceh_info);
            BCM_IF_ERROR_RETURN(rv);
            match_id_cont_2_offset = match_id_offset + ceh_info.offset;

            sal_memset(&ceh_info, 0, sizeof(soc_cancun_ceh_field_info_t));
            rv = soc_cancun_ceh_obj_field_get(unit, "MATCH_ID",
                    "Z4_MATCH_ID",
                    &ceh_info);
            BCM_IF_ERROR_RETURN(rv);
            z4_offset = match_id_offset + ceh_info.offset;
            break;
#endif
        default:
            return BCM_E_INTERNAL;
    }

    flex_db = fp_flex_db[unit].flex_data;

    for (; flex_db != NULL; flex_db = flex_db->next) {
        if (!(flex_db->presel_valid & valid_flags)) {
            continue;
        }

        width0 = width1 = 0;
        offset0 = offset1 = 0;

        if ((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) ||
            (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH)) {

            /* Population of Zone Match IDs in MATCH_ID_CONTAINERS are dynamic,
             * it gets updated based on the packet flows.
             *
             * IFP_LOGICAL_TABLE_SELECT MATCH_ID_0_CONT MATCH_ID_1_CONT MATCH_ID_2_CONT
             * ------------------------ --------------- --------------- ---------------
             * Access Flows/Transit flow ZONE_1          ZONE_2          ZONE_4
             * L3 Tunnel Termination     ZONE_1          ZONE_2          ZONE_4
             * L2 Tunnel Termination     ZONE_2          ZONE_3          ZONE_4
             */
            /* Assuming object 1 and object 2 has same obj_str_arr */
            if (sal_strcmp(flex_db->obj_str_arr[0], "Z0_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = z0_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z1_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE;
                flow_offset[0] = match_id_cont_0_offset + flex_db->ceh_info[0].offset;
                flow_type[1] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3;
                flow_offset[1] = match_id_cont_0_offset + flex_db->ceh_info[0].offset;
                num_flows = 2;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z2_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE;
                flow_offset[0] = match_id_cont_1_offset + flex_db->ceh_info[0].offset;
                flow_type[1] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3;
                flow_offset[1] = match_id_cont_1_offset + flex_db->ceh_info[0].offset;
                flow_type[2] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2;
                flow_offset[2] = match_id_cont_0_offset + flex_db->ceh_info[0].offset;
                num_flows = 3;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z3_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2;
                flow_offset[0] = match_id_cont_1_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z4_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE;
                flow_offset[0] = match_id_cont_2_offset + flex_db->ceh_info[0].offset;
                flow_type[1] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3;
                flow_offset[1] = match_id_cont_2_offset + flex_db->ceh_info[0].offset;
                flow_type[2] = _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2;
                flow_offset[2] = match_id_cont_2_offset + flex_db->ceh_info[0].offset;
                num_flows = 3;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "PKT_FLOW_ID_2") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = pkt_flow_id_2_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else {
                continue;
            }
#if defined(BCM_FLOWTRACKER_SUPPORT)
        } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {

            /* Assuming object 1 and object 2 has same obj_str_arr */
            if (sal_strcmp(flex_db->obj_str_arr[0], "Z0_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = z0_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z1_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = match_id_cont_0_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z2_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = match_id_cont_1_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z3_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = match_id_cont_2_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else if (sal_strcmp(flex_db->obj_str_arr[0], "Z4_MATCH_ID") == 0) {
                flow_type[0] = _BCM_FIELD_QUAL_OFFSET_DEFAULT;
                flow_offset[0] = z4_offset + flex_db->ceh_info[0].offset;
                num_flows = 1;
            } else {
                continue;
            }
#endif
        }

        width0 = flex_db->ceh_info[0].width;

        for (ct = 0; ct < num_flows; ct++) {
            offset0 = flow_offset[ct];
            if (flex_db->num_obj == 1) {
                _FP_PRESEL_QUAL_ADD(unit, stage_fc, flex_db->qual,
                            flow_type[ct], _bcmFieldSliceSelDisable,
                            0, _bcmFieldQualifierTypePresel,
                            offset0, width0);
            } else if (flex_db->num_obj == 2) {
               offset1 = offset0 + flex_db->ceh_info[1].offset;
               width1 = flex_db->ceh_info[1].width;

               _FP_PRESEL_QUAL_TWO_ADD(unit, stage_fc, flex_db->qual,
                            flow_type[ct], _bcmFieldSliceSelDisable,
                            0, _bcmFieldQualifierTypePresel,
                            offset0, width0,
                            offset1, width1);
            }
        }

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "%20s :: offset0:%d width0:%d, offset1:%d width1:%d\n\r"),
                  _field_qual_name(flex_db->qual), offset0, width0, offset1, width1));
    }

    return BCM_E_NONE;
}

/*
 * Function: _field_td3_flex_extractor_qual_init
 * Purpose:  Initialize Field Flex Ingress Stage Qualifiers.
 */
STATIC int
_field_td3_flex_extractor_qual_init(int unit, _field_stage_t *stage_fc)
{
    int    obj_ct, extr, rv;
    uint8  valid_field = 0;
    uint32 z0_match_id_pos[3] = {0};
    uint32 z1_match_id_pos[3] = {0};
    uint32 z2_match_id_pos[3] = {0};
    uint32 z3_match_id_pos[3] = {0};
    uint32 z4_match_id_pos[3] = {0};
    uint32 opaque1_pos = 0;
    uint32 opaque2_pos = 0;
    uint32 opaque3_pos = 0;
    uint32 opaque4_pos = 0;
    uint32 parser1_flex_hve_chk_pos = 0;
    uint32 parser2_flex_hve_chk_pos = 0;
    uint32 z0_match_id_offset[3] = {0};
    uint32 z1_match_id_offset[3] = {0};
    uint32 z2_match_id_offset[3] = {0};
    uint32 z3_match_id_offset[3] = {0};
    uint32 z4_match_id_offset[3] = {0};
    uint32 opaque1_offset = 0;
    uint32 opaque2_offset = 0;
    uint32 opaque3_offset = 0;
    uint32 opaque4_offset = 0;
    uint32 parser1_flex_hve_chk_offset = 0;
    uint32 parser2_flex_hve_chk_offset = 0;
    uint32 field_offset[2] = {0};
    uint32 field_pos[2] = {0};
    uint32 pkt_flow_id_2_pos = 0;
    uint32 pkt_flow_id_2_offset = 0;
    uint32 udf_1_chunk_5_4_pos = 0;
    uint32 udf_1_chunk_5_4_offset = 0;
    uint32 udf_1_chunk_3_2_pos = 0;
    uint32 udf_1_chunk_3_2_offset = 0;
    uint32 udf_2_chunk_5_4_pos = 0;
    uint32 udf_2_chunk_5_4_offset = 0;
    uint32 udf_2_chunk_3_2_pos = 0;
    uint32 udf_2_chunk_3_2_offset = 0;
    uint32 udf_2_chunk_1_0_pos = 0;
    uint32 udf_2_chunk_1_0_offset = 0;
    _bcm_field_flex_qual_ceh_data_t *flex_db = NULL;
    bcmi_keygen_qual_flags_bmp_t qual_flags;
    bcmi_keygen_ext_section_t ext_sel = BCMI_KEYGEN_EXT_SECTION_DISABLE;
    BCMI_KEYGEN_QUAL_CFG_DECL;

    BCMI_KEYGEN_FUNC_ENTER(unit);


    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    ZONE_0_MATCH_IDf,
                                    &z0_match_id_offset[0], &z0_match_id_pos[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    ZONE_1_MATCH_IDf,
                                    &z1_match_id_offset[0], &z1_match_id_pos[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    ZONE_2_MATCH_IDf,
                                    &z2_match_id_offset[0],
                                    &z2_match_id_pos[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    ZONE_3_MATCH_IDf,
                                    &z3_match_id_offset[0], &z3_match_id_pos[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    ZONE_4_MATCH_IDf,
                                    &z4_match_id_offset[0], &z4_match_id_pos[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    OPAQUE_1f,
                                    &opaque1_offset, &opaque1_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    OPAQUE_2f,
                                    &opaque2_offset, &opaque2_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    OPAQUE_3f,
                                    &opaque3_offset, &opaque3_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    OPAQUE_4f,
                                    &opaque4_offset, &opaque4_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    PARSER_1_FLEX_HVE_CHECK_RESULT_15_0f,
                                    &parser1_flex_hve_chk_offset,
                                    &parser1_flex_hve_chk_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    PARSER_2_FLEX_HVE_CHECK_RESULT_15_0f,
                                    &parser2_flex_hve_chk_offset,
                                    &parser2_flex_hve_chk_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_16BITfmt,
                                    PKT_FLOW_ID_2f,
                                    &pkt_flow_id_2_offset,
                                    &pkt_flow_id_2_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_32BITfmt,
                                    UDF_1_CHUNK_5_4f,
                                    &udf_1_chunk_5_4_offset,
                                    &udf_1_chunk_5_4_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_32BITfmt,
                                    UDF_1_CHUNK_3_2f,
                                    &udf_1_chunk_3_2_offset,
                                    &udf_1_chunk_3_2_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_32BITfmt,
                                    UDF_2_CHUNK_5_4f,
                                    &udf_2_chunk_5_4_offset,
                                    &udf_2_chunk_5_4_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_32BITfmt,
                                    UDF_2_CHUNK_3_2f,
                                    &udf_2_chunk_3_2_offset,
                                    &udf_2_chunk_3_2_pos);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_32BITfmt,
                                    UDF_2_CHUNK_1_0f,
                                    &udf_2_chunk_1_0_offset,
                                    &udf_2_chunk_1_0_pos);
    BCM_IF_ERROR_RETURN(rv);

    /***** 8-bit Extractors *****/
    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_8BITfmt,
                                    ZONE_0_MATCH_IDf,
                                    &z0_match_id_offset[1],
                                    &z0_match_id_pos[1]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_8BITfmt,
                                    ZONE_1_MATCH_ID_7_0f,
                                    &z1_match_id_offset[1],
                                    &z1_match_id_pos[1]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_8BITfmt,
                                    ZONE_2_MATCH_ID_7_0f,
                                    &z2_match_id_offset[1],
                                    &z2_match_id_pos[1]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_8BITfmt,
                                    ZONE_3_MATCH_ID_7_0f,
                                    &z3_match_id_offset[1],
                                    &z3_match_id_pos[1]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_8BITfmt,
                                    ZONE_4_MATCH_ID_7_0f,
                                    &z4_match_id_offset[1],
                                    &z4_match_id_pos[1]);
    BCM_IF_ERROR_RETURN(rv);

    /***** 4-bit Extractors *****/
    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_4BITfmt,
                                    ZONE_2_MATCH_ID_11_8f,
                                    &z2_match_id_offset[2],
                                    &z2_match_id_pos[2]);
    BCM_IF_ERROR_RETURN(rv);
    rv = _field_ifp_base_offset_get(unit,
                                    IFP_L0_BUS_SECTION_4BITfmt,
                                    ZONE_4_MATCH_ID_11_8f,
                                    &z4_match_id_offset[2],
                                    &z4_match_id_pos[2]);
    BCM_IF_ERROR_RETURN(rv);

    if (fp_flex_db[unit].flex_data == NULL) {
       return BCM_E_INTERNAL;
    }

    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    for (extr = 0; extr < 4; extr++) {
       flex_db = fp_flex_db[unit].flex_data;

       for (; flex_db != NULL; flex_db = flex_db->next) {
           if (0 == extr) {
              if (0 == flex_db->thirtytwo_bit_ext_valid) {
                 continue;
              }
              ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E32;
           } else if (1 == extr) {
              if (0 == flex_db->sixteen_bit_ext_valid) {
                 continue;
              }
              ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E16;
           } else if (2 == extr) {
              if (0 == flex_db->eight_bit_ext_valid) {
                 continue;
              }
              ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E8;
           } else if (3 == extr) {
              if (0 == flex_db->four_bit_ext_valid) {
                 continue;
              }
              ext_sel = BCMI_KEYGEN_EXT_SECTION_L1E4;
           }

           valid_field = 0;
           for (obj_ct = 0; obj_ct < flex_db->num_obj; obj_ct++) {
               if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "Z0_MATCH_ID") == 0) {
                   field_pos[obj_ct] = z0_match_id_pos[extr-1];
                   field_offset[obj_ct] = z0_match_id_offset[extr-1];
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "Z1_MATCH_ID") == 0) {
                   field_pos[obj_ct] = z1_match_id_pos[extr-1];
                   field_offset[obj_ct] = z1_match_id_offset[extr-1];
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "Z2_MATCH_ID") == 0) {
                   field_pos[obj_ct] = z2_match_id_pos[extr-1];
                   field_offset[obj_ct] = z2_match_id_offset[extr-1];
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "Z3_MATCH_ID") == 0) {
                   field_pos[obj_ct] = z3_match_id_pos[extr-1];
                   field_offset[obj_ct] = z3_match_id_offset[extr-1];
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "Z4_MATCH_ID") == 0) {
                   field_pos[obj_ct] = z4_match_id_pos[extr-1];
                   field_offset[obj_ct] = z4_match_id_offset[extr-1];
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "OPAQUE_1") == 0) {
                   field_pos[obj_ct] = opaque1_pos;
                   field_offset[obj_ct] = opaque1_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "OPAQUE_2") == 0) {
                   field_pos[obj_ct] = opaque2_pos;
                   field_offset[obj_ct] = opaque2_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "OPAQUE_3") == 0) {
                   field_pos[obj_ct] = opaque3_pos;
                   field_offset[obj_ct] = opaque3_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "OPAQUE_4") == 0) {
                   field_pos[obj_ct] = opaque4_pos;
                   field_offset[obj_ct] = opaque4_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "PARSER_1_FLEX_HVE_CHECK_RESULT_15_0") == 0) {
                   field_pos[obj_ct] = parser1_flex_hve_chk_pos;
                   field_offset[obj_ct] = parser1_flex_hve_chk_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "PARSER_2_FLEX_HVE_CHECK_RESULT_15_0") == 0) {
                   field_pos[obj_ct] = parser2_flex_hve_chk_pos;
                   field_offset[obj_ct] = parser2_flex_hve_chk_offset;
                   valid_field = 1;
               } else if (sal_strcmp(flex_db->obj_str_arr[obj_ct], "PKT_FLOW_ID_2") == 0) {
                   field_pos[obj_ct] = pkt_flow_id_2_pos;
                   field_offset[obj_ct] = pkt_flow_id_2_offset;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_1_CHUNK_4")) {
                   field_pos[obj_ct] = udf_1_chunk_5_4_pos;
                   field_offset[obj_ct] = udf_1_chunk_5_4_offset;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_1_CHUNK_5")) {
                   field_pos[obj_ct] = udf_1_chunk_5_4_pos;
                   field_offset[obj_ct] = udf_1_chunk_5_4_offset + 16;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_2_CHUNK_5")) {
                   field_pos[obj_ct] = udf_2_chunk_5_4_pos;
                   field_offset[obj_ct] = udf_2_chunk_5_4_offset + 16;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_2_CHUNK_2")) {
                   field_pos[obj_ct] = udf_2_chunk_3_2_pos;
                   field_offset[obj_ct] = udf_2_chunk_3_2_offset;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_2_CHUNK_3")) {
                   field_pos[obj_ct] = udf_2_chunk_3_2_pos;
                   field_offset[obj_ct] = udf_2_chunk_3_2_offset + 16;
                   valid_field = 1;
               } else if (0 == sal_strcmp(flex_db->obj_str_arr[obj_ct], "UDF_1_CHUNK_2")) {
                   field_pos[obj_ct] = udf_1_chunk_3_2_pos;
                   field_offset[obj_ct] = udf_1_chunk_3_2_offset;
                   valid_field = 1;
               }
           }

           if (valid_field == 0) {
              continue;
           }

           if (flex_db->num_obj == 1) {
               BCMI_KEYGEN_QUAL_CFG_INIT(unit);
               BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                0, ext_sel, field_pos[0],
                                flex_db->ceh_info[0].offset + field_offset[0],
                                flex_db->ceh_info[0].width);
               BCMI_KEYGEN_QUAL_CFG_INSERT(unit, stage_fc->qual_cfg_info_db,
                                           flex_db->qual, qual_flags);

               LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "%20s :: [%d] pos:%d offset:%d width:%d field_offset:%d\n\r"),
                         _field_qual_name(flex_db->qual), extr, field_pos[0],
                         flex_db->ceh_info[0].offset,
                         flex_db->ceh_info[0].width, field_offset[0]));
           } else if (flex_db->num_obj == 2) {
               BCMI_KEYGEN_QUAL_CFG_INIT(unit);
               BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 0, ext_sel, field_pos[0],
                                 flex_db->ceh_info[0].offset + field_offset[0],
                                 flex_db->ceh_info[0].width);
               BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 1, ext_sel, field_pos[1],
                                 flex_db->ceh_info[1].offset + field_offset[1],
                                 flex_db->ceh_info[1].width);
               BCMI_KEYGEN_QUAL_CFG_INSERT(unit, stage_fc->qual_cfg_info_db,
                                 flex_db->qual, qual_flags);

               LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "%20s :: [%d] pos0:%d pos1:%d off0:%d width0:%d off1:%d"
                        " width1:%d field_offset0:%d field_offset1:%d\n\r"),
                        _field_qual_name(flex_db->qual), extr,
                        field_pos[0],
                        field_pos[1],
                        flex_db->ceh_info[0].offset,
                        flex_db->ceh_info[0].width,
                        flex_db->ceh_info[1].offset,
                        flex_db->ceh_info[1].width,
                        field_offset[0],
                        field_offset[1]));
           } else {
               return BCM_E_INTERNAL;
           }
       }
    }

    /******** 8-bit + 4-bit section *******/
    {
       soc_cancun_ceh_field_info_t ceh_info[3];

       /* ZONE_2_MATCH_ID_7_0 + ZONE_2_MATCH_ID_11_8 */
       rv = soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID", "IPV4", &ceh_info[0]);
       BCM_IF_ERROR_RETURN(rv);
       rv = soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID", "IPV6", &ceh_info[1]);
       BCM_IF_ERROR_RETURN(rv);
       rv = soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID", "AH", &ceh_info[2]);
       BCM_IF_ERROR_RETURN(rv);

       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 0, BCMI_KEYGEN_EXT_SECTION_L1E8,
                                 z2_match_id_pos[1],
                                 ceh_info[0].offset + z2_match_id_offset[1],
                                 ceh_info[0].width);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 1, BCMI_KEYGEN_EXT_SECTION_L1E4, z2_match_id_pos[2],
                                 ceh_info[2].offset + z2_match_id_offset[2],
                                 ceh_info[2].width);
       BCMI_KEYGEN_QUAL_CFG_INSERT(
                                 unit, stage_fc->qual_cfg_info_db,
                                 bcmFieldQualifyIpv4OptionHdrType,
                                 qual_flags);

       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 0, BCMI_KEYGEN_EXT_SECTION_L1E8,
                                 z2_match_id_pos[1],
                                 ceh_info[1].offset + z2_match_id_offset[1],
                                 ceh_info[1].width);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 1, BCMI_KEYGEN_EXT_SECTION_L1E4,
                                 z2_match_id_pos[2],
                                 ceh_info[2].offset + z2_match_id_offset[2],
                                 ceh_info[2].width);
       BCMI_KEYGEN_QUAL_CFG_INSERT(
                                 unit, stage_fc->qual_cfg_info_db,
                                 bcmFieldQualifyIpv6ExtensionHdrType,
                                 qual_flags);

       /* ZONE_4_MATCH_ID_7_0 + ZONE_4_MATCH_ID_11_8 */
       rv = soc_cancun_ceh_obj_field_get(unit, "Z4_MATCH_ID", "IPV4", &ceh_info[0]);
       BCM_IF_ERROR_RETURN(rv);
       rv = soc_cancun_ceh_obj_field_get(unit, "Z4_MATCH_ID", "IPV6", &ceh_info[1]);
       BCM_IF_ERROR_RETURN(rv);
       rv = soc_cancun_ceh_obj_field_get(unit, "Z4_MATCH_ID", "AH", &ceh_info[2]);
       BCM_IF_ERROR_RETURN(rv);

       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 0, BCMI_KEYGEN_EXT_SECTION_L1E8,
                                 z4_match_id_pos[1],
                                 ceh_info[0].offset + z4_match_id_offset[1],
                                 ceh_info[0].width);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 1, BCMI_KEYGEN_EXT_SECTION_L1E4,
                                 z4_match_id_pos[2],
                                 ceh_info[2].offset + z4_match_id_offset[2],
                                 ceh_info[2].width);
       BCMI_KEYGEN_QUAL_CFG_INSERT(
                                 unit, stage_fc->qual_cfg_info_db,
                                 bcmFieldQualifyOverlayIpv4OptionHdrType,
                                 qual_flags);

       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 0, BCMI_KEYGEN_EXT_SECTION_L1E8,
                                 z4_match_id_pos[1],
                                 ceh_info[1].offset + z4_match_id_offset[1],
                                 ceh_info[1].width);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(
                                 1, BCMI_KEYGEN_EXT_SECTION_L1E4,
                                 z4_match_id_pos[2],
                                 ceh_info[2].offset + z4_match_id_offset[2],
                                 ceh_info[2].width);
       BCMI_KEYGEN_QUAL_CFG_INSERT(
                                 unit, stage_fc->qual_cfg_info_db,
                                 bcmFieldQualifyOverlayIpv6ExtensionHdrType,
                                 qual_flags);

    }

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function: _field_td3_flex_ingress_qualifiers_init
 * Purpose:  Initialize Field Flex Ingress Stage Qualifiers.
 */
int
_field_td3_flex_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int rv;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _field_td3_flex_extractor_qual_init(unit, stage_fc);
    return rv;
}


/*
 * Function:
 *    _field_td3_ingress_action_profiles_init
 * Purpose:
 *   Initialize action profiles based on group operational mode.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
int
_field_td3_ingress_action_profiles_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem;               /* SOC memory names.         */
    int entry_words;             /* Entry size in words.      */
    int inst;                    /* Instance iterator.        */
    int rv;                      /* Operation return status.  */

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for Ingress Stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->oper_mode) {

        /* Field Groups operational in Global mode. */
        case bcmFieldGroupOperModeGlobal:

            /* Initialize action profile memory name. */
            mem = IFP_POLICY_ACTION_PROFILEm;

            /* Determine action profile entry size in number of words. */
            entry_words = soc_mem_entry_words(unit, mem);

            /* Create action program profile table. */
            rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                    &stage_fc->action_profile[_FP_DEF_INST]);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d) Error: action profile creation failed."
                   "=%d\n"), unit, rv));
                return (rv);
            }
            break;

        /* Field Groups operational in Per-Pipe mode. */
        case bcmFieldGroupOperModePipeLocal:

            /* Determine action profiles entry size in number of words. */
            entry_words = soc_mem_entry_words(unit,
                                 IFP_POLICY_ACTION_PROFILE_PIPE0m);

            for (inst = 0; inst < stage_fc->num_instances; inst++) {

                /* Initialize action program profile memory name. */
                rv = _bcm_field_mem_instance_get(unit,
                                                   IFP_POLICY_ACTION_PROFILEm,
                                                   inst,
                                                   &mem);

                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                       "FP(unit %d) Error: Action Profile mem instance"
                       " get failed"
                       "=%d\n"), unit, rv));
                    return (rv);
                }

                /* Create action program profile table. */
                rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                        &stage_fc->action_profile[inst]);

                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                       "FP(unit %d) Error: action profile creation failed."
                       "=%d\n"), unit, rv));
                    return (rv);
                }
            }
            break;

        default:
            /*
             * Invalid Group Operational mode, should never hit this condition.
             */
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

STATIC int
_field_td3_flex_actions_init(int unit, _field_stage_t *stage_fc)
{
    int rv1 = BCM_E_UNAVAIL, rv2 = BCM_E_UNAVAIL;
    _FP_ACTION_DECL
    uint8 extr_valid = 0;
    uint8 edit_valid = 0;
    soc_cancun_ceh_field_info_t ceh_info;
    soc_cancun_ceh_field_info_t ceh_info1;

    rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_ENABLE",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_ENABLE_MASK",
                                              &ceh_info);
       rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE_MASK",
                                              &ceh_info);
       if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
          /* INT Encap Enable Action */
          _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIntEncapEnable,
                                     0, _FieldActionSetNone, 0, 0, -1);
          extr_valid = 1;
          edit_valid = 1;
       }
    }

    if (SOC_SUCCESS(rv2)) {
       /* INT Encap Update Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIntEncapUpdate,
                                    0, _FieldActionSetNone, 0, 0, -1);
       extr_valid = 1;
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_TURNAROUND",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_TURNAROUND_MASK",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       /* INT TurnAround Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIntTurnAround,
                                     0, _FieldActionSetNone, 0, 0, -1);
       edit_valid = 1;
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "PRESERVE_INCOMING_OTAG",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "PRESERVE_INCOMING_OTAG_MASK",
                                              &ceh_info);
    if ((BCM_E_NOT_FOUND == rv1) && (BCM_E_NOT_FOUND == rv2)) {
        rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "ADD_ING_OTAG_TO_EGR_OTAG",
                                                  &ceh_info);
        rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "ADD_ING_OTAG_TO_EGR_OTAG_MASK",
                                                  &ceh_info);
    }
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       /* bcmFieldActionAddIngOuterVlanToEgrOuterVlan Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionAddIngOuterVlanToEgrOuterVlan,
                                     0, _FieldActionSetNone, 0, 0, -1);
       extr_valid = 1;
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "GBP_PRESENT_IN_PKT",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "GBP_PRESENT_IN_PKT_MASK",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       /* bcmFieldActionGbpClassifierAdd Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGbpClassifierAdd,
                                     0, _FieldActionSetNone, 0, 0, -1);
       extr_valid = 1;
    }


    rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "GBP_SID_DID_CHECK",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "GBP_SID_DID_CHECK_MASK",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       /* bcmFieldActionGbpSrcMacMcastBitSet Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGbpSrcMacMcastBitSet,
                                     0, _FieldActionSetNone, 0, 0, -1);
       extr_valid = 1;
    }

    if (SOC_IS_TRIDENT3X(unit)) {
       /* bcmFieldActionErspan3HdrVlanCosPktCopy Action */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionErspan3HdrVlanCosPktCopy,
                                     0, _FieldActionSetNone, 0, 0, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "ADD_NSH",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "SPID_SI_LOOKUP_ENABLE",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
       rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "ADD_NSH_MASK",
                                              &ceh_info);
       rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "SPID_SI_LOOKUP_ENABLE_MASK",
                                              &ceh_info);
       if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
          /* NSH Encap Enable Action */
          _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionNshEncapEnable,
                                     0, _FieldActionSetNone, 0, 0, -1);
          extr_valid = 1;
          edit_valid = 1;
       }
    }

    if (extr_valid == 1) {
       /* EXTRACTION_CTRL_ID_ACTION_SET */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionExtractionCtrlId,
                                     0, _FieldActionExtractionCtrlIdActionSet, 0, 16, -1);
    }

    if (edit_valid == 1) {
       /* EDIT_CTRL_ID_ACTION_SET */
       _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionEditCtrlId,
                                     0, _FieldActionEditCtrlIdSet, 0, 16, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_2", "SPID_7_0",
                                              &ceh_info);
    rv2 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_1", "SPID_23_8",
                                              &ceh_info1);
    if (SOC_SUCCESS(rv1) && SOC_SUCCESS(rv2)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque1,
                                     0, _FieldActionOpaque1ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque2,
                                     0, _FieldActionOpaque2ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionNshServicePathId,
                                     0, _FieldActionSetNone,
                                     ceh_info.offset,
                                     ceh_info.width, -1,
                                     ceh_info1.offset,
                                     ceh_info1.width, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_1", "GBP_SID",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque1,
                                     0, _FieldActionOpaque1ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionGbpSrcIdNew, 0,
                                     _FieldActionSetNone,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_1", "LARGE_SCALE_NAT_DST_EDIT_INDEX",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque1,
                                     0, _FieldActionOpaque1ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionAssignNatClassId, 0,
                                     _FieldActionSetNone,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }
    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_2", "GBP_DID",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque2,
                                     0, _FieldActionOpaque2ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionGbpDstIdNew, 0,
                                     _FieldActionSetNone,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_2", "SI",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque2,
                                     0, _FieldActionOpaque2ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionNshServiceIndex, 0,
                                     _FieldActionSetNone,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_3", "CHANGE_L2_FIELDS_EDIT_INDEX",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque3,
                                     0, _FieldActionOpaque3ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionAssignChangeL2FieldsClassId, 0,
                                     _FieldActionOpaque3ActionSet,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }

    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_4", "ASSIGN_IFA_AND_MD_HDR",
                                              &ceh_info);
    if (SOC_SUCCESS(rv1)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, _bcmFieldActionOpaque4,
                                     0, _FieldActionOpaque4ActionSet, 0, 16, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                                     bcmFieldActionEncapIfaMetadataHdr, 0,
                                     _FieldActionOpaque4ActionSet,
                                     ceh_info.offset,
                                     ceh_info.width, -1);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_td3_common_actions_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 * Returns:
 *     BCM_E_NONE
 */
int
_field_td3_common_actions_init(int unit, _field_stage_t *stage_fc)
{
    _FP_ACTION_DECL
    uint32 offset = 0;                  /* General variable to carry offset. */
    uint32 profile_one_offset = 0;      /* Offset of PROFILE_ONE_SET. */
    uint32 input_priority_set = 0;      /* Offset of CHANGE_INPUT_PRIORITY_SET*/
    uint32 int_cn_set = 0;              /* Offset of CHANGE_INT_CN_SET. */
    uint32 cos_or_int_pri_set = 0;      /* Offset of CHANGE_COS_OR_INT_PRI_SET*/
    uint32 drop_precendence_set = 0;   /* Offset of CHANGE_DROP_PRECEDENCE_SET*/

    uint32 profile_two_offset = 0;      /* Offset of PROFILE_TWO_SET. */
    uint32 dscp_tos_set = 0;            /* Offset of CHANGE_DSCP_TOS_SET. */
    uint32 pkt_pri_set = 0;             /* Offset of CHANGE_PKT_PRI_SET. */
    uint32 ecn_set = 0;                 /* Offset of CHANGE_ECN_SET. */

    uint32 redirect_offset = 0;         /* Offset of REDIRECT_SET. */
    uint32 l3swl2change_offset = 0;     /* Offset of L3SW_CHANGE_L2_SET. */
    uint32 nat_offset = 0;              /* Offset of NAT_SET. */
    uint32 mirror_offset = 0;           /* Offset of MIRROR_SET. */
    uint32 lb_offset = 0;               /* Offset of LB_CONTROL_SET. */
    uint32 nat_override_offset = 0;     /* Offset of NAT_OVERRIDE_SET. */
    uint32 copytocpu_offset = 0;        /* Offset of COPY_TO_CPU_SET. */
    uint32 cutthrough_offset = 0;       /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32 urpf_offset = 0;             /* Offset of URPF_OVERRIDE_SET. */
    uint32 ttl_offset = 0;              /* Offset of TTL_SET. */
    uint32 cpucos_offset = 0;           /* Offset of CHANGE_CPU_COS_SET. */
    uint32 drop_offset = 0;             /* Offset of DROP_SET. */
    uint32 mirror_override_offset = 0;  /* Offset of MIRROR_OVERRIDE_SET. */
    uint32 sflow_offset = 0;            /* Offset of SFLOW_SET. */
    uint32 debug_offset = 0;            /* Offset of INSTRUMENTATION_SET */
#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    uint32 modid_base_offset = 0;       /* Offset of USE_IFP_MODID_BASE */
#endif
    /* New action sets for TD3 */
#if 0
    uint32 ifp_meter_set = 0;            /* Offset of METER_SET. */
    uint32 fcoe_vsan_set = 0;            /* Offset of FCOE_VSAN_SET. */
    uint32 edit_ctrl_id_action_set = 0;  /* Offset of EDIT_CTRL_ID_ACTION_SET.*/
    uint32 ifp_counter_set = 0;          /* Offset of COUNTER_SET. */
    uint32 green_to_pid_set = 0;         /* Offset of GREEN_TO_PID_SET. */
    uint32 ignore_fcoe_zone_check_set = 0;
                             /* Offset of IGNORE_FCOE_ZONE_CHECK_SET. */
    uint32 rx_timestamp_insertion_set = 0;
                             /* Offset of RX_TIMESTAMP_INSERTION_SET. */
    uint32 tx_timestamp_insertion_set = 0;
                             /* Offset of TX_TIMESTAMP_INSERTION_SET. */
    uint32 opaque_1_action_set = 0;      /* Offset of OPAQUE_1_ACTION_SET. */
    uint32 opaque_2_action_set = 0;      /* Offset of OPAQUE_2_ACTION_SET. */
    uint32 opaque_3_action_set = 0;      /* Offset of OPAQUE_3_ACTION_SET. */
    uint32 opaque_4_action_set = 0;      /* Offset of OPAQUE_4_ACTION_SET. */
    uint32 loopback_profile_action_set = 0;
                              /* Offset of LOOPBACK_PROFILE_ACTION_SET. */
#endif

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        /* Offset of CHANGE_INPUT_PRIORITY_SET within the
           profile_set_1. */
        input_priority_set = 51;
        /* Offset of CHANGE_INT_CN_SET within the
           profile_set_1. */
        int_cn_set = 42;
        /* Offset of CHANGE_COS_OR_INT_PRI_SET within the
           profile_set_1. */
        cos_or_int_pri_set = 6;
        /* Offset of CHANGE_DROP_PRECEDENCE_SET within the
           profile_set_1. */
        drop_precendence_set = 0;
        profile_two_offset= (SOC_IS_HELIX5(unit) || SOC_IS_FIREBOLT6(unit)) ? 62 : 56;
        /* Offset of CHANGE_DSCP_TOS_SET within the
           profile_set_2. */
        dscp_tos_set = 27;
        /* Offset of CHANGE_PKT_PRI_SET within the
           profile_set_1. */
        pkt_pri_set = 9;
        /* Offset of CHANGE_ECN_SET within the
           profile_set_2. */
        ecn_set = 0;
    }

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset + input_priority_set;
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionUntaggedPacketPriorityNew,
                                      0, _FieldActionChangeInputPrioritySet,
                                      offset + 1, 4, -1, offset + 0, 1, 1);

    offset = profile_one_offset + drop_precendence_set;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 0, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 2, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet, offset + 4, 2, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDropPrecedence,
                                 0, _FieldActionChangeDropPrecendenceSet,
                                  offset + 4, 2, -1);


    offset = profile_one_offset + cos_or_int_pri_set;
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 10, -1, offset + 30, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 2, -1, offset + 30, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 30, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 8, -1, offset + 30, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 30, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 30, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpUcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 5, -1, offset + 30, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpMcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 5, 5, -1, offset + 30, 4, 9);

        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 10, 10, -1, offset + 34, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 10, 2, -1, offset + 34, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 34, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 10, 8, -1, offset + 34, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 34, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 34, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpUcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 10, 5, -1, offset + 34, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpMcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 15, 5, -1, offset + 34, 4, 9);

        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 20, 10, -1, offset + 38, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 20, 2, -1, offset + 38, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 38, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 20, 8, -1, offset + 38, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 38, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 38, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpUcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 5, -1, offset + 38, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpMcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 25, 5, -1, offset + 38, 4, 9);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 10, -1, offset + 38, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCopy,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 38, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionPrioIntNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 10, -1, offset + 38, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntTos,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 38, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCancel,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 38, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionUcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 5, -1, offset + 38, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 25, 5, -1, offset + 38, 4, 9);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosMapNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 2, -1, offset + 38, 4, 2);
#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
        offset = profile_one_offset + modid_base_offset ;
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionMacSecModidBase,
                                                   0, _FieldActionMacSecModidBase,
                                                   offset + 0, 1, 1);
#endif
    } else
#endif
    {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 8, -1, offset + 24, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 2, -1, offset + 24, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 24, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 8, -1, offset + 24, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 24, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 24, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpUcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset , 4, -1, offset + 24, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpMcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 4, 4, -1, offset + 24, 4, 9);

        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 8, 8, -1, offset + 28, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 8, 2, -1, offset + 28, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 28, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 8, 8, -1, offset + 28, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 28, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 28, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpUcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 8, 4, -1, offset + 28, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpMcastCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 12, 4, -1, offset + 28, 4, 9);

        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosQNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 16, 8, -1, offset + 32, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosMapNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 16, 2, -1, offset + 32, 4, 2);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCopy,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 32, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioIntNew,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 16, 8, -1, offset + 32, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntTos,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 32, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCancel,
                                                   0, _FieldActionChangeCosOrIntPriSet,
                                                   offset + 32, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpUcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 16, 4, -1, offset + 32, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpMcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 4, -1, offset + 32, 4, 9);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 16, 8, -1, offset + 32, 4, 1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCopy,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 32, 4, 4);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionPrioIntNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 16, 8, -1, offset + 32, 4, 5);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntTos,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 32, 4, 6);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCancel,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 32, 4, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionUcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 16, 4, -1, offset + 32, 4, 8);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMcastCosQNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 20, 4, -1, offset + 32, 4, 9);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosMapNew,
                                                  0, _FieldActionChangeCosOrIntPriSet,
                                                  offset + 16, 2, -1, offset + 32, 4, 2);
    }

    if (soc_feature(unit, soc_feature_ecn_wred)) {
        offset = profile_one_offset + int_cn_set;
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                           bcmFieldActionGpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           _FieldActionChangeIntCNSet,
                           offset + 4, 2, -1, offset + 8, 1, 1);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                           bcmFieldActionYpIntCongestionNotificationNew,
                           _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                           _FieldActionChangeIntCNSet,
                           offset + 2, 2, -1, offset + 7, 1, 1);
       _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                          bcmFieldActionRpIntCongestionNotificationNew,
                          _BCM_FIELD_ACTION_NO_EM_SUPPORT,
                          _FieldActionChangeIntCNSet,
                          offset, 2, -1, offset + 6, 1, 1);
    }
    if (soc_feature(unit, soc_feature_field_action_pfc_class)) {
       _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc,
                          bcmFieldActionPfcClassNew,
                          0, _FieldActionChangeInputPrioritySet,
                          offset + 1, 4, -1, offset + 0, 1, 1);
    }

    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorIngress,
                                          0, _FieldActionMirrorSet,
                                          offset + 8, 4, 0, offset , 8, 0);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorEgress,
                                          0, _FieldActionMirrorSet,
                                          offset + 8, 4, 0, offset , 8, 0);

    /* IFP_LB_CONTROLS_SET(3b) */
    offset = lb_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionHgTrunkRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionTrunkRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 1, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
                       bcmFieldActionEcmpRandomRoundRobinHashCancel,
                       0, _FieldActionLbControlSet,
                       offset + 2, 1, 1);

    /* IFP_NAT_OVERRIDE_SET(1b) */
    offset = nat_override_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionNatCancel,
                                 0, _FieldActionNatOverrideSet, offset + 0, 1, 1);
    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 0, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 3, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 3, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 3, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCopyToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuCancel,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuReinstate,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 6, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionTimeStampToCpu,
                                        0, _FieldActionCopyToCpuSet,
                                        offset + 9, 8, -1, offset + 6, 3, 5);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionOamCopyToCpu,
                                            0, _FieldActionCopyToCpuSet,
                                            offset + 9, 8, -1, offset + 6, 3, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpOamCopyToCpu,
                                            0, _FieldActionCopyToCpuSet,
                                            offset + 9, 8, -1, offset + 6, 3, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpOamCopyToCpu,
                                            0, _FieldActionCopyToCpuSet,
                                            offset + 9, 8, -1, offset + 3, 3, 7);
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpOamCopyToCpu,
                                            0, _FieldActionCopyToCpuSet,
                                            offset + 9, 8, -1, offset + 0, 3, 7);
    }
#endif /* BCM_HELIX5_SUPPORT */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTimeStampToCpuCancel,
                                        0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpTimeStampToCpuCancel,
                                        0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpTimeStampToCpuCancel,
                                        0, _FieldActionCopyToCpuSet, 0, 0, 0);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpTimeStampToCpuCancel,
                                        0, _FieldActionCopyToCpuSet, 0, 0, 0);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCutThrough,
                                 0, _FieldActionCutThrOverrideSet, offset + 0, 1, 1);

    /* IFP_URPF_OVERRIDE_SET(1b) */
    offset = urpf_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCheckUrpf,
                                 0, _FieldActionUrpfOverrideSet, offset + 0, 1, 1);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotChangeTtl,
                                 0, _FieldActionTtlOverrideSet, offset + 0, 1, 1);

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset + ecn_set;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 0, 2, -1, offset + 6, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 2, 2, -1, offset + 7, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 4, 2, -1, offset + 8, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEcnNew,
                                        0, _FieldActionChangeEcnSet,
                                        offset + 4, 2, -1, offset + 8, 1, 1);


    offset = profile_two_offset + pkt_pri_set;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioPktNew,
                                       0, _FieldActionChangePktPriSet,
                                       offset , 3, -1, offset + 9, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 9, 3, 7);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 3, 3, -1, offset + 12, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 12, 3, 7);

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDot1pPreserve,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCopy,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 6, 3, -1, offset + 15, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktTos,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCancel,
                                 0, _FieldActionChangePktPriSet, offset + 15, 3, 7);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCopy,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionPrioPktNew,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 6, 3, -1, offset + 15, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktTos,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCancel,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 7);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDot1pPreserve,
                                 0, _FieldActionChangePktPriSet,
                                 offset + 15, 3, 3);


    offset = profile_two_offset + dscp_tos_set;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionRpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 13, 6, -1, offset , 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset , 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset , 2, 3);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionYpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 7, 6, -1, offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 2, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 2, 2, 3);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 19, 6, -1, offset + 4, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 5);

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionGpTosPrecedenceNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 19, 6, -1, offset + 4, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpTosPrecedenceCopy,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionDscpNew,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 19, 6, -1, offset + 4, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDscpCancel,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDscpPreserve,
                                      0, _FieldActionChangeDscpTosSet,
                                      offset + 4, 3, 5);

    /* IFP_L3SW_CHANGE_L2_SET(33b) */
    offset = l3swl2change_offset;
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionChangeL2Fields,
                                          0, _FieldActionL3SwChangeL2Set,
                                          offset + 0, 16, -1,
                                          offset + 19, 4, 1);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionChangeL2Fields,
                                          0, _FieldActionL3SwChangeL2Set,
                                          offset + 0, 15, -1,
                                          offset + 19, 4, 1);
    } else
#endif
    {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionChangeL2Fields,
                                          0, _FieldActionL3SwChangeL2Set,
                                          offset + 0, 16, -1,
                                          offset + 19, 4, 1);
    }
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionChangeL2FieldsCancel,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 19, 4, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionFabricQueue,
                                      0, _FieldActionL3SwChangeL2Set,
                     offset + 0, 2, -1,  offset + 2, 16, -1, offset + 19, 4, 3);
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                       _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,
                                       _FieldActionL3SwChangeL2Set,
                                       offset + 0, 16, -1,
                                       offset + 17, 1, 0, offset + 19, 4, 6);
    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                       _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,
                                       _FieldActionL3SwChangeL2Set,
                                       offset + 0, 15, -1,
                                       offset + 17, 1, 0, offset + 19, 4, 6);
    } else
#endif
    {
        _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                       _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,
                                       _FieldActionL3SwChangeL2Set,
                                       offset + 0, 16, -1,
                                       offset + 17, 1, 0, offset + 19, 4, 6);
    }

    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                                   _BCM_FIELD_ACTION_L3SWITCH_ECMP,
                                   _FieldActionL3SwChangeL2Set,
                                   offset + 0, SOC_IS_HELIX5(unit) ? 10 : (SOC_IS_HURRICANE4(unit) ? 8 : 12), -1,
                                   offset + 17, 1, 1, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionMultipathHash,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 12, 3, -1, offset + 19, 4, 6);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionL3SwitchCancel,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 19, 4, 7);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressClassSelect,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 0, 4, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionHiGigClassSelect,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 4, 3, -1, offset + 19, 4, 8);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionNewClassId,
                                      0, _FieldActionL3SwChangeL2Set,
                                      offset + 7, 9, -1, offset + 19, 4, 8);
    if (soc_feature(unit, soc_feature_bfd)) {
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionBFDSessionIdNew,
                                          0, _FieldActionL3SwChangeL2Set,
                                          offset + 0, 12, -1, offset + 19, 4, 9);
    }
    /* IFP_CHANGE_CPU_COS_SET(8b) */
    offset = cpucos_offset;

    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionCosQCpuNew,
                                        0, _FieldActionChangeCpuCosSet,
                                        offset + 0, 6, -1, offset + 6, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionServicePoolIdNew, 0,
                                                   _FieldActionChangeCpuCosSet,
                                                              offset + 2, 2, -1,
                                                              offset + 5, 1, 1,
                                                              offset + 6, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionServicePoolIdPrecedenceNew,
                                                              0, _FieldActionChangeCpuCosSet,
                                                              offset + 2, 2, -1,
                                                              offset + 0, 2, -1,
                                                              offset + 4, 2, 3,
                                                              offset + 6, 2, 2);

    /* IFP_DROP_SET(6b) */
    offset = drop_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDrop,
                                 0, _FieldActionDropSet, offset + 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropCancel,
                                 0, _FieldActionDropSet, offset + 0, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDrop,
                                 0, _FieldActionDropSet, offset + 2, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropCancel,
                                 0, _FieldActionDropSet,  offset + 2, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDrop,
                                 0, _FieldActionDropSet, offset + 4, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropCancel,
                                 0, _FieldActionDropSet, offset + 4, 2, 2);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDrop,
                                 0, _FieldActionDropSet, offset + 4, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDropCancel,
                                 0, _FieldActionDropSet, offset + 4, 2, 2);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionMirrorOverride,
                                 0, _FieldActionMirrorOverrideSet, offset + 0, 1, 1);

    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngSampleEnable,
                                 0, _FieldActionSflowSet, offset + 0, 1, 1);

    /* IFP_INSTRUMENTATION_SET*/
    offset = debug_offset;
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionPacketTraceEnable,
                             0,
                             _FieldActionInstrumentationSet, offset + 0, 1, 1);

    /* IFP_REDIRECT_SET(38b) */
    offset = redirect_offset;

    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                                _FieldActionRedirectSet,
                                         offset + 6, 16, -1, offset + 22 , 1, 0,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirect,
                                                 _BCM_FIELD_ACTION_REDIRECT_DVP,
                                                _FieldActionRedirectSet,
                                          offset + 6, 14, -1,
                                            offset + 3, 3, 6, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                               _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectTrunk,
                                                _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectTrunk,
                                            _BCM_FIELD_ACTION_REDIRECT_DVP,
                                            _FieldActionRedirectSet,
                                          offset + 6, 14, -1,
                                          offset + 3, 3, 6, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                     bcmFieldActionUnmodifiedPacketRedirectPort,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 0,
                                            offset + 3, 3, 1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc,
                                     bcmFieldActionUnmodifiedPacketRedirectPort,
                                                _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                                _FieldActionRedirectSet,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 1, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                            _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP,
                                                _FieldActionRedirectSet,
                        offset + 6, 16, -1, offset + 3, 3, 2, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                                _BCM_FIELD_ACTION_REDIRECT_ECMP,
                                                _FieldActionRedirectSet,
                        offset + 6, 16, -1, offset + 3, 3, 3, offset + 0, 3, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRedirectCancel,
                                   0, _FieldActionRedirectSet, offset + 0, 3, 2);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectPbmp, 0,
                                          _FieldActionRedirectSet,
                       offset + 6, 10, -1, offset + 23, 2, 0, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectVlan, 0,
                                          _FieldActionRedirectSet,
                        offset + 18, 1, 0, offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectBcastPbmp, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 18, 1, 1,
                                          offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectMcast, 0,
                                           _FieldActionRedirectSet,offset + 6, 14, -1,
                                           offset + 23, 2, 2, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectIpmc, 0,
                                          _FieldActionRedirectSet,offset + 6, 14, -1,
                                          offset + 23, 2, 3, offset + 0, 3, 3);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressMask, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 0, 3, 4);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionEgressPortsAdd, 0,
                                          _FieldActionRedirectSet,
                                          offset + 6, 10, -1, offset + 0, 3, 5);
    _FP_ACTION_WITH_ASET_ADD_THREE(unit, stage_fc, bcmFieldActionFabricEHAddOrUpdate, 0,
                                          _FieldActionRedirectSet, offset + 3, 32, -1,
                                          offset + 35, 3, -1, offset + 0, 3, 6);

    /* IFP_NAT_SET(12b) */
    offset = nat_offset;

    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionNat,
                                    0, _FieldActionNatSet, offset + 11, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionNatEgressOverride,
                                    0, _FieldActionNatSet,
                                    offset + 0, 10, -1, offset + 10, 1, -1);

    /* ExactMatch Action Class Id (12b) */
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionExactMatchClassId,
                       _BCM_FIELD_ACTION_NO_IFP_SUPPORT, 0, 12, 0);


    /* All actions which have corresponding Colored(Gp/Yp/Rp) Actions needs to be
     * initialized with some default configuration(offset = 0, width = 0, value = 0).
     */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionColorIndependent,
                                 0, _FieldActionGreenToPidSet, 0, 0, 0);

    /* TD3 New actions initialzation */
#if defined(INCLUDE_L3)
    /* PROTECTION_SWITCHING_DROP_OVERIDE_SET (1b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionRecoverableDropCancel,
                                 0, _FieldActionProtectionSwitchingDropOverrideSet,
                                 0, 1, 1);

    /* IFP_DLB_ALTERNATE_PATH_CONTROL_SET (10b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgm,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmThreshold,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 7, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmBias,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 4, 3, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDgmCost,
                                 0, _FieldActionIfpDlbAlternatePathControlSet, 1, 3, -1);

    /* ECMP_DLB_ACTION_SET (2b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpEnable,
                                 0, _FieldActionEcmpDlbActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicEcmpCancel,
                                 0, _FieldActionEcmpDlbActionSet, 1, 1, 1);
#endif /* INCLUDE_L3 */
    /* HGT_LAG_DLB_ACTION_SET (2b) */
    if (soc_feature(unit, soc_feature_lag_dlb)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicTrunkEnable,
                                     0, _FieldActionHgtLagDlbActionSet, 0, 1, 1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicTrunkCancel,
                                     0, _FieldActionHgtLagDlbActionSet, 1, 1, 1);
    }

    if (soc_feature(unit, soc_feature_hg_dlb)) {
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicHgTrunkEnable,
                                     0, _FieldActionHgtLagDlbActionSet, 0, 1, 1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicHgTrunkCancel,
                                     0, _FieldActionHgtLagDlbActionSet, 1, 1, 1);
    }

    /* ECMP1_RH_ACTION_SET */

    /* ECMP2_RH_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEcmpResilientHashEnable,
                                 0, _FieldActionEcmp2RhActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEcmpResilientHashDisable,
                                 0, _FieldActionEcmp2RhActionSet, 1, 1, 1);

    /* LAG_RH_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTrunkResilientHashEnable,
                                 0, _FieldActionLagRhActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionTrunkResilientHashDisable,
                                 0, _FieldActionLagRhActionSet, 1, 1, 1);

    /* HGT_RH_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionHiGigTrunkResilientHashEnable,
                                 0, _FieldActionHgtRhActionSet, 0, 1, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionHiGigTrunkResilientHashDisable,
                                 0, _FieldActionHgtRhActionSet, 1, 1, 1);

    /* LOOPBACK_PROFILE_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionLoopbackType,
                                 0, _FieldActionLoopbackProfileActionSet, 0, 4, -1);

    /* OPAQUE_4_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionAssignOpaqueObject4,
                                 0, _FieldActionOpaque4ActionSet, 0, 16, -1);
    /* OPAQUE_3_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionAssignOpaqueObject3,
                                 0, _FieldActionOpaque3ActionSet, 0, 16, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionLoopbackSubtype,
                                 0, _FieldActionOpaque3ActionSet, 0, 16, -1);
    /* OPAQUE_2_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionAssignOpaqueObject2,
                                 0, _FieldActionOpaque2ActionSet, 0, 16, -1);
    /* OPAQUE_1_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionAssignOpaqueObject1,
                                 0, _FieldActionOpaque1ActionSet, 0, 16, -1);
    /* EXTRACTION_CTRL_ID_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionAssignExtractionCtrlId,
                                 0, _FieldActionExtractionCtrlIdActionSet,
                                 0, 8, -1,
                                 8, 8, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressFlowControlEnable,
                                 0, _FieldActionExtractionCtrlIdActionSet,
                                 0, 16, -1);
    /* EDIT_CTRL_ID_ACTION_SET */
    _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionAssignEditCtrlId,
                                 0, _FieldActionEditCtrlIdSet,
                                 0, 8, -1,
                                 8, 8, -1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressFlowEncapEnable,
                                 0, _FieldActionEditCtrlIdSet,
                                 0, 16, -1);
    /* TX_TIMESTAMP_INSERTION_SET (4b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsert,
                                 0, _FieldActionTxTimestampInsertionSet, 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionEgressTimeStampInsertCancel,
                                 0, _FieldActionTxTimestampInsertionSet, 0, 2, 2);

    /* RX_TIMESTAMP_INSERTION_SET (4b) */
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsert,
                                 0, _FieldActionRxTimestampInsertionSet, 0, 2, 1);
    _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionIngressTimeStampInsertCancel,
                                 0, _FieldActionRxTimestampInsertionSet, 0, 2, 2);

    if (soc_feature(unit, soc_feature_fcoe)) {
        /* IGNORE_FCOE_ZONE_CHECK_SET (1b) */
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionFibreChanZoneCheckActionCancel,
                                     0, _FieldActionIgnoreFcoEZoneCheckSet, 0, 1, 1);

        /* FCOE_VSAN_SET (16b) */
        _FP_ACTION_WITH_ASET_ADD_TWO(unit, stage_fc, bcmFieldActionFibreChanIntVsanPri,
                                     0, _FieldActionFcoEVsanSet, 0, 1, 1, 1, 3, -1);
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc, bcmFieldActionFibreChanVsanId,
                                     0, _FieldActionFcoEVsanSet, 4, 12, -1);
    }

#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        /* TS0_SOURCE_SET (1b) */
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
            bcmFieldActionFlowTimestampType, _BCM_FIELD_ACTION_NO_EM_SUPPORT,
            _FieldActionTimestampTypeSet, 0, 1, -1);

        /* IFP_TS0_TYPE_SET (2b) */
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
            bcmFieldActionFlowTimestampSource0, _BCM_FIELD_ACTION_NO_EM_SUPPORT,
            _FieldActionTimestampSource0Set, 0, 2, -1);

        /* IFP_TS1_TYPE_SET (2b) */
        _FP_ACTION_WITH_ASET_ADD_ONE(unit, stage_fc,
            bcmFieldActionFlowTimestampSource1, _BCM_FIELD_ACTION_NO_EM_SUPPORT,
            _FieldActionTimestampSource1Set, 0, 2, -1);
    }
#endif /* BCM_HURRICANE4_SUPPORT & BCM_FIREBOLT6_SUPPORT */

    return (_field_td3_flex_actions_init(unit, stage_fc));
}

/*
 * Function:
 *     _field_td3_ingress_entry_policy_mem_install
 * Purpose:
 *     Install the entry actions into the policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     policy_mem   - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 * Returns:
 *     BCM_E_XXX
 */
int
_field_td3_ingress_entry_policy_mem_install(int unit, _field_entry_t *f_ent,
                                           soc_mem_t policy_mem, int tcam_idx)
{
    int               rv=BCM_E_NONE;
                                       /* Operation return status.            */
    int               policy_idx;      /* Policy index in the Policy table.   */
    uint32            abuf[_BCM_FP_MEM_FIELD_BYTES] = {0};
                                       /* Action Profile Data Buffer. */
    _field_group_t *fg = NULL;         /* Field Group Control.        */
    ifp_policy_table_entry_t ibuf_nar; /* Narrow Entry Buffer.      */
    ifp_policy_table_wide_entry_t ibuf_wide;    /* Wide Entry Buffer.        */
    uint32 *bufp = NULL;               /* Hardware Buffer Ptr.      */
    uint32 ap_idx;                     /* Action profile Index*/
    uint8 entry_part;                  /* Entry part number */
    _field_entry_t *f_ent_primary = NULL;

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }
    fg = f_ent->group;
    /* Get Hardware Buffer Pointer and memory identifier. */
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
           !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        bufp = (uint32 *)&ibuf_nar;
    } else {
        bufp = (uint32 *)&ibuf_wide;
    }

    /* Initialize exact match entries. */
    sal_memcpy(bufp, soc_mem_entry_null(unit, policy_mem),
                soc_mem_entry_words(unit, policy_mem) * sizeof(uint32));

    /* (Intra Slice) Double Wide/ Triple wide modes,
     * IFP_TCAM_WIDE have 256 entries
     * but IFP_POLICY_TABLE will have 512 entries.
     */
    policy_idx = tcam_idx;
    _bcm_field_th_entry_flags_to_tcam_part(unit, f_ent->flags,
                                   fg->flags, &entry_part);
    ap_idx = fg->action_profile_idx[entry_part];
    if (ap_idx != -1) {
        /* Set Action Profile Data. */
        rv = _field_entry_get(unit, f_ent->eid, _FP_ENTRY_PRIMARY, &f_ent_primary);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        rv = _field_th_entry_action_profile_data_set(unit,
                                            f_ent_primary,
                                            &ap_idx, policy_mem,
                                            abuf);
        BCM_IF_ERROR_RETURN(rv);
        /* Write the POLICY Table */
        /* Set Action Data in entry Buffer. */
        soc_mem_field32_set(unit, policy_mem,
                            bufp,
                            DATA_TYPEf, ap_idx);
        soc_mem_field_set(unit, policy_mem,
                            bufp,
                            POLICY_DATAf, abuf);

        rv = soc_th_ifp_mem_write(unit, policy_mem,
                             MEM_BLOCK_ALL, policy_idx, bufp);
    }
    return (rv);
}

/*
 * Function:
 *  _field_td3_field_group_aset_update
 * Purpose:
 *  Update group aset as per aset updated by user.
 * Parameters:
 *  unit     - (IN) BCM device number
 *  fg       - (IN/OUT) field group structure
 *  aset     - (IN) User specified action set
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This function logic takes care of IFP stage in TD3.
 *  It will add action from aset provided by user as input.
 *  Final group aset will be union of field group aset
 *  already present and aset provided by user. It will update
 *  aset only if group mode does not change due to new actions.
 */
int
_field_td3_field_group_aset_update(int unit, _field_group_t *fg,
                                         bcm_field_aset_t  *aset)
{
    int rv = BCM_E_NONE;                   /* operational status */
    _field_control_t *fc = NULL;           /* field control structure */
    _field_stage_t *stage_fc = NULL;       /* stage control strucutre */
    _field_entry_t *f_ent = NULL;          /* field entry strucutre */
    soc_mem_t action_profile_mem, policy_mem;
                                           /* policy profile and policy memory */
    ifp_policy_table_entry_t ifp_act_policy_narrow;
                                           /* narrow policy entry */
    ifp_policy_table_wide_entry_t ifp_act_policy_wide;
                                           /* wide policy entry */
    uint32 *act_policy_entry = NULL;       /* policy entry pointer */
    ifp_policy_action_profile_entry_t
            ifp_act_prof_entry[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* policy profile entry */
    ifp_policy_action_profile_entry_t
            ifp_act_prof_entry_new[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* policy profile entry */
    void *act_prof_entry[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* policy profile entry pointer */
    void *act_prof_entry_new[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* policy profile entry pointer */
    void *entries[1];                      /* policy profile entry generic pointer */
    uint16 min_hw_aset_idx_loc[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* minimum hw action set index */
    uint32 old_ap_idx[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* old action profile index */
    uint32 new_ap_idx[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* new action profile index */
    bcm_field_aset_t aset_incoming;        /* input user action set */
    _bcm_field_aset_t aset_orig;           /* field group original action set */
    _bcm_field_aset_t aset_new;            /* new action set to add */
    uint8 hw_bitmap_entries = 0;           /* no. of hw bitmap enties */
    int hw_aset_idx = 0;                   /* hw action set iter */
    uint16 ap_idx = 0;                     /* action profile index iter */
    uint16 a_idx = 0, aset_diff = 0;       /* action iter and diff */
    _bcm_field_action_set_t *action_set_ptr = NULL;
                                           /* action set pointer */
    _bcm_field_action_conf_t *action_conf_ptr = NULL;
                                           /* action configuration pointer */
    bcm_field_group_mode_t mode = 0;       /* field group mode */
    uint16 mode_violation = 0;             /* field group mode violation */
    uint16 multi_mode = 0, multi_mode_transition = 0;
                                           /* group multi mode variables */
    uint16 aset_size_old = 0, aset_size_new = 0;
                                           /* action set size */
    uint16 group_new_action_profile = 0;   /* new action profile */
    uint16 entry_new_action_profile = 0;   /* new action profile */
    uint16 entry_install_flag = 0;         /* entry install flag */
    uint32 act_valid = 0;                  /* action set valid */
    uint8 field_action_set = 0;            /* field action set */
    uint32 abuf[_BCM_FP_MEM_FIELD_BYTES] = {0};
                                           /* action data buffer */
    int eid = 0, idx = 0, tmp = 0, tcam_idx = 0;
                                           /* entry variables */
    int parts_count = 0;                   /* entry parts count */

    /* input params validation */
    if ((fg == NULL) || (fg->stage_id != _BCM_FIELD_STAGE_INGRESS)) {
        return (BCM_E_INTERNAL);
    }

    /* get field and stage control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id, &stage_fc));

    /* assign memory */
    action_profile_mem = IFP_POLICY_ACTION_PROFILEm;
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
       !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        policy_mem = IFP_POLICY_TABLEm;
        act_policy_entry = (uint32 *)&ifp_act_policy_narrow;
    } else {
        policy_mem = IFP_POLICY_TABLE_WIDEm;
        act_policy_entry = (uint32 *)&ifp_act_policy_wide;
    }

    /* initialize local params */
    sal_memset(&(aset_incoming), 0, sizeof(bcm_field_aset_t));
    sal_memset(&(aset_orig), 0, sizeof(_bcm_field_aset_t));
    sal_memset(&(aset_new), 0, sizeof(_bcm_field_aset_t));
    sal_memset(&ifp_act_policy_narrow, 0, sizeof(ifp_act_policy_narrow));
    sal_memset(&ifp_act_policy_wide, 0, sizeof(ifp_act_policy_wide));
    for (ap_idx = 0; ap_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx++) {
        act_prof_entry[ap_idx] = &ifp_act_prof_entry[ap_idx];
        act_prof_entry_new[ap_idx] = &ifp_act_prof_entry_new[ap_idx];
        sal_memcpy(act_prof_entry[ap_idx], soc_mem_entry_null(unit, action_profile_mem),
                        soc_mem_entry_words(unit, action_profile_mem) * sizeof(uint32));
        sal_memcpy(act_prof_entry_new[ap_idx], soc_mem_entry_null(unit, action_profile_mem),
                        soc_mem_entry_words(unit, action_profile_mem) * sizeof(uint32));
        min_hw_aset_idx_loc[ap_idx] = -1;
        old_ap_idx[ap_idx] = -1;
        new_ap_idx[ap_idx] = -1;
    }
    sal_memcpy(&aset_incoming, aset, sizeof(bcm_field_aset_t));
#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        hw_bitmap_entries = sizeof(ifp_hr4_action_profile_bitmap)/sizeof(uint8);
    } else
#endif
    {
        hw_bitmap_entries = sizeof(ifp_td3_action_profile_bitmap)/sizeof(uint8);
    }

    /* copy field group original aset */
    sal_memcpy(&aset_orig, &fg->aset, sizeof(_bcm_field_aset_t));

    /* calculate action size with old field group aset */
    BCM_IF_ERROR_RETURN(_field_calc_group_aset_size(unit, fg, &aset_size_old));

    /* check if old group was in multi mode or not */
    if (aset_size_old > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE) {
        multi_mode = 1;
    }

    /* update new and field group aset */
    for (a_idx = 0; a_idx < bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(aset_incoming, a_idx)) {
            if (!_field_stage_action_support_check(unit, fg, a_idx)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                         (BSL_META_U(unit, "FP(unit %d) Error: action =%s is"
                                    "not supported for given group stage.\n"),
                                    unit, _field_action_name(a_idx)));
                rv = BCM_E_UNAVAIL;
                goto cleanup;
            }
            if (!BCM_FIELD_ASET_TEST(fg->aset, a_idx)) {
                BCM_FIELD_ASET_ADD(aset_new, a_idx);
                BCM_FIELD_ASET_ADD(fg->aset, a_idx);
                aset_diff = 1;
            }
        }
    }

    /* if no aset difference then return success as nothing to do */
    if (!aset_diff) {
        return (BCM_E_NONE);
    }

    /* Update internal pseudo actions for the given aset */
    rv = _bcm_field_qset_update_with_internal_actions(unit, fg);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* calculate action size with updated field group aset */
    rv = _field_calc_group_aset_size(unit, fg, &aset_size_new);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* get field group mode */
    rv = _bcm_esw_field_group_mode_get(fg->flags, &mode);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* check new aset_size can accomodate in group mode */
    if (mode == bcmFieldGroupModeSingle) {
        if (aset_size_new > _FP_ASET_DATA_SIZE_FOR_POLICY_NARROW) {
            mode_violation = 1;
        }
    } else if (mode == bcmFieldGroupModeDouble ||
               mode == bcmFieldGroupModeTriple) {
        if (aset_size_new > _FP_ASET_DATA_SIZE_WITH_ALL_ACTIONS) {
            mode_violation = 1;
        }
        if ((!multi_mode) && (aset_size_new > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE)) {
            multi_mode_transition = 1;
        }
    } else if (mode == bcmFieldGroupModeIntraSliceDouble) {
        if (aset_size_new > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE) {
            mode_violation = 1;
        }
    } else {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* if group mode is violated return error */
    if (mode_violation) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit, "FP(unit %d) Error: New Aset size =%d cannot"
                             "be accomodated in this group mode.\n"),
                             unit, aset_size_new));
        rv = BCM_E_PARAM;
        goto cleanup;
    }

    /* check if there is any other group with same action_profile index */
    rv = _field_group_check_action_profile(unit, fg, &group_new_action_profile);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* get min aset location in both action profile */
    for (ap_idx = 0; ap_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx++) {
        if (fg->action_profile_idx[ap_idx] != -1) {
            entries[0] = act_prof_entry[ap_idx];
            soc_profile_mem_get(unit,
                                &stage_fc->action_profile[fg->instance],
                                fg->action_profile_idx[ap_idx], 1, entries);
            for (hw_aset_idx = (hw_bitmap_entries-1); hw_aset_idx >= 0; hw_aset_idx--) {
                _bcm_field_th_val_get(act_prof_entry[ap_idx], &act_valid, hw_aset_idx, 1);
                if (act_valid) {
                    min_hw_aset_idx_loc[ap_idx] = hw_aset_idx;
                }
            }

        }
    }

    /* verify if new actions can be updated with same profile
     * or new profile has to be created. */
    action_set_ptr = stage_fc->action_set_ptr;
    if ((!multi_mode) && (!multi_mode_transition)) {

        /* now update action profile entry with new action set */
        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(aset_new, a_idx)) {
                action_conf_ptr = stage_fc->f_action_arr[a_idx];
                if (NULL != action_conf_ptr) {
                    field_action_set = action_conf_ptr->offset->aset;
                } else if (a_idx == bcmFieldActionPolicerGroup) {
                    field_action_set = _FieldActionMeterSet;
                } else if (a_idx == bcmFieldActionStatGroup) {
                    field_action_set = _FieldActionCounterSet;
                } else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                if ((0 == field_action_set) ||
                    (0 == action_set_ptr[field_action_set].size)) {
                    continue;
                } else {
                    soc_mem_field32_set(unit, action_profile_mem, act_prof_entry[0],
                            action_set_ptr[field_action_set].hw_field, 1);
                    soc_mem_field32_set(unit, action_profile_mem, act_prof_entry_new[0],
                            action_set_ptr[field_action_set].hw_field, 1);
                }
            }
        }
        /* now check new action set position with earlier to judge profile update need */
        for (hw_aset_idx = hw_bitmap_entries-1; hw_aset_idx >= 0; hw_aset_idx--) {
            _bcm_field_th_val_get(act_prof_entry_new[0], &act_valid, hw_aset_idx, 1);
            if (act_valid) {
                if (hw_aset_idx > min_hw_aset_idx_loc[0]) {
                    entry_new_action_profile = 1;
                    break;
                }
            }
        }

    } else if ((!multi_mode) && (multi_mode_transition)) {

        /* Entry moving from one to two action profile, clear earlier entry first */
        sal_memcpy(act_prof_entry[0], soc_mem_entry_null(unit, action_profile_mem),
                soc_mem_entry_words(unit, action_profile_mem) * sizeof(uint32));
        sal_memcpy(act_prof_entry[1], soc_mem_entry_null(unit, action_profile_mem),
                soc_mem_entry_words(unit, action_profile_mem) * sizeof(uint32));
        entry_new_action_profile = 1;
        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(fg->aset, a_idx)) {
                action_conf_ptr = stage_fc->f_action_arr[a_idx];
                if (NULL != action_conf_ptr) {
                    field_action_set = action_conf_ptr->offset->aset;
                } else if (a_idx == bcmFieldActionPolicerGroup) {
                    field_action_set = _FieldActionMeterSet;
                } else if (a_idx == bcmFieldActionStatGroup) {
                    field_action_set = _FieldActionCounterSet;
                } else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                if ((0 == field_action_set) ||
                    (0 == action_set_ptr[field_action_set].size)) {
                    continue;
                } else {
                    if (1 == action_set_ptr[field_action_set].is_color_dependent) {
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry[0],
                                action_set_ptr[field_action_set].hw_field, 1);
                    } else {
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry[1],
                                action_set_ptr[field_action_set].hw_field, 1);
                    }
                }
            }
        }

    } else {

        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(aset_new, a_idx)) {
                action_conf_ptr = stage_fc->f_action_arr[a_idx];
                if (NULL != action_conf_ptr) {
                    field_action_set = action_conf_ptr->offset->aset;
                } else if (a_idx == bcmFieldActionPolicerGroup) {
                    field_action_set = _FieldActionMeterSet;
                } else if (a_idx == bcmFieldActionStatGroup) {
                    field_action_set = _FieldActionCounterSet;
                } else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
                if ((0 == field_action_set) ||
                    (0 == action_set_ptr[field_action_set].size)) {
                    continue;
                } else {
                    if (1 == action_set_ptr[field_action_set].is_color_dependent) {
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry[0],
                                action_set_ptr[field_action_set].hw_field, 1);
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry_new[0],
                                action_set_ptr[field_action_set].hw_field, 1);
                    } else {
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry[1],
                                action_set_ptr[field_action_set].hw_field, 1);
                        soc_mem_field32_set(unit, action_profile_mem, act_prof_entry_new[1],
                                action_set_ptr[field_action_set].hw_field, 1);
                    }
                }
            }
        }

        /* now check new action set position with earlier to judge profile update need */
        for (hw_aset_idx = hw_bitmap_entries-1; hw_aset_idx >= 0; hw_aset_idx--) {
            _bcm_field_th_val_get(act_prof_entry_new[0], &act_valid, hw_aset_idx, 1);
            if (act_valid) {
                if (hw_aset_idx > min_hw_aset_idx_loc[0]) {
                    entry_new_action_profile = 1;
                    break;
                }
            }
        }

        for (hw_aset_idx = hw_bitmap_entries-1; hw_aset_idx >= 0; hw_aset_idx--) {
            _bcm_field_th_val_get(act_prof_entry_new[1], &act_valid, hw_aset_idx, 1);
            if (act_valid) {
                if (hw_aset_idx > min_hw_aset_idx_loc[1]) {
                    entry_new_action_profile = 1;
                    break;
                }
            }
        }
    }

    /* check if there is any entry installed in group */
    while (tmp < fg->group_status.entry_count) {
        eid = (fg->entry_arr[tmp])->eid;

        rv = _field_entry_get(unit, eid, _FP_ENTRY_PRIMARY, &f_ent);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        if (f_ent->flags & _FP_ENTRY_INSTALLED) {
            entry_install_flag = 1;
            break;
        }
        tmp++;
    }

    /* if no entry is installed but action set is different,still update same profile */
    if (entry_new_action_profile) {
        if (!entry_install_flag) {
            entry_new_action_profile = 0;
        }
    }

    /* time to update action profile and group entries */
    if (group_new_action_profile || entry_new_action_profile) {

        /* create new profile entry and get new action profile index */
        if ((!multi_mode) && (!multi_mode_transition)) {
            entries[0] = act_prof_entry[0];
            rv = soc_profile_mem_add(unit,
                                     &stage_fc->action_profile[fg->instance],
                                     entries, 1, &new_ap_idx[0]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            old_ap_idx[0] = fg->action_profile_idx[0];
            fg->action_profile_idx[0] = new_ap_idx[0];
        } else if ((!multi_mode) && (multi_mode_transition)) {
            entries[0] = act_prof_entry[0];
            rv = soc_profile_mem_add(unit,
                                     &stage_fc->action_profile[fg->instance],
                                     entries, 1, &new_ap_idx[0]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            entries[0] = act_prof_entry[1];
            rv = soc_profile_mem_add(unit,
                                     &stage_fc->action_profile[fg->instance],
                                     entries, 1, &new_ap_idx[1]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            old_ap_idx[0] = fg->action_profile_idx[0];
            fg->action_profile_idx[0] = new_ap_idx[0];
            fg->action_profile_idx[1] = new_ap_idx[1];
        } else {
            entries[0] = act_prof_entry[0];
            rv = soc_profile_mem_add(unit,
                                     &stage_fc->action_profile[fg->instance],
                                     entries, 1, &new_ap_idx[0]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            entries[0] = act_prof_entry[1];
            rv = soc_profile_mem_add(unit,
                                     &stage_fc->action_profile[fg->instance],
                                     entries, 1, &new_ap_idx[1]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            old_ap_idx[0] = fg->action_profile_idx[0];
            old_ap_idx[1] = fg->action_profile_idx[1];
            fg->action_profile_idx[0] = new_ap_idx[0];
            fg->action_profile_idx[1] = new_ap_idx[1];
        }

        /* loop through all entries in group and update only installed entries */
        while (tmp < fg->group_status.entry_count) {
            eid = (fg->entry_arr[tmp])->eid;

            /* get entry description structure. */
            rv = _field_entry_get(unit, eid, _FP_ENTRY_PRIMARY, &f_ent);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            if (f_ent->flags & _FP_ENTRY_INSTALLED) {
                rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                       fg->flags, &parts_count);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                for (idx = 0; idx < parts_count; idx++) {
                    sal_memset(abuf, 0, sizeof(abuf));
                    /* Install entry per part here, first get tcam_idx */
                    rv = _bcm_field_entry_tcam_idx_get(unit, f_ent + idx, &tcam_idx);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }

                    /* set action profile data. */
                    if (new_ap_idx[idx] != -1) {
                        rv = _field_th_entry_action_profile_data_set(unit,
                                                                     f_ent,
                                                                     &new_ap_idx[idx],
                                                                     policy_mem,
                                                                     abuf);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }

                        /* write the policy table */
                        soc_mem_field32_set(unit, policy_mem,
                                            act_policy_entry,
                                            DATA_TYPEf, new_ap_idx[idx]);
                        soc_mem_field_set(unit, policy_mem,
                                          act_policy_entry,
                                          POLICY_DATAf, abuf);

                        rv = soc_th_ifp_mem_write(unit, policy_mem,
                                                  MEM_BLOCK_ALL, tcam_idx,
                                                  act_policy_entry);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
            }
            tmp++;
        }

        /* delete old profile index entry */
        for (ap_idx = 0; ap_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx++) {
            if (old_ap_idx[ap_idx] != -1) {
                BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                                    &stage_fc->action_profile[fg->instance],
                                    old_ap_idx[ap_idx]));
            }
        }

    } else {

        /* update action profile entry at same index */
        for (ap_idx = 0; ap_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx++) {
            entries[0] = act_prof_entry[ap_idx];
            if (fg->action_profile_idx[ap_idx] != -1) {
                soc_profile_mem_set(unit,
                        &stage_fc->action_profile[fg->instance],
                        entries,
                        fg->action_profile_idx[ap_idx]);
            }
        }
    }

    return rv;

cleanup:
    /* restore original field group aset */
    sal_memcpy(&fg->aset, &aset_orig, sizeof(_bcm_field_aset_t));
    for (ap_idx = 0; ap_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx++) {
        if (new_ap_idx[ap_idx] != -1) {
            BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                                &stage_fc->action_profile[fg->instance],
                                new_ap_idx[ap_idx]));
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_td3_presel_qualifiers_init
 *
 * Purpose:
 *    Initialize Field Ingress Stage Preselector Qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_td3_presel_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /*
     * All the qualifiers offset mentioned are the actual individual HW field
     * offset in IFP_LOGICAL_TABLE_SELECT Memory which are starting from bit
     * position 2. "KEY" overlap field in the memory is used during write,
     * hence to accomidate in the field, the offsets of the qualifiers has to
     * be substracted by 2.
     */
    static int8 key_base_off;
    soc_mem_t mem = INVALIDm;
    soc_field_info_t *field_info = NULL;
    _FP_QUAL_DECL;

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_presel_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
        "IFP Preselector qualifiers");

    if (stage_fc->f_presel_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
       mem = EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
       mem = BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm;
#endif
    } else {
       mem = IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
    }

    field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
    if (field_info == NULL) {
       return BCM_E_PARAM;
    }
    key_base_off = field_info->bp;

    /* Preselector qualifiers. */
    /* SOURCE_CLASS - 32 */
    field_info = soc_mem_fieldinfo_get(unit, mem, SOURCE_CLASSf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMixedSrcClassId,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 32);
    }

    /* EXACT_MATCH_LOGICAL_TABLE_ID_0 - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem, EXACT_MATCH_LOGICAL_TABLE_ID_0f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc,
            _bcmFieldQualifyExactMatchGroupClassIdLookup0,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 5);
    }

    /* EXACT_MATCH_LOGICAL_TABLE_ID_1 - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem, EXACT_MATCH_LOGICAL_TABLE_ID_1f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc,
            _bcmFieldQualifyExactMatchGroupClassIdLookup1,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 5);
    }

    /* VFI_VALID - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, VFI_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2Type,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }
    /* CPU_PKT_PROFILE_KEY_CONTROL - 1 */
#if 0
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualify,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        45 - key_base_off , 1);
#endif
    /* MIRROR - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, MIRRORf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* DROP - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, DROPf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* PKT_RESOLUTION - 6 */
    field_info = soc_mem_fieldinfo_get(unit, mem, PKT_RESOLUTIONf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 6);
    }

    /* FORWARDING_TYPE - 4 */
    field_info = soc_mem_fieldinfo_get(unit, mem, FORWARDING_TYPEf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 4);
    }

    /* LOOKUP_STATUS_VECTOR - 19 */
    field_info = soc_mem_fieldinfo_get(unit, mem, LOOKUP_STATUS_VECTORf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+0 - key_base_off , 3);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+3 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 2);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStpState,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 2);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+6 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+7 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+8 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+9 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
             _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+10 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+10 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+11 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcRouteHit,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+12 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+13 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
            _BCM_FIELD_QUAL_OFFSET_NO_FT_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+14 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+15 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+16 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+16 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+16 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+16 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+17 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+17 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+17 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+17 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+18 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+19 - key_base_off , 1);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+23 - key_base_off , 1);
    }

    /* SVP_VALID - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, SVP_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }
    /* TUNNEL_TERMINATED - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, TUNNEL_TERMINATEDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }
    /* MY_STATION_HIT - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, MY_STATION_HITf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStation2Hit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* L4_VALID - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, L4_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* L3_TYPE - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem, L3_TYPEf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 5);
    }

    /* PKT_FLOW_ID_0 - 171 */
    field_info = soc_mem_fieldinfo_get(unit, mem, PKT_FLOW_ID_0f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFrontPanelPkt,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 8);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 8);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxyTunnelType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 8);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+4 - key_base_off , 8);
    }

    /* PKT_FLOW_ID_1 - 157 */
    field_info = soc_mem_fieldinfo_get(unit, mem, PKT_FLOW_ID_1f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelSubType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 6);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingPacketRes,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+6 - key_base_off , 4);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp+10 - key_base_off, 4);
    }
    /* Additional fields for flowtracker stage */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) &&
        (soc_feature(unit, soc_feature_field_flowtracker_support))) {
        BCM_IF_ERROR_RETURN(_field_hx5_ft_presel_qualifiers_init(unit, stage_fc));
    }
#endif

    return _field_td3_flex_presel_qual_init(unit, stage_fc);
}

/*
 * Function:
 *     _bcm_field_td3_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_td3_action_params_check(int unit,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
#if defined (BCM_HURRICANE4_SUPPORT) && defined (INCLUDE_XFLOW_MACSEC)
    int rv = BCM_E_NONE;
    int port = -1;
    int modid = -1;
    int isLocal = 0;
    soc_info_t *si = &SOC_INFO(unit);
#endif

    if (NULL == f_ent ||
        NULL == fa) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id) {
        return BCM_E_INTERNAL;
    }

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
           switch (fa->action) {
              case bcmFieldActionVlanActions:
                  if (f_ent->vlan_action_set == NULL) {
                     return BCM_E_INTERNAL;
                  }
                  BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit,
                                              f_ent->vlan_action_set));
                  return BCM_E_NONE;
              case bcmFieldActionAssignOpaqueObject4:
              case bcmFieldActionAssignOpaqueObject3:
              case bcmFieldActionAssignOpaqueObject2:
              case bcmFieldActionAssignOpaqueObject1:
                  if (fa->param[0] >> 16) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
              case bcmFieldActionAssignExtractionCtrlId:
                  if ((fa->param[0] >> 8) || (fa->param[1] >> 8)) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
#if defined (BCM_HELIX5_SUPPORT)
              case bcmFieldActionCosMapSelect:
                  if (bcmFieldCosMapSelectCount <= fa->param[0]) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
#endif
              default:
                  break;
           }
           break;
        case _BCM_FIELD_STAGE_EGRESS:
           switch (fa->action) {
#if defined (BCM_HURRICANE4_SUPPORT) && defined (INCLUDE_XFLOW_MACSEC)
              case bcmFieldActionMacSecSubPortNumAdd:
                 /* coverity[dead_error_condition] */
                  /* coverity[unsigned_compare] */
                  if (BCM_XFLOW_MACSEC_SUBPORT_ID_INDEX_GET(fa->param[0]) < 0) {
                      /* coverity[dead_error_begin] */
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
              case bcmFieldActionMacSecUseSubportNumberFromEvxlt:
                  return BCM_E_NONE;
              case bcmFieldActionMacSecDecryptPktType:
              case bcmFieldActionMacSecEncryptPktType:
                  if((fa->param[0] != bcmFieldMacSecEncryptDecryptPktData)
                          && (fa->param[0] != bcmFieldMacSecEncryptDecryptPktControl)) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
#endif
#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
              case bcmFieldActionSvtagSignature:
              case bcmFieldActionSvtagOffset:
              case bcmFieldActionSvtagSubportNum:
                  if (fa->param[0] >> 8) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
              case bcmFieldActionSvtagPrio:
                  if (fa->param[0] >> 1) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
              case bcmFieldActionSvtagDeviceId:
                  if (fa->param[0] >> 2) {
                      return BCM_E_PARAM;
                  }
                  return BCM_E_NONE;
              case bcmFieldActionSvtagPktType:
                switch (fa->param[0]) {
                    case bcmFieldMacSecPktTypeNonMacSec:
                    case bcmFieldMacSecPktTypeMacSecMgmt:
                    case bcmFieldMacSecPktTypeKaYFrames:
                    case bcmFieldMacSecPktTypeMacSec:
                        break;
                    default:
                        return BCM_E_PARAM;
                }
                return BCM_E_NONE;
#endif
              default:
                  break;
           }
           break;
#if defined (BCM_HURRICANE4_SUPPORT) && defined (INCLUDE_XFLOW_MACSEC)
        case _BCM_FIELD_STAGE_INGRESS:
           switch (fa->action) {
              case bcmFieldActionRedirectPort:
                  /* Allow MACSEC port only if speed is set */
                  if (SOC_IS_HURRICANE4(unit) &&
                      soc_feature(unit, soc_feature_xflow_macsec_svtag) &&
                      (!BCM_GPORT_IS_SET(fa->param[1]) ||
                       BCM_GPORT_IS_LOCAL(fa->param[1]) ||
                       BCM_GPORT_IS_MODPORT(fa->param[1]))) {

                      if (BCM_GPORT_IS_LOCAL(fa->param[1])) {
                          rv = bcm_esw_stk_my_modid_get(unit, &modid);
                          if (BCM_FAILURE(rv)) {
                              break;
                          }
                          port = BCM_GPORT_LOCAL_GET(fa->param[1]);
                      } else if (BCM_GPORT_IS_MODPORT(fa->param[1])) {
                          modid = BCM_GPORT_MODPORT_MODID_GET(fa->param[1]);
                          port = BCM_GPORT_MODPORT_PORT_GET(fa->param[1]);
                      } else {
                          modid = fa->param[0];
                          port = fa->param[1];
                      }

                      rv = _bcm_esw_modid_is_local(unit, modid, &isLocal);
                      if (BCM_SUCCESS(rv) && isLocal &&
                          (port > 0) && (port < SOC_MAX_NUM_PORTS)) {
                          if (IS_MACSEC_PORT(unit, port) &&
                              (si->port_init_speed[port] == 0)) {
                              return BCM_E_PARAM;
                          }
                      }
                  }
              default:
                  break;
           }
           break;
#endif
        default:
            ;
    }

    return _bcm_field_th_action_params_check(unit, f_ent, fa);
}

/*
 * Routine to Set Match ID width partition for each zone
 * for a given stage.
 */
int
_bcm_field_td3_zone_match_id_partition_init(int unit,
                                            _field_stage_t *stage_fc)
{
    uint8 inst;
    uint32 entry;
    soc_reg_t *reg;
    soc_reg_t ifp_ing_match_id_cfg_reg[_FP_MAX_NUM_PIPES + 1] =
             {
                 IFP_ING_ZONE_MATCH_ID_CONFIG_PIPE0r,
                 IFP_ING_ZONE_MATCH_ID_CONFIG_PIPE1r,
                 INVALIDr,
                 INVALIDr,
#if defined (BCM_TOMAHAWK3_SUPPORT)
                 INVALIDr,
                 INVALIDr,
                 INVALIDr,
                 INVALIDr,
#endif
                 IFP_ING_ZONE_MATCH_ID_CONFIGr
             };
    soc_reg_t em_ing_match_id_cfg_reg[_FP_MAX_NUM_PIPES + 1] =
             {
                 EXACT_MATCH_ING_ZONE_MATCH_ID_CONFIG_PIPE0r,
                 EXACT_MATCH_ING_ZONE_MATCH_ID_CONFIG_PIPE1r,
                 INVALIDr,
                 INVALIDr,
#if defined (BCM_TOMAHAWK3_SUPPORT)
                 INVALIDr,
                 INVALIDr,
                 INVALIDr,
                 INVALIDr,
#endif
                 EXACT_MATCH_ING_ZONE_MATCH_ID_CONFIGr
             };

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
       reg = ifp_ing_match_id_cfg_reg;
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
       reg = em_ing_match_id_cfg_reg;
    } else {
       return BCM_E_PARAM;
    }

    for (inst = 0; inst < stage_fc->num_instances; inst++) {
       if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
          inst = _FP_MAX_NUM_PIPES;
       }

       entry = 0;
       soc_reg_field_set(unit, reg[inst], &entry, ZONE_0_MATCH_ID_WIDTHf, _FP_TD3_ZONE_0_MATCH_ID_WIDTH);
       soc_reg_field_set(unit, reg[inst], &entry, ZONE_1_MATCH_ID_WIDTHf, _FP_TD3_ZONE_1_MATCH_ID_WIDTH);
       soc_reg_field_set(unit, reg[inst], &entry, ZONE_2_MATCH_ID_WIDTHf, _FP_TD3_ZONE_2_MATCH_ID_WIDTH);
       soc_reg_field_set(unit, reg[inst], &entry, ZONE_3_MATCH_ID_WIDTHf, _FP_TD3_ZONE_3_MATCH_ID_WIDTH);
       soc_reg_field_set(unit, reg[inst], &entry, ZONE_4_MATCH_ID_WIDTHf, _FP_TD3_ZONE_4_MATCH_ID_WIDTH);

       BCM_IF_ERROR_RETURN (soc_reg32_set(unit, reg[inst], REG_PORT_ANY, 0, entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_td3_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_action_support_check(int unit,
                                    _field_entry_t *f_ent,
                                    bcm_field_action_t action,
                                    int *result)
{
    if ((NULL == result) || (NULL == f_ent)) {
        return BCM_E_PARAM;
    }

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
             switch (action) {
                  case bcmFieldActionVlanActions:
                  case bcmFieldActionPacketTraceEnable:
                  case bcmFieldActionAssignOpaqueObject4:
                  case bcmFieldActionAssignOpaqueObject3:
                  case bcmFieldActionAssignOpaqueObject2:
                  case bcmFieldActionAssignOpaqueObject1:
                  case bcmFieldActionAssignExtractionCtrlId:
                      *result = TRUE;
                      return BCM_E_NONE;
#if defined (BCM_HELIX5_SUPPORT)
                  case bcmFieldActionCosMapSelect:
                      *result = (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit));
                      return BCM_E_NONE;
#endif
                  case bcmFieldActionFlowtrackerGroupId:
                      *result = soc_feature(unit, soc_feature_mv2_style_flowtracker)
                                                                      ? TRUE : FALSE;
                      return BCM_E_NONE;
                  case bcmFieldActionOuterVlanCfiNew:
                  case bcmFieldActionOuterVlanAdd:
                  case bcmFieldActionOuterVlanNew:
                  case bcmFieldActionOuterVlanPrioNew:
                  case bcmFieldActionOuterVlanPrioCopyInner:
                  case bcmFieldActionOuterVlanCfiCopyInner:
                  case bcmFieldActionOuterVlanCopyInner:
                  case bcmFieldActionOuterVlanLookup:
                  case bcmFieldActionInnerVlanCfiNew:
                  case bcmFieldActionInnerVlanAdd:
                  case bcmFieldActionInnerVlanDelete:
                  case bcmFieldActionInnerVlanNew:
                  case bcmFieldActionInnerVlanPrioNew:
                  case bcmFieldActionInnerVlanPrioCopyOuter:
                  case bcmFieldActionInnerVlanCfiCopyOuter:
                  case bcmFieldActionInnerVlanCopyOuter:
                  case bcmFieldActionPrioPktNew:
                  case bcmFieldActionPrioPktAndIntNew:
                       *result = FALSE;
                       return BCM_E_NONE;
                  default:
                       break;
             }
             break;
        case _BCM_FIELD_STAGE_EGRESS:
             switch (action) {
#if defined(INCLUDE_XFLOW_MACSEC) && defined (BCM_HURRICANE4_SUPPORT)
                  case bcmFieldActionMacSecSubPortNumAdd:
                  case bcmFieldActionMacSecUseSubportNumberFromEvxlt:
                  case bcmFieldActionMacSecDecryptPktType:
                  case bcmFieldActionMacSecEncryptPktType:
                      *result = soc_feature(unit, soc_feature_xflow_macsec_svtag);
                      return BCM_E_NONE;
#endif
#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
                  case bcmFieldActionSvtagSignature:
                  case bcmFieldActionSvtagOffset:
                  case bcmFieldActionSvtagPrio:
                  case bcmFieldActionSvtagDeviceId:
                  case bcmFieldActionSvtagPktType:
                  case bcmFieldActionSvtagSubportNum:
                  case bcmFieldActionSvtagEnable:
                      *result = soc_feature(unit, soc_feature_channelized_switching);
                      return BCM_E_NONE;
#endif
                 default:
                     break;
             }
             break;
        default:
             break;
    }

    return _bcm_field_th_action_support_check(unit, f_ent, action, result);
}

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
int
_field_td3_flex_data_mask_set(int unit, _field_control_t *fc)
{
    uint32 mask = 0;
    uint32 full_mask = 0;
    uint32 idx = 0;
    soc_mem_t mem = INVALIDm;
    uint32 index_max = 0;

    if (NULL == fc) {
        return BCM_E_PARAM;
    }

    mem = EFP_FLEX_DATA_MASKm;
    /* coverity[large_shift: FALSE] */
    index_max = soc_mem_index_max(unit, mem);

    /*
     * Construct a mask with bits set as per the memory depth.
     * Make it a negative value so that right shifting will carry the bits set from MSB.
     */
    /* coverity[assignment: FALSE] */
    /* coverity[large_shift: FALSE] */
    full_mask = ~((((1 << index_max) - 1) << 1) | 1);
    for (idx = soc_mem_index_min(unit, mem); idx <= index_max; idx++) {
        /*
         * Program Indices with bitmap of respective number of bits set.
         * i.e., idx=0 with bitmap of 1 bit  are set from MSB i.e., 0x8000
         * idx=1 with bitmap of 2 bits set from MSB i.e., 0xc000
         * idx=2 with bitmap of 3 bits set from MSB i.e., 0xe000
         * idx=3 with bitmap of 4 bits set from MSB i.e., 0xf000
         * idx=0xF with bitmap of 16 bits set from MSB i.e., 0xffff
         */
        /* coverity[bad_shift: FALSE] */
        mask = full_mask >> (idx + 1);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &mask));
    }

    return BCM_E_NONE;
}

int
_bcm_td3_flex_data2_policy_value_get(int unit,
                                     _field_entry_t *f_ent,
                                     _field_action_t *fa,
                                     uint32 *value)
{
    _field_action_t *fa_temp = NULL;

    *value = 0;
    for (fa_temp = f_ent->actions; (NULL != fa_temp); fa_temp = fa_temp->next) {
        switch (fa_temp->action) {
            case bcmFieldActionSvtagSubportNum:
                *value |= fa_temp->param[0];
                break;
            case bcmFieldActionSvtagPrio:
                *value |= fa_temp->param[0] << 15;
                break;
            case bcmFieldActionSvtagDeviceId:
                *value |= fa_temp->param[0] << 12;
                break;
            case bcmFieldActionSvtagPktType:
                switch (fa_temp->param[0]) {
                    case bcmFieldMacSecPktTypeNonMacSec:
                        *value |= 0x0 << 10;
                        break;
                    case bcmFieldMacSecPktTypeMacSecMgmt:
                        *value |= 0x2 << 10;
                        break;
                    case bcmFieldMacSecPktTypeKaYFrames:
                        *value |= 0x3 << 10;
                        break;
                    case bcmFieldMacSecPktTypeMacSec:
                        *value |= 0x1 << 10;
                        break;
                    default:
                        return BCM_E_INTERNAL;
                }
            default:
                /* Nothing to do.. break switch statement. Go over loop till end */
                break;
        }
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     _bcm_field_td3_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_td3_action_get(int             unit,
                          soc_mem_t       mem,
                          _field_entry_t  *f_ent,
                          int             tcam_idx,
                          _field_action_t *fa,
                          uint32          *buf
                          )
{
    bcm_vlan_action_set_t *actions;
#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
    uint32 value = 0;
    _field_action_t *fa_temp = NULL;
#endif
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
        case bcmFieldActionVlanActions:
            if ((f_ent->vlan_action_set == NULL) ||
                (fa->hw_index == _FP_INVALID_INDEX)) {
               return BCM_E_CONFIG;
            }
            actions = f_ent->vlan_action_set;
            PolicySet(unit, mem, buf, NEW_INNER_CFIf, actions->new_inner_cfi);
            PolicySet(unit, mem, buf, NEW_INNER_DOT1Pf, actions->new_inner_pkt_prio);
            PolicySet(unit, mem, buf, NEW_OUTER_CFIf, actions->new_outer_cfi);
            PolicySet(unit, mem, buf, NEW_OUTER_DOT1Pf, actions->priority);
            PolicySet(unit, mem, buf, NEW_INNER_VLANf, actions->new_inner_vlan);
            PolicySet(unit, mem, buf, NEW_OUTER_VLANf, actions->new_outer_vlan);
            PolicySet(unit, mem, buf, TAG_ACTION_PROFILE_PTRf, fa->hw_index);
            break;
        case bcmFieldActionPacketTraceEnable:
            PolicySet(unit, mem, buf, INSTRUMENTATION_TRIGGERS_ENABLEf, 0x1);
            break;
#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
        case bcmFieldActionMacSecSubPortNumAdd:
            /* FLEX_DATA_MASK (0) = 0000000000000000
             * FLEX_DATA_MASK (1) = 1100000000000000
             * FLEX_DATA_MASK (2) = 0000001111111111
             */
            PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x2);
            PolicySet(unit, mem, buf, FLEX_DATA_2_ACTIONf, 0x1);
            PolicySet(unit, mem, buf, FLEX_DATA_2f, BCM_XFLOW_MACSEC_SUBPORT_ID_INDEX_GET(fa->param[0]));
            break;
        case bcmFieldActionMacSecUseSubportNumberFromEvxlt:
            PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x0);
            PolicySet(unit, mem, buf, FLEX_DATA_2_ACTIONf, 0x3);
            break;
        case bcmFieldActionMacSecEncryptPktType:
            PolicySet(unit, mem, buf, FLEX_DATA_1_MASK_INDEXf, 0x1);
            PolicySet(unit, mem, buf, FLEX_TAG_ACTION_VALIDf, 0x1);
            PolicySet(unit, mem, buf, FLEX_DATA_1_ACTIONf, 0x2);
            if (fa->param[0] == bcmFieldMacSecEncryptDecryptPktData) {
                PolicySet(unit, mem, buf, FLEX_DATA_1f, 0x0);
            } else {
                PolicySet(unit, mem, buf, FLEX_DATA_1f, 0x4000);
                PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x2);
                PolicySet(unit, mem, buf, FLEX_DATA_2_ACTIONf, 0x1);
            }
            break;
        case bcmFieldActionMacSecDecryptPktType:
            PolicySet(unit, mem, buf, FLEX_DATA_1_MASK_INDEXf, 0x1);
            PolicySet(unit, mem, buf, FLEX_DATA_1_ACTIONf, 0x2);
            PolicySet(unit, mem, buf, FLEX_TAG_ACTION_VALIDf, 0x1);
            if (fa->param[0] == bcmFieldMacSecEncryptDecryptPktData) {
                PolicySet(unit, mem, buf, FLEX_DATA_1f, 0x8000);
            } else {
                PolicySet(unit, mem, buf, FLEX_DATA_1f, 0xc000);
            }
            PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x0);
            PolicySet(unit, mem, buf, FLEX_DATA_2_ACTIONf, 0x2);
            PolicySet(unit, mem, buf, FLEX_DATA_2f, 0x0);
            break;
#endif
#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
        case bcmFieldActionSvtagEnable:
            PolicySet(unit, mem, buf, FLEX_TAG_ACTION_VALIDf, 0x1);
            PolicySet(unit, mem, buf, FLEX_DATA_2_ACTIONf, 0x3);
            break;
        case bcmFieldActionSvtagSignature:
            for (fa_temp = f_ent->actions;
                (NULL != fa_temp) && (bcmFieldActionSvtagOffset != fa_temp->action);
                fa_temp = fa_temp->next);
            value = 0;
            if (NULL != fa_temp) {
                value = fa_temp->param[0];
            }
            value |= fa->param[0] << 8;
            PolicySet(unit, mem, buf, FLEX_DATA_1f, value);
            PolicySet(unit, mem, buf, FLEX_DATA_1_ACTIONf, 0x1);
            PolicySet(unit, mem, buf, FLEX_DATA_1_MASK_INDEXf, 0xF);
            break;
        case bcmFieldActionSvtagOffset:
            for (fa_temp = f_ent->actions;
                (NULL != fa_temp) && (bcmFieldActionSvtagSignature != fa_temp->action);
                fa_temp = fa_temp->next);
            value = 0;
            if (NULL != fa_temp) {
                value = fa_temp->param[0] << 8;
            }
            value |= fa->param[0];
            PolicySet(unit, mem, buf, FLEX_DATA_1f, value);
            PolicySet(unit, mem, buf, FLEX_DATA_1_ACTIONf, 0x1);
            /* Index to EFP_FLEX_DATA_MASKm */
            PolicySet(unit, mem, buf, FLEX_DATA_1_MASK_INDEXf, 0xF);
            break;
        case bcmFieldActionSvtagSubportNum:
            BCM_IF_ERROR_RETURN(_bcm_td3_flex_data2_policy_value_get(unit, f_ent, fa, &value));
            /*
             * bcmFieldActionSvtagSubportNum and bcmFieldActionSvtagPktType are not
             * conflicts as both can be present and bcmFieldActionSvtagSubportNum programs
             * whats needed for bcmFieldActionSvtagPktType as well.
             */
            /* Index to EFP_FLEX_DATA_MASKm */
            PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0xF);
            PolicySet(unit, mem, buf, FLEX_DATA_2f, value);
            break;
        case bcmFieldActionSvtagPrio:
        case bcmFieldActionSvtagDeviceId:
            BCM_IF_ERROR_RETURN(_bcm_td3_flex_data2_policy_value_get(unit, f_ent, fa, &value));
            PolicySet(unit, mem, buf, FLEX_DATA_2f, value);
            break;
        case bcmFieldActionSvtagPktType:
            switch (fa->param[0]) {
                case bcmFieldMacSecPktTypeNonMacSec:
                case bcmFieldMacSecPktTypeMacSecMgmt:
                case bcmFieldMacSecPktTypeKaYFrames:
                    /* Index to EFP_FLEX_DATA_MASKm */
                    PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x7);
                    break;
                case bcmFieldMacSecPktTypeMacSec:
                    /* Index to EFP_FLEX_DATA_MASKm */
                    PolicySet(unit, mem, buf, FLEX_DATA_2_MASK_INDEXf, 0x6);
                    break;
            }
            BCM_IF_ERROR_RETURN(_bcm_td3_flex_data2_policy_value_get(unit, f_ent, fa, &value));
            PolicySet(unit, mem, buf, FLEX_DATA_2f, value);
            break;
#endif
        default:
            return (BCM_E_UNAVAIL);
    }

    return BCM_E_NONE;
}

/*
 * Function to allocate HW resource for ING_VLAN_TAG_ACTION_PROFILE_TABLE
 * for the given VLAN action set.
 */
int _bcm_field_td3_vlan_action_hw_alloc(int unit, _field_entry_t *f_ent)
{
    int               rv;
    uint32            profile_idx;
    _field_action_t  *fa;

    if (f_ent == NULL) {
       return BCM_E_PARAM;
    }

    /* Extract action info from the entry structure. */
    for (fa = f_ent->actions;
         ((fa != NULL) && (_FP_ACTION_VALID & fa->flags));
         fa = fa->next) {
        /* Allocate mtp destination only for mirroring actions */
        if (bcmFieldActionVlanActions != fa->action) {
            continue;
        }

        if(_FP_ACTION_MODIFY & fa->flags) {
            fa->old_index = fa->hw_index;
            fa->hw_index = _FP_INVALID_INDEX;
        }

        if (_FP_INVALID_INDEX == fa->hw_index) {
            rv = _bcm_trx_vlan_action_profile_entry_add(unit,
                                      f_ent->vlan_action_set,
                                               &profile_idx);
            if ((BCM_E_RESOURCE == rv) &&
               (_FP_INVALID_INDEX != fa->old_index)) {
               int ref_count = 0;

               /*
                * Retrieve the reference count and if ref_count <= 1,
                * delete the profile and add new profile.
                */
               rv = _bcm_trx_vlan_action_profile_ref_count_get(unit,
                                         fa->old_index, &ref_count);
               if (BCM_FAILURE(rv)) {
                  return rv;
               }

                if (ref_count > 1) {
                    return (BCM_E_RESOURCE);
                }

               /*
                * Destroy old profile ONLY if it is not used by other entries.
                */
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                     fa->old_index);
                if (BCM_FAILURE(rv)) {
                   return rv;
                }

                /* Destroy old profile is no longer required. */
                fa->old_index = _FP_INVALID_INDEX;

                /* Reallocate profile for new action. */
                rv = _bcm_trx_vlan_action_profile_entry_add(unit,
                                          f_ent->vlan_action_set,
                                                   &profile_idx);
                if (BCM_FAILURE(rv)) {
                   return rv;
                }
            }
            fa->hw_index = profile_idx;
        }
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function to free HW resource allocated for ING_VLAN_TAG_ACTION_PROFILE_TABLE
 * for the given VLAN action set.
 */
int
_bcm_field_td3_vlan_action_hw_free(int unit, _field_entry_t *f_ent, uint32 flags)
{
    _field_action_t *fa;  /* Field action descriptor. */
    int rv = BCM_E_NONE;  /* Operation return status. */

    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_LOOKUP) {
        return (BCM_E_NONE);
    }

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionVlanActions:
              if ((flags & _FP_ACTION_RESOURCE_FREE) &&
                  (_FP_INVALID_INDEX != fa->hw_index)) {
                  rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                        fa->hw_index);
                  if (BCM_E_NOT_FOUND == rv) {
                      /*
                       * This issue should occur only when VlanActions is added and installed
                       * in the Field Entry and user triggered 'rc'.
                       * During 'rc', All VLAN action profiles are getting cleared by
                       * PORT/VLAN module init.
                       * Profile entry delete is expected to fail with BCM_E_NOT_FOUND error.
                       */
                      LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "Warning: Failed to delete the profile index of"
                                          " Action 'VlanActions' for Entry %d "
                                          "\n"),
                               f_ent->eid));
                      /* Overwrite the error as SUCCESS if BCM_E_NOT_FOUND */
                      rv = BCM_E_NONE;
                  }
                  BCM_IF_ERROR_RETURN(rv);
                  fa->hw_index = _FP_INVALID_INDEX;
              }

              if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) &&
                  (_FP_INVALID_INDEX != fa->old_index)) {
                  rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                        fa->old_index);
                  BCM_IF_ERROR_RETURN(rv);
                  fa->old_index = _FP_INVALID_INDEX;
              }
              break;
          default:
              break;
        }
    }
    return (rv);
}

/*
 * Update bcm_vlan_action_set_t structure from HW for a given profile index
 * and FP Policy memory.
 * Note: Policy entry must contain the policy mem data.
 */
int _bcm_field_td3_vlan_action_profile_entry_get(int unit,
                                                 soc_mem_t policy_mem,
                                                 uint32 *policy_entry,
                                                 uint32 profile_index,
                                                 bcm_vlan_action_set_t *actions)
{
    if ((NULL == policy_entry) || (actions == NULL)) {
       return BCM_E_PARAM;
    }

    _bcm_trx_vlan_action_profile_entry_get(unit, actions, profile_index);

    actions->new_inner_pkt_prio = PolicyGet(unit, policy_mem, policy_entry, NEW_INNER_DOT1Pf);
    actions->new_outer_cfi = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_CFIf);
    actions->new_inner_cfi = PolicyGet(unit, policy_mem, policy_entry, NEW_INNER_CFIf);
    actions->priority = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_DOT1Pf);
    actions->new_inner_vlan = PolicyGet(unit, policy_mem, policy_entry, NEW_INNER_VLANf);
    actions->new_outer_vlan = PolicyGet(unit, policy_mem, policy_entry, NEW_OUTER_VLANf);

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_td3_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_field_td3_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int key_off = 0;
    _FP_QUAL_DECL;

    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
         "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        key_off = -1;
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

   /* EFP_KEY4(L2 Key) Qualifiers */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    if (soc_feature(unit, soc_feature_niv)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                                   _bcmFieldSliceSelEgrDvpKey4, 0,
                                   1, 17);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                                   _bcmFieldSliceSelEgrDvpKey4, 0,
                                   1, 17);
    }
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                              _bcmFieldDevSelDisable, 0,
                              _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                              _bcmFieldSliceSelEgrDvpKey4, 0,
                              0,
                              1, 17,
                              227, 2, /* DVP TYPE */
                              0, 0,
                              0
                              );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstGports,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDestVirtualPortValid,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    if (!SOC_IS_HURRICANE4(unit) && ! SOC_IS_FIREBOLT6(unit)) {
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    }
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141 + key_off, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142 + key_off, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145 + key_off, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157 + key_off, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167 + key_off, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167 + key_off, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 180 + key_off, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 183 + key_off, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185 + key_off, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186 + key_off, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194 + key_off, 12
                               );
    /*
     * bcmFieldQualifyInterfaceClassPort refers to EGR_PORT_GROUP_ID which gets
     * passed in CLASS_ID container for 0, 1 and 2 encoding of
     * EFP_CLASS_ID_A_SELECT for KEY4.
     */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               186 + key_off, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               194 + key_off, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 2,
                               186 + key_off, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     194 + key_off, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 3,
                               0,
                               194 + key_off, 8, /* DST_PORT */
                               186 + key_off, 8, /* DST_MODID */
                               0, 0,
                               0
                               );

    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 1,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 2,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 3,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 4,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 6,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 7,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               207 + key_off, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassIdBF4, 9,
                               207 + key_off, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                                   _bcmFieldSliceSelEgrClassIdBF4, 10,
                                   207 + key_off, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 222 + key_off, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227 + key_off, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227 + key_off, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 227 + key_off,
                 SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 13));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 241 + key_off, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 243 + key_off, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 245 + key_off, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 246 + key_off, 2);
#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (soc_feature(unit, soc_feature_xflow_macsec_svtag)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecControlPkt,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 252, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacSecFlow,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 260, 8);
    }
#endif

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEgressPortCtrlType,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 260, 8);
    }
#endif

    /* EFP_KEY1(IPv4 Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 57, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 65, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 97, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 129, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 139, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 179, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               180, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrDestPortF1, 0,
                               188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 1,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 2,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 1,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 2,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 3,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 4,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 6,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 7,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassIdBF1, 9,
                               214, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                                   _bcmFieldSliceSelEgrClassIdBF1, 10,
                                   214, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 229,
                 SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 13));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 243, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 247, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);
    /* EFP_KEY2 (IPv6 Singlewide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               9, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               73, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 2,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               196, 8
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 5,
                               0,
                               196, 8, /* DST_PORT */
                               188, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 6,
                               187, 8);
    /* EGR_L3_NEXT_HOP.MPLS.CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 6,
                               196, 12);
    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 0,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 1,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 2,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 3,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 4,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 5,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 6,
                               188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 6,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 7,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 8,
                               209, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassIdBF2, 9,
                               209, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                                   _bcmFieldSliceSelEgrClassIdBF2, 10,
                                   209, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 224, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 229,
                 SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 13));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 243, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 247, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    /* EFP_KEY3 (IPv6 Double Wide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 57, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 185, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 201, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);

    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 0,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 1,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 2,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 3,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 4,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 5,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 6,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 7,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 8,
                               219, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassIdBF3, 9,
                               219, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                                   _bcmFieldSliceSelEgrClassIdBF3, 10,
                                   219, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 234, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 236, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);


    /* FCOE EFP_KEY5 */

    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTFabricId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 0, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 12, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTValid,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 20, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 21, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 29, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 37, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 61, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 69, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 93, 24);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 117, 8);
    }
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 125, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 127, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 139, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 147, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 149, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 162, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 167, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               168, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 0,
                               176, 8
                               );
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldDevSelDisable, 0,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrDestPortF5, 1,
                               0,
                               176, 8, /* DST_PORT */
                               168, 8, /* DST_MODID */
                               0, 0,
                               0
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 184, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185,
                 SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 13));
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 199, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressInterfaceClassVPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 201, 13);
    if (soc_feature(unit, soc_feature_fcoe)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 229, 3);
    }
    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 0,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 1,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 2,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 3,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 4,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 5,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 6,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 7,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF4, 8,
                               214, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                               _bcmFieldSliceSelEgrClassIdBF5, 9,
                               214, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5,
                                   _bcmFieldSliceSelEgrClassIdBF5, 10,
                                   214, 12);
    }
#endif

    /*EFP_KEY6 (HiGiG Key)  Qualifiers*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstMulticast,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstMulticastGroupId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigTrafficClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 144, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstModuleGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 136, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstPortGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstModPortGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 128, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcModuleGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 120, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcPortGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 112, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcModPortGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 112, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLoadBalanceID,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 104, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 102, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigExtendedHeaderPresent,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 101, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 99, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMirrorOrSwitchPkt,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 88, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressTagged,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 91, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstTrunk,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 95, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstTrunkId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 92, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressL3SwitchPkt,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 85, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLabelType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 86, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMplsPkt,
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 84, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigLabel,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigReplicationId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 19);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigPortFilteringMode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcTrunk,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 45, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigPreserveFlags,
                 _bcmFieldDevSelDisable, 0,
                _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                _bcmFieldDevSelDisable, 0,
                0,
                44, 1,
                43, 1,
                0, 0,
                0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigIngressClassificationTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 80, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigEgressMcast,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 95, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 90, 5);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVni,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 80, 10,
                 36, 4,
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMulticastIndex,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 64, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpReplicationId,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 64, 16,
                 40, 3,
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 48, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigDoNotFlags,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 46, 1,
                 45, 1,
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpLagFailoverPacket,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 44, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProtectionSwitchingStatus,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 43, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigMirrorToVp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 47, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigVpPreserveFlags,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 35, 1,
                 34, 1,
                 0, 0,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigDstType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigSrcType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 32, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineClassificationTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 80, 16);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineDeferredFlags,
                 _bcmFieldDevSelDisable, 0,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                 _bcmFieldDevSelDisable, 0,
                 0,
                 79, 1,
                 76, 1,
                 72, 1,
                 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineVxltStatus,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 77, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePktPriNew,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 73, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineDscpNew,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 66, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePreserveDscp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 47, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEnginePreserveDot1p,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 46, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigOffloadEngineSrcType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 43, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 40, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 162, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 170, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 178, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 3,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 3,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 3,
                               186, 12);


    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 0,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 1,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 2,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 3,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 4,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 5,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 6,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 7,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 8,
                               199, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassIdBF6, 9,
                               199, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                                   _bcmFieldSliceSelEgrClassIdBF6, 10,
                                   199, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 214, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 216, 1);

    /* EFP_KEY7(Loopback Key) qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackQueue,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 116, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 111, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackSrcGport,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 94, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPktIsVisible,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 93, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackCpuMasqueradePktProfile,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 90, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackTrafficClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopBackPacketProcessingPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 129, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 138, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 146, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 154, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 158, 2);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 1,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 3,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 3,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 3,
                               160, 12);
    

    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 0,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 1,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 2,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 3,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 4,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 5,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 6,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 7,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 8,
                               173, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassIdBF7, 9,
                               173, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                                   _bcmFieldSliceSelEgrClassIdBF7, 10,
                                   173, 12);
    }
#endif

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 188, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 190, 1);

    /* EFP_KEY8(Bytes After L2 Key) Qualifiers */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL2PayLoad,
                    _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                    _bcmFieldSliceSelDisable, 0, 59, 144);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyDestVirtualPortValid,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 0,
                               0, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 1,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 3,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 3,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey8, 3,
                               1, 12);

    /* CLASS_ID_B */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 0,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 1,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 2,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 3,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3InterfaceTunnel,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 4,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassVxlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 5,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 6,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 7,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 8,
                               216, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressForwardingClassId,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassIdBF8, 9,
                               216, 15);
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassPort,
                                   _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                                   _bcmFieldSliceSelEgrClassIdBF8, 10,
                                   216, 12);
    }
#endif
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 17, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 23, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 40, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 41, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 49, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 50, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 1,
                               203, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                     _bcmFieldSliceSelEgrClassF8, 2,
                     0,
                     203, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 231,
                 SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 14));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 245, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 247, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 249, 1);

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_td3_egress_secondary_selcode_set
 *
 * Purpose:
 *     Sets the secondary select codes based on field values in different
 *     registers EFP_CLASSID_SELECTORr, EFP_KEY4_DVP_SELECTORr
 *
 * Parameters:
 *     unit             - (IN) BCM unit number.
 *     fg               - (IN/OUT) Select code information filled into the group
 *     slice_idx        - (IN) Slice id
 *     part_idx         - (IN) Part id of the given slice in the given group
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_egress_secondary_selcodes_set(int unit, _field_group_t *fg,
                                             int slice_idx, uint8 part_idx)
{
    soc_field_t field = INVALIDf;   /* H/W Register field.            */
    uint64 reg_val64;    /* 64 bit H/W Register value.     */
    uint32 reg_val;
    uint32 fldval;       /* Field in a H/W register value. */
    uint8  num_fields;   /* Number of fields.              */
    soc_reg_t reg = INVALIDr;       /* H/W register.                  */
    int rv = BCM_E_NONE; /* Operation status.              */
    int idx;             /* Index.                         */
    int inst;            /* Instance.                      */
    _field_stage_t     *stage_fc = NULL;
    soc_reg_t       efp_classid_sel_reg;
    soc_reg_t       efp_classid_b_sel_reg;
    soc_reg_t       efp_key4_dvp_reg;
    soc_reg_t       efp_key8_dvp_reg;
    soc_reg_t       efp_dst_port_reg;

    static const soc_field_t efp_dest_port_selector_flds[][2] = {
             {SLICE_0_F1_DEST_SELf, SLICE_0_F5_DEST_SELf},
             {SLICE_1_F1_DEST_SELf, SLICE_1_F5_DEST_SELf},
             {SLICE_2_F1_DEST_SELf, SLICE_2_F5_DEST_SELf},
             {SLICE_3_F1_DEST_SELf, SLICE_3_F5_DEST_SELf},
        };


    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_EGRESS, &stage_fc));
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
       inst = _FP_GLOBAL_INST;
    } else {
       inst = fg->instance;
    }

    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                       EFP_CLASSID_SELECTOR_Ar,
                                                       inst,
                                                       &efp_classid_sel_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                  EFP_CLASSID_B_SELECTOR_KEY_TYPEr,
                                  inst, &efp_classid_b_sel_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                   EFP_KEY4_DVP_SELECTORr,
                                                   inst, &efp_key4_dvp_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                   EFP_KEY8_DVP_SELECTORr,
                                                   inst, &efp_key8_dvp_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                       EFP_DEST_PORT_SELECTORr,
                                                       inst,
                                                       &efp_dst_port_reg));

    fg->sel_codes[part_idx].egr_class_f1_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f2_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f3_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f4_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f6_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f7_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_class_f8_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_key4_dvp_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_key8_dvp_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_key4_mdl_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f1_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f2_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f3_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f4_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f5_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f6_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f7_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_classId_B_f8_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_dest_port_f1_sel = _FP_SELCODE_DONT_CARE;
    fg->sel_codes[part_idx].egr_dest_port_f5_sel = _FP_SELCODE_DONT_CARE;


    reg = efp_classid_sel_reg;
    rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &reg_val64);
    num_fields = sizeof(classfldtbl[0]) /
                   sizeof(classfldtbl[0][0]);

    if (BCM_SUCCESS(rv)) {
        for (idx = 0 ; idx < num_fields; idx++) {
            field = classfldtbl[slice_idx][idx];
            if (!soc_reg_field_valid(unit, reg, field)) {
                continue;
            }

            fldval =  soc_reg64_field32_get(unit, reg, reg_val64, field);

            switch(idx) {
                case 0:
                     fg->sel_codes[part_idx].egr_class_f1_sel = fldval;
                     break;
                case 1:
                     fg->sel_codes[part_idx].egr_class_f2_sel = fldval;
                     break;
                case 2:
                     fg->sel_codes[part_idx].egr_class_f3_sel = fldval;
                     break;
                case 3:
                     fg->sel_codes[part_idx].egr_class_f4_sel = fldval;
                     break;
                case 4:
                     fg->sel_codes[part_idx].egr_class_f6_sel = fldval;
                     break;
                case 5:
                     fg->sel_codes[part_idx].egr_class_f7_sel = fldval;
                     break;
                case 6:
                     fg->sel_codes[part_idx].egr_class_f8_sel = fldval;
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,
                               (BSL_META_U(unit,
                                           "FP(unit %d) Error:Invalid field in efp classid_A selector \n"),
                                unit));
                     return BCM_E_INTERNAL;
            }
        }
    }

    reg = efp_classid_b_sel_reg;

    for (idx = 0 ; idx < 8; idx++) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &reg_val));
        if (!soc_reg_field_valid(unit, reg, classIdBfldtbl[slice_idx])) {
            continue;
        }

        fldval = soc_reg_field_get(unit, reg, reg_val, classIdBfldtbl[slice_idx]);

        switch(idx) {
            case 0:
                 fg->sel_codes[part_idx].egr_classId_B_f1_sel = fldval;
                 break;
            case 1:
                 fg->sel_codes[part_idx].egr_classId_B_f2_sel = fldval;
                 break;
            case 2:
                 fg->sel_codes[part_idx].egr_classId_B_f3_sel = fldval;
                 break;
            case 3:
                 fg->sel_codes[part_idx].egr_classId_B_f4_sel = fldval;
                 break;
            case 4:
                 fg->sel_codes[part_idx].egr_classId_B_f5_sel = fldval;
                 break;
            case 5:
                 fg->sel_codes[part_idx].egr_classId_B_f6_sel = fldval;
                 break;
            case 6:
                 fg->sel_codes[part_idx].egr_classId_B_f7_sel = fldval;
                 break;
            case 7:
                 fg->sel_codes[part_idx].egr_classId_B_f8_sel = fldval;
                 break;
                 /* coverity[dead_error_begin] */
            default:
                 LOG_ERROR(BSL_LS_BCM_FP,
                           (BSL_META_U(unit,
                                       "FP(unit %d) Error:Invalid field in efp classid_B selector \n"),
                            unit));
                 return BCM_E_INTERNAL;
        }
    }

    reg = efp_key4_dvp_reg;
    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);

    if (BCM_SUCCESS(rv)) {
        field = dvpfldtbl[slice_idx];
        if (soc_reg_field_valid(unit, reg, field)) {
            fldval = soc_reg_field_get(unit, reg, reg_val, field);
            fg->sel_codes[part_idx].egr_key4_dvp_sel = fldval;
        }
    }

    reg = efp_key8_dvp_reg;
    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);

    if (BCM_SUCCESS(rv)) {

        field = dvpfldtbl[slice_idx];
        if (soc_reg_field_valid(unit, reg, field)) {

            fldval = soc_reg_field_get(unit, reg, reg_val, field);
            fg->sel_codes[part_idx].egr_key8_dvp_sel = fldval;
        }
    }

    if (SOC_REG_IS_VALID(unit, EFP_DEST_PORT_SELECTORr)) {

        reg = efp_dst_port_reg;
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);

        if (BCM_SUCCESS(rv)) {
            num_fields = sizeof(efp_dest_port_selector_flds[0]) /
                             sizeof(efp_dest_port_selector_flds[0][0]);
            for (idx = 0 ; idx < num_fields; idx++) {
                field = efp_dest_port_selector_flds[slice_idx][idx];
                if (!soc_reg_field_valid(unit, reg, field)) {
                    continue;
                }
                fldval = soc_reg_field_get(unit, reg, reg_val, field);

                switch(idx) {
                    case 0:
                        fg->sel_codes[part_idx].egr_dest_port_f1_sel = fldval;
                        break;
                    case 1:
                        fg->sel_codes[part_idx].egr_dest_port_f5_sel = fldval;
                        break;
                    default:
                        LOG_ERROR(BSL_LS_BCM_FP,
                                (BSL_META_U(unit,
                                 "FP(unit %d) Error"
                                 ":Invalid field in efp dest port selector \n"),
                                 unit));
                        return BCM_E_INTERNAL;
                }
            }
        }

    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_td3_egress_selcodes_install
 *
 * Purpose:
 *     Writes the field egress secondary select codes.
 *
 * Parameters:
 *     unit          - (IN) BCM device number
 *     fg            - (IN) Installed group structure.
 *     slice_numb    - (IN) Slice number to set mode for.
 *     selcode_index - (IN) Index to group slecodes.
 *
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */

int
_field_td3_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   int            selcode_idx
                                   )
{
    int             inst =-1;
    uint8           idx; /* Index to EFP keys. */
    uint8           idx_b; /* Index to EFP CLASSID B keys. */
    uint32          reg_val;
    _field_sel_t    *sel;
    _field_stage_t  *stage_fc = NULL;
    soc_reg_t       efp_classid_sel_r = INVALIDr;
    soc_reg_t       efp_classid_b_sel_reg = INVALIDr;
    soc_reg_t       efp_key4_dvp_reg = INVALIDr;
    soc_reg_t       efp_key8_dvp_reg = INVALIDr;
    soc_reg_t       efp_dst_port_reg = INVALIDr;

    static const soc_field_t efp_dest_port_selector_flds[][2] = {
             {SLICE_0_F1_DEST_SELf, SLICE_0_F5_DEST_SELf},
             {SLICE_1_F1_DEST_SELf, SLICE_1_F5_DEST_SELf},
             {SLICE_2_F1_DEST_SELf, SLICE_2_F5_DEST_SELf},
             {SLICE_3_F1_DEST_SELf, SLICE_3_F5_DEST_SELf},
        };

    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    /* Max number of slices on Tomahawk for EFP is 4(slice 0 - slice 3) */
    if (slice_num >= 4)  {
        return BCM_E_INTERNAL;
    }

    sel = &fg->sel_codes[selcode_idx];
    if (sel == NULL) {
       return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                                                 &stage_fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
       inst = _FP_GLOBAL_INST;
    } else {
       inst = fg->instance;
    }

    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                       EFP_CLASSID_SELECTOR_Ar,
                                                       inst,
                                                       &efp_classid_sel_r));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                  EFP_CLASSID_B_SELECTOR_KEY_TYPEr,
                                  inst, &efp_classid_b_sel_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                   EFP_KEY4_DVP_SELECTORr,
                                                   inst, &efp_key4_dvp_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                   EFP_KEY8_DVP_SELECTORr,
                                                   inst, &efp_key8_dvp_reg));
    BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                    EFP_DEST_PORT_SELECTORr,
                                                    inst,
                                                    &efp_dst_port_reg));

    idx = 0;
    /* Class Id Selector codes for different EFP keys (KEY1 - KEY8) */
    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f1_sel
                                                   ));
    }
    idx_b = 0;
    if (sel->egr_classId_B_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f1_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }
    idx++;
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f2_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f2_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }
    idx++;
    if (sel->egr_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f3_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f3_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f4_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f4_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    idx_b++;
    if (sel->egr_classId_B_f5_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f5_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f6_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f6_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f6_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f6_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f7_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f7_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f7_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f7_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    idx++;
    if (sel->egr_class_f8_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_classid_sel_r,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f8_sel
                                                   ));
    }
    idx_b++;
    if (sel->egr_classId_B_f8_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      &reg_val
                                      ));
        soc_reg_field_set(unit,
                          efp_classid_b_sel_reg,
                          &reg_val,
                          classIdBfldtbl[slice_num],
                          sel->egr_classId_B_f8_sel);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                                      efp_classid_b_sel_reg,
                                      REG_PORT_ANY,
                                      idx_b,
                                      reg_val));
    }

    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_key4_dvp_reg,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key4_dvp_sel
                                                  ));
    }
    /* SELECTOR CODES for EFP_KEY8_DVP_SELECTOR register */
    if (sel->egr_key8_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   efp_key8_dvp_reg,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key8_dvp_sel
                                                  ));
    }

    /* SELECTOR CODES for EFP_DEST_PORT_SELECTORr */
    if (SOC_REG_IS_VALID(unit, EFP_DEST_PORT_SELECTORr)) {
        if (sel->egr_dest_port_f1_sel != _FP_SELCODE_DONT_CARE) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        efp_dst_port_reg,
                        REG_PORT_ANY,
                        efp_dest_port_selector_flds[slice_num][0],
                        sel->egr_dest_port_f1_sel
                        )
                    );
        }
        if (sel->egr_dest_port_f5_sel != _FP_SELCODE_DONT_CARE) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        efp_dst_port_reg,
                        REG_PORT_ANY,
                        efp_dest_port_selector_flds[slice_num][1],
                        sel->egr_dest_port_f5_sel
                        )
                    );
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_td3_em_key_attributes_init
 * Purpose:
 *  Initialize Exact Match Key Attribute memory.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  stage_fc - (IN) Field Processor stage control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td3_em_key_attributes_init(int unit, _field_stage_t *stage_fc)
{
    exact_match_key_attributes_entry_t em_entry;
                                      /* key attributes Entry buffer.  */
    uint32 *em_key_attr_entry = NULL; /* Hardware Buffer Ptr.      */
    int idx;
    soc_mem_t mem;                    /* Memory Identifier.        */
    int mem_inst = 0;
    static soc_mem_t em_key_attributes_mem;

    mem = EXACT_MATCH_KEY_ATTRIBUTESm;

    /* assign exact match key attribute entry buffer. */
    em_key_attr_entry = (uint32 *)&em_entry;

    for (idx = 0; idx < stage_fc->num_instances; idx++) {

        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            mem_inst = _FP_GLOBAL_INST;
        } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
            mem_inst = idx;
        } else {
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                    EXACT_MATCH_KEY_ATTRIBUTESm,
                    mem_inst, &em_key_attributes_mem));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        /* Mode 128: entry 0 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                    MEM_BLOCK_ANY, 0, em_key_attr_entry));

        /* Mode 128: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 1);
        /* Mode 128: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 1);
        /* Mode 128: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 6);
        /* Mode 128: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 0);

        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 0, em_key_attr_entry));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        /* Mode 160: entry 1 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                    MEM_BLOCK_ANY, 1, em_key_attr_entry));

        /* Mode 160: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 1);
        /* Mode 160: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 1);
        /* Mode 160: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 14);
        /* Mode 160: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 0);
        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 1, em_key_attr_entry));

        /* Initialize buffer to 0 */
        sal_memcpy(em_key_attr_entry, soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        /* Mode 320: entry 2 */
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, em_key_attributes_mem,
                    MEM_BLOCK_ANY, 2, em_key_attr_entry));

        /* Mode 320: Set BUCKET_MODE. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, BUCKET_MODEf, 2);
        /* Mode 320: Set KEY_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_BASE_WIDTHf, 3);
        /* Mode 320: Set KEY_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, KEY_WIDTHf, 1);
        /* Mode 320: Set DATA_BASE_WIDTH. */
        soc_mem_field32_set(unit, em_key_attributes_mem,
                em_key_attr_entry, DATA_BASE_WIDTHf, 0);

        /* Install entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, em_key_attributes_mem,
                    MEM_BLOCK_ALL, 2, em_key_attr_entry));
    }
    return BCM_E_NONE;
}


/*    _field_td3_stage_quals_ibus_map_init
 * Purpose:
 *    Initialize Field Ingress Stage Qualifiers to their extractors mapping.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field stage control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_INTERNAL - Invalid CAP Stage ID.
 *    BCM_E_NONE - Success.
 */
int
_field_td3_stage_quals_ibus_map_init(int unit, _field_stage_t *stage_fc)
{
    uint32 cancun_ver = 0;
    bcmi_keygen_qual_flags_bmp_t qual_flags;
    bcmi_keygen_qual_cfg_info_db_t *db = NULL;
    BCMI_KEYGEN_QUAL_CFG_DECL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    /*
     * L0 BUS extractor sections offset:
     *    - 32bit extractor = 0
     *    - 16bit extractor = 608
     *    - 8bit extractor  = 1120
     *    - 4bit extractor  = 1280
     *    - 2bit extractor  = 1400
     */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocate stage qualifiers extraction mapping array. */
    _FP_XGS3_ALLOC(stage_fc->qual_cfg_info_db,
         sizeof(bcmi_keygen_qual_cfg_info_db_t),
         "IFP qualifiers");
    if (NULL == stage_fc->qual_cfg_info_db) {
        return (BCM_E_MEMORY);
    }

    db = stage_fc->qual_cfg_info_db;
    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    /* Pseudo qualifiers */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStage, qual_flags);

    if (soc_feature(unit, soc_feature_field_exact_match_support)) {
        SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_IFP);
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStageIngressExactMatch, qual_flags);
        SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_IFP);
    }

    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStageIngress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchHitStatus, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchActionClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExactMatchGroupClassId, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyNormalizeIpAddrs, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyNormalizeMacAddrs, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp4, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6, qual_flags);

    /* UDF Qualifiers. */
    /*************** 32 BIT Section *************/
    /* UDF_1_CHUNK_1_0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 8, 256, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData0, qual_flags);

    /* UDF_1_CHUNK_1_0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 8, 272, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData1, qual_flags);

    /* UDF_1_CHUNK_3_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 9, 288, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData2, qual_flags);

    /* UDF_1_CHUNK_3_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 9, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData3, qual_flags);

    /* UDF_1_CHUNK_5_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData4, qual_flags);

    /* UDF_1_CHUNK_5_4 */
    /* Identifier field in IP Header is mapped to CHUNK 4 */
    BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
    if (SOC_CANCUN_VERSION_DEF_5_2_2 > cancun_ver) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 320, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpIdentifier, qual_flags);
    }

    /* UDF_1_CHUNK_5_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 10, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData5, qual_flags);

    /* UDF_2_CHUNK_1_0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 5, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData8, qual_flags);

    /* UDF_2_CHUNK_1_0 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 5, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData9, qual_flags);

    /* UDF_2_CHUNK_3_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 6, 192, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData10, qual_flags);

    /* UDF_2_CHUNK_3_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 6, 208, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData11, qual_flags);

    /* UDF_2_CHUNK_3_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 6, 192, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyINTReserved4Bytes, qual_flags);
    /* UDF_2_CHUNK_5_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 7, 224, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData12, qual_flags);

    /* UDF_2_CHUNK_5_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 7, 240, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData13, qual_flags);

    /* UDF_2_CHUNK_5_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 7, 224, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyINTProbeMarker2, qual_flags);

    /*************** 16 BIT Section *************/
    /* UDF_1_CHUNK_6 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 8, 128, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData6, qual_flags);

    /* UDF_1_CHUNK_7 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData7, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 9, 144, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyLoopbackSubtype, qual_flags);

    /* UDF_2_CHUNK_6 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 6, 96, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData14, qual_flags);

    /* UDF_2_CHUNK_7 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 7, 112, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyData15, qual_flags);

    /* UDF_2_CHUNK_6 and UDF_2_CHUNK_7 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 6, 96, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 7, 112, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyINTProbeMarker1, qual_flags);

    /* OPAQUE_4 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 45, 720, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueObject4, qual_flags);

    /* OPAQUE_3 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 44, 704, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueObject3, qual_flags);

    /* OPAQUE_2 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 43, 688, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueObject2, qual_flags);

    /* OPAQUE_1 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 42, 672, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOpaqueObject1, qual_flags);

    /* Packet Fields */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 400, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlan, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 400, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 4, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 5, 20, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 412, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanCfi, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 25, 413, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterVlanPri, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 416, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlan, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 416, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 3, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 4, 16, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 428, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanCfi, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 26, 429, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerVlanPri, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 2, 64, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 30, 480, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMac, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 3, 96, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 31, 496, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstMac, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 14, 464, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyArpSenderIp4, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 384, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyArpTargetIp4, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 384, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32, 13, 416, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32, 14, 448, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 384, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6Low, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 13, 416, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 14, 448, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6High, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 16, 512, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32, 17, 544, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32, 18, 576, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 16, 512, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6Low, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 17, 544, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 18, 576, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6High, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 20, 640, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 21, 672, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySubportGroupIds, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 29, 464, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyEtherType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 28, 448, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 28, 448, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
            bcmFieldQualifyIcmpTypeCode, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 27, 432, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyISid, qual_flags);

    /* L1E4, 15 for Lookup status vector 5 */
    /* L1E4, 16 for FLOW_1_HDR_VALID_CHK_RES_FOR_KEYGEN */
    /* L1E4, 17 for FLOW_2_HDR_VALID_CHK_RES_FOR_KEYGEN */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 24);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2GreVsid, qual_flags);

    /* INGRESS_FIELD_BUS_CONTAINER_A 32-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 11);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 11);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);
    /* INGRESS_FIELD_BUS_CONTAINER_B 32-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 12);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 12);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);
    /* INGRESS_FIELD_BUS_CONTAINER_C 32-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 13);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 13);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);
    /* INGRESS_FIELD_BUS_CONTAINER_D 32-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 14);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 14);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    /* INGRESS_FIELD_BUS_CONTAINER_A 16-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 11);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 11);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    /* INGRESS_FIELD_BUS_CONTAINER_B 16-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 12);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 12);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    /* INGRESS_FIELD_BUS_CONTAINER_C 16-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 13);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 13);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    /* INGRESS_FIELD_BUS_CONTAINER_D 16-bit Mux */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 14);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 14);
    BCMI_KEYGEN_QUAL_CFG_SEC_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_ING_FBUS_CONT, _bcmFieldIngFieldBusContOuterVlan);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIncomingOuterVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 24);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 24);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 4, 128, 24);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanNetworkId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanFlags, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanFlags, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanFlags, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanFlags, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 24);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits8_31, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 24);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits8_31, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits8_31, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits8_31, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 24, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits56_63, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits56_63, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits56_63, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVxlanHeaderBits56_63, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyETag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyETag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyETag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyETag, qual_flags);

    if (soc_feature(unit, soc_feature_niv)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 0);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 0);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 0);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 0);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVnTag, qual_flags);
    }

    if (soc_feature(unit, soc_feature_CnTag)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCnTag, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCnTag, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFabricQueueTag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFabricQueueTag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFabricQueueTag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFabricQueueTag, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 12, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 44, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 44, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 76, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 9, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 41, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 41, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 73, 3);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelExp, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 8, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 40, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 40, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 72, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelBos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlWord, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 20);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIp6FlowLabel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtl, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 11, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTos, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 13, 104, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocol, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 2, 8, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 3, 12, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpControl, qual_flags);

    if (soc_feature(unit, soc_feature_fcoe)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 27, 432, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFcoeRxID, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 28, 448, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFcoeOxID, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 16, 256, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyExactMatchGroupClassIdLookup0, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 16, 264, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyExactMatchGroupClassIdLookup1, qual_flags);

    /* For stage ingress, the extractors are fixed for ltid and
     * ltid init for ingress is done along with pmux init
     */

    if (soc_feature(unit, soc_feature_fp_ltid_match_all_parts) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 5, 40, 5);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_LTID, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyPreLogicalTableId1, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 5, 40, 5);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_LTID, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyPreLogicalTableId2, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 15, 120, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExtensionHeaderType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 14, 112, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyExtensionHeader2Type, qual_flags);

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5X(unit)) {
        /* RANGE_CHECK_MASK_RESULT */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 26, 208, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRangeCheckGroup, qual_flags);
    }
#endif

    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 31, 124, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMHOpcode, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 15, 30, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFlags, qual_flags);

    /*Packet Types*/
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 17, 35, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGig, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 5, 11, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigProxy, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 5, 10, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCpuVisibilityPacket, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 13, 54, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDosAttack, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 1, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4Ports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 17, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4Ports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 6, 13, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4Ports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 17, 34, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMirrorCopy, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 16, 32, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFrag, qual_flags);

    if (soc_feature(unit, soc_feature_nat) ||
        soc_feature(unit, soc_feature_large_scale_nat)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 0, 0, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyNatSrcRealmId, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 6, 12, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFragNonOrFirst, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFragNonOrFirst, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpFragNonOrFirst, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 4, 8, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIcmpError, qual_flags);

#if 0
    /*removed from td3 reg file */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 4, 9, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRepCopy, qual_flags);
#endif

    /*Table HIT/Lookup Status bits*/
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 11, 46, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2SrcHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 12, 48, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2DestHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 12, 50, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3SrcHostHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 12, 50, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpmcStarGroupHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 12, 51, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3DestHostHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 13, 52, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3SrcRouteHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 13, 53, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2StationMove, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 12, 49, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2CacheHit, qual_flags);

    /* not supported for EM */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 11, 47, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2SrcStatic, qual_flags);

    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 13, 55, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpTunnelHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 10, 40, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslationHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 56, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsLabel1Hit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 56, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTrillEgressRbridgeHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 56, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2GreSrcIpHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 56, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMimSrcGportHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 57, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsLabel2Hit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 57, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTrillIngressRbridgeHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 57, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2GreVfiHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 57, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMimVfiHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 7, 14, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIntCongestionNotification, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 14, 29, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMyStationHit, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 58, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsTerminated, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 13, 27, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIpLocal, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 14, 28, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3Routable, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7AHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRtag7BHashLower, qual_flags);

    /*Class Ids from different Tables*/

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassVPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 216, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 200, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassField, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 248, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 232, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceLookupClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceIngressKeySelectClassPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyUdfClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 216, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 200, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 10);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 216, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 200, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstClassL3, qual_flags);

    /* Control selectors 7-13 are not supported in EM */
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 7);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanTranslateClassId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup2, qual_flags);

#if 0
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, );
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup3, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassMplsLookup3, qual_flags);
#endif

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInterfaceClassL3Tunnel, qual_flags);

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        /* BROADSCAN_GROUP_CLASS_ID */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 15);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerClassId, qual_flags);
    }

    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

#ifdef BCM_FIREBOLT6_SUPPORT
    if (soc_feature(unit, soc_feature_field_aacl_compression)) {
        /* L3_SRC_CLASS_ID */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcCompressionClassId, qual_flags);

        /* L3_SRC_CLASS_ID Ipv4 */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv4CompressedClassId, qual_flags);

        /* L3_SRC_CLASS_ID Ipv6 */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifySrcIpv6CompressedClassId, qual_flags);

        /* L3_DST_CLASS_ID */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstCompressionClassId, qual_flags);

        /* L3_DST_CLASS_ID Ipv4 */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv4CompressedClassId, qual_flags);

        /* L3_DST_CLASS_ID Ipv6 */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 15, 240, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 14, 224, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 13, 208, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 12, 192, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 9, 72, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 8, 64, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 7, 56, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 6, 48, 8);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 9, 36, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 8, 32, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 7, 28, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 6, 24, 4);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D, 17);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyDstIpv6CompressedClassId, qual_flags);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 5, 46, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyGroupClass, qual_flags);

    /* Ingress/Egress Objects */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 10, 43, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 11, 44, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIngressStpState, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 11, 44, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyStpState, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingVlanId, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 8, 16, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVirtualPortValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 8, 17, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDestVirtualPortValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunkMemberGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 168, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModuleGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModPortGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunkMemberGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 182, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcModuleGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMimGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcMimGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcWlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcWlanGports, qual_flags);

    if (soc_feature(unit, soc_feature_niv)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcNivGports, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcNivGports, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 10, 160, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 5, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 14);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 2, 4, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 2, 16, 8);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 32, 128, 4);
    if (SOC_IS_FIREBOLT6(unit)) {
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E2, 28, 56, 1);
    }
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVrf, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 1, 8, 8);
#ifdef BCM_HURRICANE4_SUPPORT
    /* HR4 has 1k depth VFI */
    if (SOC_IS_HURRICANE4(unit)) {
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 0, 0, 2);
    } else
#endif
    {
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 0, 0, 4);
        /* HX5 has 4k depth VFI */
        if (!SOC_IS_HELIX5(unit)) {
            BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E2, 1, 2, 2);
        }
    }
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVpn, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    /* HX5 has only 4k depth VFI */
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 17, 272, SOC_IS_HELIX5(unit) ? 12 : (SOC_IS_HURRICANE4(unit) ? 10 : 14));
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVpn, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 18, 288, 13);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3Ingress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 0, 0, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInPort, qual_flags);

    /*Miscellaneous*/
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 30, 120, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIntPriority, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 23, 46, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyColor, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 24, 49, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
            bcmFieldQualifyRouterAlertLabelValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 24, 48, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
            bcmFieldQualifyGenericAssociatedChannelLabelValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 25, 50, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
            bcmFieldQualifyMplsControlWordValid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 12, 24, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMacAddrsNormalized, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 12, 25, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpAddrsNormalized, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 19, 38, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyVlanFormat, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 20, 40, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTranslatedVlanFormat, qual_flags);

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 30, 60, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowControlType, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 26, 53, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMacSecDstMacRangeHit, qual_flags);
    }
#endif

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 384, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E32, 13, 416, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(3, BCMI_KEYGEN_EXT_SECTION_L1E32, 14, 448, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(4, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(5, BCMI_KEYGEN_EXT_SECTION_L1E32, 16, 512, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(6, BCMI_KEYGEN_EXT_SECTION_L1E32, 17, 544, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(7, BCMI_KEYGEN_EXT_SECTION_L1E32, 18, 576, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2PayLoad, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 17, 544, 32);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E32, 18, 576, 32);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
            bcmFieldQualifyL2PayloadFirstEightBytes, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 26, 52, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpInfo, qual_flags);

    if (soc_feature(unit, soc_feature_bfd)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 0, 0, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 10);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 1, 32, 32);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 10);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 2, 32, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 3, 48, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B, 10);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 4, 64, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 5, 80, 16);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A, 10);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdYourDiscriminator, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketLength, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 16);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketLength, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 23, 368, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 24, 384, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRangeCheck, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 11, 176, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 3, 6, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcTrunkMemberGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 29, 116, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsForwardingLabelAction, qual_flags);


    
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 10, 80, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPacketRes, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 1, 4, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 3, 6, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 18, 36, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL2Format, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 22, 44, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyInnerTpid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 21, 42, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyOuterTpid, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 18, 72, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingType, qual_flags);

    /* Compression Qualifiers */

    if (soc_feature(unit, soc_feature_fcoe)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 20, 80, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFcoeOxIDClass, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 19, 76, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFcoeRxIDClass, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 20, 80, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 6, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 22, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4SrcPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 19, 76, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 2, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 18, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL4DstPortClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 22, 88, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocolClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 10, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocolClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 26, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpProtocolClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 21, 84, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyEtherTypeClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtlClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTtlClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 11, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTosClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 11, 88, 8);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTosClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 2, 8, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 3, 12, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 0);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpClassZero, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 2, 8, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E4, 3, 12, 4);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTcpClassOne, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIpClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIpClassLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIpClassUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIpClassMsbNibble, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6Class, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6ClassLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6ClassUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifySrcIp6ClassMsbNibble, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIpClass, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIpClassLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIpClassUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIpClassMsbNibble, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6Class, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6ClassLower, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6ClassUpper, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstIp6ClassMsbNibble, qual_flags);

    if (soc_feature(unit, soc_feature_fcoe)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanSrcIdClass, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 21, 336, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanSrcIdClassLower, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 22, 352, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanSrcIdClassUpper, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 23, 92, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanSrcIdClassMsbNibble, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDstIdClass, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 19, 304, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDstIdClassLower, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 20, 320, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDstIdClassUpper, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 24, 96, 4);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDstIdClassMsbNibble, qual_flags);

        /*Fiber Channel Qualifiers */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 360, 24);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanSrcId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 355, 3);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanInner, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 11, 352, 3);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanOuter, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 392, 24);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanFCtl, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 12, 384, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFcoeSOF, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 488, 24);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDstId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E32, 15, 480, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanRCtl, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 11, 88, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanCSCtl, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 12, 96, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTHopCount, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 12, 3);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 7);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTPri, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 0, 0, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D, 7);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTFabricId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 28, 3);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 7);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTPri, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 1, 16, 12);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C, 7);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTFabricId, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 13, 104, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanType, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 15, 120, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanDFCtl, qual_flags);

    }
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
        BCMI_KEYGEN_EXT_SECTION_LTID, 0, 144, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1,
        BCMI_KEYGEN_EXT_SECTION_LTID, 1, 150, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
        _bcmFieldQualifyPreLogicalTableId, qual_flags);

    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    /* Post Mux Qualifers */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_DROP, 0, 159, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDrop, qual_flags);

    if (soc_feature(unit, soc_feature_nat) ||
        soc_feature(unit, soc_feature_large_scale_nat)) {
        /* NAT Qualifiers */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_NAT_NEEDED, 1, 158, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyNatNeeded, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_NAT_DST_REALMID, 2, 156, 2);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyNatDstRealmId, qual_flags);
    }

    /* Exact Match Class Id  and HIT status Qualifiers. */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
        BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_0_3, 8, 120, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1,
        BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_4_7, 9, 124, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2,
        BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_CLASSID_BITS_8_11, 10, 128, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
                  _bcmFieldQualifyExactMatchActionClassIdLookup0, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
        BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_0_3, 5, 132, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1,
        BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_4_7, 6, 136, 4);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(2,
        BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_CLASSID_BITS_8_11, 7, 140, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
                   _bcmFieldQualifyExactMatchActionClassIdLookup1, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
        BCMI_KEYGEN_EXT_SECTION_EM_FIRST_LOOKUP_HIT, 3, 154, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
        _bcmFieldQualifyExactMatchHitStatusLookup0, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0,
        BCMI_KEYGEN_EXT_SECTION_EM_SECOND_LOOKUP_HIT, 3, 155, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db,
        _bcmFieldQualifyExactMatchHitStatusLookup1, qual_flags);

    /* Destination Port Object(PORT/LAG/VP/VPLAG) -
     * Any other Mode than SINGLE.
     */
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_PMUX);
    SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGports, qual_flags);

    if (soc_feature(unit, soc_feature_niv)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGports, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGports, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHopsUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHopsUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathOverlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathOverlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 14, 48, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 13, 64, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 34);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 3);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDevicePortBitmap, qual_flags);

    if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 34);
       BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 1);
       BCMI_KEYGEN_QUAL_CFG_INSERT
           (unit, db, bcmFieldQualifySystemPortBitmap, qual_flags);
    } else {
       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 34);
       BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 1);
       BCMI_KEYGEN_QUAL_CFG_INSERT
           (unit, db, bcmFieldQualifyInPorts, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_IPBM, 0, 0, 34);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT, 2);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifySourceGportBitmap, qual_flags);

    /* Destination Port Object(PORT/LAG/VP/VPLAG) - SINGLE Wide Mode */
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_SINGLE);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstPort, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyDstTrunk, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMimGports, qual_flags);

    if (soc_feature(unit, soc_feature_niv)) {
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGport, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGports, qual_flags);

        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
            (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
        BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
        BCMI_KEYGEN_QUAL_CFG_INSERT
            (unit, db, bcmFieldQualifyDstNivGports, qual_flags);
    }

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVxlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstVlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstWlanGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGport, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 5);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMplsGports, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHops, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHopsUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 10);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3EgressNextHopsUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 9);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3Egress, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipath, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathOverlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 7);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathOverlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstMultipathUnderlay, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 11);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL3MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_0, 12, 88, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 152, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (0, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINER_1, 11, 104, 16);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD
        (1, BCMI_KEYGEN_EXT_SECTION_SRC_DST_CONTAINERS_MSB, 4, 153, 1);
    BCMI_KEYGEN_QUAL_CFG_PRI_CTRL_SEL_ADD(BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1, 12);
    BCMI_KEYGEN_QUAL_CFG_INSERT
        (unit, db, bcmFieldQualifyDstL2MulticastGroup, qual_flags);
    SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);

    /* Non Post MUX qualifiers initialization starts from here */
    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        /* WIRED_WIRELESS */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 27, 54, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyCosMapSelect, qual_flags);
    }
#endif
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {

        /* BROADSCAN_LEARN */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 27, 54 + 1, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerLearn, qual_flags);

        /* BROADSCAN_GROUP_ID */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 52, 832, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerGroupId, qual_flags);

        /* BROADSCAN_EVENT_VECTOR */
        /* GROUP_VALID */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerGroupValid, qual_flags);

        /* DO_NOT_FT */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 1, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerDisabled, qual_flags);

        /* ALU16_EVENTS */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 2, 8);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerCheck, qual_flags);

        /* INITIAL_OR_SAMPLE_COPY_TO_COLLECTOR */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 10, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerCollectorCopy, qual_flags);

        /* FLOW_METER_EXCEED */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 11, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerMeteringExceeded, qual_flags);

        /* FT_TABLE_FULL */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 12, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerFlowTableFull, qual_flags);

        /* GROUP_FLOW_EXCEED */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 13, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerGroupFlowExceeded, qual_flags);

        /* EXPORT_FIFO_FULL */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 51, 816 + 14, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFlowtrackerExportQueueFull, qual_flags);

        /* BROADSCAN_INDEX_VALID */
    }

    if (soc_feature(unit, soc_feature_fcoe)) {
        /* FCOE_VSAN_ID (40) */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 40, 640, 16);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFibreChanVFTVsanId, qual_flags);
    }

    /* PKT_FLOW_ID_0 (39) - 624 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 39, 628, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyFrontPanelPkt, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 39, 628, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyLoopbackType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 39, 628, 8);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyHiGigProxyTunnelType, qual_flags);

    /* PKT_FLOW_ID_1 (38) - 608 */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 38, 608, 6);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelSubType, qual_flags);

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 38, 614, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyForwardingPacketRes, qual_flags);
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 38, 618, 4);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyTunnelType, qual_flags);

    /* LOOKUP_STATUS_VECTOR_4 - 19th Bit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 14, 59, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyIpmcStarGroupHit, qual_flags);

    /* LOOKUP_STATUS_VECTOR_5 - 23rd Bit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E4, 15, 63, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyL3DestRouteHit, qual_flags);

    /************* 2 BIT Section **********/
    /* L3_TYPE_1 - 1st Bit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 3, 7, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyPimBidirDfStatus, qual_flags);

    /* CW_VALID_PROTECTION_DROP - 1st Bit */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 25, 51, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyRecoverableDrop, qual_flags);


    /* 8 BIT Section - BFD_BUS (IFP_BFD_BUS_FORMAT) */
    /* MPLS_CONTROL_PKT */
    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 16, 128, 1);
    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMplsControlPkt, qual_flags);

    if (soc_feature(unit, soc_feature_bfd)) {
        /* GOOD_BFD_PKT */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 16, 129, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdGoodPkt, qual_flags);

        /* BFD_DIP_LOOPBACK */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E8, 16, 130, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyBfdPktWithDstIpLoopbackAddr, qual_flags);
    }

    if (!soc_feature(unit, soc_feature_ifp_no_inports_support)) {
       /* OPAQUE_3 */
       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 44, 704, 16);
       BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyInPorts_1, qual_flags);

       /* OPAQUE_4 */
       BCMI_KEYGEN_QUAL_CFG_INIT(unit);
       BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E16, 45, 720, 16);
       BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, _bcmFieldQualifyInPorts_2, qual_flags);
    }

#if defined(BCM_HURRICANE4_SUPPORT) && defined(INCLUDE_XFLOW_MACSEC)
    if (soc_feature(unit, soc_feature_xflow_macsec_svtag)) {
        SHR_BITSET(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 4, 9, 1);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(1, BCMI_KEYGEN_EXT_SECTION_L1E2, 13, 26, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMacSecFlow, qual_flags);
        /* IPV4_CHECKSUM_OK_MACDA_RANGE_CHECK_RESULT */
        BCMI_KEYGEN_QUAL_CFG_INIT(unit);
        BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(0, BCMI_KEYGEN_EXT_SECTION_L1E2, 26, 53, 1);
        BCMI_KEYGEN_QUAL_CFG_INSERT(unit, db, bcmFieldQualifyMacSecDstMacRangeHit, qual_flags);
        SHR_BITCLR(qual_flags.w, BCMI_KEYGEN_QUAL_FLAGS_NOT_IN_EM);
    }
#endif

    return _field_td3_flex_extractor_qual_init(unit, stage_fc);

exit:
    BCMI_KEYGEN_FUNC_EXIT();
}

#if defined(BCM_HURRICANE4_SUPPORT)
#if defined(INCLUDE_XFLOW_MACSEC)

/*
 * Function:
 *     _bcm_hr4_field_qualify_MacSecFlow
 *
 * Purpose:
 *   Qualifies on the MACSEC packet flow
 *
 * Parameters:
 *    unit     BCM device number
 *    entry    Field entry ID
 *    data     Qualifier match data
 *    mask     Qualifier match mask
 *
 * Returns:
 *    BCM_E_NONE        Operation completed successfully
 *    BCM_E_INIT        BCM unit not initialized
 *    BCM_E_PARAM       Invalid Parameter passed
 *    BCM_E_NOT_FOUND   Entry ID not found
 */
int
_bcm_hr4_field_qualify_MacSecFlow(
               int unit,
               bcm_field_entry_t entry_id,
               uint8 data,
               uint8 mask)
{
    int    rv = BCM_E_UNAVAIL;
    _field_entry_t      *f_ent;      /* Field entry pointer. */
    _field_group_t      *fg;         /* Field group structure. */
    _field_stage_t      *stage_fc;   /* Stage field control. */

    FP_LOCK(unit);
    rv = _bcm_field_entry_get_by_id(unit, entry_id, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }
    fg = f_ent->group;
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }
    if ((_BCM_FIELD_STAGE_LOOKUP  == stage_fc->stage_id) ||
        (_BCM_FIELD_STAGE_INGRESS  == stage_fc->stage_id)) {
        switch (data) {
            case bcmFieldMacSecFlowAfterEncrypt:
                data = 1;
                break;
            case bcmFieldMacSecFlowAfterDecrypt:
                data = 3;
                break;
            default :
                FP_UNLOCK(unit);
                return BCM_E_PARAM;
        }
    } else if (_BCM_FIELD_STAGE_EGRESS  == stage_fc->stage_id) {
#if defined(CANCUN_SUPPORT)
        if (soc_feature(unit, soc_feature_cancun)) {
            switch (data) {
                case bcmFieldMacSecFlowForEncrypt:
                    data = 18;
                    break;
                case bcmFieldMacSecFlowForDecrypt:
                    data = 3;
                    break;
                default :
                    FP_UNLOCK(unit);
                    return BCM_E_PARAM;
            }
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
    } else {
        /* Not supported in other stages */
        return BCM_E_UNAVAIL;
    }
    rv = _field_qualify32(unit, entry_id, bcmFieldQualifyMacSecFlow, data, mask);
    FP_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *     _bcm_hr4_field_qualify_MacSecFlow_get
 *
 * Purpose:
 * To get the qualified inputs on the MACSEC packet flow
 *
 * Parameters:
 *    unit     BCM device number
 *    entry    Field entry ID
 *    data     Qualifier match data
 *    mask     Qualifier match mask
 *
 * Returns:
 *    BCM_E_NONE        Operation completed successfully
 *    BCM_E_INIT        BCM unit not initialized
 *    BCM_E_PARAM       Invalid Parameter passed
 *    BCM_E_NOT_FOUND   Entry ID not found
 */
int
_bcm_hr4_field_qualify_MacSecFlow_get(
               int unit,
               bcm_field_entry_t entry_id,
               uint32 *data,
               uint32 *mask)
{
    int    rv = BCM_E_NONE;
    _field_entry_t      *f_ent;      /* Field entry pointer. */
    _field_group_t      *fg;         /* Field group structure. */
    _field_stage_t      *stage_fc;   /* Stage field control. */

    *mask = 0xFF;
    FP_LOCK(unit);
    rv = _bcm_field_entry_get_by_id(unit, entry_id, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }
    fg = f_ent->group;
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }
    if ((_BCM_FIELD_STAGE_LOOKUP  == stage_fc->stage_id) ||
        (_BCM_FIELD_STAGE_INGRESS  == stage_fc->stage_id)) {
        switch (*data) {
            case 1:
                *data = bcmFieldMacSecFlowAfterEncrypt;
                break;
            case 3:
                *data = bcmFieldMacSecFlowAfterDecrypt;
                break;
            default :
                FP_UNLOCK(unit);
                return BCM_E_PARAM;
        }
    } else if (_BCM_FIELD_STAGE_EGRESS  == stage_fc->stage_id) {
#if defined(CANCUN_SUPPORT)
        if (soc_feature(unit, soc_feature_cancun)) {
            switch (*data) {
                case 18:
                    *data = bcmFieldMacSecFlowForEncrypt;
                    break;
                case 3:
                    *data = bcmFieldMacSecFlowForDecrypt;
                    break;
                default :
                    FP_UNLOCK(unit);
                    return BCM_E_PARAM;
            }
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
    } else {
        /* Not supported in other stages */
        FP_UNLOCK(unit);
        return BCM_E_UNAVAIL;
    }
    FP_UNLOCK(unit);
    return rv;
}
#endif /* INCLUDE_XFLOW_MACSEC */
#endif /* BCM_HURRICANE4_SUPPORT */

/*
 * Function: _bcm_field_td3_ForwardingType_set
 *
 * Purpose:
 *     Add qualification data to an entry for IP Type.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     stage_id - (IN) Stage ID.
 *     entry    - (IN) Entry ID.
 *     type     - (IN) Forwarding type.
 *     data     - (OUT) Data.
 *     mask     - (OUT) Mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask)
{

    if (NULL == data || NULL == mask) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
       case _BCM_FIELD_STAGE_FLOWTRACKER:
           switch (type) {
            case bcmFieldForwardingTypeAny:
                *data = 0;
                *mask = 0;
                break;
            case bcmFieldForwardingTypeL2:
                *data = 0;
                *mask = 0xE;
                break;
            case bcmFieldForwardingTypeL2Independent:
                *data = 0;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Shared:
                *data = 1;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Vpn:
                *data = 2;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2SharedVpn:
                *data = 3;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypePortDirect:
                *data = 4;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeSvpDirect:
                *data = 5;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeVlanDirect:
                *data = 6;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2VpnDirect:
/* PTP_VFI */
                *data = 7;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3:
                *data = 8;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3Direct:
                *data = 9;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeMpls:
                *data = 10;
                *mask = 0xF;
                break;
            default:
               return (BCM_E_PARAM);
          }
          break;
      default:
          return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_td3_forwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stage_id - (IN) Stage ID.
 *      entry    - (IN) BCM field entry id.
 *      type     - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_td3_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type)
{
    if (NULL == type) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
       case _BCM_FIELD_STAGE_EXACTMATCH:
       case _BCM_FIELD_STAGE_FLOWTRACKER:
          if ((0 == data) && (mask == 0)) {
              *type = bcmFieldForwardingTypeAny;
          } else if ((0 == data) && (mask == 0xE)) {
              *type = bcmFieldForwardingTypeL2;
          } else if ((0 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Independent;
          } else if ((1 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Shared;
          } else if ((2 == data) && (mask == 0xF)) {
              *type =  bcmFieldForwardingTypeL2Vpn;
          } else if ((7 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2VpnDirect;
          } else if ((9 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3Direct;
          } else if ((8 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3;
          } else if ((10 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeMpls;
          } else {
              return (BCM_E_INTERNAL);
          }
           break;
       default:
           return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}
/* Function    : _bcm_field_td3_tpid_hw_encode
 * Description : Encode tpid value to hw specific value.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) tpid   TPID to encode
 *               (OUT) hw_code HW code.
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_tpid_hw_encode(int unit, uint16 tpid, uint16 outer_tpid,
                                                     uint32 *hw_code)
{
    int rv = BCM_E_NONE;  /* Operation return status. */
    soc_reg_t *tpid_r;
    uint32 reg_val;
    uint16 tpid_value;
    int idx;

    if (outer_tpid == 1) {
	tpid_r = outer_tpid_r;
    } else {
	tpid_r = inner_tpid_r;
    }
    for (idx=0; idx<4; idx++) {
	BCM_IF_ERROR_RETURN(soc_reg32_get(unit, tpid_r[idx],
                                             REG_PORT_ANY, 0,
                                             &reg_val));
        tpid_value = soc_reg_field_get(unit, tpid_r[idx],
                                             reg_val, TPIDf);
	if (tpid == tpid_value) {
	    *hw_code = idx;
	    break;
	}
    }
    if (idx == 4) {
    /* None of the TPID REG values matched with
     * the user qualified value. Return error */
      rv = (BCM_E_UNAVAIL);
    }
    return rv;
}
/* Function    : _field_tpid_hw_decode
 * Description : Encode tpid value to hw specific value.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) hw_code HW code.
 *               (OUT) tpid  decoded TPID
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_tpid_hw_decode(int unit, uint32 hw_code, uint16 outer_tpid,
                                                           uint16 *tpid)
{
    soc_reg_t *tpid_r;
    uint32 reg_val;

    if (outer_tpid == 1) {
	tpid_r = outer_tpid_r;
    } else {
	tpid_r = inner_tpid_r;
    }
    if (hw_code >= 4) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, tpid_r[hw_code],
                                      REG_PORT_ANY, 0,
                                      &reg_val));
    *tpid = soc_reg_field_get(unit, tpid_r[hw_code],
                              reg_val, TPIDf);
    return (BCM_E_NONE);
}

/* Function    : _field_td3_drop_vector_mask_init
 * Description : To initialize drop vector mask for IFP and EM stages.
 * Parameters  : (IN) unit   BCM driver unit
 *               (IN) stage.
 *               (OUT) Drop vector in Hw set to default values
 * Returns     : BCM_E_XXX
 */
int
_bcm_field_td3_drop_vector_mask_init(int unit)
{
    uint32 drop_vector_mask[3];
    uint32 entry_buf[_BCM_FP_MEM_FIELD_BYTES] = {0};
    soc_format_t fmt = PKT_PROC_DROP_VECTORfmt;


     /* drop vector mask is 96 bits width. Setting all bits to 1. */
     sal_memset(drop_vector_mask, -1, sizeof(uint32) * 3);

     /* clear the entry */
     sal_memset(entry_buf, 0x0, sizeof(entry_buf));
     /* IFP_EM_DROP_VECTOR_MASK_PROFILE_1 for IFP_TCAM */
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       entry_buf, IFP_EM_KEY_MASK_0f, drop_vector_mask);
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       entry_buf, IFP_EM_KEY_MASK_1f, drop_vector_mask);
     /* MEM write */
     BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                       IFP_EM_DROP_VECTOR_MASK_PROFILE_1m,
                       MEM_BLOCK_ANY, 1, entry_buf));

     /* clear the entry for writing profile 0*/
     sal_memset(entry_buf, 0x0, sizeof(entry_buf));
     /* PFMf */
     soc_format_field32_set(unit, fmt, drop_vector_mask, PFMf, 0);
     /* PVLAN_VP_EFILTERf */
     soc_format_field32_set(unit, fmt, drop_vector_mask, PVLAN_VP_EFILTERf, 0);
     /* MC_PBM_ZEROf */
     soc_format_field32_set(unit, fmt, drop_vector_mask, RESERVED_81f, 0);
     /* MULTICAST_INDEX_ERRORf */
     soc_format_field32_set(unit, fmt, drop_vector_mask, MULTICAST_INDEX_ERRORf, 0);

     /* IFP_EM_DROP_VECTOR_MASK_PROFILE_0 is for IFP_LOGICAL_TABLE_SELECT */
     soc_mem_field_set(unit, IFP_EM_DROP_VECTOR_MASK_PROFILE_0m, entry_buf,
                       IFP_EM_LOGICAL_TABLE_SELECT_KEY_MASKf, drop_vector_mask);
     /* MEM write */
     BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                       IFP_EM_DROP_VECTOR_MASK_PROFILE_0m,
                       MEM_BLOCK_ANY, 1, entry_buf));
     return (BCM_E_NONE);
}

STATIC
_flex_ceh_field_type_t flex_pkt_type_arr[bcmFieldPktTypeLastCount] = {
    {"MPLS"},
    {"MPLS"},
    {"MPLS_1_LABEL"},
    {"MPLS_2_LABEL"},
    {"MPLS_3_LABEL"},
    {"MPLS_4_LABEL"},
    {"MPLS_5_LABEL"},
    {"MPLS_6_LABEL"},
    {"MPLS_7_LABEL"},
    {"MPLS_MORE_THAN_7_LABEL"},
    {"FCOE"},
    {"FCOE"},
    {"FCOE_VFT"},
    {"FCOE_ENCAP"},
    {"FCOE_IFR"},
    {"FCOE_STD"},
    {"FCOE_VFT_STD"},
    {"FCOE_ENCAP_STD"},
    {"FCOE_IFR_STD"},
    {"FIP"},
    {"1588"},
    {"ARP"},
    {"RARP"},
    {"UNKNOWN_ETHERTYPE"},
    {"MIM"},
    {"OAM"},
    {"CNM"},
    {"NSH_OVER_L2"},
    {"NSH_OVER_MPLS_1_LABEL"},
    {"NSH_OVER_MPLS_2_LABEL"},
    {"IPV4"},
    {"IPV4"},
    {"NSH"},
    {"GPE_NSH"},
    {"GRE"},
    {"BFD"},
    {"VXLAN"},
    {"GPE"},
    {"GENEVE_NSH"},
    {"1588_UDP"},
    {"VXLAN_FLEX"},
    {"IPINIP"},
    {"IPINGRE"},
    {"NVGRE"},
    {"GENEVE"},
    {"GUE"},
    {"TCP"},
    {"UDP"},
    {"ICMP"},
    {"IGMP"},
    {"IP_WITH_EXTRA_4_BYTES"},
    {"SCTP"},
    {"MPLS_IN_IP_WITH_ONE_LABEL"},
    {"MPLS_IN_IP_WITH_ONE_LABEL"},
    {"MPLS_IN_IP_WITH_TWO_LABEL"},
    {"MPLS_IN_IP_WITH_THREE_LABEL"},
    {"MPLS_IN_IP_WITH_FOUR_LABEL"},
    {"MPLS_IN_IP_WITH_ONE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_ONE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_TWO_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_THREE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_FOUR_LABEL_AND_CW"},
    {"IPV6"},
    {"IPV6"},
    {"NSH"},
    {"GPE_NSH"},
    {"GRE"},
    {"BFD"},
    {"VXLAN"},
    {"GPE"},
    {"GENEVE_NSH"},
    {"1588_UDP"},
    {"VXLAN_FLEX"},
    {"IPINIP"},
    {"IPINGRE"},
    {"NVGRE"},
    {"GENEVE"},
    {"GUE"},
    {"TCP"},
    {"UDP"},
    {"ICMP"},
    {"IGMP"},
    {"IP_WITH_EXTRA_4_BYTES"},
    {"SCTP"},
    {"MPLS_IN_IP_WITH_ONE_LABEL"},
    {"MPLS_IN_IP_WITH_ONE_LABEL"},
    {"MPLS_IN_IP_WITH_TWO_LABEL"},
    {"MPLS_IN_IP_WITH_THREE_LABEL"},
    {"MPLS_IN_IP_WITH_FOUR_LABEL"},
    {"MPLS_IN_IP_WITH_ONE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_ONE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_TWO_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_THREE_LABEL_AND_CW"},
    {"MPLS_IN_IP_WITH_FOUR_LABEL_AND_CW"},
    {"NSH_OVER_MPLS_3_LABEL"},
    {"INT"},
    {"INT"},
    {"INT"},
    {"GPE_INT"},
    {"GPE_INT"},
    {"GPE_INT"},
    {"TCP_INT"},
    {"TCP_INT"},
    {"TCP_INT"},
    {"IFA_TCP"},
    {"IFA_UDP"},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {"GPE_IOAM_E2E"},
    {"GPE_IOAM_E2E"},
    {"GPE_IOAM_E2E"},
    {"IFA_UDP"},
    {"IFA_TCP"},
};

int
_bcm_field_td3_qualify_PktType(int unit, bcm_field_pkt_type_t type,
                               uint32 *data, uint32 *mask)
{
    uint32 cancun_ver = 0;
    int rv;
    uint32 mplsType = 0x0;
    uint32 fcoEType = 0x0;
    uint32 ipv4Type = 0x0;
    uint32 ipv6Type = 0x0;
    uint32 hw_data = 0x0;
    uint32 hw_mask = 0x0;

    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       flex_pkt_type_arr[type].field_str,
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "type:%d obj_str:%s field_str:%s\n\r"), type, "Z2_MATCH_ID",
               flex_pkt_type_arr[type].field_str));
       /*
        * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
        * supported in the current CANCUN release or for this chip.
        */
       rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
       return rv;
    }
    if (sal_strcmp(flex_pkt_type_arr[type].field_str, "MPLS") == 0) {
       mplsType = ceh_info.value;
    } else if (sal_strcmp(flex_pkt_type_arr[type].field_str, "FCOE") == 0) {
       fcoEType = ceh_info.value;
    } else if (sal_strcmp(flex_pkt_type_arr[type].field_str, "IPV4") == 0) {
       ipv4Type = ceh_info.value;
    } else if (sal_strcmp(flex_pkt_type_arr[type].field_str, "IPV6") == 0) {
       ipv6Type = ceh_info.value;
    }

    hw_mask = (1 << ceh_info.width) - 1;
    hw_data = ceh_info.value;

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
             type, ceh_info.offset, ceh_info.width, ceh_info.value, hw_data, hw_mask));

    switch (type) {
       case bcmFieldPktTypeMplsNot:
            *data = 0;
            *mask = mplsType;
            break;
       case bcmFieldPktTypeMplsAny:
            *data = mplsType;
            *mask = mplsType;
            break;
       case bcmFieldPktTypeFCoENot:
            *data = 0;
            *mask = fcoEType;
            break;
       case bcmFieldPktTypeFCoEAny:
            *data = fcoEType;
            *mask = fcoEType;
            break;
       case bcmFieldPktTypeIp4Not:
            *data = 0x0;
            *mask = ipv4Type;
            break;
       case bcmFieldPktTypeIp4Any:
            *data = ipv4Type;
            *mask = ipv4Type;
            break;
       case bcmFieldPktTypeIp4Nsh:
       case bcmFieldPktTypeIp4GpeNsh:
       case bcmFieldPktTypeIp4Gre:
       case bcmFieldPktTypeIp4Bfd:
       case bcmFieldPktTypeIp4Vxlan:
       case bcmFieldPktTypeIp4Gpe:
       case bcmFieldPktTypeIp4GeneveNsh:
       case bcmFieldPktTypeIp41588Udp:
       case bcmFieldPktTypeIp4VxlanFlex:
       case bcmFieldPktTypeIp4IPinIP:
       case bcmFieldPktTypeIp4IPinGre:
       case bcmFieldPktTypeIp4Nvgre:
       case bcmFieldPktTypeIp4Geneve:
       case bcmFieldPktTypeIp4Gue:
       case bcmFieldPktTypeIp4Tcp:
       case bcmFieldPktTypeIp4Udp:
       case bcmFieldPktTypeIp4Icmp:
       case bcmFieldPktTypeIp4Igmp:
       case bcmFieldPktTypeIp4IpWithExtra4Bytes:
       case bcmFieldPktTypeIp4Sctp:
       case bcmFieldPktTypeIpv4UdpINT:
       case bcmFieldPktTypeIpv4IOAM:
       case bcmFieldPktTypeIpv4IOAME2E:
       case bcmFieldPktTypeIpv4TcpINT:
       case bcmFieldPktTypeIp4MplsInIpAny:
       case bcmFieldPktTypeIp4MplsInIpWithOneLabel:
       case bcmFieldPktTypeIp4MplsInIpWithTwoLabel:
       case bcmFieldPktTypeIp4MplsInIpWithThreeLabel:
       case bcmFieldPktTypeIp4MplsInIpWithFourLabel:
       case bcmFieldPktTypeIp4MplsInIpWithAnyCw:
       case bcmFieldPktTypeIp4MplsInIpWithOneLabelAndCw:
       case bcmFieldPktTypeIp4MplsInIpWithTwoLabelAndCw:
       case bcmFieldPktTypeIp4MplsInIpWithThreeLabelAndCw:
       case bcmFieldPktTypeIp4MplsInIpWithFourLabelAndCw:
            rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                               "IPV4", &ceh_info));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"),
                         type, "Z2_MATCH_ID", "IPV4"));
               return rv;
            }
            *mask = (1 << ceh_info.width) - 1;
            *data = ((hw_data << ceh_info.width) | ceh_info.value);
            *mask = ((hw_mask << ceh_info.width) | *mask);
            break;

       case bcmFieldPktTypeIp6Not:
            *data = 0x0;
            *mask = ipv6Type;
            break;
       case bcmFieldPktTypeIp6Any:
            *data = ipv6Type;
            *mask = ipv6Type;
            break;
       case bcmFieldPktTypeIp6Nsh:
       case bcmFieldPktTypeIp6GpeNsh:
       case bcmFieldPktTypeIp6Gre:
       case bcmFieldPktTypeIp6Bfd:
       case bcmFieldPktTypeIp6Vxlan:
       case bcmFieldPktTypeIp6Gpe:
       case bcmFieldPktTypeIp6GeneveNsh:
       case bcmFieldPktTypeIp61588Udp:
       case bcmFieldPktTypeIp6VxlanFlex:
       case bcmFieldPktTypeIp6IPinIP:
       case bcmFieldPktTypeIp6IPinGre:
       case bcmFieldPktTypeIp6Nvgre:
       case bcmFieldPktTypeIp6Geneve:
       case bcmFieldPktTypeIp6Gue:
       case bcmFieldPktTypeIp6Tcp:
       case bcmFieldPktTypeIp6Udp:
       case bcmFieldPktTypeIp6Icmp:
       case bcmFieldPktTypeIp6Igmp:
       case bcmFieldPktTypeIp6IpWithExtra4Bytes:
       case bcmFieldPktTypeIp6Sctp:
       case bcmFieldPktTypeIpv6UdpINT:
       case bcmFieldPktTypeIpv6IOAM:
       case bcmFieldPktTypeIpv6IOAME2E:
       case bcmFieldPktTypeIpv6TcpINT:
       case bcmFieldPktTypeIp6MplsInIpAny:
       case bcmFieldPktTypeIp6MplsInIpWithOneLabel:
       case bcmFieldPktTypeIp6MplsInIpWithTwoLabel:
       case bcmFieldPktTypeIp6MplsInIpWithThreeLabel:
       case bcmFieldPktTypeIp6MplsInIpWithFourLabel:
       case bcmFieldPktTypeIp6MplsInIpWithAnyCw:
       case bcmFieldPktTypeIp6MplsInIpWithOneLabelAndCw:
       case bcmFieldPktTypeIp6MplsInIpWithTwoLabelAndCw:
       case bcmFieldPktTypeIp6MplsInIpWithThreeLabelAndCw:
       case bcmFieldPktTypeIp6MplsInIpWithFourLabelAndCw:
            rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                               "IPV6", &ceh_info));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"),
                        type, "Z2_MATCH_ID", "IPV6"));
               return rv;
            }
            *mask = (1 << ceh_info.width) - 1;
            *data = ((hw_data << ceh_info.width) | ceh_info.value);
            *mask = ((hw_mask << ceh_info.width) | *mask);
            break;

       case bcmFieldPktTypeUdpINT:
       case bcmFieldPktTypeIOAM:
       case bcmFieldPktTypeIOAME2E:
       case bcmFieldPktTypeTcpINT:
       case bcmFieldPktTypeTcpIfa:
       case bcmFieldPktTypeUdpIfa:
            rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                             flex_pkt_type_arr[type].field_str,
                                             &ceh_info));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"),
                         type, "Z2_MATCH_ID", flex_pkt_type_arr[type].field_str));
               return rv;
            }
            *data = (hw_data << ceh_info.offset);
            *mask = (hw_mask << ceh_info.offset);
            break;

       case bcmFieldPktTypeUdpManyInOne:
       case bcmFieldPktTypeTcpManyInOne:
            BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
            if ((SOC_IS_TRIDENT3(unit)
                     && (cancun_ver < SOC_CANCUN_VERSION_DEF_6_3_3))
                 || (SOC_IS_HELIX5(unit)
                        && (cancun_ver < SOC_CANCUN_VERSION_DEF_4_2_0))
                 || (SOC_IS_MAVERICK2(unit)
                        && (cancun_ver < SOC_CANCUN_VERSION_DEF_4_3_2))) {
                return BCM_E_UNAVAIL;
            }
            rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                             flex_pkt_type_arr[type].field_str,
                                             &ceh_info));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"),
                         type, "Z2_MATCH_ID", flex_pkt_type_arr[type].field_str));
               return rv;
            }
            *data = (hw_data << ceh_info.offset);
            if (bcmFieldPktTypeUdpManyInOne == type) {
                /* Clear Bit1 and Bit2 to match Many rules in 1.*/
                hw_mask = hw_mask & (~(0x6));
            } else if (bcmFieldPktTypeTcpManyInOne == type) {
                /* Clear Bit1 to match Many rules in 1.*/
                hw_mask = hw_mask & (~(0x2));
            }
            *mask = (hw_mask << ceh_info.offset);
            break;

       default:
            *data = hw_data;
            *mask = hw_mask;
            break;
    }

    return BCM_E_NONE;
}


int
_bcm_field_td3_qualify_PktType_get(int unit,
                                   uint32 data,
                                   uint32 mask,
                                   bcm_field_pkt_type_t *type)
{
    int rv;
    int idx;
    uint32 mplsType = 0x0;
    uint32 fcoEType = 0x0;
    uint32 ipv4Type = 0x0;
    uint32 ipv6Type = 0x0;
    uint32 ipv4Mask = 0x0;
    uint32 ipv6Mask = 0x0;
    uint32 ipv4Width = 0x0;
    uint32 ipv6Width = 0x0;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    *type = bcmFieldPktTypeLastCount;

    /* Check if it is generic INT type (for v4/v6 over UDP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "INT", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                 "Z2_MATCH_ID", "INT"));
       return rv;
    }
    if ((data == ceh_info.value) && (mask == ((1 << ceh_info.width) - 1))) {
       *type = bcmFieldPktTypeUdpINT;
       return rv;
    }

    /* Check if it is generic IOAM type (without IP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "GPE_INT", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "GPE_INT"));
       return rv;
    }
    if ((data == ceh_info.value) && (mask == ((1 << ceh_info.width) - 1))) {
       *type = bcmFieldPktTypeIOAM;
       return rv;
    }

    /* Check if it is generic INT type (for v4/v6 over TCP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "TCP_INT", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "TCP_INT"));
       return rv;
    }
    if ((data == ceh_info.value) && (mask == ((1 << ceh_info.width) - 1))) {
       *type = bcmFieldPktTypeTcpINT;
       return rv;
    }

    /* Check if it is generic IFA type (Over TCP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "IFA_TCP", &ceh_info));
    if ((BCM_E_NOT_FOUND != rv) && SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "IFA_TCP"));
       return rv;
    }
    if ((data == ceh_info.value << ceh_info.offset)
            && (mask == ((1 << ceh_info.width) - 1) << ceh_info.offset)) {
       *type = bcmFieldPktTypeTcpIfa;
       return rv;
    }

    if ((data == ceh_info.value << ceh_info.offset)
            && (mask == (((1 << ceh_info.width) - 1) & (~(0x2))) << ceh_info.offset)) {
       *type = bcmFieldPktTypeTcpManyInOne;
       return rv;
    }

    /* Check if it is generic IFA type (Over UDP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "IFA_UDP", &ceh_info));
    if ((BCM_E_NOT_FOUND != rv) && SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "IFA_UDP"));
       return rv;
    }
    if ((data == ceh_info.value << ceh_info.offset)
            && (mask == ((1 << ceh_info.width) - 1) << ceh_info.offset)) {
       *type = bcmFieldPktTypeUdpIfa;
       return rv;
    }

    if ((data == ceh_info.value << ceh_info.offset)
            && (mask == (((1 << ceh_info.width) - 1) & (~(0x6))) << ceh_info.offset)) {
       *type = bcmFieldPktTypeUdpManyInOne;
       return rv;
    }

    /* Check if it is generic IFA type (Over UDP) */
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "GPE_IOAM_E2E", &ceh_info));
    if ((BCM_E_NOT_FOUND != rv) && SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "GPE_IOAM_E2E"));
       return rv;
    }
    if ((data == ceh_info.value << ceh_info.offset)
            && (mask == ((1 << ceh_info.width) - 1) << ceh_info.offset)) {
       *type = bcmFieldPktTypeIOAME2E;
       return rv;
    }

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "IPV4", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                   "Z2_MATCH_ID", "IPV4"));
       return rv;
    }
    ipv4Type = ceh_info.value;
    ipv4Mask = (1 << ceh_info.width) - 1;
    ipv4Width = ceh_info.width;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "IPV6", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("obj_str:%s field_str:%s\n\r"),
                  "Z2_MATCH_ID", "IPV6"));
       return rv;
    }
    ipv6Type = ceh_info.value;
    ipv6Mask = (1 << ceh_info.width) - 1;
    ipv6Width = ceh_info.width;

    for (idx = 0; idx < COUNTOF(flex_pkt_type_arr); idx++) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           flex_pkt_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"), idx, "Z2_MATCH_ID",
                        flex_pkt_type_arr[idx].field_str));
            continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("type:%d obj_str:%s field_str:%s\n\r"), idx, "Z2_MATCH_ID",
                   flex_pkt_type_arr[idx].field_str));
           return rv;
        }
        if (sal_strcmp(flex_pkt_type_arr[idx].field_str, "MPLS") == 0) {
           mplsType = ceh_info.value;
           if ((data == 0) && (mask == mplsType)) {
              *type = bcmFieldPktTypeMplsNot;
           } else  if ((data == mplsType) && (mask == mplsType)) {
              *type = bcmFieldPktTypeMplsAny;
           }
        } else if (sal_strcmp(flex_pkt_type_arr[idx].field_str, "FCOE") == 0) {
           fcoEType = ceh_info.value;
           if ((data == 0) && (mask == fcoEType)) {
              *type = bcmFieldPktTypeFCoENot;
           } else if ((data == fcoEType) && (mask == fcoEType)) {
              *type = bcmFieldPktTypeFCoEAny;
           }
        } else if (sal_strcmp(flex_pkt_type_arr[idx].field_str, "IPV4") == 0) {
           ipv4Type = ceh_info.value;
           ipv6Type = 0;
           if ((data == 0x0) && (mask == ipv4Type)) {
              *type = bcmFieldPktTypeIp4Not;
           } else if ((data == ipv4Type) && (mask == ipv4Type)) {
              *type = bcmFieldPktTypeIp4Any;
           }
        } else if (sal_strcmp(flex_pkt_type_arr[idx].field_str, "IPV6") == 0) {
           ipv6Type = ceh_info.value;
           ipv4Type = 0;
           if ((data == 0x0) && (mask == ipv6Type)) {
              *type = bcmFieldPktTypeIp6Not;
           } else if ((data == ipv6Type) && (mask == ipv6Type)) {
              *type = bcmFieldPktTypeIp6Any;
           }
        }

        if (*type != bcmFieldPktTypeLastCount) {
            return BCM_E_NONE;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        if (data & ipv4Type) {
           hw_data <<= ipv4Width;
           hw_data |= ipv4Type;
           hw_mask <<= ipv4Width;
           hw_mask |= ipv4Mask;
        } else if (data & ipv6Type) {
           hw_data <<= ipv6Width;
           hw_data |= ipv6Type;
           hw_mask <<= ipv6Width;
           hw_mask |= ipv6Mask;
        }

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                   data, mask, hw_data, hw_mask));

        if ((hw_data == data) && (hw_mask == mask)) {
           *type = idx;
           return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

STATIC
_flex_ceh_field_type_t flex_ip_option_type_arr[bcmFieldIpOptionHdrTypeLastCount] = {
    {"AH"},
    {"ESP"},
    {"WESP"},
    {"AH_ESP"},
    {"AH_WESP"},
    {"GENERIC"},
    {"FRAGMENTATION"},
    {"GENERIC_GENERIC"},
    {"GENERIC_FRAGMENTATION"},
    {"GENERIC_AH"},
    {"GENERIC_ESP"},
    {"GENERIC_WESP"},
    {"FRAGMENTATION_AH"},
    {"FRAGMENTATION_ESP"},
    {"FRAGMENTATION_WESP"}};

int
_bcm_field_td3_qualify_IpOptionHdrType(int unit, bcm_field_qualify_t qual_id,
                                       bcm_field_ip_option_hdr_type_t type,
                                       uint8 *data, uint8 *mask)
{
   int rv;
   uint8 hw_data;
   uint8 hw_mask;
   uint8 ip_type_data;
   uint8 ip_type_mask;
   uint8 ip_type_width;

   soc_cancun_ceh_field_info_t ceh_info;

   if ((qual_id == bcmFieldQualifyIpv4OptionHdrType) ||
       (qual_id == bcmFieldQualifyOverlayIpv4OptionHdrType)) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           "IPV4", &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                     "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID", "IPV4"));
           /*
            * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
            * supported in the current CANCUN release or for this chip.
            */
           rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
           return rv;
        }
        ip_type_data = ceh_info.value;
        ip_type_width = ceh_info.width;
   } else {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           "IPV6", &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                    "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID", "IPV6"));
           /*
            * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
            * supported in the current CANCUN release or for this chip.
            */
           rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
           return rv;
        }
        ip_type_data = ceh_info.value;
        ip_type_width = ceh_info.width;
   }

   ip_type_mask = (1 << ip_type_width) - 1;

   rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                      flex_ip_option_type_arr[type].field_str,
                                      &ceh_info));
   if (SOC_FAILURE(rv)) {
      LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
              "type:%d obj_str:%s field_str:%s\n\r"), type, "Z2_MATCH_ID",
              flex_ip_option_type_arr[type].field_str));
      /*
       * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
       * supported in the current CANCUN release or for this chip.
       */
      rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
      return rv;
   }

   hw_mask = (1 << ceh_info.width) - 1;
   hw_data = ceh_info.value;

   hw_data <<= ip_type_width;
   hw_mask <<= ip_type_width;

   hw_data |= ip_type_data;
   hw_mask |= ip_type_mask;

   LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
            type, ceh_info.offset, ceh_info.width, ceh_info.value, hw_data, hw_mask));

   *data = hw_data;
   *mask = hw_mask;

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_IpOptionHdrType_get(int unit,
                                           bcm_field_qualify_t qual_id,
                                           uint8 data,
                                           uint8 mask,
                                           bcm_field_ip_option_hdr_type_t *type)
{
    int rv;
    int idx;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    uint8 ip_type_data;
    uint8 ip_type_mask;
    uint8 ip_type_width;

    soc_cancun_ceh_field_info_t ceh_info;

    if ((qual_id == bcmFieldQualifyIpv4OptionHdrType) ||
       (qual_id == bcmFieldQualifyOverlayIpv4OptionHdrType)) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           "IPV4", &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                    "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID", "IPV4"));
           return rv;
        }
        ip_type_data = ceh_info.value;
        ip_type_width = ceh_info.width;
    } else {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           "IPV6", &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                     "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID", "IPV6"));
           return rv;
        }
        ip_type_data = ceh_info.value;
        ip_type_width = ceh_info.width;
    }

    ip_type_mask = (1 << ip_type_width) - 1;

    for (idx = 0; idx < bcmFieldIpOptionHdrTypeLastCount; idx++) {
        if (sal_strcmp(flex_ip_option_type_arr[idx].field_str, "\0") == 0) {
           continue;
        }

        rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                           flex_ip_option_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                     "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z2_MATCH_ID",
                      flex_ip_option_type_arr[idx].field_str));
           continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                     "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z2_MATCH_ID",
                      flex_ip_option_type_arr[idx].field_str));
           return rv;
        }
        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        hw_data <<= ip_type_width;
        hw_mask <<= ip_type_width;

        hw_data |= ip_type_data;
        hw_mask |= ip_type_mask;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "type:%d data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                  idx, data, mask, hw_data, hw_mask));

        if ((hw_data == data) && (hw_mask == mask)) {
           *type = idx;
           return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

STATIC
_flex_ceh_field_type_t flex_l2_pkt_type_arr[bcmFieldL2PktTypeLastCount] = {
      {"ETH"},
      {"SNAP"},
      {"LLC"}};

int
_bcm_field_td3_qualify_L2PktType(int unit,
                                 bcm_field_l2_pkt_type_t type,
                                 uint8 *data,
                                 uint8 *mask)
{
    int rv;
    uint32 hw_data = 0x0;
    uint32 hw_mask = 0x0;

    soc_cancun_ceh_field_info_t ceh_info;

    if (sal_strcmp(flex_l2_pkt_type_arr[type].field_str, "\0") != 0) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                            flex_l2_pkt_type_arr[type].field_str,
                                            &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                  "type:%d obj_str:%s field_str:%s\n\r"), type, "Z1_MATCH_ID",
                   flex_l2_pkt_type_arr[type].field_str));
           /*
            * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
            * supported in the current CANCUN release or for this chip.
            */
           rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
           return rv;
        }
        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        *data = hw_data;
        *mask = hw_mask;
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
              type, ceh_info.offset, ceh_info.width,
              ceh_info.value, hw_data, hw_mask));
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_L2PktType_get(int unit,
                                     uint8 data,
                                     uint8 mask,
                                     bcm_field_l2_pkt_type_t *type)
{
    int rv;
    int idx;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    for (idx = 0; idx < bcmFieldL2PktTypeLastCount; idx++) {

        if (sal_strcmp(flex_l2_pkt_type_arr[idx].field_str, "\0") == 0) {
           continue;
        }

        rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                           flex_l2_pkt_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                            "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z1_MATCH_ID",
                        flex_l2_pkt_type_arr[idx].field_str));
            continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                   "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z1_MATCH_ID",
                   flex_l2_pkt_type_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                  data, mask, hw_data, hw_mask));

        if ((hw_data == data) && (hw_mask == mask)) {
           *type = idx;
           return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

STATIC
_flex_ceh_field_type_t flex_nwtag_type_arr[bcmFieldNetworkTagLastCount] = {
      {"VNTAG"},
      {"ETAG"}};

 int
_bcm_field_td3_qualify_NetworkTagType(int unit,
                                      bcm_field_network_tag_type_t type,
                                      uint8 *data, uint8 *mask)
 {
    int rv;
    uint32 hw_data = 0x0;
    uint32 hw_mask = 0x0;

    soc_cancun_ceh_field_info_t ceh_info;

    if ((bcmFieldNetworkTagVnTag == type) &&
        !soc_feature(unit, soc_feature_niv)) {
        return BCM_E_PARAM;
    }
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                        flex_nwtag_type_arr[type].field_str,
                                        &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "type:%d obj_str:%s field_str:%s\n\r"), type, "Z1_MATCH_ID",
               flex_nwtag_type_arr[type].field_str));
       /*
        * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
        * supported in the current CANCUN release or for this chip.
        */
       rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
       return rv;
    }
    hw_mask = (1 << ceh_info.width) - 1;
    hw_data = ceh_info.value;

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
            type, ceh_info.offset, ceh_info.width,
            ceh_info.value, hw_data, hw_mask));

    *data = hw_data;
    *mask = hw_mask;

    return BCM_E_NONE;
 }


 int
_bcm_field_td3_qualify_NetworkTagType_get(int       unit,
                                          uint8     data,
                                          uint8     mask,
                                          bcm_field_network_tag_type_t *type)
{
    int rv;
    int idx;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    for (idx = 0; idx < bcmFieldNetworkTagLastCount; idx++) {
        if (sal_strcmp(flex_nwtag_type_arr[idx].field_str, "\0") == 0) {
           continue;
        }

        rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                           flex_nwtag_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                            "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z1_MATCH_ID",
                        flex_nwtag_type_arr[idx].field_str));
            continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                  "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z1_MATCH_ID",
                  flex_nwtag_type_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                   data, mask, hw_data, hw_mask));

        if ((hw_data == data) && (hw_mask == mask)) {
           *type = idx;
           return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

int
_bcm_field_td3_qualify_TimeStampTxPktType(int unit,
                       bcm_field_timestamp_transmit_pkt_type_t ts_pkt_type,
                       uint16 *data,
                       uint16 *mask)
{
    bcm_error_t rv = BCM_E_NONE;
    if ((NULL == data) || (NULL == mask)) {
        return BCM_E_PARAM;
    }
    switch (ts_pkt_type) {
        case bcmFieldTimestampTwampOwamp:
             *data = BCM_PKT_HDR_TS_OFFSET_TWAMP_OWAMP;
             *mask = 0xffff;
             break;
        default:
             return BCM_E_UNAVAIL;
    }
    return rv;
}

int
_bcm_field_td3_qualify_TimeStampTxPktType_get(int unit,
                       uint16 data,
                       uint16 mask,
                       bcm_field_timestamp_transmit_pkt_type_t *ts_pkt_type)
{
    bcm_error_t rv = BCM_E_NONE;
    if (NULL == ts_pkt_type) {
        return BCM_E_PARAM;
    }
    switch (data & mask) {
        case BCM_PKT_HDR_TS_OFFSET_TWAMP_OWAMP:
             *ts_pkt_type = bcmFieldTimestampTwampOwamp;
             break;
        default:
             return BCM_E_INTERNAL;
    }
    return rv;
}

STATIC
_flex_ceh_field_type_t flex_syshdr_type_arr[bcmFieldSysHdrLastCount] = {
    {"NO_SYS_HDR"},
    {"COE"},
    {"LOOPBACK"},
    {"LOOPBACK"},
    {"GENERIC_LOOPBACK"},
    {"CPU_MASQUERADE"},
    {"HIGIG"},
    {"HIGIG"},
    {"PPD0"},
    {"PPD2"},
    {"PPD0_EH_TYPE0"},
    {"PPD0_EH_TYPE1"},
    {"PPD0_EH_TYPE2"},
    {"PPD2_EH_TYPE0"},
    {"PPD2_EH_TYPE1"},
    {"PPD2_EH_TYPE2"},
    {"UNKNOWN_PPD_TYPE"},
    {"UNKNOWN_PPD_TYPE_WITH_EH"},
    {"PPD1"},
    {"PPD1_WITH_EH"},
    {"SOBMH_TS"},
    {"REDIRECT_TO_DEST"},
    };

int
_bcm_field_td3_qualify_SysHdrType(int unit,
                                  bcm_field_sys_hdr_type_t type,
                                  uint8 *data, uint8 *mask)
{
    int rv;
    soc_cancun_ceh_field_info_t ceh_info;

#if 0
        {bcmFieldSysHdrHiGigTypePpd0WithCustomEhType,       "PPD0_WITH_CUSTOM_EH_TYPE"},
        {bcmFieldSysHdrHiGigTypePpd1WithCustomEhType,       "PPD2_WITH_CUSTOM_EH_TYPE"},
        {bcmFieldSysHdrHiGigTypeUnknownPpdWithCustomEhType, "UNKNOWN_PPD_WITH_CUSTOM_EH_TYPE"},
        {bcmFieldSysHdrHiGigTypeSubPort,                    "HIGIG_WITH_COE"};
#endif
    if ((bcmFieldSysHdrSubPort == type) &&
        !(soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
          (soc_feature(unit, soc_feature_channelized_switching)))) {
        return BCM_E_PARAM;
    }
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z0_MATCH_ID",
                                       flex_syshdr_type_arr[type].field_str,
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "type:%d obj_str:%s field_str:%s\n\r"), type, "Z0_MATCH_ID",
               flex_syshdr_type_arr[type].field_str));
       /*
        * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
        * supported in the current CANCUN release or for this chip.
        */
       rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
       return rv;
    }

    *mask = (1 << ceh_info.width) - 1;
    *data = ceh_info.value;

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Type:%d - offset:%d width:%d value:%d mask:0x%x\n\r"),
              type, ceh_info.offset, ceh_info.width, ceh_info.value, *mask));


    switch(type) {
       case bcmFieldSysHdrHiGigNot:
       case bcmFieldSysHdrLoopbackNot:
            *data = 0x0;
            *mask <<= ceh_info.offset;
            break;
       case bcmFieldSysHdrHiGigAny:
       case bcmFieldSysHdrLoopbackAny:
       case bcmFieldSysHdrTxTimestamp:
            *data <<= ceh_info.offset;
            *mask <<= ceh_info.offset;
            break;
       default:
            break;
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_SysHdrType_get(int unit, uint8 data, uint8 mask,
                                      bcm_field_sys_hdr_type_t *type)
{
    int idx;
    int rv;
    uint8 lbType;
    uint8 hgType;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z0_MATCH_ID", "LOOPBACK",
                                       &ceh_info));
    SOC_IF_ERROR_RETURN(rv);
    lbType = ceh_info.value;
    lbType <<= ceh_info.offset;
    rv = (soc_cancun_ceh_obj_field_get(unit, "Z0_MATCH_ID", "HIGIG",
                                       &ceh_info));
    SOC_IF_ERROR_RETURN(rv);
    hgType = ceh_info.value;
    hgType <<= ceh_info.offset;

    *type = bcmFieldSysHdrLastCount;

    if (data == 0x0 && mask == lbType) {
       *type = bcmFieldSysHdrLoopbackNot;
    } else if (data == lbType && mask == lbType) {
       *type = bcmFieldSysHdrLoopbackAny;
    } else if (data == 0x0 && mask == hgType) {
       *type = bcmFieldSysHdrHiGigNot;
    } else if (data == hgType && mask == hgType) {
       *type = bcmFieldSysHdrHiGigAny;
    }

    if (*type != bcmFieldSysHdrLastCount) {
       return BCM_E_NONE;
    }

    for (idx = 0; idx < COUNTOF(flex_syshdr_type_arr); idx++) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "Z0_MATCH_ID",
                                           flex_syshdr_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                    "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z0_MATCH_ID",
                    flex_syshdr_type_arr[idx].field_str));
           continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                    "type:%d obj_str:%s field_str:%s\n\r"), idx, "Z0_MATCH_ID",
                    flex_syshdr_type_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                  data, mask, hw_data, hw_mask));

        if ((hw_data == data) && (hw_mask == mask)) {
           *type = idx;
           return BCM_E_NONE;
        }
    }

    return BCM_E_PARAM;
}

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
int
_bcm_field_td3_qual_EgressPortCtrlType_set(int unit,
                                           bcm_field_entry_t entry,
                                           bcm_field_egress_port_ctrl_t ctrl_type,
                                           uint32 *data, uint32 *mask)
{
    if ((NULL == data) || (NULL == mask)) {
        return BCM_E_PARAM;
    }

    switch (ctrl_type) {
        case bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt:
        /* CMH value for SubportSvtagEncrypt */
            *data = 18;
            *mask = BCM_FIELD_EXACT_MATCH_MASK;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qual_EgressPortCtrlType_get(int unit,
                                           bcm_field_entry_t entry,
                                           uint32 data, uint32 mask,
                                           bcm_field_egress_port_ctrl_t *ctrl_type)
{
    if (NULL == ctrl_type) {
        return BCM_E_PARAM;
    }

    switch (data & mask) {
        case 18:
            *ctrl_type = bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt;
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

#endif

STATIC
_flex_ceh_field_type_t flex_tunnel_type_arr[bcmFieldTunnelTypeCount] = {
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"\0"},
            {"VXLAN_FLEX"},
            {"GPE"},
            {"GENEVE"},
            {"MPLS_OVER_GRE"},
            {"NSH"}};


int
_bcm_field_td3_qualify_TunnelType(int unit,
                                  bcm_field_TunnelType_t tunnel_type,
                                  uint32 *tcam_data,
                                  uint32 *tcam_mask)
{
    int rv;
    soc_cancun_ceh_field_info_t ceh_info;

    if (sal_strcmp(flex_tunnel_type_arr[tunnel_type].field_str, "\0") != 0) {
        uint32 hw_data, hw_mask;

        rv = (soc_cancun_ceh_obj_field_get(unit, "PKT_FLOW_ID_1",
                                           flex_tunnel_type_arr[tunnel_type].field_str,
                                           &ceh_info));
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("tunnel_type:%d obj_str:%s field_str:%s\n\r"),
                               tunnel_type, "PKT_FLOW_ID_1",
                               flex_tunnel_type_arr[tunnel_type].field_str));
           /*
            * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
            * supported in the current CANCUN release or for this chip.
            */
           rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
                                tunnel_type, ceh_info.offset, ceh_info.width, ceh_info.value, hw_data, hw_mask));
        *tcam_data = hw_data;
        *tcam_mask = hw_mask;

        return BCM_E_NONE;
    } else {
        switch (tunnel_type) {
            case bcmFieldTunnelTypeNone:
                *tcam_data = 0x1;
                *tcam_mask = 0xf;
                break;
            case bcmFieldTunnelTypeMpls:
                *tcam_data = 0x6;
                *tcam_mask = 0xf;
                break;
            case bcmFieldTunnelTypeMim:
                *tcam_data = 0x2;
                *tcam_mask = 0xf;
                break;
            case bcmFieldTunnelTypeIp:
                *tcam_data = 0x5;
                *tcam_mask = 0xf;
                break;
            case bcmFieldTunnelTypeL2Gre:
                *tcam_data = 0x3;
                *tcam_mask = 0xf;
                break;
            case bcmFieldTunnelTypeVxlan:
                *tcam_data = 0x4;
                *tcam_mask = 0xf;
                break;
            default:
                return (BCM_E_PARAM);
        }

    }

    return (BCM_E_NONE);
}

int
_bcm_field_td3_qualify_TunnelType_get(int unit,
                                      uint8 tcam_data,
                                      uint8 tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type)
{
    int idx;
    int rv;
    uint8 hw_data = 0;
    uint8 hw_mask = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    for (idx = 0; idx < COUNTOF(flex_tunnel_type_arr); idx++) {
        if (sal_strcmp(flex_tunnel_type_arr[idx].field_str, "\0") == 0) {
           continue;
        }

        rv = (soc_cancun_ceh_obj_field_get(unit, "PKT_FLOW_ID_1",
                                           flex_tunnel_type_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META("tunnel_type:%d obj_str:%s field_str:%s\n\r"),
                                    idx, "PKT_FLOW_ID_1",
                                    flex_tunnel_type_arr[idx].field_str));
           continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("tunnel_type:%d obj_str:%s field_str:%s\n\r"),
                                    idx, "PKT_FLOW_ID_1",
                                    flex_tunnel_type_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                                   tcam_data, tcam_mask, hw_data, hw_mask));

        if ((hw_data == tcam_data) && (hw_mask == tcam_mask)) {
           *tunnel_type = idx;
           return BCM_E_NONE;
        }
    }

    switch (tcam_data & tcam_mask) {
        case 0x5:
            *tunnel_type = bcmFieldTunnelTypeIp;
            break;
        case 0x6:
            *tunnel_type = bcmFieldTunnelTypeMpls;
            break;
        case 0x2:
            *tunnel_type = bcmFieldTunnelTypeMim;
            break;
        case 0x3:
            *tunnel_type = bcmFieldTunnelTypeL2Gre;
            break;
        case 0x4:
            *tunnel_type = bcmFieldTunnelTypeVxlan;
            break;
        case 0x1:
            if(tcam_mask) {
                *tunnel_type = bcmFieldTunnelTypeNone;
                break;
            }
        default:
            return (BCM_E_INTERNAL);
    }

    return BCM_E_NONE;
}


int
_bcm_field_td3_qualify_LoopbackType(bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask)
{
    uint32 lbType = 1 << 4;

    switch (loopback_type) {
        case bcmFieldLoopbackTypeAny:
            *tcam_data = lbType | 6;
            *tcam_mask = 0xff;
            break;
        case bcmFieldLoopbackTypeTunnelAny:
            *tcam_data = lbType | 7;
            *tcam_mask = 0xff;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}

int
_bcm_field_td3_qualify_LoopbackType_get(uint8                    tcam_data,
                                        uint8                    tcam_mask,
                                        bcm_field_LoopbackType_t *loopback_type
                                        )
{
    switch (tcam_data & tcam_mask) {
        case 0x16:
            *loopback_type = bcmFieldLoopbackTypeAny;
            break;
        case 0x17:
            *loopback_type = bcmFieldLoopbackTypeTunnelAny;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

STATIC
_flex_ceh_field_type_t flex_loopback_subtype_arr[bcmFieldLoopbackSubtypeCount] = {
            {"LOOPBACK_SUBTYPE"}};

int
_bcm_field_td3_qualify_LoopbackSubtype(int unit,
                                       bcm_field_loopback_subtype_t subtype,
                                       uint32  *hw_data, uint32 *hw_mask)
{
    int rv = BCM_E_UNAVAIL;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "UDF_1_CHUNK_7",
                                       flex_loopback_subtype_arr[subtype].field_str,
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"),
                        subtype, "UDF_1_CHUNK_7",
                        flex_loopback_subtype_arr[subtype].field_str));
       /*
        * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
        * supported in the current CANCUN release or for this chip.
        */
       rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
       return rv;
    }

    *hw_mask = (1 << ceh_info.width) - 1;
    *hw_data = ceh_info.value;

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_LoopbackSubtype_get(int unit,
                                           uint8                    tcam_data,
                                           uint8                    tcam_mask,
                                           bcm_field_loopback_subtype_t *subtype)
{
    int idx;
    int rv;
    uint8 hw_data = 0x0;
    uint8 hw_mask = 0x0;
    soc_cancun_ceh_field_info_t ceh_info;

    for (idx = 0; idx < COUNTOF(flex_loopback_subtype_arr); idx++) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "UDF_1_CHUNK_7",
                                           flex_loopback_subtype_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"),
                       idx, "UDF_1_CHUNK_7",
                       flex_loopback_subtype_arr[idx].field_str));
           continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"),
                     idx, "UDF_1_CHUNK_7",
                     flex_loopback_subtype_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                                    tcam_data, tcam_mask, hw_data, hw_mask));

        if ((hw_data == tcam_data) && (hw_mask == tcam_mask)) {
           *subtype = idx;
           return BCM_E_NONE;
        }
    }

    return (BCM_E_PARAM);
}

/* PKT_FLOW types */
#define _FP_PKT_FLOW_0_FRONT_PANEL                       0x10
#define _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_ANY    0x12
#define _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_VXLAN  0x13
#define _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_NVGRE  0x14
#define _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_MIM    0x15

int
_bcm_field_td3_qualify_flow_type(int unit, int qual_id,
                                 uint8 data, uint8 mask,
                                 uint8 *tcam_data, uint8 *tcam_mask)
{
   if (data & ~(0x1)) {
      return BCM_E_PARAM;
   }

   switch (qual_id) {
      case bcmFieldQualifyFrontPanelPkt:
           *tcam_data = _FP_PKT_FLOW_0_FRONT_PANEL;
           *tcam_mask = 0xff;
           break;
      default:
           return BCM_E_PARAM;
   }

   if (data == 0) {
      *tcam_mask = *tcam_data;
      *tcam_data = 0x0;
   }

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_flow_type_get(int unit, int qual_id,
                                     uint8 tcam_data, uint8 tcam_mask,
                                     uint8 *data, uint8 *mask)
{
   switch (tcam_data & tcam_mask) {
      case _FP_PKT_FLOW_0_FRONT_PANEL:
           *data = 0x1;
           *mask = 0x1;
           break;
      case 0:
           *data = 0x0;
           *mask = 0x1;
      default:
           return BCM_E_PARAM;
   }

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_HiGigProxyTunnelType(bcm_field_higig_proxy_tunnel_type_t type,
                                            uint8 *tcam_data, uint8 *tcam_mask)
{
   switch (type) {
      case bcmFieldHiGigProxyTunnelTypeAny:
           *tcam_data = _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_ANY;
           *tcam_mask = 0xff;
           break;
      case bcmFieldHiGigProxyTunnelTypeVxlan:
           *tcam_data = _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_VXLAN;
           *tcam_mask = 0xff;
           break;
      case bcmFieldHiGigProxyTunnelTypeL2Gre:
           *tcam_data = _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_NVGRE;
           *tcam_mask = 0xff;
           break;
      case bcmFieldHiGigProxyTunnelTypeMim:
           *tcam_data = _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_MIM;
           *tcam_mask = 0xff;
           break;
      default:
           return BCM_E_PARAM;
   }

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_HiGigProxyTunnelType_get(uint8 tcam_data, uint8 tcam_mask,
                                       bcm_field_higig_proxy_tunnel_type_t *type)
{
   if (tcam_mask != 0xff) {
      return BCM_E_INTERNAL;
   }

   switch (tcam_data) {
      case _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_ANY:
           *type = bcmFieldHiGigProxyTunnelTypeAny;
           break;
      case _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_VXLAN:
           *type = bcmFieldHiGigProxyTunnelTypeVxlan;
           break;
      case _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_NVGRE:
           *type = bcmFieldHiGigProxyTunnelTypeL2Gre;
           break;
      case _FP_PKT_FLOW_0_SYSTEM_HDR_PROXY_LEAF_ONLY_MIM:
           *type = bcmFieldHiGigProxyTunnelTypeMim;
           break;
      default:
           return BCM_E_INTERNAL;
   }

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_InterfaceClassL2Type(bcm_field_interface_class_l2_type_t type,
                                            uint8 *tcam_data, uint8 *tcam_mask)
{
   switch (type) {
      case bcmFieldInterfaceClassL2TypeVlan:
           *tcam_data = 0;
           *tcam_mask = 1;
           break;
      case bcmFieldInterfaceClassL2TypeVfi:
           *tcam_data = 1;
           *tcam_mask = 1;
           break;
      default:
           return BCM_E_PARAM;
   }

   return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_InterfaceClassL2Type_get(uint8 tcam_data, uint8 tcam_mask,
                                       bcm_field_interface_class_l2_type_t *type)
{
   switch (tcam_data) {
      case 0:
           *type = bcmFieldInterfaceClassL2TypeVlan;
           break;
      case 1:
           *type = bcmFieldInterfaceClassL2TypeVfi;
           break;
      default:
           return BCM_E_INTERNAL;
   }

   return BCM_E_NONE;
}

STATIC
_flex_ceh_field_type_t flex_tunnel_subtype_arr[bcmFieldTunnelSubTypeCount] = {
            {"GPE_NSH_PYLD_L3"},
            {"L2_NSH_PYLD_L3"},
            {"MPLS_NSH_PYLD_L3"},
};

int
_bcm_field_td3_qualify_TunnelSubType(int unit,
                                     bcm_field_TunnelSubType_t subtype,
                                     uint32 *tcam_data,
                                     uint32 *tcam_mask)
{
    int rv = BCM_E_NONE;
    uint32 hw_data = 0x0;
    uint32 hw_mask = 0x0;
    soc_cancun_ceh_field_info_t ceh_info;

    switch (subtype) {
        case bcmFieldTunnelSubTypeUnknown:
            *tcam_data = 0;
            *tcam_mask = 0x3f;
            return BCM_E_NONE;
        default:
            break;
    }

    rv = (soc_cancun_ceh_obj_field_get(unit, "PKT_FLOW_ID_1",
                                       flex_tunnel_subtype_arr[subtype].field_str,
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"),
                        subtype, "PKT_FLOW_ID_1",
                        flex_tunnel_subtype_arr[subtype].field_str));
       /*
        * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
        * supported in the current CANCUN release or for this chip.
        */
       rv  = (BCM_E_NOT_FOUND == rv) ? (BCM_E_UNAVAIL) : rv ;
       return rv;
    }

    hw_mask = (1 << ceh_info.width) - 1;
    hw_data = ceh_info.value;

    *tcam_data = hw_data;
    *tcam_mask = hw_mask;
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Type:%d - offset:%d width:%d value:%d data:0x%x mask:0x%x\n\r"),
                             subtype, ceh_info.offset, ceh_info.width, ceh_info.value, hw_data, hw_mask));

    return (BCM_E_NONE);
}

int
_bcm_field_td3_qualify_TunnelSubType_get(int unit,
                                         uint8 tcam_data,
                                         uint8 tcam_mask,
                                         bcm_field_TunnelSubType_t *tunnel_type)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    uint8 hw_data = 0x0;
    uint8 hw_mask = 0x0;
    soc_cancun_ceh_field_info_t ceh_info;

    if ((0 == tcam_data) && (0x3f == tcam_mask)) {
        *tunnel_type = bcmFieldTunnelSubTypeUnknown;
        return BCM_E_NONE;
    }

    for (idx = 0; idx < COUNTOF(flex_tunnel_subtype_arr); idx++) {
        rv = (soc_cancun_ceh_obj_field_get(unit, "PKT_FLOW_ID_1",
                                           flex_tunnel_subtype_arr[idx].field_str,
                                           &ceh_info));
        /*
         * Ignore if CANCUN mapping was not found. Could be the Mapping is no longer
         * supported in the current CANCUN release or for this chip.
         */
        if (BCM_E_NOT_FOUND == rv) {
           LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"),
                       idx, "PKT_FLOW_ID_1",
                       flex_tunnel_subtype_arr[idx].field_str));
           continue;
        }
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("subtype:%d obj_str:%s field_str:%s\n\r"), idx, "PKT_FLOW_ID_1",
                   flex_tunnel_subtype_arr[idx].field_str));
           return rv;
        }

        hw_mask = (1 << ceh_info.width) - 1;
        hw_data = ceh_info.value;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "data:0x%x mask:0x%x hw_data:0x%x hw_mask:0x%x\n\r"),
                                    tcam_data, tcam_mask, hw_data, hw_mask));

        if ((hw_data == tcam_data) && (hw_mask == tcam_mask)) {
           *tunnel_type = idx;
           return BCM_E_NONE;
        }
    }

    return (BCM_E_PARAM);
}


int
_bcm_field_td3_qualify_VlanFormat(int unit,
                                  bcm_field_qualify_t qual_id,
                                  uint8 data,
                                  uint8 mask,
                                  uint8 *hw_data,
                                  uint8 *hw_mask)
{
    int rv;
    uint8 otag_val;
    uint8 itag_val;
    uint8 notag_val;
    uint8 otag_offset;
    uint8 itag_offset;
    uint8 otag_itag_offset;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                     "NO_OTAG_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "NO_OTAG_ITAG"));
       return rv;
    }
    notag_val = ceh_info.value;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "SINGLE_OTAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "OTAG"));
       return rv;
    }
    otag_val = ceh_info.value;
    otag_offset = ceh_info.offset;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "SINGLE_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "ITAG"));
       return rv;
    }
    itag_val = ceh_info.value;
    itag_offset = ceh_info.offset;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "OTAG_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "OTAG_ITAG"));
       return rv;
    }
    otag_itag_offset = ceh_info.offset;

    if (data == BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
       *hw_data = otag_val << (otag_offset - otag_itag_offset);
    } else if (data == BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
       *hw_data = itag_val << (itag_offset - otag_itag_offset);
    } else if (data == (BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED)) {
       *hw_data = ceh_info.value;
    } else if (data & ~(BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED)) {
       *hw_data = notag_val;
    }

    if (mask == BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
       *hw_mask = otag_val << (otag_offset - otag_itag_offset);
    } else if (mask == BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
       *hw_mask = itag_val << (itag_offset - otag_itag_offset);
    } else if (mask == (BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED)) {
       *hw_mask = (1 << ceh_info.width) - 1;
    } else {
       return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_VlanFormat_get(int unit,
                                  bcm_field_qualify_t qual_id,
                                  uint8 hw_data,
                                  uint8 hw_mask,
                                  uint8 *data,
                                  uint8 *mask)
{
    int rv;
    uint8 otag_val;
    uint8 itag_val;
    uint8 notag_val;
    uint8 otag_offset;
    uint8 itag_offset;
    uint8 otag_itag_offset;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                     "NO_OTAG_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "NO_OTAG_ITAG"));
       return rv;
    }
    notag_val = ceh_info.value;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "SINGLE_OTAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "OTAG"));
       return rv;
    }
    otag_val = ceh_info.value;
    otag_offset = ceh_info.offset;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "SINGLE_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "ITAG"));
       return rv;
    }
    itag_val = ceh_info.value;
    itag_offset = ceh_info.offset;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z1_MATCH_ID",
                                       "OTAG_ITAG", &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Error: obj_str:%s field_str:%s\n\r"), "Z1_MATCH_ID",
                  "OTAG_ITAG"));
       return rv;
    }
    otag_itag_offset = ceh_info.offset;

    if ((hw_data >> (otag_offset - otag_itag_offset)) == otag_val) {
       *data = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
    } else if ((hw_data >> (itag_offset - otag_itag_offset)) == itag_val) {
       *data = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    } else if (hw_data == notag_val) {
       *data = ~(BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                 BCM_FIELD_VLAN_FORMAT_INNER_TAGGED);
    } else if (hw_data == ceh_info.value) {
       *data = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
               BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    }

    if ((hw_mask >> (otag_offset - otag_itag_offset)) == otag_val) {
       *mask = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
    } else if ((hw_mask >> (itag_offset - otag_itag_offset)) == itag_val) {
       *mask = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
    } else if (hw_mask == ((1 << ceh_info.width) - 1)) {
       *mask = (BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED |
                BCM_FIELD_VLAN_FORMAT_INNER_TAGGED);
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_MplsType(int unit, bcm_field_qualify_t qual_id,
                                uint8 data, uint8 mask,
                                uint8 *hw_data, uint8 *hw_mask)
{
    int rv;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "MPLS",
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID",
               "MPLS"));
       return rv;
    }

    *hw_data = data << ceh_info.width;
    *hw_data |= ceh_info.value;

    *hw_mask = mask << ceh_info.width;
    *hw_mask |= ((1 << ceh_info.width) - 1);

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_MplsType_get(int unit, bcm_field_qualify_t qual_id,
                                    uint8 hw_data, uint8 hw_mask,
                                    uint8 *data, uint8 *mask)
{
    int rv;
    soc_cancun_ceh_field_info_t ceh_info;

    rv = (soc_cancun_ceh_obj_field_get(unit, "Z2_MATCH_ID",
                                       "MPLS",
                                       &ceh_info));
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "obj_str:%s field_str:%s\n\r"), "Z2_MATCH_ID",
               "MPLS"));
       return rv;
    }

    *data = hw_data >> ceh_info.width;
    *mask = hw_mask >> ceh_info.width;

    return BCM_E_NONE;
}

/*********************************** UDF Routines ***************************************/

/* UDF Chunk to Qualifier Mapping */
uint32 _bcm_udf_chunk_to_qual_mapping[_BCM_UDF_OFFSET_CHUNKS16] =
                        {
                           _bcmFieldQualifyData0,  /* Chunk 0  */
                           _bcmFieldQualifyData1,  /* Chunk 1  */
                           _bcmFieldQualifyData2,  /* Chunk 2  */
                           _bcmFieldQualifyData3,  /* Chunk 3  */
                           _bcmFieldQualifyData4,  /* Chunk 4  */
                           _bcmFieldQualifyData5,  /* Chunk 5  */
                           _bcmFieldQualifyData6,  /* Chunk 6  */
                           _bcmFieldQualifyData7,  /* Chunk 7  */
                           _bcmFieldQualifyData8,  /* Chunk 8  */
                           _bcmFieldQualifyData9,  /* Chunk 9  */
                           _bcmFieldQualifyData10, /* Chunk 10 */
                           _bcmFieldQualifyData11, /* Chunk 11 */
                           _bcmFieldQualifyData12, /* Chunk 12 */
                           _bcmFieldQualifyData13, /* Chunk 13 */
                           _bcmFieldQualifyData14, /* Chunk 14 */
                           _bcmFieldQualifyData15  /* Chunk 15 */
                        };

/*
 * Function:
 *      _bcm_field_td3_udf_chunks_to_int_qset
 * Purpose:
 *      Sets internal data qualifiers corresponding to the
 *      offset chunks used by the UDF.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      chunk_bmap     - (IN)  UDF offsets chunks map
 *      qset            - (OUT) Qualifier set
 * Returns:
 *      BCM_E_NONE
 * Notes:
 */
int
_bcm_field_td3_udf_chunks_to_int_qset(int unit, uint32 chunk_bmap,
                                      bcm_field_qset_t *qset)
{
    int idx, max_chunks;

    if (qset == NULL) {
       return BCM_E_PARAM;
    }

    max_chunks = _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit);

    for (idx = 0; idx < max_chunks; idx++) {
        if(chunk_bmap & (1 << idx)) {
            BCM_FIELD_QSET_ADD_INTERNAL(*qset,
                                        _bcm_udf_chunk_to_qual_mapping[idx]);
        }
    }
    return BCM_E_NONE;
}


STATIC
int _bcm_field_td3_qset_id_multi_set(
    int unit,
    bcm_field_qualify_t qualifier,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx;
    int rv = BCM_E_NONE;
    uint32 udf_chunks = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;


    if (qualifier != bcmFieldQualifyUdf) {
       return BCM_E_UNAVAIL;
    }

    if (qset == NULL) {
       return BCM_E_PARAM;
    }

    for (idx = 0; idx < num_objects; idx++) {
        /* Fetch udf info for hw_bmap */
        rv = _bcm_udf_td3_object_info_get(unit, objects_list[idx], &obj_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        udf_chunks |= obj_info->chunk_bmap;
    }

    /* Update udf_chuks bmap with qset */
    for (idx = 0; idx < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); idx++) {
        /* coverity[ptr_arith] */
        if (SHR_BITGET(qset->udf_map, idx)) {
            SHR_BITSET(&udf_chunks, idx);
        }
    }
    /* Update qset with internal data qualifiers based on udf_chunks */
    rv = _bcm_field_td3_udf_chunks_to_int_qset(unit, udf_chunks, qset);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* se ting udf_map helps in gets/deletes */
    for (idx = 0; idx < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); idx++) {
        /* coverity[ptr_arith] */
        if (SHR_BITGET(&udf_chunks, idx)) {
            SHR_BITSET(qset->udf_map, idx);
        }
    }

    return rv;
}

STATIC int
_bcm_field_td3_qset_id_multi_get(
    int unit,
    bcm_field_qset_t qset,
    bcm_field_qualify_t qualifier,
    int max,
    int *objects_list,
    int *actual)
{
    int idx = 0;
    uint32 udf_chunks = 0;


    if (qualifier != bcmFieldQualifyUdf) {
       return BCM_E_UNAVAIL;
    }

    if (((max > 0) && (objects_list == NULL)) || (actual == NULL)) {
       return BCM_E_PARAM;
    }

    for (idx = 0; idx < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); idx++) {
        /* coverity[ptr_arith] */
        if (SHR_BITGET(qset.udf_map, idx)) {
            SHR_BITSET(&udf_chunks, idx);
        }
    }

    return _bcm_udf_td3_chunk_id_multi_get(unit, udf_chunks,
                                           max, objects_list,
                                           actual);
}

STATIC int
_bcm_field_td3_qset_id_multi_delete(
    int unit,
    bcm_field_qualify_t qualifier,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx;
    int rv = BCM_E_NONE;
    uint32 udf_chunks = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;


    if (qualifier != bcmFieldQualifyUdf) {
       return BCM_E_UNAVAIL;
    }

    if (qset == NULL) {
       return BCM_E_PARAM;
    }

    for (idx = 0; idx < num_objects; idx++) {
        /* Fetch udf info for hw_bmap */
        rv = _bcm_udf_td3_object_info_get(unit, objects_list[idx], &obj_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        udf_chunks |= obj_info->chunk_bmap;
    }

    /* Update qset with internal data qualifiers based on udf_chunks */
    rv = _bcm_field_td3_udf_chunks_to_int_qset(unit, udf_chunks, qset);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* setting udf_map helps in gets/deletes */
    for (idx = 0; idx < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); idx++) {
        /* coverity[ptr_arith] */
        if (SHR_BITGET(&udf_chunks, idx)) {
            SHR_BITCLR(qset->udf_map, idx);
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcm_field_td3_qualify_udf
 * Purpose:
 *      Add UDF data that the packet must match to trigger qualifier.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      eid             - (IN)  Field Entry.
 *      udf_id          - (IN)  UDF ID.
 *      length          - (IN)  Number of bytes to match in field entry.
 *      data            - (IN)  Input data to qualify the entry.
 *      mask            - (IN)  Input mask to qualify the entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_field_td3_qualify_udf(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    int rv;
    int gran;
    int chunk;
    int bytes;
    int len = 0;
    int bits = 8;
    uint32 q_data, q_mask;
    int      size = 0;
    uint8    hints_present;
    uint8    pdata[32];
    uint8    pmask[32];
    uint32 chunk_bmap = 0;
    _bcm_field_internal_qualify_t qual;
    _field_entry_t *f_ent = NULL;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* Fetch udf info for hw_bmap */
    rv = _bcm_udf_td3_object_info_get(unit, udf_id, &obj_info);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    /* If provided entry data length is greater than UDF width */
    if ((length > obj_info->width) || (length <= 0)) {
        return (BCM_E_PARAM);
    }

    /* Chunk Granularity */
    gran = _BCM_UDF_CTRL_OFFSET_GRAN(unit);

    chunk_bmap = obj_info->chunk_bmap;

    /* Get entry info. */
    BCM_IF_ERROR_RETURN(_field_entry_get(unit, eid,
                       _FP_ENTRY_PRIMARY, &f_ent));

    sal_memset(pdata, 0, 32 * sizeof(uint8));
    sal_memset(pmask, 0, 32 * sizeof(uint8));
    sal_memcpy(pdata, data, length);
    sal_memcpy(pmask, mask, length);

    rv = _field_udf_hints_present_get(unit, eid, udf_id, &hints_present);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    size = hints_present ? length : obj_info->width;
    /*
     * If the size is odd number,
     * make it even by adding 1 to it as the pdata and pmask are already filled
     * with 0s at that size + 1 position.
     */
    size = (size == 1) ? (size + 1) : size;
    len = size - 1;
    /* Update udf_chuks bmap with qset */
    for (chunk = _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit) - 1; chunk >= 0; chunk--) {
        if(chunk_bmap & (1 << chunk)) {
            qual = _bcm_udf_chunk_to_qual_mapping[chunk];
            q_data = q_mask = 0;
            for (bytes = gran - 1; bytes >= 0; bytes--) {
                if (len < 0) {
                   break;
                }
                q_data |= pdata[len] << ((gran - 1 - bytes) * bits);
                q_mask |= pmask[len] << ((gran - 1 - bytes) * bits);
                len--;
            }

            if (f_ent->group->stage_id != _BCM_FIELD_STAGE_LOOKUP) {
               /* coverity[address_of : FALSE] */
               /* coverity[callee_ptr_arith : FALSE] */
               rv = _bcm_field_th_qualify_set(unit, eid, qual,
                                              &q_data, &q_mask,
                                              _FP_QUALIFIER_ADD);
               if ((BCM_E_PARAM == rv) && hints_present) {
                   if (!BCM_FIELD_QSET_TEST_INTERNAL(f_ent->group->qset, qual)) {
                       rv = BCM_E_NONE;
                   }
               }
            } else {
               rv = _field_qualify32(unit, eid, qual, q_data, q_mask);
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_field_td3_qualify_udf_get(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    int rv;
    int gran;
    int chunk;
    int bytes;
    int len = 0;
    int bits = 8;
    uint8    hints_present;
    uint32 q_data, q_mask;
    uint32 chunk_bmap = 0;
    _bcm_field_internal_qualify_t qual;
    _field_entry_t *f_ent = NULL;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* Input parameters check. */
    if ((NULL == data) || (NULL == mask) ||
        (NULL == actual_length) || (max_length == 0)) {
        return (BCM_E_PARAM);
    }

    /* Fetch udf info for hw_bmap */
    rv = _bcm_udf_td3_object_info_get(unit, udf_id, &obj_info);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    /*
     * If provided entry data length is less than data length used
     * to create qualifier then return error
     */
    if (max_length < obj_info->width) {
        return (BCM_E_PARAM);
    }


    /* Chunk Granularity */
    gran = _BCM_UDF_CTRL_OFFSET_GRAN(unit);

    chunk_bmap = obj_info->chunk_bmap;

    /* Get entry info. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_get_by_id(unit, eid, &f_ent));


    rv = _field_udf_hints_present_get(unit, eid, udf_id, &hints_present);
    BCM_IF_ERROR_RETURN(rv);

    /* Update udf_chuks bmap with qset */
    for (chunk = 0; chunk < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); chunk++) {
        if(chunk_bmap & (1 << chunk)) {
            qual = _bcm_udf_chunk_to_qual_mapping[chunk];
            q_data = q_mask = 0;
            rv = _bcm_field_entry_qualifier_uint32_get(unit, eid, qual,
                                                       &q_data, &q_mask);
            if ((BCM_E_NOT_FOUND == rv) && hints_present) {
                if (!BCM_FIELD_QSET_TEST_INTERNAL(f_ent->group->qset, qual)) {
                    rv = BCM_E_NONE;
                }
            }
            BCM_IF_ERROR_RETURN(rv);

            for (bytes = 0; bytes < gran; bytes++) {
                if (len >= max_length) {
                   break;
                }

                data[len] = q_data >> ((gran - 1 - bytes) * bits);
                mask[len] = q_mask >> ((gran - 1 - bytes) * bits);
                len++;
            }
        }
    }
    *actual_length = len;

    return BCM_E_NONE;
}

int
_field_td3_flex_action_set(int unit,
                           _field_action_t *fa,
                           _bcm_field_action_offset_t *a_offset)
{
    int               rv1 = BCM_E_UNAVAIL;
    int               rv2 = BCM_E_UNAVAIL;
    int                         valid = 0;
    char                   *object = NULL;
    char              *object_mask = NULL;
    soc_cancun_ceh_field_info_t ceh_info1;
    soc_cancun_ceh_field_info_t ceh_info2;

    switch (fa->action) {
        case bcmFieldActionEgressFlowControlEnable:
            valid = 0;
            switch (fa->param[0]) {
                case bcmFieldEgressFlowControlLoopbackRedirect:
                     object = "REDIRECT_TO_DEST";
                     object_mask = "REDIRECT_TO_DEST_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlAppendIncomingOuterVlan:
                     object = "PRESERVE_INCOMING_OTAG";
                     object_mask = "PRESERVE_INCOMING_OTAG_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlAppendOuterVlanNew:
                     object = "ASSIGN_PRESERVE_OTAG";
                     object_mask = "ASSIGN_PRESERVE_OTAG_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateSnatSrcIpOnly:
                     object = "SIP_ONLY_TRANSLATE";
                     object_mask = "SIP_ONLY_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateDnatSrcIpDstIp:
                     object = "SIP_DIP_TRANSLATE";
                     object_mask = "SIP_DIP_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateSnatSrcIpDstIp:
                     object = "SIP_DIP_TRANSLATE_SNAT";
                     object_mask = "SIP_DIP_TRANSLATE_SNAT_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateSnaptSrcIpL4SrcPort:
                     object = "SIP_L4SRC_PORT_TRANSLATE";
                     object_mask = "SIP_L4SRC_PORT_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateSnaptSrcIpDstIpL4SrcPortL4DstPort:
                     object = "SIP_L4SRC_PORT_DIP_L4DST_PORT_TRANSLATE_SNAT";
                     object_mask = "SIP_L4SRC_PORT_DIP_L4DST_PORT_TRANSLATE_SNAT_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateDnaptSrcIpDstIpL4SrcPortL4DstPort:
                     object = "SIP_L4SRC_PORT_DIP_L4DST_PORT_TRANSLATE";
                     object_mask = "SIP_L4SRC_PORT_DIP_L4DST_PORT_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateDnatDstIpOnly:
                     object = "DIP_ONLY_TRANSLATE";
                     object_mask = "DIP_ONLY_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateDnaptDstIpL4DstPort:
                     object = "DIP_L4DST_PORT_TRANSLATE";
                     object_mask = "DIP_L4DST_PORT_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTranslateIpmc:
                     object = "IPMC_TRANSLATE";
                     object_mask = "IPMC_TRANSLATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlSrcMacDstMacVlanNew:
                     object = "CHANGE_L2_FIELDS_DA_SA_VLAN";
                     object_mask = "CHANGE_L2_FIELDS_DA_SA_VLAN_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlDstMacVlanNew:
                     object = "CHANGE_L2_FIELDS_DA_VLAN";
                     object_mask = "CHANGE_L2_FIELDS_DA_VLAN_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlSrcMacVlanNew:
                     object = "CHANGE_L2_FIELDS_SA_VLAN";
                     object_mask = "CHANGE_L2_FIELDS_SA_VLAN_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlVlanNew:
                     object = "CHANGE_L2_FIELDS_VLAN";
                     object_mask = "CHANGE_L2_FIELDS_VLAN_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlIfaCopyToCpuWithMetadata:
                     object = "IFA_COPY_TO_CPU_WITH_METADATA";
                     object_mask = "IFA_COPY_TO_CPU_WITH_METADATA_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlIfaLastNodeWithMetadata:
                     object = "IFA_LAST_NODE_WITH_METADATA";
                     object_mask = "IFA_LAST_NODE_WITH_METADATA_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlIfaProbeSecondPass:
                     object = "IFA_PROBE_SECOND_PASS";
                     object_mask = "IFA_PROBE_SECOND_PASS_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlTunnelDecap:
                     object = "TUNNEL_DECAP";
                     object_mask = "TUNNEL_DECAP_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlNshInsert:
                     object = "INSERT_NSH_HDR";
                     object_mask = "INSERT_NSH_HDR_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlNshTerminate:
                     object = "NSH_TERMINATION";
                     object_mask = "NSH_TERMINATION_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlNshTermMdClassPayload:
                     object = "NSH_TERMINATION_MC_PAYLOAD";
                     object_mask = "NSH_TERMINATION_MC_PAYLOAD_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowControlIfaHdrInsertionOverTrunk:
                     object = "IFA_HEADER_INSERTION_OVER_LAG";
                     object_mask = "IFA_HEADER_INSERTION_OVER_LAG_MASK";
                     valid = 1;
                     break;
                default:
                    break;
            }
            if (valid)  {
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                                        object, &ceh_info1);
                    rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                                        object_mask, &ceh_info2);
                    if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    a_offset->value[0] |= ((ceh_info2.value) << ceh_info2.offset);
            }
            break;
        case bcmFieldActionEgressFlowEncapEnable:
            valid = 0;
            switch (fa->param[0]) {
                case bcmFieldEgressFlowEncapAppendIncomingOuterVlan:
                     object = "PRESERVE_INCOMING_OTAG";
                     object_mask = "PRESERVE_INCOMING_OTAG_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapTwampOwampTxTimestamp:
                     object = "TWAMP_OWAMP_TS";
                     object_mask = "TWAMP_OWAMP_TS_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIOAMEnable:
                     object = "IOAM_ENABLE";
                     object_mask = "IOAM_ENABLE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIntReply:
                     object = "INT_REPLY";
                     object_mask = "INT_REPLY_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIOAMOverflowFlagSet:
                     object = "IOAM_OVERFLOW_BIT_SET";
                     object_mask = "IOAM_OVERFLOW_BIT_SET_MASK";
                     valid = 1;
                     break;
#if defined(BCM_HURRICANE4_SUPPORT)
#if defined(INCLUDE_XFLOW_MACSEC)
                case bcmFieldEgressFlowEncapMacsecEncryptRxControlPktSvtagAdd:
                     object = "MACSEC_ENCRYPT_RX_ADD_SVTAG_FOR_CONTROL_PKT";
                     object_mask = "MACSEC_ENCRYPT_RX_ADD_SVTAG_FOR_CONTROL_PKT_MASK";
                     valid = 1;
                     break;
#endif
                case bcmFieldEgressFlowEncapIOAME2EEnable:
                     object = "ADD_IOAM_E2E";
                     object_mask = "ADD_IOAM_E2E_MASK";
                     valid = 1;
                     break;
#endif
                case bcmFieldEgressFlowEncapIfaCopyToCpuWithMetadata:
                     object = "IFA_COPY_TO_CPU_WITH_METADATA";
                     object_mask = "IFA_COPY_TO_CPU_WITH_METADATA_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIfaUdpHdrUpdate:
                     object = "IFA_UDP_HDR_UPDATE";
                     object_mask = "IFA_UDP_HDR_UPDATE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapTunnelDecap:
                     object = "TUNNEL_DECAP";
                     object_mask = "TUNNEL_DECAP_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapNshMdTypeEqualToOne:
                     object = "NSH_MD_TYPE_EQ_TO_ONE";
                     object_mask = "NSH_MD_TYPE_EQ_TO_ONE_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapNshMdTypeEqualToTwo:
                     object = "NSH_MD_TYPE_EQ_TO_TWO";
                     object_mask = "NSH_MD_TYPE_EQ_TO_TWO_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIfaCopyToCpuWithMetadataOverTrunk:
                     object = "IFA_COPY_TO_CPU_WITH_METADATA_OVER_LAG";
                     object_mask = "IFA_COPY_TO_CPU_WITH_METADATA_OVER_LAG_MASK";
                     valid = 1;
                     break;
                case bcmFieldEgressFlowEncapIfaOverTrunkEnable:
                     object = "IFA_ENABLE_OVER_LAG";
                     object_mask = "IFA_ENABLE_OVER_LAG_MASK";
                     valid = 1;
                     break;
                default:
                    break;
            }
            if (valid)  {
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID",
                                                        object, &ceh_info1);
                    rv2 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID",
                                                        object_mask, &ceh_info2);
                    if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    a_offset->value[0] |= ((ceh_info2.value) << ceh_info2.offset);
            }
            break;
        case bcmFieldActionAssignChangeL2FieldsClassId:
            rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_3",
                                               "CHANGE_L2_FIELDS_EDIT_INDEX",
                                               &ceh_info1);
            if (SOC_FAILURE(rv1)) {
                return BCM_E_UNAVAIL;
            }
            a_offset->value[0] = fa->param[0];
            break;
        case bcmFieldActionLoopbackSubtype:
            switch (fa->param[0]) {
                case bcmFieldLoopbackSubtypeIfa:
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_3",
                                                       "IFA_LOOPBACK_SUBTYPE",
                                                        &ceh_info1);
                    if (SOC_FAILURE(rv1)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    break;
                default:
                    break;
            }
            break;
        case bcmFieldActionEncapIfaMetadataHdr:
            rv1 = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_4",
                                               "ASSIGN_IFA_AND_MD_HDR",
                                               &ceh_info1);
            if (SOC_FAILURE(rv1)) {
                return BCM_E_UNAVAIL;
            }
            a_offset->value[0] = fa->param[0];
            break;
        case bcmFieldActionLoopbackType:
            switch (fa->param[0]) {
                case bcmFieldLoopbackTypeRedirect:
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "LOOPBACK_PROFILE_PTR",
                                                       "REDIRECT_TO_DEST",
                                                        &ceh_info1);
                    if (SOC_FAILURE(rv1)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    break;
                case bcmFieldLoopbackTypeMasquerade:
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "LOOPBACK_PROFILE_PTR",
                                                       "CPU_MASQUERADE",
                                                        &ceh_info1);
                    if (SOC_FAILURE(rv1)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    break;
                case bcmFieldLoopbackTypeMasqueradeWithSwitchDropIndication:
                    rv1 = soc_cancun_ceh_obj_field_get(unit, "LOOPBACK_PROFILE_PTR",
                                                       "CPU_MASQUERADE_WITH_DROP_INDICATION",
                                                        &ceh_info1);
                    if (SOC_FAILURE(rv1)) {
                        return BCM_E_UNAVAIL;
                    }
                    a_offset->value[0] = ceh_info1.value;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_internal_action_profile_data_set(int unit,
                                                _field_entry_t *f_ent,
                                                _field_action_t *fa,
                                                _field_em_action_set_t *acts_buf)
{
    int                         rv, rv1, rv2, val;
    uint8                       field_action_set = 0;
    _field_stage_t              *stage_fc;
    _field_em_action_set_t      *edit_acts = NULL;
    _field_em_action_set_t      *ext_acts = NULL;
    _field_em_action_set_t      *opaque_acts = NULL;
    _field_em_action_set_t      *opaque_acts1 = NULL;
    _bcm_field_action_conf_t    *action_conf_ptr;
    _bcm_field_action_offset_t  edit_action_offset;
    _bcm_field_action_offset_t  ext_action_offset;
    _bcm_field_action_offset_t  opaque_action_offset;
    _bcm_field_action_offset_t  opaque_action_offset1;
    soc_cancun_ceh_field_info_t ceh_info1;
    soc_cancun_ceh_field_info_t ceh_info2;

    if ((fa->action != bcmFieldActionIntEncapEnable) &&
        (fa->action != bcmFieldActionIntTurnAround) &&
        (fa->action != bcmFieldActionIntEncapUpdate) &&
        (fa->action != bcmFieldActionAddIngOuterVlanToEgrOuterVlan) &&
        (fa->action != bcmFieldActionErspan3HdrVlanCosPktCopy) &&
        (fa->action != bcmFieldActionGbpClassifierAdd) &&
        (fa->action != bcmFieldActionGbpSrcMacMcastBitSet) &&
        (fa->action != bcmFieldActionGbpSrcIdNew) &&
        (fa->action != bcmFieldActionGbpDstIdNew) &&
        (fa->action != bcmFieldActionAssignNatClassId) &&
        (fa->action != bcmFieldActionErspan3HdrGbpSrcIdAdd) &&
        (fa->action != bcmFieldActionErspan3HdrUdf2Add) &&
        (fa->action != bcmFieldActionNshEncapEnable) &&
        (fa->action != bcmFieldActionNshServicePathId) &&
        (fa->action != bcmFieldActionNshServiceIndex)) {
        return BCM_E_NONE;
    }

    /* Get stage control structure. */
    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    if ((fa->action == bcmFieldActionIntEncapEnable) ||
        (fa->action == bcmFieldActionIntTurnAround) ||
        (fa->action == bcmFieldActionNshEncapEnable)) {
       /* Get action conf ptr from stage structure for EditCtrlId */
       action_conf_ptr = stage_fc->f_action_arr[_bcmFieldActionEditCtrlId];
       if (NULL != action_conf_ptr) {
          /* get the aset enum from action_conf_pointer */
          field_action_set = action_conf_ptr->offset->aset;
       } else {
          return BCM_E_INTERNAL;
       }

       /* Set Action Set Buffer. */
       edit_acts = &acts_buf[field_action_set];

       rv = _bcm_field_action_offset_get(unit, stage_fc, _bcmFieldActionEditCtrlId, &edit_action_offset, 0);
       BCM_IF_ERROR_RETURN(rv);
    }

    if ((fa->action == bcmFieldActionIntEncapEnable) ||
        (fa->action == bcmFieldActionIntEncapUpdate) ||
        (fa->action == bcmFieldActionAddIngOuterVlanToEgrOuterVlan) ||
        (fa->action == bcmFieldActionErspan3HdrVlanCosPktCopy) ||
        (fa->action == bcmFieldActionGbpClassifierAdd) ||
        (fa->action == bcmFieldActionGbpSrcMacMcastBitSet) ||
        (fa->action == bcmFieldActionNshEncapEnable)) {
       action_conf_ptr = stage_fc->f_action_arr[_bcmFieldActionExtractionCtrlId];
       if (NULL != action_conf_ptr) {
          /* get the aset enum from action_conf_pointer */
          field_action_set = action_conf_ptr->offset->aset;
       } else {
          return BCM_E_INTERNAL;
       }

       /* Set Action Set Buffer. */
       ext_acts = &acts_buf[field_action_set];

       rv = _bcm_field_action_offset_get(unit, stage_fc, _bcmFieldActionExtractionCtrlId, &ext_action_offset, 0);
       BCM_IF_ERROR_RETURN(rv);
    }

    if ((fa->action == bcmFieldActionErspan3HdrGbpSrcIdAdd) ||
        (fa->action == bcmFieldActionErspan3HdrUdf2Add) ||
        (fa->action == bcmFieldActionAssignNatClassId) ||
        (fa->action == bcmFieldActionGbpSrcIdNew) ||
        (fa->action == bcmFieldActionGbpDstIdNew) ||
        (fa->action == bcmFieldActionNshServiceIndex)) {
       int _action;

       if (bcmFieldActionErspan3HdrGbpSrcIdAdd == fa->action) {
          _action = _bcmFieldActionOpaque3;
       } else if ((fa->action == bcmFieldActionErspan3HdrUdf2Add) ||
                  (bcmFieldActionAssignNatClassId == fa->action) ||
                  (bcmFieldActionGbpSrcIdNew == fa->action)) {
          _action = _bcmFieldActionOpaque1;
       } else {
          _action = _bcmFieldActionOpaque2;
       }

       action_conf_ptr = stage_fc->f_action_arr[_action];
       if (NULL != action_conf_ptr) {
          /* get the aset enum from action_conf_pointer */
          field_action_set = action_conf_ptr->offset->aset;
       } else {
          return BCM_E_INTERNAL;
       }

       /* Set Action Set Buffer. */
       opaque_acts = &acts_buf[field_action_set];

       rv = _bcm_field_action_offset_get(unit, stage_fc, _action, &opaque_action_offset, 0);
       BCM_IF_ERROR_RETURN(rv);

    }

    if (fa->action == bcmFieldActionNshServicePathId) {
       int _action;

       _action = _bcmFieldActionOpaque2;
       action_conf_ptr = stage_fc->f_action_arr[_action];
       if (NULL != action_conf_ptr) {
          /* get the aset enum from action_conf_pointer */
          field_action_set = action_conf_ptr->offset->aset;
       } else {
          return BCM_E_INTERNAL;
       }

       /* Set Action Set Buffer. */
       opaque_acts = &acts_buf[field_action_set];

       rv = _bcm_field_action_offset_get(unit, stage_fc, _action, &opaque_action_offset, 0);
       BCM_IF_ERROR_RETURN(rv);


       _action = _bcmFieldActionOpaque1;
       action_conf_ptr = stage_fc->f_action_arr[_action];
       if (NULL != action_conf_ptr) {
          /* get the aset enum from action_conf_pointer */
          field_action_set = action_conf_ptr->offset->aset;
       } else {
          return BCM_E_INTERNAL;
       }

       /* Set Action Set Buffer. */
       opaque_acts1 = &acts_buf[field_action_set];

       rv = _bcm_field_action_offset_get(unit, stage_fc, _action, &opaque_action_offset1, 0);
       BCM_IF_ERROR_RETURN(rv);
    }

    switch (fa->action) {
       case bcmFieldActionIntEncapEnable:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_ENABLE",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_ENABLE_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           edit_action_offset.value[0] = ceh_info1.value;
           edit_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, edit_acts->data, &edit_action_offset);
           edit_acts->valid = 1;

           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionIntEncapUpdate:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "INT_ENABLE_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionIntTurnAround:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_TURNAROUND",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "INT_TURNAROUND_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           edit_action_offset.value[0] = ceh_info1.value;
           edit_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, edit_acts->data, &edit_action_offset);
           edit_acts->valid = 1;
           break;

       case bcmFieldActionAddIngOuterVlanToEgrOuterVlan:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "PRESERVE_INCOMING_OTAG", &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "PRESERVE_INCOMING_OTAG_MASK", &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
               if ((BCM_E_NOT_FOUND == rv1) && (BCM_E_NOT_FOUND == rv2)) {
                   rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                                      "ADD_ING_OTAG_TO_EGR_OTAG",
                                                      &ceh_info1);
                   rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                                      "ADD_ING_OTAG_TO_EGR_OTAG_MASK",
                                                      &ceh_info2);
               }
               if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
                   return BCM_E_INTERNAL;
               }
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionErspan3HdrVlanCosPktCopy:
           ext_action_offset.value[0] = _BCM_FIELD_EXTR_CTRL_ID_ERSPAN3_VLAN_PKT_COPY;
           ext_action_offset.value[0] |= 0xff00;
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionGbpSrcMacMcastBitSet:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "GBP_SID_DID_CHECK", &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "GBP_SID_DID_CHECK_MASK", &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionGbpClassifierAdd:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "GBP_PRESENT_IN_PKT", &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID",
                                              "GBP_PRESENT_IN_PKT_MASK", &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;

       case bcmFieldActionErspan3HdrGbpSrcIdAdd:
       case bcmFieldActionErspan3HdrUdf2Add:
       case bcmFieldActionAssignNatClassId:
       case bcmFieldActionGbpDstIdNew:
       case bcmFieldActionGbpSrcIdNew:
           opaque_action_offset.value[0] = fa->param[0];
           ACTION_SET(unit, f_ent, opaque_acts->data, &opaque_action_offset);
           opaque_acts->valid = 1;
           break;
       case bcmFieldActionNshEncapEnable:
           rv1 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "ADD_NSH",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EDIT_CTRL_ID", "ADD_NSH_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           edit_action_offset.value[0] = ceh_info1.value;
           edit_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, edit_acts->data, &edit_action_offset);
           edit_acts->valid = 1;

           rv1 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "SPID_SI_LOOKUP_ENABLE",
                                              &ceh_info1);
           rv2 = soc_cancun_ceh_obj_field_get(unit, "EXTRACTION_CTRL_ID", "SPID_SI_LOOKUP_ENABLE_MASK",
                                              &ceh_info2);
           if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
              return BCM_E_INTERNAL;
           }
           ext_action_offset.value[0] = ceh_info1.value;
           ext_action_offset.value[0] |= ((ceh_info2.value) << ceh_info2.offset);
           ACTION_SET(unit, f_ent, ext_acts->data, &ext_action_offset);
           ext_acts->valid = 1;
           break;
       case bcmFieldActionNshServiceIndex:
           rv = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_2", "SI",
                                              &ceh_info1);
           BCM_IF_ERROR_RETURN(rv);
           opaque_action_offset.width[0] = ceh_info1.width;
           opaque_action_offset.value[0] = fa->param[0];
           ACTION_SET(unit, f_ent, opaque_acts->data, &opaque_action_offset);
           opaque_acts->valid = 1;
           break;
       case bcmFieldActionNshServicePathId:
           rv = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_2", "SPID_7_0",
                                              &ceh_info1);
           BCM_IF_ERROR_RETURN(rv);
           opaque_action_offset.offset[0] = ceh_info1.offset;
           opaque_action_offset.width[0] = ceh_info1.width;
           val = ((1 << opaque_action_offset.width[0]) - 1) & fa->param[0];
           opaque_action_offset.value[0] = val;

           rv = soc_cancun_ceh_obj_field_get(unit, "OPAQUE_1", "SPID_23_8",
                                              &ceh_info1);
           BCM_IF_ERROR_RETURN(rv);
           opaque_action_offset1.offset[0] = ceh_info1.offset;
           opaque_action_offset1.width[0] = ceh_info1.width;
           val = (fa->param[0] >> opaque_action_offset.width[0]);
           opaque_action_offset1.value[0] = val;

           ACTION_SET(unit, f_ent, opaque_acts->data, &opaque_action_offset);
           opaque_acts->valid = 1;
           ACTION_SET(unit, f_ent, opaque_acts1->data, &opaque_action_offset1);
           opaque_acts1->valid = 1;
           break;

       default:
           return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_td3_em_udf_keygen_mask_get
 * Purpose:
 *  Get the mask to be applied for data qualifiers.
 * Parameters:
 *  unit         - (IN)     BCM device number.
 *  fg           - (IN)     Field group structure.
 *  qid          - (IN)     Qualifier number.
 *  mask         - (OUT)    Mask to be applied.
 * Returns:
 *  BCM_E_XXX
 */
int _bcm_field_td3_em_udf_keygen_mask_get(int unit,
                                         _field_group_t *fg,
                                         uint16 qid, uint32 *mask)
{
    uint8 chunk;
    uint16 qual;
    _field_hints_t        *f_ht = NULL;      /* Field hints Structure. */
    _field_hint_t         *hint_node = NULL; /* Field hint Structure. */

    /* Parameters Check. */
    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    /* Update udf_chuks bmap with qset */
    for (chunk = 0; chunk < _BCM_UDF_CTRL_MAX_UDF_CHUNKS(unit); chunk++) {
        qual = _bcm_udf_chunk_to_qual_mapping[chunk];
        if (qid == qual) {
            if (!fg->hintid) {
                /* coverity[ptr_arith] */
                if (SHR_BITGET(fg->qset.udf_map, chunk)) {
                    *mask |= 0xFFFF;
                }
            } else {
                if (!SHR_BITGET(fg->qset.udf_map, chunk)) {
                    return BCM_E_NONE;
                }
                BCM_IF_ERROR_RETURN(_field_hints_control_get (unit, fg->hintid, &f_ht));
                /* Not a valid hint Id. */
                if (NULL == f_ht) {
                    *mask |= 0xFFFF;
                    return BCM_E_NONE;
                }
                /* No hints are attached to the hintid. */
                if (NULL == f_ht->hints) {
                    *mask |= 0xFFFF;
                    return BCM_E_NONE;
                }
                /* Go through all the hints in the hintid. */
                hint_node = f_ht->hints;
                while (NULL != hint_node) {
                    if (bcmFieldHintTypeExtraction == hint_node->hint->hint_type) {
                        BCM_IF_ERROR_RETURN(_bcm_th_qual_hint_bmp_get(unit, qual, 1, hint_node, mask,
                                                                  0, NULL, NULL));
                    }
                    hint_node = hint_node->next;
                }
            }
            if (0 == *mask) {
                *mask |= 0xFFFF;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_td3_ifp_inports_entry_tcam_install
 * Purpose:
 *     Set the per-pipe PBMP in the Ingress TCAM
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     tcam_idx     - (IN) Index in the TCAM memory.
 *     _f_pbmp      - (IN) Reference to field PBMP.
 *     num_pipe     - (IN) Number of Pipes the _f_pbmp has.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_td3_ifp_inports_entry_tcam_install(int unit, _field_entry_t *f_ent,
                                              int tcam_idx, uint8 set_pbmp,
                                             _field_pbmp_t *_f_pbmp,
                                              int num_pipe)
{
    uint32         port_class = 0;
    bcm_module_t   local_mod = 0;
    bcm_gport_t    gport = -1;
    int            rv = BCM_E_NONE;
    int            inst = -1;
    int            valid = 0;
    char           buf_ipbm[100];
    uint32         entry_buf[_BCM_FP_MEM_FIELD_BYTES] = {0};
    _field_tcam_t  *tcam = NULL;
    soc_mem_t      ifp_tcam_wide = INVALIDm;
    static uint32 pbm_34_49[2];
    static uint32 pbm_50_65[2];
    static uint32 pbm_34_49_mask[2];
    static uint32 pbm_50_65_mask[2];
    static bcm_pbmp_t ipbm_data[2];
    static bcm_pbmp_t ipbm_mask[2];
    static uint8  no_pbm_access = 0;

    if (f_ent == NULL || _f_pbmp == NULL) {
       return BCM_E_PARAM;
    }

    /* Update the valid bit based on the mode and group flags */
    valid = (f_ent->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 0;

    tcam = &f_ent->tcam;

    if (no_pbm_access == 0) {
       uint8       set_data = 0;
       bcm_port_t  port = 0;
       bcm_port_t  temp_p = 0;
       uint8 ipbm_hit, opq1_hit, opq2_hit;

       for (inst = 0; inst < num_pipe; inst++) {
          pbm_34_49[inst] = 0;
          pbm_50_65[inst] = 0;
          pbm_34_49_mask[inst] = 0;
          pbm_50_65_mask[inst] = 0;
          BCM_PBMP_CLEAR(ipbm_data[inst]);
          BCM_PBMP_CLEAR(ipbm_mask[inst]);
       }

       for (inst = 0; inst < num_pipe; inst++) {
          BCM_PBMP_ITER(_f_pbmp[inst].mask, port) {
              ipbm_hit = opq1_hit = opq2_hit = 0;

              if (BCM_PBMP_MEMBER(_f_pbmp[inst].data, port)) {
                 set_data = 1;
              }

              BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_mod));
              BCM_GPORT_MODPORT_SET(gport, local_mod,
                      (inst == 0) ? port : _BCM_TD3_NUM_PIPE_PORTS + port);

              rv = (bcm_esw_port_class_get(unit,
                          gport,
                          bcmPortClassFieldIngressSystemPort,
                          &port_class));
              if (BCM_E_PORT == rv) {
                  continue;
              }

              if (port_class < _FP_TD3_TCAM_IPBMP_SIZE) {
                 BCM_PBMP_PORT_ADD(ipbm_mask[inst], port_class);

                 if (set_data == 1) {
                    BCM_PBMP_PORT_ADD(ipbm_data[inst], port_class);
                 }
                 ipbm_hit = 1;
              } else if (port_class < (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE)) {
                 temp_p = port_class - _FP_TD3_TCAM_IPBMP_SIZE;

                 if (set_data == 1) {
                    pbm_34_49[inst] |= 1 << temp_p;
                 }

                 pbm_34_49_mask[inst] |= 1 << temp_p;
                 opq1_hit = 1;
              } else if (port_class < (_FP_TD3_TCAM_IPBMP_SIZE + (_FP_FLEX_PBMP_SIZE * 2))) {
                 temp_p = port_class - (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE);

                 if (set_data == 1) {
                    pbm_50_65[inst] |= 1 << temp_p;
                 }

                 pbm_50_65_mask[inst] |= 1 << temp_p;
                 opq2_hit=1;
              }

              LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                  "INST:%d port:%d temp_p:%d dev_port:%d ipbm_hit:%d opq1_hit:%d"
                  " opq2_hit:%d set_data:%d\n\r"),
                  inst, port, temp_p, port+66, ipbm_hit, opq1_hit, opq2_hit, set_data));
              set_data = 0;
          }

          LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                 "INSTANCE:%d ipbm_data:%s ipbm_mask:%s pbm_34_49:0x%x "
                 "pbm_34_49_mask:0x%x pbm_50_65:0x%x pbm_50_65_mask:0x%x\n\r"),
                 inst, SOC_PBMP_FMT(ipbm_data[inst], buf_ipbm),
                 SOC_PBMP_FMT(ipbm_mask[inst], buf_ipbm),
                 pbm_34_49[inst], pbm_34_49_mask[inst], pbm_50_65[inst],
                 pbm_50_65_mask[inst]));

          if (soc_feature(unit, soc_feature_xy_tcam_x0y)) {
             BCM_PBMP_XOR(ipbm_mask[inst], ipbm_data[inst]);
             BCM_PBMP_CLEAR(ipbm_data[inst]);

             pbm_34_49_mask[inst] ^= pbm_34_49[inst];
             pbm_34_49[inst] = 0;

             pbm_50_65_mask[inst] ^= pbm_50_65[inst];
             pbm_50_65[inst] = 0;

            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
              "INSTANCE:%d ipbm_data:%s ipbm_mask:%s pbm_34_49:0x%x "
              "pbm_34_49_mask:0x%x pbm_50_65:0x%x pbm_50_65_mask:0x%x\n\r"),
              inst, SOC_PBMP_FMT(ipbm_data[inst], buf_ipbm),
              SOC_PBMP_FMT(ipbm_mask[inst], buf_ipbm),
              pbm_34_49[inst], pbm_34_49_mask[inst], pbm_50_65[inst],
              pbm_50_65_mask[inst]));
          }
       }
       no_pbm_access = 1;
    }

    for (inst = 0; inst < num_pipe; inst++) {
        rv = _bcm_field_th_qualify_set(unit, f_ent->eid,
                                   _bcmFieldQualifyInPorts_1,
                                   &pbm_34_49[inst],
                                   &pbm_34_49_mask[inst],
                                   _FP_QUALIFIER_ADD);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_field_th_qualify_set(unit, f_ent->eid,
                                   _bcmFieldQualifyInPorts_2,
                                   &pbm_50_65[inst],
                                   &pbm_50_65_mask[inst],
                                   _FP_QUALIFIER_ADD);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                              IFP_TCAM_WIDEm,
                                              inst,
                                              &ifp_tcam_wide));

        sal_memset(entry_buf, 0x0, sizeof(entry_buf));
        soc_mem_field_set(unit, ifp_tcam_wide, entry_buf, KEYf, tcam->key);
        soc_mem_field_set(unit, ifp_tcam_wide, entry_buf, MASKf, tcam->mask);

        if (set_pbmp == 1) {
            soc_mem_field_set(unit, ifp_tcam_wide, entry_buf,
                               IPBMf, (uint32 *)&ipbm_data[inst]);
            soc_mem_field_width_fit_set(unit, ifp_tcam_wide, entry_buf,
                               IPBM_MASKf, (uint32 *)&ipbm_mask[inst]);
            no_pbm_access = 0;
        }

        soc_mem_field32_set(unit, ifp_tcam_wide, entry_buf, VALIDf, valid);
        BCM_IF_ERROR_RETURN(soc_th_ifp_mem_write(unit,
                                        ifp_tcam_wide,
                                        MEM_BLOCK_ALL,
                                        tcam_idx,
                                        entry_buf));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_field_td3_qualify_InPorts
 * Purpose:
 *      Match on IPBM Source bitmap for the given bitmap.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) IPBM Qualifier type
 *      data  - (IN) IPBM data
 *      mask  - (IN) IPBM Mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_field_td3_qualify_InPorts(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_qualify_t qual,
                               bcm_pbmp_t data,
                               bcm_pbmp_t mask)
{
    int               rv = BCM_E_NONE;
    uint32            port_class = 0;
    bcm_module_t      local_mod = 0;
    bcm_gport_t       gport = -1;
    bcm_port_t        port;
    bcm_pbmp_t        lb_pbm;
    bcm_pbmp_t        temp_pbm;
    bcm_pbmp_t        valid_pbm;
    _field_group_t   *fg =  NULL;
    _field_entry_t   *f_ent = NULL;
    _field_stage_t   *stage_fc = NULL;
    bcm_pbmp_t        ipbm_data;
    bcm_pbmp_t        ipbm_mask;
    uint32            pbm_34_49 = 0;
    uint32            pbm_50_65 = 0;
    uint32            pbm_34_49_mask = 0;
    uint32            pbm_50_65_mask = 0;

    /* Validate the qualifier. */
    if (!_BCM_FIELD_IS_PBMP_QUALIFIER(qual)) {
        return BCM_E_PARAM;
    }

    if (qual != bcmFieldQualifyInPorts) {
        return _bcm_field_th_qualify_InPorts(unit, entry, qual, data, mask);
    }

    /* Get field entry structure pointer. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    fg = f_ent->group;
    /* Get the stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Confirm that InPorts/PortBitmap Qualifiers are in group's Qset. */
    if (!_BCM_FIELD_QSET_PBMP_TEST(fg->qset)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "Error: %s not in Group=%d Qset.\n"),
                  _field_qual_name(qual), fg->gid));
        return (BCM_E_NOT_FOUND);
    }

    if (bcmFieldGroupOperModePipeLocal == stage_fc->oper_mode) {
       BCM_PBMP_ASSIGN(valid_pbm, PBMP_PIPE(unit, fg->instance));
       BCM_PBMP_ASSIGN(lb_pbm, valid_pbm);
       BCM_PBMP_AND(lb_pbm, PBMP_LB(unit));
       BCM_PBMP_REMOVE(valid_pbm, PBMP_LB(unit));
    } else {
       /*
        * In-case of Atomic update, InPorts/PortBitmap qualifiers are not
        * support in Global mode.
        */
       if (soc_property_get(unit, spn_FIELD_ATOMIC_UPDATE, FALSE) == TRUE) {
          return BCM_E_UNAVAIL;
       }

       BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &valid_pbm));
       BCM_PBMP_ASSIGN(lb_pbm, PBMP_LB(unit));
    }

    /* Validate data */
    BCM_PBMP_ASSIGN(temp_pbm, valid_pbm);
    BCM_PBMP_OR(temp_pbm, data);
    if (BCM_PBMP_NEQ(temp_pbm, valid_pbm)) {
       return BCM_E_PARAM;
    }

    /*
     * Check and clear invalid ports in the mask.
     */
    BCM_PBMP_AND(valid_pbm, mask);
    BCM_PBMP_ASSIGN(mask, valid_pbm);
    BCM_PBMP_OR(mask, lb_pbm);

    if (bcmFieldGroupOperModePipeLocal == stage_fc->oper_mode) {
       uint8       set_data = 0;
       bcm_port_t  temp_p = 0;
       uint8 ipbm_hit, opq1_hit, opq2_hit;
       char           buf_ipbm[100];

       BCM_PBMP_CLEAR(ipbm_data);
       BCM_PBMP_CLEAR(ipbm_mask);
       BCM_PBMP_CLEAR(f_ent->pbmp.data);
       BCM_PBMP_CLEAR(f_ent->pbmp.mask);

       BCM_PBMP_ITER(mask, port) {
          ipbm_hit = opq1_hit = opq2_hit = 0;

          BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_mod));
          BCM_GPORT_MODPORT_SET(gport, local_mod,
                  (fg->instance == 0) ? port : _BCM_TD3_NUM_PIPE_PORTS + port);

          rv = (bcm_esw_port_class_get(unit,
                      gport,
                      bcmPortClassFieldIngressSystemPort,
                      &port_class));
          if (BCM_E_PORT == rv) {
              continue;
          }
          BCM_PBMP_PORT_ADD(f_ent->pbmp.mask, port);
          if (BCM_PBMP_MEMBER(data, port)) {
             set_data = 1;
             BCM_PBMP_PORT_ADD(f_ent->pbmp.data, port);
          }

          if (port_class < _FP_TD3_TCAM_IPBMP_SIZE) {
             BCM_PBMP_PORT_ADD(ipbm_mask, port_class);

             if (set_data == 1) {
                BCM_PBMP_PORT_ADD(ipbm_data, port_class);
             }
             ipbm_hit = 1;
          } else if (port_class < (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE)) {
             port_class = port_class - _FP_TD3_TCAM_IPBMP_SIZE;

             if (set_data == 1) {
                pbm_34_49 |= 1 << port_class;
             }

             pbm_34_49_mask |= 1 << port_class;

             opq1_hit = 1;
          } else if (port_class < (_FP_TD3_TCAM_IPBMP_SIZE + (_FP_FLEX_PBMP_SIZE * 2))) {
             port_class = port_class - (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE);

             if (set_data == 1) {
                pbm_50_65 |= 1 << port_class;
             }

             pbm_50_65_mask |= 1 << port_class;
             opq2_hit = 1;
          } else {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "InPorts Param exceeds %d ports in the bitmap.\n\r"),
                 _FP_TD3_TCAM_IPBMP_SIZE + (_FP_FLEX_PBMP_SIZE * 2)));
             return BCM_E_PARAM;
          }

          LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
            "INST:%d port:%d temp_p:%d ipbm_hit:%d opq1_hit:%d opq2_hit:%d set_data:%d\n\r"),
            fg->instance, port, temp_p, ipbm_hit, opq1_hit, opq2_hit, set_data));

          LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                 "INST:%d ipbm_data:%s ipbm_mask:%s\n\r"
                 "       pbm_34_49:0x%x pbm_34_49_mask:0x%x pbm_50_65:0x%x pbm_50_65_mask:0x%x\n\r"
                 "       HW_val:0x%x\n\r"),
                 fg->instance, SOC_PBMP_FMT(ipbm_data, buf_ipbm),
                 SOC_PBMP_FMT(ipbm_mask, buf_ipbm),
                 pbm_34_49, pbm_34_49_mask, pbm_50_65,
                 pbm_50_65_mask,
                 ((ipbm_hit == 1) ? port_class : (1 << port_class))));

          set_data = 0;
       }

       if (soc_feature(unit, soc_feature_xy_tcam_x0y)) {
          BCM_PBMP_XOR(ipbm_mask, ipbm_data);
          BCM_PBMP_CLEAR(ipbm_data);

          pbm_34_49_mask ^= pbm_34_49;
          pbm_34_49 = 0;

          pbm_50_65_mask ^= pbm_50_65;
          pbm_50_65 = 0;

          LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META(
                 "INSTANCE:%d ipbm_data:%s ipbm_mask:%s\n\r"
                 "            pbm_34_49:0x%x pbm_34_49_mask:0x%x pbm_50_65:0x%x pbm_50_65_mask:0x%x\n\r"),
                 fg->instance, SOC_PBMP_FMT(ipbm_data, buf_ipbm),
                 SOC_PBMP_FMT(ipbm_mask, buf_ipbm),
                 pbm_34_49, pbm_34_49_mask, pbm_50_65,
                 pbm_50_65_mask));

       }

       /* coverity[address_of : FALSE] */
       /* coverity[callee_ptr_arith : FALSE] */
       rv = _bcm_field_th_qualify_set(unit, entry, qual,
                                      (uint32 *)&ipbm_data,
                                      (uint32 *)&ipbm_mask,
                                      _FP_QUALIFIER_ADD);
       BCM_IF_ERROR_RETURN(rv);
       rv = _bcm_field_th_qualify_set(unit, entry,
                                      _bcmFieldQualifyInPorts_1,
                                      (uint32 *)&pbm_34_49,
                                      (uint32 *)&pbm_34_49_mask,
                                      _FP_QUALIFIER_ADD);
       BCM_IF_ERROR_RETURN(rv);
       rv = _bcm_field_th_qualify_set(unit, entry,
                                      _bcmFieldQualifyInPorts_2,
                                      (uint32 *)&pbm_50_65,
                                      (uint32 *)&pbm_50_65_mask,
                                      _FP_QUALIFIER_ADD);
       BCM_IF_ERROR_RETURN(rv);
    } else {
       BCM_PBMP_ASSIGN(f_ent->pbmp.data, data);
       BCM_PBMP_ASSIGN(f_ent->pbmp.mask, mask);
    }

    f_ent->flags |= _FP_ENTRY_DIRTY;

    return BCM_E_NONE;
}

/*
 * Initialize IPBM and Opaque object fields of source_trunk_map table
 * to unique values to achieve InPorts functionality.
 */
int
_bcm_td3_field_inports_flex_config_init(int unit, int port)
{
    int rv;
    int temp_p;
    int stm_idx;
    int my_modid;
    int pipe_port;
    uint32 ipbm_val = 0;
    uint32 opq_obj_val = 0;
    uint8 opaque_3 = 3;
    uint8 opaque_4 = 4;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_field_t ipbm_fld = IPBM_INDEXf;
    soc_field_t opq_obj_fld = OPAQUE_OBJECTf;
    source_trunk_map_table_entry_t stm_entry;

    if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
       return BCM_E_NONE;
    }

    if (!(SOC_MEM_FIELD_VALID(unit, mem, ipbm_fld) &&
          SOC_MEM_FIELD_VALID(unit, mem, opq_obj_fld))) {
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                            port, &stm_idx));
    /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Set IPBM_INDEX for HG Ports */
    pipe_port = (port >= _BCM_TD3_NUM_PIPE_PORTS) ?
             (port - _BCM_TD3_NUM_PIPE_PORTS) : port;

    /* Set IPBM_INDEX */
    ipbm_val = (pipe_port < _FP_TD3_TCAM_IPBMP_SIZE) ? pipe_port : _FP_TD3_TCAM_IPBMP_SIZE+1;
    soc_mem_field32_set(unit, mem, &stm_entry, ipbm_fld, ipbm_val);

    /* Set OPAQUE_OBJECT */
    if (pipe_port >= _FP_TD3_TCAM_IPBMP_SIZE) {
       if (pipe_port >= (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE)) {
          opq_obj_val = opaque_4;
          temp_p = pipe_port - (_FP_TD3_TCAM_IPBMP_SIZE + _FP_FLEX_PBMP_SIZE);
       } else {
          opq_obj_val = opaque_3;
          temp_p = pipe_port - _FP_TD3_TCAM_IPBMP_SIZE;
       }
       opq_obj_val = ((1 << temp_p) << 3) | opq_obj_val;
       soc_mem_field32_set(unit, mem, &stm_entry, opq_obj_fld, opq_obj_val);
    }

    /* Write source trunk map table */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, stm_idx, &stm_entry);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
_bcm_field_td3_inports_flex_init(int unit)
{
    int rv;
    int port;
    int min_port = 0;
    int max_port = 0;

    if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
       return BCM_E_NONE;
    }

    max_port = _BCM_TD3_NUM_PIPE_PORTS * NUM_PIPE(unit);

    for (port = min_port; port < max_port; port++) {
       rv = _bcm_td3_field_inports_flex_config_init(unit, port);
       BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}


/*
 * Update free available control selector for a given container.
 */
STATIC int
_field_ing_fbus_cont_free_ctrl_sel_val_get(int unit, int instance,
                                         _field_stage_t *stage_fc,
                                                  uint16 cont_val,
                                               uint8 *free_arr_ct,
                                     uint8 *free_ctrl_sel_val_arr,
                                        int8  *found_ctrl_sel_val)
{
    int idx;
    int ct = 0;
    uint16 cont;
    uint8 cont_sel[_FP_MAX_IBUS_CONTS] = {11, 12, 13, 14};

    for (idx = 0; idx < _FP_MAX_IBUS_CONTS; idx++) {
       if (stage_fc->ifbus_cont_stat[instance][idx].ref_count != 0) {
          cont = stage_fc->ifbus_cont_stat[instance][idx].cont_val;
          if (cont_val == cont) {
             *found_ctrl_sel_val = cont_sel[idx];
             ct = 0;
             break;
          }
          continue;
       }

       free_ctrl_sel_val_arr[ct++] = cont_sel[idx];
    }

    *free_arr_ct = ct;

    return BCM_E_NONE;
}

/*
 * Retrieve qualifier selector values.
 */
int
_bcm_field_qual_cfg_info_db_update(int unit,
                                   int instance,
                                   bcm_field_qualify_t qual,
                                   _field_stage_t *stage_fc)
{
    int rv;
    uint8 cfg = 0;
    uint16 cont_val = 0;
    int8 found_ctrl_val = -1;
    uint8 allocated_cont = 0;
    uint8 num_qual_cfg = 0;
    uint8 free_ct = _FP_MAX_IBUS_CONTS;
    uint8 free_pri_ctrl_val_arr[_FP_MAX_IBUS_CONTS] = {0};
    bcmi_keygen_qual_cfg_info_db_t *qual_db;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_t *new_qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_t new_qual_cfg[_FP_MAX_IBUS_CONTS * 2];

    if (stage_fc == NULL) {
       return BCM_E_NONE;
    }

    qual_db = stage_fc->qual_cfg_info_db;

    /*
     * Retrieve Qualifier container Id.
     */
    if ((qual_db == NULL) ||
        (qual_db->qual_cfg_info[qual] == NULL)) {
        return BCM_E_PARAM;
    }

    cont_val = qual_db->qual_cfg_info[qual]->qual_cfg_arr->sec_ctrl_sel_val;

    rv = _field_ing_fbus_cont_free_ctrl_sel_val_get(unit, instance,
                                                   stage_fc,
                                                   cont_val,
                                                   &free_ct,
                                                   free_pri_ctrl_val_arr,
                                                   &found_ctrl_val);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * If the given qual is already allocated to any container,
     * use the same container
     */
    if (found_ctrl_val != -1) {
       allocated_cont = found_ctrl_val;
    } else {
       /* All containers are already used. */
       if (free_ct == 0) {
          return BCM_E_RESOURCE;
       }

       /* Reserve one container and retrieve the selcodes for the same */
       allocated_cont = free_pri_ctrl_val_arr[0];
    }

    qual_cfg_arr = qual_db->qual_cfg_info[qual]->qual_cfg_arr;
    for (cfg = 0; cfg < qual_db->qual_cfg_info[qual]->num_qual_cfg; cfg++) {
        if (qual_cfg_arr[cfg].pri_ctrl_sel_val == allocated_cont) {
           sal_memcpy(&new_qual_cfg[num_qual_cfg], &qual_cfg_arr[cfg],
                          sizeof(bcmi_keygen_qual_cfg_t));
           num_qual_cfg += 1;
        }
    }

    new_qual_cfg_arr = NULL;
    _FP_XGS3_ALLOC(new_qual_cfg_arr,
                   num_qual_cfg * sizeof(bcmi_keygen_qual_cfg_t),
                   "Qual DB new_qual_cfg_arr alloc");
    if (new_qual_cfg_arr == NULL) {
       return BCM_E_MEMORY;
    }

    sal_memcpy(new_qual_cfg_arr, new_qual_cfg,
             (num_qual_cfg * sizeof(bcmi_keygen_qual_cfg_t)));

    qual_db->qual_cfg_info[qual]->qual_cfg_arr = new_qual_cfg_arr;
    qual_db->qual_cfg_info[qual]->num_qual_cfg = num_qual_cfg;

    rv = _bcm_field_td3_field_bus_cont_sel_install(unit, instance, stage_fc,
                                                   NULL, qual);
    return rv;
}

/*
 * Function to configure ING field bus container value.
 */
int
_bcm_field_td3_field_bus_cont_sel_install(int unit,
                                          int instance,
                                          _field_stage_t *stage_fc,
                                          _field_group_t *fg,
                                          bcm_field_qualify_t qual)
{
    int rv;
    uint8 cont = 0;
    uint32 reg_val = 0;
    uint16 cont_val = 0;
    uint8 ctrl_sel_val = 0;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;

    if (instance >= NUM_PIPE(unit)) {
       return BCM_E_PARAM;
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
       reg = IFP_AUX_TAG_FIELD_BUS_CONTAINER_SELr;
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
       reg = EXACT_MATCH_AUX_TAG_FIELD_BUS_CONTAINER_SELr;
    } else {
       return BCM_E_PARAM;
    }

    cont_val =
      stage_fc->qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr->sec_ctrl_sel_val;
    ctrl_sel_val =
      stage_fc->qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr->pri_ctrl_sel_val;

    if ((stage_fc->ifbus_cont_stat[instance][cont].cont_val == cont_val) &&
        (stage_fc->ifbus_cont_stat[instance][cont].ref_count != 0)) {
        stage_fc->ifbus_cont_stat[instance][cont].ref_count += 1;
        return BCM_E_NONE;
    }

    switch (ctrl_sel_val) {
       case 11:
            cont = 0;
            field = INGRESS_FIELD_BUS_CONTAINER_Af;
            break;
       case 12:
            cont = 1;
            field = INGRESS_FIELD_BUS_CONTAINER_Bf;
            break;
       case 13:
            cont = 2;
            field = INGRESS_FIELD_BUS_CONTAINER_Cf;
            break;
       case 14:
            cont = 3;
            field = INGRESS_FIELD_BUS_CONTAINER_Df;
            break;
       default:
            return BCM_E_PARAM;
    }

    if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
       reg = SOC_REG_UNIQUE_ACC(unit, reg)[instance];
    }

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);
    BCM_IF_ERROR_RETURN(rv);

    soc_reg_field_set(unit, reg, &reg_val, field, cont_val);

    rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val);
    BCM_IF_ERROR_RETURN(rv);

    /* update stage db */
    stage_fc->ifbus_cont_stat[instance][cont].cont_val = cont_val;
    stage_fc->ifbus_cont_stat[instance][cont].ref_count++;

    return BCM_E_NONE;
}

/*
 * Function to configure ING field bus container value.
 */
int
_bcm_field_td3_field_bus_cont_sel_remove(int unit,
                                         int instance,
                                         _field_stage_t *stage_fc,
                                         _field_group_t *fg,
                                         bcm_field_qualify_t qual)
{
    int rv;
    uint8 cont = 0;
    uint8 ctrl_val = 0;
    uint32 reg_val = 0;
    uint16 cont_val = 0;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;

    if (instance >= NUM_PIPE(unit)) {
       return BCM_E_PARAM;
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
       reg = IFP_AUX_TAG_FIELD_BUS_CONTAINER_SELr;
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
       reg = EXACT_MATCH_AUX_TAG_FIELD_BUS_CONTAINER_SELr;
    } else {
       return BCM_E_PARAM;
    }

    cont_val =
      stage_fc->qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr->sec_ctrl_sel_val;
    ctrl_val =
      stage_fc->qual_cfg_info_db->qual_cfg_info[qual]->qual_cfg_arr->pri_ctrl_sel_val;

    switch (ctrl_val) {
       case 11:
            cont = 0;
            field = INGRESS_FIELD_BUS_CONTAINER_Af;
            break;
       case 12:
            cont = 1;
            field = INGRESS_FIELD_BUS_CONTAINER_Bf;
            break;
       case 13:
            cont = 2;
            field = INGRESS_FIELD_BUS_CONTAINER_Cf;
            break;
       case 14:
            cont = 3;
            field = INGRESS_FIELD_BUS_CONTAINER_Df;
            break;
       default:
            return BCM_E_PARAM;
    }


    if (stage_fc->ifbus_cont_stat[instance][cont].cont_val == cont_val) {
       if (stage_fc->ifbus_cont_stat[instance][cont].ref_count == 0) {
          return BCM_E_INTERNAL;
       }
       stage_fc->ifbus_cont_stat[instance][cont].ref_count -= 1;
       if (stage_fc->ifbus_cont_stat[instance][cont].ref_count != 0) {
          return BCM_E_NONE;
       }
    } else {
       return BCM_E_INTERNAL;
    }

    if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
       reg = SOC_REG_UNIQUE_ACC(unit, reg)[instance];
    }

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);
    BCM_IF_ERROR_RETURN(rv);

    soc_reg_field_set(unit, reg, &reg_val, field, 0x0);

    rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val);
    BCM_IF_ERROR_RETURN(rv);

    /* update stage db */
    stage_fc->ifbus_cont_stat[instance][cont].cont_val = -1;

    return BCM_E_NONE;
}

int _bcm_field_td3_qset_udf_chunk_validate(int unit,
                                           bcm_udf_chunk_info_t *udf_info,
                                           bcm_udf_alloc_hints_t *hints)
{
#define _BCM_FIELD_IS_QUAL_USES_UDF_CHUNK(_qual_, _chunk_)    \
    (BCM_FIELD_QSET_TEST(hints->qset, _qual_)                 \
        && (udf_info->chunk_bmap & (1 << _chunk_)))

    if (_BCM_FIELD_IS_QUAL_USES_UDF_CHUNK(bcmFieldQualifyIpIdentifier, 4)
        || _BCM_FIELD_IS_QUAL_USES_UDF_CHUNK(bcmFieldQualifyHiGigGbpSrcId, 5)
        || _BCM_FIELD_IS_QUAL_USES_UDF_CHUNK(bcmFieldQualifyLoopbackSubtype, 7)) {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

int
_bcm_field_td3_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask)
{
    uint8 data1 = 0;
    uint8 mask1 = 0;
    uint8 valid_range;

    if (NULL == data || NULL == mask) {
       return BCM_E_PARAM;
    }

    valid_range = BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT |
                  BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT |
                  BCM_FIELD_VXLT_LOOKUP_STATUS_HIT;
    /* exact match mask. */
    if (0xff == *mask) {
      *mask = valid_range;
    }

    if (((BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT != *data)
         || (BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT != *mask))
         && ((*data & ~(valid_range)) || (*mask & ~(valid_range)))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                  "Error: Invalid input paramaeters *data=0x%x *mask=0x%x\n"),
                   *data, *mask));
        return BCM_E_PARAM;
    }

    if (BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT == *data) {
        data1 = 0;
    } else {
        if (*data & BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT) {
           data1 |= 0x1;
        }

        if ((*data & BCM_FIELD_VXLT_LOOKUP_STATUS_HIT)
            || (*data & BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT)) {
           data1 |= 0x2;
        }
    }

    if (BCM_FIELD_VXLT_LOOKUP_STATUS_NO_HIT ==  *mask) {
        mask1 = 0x0;
    } else {
        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_FIRST_HIT) {
           mask1 |= 0x1;
        }

        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_HIT) {
           mask1 |= 0x2;
        }

        if (*mask & BCM_FIELD_VXLT_LOOKUP_STATUS_SECOND_HIT) {
           mask1 |= 0x3;
        }
    }

    *data = data1;
    *mask = mask1;
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_td3_functions_init
 *
 * Purpose:
 *    Set up functions pointers
 *
 * Parameters:
 *    functions - (IN/OUT) Pointer to device specific field module utility
 *                         routines.
 *
 * Returns:
 *     Nothing
 */
void
_bcm_field_td3_functions_init(_field_funct_t *functions)
{
    (void) _bcm_field_th_functions_init(functions);

    functions->fp_action_support_check =
                                            _bcm_field_td3_action_support_check;
    functions->fp_action_params_check  =
                                             _bcm_field_td3_action_params_check;
    functions->fp_vlan_actions_profile_hw_alloc =
                                            _bcm_field_td3_vlan_action_hw_alloc;
    functions->fp_vlan_actions_profile_hw_free =
                                             _bcm_field_td3_vlan_action_hw_free;
    functions->fp_zone_match_id_partition_init =
                                    _bcm_field_td3_zone_match_id_partition_init;
    functions->fp_entry_stat_extended_attach =
                                  _bcm_field_td2plus_entry_stat_extended_attach;
    functions->fp_entry_stat_extended_get =
                                     _bcm_field_td2plus_entry_stat_extended_get;
    functions->fp_qset_id_multi_set = _bcm_field_td3_qset_id_multi_set;
    functions->fp_qset_id_multi_get = _bcm_field_td3_qset_id_multi_get;
    functions->fp_qset_id_multi_delete = _bcm_field_td3_qset_id_multi_delete;
    functions->fp_qualify_udf = _bcm_field_td3_qualify_udf;
    functions->fp_qualify_udf_get = _bcm_field_td3_qualify_udf_get;
    functions->fp_qualify_packet_res = _bcm_field_th_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _bcm_field_th_qualify_PacketRes_get;
    functions->fp_qualify_inports = _bcm_field_td3_qualify_InPorts;
}


#endif /* BCM_TRIDENT3_SUPPORT */
