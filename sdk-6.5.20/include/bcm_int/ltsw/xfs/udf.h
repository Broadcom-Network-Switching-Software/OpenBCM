/*! \file udf.h
 *
 * UDF headfiles to declare internal APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XFS_LTSW_UDF_H
#define XFS_LTSW_UDF_H

#include <bcm/udf.h>

/******************************************************************************
 * Local definitions
 */

/* UDF LT table info */
typedef struct bcmint_udf_lt_tbls_info_s {
    /* UDF template SID */
    char *udf_sid;
    /* UDF Object key FID */
    char *udf_key_fid;
    /* UDF Anchor FID */
    char *udf_anchor_fid;
    /* UDF Offset FID */
    char *udf_offset_fid;
    /* UDF Container_1_byte FID */
    char *udf_cont_1_byte_fid;
    /* UDF Container_2_byte FID */
    char *udf_cont_2_byte_fid;
    /* UDF Container_4_byte FID */
    char *udf_cont_4_byte_fid;
    /* UDF info template SID */
    char *udf_info_sid;
    /* UDF info key template SID */
    char *udf_info_key_fid;
    /* UDF info num_cont_1_byte FID */
    char *udf_num_cont_1b_fid;
    /* UDF info num_cont_2_byte FID */
    char *udf_num_cont_2b_fid;
    /* UDF info num_cont_4_byte FID */
    char *udf_num_cont_4b_fid;
    /* UDF info max_container FID */
    char *udf_max_cont_fid;
} bcmint_udf_lt_tbls_info_t;

/* UDF table info. */
extern bcmint_udf_lt_tbls_info_t *udf_tbls_info[BCM_MAX_NUM_UNITS];

/* UDF packet format LT info flags */
#define BCMINT_UDF_LT_ANCHOR_TYPE_OUTER  (1 << 0)
#define BCMINT_UDF_LT_ANCHOR_TYPE_INNER  (1 << 1)

/*
 * UDF abstract pkt format info
 */
typedef struct bcmint_udf_abstr_pkt_fmt_ltinfo_s {
    char        *pkt_header;    /* LT Packet header field */
    int         flags;          /* flags */
    uint16_t    num_1b_chunks;  /* Number of 1 byte containers. */
    uint16_t    num_2b_chunks;  /* Number of 2 byte containers. */
    uint16_t    num_4b_chunks;  /* Number of 4 byte containers. */
    uint16_t    num_avail_chunks; /* Number of available chunks supported. */
    uint32_t    avail_1b_cbmap; /* Valid 1 byte chunk bitmap */
    uint32_t    avail_2b_cbmap; /* Valid 2 byte chunk bitmap */
    uint32_t    avail_4b_cbmap; /* Valid 4 byte chunk bitmap */
} bcmint_udf_abstr_pkt_fmt_ltinfo_t;

extern bcmint_udf_abstr_pkt_fmt_ltinfo_t
         *ltsw_udf_abstr_pkt_fmt_ltinfo[BCM_MAX_NUM_UNITS]
                                       [bcmUdfAbstractPktFormatLastCount];

/*!
 * \brief Initialize UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_init(int unit);

/*!
 * \brief Detach UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_detach(int unit);

/*!
 * \brief Create UDF object for a given multi-chunk info.
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 * \param [in] info    Reference to multi-chunk info.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_multi_chunk_create(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info);

/*!
 * \brief Retrieve multi-chunk info for a given UDF object
 *
 * \param [in]  unit    Unit number.
 * \param [in]  udf_id  udf object ID.
 * \param [out] info    Reference to multi-chunk info.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_multi_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info,
    int *flags);

/*!
 * \brief Destroy UDF object
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to find the UDF object 
 *
 * \param [in] unit    Unit number.
 * \param [in] udf_id  udf object ID.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id);

/*!
 * \brief Function to find the UDF object 
 *
 * \param [in]  unit          Unit number.
 * \param [in]  max           Max UDF array size.
 * \param [in]  udf_obj_list  Array of udf object IDs.
 * \param [out] actual        Actual UDF array size.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
xfs_ltsw_udf_object_list_get(
    int unit,
    int max,
    bcm_udf_id_t *udf_obj_list,
    int *actual);

extern int
xfs_ltsw_udf_multi_pkt_fmt_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t pkt_fmt,
    bcm_udf_multi_abstract_pkt_format_info_t *info);

extern int
xfs_ltsw_udf_abstr_pkt_fmt_object_list_get(
    int unit, 
    bcm_udf_abstract_pkt_format_t pkt_fmt, 
    int max, 
    bcm_udf_id_t *udf_id_list, 
    int *actual);
extern int
xfs_ltsw_udf_abstr_pkt_fmt_list_get(
    int unit,
    int max,
    bcm_udf_abstract_pkt_format_t *pkt_fmt_list,
    int *actual);
#endif /* XFS_LTSW_UDF_H */
