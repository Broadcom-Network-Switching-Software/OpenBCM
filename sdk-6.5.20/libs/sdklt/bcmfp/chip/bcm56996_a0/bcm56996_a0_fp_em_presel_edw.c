/*! \file bcm56996_a0_fp_em_presel_edw.c
 *
 * EM action configuration DB initialization
 * function for Tomahawk-4GG(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56996_a0_fp_em_presel_edw_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_group_oper_info_t *opinfo,
                                 uint32_t **edw)
{
    uint32_t fv = 0;
    uint32_t ebuff[16];
    uint8_t part = 0;
    uint8_t num_parts = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, TRUE, opinfo->flags, &num_parts));

    for (part = 0; part < num_parts; part++) {
        ext_codes = &(opinfo->ext_codes[part]);
        sal_memset(ebuff, 0, sizeof(ebuff));

        /* ALT_TTL_FN_SEL Select */
        if (ext_codes->alt_ttl_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->alt_ttl_fn_sel;
            bcmdrd_field_set(ebuff, 0, 0, &fv);
        }

        /* AUX_TAG_A Select */
        if (ext_codes->aux_tag_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->aux_tag_a_sel;
            bcmdrd_field_set(ebuff, 1, 4, &fv);
        }

        /* AUX_TAG_B Select */
        if (ext_codes->aux_tag_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->aux_tag_b_sel;
            bcmdrd_field_set(ebuff, 5, 8, &fv);
        }

        /* CLASS_ID_CONTAINER_A_SEL Select */
        if (ext_codes->class_id_cont_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->class_id_cont_a_sel;
            bcmdrd_field_set(ebuff, 9, 12, &fv);
        }

        /* CLASS_ID_CONTAINER_B_SEL Select */
        if (ext_codes->class_id_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->class_id_cont_b_sel;
            bcmdrd_field_set(ebuff, 13, 16, &fv);
        }

        /* DEFAULT Policy enable */
        if (ext_codes->default_policy_enable != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->default_policy_enable;
            bcmdrd_field_set(ebuff, 17, 17, &fv);
        }

        /* DST_CONTAINER_A_SEL Select */
        if (ext_codes->dst_cont_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->dst_cont_a_sel;
            bcmdrd_field_set(ebuff, 18, 20, &fv);
        }

        /* DST_CONTAINER_B_SEL Select */
        if (ext_codes->dst_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->dst_cont_b_sel;
            bcmdrd_field_set(ebuff, 21, 23, &fv);
        }

        /* Lookup enable. */
        fv = opinfo->enable;
        bcmdrd_field_set(ebuff, 24, 24, &fv);

        /* keygen program profile index */
        if (ext_codes->keygen_index != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->keygen_index;
            bcmdrd_field_set(ebuff, 25, 28, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* KEY_TYPE */
        if (opinfo->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
            ext_codes[part].key_type = 1;
        } else if (opinfo->group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
            ext_codes[part].key_type = 0;
        } else if (opinfo->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            ext_codes[part].key_type = 2;
        }
        fv = ext_codes[part].key_type;
        bcmdrd_field_set(ebuff, 29, 30, &fv);

        /* LTID must be present. */
        if (ext_codes->ltid != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ltid;
            bcmdrd_field_set(ebuff, 33, 36, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* SRC_CONTAINER_A_SEL Select */
        if (ext_codes->src_cont_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->src_cont_a_sel;
            bcmdrd_field_set(ebuff, 39, 40, &fv);
        }

        /* SRC_CONTAINER_B_SEL Select */
        if (ext_codes->src_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->src_cont_b_sel;
            bcmdrd_field_set(ebuff, 41, 42, &fv);
        }

        /* TCP_FN_SEL Select */
        if (ext_codes->tcp_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->tcp_fn_sel;
            bcmdrd_field_set(ebuff, 43, 44, &fv);
        }

        /* TOS_FN_SEL Select */
        if (ext_codes->tos_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->tos_fn_sel;
            bcmdrd_field_set(ebuff, 45, 46, &fv);
        }

        /* TTL_FN_SEL Select */
        if (ext_codes->ttl_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ttl_fn_sel;
            bcmdrd_field_set(ebuff, 47, 48, &fv);
        }

        /* UDF_SEL Select */
        if (ext_codes->udf_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->udf_sel;
            bcmdrd_field_set(ebuff, 49, 49, &fv);
        }

        /* Copy 55 bits from ebuff to edw[part][0] */
        bcmdrd_field_set(&(edw[part][0]), 178, 233, ebuff);
    }

exit:
    SHR_FUNC_EXIT();
}

