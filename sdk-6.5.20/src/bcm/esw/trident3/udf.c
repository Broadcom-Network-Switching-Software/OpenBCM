/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * File:       udf.c
 * Purpose:    BCM56870 Field Processor functions.
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <soc/field.h>
#include <soc/format.h>
#include <soc/scache.h>
#include <soc/error.h>
#include <shared/bitop.h>
#include "include/soc/esw/cancun.h"
#include <include/soc/mcm/formatacc.h>
#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm/udf.h>
#include <bcm_int/esw/udf.h>

#if defined (BCM_WARM_BOOT_SUPPORT)
/* Warmboot Versions */
#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_2_0     SOC_SCACHE_VERSION(2, 0)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_2_0
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Maximum number of UDF objects - (Max Chunks * number of pkt abstracts) */
#define _BCM_UDF_TD3_MAX_OBJECTS(_u_)   \
     (_BCM_UDF_CTRL_MAX_UDF_CHUNKS(_u_) * bcmUdfAbstractPktFormatLastCount)

/* UDF Object Min and Max Values. */
#define _BCM_UDF_TD3_OBJ_MIN_ID 0x1
#define _BCM_UDF_TD3_OBJ_MAX_ID 0xfffe

/* UDF Parse limit supported by SRC_PKT_OFFSET in CMD_POLICY_DATA */
#define _BCM_UDF_TD3_MAX_PARSE_LIMIT 128

/* Number of commands dedicated to UDF in CMD_POLICY_DATAf */
#define _BCM_UDF_MAX_POLICY_CMDS 8

/* Device Parser level from which UDF is extracted */
#define _BCM_UDF_PARSER0 3
#define _BCM_UDF_PARSER1 1
#define _BCM_UDF_PARSER2 2

/* Global UDF control structure pointers */
_bcm_udf_td3_ctrl_t *udf_td3_control[BCM_MAX_NUM_UNITS];

#define UDF_TD3_CTRL(_u_) udf_td3_control[_u_]

/*
 *  UDF Container Chunks (2 bytes each) to Destination container ID mapping.
 *
 *  UDF Chunks  Container ID
 *  ==========  ============
 *  UDF1_0      CONT_20
 *  UDF1_1      CONT_19
 *  UDF1_2      CONT_18
 *  UDF1_3      CONT_17
 *  UDF1_4      CONT_16
 *  UDF1_5      CONT_15
 *  UDF1_6      CONT_14
 *  UDF1_7      CONT_13
 *  UDF2_0      CONT_28
 *  UDF2_1      CONT_27
 *  UDF2_2      CONT_26
 *  UDF2_3      CONT_25
 *  UDF2_4      CONT_24
 *  UDF2_5      CONT_23
 *  UDF2_6      CONT_22
 *  UDF2_7      CONT_21
 */
STATIC
uint8 _bcm_udf_chunk_to_dest_cont_id_mapping[16] =
      {20, 19, 18, 17, 16, 15, 14, 13, 28, 27, 26, 25, 24, 23, 22, 21};

/* Opaque to UDF chunks */
STATIC
uint8 _bcm_udf_opaque_cont_to_chunk_mapping[16] =
      {7, 6, 5 ,4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8};

/* Range Check supported UDF Chunk - UDF1_CHUNK2 */
#define _BCM_UDF_TD3_RANGE_CHUNK_BMAP 0x4

/* Maximum number of Chunks Supported. */
#define _BCM_UDF_TD3_MAX_CHUNKS     16

/* Maximum Chunk Bitmap. */
#define _BCM_UDF_TD3_MAX_CHUNK_BMAP 0xffff

/*
 * Global Variable to hold Abstract
 * Packet format HW information.
 */
_bcm_udf_td3_abstr_pkt_fmt_hw_info_t *abstr_pkt_fmt_hw_info[BCM_MAX_NUM_UNITS][bcmUdfAbstractPktFormatLastCount];

/*
 * Macro to retrieve pointer to _bcm_udf_td3_abstr_pkt_fmt_hw_info_t
 * structure for a given abstr pkt format.
 */
#define _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_u_, _pkt_fmt_)   \
                 abstr_pkt_fmt_hw_info[_u_][_pkt_fmt_]


/* UDF Byte to Chunk Conversion */
#define _BCM_UDF_BYTE_TO_CHUNK(_val_) (_val_/2)

/* UDF Chunk to Byte Conversion */
#define _BCM_UDF_CHUNK_TO_BYTE(_val_) (_val_ * 2)

/* Validate Abstract Packet format */
#define _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(_u_, _pkt_fmt_)       \
    do {                                                         \
       if ((_pkt_fmt_ < bcmUdfAbstractPktFormatLlc) ||           \
           (_pkt_fmt_ >= bcmUdfAbstractPktFormatLastCount)) {    \
           return BCM_E_PARAM;                                   \
       }                                                         \
       if (abstr_pkt_fmt_hw_info[_u_][_pkt_fmt_]->max_extract_bytes == 0) {      \
           return BCM_E_UNAVAIL;                                 \
       }                                                         \
    } while (0)

/* Validate Extraction bytes for a given packet format */
#define _BCM_UDF_EXTRACTION_BYTES_VALIDATE(_u_, _pkt_format_, _bytes_)       \
    do {                                                                     \
       _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *_pkt_format_hw_info_ = NULL;    \
                                                                             \
       _pkt_format_hw_info_ = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_u_, _pkt_format_);  \
       if (_pkt_format_hw_info_ == NULL)  {                                  \
          return BCM_E_INTERNAL;                                             \
       }                                                                     \
       if (_bytes_ > (_pkt_format_hw_info_->max_extract_bytes -              \
                      _pkt_format_hw_info_->extracted_bytes)) {              \
          return BCM_E_CONFIG;                                               \
       }                                                                     \
    } while(0)

/* Validate chunk */
#define _BCM_UDF_CHUNK_VALIDATE(_u_, _chunk_bmap_, _pkt_fmt_)                      \
    do {                                                                           \
       uint32 _unavail_cbmap;                                                      \
       if ((_chunk_bmap_ == 0x0) ||                                                \
           (_chunk_bmap_ > _BCM_UDF_TD3_MAX_CHUNK_BMAP)) {                         \
            return BCM_E_PARAM;                                                    \
       }                                                                           \
       _unavail_cbmap = abstr_pkt_fmt_hw_info[_u_][_pkt_fmt_]->unavail_chunk_bmap; \
       if (_unavail_cbmap & _chunk_bmap_) {                                        \
           LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,                             \
             "The chunks in the given chunk bitmap are reserved/unavailable: 0x%x\n\r"), \
              _unavail_cbmap & _chunk_bmap_));                                     \
           return BCM_E_CONFIG;                                                    \
       }                                                                           \
    } while(0)

/* Validate Chunk bitmap */
#define _BCM_UDF_OBJECT_CHUNK_IN_USE_VALIDATE(_u_, _pkt_format_, _chunk_bmap_)   \
    do {                                                                         \
       _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *_pkt_format_hw_info_ = NULL;        \
                                                                                 \
       _pkt_format_hw_info_ = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(_u_, _pkt_format_); \
       if (_pkt_format_hw_info_ == NULL)  {                                      \
          return BCM_E_INTERNAL;                                                 \
       }                                                                         \
       if (_pkt_format_hw_info_->chunk_bmap_used & _chunk_bmap_) {               \
          return BCM_E_CONFIG;                                                   \
       }                                                                         \
    } while(0)

/*
 * Typedef:
 *    _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL
 */
#define _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL    \
    int _rv_;                                        \
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t _pkt_fmt_hw_cfg_;

/*
 * Typedef:
 *    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT
 * Purpose:
 *    Insert UDF abstract packet format configuration.
 */
#define _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(_unit_, _pkt_format_, _parser_)  \
      do {                                                                     \
            uint8 _stg_;                                                       \
            sal_memset(&_pkt_fmt_hw_cfg_, 0x0,                                 \
                           sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t));      \
            (_pkt_fmt_hw_cfg_).parser = (_parser_);                            \
            (_pkt_fmt_hw_cfg_).base_offset = 0x0;                              \
            (_pkt_fmt_hw_cfg_).max_extract_bytes = 0x0;                        \
            for (_stg_ = 0; _stg_ < _BCM_UDF_MAX_PARSER_STAGES; _stg_++) {     \
                (_pkt_fmt_hw_cfg_).stage_policy_mem[_stg_] = INVALIDm;         \
                (_pkt_fmt_hw_cfg_).hfe_prof_ptr_arr_len[_stg_] = 0;            \
                (_pkt_fmt_hw_cfg_).hfe_profile_ptr[_stg_]  = NULL;             \
            }                                                                  \
            _rv_ = _bcm_udf_td3_abstr_pkt_fmt_hw_info_insert((_unit_),         \
                                  (_pkt_format_), &(_pkt_fmt_hw_cfg_));        \
            BCM_IF_ERROR_RETURN(_rv_);                                         \
      } while(0)


/*
 * Macros to manage (add, delete, get) the Object Node
 */
#define _BCM_UDF_OBJ_NODE_ADD(_u_, _new_, _head_)             \
        _UDF_DLL_NODE_ADD(_u_, _new_, _head_)

#define _BCM_UDF_OBJ_NODE_DEL(_u_, _del_, _head_)             \
    do {                                                      \
        _bcm_udf_td3_obj_info_t *_temp_;                      \
        _UDF_DLL_NODE_DEL(_u_, _del_, _head_, _temp_);        \
    } while (0)

#define _BCM_UDF_OBJ_NODE_GET(_u_, _id_, _head_, _node_)      \
    do {                                                      \
        _bcm_udf_td3_obj_info_t *_temp_;                      \
        _UDF_DLL_NODE_GET(_u_, _id_, _head_, _temp_, _node_); \
    } while (0)

#define _BCM_UDF_OBJ_NODE_DEL_ALL(_u_, _head_)                        \
    do {                                                              \
         _bcm_udf_td3_obj_info_t *_temp_;                             \
         while(_head_ != NULL) {                                      \
             _temp_ = _head_;                                         \
             _BCM_UDF_OBJ_NODE_DEL(_u_, _temp_, _head_);              \
             _BCM_UDF_FREE(_temp_);                                   \
         }                                                            \
    } while (0)

#define _BCM_UDF_OBJ_NODE_NEXT(_node_)                   \
        _node_ = _node_->prev;


soc_field_t src_pkt_offset_p0_cmds[8] = {
                                        INVALIDf,
                                        INVALIDf,
                                        INVALIDf,
                                        INVALIDf,
                                        INVALIDf,
                                        INVALIDf,
                                        CMD_A_8_SRC_PKT_OFFSETf,
                                        CMD_A_9_SRC_PKT_OFFSETf
                                     };
soc_field_t valid_p0_cmds[8] = {
                               INVALIDf,
                               INVALIDf,
                               INVALIDf,
                               INVALIDf,
                               INVALIDf,
                               INVALIDf,
                               CMD_A_8_VALIDf,
                               CMD_A_9_VALIDf,
                            };

soc_field_t src_pkt_offset_cmds[8] = {
                                        CMD_C_4_SRC_PKT_OFFSETf,
                                        CMD_C_5_SRC_PKT_OFFSETf,
                                        CMD_C_6_SRC_PKT_OFFSETf,
                                        CMD_C_7_SRC_PKT_OFFSETf,
                                        CMD_C_8_SRC_PKT_OFFSETf,
                                        CMD_C_9_SRC_PKT_OFFSETf,
                                        CMD_C_10_SRC_PKT_OFFSETf,
                                        CMD_C_11_SRC_PKT_OFFSETf
                                     };
soc_field_t dest_cont_id_cmds[8] = {
                                      CMD_C_4_DEST_CONT_IDf,
                                      CMD_C_5_DEST_CONT_IDf,
                                      CMD_C_6_DEST_CONT_IDf,
                                      CMD_C_7_DEST_CONT_IDf,
                                      CMD_C_8_DEST_CONT_IDf,
                                      CMD_C_9_DEST_CONT_IDf,
                                      CMD_C_10_DEST_CONT_IDf,
                                      CMD_C_11_DEST_CONT_IDf
                                   };
soc_field_t valid_cmds[8] = {
                               CMD_C_4_VALIDf,
                               CMD_C_5_VALIDf,
                               CMD_C_6_VALIDf,
                               CMD_C_7_VALIDf,
                               CMD_C_8_VALIDf,
                               CMD_C_9_VALIDf,
                               CMD_C_10_VALIDf,
                               CMD_C_11_VALIDf
                            };

#define _UDF_TD3_GET_HFE_CMD_FORMAT(_parser_, _fmt_) \
      do {                                                  \
            if (_parser_ == _BCM_UDF_PARSER1) {             \
               _fmt_ = IP_PARSER1_HFE_CMD_POLICY_DATAfmt;   \
            } else if (_parser_ == _BCM_UDF_PARSER2) {      \
               _fmt_ = IP_PARSER2_HFE_CMD_POLICY_DATAfmt;   \
            } else if (_parser_ == _BCM_UDF_PARSER0) {      \
               _fmt_ = IP_PARSER0_HFE_CMD_POLICY_DATAfmt;   \
            } else {                                        \
               _fmt_ = INVALIDfmt;                          \
            }                                               \
         } while(0)


/* Local Structure to hold chunk information assigned to UDF object. */
typedef struct _udf_obj_pkt_fmt_hw_cfg_s {
   uint8   num_chunks;                           /* Number of chunks. */
   uint8   chunk_arr[_BCM_UDF_TD3_MAX_CHUNKS];   /* Chunk array. */
   uint16  cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES]; /* CMD Bitmap for each stage */
} _udf_obj_pkt_fmt_hw_cfg_t;

/* Function Declarations */
#if defined (BCM_WARM_BOOT_SUPPORT)
STATIC int
_bcm_udf_td3_reinit(int unit);

STATIC int
_bcm_udf_td3_wb_alloc(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
STATIC int
_bcm_udf_td3_abstr_pkt_fmt_hw_info_insert(int unit, bcm_udf_abstract_pkt_format_t format,
                                   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_format_info);

extern int
_bcm_field_td3_qset_udf_chunk_validate(int unit,
                                       bcm_udf_chunk_info_t *udf_info,
                                       bcm_udf_alloc_hints_t *hints);

soc_mem_t _udf_hfe_to_hme_mem_conv[11][2] = {
               {IP_PARSER0_HME_STAGE_TCAM_DATA_ONLY_0m, IP_PARSER0_HFE_POLICY_TABLE_0m},
               {IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_0m, IP_PARSER1_HFE_POLICY_TABLE_0m},
               {IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_1m, IP_PARSER1_HFE_POLICY_TABLE_1m},
               {IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_2m, IP_PARSER1_HFE_POLICY_TABLE_2m},
               {IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_3m, IP_PARSER1_HFE_POLICY_TABLE_3m},
               {IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_4m, IP_PARSER1_HFE_POLICY_TABLE_4m},
               {IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_0m, IP_PARSER2_HFE_POLICY_TABLE_0m},
               {IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_1m, IP_PARSER2_HFE_POLICY_TABLE_1m},
               {IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_2m, IP_PARSER2_HFE_POLICY_TABLE_2m},
               {IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_3m, IP_PARSER2_HFE_POLICY_TABLE_3m},
               {IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_4m, IP_PARSER2_HFE_POLICY_TABLE_4m}};


STATIC int
 _udf_td3_hme_to_hfe_mem_get(soc_mem_t hme_mem, soc_mem_t  *hfe_mem) {
     int ct;

     for (ct = 0; ct < 11; ct++) {
        if (_udf_hfe_to_hme_mem_conv[ct][0] == hme_mem) {
           *hfe_mem = _udf_hfe_to_hme_mem_conv[ct][1];
           break;
        }
     }

     if (ct >= 11) {
        return BCM_E_CONFIG;
     }

     return BCM_E_NONE;
}

STATIC int
_udf_td3_opaque_cont_to_chunk_bmap(uint32 opq_cont_bmap, uint32 *chunk_bmap)
{
    int id;

    for (id = 0; id < _BCM_UDF_TD3_MAX_CHUNKS; id++) {
        if (opq_cont_bmap & (1 << id)) {
           *chunk_bmap |= (1 << _bcm_udf_opaque_cont_to_chunk_mapping[id]);
        }
    }

    return BCM_E_NONE;
}

STATIC char *
_udf_abstr_pkt_fmt_name(bcm_udf_abstract_pkt_format_t abstr_pkt_fmt)
{
    static char *text[] = BCM_UDF_ABSTRACT_PKT_FORMAT;

    if (abstr_pkt_fmt >= COUNTOF(text)) {
        return "??";
    }
    return text[abstr_pkt_fmt];
}


/*
 * Free UDF device control structure.
 */
int _bcm_udf_td3_ctrl_free(int unit, _bcm_udf_td3_ctrl_t *udf_ctrl)
{
   if (udf_ctrl == NULL) {
      return BCM_E_NONE;
   }

   if (udf_ctrl->udf_obj_head != NULL) {
      _BCM_UDF_OBJ_NODE_DEL_ALL(_u_, udf_ctrl->udf_obj_head);
      udf_ctrl->udf_obj_head = NULL;
   }

   _BCM_UDF_FREE(udf_ctrl);
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_detach
 * Purpose:
 *      Detaches udf module and cleans software state.
 * Parameters:
 *      unit            - (IN)      Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_detach(int unit)
{
    uint8 stage;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    /* Free resources related to UDF module */
    BCM_IF_ERROR_RETURN(_bcm_udf_td3_ctrl_free(unit, UDF_TD3_CTRL(unit)));

    UDF_TD3_CTRL(unit) = NULL;

    /* Free Abstract Packet Format HW Info. */
    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        pkt_fmt_hw_info = abstr_pkt_fmt_hw_info[unit][pkt_fmt];
        if (pkt_fmt_hw_info != NULL) {
           for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
              if (pkt_fmt_hw_info->hfe_profile_ptr[stage] != NULL) {
                  _BCM_UDF_FREE(pkt_fmt_hw_info->hfe_profile_ptr[stage]);
              }

              if (pkt_fmt_hw_info->cmd_policy_data[stage] != NULL) {
                 _BCM_UDF_FREE(pkt_fmt_hw_info->cmd_policy_data[stage]);
              }
           }

           _BCM_UDF_FREE(abstr_pkt_fmt_hw_info[unit][pkt_fmt]);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_ctrl_init
 * Purpose:
 *      Initialize UDF control and internal data structures.
 * Parameters:
 *      unit           - (IN) bcm device.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_udf_td3_ctrl_init(int unit)
{
    int alloc_sz;
    _bcm_udf_td3_ctrl_t *udf_ctrl = NULL;

    /* Detach module if already installed */
    if (UDF_TD3_CTRL(unit) != NULL) {
       _bcm_udf_td3_detach(unit);
    }

    /* Allocating memory of UDF control structure */
    alloc_sz = sizeof(_bcm_udf_td3_ctrl_t);
    _BCM_UDF_ALLOC(udf_ctrl, alloc_sz, "UDF td3 control");
    if (udf_ctrl == NULL) {
        return BCM_E_MEMORY;
    }

    UDF_TD3_CTRL(unit) = udf_ctrl;

    /* UDF Range Checker support */
    udf_ctrl->flags |= _BCM_UDF_TD3_CTRL_RANGE_CHECK;

    /*
     * Update UDF Common control fields.
     */
    UDF_CONTROL(unit)->gran = _BCM_UDF_OFFSET_GRAN2;
    UDF_CONTROL(unit)->num_chunks = _BCM_UDF_OFFSET_CHUNKS16;
    UDF_CONTROL(unit)->min_obj_id = _BCM_UDF_TD3_OBJ_MIN_ID;
    UDF_CONTROL(unit)->max_obj_id = _BCM_UDF_TD3_OBJ_MAX_ID;
    UDF_CONTROL(unit)->parse_limit = _BCM_UDF_TD3_MAX_PARSE_LIMIT;
    UDF_CONTROL(unit)->max_udfs = _BCM_UDF_TD3_MAX_OBJECTS(unit);
    return BCM_E_NONE;
}

char *chunk_str[] =    {   "UDF_1_CHUNK_0",
                           "UDF_1_CHUNK_1",
                           "UDF_1_CHUNK_2",
                           "UDF_1_CHUNK_3",
                           "UDF_1_CHUNK_4",
                           "UDF_1_CHUNK_5",
                           "UDF_1_CHUNK_6",
                           "UDF_1_CHUNK_7",
                           "UDF_2_CHUNK_0",
                           "UDF_2_CHUNK_1",
                           "UDF_2_CHUNK_2",
                           "UDF_2_CHUNK_3",
                           "UDF_2_CHUNK_4",
                           "UDF_2_CHUNK_5",
                           "UDF_2_CHUNK_6",
                           "UDF_2_CHUNK_7",
                           };

STATIC int
_bcm_udf_flex_feature_chunk_bmap_get(int unit,
                                      char *feature,
                                      uint32 *flex_feat_bmp)
{
    int rv = BCM_E_NONE;
    int chunk = 0;
    soc_cancun_ceh_field_info_t ceh_info;

    for (chunk = 0; chunk < 16; chunk++) {
        rv = soc_cancun_ceh_obj_field_get(unit, feature, chunk_str[chunk],
                                                &ceh_info);
        if (BCM_FAILURE(rv)) {
            continue;
        }
        SHR_BITSET(flex_feat_bmp, chunk);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_udf_flex_feature_based_unavail_chunk_init(int unit)
{
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info = NULL;
    bcm_error_t rv = BCM_E_NONE;
    uint32 avail_chunk_bmap = 0;
    uint32 unavail_chunk_bmap = 0;
    uint32 rsrv_bmap = 0;
    uint32 reg_value = 0;
    uint32 f_int_bmp = 0, int_enable = -1;
    uint32 f_ifa_bmp = 0;
    uint32 f_nsh_bmp = 0, nsh_enable = -1;
    uint32 f_gbp_sid_bmp = 0, gbp_sid_enable = -1;
    uint32 f_gpe_ioam_bmp = 0, gpe_ioam_enable = -1;
    uint32 f_llc_bmp = 0, llc_enable = -1;
    uint32 f_ipsec_spi_bmp = 0, ipsec_spi_enable = -1;
    uint32 f_mpls_evpn_bmp = 0, mpls_evpn_enable = -1;
    uint32 f_mim_bmp = 0, mim_enable = -1;

    if (!soc_feature(unit, soc_feature_enable_flow_based_udf_extraction)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "INT", &f_int_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "IFA", &f_ifa_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "NSH", &f_nsh_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "GBP_SID", &f_gbp_sid_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "GPE_IOAM", &f_gpe_ioam_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "IPSEC_SPI", &f_ipsec_spi_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "MPLS_EVPN", &f_mpls_evpn_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "MIM", &f_mim_bmp));
    BCM_IF_ERROR_RETURN
        (_bcm_udf_flex_feature_chunk_bmap_get(unit, "LLC", &f_llc_bmp));

    if ((1 == soc_property_get(unit, spn_NSH_DISABLE,0))
            && SOC_REG_IS_VALID(unit, ING_NSH_ETHERTYPEr)) {
        rv = soc_reg32_get(unit, ING_NSH_ETHERTYPEr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if (SOC_REG_FIELD_VALID(unit, ING_NSH_ETHERTYPEr,
                                ENABLEf)) {
            nsh_enable = soc_reg_field_get(unit, ING_NSH_ETHERTYPEr, reg_value,
                                   ENABLEf);
        }
    }
    if ((1 == soc_property_get(unit, spn_MPLS_EVPN_DISABLE, 0))
            && SOC_REG_IS_VALID(unit, MPLS_EVPN_CONTROLr)) {
        rv = soc_reg32_get(unit, MPLS_EVPN_CONTROLr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if (SOC_REG_FIELD_VALID(unit, MPLS_EVPN_CONTROLr,
                                ENABLE_EVPN_LABEL_EXTRACTIONf)) {
            mpls_evpn_enable = soc_reg_field_get(unit, MPLS_EVPN_CONTROLr,
                                   reg_value,
                                   ENABLE_EVPN_LABEL_EXTRACTIONf);
        }
    }
    if ((1 == soc_property_get(unit, spn_MIM_DISABLE, 0))
            && SOC_REG_IS_VALID(unit, EGR_FLEX_CONFIGr)) {
        rv = soc_reg32_get(unit, EGR_FLEX_CONFIGr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if (SOC_REG_FIELD_VALID(unit, EGR_FLEX_CONFIGr,
                                ENABLE_FLEX_MIM_ISID_EXTRACTIONf)) {
            mim_enable = soc_reg_field_get(unit, EGR_FLEX_CONFIGr, reg_value,
                                   ENABLE_FLEX_MIM_ISID_EXTRACTIONf);
        }
    }
    if (SOC_REG_IS_VALID(unit, ING_FLEX_CONFIGr)) {
        rv = soc_reg32_get(unit, ING_FLEX_CONFIGr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if ((1 == soc_property_get(unit, spn_LLC_DISABLE, 0))
                && SOC_REG_FIELD_VALID(unit, ING_FLEX_CONFIGr,
                                ENABLE_LLC_HDR_EXTRACTIONf)) {
            llc_enable = soc_reg_field_get(unit, ING_FLEX_CONFIGr, reg_value,
                                   ENABLE_LLC_HDR_EXTRACTIONf);
        }
        if ((1 == soc_property_get(unit, spn_GPE_IOAM_DISABLE, 0))
                && SOC_REG_FIELD_VALID(unit, ING_FLEX_CONFIGr,
                                GPE_IOAM_ENABLEf)) {
            gpe_ioam_enable = soc_reg_field_get(unit, ING_FLEX_CONFIGr,
                                   reg_value,
                                   GPE_IOAM_ENABLEf);
        }
        if ((1 == soc_property_get(unit, spn_IPSEC_SPI_DISABLE, 0))
                && SOC_REG_FIELD_VALID(unit, ING_FLEX_CONFIGr,
                                ENABLE_IPSEC_SPI_EXTRACTIONf)) {
            ipsec_spi_enable = soc_reg_field_get(unit, ING_FLEX_CONFIGr,
                                    reg_value,
                                    ENABLE_IPSEC_SPI_EXTRACTIONf);
        }
    }
    if ((1 == soc_property_get(unit, spn_GBP_DISABLE, 0))
            && SOC_REG_IS_VALID(unit, ING_GBP_ETHERTYPEr)) {
        rv = soc_reg32_get(unit, ING_GBP_ETHERTYPEr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if (SOC_REG_FIELD_VALID(unit, ING_GBP_ETHERTYPEr,
                                ENABLEf)) {
            gbp_sid_enable = soc_reg_field_get(unit, ING_GBP_ETHERTYPEr,
                                  reg_value,
                                  ENABLEf);
        }
    }
    if ((0 == soc_property_get(unit, spn_INT_DISABLE, 0))
            && SOC_REG_IS_VALID(unit, INT_PARSE_CONTROLr)) {
        rv = soc_reg32_get(unit, INT_PARSE_CONTROLr, REG_PORT_ANY,0,
                           &reg_value);
        BCM_IF_ERROR_RETURN(rv);
        if (SOC_REG_FIELD_VALID(unit, INT_PARSE_CONTROLr,
                                PROBE_MARKER_BASED_ENABLEf)) {
            int_enable = soc_reg_field_get(unit, INT_PARSE_CONTROLr, reg_value,
                                   PROBE_MARKER_BASED_ENABLEf);
        }
    }

    if (0 == nsh_enable) {
        unavail_chunk_bmap |= f_nsh_bmp;
    } else {
        avail_chunk_bmap |= f_nsh_bmp;
    }
    if (0 == mpls_evpn_enable) {
        unavail_chunk_bmap |= f_mpls_evpn_bmp;
    } else {
        avail_chunk_bmap |= f_mpls_evpn_bmp;
    }
    if (0 == mim_enable) {
        unavail_chunk_bmap |= f_mim_bmp;
    } else {
        avail_chunk_bmap |= f_mim_bmp;
    }
    if (0 == gpe_ioam_enable) {
        unavail_chunk_bmap |= f_gpe_ioam_bmp;
    } else {
        avail_chunk_bmap |= f_gpe_ioam_bmp;
    }
    if (0 == llc_enable) {
        unavail_chunk_bmap |= f_llc_bmp;
    } else {
        avail_chunk_bmap |= f_llc_bmp;
    }
    if (0 == ipsec_spi_enable) {
        unavail_chunk_bmap |= f_ipsec_spi_bmp;
    } else {
        avail_chunk_bmap |= f_ipsec_spi_bmp;
    }
    if (0 == gbp_sid_enable) {
        unavail_chunk_bmap |= f_gbp_sid_bmp;
    } else {
        avail_chunk_bmap |= f_gbp_sid_bmp;
    }
    if (0 == int_enable) {
        unavail_chunk_bmap |= f_ifa_bmp;
        unavail_chunk_bmap |= f_int_bmp;
    } else {
        avail_chunk_bmap |= f_ifa_bmp;
        avail_chunk_bmap |= f_int_bmp;
    }
    rsrv_bmap = unavail_chunk_bmap & avail_chunk_bmap;

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
            LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                            "UDF Abstract pkt fmt '%s' not found in flex db.\n\r"),
                        _udf_abstr_pkt_fmt_name(pkt_fmt)));
            continue;
        }
        if (3 == pkt_fmt_hw_info->parser) {
            /*
             * H/W Limitation : Parser 0 abstract fmt should be realized with only 2 chunks
             */
            continue;
        }

        if (0 == nsh_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_nsh_bmp & (~rsrv_bmap));
        }
        if (0 == mpls_evpn_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_mpls_evpn_bmp & (~rsrv_bmap));
        }
        if (0 == mim_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_mim_bmp & (~rsrv_bmap));
        }
        if (0 == gpe_ioam_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_gpe_ioam_bmp & (~rsrv_bmap));
        }
        if (0 == llc_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_llc_bmp & (~rsrv_bmap));
        }
        if (0 == ipsec_spi_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_ipsec_spi_bmp & (~rsrv_bmap));
        }
        if (0 == gbp_sid_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~(f_gbp_sid_bmp & (~rsrv_bmap));
        }
        if (0 == int_enable) {
            pkt_fmt_hw_info->unavail_chunk_bmap &= ~((f_ifa_bmp | f_int_bmp) & (~rsrv_bmap));
        }
    }

     return BCM_E_NONE;
}

/* Returns Number of chunks present in the chunk bitmap. No error check */
STATIC
int _bcm_udf_count_chunks(uint32 bmp)
{
    uint8 ct = 0;

    if (0 == bmp) {
        /* Nothing to count, return 0. */
        return 0;
    }

    /* Count always starts from 1 as single set bit bitmaps can't be processed */
    for (ct = 1; (bmp = (bmp & (bmp - 1))); ct++) {
    }

    return ct;
}

int
_bcm_udf_abstr_pkt_fmt_flex_init(int unit)
{
    int rv;
    uint32 stg, num_stage;
    uint32 *hfe_prof_ptr_arr[_BCM_UDF_MAX_PARSER_STAGES];
    uint32 hfe_prof_ptr_arr_len[_BCM_UDF_MAX_PARSER_STAGES] = {0};
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;
    soc_cancun_udf_stage_info_t udf_ccinfo_arr[_BCM_UDF_MAX_PARSER_STAGES];

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "UDF Abstract pkt fmt '%s' not found in flex db.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt)));
           continue;
        }

        num_stage = 0;
        sal_memset(udf_ccinfo_arr, 0x0,
                   sizeof(soc_cancun_udf_stage_info_t) * _BCM_UDF_MAX_PARSER_STAGES);
        rv = soc_cancun_udf_abstr_type_info_get(unit, pkt_fmt,
                                                pkt_fmt_hw_info->parser,
                                                0, NULL, NULL,
                                                &num_stage,
                                                hfe_prof_ptr_arr_len);
        if (SOC_SUCCESS(rv)) {
           if (hfe_prof_ptr_arr_len[0] == 0) {
              LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "UDF Abstract pkt fmt '%s' has null hfe_prof_ptr.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt)));
              continue;
           }

           for (stg = 0; stg < num_stage; stg++) {
               hfe_prof_ptr_arr[stg] = NULL;
               _BCM_UDF_ALLOC(hfe_prof_ptr_arr[stg],
                              sizeof(uint32) * hfe_prof_ptr_arr_len[stg],
                              "UDF Hfe Prof Ptr Alloc.");
           }

           rv = soc_cancun_udf_abstr_type_info_get(unit, pkt_fmt,
                                                pkt_fmt_hw_info->parser,
                                                num_stage, hfe_prof_ptr_arr,
                                                udf_ccinfo_arr,
                                                NULL, NULL);
        }

        if (SOC_FAILURE(rv)) {
           if (rv == SOC_E_NOT_FOUND) {
              LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "UDF Abstract pkt fmt '%s' not found in flex db.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt)));

              continue;
           }
           LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                     "UDF Flex DB Error for Abstract pkt fmt '%s'.\n\r"),
                     _udf_abstr_pkt_fmt_name(pkt_fmt)));
           continue;
        }

        pkt_fmt_hw_info->num_stages = num_stage;
        for (stg = 0; stg < num_stage; stg++) {
            pkt_fmt_hw_info->base_offset = udf_ccinfo_arr[stg].start_pos;
            rv = _udf_td3_hme_to_hfe_mem_get(udf_ccinfo_arr[stg].policy_mem,
                                     &pkt_fmt_hw_info->stage_policy_mem[stg]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "Unknown Policy Mem from flex db:%d for abstr_type:[%s]\n\r"),
                          pkt_fmt_hw_info->stage_policy_mem[stg], _udf_abstr_pkt_fmt_name(pkt_fmt)));
                return rv;
            }

            pkt_fmt_hw_info->hfe_prof_ptr_arr_len[stg] = udf_ccinfo_arr[stg].hfe_prof_ptr_arr_len;
            pkt_fmt_hw_info->hfe_profile_ptr[stg] = udf_ccinfo_arr[stg].hfe_profile_ptr;
            pkt_fmt_hw_info->num_cmds[stg] = (udf_ccinfo_arr[stg].size)/2;
            pkt_fmt_hw_info->max_extract_bytes += (pkt_fmt_hw_info->num_cmds[stg] * 2);
            rv = _udf_td3_opaque_cont_to_chunk_bmap(udf_ccinfo_arr[stg].unavail_contbmap,
                                                    &pkt_fmt_hw_info->unavail_chunk_bmap);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "Unknown Unavail Opaque container from flex db:0x%x for abstr_type:[%s]\n\r"),
                          udf_ccinfo_arr[stg].unavail_contbmap, _udf_abstr_pkt_fmt_name(pkt_fmt)));
                return rv;
            }

            if (((_BCM_UDF_TD3_MAX_CHUNKS/2)==
                        pkt_fmt_hw_info->num_cmds[stg])
                    && (num_stage > 1)
                    && (pkt_fmt_hw_info->unavail_chunk_bmap)) {
                /*
                 * As it is Stage based, There could be possibility of 
                 * Cumulative max_extract_bytes equal to max avail in the device
                 * Like 16 bytes in stage 1 and 16 bytes in stage 2
                 * makes 32 bytes of extraction support.
                 * But the same abstarct fmt can have few chunks unavilable.
                 * So need to reduce the unavailable bytes from
                 * the Max allowed extract bytes in this scenario.
                 */
                pkt_fmt_hw_info->max_extract_bytes -=
                                    _BCM_UDF_CHUNK_TO_BYTE(_bcm_udf_count_chunks(
                                            pkt_fmt_hw_info->unavail_chunk_bmap));
            }
            LOG_DEBUG(BSL_LS_BCM_UDF, (BSL_META_U(unit,
               "UDF Abstract pkt fmt %d-'%s': max_extract_bytes:%d"
                 " hfe_profile_ptr:%d mem:%d unavail_chunk_bmap:0x%x.\n\r"), pkt_fmt,
                 _udf_abstr_pkt_fmt_name(pkt_fmt), pkt_fmt_hw_info->max_extract_bytes,
                 pkt_fmt_hw_info->hfe_profile_ptr[stg][0], pkt_fmt_hw_info->stage_policy_mem[stg],
                 pkt_fmt_hw_info->unavail_chunk_bmap));
        }
    }

    if (soc_feature(unit, soc_feature_enable_flow_based_udf_extraction)) {
        BCM_IF_ERROR_RETURN
            (_bcm_udf_flex_feature_based_unavail_chunk_init(unit));
    }

    return BCM_E_NONE;
}

/*
 * Initialize Abstract Packet Formats with offset and default values.
 */
int
_bcm_udf_abstr_pkt_format_info_init(int unit)
{
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _BCM_UDF_ABSTRACT_PKT_FORMAT_MAPPING_DECL
    /* Validate whether UDF module is attached */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
        if (abstr_pkt_fmt_hw_info[unit][pkt_fmt] != NULL) {
           sal_free(abstr_pkt_fmt_hw_info[unit][pkt_fmt]);
           abstr_pkt_fmt_hw_info[unit][pkt_fmt] = NULL;
        }

        _BCM_UDF_ALLOC(abstr_pkt_fmt_hw_info[unit][pkt_fmt],
                   sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t),
                   "UDF Abstract Pkt Format INIT");
        if (abstr_pkt_fmt_hw_info[unit][pkt_fmt] == NULL) {
           return BCM_E_MEMORY;
        }
    }

    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatLlc, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerLlc, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownNonIp, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerKnownNonIp, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3Mim, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3FcoeStdEncap, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3FcoeVftIfr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3MplsOneLabel, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3MplsTwoLabel, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3MplsThreeLabel, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3MplsFourLabel, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3MplsMoreThanFourLabel, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL4, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL4, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpUnknownL5, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpUnknownL5WithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdpUnknownL5WithoutExtnHdr, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdpUnknownL5, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpGpeGeneve, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpBfd, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpBfdWithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdpBfd, 2);
    /* _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpBfdUnknown, 1); */
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpVxlan, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpVxlanWithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdp1588, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdp1588WithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdp1588WithoutExtnHdr, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatTcpUnknownL5, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerTcpUnknownL5, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerTcpUnknownL5WithoutExtnHdr, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatSctpUnknownL5, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerSctpUnknownL5, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreWithoutKey, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreWithKey, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreWithoutChecksumRouting, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreWithoutChecksumRouting, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreWithoutRouting, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatAchNonIp, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpINT, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpINTWithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatTcpUnknownL5WithIpExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3GreUnknownPayload, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreWithKeyRouting, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreWithKeyRouting, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdp1588, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatTcpINT, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownL3FcoeUnknownRctl, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd0, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd0EHType0, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd0EHType1, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd0EHType2, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd2, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd2EHType0, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd2EHType1, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatHiGigPpd2EHType2, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGpeIOAM, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGpeIOAMEdgeToEdge, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatKnownMplsMcast, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatLoopbackHdrType0, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatLoopbackHdrType1, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3Ipv4Fragmented, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3Ipv4FragmentedWithOptions, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3Ipv4WithoutOptions, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3Ipv4WithOptions, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL3Ipv6, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2Untagged, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2OuterVlanTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2InnerVlanTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2DoubleVlanTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GbpTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2OuterVlanWithGbpTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2InnerVlanWithGbpTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2DoubleVlanWithGbpTag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2Vntag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2OuterVlanWithVntag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2InnerVlanWithVntag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2DoubleVlanWithVntag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2Etag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2OuterVlanEtag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2InnerVlanEtag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2DoubleVlanEtag, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatMacsecDecrypt, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL3Ipv4, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL3Ipv4Fragmented, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL3Ipv6, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2Untagged, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2OuterVlanTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2InnerVlanTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2DoubleVlanTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2GbpTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2OuterVlanWithGbpTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2InnerVlanWithGbpTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerL2DoubleVlanWithGbpTag, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomEtherType1, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomEtherType2, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomIpProtocol1, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomIpProtocol2, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomUdpDstPort1, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUnknownL3CustomUdpDstPort2, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomEtherType1, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomEtherType2, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomIpProtocol1, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomIpProtocol2, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomUdpDstPort1, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUnknownL3CustomUdpDstPort2, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpProbeBasedINT, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatUdpProbeBasedINTWithoutExtnHdr, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatInnerUdpBfdWithoutExtnHdr, 2);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatIfaUdp, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatIfaTcp, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatNshOverL2, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatCesBackplane, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatLoopbackType1WithDropIndication, 3);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreHdrOfFourBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreHdrOfEightBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreHdrOfTwelveBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreHdrOfSixteenBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatGreHdrOfTwentyBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreHdrOfFourBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreHdrOfEightBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreHdrOfTwelveBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreHdrOfSixteenBytes, 1);
    _BCM_UDF_ABSTRACT_PKT_FORMAT_CFG_INIT(unit, bcmUdfAbstractPktFormatL2GreHdrOfTwentyBytes, 1);

    return _bcm_udf_abstr_pkt_fmt_flex_init(unit);
}


/*
 * Reset HFE Profile indices for all abstract pkt formats.
 */
STATIC int
_bcm_udf_td3_hfe_profile_hw_init(int unit)
{
    int rv;
    int len;
    uint8 stage;
    soc_format_t fmt;
    soc_mem_t policy_mem;
    uint8 buf[100];
    uint32 hfe_profile_ptr;
    uint8 *cmd_pol_data_buf = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        for (stage = 0; stage < pkt_fmt_hw_info->num_stages; stage++) {
            policy_mem = pkt_fmt_hw_info->stage_policy_mem[stage];
            hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr[stage][0];

            if (policy_mem == INVALIDm) {
               LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "Invalid Policy mem for Abstract pkt fmt '%s', stage:%d.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt), stage));
               return BCM_E_INTERNAL;
            }

            _UDF_TD3_GET_HFE_CMD_FORMAT(pkt_fmt_hw_info->parser, fmt);
            if (fmt == INVALIDfmt) {
               return BCM_E_INTERNAL;
            }

            sal_memset(buf, 0x0, sizeof(buf));
            rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                              hfe_profile_ptr, (uint32 *) buf);
            BCM_IF_ERROR_RETURN(rv);

            len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);
            cmd_pol_data_buf = NULL;
            _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF default CMD Policy data");
            if (cmd_pol_data_buf == NULL) {
               return BCM_E_MEMORY;
            }

            soc_mem_field_get(unit, policy_mem, (uint32 *)buf,
                          CMD_POLICY_DATAf, (uint32 *)cmd_pol_data_buf);

            pkt_fmt_hw_info->cmd_policy_data[stage] = cmd_pol_data_buf;

#if 0
            for (i = 0; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
                soc_format_field32_set(unit, fmt, (uint32 *)f_buf,
                                                valid_cmds[i], 0);
            }

            soc_mem_field_set(unit, policy_mem, (uint32 *)buf,
                              CMD_POLICY_DATAf, (uint32 *)f_buf);

            rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ANY,
                               hfe_profile_ptr, (uint32 *)buf);
            BCM_IF_ERROR_RETURN(rv);
#endif

        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_init
 * Purpose:
 *      initializes/ReInitializes the UDF module.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_init(int unit)
{
    int rv;

    /* Init control structures */
    rv = _bcm_udf_td3_ctrl_init(unit);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /*
     * Initialize Abstract Packet Formats with offset
     * and default values.
     */
    rv = _bcm_udf_abstr_pkt_format_info_init(unit);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /* Initialize hardware HFE Profile Indices with default values */
    rv = _bcm_udf_td3_hfe_profile_hw_init(unit);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);
#if defined (BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_udf_td3_reinit(unit);
        BCM_IF_ERROR_GOTO_CLEANUP(rv);
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
#if defined (BCM_WARM_BOOT_SUPPORT)
        rv = _bcm_udf_td3_wb_alloc(unit);
        BCM_IF_ERROR_GOTO_CLEANUP(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    return BCM_E_NONE;

cleanup:
    (void) _bcm_udf_td3_detach(unit);
    return rv;
}

STATIC int
_bcm_udf_td3_abstr_pkt_fmt_hw_info_insert(int unit, bcm_udf_abstract_pkt_format_t abstr_pkt_fmt,
                                          _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_format_info)
{
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info = NULL;

    if (pkt_format_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, abstr_pkt_fmt);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    sal_memcpy(pkt_fmt_hw_info, pkt_format_info,
               sizeof(_bcm_udf_td3_abstr_pkt_fmt_hw_info_t));

    return BCM_E_NONE;
}

/* Routine to retrieve chunks from the bitmap. */
int
_udf_chunk_bmap_to_chunks(uint32 chunk_bmap, uint8 *num_chunks, uint8 *chunk_arr)
{
    uint8 bit, i;
    uint8 num = 0;

    for (i = 0; i < _BCM_UDF_TD3_MAX_CHUNKS; i++) {
        bit = (chunk_bmap >> i) & 1;
        if (bit) {
           if (chunk_arr != NULL) {
              chunk_arr[num] = i;
           }
           num++;
        }
    }

    *num_chunks = num;

    return BCM_E_NONE;
}

/* To sanitize udf_info/pkt_format_info input structures */
STATIC int
_udf_td3_sanitize_inputs(int unit, bcm_udf_chunk_info_t *udf_info,
                         bcm_udf_alloc_hints_t *hints,
                         bcm_udf_id_t *udf_id)
{
    uint8 num_chunks = 0;

    /* NULL check */
    BCM_IF_NULL_RETURN_PARAM(udf_id);
    BCM_IF_NULL_RETURN_PARAM(udf_info);

    /* start and width should be at byte boundary */
    if (((udf_info->offset % 8) != 0) || ((udf_info->width % 8) != 0)) {
       return BCM_E_PARAM;
    }

    /* start and width should be less than max byte selection */
    if (BITS2BYTES(udf_info->offset + udf_info->width) >
                   UDF_CONTROL(unit)->parse_limit) {
       return BCM_E_PARAM;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(unit, udf_info->abstract_pkt_format);

    /* Extraction bytes (width) Validate */
    _BCM_UDF_EXTRACTION_BYTES_VALIDATE(unit, udf_info->abstract_pkt_format,
                                       BITS2BYTES(udf_info->width));

    if (hints != NULL) {
        /* Replace flag is not supported. */
        if (hints->flags & BCM_UDF_CREATE_O_REPLACE) {
            return BCM_E_PARAM;
        }

        /* Validate UDF ID */
        if (hints->flags & BCM_UDF_CREATE_O_WITHID) {
           _BCM_UDF_ID_VALIDATE(unit, *udf_id);
        }

        /* check if chip supports Range Check */
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
           /* Check if device supports Range Check. */
           if (!(UDF_TD3_CTRL(unit)->flags & _BCM_UDF_TD3_CTRL_RANGE_CHECK)) {
              return BCM_E_PARAM;
           }

           /*
            * If RANGE_CHECK Flag is set, chunk bitmap should be
            * as per the device supported chunks.
            */
           if (udf_info->chunk_bmap != _BCM_UDF_TD3_RANGE_CHUNK_BMAP) {
              return BCM_E_PARAM;
           }
        }
    }

    /* Chunk Validate */
    /* Validate whether the given chunk bmap is part of reserved chunk bitmap */
    _BCM_UDF_CHUNK_VALIDATE(unit, udf_info->chunk_bmap, udf_info->abstract_pkt_format);

    /* Validate given chunk_bmap matches with width */
    _udf_chunk_bmap_to_chunks(udf_info->chunk_bmap, &num_chunks, NULL);
    if (BITS2BYTES(udf_info->width) != _BCM_UDF_CHUNK_TO_BYTE(num_chunks)) {
       return BCM_E_PARAM;
    }

    if (hints != NULL) {
        /* Validate whether given chunk bmap used by qualifiers in given qset */
        SOC_IF_ERROR_RETURN(_bcm_field_td3_qset_udf_chunk_validate(unit, udf_info, hints));
    }

    /* Check whether chunks given in the bitmap are already in use */
    _BCM_UDF_OBJECT_CHUNK_IN_USE_VALIDATE(unit, udf_info->abstract_pkt_format,
                                          udf_info->chunk_bmap);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_add
 * Purpose:
 *      To convert the UDF info to object info and add to the linked list.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_info        - (IN)      UDF info to be added to linked list.
 *      object_info     - (INOUT)   Reference to allocated object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_object_info_add(int unit, bcm_udf_chunk_info_t *udf_info,
                             _bcm_udf_td3_obj_info_t **obj_info)
{
    /* Allocate memory for UDF object info. */
    _BCM_UDF_ALLOC(*obj_info, sizeof(_bcm_udf_td3_obj_info_t),
                   "UDF object info.");
    if (*obj_info == NULL) {
       return BCM_E_MEMORY;
    }

    /* Insert user inputs into the object_info struct */
    (*obj_info)->width = BITS2BYTES(udf_info->width);
    (*obj_info)->offset = BITS2BYTES(udf_info->offset);
    (*obj_info)->chunk_bmap = udf_info->chunk_bmap;
    (*obj_info)->abstr_pkt_fmt = udf_info->abstract_pkt_format;
    (*obj_info)->next = NULL;
    (*obj_info)->prev = NULL;

    /* Add obj_info item to the linked list */
    _BCM_UDF_OBJ_NODE_ADD(unit, (*obj_info),
                          UDF_TD3_CTRL(unit)->udf_obj_head);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_delete
 * Purpose:
 *      Remove the UDF object info from the database for a given object.
 * Parameters:
 *      unit         - (IN)  Unit number.
 *      obj_info     - (IN)  Reference to allocated object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_object_info_delete(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
    if (obj_info == NULL) {
       return BCM_E_PARAM;
    }

    /* Delete obj_info from the linked list */
    _BCM_UDF_OBJ_NODE_DEL(unit, obj_info, UDF_TD3_CTRL(unit)->udf_obj_head);
    _BCM_UDF_FREE(obj_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_udf_td3_object_info_get
 * Purpose:
 *      Retrieves UDF object info for the given udf id.
 * Parameters:
 *      unit         - (IN)      Unit number.
 *      udf_id       - (IN)      UDF Id.
 *      obj_info     - (INOUT)   Retrieve udf object info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_udf_td3_object_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    _bcm_udf_td3_obj_info_t **obj_info)
{
    _BCM_UDF_OBJ_NODE_GET(unit, udf_id,
                          UDF_TD3_CTRL(unit)->udf_obj_head,
                          *obj_info);

    if (*obj_info == NULL) {
       return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * To allocate running id for UDF IDs.
 */
static int
_bcm_udf_td3_object_running_id_alloc(int unit, bcm_udf_id_t *udf_id)
{
    int rv;
    int id;
    int min_id, max_id;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    min_id = UDF_CONTROL(unit)->min_obj_id;
    max_id = UDF_CONTROL(unit)->max_obj_id;

    id = _BCM_UDF_OBJ_ID_RUNNING(unit);

    if (id > max_id) {
        for (id = min_id; id <= max_id; id++) {
            rv = _bcm_udf_td3_object_info_get(unit, id, &obj_info);
            if (BCM_E_NOT_FOUND == rv) {
               break;
            }

            if (BCM_FAILURE(rv)) {
               return BCM_E_INTERNAL;
            }
        }

        /* All the udf ids are used up */
        if (id > max_id) {
           return BCM_E_FULL;
        }
    }

    *udf_id = id;

    return BCM_E_NONE;
}

static int _udf_parser0_chunk_to_cmd_get(int unit, uint32 chunk_bmap,
                                         uint16 *cmd_bmap)
{
    uint8 chunk;

    chunk = _bcm_udf_opaque_cont_to_chunk_mapping[2];
    if (chunk_bmap & (1 << chunk)) {
        /* CMD8 is indexed at (_BCM_UDF_MAX_POLICY_CMDS - 2) */
        *cmd_bmap |= (1 << (_BCM_UDF_MAX_POLICY_CMDS - 2));
    }

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        chunk = _bcm_udf_opaque_cont_to_chunk_mapping[1];
    } else
#endif
    {
        chunk = _bcm_udf_opaque_cont_to_chunk_mapping[0];
    }
    if (chunk_bmap & (1 << chunk)) {
        /* CMD9 is indexed at (_BCM_UDF_MAX_POLICY_CMDS - 1) */
        *cmd_bmap |= (1 << (_BCM_UDF_MAX_POLICY_CMDS - 1));
    }

    return BCM_E_NONE;
}

static int
_udf_td3_hw_policy_cmd_alloc(
    int unit,
    _bcm_udf_td3_obj_info_t *obj_info,
    _udf_obj_pkt_fmt_hw_cfg_t *cfg_data)
{
   int i;
   uint8 stage;
   uint8 start_cmd;
   uint8 num_cmds_required;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
   if (hw_info == NULL) {
      return BCM_E_INTERNAL;
   }

   if (hw_info->chunk_bmap_used & obj_info->chunk_bmap) {
      return BCM_E_PARAM;
   }

   BCM_IF_ERROR_RETURN(_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                                 &cfg_data->num_chunks,
						 cfg_data->chunk_arr));

   num_cmds_required = (hw_info->parser == _BCM_UDF_PARSER0) ?
                                        0 : obj_info->width/2;

   for (stage = 0; stage < hw_info->num_stages; stage++) {
      if (hw_info->stage_policy_mem[stage] != INVALIDm) {
          if (hw_info->parser == _BCM_UDF_PARSER0) {
              BCM_IF_ERROR_RETURN(_udf_parser0_chunk_to_cmd_get(unit,
                                      obj_info->chunk_bmap,
                                      &cfg_data->cmd_bmap[stage]));
              hw_info->valid_cmd_bmap[stage] |= cfg_data->cmd_bmap[stage];
              continue;
          }

          start_cmd = _BCM_UDF_MAX_POLICY_CMDS - hw_info->num_cmds[stage];
          for (i = start_cmd; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
             if (num_cmds_required == 0) {
                break;
             }

             /* Allocate CMD if the bit is not set */
             if ((hw_info->valid_cmd_bmap[stage] & (1 << i)) == 0) {
                cfg_data->cmd_bmap[stage] |= (1 << i);
                num_cmds_required--;
             }
          }
          hw_info->valid_cmd_bmap[stage] |= cfg_data->cmd_bmap[stage];
      }
   }

   if (num_cmds_required != 0) {
      return BCM_E_INTERNAL;
   }

   return BCM_E_NONE;
}

/*
 * Install HW Policy Commands for given UDF object.
 */
static int
_udf_td3_hw_policy_cmd_install(
    int unit,
    _bcm_udf_td3_obj_info_t *obj_info,
    _udf_obj_pkt_fmt_hw_cfg_t *cfg_data)
{
   int rv;
   uint8 i;
   uint8 gran = 2;
   uint8 chunk;
   uint8 stage;
   uint8 chunk_ct = 0;
   uint8 curr_chunk = 0;
   uint8 hfe_ct;
   uint8 start_cmd;
   uint32 offset = 0;
   uint32 curr_offset = 0;
   uint32 dest_cont_id;
   int fmt_fld_len = 0;
   soc_format_t fmt;
   uint32 cancun_ver;
   uint8 skip_hw_change = 0;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   uint8 f_buf[_BCM_UDF_MAX_PARSER_STAGES][50];
   uint8 buf[_BCM_UDF_MAX_PARSER_STAGES][100];
   uint16 cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES];
   uint8 null_update[_BCM_UDF_MAX_PARSER_STAGES] = {0};
   soc_mem_t policy_mem[_BCM_UDF_MAX_PARSER_STAGES];
   uint32 hfe_profile_ptr[_BCM_UDF_MAX_PARSER_STAGES];
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);

   offset = obj_info->offset;

   sal_memset(f_buf, 0x0, sizeof(f_buf));
   sal_memset(buf, 0x0, sizeof(buf));

   BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
   if (SOC_CANCUN_VERSION_5_2_UNDER_SERIES(cancun_ver)) {
       if ((pkt_fmt == bcmUdfAbstractPktFormatUdpINT) ||
           (pkt_fmt == bcmUdfAbstractPktFormatTcpINT)) {
           skip_hw_change = 1;
       }
   }

   for (stage = 0; stage < hw_info->num_stages; stage++) {
       cmd_bmap[stage] = cfg_data->cmd_bmap[stage];
       policy_mem[stage] = hw_info->stage_policy_mem[stage];

       /*
        * Currently parser0 supports only 2 commands 8 & 9.
        * As src_pkt_offset_p0_cmds array has these commands as last 2 members of it.
        * Always start_cmd should be _BCM_UDF_MAX_POLICY_CMDS - 2.
        */
       if (hw_info->parser == _BCM_UDF_PARSER0) {
           start_cmd = _BCM_UDF_MAX_POLICY_CMDS - 2;
       } else {
           start_cmd = _BCM_UDF_MAX_POLICY_CMDS - hw_info->num_cmds[stage];
       }

       if (policy_mem[stage] == INVALIDm) {
          LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                   "Invalid Policy mem for Abstract pkt fmt '%s', stage:%d.\n\r"),
                   _udf_abstr_pkt_fmt_name(pkt_fmt), stage));
          return BCM_E_INTERNAL;
       }

       if (skip_hw_change == 1) {
          continue;
       }

       _UDF_TD3_GET_HFE_CMD_FORMAT(hw_info->parser, fmt);
       if (fmt == INVALIDfmt) {
          return BCM_E_INTERNAL;
       }

       null_update[stage] = 0;
       curr_chunk = chunk_ct;
       curr_offset = offset;
       for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stage]; hfe_ct++) {
           hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr[stage][hfe_ct];;

           rv = soc_mem_read(unit, policy_mem[stage], MEM_BLOCK_ANY,
                             hfe_profile_ptr[stage], (uint32 *) buf[stage]);
           BCM_IF_ERROR_RETURN(rv);

           soc_mem_field_get(unit, policy_mem[stage], (uint32 *)buf[stage],
                             CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);

           for (i = start_cmd; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
               if (cmd_bmap[stage] & (1 << i)) {
                   /* Check for valid offset */
                  if (hw_info->parser == _BCM_UDF_PARSER0) {
                       fmt_fld_len = soc_format_field_length(unit, fmt,
                                                   src_pkt_offset_p0_cmds[i]);
                      if ((((offset + 1) * 8) >> fmt_fld_len)) {
                          LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                                          "Offset for Abstract pkt fmt '%s' is out of range."
                                          "Max value : %d\n\r"),
                                      _udf_abstr_pkt_fmt_name(pkt_fmt),
                                      ((1 << fmt_fld_len) - 1)));
                          return BCM_E_PARAM;
                      }
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                        src_pkt_offset_p0_cmds[i], offset * 8);
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                          valid_p0_cmds[i], 1);
                  } else {
                      fmt_fld_len = soc_format_field_length(unit, fmt,
                                                  src_pkt_offset_cmds[i]);
                      if ((offset + 1) >> fmt_fld_len) {
                          LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                                          "Offset for Abstract pkt fmt '%s' is out of range."
                                          "Max value : %d\n\r"),
                                      _udf_abstr_pkt_fmt_name(pkt_fmt),
                                      ((1 << fmt_fld_len) - 1)));
                          return BCM_E_PARAM;
                      }
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                               src_pkt_offset_cmds[i], offset);
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                             valid_cmds[i], 1);
                      if (chunk_ct <= cfg_data->num_chunks) {
                         chunk = cfg_data->chunk_arr[chunk_ct];
                         dest_cont_id = _bcm_udf_chunk_to_dest_cont_id_mapping[chunk];
                         soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                               dest_cont_id_cmds[i], dest_cont_id);
                         chunk_ct++;
                      } else {
                         return BCM_E_INTERNAL;
                      }
                  }
                  offset += gran;
               } else if (hw_info->chunk_bmap_used == 0) {
                  if (hw_info->parser == _BCM_UDF_PARSER0) {
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                          valid_p0_cmds[i], 0);
                  } else {
                      soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                             valid_cmds[i], 0);
                  }
                  null_update[stage] = 1;
               }
           }

           if (cmd_bmap[stage] || (null_update[stage])) {
              soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                             CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);
           }

           if ((hfe_ct + 1) < hw_info->hfe_prof_ptr_arr_len[stage]) {
               chunk_ct = curr_chunk;
               offset = curr_offset;
           }
       }
   }

   if (skip_hw_change == 0) {
      for (stage = 0; stage < hw_info->num_stages; stage++) {
          for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stage]; hfe_ct++) {
              hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr[stage][hfe_ct];;
              if (cmd_bmap[stage] || (null_update[stage])) {
                 rv = soc_mem_write(unit, policy_mem[stage], MEM_BLOCK_ANY,
                         hfe_profile_ptr[stage], (uint32 *)buf[stage]);
                 BCM_IF_ERROR_RETURN(rv);
              }
          }
      }
   }

   for (stage = 0; stage < hw_info->num_stages; stage++) {
       obj_info->hw_cmd_bmap[stage] = cfg_data->cmd_bmap[stage];
   }
   hw_info->extracted_bytes += _BCM_UDF_CHUNK_TO_BYTE(cfg_data->num_chunks);
   hw_info->chunk_bmap_used |= obj_info->chunk_bmap;
   return BCM_E_NONE;
}

/*
 * Un-install HW Policy Commands for given UDF object.
 */
STATIC
int _udf_td3_hw_policy_cmd_uninstall(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
   int rv;
   uint8 stage, i, hfe_ct;
   uint8 num_chunks = 0;
   uint8 start_cmd;
   uint8 skip_hw_init = 0;
   soc_format_t fmt;
   uint32 cancun_ver;
   uint8 *cmd_policy_data = NULL;
   bcm_udf_abstract_pkt_format_t pkt_fmt;
   uint8 f_buf[_BCM_UDF_MAX_PARSER_STAGES][50];
   uint8 buf[_BCM_UDF_MAX_PARSER_STAGES][100];
   uint16 cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES];
   soc_mem_t policy_mem[_BCM_UDF_MAX_PARSER_STAGES];
   uint32 hfe_profile_ptr[_BCM_UDF_MAX_PARSER_STAGES];
   uint8 null_update[_BCM_UDF_MAX_PARSER_STAGES] = {0};
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

   pkt_fmt = obj_info->abstr_pkt_fmt;
   hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);

   BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
   if (SOC_CANCUN_VERSION_5_2_UNDER_SERIES(cancun_ver)) {
       if ((pkt_fmt == bcmUdfAbstractPktFormatUdpINT) ||
           (pkt_fmt == bcmUdfAbstractPktFormatTcpINT)) {
           skip_hw_init = 1;
       }
   }

   if (skip_hw_init == 0) {
      sal_memset(f_buf, 0x0, sizeof(f_buf));
      sal_memset(buf, 0x0, sizeof(buf));

      for (stage = 0; stage < hw_info->num_stages; stage++) {
          cmd_bmap[stage] = obj_info->hw_cmd_bmap[stage];
          policy_mem[stage] = hw_info->stage_policy_mem[stage];
          cmd_policy_data = hw_info->cmd_policy_data[stage];

          if (policy_mem[stage] == INVALIDm) {
             continue;
          }

          _UDF_TD3_GET_HFE_CMD_FORMAT(hw_info->parser, fmt);
          if (fmt == INVALIDfmt) {
             return BCM_E_INTERNAL;
          }

          if (cmd_policy_data == NULL) {
             return BCM_E_INTERNAL;
          }

          for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stage]; hfe_ct++) {
              hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr[stage][hfe_ct];;
              rv = soc_mem_read(unit, policy_mem[stage], MEM_BLOCK_ANY,
                                hfe_profile_ptr[stage], (uint32 *) buf[stage]);
              BCM_IF_ERROR_RETURN(rv);
              soc_mem_field_get(unit, policy_mem[stage], (uint32 *)buf[stage],
                                CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);

              if (hw_info->parser == _BCM_UDF_PARSER0) {
                  start_cmd = _BCM_UDF_MAX_POLICY_CMDS - 2;
              } else {
                  start_cmd = _BCM_UDF_MAX_POLICY_CMDS - hw_info->num_cmds[stage];
              }

              for (i = start_cmd; i < _BCM_UDF_MAX_POLICY_CMDS; i++) {
                 if (cmd_bmap[stage] & (1 << i)) {
                    if (hw_info->parser == _BCM_UDF_PARSER0) {
                        soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                            valid_p0_cmds[i], 0);
                        soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                   src_pkt_offset_p0_cmds[i], 0);
                    } else {
                        soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                               valid_cmds[i], 0);
                        soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                        dest_cont_id_cmds[i], 0);
                        soc_format_field32_set(unit, fmt, (uint32 *)f_buf[stage],
                                                      src_pkt_offset_cmds[i], 0);
                    }
                 }
              }

              if (cmd_bmap[stage]) {
                 soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                                CMD_POLICY_DATAf, (uint32 *)f_buf[stage]);
              }

              if (!(hw_info->chunk_bmap_used & ~(obj_info->hw_cmd_bmap[stage]))) {
                  soc_mem_field_set(unit, policy_mem[stage], (uint32 *)buf[stage],
                                CMD_POLICY_DATAf, (uint32 *)cmd_policy_data);
                  null_update[stage] = 1;
              }
          }
      }

      for (stage = 0; stage < hw_info->num_stages; stage++) {
          for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stage]; hfe_ct++) {
              hfe_profile_ptr[stage] = hw_info->hfe_profile_ptr[stage][hfe_ct];;
              if ((cmd_bmap[stage]) || (null_update[stage] == 1)) {
                 rv = soc_mem_write(unit, policy_mem[stage], MEM_BLOCK_ANY,
                             hfe_profile_ptr[stage], (uint32 *)buf[stage]);
                 BCM_IF_ERROR_RETURN(rv);
              }
          }
      }
   }

   (void)_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                   &num_chunks, NULL);

   /* Update Abstr Pkt Format HW variables. */
   for (stage = 0; stage < hw_info->num_stages; stage++) {
      hw_info->valid_cmd_bmap[stage] &= ~(obj_info->hw_cmd_bmap[stage]);
      obj_info->hw_cmd_bmap[stage] = 0x0;
   }
   hw_info->extracted_bytes -= _BCM_UDF_CHUNK_TO_BYTE(num_chunks);
   hw_info->chunk_bmap_used &= ~(obj_info->chunk_bmap);

   return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_udf_td3_chunk_create
 * Purpose:
 *      Creates a UDF object
 * Parameters:
 *      unit - (IN) Unit number.
 *      hints - (IN) Hints to UDF allocator
 *      udf_info - (IN) UDF Chunk Info structure
 *      udf_id - (IN/OUT) UDF ID
 * Returns:
 *      BCM_E_NONE          UDF created successfully.
 *      BCM_E_EXISTS        Entry already exists.
 *      BCM_E_FULL          UDF table full.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
_bcm_udf_td3_chunk_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_chunk_info_t  *udf_info,
    bcm_udf_id_t *udf_id)
{
    int id_running_allocated = 0;
    int rv = BCM_E_NONE;
    _udf_obj_pkt_fmt_hw_cfg_t cfg_data;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* UDF Device Module Init check */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    /* Sanitize input parameters */
    rv = _udf_td3_sanitize_inputs(unit, udf_info, hints, udf_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Check if similar entry already existing */
    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_WITHID)) {
        /* Retrieve UDF object info; get should be successful */
        rv = _bcm_udf_td3_object_info_get(unit, *udf_id, &obj_info);
        if (BCM_SUCCESS(rv)) {
           return BCM_E_EXISTS;
        }
    } else {
        /* Allocate new udf id */
        rv = _bcm_udf_td3_object_running_id_alloc(unit, udf_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        } else {
            id_running_allocated = 1;
        }
    }

    /* Add udf info structure to linked list */
    rv = _bcm_udf_td3_object_info_add(unit, udf_info, &obj_info);
    if ((BCM_FAILURE(rv)) || (obj_info == NULL)) {
        return rv;
    }

    sal_memset(&cfg_data, 0x0, sizeof(cfg_data));
    /* Allocate udf chunks */
    rv = _udf_td3_hw_policy_cmd_alloc(unit, obj_info, &cfg_data);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /* Install UDF object */
    rv = _udf_td3_hw_policy_cmd_install(unit, obj_info, &cfg_data);
    BCM_IF_ERROR_GOTO_CLEANUP(rv);

    /* update udf_id in the offset_info */
    obj_info->udf_id = *udf_id;

    if (hints != NULL) {
        
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_IFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_VFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
            obj_info->flags |= _BCM_UDF_TD3_OBJ_RANGE_CHECK;
        }
    }

cleanup:
    if (BCM_FAILURE(rv)) {
        if ((id_running_allocated == 1) &&
            (UDF_CONTROL(unit)->udf_id_running != _BCM_UDF_TD3_OBJ_MAX_ID)) {
            /* Decrement the running udf id */
            UDF_CONTROL(unit)->udf_id_running -= 1;
        }

        /* Delete the object node */
        if (obj_info != NULL) {
            _BCM_UDF_OBJ_NODE_DEL(unit, obj_info,
                                  UDF_TD3_CTRL(unit)->udf_obj_head);
            _BCM_UDF_FREE(obj_info);
        }
    }

    return rv;
}
int
_bcm_udf_td3_destroy(int unit, bcm_udf_id_t udf_id)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    /* De-allocate HW Resource */
    BCM_IF_ERROR_RETURN(_udf_td3_hw_policy_cmd_uninstall(unit, obj_info));
    /* Detach UDF object info from its database. */
    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_delete(unit, obj_info));

    return BCM_E_NONE;
}

int
_bcm_udf_td3_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_chunk_info_t *udf_chunk_info)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    if (udf_chunk_info == NULL) {
       return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    udf_chunk_info->offset = obj_info->offset;
    udf_chunk_info->width = obj_info->width;
    udf_chunk_info->abstract_pkt_format = obj_info->abstr_pkt_fmt;
    udf_chunk_info->chunk_bmap = obj_info->chunk_bmap;
    return BCM_E_NONE;
}

int
_bcm_udf_td3_abstr_pkt_format_obj_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int ct = 0;
    bcm_udf_id_t id_list[32] = {0};
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* Validate input params */
    if ((udf_id_list == NULL) || (actual == NULL)) {
       return BCM_E_PARAM;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(unit, abstract_pkt_format);

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;

    while (obj_info != NULL) {
        if (obj_info->abstr_pkt_fmt == abstract_pkt_format) {
           id_list[ct++] = obj_info->udf_id;
        }
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }

    if (ct > max) {
       return BCM_E_PARAM;
    }

    sal_memcpy(udf_id_list, id_list, sizeof(bcm_udf_id_t) * ct);
    *actual = ct;
    return BCM_E_NONE;
}

STATIC
int _bcm_udf_td3_abstr_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    bcm_udf_abstract_pkt_format_info_t *pkt_format_info)
{
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    if (pkt_format_info == NULL) {
       return BCM_E_PARAM;
    }

    /* Validate whether UDF module is attached */
    if (UDF_TD3_CTRL(unit) == NULL) {
        return BCM_E_INIT;
    }

    /* check if pkt format is in valid range */
    _BCM_UDF_ABSTR_PKT_FORMAT_VALIDATE(unit, abstract_pkt_format);

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, abstract_pkt_format);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_format_info->base_offset = pkt_fmt_hw_info->base_offset;
    pkt_format_info->num_chunks_max =
                         _BCM_UDF_BYTE_TO_CHUNK(pkt_fmt_hw_info->max_extract_bytes);
    pkt_format_info->chunk_bmap_used = pkt_fmt_hw_info->chunk_bmap_used;
    pkt_format_info->unavail_chunk_bmap = pkt_fmt_hw_info->unavail_chunk_bmap;

    return BCM_E_NONE;
}

STATIC int
_bcm_udf_td3_abstr_pkt_fmt_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual)
{
    int cnt = 0;
    bcm_udf_abstract_pkt_format_t pkt_fmt;

    if ((NULL == pkt_fmt_list) || (NULL == actual)) {
        return BCM_E_PARAM;
    }

    for (pkt_fmt = 0; pkt_fmt < bcmUdfAbstractPktFormatLastCount; pkt_fmt++) {
         if (0 == abstr_pkt_fmt_hw_info[unit][pkt_fmt]->max_extract_bytes) {
             continue;
         }

         pkt_fmt_list[cnt++] = pkt_fmt;
         if (cnt > max) {
             break;
         }
   }

   *actual = cnt;

   return BCM_E_NONE;
}

/* ceh field string */
typedef struct _flex_ceh_field_str_s {
    char  field_str[40];
} _flex_ceh_field_str_t;

/* Tunnel term type to CEH string mapping */
STATIC
_flex_ceh_field_str_t flex_tunnel_term_type_arr[bcmUdfTunnelTermFlowTypeLastCount] = {
            {"MIM_TERM_PYLD_TYPE_IP"},
            {"MIM_TERM_PYLD_TYPE_NON_IP"},
            {"L2_MPLS_TERM_PYLD_TYPE_IP"},
            {"L2_MPLS_TERM_PYLD_TYPE_NON_IP"},
            {"L2GRE_TERM_PYLD_TYPE_IP"},
            {"L2GRE_TERM_PYLD_TYPE_NON_IP"},
            {"VXLAN_TERM_PYLD_TYPE_IP"},
            {"VXLAN_TERM_PYLD_TYPE_NON_IP"},
            {"L3_MPLS_TERM"},
            {"L3_TUN_TERM"},
            {"GPE_GENEVE_L3_TUN_TERM"},
            {"L2_TUN_TERM_PYLD_TYPE_IP"},
            {"L2_TUN_TERM_PYLD_TYPE_NON_IP"},
            {"TRANSIT_AND_NON_TUNNEL"},
};

STATIC
_flex_ceh_field_str_t _udf_chunk_ceh_str[_BCM_UDF_TD3_MAX_CHUNKS] = {
            {"UDF_1_CHUNK_0"}, {"UDF_1_CHUNK_1"},
            {"UDF_1_CHUNK_2"}, {"UDF_1_CHUNK_3"},
            {"UDF_1_CHUNK_4"}, {"UDF_1_CHUNK_5"},
            {"UDF_1_CHUNK_6"}, {"UDF_1_CHUNK_7"},
            {"UDF_2_CHUNK_0"}, {"UDF_2_CHUNK_1"},
            {"UDF_2_CHUNK_2"}, {"UDF_2_CHUNK_3"},
            {"UDF_2_CHUNK_4"}, {"UDF_2_CHUNK_5"},
            {"UDF_2_CHUNK_6"}, {"UDF_2_CHUNK_7"},
};

/*
 * Get API to retrieve chunk bitmap.
 */
STATIC int
_bcm_udf_td3_field_bus_merge_profile_get(int unit,
                                         bcm_udf_tunnel_term_flow_type_t flow_type,
                                         uint32 *chunk_bmap)
{
    int rv;
    uint32 fmt_buf = 0;
    uint32 val;
    uint8 chunk, f;
    uint32 buf[100]={0};
    uint32 profile_idx;
    soc_field_t assign_ctrl_field;
    soc_cancun_ceh_field_info_t ceh_info;
    soc_format_t fmt = FLEX_ASSIGNMENT_CTRL_FORMATfmt;
    soc_mem_t mem = FIELD_BUS_MERGE_PROFILEm;
    soc_mem_info_t  *meminfo;

    meminfo = &SOC_MEM_INFO(unit, mem);

    /* Retrieve flow index from CEH flex db */
    rv = soc_cancun_ceh_obj_field_get(unit,
                                      "FIELD_BUS_MERGE_PROFILE_INDEX",
                                      flex_tunnel_term_type_arr[flow_type].field_str,
                                      &ceh_info);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                 "Flow type %d not found in CEH DB.\n\r"), flow_type));
        return rv;
    }

    profile_idx = ceh_info.value;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, profile_idx, buf);
    BCM_IF_ERROR_RETURN(rv);

    for (chunk = 0; chunk < _BCM_UDF_TD3_MAX_CHUNKS; chunk++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                                          "FIELD_BUS_MERGE_PROFILE_UDF_CONTAINERS",
                                          _udf_chunk_ceh_str[chunk].field_str,
                                          &ceh_info);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                 "Chunk[%d : %s] Assignment control offset not found in CEH DB.\n\r"),
                 chunk, _udf_chunk_ceh_str[chunk].field_str));
            return rv;
        }

        assign_ctrl_field = INVALIDf;
        for (f = 0; f < meminfo->nFields; f++) {
            if (ceh_info.offset == meminfo->fields[f].bp) {
                assign_ctrl_field = meminfo->fields[f].field;
                break;
            }
        }

        if (assign_ctrl_field == INVALIDf) {
            return BCM_E_INTERNAL;
        }
 
        soc_mem_field_get(unit, mem, buf, assign_ctrl_field, &fmt_buf);
        /* coverity[address_of : FALSE] */
        /* coverity[callee_ptr_arith : FALSE] */
        soc_format_field_get(unit, fmt, &fmt_buf, SELECTf, &val);
        if (val == 0) {
            *chunk_bmap |= 1 << chunk;
        }
    }

    return rv;
}

/*
 * SET field bus merge profile for given UDF chunk bitmap
 */
STATIC int
_bcm_udf_td3_field_bus_merge_profile_set(int unit,
                                         bcm_udf_tunnel_term_flow_type_t flow_type,
                                         uint32 chunk_bmap)
{
    int rv;
    uint32 fmt_buf = 0;
    uint32 val;
    uint8 chunk, f;
    uint32 buf[100]={0};
    uint32 profile_idx;
    soc_field_t assign_ctrl_field;
    soc_cancun_ceh_field_info_t ceh_info;
    soc_format_t fmt = FLEX_ASSIGNMENT_CTRL_FORMATfmt;
    soc_mem_t mem = FIELD_BUS_MERGE_PROFILEm;
    soc_mem_info_t  *meminfo;

    meminfo = &SOC_MEM_INFO(unit, mem);
    /* Retrieve flow index from CEH flex db */
    rv = soc_cancun_ceh_obj_field_get(unit,
                                      "FIELD_BUS_MERGE_PROFILE_INDEX",
                                      flex_tunnel_term_type_arr[flow_type].field_str,
                                      &ceh_info);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                 "Flow type %d not found in CEH DB.\n\r"), flow_type));
        return rv;
    }

    profile_idx = ceh_info.value;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, profile_idx, buf);
    BCM_IF_ERROR_RETURN(rv);

    for (chunk = 0; chunk < _BCM_UDF_TD3_MAX_CHUNKS; chunk++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                                          "FIELD_BUS_MERGE_PROFILE_UDF_CONTAINERS",
                                          _udf_chunk_ceh_str[chunk].field_str,
                                          &ceh_info);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                 "Chunk[%d : %s] Assignment control offset not found in CEH DB.\n\r"),
                 chunk, _udf_chunk_ceh_str[chunk].field_str));
            return rv;
        }

        assign_ctrl_field = INVALIDf;
        for (f = 0; f < meminfo->nFields; f++) {
            if (ceh_info.offset == meminfo->fields[f].bp) {
                assign_ctrl_field = meminfo->fields[f].field;
                break;
            }
        }

        if (assign_ctrl_field == INVALIDf) {
            return BCM_E_INTERNAL;
        }
        fmt_buf = 0x0;
        val = (chunk_bmap & (1 << chunk)) ? 0 : 1;
        soc_format_field32_set(unit, fmt, &fmt_buf, SELECTf, val);
        soc_mem_field_set(unit, mem, (uint32 *)buf,
                           assign_ctrl_field, &fmt_buf);

    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                       profile_idx, buf);

    return rv;
}

STATIC
int _bcm_udf_flow_based_chunk_arrange_set(
        int unit,
        bcm_udf_tunnel_term_flow_type_t flow_type,
        uint32 chunk_bmap)
{
    if (flow_type >= bcmUdfTunnelTermFlowTypeLastCount) {
        return BCM_E_PARAM;
    }

    if (chunk_bmap & ~((1 << _BCM_UDF_TD3_MAX_CHUNKS) - 1)) {
        return BCM_E_PARAM;
    }

    return _bcm_udf_td3_field_bus_merge_profile_set(unit,
                                  flow_type, chunk_bmap);
}

STATIC
int _bcm_udf_flow_based_chunk_arrange_get(
        int unit,
        bcm_udf_tunnel_term_flow_type_t flow_type,
        uint32 *chunk_bmap)
{
    if (chunk_bmap == NULL) {
        return BCM_E_PARAM;
    }

    return _bcm_udf_td3_field_bus_merge_profile_get(unit,
                                  flow_type, chunk_bmap);
}

STATIC int
_bcm_udf_td3_format_field_dump(int unit, soc_mem_t mem, uint32 index)
{
   uint32 buf[100]={0};
   uint32 f_buf[50]={0};
   soc_format_t fmt;
   uint32 id = 0, valid, dst_cont_id, src_offset;
   int rv;

   if (!((mem == IP_PARSER0_HFE_POLICY_TABLE_0m) ||
         (mem == IP_PARSER1_HFE_POLICY_TABLE_0m) ||
         (mem == IP_PARSER1_HFE_POLICY_TABLE_1m) ||
         (mem == IP_PARSER1_HFE_POLICY_TABLE_2m) ||
         (mem == IP_PARSER1_HFE_POLICY_TABLE_3m) ||
         (mem == IP_PARSER1_HFE_POLICY_TABLE_4m) ||
         (mem == IP_PARSER2_HFE_POLICY_TABLE_0m) ||
         (mem == IP_PARSER2_HFE_POLICY_TABLE_1m) ||
         (mem == IP_PARSER2_HFE_POLICY_TABLE_2m) ||
         (mem == IP_PARSER2_HFE_POLICY_TABLE_3m) ||
         (mem == IP_PARSER2_HFE_POLICY_TABLE_4m))) {
       return BCM_E_NONE;
   }

   rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, (uint32 *)buf);
   BCM_IF_ERROR_RETURN(rv);

   soc_mem_field_get(unit, mem, (uint32 *)buf, CMD_POLICY_DATAf, (uint32 *)f_buf);

   if (mem == IP_PARSER0_HFE_POLICY_TABLE_0m) {
       soc_format_t fmt = IP_PARSER0_HFE_CMD_POLICY_DATAfmt;
 
       LOG_CLI(("CMD VALID SRC_PKT_OFFSET\n\r"));
       LOG_CLI(("=== ===== ==============\n\r"));
       id = 8;
       valid = soc_format_field32_get(unit, fmt, f_buf, CMD_A_8_VALIDf);
       src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_A_8_SRC_PKT_OFFSETf);
       LOG_CLI(("%2d %5d %10d\n\r", id, valid, src_offset));

       valid = soc_format_field32_get(unit, fmt, f_buf, CMD_A_9_VALIDf);
       src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_A_9_SRC_PKT_OFFSETf);
       id++; LOG_CLI(("%2d %5d %10d\n\r", id, valid, src_offset));
       return BCM_E_NONE;
   }

   if ((mem == IP_PARSER1_HFE_POLICY_TABLE_0m) ||
       (mem == IP_PARSER1_HFE_POLICY_TABLE_1m) ||
       (mem == IP_PARSER1_HFE_POLICY_TABLE_2m) ||
       (mem == IP_PARSER1_HFE_POLICY_TABLE_3m) ||
       (mem == IP_PARSER1_HFE_POLICY_TABLE_4m)) {
       fmt = IP_PARSER1_HFE_CMD_POLICY_DATAfmt;
   } else {
       fmt = IP_PARSER2_HFE_CMD_POLICY_DATAfmt;
   }

   LOG_CLI(("ID VALID DEST_CONT_ID SRC_PKT_OFFSET\n\r"));
   LOG_CLI(("== ===== ============ ==============\n\r"));
   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_0_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_0_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_0_SRC_PKT_OFFSETf);
   LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_1_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_1_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_1_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));


   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_2_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_2_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_2_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_3_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_3_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_3_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_4_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_4_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_4_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_5_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_5_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_5_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_6_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_6_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_6_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_7_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_7_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_7_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_8_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_8_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_8_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_9_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_9_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_9_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_10_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_10_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_10_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   valid = soc_format_field32_get(unit, fmt, f_buf, CMD_C_11_VALIDf);
   dst_cont_id = soc_format_field32_get(unit, fmt, f_buf, CMD_C_11_DEST_CONT_IDf);
   src_offset = soc_format_field32_get(unit, fmt, f_buf, CMD_C_11_SRC_PKT_OFFSETf);
   id++; LOG_CLI(("%2d %5d %10d %10d\n\r", id, valid, dst_cont_id, src_offset));

   return BCM_E_NONE;
}

STATIC
void _udf_print_in_bin(uint16 n)
{
    int ct = 0;

    while (n) {
       LOG_CLI(("%d", (n & 1)));
       n >>= 1;

       ct++;
       if (ct == 4) {
          LOG_CLI((" "));
          ct = 0;
       }
    }
}

int
_bcm_udf_object_hw_info_dump(int unit, bcm_udf_id_t udf_id)
{
    uint8 stg, hfe_ct;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
   _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *hw_info = NULL;

    BCM_IF_ERROR_RETURN(_bcm_udf_td3_object_info_get(unit, udf_id, &obj_info));

    LOG_CLI(("UDF Object ID [%d] Information:\n\r", udf_id));
    LOG_CLI(("Offset..................%d\n\r", obj_info->offset));
    LOG_CLI(("Width...................%d\n\r", obj_info->width));
    LOG_CLI(("Abstr Pkt Fmt...........%s\n\r", _udf_abstr_pkt_fmt_name(obj_info->abstr_pkt_fmt)));
    LOG_CLI(("Chunk Bitmap............0x%x\n\r", obj_info->chunk_bmap));
    for (stg = 0; stg < _BCM_UDF_MAX_PARSER_STAGES; stg++) {
       LOG_CLI(("HW CMD Bitmap%d..........0x%x\n\r", stg, obj_info->hw_cmd_bmap[stg]));
    }

    hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, obj_info->abstr_pkt_fmt);
    if (hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    LOG_CLI(("Abstr Packet Format Information:\n\r"));
    LOG_CLI(("Parser............................%d\n\r", hw_info->parser));
    LOG_CLI(("Num stages supported..............%d\n\r", hw_info->num_stages));
    for (stg = 0; stg < hw_info->num_stages; stg++) {
       if (hw_info->stage_policy_mem[stg] != INVALIDm) {
          LOG_CLI(("Stage[%d] Policy Mem........................%s\n\r", stg,
               SOC_MEM_NAME(unit, hw_info->stage_policy_mem[stg])));
       }
       LOG_CLI(("Stage[%d] HFE Profile Ptr....................{", stg));
       for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stg]; hfe_ct++) {
            if (hfe_ct == (hw_info->hfe_prof_ptr_arr_len[stg] - 1)) {
                LOG_CLI(("%d}\n\r", hw_info->hfe_profile_ptr[stg][hfe_ct]));
            } else {
                LOG_CLI(("%d, ", hw_info->hfe_profile_ptr[stg][hfe_ct]));
            }
       }
       LOG_CLI(("Stage[%d] Number of CMDs supported...........%d\n\r", stg,
               hw_info->num_cmds[stg]));
    }
    LOG_CLI(("Maximum Extr Bytes................%d\n\r", hw_info->max_extract_bytes));
    LOG_CLI(("Used Extr Bytes...................%d\n\r", hw_info->extracted_bytes));
    LOG_CLI(("Chunk Bitmap Used (bits)..........0x%x (", hw_info->chunk_bmap_used));
    _udf_print_in_bin((uint16)hw_info->chunk_bmap_used);
    LOG_CLI((")\n\r"));
    LOG_CLI(("Unavailable Chunk Bitmap (bits)...0x%x (", hw_info->unavail_chunk_bmap));
    _udf_print_in_bin((uint16)hw_info->chunk_bmap_used);
    LOG_CLI((")\n\r"));
    for (stg = 0; stg < hw_info->num_stages; stg++) {
        LOG_CLI(("CMD Bitmap%d (bits)................0x%x (", stg, hw_info->valid_cmd_bmap[stg]));
           _udf_print_in_bin(hw_info->valid_cmd_bmap[stg]);
        LOG_CLI((")\n\r"));
    }

    for (stg = 0; stg < hw_info->num_stages; stg++) {
       if (hw_info->stage_policy_mem[stg] != INVALIDm) {
          for (hfe_ct = 0; hfe_ct < hw_info->hfe_prof_ptr_arr_len[stg]; hfe_ct++) {
              _bcm_udf_td3_format_field_dump(unit, hw_info->stage_policy_mem[stg],
                                         hw_info->hfe_profile_ptr[stg][hfe_ct]);
          }
       }
    }

    return BCM_E_NONE;
}

/*
 * Routine to dump all the configured UDF objects info.
 */
STATIC void
_bcm_udf_td3_sw_dump(int unit)
{
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    if (UDF_TD3_CTRL(unit)->udf_obj_head == NULL) {
       return;
    }

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;

    while (obj_info != NULL) {
        _bcm_udf_object_hw_info_dump(unit, obj_info->udf_id);
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
}


/*
 * Routine to retrieve number of UDF objects associated
 * to the given UDF Chunk Bitmap
 */
int
_bcm_udf_td3_chunk_id_multi_get(
    int unit,
    uint32 udf_chunk_bmap,
    int max,
    int *objects_list,
    int *actual)
{
    int idx = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;
    while (obj_info != NULL) {
        if ((obj_info->chunk_bmap & udf_chunk_bmap) == obj_info->chunk_bmap) {
            if (max > 0) {
                if (idx >= max) {
                   return BCM_E_PARAM;
                }
                objects_list[idx] = obj_info->udf_id;
            }
            idx++;
        }
        _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
    *actual = idx;

    return BCM_E_NONE;
}

/*
 * Routine to return UDF chunk dedicated to Range Checker.
 */
int
_bcm_udf_td3_range_checker_chunk_info_get(
    int unit,
    uint8 *num_chunks,
    uint32 *chunk_bmap)
{
    if ((num_chunks == NULL) || (chunk_bmap == NULL)) {
       return BCM_E_PARAM;
    }

    *num_chunks = 1;
    *chunk_bmap = _BCM_UDF_TD3_RANGE_CHUNK_BMAP;
    return BCM_E_NONE;
}

#if defined (BCM_WARM_BOOT_SUPPORT)
 
STATIC int
_udf_td3_wb_abstr_pkt_fmt_recover(int unit, _bcm_udf_td3_obj_info_t *obj_info)
{
    uint8 stg;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, obj_info->abstr_pkt_fmt);
    if (pkt_fmt_hw_info == NULL) {
       return BCM_E_INTERNAL;
    }

    pkt_fmt_hw_info->extracted_bytes += obj_info->width;
    pkt_fmt_hw_info->chunk_bmap_used |= obj_info->chunk_bmap;
    for (stg = 0; stg < pkt_fmt_hw_info->num_stages; stg++) {
        pkt_fmt_hw_info->valid_cmd_bmap[stg] |= obj_info->hw_cmd_bmap[stg];
    }

    return BCM_E_NONE;
}

/*
 * Routine to Recover UDF Object information during WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_recover_1_0(int unit, uint8 **scache_ptr)
{
    _bcm_udf_td3_wb_obj_info_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    int ct;
    uint16 num_udfs = 0;
    uint8 num_chunks = 0;

    /* Retrieve number of UDFs */
    sal_memcpy(&num_udfs, *scache_ptr, sizeof(num_udfs));
    *scache_ptr += sizeof(num_udfs);

    /* Retrieve the UDF objects */
    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_t *)(*scache_ptr);
    for (ct = 0; ct < num_udfs; ct++) {
        /* Allocate memory for UDF object info. */
        obj_info = NULL;
        _BCM_UDF_ALLOC(obj_info, sizeof(_bcm_udf_td3_obj_info_t),
                       "UDF object info.");
        if (obj_info == NULL) {
           return BCM_E_MEMORY;
        }

        obj_info->udf_id = scache_obj_ptr->udf_id;
        obj_info->offset = scache_obj_ptr->offset;
        obj_info->chunk_bmap = scache_obj_ptr->chunk_bmap;

        (void)_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                        &num_chunks, NULL);
        obj_info->width = _BCM_UDF_CHUNK_TO_BYTE(num_chunks);
        obj_info->hw_cmd_bmap[0] = scache_obj_ptr->hw_cmd_bmap1;
        obj_info->hw_cmd_bmap[1] = scache_obj_ptr->hw_cmd_bmap2;
        obj_info->abstr_pkt_fmt = scache_obj_ptr->abstr_pkt_fmt;

        BCM_IF_ERROR_RETURN(_udf_td3_wb_abstr_pkt_fmt_recover(unit, obj_info));

        /* Add obj_info item to the linked list */
        _BCM_UDF_OBJ_NODE_ADD(unit, (obj_info),
                              UDF_TD3_CTRL(unit)->udf_obj_head);
        scache_obj_ptr++;
    }

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}

/*
 * Routine to recover UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_1_0(int unit, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint8 buf[100];
    uint32 hfe_profile_ptr;
    uint8 *cmd_pol_data_buf = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        /*
         * INT containers are fixed, need not sync the containers.
         */
        if ((pkt_fmt == bcmUdfAbstractPktFormatUdpINT) ||
            (pkt_fmt == bcmUdfAbstractPktFormatTcpINT)) {
           continue;
        }

        for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
            policy_mem = pkt_fmt_hw_info->stage_policy_mem[stage];
            hfe_profile_ptr = pkt_fmt_hw_info->hfe_profile_ptr[stage][0];

            if (policy_mem == INVALIDm || policy_mem == 0) {
               continue;
            }

            len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);

            cmd_pol_data_buf = NULL;
            if (pkt_fmt_hw_info->chunk_bmap_used) {
                _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF WB CMD_POLICY_DATA alloc");
                if (cmd_pol_data_buf == NULL) {
                   return BCM_E_MEMORY;
                }

                sal_memcpy(cmd_pol_data_buf, *scache_ptr, len);
                *scache_ptr += len;
            } else {
                sal_memset(buf, 0x0, sizeof(buf));

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                                    hfe_profile_ptr, (uint32 *) buf));

                _BCM_UDF_ALLOC(cmd_pol_data_buf, len, "UDF default CMD Policy data");
                if (cmd_pol_data_buf == NULL) {
                   return BCM_E_MEMORY;
                }

                soc_mem_field_get(unit, policy_mem, (uint32 *)buf,
                             CMD_POLICY_DATAf, (uint32 *)cmd_pol_data_buf);
            }

            pkt_fmt_hw_info->cmd_policy_data[stage] = cmd_pol_data_buf;
        }
    }

    return BCM_E_NONE;
}


/*
 * Routine to Recover UDF Object information during WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_recover_2_0(int unit, uint16 recovered_version, uint8 **scache_ptr)
{
    int ct;
    uint8 stg;
    uint8 num_chunks = 0;
    uint16 num_udfs = 0;
    uint8 num_stages = 0;
    _bcm_udf_td3_wb_obj_info_2_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;

    /* Retrieve number of UDFs */
    sal_memcpy(&num_udfs, *scache_ptr, sizeof(num_udfs));
    *scache_ptr += sizeof(num_udfs);

    /* Retrieve the UDF objects */
    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_2_t *)(*scache_ptr);
    for (ct = 0; ct < num_udfs; ct++) {
        /* Allocate memory for UDF object info. */
        obj_info = NULL;
        _BCM_UDF_ALLOC(obj_info, sizeof(_bcm_udf_td3_obj_info_t),
                       "UDF object info.");
        if (obj_info == NULL) {
           return BCM_E_MEMORY;
        }

        obj_info->udf_id = scache_obj_ptr->udf_id;
        obj_info->offset = scache_obj_ptr->offset;
        obj_info->chunk_bmap = scache_obj_ptr->chunk_bmap;

        (void)_udf_chunk_bmap_to_chunks(obj_info->chunk_bmap,
                                        &num_chunks, NULL);
        obj_info->width = _BCM_UDF_CHUNK_TO_BYTE(num_chunks);
        if (recovered_version > BCM_WB_VERSION_1_0) {
           num_stages = scache_obj_ptr->num_stages;
        } else {
           num_stages = 2;
        }

        for (stg = 0; stg < num_stages; stg++) {
           obj_info->hw_cmd_bmap[stg] = scache_obj_ptr->hw_cmd_bmap[stg];
        }
        obj_info->abstr_pkt_fmt = scache_obj_ptr->abstr_pkt_fmt;

        BCM_IF_ERROR_RETURN(_udf_td3_wb_abstr_pkt_fmt_recover(unit, obj_info));

        /* Add obj_info item to the linked list */
        _BCM_UDF_OBJ_NODE_ADD(unit, (obj_info),
                              UDF_TD3_CTRL(unit)->udf_obj_head);
        scache_obj_ptr++;
    }

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}

/*
 * Routine to recover UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_2_0(int unit, uint16 recovered_version, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint8 num_stages = 0;
    uint8 *cmd_pol_data_buf = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        if (recovered_version == BCM_WB_VERSION_1_0) {
           if (pkt_fmt >= bcmUdfAbstractPktFormatL3GreUnknownPayload) {
              continue;
           }
           num_stages = 2;
        } else {
           num_stages = pkt_fmt_hw_info->num_stages;
        }

        for (stage = 0; stage < num_stages; stage++) {
            policy_mem = pkt_fmt_hw_info->stage_policy_mem[stage];

            if (policy_mem == INVALIDm) {
               LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "Invalid Policy mem for Abstract pkt fmt '%s', stage:%d.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt), stage));
               continue;
            }

            len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);

            if (pkt_fmt_hw_info->chunk_bmap_used) {
                cmd_pol_data_buf = pkt_fmt_hw_info->cmd_policy_data[stage];
                sal_memcpy(cmd_pol_data_buf, *scache_ptr, len);
                *scache_ptr += len;
                pkt_fmt_hw_info->cmd_policy_data[stage] = cmd_pol_data_buf;
            }
        }
    }

    return BCM_E_NONE;
}



/*
 * Routine to Sync UDF Object information for WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_sync_1_0(int unit, uint8 **scache_ptr)
{
    _bcm_udf_td3_wb_obj_info_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    uint16 *scache_num_udf_ptr = NULL;
    uint16 num_udfs = 0;

    scache_num_udf_ptr = (uint16 *)(*scache_ptr);
    *scache_ptr += sizeof(num_udfs);

    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_t *)(*scache_ptr);

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;
    while (obj_info != NULL) {
         scache_obj_ptr->udf_id = obj_info->udf_id;
         scache_obj_ptr->offset = obj_info->offset;
         scache_obj_ptr->chunk_bmap = obj_info->chunk_bmap;
         scache_obj_ptr->abstr_pkt_fmt = obj_info->abstr_pkt_fmt;
         scache_obj_ptr->hw_cmd_bmap1 = obj_info->hw_cmd_bmap[0];
         scache_obj_ptr->hw_cmd_bmap2 = obj_info->hw_cmd_bmap[1];

         scache_obj_ptr++;
         num_udfs++;
         _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
    sal_memcpy(scache_num_udf_ptr, &num_udfs, sizeof(num_udfs));

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}
/*
 * Routine to Sync UDF Object information for WarmBoot.
 */
STATIC int
_bcm_udf_td3_wb_obj_info_sync_2_0(int unit, uint8 **scache_ptr)
{
    uint8 stg;
    _bcm_udf_td3_wb_obj_info_2_t *scache_obj_ptr;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    uint16 *scache_num_udf_ptr = NULL;
    uint16 num_udfs = 0;

    scache_num_udf_ptr = (uint16 *)(*scache_ptr);
    *scache_ptr += sizeof(num_udfs);

    scache_obj_ptr = (_bcm_udf_td3_wb_obj_info_2_t *)(*scache_ptr);

    obj_info = UDF_TD3_CTRL(unit)->udf_obj_head;
    while (obj_info != NULL) {
         scache_obj_ptr->udf_id = obj_info->udf_id;
         scache_obj_ptr->offset = obj_info->offset;
         scache_obj_ptr->chunk_bmap = obj_info->chunk_bmap;
         scache_obj_ptr->abstr_pkt_fmt = obj_info->abstr_pkt_fmt;
         scache_obj_ptr->num_stages = _BCM_UDF_MAX_PARSER_STAGES;
         for (stg = 0; stg < _BCM_UDF_MAX_PARSER_STAGES; stg++) {
            scache_obj_ptr->hw_cmd_bmap[stg] = obj_info->hw_cmd_bmap[stg];
         }

         scache_obj_ptr++;
         num_udfs++;
         _BCM_UDF_OBJ_NODE_NEXT(obj_info);
    }
    sal_memcpy(scache_num_udf_ptr, &num_udfs, sizeof(num_udfs));

    *scache_ptr = (uint8 *)scache_obj_ptr;
    return BCM_E_NONE;
}

/*
 * Routine to sync UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_1_0(int unit, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint8 *cmd_policy_data = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

        /*
         * INT containers are fixed, need not sync the containers.
         */
        if ((pkt_fmt == bcmUdfAbstractPktFormatUdpINT) ||
            (pkt_fmt == bcmUdfAbstractPktFormatTcpINT)) {
           continue;
        }

        if (pkt_fmt_hw_info->chunk_bmap_used) {
            for (stage = 0; stage < _BCM_UDF_MAX_PARSER_STAGES; stage++) {
                policy_mem = pkt_fmt_hw_info->stage_policy_mem[stage];
                cmd_policy_data = pkt_fmt_hw_info->cmd_policy_data[stage];

                if (policy_mem == INVALIDm || policy_mem == 0) {
                   continue;
                }

                len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);
                sal_memcpy(*scache_ptr, cmd_policy_data, len);
                *scache_ptr += len;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Routine to sync UDF Abstract Pkt Fmt default cmd_policy_data.
 */
STATIC int
_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_2_0(int unit, uint8 **scache_ptr)
{
    uint8 stage, len;
    soc_mem_t policy_mem;
    uint32 cancun_ver;
    uint8 *cmd_policy_data = NULL;
    bcm_udf_abstract_pkt_format_t pkt_fmt;
    _bcm_udf_td3_abstr_pkt_fmt_hw_info_t *pkt_fmt_hw_info;

    for (pkt_fmt = bcmUdfAbstractPktFormatLlc;
         pkt_fmt < bcmUdfAbstractPktFormatLastCount;
         pkt_fmt++) {
        pkt_fmt_hw_info = _UDF_ABSTR_PKT_FMT_HW_INFO_GET(unit, pkt_fmt);
        if (pkt_fmt_hw_info == NULL) {
           continue;
        }

       /*
        * INT containers are fixed, need not sync the containers.
        */
        BCM_IF_ERROR_RETURN(soc_cancun_version_get(unit, &cancun_ver));
        if (SOC_CANCUN_VERSION_5_2_UNDER_SERIES(cancun_ver)) {
            if ((pkt_fmt == bcmUdfAbstractPktFormatUdpINT) ||
                (pkt_fmt == bcmUdfAbstractPktFormatTcpINT)) {
                continue;
            }
        }

        if (pkt_fmt_hw_info->chunk_bmap_used) {
            for (stage = 0; stage < pkt_fmt_hw_info->num_stages; stage++) {
                policy_mem = pkt_fmt_hw_info->stage_policy_mem[stage];
                cmd_policy_data = pkt_fmt_hw_info->cmd_policy_data[stage];

                if (policy_mem == INVALIDm || policy_mem == 0) {
                   LOG_ERROR(BSL_LS_BCM_UDF, (BSL_META_U(unit,
                          "Invalid Policy mem for Abstract pkt fmt '%s', stage:%d.\n\r"),
                          _udf_abstr_pkt_fmt_name(pkt_fmt), stage));
                   return BCM_E_INTERNAL;
                }

                len = soc_mem_field_length(unit, policy_mem, CMD_POLICY_DATAf);
                sal_memcpy(*scache_ptr, cmd_policy_data, len);
                *scache_ptr += len;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_udf_td3_udf_wb_sync
 * Purpose:
 *      Syncs UDF warmboot state to scache
 * Parameters:
 *      unit   - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_wb_sync(int unit)
{
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));
    if (UDF_CONTROL(unit) != NULL) {
       if (BCM_WB_DEFAULT_VERSION == BCM_WB_VERSION_1_0) {
          BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_obj_info_sync_1_0(unit, &scache_ptr));
          BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_1_0(unit, &scache_ptr));
       } else {
          BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_obj_info_sync_2_0(unit, &scache_ptr));
          BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_abstr_pkt_fmt_info_sync_2_0(unit, &scache_ptr));
       }
    }
    return BCM_E_NONE;
}

/* Returns required scache size in bytes for version = 1_0 */
STATIC int
_bcm_udf_td3_wb_scache_size_get_1_0(int unit, int *req_scache_size)
{
    int len;
    int alloc_size = 0;

    /* Number of UDFs */
    alloc_size += sizeof(uint16);

    /* Max UDF Object Nodes. */
    alloc_size += (UDF_CONTROL(unit)->max_udfs *
                   sizeof(_bcm_udf_td3_wb_obj_info_t));

    /* Max UDF Abstract Type CMD_POLICY_DATA. */
    len = soc_mem_field_length(unit, IP_PARSER1_HFE_POLICY_TABLE_3m, CMD_POLICY_DATAf);
    alloc_size += ((bcmUdfAbstractPktFormatLastCount - 1) * len);

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

/* Returns required scache size in bytes for version = 2_0 */
STATIC int
_bcm_udf_td3_wb_scache_size_get_2_0(int unit, int *req_scache_size)
{
    int len;
    int alloc_size = 0;

    /* Number of UDFs */
    alloc_size += sizeof(uint16);

    /* Max UDF Object Nodes. */
    alloc_size += (UDF_CONTROL(unit)->max_udfs *
                   sizeof(_bcm_udf_td3_wb_obj_info_2_t));

    /* Max UDF Abstract Type CMD_POLICY_DATA. */
    len = soc_mem_field_length(unit, IP_PARSER1_HFE_POLICY_TABLE_3m, CMD_POLICY_DATAf);
    alloc_size += ((bcmUdfAbstractPktFormatLastCount - 1) * len);

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}


/* Returns required scache size for UDF module */
STATIC int
_bcm_udf_td3_wb_scache_size_get(int unit, int *req_scache_size)
{
    if (BCM_WB_DEFAULT_VERSION > BCM_WB_VERSION_1_0) {
       BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_scache_size_get_2_0(unit,
                                                   req_scache_size));
    } else {
       BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_scache_size_get_1_0(unit,
                                                  req_scache_size));
    }
    return BCM_E_NONE;
}

/* Allocates required scache size for the UDF module recovery */
STATIC int
_bcm_udf_td3_wb_alloc(int unit)
{
    int     rv;
    int     req_scache_size;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = _bcm_udf_td3_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 req_scache_size, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_udf_td3_reinit
 * Purpose:
 *      Recovers UDF warmboot state from scache
 * Parameters:
 *      unit  - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_udf_td3_reinit(int unit)
{
    int     rv = BCM_E_INTERNAL;
    uint8   *scache_ptr;
    uint16  recovered_ver = 0;
    soc_scache_handle_t scache_handle;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    if (recovered_ver > BCM_WB_VERSION_1_0) {
       rv = _bcm_udf_td3_wb_obj_info_recover_2_0(unit, recovered_ver, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);

       rv = _bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_2_0(unit, recovered_ver, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);
    } else if (recovered_ver == BCM_WB_VERSION_1_0) {
       rv = _bcm_udf_td3_wb_obj_info_recover_1_0(unit, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);

       rv = _bcm_udf_td3_wb_abstr_pkt_fmt_info_recover_1_0(unit, &scache_ptr);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (BCM_SUCCESS(rv) &&
       (recovered_ver < BCM_WB_DEFAULT_VERSION)) {
        /*
         * This will handle the below cases
         * 1. When warboot from release which didn't had UDF module in SCACHE or
         * 2. The current release SCACHE size required is more than previous version or
         * 3. Level 1 warmboot in this case memory will not be allocated.
         */
        BCM_IF_ERROR_RETURN(_bcm_udf_td3_wb_alloc(unit));
    }
    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
/*
 * Routine to dump UDF mem info.
 */
STATIC void
_bcm_udf_td3_sw_mem_dump(int unit, soc_mem_t mem, int idx)
{
    _bcm_udf_td3_format_field_dump(unit, mem, idx);
}


/*
 * Function:
 *    _bcm_udf_td3_functions_init
 *
 * Purpose:
 *    Set up functions pointers
 *
 * Parameters:
 *    functions - (IN/OUT) Pointer to device specific UDF module utility
 *                         routines.
 *
 * Returns:
 *     Nothing
 */
void
_bcm_udf_td3_functions_init(_bcm_udf_funct_t *functions)
{
    functions->udf_init                = _bcm_udf_td3_init;
    functions->udf_detach              = _bcm_udf_td3_detach;
    functions->udf_chunk_create        = _bcm_udf_td3_chunk_create;
    functions->udf_destroy             = _bcm_udf_td3_destroy;
    functions->udf_chunk_info_get      = _bcm_udf_td3_chunk_info_get;
    functions->udf_flow_based_chunk_arrange_set =
                                         _bcm_udf_flow_based_chunk_arrange_set;
    functions->udf_flow_based_chunk_arrange_get =
                                         _bcm_udf_flow_based_chunk_arrange_get;
    functions->udf_abstr_pkt_format_obj_list_get =
                                         _bcm_udf_td3_abstr_pkt_format_obj_list_get;
    functions->udf_abstr_pkt_format_info_get =
                                         _bcm_udf_td3_abstr_pkt_format_info_get;
    functions->udf_abstr_pkt_fmt_list_get =
                                         _bcm_udf_td3_abstr_pkt_fmt_list_get;
#ifdef BCM_WARM_BOOT_SUPPORT
    functions->udf_wb_sync = _bcm_udf_td3_wb_sync;
#endif /* BCM_WARM_BOOT_SUPPORT */
    functions->udf_range_checker_chunk_info_get =
                                         _bcm_udf_td3_range_checker_chunk_info_get;
    functions->udf_sw_dump             = _bcm_udf_td3_sw_dump;
    functions->udf_sw_mem_dump         = _bcm_udf_td3_sw_mem_dump;
}
#endif /* (BCM_TRIDENT3_SUPPORT) */
