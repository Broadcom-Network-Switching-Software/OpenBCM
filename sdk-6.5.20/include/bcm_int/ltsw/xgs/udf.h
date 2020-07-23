/*! \file udf.h
 *
 * UDF headfiles to declare internal APIs for XGS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XGS_LTSW_XGS_UDF_H
#define XGS_LTSW_XGS_UDF_H

#include <bcm_int/ltsw/udf_int.h>

/******************************************************************************
 * Local definitions
 */

/* Maximum number of pipes */
#define UDF_MAX_PIPES                                  16

/* Maximum number of chunks */
#define UDF_MAX_CHUNKS       BCMINT_UDF_HA_UDF_CHUNKS_MAX

/*! Maximum number of Udf per packet format */
#define UDF_MAX_ID_LIST     BCMINT_UDF_HA_UDF_ID_LIST_MAX

/* UDF offset info flags */
#define BCMI_UDF_OFFSET_INFO_IN_USE             (1 << 0)
#define BCMI_UDF_OFFSET_INFO_SHARED             (1 << 1)
#define BCMI_UDF_OFFSET_INFO_FLEXHASH           (1 << 2)
#define BCMI_UDF_OFFSET_INFO_ING_FP             (1 << 3)
#define BCMI_UDF_OFFSET_INFO_VLAN_FP            (1 << 4)
#define BCMI_UDF_OFFSET_INFO_POLICER            (1 << 5)
#define BCMI_UDF_OFFSET_INFO_UDFHASH            (1 << 6)
#define BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG     (1 << 7)
#define BCMI_UDF_OFFSET_INFO_RANGE_CHECK        (1 << 8)

/* UDF packet format info flags */
#define BCMI_UDF_PKT_FMT_INFO_IN_USE            (1 << 0)
#define BCMI_UDF_PKT_FMT_INFO_INSTALLED         (1 << 1)
#define BCMI_UDF_PKT_FMT_INFO_SKIP_OUTER_IFA_MD (1 << 2)

/*
 * Typedef:
 *     bcmi_xgs_udf_pkt_format_misc_type_t
 * Purpose:
 *     Enumerations of different boolean keys in UDF LT.
 */
typedef enum bcmi_xgs_udf_pkt_format_misc_type_e {
    /* Packet format type Higig */
    bcmiXgsUdfPktFormatMiscTypeHigig           = 0,

    /* Packet format type VNTAG */
    bcmiXgsUdfPktFormatMiscTypeVntag           = 1,

    /* Packet format type ETAG */
    bcmiXgsUdfPktFormatMiscTypeEtag            = 2,

    /* Packet format tape ICMN */
    bcmiXgsUdfPktFormatMiscTypeIcnm            = 3,

    /* Packet format type CNTAG */
    bcmiXgsUdfPktFormatMiscTypeCntag           = 4,

    /* Packet format type CNTAG */
    bcmiXgsUdfPktFormatMiscTypeSubportTag      = 5,

    /* Packet format type Inband telemetry */
    bcmiXgsUdfPktFormatMiscTypeIntPkt          = 6,

    /* Packet format type Outer IFA */
    bcmiXgsUdfPktFormatMiscTypeOuterIFA        = 7,

    /* Packet format type Inner IFA */
    bcmiXgsUdfPktFormatMiscTypeInnerIFA        = 8,

    /* Always last */
    bcmiXgsUdfPktFormatMiscTypeCount

} bcmi_xgs_udf_pkt_format_misc_type_t;

/* Set field to internal pkt format misc member */
#define BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(_type_, _data_, _info_)    \
    do {                                                                \
        if (_type_ >= 16) {                                             \
            SHR_ERR_EXIT(SHR_E_INTERNAL);                        \
        }                                                               \
        (_info_)->misc_en_fields |= ((_data_) << ((_type_)*2));         \
    } while (0)

/* Get field from internal pkt format misc member */
#define BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(_type_, _info_, _data_)    \
    do {                                                                \
        if (_type_ >= 16) {                                             \
            SHR_ERR_EXIT(SHR_E_INTERNAL);                        \
        }                                                               \
        (_data_) = (((_info_)->misc_en_fields) >> ((_type_)*2)) & 0x3;  \
    } while (0)

/*
 * Typedef:
 *     bcmi_xgs_udf_offset_entry_t
 * Purpose:
 *     UDF offset entry structure to manage offset entry.
 */
typedef struct bcmi_xgs_udf_offset_entry_s {
    /*! flags */
#define BCMI_XGS_UDF_OFFSET_ENTRY_GROUP     (1 << 0)
#define BCMI_XGS_UDF_OFFSET_ENTRY_PART      (1 << 1)
#define BCMI_XGS_UDF_OFFSET_ENTRY_SHARED    (1 << 2)
    uint8 flags;

    /*! first chunk id */
    uint8 grp_id;

    /*! number of associated udf ids */
    uint16 num_udfs;

} bcmi_xgs_udf_offset_entry_t;

/* Maximum UDF Hash chunks */
#define BCMI_XGS_UDF_HASH_CHUNKS_MAX                  2

/*
 * Typedef:
 *     bcmi_xgs_udf_hash_chunks_info_t
 * Purpose:
 *     Stucture to maintain UDF chunks usage for hash
 */
typedef struct bcmi_xgs_udf_hash_chunks_info_s {
    /*! Number of hash related chunks */
    int chunks;

    /*! Chunk Ids */
    uint8 chunk_ids[BCMI_XGS_UDF_HASH_CHUNKS_MAX];

    /*! Fields */
    const char *field_str[BCMI_XGS_UDF_HASH_CHUNKS_MAX];

    /* Number of bytes for hashing */
    int bytes;

    /*! Hash byte ordering */
    uint8 byte_order[BCMI_XGS_UDF_HASH_CHUNKS_MAX * 2];

} bcmi_xgs_udf_hash_chunks_info_t;

/*
 * Typedef:
 *     bcmi_xgs_udf_appl_chunks_info_t
 * Purpose:
 *     UDF chunks information used for various applications.
 */
typedef struct bcmi_xgs_udf_appl_chunks_info_s {
#define BCMI_XGS_UDF_APPL_FLEXHASH_CHUNKS         4
    int flexhash_chunks;
    uint8 flexhash[BCMI_XGS_UDF_APPL_FLEXHASH_CHUNKS];

#define BCMI_XGS_UDF_APPL_POLICER_CHUNKS          2
    int policer_chunks;
    uint8 policer[BCMI_XGS_UDF_APPL_POLICER_CHUNKS];

    int udfhash_chunks;
    uint8 udfhash[BCMI_XGS_UDF_HASH_CHUNKS_MAX];

#define BCMI_XGS_UDF_APPL_RANGECHECK_CHUNKS       1
    int rangecheck_chunks;
    uint8 rangecheck[BCMI_XGS_UDF_APPL_RANGECHECK_CHUNKS];

#define BCMI_XGS_UDF_APPL_FLEXCTR_CHUNKS          2
    int flexctr_chunks;
    uint8 flexctr[BCMI_XGS_UDF_APPL_FLEXCTR_CHUNKS];

    int def_appl_chunks;
    uint8 def_appl[UDF_MAX_CHUNKS];
} bcmi_xgs_udf_appl_chunks_info_t;

typedef int (*bcmi_xgs_udf_enum_sym_get_f)(int unit,
    char *sid, char *fid, int enum_value, const char **symbol);

/*
 * Typedef:
 *     bcmi_xgs_udf_tbl_info_t
 * Purpose:
 *     UDF LT table information.
 */
typedef struct bcmi_xgs_udf_lt_tbls_info_s {
    /*! Config LT name */
    char *udf_config_sid;

    /*! Pipe unique */
    char *udf_opermode_pipeuniue_fid;

    /*! LT name */
    char *udf_sid;

    /*! Etag */
    char *etag_fid;

    /*! Etag mask */
    char *etag_mask_fid;

    /*! CNtag */
    char *cntag_fid;

    /*! CNtag mask */
    char *cntag_mask_fid;

    /*! VNtag */
    char *vntag_fid;

    /*! VNtag mask */
    char *vntag_mask_fid;

    /*! Higig */
    char *higig_fid;

    /*! Higig mask */
    char *higig_mask_fid;

    /*! port */
    char *port_id_fid;

    /*! port mask */
    char *port_id_mask_fid;

    /*! loopback type */
    char *lb_pkt_type_fid;

    /*! loopback type mask */
    char *lb_pkt_type_mask_fid;

    /*! Loopback header */
    char *loopback_hdr_fid;

    /*! Loopback header mask */
    char *loopback_hdr_mask_fid;

    /*! Opaque tag */
    char *opaque_tag_fid;

    /*! Opaque tag mask */
    char *opaque_tag_mask_fid;

    /*! L4 Dst Port */
    char *l4dst_port_fid;

    /*! L4 Dst Port Mask */
    char *l4dst_port_mask_fid;

    /*! In-band telemetry */
    char *inband_telemetry_fid;

    /*! In-band telemetry mask */
    char *inband_telemetry_mask_fid;

    /*! L2 type */
    char *l2_type_fid;

    /*! L2 type Mask */
    char *l2_type_mask_fid;

    /*! Ethertype */
    char *ethertype_fid;

    /*! Ethertype Mask */
    char *ethertype_mask_fid;

    /*! Vlan tag */
    char *vlan_tag_fid;

    /*! Vlan tag mask  */
    char *vlan_tag_mask_fid;

    /*! L3 fields */
    char *l3_fields_fid;

    /*! L3 fields mask */
    char *l3_fields_mask_fid;

    /*! Outer L3 header */
    char *outer_l3_hdr_fid;

    /*! Outer L3 header mask */
    char *outer_l3_hdr_mask_fid;

    /*! Inner L3 header */
    char *inner_l3_hdr_fid;

    /*! Inner L3 header mask */
    char *inner_l3_hdr_mask_fid;

    /*! Outer FCOE header */
    char *outer_fcoe_hdr_fid;

    /*! Outer FCOE header mask */
    char *outer_fcoe_hdr_mask_fid;

    /*! Inner FCOE header */
    char *inner_fcoe_hdr_fid;

    /*! Inner FCOE header mask */
    char *inner_fcoe_hdr_mask_fid;

    /*! Outer IFA header */
    char *outer_ifa_hdr_fid;

    /*! Outer IFA header mask */
    char *outer_ifa_hdr_mask_fid;

    /*! Inner IFA header */
    char *inner_ifa_hdr_fid;

    /*! Inner IFA header mask */
    char *inner_ifa_hdr_mask_fid;

    /*! Class Id */
    char *class_id_fid;

    /*! Flex hash */
    char *flex_hash_fid;

    /*! Skip Outer Ifa metadata */
    char *skip_outer_ifa_md_fid;

    /*! Layer */
    char *layer_fid;

    /*! Offset */
    char *offset_fid;

} bcmi_xgs_udf_lt_tbls_info_t;

/*
 * Typedef:
 *     bcmi_xgs_udf_control_info_t
 * Purpose:
 *     UDF control structure to manage and interact with
 *     device specific driver functions.
 */
typedef struct bcmi_xgs_udf_control_info_s {
    /*! Flags for LT fields  */
#define BCMI_XGS_UDF_LT_PKT_FORMAT_HIGIG                (1 << 0)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_VNTAG                (1 << 1)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_ETAG                 (1 << 2)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_ICNM                 (1 << 3)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_CNTAG                (1 << 4)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_SUBPORT_TAG          (1 << 5)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_INT_PKT              (1 << 6)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_HDR         (1 << 7)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_OPAQUE_TAG_TYPE      (1 << 8)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_L4DST_PORT           (1 << 9)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_SRC_PORT             (1 << 10)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_TYPE        (1 << 11)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_FCOE_HDR             (1 << 12)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_OUTER_IFA_HDR        (1 << 13)
#define BCMI_XGS_UDF_LT_PKT_FORMAT_INNER_IFA_HDR        (1 << 14)
#define BCMI_XGS_UDF_LT_OUTPUT_CLASS_ID                 (1 << 15)
#define BCMI_XGS_UDF_LT_OUTPUT_FLEX_HASH                (1 << 16)
#define BCMI_XGS_UDF_LT_OFFSET_SKIP_OUTER_IFA_MD        (1 << 17)
    uint32 lt_flags;

    /*! Maximum entries */
    uint8 max_entries;

    /*! Granularity */
    uint8 gran;

    /*! Maximum parsable bytes */
    uint8 max_parse_bytes;

    /*! number of udf ids configured */
    uint16 num_udfs;

    /*! Maximum number of UDF Objects that can be created */
    uint16 max_udfs;

    /*! number of packet format configured */
    uint16 num_pkt_formats;

    /*! Maximum number of Pkt format Objects that can be created */
    uint16 max_pkt_formats;

    /*! pointer to udf offset info */
    bcmint_udf_offset_info_t *offset_info_array;

    /*! pointer to pkt format info */
    bcmint_udf_pkt_format_info_t *format_info_array;

    /*! Bitmap for Chunks status */
    SHR_BITDCL hw_bmap[UDF_MAX_PIPES];

    /*! Bitmap for chunks byte status  */
    SHR_BITDCL byte_bmap[UDF_MAX_PIPES];

    /*! pointer to chunk info */
    bcmi_xgs_udf_offset_entry_t *offset_entry_arr[UDF_MAX_PIPES];

    /*! UDF hash chunks info */
    bcmi_xgs_udf_hash_chunks_info_t hash_chunks_info;

    /*! UDF application offsets info */
    bcmi_xgs_udf_appl_chunks_info_t appl_chunks_info;

    /*! UDF table info */
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info;

    /*! UDF field enum symbol get */
    bcmi_xgs_udf_enum_sym_get_f enum_sym_get;

} bcmi_xgs_udf_control_info_t;

/* Variable for XGS UDF control information */
extern bcmi_xgs_udf_control_info_t *xgs_udf_ctrl_info[BCM_MAX_NUM_UNITS];

#define XGS_UDF_CTRL(_u_)           xgs_udf_ctrl_info[(_u_)]

/* Maximum parsable bytes */
#define XGS_UDF_PARSE_BYTES(_u_)    XGS_UDF_CTRL(_u_)->max_parse_bytes

/* Maximum number of entries */
#define XGS_UDF_MAX_ENTRIES(_u_)    XGS_UDF_CTRL(_u_)->max_entries

/* Granularity */
#define XGS_UDF_CHUNK_GRAN(_u_)     XGS_UDF_CTRL(_u_)->gran

/* Table info */
#define XGS_UDF_TBL(_u_)            XGS_UDF_CTRL(_u_)->tbls_info

/* UDF Enum symbol get function pointer */
#define XGS_UDF_ENUM_SYM_GET(_u_)   XGS_UDF_CTRL(_u_)->enum_sym_get

#define XGS_UDF_CHUNK_BYTE_BMAP_GET(_bbmap_, _c_, _g_, _cbmap_)     \
    SHR_BITCOPY_RANGE((_cbmap_), 0, (_bbmap_), ((_c_)*(_g_)), (_g_))

/* Hw bitmap of Udf */
#define XGS_UDF_OBJ_OFFSET_HW_BMAP_GET(_u_, _info_, _hw_bmap_)      \
    do {                                                            \
        int _c;                                                     \
        uint32 _val;                                                \
        (_hw_bmap_) = 0;                                            \
        for (_c = 0; _c < BCMINT_UDF_MAX_CHUNKS(_u_); _c++) {       \
            _val = 0;                                               \
            XGS_UDF_CHUNK_BYTE_BMAP_GET(&((_info_)->byte_bmap),     \
                    _c, XGS_UDF_CHUNK_GRAN(_u_), &_val);            \
            if (_val) {                                             \
                (_hw_bmap_) |= (1 << _c);                           \
            }                                                       \
        }                                                           \
    } while (0)

/*
 * Typedef:
 *     bcmi_xgs_udf_pkt_format_misc_type_info_t
 * Purpose:
 *     Internal structure for UDF LT boolean Key
 *     related information.
 */
typedef struct bcmi_xgs_udf_pkt_format_misc_type_info_s {
    /* Field name */
    const char *field;

    /* Mask name */
    const char *mask;

    /* Enum value for Present in packet format */
    uint8 present;

    /* Enum value for not present in packet format */
    uint8 none;

    /*Enum value for do not care condition */
    uint8 any;

} bcmi_xgs_udf_pkt_format_misc_type_info_t;

/*
 * Typedef:
 *     bcmi_xgs_udf_lt_hash_config_t
 * Purpose:
 *     Internal structure for Hash configuration
 *     for a chunk.
 */
typedef struct bcmi_xgs_udf_lt_hash_config_s {
    /*! Number of hash related chunks */
    int chunks;

    bool chunk_en[BCMI_XGS_UDF_HASH_CHUNKS_MAX];

    /* Hash Mask */
    uint16 mask[BCMI_XGS_UDF_HASH_CHUNKS_MAX];

} bcmi_xgs_udf_lt_hash_config_t;

/*!
 * \brief Allocate XGS UDF Control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_control_alloc(int unit);

/*!
 * \brief Free XGS UDF Control.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_control_free(int unit);


/*!
 * \brief Initialize UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_init(int unit);

/*!
 * \brief Detach UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_detach(int unit);

/*!
 * \brief Destroy UDF object
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to find the UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to create the UDF object
 *
 * \param [in] unit     Unit number.
 * \param [in] hints    Hints
 * \param [in] udf_info Udf info
 * \param [in] udf_id   Udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id);

/*!
 * \brief Function to get the UDF object
 *
 * \param [in] unit      Unit number.
 * \param [in] udf_id    Udf object ID.
 * \param [out] udf_info Udf info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info);

/*!
 * \brief Retrieve multi-chunk info for a given UDF object
 *
 * \param [in]  unit    Unit number.
 * \param [in]  udf_id  udf object ID.
 * \param [out] info    Reference to multi-chunk info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info);

/*!
 * \brief Function to get all UDF objects
 *
 * \param [in] unit    Unit number.
 * \param [in] max     Size of udf_id_list.
 * \param [out] udf_id_list  udf object ID list
 * \param [inout] actual valid size of udf object ID list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_get_all(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

/*!
 * \brief Function to create UDF packet format object
 *
 * \param [in] unit Unit number.
 * \param [in] options Options
 * \param [in] pkt_format Packet format info
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id);

/*!
 * \brief Function to get UDF packet format information.
 *
 * \param [in] unit Unit number.
 * \param [in] pkt_format_id  udf pkt format object ID.
 * \param [out] Packet format info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format);

/*!
 * \brief Function to destroy UDF packet format object.
 *
 * If UDF packet format is associated with UDF object,
 * this API shall fail with SHR_E_BUSY.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_destroy(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to check if the UDF packet format object exists.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_is_pkt_format_exist(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to associated UDF and Packet format objects.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [out] pkt_format_id  udf packet format object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to get list of all UDF objects associated
 *        with given UDF packet format.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id  udf packet format object ID.
 * \param [in] max Size of 'udf_id_list'
 * \param [out] udf_id_list List of Udfs to be filled.
 * \param [out] actual Actual number of valid elements in udf_id_list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

/*!
 * \brief Function to disassociate UDF object and UDF packet format Object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] pkt_format_id Udf Packet format Object
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id);

/*!
 * \brief Function to get all packet format Object associated with
 *        given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] max Size of 'pkt_format_id_list'
 * \param [out] udf_id_list List of Packet formats to be filled.
 * \param [out] actual Actual number of valid elements in pkt_foramt_id_list
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual);

/*!
 * \brief Function to disassociate all packet format Objects
 *        associated with given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_delete_all(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to operational mode of UDF module.
 *
 * \param [in] unit    Unit number.
 * \param [out] mode UDF operational mode.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode);

/*!
 * \brief Function to add UDF hash configurations to the
 *        given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] options Options.
 * \param [in] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to delete UDF hash configurations from
 *        UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [in] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to delete all UDF hash configurations.
 *
 * \param [in] unit    Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_hash_config_delete_all(
    int unit);

/*!
 * \brief Function to get UDF hash configurations associated
 *        with given UDF object.
 *
 * \param [in] unit    Unit number.
 * \param [out] config UDF hash configurations.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config);

/*!
 * \brief Function to get all UDF hash configurations.
 *
 * \param [in] unit    Unit number.
 * \param [in] max Size of config_list
 * \param [out] config_list List to be populated.
 * \param [out] actual Actual number of valid elements in config_list.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual);

/*!
 * \brief Function to apply action to pkt format id.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id UDF Packet format Object Id.
 * \param [in] action Actions.
 * \param [in] arguemnt form action.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_action_set(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg);

/*!
 * \brief Function to get action for pkt format id.
 *
 * \param [in] unit    Unit number.
 * \param [in] pkt_format_id UDF Packet format Object Id.
 * \param [in] action Actions.
 * \param [out] arguemnt form action.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_pkt_format_action_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg);

/*!
 * \brief Function to check given UDF Object is Range Check
 *
 * \param [in] unit    Unit number.
 * \param [in] UDF Object
 * \param [out] result
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
xgs_ltsw_udf_obj_is_range_check(
        int unit,
        bcm_udf_id_t udf_id,
        bool *res);

#endif /* XGS_LTSW_UDF_H */
