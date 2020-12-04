/*! \file bcmptm_ptcache_tcam.c
 *
 * Tcam related utilities for PTM, ptcache
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC
#define KMF_INFO_WORD_COUNT 3


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Find minbit, width for user-specified (key, mask) fields
 * \n Note: Uses string based search - so not recommended for run-time.
 *
 * \param [in] u Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] keyf Name of key field
 * \param [in] maskf Name of corresponding mask field
 *
 * \param [out] info_words field info words for key, mask pair
 *
 * \retval SHR_E_NONE Success. User-specified keyf, maskf exist for this sid
 * \retval !SHR_E_NONE Not Found. User-specified keyf, maskf do not exist for
 * this sid.
 *
 * \Note
 * Init 3 words for every key,mask field pair:
 *      word0 - minbit, maxbit for key
 *      word1 - minbit, maxbit for mask
 *      word2 - bit_count, word_count (same) for key, mask field
 */
static int
tcam_km_fi_get(int u,
               bcmdrd_sid_t sid,
               const char *keyf,
               const char *maskf,
               uint32_t *info_words)
{
    int minbit, maxbit, tmp_rv;
    uint32_t kwidth, mwidth;
    bcmdrd_fid_t kfid, mfid;
    bcmptm_tcam_iw_fi_t tcam_info_word;
    SHR_FUNC_ENTER(u);

    /* Searching of field by names MUST NOT BE DONE DURING RUNTIME
     * Must execute this once during init.
     */
    tmp_rv = bcmdrd_pt_field_name_to_fid(u, sid, keyf, &kfid);
    if (SHR_FAILURE(tmp_rv)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    tmp_rv = bcmdrd_pt_field_name_to_fid(u, sid, maskf, &mfid);
    if (SHR_FAILURE(tmp_rv)) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    /* Must not use SHR_IF_ERR_EXIT() above - because that will also display
     * ERRor message - and this is not really error because if not all tcams
     * have all keyf, maskf named fields
     */

    /* info_word_key */
    minbit = bcmdrd_pt_field_minbit(u, sid, kfid);
    maxbit = bcmdrd_pt_field_maxbit(u, sid, kfid);
    SHR_IF_ERR_EXIT((minbit < 0) ? SHR_E_FAIL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((minbit > BCMPTM_TCAM_IW_LIMIT_MINBIT) ? SHR_E_FAIL
                                                           : SHR_E_NONE);
    SHR_IF_ERR_EXIT((maxbit < minbit) ? SHR_E_FAIL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((maxbit > BCMPTM_TCAM_IW_LIMIT_MAXBIT) ? SHR_E_FAIL
                                                           : SHR_E_NONE);
    kwidth = maxbit - minbit + 1;
    SHR_IF_ERR_EXIT((kwidth > BCMPTM_TCAM_IW_LIMIT_WIDTH_BC) ? SHR_E_FAIL
                                                             : SHR_E_NONE);
    tcam_info_word.entry = 0;
    tcam_info_word.fi01.minbit = minbit;
    tcam_info_word.fi01.maxbit = maxbit;
    *(info_words + 0) = tcam_info_word.entry;   /* info about key field */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "For sid=%0d, found %s, kmaxbit=%0u, kminbit=%0u\n"),
         sid, keyf, maxbit, minbit));


    /* info_word_mask */
    minbit = bcmdrd_pt_field_minbit(u, sid, mfid);
    maxbit = bcmdrd_pt_field_maxbit(u, sid, mfid);
    SHR_IF_ERR_EXIT((minbit < 0) ? SHR_E_FAIL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((minbit > BCMPTM_TCAM_IW_LIMIT_MINBIT) ? SHR_E_FAIL
                                                           : SHR_E_NONE);
    SHR_IF_ERR_EXIT((maxbit < minbit) ? SHR_E_FAIL : SHR_E_NONE);
    mwidth = maxbit - minbit + 1;
    SHR_IF_ERR_EXIT((mwidth > BCMPTM_TCAM_IW_LIMIT_WIDTH_BC) ? SHR_E_FAIL
                                                             : SHR_E_NONE);
    tcam_info_word.entry = 0;
    tcam_info_word.fi01.minbit = minbit;
    tcam_info_word.fi01.maxbit = maxbit;
    *(info_words + 1) = tcam_info_word.entry;   /* info about mask field */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "For sid=%0d, found %s, mmaxbit=%0u, mminbit=%0u\n"),
         sid, maskf, maxbit, minbit));

    /* info_word_common */
    SHR_IF_ERR_EXIT((kwidth != mwidth) ? SHR_E_FAIL : SHR_E_NONE);
    tcam_info_word.entry = 0;
    tcam_info_word.fi2.width_bit_count = kwidth;
    tcam_info_word.fi2.width_word_count = BCMPTM_BITS2WORDS(kwidth);
    *(info_words + 2) = tcam_info_word.entry;   /* info common to both fields */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find and fill minbit, width for ALL key, mask fields for TCAM type PT
 * \n Note: Goes through a pre-defined list of popular KEY, MASK field names.
 * For every new device, we must ensure that this list gets updated.
 *
 * \param [in] u Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] fill_mode 0 => only count, 1 => fill and count
 * \param [in] iw_ptr Pointer into info_seg where fill words will be written
 *
 * \param [out] kmf_count number of key,mask field pairs
 *
 * \retval SHR_E_NONE Success. (word_count is guaranteed to be > 0)
 * \retval !SHR_E_NONE word_count may not be written
 */
static int
tcam_km_fields_set(int u,
                   bcmdrd_sid_t sid,
                   bool fill_mode,
                   uint32_t *iw_ptr,
                   uint32_t *kmf_count)
{
    int tmp_rv;
    int pri_found;
    uint32_t i, j, info_words[KMF_INFO_WORD_COUNT], tmp_word_count;

    static const struct {
        char *keyf;
        char *maskf;
        int pri;
    } kmf_list[] = {
        /* Priority 0 - total field overlays */
        {"FULL_KEY", "FULL_MASK", 0},
        {"KEYS", "MASKS", 0},


        /* Priority 1 - 2nd level field overlays */
        {"LWR_KEYS", "LWR_MASKS", 1},
        {"UPR_KEYS", "UPR_MASKS", 1},


        /* Category 2 - small fields */
        {"KEY0", "MASK0", 2},
        {"KEY1", "MASK1", 2},
        /* {"KEY2", "MASK2", 2}, */
        /* {"KEY3", "MASK3", 2}, */

        {"KEY0_UPR", "MASK0_UPR", 2},
        {"KEY1_UPR", "MASK1_UPR", 2},
        /* {"KEY2_UPR", "MASK2_UPR", 2}, */
        /* {"KEY3_UPR", "MASK3_UPR", 2}, */

        {"KEY0_LWR", "MASK0_LWR", 2},
        {"KEY1_LWR", "MASK1_LWR", 2}
        /* {"KEY2_LWR", "MASK2_LWR", 2}, */
        /* {"KEY3_LWR", "MASK3_LWR", 2}, */
    };
    SHR_FUNC_ENTER(u);

    j = 0; /* count (key, mask) pairs found */
    pri_found = 99; /* very low priority found */
    for (i = 0; i < COUNTOF(kmf_list); i++) {
        tmp_rv = tcam_km_fi_get(u, sid,
                                kmf_list[i].keyf, kmf_list[i].maskf,
                                &info_words[0]);

        if (tmp_rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(tmp_rv); /* any other failure - problem */

        if (kmf_list[i].pri <= pri_found) {
            if (fill_mode) {
                *iw_ptr++ = info_words[0]; /* Key field info */
                *iw_ptr++ = info_words[1]; /* Mask field info */
                *iw_ptr++ = info_words[2]; /* Common info */
            }
            j++;
            pri_found = kmf_list[i].pri;
        } else {
            break; /* use highest level overlays */

        }
    }
    if (j == 0) { /* if non-std fields not found, check for std fields */
        SHR_IF_ERR_EXIT(
            tcam_km_fi_get(u, sid, "KEY", "MASK", &info_words[0]));
            /* SHR_E_NOT_FOUND here means no key, mask fields found
             * for this tcam - error condition !! */

        if (fill_mode) {
            *iw_ptr++ = info_words[0]; /* Key field info */
            *iw_ptr++ = info_words[1]; /* Mask field info */
            *iw_ptr++ = info_words[2]; /* Common info */
        }
        j++;
    }

    /* j is count of (key, mask) pairs */
    SHR_IF_ERR_EXIT((!j || (j > BCMPTM_TCAM_IW_LIMIT_KMF_COUNT)) ?
                    SHR_E_INTERNAL : SHR_E_NONE);

    tmp_word_count = j*(KMF_INFO_WORD_COUNT);
    SHR_IF_ERR_EXIT((tmp_word_count > BCMPTM_TCAM_IW_LIMIT_WORD_COUNT) ?
                    SHR_E_INTERNAL : SHR_E_NONE);

    *kmf_count = j;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert tcam (key, mask) word to (x,y) format word as needed by tcam
 * \n Note: Meaning of km format:
 * \n       mask bit value of 1 means compare against the key
 * \n       mask bit value of 0 means dont_compare against the key
 *
 * \param [in] u Logical device id
 * \param [in] tcam_km_format Format of key, mask required by TCAM
 * \param [in] keyp Ptr to key word (will be replaced by x)
 * \param [in] maskp Ptr to mask word (will be replaced by y)
 */
static int
tcam_word_km_to_xy(int u,
                   bcmptm_pt_tcam_km_format_t tcam_km_format,
                   uint32_t *keyp, /* in, out */
                   uint32_t *maskp) /* in, out */
{
    uint32_t key, mask;
    uint32_t k0, k1, x, y;
    /* SHR_FUNC_ENTER(u); */

    key = *keyp;
    mask = *maskp;

    /*
     * Notes:
     *     - In KM format, mask bit value of 1 means to compare against key
     *     - For writes (K0, K1) need to be written to TCAM
     */
    switch(tcam_km_format) {
        case BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = MASK
             *
             *    Decode: KEY = K0 (with info loss)
             *            MASK = K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   0     0   0
             *     1   0     0   0     0   0  =====> info loss
             *     0   1     0   1     0   1
             *     1   1     1   1     1   1
             */
            k0 = mask & key;
            k1 = mask;
            break;

        case BCMPTM_TCAM_KM_FORMAT_40NM_XY:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = ~MASK | KEY
             *
             *    Decode: KEY = K0
             *            MASK = K0 | ~K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   1     0   0
             *     1   0     0   1     0   0  =====> info loss
             *     0   1     0   0     0   1
             *     1   1     1   1     1   1
             */
            k0 = mask & key;
            k1 = ~mask | key;
            break;

        case BCMPTM_TCAM_KM_FORMAT_X_YBAR:
            /*
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            k0 = mask & key;
            k1 = mask & ~key;
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT:
            /* Step1: Same as X_YBAR
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            x = mask & key;  /* k0 */
            y = mask & ~key; /* k1 */

            /* Step2: Convert XY -> LPT
             *
             * lpt_x[1] = !D[1] & M[1] | !D[0] & M[0] = y[1] | y[0];
             * lpt_x[0] =  D[1] & M[1] | !D[0] & M[0] = x[1] | y[0];
             *
             * lpt_y[1] = !D[1] & M[1] |  D[0] & M[0] = y[1] | x[0];
             * lpt_y[0] =  D[1] & M[1] |  D[0] & M[0] = x[1] | x[0];
             */
            k0 = ((y | (y << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | y) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            k1 = ((y | (x << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | x) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Write: k=0x%8x, m=0x%8x, lptx=0x%8x, lpty=0x%8x\n"),
                 key, mask, k0, k1));
             */
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT_7NM:
            /* Step 1 */
            x = mask & key;
            y = mask & ~key;

            /* Step 2: Convert XY -> LPT
             *
             * lpt_x[1] =  D[1] & M[1] | !D[0] & M[0] = x[1] | y[0];
             * lpt_x[0] =  D[1] & M[1] |  D[0] & M[0] = x[1] | x[0];
             *
             * lpt_y[1] = !D[1] & M[1] |  D[0] & M[0] = y[1] | x[0];
             * lpt_y[0] = !D[1] & M[1] | !D[0] & M[0] = y[1] | y[0];
             */
            k0 = ((x | (y << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((x >> 1) | x) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */

            k1 = ((y | (x << 1)) & 0xaaaaaaaa) | /* bits 31, 29, ..., 3, 1 */
                 (((y >> 1) | y) & 0x55555555);  /* bits 30, 28, ..., 2, 0 */
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Write: k=0x%8x, m=0x%8x, x=0x%8x, y=0x%8x\n"),
                 key, mask, k0, k1));
             */
            break;

        default: /*  BCMPTM_TCAM_KM_FORMAT_IS_KM - no conversion */
            k0 = key;
            k1 = mask;
            break;
    } /* tcam_km_format */

    *keyp = k0;
    *maskp = k1;

/* exit: */
    /* SHR_FUNC_EXIT(); */
    return SHR_E_NONE;
}

/*!
 * \brief Convert tcam (k0, k1) word to (key, mask) format word
 * \n Note: Meaning of km format:
 * \n       mask bit value of 1 means compare against the key
 * \n       mask bit value of 0 means dont_compare against the key
 *
 * \param [in] u Logical device id
 * \param [in] tcam_km_format Format of key, mask required by TCAM
 * \param [in] k0p Ptr to k0 word (will be replaced by key)
 * \param [in] k1p Ptr to k1 word (will be replaced by mask)
 */
static int
tcam_word_xy_to_km(int u,
                   bcmptm_pt_tcam_km_format_t tcam_km_format,
                   uint32_t *k0p, /* in, out */
                   uint32_t *k1p) /* in, out */
{
    uint32_t k0, k1, x, y, xbar, ybar;
    /* uint32_t old_k0, old_k1; */
    uint32_t key, mask;
    /* SHR_FUNC_ENTER(u); */

    k0 = *k0p;
    k1 = *k1p;

    /*
     * Notes:
     *     - In KM format, mask bit value of 1 means to compare against key
     *     - For reads (K0, K1) will be returned from TCAM
     */
    switch(tcam_km_format) {
        case BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = MASK
             *
             *    Decode: KEY = K0 (with info loss)
             *            MASK = K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   0     0   0
             *     1   0     0   0     0   0  =====> info loss
             *     0   1     0   1     0   1
             *     1   1     1   1     1   1
             */
            key = k0; /* with info loss */
            mask = k1;
            break;

        case BCMPTM_TCAM_KM_FORMAT_40NM_XY:
            /*
             *    Encode: K0 = MASK & KEY
             *            K1 = ~MASK | KEY
             *
             *    Decode: KEY = K0
             *            MASK = K0 | ~K1
             *
             *              (encode)  (decode)
             *    KEY MASK   K0  K1   KEY MASK
             *    --------   ------   --------
             *     0   0     0   1     0   0
             *     1   0     0   1     0   0  =====> info loss
             *     0   1     0   0     0   1
             *     1   1     1   1     1   1
             */
            key = k0; /* with info loss */
            mask = k0 | ~k1;
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT_7NM:
            /*
             * D[1] = (!LPT_Y[1] | !LPT_Y[0]) &
             *        LPT_X[0] &
             *        LPT_X[1]; -- OVERLAPPING STATES
             *
             * D[0] = LPT_Y[1] &
             *        (!LPT_X[1] | !LPT_Y[0]) &
             *        LPT_X[0]; -- OVERLAPPING STATES
             *
             * M[1] = (!LPT_Y[1] & LPT_X[1] & LPT_X[0]) |
             *        (LPT_X[1] & !LPT_Y[0] & LPT_X[0]) |
             *        (LPT_Y[1] & LPT_Y[0] & !LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_X[1] & LPT_Y[0]);
             *
             * M[0] = (!LPT_Y[1] & LPT_X[1] & LPT_Y[0]) |
             *        (LPT_X[1] & LPT_Y[0] & !LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_Y[0] & LPT_X[0]) |
             *        (LPT_Y[1] & !LPT_X[1] & LPT_X[0]);
             */
            x = k0;
            y = k1;
            xbar = ~x;
            ybar = ~y;

            /* Following can be made more efficient - but not needed because
             * xy -> km conversion is done only for debug read from HW.
             */
            key = (0xaaaaaaaa & ((ybar | (ybar << 1)) &
                                 (x << 1)             &
                                 x
                                )
                  ) |
                  (0x55555555 & ((y >> 1)             &
                                 ((xbar >> 1) | ybar) &
                                 x
                                )
                  );
            mask = (0xaaaaaaaa & ((ybar & x           & (x << 1))    |
                                  (x    & (ybar << 1) & (x << 1))    |
                                  (y    & (y << 1)    & (xbar << 1)) |
                                  (y    & xbar        & (y << 1))
                                 )
                   ) |
                   (0x55555555 & (((ybar >> 1) & (x >> 1)    & y)    |
                                  ((x >> 1)    & y           & xbar) |
                                  ((y >> 1)    & ybar        & x)    |
                                  ((y >> 1)    & (xbar >> 1) & x)
                                 )
                   );
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Read: x=0x%8x, y=0x%8x, k=0x%8x, m=0x%8x\n"),
                 x, y, key, mask));
            */
            break;

        case BCMPTM_TCAM_KM_FORMAT_LPT:
            /*
             * D[1] = LPT_Y[0] & LPT_X[0]
             * D[0] = LPT_Y[1] & LPT_Y[0]
             *
             * M[1] = LPT_Y[1] & LPT_X[1] | LPT_Y[0] & LPT_X[0];
             * M[0] = LPT_X[1] & LPT_X[0] | LPT_Y[1] & LPT_Y[0];
             *
             * M[1]= LPT_Y[1] & LPT_X[1] | D[1];
             * M[0]= LPT_X[1] & LPT_X[0] | D[0];
             */

            /* Step1: Convert LPT -> X_YBAR
             *
             * xybar_x[1:0] = {(lpt_y[0] & lpt_x[0]), (lpt_y[1] & lpt_y[0])}
             * xybar_y[1:0] = {(lpt_y[1] & lpt_x[1]), (lpt_x[1] & lpt_x[0])}
             */
            x = (((k1 & k0) << 1) & 0xaaaaaaaa) | /* D[1] */
                (((k1 >> 1) & k1) & 0x55555555);  /* D[0] */

            y = ((k1 & k0) & 0xaaaaaaaa) |       /* 1st term of M[1] */
                (((k0 >> 1) & k0) & 0x55555555); /* 1st term of M[0] */

            /* Step2: X_YBAR -> key, mask*/
            /* old_k0 = k0; */
            /* old_k1 = k1; */
            k0 = x;
            k1 = y;
            /* break; goto next case item */

        case BCMPTM_TCAM_KM_FORMAT_X_YBAR:
            /*
             *     Encode: K0 = MASK & KEY
             *             K1 = MASK & ~KEY
             *
             *     Decode: KEY = K0
             *             MASK = K0 | K1
             *
             *     KEY MASK   K0  K1   KEY MASK
             *     --------   ------   --------
             *      0   0     0   0     0   0
             *      1   0     0   0     0   0  =====> info loss
             *      0   1     0   1     0   1
             *      1   1     1   0     1   1
             */
            key = k0; /* with info loss */
            mask = k0 | k1;
            /*
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Read: lptx=0x%8x, lpty=0x%8x, key=0x%8x, mask=0x%8x\n"),
                 old_k0, old_k1, key, mask));
             */
            break;

        default: /*  BCMPTM_TCAM_KM_FORMAT_IS_KM - no conversion */
            key = k0;
            mask = k1;
            break;
    } /* tcam_km_format */

    *k0p = key;
    *k1p = mask;

/* exit: */
    /* SHR_FUNC_EXIT(); */
    return SHR_E_NONE;
}


/*******************************************************************************
 * Public Functions
 */
/* Change format for (key, mask) fields for one or multiple entries
 * User can specify to replace dm format with xy format by specifying
 * xy_entry = km_entry
 *
 * This func must take into account tcam type, chip type,
 * figure out where tcam_data, tcam_mask are in entry_words
 * and ensure that it does not modify assoc_data portion for case
 * when sid is aggr of tcam+assoc_dat
 *
 * For newer chips, SW does not need to do format change during
 * writes - it will by done inside chip by HW logic.
 */
int
bcmptm_pt_tcam_km_to_xy(int u,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *km_entry,
                        uint32_t *xy_entry)
{
    uint32_t key[BCMPTM_MAX_PT_FIELD_WORDS], mask[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t field_count, entry_index, i, word, entry_word_count,
             entry_byte_count;
    bcmptm_pt_tcam_km_format_t tcam_km_format;
    bcmptm_tcam_iw_fi_t info_word;
    uint32_t *iw_base_ptr, *iw0_base_ptr;
    bool hw_does_km_to_xy = FALSE;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(bcmptm_ptcache_iw_base(u, sid, &iw_base_ptr));
    info_word.entry = *iw_base_ptr++;
    SHR_IF_ERR_EXIT(!info_word.cw.kmf_count ? SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((info_word.cw.type != BCMPTM_TCAM_IW_TYPE_CW) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
    field_count = info_word.cw.kmf_count;

    SHR_IF_ERR_EXIT(bcmptm_hw_does_km_to_xy(u, &hw_does_km_to_xy));
    SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(u, sid, &tcam_km_format));
    entry_word_count = bcmdrd_pt_entry_wsize(u, sid);
    entry_byte_count = entry_word_count * sizeof(uint32_t);

    iw0_base_ptr = iw_base_ptr; /* points to info_word[0] for 1st kmf field */
    for (entry_index = 0; entry_index < entry_count; entry_index++) {
        iw_base_ptr = iw0_base_ptr;

        if (xy_entry != km_entry) {
            /* User can ask us to do replace by specifying same ptr for
             * km_entry, xy_entry */
            sal_memcpy(xy_entry, km_entry, entry_byte_count);
        }
        if (tcam_km_format == BCMPTM_TCAM_KM_FORMAT_IS_KM ||
            hw_does_km_to_xy) {
            km_entry += entry_word_count;
            xy_entry += entry_word_count;
            continue; /* to next entry */
        }
        for (i = 0; i < field_count; i++) {
            uint32_t k_sbit, k_ebit, m_sbit, m_ebit;
            uint32_t field_bit_count, field_word_count;

            /* info for key field */
            info_word.entry = *iw_base_ptr++; /* word0 */
            k_sbit = info_word.fi01.minbit;
            k_ebit = info_word.fi01.maxbit;

            /* info for mask field */
            info_word.entry = *iw_base_ptr++; /* word1 */
            m_sbit = info_word.fi01.minbit;
            m_ebit = info_word.fi01.maxbit;

            /* common info */
            info_word.entry = *iw_base_ptr++; /* word2 */
            field_bit_count = info_word.fi2.width_bit_count;
            field_word_count = info_word.fi2.width_word_count;

            /* extract words of key, mask fields */
            bcmdrd_field_get(km_entry, k_sbit, k_ebit, key);
            bcmdrd_field_get(km_entry, m_sbit, m_ebit, mask);

            for (word = 0; word < field_word_count; word++) {
                SHR_IF_ERR_EXIT(
                    tcam_word_km_to_xy(u,
                                       tcam_km_format,
                                       &key[word],
                                       &mask[word]));
            }

            /* Zero out unused bits in last word of key, mask fields */
            if ((field_bit_count & 0x1f) != 0) {
                /* means valid bits in last word < 32 */
                key[word - 1]  &= (1 << (field_bit_count & 0x1f)) - 1;
                mask[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
            }
                /* Example: if field_bit_count = 5
                 * then (1 << 5) = 32 and so RHS will be 31 = 0x1f
                 * and thus we will be zeroing out all mask bits except least
                 * significant 5 bits */

                /* Basically, extra bits in last key, mask word should be 0
                 * and for 28nm they translate into (K0=0, k1=0), but for
                 * 40nm, they translate to (K0=0, K1=1) - so cleanup is needed
                 * in last mask word - because when generating K0, K1 we did not
                 * bother to check num of bits in last key, mask words
                 */

            /* Stuff converted_key, _mask fields into xy_entry */
            bcmdrd_field_set(xy_entry, k_sbit, k_ebit, key);
            bcmdrd_field_set(xy_entry, m_sbit, m_ebit, mask);
        } /* for each {key, mask} field pair in this entry */

        /* Point to next entry */
        km_entry += entry_word_count;
        xy_entry += entry_word_count;
    } /* for each entry */

exit:
    SHR_FUNC_EXIT();
}

/* Change format for (key, mask) fields for one or multiple entries
 * User can specify to replace dm format with xy format by specifying
 * km_entry = xy_entry
 *
 * This func must take into account tcam type, chip type,
 * figure out where tcam_data, tcam_mask are in entry_words
 * and ensure that it does not modify assoc_data portion for case
 * when sid is aggr of tcam+assoc_dat.
 */
int
bcmptm_pt_tcam_xy_to_km(int u,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *xy_entry,
                        uint32_t *km_entry)
{
    uint32_t k0[BCMPTM_MAX_PT_FIELD_WORDS], k1[BCMPTM_MAX_PT_FIELD_WORDS];
    uint32_t field_count, entry_index, i, word, entry_word_count,
             entry_byte_count;
    bcmptm_pt_tcam_km_format_t tcam_km_format;
    bcmptm_tcam_iw_fi_t info_word;
    uint32_t *iw_base_ptr, *iw0_base_ptr;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(bcmptm_ptcache_iw_base(u, sid, &iw_base_ptr));
    info_word.entry = *iw_base_ptr++;
    SHR_IF_ERR_EXIT(!info_word.cw.kmf_count ? SHR_E_INTERNAL : SHR_E_NONE);
    SHR_IF_ERR_EXIT((info_word.cw.type != BCMPTM_TCAM_IW_TYPE_CW) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
    field_count = info_word.cw.kmf_count;

    SHR_IF_ERR_EXIT(bcmptm_pt_tcam_km_format(u, sid, &tcam_km_format));
    entry_word_count = bcmdrd_pt_entry_wsize(u, sid);
    entry_byte_count = entry_word_count * sizeof(uint32_t);

    iw0_base_ptr = iw_base_ptr; /* points to info_word[0] for 1st kmf field */
    for (entry_index = 0; entry_index < entry_count; entry_index++) {
        iw_base_ptr = iw0_base_ptr;

        if (km_entry != xy_entry) {
            /* User can ask us to do replace by specifying same ptr for
             * km_entry, xy_entry */
            sal_memcpy(km_entry, xy_entry, entry_byte_count);
        }
        if (tcam_km_format == BCMPTM_TCAM_KM_FORMAT_IS_KM) {
            xy_entry += entry_word_count;
            km_entry += entry_word_count;
            continue; /* to next entry */
        }
        for (i = 0; i < field_count; i++) {
            uint32_t k_sbit, k_ebit, m_sbit, m_ebit;
            uint32_t field_bit_count, field_word_count;

            /* info for key field */
            info_word.entry = *iw_base_ptr++; /* word0 */
            k_sbit = info_word.fi01.minbit;
            k_ebit = info_word.fi01.maxbit;

            /* info for mask field */
            info_word.entry = *iw_base_ptr++; /* word1 */
            m_sbit = info_word.fi01.minbit;
            m_ebit = info_word.fi01.maxbit;

            /* common info */
            info_word.entry = *iw_base_ptr++; /* word2 */
            field_bit_count = info_word.fi2.width_bit_count;
            field_word_count = info_word.fi2.width_word_count;

            /* extract words of k0, k1 fields */
            bcmdrd_field_get(xy_entry, k_sbit, k_ebit, k0);
            bcmdrd_field_get(xy_entry, m_sbit, m_ebit, k1);

            for (word = 0; word < field_word_count; word++) {
                SHR_IF_ERR_EXIT(
                    tcam_word_xy_to_km(u,
                                       tcam_km_format,
                                       &k0[word],
                                       &k1[word]));
            }

            /* Zero out extra bits in last word of key, mask fields */
            if ((field_bit_count & 0x1f) != 0) {
                k0[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
                k1[word - 1] &= (1 << (field_bit_count & 0x1f)) - 1;
            }

            /* stuff converted_k0, _k1 fields into km_entry */
            bcmdrd_field_set(km_entry, k_sbit, k_ebit, k0);
            bcmdrd_field_set(km_entry, m_sbit, m_ebit, k1);
        } /* for each {k0, k1} field pair in this entry */

        /* Point to next entry */
        xy_entry += entry_word_count;
        km_entry += entry_word_count;
    } /* For each entry */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_iw_count(int u, bcmdrd_sid_t sid, uint32_t *iw_count)
{
    uint32_t kmf_count = 0;
    SHR_FUNC_ENTER(u);

    *iw_count = 0;

    if (bcmdrd_pt_attr_is_cam(u, sid)) {
        /* Dont optimize based on whether TCAM needs xy conversion or not */
        SHR_IF_ERR_EXIT(tcam_km_fields_set(u, sid,
                                           FALSE, /* only count */
                                           0,     /* fill index is dont_care */
                                           &kmf_count));
        *iw_count = (kmf_count * KMF_INFO_WORD_COUNT) + 1;
            /* +1 word in info_seg to hold the control_word (cw) */
    }
    /* else {
        info_words not needed - can be expanded for hash, etc
    } */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_iw_fill(int u,
                  bcmdrd_sid_t sid,
                  uint32_t *iw_base_ptr)
{
    bcmptm_tcam_iw_fi_t tcam_info_word;
    uint32_t kmf_count = 0;
    SHR_FUNC_ENTER(u);

    if (bcmdrd_pt_attr_is_cam(u, sid)) {
        SHR_IF_ERR_EXIT(
            tcam_km_fields_set(u, sid,
                               TRUE, /* fill_mode */
                               iw_base_ptr + 1,
                                   /* skip info_seg.word0 (used to store
                                    * ctl_word) */
                               &kmf_count));

        /* Add info_word of type CW to store word count */
        tcam_info_word.entry = 0;
        tcam_info_word.cw.type = BCMPTM_TCAM_IW_TYPE_CW;
        tcam_info_word.cw.kmf_count = kmf_count;
        tcam_info_word.cw.word_count = kmf_count * KMF_INFO_WORD_COUNT;
            /* Note: Only includes words needed to store fields.
             *       Does not include ctl_word itself */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "entry=0x%8x: type=%0d, kmf_count=%0d, "
                        "word_count=%0d\n"),
             tcam_info_word.entry, tcam_info_word.cw.type,
             tcam_info_word.cw.kmf_count, tcam_info_word.cw.word_count));

        *iw_base_ptr = tcam_info_word.entry;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            /* Currently, we only support storing field info for TCAMs.
             * ptcache should not be asking us to fill words for any other
             * memory */
    }

exit:
    SHR_FUNC_EXIT();
}
