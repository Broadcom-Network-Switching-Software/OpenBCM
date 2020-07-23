/*! \file bcm56990_a0_fp_ing_presel_edw.c
 *
 * IFP action configuration DB initialization
 * function for Tomahawk-4(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56990_a0_fp_ing_presel_edw_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  uint32_t **edw)
{
    uint32_t fv = 0;
    uint32_t isdw_enable = 1;
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
            bcmdrd_field_set(ebuff, 9, 11, &fv);
        }

        /* CLASS_ID_CONTAINER_B_SEL Select */
        if (ext_codes->class_id_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->class_id_cont_b_sel;
            bcmdrd_field_set(ebuff, 12, 14, &fv);
        }

        /* CLASS_ID_CONTAINER_C_SEL Select */
        if (ext_codes->class_id_cont_c_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->class_id_cont_c_sel;
            bcmdrd_field_set(ebuff, 15, 18, &fv);
        }

        /* CLASS_ID_CONTAINER_D_SEL Select */
        if (ext_codes->class_id_cont_d_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->class_id_cont_d_sel;
            bcmdrd_field_set(ebuff, 19, 22, &fv);
        }

        /* DST_CONTAINER_A_SEL Select */
        if (ext_codes->dst_cont_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->dst_cont_a_sel;
            bcmdrd_field_set(ebuff, 23, 25, &fv);
        }

        /* DST_CONTAINER_B_SEL Select */
        if (ext_codes->dst_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->dst_cont_b_sel;
            bcmdrd_field_set(ebuff, 26, 28, &fv);
        }

        /* Lookup enable. */
        fv = opinfo->enable;
        bcmdrd_field_set(ebuff, 29, 29, &fv);

        /* IPBM Present  and IPBM Source */
        if (ext_codes->ipbm_present != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ipbm_present;
            bcmdrd_field_set(ebuff, 30, 30, &fv);
            fv = ext_codes->ipbm_source;
            bcmdrd_field_set(ebuff, 31, 32, &fv);
        }
        if (opinfo->group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
            fv = isdw_enable;
            bcmdrd_field_set(ebuff, 33, 34, &fv);
        }
        /* keygen program profile index */
        if (ext_codes->keygen_index != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->keygen_index;
            bcmdrd_field_set(ebuff, 35, 39, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* LTID must be present. */
        if (ext_codes->ltid != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ltid;
            bcmdrd_field_set(ebuff, 42, 45, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        /* MultiWide mode */
        if (num_parts == 1) {
            fv = 0;
        } else if (num_parts == 2) {
            fv = (part == 0) ? 1 : 2;
        } else if (num_parts == 3) {
            fv = (part == 0) ? 1 : ((part == 1) ? 3 : 4);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        bcmdrd_field_set(ebuff, 46, 48, &fv);

        /* SRC_CONTAINER_A_SEL Select */
        if (ext_codes->src_cont_a_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->src_cont_a_sel;
            bcmdrd_field_set(ebuff, 51, 52, &fv);
        }

        /* SRC_CONTAINER_B_SEL Select */
        if (ext_codes->src_cont_b_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->src_cont_b_sel;
            bcmdrd_field_set(ebuff, 53, 54, &fv);
        }

        /* TCP_FN_SEL Select */
        if (ext_codes->tcp_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->tcp_fn_sel;
            bcmdrd_field_set(ebuff, 55, 56, &fv);
        }

        /* TOS_FN_SEL Select */
        if (ext_codes->tos_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->tos_fn_sel;
            bcmdrd_field_set(ebuff, 57, 58, &fv);
        }

        /* TTL_FN_SEL Select */
        if (ext_codes->ttl_fn_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ttl_fn_sel;
            bcmdrd_field_set(ebuff, 59, 60, &fv);
        }

        /* UDF_SEL Select */
        if (ext_codes->udf_sel != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->udf_sel;
            bcmdrd_field_set(ebuff, 61, 61, &fv);
        }

        /* Copy 62 bits from ebuff to edw[part][0] */
        bcmdrd_field_set(&(edw[part][0]), 210, 271, ebuff);
    }

exit:
    SHR_FUNC_EXIT();
}

