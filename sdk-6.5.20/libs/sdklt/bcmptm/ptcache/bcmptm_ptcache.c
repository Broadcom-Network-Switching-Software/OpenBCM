/*! \file bcmptm_ptcache.c
 *
 * Miscellaneous utilities for PTM
 *
 * This file implements PTcache (SW cache) for physical tables
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/generated/bcmptm_ptcache_local.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CACHE

/* #define PTCACHE_REGS_DISABLE */
    /* Note: regs protected by ser will still be cached */

#define PTCACHE_SENTINEL_ME_ENABLE
    /* Enable SENTINEL also for multi_entry_OR_cam_SIDs */

#define PTCACHE_SENTINEL_SE_ENABLE
    /* Enable SENTINEL also for single_entry_AND_non_cam_SIDs
     * For these type of SIDs, info is stored directly in varinfo_array without
     * any data_seg, valid_seg. Most of these SIDs are reg32, reg64, so Sentinel
     * will add 100% or 50% overhead respectively */

#define PTCACHE_FLAGS_LTID_SIZE16 0x00000001

#define SINFO_NUM_RSVD_WORDS 8

#define SINFO_RSVD_INDEX_SIGN_WORD -8
    /* word in sinfo_array to store sinfo_signature word */

#define SINFO_RSVD_INDEX_UNUSED -7
    /* word in sinfo_array - unused */

#define SINFO_RSVD_INDEX_FLAGS_WORD -6
    /* word in sinfo_array to store flags_word */

#define SINFO_RSVD_INDEX_SID_COUNT_WORD -5
    /* word in sinfo_array to store sid_count */

#define SINFO_RSVD_INDEX_SEDS_COUNT_WORD -4
    /* word in sinfo_array to store num of words in seds section of seds_cwme */

#define SINFO_RSVD_INDEX_CWME_COUNT_WORD -3
    /* word in sinfo_array to store num of cwme elements */

#define SINFO_RSVD_INDEX_RSVD2 -2
    /* word in sinfo_array - reserved */

#define SINFO_RSVD_INDEX_RSVD1 -1
    /* word in sinfo_array - reserved */

#define SER 1
#define ALSO_INIT_SER_PTCACHE_P TRUE

/* Defn for PTCACHE_TYPE in sinfo */
#define PTCACHE_TYPE_UNINIT 0        /* Has not yet been initialized */
#define PTCACHE_TYPE_NO_CACHE 1
#define PTCACHE_TYPE_ME 2            /* multi entry - data_seg, valid_seg */
#define PTCACHE_TYPE_ME_CCI 3        /* counter which also has cfg fields and is
                                        therefore also cached */
#define PTCACHE_TYPE_SE_NOT_VALID 4  /* single entry, not_valid */
#define PTCACHE_TYPE_SE_VALID 5      /* single entry, valid */
#define PTCACHE_TYPE_CCI_ONLY 6      /* completely managed by CCI */
#define PTCACHE_TYPE_ME_RSVD 7       /* Reserved for PT with 0 entries
                                        (in non-Default PT group) */
#define PTCACHE_TYPE_SE_CACHE0 8     /* Cacheable but without resources */

/* Max value of SID that can be stored in overlay_info word */
#define OVERLAY_INFO_SID_WIDTH 22 /* 4 M SIDs */
#define OVERLAY_INFO_SID_MAX (1 << OVERLAY_INFO_SID_WIDTH)
/* Number of narrow view entries in one widest view entry */
#define OVERLAY_INFO_NUM_NE_WIDTH 5 /* 32 NEs in 1 WE */
#define OVERLAY_INFO_NUM_NE_MAX (1 << OVERLAY_INFO_NUM_NE_WIDTH)
/* Number of wide_entry_fields needed to create narrow_entry */
#define OVERLAY_INFO_WEF_WIDTH 4 /* 16 wide_entry_fields in 1 narrow entry */
#define OVERLAY_INFO_NUM_WEF_MAX (1 << OVERLAY_INFO_WEF_WIDTH)

/* Max value of narrow_view_width that can be stored in overlay_info word */
#define OVERLAY_INFO_NE_WIDTH 16 /* 64K bits Narrow entry width */
#define OVERLAY_INFO_NE_WIDTH_FMAX (1 << OVERLAY_INFO_NE_WIDTH)

/* Max value of start_bit that can be stored in overlay_info word */
#define OVERLAY_INFO_SB_WIDTH 16 /* 64K start_bit position */
#define OVERLAY_INFO_SB_FMAX (1 << OVERLAY_INFO_SB_WIDTH)

/* wsize of overlay_info */
#define OVERLAY_INFO_WSIZE 2 /* num uint32_t words to store overlay info  */

#define XINFO_NUM_WEF_WIDTH 8
#define XINFO_NUM_WEF_MAX ((1 << XINFO_NUM_WEF_WIDTH) - 4)
/*
 * To support 1 (width, start_bit) words,   we need 1   words in data_seg
 * To support 2 (width, start_bit) words,   we need 2   words in data_seg
 * To support N (width, start_bit) words,   we need N   words in data_seg
 * To support 255 (width, start_bit) words, we need 255 words in data_seg
 *
 * 255 = (2^8 - 1)
 *
 * I reserved more by using (2^8 - 4) just to be safe and
 * also illustrate how we can reserve space in data_seg for other uses
 * besides (width, start_bit) pairs
 */

/* Indicators that start_bank is not applicable/supported */
#define PTCACHE_GROUP_START_BANK_RSVD 0xFFFFFFFF


/*******************************************************************************
 * Typedefs
 */
/*! Definition of bits in overlay_info_word0
 */
typedef union ptcache_oinfo_w0_s {
    struct {
        /*! sid of widest view */
        uint32_t sid:OVERLAY_INFO_SID_WIDTH;

        /*! num of narrow_view_entries in one widest_view_entry */
        uint32_t num_ne:OVERLAY_INFO_NUM_NE_WIDTH;

        /*! num of wide_entry_fields in one narrow_entry */
        uint32_t num_wef:OVERLAY_INFO_WEF_WIDTH;

        /*! sid is of type alpm - needs special address handling */
        uint32_t addr_mode_alpm:1;
    } f;
    uint32_t entry;
} ptcache_oinfo_w0_t;

/*! Definition of bits in overlay_info_word1
 */
typedef union ptcache_oinfo_wn_s {
    struct {
        /*! Bit-width of overlay (narrow) view
         *  For Hash type overlay, oinfo_w1.width - represents width_ne
         *  For Tcam type overlay, oinfo_w1.width - represents width of
         *  wide_entry_field */
        uint32_t width:OVERLAY_INFO_NE_WIDTH;

        /*! Start Bit of NE in WE
         *  Meaningful only for Tcam type overlay. Specifies start position of
         *  field in wide entry */
        uint32_t start_bit:OVERLAY_INFO_SB_WIDTH;
    } f;
    uint32_t entry;
} ptcache_oinfo_wn_t;

typedef struct pt_group_ptrs_s {
    uint32_t *vinfo_bp;
    void *ltid_seg_base_ptr;
} pt_group_ptrs_t;


/*******************************************************************************
 * Private variables
 */
static bcmptm_ptcache_sinfo_t *sinfo_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_pt_group_info_t *pt_group_info_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static pt_group_ptrs_t *pt_group_ptrs_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static uint32_t *seds_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bcmptm_ptcache_cwme_t *cwme_bp[BCMDRD_CONFIG_MAX_UNITS][2];
static bool ltid_size16b[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static inline int
tbl_inst_count_get(int u, bcmdrd_sid_t sid, uint32_t *tbl_inst_count)
{
    int tinst_count;
    tinst_count = bcmdrd_pt_num_tbl_inst(u, sid);
    if (tinst_count < 0) {
        return SHR_E_INTERNAL;
    }
    *tbl_inst_count = tinst_count;
    return SHR_E_NONE;
}

/*!
 * \brief Detect reg that is protected by SER
 */
#if defined(PTCACHE_REGS_DISABLE)
static bool
ptcache_pt_is_reg_with_ser(int u, bcmdrd_sid_t sid)
{
    bool is_reg;
    bcmdrd_ser_resp_t ser_resp;

    is_reg = bcmdrd_pt_is_reg(u, sid);
    ser_resp = bcmptm_pt_ser_resp(u, sid);
    return (is_reg &&
            (ser_resp == BCMDRD_SER_RESP_CACHE_RESTORE) &&
            (ser_resp == BCMDRD_SER_RESP_ECC_CORRECTABLE) &&
            (ser_resp == BCMDRD_SER_RESP_WRITE_CACHE_RESTORE)
           );
}
#endif /* PTCACHE_REGS_DISABLE */

/*!
 * \brief Compute the sentinel word for sid
 * \n - one sentinel word is allocated for every data_seg, valid_seg,
 * tcam_corrupt_seg
 * \n - sentinel words are stored in vinfo array just before the word0 of
 * data_seg, valid_seg, tcam_corrupt_seg
 * \n - one sentinel word is allocated for each SE type PT and is stored just
 * before the data word0 of SE
 * \n - sentinel word will be unique for each segment of PT
 */
static uint32_t
ptcache_sentinel_calc(int u, bcmdrd_sid_t sid, uint32_t offset)
{
    return (sid << u) + u + offset;
    /* Purpose of offset is to create unique sentinel word for each segment of
     * same sid.
     * Similarly, using 'u' helps uniquify sentinel word for same sid on
     * different units
     */
}

/*!
 * \brief Verify the sentinel word for sid
 */
static int
ptcache_sentinel_check(int u, bcmdrd_sid_t sid, uint32_t offset,
                       uint32_t obs_sentinel, const char *sentinel_type,
                       bool ltid_seg)
{
    uint32_t exp_sentinel;
    if (ltid_seg) {
        if (ltid_size16b[u]) { /* 16b ltid */
            exp_sentinel = BCMPTM_LTID_SENTINEL16;
        } else { /* 32b ltid */
            exp_sentinel = BCMPTM_LTID_SENTINEL;
        }
    } else {
        exp_sentinel = ptcache_sentinel_calc(u, sid, offset);
    }

    if (exp_sentinel != obs_sentinel) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "Sentinel %s Mismatch for sid=%0d, pt=%s, "
                        "exp_sentinel=0x%8x, obs_sentinel=0x%8x, "
                        "vinfo_sw_index=%0d\n"),
             sentinel_type, sid, bcmdrd_pt_sid_to_name(u, sid),
             exp_sentinel, obs_sentinel, offset));
        return SHR_E_FAIL;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Sentinel %s ok for sid=%0d, pt=%s, "
                        "exp_sentinel=0x%8x, obs_sentinel=0x%8x, "
                        "vinfo_sw_index=%0d\n"),
             sentinel_type, sid, bcmdrd_pt_sid_to_name(u, sid),
             exp_sentinel, obs_sentinel, offset));
        return SHR_E_NONE;
    }
}

/*!
 * \brief Init overlay info for a given sid
 */
static int
ptcache_overlay_info_set(int u, bcmdrd_sid_t sid,
                         const bcmptm_overlay_info_t *overlay_info,
                         uint32_t *in_vinfo_ptr,
                         uint32_t *overlay_num_cw)
{
    ptcache_oinfo_w0_t oinfo_w0;
    ptcache_oinfo_wn_t oinfo_w1;
    ptcache_oinfo_wn_t oinfo_wn;
    bcmptm_field_info_raw_t *wef_array;
    uint32_t i = 0, j = 0, *vinfo_p = in_vinfo_ptr;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT((overlay_info->w_sid >= OVERLAY_INFO_SID_MAX) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->num_ne >= OVERLAY_INFO_NUM_NE_MAX) ?
                    SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->num_we_field >= OVERLAY_INFO_NUM_WEF_MAX) ?
                    SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->width_ne >= OVERLAY_INFO_NE_WIDTH_FMAX) ?
                    SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT((overlay_info->start_bit_ne_in_we >= OVERLAY_INFO_SB_FMAX) ?
                    SHR_E_PARAM : SHR_E_NONE);

    oinfo_w0.entry = 0;
    oinfo_w0.f.sid = overlay_info->w_sid;
    oinfo_w0.f.num_ne = overlay_info->num_ne;
    oinfo_w0.f.num_wef = overlay_info->num_we_field;
    oinfo_w0.f.addr_mode_alpm = (overlay_info->is_alpm ? 1 : 0);
    *vinfo_p = oinfo_w0.entry; /* @ i = 0 */
    vinfo_p++;
    i++;

    oinfo_w1.entry = 0;
    oinfo_w1.f.width = overlay_info->width_ne;
    oinfo_w1.f.start_bit = overlay_info->start_bit_ne_in_we;
    *vinfo_p = oinfo_w1.entry; /* @ i = 1 */
    vinfo_p++;
    i++;

    if (overlay_info->num_we_field <= 1) {
        SHR_EXIT();
    }

    wef_array = overlay_info->we_field_array;
    SHR_NULL_CHECK(wef_array, SHR_E_INTERNAL);
    for(j = 0; (j + 1) < overlay_info->num_we_field; j++) {
        oinfo_wn.entry = 0;
        oinfo_wn.f.width = wef_array[j].width;
        oinfo_wn.f.start_bit = wef_array[j].start_bit;

        *vinfo_p = oinfo_wn.entry; /* @ i = 2 + j */
        vinfo_p++;
        i++;

        /* Overlay w0, w1 are already written
         *
         * num_we_field   j       i = 2+j (overlay cw)
         *     2          0       2
         *     3          0,1     2,3
         *     4          0,1,2   2,3,4
         *     ...        ...     ...
         */
    } /* write info for remaining fields */

exit:
    *overlay_num_cw = i; /* total num of overlay cw that were written */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find narrow entry details from wide entry.
 * - For given index of narrow entry, find index of corresponding wide_entry
 * - Find start_bit, end_bit for narrow entry in wide entry
 * - Common to both ptcache_read and ptcache_update functions
 */
static int
ptcache_w_entry_calc(int u, ptcache_oinfo_w0_t *oinfo_w0,
                     ptcache_oinfo_wn_t *oinfo_w1,
                     uint32_t tbl_inst, uint32_t n_index,
                     uint32_t w_pt_index_count,
                     uint32_t w_entry_wsize,
                     uint32_t *w_index, /* out */
                     uint32_t *w_sbit, uint32_t *w_ebit, /* out */
                     uint32_t *w_vinfo_ds_index)    /* in, out */
{
    uint32_t w_sub_index, w_entry_num;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(!w_pt_index_count ? SHR_E_INTERNAL : SHR_E_NONE);

    /* For given n_index within table instance of narrow view,
     * find within table instance of wide view,
     * a. row (w_index) in wide view, and
     * b. sub_index (w_sub_index) within that row of wide view */
    if (oinfo_w0->f.addr_mode_alpm) {
        *w_index = n_index & (w_pt_index_count - 1);
        SHR_IF_ERR_EXIT((*w_index >= w_pt_index_count) ? SHR_E_INTERNAL :
                                                         SHR_E_NONE);
        SHR_IF_ERR_EXIT(w_pt_index_count <= 0? SHR_E_INTERNAL : SHR_E_NONE);
        w_sub_index = n_index / w_pt_index_count;
        SHR_IF_ERR_EXIT((w_sub_index >= oinfo_w0->f.num_ne) ? SHR_E_INTERNAL
                                                           :  SHR_E_NONE);
    } else { /* non_alpm */
        if (oinfo_w1->f.start_bit) {
            *w_index = oinfo_w1->f.start_bit << 10;
                /*   = oinfo_w1->f.start_bit * 1024 */
            *w_index += n_index / oinfo_w0->f.num_ne;
            /* Example:
             * Two narrow entry views (1x) of 64K each point to two halves of
             * the same 32K 4x view (example: n_index = 32K, num_ne = 4):
             * For 64K _SINGLEm view (lower addr range):
             *      start_bit (offset) = 0, so
             *      w_1ndex = n_index/num_ne; = 32K/4 = 8K;
             * For 64K _SINGLE_2m view (higher addr range):
             *      start_bit (offset) = 16, so
             *      w_1ndex = 16K + n_index/num_ne; = 16K + (32K/4) = 24K;
             */
        } else { /* save energy to shift and add */
            *w_index = n_index / oinfo_w0->f.num_ne;
        }
        SHR_IF_ERR_EXIT((*w_index >= w_pt_index_count) ? SHR_E_INTERNAL
                                                       : SHR_E_NONE);
        w_sub_index = n_index % oinfo_w0->f.num_ne;
        /* Example:
         * num_ne = 1 :there are 1 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0          0          0
         * 1          1          0
         * ....       ....       ....
         *
         * num_ne = 2 :there are 2 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1        0          0,1
         * 2,3        1          0,1
         * ....       ....       ....
         *
         * num_ne = 3 :there are 3 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1,2      0          0,1,2
         * 3,4,5      1          0,1,2
         * ....       ....       ....
         *
         * num_ne = 4 :there are 4 narrow_entries in every wide_entry
         * n_index    w_index    w_sub_index
         * 0,1,2,3    0          0,1,2,3
         * 4,5,6,7    1          0,1,2,3
         * ....       ....       ....
         */
    }

    w_entry_num = (tbl_inst * w_pt_index_count) + (*w_index);

    if (w_vinfo_ds_index != NULL) {
        *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* points to 1st uint32_t word of wide entry */
    }

    if ((w_sbit != NULL) && (w_ebit != NULL)) {
        *w_sbit = oinfo_w1->f.width * w_sub_index;
        /* Example, assume a 420b entry:
         * width    w_sub_index     sbit
         * 70       0,1,2,3,4,5     0,70,140,210,280,350
         * 105      0,1,2,3         0,105,210,315
         * 140      0,1,2           0,140, 280
         * 210      0,1             0,210
         */
        *w_ebit = *w_sbit + oinfo_w1->f.width - 1;
        /* Above ASSUMES that HW always concatenates narrow view entries in one wide
         * view entry - WITHOUT ANY HIT_BITs/ECC_BITs in between */
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Read narrow entry from wide entry for TCAM_DATA type overlay
 * - This is overlay type where we need to assemble narrow entry from one or
 * more fields in wide entry
 */
static int
ptcache_overlay_tcam_data_read(int u,
                               ptcache_oinfo_w0_t *oinfo_w0,
                               uint32_t *vinfo_bp,
                               uint32_t ds_index,
                               uint32_t tbl_inst,
                               uint32_t n_index,
                               uint32_t w_pt_index_count,
                               uint32_t w_entry_wsize,
                               uint32_t n_entry_wsize,
                               uint32_t entry_count,

                               uint32_t *rsp_entry_words,  /* out */
                               uint32_t *w_vinfo_ds_index) /* in, out */
{
    uint32_t w_index, w_entry_num;
    uint32_t w_sbit, w_ebit, n_sbit, n_ebit, i, e;
    ptcache_oinfo_wn_t oinfo_wn;
    uint32_t w_field_buf[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *vinfo_p;
    SHR_FUNC_ENTER(u);

    /* assert(rsp_entry_words != NULL); ok to be NULL */

    w_index = n_index; /* for tcam_data overlays */
    SHR_IF_ERR_EXIT((w_index >= w_pt_index_count) ? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
    w_entry_num = (tbl_inst * w_pt_index_count) + w_index;
    *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* Offset to 1st uint32_t word of wide entry from which we need to
         * extract different fields to construct narrow entry */

    /* if caller does not need rsp data, exit */
    if (rsp_entry_words == NULL) {
        SHR_EXIT();
    }

    if (oinfo_w0->f.num_wef == 1) { /* Narrow entry has 1 wide_entry_field */
        /* Following computation is valid for all entries */
        n_sbit = 0;
        vinfo_p = vinfo_bp + ds_index; /* first (start, width) field info */
        oinfo_wn.entry = *vinfo_p;
        n_ebit = n_sbit + oinfo_wn.f.width - 1;

        w_sbit = oinfo_wn.f.start_bit;
        w_ebit = w_sbit + oinfo_wn.f.width - 1;

        for (e = 0; e < entry_count; e++) {
            /* sal_memset(w_field_buf, 0, sizeof(w_field_buf)); */

            /* Copy wide_entry_field into tmp field buffer */
            bcmdrd_field_get(vinfo_bp + (*w_vinfo_ds_index), /* w_entry_buf */
                             w_sbit, w_ebit, w_field_buf);

            /* Copy field from tmp field buffer to 'right place' in
             * rsp_entry_words */
            bcmdrd_field_set(rsp_entry_words, n_sbit, n_ebit, w_field_buf);

            /* Next entry */
            rsp_entry_words += n_entry_wsize;
            *w_vinfo_ds_index += w_entry_wsize;
        } /* foreach entry */
    } else { /* Narrow entry has multiple wide_entry_fields */
        for (e = 0; e < entry_count; e++) {
            /* Following assumes:
             * - Wide_entry_fields can be non-consecutive
             * - Narrow_entry_fields are filled consecutively
             * - Order of Wide_entry_field info in narrow view's overlay_info
             *   words must be such that we can fill narrow_entry_fields
             *   consecutively.This is why we dont need to store start_bit for
             *   narrow_entry_fields.
             *
             * - ds_index is already pointing to word which holds first (start, width)
             *   field info
             */
            n_sbit = 0;
            vinfo_p = vinfo_bp + ds_index; /* first (start, width) field info */
            for (i = 0; i < oinfo_w0->f.num_wef; i++) {
                oinfo_wn.entry = *vinfo_p;

                /* sal_memset(w_field_buf, 0, sizeof(w_field_buf)); */

                /* Copy wide_entry_field into tmp field buffer */
                w_sbit = oinfo_wn.f.start_bit;
                w_ebit = w_sbit + oinfo_wn.f.width - 1;
                bcmdrd_field_get(vinfo_bp + (*w_vinfo_ds_index), /* w_entry_buf */
                                 w_sbit, w_ebit, w_field_buf);

                /* Copy field from tmp field buffer to 'right place' in
                 * rsp_entry_words */
                n_ebit = n_sbit + oinfo_wn.f.width - 1;
                bcmdrd_field_set(rsp_entry_words, n_sbit, n_ebit, w_field_buf);
                n_sbit = n_ebit + 1;

                vinfo_p++;
            } /* foreach wide_entry_field */

            /* Next entry */
            rsp_entry_words += n_entry_wsize;
            *w_vinfo_ds_index += w_entry_wsize;
        } /* foreach entry */
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write narrow entry into wide entry for TCAM_DATA type overlay
 */
static int
ptcache_overlay_tcam_data_write(int u,
                                ptcache_oinfo_w0_t *oinfo_w0,
                                uint32_t *vinfo_bp,
                                uint32_t ds_index,
                                uint32_t tbl_inst,
                                uint32_t n_index,
                                uint32_t w_pt_index_count,
                                uint32_t w_entry_wsize,
                                uint32_t n_entry_wsize,
                                uint32_t entry_count,
                                uint32_t *req_entry_words,  /* in */

                                uint32_t *w_vinfo_ds_index) /* in, out */
{
    uint32_t w_index, w_entry_num;
    uint32_t w_sbit, w_ebit, n_sbit, n_ebit, i, e;
    ptcache_oinfo_wn_t oinfo_wn;
    uint32_t n_field_buf[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t *vinfo_p;
    SHR_FUNC_ENTER(u);

    /* assert(req_entry_words != NULL); ok for req_entry_words to be null */

    w_index = n_index; /* for tcam_data overlays */
    SHR_IF_ERR_EXIT((w_index >= w_pt_index_count) ? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
    w_entry_num = (tbl_inst * w_pt_index_count) + w_index;
    *w_vinfo_ds_index += w_entry_num * w_entry_wsize;
        /* Offset to 1st uint32_t word of wide entry where we need to
         * assemble narrow_entry_fields to construct wide entry */

    /* if caller does not want write of entry_words, exit */
    if (req_entry_words == NULL) {
        SHR_EXIT();
    }

    if (oinfo_w0->f.num_wef == 1) { /* Narrow entry has 1 wide_entry_field */
        /* Following computation is valid for all entries */
        n_sbit = 0;
        vinfo_p = vinfo_bp + ds_index; /* first (start, width) field info */
        oinfo_wn.entry = *vinfo_p;
        n_ebit = n_sbit + oinfo_wn.f.width - 1;

        w_sbit = oinfo_wn.f.start_bit;
        w_ebit = w_sbit + oinfo_wn.f.width - 1;

        for (e = 0; e < entry_count; e++) {
            /* sal_memset(n_field_buf, 0, sizeof(n_field_buf)); */

            /* Extract n_field from n_entry into n_field buffer */
            bcmdrd_field_get(req_entry_words, /* n_entry_buf */
                             n_sbit, n_ebit, n_field_buf);


            /* Copy n_field into 'right place' in w_entry */
            bcmdrd_field_set(vinfo_bp + (*w_vinfo_ds_index), /* w_entry_buf */
                             w_sbit, w_ebit, n_field_buf);

            /* Next entry */
            req_entry_words += n_entry_wsize;
            *w_vinfo_ds_index += w_entry_wsize;
        } /* foreach entry */
    } else { /* Narrow entry has multiple wide_entry_fields */
        for (e = 0; e < entry_count; e++) {
            /* Following assumes:
             * - Narrow_entry_fields are extracted consecutively
             * - Wide_entry_fields can be non-consecutive
             * - Order of Wide_entry_field info in narrow view's overlay_info
             *   cw must be such that we can extract narrow_entry_fields
             *   consecutively. This is why we dont need to store start_bit for
             *   narrow_entry_fields.
             *
             * - ds_index is already pointing to cw which holds first
             *   (start, width) field_info.
             */
            n_sbit = 0;
            vinfo_p = vinfo_bp + ds_index; /* first (start, width) field info */
            for (i = 0; i < oinfo_w0->f.num_wef; i++) {
                oinfo_wn.entry = *vinfo_p;

                /* sal_memset(n_field_buf, 0, sizeof(n_field_buf)); */

                /* Extract n_field from n_entry into n_field buffer */
                n_ebit = n_sbit + oinfo_wn.f.width - 1;
                bcmdrd_field_get(req_entry_words, /* n_entry_buf */
                                 n_sbit, n_ebit, n_field_buf);
                n_sbit = n_ebit + 1;

                /* Copy n_field into 'right place' in w_entry */
                w_sbit = oinfo_wn.f.start_bit;
                w_ebit = w_sbit + oinfo_wn.f.width - 1;
                bcmdrd_field_set(vinfo_bp + (*w_vinfo_ds_index), /* w_entry_buf */
                                 w_sbit, w_ebit, n_field_buf);
                vinfo_p++;
            } /* foreach wide_entry_field */

            /* Next entry */
            req_entry_words += n_entry_wsize;
            *w_vinfo_ds_index += w_entry_wsize;
        } /* foreach entry */
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Is size of ltid 16 bits or 32 bits
 */
static int
ptcache_ltid_size_set(int u)
{
    size_t ltid_count;
    SHR_FUNC_ENTER(u);

    ltid_count = BCMLTD_TABLE_COUNT;
    /* There may be 65K tables with LTIDs 0-(65K-1) in multi-u system.
     * Unit may support only 1K tables but with LTIDs ranging from 64K - (65K-1)
     * BCMLTD_TABLE_COUNT will be 65K in this case and bcmlrd_table_count_get()
     * will be 1K for this u. We will still need 32 bits to store LTIDs for
     * this u.
     */

    ltid_size16b[u] = FALSE; /* default - use 32b ltid */

    if (ltid_count < ((1 << 16) - 8)) {
        /* leave few (8) spots for things like BCMPTM_LTID_IREQ, etc */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Number of ltid = 0x%0x, ptcache will use 16b LTID\n"),
             (uint32_t)ltid_count));
        ltid_size16b[u] = TRUE; /* can use 16b ltid */
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Number of ltid = 0x%0x, ptcache will use 32b LTID\n"),
             (uint32_t)ltid_count));
    }
/* exit: */
    SHR_FUNC_EXIT();
}

/*!
 * \brief ltid_get helper function for ptcache_read()
 */
static int
ptcache_ltid_get(int u,
                 bcmdrd_sid_t sid,
                 uint32_t entry_num,
                 void *ltid_seg_base_ptr,
                 uint32_t ltid_seg_index,
                 bcmltd_sid_t *rsp_entry_ltid)
{
    SHR_FUNC_ENTER(u);

    if (ltid_size16b[u]) { /* 16b ltid */
        uint16_t *ltid_p = (uint16_t *)ltid_seg_base_ptr;
        *rsp_entry_ltid = *(ltid_p + ltid_seg_index + entry_num);
        SHR_IF_ERR_EXIT((*rsp_entry_ltid > BCMPTM_LTID_IREQ16) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        if (*rsp_entry_ltid == BCMPTM_LTID_IREQ16) {
            *rsp_entry_ltid = BCMPTM_LTID_IREQ;
        }
    } else { /* 32b ltid */
        uint32_t *ltid_p = (uint32_t *)ltid_seg_base_ptr;
        *rsp_entry_ltid = *(ltid_p + ltid_seg_index + entry_num);
        SHR_IF_ERR_EXIT((*rsp_entry_ltid > BCMPTM_LTID_IREQ) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
    } /* 32b ltid */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sid = %0d, ltid_seg_index=%0d\n"),
         sid, ltid_seg_index));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief dfid_get helper function for ptcache_read()
 */
static int
ptcache_dfid_get(int u,
                 bcmdrd_sid_t sid,
                 uint8_t dfid_bytes,
                 uint32_t entry_num,
                 uint32_t *vinfo_bp,
                 uint32_t vinfo_dfid_seg_index,
                 uint16_t *rsp_entry_dfid)
{
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        (dfid_bytes == 0 || dfid_bytes > 2) ? SHR_E_INIT : SHR_E_NONE);

    if (dfid_bytes == 2) { /* 16b dfid */
        uint16_t *dfid_seg_base_ptr;
        dfid_seg_base_ptr = (uint16_t *)(vinfo_bp + vinfo_dfid_seg_index);
        *rsp_entry_dfid = *(dfid_seg_base_ptr + entry_num);
        SHR_IF_ERR_EXIT((*rsp_entry_dfid > BCMPTM_DFID_MREQ) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be read
         * from cache */
    } else { /* 8b dfid */
        uint8_t *dfid_seg_base_ptr;
        dfid_seg_base_ptr = (uint8_t *)(vinfo_bp + vinfo_dfid_seg_index);
        *rsp_entry_dfid = *(dfid_seg_base_ptr + entry_num);
        SHR_IF_ERR_EXIT((*rsp_entry_dfid > BCMPTM_DFID_MREQ8) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be read
         * from cache */
        if (*rsp_entry_dfid == BCMPTM_DFID_IREQ8) {
            *rsp_entry_dfid = BCMPTM_DFID_IREQ;
        } else if (*rsp_entry_dfid == BCMPTM_DFID_MREQ8) {
            *rsp_entry_dfid = BCMPTM_DFID_MREQ;
        }
    } /* 8b dfid */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "dfid_seg_index=%0d\n"),
         vinfo_dfid_seg_index));
    if (*rsp_entry_dfid == BCMPTM_DFID_IREQ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Reserved Data Format ID (interactive) was read "
                        "for sid=%0d !!\n"),
             sid));
    } else if (*rsp_entry_dfid == BCMPTM_DFID_MREQ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Reserved Data Format ID (modeled) was read "
                        "for sid=%0d !!\n"),
             sid));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ltid_set helper function for ptcache_update()
 */
static int
ptcache_ltid_set(int u,
                 bcmdrd_sid_t sid,
                 uint32_t entry_num,
                 void *ltid_seg_base_ptr,
                 uint32_t ltid_seg_index,
                 bcmltd_sid_t req_ltid)
{
    SHR_FUNC_ENTER(u);

    if (ltid_size16b[u]) { /* 16b ltid */
        bcmltd_sid_t req2_ltid;
        uint16_t *ltid_p = (uint16_t *)ltid_seg_base_ptr;
        if (req_ltid == BCMPTM_LTID_IREQ) {
            req2_ltid = BCMPTM_LTID_IREQ16;
        } else {
            req2_ltid = req_ltid;
        }
        SHR_IF_ERR_EXIT((req2_ltid > BCMPTM_LTID_IREQ16) ? /* range chk */
                        SHR_E_INTERNAL : SHR_E_NONE);
        *(ltid_p + ltid_seg_index + entry_num) = req2_ltid;
    } else { /* 32b ltid */
        uint32_t *ltid_p = (uint32_t *)ltid_seg_base_ptr;
        SHR_IF_ERR_EXIT((req_ltid > BCMPTM_LTID_IREQ) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        *(ltid_p + ltid_seg_index + entry_num) = req_ltid;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "ltid_seg_index for write=%0d\n"),
         ltid_seg_index + entry_num));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief dfid_set helper function for ptcache_update()
 */
static int
ptcache_dfid_set(int u,
                 bcmdrd_sid_t sid,
                 uint8_t dfid_bytes,
                 uint32_t entry_num,
                 uint32_t *vinfo_bp,
                 uint32_t vinfo_dfid_seg_index,
                 uint16_t req_dfid)
{
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        (dfid_bytes == 0 || dfid_bytes > 2) ? SHR_E_INIT : SHR_E_NONE);

    if (req_dfid == BCMPTM_DFID_IREQ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Reserved Data Format ID is being written from "
                        "Interactive path for sid=%0d !!\n"),
             sid));
    } else if (req_dfid == BCMPTM_DFID_MREQ) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Reserved Data Format ID is being written from "
                        "Modeled path for sid=%0d !!\n"),
             sid));
    }
    if (dfid_bytes == 2) { /* 16b dfid */
        uint16_t *dfid_seg_base_ptr;
        dfid_seg_base_ptr = (uint16_t *)(vinfo_bp + vinfo_dfid_seg_index);

        SHR_IF_ERR_EXIT((req_dfid > BCMPTM_DFID_MREQ) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        /* usage of > in above allows DFID_IREQ, MREQ to be
         * written in cache */
        *(dfid_seg_base_ptr + entry_num) = req_dfid;
    } else { /* 8b dfid */
        uint16_t req_dfid2;
        uint8_t *dfid_seg_base_ptr;
        dfid_seg_base_ptr = (uint8_t *)(vinfo_bp + vinfo_dfid_seg_index);

        /* In 8b mode, user can provide following dfid values:
         *      dfid < IREQ8, or
         *      dfid = IREQ, or
         *      dfid = MREQ
         */
        if (req_dfid == BCMPTM_DFID_IREQ || req_dfid == BCMPTM_DFID_MREQ) {
            req_dfid2 = req_dfid & 0xFF; /* change to IREQ8, MREQ8 */
        } else {
            SHR_IF_ERR_EXIT((req_dfid >= BCMPTM_DFID_IREQ8) ? /* range chk */
                            SHR_E_INTERNAL : SHR_E_NONE);
            req_dfid2 = req_dfid;
        }
        *(dfid_seg_base_ptr + entry_num) = req_dfid2;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "dfid_seg_index=%0d\n"),
         vinfo_dfid_seg_index));

exit:
    SHR_FUNC_EXIT();
}

static inline int
ptcache_sid_chk(int u, int s, bcmdrd_sid_t sid)
{
    uint32_t all_sid_count;
    bcmptm_ptcache_sinfo_t *sinfo_bp_us = sinfo_bp[u][s];
    if (sinfo_bp_us == NULL) {
        return SHR_E_INIT;
    }
    all_sid_count = (sinfo_bp_us + SINFO_RSVD_INDEX_SID_COUNT_WORD)->cw_index;
    if (sid >= all_sid_count) {
        return SHR_E_UNAVAIL;
    } else {
        return SHR_E_NONE;
    }
}

static int
fill_null_entry(int u, bcmdrd_sid_t sid, uint32_t *in_ds_ptr)
{
    uint32_t i, j, entry_wsize, pt_entry_count, tbl_inst_count;
    uint32_t *ds_ptr = in_ds_ptr;
    const uint32_t *null_entryp;
    SHR_FUNC_ENTER(u);

    null_entryp = bcmdrd_pt_default_value_get(u, sid);
    entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
    SHR_NULL_CHECK(null_entryp, SHR_E_INTERNAL);
    entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
    pt_entry_count = bcmptm_pt_index_count(u, sid);
    SHR_IF_ERR_EXIT(tbl_inst_count_get(u, sid, &tbl_inst_count));

    for (i = 0; i < tbl_inst_count; i++) {
        for (j = 0; j < pt_entry_count; j++) {
            sal_memcpy(ds_ptr, null_entryp, 4 * entry_wsize);
            ds_ptr += entry_wsize;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
cache_vbit_update(int u, bool set_cache_vbit,
                  uint32_t *vinfo_bp,
                  uint32_t vinfo_vs_index_base,
                  uint32_t f_vinfo_vs_index,
                  uint32_t f_entry_num,
                  uint32_t entry_count)
{
    uint32_t tmp_word, f_entry_bit, l_entry_num, l_entry_bit,
             l_vinfo_vs_index, vinfo_vs_index_count;

    if (entry_count > 1) {
        f_entry_bit = f_entry_num % 32;

        l_entry_num = f_entry_num + entry_count - 1;

        /* vinfo_vs_index_base points to 1st word in valid_seg */
        l_vinfo_vs_index = vinfo_vs_index_base + (l_entry_num / 32);

        l_entry_bit = l_entry_num % 32;

        vinfo_vs_index_count = l_vinfo_vs_index - f_vinfo_vs_index + 1;

        if (vinfo_vs_index_count == 1) {
            int n;
            /* Set bits [l:f] in tmp_word */
            n = l_entry_bit - f_entry_bit + 1; /* n will be >= 1 */
            if (n < 32) { /* left-shift of 32 bit var by 32 is unpredictable */
                tmp_word = (1 << n) - 1; /* set n LSBs */
                tmp_word = tmp_word << f_entry_bit; /* insert 'f' zeros at
                                                       bits (f-1):0 */
            } else {
                tmp_word = 0xFFFFFFFF;
            }

            if (set_cache_vbit) {
                *(vinfo_bp + f_vinfo_vs_index) |= tmp_word;
            } else {
                *(vinfo_bp + f_vinfo_vs_index) &= ~tmp_word;
            }
        } else { /* multiple vinfo_vs words */
            /* First vinfo_vs entry */
            /* Need to set/clr bits [31:f_entry_bit] */
            tmp_word = 0xFFFFFFFF << f_entry_bit;
            if (set_cache_vbit) {
                *(vinfo_bp + f_vinfo_vs_index) |= tmp_word;
            } else {
                *(vinfo_bp + f_vinfo_vs_index) &= ~tmp_word;
            }

            /* Intermediate vinfo_vs entries */
            if (vinfo_vs_index_count > 2) {
                sal_memset(vinfo_bp + f_vinfo_vs_index + 1,
                           set_cache_vbit ? 0xFF : 0,
                           (vinfo_vs_index_count - 2) * 4);
            }

            /* Last entry */
            /* Need to set/clr bits [l_entry_bit:0] */
            tmp_word = 0xFFFFFFFF << l_entry_bit;
            tmp_word = tmp_word << 1;
            /* As per Coverity Left shift of 32 bit var by 32 can lead
             * to unpredictable results */
            if (set_cache_vbit) {
                *(vinfo_bp + l_vinfo_vs_index) |= ~tmp_word;
            } else {
                *(vinfo_bp + l_vinfo_vs_index) &= tmp_word;
            }
        } /* multiple vinfo_vs words */
    } else { /* entry_count = 1 */
        if (set_cache_vbit) {
            BCMPTM_WORD_BMAP_SET(*(vinfo_bp + f_vinfo_vs_index), f_entry_num);
        } else { /* clear cache_vbit */
            BCMPTM_WORD_BMAP_CLR(*(vinfo_bp + f_vinfo_vs_index), f_entry_num);
        }
    }
}

/* Write sinfo for this sid.
 * Compute number of words needed in se, cwme, vs, ds, dfs, is, tcs, ls
 * sections for this SID.
 *
 * For warm case:
 * - in_seds_index, in_cwme_index are dont_care
 * - all returned counts will be 0.
 * - skipped will be FALSE
 * - already_init will be true for non-CCI SIDs.
 *
 * For pt_size_change case:
 * - can be asserted only for non-DEFAULT group
 * - in_seds_index, in_cwme_index are dont_care
 * - Returned cwme_count, seds_count will be 0.
 * - Other returned counts may be 0 or non-zero depending on size of the table.
 * - cw_index in sinfo will be preserved.
 * - skipped can be TRUE for non-cacheable PTs in non-DEFAULT group
 *
 * Assume:
 * - mems which are declared as overlay type CACHE0 are not part of non-DEFAULT
 *   pt_group.
 * - This function is never called when acc_for_ser = True.
 */
static int
sinfo_init(int u, bool warm, bool pt_size_change, bcmdrd_sid_t sid,
           uint32_t pt_group, uint32_t in_seds_index, uint32_t in_cwme_index,
           bool *skipped, bool *already_init, uint32_t *seds_count,
           uint32_t *cwme_count, uint32_t *vsw_count, uint32_t *dsw_count,
           uint32_t *dfsw_count, uint32_t *isw_count, uint32_t *tcsw_count,
           uint32_t *lsw_count)
{
    uint32_t pt_index_count = 0, tbl_inst_count = 0, pt_iw_count = 0;
    const bcmptm_overlay_info_t *overlay_info;
    bool ltid_seg_en = FALSE;
    uint8_t dfid_seg_en = 0;
    bool pt_attr_is_cacheable = FALSE;
    bool pt_is_ctr_with_cfg_fields = FALSE;
    bool tc_seg_en = FALSE;
    uint32_t cci_map_id = 0;
    uint32_t total_vinfo_w_count;
    bcmptm_ptcache_sinfo_t in_sinfo, *sinfo_p;
    int s = 0;
    SHR_FUNC_ENTER(u);

    *skipped = FALSE;
    *already_init = FALSE;
    *seds_count = 0;
    *cwme_count = 0;
    *vsw_count = 0;
    *dsw_count = 0;
    *dfsw_count = 0;
    *isw_count = 0;
    *tcsw_count = 0;
    *lsw_count = 0;
    sinfo_p = sinfo_bp[u][s] + sid;
    in_sinfo = *sinfo_p; /* store initial value (that came in */

    if (warm) {
        /* All sinfo elements are already initialized - so no need to re-init.
         * - reduces WB time.
         * - ensures that we do not change SE_VALID to SE_NOT_VALID.
         *
         * Only counter related SIDs must be re-processed because CCI treats
         * WB same as CB.
         *
         * No need to weed out PTs of non-DEFAULT group when going through PTs
         * of DEFAULT group - because there is no re-init happening.
         */
        switch (in_sinfo.ptcache_type) {
        case PTCACHE_TYPE_UNINIT:
            /* All PTs are expected to have ptcache_type NONE/ME/SE */
            SHR_IF_ERR_MSG_EXIT(
                SHR_E_INTERNAL,
                (BSL_META_U(u, "WB: sinfo_init ptcache_type = uninit for "
                            "sid=%s in pt_group = %0u\n"),
                 bcmdrd_pt_sid_to_name(u, sid), pt_group));
            /* break; unreachable */

        case PTCACHE_TYPE_ME_CCI:
        case PTCACHE_TYPE_CCI_ONLY:
            /* No need to call bcmptm_cci_ctr_reg() */
            break;
        default:
            *already_init = TRUE;
            break;
        } /* ptcache_type */

        SHR_EXIT();
    } /* warm */

    /* Coldboot or pt_size_change */
    if (pt_size_change) {
        SHR_IF_ERR_MSG_EXIT(
            (in_sinfo.ptcache_type == PTCACHE_TYPE_UNINIT) ||
            (pt_group == BCMPTM_PT_GROUP_DEFAULT) ? SHR_E_INTERNAL : SHR_E_NONE,
            (BSL_META_U(u, "sinfo_init with pt_size_change called for"
                        "sid=%s in pt_group = %0d, ptcache_type = %0d\n"),
             bcmdrd_pt_sid_to_name(u, sid), pt_group, in_sinfo.ptcache_type));
    } else if (in_sinfo.ptcache_type != PTCACHE_TYPE_UNINIT) {
        *already_init = TRUE;
        /* Means we have already processed this SID for PTcache consideration.
         * We traverse all the SIDs in DEFAULT PT group and that too after we
         * traversed SIDs for all other non-default groups - so we can encounter
         * 'already initialized' PTs only when traversing DEFAULT group.
         */
        SHR_IF_ERR_MSG_EXIT(
            (pt_group != BCMPTM_PT_GROUP_DEFAULT) ? SHR_E_INTERNAL
                                                  : SHR_E_NONE,
            (BSL_META_U(u, "sinfo_init encountered already_init for "
                        "sid=%s in non-DEFAULT pt_group = %0d\n"),
             bcmdrd_pt_sid_to_name(u, sid), pt_group));
        SHR_EXIT(); /* goto exit without error */
    }

    pt_index_count = bcmptm_pt_index_count(u, sid);
    SHR_IF_ERR_EXIT(tbl_inst_count_get(u, sid, &tbl_inst_count));
    SHR_IF_ERR_EXIT(bcmptm_pt_attr_is_cacheable(u, sid,
                    &pt_attr_is_cacheable));

    sal_memset(sinfo_p, 0, sizeof(bcmptm_ptcache_sinfo_t));
            /* clear all fields */

    if (pt_index_count == 0 || tbl_inst_count == 0) {
        if (pt_group != BCMPTM_PT_GROUP_DEFAULT) {
            if (pt_attr_is_cacheable) {
                sinfo_p->ptcache_type = PTCACHE_TYPE_ME_RSVD;
                /* Reserve cwme ONLY for cacheable PTs in non-DEFAULT group */
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "pt=%s, sid=%0d, group=%0d"
                                "Caching SKIPPED as pt_is not cacheable\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid, pt_group));

                sinfo_p->ptcache_type = PTCACHE_TYPE_NO_CACHE;
                *skipped = TRUE;
                SHR_EXIT(); /* done */
            }

            /* For pt_size_change case, in_cwme_index is dont_care.
             * cw_index in sinfo is correct and MUST NOT be changed. */
            if (pt_size_change) {
                sinfo_p->cw_index = in_sinfo.cw_index;
            } else {
                /* Still reserve one entry in cwme array for each ME type SID */
                sinfo_p->cw_index = in_cwme_index;
                *cwme_count = 1;
            }
        } else { /* DEFAULT group */
            /* Should not be here for pt_size_change */
            SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

            /* Ret val of bcmptm_pt_index_count() is always >= 0
             * Ret val of 0 implies case where PT could be disabled due to
             * latency mode and no cache should be allocated
             */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "pt=%s, sid=%0d, "
                            "Caching SKIPPED as pt_index_count = %0d, "
                            "tbl_inst_count = %0d\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid, pt_index_count,
                 tbl_inst_count));

            sinfo_p->ptcache_type = PTCACHE_TYPE_NO_CACHE;
            *skipped = TRUE;
        }
        SHR_EXIT(); /* done */
    } else {
        if (pt_index_count > 1) {
           sinfo_p->index_en = 1;
        }
        if (tbl_inst_count > 1) {
            pt_index_count *= tbl_inst_count;
            sinfo_p->tbl_inst_en = 1;
        }
    }

    SHR_IF_ERR_EXIT(bcmptm_pt_needs_ltid(u, sid, &ltid_seg_en));
    SHR_IF_ERR_EXIT(bcmptm_pt_needs_dfid(u, sid, &dfid_seg_en));
    SHR_IF_ERR_EXIT(bcmptm_pt_overlay_info_get(u, sid, &overlay_info));
    SHR_IF_ERR_EXIT(bcmptm_tcam_corrupt_bits_enable(u, &tc_seg_en));
    SHR_IF_ERR_EXIT(bcmptm_pt_iw_count(u, sid, &pt_iw_count));

    /* Counters */
    if (bcmdrd_pt_is_valid(u, sid) && bcmdrd_pt_is_counter(u, sid)) {
        int tmp_rv = SHR_E_NONE;

        /* Should not be here for pt_size_change */
        SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

        /* ignore cci_map_id in SUB_PHASE_1 */
        tmp_rv = bcmptm_cci_ctr_reg(u, sid, &cci_map_id);
        if (SHR_FAILURE(tmp_rv)) {
            sinfo_p->ptcache_type = PTCACHE_TYPE_NO_CACHE;
            *skipped = TRUE;
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(u, "Caching SKIPPED for counter pt=%s, "
                            "sid=%0d as cci_ctr_reg failed (%0d)\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid, tmp_rv));
            SHR_FUNC_EXIT(); /* done */
        }

        pt_is_ctr_with_cfg_fields = pt_attr_is_cacheable;

        /* Do we need to cache this counter table in PTcache ? */
        if (pt_is_ctr_with_cfg_fields) { /* must cache this */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "pt=%s, sid=%0d, "
                            "Caching in PTcache enabled for COUNTER\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid));

            /* Prep vars to re-use PTCACHE_ME code */
            ltid_seg_en = FALSE;
            dfid_seg_en = FALSE;
            /* overlay_info->mode will be BCMPTM_OINFO_MODE_NONE */
            tc_seg_en = FALSE;
            pt_iw_count = 1; /* to store cci_map_id */

            /* Actual work will be done in PTCACHE_ME code below */

        } else { /* cci_only - no need to cache in PTcache */
            sinfo_p->ptcache_type = PTCACHE_TYPE_CCI_ONLY;
            *skipped = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "pt=%s, sid=%0d, Caching "
                            "in PTcache SKIPPED for CCI_ONLY COUNTER\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid));
            SHR_FUNC_EXIT(); /* done */
        } /* cci_only */
    } /* counters */

    if (bcmdrd_pt_is_valid(u, sid)
        && (pt_attr_is_cacheable || pt_is_ctr_with_cfg_fields)
        && pt_index_count /* alloc only if pt_index_count > 0 */
#if defined(PTCACHE_REGS_DISABLE)
        && (bcmdrd_pt_is_mem(u, sid) ||
            ptcache_pt_is_reg_with_ser(u, sid))
#endif /* PTCACHE_REGS_DISABLE */
       ) {
        if (overlay_info->mode == BCMPTM_OINFO_MODE_CACHE0) {
            /* We could be here even for non default PT_GROUPs, and therefore
             * even for pt_size_changes. */

            /* Treat such views like SE mems */

            sinfo_p->ptcache_type = PTCACHE_TYPE_SE_CACHE0;

            /* space to store data for CACHE0 type PTs - one 32 bit word */
            *seds_count = 1;
            /* We need nothing for such PTs, but to keep verification minimum
             * pretend that is is like a 32 bit reg from caching perspective. */

#if defined(PTCACHE_SENTINEL_SE_ENABLE)
            *seds_count += 1; /* sentinel for SE data */
            sinfo_p->cw_index = in_seds_index + 1;
#else
            sinfo_p->cw_index = in_seds_index;
#endif /* PTCACHE_SENTINEL_SE_ENABLE */

            /* Must not do this if warm is TRUE
             * (prevent marking valid register invalid in cache) */
        } else if ((pt_index_count == 1) &&
            !bcmdrd_pt_attr_is_cam(u, sid) &&
            !pt_is_ctr_with_cfg_fields &&
            (overlay_info->mode == BCMPTM_OINFO_MODE_NONE) &&
            !ltid_seg_en && !dfid_seg_en &&
            (pt_group == BCMPTM_PT_GROUP_DEFAULT)) {

            /* Should not be here for pt_size_change */
            SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

            sinfo_p->ptcache_type = PTCACHE_TYPE_SE_NOT_VALID;
            /* Covers REGs (and other single entry MEMs, if any) for which
             * during cache access, we can ignore index, tbl_inst.
             *
             * - For such PTs, there are no cwme element.
             *   sinfo.cw_index points directly to 1st data word
             *   for this PT in seds array.
             *
             *   For all other PTs, sinfo.cw_index points to cwme
             *   element and which in turn will provide offsets into valid,
             *   data, ltid, dfid, etc segments.
             *
             * - Note:
             *         Only PTs of pt_group=0 can be classified as SE - because
             *         seds is allocated as part of cwme array and there is no
             *         place to specify group id in sinfo.
             *         We could break sinfo.cw_index into
             *         (group id, index) for SE type mems - but because it
             *         is very rare to have SE in group other than pt_group 0,
             *         we can defer this optimization.
             *
             *         Solution: we classify all PTs in group other than
             *         pt_group 0 as ME (even if they have only single-entry).
             *
             * - Change of state from SE_NOT_VALID to SE_VALID will
             *   happen during write access and from SE_VALID to
             *   SE_NOT_VALID will happen by cache_invalidate
             *
             * - Note: Have purposely excluded case of single entry cam.
             *         Have purposely excluded case of single entry counter.
             *         In reality there should not be such sid and if there
             *         is one - we will treat it like multi-entry memory
             *         even though it is inefficient.
             */

            /* space to store data for SE type PTs */
            *seds_count = bcmdrd_pt_entry_wsize(u, sid);

#if defined(PTCACHE_SENTINEL_SE_ENABLE)
            *seds_count += 1; /* sentinel for SE data */
            sinfo_p->cw_index = in_seds_index + 1;
#else
            sinfo_p->cw_index = in_seds_index;
#endif /* PTCACHE_SENTINEL_SE_ENABLE */

            /* Must not do this if warm is TRUE
             * (prevent marking valid register invalid in cache) */
        } else { /* ME type - may or may not have pt_index_count > 1 */
            if (pt_is_ctr_with_cfg_fields) {
                sinfo_p->ptcache_type = PTCACHE_TYPE_ME_CCI;

                /* Should not be here for pt_size_change */
                SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);
            } else {
                sinfo_p->ptcache_type = PTCACHE_TYPE_ME;
            }

            /* Allocate one entry in cwme array for each ME type SID */
            if (!pt_size_change) {
                *cwme_count = 1;
            }

            /* space for valid_seg in vinfo array */
            *vsw_count    = BCMPTM_BITS2WORDS(pt_index_count);
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
            *vsw_count += 1; /* sentinel for valid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */


            /* overlay words / data words in data_seg in vinfo_array */
            if (overlay_info->mode != BCMPTM_OINFO_MODE_NONE) {
                bool w_pt_attr_is_cacheable = TRUE;
                SHR_IF_ERR_EXIT(
                    bcmptm_pt_attr_is_cacheable(u, overlay_info->w_sid,
                    &w_pt_attr_is_cacheable));
                if (w_pt_attr_is_cacheable) {
                    /* space for overlay_words in data_seg */
                    if (overlay_info->mode == BCMPTM_OINFO_MODE_TCAM_DATA &&
                        overlay_info->num_we_field) {
                       *dsw_count += 1 + overlay_info->num_we_field;
                    } else { /* (mode == TCAM_DATA && num_we_field = 0) ||
                                (mode != TCAM_DATA) */
                       *dsw_count += 2;
                           /* to store 1st two oinfo_words - min */
                    }
                } else { /* w_sid is not cacheable, so treat n_sid as
                            non-overlay mem */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "w_sid is not cacheable, so ignoring "
                                    "overlay_mode for n_sid %s\n"),
                         bcmdrd_pt_sid_to_name(u, sid)));

                    /* space for data words in data_seg */
                    *dsw_count = pt_index_count * bcmdrd_pt_entry_wsize(u, sid);
                }
            } else { /* BCMPTM_OINFO_MODE_NONE */
                /* space for data words in data_seg */
                *dsw_count = pt_index_count * bcmdrd_pt_entry_wsize(u, sid);
            }
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
            *dsw_count += 1; /* sentinel for data_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */


            /* space for info_word_seg in vinfo array */
            if (pt_iw_count) {
                *isw_count = pt_iw_count;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                *isw_count += 1; /* sentinel for info_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
            }


            /* space for tcam_corrupt_seg in vinfo array */
            if (bcmdrd_pt_attr_is_cam(u, sid) && tc_seg_en) {
                *tcsw_count = BCMPTM_BITS2WORDS(pt_index_count);
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                *tcsw_count += 1; /* sentinels for corrupt_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* Optimization
                 * For some cams, we have both aggr view and cam_only view
                 * and so we will end up caching both such views - even
                 * though we may use only aggr view. Must not cache cam_only
                 * views for such cases. But then we cannot do xy conversion for
                 * cam_only views. Must make list of
                 * 'unused_cam_only_mems_with_aggr_view' and disable vs, ds, tcs
                 * for such mems.
                 *
                 * SER correction logic hard-codes view that is used by RM as
                 * the view for doing correction.
                 */
            }

            /* space for dfid_seg in vinfo array */
            if (dfid_seg_en) {
                if (dfid_seg_en == 2) { /* 16b dfid */
                    *dfsw_count    = BCMPTM_DBYTES2WORDS(pt_index_count);
                } else { /* 8b dfid */
                    *dfsw_count    = BCMPTM_BYTES2WORDS(pt_index_count);
                }
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                *dfsw_count += 1; /* sentinels for dfid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
            }


            /* space for ltid in ltid_seg */
            if (ltid_seg_en) {
                *lsw_count = pt_index_count;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                *lsw_count += 1; /* sentinels for ltid_seg */
#endif /* PTCACHE_SENTINEL_ME_ENABLE */
            }


            /* Update sinfo word */
            if (pt_size_change) {
                sinfo_p->cw_index = in_sinfo.cw_index;;
            } else {
                sinfo_p->cw_index = in_cwme_index;
            }
        } /* ME type */

        total_vinfo_w_count = *vsw_count + *dsw_count + *dfsw_count +
                              *isw_count + *tcsw_count;

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "pt=%s, sid=%0d, entry_count=%0d, "
                        "entry_size=%0u, "
                        "single_inst_index_count=%0d, "
                        "tbl_inst_count=%0d, "
                        "is_cam=%0d, tc_en=%0d, overlay_mode=%0d, iw_en=%0d, "
                        "ltid_seg_en=%0d, dfid_seg_en=%0d, "
                        "seds_count=%0d, cwme_count=%0d, "
                        "vsw_count=%0d, dsw_count=%0d, lsw_count=%0d, "
                        "dfsw_count=%0d, isw_count=%0d, tcsw_count=%0d, "
                        "in_seds_index=%0d, in_cwme_index=%0d, "
                        "total_vinfo_w_count=%0d, totalMB=%0d\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid, pt_index_count,
             bcmdrd_pt_entry_wsize(u, sid),
             bcmptm_pt_index_count(u, sid), tbl_inst_count,
             bcmdrd_pt_attr_is_cam(u, sid),
             tc_seg_en, overlay_info->mode, pt_iw_count, ltid_seg_en,
             dfid_seg_en, *seds_count, *cwme_count, *vsw_count, *dsw_count,
             *lsw_count, *dfsw_count, *isw_count, *tcsw_count,
             in_seds_index, in_cwme_index,
             total_vinfo_w_count, (total_vinfo_w_count*4)/1000000));
    } else {
        /* assert(!pt_size_change);
         *
         * For example, we can be here for non-cacheable PTs in non-DEFAULT
         * group.
         */

        sal_memset(sinfo_p, 0, sizeof(bcmptm_ptcache_sinfo_t)); /* clear all fields */
        sinfo_p->ptcache_type = PTCACHE_TYPE_NO_CACHE;
        *skipped = TRUE;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "pt=%s, sid=%0d, entry_count=%0d, "
                        "entry_size=%0u, "
                        "single_inst_index_count=%0d, "
                        "tbl_inst_count=%0d, "
                        "is_cam=%0d, tc_en=%0d, overlay_mode=%0d, iw_en=%0d, "
                        "ltid_seg_en=%0d, dfid_seg_en=%0d, "
                        "pt_size_change=%s, SKIPPED\n"),
             bcmdrd_pt_sid_to_name(u, sid), sid, pt_index_count,
             bcmdrd_pt_entry_wsize(u, sid),
             bcmptm_pt_index_count(u, sid), tbl_inst_count,
             bcmdrd_pt_attr_is_cam(u, sid),
             tc_seg_en, overlay_info->mode, pt_iw_count, ltid_seg_en,
             dfid_seg_en, pt_size_change ? "TRUE" : "FALSE"));
    }
exit:
    SHR_FUNC_EXIT();
} /* sinfo_init */

/* Run sinfo_init() for all PTs in a PT group.
 *
 * Returns word_count for (vinfo array, ltid_seg, seds array, cwme array) as
 * needed for this PT group. Will not allocate any mem for these arrays.
 *
 * Also initializes counts for ltid_seg and different vinfo sub-segments in
 * pt_group_info.
 *
 * Assume:
 *  - This function is never called when acc_for_ser = True.
 */
static int
sinfo_init_group(int u, bool warm, bool pt_size_change, size_t all_sid_count,
                 uint32_t pt_group,
                 uint32_t in_seds_index, uint32_t in_cwme_index,
                 /* out */
                 uint32_t *vinfo_word_count,
                 uint32_t *ltid_seg_word_count,
                 uint32_t *group_seds_count,
                 uint32_t *group_cwme_count)
{
    int tmp_rv;
    bool skipped, already_init;
    bcmdrd_sid_t sid;
    const bcmdrd_sid_t *pt_list = NULL;
    uint32_t pt_count = 0, i,
             valid_seg_word_count = 0,
             data_seg_word_count = 0, dfid_seg_word_count = 0,
             info_seg_word_count = 0, tc_seg_word_count = 0,
             vsw_count = 0, dsw_count = 0, dfsw_count = 0, isw_count = 0,
             tcsw_count = 0, lsw_count = 0, seds_count = 0, cwme_count = 0,
             pt_cached_count = 0, pt_skip_count = 0, pt_already_init_count = 0,
             seds_index, cwme_index;
    bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
    int s = 0;
    SHR_FUNC_ENTER(u);

    pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
    *vinfo_word_count = 0;
    *ltid_seg_word_count = 0;
    *group_seds_count = 0;
    *group_cwme_count = 0;

    /* Determine PTs in this group */
    if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
        /* PT sizes are fixed */
        pt_count = (uint32_t)all_sid_count;
    } else { /* non-DEFAULT PT group */
        tmp_rv = bcmptm_pt_group_info_get(u, pt_group, &pt_count, NULL,
                                          &pt_list);
        SHR_IF_ERR_MSG_EXIT(
            ((tmp_rv != SHR_E_NONE) || !pt_count) ? SHR_E_INTERNAL : SHR_E_NONE,
            (BSL_META_U(u, "group_info_get returned rv = %0d, "
                        "pt_count = %0d for group_id = %0d\n"),
             tmp_rv, pt_count, pt_group));
    }

    /* Init sinfo for all PTs in this group, and compute size for
     * vinfo_array, ltid_seg for this group.
     *
     * Also find seds_count, cwme_count usage for this PT group.
     */
    seds_index = in_seds_index;
    cwme_index = in_cwme_index;
    for (i = 0; i < pt_count; i++) {
        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            sid = i;
        } else {
            sid = pt_list[i];
        }
        SHR_IF_ERR_EXIT(
            sinfo_init(u, warm, pt_size_change, sid, pt_group,
                       seds_index, /* offset in SE_data_seg for this sid */
                       cwme_index, /* offset in cwme_seg for this sid */
                       &skipped, &already_init,
                       &seds_count, &cwme_count,
                       &vsw_count, &dsw_count, &dfsw_count,
                       &isw_count, &tcsw_count, &lsw_count));
        if (!skipped && !already_init) {
            pt_cached_count++;

            seds_index += seds_count; /* for next SE type PT */
            *group_seds_count += seds_count;

            cwme_index += cwme_count; /* for next ME type PT */
            *group_cwme_count += cwme_count;

            valid_seg_word_count += vsw_count;
            data_seg_word_count += dsw_count;
            dfid_seg_word_count += dfsw_count;
            info_seg_word_count += isw_count;
            tc_seg_word_count += tcsw_count;

            *ltid_seg_word_count += lsw_count;
            if (pt_group != BCMPTM_PT_GROUP_DEFAULT) {
            }
        } else if (skipped) {
            pt_skip_count++;
        } else if (already_init) {
            pt_already_init_count++;
        }
    } /* foreach sid */
    *vinfo_word_count = valid_seg_word_count + data_seg_word_count +
                        dfid_seg_word_count + info_seg_word_count +
                        tc_seg_word_count;

    /* Store / Verify pt_group_info_p->counts */
    if (warm) {
        SHR_IF_ERR_EXIT(
            *group_seds_count || *group_cwme_count ||
            *vinfo_word_count || *ltid_seg_word_count ||
            pt_skip_count ? SHR_E_INTERNAL : SHR_E_NONE);

        /* Return counts from pt_group_info - to allow ptcache_init to know
         * req_size for vinfo array, ltid_seg (and retrieve new ptr for
         * these) */
        *vinfo_word_count = pt_group_info_p->valid_seg_word_count +
                            pt_group_info_p->data_seg_word_count +
                            pt_group_info_p->dfid_seg_word_count +
                            pt_group_info_p->info_seg_word_count +
                            pt_group_info_p->tc_seg_word_count;
        *ltid_seg_word_count = pt_group_info_p->ltid_seg_word_count;

        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            bcmptm_ptcache_sinfo_t *sinfo_bp_us = sinfo_bp[u][s];
            *group_seds_count =
                (sinfo_bp_us + SINFO_RSVD_INDEX_SEDS_COUNT_WORD)->cw_index;
            *group_cwme_count =
                (sinfo_bp_us + SINFO_RSVD_INDEX_CWME_COUNT_WORD)->cw_index;
            /* Note: cwme_count above will not match with coldboot display
             *       because it includes non-DEFAULT groups also. We are not
             *       keeping track of cwme_count for non-DEFAULT groups. */
        }
    } else { /* coldboot or pt_size_change */
           /* Note: pt_skip_count can be non-zero if non-cacheable mems are
            *       part of non-DEFAULT PT group. */
        if (pt_size_change) {
           SHR_IF_ERR_EXIT(
               *group_seds_count ||
               *group_cwme_count ||
               pt_already_init_count ? SHR_E_INTERNAL : SHR_E_NONE);
        }

        /* Record counts in pt_group_info */
        pt_group_info_p->valid_seg_word_count = valid_seg_word_count;
        pt_group_info_p->data_seg_word_count = data_seg_word_count;
        pt_group_info_p->dfid_seg_word_count = dfid_seg_word_count;
        pt_group_info_p->info_seg_word_count = info_seg_word_count;
        pt_group_info_p->tc_seg_word_count = tc_seg_word_count;
        pt_group_info_p->ltid_seg_word_count = *ltid_seg_word_count;
    } /* coldboot or pt_size_change */

    /* Now: seds_index
     *          has total num (data_words, sentinel_words) for SE mems.
     *
     *      cwme_index
     *          has total num (cwme elements) for ME type mems.
     *
     *      vinfo_word_count has
     *          num (valid, data, dfid, info, tc) words for ME mems.
     *
     *      ltid_seg_word_count has total number of words in ltid_seg.
     */

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "\n\nSummary for sinfo_init: "
                    "WB = %s, pt_size_change = %s, "
                    "group = %0d, MB = %0d, "
                    "cached_pt_count = %0d, "
                    "skipped_pt_count = %0d, "
                    "already_init_pt_count = %0d, "
                    "num_SEwords = %0d, num_CWME = %0d, "
                    "vinfo.num_W = %0d, ltid_seg.num_W = %0d, "
                    "sinfo->num_W = %0u\n\n"),
         warm ? "T" : "F",
         pt_size_change ? "T" : "F",
         pt_group,
         ((*group_seds_count + *group_cwme_count +
           *vinfo_word_count + *ltid_seg_word_count)*4)/1000000,
         pt_cached_count, pt_skip_count,
         pt_already_init_count, *group_seds_count, *group_cwme_count,
         *vinfo_word_count, *ltid_seg_word_count, pt_count));
    BCMPTM_LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "\nSummary: group = %0d, KBytes = %0d, "
                    "(WB = %s, pt_size_change = %s)\n,"
                    "seds = %d, cwme = %d, vsw = %d, dsw = %d, dfsw = %d, "
                    "isw = %d, tcsw = %d, vinfow = %d, ltidw = %d\n"),
         pt_group,
         ((*group_seds_count + *group_cwme_count +
           *vinfo_word_count + *ltid_seg_word_count)*4)/1000,
         warm ? "T" : "F",
         pt_size_change ? "T" : "F",

         *group_seds_count, *group_cwme_count,
         pt_group_info_p->valid_seg_word_count,
         pt_group_info_p->data_seg_word_count,
         pt_group_info_p->dfid_seg_word_count,
         pt_group_info_p->info_seg_word_count,
         pt_group_info_p->tc_seg_word_count,
         *vinfo_word_count, *ltid_seg_word_count));
exit:
    SHR_FUNC_EXIT();
} /* sinfo_init_group */

/*
 * Read sinfo for this sid and accordingly:
 * - For SE type PTs:
 *   - Write sentinel words in seds
 *   - Fills HW default values for entries in seds
 *
 * - For ME type PTs:
 *   - Initialize cwme element.
 *   - Write sentinel words for vs, ds, dfs, is, tcs, ltid_seg
 *   - Writes overlay_info_words in ds
 *   - Fills HW default values for entries in ds
 *   - Fills key, mask info words for TCAM type PTs
 *
 * - For CCI type PTs:
 *   - Fills map_id in sinfo->cw_index
 *
 * - For ME_CCI type PTs:
 *   - Fills map_id in info_seg
 *
 * - For WB
 *   - Fills map_id for CCI type PTs
 *   - Does not modify any of outputs (vinfo_xy_index, etc)
 *   - No other change.
 *
 * Assume:
 *  - This function is never called when acc_for_ser = True.
 */
static int
cwme_vinfo_init(int u, bool warm, bool pt_size_change, bcmdrd_sid_t sid,
                uint32_t pt_group,
                bcmptm_ptcache_pt_group_info_t *pt_group_info_p,
                pt_group_ptrs_t *pt_group_ptrs_p,

                uint32_t *vinfo_vs_index,
                uint32_t *vinfo_ds_index,
                uint32_t *vinfo_dfs_index,
                uint32_t *vinfo_is_index,
                uint32_t *vinfo_tcs_index,
                uint32_t *ltid_seg_index) /* 1st word in lsw section for current sid */
{
    uint32_t pt_entry_count = 0, tbl_inst_count = 0, pt_word_count = 0,
             pt_iw_count = 0, vs_word_count = 0, tcs_word_count = 0,
             cci_map_id = 0, pt_index_count = 0, seds_index;
    uint32_t *vinfo_bp = pt_group_ptrs_p->vinfo_bp;
    bcmptm_ptcache_cwme_t *cwme_p;
    uint8_t dfid_seg_en = 0;
    const bcmptm_overlay_info_t *overlay_info;
    bcmptm_ptcache_xinfo_t *in_xinfo, xinfo;
    bool ltid_seg_en = FALSE, tc_seg_en = FALSE;
    bcmptm_ptcache_sinfo_t *sinfo_p;
    int pt_index_min;
    int s = 0;
    SHR_FUNC_ENTER(u);

    /* get sinfo for this sid */
    sinfo_p = sinfo_bp[u][s] + sid;

    if (warm) {
        /* No need to call bcmptm_cci_ctr_reg()
         * Can add some checks.
         */
        SHR_EXIT();
    } /* warm */

    /* All subsequent code is for coldboot, pt_size_change */

    switch (sinfo_p->ptcache_type) {
        case PTCACHE_TYPE_CCI_ONLY:
            /* Should not be here for pt_size_change */
            SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

            /* Note: Counters are only in default group. */
            SHR_IF_ERR_EXIT(pt_group != BCMPTM_PT_GROUP_DEFAULT ?
                            SHR_E_INTERNAL : SHR_E_NONE);

            SHR_IF_ERR_EXIT(
                bcmptm_cci_ctr_reg(u, sid, &cci_map_id));

            /* Store cci_map_id in Pass_2 */
            sinfo_p->cw_index = cci_map_id;

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, CCI_only"
                            "cci_map_id=%0d\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid, cci_map_id));

            /* No words in seds, cwme, vinfo arrays for such sid */
            break; /* PTCACHE_TYPE_CCI_ONLY */

        case PTCACHE_TYPE_SE_VALID: /* can happen during WB */
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            /* break; unreachable */

        case PTCACHE_TYPE_SE_NOT_VALID:
            /* Should not be here for pt_size_change */
            SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

            SHR_IF_ERR_EXIT(pt_group != BCMPTM_PT_GROUP_DEFAULT ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            pt_word_count = bcmdrd_pt_entry_wsize(u, sid);
            if (!pt_word_count) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                "pt_word_count=%0d\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid, pt_word_count));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            seds_index = sinfo_p->cw_index;
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
            *(seds_bp[u][s] + seds_index - 1) =
                ptcache_sentinel_calc(u, sid, seds_index - 1);
#endif /* PTCACHE_SENTINEL_SE_ENABLE */

            /* Program entries in se section with null_entry. */
            if (bcmdrd_pt_default_value_is_nonzero(u, sid)) {
                SHR_IF_ERR_EXIT(
                    fill_null_entry(u, sid, seds_bp[u][s] + seds_index));
            }
            break;

        case PTCACHE_TYPE_SE_CACHE0:
            /* Can be here even for pt_size_change */
            /* Can be here even for non default PT_GROUPs */
            seds_index = sinfo_p->cw_index;
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
            *(seds_bp[u][s] + seds_index - 1) =
                ptcache_sentinel_calc(u, sid, seds_index - 1);
#endif /* PTCACHE_SENTINEL_SE_ENABLE */
            break;

        case PTCACHE_TYPE_ME_RSVD:
        case PTCACHE_TYPE_ME:
            pt_index_min = bcmptm_pt_index_min(u, sid);
            SHR_IF_ERR_EXIT(pt_index_min < 0 ? SHR_E_INTERNAL : SHR_E_NONE);
            pt_index_count = bcmptm_pt_index_count(u, sid);
            SHR_IF_ERR_EXIT(tbl_inst_count_get(u, sid, &tbl_inst_count));
            pt_entry_count = pt_index_count * tbl_inst_count;
            pt_word_count = pt_entry_count * bcmdrd_pt_entry_wsize(u, sid);
            vs_word_count = BCMPTM_BITS2WORDS(pt_entry_count);

            tcs_word_count = vs_word_count;
            SHR_IF_ERR_EXIT(bcmptm_pt_overlay_info_get(u, sid, &overlay_info));
            SHR_IF_ERR_EXIT(bcmptm_pt_needs_dfid(u, sid, &dfid_seg_en));
            SHR_IF_ERR_EXIT(bcmptm_pt_needs_ltid(u, sid, &ltid_seg_en));
            SHR_IF_ERR_EXIT(bcmptm_tcam_corrupt_bits_enable(u, &tc_seg_en));
            SHR_IF_ERR_EXIT(bcmptm_pt_iw_count(u, sid, &pt_iw_count));
            /* break; */
            /* coverity[fallthrough: FALSE] */
        case PTCACHE_TYPE_ME_CCI:
            if (sinfo_p->ptcache_type == PTCACHE_TYPE_ME_CCI) {
                SHR_IF_ERR_EXIT(pt_size_change ? SHR_E_INTERNAL : SHR_E_NONE);

                pt_index_min = bcmptm_pt_index_min(u, sid);
                SHR_IF_ERR_EXIT(pt_index_min < 0 ? SHR_E_INTERNAL : SHR_E_NONE);
                pt_index_count = bcmptm_pt_index_count(u, sid);
                SHR_IF_ERR_EXIT(tbl_inst_count_get(u, sid, &tbl_inst_count));
                pt_entry_count = pt_index_count * tbl_inst_count;
                pt_word_count = pt_entry_count * bcmdrd_pt_entry_wsize(u, sid);
                vs_word_count = BCMPTM_BITS2WORDS(pt_entry_count);

                tcs_word_count = 0; /* dont_care as tc_seg_en = 0 */
                SHR_IF_ERR_EXIT(
                    bcmptm_pt_overlay_info_get(u, sid, &overlay_info));
                    /* overlay_info->mode will be BCMPTM_OINFO_MODE_NONE */
                dfid_seg_en = FALSE;
                ltid_seg_en = FALSE;
                tc_seg_en = FALSE;
                pt_iw_count = 1; /* to store cci_map_id */
            }

            /* All MEs have an element in cwme array.
             *
             * For every ME type PT, vinfo array will have
             * - valid_seg
             * - data_seg (to store data_words or overlay_info_words)
             *
             * For every ME type PT, vinfo array may optionally have
             * - dfid_seg
             * - info_seg
             * - tc_seg
             *
             * For every ME type PT, vinfo array may optionally have
             *
             * ---ltid_seg (optional)
             *
             * Note: For overlay case:
             *     overlay_info_words are stored in ds starting at ds_base
             */
            cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;

            in_xinfo = &cwme_p->xinfo;
            if (pt_size_change) {
                SHR_IF_ERR_EXIT(
                    (pt_group == BCMPTM_PT_GROUP_DEFAULT) ||
                    (pt_group != in_xinfo->pt_group) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
            } else if (in_xinfo->pt_group != BCMPTM_PT_GROUP_DEFAULT) {
                /* Filter out previously processed PTs.
                 * During coldboot, this should happen only when we are
                 * traversing PTs in DEFAULT group. */
                if ((pt_group != BCMPTM_PT_GROUP_DEFAULT)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                    "was processed in group=%0d, and we now "
                                    "see this PT again in group=%0d\n"),
                         bcmdrd_pt_sid_to_name(u, sid), sid,
                         in_xinfo->pt_group, pt_group));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                } else { /* skipping previously processed SID */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Pass_2: Skipping pt=%s, sid=%0d, "
                                    "was processed in group=%0d\n"),
                         bcmdrd_pt_sid_to_name(u, sid), sid,
                         in_xinfo->pt_group));
                    SHR_EXIT(); /* without display */
                }
            }

            /* Zero out cwme info - to clear xinfo and also all offsets like
             * vs_base, ds_base, etc.
             * This is redundant for coldboot case, but is MUST for case of
             * pt_size_change.
             */
            sal_memset(cwme_p, 0, sizeof(bcmptm_ptcache_cwme_t));

            /* Init pt_group in xinfo. */
            sal_memset(&xinfo, 0, sizeof(bcmptm_ptcache_xinfo_t));
            xinfo.pt_group = pt_group;

            /* Misc checks */
            if (sinfo_p->ptcache_type == PTCACHE_TYPE_ME_RSVD) {
                /* No words in vs, ds, dfs, is, tcs, ltid_seg for this PT.
                 * Offsets to in cwme_p are already 0. */
                cwme_p->xinfo = xinfo;
                SHR_EXIT(); /* done */
            } else if (!pt_word_count || !vs_word_count) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                "pt_word_count=%0d, "
                                "vs_word_count=%0d\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid, pt_word_count,
                     vs_word_count));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Compute rest of xinfo */
            /* xinfo.valid_seg_en = 1; implicit */

            xinfo.data_seg_en = 1; /* default */
            if (overlay_info->mode != BCMPTM_OINFO_MODE_NONE) {
                bool w_pt_attr_is_cacheable = TRUE;
                SHR_IF_ERR_EXIT(
                    bcmptm_pt_attr_is_cacheable(u, overlay_info->w_sid,
                    &w_pt_attr_is_cacheable));
                if (w_pt_attr_is_cacheable) {
                    xinfo.overlay_mode = overlay_info->mode;
                    xinfo.data_seg_en = 0;

                    /* Make sure that we are not violating limits of
                     * xinfo.num_we_fields. */
                    if (overlay_info->num_we_field) {
                        SHR_IF_ERR_EXIT(
                            (overlay_info->num_we_field >= XINFO_NUM_WEF_MAX) ?
                            SHR_E_INTERNAL : SHR_E_NONE);
                       xinfo.num_we_field =
                           (1 + overlay_info->num_we_field);
                    } else { /* num_we_field = 0 */
                       xinfo.num_we_field = 2;
                           /* To store 1st two oinfo_words - min */
                    }
                }
            }
            if (pt_iw_count) {
                xinfo.info_seg_en = 1;
            }
            if (bcmdrd_pt_attr_is_cam(u, sid) && tc_seg_en) {
                xinfo.tc_seg_en = 1;
            }
            if (dfid_seg_en) {
                xinfo.dfid_seg_en = dfid_seg_en;
            }
            if (ltid_seg_en) {
                xinfo.ltid_seg_en = 1;
            }

            /* Store xinfo in cwme */
            cwme_p->xinfo = xinfo;

            /* Store pt_index_count, pt_index_min, tbl_inst_max in cwme */
            cwme_p->pt_index_count = pt_index_count;
            cwme_p->pt_index_min = pt_index_min;
            SHR_IF_ERR_EXIT(tbl_inst_count > 0x8000 ?
                            SHR_E_INTERNAL : SHR_E_NONE);
                /* so tbl_inst_count - 1 fits into int16_t */
            cwme_p->tbl_inst_max = tbl_inst_count - 1;

#if defined(PTCACHE_SENTINEL_ME_ENABLE)
            /* program sentinel for valid_seg */
            *(vinfo_bp + (*vinfo_vs_index)) =
                ptcache_sentinel_calc(u, sid, *vinfo_vs_index);
            *vinfo_vs_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

            /* program offset for valid_seg */
            cwme_p->vs_base = *vinfo_vs_index;

            /* advance vs_index for next sid */
            *vinfo_vs_index += vs_word_count;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                            "vs_word_count=%0d, "
                            "next vinfo_vs_index=%0d\n"),
                 bcmdrd_pt_sid_to_name(u, sid), sid, vs_word_count,
                 *vinfo_vs_index));


            if (xinfo.data_seg_en) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for data_seg */
                *(vinfo_bp + (*vinfo_ds_index)) =
                    ptcache_sentinel_calc(u, sid, *vinfo_ds_index);
                *vinfo_ds_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for data_seg */
                cwme_p->ds_base = *vinfo_ds_index;

                /* program entries in cache with null_entry.
                 * Recall, for WB case we cannot be here - so don't need
                 * to qualify following with !warm */
                if (bcmdrd_pt_default_value_is_nonzero(u, sid)) {
                    SHR_IF_ERR_EXIT(
                        fill_null_entry(u, sid, vinfo_bp + (*vinfo_ds_index)));
                }

                /* advance ds_index for next sid */
                *vinfo_ds_index += pt_word_count;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                "pt_word_count=%0d, "
                                "next vinfo_ds_index=%0d\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid, pt_word_count,
                     *vinfo_ds_index));
            }

            /* For overlay(narrow) sid, overlay words are stored in data_seg.
             * (xinfo.data_seg_en was set to 0 when computing
             *  xinfo for such sid) */
            if (xinfo.overlay_mode != BCMPTM_OINFO_MODE_NONE) {
                uint32_t overlay_num_cw = 0;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for data_seg */
                *(vinfo_bp + (*vinfo_ds_index)) =
                    ptcache_sentinel_calc(u, sid, *vinfo_ds_index);
                *vinfo_ds_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for data_seg */
                cwme_p->ds_base = *vinfo_ds_index;

                /* program overlay_words in data_seg */
                SHR_IF_ERR_EXIT(
                    ptcache_overlay_info_set(u, sid, overlay_info,
                                             vinfo_bp + (*vinfo_ds_index),
                                             &overlay_num_cw));
                *vinfo_ds_index += overlay_num_cw;
            }


            if (xinfo.dfid_seg_en) {
                uint32_t dfid_seg_wsize = 0;
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for dfid_seg */
                *(vinfo_bp + (*vinfo_dfs_index)) =
                    ptcache_sentinel_calc(u, sid, *vinfo_dfs_index);
                *vinfo_dfs_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for dfid_seg */
                cwme_p->dfs_base = *vinfo_dfs_index;

                /* advance dfs_index for next sid */
                if (xinfo.dfid_seg_en == 2) { /* 16b dfid */
                    dfid_seg_wsize = BCMPTM_DBYTES2WORDS(pt_entry_count);
                } else { /* 8b dfid */
                    dfid_seg_wsize = BCMPTM_BYTES2WORDS(pt_entry_count);
                }
                *vinfo_dfs_index += dfid_seg_wsize;
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                "dfid_seg_wsize=%0d, "
                                "next vinfo_dfs_index=%0d\n"),
                     bcmdrd_pt_sid_to_name(u, sid), sid, dfid_seg_wsize,
                     *vinfo_dfs_index));
            }


            if (pt_iw_count) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for info_seg */
                *(vinfo_bp + (*vinfo_is_index)) =
                    ptcache_sentinel_calc(u, sid, *vinfo_is_index);
                *vinfo_is_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for info_seg */
                cwme_p->is_base = *vinfo_is_index;

                /* Now fill correct info */
                if (sinfo_p->ptcache_type == PTCACHE_TYPE_ME_CCI) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_cci_ctr_reg(u, sid, &cci_map_id));

                    /* Store cci_map_id in Pass_2, but this time in info_seg.
                     * Cannot use data_seg because for such SIDs, entry also
                     * contains cfg fields - so entries must be cached and
                     * stored in data_seg). */
                    *(vinfo_bp + (*vinfo_is_index)) = cci_map_id;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Pass_2: pt=%s, sid=%0d, "
                                    "ME_CCI cci_map_id=%0d\n"),
                         bcmdrd_pt_sid_to_name(u, sid), sid, cci_map_id));
                } else { /* info words for Tcams for example */
                    SHR_IF_ERR_EXIT(
                        bcmptm_pt_iw_fill(u, sid,
                                          vinfo_bp + (*vinfo_is_index)));
                } /* info words for Tcams for example */

                /* Advance is_index for next sid */
                *vinfo_is_index += pt_iw_count;
            }


            if (xinfo.tc_seg_en) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for tc_seg */
                *(vinfo_bp + (*vinfo_tcs_index)) =
                    ptcache_sentinel_calc(u, sid, *vinfo_tcs_index);
                *vinfo_tcs_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for tcam_corrupt_seg */
                cwme_p->tcs_base = *vinfo_tcs_index;

                /* offset to valid_seg for next sid */
                *vinfo_tcs_index += tcs_word_count;
            }


            if (xinfo.ltid_seg_en) {
#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* program sentinel for ltid_seg */
                if (ltid_size16b[u]) { /* 16b ltid */
                    uint16_t *ltid_p =
                        (uint16_t *)pt_group_ptrs_p->ltid_seg_base_ptr;
                    *(ltid_p + (*ltid_seg_index)) = BCMPTM_LTID_SENTINEL16;
                } else { /* 32b ltid */
                    uint32_t *ltid_p =
                        (uint32_t *)pt_group_ptrs_p->ltid_seg_base_ptr;
                    *(ltid_p + (*ltid_seg_index)) = BCMPTM_LTID_SENTINEL;
                }
                *ltid_seg_index += 1;
#endif /* PTCACHE_SENTINEL_ME_ENABLE */

                /* program offset for ltid_seg */
                cwme_p->ls_base = *ltid_seg_index;

                /* offset to ltid_seg for next sid */
                *ltid_seg_index += pt_entry_count;
            }

            break; /* PTCACHE_TYPE_ME, PTCACHE_TYPE_ME_CCI */

        default: /* PTCACHE_TYPE_NO_CACHE */
            SHR_IF_ERR_EXIT(sinfo_p->ptcache_type == PTCACHE_TYPE_UNINIT ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            break;
    }
exit:
    SHR_FUNC_EXIT();
} /* cwme_vinfo_init */

/* Run cwme_vinfo_init for all PTs in a group.
 */
static int
cwme_vinfo_init_group(int u, bool warm, bool pt_size_change,
                      size_t all_sid_count, uint32_t pt_group)

{
    int tmp_rv;
    bcmdrd_sid_t sid;
    const bcmdrd_sid_t *pt_list = NULL;
    uint32_t pt_count = 0, i,
             vinfo_vs_index, vinfo_ds_index, vinfo_dfs_index,
             vinfo_is_index, vinfo_tcs_index, ltid_seg_index,
             end_vinfo_vs_index, end_vinfo_ds_index, end_vinfo_dfs_index,
             end_vinfo_is_index, end_vinfo_tcs_index, vinfo_word_count;
    bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = 0;
    SHR_FUNC_ENTER(u);

    pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
    pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + pt_group;

    /* Determine PTs in this group */
    if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
        /* PT sizes are fixed */
        pt_count = (uint32_t)all_sid_count;
    } else {
        tmp_rv = bcmptm_pt_group_info_get(u, pt_group, &pt_count, NULL,
                                          &pt_list);
        SHR_IF_ERR_MSG_EXIT(
            ((tmp_rv != SHR_E_NONE) || !pt_count) ? SHR_E_INTERNAL : SHR_E_NONE,
            (BSL_META_U(u, "group_info_get returned rv = %0d, "
                        "pt_count = %0d for group_id = %0d\n"),
             tmp_rv, pt_count, pt_group));
    }

    /* 1st word in valid, data, info, tc, dfid segments.
     * Following order is arbitrary and can be changed, if needed. */
    vinfo_vs_index = 0;
    vinfo_ds_index = vinfo_vs_index + pt_group_info_p->valid_seg_word_count;
    vinfo_dfs_index = vinfo_ds_index + pt_group_info_p->data_seg_word_count;
    vinfo_is_index = vinfo_dfs_index + pt_group_info_p->dfid_seg_word_count;
    vinfo_tcs_index = vinfo_is_index + pt_group_info_p->info_seg_word_count;
    ltid_seg_index = 0;

    /* (last+1) word in valid, data, info, tc, dfid segments */
    end_vinfo_vs_index = vinfo_ds_index;
    end_vinfo_ds_index = vinfo_dfs_index;
    end_vinfo_dfs_index = vinfo_is_index;
    end_vinfo_is_index = vinfo_tcs_index;
    vinfo_word_count = pt_group_info_p->valid_seg_word_count +
                       pt_group_info_p->data_seg_word_count +
                       pt_group_info_p->dfid_seg_word_count +
                       pt_group_info_p->info_seg_word_count +
                       pt_group_info_p->tc_seg_word_count;
    end_vinfo_tcs_index = vinfo_word_count;

    /* Init cwme words, vinfo_array for each PT in group */
    for (i = 0; i < pt_count; i++) {
        if (pt_group != BCMPTM_PT_GROUP_DEFAULT) {
            sid = pt_list[i];
        } else {
            sid = i;
        }
        SHR_IF_ERR_EXIT(
            cwme_vinfo_init(u, warm, pt_size_change, sid,
                            pt_group, pt_group_info_p, pt_group_ptrs_p,
                            &vinfo_vs_index, &vinfo_ds_index, &vinfo_dfs_index,
                            &vinfo_is_index, &vinfo_tcs_index, &ltid_seg_index));
    } /* foreach sid in group */

    /* End checks for group.vinfo_array, group.ltid_seg */
    if (!warm &&
        ((vinfo_vs_index != end_vinfo_vs_index)
         || (vinfo_ds_index != end_vinfo_ds_index)
         || (vinfo_dfs_index != end_vinfo_dfs_index)
         || (vinfo_is_index != end_vinfo_is_index)
         || (vinfo_tcs_index != end_vinfo_tcs_index)
         || (ltid_seg_index != pt_group_info_p->ltid_seg_word_count))) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "cwme_vinfo_init: End check failed for group "
                        "%0d: exp vinfo_vs_index = %0d, obs = %0d, "
                        "exp vinfo_ds_index = %0d, obs = %0d, "
                        "exp vinfo_dfs_index = %0d, obs = %0d, "
                        "exp vinfo_is_index = %0d, obs = %0d, "
                        "exp vinfo_tcs_index = %0d, obs = %0d, "
                        "exp ltid_seg_index = %0d, obs = %0d\n"),
             pt_group,
             end_vinfo_vs_index, vinfo_vs_index,
             end_vinfo_ds_index, vinfo_ds_index,
             end_vinfo_dfs_index, vinfo_dfs_index,
             end_vinfo_is_index, vinfo_is_index,
             end_vinfo_tcs_index, vinfo_tcs_index,
             pt_group_info_p->ltid_seg_word_count, ltid_seg_index));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "cwme_vinfo: completed without errors for "
                        "group %0d\n"), pt_group));
    }
exit:
    SHR_FUNC_EXIT();
} /* cwme_vinfo_init_group */

static int
sinfo_warm_check(int u, uint32_t ref_sinfo_sign,
                 uint32_t ptcache_flags_word, size_t sid_count)
{
    uint32_t tmp_w, tmp_flags_word;
    bool old_ltid_size16b, new_ltid_size16b;
    int s = 0; /* Assume: This fn is never called when acc_for_ser = True. */
    bcmptm_ptcache_sinfo_t *sinfo_bp_us = sinfo_bp[u][s];
    SHR_FUNC_ENTER(u);

    /* Check HA signature for SINFO array */
    tmp_w = (sinfo_bp_us + SINFO_RSVD_INDEX_SIGN_WORD)->cw_index;
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != ref_sinfo_sign ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(u, "WB: PTcache.SINFO array signature mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         ref_sinfo_sign, tmp_w));

    /* Check flags_word in SINFO array */
    tmp_w = (sinfo_bp_us + SINFO_RSVD_INDEX_FLAGS_WORD)->cw_index;
    old_ltid_size16b = tmp_w & PTCACHE_FLAGS_LTID_SIZE16;
    new_ltid_size16b = ptcache_flags_word & PTCACHE_FLAGS_LTID_SIZE16;
    if (old_ltid_size16b != new_ltid_size16b) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "LTID storage size change: old_ltid_size16b=%0d, "
                        "new_ltid_size16b=%0d\n"),
             old_ltid_size16b, new_ltid_size16b));
    }
    tmp_w &= ~PTCACHE_FLAGS_LTID_SIZE16;
    tmp_flags_word = ptcache_flags_word & ~PTCACHE_FLAGS_LTID_SIZE16;
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != tmp_flags_word ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(u, "WB: PTcache.SINFO array flags_word mismatch, "
                    "exp=0x%8x, obs=0x%8x\n"),
         tmp_flags_word, tmp_w));

    /* Check sid_count in SINFO array */
    tmp_w = (sinfo_bp_us + SINFO_RSVD_INDEX_SID_COUNT_WORD)->cw_index;
    SHR_IF_ERR_MSG_EXIT(
        tmp_w != sid_count ? SHR_E_FAIL : SHR_E_NONE,
        (BSL_META_U(u, "WB: PTcache.SINFO array sid_count mismatch, "
                    "exp=%0d, obs=%0d\n"),
         (uint32_t)sid_count, tmp_w));
exit:
    SHR_FUNC_EXIT();
} /* sinfo_warm_check */

/*!
 * \brief Change HA mem block for vinfo array or ltid_seg.
 * Change may imply: alloc / realloc / free.
 * For resize case, (re)allocated mem will be cleared, sign word added.
 */
static int
ptcache_ha_mem_change(int u, uint32_t ref_sign_word, uint8_t ha_submod_id,
                           uint32_t old_word_count, /* excluding sign_word */
                           uint32_t new_word_count, /* excluding sign_word */
                           void *old_base_ptr,
                           const char *submod_name,
                           bool ltid_seg_with_16b_ltid,

                           /* out */
                           void **new_base_ptr)
{
    bool warm = FALSE;
    uint32_t new_req_size, *old_alloc_ptr, *new_alloc_ptr;
    SHR_FUNC_ENTER(u);

    /* Re-allocate vinfo array / ltid_seg for this group */
    if (new_word_count) {
        if (old_word_count) { /* use realloc */
            if (ltid_seg_with_16b_ltid) {
                new_req_size = new_word_count * sizeof(uint16_t) +
                               sizeof(uint32_t); /* sign_word */
            } else { /* 32b ltid */
                new_req_size = (1 + new_word_count) * sizeof(uint32_t);
            }
            old_alloc_ptr = ((uint32_t *)old_base_ptr) - 1;
                                    /* include sign_word */
            new_alloc_ptr = shr_ha_mem_realloc(u, old_alloc_ptr, new_req_size);
            SHR_IF_ERR_MSG_EXIT(
                !new_alloc_ptr ? SHR_E_MEMORY : SHR_E_NONE,
                (BSL_META_U(u, "shr_ha_mem_realloc returned "
                            "NULL for %s realloc, "
                            "old_word_count = %0d, "
                            "new_word_count = %0d, "
                            "new_req_size = %0d\n"),
                 submod_name, old_word_count, new_word_count,
                 new_req_size));
            sal_memset(new_alloc_ptr, 0, new_req_size);

            /* Write signature again.
             *
             * As per current shr_ha_mem_realloc implementation, allocated mem:
             * will be same block, if (new_word_count <= old_word_count)
             *
             * May/not be completely new block if
             *     (new_word_count > old_word_count).
             */
            *new_alloc_ptr = ref_sign_word;
        } else { /* fresh alloc */
            if (ltid_seg_with_16b_ltid) {
                new_req_size = new_word_count * sizeof(uint16_t);
            } else {
                new_req_size = new_word_count * sizeof(uint32_t);
            }

            /* do_ha_alloc below will inc req_size for sign_word, clear mem and
             * write sign_word */
            SHR_IF_ERR_EXIT(
                bcmptm_do_ha_alloc(u, warm, new_req_size,
                                   submod_name, ha_submod_id,
                                   ref_sign_word, &new_alloc_ptr));
        }
        *new_base_ptr = (new_alloc_ptr + 1); /* skip sign_word */
    } else { /* new_word_count is 0 */
        if (old_word_count) { /* free up previously alloc mem */
            old_alloc_ptr = ((uint32_t *)old_base_ptr) - 1;
                                    /* include sign_word */
            SHR_IF_ERR_EXIT(shr_ha_mem_free(u, old_alloc_ptr));
        }
        *new_base_ptr = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function that will be called when user changes UFT banks usage.
 */
static void
ptcache_pt_group_changed(int u, const char *event, uint64_t ev_data)
{
    bcmptm_pt_banks_info_t new_banks_info;
    uint32_t pt_count, group_count = 0, pt_group;
    bool group_bank_start_en = FALSE;
    bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
    pt_group_ptrs_t *pt_group_ptrs_p;
    const bcmdrd_sid_t *pt_list;
    int tmp_rv;
    int s = 0; /*  Assume: This fn is never called when acc_for_ser = True. */
    char alloc_str_id[16];
    SHR_FUNC_ENTER(u);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "ptcache_pt_group_changed is called for event %s "
                    "and ev_data=%" PRIx64 "\n"),
         event, ev_data));

    /* Find number of PT groups for this device.
     * PT group_count will be 1 for devices where size of PTs are constant.
     * This is not an error. */
    tmp_rv = bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                      &group_count, NULL, &pt_list);
    SHR_IF_ERR_MSG_EXIT(
        tmp_rv,
        (BSL_META_U(u, "pt_group_info_get returned rv = %0d, "
                    "group_count = %0d\n"),
         tmp_rv, group_count));

    for (pt_group = 0; pt_group < group_count; pt_group++) {
        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            continue; /* DEFAULT group has PTs with fixed entries */
        }

        /* Find the widest ptsid for this PT group */
        group_bank_start_en = FALSE;
        tmp_rv = bcmptm_pt_group_info_get(u, pt_group, &pt_count,
                                          &group_bank_start_en, &pt_list);
        SHR_IF_ERR_MSG_EXIT(
            ((tmp_rv != SHR_E_NONE) || !pt_count) ? SHR_E_INTERNAL : SHR_E_NONE,
            (BSL_META_U(u, "group_info_get returned rv = %0d, "
                        "pt_count = %0d for group_id = %0d\n"),
             tmp_rv, pt_count, pt_group));

        /* Find total number of banks, start_bank for this PT group.
         * It is ok for num_banks to become 0 for a PT group. */
        sal_memset(&new_banks_info, 0, sizeof(bcmptm_pt_banks_info_t));
        tmp_rv = bcmptm_cth_uft_bank_info_get_from_ptcache(u, pt_list[0],
                                                           &new_banks_info);
        SHR_IF_ERR_MSG_EXIT(
            tmp_rv,
            (BSL_META_U(u, "uft_bank_info_get returned rv = %0d, "
                        "and new_bank_count = %0d, new_start_bank = %0d for "
                        "group = %0d\n"),
             tmp_rv, new_banks_info.bank_cnt, new_banks_info.bank[0].bank_id,
             pt_group));

        /* Find if num_banks and/or start_bank changed for this group.
         * Must initiate dealloc, realloc of vinfo, ltid_seg ONLY if bank
         * count and/or start_bank changed for this group. */
        pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
        pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + pt_group;
        if ((new_banks_info.bank_cnt != pt_group_info_p->num_banks) ||
            (group_bank_start_en &&
             (new_banks_info.bank[0].bank_id != pt_group_info_p->start_bank))) {
            bool warm, pt_size_change;
            uint32_t all_sid_count, seds_index, cwme_index,
                     vinfo_word_count, ltid_seg_word_count,
                     group_seds_count, group_cwme_count,
                     old_vinfo_word_count, old_ltid_seg_word_count,
                     ref_sign_word;
            void *new_base_ptr;
            uint8_t ha_submod_id;
            bcmptm_index_count_set_info_t index_count_set_info;

            /* presets */
            warm = FALSE;
            pt_size_change = TRUE;
            all_sid_count = (sinfo_bp[u][s] +
                             SINFO_RSVD_INDEX_SID_COUNT_WORD)->cw_index;
            seds_index = 0;
                /* all PTs in !DEF group are of type ME, we should not see
                 * !zero group_seds_count */
            cwme_index = 0;
                /* all PTs in !DEF group already have cwme word associated and
                 * must not use in_cwme_index when pt_size_change is TRUE. */

            /* Update num_banks, start_bank in pt_group_info */
            pt_group_info_p->num_banks = new_banks_info.bank_cnt;
            if (group_bank_start_en) {
                pt_group_info_p->start_bank = new_banks_info.bank[0].bank_id;
            } else {
                pt_group_info_p->start_bank = PTCACHE_GROUP_START_BANK_RSVD;
            }

            /* Update index_count for all PTs in this group */
            index_count_set_info.init = FALSE;
            index_count_set_info.for_non_ser = TRUE;
            index_count_set_info.for_ser = FALSE;
#ifdef ALSO_INIT_SER_PTCACHE_P
            index_count_set_info.for_ser = TRUE;
#endif
            SHR_IF_ERR_MSG_EXIT(
                bcmptm_pt_group_index_count_set(u, pt_group,
                                                &index_count_set_info,
                                                &new_banks_info.bank_cnt,
                                                &new_banks_info.bank[0].bank_id),
                (BSL_META_U(u, "group_index_count_set failed "
                            "for group_id = %0d, num_banks = %0d, "
                            "start_bank = %0d\n"),
                 pt_group, new_banks_info.bank_cnt, new_banks_info.bank[0].bank_id));


            /* Record old_vinfo_word_count, ltid_seg_word_count */
            old_vinfo_word_count = pt_group_info_p->valid_seg_word_count +
                                   pt_group_info_p->data_seg_word_count +
                                   pt_group_info_p->dfid_seg_word_count +
                                   pt_group_info_p->info_seg_word_count +
                                   pt_group_info_p->tc_seg_word_count;
            old_ltid_seg_word_count = pt_group_info_p->ltid_seg_word_count;


            /* Run sinfo_init for all PTs in this PT group. This will:
             * - make changes to sinfo word as needed (but will not change
             *   cw_index (offset into cwme array)
             * - over-write pt_group_info with new counts.
             */
            SHR_IF_ERR_EXIT(
                sinfo_init_group(u, warm, pt_size_change, all_sid_count,
                                 pt_group,
                                 seds_index, cwme_index,
                                 /* out */
                                 &vinfo_word_count, &ltid_seg_word_count,
                                 &group_seds_count, &group_cwme_count));
            SHR_IF_ERR_EXIT(group_seds_count || group_cwme_count ?
                            SHR_E_INTERNAL : SHR_E_NONE);


            /* Change (alloc/realloc/free) vinfo array */
            ha_submod_id = BCMPTM_HA_SUBID_PTCACHE_VINFO + pt_group;
            ref_sign_word = BCMPTM_HA_SIGN_PTCACHE_VINFO + all_sid_count +
                            pt_group;
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheVinf", pt_group);
            SHR_IF_ERR_EXIT(
                ptcache_ha_mem_change(u, ref_sign_word, ha_submod_id,
                                      old_vinfo_word_count,
                                      vinfo_word_count, /* new */
                                      pt_group_ptrs_p->vinfo_bp, /* old */
                                      alloc_str_id,
                                      FALSE,
                                      &new_base_ptr));
            pt_group_ptrs_p->vinfo_bp = (uint32_t *)new_base_ptr;


            /* Change (alloc/realloc/free) ltid_seg */
            ha_submod_id = BCMPTM_HA_SUBID_PTCACHE_LTID_SEG + pt_group;
            ref_sign_word = BCMPTM_HA_SIGN_PTCACHE_LTID_SEG + all_sid_count +
                            pt_group;
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheLtid", pt_group);
            SHR_IF_ERR_EXIT(
                ptcache_ha_mem_change(u, ref_sign_word, ha_submod_id,
                                      old_ltid_seg_word_count,
                                      ltid_seg_word_count, /* new */
                                      pt_group_ptrs_p->ltid_seg_base_ptr, /* old */
                                      alloc_str_id,
                                      ltid_size16b[u],
                                      &new_base_ptr));
            pt_group_ptrs_p->ltid_seg_base_ptr = new_base_ptr;


            /* Re-init vinfo array, ltid_seg
             * Note: Following will first clean up cwme offsets and then
             * re-write them. */
            SHR_IF_ERR_EXIT(
                cwme_vinfo_init_group(u, warm, pt_size_change, all_sid_count,
                                      pt_group));
        } /* num_banks, start_bank has changed for a group */
    } /* foreach PT group */

exit:
    /* SHR_FUNC_EXIT(); Return type for this function is void */

    /* Without following display, we will see log msg when function
     * enters but not when it exits. */
    LOG_DEBUG(BSL_LOG_MODULE,
        (BSL_META_U(_func_unit, "exit\n")));

    /* Otherwise, _func_rv will be unused */
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "ptcache_pt_group_changed exited with rv = %0d\n"),
             _func_rv));
    }
    /* return _func_rv; */
} /* ptcache_pt_group_changed */

static int
ptcache_ltid_swap(int u, bool new_ltid_size16b, uint32_t old_ltid,
                  uint32_t *new_ltid)
{
    uint32_t tmp_ltid;
    int tmp_rv;
    SHR_FUNC_ENTER(u);
    tmp_rv = bcmissu_ltid_to_current(u, old_ltid, &tmp_ltid);
    if (tmp_rv == SHR_E_UNAVAIL) {
        /* Means: LT corresponding to old_ltid no longer exists in new
         * SW version. Application guarantees that such LTs were empty. */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "LT corresponding to old_ltid = %0d does not exist "
                        " in new SDK version.\n"),
             old_ltid));
        if (new_ltid_size16b) {
            *new_ltid = BCMPTM_LTID_IREQ16;
        } else {
            *new_ltid = BCMPTM_LTID_IREQ;
        }
    } else if (SHR_FAILURE(tmp_rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "bcmissu_ltid_to_current returned rv = %0d, "
                        "for old_ltid = %0d\n"),
             tmp_rv, old_ltid));
        SHR_ERR_EXIT(tmp_rv);
    } else {
        if (new_ltid_size16b) {
            SHR_IF_ERR_EXIT(tmp_ltid >= BCMPTM_LTID_IREQ16 ?
                            SHR_E_INTERNAL : SHR_E_NONE);
        } else {
            SHR_IF_ERR_EXIT(tmp_ltid >= BCMPTM_LTID_IREQ ?
                            SHR_E_INTERNAL : SHR_E_NONE);
        }
        *new_ltid = tmp_ltid;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Alloc tmp block of mem, copy old ltids to tmp block.
 * Notes:
 *      - Crash during copying of ltids (during ISSU) is not
 *        supported, so tmp block need not be in HA mem.
 */
static int
ptcache_ltid_copy_remap(int u, bool old_ltid_size16b,
                        uint32_t ref_ltid_seg_sign,
                        uint32_t ltid_seg_word_count, void *ltid_seg_base_ptr,
                        uint32_t *alloc_ptr)
{
    uint32_t tmp_size, w, new_req_size, old_ltid, new_ltid;
    void *tmp0_alloc_ptr = NULL;
    SHR_FUNC_ENTER(u);
    /*
     * req_size contains size of old ltid_seg without sign_word.
     * ltid_seg_base_ptr points to 1st old ltid.
     * alloc_ptr points to sign_word for old ltid_seg.
     */

    if (old_ltid_size16b) { /* new ltid_size is 32b */
        uint16_t *tmp_alloc_ptr; /* will point to copy */
        uint32_t *new_alloc_ptr; /* new ltid seg */
        tmp_size = ltid_seg_word_count * sizeof(uint16_t);
        tmp_alloc_ptr = sal_alloc(tmp_size, "bcmptmLtidChangeBuffer");
        SHR_IF_ERR_MSG_EXIT(
            !tmp_alloc_ptr ? SHR_E_MEMORY : SHR_E_NONE,
            (BSL_META_U(u, "sal_alloc returned "
                        "NULL for ltid_seg copy during issu, "
                        "req_size = %0d\n"),
             tmp_size));
        sal_memcpy(tmp_alloc_ptr, (uint16_t *)ltid_seg_base_ptr,
                   tmp_size);
        tmp0_alloc_ptr = tmp_alloc_ptr;

        /* Realloc ltid_seg_base_ptr */
        new_req_size = (1 + ltid_seg_word_count) * sizeof(uint32_t);
        new_alloc_ptr = shr_ha_mem_realloc(u, alloc_ptr, /* old */
                                           new_req_size);
        SHR_IF_ERR_MSG_EXIT(
            !new_alloc_ptr ? SHR_E_MEMORY : SHR_E_NONE,
            (BSL_META_U(u, "shr_ha_mem_realloc returned "
                        "NULL for ltid_seg realloc during issu, "
                        "word_count = %0d, "
                        "new_req_size = %0d\n"),
             ltid_seg_word_count, new_req_size));
        sal_memset(new_alloc_ptr, 0, new_req_size);
        *new_alloc_ptr = ref_ltid_seg_sign;
        new_alloc_ptr++; /* points to 1st new ltid */

        /* copy from tmp, remap, write to new */
        for (w = 0; w < ltid_seg_word_count; w++) {
            old_ltid = *tmp_alloc_ptr;
            if (old_ltid == BCMPTM_LTID_SENTINEL16) {
                new_ltid = BCMPTM_LTID_SENTINEL;
            } else if (old_ltid == BCMPTM_LTID_IREQ16) {
                new_ltid = BCMPTM_LTID_IREQ;
            } else {
                SHR_IF_ERR_EXIT(
                    ptcache_ltid_swap(u, !old_ltid_size16b, old_ltid,
                                      &new_ltid));
            }
            *new_alloc_ptr = new_ltid;
            tmp_alloc_ptr++;
            new_alloc_ptr++;
        }
    } else { /* new ltid_size is 16b */
        uint32_t *tmp_alloc_ptr, *new0_alloc_ptr;
        uint16_t *new_alloc_ptr; /* new ltid seg */
        tmp_size = ltid_seg_word_count * sizeof(uint32_t);
        tmp_alloc_ptr = sal_alloc(tmp_size, "bcmptmLtidChangeBuffer");
        SHR_IF_ERR_MSG_EXIT(
            !tmp_alloc_ptr ? SHR_E_MEMORY : SHR_E_NONE,
            (BSL_META_U(u, "sal_alloc returned "
                        "NULL for ltid_seg copy during issu, "
                        "req_size = %0d\n"),
             tmp_size));
        sal_memcpy(tmp_alloc_ptr, (uint32_t *)ltid_seg_base_ptr,
                   tmp_size);
        tmp0_alloc_ptr = tmp_alloc_ptr;

        /* Realloc ltid_seg_base_ptr */
        new_req_size = ltid_seg_word_count * sizeof(uint16_t) +
                       sizeof(uint32_t);
        new0_alloc_ptr = shr_ha_mem_realloc(u, alloc_ptr, /* old */
                                            new_req_size);
        SHR_IF_ERR_MSG_EXIT(
            !new0_alloc_ptr ? SHR_E_MEMORY : SHR_E_NONE,
            (BSL_META_U(u, "shr_ha_mem_realloc returned "
                        "NULL for ltid_seg realloc during issu, "
                        "word_count = %0d, "
                        "new_req_size = %0d\n"),
             ltid_seg_word_count, new_req_size));
        sal_memset(new0_alloc_ptr, 0, new_req_size);
        *new0_alloc_ptr = ref_ltid_seg_sign;
        new_alloc_ptr = (uint16_t *)(new0_alloc_ptr + 1);

        /* copy from tmp, remap, write to new */
        for (w = 0; w < ltid_seg_word_count; w++) {
            old_ltid = *tmp_alloc_ptr;
            if (old_ltid == BCMPTM_LTID_SENTINEL) {
                new_ltid = BCMPTM_LTID_SENTINEL16;
            } else if (old_ltid == BCMPTM_LTID_IREQ) {
                new_ltid = BCMPTM_LTID_IREQ16;
            } else {
                SHR_IF_ERR_EXIT(
                    ptcache_ltid_swap(u, !old_ltid_size16b, old_ltid,
                                      &new_ltid));
            }
            *new_alloc_ptr = new_ltid;
            tmp_alloc_ptr++;
            new_alloc_ptr++;
        }
    } /* new ltid_size is 16b */
exit:

    /* Release tmp mem */
    if (tmp0_alloc_ptr) {
        sal_free(tmp0_alloc_ptr);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_ptcache_init(int u, bool warm)
{
    size_t sid_count = 0;
    uint32_t req_size, alloc_size, *alloc_ptr;
    uint32_t seds_index, cwme_index, group_seds_count, group_cwme_count,
             vinfo_word_count, ltid_seg_word_count;
    bool pt_size_change = FALSE;
    uint32_t ptcache_flags_word = 0;
    uint32_t ref_sinfo_sign, ref_cwme_seg_sign, ref_pt_group_info_sign,
             ref_seds_sign, ref_vinfo_sign, ref_ltid_seg_sign;
    bcmptm_ptcache_sinfo_t *sinfo_alloc_p = NULL, *sinfo_bp_us = NULL;
    uint32_t pt_group_count = 0, pt_group = 0;
    const bcmdrd_sid_t *pt_list;
    int s = 0; /*  Assume: This fn is never called when acc_for_ser = True. */
    char alloc_str_id[16];
    SHR_FUNC_ENTER(u);

    /* Basic info */
    SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &sid_count));
    SHR_IF_ERR_EXIT(ptcache_ltid_size_set(u));
    if (ltid_size16b[u]) {
        ptcache_flags_word |= PTCACHE_FLAGS_LTID_SIZE16;
    }
    ref_sinfo_sign = BCMPTM_HA_SIGN_PTCACHE_SINFO + sid_count;
    ref_cwme_seg_sign = BCMPTM_HA_SIGN_PTCACHE_CWME_SEG + sid_count;
    ref_pt_group_info_sign = BCMPTM_HA_SIGN_PTCACHE_PT_GROUP_INFO + sid_count;
    ref_seds_sign = BCMPTM_HA_SIGN_PTCACHE_SEDS + sid_count;

    /* Determine num of pt_groups supported for this u */
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                 &pt_group_count, NULL, &pt_list),
        (BSL_META_U(u, "pt_group_info_get failed (group_count = %0d)\n"),
         pt_group_count));
    SHR_IF_ERR_MSG_EXIT(
        (pt_group_count > BCMPTM_PT_GROUP_COUNT_MAX) ? SHR_E_MEMORY : SHR_E_NONE,
        (BSL_META_U(u, "pt_group_count = %0d cannot be supported\n"),
         pt_group_count));

    /* Alloc space for SID info (sinfo) array.
     * - sinfo array stores info for each PT irrespective of whether PT
     *   requires cache or not - refer to bcmptm_ptcache_sinfo_t for details.
     * - This info contains offset/index into vinfo_array and
     *   also hints about how to interpret index, tbl_inst
     */
    req_size = SINFO_NUM_RSVD_WORDS * sizeof(bcmptm_ptcache_sinfo_t);
        /* Note usage of bcmptm_ptcache_sinfo_t in sizeof above.
         * Reserve first N sinfo elements to store sign, sid_count, hash_word,
         * flags_word, etc. */
    req_size += sid_count * sizeof(bcmptm_ptcache_sinfo_t);
    alloc_size = req_size;
    sinfo_alloc_p = shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                     BCMPTM_HA_SUBID_PTCACHE_SINFO,
                                     "ptmCacheSinfo",
                                     &alloc_size);
    SHR_NULL_CHECK(sinfo_alloc_p, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ?  SHR_E_MEMORY : SHR_E_NONE);
    sinfo_bp[u][s] = sinfo_alloc_p + SINFO_NUM_RSVD_WORDS;
    sinfo_bp_us = sinfo_bp[u][s];
#ifdef ALSO_INIT_SER_PTCACHE_P
    sinfo_bp[u][SER] = sinfo_bp[u][s];
#endif
        /* skip rsvd sinfo*/
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "SINFO_ARRAY: Number of sid = %0u, group_count = %0d "
                    "requested bytes = %0u, allocated bytes = %0u, \n"),
         (uint32_t)sid_count, pt_group_count, req_size, alloc_size));

    if (warm) {
        SHR_IF_ERR_EXIT(
            sinfo_warm_check(u, ref_sinfo_sign,
                             ptcache_flags_word, sid_count));
    } else {
        sal_memset(sinfo_alloc_p, 0, alloc_size);
            /* For all sid set:
             * - ptcache_type = UNINIT.
             *
             * Also zero out reserved words - these will get initialized at end.
             * Also zero out info for all PT groups.
             */
        SHR_IF_ERR_EXIT(
            bcmissu_struct_info_report(u, BCMMGMT_PTM_COMP_ID,
                                             BCMPTM_HA_SUBID_PTCACHE_SINFO,
                                             0, /* offset */
            /* Reserved words (including signature) are also stored in sinfo_t
             * elements. Hence offset is 0 */
                                             sizeof(bcmptm_ptcache_sinfo_t),
                                             SINFO_NUM_RSVD_WORDS +
                                             sid_count,
                                             BCMPTM_PTCACHE_SINFO_T_ID));
    }

    /* Alloc space for pt_group_info array. */
    req_size = pt_group_count * sizeof(bcmptm_ptcache_pt_group_info_t);
    SHR_IF_ERR_EXIT(
        bcmptm_do_ha_alloc(u, warm, req_size,
                           "ptmCachePtGroup",
                           BCMPTM_HA_SUBID_PTCACHE_PT_GROUP_INFO,
                           ref_pt_group_info_sign, &alloc_ptr));
    pt_group_info_bp[u][s] =
        (bcmptm_ptcache_pt_group_info_t *)(alloc_ptr + 1);
#ifdef ALSO_INIT_SER_PTCACHE_P
    pt_group_info_bp[u][SER] = pt_group_info_bp[u][s];
#endif
    if (!warm) {
        SHR_IF_ERR_EXIT(
            bcmissu_struct_info_report(u, BCMMGMT_PTM_COMP_ID,
                                             BCMPTM_HA_SUBID_PTCACHE_PT_GROUP_INFO,
                                             4, /* offset */
                                             /* 1st 4 bytes are sign_word */
                                             sizeof(bcmptm_ptcache_pt_group_info_t),
                                             pt_group_count,
                                             BCMPTM_PTCACHE_PT_GROUP_INFO_T_ID));
    }

    /* Alloc space in heap to store vinfo_bp,
     * ltid_seg_ptr for all pt_groups. */
    req_size = pt_group_count * sizeof(pt_group_ptrs_t);
    alloc_ptr = NULL;
    alloc_ptr = sal_alloc(req_size, "bcmptmPtcachePtGroupPtrs");
    SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
    sal_memset(alloc_ptr, 0, req_size);
    pt_group_ptrs_bp[u][s] = (pt_group_ptrs_t *)alloc_ptr;
#ifdef ALSO_INIT_SER_PTCACHE_P
    pt_group_ptrs_bp[u][SER] = pt_group_ptrs_bp[u][s];
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_cci_comp_config(u, warm, BCMPTM_SUB_PHASE_1));

    /* Iterate through each group starting from last PT group down to
     * DEFAULT PT group. This allows us to work without knowing list of PTs
     * in DEFAULT group. */
    seds_index = 0;
    cwme_index = 0;
    pt_group = pt_group_count - 1; /* Start with last PT group */
    do {
        uint8_t bank_count, start_bank;
        bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
        pt_group_ptrs_t *pt_group_ptrs_p;
        pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
        pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + pt_group;

        if (pt_group != BCMPTM_PT_GROUP_DEFAULT) { /* non-DEFAULT PT group */
            /* Determine group_bank_start_en */
            bool group_bank_start_en = FALSE;
            uint32_t tmp_pt_count; /* dont_care */
            const bcmdrd_sid_t *tmp_pt_list; /* dont_care */
            bcmptm_index_count_set_info_t index_count_set_info;
            SHR_IF_ERR_MSG_EXIT(
                bcmptm_pt_group_info_get(u, pt_group, &tmp_pt_count,
                                         &group_bank_start_en, &tmp_pt_list),
                (BSL_META_U(u, "CB: group_index_info_get failed "
                            "for group_id = %0d\n"),
                 pt_group));

            if (warm) {
                /* Re-init size of all PTs for every PT group as per bank_count,
                 * start_bank stored in HA mem. */

                SHR_IF_ERR_EXIT(pt_group_info_p->num_banks > 255 ?
                                SHR_E_INTERNAL : SHR_E_NONE);
                bank_count = pt_group_info_p->num_banks;

                if (group_bank_start_en) {
                    SHR_IF_ERR_EXIT(
                        pt_group_info_p->start_bank > 255 ? SHR_E_INTERNAL
                                                          : SHR_E_NONE);
                    start_bank = pt_group_info_p->start_bank;
                } else {
                    /* Means bcmptm_pt_group_index_count_set() will ignore
                     * start_bank input param. */
                    start_bank = 0;
                }

                index_count_set_info.init = FALSE; /* init_bank_count */
                index_count_set_info.for_non_ser = TRUE;
                index_count_set_info.for_ser = FALSE;
#ifdef ALSO_INIT_SER_PTCACHE_P
                index_count_set_info.for_ser = TRUE;
#endif
                SHR_IF_ERR_MSG_EXIT(
                    bcmptm_pt_group_index_count_set(u, pt_group,
                                                    &index_count_set_info,
                                                    &bank_count,  /* in */
                                                    &start_bank), /* in */
                    (BSL_META_U(u, "WB: group_index_count_set failed "
                                "for group_id = %0d, num_banks = %0d\n"),
                     pt_group, bank_count));
            } else { /* coldboot */
                /* Init size of PTs for every PT in group to HW default.
                 * Also, set the HA_mem.bank_count for each PT group to HW
                 * default bank_count. */
                index_count_set_info.init = TRUE; /* init_bank_count */
                index_count_set_info.for_non_ser = TRUE;
                index_count_set_info.for_ser = FALSE;
#ifdef ALSO_INIT_SER_PTCACHE_P
                index_count_set_info.for_ser = TRUE;
#endif
                SHR_IF_ERR_MSG_EXIT(
                    bcmptm_pt_group_index_count_set(u, pt_group,
                                                    &index_count_set_info,
                                                    &bank_count,  /* out */
                                                    &start_bank), /* out */
                    (BSL_META_U(u, "CB: group_index_count_set failed "
                                "for group_id = %0d, num_banks = %0d\n"),
                     pt_group, bank_count));

                /* Store the HW default bank_count for each PT group in HA mem */
                pt_group_info_p->num_banks = bank_count;
                if (group_bank_start_en) {
                    pt_group_info_p->start_bank = start_bank;
                } else {
                    pt_group_info_p->start_bank = PTCACHE_GROUP_START_BANK_RSVD;
                }
            } /* coldboot */
        } /* !DEFAULT group */

        /* Run sinfo_init for all PTs in this PT group */
        SHR_IF_ERR_EXIT(
            sinfo_init_group(u, warm, pt_size_change, sid_count,
                             pt_group,
                             seds_index, cwme_index,
                             /* out */
                             &vinfo_word_count, &ltid_seg_word_count,
                             &group_seds_count, &group_cwme_count));

        /* Update cumulative offset into seds, cwme arrays */
        seds_index += group_seds_count;
        cwme_index += group_cwme_count;

        /* seds_index should not increment for non DEFAULT group. */
        SHR_IF_ERR_EXIT((seds_index &&
                         (pt_group != BCMPTM_PT_GROUP_DEFAULT)) ?
                        SHR_E_INTERNAL : SHR_E_NONE);

        /* Allocate vinfo array for this group */
        req_size = vinfo_word_count * sizeof(uint32_t);
        ref_vinfo_sign = BCMPTM_HA_SIGN_PTCACHE_VINFO + sid_count + pt_group;
        if (req_size) {
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheVinf", pt_group);
            SHR_IF_ERR_EXIT(
                bcmptm_do_ha_alloc(u, warm, req_size,
                                   alloc_str_id,
                                   BCMPTM_HA_SUBID_PTCACHE_VINFO + pt_group,
                                   ref_vinfo_sign, &alloc_ptr));
            pt_group_ptrs_p->vinfo_bp = alloc_ptr + 1; /* skip_sign_word */
        } else {
            pt_group_ptrs_p->vinfo_bp = NULL;
        }

        /* Allocate ltid seg for this group */
        if (ltid_size16b[u]) {
            req_size = pt_group_info_p->ltid_seg_word_count * sizeof(uint16_t);
        } else {
            req_size = pt_group_info_p->ltid_seg_word_count * sizeof(uint32_t);
        }
        ref_ltid_seg_sign = BCMPTM_HA_SIGN_PTCACHE_LTID_SEG + sid_count +
                            pt_group;
        if (req_size) {
            (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                               "ptmCacheLtid", pt_group);
            SHR_IF_ERR_EXIT(
                bcmptm_do_ha_alloc(u, warm, req_size,
                                   alloc_str_id,
                                   BCMPTM_HA_SUBID_PTCACHE_LTID_SEG + pt_group,
                                   ref_ltid_seg_sign, &alloc_ptr));
            pt_group_ptrs_p->ltid_seg_base_ptr = alloc_ptr + 1; /* skip_sign_word */
        } else {
            pt_group_ptrs_p->ltid_seg_base_ptr = NULL;
        }

        /* Cannot allocate space for cwme, seds until we finish processing
         * SIDs in DEFAULT group because:
         * - cwme_index can be incremented in all groups
         * - seds_index can only be incremented in group 0
         * - cwme, seds array are share same HW block.
         */

        /* sinfo_init done for for all groups. */
        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            if (warm) {
                /* Retrieve counts from sinfo array */
                seds_index = (sinfo_bp_us +
                              SINFO_RSVD_INDEX_SEDS_COUNT_WORD)->cw_index;
                cwme_index = (sinfo_bp_us +
                              SINFO_RSVD_INDEX_CWME_COUNT_WORD)->cw_index;
            } else {
                /* Store counts in sinfo array for WB */
                (sinfo_bp_us + SINFO_RSVD_INDEX_SEDS_COUNT_WORD)->cw_index =
                    seds_index;
                (sinfo_bp_us + SINFO_RSVD_INDEX_CWME_COUNT_WORD)->cw_index =
                    cwme_index;
            }

            /* Allocate space for seds array - pt_group 0 ONLY */
            req_size = seds_index * sizeof(uint32_t);
            SHR_IF_ERR_EXIT(
                bcmptm_do_ha_alloc(u, warm, req_size, "ptmCacheSeds",
                                   BCMPTM_HA_SUBID_PTCACHE_SEDS,
                                   ref_seds_sign, &alloc_ptr));
            seds_bp[u][s] = (uint32_t *)(alloc_ptr + 1); /* skip sign_word */
#ifdef ALSO_INIT_SER_PTCACHE_P
            seds_bp[u][SER] = seds_bp[u][s];
#endif

            /* Allocate space for cwme array - pt_group 0 ONLY */
            req_size = cwme_index * sizeof(bcmptm_ptcache_cwme_t);
            SHR_IF_ERR_EXIT(
                bcmptm_do_ha_alloc(u, warm, req_size, "ptmCacheCwme",
                                   BCMPTM_HA_SUBID_PTCACHE_CWME_SEG,
                                   ref_cwme_seg_sign, &alloc_ptr));
            cwme_bp[u][s] = (bcmptm_ptcache_cwme_t *)(alloc_ptr + 1); /* skip sign_word */
#ifdef ALSO_INIT_SER_PTCACHE_P
            cwme_bp[u][SER] = cwme_bp[u][s];
#endif
            if (!warm) {
                SHR_IF_ERR_EXIT(
                    bcmissu_struct_info_report(u, BCMMGMT_PTM_COMP_ID,
                                                     BCMPTM_HA_SUBID_PTCACHE_CWME_SEG,
                                                     4, /* offset */
                                                    /* 1st 4 bytes are sign_word */
                                                     sizeof(bcmptm_ptcache_cwme_t),
                                                     cwme_index,
                                                     BCMPTM_PTCACHE_CWME_T_ID));
            }

            pt_group_info_p->num_banks = 0; /* Not applicable for DEFAULT group */

            break; /* once we finish processing last (DEFAULT) group */
        } else { /* process next group */
            pt_group--;
        }
    } while (1); /* foreach PT group */


    /* Pass_2: Populate seds, cwme array, vinfo array, ltid_seg */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_comp_config(u, warm, BCMPTM_SUB_PHASE_2));

    /* Iterate through each group starting from last PT group down to
     * DEFAULT PT group. This allows us to work without knowing list of PTs
     * in DEFAULT group. */
    seds_index = 0;
    cwme_index = 0;
    pt_group = pt_group_count - 1; /* Start with last PT group */
    do {
        /* Run cwme_vinfo for all PTs in this group */
        SHR_IF_ERR_EXIT(
            cwme_vinfo_init_group(u, warm, pt_size_change, sid_count,
                                  pt_group));

        if (pt_group == BCMPTM_PT_GROUP_DEFAULT) {
            /* cwme_vinfo_init done for for all groups. */

            /* Can add more checks here. */

            break; /* once we finish processing last (DEFAULT) group */
        } else { /* process next group */
            pt_group--;
        }
    } while (1); /* foreach PT group */

    /* Verify all sentinels (practice) */
    SHR_IF_ERR_EXIT(bcmptm_ptcache_verify(u, FALSE)); /* non_ser */

    /* Register to know the changes in PT sizes */
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(u, BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        ptcache_pt_group_changed));

    /* Save important info in reserved sinfo section. */
    if (!warm) {
        (sinfo_bp_us + SINFO_RSVD_INDEX_UNUSED)->cw_index = 0;
        (sinfo_bp_us + SINFO_RSVD_INDEX_FLAGS_WORD)->cw_index =
            ptcache_flags_word;
        (sinfo_bp_us + SINFO_RSVD_INDEX_SID_COUNT_WORD)->cw_index = sid_count;
        /* And, finally, write signature - very last on purpose */
        (sinfo_bp_us + SINFO_RSVD_INDEX_SIGN_WORD)->cw_index = ref_sinfo_sign;
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmptm_ptcache_cleanup(u);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_cleanup(int u)
{
    bool also_cleanup_ser_ptcache_p = TRUE; 
    int s = 0;
    SHR_FUNC_ENTER(u);

    /* Release heap mem */
    if (pt_group_ptrs_bp[u][s]) {
        sal_free(pt_group_ptrs_bp[u][s]);
    }

    /* No need to release HA mem */

    /* Reset static vars */
    sinfo_bp[u][s] = NULL;
    seds_bp[u][s] = NULL;
    cwme_bp[u][s] = NULL;
    pt_group_info_bp[u][s] = NULL;
    pt_group_ptrs_bp[u][s] = NULL;
    if (also_cleanup_ser_ptcache_p) {
        sinfo_bp[u][SER] = NULL;
        seds_bp[u][SER] = NULL;
        cwme_bp[u][SER] = NULL;
        pt_group_info_bp[u][SER] = NULL;
        pt_group_ptrs_bp[u][SER] = NULL;
    }

/* exit: */
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_verify(int u, bool acc_for_ser)
{
    bcmdrd_sid_t sid;
    size_t sid_count = 0;
    uint32_t sw_index; /* index for sentinel word */
    bcmptm_ptcache_sinfo_t *sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo;
    bcmptm_ptcache_cwme_t *cwme_p;
    uint32_t *vinfo_bp;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = 0;
    SHR_FUNC_ENTER(u);
    if (acc_for_ser) {
        s = SER;
    }

    if (sinfo_bp[u][s] == NULL) {
       SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &sid_count));
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Number of sid = %0u\n"), (uint32_t)sid_count));

    /* return SHR_E_NONE; */
    for (sid = 0; sid < (uint32_t)sid_count; sid++) {
        sinfo_p = sinfo_bp[u][s] + sid;
        switch (sinfo_p->ptcache_type) {
            case PTCACHE_TYPE_SE_CACHE0:
            case PTCACHE_TYPE_SE_NOT_VALID:
            case PTCACHE_TYPE_SE_VALID:
#if defined(PTCACHE_SENTINEL_SE_ENABLE)
                sw_index = sinfo_p->cw_index - 1;
                SHR_IF_ERR_EXIT(
                    ptcache_sentinel_check(u, sid, sw_index,
                                           *(seds_bp[u][s] + sw_index),
                                           "REG", FALSE));
#endif /* PTCACHE_SENTINEL_SE_ENABLE */
                break;

            case PTCACHE_TYPE_ME:
            case PTCACHE_TYPE_ME_CCI:
                cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
                xinfo = &cwme_p->xinfo;
                pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
                vinfo_bp = pt_group_ptrs_p->vinfo_bp;

#if defined(PTCACHE_SENTINEL_ME_ENABLE)
                /* valid_seg */
                sw_index = cwme_p->vs_base - 1;
                SHR_IF_ERR_EXIT(
                    ptcache_sentinel_check(u, sid, sw_index,
                                           *(vinfo_bp + sw_index), "VS",
                                           FALSE));
                /* data_seg */
                if (xinfo->data_seg_en || xinfo->num_we_field) {
                    /* - Asserted data_seg_en means ds is used to store data
                     *   words.
                     * - Asserted num_we_field means ds is used to store
                     *   oinfo_words.
                     */
                    sw_index = cwme_p->ds_base - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(u, sid, sw_index,
                                               *(vinfo_bp + sw_index), "DS",
                                               FALSE));
                }

                /* dfid_seg */
                if (xinfo->dfid_seg_en) {
                    sw_index = cwme_p->dfs_base - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(u, sid, sw_index,
                                               *(vinfo_bp + sw_index), "DFS",
                                               FALSE));
                }
                /* info_seg */
                if (xinfo->info_seg_en) {
                    sw_index = cwme_p->is_base - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(u, sid, sw_index,
                                               *(vinfo_bp + sw_index), "IS",
                                               FALSE));
                }
                /* tc_seg */
                if (xinfo->tc_seg_en) {
                    sw_index = cwme_p->tcs_base - 1;
                    SHR_IF_ERR_EXIT(
                        ptcache_sentinel_check(u, sid, sw_index,
                                               *(vinfo_bp + sw_index), "TCS",
                                               FALSE));
                }
                /* ltid_seg */
                if (xinfo->ltid_seg_en) {
                    sw_index = cwme_p->ls_base - 1;
                    if (ltid_size16b[u]) { /* 16b ltid */
                        uint16_t *ltid_p =
                            (uint16_t *)pt_group_ptrs_p->ltid_seg_base_ptr;
                        SHR_IF_ERR_EXIT(
                            ptcache_sentinel_check(u, sid, sw_index,
                                                   *(ltid_p + sw_index),
                                                   "LTID_SEG", TRUE));
                    } else {
                        uint32_t *ltid_p =
                            (uint32_t *)pt_group_ptrs_p->ltid_seg_base_ptr;
                        SHR_IF_ERR_EXIT(
                            ptcache_sentinel_check(u, sid, sw_index,
                                                   *(ltid_p + sw_index),
                                                   "LTID_SEG", TRUE));
                    }
                }

#endif /* PTCACHE_SENTINEL_ME_ENABLE */
                break; /* PTCACHE_TYPE_ME, ME_CCI */

            default: /* PTCACHE_TYPE_NO_CACHE */
                /* ok */
                break;
        } /* ptcache_type */
    } /* BCMPTM_FOR_ITER */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_ctr_info_get(int u, bool acc_for_ser, bcmdrd_sid_t sid,
                            bcmptm_ptcache_ctr_info_t *ctr_info)
{
    bcmptm_ptcache_sinfo_t *sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo;
    bcmptm_ptcache_cwme_t *cwme_p;
    uint32_t *vinfo_bp;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = 0;
    SHR_FUNC_ENTER(u);
    if (acc_for_ser) {
        s = SER;
    }

    SHR_NULL_CHECK(ctr_info, SHR_E_PARAM);
    ctr_info->pt_is_ctr = FALSE; /* Symbols not relevant to PTcache are not
                                    counters */
    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);

    /* Read sinfo */
    sinfo_p = sinfo_bp[u][s] + sid;
    switch (sinfo_p->ptcache_type) {
        case PTCACHE_TYPE_CCI_ONLY:
            ctr_info->pt_is_ctr = TRUE;
            ctr_info->ctr_is_cci_only = TRUE;
            ctr_info->cci_map_id = sinfo_p->cw_index;
            break;
        case PTCACHE_TYPE_ME_CCI:
            cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
            xinfo = &cwme_p->xinfo;
            pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
            vinfo_bp = pt_group_ptrs_p->vinfo_bp;

            if (!xinfo->info_seg_en) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            ctr_info->pt_is_ctr = TRUE;
            ctr_info->ctr_is_cci_only = FALSE;
            ctr_info->cci_map_id = *(vinfo_bp + cwme_p->is_base);
            break;
        default:
            ctr_info->pt_is_ctr = FALSE;
            ctr_info->ctr_is_cci_only = FALSE;
            ctr_info->cci_map_id = 0;
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_read(int u,
                    bool acc_for_ser,
                    uint64_t flags,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    uint32_t entry_count,
                    size_t rsp_entry_wsize,
                    void *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    uint16_t *rsp_entry_dfid,
                    bcmltd_sid_t *rsp_entry_ltid)
{
    bcmdrd_sid_t w_sid = 0;
    uint32_t pt_index_count;
    uint32_t vinfo_ds_index, vinfo_vs_index, w_vinfo_vs_index, entry_num;
    uint32_t index = 0, entry_valid = 0;
    int tbl_inst = 0;
    uint32_t entry_wsize, w_entry_wsize;
    uint32_t vinfo_ltid_seg_index, vinfo_dfid_seg_index;
    bcmptm_ptcache_sinfo_t *sinfo_p, *w_sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo, *w_xinfo;
    bcmptm_ptcache_cwme_t *cwme_p = NULL, *w_cwme_p = NULL;
    uint32_t *vinfo_bp;
    void *ltid_seg_base_ptr;
    pt_group_ptrs_t *pt_group_ptrs_p;
    bool ret_ptcache_entry_p = FALSE;
    int s = 0;
    SHR_FUNC_ENTER(u);
    if (acc_for_ser) {
        s = SER;
    }

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_cache_vbit, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_dfid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_ltid, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(entry_count == 0 ? SHR_E_PARAM : SHR_E_NONE);

    /* defaults */
    *rsp_entry_cache_vbit = FALSE;
    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOCACHE;
    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOCACHE;
    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);

    ret_ptcache_entry_p = flags & BCMPTM_REQ_FLAGS_PTCACHE_RETURN_ENTRY_P;

    sinfo_p = sinfo_bp[u][s] + sid;

    switch (sinfo_p->ptcache_type) {
        case PTCACHE_TYPE_ME:
        case PTCACHE_TYPE_ME_CCI:
            if (sinfo_p->tbl_inst_en) {
                if (dyn_info->tbl_inst >= 0) {
                    tbl_inst = dyn_info->tbl_inst;
                    SHR_IF_ERR_EXIT(
                        (tbl_inst >= bcmdrd_pt_num_tbl_inst(u, sid)) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                } /* else tbl_inst = 0; --ANY_COPY for reads */
            }
            if (sinfo_p->index_en && (dyn_info->index > 0)) {
                index = dyn_info->index;
            }

            cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
            if (cwme_p->pt_index_min) {
                SHR_IF_ERR_EXIT(index < cwme_p->pt_index_min ?
                                SHR_E_PARAM : SHR_E_NONE);
                index -= cwme_p->pt_index_min; /* ptcache uses 0-based index */
            }
            pt_index_count = cwme_p->pt_index_count;
            SHR_IF_ERR_EXIT(
                ((index + entry_count) > pt_index_count) ?
                SHR_E_PARAM : SHR_E_NONE);
            if (tbl_inst) {
                entry_num = (tbl_inst * pt_index_count) + index;
            } else {
                entry_num = index;
            }

            /* Read xinfo */
            xinfo = &cwme_p->xinfo;
            pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
            vinfo_bp = pt_group_ptrs_p->vinfo_bp;
            ltid_seg_base_ptr = pt_group_ptrs_p->ltid_seg_base_ptr;

            /* Limitation:
             *      1. We do not support DMA for PTs which require ltid, dfid
             *         storage - as this would require exchange of ltid, dfid
             *         arrays in mreq APIs.
             *
             *      2. We do not support DMA for PTs that are overlays (narrow
             *         views of wider PT. Performance degradation involved in
             *         extraction of narrow view from wider view may defeat
             *         gains from doing DMA.
             */
            if ((xinfo->overlay_mode &&
                 (xinfo->overlay_mode != BCMPTM_OINFO_MODE_TCAM_DATA)) ||
                xinfo->ltid_seg_en || xinfo->dfid_seg_en) {
                SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_UNAVAIL
                                                 : SHR_E_NONE);
            }

            vinfo_vs_index = cwme_p->vs_base;
                /* LHS points to 1st word in valid_seg */
            vinfo_vs_index += entry_num / 32;
            if (BCMPTM_WORD_BMAP_TST(*(vinfo_bp + vinfo_vs_index), entry_num)) {
                entry_valid = 1;
            }

            if (xinfo->overlay_mode) {
                ptcache_oinfo_w0_t oinfo_w0;
                ptcache_oinfo_wn_t oinfo_w1;
                uint32_t w_vinfo_ds_index, w_sbit, w_ebit, ne_entry_wsize;

                /* We cannot support this for narrow overlay type SIDs. */
                SHR_IF_ERR_EXIT(ret_ptcache_entry_p ? SHR_E_PARAM : SHR_E_NONE);

                /* Always return the data from widest_entry even if:
                 * a. narrow_entry_valid = 0
                 * b. widest_entry_valid = 0
                 */

                /* req entry size */
                entry_wsize = bcmdrd_pt_entry_wsize(u, sid);

                SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize) ?
                                SHR_E_PARAM : SHR_E_NONE);

                /* Read oinfo_w0,1 for narrow_sid */
                oinfo_w0.entry = *(vinfo_bp + cwme_p->ds_base);
                oinfo_w1.entry = *(vinfo_bp + cwme_p->ds_base + 1);

                /* Narrow entry_size */
                ne_entry_wsize = BCMPTM_BITS2WORDS(oinfo_w1.f.width);

                /* Read sinfo for widest_sid */
                w_sid = oinfo_w0.f.sid;
                w_sinfo_p = sinfo_bp[u][s] + w_sid;
                w_entry_wsize = bcmdrd_pt_entry_wsize(u, w_sid);

                /* Read xinfo for widest_sid */
                w_cwme_p = cwme_bp[u][s] + w_sinfo_p->cw_index;
                w_xinfo = &w_cwme_p->xinfo;
                /* All PTs in same group share vinfo_array, ltid_seg
                 * so wide PT will have same pt_group, vinfo_bp,
                 * ltid_seg_base_ptr, * etc. */

                /* offset for 1st word in data_seg of widest_view */
                w_vinfo_ds_index = w_cwme_p->ds_base;

                /* Few sanity checks for widest_view */
                if (w_sinfo_p->ptcache_type != PTCACHE_TYPE_ME ||
                    (ne_entry_wsize > entry_wsize) ||
                    (w_sinfo_p->tbl_inst_en != sinfo_p->tbl_inst_en) ||
                    (w_xinfo->pt_group != xinfo->pt_group) ||
                    w_xinfo->overlay_mode ||
                    !w_xinfo->data_seg_en) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }

                /* Extract narrow_entry data from wide_entry */
                if (xinfo->overlay_mode == BCMPTM_OINFO_MODE_HASH) {
                    uint32_t w_index;
                    /* Note: we are not checking validity of w_entry on purpose
                     *       For eg: even if wide_entry invalid, any/all narrow
                     *               entry/entries on that wide_row could be
                     *               valid.
                     */

                    /* Retrieve entry info for widest view */
                    SHR_IF_ERR_EXIT(
                        ptcache_w_entry_calc(u, &oinfo_w0, &oinfo_w1,
                                             /* sinfo_p->tbl_inst_en, */
                                                /* same for narrow,wide views */
                                             tbl_inst,
                                                /* same for narrow,wide views */
                                             index,
                                                /* in tbl_inst of narrow view */
                                             w_cwme_p->pt_index_count,
                                             w_entry_wsize,

                                             &w_index, &w_sbit, &w_ebit,
                                             &w_vinfo_ds_index));

                    /* Extract the narrow entry data (without hit bits)
                     * from wide entry into rsp buffer */
                    bcmdrd_field_get(vinfo_bp + (w_vinfo_ds_index),
                                     w_sbit, w_ebit, rsp_entry_words);

                    /* For debug display below */
                    vinfo_ds_index = w_vinfo_ds_index;
                } else { /* overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA */
                    /* Retrieve narrow_entry from wide_entry */
                    SHR_IF_ERR_EXIT(
                        ptcache_overlay_tcam_data_read(
                            u, &oinfo_w0,
                            vinfo_bp, /* same for all PTs in same group */
                            cwme_p->ds_base + 1,
                               /* overlay word (for narrow entry view) that
                                * holds 1st (width, start_bit) pair for
                                * narrow entry */
                            tbl_inst,
                               /* same for narrow,wide views */
                            index,
                               /* in tbl_inst of narrow view */
                            w_cwme_p->pt_index_count,
                            w_entry_wsize,
                            entry_wsize, /* size of narrow entry */
                            entry_count,

                            rsp_entry_words,
                            &w_vinfo_ds_index));

                    /* For debug display below
                     * - Will point to last entry when entry_count > 1.
                     *   Don't add logic just for debug. */
                    vinfo_ds_index = w_vinfo_ds_index;

                    /* For Tcam_data type overlay, valid bit of narrow entry is
                     * not meaningful. Only valid bit of parent wide entry is
                     * used.
                     * Also, for Tcam_data type overlays, entry_num of wide
                     * entry is same as that for narrow entry */
                    w_vinfo_vs_index = w_cwme_p->vs_base;
                        /* LHS points to 1st word in valid_seg */
                    w_vinfo_vs_index += entry_num / 32;
                    if (BCMPTM_WORD_BMAP_TST(*(vinfo_bp + w_vinfo_vs_index),
                                             entry_num)) {
                        entry_valid = 1;
                    } else {
                        entry_valid = 0;
                    }

                    /* For debug display below */
                    vinfo_vs_index = w_vinfo_vs_index;
                } /* BCMPTM_OINFO_MODE_TCAM_DATA */

            } else { /* non_overlay mem */
                sal_memset(&w_xinfo, 0, sizeof(bcmptm_ptcache_xinfo_t));
                SHR_IF_ERR_EXIT(!xinfo->data_seg_en ? SHR_E_INTERNAL
                                                    : SHR_E_NONE);
                entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
                vinfo_ds_index = cwme_p->ds_base;
                    /* LHS points to 1st word of 1st_entry in data_seg */
                vinfo_ds_index += entry_num * entry_wsize;
                if (ret_ptcache_entry_p) {
                    /* caller only needs pointer to PTcache data */
                    uint32_t **ptcache_entry_p = rsp_entry_words;
                    *ptcache_entry_p = vinfo_bp + (vinfo_ds_index);
                } else { /* Normal case */
                    uint32_t *rsp_buf_p = rsp_entry_words;
                    SHR_IF_ERR_EXIT(
                        ((entry_count * entry_wsize) > rsp_entry_wsize) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    sal_memcpy(rsp_buf_p,
                               vinfo_bp + (vinfo_ds_index),
                               4 * entry_wsize * entry_count);
                }
            } /* non_overlay mem */

            if (entry_valid == 1) { /* strict checking */
                *rsp_entry_cache_vbit = TRUE;

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "MemData read from cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay_mode=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d\n"),
                     sid, tbl_inst, index, xinfo->overlay_mode,
                     vinfo_vs_index, vinfo_ds_index));

                /* rsp LTID */
                if ((xinfo->overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_xinfo->ltid_seg_en)) {
                    vinfo_ltid_seg_index = w_cwme_p->ls_base;
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_get(u, w_sid, /* for display only */
                                         entry_num,
                                         ltid_seg_base_ptr,
                                                /* same for narrow/wide */
                                         vinfo_ltid_seg_index,
                                         rsp_entry_ltid));
                } else if (xinfo->ltid_seg_en) {
                    /* For non_overlay case:
                     *      we must check ltid_seg_en - Normal.
                     *
                     * For overlay_hash case:
                     *      ltid is stored separately for overlay, wide views.
                     *
                     * For overlay_tcam case,
                     *      we are guaranteed that ltid_seg_en = 0 for narrow
                     *      (overlay) - so we cannot be here.
                     */
                    vinfo_ltid_seg_index = cwme_p->ls_base;
                    /* LHS points to 1st word of 1st entry in ltid_seg */

                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_get(u, sid, /* for display only */
                                         entry_num,
                                         ltid_seg_base_ptr,
                                         vinfo_ltid_seg_index,
                                         rsp_entry_ltid));
                } else {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
                } /* ltid_not_supported */

                /* rsp DFID */
                if ((xinfo->overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    w_xinfo->dfid_seg_en) {
                    vinfo_dfid_seg_index = w_cwme_p->dfs_base;
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_get(u, w_sid, /* for display only */
                                         w_xinfo->dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_bp, /* same for narrow/wide sid */
                                         vinfo_dfid_seg_index,
                                         rsp_entry_dfid));
                } else if (xinfo->dfid_seg_en) {
                    /* For non_overlay case:
                     *      we must check dfid_seg_en - Normal.
                     *
                     * For overlay_hash case:
                     *      dfid is stored separately for overlay, wide view.
                     *
                     * For overlay_tcam case,
                     *      we are guaranteed that dfid_seg_en = 0 for narrow
                     *      (overlay) - so we cannot be here.
                     */
                    vinfo_dfid_seg_index = cwme_p->dfs_base;
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_get(u, sid, /* for display only */
                                         xinfo->dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_bp,
                                         vinfo_dfid_seg_index,
                                         rsp_entry_dfid));
                } else {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
                } /* dfid_not_supported */

            } else { /* entry is not valid in cache */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "MemData not valid in cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay_mode=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d, "
                                "vinfo_ltid_seg_index=%0d\n, "
                                "vinfo_dfid_seg_index=%0d\n"),
                     sid, tbl_inst, index, xinfo->overlay_mode,
                     vinfo_vs_index, vinfo_ds_index, -1, -1));
                if (xinfo->ltid_seg_en) {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_INVALID;
                } else {
                    *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
                } /* ltid_not_supported */

                if (xinfo->dfid_seg_en) {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_INVALID;
                } else {
                    *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
                } /* dfid_not_supported */
            }
            break; /* PTCACHE_TYPE_ME, ME_CCI */

        case PTCACHE_TYPE_SE_VALID:
        case PTCACHE_TYPE_SE_NOT_VALID:
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
            if (ret_ptcache_entry_p) {
                /* caller only needs pointer to PTcache data */
                uint32_t **ptcache_entry_p = rsp_entry_words;
                *ptcache_entry_p = seds_bp[u][s] + sinfo_p->cw_index;
            } else { /* Normal case */
                uint32_t *rsp_buf_p = rsp_entry_words;
                SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize) ?
                                SHR_E_PARAM : SHR_E_NONE);
                /* Because we have initialized cache with HW default value,
                 * data in cache is still correct */
                sal_memcpy(rsp_buf_p,
                           seds_bp[u][s] + sinfo_p->cw_index,
                           4 * entry_wsize);
            }
            if (sinfo_p->ptcache_type == PTCACHE_TYPE_SE_VALID) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "RegData read from cache... for sid=%0d, "
                                "tbl_inst=%0d, index=%0d, seds_index=%0d\n"),
                     sid, tbl_inst, index, sinfo_p->cw_index));
                *rsp_entry_cache_vbit = TRUE;
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "RegData not valid in cache... for sid=%0d, "
                                "tbl_inst=%0d, index=%0d, seds_index=%0d\n"),
                     sid, tbl_inst, index, sinfo_p->cw_index));
            }
            /* Note: No LTID storage for Register type PTs */
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
            break;

        case PTCACHE_TYPE_SE_CACHE0:
            /* Return rsp_entry_cache_vbit = FALSE
             * Return SHR_E_NONE
             * Return all_zeros as the data.
             *
             * RM should never be reading the CACHE0 views (eg: HitBitOnly).
             *
             * Undo logic in WAL can issue read to this table when it
             * gets wal_write - to store previous value in undo_words.
             *
             * Interactive path, can read this table - but because we
             * return rsp_entry_cache_vbit FALSE - so read will be done from HW.
             */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
            /* Return pointer to PTcache read_data is not supported for
             * such (HitBitOnly) views */
            SHR_IF_ERR_EXIT(ret_ptcache_entry_p ? SHR_E_PARAM : SHR_E_NONE);
            {
                uint32_t *rsp_buf_p = rsp_entry_words;
                SHR_IF_ERR_EXIT((entry_wsize > rsp_entry_wsize) ?
                                SHR_E_PARAM : SHR_E_NONE);
                /* Return 0s to rsp_buffer.
                 * For WAL undo, writes to HitBitOnly view in PTcache will
                 * be ignored. So data that we return for read is
                 * dont-care.
                 */
                sal_memset(rsp_buf_p, 0, 4 * entry_wsize);
                /* Always return rsp_cache_vbit FALSE */
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Data not avail in cache (SE_CACHE0)...for"
                            " sid=%0d, tbl_inst=%0d, index=%0d, "
                            "seds_index=%0d\n"),
                 sid, tbl_inst, index, sinfo_p->cw_index));

            /* We dont store rsp_entry_ltid for HitBitOnly type PTs */
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOT_SUPPORTED;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOT_SUPPORTED;
            break;

        default: /* PTCACHE_TYPE_NO_CACHE, PTCACHE_TYPE_CCI_ONLY */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "sid not cached ... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                 sid, tbl_inst, index));
            *rsp_entry_ltid = BCMPTM_LTID_RSP_NOCACHE;
            *rsp_entry_dfid = BCMPTM_DFID_RSP_NOCACHE;
            SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Return values:
 *     SHR_E_INTERNAL - tbl_inst_max < tbl_inst_min when dyn_info.tbl_inst=-1
 *     SHR_E_PARAM
 */
int
bcmptm_ptcache_update(int u, bool acc_for_ser,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      uint32_t entry_count,
                      uint32_t *entry_words,
                      bool set_cache_vbit,
                      bcmltd_sid_t req_ltid,
                      uint16_t req_dfid)
{
    bcmdrd_sid_t w_sid = 0;
    uint32_t pt_index_count;
    uint32_t vinfo_ds_index, vinfo_vs_index, w_vinfo_vs_index, entry_num;
    uint32_t vinfo_vs_index_base, index = 0;
    int tbl_inst = 0, tbl_inst_min = 0, tbl_inst_max = 0;
    uint32_t entry_wsize;
    uint32_t vinfo_ltid_seg_index, vinfo_dfid_seg_index;
    bcmptm_ptcache_sinfo_t *sinfo_p, *w_sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo, *w_xinfo;
    bcmptm_ptcache_cwme_t *cwme_p = NULL, *w_cwme_p = NULL;
    uint32_t *vinfo_bp;
    void *ltid_seg_base_ptr;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = 0;
    SHR_FUNC_ENTER(u);
    if (acc_for_ser) {
        s = SER;
    }

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(entry_count == 0 ? SHR_E_PARAM : SHR_E_NONE);
    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);

    sinfo_p = sinfo_bp[u][s] + sid;

    switch (sinfo_p->ptcache_type) {
        case PTCACHE_TYPE_ME:
        case PTCACHE_TYPE_ME_CCI:
            if (sinfo_p->index_en && (dyn_info->index > 0)) {
                index = dyn_info->index;
            }

            /* Validate index */
            cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
            if (cwme_p->pt_index_min) {
                SHR_IF_ERR_EXIT(index < cwme_p->pt_index_min ?
                                SHR_E_PARAM : SHR_E_NONE);
                index -= cwme_p->pt_index_min; /* ptcache uses 0-based index */
            }
            pt_index_count = cwme_p->pt_index_count;
            SHR_IF_ERR_EXIT(
                ((index + entry_count) > pt_index_count) ?
                SHR_E_PARAM : SHR_E_NONE);

            /* set tbl_inst_min, max */
            if (sinfo_p->tbl_inst_en) {
                tbl_inst_max = cwme_p->tbl_inst_max;
                if (dyn_info->tbl_inst >= 0) { /* SINGLE INST WRITE */
                    SHR_IF_ERR_EXIT(
                        (dyn_info->tbl_inst > tbl_inst_max) ?
                        SHR_E_PARAM : SHR_E_NONE);
                    tbl_inst_min = tbl_inst_max = dyn_info->tbl_inst;
                } else { /* WRITE_ALL_COPIES */
                    tbl_inst_min = 0;
                }
            }
            /* req entry size */
            entry_wsize = bcmdrd_pt_entry_wsize(u, sid);

            /* Read xinfo */
            xinfo = &cwme_p->xinfo;
            pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
            vinfo_bp = pt_group_ptrs_p->vinfo_bp;
            ltid_seg_base_ptr = pt_group_ptrs_p->ltid_seg_base_ptr;

            /* Limitation:
             *      1. We do not support DMA for PTs which require ltid, dfid
             *         storage - as this would require exchange of ltid, dfid
             *         arrays in mreq APIs.
             *
             *      2. We do not support DMA for PTs that are overlays (narrow
             *         views of wider PT. Performance degradation involved in
             *         extraction of narrow view from wider view may defeat
             *         gains from doing DMA.
             */
            if ((xinfo->overlay_mode &&
                 (xinfo->overlay_mode != BCMPTM_OINFO_MODE_TCAM_DATA)) ||
                xinfo->ltid_seg_en || xinfo->dfid_seg_en) {
                SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_UNAVAIL
                                                 : SHR_E_NONE);
            }

            for (tbl_inst = tbl_inst_min; tbl_inst <= tbl_inst_max; tbl_inst++) {
                if (tbl_inst) {
                    entry_num = (tbl_inst * pt_index_count) + index;
                } else {
                    entry_num = index;
                }

                vinfo_vs_index = cwme_p->vs_base;
                    /* LHS points to 1st word in valid_seg */
                vinfo_vs_index_base = vinfo_vs_index; /* vinfo_vs_index will get
                                                         modified, so save base */
                vinfo_vs_index += entry_num / 32;

                /* Need to update the entry_words in cache */
                if (xinfo->overlay_mode) {
                    ptcache_oinfo_w0_t oinfo_w0;
                    ptcache_oinfo_wn_t oinfo_w1;
                    uint32_t w_vinfo_ds_index, w_sbit, w_ebit,
                             ne_entry_wsize, w_entry_wsize;

                    /* Read oinfo_w0,1 for narrow_sid */
                    oinfo_w0.entry = *(vinfo_bp + cwme_p->ds_base);
                    oinfo_w1.entry = *(vinfo_bp + cwme_p->ds_base + 1);

                    /* Narrow entry_size */
                    ne_entry_wsize = BCMPTM_BITS2WORDS(oinfo_w1.f.width);

                    /* Read sinfo for widest_sid */
                    w_sid = oinfo_w0.f.sid;
                    w_sinfo_p = sinfo_bp[u][s] + w_sid;
                    w_entry_wsize = bcmdrd_pt_entry_wsize(u, w_sid);

                    /* Read xinfo for widest_sid */
                    w_cwme_p = cwme_bp[u][s] + w_sinfo_p->cw_index;
                    w_xinfo = &w_cwme_p->xinfo;
                    /* All PTs in same group share vinfo_array, ltid_seg
                     * so wide PT will have same pt_group, vinfo_bp,
                     * ltid_seg_base_ptr, * etc. */

                    /* offset to 1st word in data_seg of widest_view */
                    w_vinfo_ds_index = w_cwme_p->ds_base;

                    /* Few sanity checks for widest_view */
                    if (w_sinfo_p->ptcache_type != PTCACHE_TYPE_ME ||
                        (ne_entry_wsize > entry_wsize) ||
                        (w_sinfo_p->tbl_inst_en != sinfo_p->tbl_inst_en) ||
                        (w_xinfo->pt_group != xinfo->pt_group) ||
                        w_xinfo->overlay_mode ||
                        !w_xinfo->data_seg_en) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }

                    if (xinfo->overlay_mode == BCMPTM_OINFO_MODE_HASH) {
                        uint32_t w_index;
                        /* Retrieve entry info for widest view */
                        SHR_IF_ERR_EXIT(
                            ptcache_w_entry_calc(u, &oinfo_w0, &oinfo_w1,
                                                 /* sinfo_p->tbl_inst_en, */
                                                /* same for narrow,wide views */
                                                 tbl_inst,
                                                /* same for narrow,wide views */
                                                 index,
                                                /* in tbl_inst of narrow view */
                                                 w_cwme_p->pt_index_count,
                                                 w_entry_wsize,

                                                 &w_index, &w_sbit, &w_ebit,
                                                 &w_vinfo_ds_index));

                        /* Stuff narrow entry data into wide entry.
                         * Note: following will not copy Hit bits, etc from
                         *       'entry_words' into wide_cache_entry */
                        bcmdrd_field_set(vinfo_bp + w_vinfo_ds_index,
                                         w_sbit, w_ebit, entry_words);

                        /* For debug display below */
                        vinfo_ds_index = w_vinfo_ds_index;
                    } else { /* overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA */
                        SHR_IF_ERR_EXIT(
                            ptcache_overlay_tcam_data_write(
                                u,
                                &oinfo_w0,
                                vinfo_bp, /* same for all PTs in same group */
                                cwme_p->ds_base + 1,
                                   /* overlay word (for narrow entry view) that
                                    * holds 1st (width, start_bit) pair for
                                    * narrow entry */
                                tbl_inst,
                                   /* same for narrow,wide views */
                                index,
                                   /* in tbl_inst of narrow view */
                                w_cwme_p->pt_index_count,
                                w_entry_wsize,
                                entry_wsize, /* size of narrow entry */
                                entry_count,
                                entry_words,

                                &w_vinfo_ds_index));

                        /* For debug display below
                         * - Will point to last entry when entry_count > 1.
                         *   Don't add logic just for debug. */
                        vinfo_ds_index = w_vinfo_ds_index;

                        /* For Tcam_data type overlay, valid bit of narrow entry
                         * is not meaningful. Only valid bit of parent wide
                         * entry is used.
                         * Also, for Tcam_data type overlays, entry_num of wide
                         * entry is same as that for narrow entry */
                        w_vinfo_vs_index = w_cwme_p->vs_base;
                            /* LHS points to 1st word in valid_seg */
                        vinfo_vs_index_base = w_vinfo_vs_index; /* for later */
                        w_vinfo_vs_index += entry_num / 32;

                        /* Setting of cache_vbit will be done only for parent
                         * view (see below).
                         * Also helps debug display */
                        vinfo_vs_index = w_vinfo_vs_index;

                    } /* BCMPTM_OINFO_MODE_TCAM_DATA */
                } else { /* non_overlay */
                    sal_memset(&w_xinfo, 0, sizeof(bcmptm_ptcache_xinfo_t));
                    SHR_IF_ERR_EXIT(!xinfo->data_seg_en ?
                                    SHR_E_INTERNAL : SHR_E_NONE);

                    vinfo_ds_index = cwme_p->ds_base;
                    /* LHS points to 1st word of 1st entry in data_seg */
                    vinfo_ds_index += entry_num * entry_wsize;

                    sal_memcpy(vinfo_bp + vinfo_ds_index,
                               entry_words, 4 * entry_wsize * entry_count);
                } /* non_xinfo or non_overlay mem */

                /* Update valid bit for cache_entry */
                cache_vbit_update(u, set_cache_vbit,
                                  vinfo_bp,
                                  vinfo_vs_index_base,
                                        /* 1st word in valid seg */
                                  vinfo_vs_index,
                                        /* vs_index for first entry */
                                  entry_num, /* first_entry_num */
                                  entry_count);
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "MemData written to cache... for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "overlay=%0d, vinfo_vs_index=%0d, "
                                "vinfo_ds_index=%0d\n"),
                     sid, tbl_inst, index, xinfo->overlay_mode,
                     vinfo_vs_index, vinfo_ds_index));

                if (set_cache_vbit &&
                    (xinfo->overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    (w_xinfo->ltid_seg_en)) {
                    vinfo_ltid_seg_index = w_cwme_p->ls_base;
                    /* LHS points to 1st word of 1st entry in ltid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_set(u, w_sid, /* for display only */
                                         entry_num,
                                         ltid_seg_base_ptr,
                                                /* same for narrow/wide */
                                         vinfo_ltid_seg_index,
                                         req_ltid));
                } else if (set_cache_vbit && xinfo->ltid_seg_en) {
                    /* Presence of ltid_seg has no relation to overlay_en.
                     * Even if this SID is an overlay (thin view) of wider mem,
                     * we maintain separate cache_vbit, ltid for this view.
                     *
                     * Also, it is guaranteed that xinfo_en will be 1 if we have
                     * ltid_seg for a SID - so need to check for other
                     * combinations of xinfo_en and ltid_seg_en
                     */
                    /* For non_overlay case:
                     *      we must check ltid_seg_en - Normal
                     *
                     * For overlay_hash case:
                     *      ltid is stored separately for overlay, wide views.
                     *
                     * For overlay_tcam case:
                     *      we are guaranteed that ltid_seg_en = 0 for narrow
                     *      view - so we cannot be here.
                     */
                    vinfo_ltid_seg_index = cwme_p->ls_base;
                    /* LHS points to 1st word of 1st entry in ltid_seg */

                    SHR_IF_ERR_EXIT(
                        ptcache_ltid_set(u, sid, /* for display only */
                                         entry_num,
                                         ltid_seg_base_ptr,
                                         vinfo_ltid_seg_index,
                                         req_ltid));
                } /* ltid_seg_en */

                if (set_cache_vbit &&
                    (xinfo->overlay_mode == BCMPTM_OINFO_MODE_TCAM_DATA) &&
                    w_xinfo->dfid_seg_en) {
                    vinfo_dfid_seg_index = w_cwme_p->dfs_base;
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_set(u, w_sid, /* for display only */
                                         w_xinfo->dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_bp, /* same for narrow/wide sid */
                                         vinfo_dfid_seg_index,
                                         req_dfid));
                } else if (set_cache_vbit && xinfo->dfid_seg_en) {
                    /* Presence of dfid_seg has no relation to overlay_en.
                     * Even if this SID is an overlay (thin view) of wider mem,
                     * we maintain separate cache_vbit, dfid for this view.
                     *
                     * Also, it is guaranteed that xinfo_en will be 1 if we have
                     * dfid_seg for a SID - so need to check for other
                     * combinations of xinfo_en and dfid_seg_en
                     */
                    /* For non_overlay case:
                     *      we must check dfid_seg_en - Normal.
                     *
                     * For overlay_hash case:
                     *      dfid is stored separately for overlay, wide view.
                     *
                     * For overlay_tcam case,
                     *      we are guaranteed that dfid_seg_en = 0 for narrow
                     *      (overlay) - so we cannot be here.
                     */
                    vinfo_dfid_seg_index = cwme_p->dfs_base;
                    /* LHS points to 1st word of 1st entry in dfid_seg */
                    SHR_IF_ERR_EXIT(
                        ptcache_dfid_set(u, sid, /* for display only */
                                         xinfo->dfid_seg_en, /* dfid_bytes */
                                         entry_num,
                                         vinfo_bp,
                                         vinfo_dfid_seg_index,
                                         req_dfid));
                } else if (set_cache_vbit && req_dfid &&
                           (req_dfid < BCMPTM_DFID_IREQ)) {
                    /* RM trying to write non-zero dfid for sid that does not
                       support dfid */
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "dfid not supported for sid=%s(%0d), "
                                    "tbl_inst=%0d, index=%0d, req_dfid=%0d\n"),
                         bcmdrd_pt_sid_to_name(u, sid), sid, tbl_inst, index,
                         req_dfid));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } /* for each tbl_inst */
            break; /* PTCACHE_TYPE_ME, ME_CCI */

        case PTCACHE_TYPE_SE_NOT_VALID:
        case PTCACHE_TYPE_SE_VALID:
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            entry_wsize = bcmdrd_pt_entry_wsize(u, sid);
            sal_memcpy(seds_bp[u][s] + sinfo_p->cw_index,
                       entry_words,
                       4 * entry_wsize);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "RegData written to cache... for "
                            "sid=%0d, tbl_inst=%0d, index=%0d, "
                            "vinfo_cw_index=%0d\n"),
                 sid, tbl_inst, index, sinfo_p->cw_index));

            /* Valid bit for cache_entry */
            sinfo_p->ptcache_type = set_cache_vbit ?
                PTCACHE_TYPE_SE_VALID : PTCACHE_TYPE_SE_NOT_VALID;
            break; /* PTCACHE_TYPE_SE */

        case PTCACHE_TYPE_SE_CACHE0:
            /* Ignore the write and return SHR_E_NONE */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "SE_CACHE0 view write ignored ... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                 sid, tbl_inst, index));
            break;

        default: /* PTCACHE_TYPE_NO_CACHE, PTCACHE_TYPE_CCI_ONLY */
            SHR_IF_ERR_EXIT(entry_count != 1 ? SHR_E_PARAM : SHR_E_NONE);
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "sid not cached ... for sid=%0d, "
                            "tbl_inst=%0d, index=%0d\n"),
                 sid, tbl_inst, index));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_update_ireq(int u,
                           bool acc_for_ser,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           uint32_t *entry_words,
                           bool set_cache_vbit,
                           bcmltd_sid_t req_ltid,
                           uint16_t req_dfid)
{
    int tmp_rv;
    uint32_t tmp_entry_words[BCMPTM_MAX_PT_ENTRY_WORDS];
    bcmltd_sid_t tmp_rsp_ltid, update_ltid;
    uint16_t tmp_rsp_dfid, update_dfid;
    bool tmp_rsp_cache_vbit = FALSE;
    uint32_t entry_count = 1;
    SHR_FUNC_ENTER(u);

    /* For debug reasons, it is possible that user may want to modify a modeled
     * table on interactive path.
     *
     * NOTE: This will work ONLY if user knows that modeled path is currently
     * not * working on this table and/or entry. This is user-responsibility.
     *
     * Our responsibility:
     *     If the entry was already valid in cache and owner was mreq path,
     *     write on interactive path must not trash the LTID, DFID - as this
     *     will make entry un-recognizable by Modeled path after this
     *     modification.
     *
     *     If entry was valid in cache and owner was ireq path, it is ok to
     *     modify LTID, DFID for this entry.
     *
     *     Must Document:
     *     If entry was not valid in cache, it is ok to modify LTID, DFID for
     *     this entry. If this table is modeled, the rsrc_mgr blocks will
     *     encounter a valid entry in cache - but for which dfid = DFID_IREQ -
     *     so they must consider this as 'invalid' entry.
     */

    /* First we must read previous state of entry in cache */
    tmp_rv = bcmptm_ptcache_read(u, acc_for_ser,
                                 0, /* flags */
                                 sid,
                                 dyn_info,
                                 entry_count,
                                 COUNTOF(tmp_entry_words),
                                 tmp_entry_words, /* dont_care */
                                 &tmp_rsp_cache_vbit,
                                 &tmp_rsp_dfid,
                                 &tmp_rsp_ltid);
    SHR_IF_ERR_EXIT_EXCEPT_IF(tmp_rv, SHR_E_UNAVAIL);
    if (tmp_rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
        /* silently because it is ok if cache is not alloc for this sid */
    }

    if (tmp_rsp_cache_vbit) { /* entry is valid in cache */
        if (tmp_rsp_ltid >= BCMPTM_LTID_IREQ) {
           update_ltid = req_ltid; /* allow over-write */
        } else {
           update_ltid = tmp_rsp_ltid; /* preserve (managed by RM) */
        }

        if (tmp_rsp_dfid >= BCMPTM_DFID_IREQ) {
           update_dfid = req_dfid; /* over-write */
        } else {
           update_dfid = tmp_rsp_dfid; /* preserve (managed by RM) */
        }
    } else { /* entry is not valid in cache, allow over-write */
        update_ltid = req_ltid;
        update_dfid = req_dfid;
    }
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_update(u, acc_for_ser,
                              sid,
                              dyn_info,
                              entry_count,
                              entry_words, /* km_entry */
                              TRUE,        /* Cache_valid_set */
                              update_ltid,
                              update_dfid));
exit:
    SHR_FUNC_EXIT();
}

/* Return values:
 *     SHR_E_PARAM
 *     SHR_E_UNAVAIL  - If cache is not alloc for this pt
 *     SHR_E_INTERNAL - tbl_inst_max < tbl_inst_min when dyn_info.tbl_inst=-1
 *     SHR_E_NONE - cache is allocated for this pt, wr cache and set valid
 */
int
bcmptm_ptcache_tc_cmd(int u,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      uint32_t tc_cmd,
                      uint32_t *prev_tc_rdata)
{
    uint32_t pt_index_count;
    uint32_t vinfo_tc_index, entry_num;
    int tbl_inst = 0, tbl_inst_min = 0, tbl_inst_max = 0;
    uint32_t index = 0;
    bcmptm_ptcache_sinfo_t *sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo;
    bcmptm_ptcache_cwme_t *cwme_p;
    uint32_t *vinfo_bp;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = SER; /* Assume: this fn is ONLY called by SER logic. */
    SHR_FUNC_ENTER(u);

    SHR_NULL_CHECK(dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(prev_tc_rdata, SHR_E_PARAM);
    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);

    /* Read sinfo */
    sinfo_p = sinfo_bp[u][s] + sid;
    if (sinfo_p->ptcache_type != PTCACHE_TYPE_ME) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Read xinfo */
    cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
    xinfo = &cwme_p->xinfo;
    if (!xinfo->tc_seg_en) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
    vinfo_bp = pt_group_ptrs_p->vinfo_bp;

    /* set tbl_inst_min, max */
    if (sinfo_p->tbl_inst_en) {
        tbl_inst_max = cwme_p->tbl_inst_max;
        if (dyn_info->tbl_inst >= 0) { /* SINGLE INST WRITE */
            SHR_IF_ERR_EXIT(
                (dyn_info->tbl_inst > tbl_inst_max) ?
                SHR_E_PARAM : SHR_E_NONE);
            tbl_inst_min = tbl_inst_max = dyn_info->tbl_inst;
        } else { /* WRITE_ALL_COPIES */
            tbl_inst_min = 0;
        }
    }
    if (sinfo_p->index_en && (dyn_info->index > 0)) {
        index = dyn_info->index;
    }
    pt_index_count = cwme_p->pt_index_count;
    if (cwme_p->pt_index_min) {
        SHR_IF_ERR_EXIT
            (index < cwme_p->pt_index_min ? SHR_E_PARAM : SHR_E_NONE);
        index -= cwme_p->pt_index_min; /* ptcache uses 0-based index */
    }
    SHR_IF_ERR_EXIT((index >= pt_index_count) ? SHR_E_INTERNAL : SHR_E_NONE);

    for (tbl_inst = tbl_inst_min; tbl_inst <= tbl_inst_max; tbl_inst++) {
        if (tbl_inst) {
            entry_num = (tbl_inst * pt_index_count) + index;
        } else {
            entry_num = index;
        }
        vinfo_tc_index = cwme_p->tcs_base + (entry_num / 32);
        *prev_tc_rdata = BCMPTM_WORD_BMAP_TST(*(vinfo_bp + vinfo_tc_index),
                                              entry_num);
        switch (tc_cmd) {
            case BCMPTM_TCAM_CORRUPT_CMD_SET:
                BCMPTM_WORD_BMAP_SET(*(vinfo_bp + vinfo_tc_index), entry_num);
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "TCAM_Corrupt bit SET...for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "vinfo_tc_index=%0d, previous value = "
                                "%0d\n"),
                     sid, tbl_inst, index, vinfo_tc_index,
                     (*prev_tc_rdata ? 1 : 0)));
                break;
            case BCMPTM_TCAM_CORRUPT_CMD_CLR:
                BCMPTM_WORD_BMAP_CLR(*(vinfo_bp + vinfo_tc_index), entry_num);
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "TCAM_Corrupt bit CLR...for "
                                "sid=%0d, tbl_inst=%0d, index=%0d, "
                                "vinfo_tc_index=%0d, previous value = "
                                "%0d\n"),
                     sid, tbl_inst, index, vinfo_tc_index,
                     (*prev_tc_rdata ? 1 : 0)));
                break;
            default: /* GET */
                break;
        }
    } /* for each tbl_inst */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_iw_base(int u,
                       bcmdrd_sid_t sid,
                       uint32_t **iw_base_ptr)
{
    bcmptm_ptcache_sinfo_t *sinfo_p;
    bcmptm_ptcache_xinfo_t *xinfo;
    bcmptm_ptcache_cwme_t *cwme_p;
    uint32_t *vinfo_bp;
    pt_group_ptrs_t *pt_group_ptrs_p;
    int s = 0; 
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);

    /* Read sinfo */
    sinfo_p = sinfo_bp[u][s] + sid;
    if ((sinfo_p->ptcache_type != PTCACHE_TYPE_ME) &&
        (sinfo_p->ptcache_type != PTCACHE_TYPE_ME_CCI)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL); /* info_seg is not avail */
    }

    /* Read xinfo */
    cwme_p = cwme_bp[u][s] + sinfo_p->cw_index;
    xinfo = &cwme_p->xinfo;
    pt_group_ptrs_p = pt_group_ptrs_bp[u][s] + xinfo->pt_group;
    vinfo_bp = pt_group_ptrs_p->vinfo_bp;

    if (xinfo->info_seg_en) {
        *iw_base_ptr = vinfo_bp + cwme_p->is_base;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "iw_base: sid=%0d, pt=%s, iw_base_ptr=%p\n"),
             sid, bcmdrd_pt_sid_to_name(u, sid), (void *)*iw_base_ptr));
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL); /* info_seg is not avail */
    }
exit:
    SHR_FUNC_EXIT();
}

/* Called by ISSU:
 * - after init phase, but
 * - before pre_config phase.
 */
int
bcmptm_ptcache_ltid_change(int u)
{
    uint32_t pt_group_count = 0, pt_group, w, ltid_seg_word_count;
    uint32_t req_size, alloc_size, *alloc_ptr,ref_sinfo_sign, ref_ltid_seg_sign;
    uint32_t old_ltid, new_ltid, ptcache_flags_word = 0;
    void *ltid_seg_base_ptr;
    const bcmdrd_sid_t *pt_list;
    size_t sid_count = 0;
    bcmptm_ptcache_sinfo_t *sinfo_alloc_p, *sinfo_bp_us;
    bcmptm_ptcache_pt_group_info_t *pt_group_info_p;
    bool warm = TRUE, /* ltid_change function is called only during warm-issu */
         old_ltid_size16b;
    int tmp_rv;
    int s = 0; 
    char alloc_str_id[16];
    SHR_FUNC_ENTER(u);

    /* Evaluate sinfo_alloc_p (to be able to access old_ltid_size16b) */
    SHR_IF_ERR_EXIT(bcmptm_pt_sid_count_get(u, &sid_count));
    SHR_IF_ERR_EXIT(ptcache_ltid_size_set(u));
    if (ltid_size16b[u]) {
        ptcache_flags_word |= PTCACHE_FLAGS_LTID_SIZE16;
    }
    ref_sinfo_sign = BCMPTM_HA_SIGN_PTCACHE_SINFO + sid_count;
    req_size = SINFO_NUM_RSVD_WORDS * sizeof(bcmptm_ptcache_sinfo_t);
    req_size += sid_count * sizeof(bcmptm_ptcache_sinfo_t);
    alloc_size = req_size;
    sinfo_alloc_p = shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                     BCMPTM_HA_SUBID_PTCACHE_SINFO,
                                     "ptmCacheSinfo",
                                     &alloc_size);
    SHR_NULL_CHECK(sinfo_alloc_p, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ?  SHR_E_MEMORY : SHR_E_NONE);
    sinfo_bp[u][s] = sinfo_alloc_p + SINFO_NUM_RSVD_WORDS;
    sinfo_bp[u][SER] = sinfo_bp[u][s];
    sinfo_bp_us = sinfo_bp[u][s];
    SHR_IF_ERR_EXIT(
        sinfo_warm_check(u, ref_sinfo_sign,
                         ptcache_flags_word, sid_count));
    old_ltid_size16b = (sinfo_bp_us + SINFO_RSVD_INDEX_FLAGS_WORD)->cw_index;
    old_ltid_size16b &= PTCACHE_FLAGS_LTID_SIZE16;

    /* Evaluate pt_group_info_bp (to be able to access pt_group_info) */
    tmp_rv = bcmptm_pt_group_info_get(u, BCMPTM_PT_GROUP_DEFAULT,
                                      &pt_group_count, NULL, &pt_list);
    SHR_IF_ERR_MSG_EXIT(
        tmp_rv,
        (BSL_META_U(u, "pt_group_info_get returned rv = %0d, "
                    "pt_group_count = %0d\n"),
         tmp_rv, pt_group_count));
    req_size = pt_group_count * sizeof(bcmptm_ptcache_pt_group_info_t);
    SHR_IF_ERR_EXIT(
        bcmptm_do_ha_alloc(u, warm, req_size,
                           "ptmCachePtGroup",
                           BCMPTM_HA_SUBID_PTCACHE_PT_GROUP_INFO,
                           BCMPTM_HA_SIGN_PTCACHE_PT_GROUP_INFO + sid_count,
                           &alloc_ptr));
    pt_group_info_bp[u][s] =
        (bcmptm_ptcache_pt_group_info_t *)(alloc_ptr + 1);
    pt_group_info_bp[u][SER] = pt_group_info_bp[u][s];

    for (pt_group = 0; pt_group < pt_group_count; pt_group++) {
        pt_group_info_p = pt_group_info_bp[u][s] + pt_group;
        ltid_seg_word_count = pt_group_info_p->ltid_seg_word_count;
        if (ltid_seg_word_count == 0) {
            continue;
        }
        ref_ltid_seg_sign = BCMPTM_HA_SIGN_PTCACHE_LTID_SEG + sid_count +
                            pt_group;
        if (old_ltid_size16b) {
            req_size = (ltid_seg_word_count) * sizeof(uint16_t);
        } else {
            req_size = (ltid_seg_word_count) * sizeof(uint32_t);
        }
        /* Note: req_size must not include sign_word. It will be added by
         *       bcmptm_do_ha_alloc() below. */
        (void)sal_snprintf(alloc_str_id, 16, "%s%03d",
                           "ptmCacheLtid", pt_group);
        SHR_IF_ERR_EXIT(
            bcmptm_do_ha_alloc(u, warm, req_size,
                               alloc_str_id,
                               BCMPTM_HA_SUBID_PTCACHE_LTID_SEG + pt_group,
                               ref_ltid_seg_sign, &alloc_ptr));
        ltid_seg_base_ptr = alloc_ptr + 1; /* skip_sign_word
                                            * Above points to old ltid_seg. */

        if (old_ltid_size16b == ltid_size16b[u]) { /* No size change */
            for (w = 0; w < ltid_seg_word_count; w++) {
                if (old_ltid_size16b) {
                    uint16_t *ltid_p = (uint16_t *)ltid_seg_base_ptr;
                    old_ltid = *(ltid_p + w);
                    if (old_ltid == BCMPTM_LTID_SENTINEL16 ||
                        old_ltid == BCMPTM_LTID_IREQ16) {
                        new_ltid = old_ltid;
                    } else {
                        SHR_IF_ERR_EXIT(
                            ptcache_ltid_swap(u, old_ltid_size16b, old_ltid,
                                              &new_ltid));
                    }
                    /* Write LTID word */
                    *(ltid_p + w) = new_ltid;
                } else { /* 32b ltid */
                    uint32_t *ltid_p = (uint32_t *)ltid_seg_base_ptr;
                    old_ltid = *(ltid_p + w);
                    if (old_ltid == BCMPTM_LTID_SENTINEL ||
                        old_ltid == BCMPTM_LTID_IREQ) {
                        new_ltid = old_ltid;
                    } else {
                        SHR_IF_ERR_EXIT(
                            ptcache_ltid_swap(u, old_ltid_size16b, old_ltid,
                                              &new_ltid));
                    }
                    /* Write LTID word */
                    *(ltid_p + w) = new_ltid;
                } /* 32b ltid */
            } /* foreach ltid word in ltid_seg */

        } else { /* change in ltid_size16b */
            SHR_IF_ERR_EXIT(
                ptcache_ltid_copy_remap(u, old_ltid_size16b,
                                        ref_ltid_seg_sign, ltid_seg_word_count,
                                        ltid_seg_base_ptr, alloc_ptr));
        } /* change in ltid_size16b */

    } /* foreach PT group */
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ptcache_check(int u, bcmdrd_sid_t sid, bool *enabled)
{
    bcmptm_ptcache_sinfo_t *sinfo_p;
    int s = 0; 
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(enabled, SHR_E_PARAM);
    SHR_IF_ERR_EXIT_VERBOSE_IF(ptcache_sid_chk(u, s, sid), SHR_E_UNAVAIL);
    sinfo_p = sinfo_bp[u][s] + sid;
    switch (sinfo_p->ptcache_type) {
        case PTCACHE_TYPE_UNINIT:
            *enabled = FALSE;
            return SHR_E_INTERNAL;

        case PTCACHE_TYPE_ME:
        case PTCACHE_TYPE_ME_CCI:
        case PTCACHE_TYPE_SE_NOT_VALID:
        case PTCACHE_TYPE_SE_VALID:
        case PTCACHE_TYPE_SE_CACHE0:
            *enabled = TRUE;
            break;

        default:
            /* PTCACHE_TYPE_NO_CACHE,
             * PTCACHE_TYPE_CCI_ONLY,
             * PTCACHE_TYPE_ME_RSVD.
             *
             * Note: If device initialization wants to write non-zero
             * value to PT for which PTCACHE_TYPE_ME_RSVD,
             * bcmptm_ptcache_update() will return SHR_E_UNAVAIL, so returning
             * enabled = FALSE is correct choice.
             */
            *enabled = FALSE;
            break;
    }
exit:
    SHR_FUNC_EXIT();
}
