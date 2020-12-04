/*! \file field.c
 *
 * Field Driver for XGS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/port.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/xgs/field.h>
#include <bcm_int/ltsw/xgs/udf.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/util.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FP

typedef struct xgs_field_redir_profile_s {
    uint8_t         mask_action; /* Action types - AND, OR */
    bcm_pbmp_t      pbmp;        /* Pbmp */
} xgs_field_redir_profile_t;

typedef struct xgs_field_delayed_profile_s {
    uint8_t         mask_action; /* Action types - REDIRECT or DROP */
    bcm_pbmp_t      pbmp;        /* Pbmp */
    bool            elephant_pkts_only; /* Actions applicable to elephant pkt only */
} xgs_field_delayed_profile_t;

static ltsw_field_profile_info_t xgs_field_redir_profile_info[BCM_MAX_NUM_UNITS];
static ltsw_field_profile_info_t xgs_field_egr_mask_profile_info[BCM_MAX_NUM_UNITS];
static ltsw_field_profile_info_t xgs_field_delayed_redir_profile_info[BCM_MAX_NUM_UNITS];
static ltsw_field_profile_info_t xgs_field_delayed_drop_profile_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */
/*!
 * \brief Clear IFP presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_ifp_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_PRESEL_ENTRY_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EMFP presel configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_emfp_presel_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field presel entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_PRESEL_ENTRY_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear IFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_ifp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_GRP_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_vfp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_VLAN_GRP_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_efp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EGR_GRP_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EMFP group configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_emfp_group_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_GRP_TEMPLATEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear IFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_ifp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_POLICY_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EMFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_emfp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_RULE_TEMPLATEs));

    /* Clear Field PDD table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_PDD_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_POLICY_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EM_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_vfp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_VLAN_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_VLAN_POLICY_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_VLAN_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear EFP entry configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_xgs_efp_entry_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Field Rule table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EGR_RULE_TEMPLATEs));

    /* Clear Field Policy table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EGR_POLICY_TEMPLATEs));

    /* Clear Field Entry table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_EGR_ENTRYs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field IFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_ifp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_ifp_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_ifp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_ifp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field VFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_vfp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_vfp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_vfp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field EFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_efp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_efp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_efp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field EMFP configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_emfp_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear field presel configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_emfp_presel_clear(unit));

    /* Clear field group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_emfp_group_clear(unit));

    /* Clear field entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_xgs_emfp_entry_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear Field configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear IFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_ifp_clear(unit));

    /* Clear VFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_vfp_clear(unit));

    /* Clear EFP configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_efp_clear(unit));

    /* Clear EM configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_emfp_clear(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_port_filter_enable_set(int unit, uint32 state,
                                      bcm_pbmp_t port_pbmp)
{
    int port = 0;                   /* Port iterator.               */
    int ret_val = SHR_E_NONE;       /* Operation return value.      */

    SHR_FUNC_ENTER(unit);
    if (state >> 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    state = (state) ? 1 : 0;

    /* Iterate over all the ports but CPU set Lookup filtering. */
    BCM_PBMP_ITER(port_pbmp, port) {
        /* Set field enable to the hw. */
        ret_val =  bcm_ltsw_port_control_set(unit, port,
                                             bcmPortControlFilterLookup, state);
        SHR_IF_ERR_VERBOSE_EXIT(ret_val);
        /* Set field enable to the hw. */
        ret_val =  bcm_ltsw_port_control_set(unit, port,
                                             bcmPortControlFilterIngress, state);
        SHR_IF_ERR_VERBOSE_EXIT(ret_val);
        /* Set field enable to the hw. */
        ret_val =  bcm_ltsw_port_control_set(unit, port,
                                             bcmPortControlFilterEgress, state);
        SHR_IF_ERR_VERBOSE_EXIT(ret_val);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_group_udf_qset_update_with_hints(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_field_hint_t *hint,
    bcm_field_qset_t *udf_qset,
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap)
{
    int8_t c;
    int8_t shift_val;
    uint32_t width_rem;
    uint32_t offset_rem;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    uint16_t byte_bitmap;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(udf_qset_bitmap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_chunk_info_get(unit, udf_id, &info));

    if (hint) {
        if ((hint->start_bit > (info.width * 8))
                || (hint->end_bit) > (info.width * 8)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        width_rem = hint->end_bit - hint->start_bit + 1;
        offset_rem = (info.offset % 2) * 8 + hint->start_bit;
    } else {
        width_rem = info.width * 8;
        offset_rem = (info.offset % 2) * 8;
    }

    for (c = 0; (c < max_chunks) && (width_rem > 0); c++) {

        if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
            continue;
        }
        qual = bcmiFieldQualifyB2Chunk0 + c;
        for (shift_val = 8; shift_val >= 0; shift_val -= 8) {
            if (width_rem == 0) {
                break;
            }
            if (offset_rem >= 8) {
                offset_rem -= 8;
                continue;
            }
            if ((offset_rem + width_rem) <= 8) {
                byte_bitmap = ((1 << width_rem) - 1) << offset_rem;
                width_rem = 0;
            } else {
                if (offset_rem == 0) {
                    byte_bitmap = 0xFF;
                } else {
                    byte_bitmap = (~((1 << offset_rem) - 1)) & 0xFF;
                }
                width_rem -= (8 - offset_rem);
            }
            offset_rem = 0;
            if (udf_qset != NULL) {
                BCM_FIELD_QSET_ADD(*udf_qset, qual);
            }
            udf_qset_bitmap->chunk_b2_bitmap[c] |= (byte_bitmap << shift_val);
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_udf_qset_set(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx, c;
    bcm_udf_multi_chunk_info_t info;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);
    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < num_objects; idx++) {
        sal_memset(&info, 0, sizeof (info));

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_udf_chunk_info_get(unit, objects_list[idx], &info));

        for (c = 0; c < max_chunks; c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_ADD(*qset,
                                    bcmiFieldQualifyB2Chunk0 + c);
                /* setting udf_map helps in gets/deletes */
                SHR_BITSET(qset->udf_map, max_chunks + c);
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_udf_obj_get(
    int unit,
    bcm_field_qset_t *qset,
    int max,
    int *objects_list,
    int *actual)
{
    int c, cnt = 0;
    int idx, actual_cnt = 0;
    uint32_t chunk_2b_bmap = 0;
    uint32_t comp_chunk_2b_bmap = 0;
    bcm_udf_id_t *udf_obj_arr = NULL;
    bcm_udf_multi_chunk_info_t info;
    uint32_t max_chunks;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    if ((max > 0) && (objects_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    for (c = 0; c < max_chunks; c++) {
         if (SHR_BITGET(qset->udf_map, c + max_chunks)) {
             chunk_2b_bmap |= (1 << c);
         }
    }

    /* UDF function to traverse and retrieve the UDF chunks info */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_get_all(unit, 0, NULL, &actual_cnt));

    /* Allocate */
    BCMINT_FIELD_MEM_ALLOC(udf_obj_arr,
                actual_cnt * sizeof (bcm_udf_id_t),
                "udf objects");

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_get_all(unit, actual_cnt, udf_obj_arr,
                              &actual_cnt));

    for (idx = 0, *actual = 0; idx < actual_cnt; idx++) {
        sal_memset(&info, 0x0, sizeof(info));

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_udf_chunk_info_get(unit, udf_obj_arr[idx], &info));

        comp_chunk_2b_bmap = 0;
        for (c = 0; c < max_chunks; c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                comp_chunk_2b_bmap |= (1 << c);
            }
        }

        if ((chunk_2b_bmap & comp_chunk_2b_bmap) == comp_chunk_2b_bmap) {
            if (max > 0) {
                if (cnt >= max) {
                    break;
                }
                objects_list[cnt] = udf_obj_arr[idx];
            }
            cnt += 1;
        }
    }

    *actual = cnt;

exit:

    BCMINT_FIELD_MEM_FREE(udf_obj_arr);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_udf_qset_del(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    int idx, c;
    bcm_udf_multi_chunk_info_t info;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);
    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < num_objects; idx++) {
        sal_memset(&info, 0, sizeof (info));

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_udf_chunk_info_get(unit, objects_list[idx], &info));

        for (c = 0; c < max_chunks; c++) {
            if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c)) {
                BCMI_FIELD_QSET_REMOVE(*qset,
                                    bcmiFieldQualifyB2Chunk0 + c);
                SHR_BITCLR(qset->udf_map, max_chunks + c);
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
xgs_ltsw_field_udf_qual_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    int dunit = 0;
    int map_ct = 0;
    int8_t c, base;
    bool found;
    uint64_t data64;
    uint64_t mask64;
    int8_t shift_val;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    char *qual_mask_field = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t rule_templ = BCMLT_INVALID_HDL;
    uint32_t width_rem, offset_rem;
    uint16_t chunk_2b_qual_data[16];
    uint16_t chunk_2b_qual_mask[16];
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap = NULL;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(chunk_2b_qual_data, 0, sizeof(uint16_t) * 16);
    sal_memset(chunk_2b_qual_mask, 0, sizeof(uint16_t) * 16);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_udf_qset_bitmap_alloc(unit, &udf_qset_bitmap));

    sal_memset(&info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_chunk_info_get(unit, udf_id, &info));

    /* If provided entry data length is greater than UDF width */
    if ((length > info.width) || (length <= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->rule_sid,
                              &rule_templ));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_templ,
                               stage_info->tbls_info->rule_key_fid,
                               entry_oper->entry_id));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmint_field_group_udf_qset_bitmap_update(unit,
                group_oper->hintid, udf_id, NULL, udf_qset_bitmap));

    offset_rem = (info.offset % 2) * 8;
    base = (offset_rem) ? -1 : 0;
    width_rem = (length < info.width) ? (length * 8) : (info.width * 8);
    for (c = 0; (c < max_chunks) && (width_rem > 0); c++) {

        if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
            continue;
        }

        for (shift_val = 8; shift_val >= 0; shift_val -= 8) {
            if (width_rem == 0) {
                break;
            }
            if (offset_rem >= 8) {
                offset_rem -= 8;
                base += 1;
                continue;
            }

            chunk_2b_qual_data[c] |= (data[base] << shift_val);
            chunk_2b_qual_mask[c] |= (mask[base] << shift_val);
            width_rem -= 8;
            base += 1;
        }
        chunk_2b_qual_data[c] &= udf_qset_bitmap->chunk_b2_bitmap[c];
        chunk_2b_qual_mask[c] &= udf_qset_bitmap->chunk_b2_bitmap[c];
    }

    for (c = 0; c < max_chunks; c++) {

        if (udf_qset_bitmap->chunk_b2_bitmap[c] == 0) {
            continue;
        }
        qual = bcmiFieldQualifyB2Chunk0 + c;
        FP_HA_GROUP_OPER_QSET_TEST(group_oper, qual, found);
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }


        data64 = chunk_2b_qual_data[c];
        mask64 = chunk_2b_qual_mask[c];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, qual,
                                        stage_info, 0, 0,
                                        &map_ct, &lt_map_ref));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(rule_templ,
                                         (lt_map_ref->map[0]).lt_field_name,
                                         c, &data64,
                                         1));
        if (stage_info->stage_id != bcmiFieldStageExactMatch) {
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_field,
                 (strlen((lt_map_ref->map[0]).lt_field_name) + 6),
                 "qualifier mask field");
            sal_strcpy(qual_mask_field, (lt_map_ref->map[0]).lt_field_name);
            sal_strcat(qual_mask_field, "_MASK");
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(rule_templ,
                                             qual_mask_field,
                                             c, &mask64,
                                             1));
            sal_free(qual_mask_field);
            qual_mask_field = NULL;
        }
    }

    /* Update rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(rule_templ, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    bcmint_field_group_udf_qset_bitmap_free(unit, &udf_qset_bitmap);
    if (BCMLT_INVALID_HDL != rule_templ) {
        (void) bcmlt_entry_free(rule_templ);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the filter enable state in the Port Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_field_udf_qual_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_udf_id_t udf_id,
    int max_len,
    uint8 *data,
    uint8 *mask,
    int *actual_len)
{
    int dunit = 0;
    int map_ct = 0;
    bool found;
    uint64_t data64;
    uint64_t mask64;
    uint32_t c;
    uint32_t num_elem;
    int8_t base = 0;
    int8_t shift_val = 0;
    bcm_field_qset_t qset;
    bcmi_field_qualify_t qual;
    bcm_udf_multi_chunk_info_t info;
    char *qual_mask_field = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t rule_templ = BCMLT_INVALID_HDL;
    uint32_t width_rem, offset_rem;
    uint32_t max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    SHR_FUNC_ENTER(unit);

    if ((data == NULL) || (mask == NULL) || (actual_len == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_FIELD_QSET_INIT(qset);
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_field_udf_qset_set(unit, 1, &udf_id, &qset));

    sal_memset(&info, 0x0, sizeof(info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_chunk_info_get(unit, udf_id, &info));

    if (max_len < info.width) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->rule_sid,
                              &rule_templ));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_templ,
                               stage_info->tbls_info->rule_key_fid,
                               entry_oper->entry_id));

    /* Lookup rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(rule_templ, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    *actual_len = 0;
    width_rem = info.width * 8;
    offset_rem = (info.offset % 2) * 8;
    base = (offset_rem) ? -1 : 0;
    for (c = 0; (c < max_chunks) && (width_rem > 0); c++) {

        data64 = mask64 = 0;
        if (BCM_UDF_CBMP_CHUNK_TEST(info.two_byte_chunk_bmap, c) == 0) {
            continue;
        }
        qual = bcmiFieldQualifyB2Chunk0 + c;
        FP_HA_GROUP_OPER_QSET_TEST(group_oper, qual, found);
        if (found == TRUE) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qual_map_find(unit, qual,
                                            stage_info, 0, 0,
                                            &map_ct, &lt_map_ref));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(rule_templ,
                                             (lt_map_ref->map[0]).lt_field_name,
                                             c, &data64, 1, &num_elem));
            if (stage_info->stage_id != bcmiFieldStageExactMatch) {
                qual_mask_field = NULL;
                BCMINT_FIELD_MEM_ALLOC(qual_mask_field,
                        (strlen((lt_map_ref->map[0]).lt_field_name) + 6),
                        "qualifier mask field");
                sal_strcpy(qual_mask_field, (lt_map_ref->map[0]).lt_field_name);
                sal_strcat(qual_mask_field, "_MASK");
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(rule_templ,
                                                 qual_mask_field,
                                                 c, &mask64, 1, &num_elem));
                sal_free(qual_mask_field);
            }
        } else {
            data64 = mask64 = 0;
        }

        for (shift_val = 8; shift_val >= 0; shift_val -= 8) {
            if (width_rem == 0) {
                break;
            }
            if (offset_rem >= 8) {
                offset_rem -= 8;
                base += 1;
                continue;
            }

            data[base] = (uint8_t)(data64 >> shift_val) & 0xFF;
            mask[base] = (uint8_t)(mask64 >> shift_val) & 0xFF;
            base += 1;
            width_rem -= 8;
            *actual_len += 1;
        }
    }

exit:
    if (BCMLT_INVALID_HDL != rule_templ) {
        (void) bcmlt_entry_free(rule_templ);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get redirect profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_redir_lt_get(int unit, bcmi_ltsw_field_stage_t stage_id,
                       int index, void *profile)
{
    uint32_t cnt;
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_redir_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->redirect_profile_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->redirect_profile_key_fid,
                               (index + 1)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xgs_field_redir_profile_t *)profile;

    /* Get egress mask */
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh,
                                         stage_info->tbls_info->redirect_profile_ports_fid,
                                         i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_egr_mask_lt_get(int unit, bcmi_ltsw_field_stage_t stage_id,
                          int index, void *profile)
{
    uint32_t cnt;
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_redir_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->egress_mask_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_key_fid,
                               (index + 1)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xgs_field_redir_profile_t *)profile;

    /* Get egress mask */
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh,
                                         stage_info->tbls_info->egress_mask_profile_ports_fid,
                                         i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert egress mask profile entry to LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [in] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_redir_lt_insert(int unit, bcmi_ltsw_field_stage_t stage_id,
                          int index, void *profile)
{
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_redir_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    char *prof_lt_sid = NULL;
    char *prof_lt_key_fid = NULL;
    char *prof_ports_fid = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    p = (xgs_field_redir_profile_t *)profile;

    if (p->mask_action == BCMINT_FIELD_EGRESS_MASK_OP_OR) {
        prof_lt_sid = stage_info->tbls_info->redirect_profile_sid;
        prof_lt_key_fid = stage_info->tbls_info->redirect_profile_key_fid;
        prof_ports_fid = stage_info->tbls_info->redirect_profile_ports_fid;
    } else {
        prof_lt_sid = stage_info->tbls_info->egress_mask_profile_sid;
        prof_lt_key_fid = stage_info->tbls_info->egress_mask_profile_key_fid;
        prof_ports_fid = stage_info->tbls_info->egress_mask_profile_ports_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, prof_lt_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, prof_lt_key_fid, (index + 1)));

    /* Fill egress mask */
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        v64 = (BCM_PBMP_MEMBER(p->pbmp, i) ? 1 : 0);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(eh, prof_ports_fid, i, &v64, 1));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_redir_lt_delete(int unit,
                          bcmi_ltsw_field_stage_t stage_id,
                          int index)
{
    int dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->redirect_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->redirect_profile_key_fid,
                               index + 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_egr_mask_lt_delete(int unit,
                             bcmi_ltsw_field_stage_t stage_id,
                             int index)
{
    int dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->egress_mask_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->egress_mask_profile_key_fid,
                               index + 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of egress mask profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_redir_profile_hash(int unit, void *entries,
                    int entries_per_set, uint32_t *hash)
{
    int size = entries_per_set * sizeof(xgs_field_redir_profile_t);
    *hash = shr_crc32(0, entries, size);
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given redirect profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_redir_profile_cmp(int unit,
                    void *entries,
                    int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_field_ingress_profile_cmp
        (unit, entries, index, &xgs_field_redir_profile_info[unit], cmp);
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given egress mask profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_egr_mask_profile_cmp(int unit,
                    void *entries,
                    int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_field_ingress_profile_cmp
        (unit, entries, index, &xgs_field_egr_mask_profile_info[unit], cmp);
}


/*!
 * \brief Recover redirect profile from after wb.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_profile_recover(int unit,
                 ltsw_field_profile_info_t *pinfo)
{
    int index = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (index = 0; index < pinfo->entry_idx_max; index++) {
         rv = (bcmint_ltsw_field_profile_recover(unit,
                                        bcmiFieldStageIngress,
                                        index, pinfo));
         if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
             SHR_ERR_EXIT(rv);
         }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize redirect profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_redir_profile_init(int unit)
{
    uint32_t entry_num = 0;
    ltsw_field_profile_info_t *pinfo = &xgs_field_redir_profile_info[unit];
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    pinfo->phd = BCMI_LTSW_PROFILE_FIELD_REDIRECT;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              stage_info->tbls_info->redirect_profile_sid,
                              &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xgs_field_redir_profile_t);
    pinfo->lt_get = xgs_field_redir_lt_get;
    pinfo->lt_ins = xgs_field_redir_lt_insert;
    pinfo->lt_del = xgs_field_redir_lt_delete;

    /* unregister in case the profile already exists */
    if (bcmi_ltsw_profile_register_check(unit, pinfo->phd)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd));
    }

    /* register profile for IFP port flood block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit,
            &pinfo->phd, &pinfo->entry_idx_min,
            &pinfo->entry_idx_max, 1,
            xgs_field_ingress_redir_profile_hash,
            xgs_field_ingress_redir_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_ERR_EXIT
            (xgs_field_ingress_profile_recover(unit,
                                               pinfo));
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->redirect_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_egr_mask_profile_init(int unit)
{
    uint32_t entry_num = 0;
    ltsw_field_profile_info_t *pinfo = &xgs_field_egr_mask_profile_info[unit];
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    pinfo->phd = BCMI_LTSW_PROFILE_FIELD_EGR_MASK;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              stage_info->tbls_info->egress_mask_profile_sid,
                              &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xgs_field_redir_profile_t);
    pinfo->lt_get = xgs_field_egr_mask_lt_get;
    pinfo->lt_ins = xgs_field_redir_lt_insert;
    pinfo->lt_del = xgs_field_egr_mask_lt_delete;

    /* unregister in case the profile already exists */
    if (bcmi_ltsw_profile_register_check(unit, pinfo->phd)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd));
    }

    /* register profile for IFP port flood block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit,
            &pinfo->phd, &pinfo->entry_idx_min,
            &pinfo->entry_idx_max, 1,
            xgs_field_ingress_redir_profile_hash,
            xgs_field_ingress_egr_mask_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_ERR_EXIT
            (xgs_field_ingress_profile_recover(unit,
                                               pinfo));
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->egress_mask_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_redir_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit,
                        xgs_field_redir_profile_info[unit].phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->redirect_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_egr_mask_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit,
                        xgs_field_egr_mask_profile_info[unit].phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->egress_mask_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get delayed redirect profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_delayed_redir_lt_get(int unit,
                         bcmi_ltsw_field_stage_t stage_id,
                         int index, void *profile)
{
    uint32_t cnt;
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_delayed_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    char *prof_lt_sid = NULL;
    char *prof_lt_key_fid = NULL;
    char *prof_ports_fid = NULL;
    char *prof_elephant_pkts_fid = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    p = (xgs_field_delayed_profile_t *)profile;
    prof_lt_sid = stage_info->tbls_info->delayed_redirect_profile_sid;
    prof_lt_key_fid = stage_info->tbls_info->delayed_redirect_profile_key_fid;
    prof_ports_fid = stage_info->tbls_info->delayed_redirect_profile_ports_fid;
    prof_elephant_pkts_fid
        = stage_info->tbls_info->delayed_drop_profile_elephant_pkts_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, prof_lt_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, prof_lt_key_fid, (index + 1)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xgs_field_delayed_profile_t *)profile;

    /* Get egress mask */
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, prof_ports_fid,
                                         i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_get(eh, prof_elephant_pkts_fid, &v64));
     if (v64) {
         p->elephant_pkts_only = 1;
     }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get delayed drop profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_delayed_drop_lt_get(int unit,
                         bcmi_ltsw_field_stage_t stage_id,
                         int index, void *profile)
{
    uint32_t cnt;
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_delayed_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    char *prof_lt_sid = NULL;
    char *prof_lt_key_fid = NULL;
    char *prof_ports_fid = NULL;
    char *prof_elephant_pkts_fid = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    p = (xgs_field_delayed_profile_t *)profile;
    prof_lt_sid = stage_info->tbls_info->delayed_drop_profile_sid;
    prof_lt_key_fid = stage_info->tbls_info->delayed_drop_profile_key_fid;
    prof_ports_fid = stage_info->tbls_info->delayed_drop_profile_ports_fid;
    prof_elephant_pkts_fid
        = stage_info->tbls_info->delayed_redirect_profile_elephant_pkts_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, prof_lt_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, prof_lt_key_fid, (index + 1)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xgs_field_delayed_profile_t *)profile;

    /* Get egress mask */
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(eh, prof_ports_fid,
                                         i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_get(eh, prof_elephant_pkts_fid, &v64));
     if (v64) {
         p->elephant_pkts_only = 1;
     }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert delayed redirect/drop profile entry to LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [in] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_delayed_lt_insert(int unit,
                            bcmi_ltsw_field_stage_t stage_id,
                            int index, void *profile)
{
    uint64_t v64;
    int dunit, i, max_port_num;
    xgs_field_delayed_profile_t *p;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    char *prof_lt_sid = NULL;
    char *prof_lt_key_fid = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    char *prof_ports_fid = NULL;
    char *prof_elephant_pkts_fid = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    p = (xgs_field_delayed_profile_t *)profile;

    if (p->mask_action == BCMINT_FIELD_DELAYED_DROP) {
        prof_lt_sid = stage_info->tbls_info->delayed_drop_profile_sid;
        prof_lt_key_fid = stage_info->tbls_info->delayed_drop_profile_key_fid;
        prof_ports_fid = stage_info->tbls_info->delayed_drop_profile_ports_fid;
        prof_elephant_pkts_fid
            = stage_info->tbls_info->delayed_redirect_profile_elephant_pkts_fid;
    } else if (p->mask_action == BCMINT_FIELD_DELAYED_REDIRECT) {
        prof_lt_sid = stage_info->tbls_info->delayed_redirect_profile_sid;
        prof_lt_key_fid = stage_info->tbls_info->delayed_redirect_profile_key_fid;
        prof_ports_fid = stage_info->tbls_info->delayed_redirect_profile_ports_fid;
        prof_elephant_pkts_fid
            = stage_info->tbls_info->delayed_drop_profile_elephant_pkts_fid;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, prof_lt_sid, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, prof_lt_key_fid, (index + 1)));

    /* Fill egress mask */
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        v64 = (BCM_PBMP_MEMBER(p->pbmp, i) ? 1 : 0);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(eh, prof_ports_fid, i, &v64, 1));
    }

    v64 = p->elephant_pkts_only;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, prof_elephant_pkts_fid, v64));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete delayed redirect profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_delayed_redir_lt_delete(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  int index)
{
    int dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->delayed_redirect_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                               stage_info->tbls_info->delayed_redirect_profile_key_fid,
                               index + 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete delayed drop profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_delayed_drop_lt_delete(int unit,
                                 bcmi_ltsw_field_stage_t stage_id,
                                 int index)
{
    int dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->delayed_drop_profile_sid,
                              &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh,
                            stage_info->tbls_info->delayed_drop_profile_key_fid,
                            index + 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
         (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of egress mask profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_profile_hash(int unit, void *entries,
                    int entries_per_set, uint32_t *hash)
{
    int size = entries_per_set * sizeof(xgs_field_delayed_profile_t);
    *hash = shr_crc32(0, entries, size);
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given delayed redirect profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_redir_profile_cmp(int unit,
                    void *entries,
                    int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_field_ingress_profile_cmp
        (unit, entries, index, &xgs_field_delayed_redir_profile_info[unit], cmp);
}


/*!
 * \brief Compare profile set.
 *
 * Compare if given delayed drop profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_drop_profile_cmp(int unit,
                    void *entries,
                    int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_field_ingress_profile_cmp
        (unit, entries, index, &xgs_field_delayed_drop_profile_info[unit], cmp);
}

/*!
 * \brief Initialize delayed redirect profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_redir_profile_init(int unit)
{
    uint32_t entry_num = 0;
    ltsw_field_profile_info_t *pinfo = &xgs_field_delayed_redir_profile_info[unit];
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    pinfo->phd = BCMI_LTSW_PROFILE_FIELD_DELAYED_REDIRECT;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              stage_info->tbls_info->delayed_redirect_profile_sid,
                              &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xgs_field_delayed_profile_t);
    pinfo->lt_get = xgs_field_delayed_redir_lt_get;
    pinfo->lt_ins = xgs_field_delayed_lt_insert;
    pinfo->lt_del = xgs_field_delayed_redir_lt_delete;

    /* unregister in case the profile already exists */
    if (bcmi_ltsw_profile_register_check(unit, pinfo->phd)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd));
    }

    /* register profile for IFP port flood block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit,
            &pinfo->phd, &pinfo->entry_idx_min,
            &pinfo->entry_idx_max, 1,
            xgs_field_ingress_delayed_profile_hash,
            xgs_field_ingress_delayed_redir_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_ERR_EXIT
            (xgs_field_ingress_profile_recover(unit,
                                               pinfo));
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->delayed_redirect_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize delayed drop profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_drop_profile_init(int unit)
{
    uint32_t entry_num = 0;
    ltsw_field_profile_info_t *pinfo = &xgs_field_delayed_drop_profile_info[unit];
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    pinfo->phd = BCMI_LTSW_PROFILE_FIELD_DELAYED_DROP;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              stage_info->tbls_info->delayed_drop_profile_sid,
                              &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xgs_field_delayed_profile_t);
    pinfo->lt_get = xgs_field_delayed_drop_lt_get;
    pinfo->lt_ins = xgs_field_delayed_lt_insert;
    pinfo->lt_del = xgs_field_delayed_drop_lt_delete;

    /* unregister in case the profile already exists */
    if (bcmi_ltsw_profile_register_check(unit, pinfo->phd)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd));
    }

    /* register profile for IFP port flood block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit,
            &pinfo->phd, &pinfo->entry_idx_min,
            &pinfo->entry_idx_max, 1,
            xgs_field_ingress_delayed_profile_hash,
            xgs_field_ingress_delayed_drop_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_ERR_EXIT
            (xgs_field_ingress_profile_recover(unit,
                                               pinfo));
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit,
                       stage_info->tbls_info->delayed_redirect_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize delayed redirect profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_redir_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit,
                        xgs_field_delayed_redir_profile_info[unit].phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, stage_info->tbls_info->delayed_redirect_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

 /*
 * \brief De-initialize delayed drop profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_field_ingress_delayed_drop_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageIngress, &stage_info));

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit,
                        xgs_field_delayed_drop_profile_info[unit].phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, stage_info->tbls_info->delayed_drop_profile_sid));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_ingress_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_delayed_redir_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_delayed_drop_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_redir_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_egr_mask_profile_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_ingress_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_delayed_redir_profile_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_delayed_drop_profile_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_redir_profile_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_field_ingress_egr_mask_profile_deinit(unit));
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_profile_index_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_action_t action,
                                        uint16_t *act_prof_idx)
{
    uint64_t action_lt_val = 0x0;
    bcm_field_action_params_t params;
    bcm_field_action_match_config_t match_config;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;

    SHR_FUNC_ENTER(unit);

    /* Find prof index associated for valid actions. */
    if ((bcmFieldActionRedirectPbmp == action)
        || (bcmFieldActionEgressPortsAdd == action)
        || (bcmFieldActionEgressMask == action)
        || (bcmFieldActionRedirectBcastPbmp == action)
        || (bcmFieldActionMatchPbmpDrop ==action)
        || (bcmFieldActionMatchPbmpRedirect == action)) {

        sal_memset(&params, 0x0, sizeof(params));
        sal_memset(&match_config, 0, sizeof(match_config));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_config_info_get(unit, entry,
                                                 action, &action_lt_val,
                                                 &params, &match_config));
        *act_prof_idx = (uint16_t)action_lt_val;
    }
    /* This action has multiple lt fields set in lt_value */
    if (bcmFieldActionMatchPbmpRedirect == action) {

        /* Get stage info from entry ID . */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

        /* Find lt_map for provided action from db. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_map_find(unit, action,
                                          stage_info, &lt_map_ref));
        *act_prof_idx =
            (((1 << lt_map_ref->map[0].width[0]) - 1) & (action_lt_val));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_profile_add(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  bcm_field_action_params_t *params,
                                  bcm_field_action_match_config_t *match_config,
                                  uint64_t *act_prof_idx_val)
{
    int redir_idx = 0;
    xgs_field_redir_profile_t redir_prof;
    xgs_field_delayed_profile_t delayed_prof;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(params, SHR_E_INTERNAL);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);

    /* Populate egress mask profile params. */
    switch (action) {
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionEgressMask:
        case bcmFieldActionRedirectBcastPbmp:

            BCM_PBMP_ASSIGN(redir_prof.pbmp, params->pbmp);

            if (action == bcmFieldActionEgressMask) {
                redir_prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_AND;

                /* Add egress mask profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_add(unit, stage_id,
                     &redir_prof, 1,
                     &xgs_field_egr_mask_profile_info[unit], &redir_idx));
            } else {
                redir_prof.mask_action = BCMINT_FIELD_EGRESS_MASK_OP_OR;

                /* Add redirect profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_add(unit, stage_id,
                     &redir_prof, 1,
                     &xgs_field_redir_profile_info[unit], &redir_idx));
            }

            /* Send action profile index value. */
            *act_prof_idx_val = (uint64_t)(redir_idx + 1);
            break;
        case bcmFieldActionMatchPbmpDrop:
        case bcmFieldActionMatchPbmpRedirect:

            SHR_NULL_CHECK(match_config, SHR_E_INTERNAL);
            BCM_PBMP_ASSIGN(delayed_prof.pbmp, match_config->egr_pbmp);
            delayed_prof.elephant_pkts_only = match_config->elephant_pkt_only;

            if (action == bcmFieldActionMatchPbmpDrop) {
                delayed_prof.mask_action = BCMINT_FIELD_DELAYED_DROP;

                /* Add delayed drop profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_add(unit, stage_id,
                     &delayed_prof, 1,
                     &xgs_field_delayed_drop_profile_info[unit], &redir_idx));
            } else {
                delayed_prof.mask_action = BCMINT_FIELD_DELAYED_REDIRECT;

                /* Add delayed redirect profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_add(unit, stage_id,
                     &delayed_prof, 1,
                     &xgs_field_delayed_redir_profile_info[unit], &redir_idx));
            }

            /* Send action profile index value. */
            *act_prof_idx_val = (uint64_t)(redir_idx + 1);
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_profile_get(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  uint64_t *act_prof_idx_val,
                                  bcm_field_action_params_t *params,
                                  bcm_field_action_match_config_t *match_config)
{
    int redir_idx = 0;
    xgs_field_redir_profile_t redir_prof;
    xgs_field_delayed_profile_t delayed_prof;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);
    SHR_NULL_CHECK(params, SHR_E_INTERNAL);

    /* Retreive egress mask profile index. */
    redir_idx = (int)(COMPILER_64_LO(*act_prof_idx_val));
    redir_idx = (redir_idx - 1);

    /* Retreive egress profile entry. */
    switch (action) {
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionEgressMask:
        case bcmFieldActionRedirectBcastPbmp:
            if (action == bcmFieldActionEgressMask) {
                /* Add egress mask profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_get(unit, stage_id,
                        redir_idx,
                        &xgs_field_egr_mask_profile_info[unit], &redir_prof));
            } else {
                /* Add redirect profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_get(unit, stage_id,
                        redir_idx,
                        &xgs_field_redir_profile_info[unit], &redir_prof));
            }
            /* Populate pbmp to user. */
            BCM_PBMP_ASSIGN(params->pbmp, redir_prof.pbmp);
            break;
        case bcmFieldActionMatchPbmpDrop:
        case bcmFieldActionMatchPbmpRedirect:
            SHR_NULL_CHECK(match_config, SHR_E_INTERNAL);
            if (action == bcmFieldActionMatchPbmpDrop) {
                delayed_prof.mask_action = BCMINT_FIELD_DELAYED_DROP;
                /* Add delayed drop profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_get(unit, stage_id,
                        redir_idx,
                        &xgs_field_delayed_drop_profile_info[unit], &delayed_prof));
            } else {
                delayed_prof.mask_action = BCMINT_FIELD_DELAYED_REDIRECT;
                /* Add delayed redirect profile entry and retreive index. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_profile_get(unit, stage_id,
                        redir_idx,
                        &xgs_field_delayed_redir_profile_info[unit], &delayed_prof));
            }
            /* Populate pbmp to user. */
            BCM_PBMP_ASSIGN(match_config->egr_pbmp, delayed_prof.pbmp);
            match_config->elephant_pkt_only = delayed_prof.elephant_pkts_only;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_profile_del(int unit,
                                  bcmi_ltsw_field_stage_t stage_id,
                                  bcm_field_action_t action,
                                  uint64_t *act_prof_idx_val)
{
    int redir_idx = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(act_prof_idx_val, SHR_E_INTERNAL);

    /* Retreive egress mask profile index. */
    redir_idx = (int)(COMPILER_64_LO(*act_prof_idx_val));
    redir_idx = (redir_idx - 1);

    /* Delete profile entry created. */
    switch (action) {
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionEgressMask:
        case bcmFieldActionRedirectBcastPbmp:
            if (action == bcmFieldActionEgressMask) {
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmint_ltsw_field_profile_delete(unit, stage_id,
                       redir_idx, &xgs_field_egr_mask_profile_info[unit]));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmint_ltsw_field_profile_delete(unit, stage_id,
                       redir_idx, &xgs_field_redir_profile_info[unit]));
            }
            break;
        case bcmFieldActionMatchPbmpDrop:
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_ltsw_field_profile_delete(unit, stage_id,
                   redir_idx, &xgs_field_delayed_drop_profile_info[unit]));
            break;
        case bcmFieldActionMatchPbmpRedirect:
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_ltsw_field_profile_delete(unit, stage_id,
                   redir_idx, &xgs_field_delayed_redir_profile_info[unit]));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Driver to handle different Qualification parameters
 * for all qualifiers and across devices.
 */
int
xgs_ltsw_field_qual_value_set(int unit,
                    bcm_field_entry_t entry_id,
                    bcm_qual_field_t *qual_info)
{
    char *data_val = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyMplsForwardingLabelAction:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_PHP:
                    data_val = "PHP";
                    break;
                case BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_SWAP:
                    data_val = "SWAP";
                    break;
                case BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP:
                    data_val = "POP";
                    break;
                case BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP_USE_L2_VPN:
                    data_val = "POP_USE_L2_VPN";
                    break;
                case BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP_USE_L3_VPN:
                    data_val = "POP_USE_L3_VPN";
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            qual_info->depth = 1;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_val;
            break;
        case bcmFieldQualifyIngressStpState:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case BCM_STG_STP_DISABLE:
                    data_val = "DISABLE";
                    break;
                case BCM_STG_STP_BLOCK:
                    data_val = "BLOCK";
                    break;
                case BCM_STG_STP_LEARN:
                    data_val = "LEARN";
                    break;
                case BCM_STG_STP_FORWARD:
                    data_val = "FORWARD";
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            qual_info->depth = 1;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_val;
            break;
        case bcmFieldQualifyOuterTpid:
        case bcmFieldQualifyInnerTpid:
        case bcmFieldQualifyOverlayOuterTpid:
            switch(*((uint16_t *)(qual_info->qual_data))) {
                case 0x8100:
                    data_val = "TPID_8100";
                    break;
                case 0x9100:
                    data_val = "TPID_9100";
                    break;
                case 0x88A8:
                    data_val = "TPID_88A8";
                    break;
                default:
                    data_val = "ANY";
                    break;
            }
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT16;
            qual_info->depth = 1;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_val;
            break;
        case bcmFieldQualifyStpState:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case  BCM_FIELD_STG_STP_DISABLE:
                    data_val = "DISABLE";
                    break;
                case BCM_FIELD_STG_STP_BLOCK:
                    data_val = "BLOCK";
                    break;
                case BCM_FIELD_STG_STP_LEARN:
                    data_val = "LEARN";
                    break;
                case BCM_FIELD_STG_STP_FORWARD:
                    data_val = "FORWARD";
                    break;
                case (BCM_FIELD_STG_STP_DISABLE|BCM_FIELD_STG_STP_BLOCK):
                    data_val = "DISABLE_BLOCK";
                    break;
                case (BCM_FIELD_STG_STP_DISABLE|BCM_FIELD_STG_STP_LEARN):
                    data_val = "LEARN_DISABLE";
                    break;
                case (BCM_FIELD_STG_STP_BLOCK|BCM_FIELD_STG_STP_FORWARD):
                    data_val = "FORWARD_BLOCK";
                    break;
                case (BCM_FIELD_STG_STP_LEARN|BCM_FIELD_STG_STP_FORWARD):
                    data_val = "LEARN_FORWARD";
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            qual_info->depth = 1;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_val;
            break;
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
            switch(*((uint8_t *)(qual_info->qual_data))) {
                case BCM_FIELD_COLOR_GREEN:
                    data_val = "GREEN";
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    data_val = "YELLOW";
                    break;
                case BCM_FIELD_COLOR_RED:
                    data_val = "RED";
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }

            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            qual_info->depth = 1;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_val;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qual_info_set(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info)
{
    char *data_enum = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyMplsForwardingLabelAction:
        case bcmFieldQualifyIngressStpState:
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT8;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_enum;
            break;
        case bcmFieldQualifyOuterTpid:
        case bcmFieldQualifyInnerTpid:
        case bcmFieldQualifyOverlayOuterTpid:
            qual_info->depth = 1;
            qual_info->data_type = BCM_QUAL_DATA_TYPE_UINT16;
            qual_info->flags = BCM_FIELD_F_ENUM;
            qual_info->enum_name = data_enum;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qual_value_get(int unit,
        bcm_field_entry_t entry,
        bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qual_info, SHR_E_INTERNAL);

    switch (qual_info->sdk6_qual_enum) {
        case bcmFieldQualifyMplsForwardingLabelAction:
            if (0 == sal_strcmp(qual_info->enum_name, "PHP")) {
                **((uint8_t **)qual_info->qual_data) =
                                        BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_PHP;
            } else if (0 == sal_strcmp(qual_info->enum_name, "SWAP")) {
                **((uint8_t **)qual_info->qual_data) =
                                        BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_SWAP;
            } else if (0 == sal_strcmp(qual_info->enum_name, "POP")) {
                **((uint8_t **)qual_info->qual_data) =
                                        BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP;
            } else if (0 == sal_strcmp(qual_info->enum_name, "POP_USE_L2_VPN")) {
                **((uint8_t **)qual_info->qual_data) =
                            BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP_USE_L2_VPN;
            } else if (0 == sal_strcmp(qual_info->enum_name, "POP_USE_L3_VPN")) {
                **((uint8_t **)qual_info->qual_data) =
                            BCM_FIELD_MPLS_FORWARDING_LABEL_ACTION_POP_USE_L3_VPN;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyIngressStpState:
            if (0 == sal_strcmp(qual_info->enum_name, "DISABLE")) {
                **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_DISABLE;
            } else if (0 == sal_strcmp(qual_info->enum_name, "BLOCK")) {
                **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_BLOCK;
            } else if (0 == sal_strcmp(qual_info->enum_name, "LEARN")) {
                **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_LEARN;
            } else if (0 == sal_strcmp(qual_info->enum_name, "FORWARD")) {
                **((uint8_t **)qual_info->qual_data) = BCM_STG_STP_FORWARD;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyStpState:
            if (0 == sal_strcmp(qual_info->enum_name, "DISABLE")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_DISABLE;
            } else if (0 == sal_strcmp(qual_info->enum_name, "BLOCK")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_BLOCK;
            } else if (0 == sal_strcmp(qual_info->enum_name, "LEARN")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_LEARN;
            } else if (0 == sal_strcmp(qual_info->enum_name, "FORWARD")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_FORWARD;
            } else if (0 == sal_strcmp(qual_info->enum_name, "LEARN_FORWARD")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_LEARN|BCM_FIELD_STG_STP_FORWARD;
            } else if (0 == sal_strcmp(qual_info->enum_name, "LEARN_DISABLE")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_LEARN|BCM_FIELD_STG_STP_DISABLE;
            } else if (0 == sal_strcmp(qual_info->enum_name, "FORWARD_BLOCK")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_FORWARD|BCM_FIELD_STG_STP_BLOCK;
            } else if (0 == sal_strcmp(qual_info->enum_name, "DISABLE_BLOCK")) {
                **((uint8_t **)qual_info->qual_data) = BCM_FIELD_STG_STP_DISABLE|BCM_FIELD_STG_STP_BLOCK;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyOuterTpid:
        case bcmFieldQualifyInnerTpid:
        case bcmFieldQualifyOverlayOuterTpid:
            if (0 == sal_strcmp(qual_info->enum_name, "TPID_8100")) {
                **((uint16_t **)qual_info->qual_data) = 0x8100;
            } else if (0 == sal_strcmp(qual_info->enum_name, "TPID_9100")) {
                **((uint16_t **)qual_info->qual_data) = 0x9100;
            } else if (0 == sal_strcmp(qual_info->enum_name, "TPID_88A8")) {
                **((uint16_t **)qual_info->qual_data) = 0x88A8;
            } else if (0 == sal_strcmp(qual_info->enum_name, "ANY")) {
                /*
                 * No value is stored and Nothing to return.
                 * Keep calm and free enum_name.
                 */
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldQualifyColor:
        case bcmFieldQualifyLoopBackColor:
            if (0 == sal_strcmp(qual_info->enum_name , "GREEN")) {
                **((uint16_t **)qual_info->qual_data) = BCM_FIELD_COLOR_GREEN;
            } else if (0 == sal_strcmp(qual_info->enum_name , "RED")) {
                **((uint16_t **)qual_info->qual_data) = BCM_FIELD_COLOR_RED;
            } else if (0 == sal_strcmp(qual_info->enum_name , "YELLOW")) {
                **((uint16_t **)qual_info->qual_data) = BCM_FIELD_COLOR_YELLOW;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_int_ltsw_field_dup_val_in_lt_fields(int unit,
        int val,
        const bcm_field_action_map_t *lt_map_ref,
        uint64_t *action_value)
{
    uint8_t idx = 0;
    uint32 dst_ofset_current = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_map_ref, SHR_E_INTERNAL);

    for (idx = 0; idx < lt_map_ref->num_maps; idx++) {
        *action_value |= (((val << dst_ofset_current)));
        dst_ofset_current +=  lt_map_ref->map[idx].width[0];
    }
exit:
    SHR_FUNC_EXIT();
}

static int
xgs_int_ltsw_field_tpid_hw_encode(int unit,
                                  uint16_t tpid,
                                  uint64_t *hw_code)
{
    switch (tpid) {
        case 0x8100:
            *hw_code = 0x00;
            break;
        case 0x9100:
            *hw_code = 0x01;
            break;
        case 0x88a8:
            *hw_code = 0x02;
            break;
        default:
            *hw_code = 0x03;
    }
    return (BCM_E_NONE);
}

static int
xgs_int_ltsw_field_tpid_hw_decode(int unit,
                                  uint64_t hw_code,
                                  uint16_t *tpid)
{
    switch (hw_code) {
      case 0x0:
          *tpid = 0x8100;
          break;
      case 0x1:
          *tpid = 0x9100;
          break;
      case 0x2:
          *tpid = 0x88a8;
          break;
      default:
          return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

int
xgs_ltsw_field_action_value_set(int unit,
                        bcm_field_entry_t entry_id,
                        bcmi_ltsw_field_stage_t stage,
                        bcm_field_action_t action,
                        bcm_field_action_params_t *params,
                        bcm_field_action_match_config_t *match_config,
                        uint64_t *action_value)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    int val = 0;
    uint8_t skip_color[3] = {0};
    uint32 dst_ofset_current = 0;
    uint8_t idx = 0;
    int egr_obj_idx = 0;
    bcm_module_t mod  = -1;
    bcm_port_t lport = -1;
    int isLocal = 0;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcm_l3_egress_t egr;
    bool found = 0;
    bcm_field_action_t cpu_cancel_act_arr[2][8] = {
        {
            bcmFieldActionGpCopyToCpuCancel,
            bcmFieldActionYpCopyToCpuCancel,
            bcmFieldActionRpCopyToCpuCancel,
            bcmFieldActionCopyToCpuCancel,
            bcmFieldActionGpSwitchToCpuCancel,
            bcmFieldActionYpSwitchToCpuCancel,
            bcmFieldActionRpSwitchToCpuCancel,
            bcmFieldActionSwitchToCpuCancel
        },
        {
            bcmFieldActionGpSwitchToCpuCancel,
            bcmFieldActionYpSwitchToCpuCancel,
            bcmFieldActionRpSwitchToCpuCancel,
            bcmFieldActionSwitchToCpuCancel,
            bcmFieldActionGpCopyToCpuCancel,
            bcmFieldActionYpCopyToCpuCancel,
            bcmFieldActionRpCopyToCpuCancel,
            bcmFieldActionCopyToCpuCancel
        }
    };

    SHR_FUNC_ENTER(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

    /*
     * convert the sdk6 action values to sdklt action values
     * Do switch case based on
     * Misc actions which needs convertions or chip specific.
     */
    switch (action) {
        case bcmFieldActionIntCongestionNotification:
        case bcmFieldActionRpIntCongestionNotificationNew:
        case bcmFieldActionYpIntCongestionNotificationNew:
        case bcmFieldActionGpIntCongestionNotificationNew:
        case bcmFieldActionDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionEcnNew:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionGpPrioIntNew:
            val = params->param0;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionDscpPreserve:
        case bcmFieldActionDot1pPreserve:
            val = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioIntTos:
            val = 1;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionYpSwitchToCpuCancel:
        case bcmFieldActionRpSwitchToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
            val = 1;
            if (lt_map_ref->num_maps > 1) {
                for (idx = 0; idx < (lt_map_ref->num_maps/3); idx++) {
                    *action_value |= (((val << dst_ofset_current)));
                    dst_ofset_current +=  lt_map_ref->map[idx].width[0];
                }
                /* Find the mapping of the CPU Cancel action in the array . */
                for (idx = 0;
                     (idx < (sizeof(cpu_cancel_act_arr[0])/sizeof(cpu_cancel_act_arr[0][0])))
                        && (cpu_cancel_act_arr[0][idx] != action); idx++) {
                    /* Dont do anything here. Loop ends when the action is found. */
                }

                if (idx == (sizeof(cpu_cancel_act_arr[0])/sizeof(cpu_cancel_act_arr[0][0]))) {
                    /* End of the array reached. Action map is not found. */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_entry_action_present(unit,
                                                        cpu_cancel_act_arr[1][idx],
                                                        entry_oper, &found));
                if (found) {
                    *action_value <<= dst_ofset_current;
                }
            } else {
                *action_value = val;
            }
            break;
        case bcmFieldActionRpCosMapNew:
        case bcmFieldActionYpCosMapNew:
        case bcmFieldActionGpCosMapNew:
            /* retrieve field profile index from classifier ID */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_classifier_field_get(unit, params->param0, 0,
                                                     &val));
            *action_value = val;
            break;
        case bcmFieldActionCosMapNew:
            /* retrieve field profile index from classifier ID */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_classifier_field_get(unit, params->param0, 0,
                                                     &val));
            *action_value = val;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            if ((action == bcmFieldActionRpTimeStampToCpu) ||
                (action == bcmFieldActionGpTimeStampToCpu) ||
                (action == bcmFieldActionYpTimeStampToCpu) ||
                (action == bcmFieldActionRpCopyToCpu) ||
                (action == bcmFieldActionGpCopyToCpu) ||
                (action == bcmFieldActionYpCopyToCpu)) {
                if (params->param0) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            }
            for (idx = 0; idx < (lt_map_ref->num_maps); idx++) {
                val = 0;
                if (bcmiFieldActionParamNone == lt_map_ref->map[idx].param) {
                    /* Just enabling the action */
                    val = 1;
                } else if (bcmiFieldActionParam1 == lt_map_ref->map[idx].param) {
                    /* Match ID is valid only if param0 is non-zero */
                    if (params->param0) {
                        val = params->param1;
                    }
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
                *action_value |= (((val << dst_ofset_current)));
                dst_ofset_current +=  lt_map_ref->map[idx].width[0];
            }
            break;
        case bcmFieldActionServicePoolIdPrecedenceNew:
            for (idx = 0; idx < (lt_map_ref->num_maps); idx++) {
                val = 0;
                if (bcmiFieldActionParamNone == lt_map_ref->map[idx].param) {
                    val = params->param0;
                } else if (bcmiFieldActionParam1 == lt_map_ref->map[idx].param) {
                    switch (params->param1) {
                        case BCM_FIELD_COLOR_GREEN:
                            val = 0;
                            break;
                        case BCM_FIELD_COLOR_YELLOW:
                            val = 3;
                            break;
                        case BCM_FIELD_COLOR_RED:
                            val = 1;
                            break;
                        default:
                            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
                *action_value |= (((val << dst_ofset_current)));
                dst_ofset_current +=  lt_map_ref->map[idx].width[0];
            }
            break;
        case bcmFieldActionMatchPbmpRedirect:
             SHR_IF_ERR_VERBOSE_EXIT
                 (xgs_ltsw_field_action_profile_add(unit, stage,
                                                     action, params,
                                                     match_config,
                                                     action_value));
            *action_value |= ((params->param0) << lt_map_ref->map[0].width[0]);
            break;
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(params->param0)) {
                val = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(params->param0);
                if (-1 == val) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                *action_value = val;
                switch (action) {
                    case bcmFieldActionGpCosQNew:
                    case bcmFieldActionGpUcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_UCAST_COS;
                        break;
                    case bcmFieldActionYpCosQNew:
                    case bcmFieldActionYpUcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_YP_UCAST_COS;
                        break;
                    case bcmFieldActionRpCosQNew:
                    case bcmFieldActionRpUcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_RP_UCAST_COS;
                        break;
                    /* coverity[dead_error_begin]: FALSE*/
                    default:
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(params->param0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            } else {
                *action_value = params->param0;
            }
            break;
        case bcmFieldActionCosQNew:
        case bcmFieldActionUcastCosQNew:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(params->param0)) {
                val = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(params->param0);
                if (-1 == val) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                *action_value = val;
                entry_oper->action_flags |=
                                BCMINT_FIELD_ENTRY_ACTION_UCAST_COS;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(params->param0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            } else {
                *action_value = params->param0;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionMcastCosQNew:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(params->param0)) {
                val = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(params->param0);
                if (-1 == val) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                *action_value = val;
                entry_oper->action_flags |=
                                BCMINT_FIELD_ENTRY_ACTION_MCAST_COS;
            } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(params->param0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            } else {
                *action_value = params->param0;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_dup_val_in_lt_fields(unit, val,
                                                         lt_map_ref,
                                                         action_value));
            break;
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(params->param0)) {
                val = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(params->param0);
                if (-1 == val) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                *action_value = val;
                switch (action) {
                    case bcmFieldActionGpMcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_MCAST_COS;
                        break;
                    case bcmFieldActionYpMcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_YP_MCAST_COS;
                        break;
                    case bcmFieldActionRpMcastCosQNew:
                        entry_oper->action_flags |=
                                        BCMINT_FIELD_ENTRY_ACTION_RP_MCAST_COS;
                        break;
                    /* coverity[dead_error_begin]: FALSE*/
                    default:
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(params->param0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            } else {
                *action_value = params->param0;
            }
            break;
        case bcmFieldActionUnmodifiedPacketRedirectPort:
            if (BCM_GPORT_IS_MODPORT(params->param0)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_validate(unit, params->param0, &lport));
                *action_value = lport;
                entry_oper->action_flags |=
                               BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT;
            } else if (BCM_GPORT_IS_TRUNK(params->param0))  {
                *action_value = BCM_GPORT_TRUNK_GET(params->param0);
                entry_oper->action_flags |=
                               BCMINT_FIELD_ENTRY_ACTION_GPORT_TRUNK;
            } else {
                *action_value |= params->param1;
                entry_oper->action_flags |=
                               BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT;
            }
            break;
       case bcmFieldActionRedirectPort:
            if ((!BCM_GPORT_IS_MPLS_PORT(params->param1)) &&
                (!BCM_GPORT_IS_WLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_VLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_VXLAN_PORT(params->param1)) &&
                (!BCM_GPORT_IS_NIV_PORT(params->param1)) &&
                (!BCM_GPORT_IS_MIM_PORT(params->param1)) &&
                (!BCM_GPORT_IS_FLOW_PORT(params->param1)) &&
                (!BCM_GPORT_IS_MODPORT(params->param1))) {
                *action_value = params->param1;
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT;
                SHR_ERR_EXIT(SHR_E_NONE);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, params->param1, &lport));
            if (BCM_GPORT_IS_MPLS_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS;
            } else if (BCM_GPORT_IS_WLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN;
            } else if (BCM_GPORT_IS_VXLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN;
            } else if (BCM_GPORT_IS_VLAN_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN;
            } else if (BCM_GPORT_IS_NIV_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV;
            } else if (BCM_GPORT_IS_MIM_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM;
            } else if (BCM_GPORT_IS_FLOW_PORT(params->param1)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW;
            } else {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT;
            }
            *action_value = lport;
            break;
        case bcmFieldActionRedirectMcast:
            if ((0 == BCM_MULTICAST_IS_SET(params->param0)) ||
                (!(_BCM_MULTICAST_IS_L2(params->param0)))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = _BCM_MULTICAST_ID_GET(params->param0);
            break;
        case bcmFieldActionRedirectIpmc:
            if ((0 == BCM_MULTICAST_IS_SET(params->param0)) ||
                ((!(_BCM_MULTICAST_IS_L3(params->param0))) &&
                (!(_BCM_MULTICAST_IS_FLOW(params->param0))))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = _BCM_MULTICAST_ID_GET(params->param0);
            if (_BCM_MULTICAST_IS_FLOW(params->param0)) {
                entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_REDIRECT_IPMC_FLOW;
            }
            break;
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchAndResetOverlayNextHop:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_l3_egress_get(unit, params->param0,
                                   &egr));
            if (egr.flags2 & BCM_L3_FLAGS2_FIELD_ONLY) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                           &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
                  (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY;
            } else {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY;
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionRedirectEgrNextHopAndResetOverlayNextHop:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_l3_egress_get(unit, params->param0,
                                   &egr));
	        if (!(((params->param1 == bcmFieldRedirectTypeAll) &&
                 (egr.flags2 & BCM_L3_FLAGS2_FIELD_ONLY)) ||
                ((params->param1 == bcmFieldRedirectTypeL3) &&
                 !(egr.flags2 & BCM_L3_FLAGS2_FIELD_ONLY)))) {
               return BCM_E_PARAM;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                           &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
                  (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY;
            } else {
                entry_oper->action_flags |=
                   BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY;
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionChangeL2Fields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_l3_egress_get(unit, params->param0,
                                        &egr));
            if (!(egr.flags2 & BCM_L3_FLAGS2_FIELD_ONLY)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_resolve(unit, params->param0,
                                           &egr_obj_idx, &egr_obj_type));
            /* Check supported obj type */
            if (!(egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = egr_obj_idx;
            break;
        case bcmFieldActionLoopbackSrcPortGportNew:
            if (BCM_GPORT_IS_MODPORT(params->param0)) {
                mod = BCM_GPORT_MODPORT_MODID_GET(params->param0);
                lport = BCM_GPORT_MODPORT_PORT_GET(params->param0);
                /* Only Local Mod Port need to be supported. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_modid_is_local(unit, mod, &isLocal));
                if (!isLocal) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *action_value = lport;
            break;
        case bcmFieldActionOuterTpidNew:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_tpid_hw_encode(unit,
                                                   params->param0,
                                                   action_value));
            break;
        case bcmFieldActionColorIndependent:
            if (params->param0)  {
                *action_value = 1;
                entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_COLOR_INDEPENDENT;
            } else {
                entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_COLOR_INDEPENDENT;
            }
            break;
        case bcmFieldActionGpStatGroup:
            skip_color[0] = 1;
            *action_value = params->param0 + 1;
            break;
        case bcmFieldActionYpStatGroup:
            skip_color[1] = 1;
            *action_value = params->param0 + 1;
            break;
        case bcmFieldActionRpStatGroup:
            skip_color[2] = 1;
            *action_value = params->param0 + 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

    /* If the stat is attached. */
    if ((stage_info->flags & BCMINT_FIELD_STAGE_ENTRY_COLOR_STAT_SUPPORT) &&
        (entry_oper->flexctr_action_id) &&
        ((action == bcmFieldActionGpStatGroup) ||
         (action == bcmFieldActionYpStatGroup) ||
         (action == bcmFieldActionRpStatGroup))) {
        int a, c;
        bcmi_field_action_t iaction[3] = {bcmiFieldActionGpStatGroup,
                                          bcmiFieldActionYpStatGroup,
                                          bcmiFieldActionRpStatGroup};

        for (a = 0; a < entry_oper->num_actions; a++) {
            if (entry_oper->action_arr[a] == bcmFieldActionGpStatGroup) {
                skip_color[0] = 1;
            } else if (entry_oper->action_arr[a] == bcmFieldActionYpStatGroup) {
                skip_color[1] = 1;
            } else if (entry_oper->action_arr[a] == bcmFieldActionRpStatGroup) {
                skip_color[2] = 1;
            }
        }

        for (c = 0; c < 3; c++) {
            if (skip_color[c] == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_action_flexctr_internal_color_set(
                                       unit, NULL,
                                       stage_info, entry_oper,
                                       iaction[c], 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_value_get(int unit,
                    bcm_field_entry_t entry_id,
                    bcmi_ltsw_field_stage_t stage,
                    bcm_field_action_t action,
                    bcm_field_action_params_t *params,
                    bcm_field_action_match_config_t *match_config,
                    uint64_t *action_value)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    uint32 dst_ofset_current = 0;
    int gport = 0, val = 0;
    uint8_t idx = 0;
    uint16_t tpid = 0;
    bcm_if_t if_id = 0;
    bcm_gport_t dest_gport = 0;
    bcmi_ltsw_gport_info_t gport_info;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

   /* convert the sdk6 action values to sdklt action values */
   /* Do switch case based on
    * Misc actions which needs convertions or chip specific.
    */
    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

    switch (action) {
        case bcmFieldActionIntCongestionNotification:
        case bcmFieldActionRpIntCongestionNotificationNew:
        case bcmFieldActionYpIntCongestionNotificationNew:
        case bcmFieldActionGpIntCongestionNotificationNew:
        case bcmFieldActionDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionEcnNew:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionGpPrioIntNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            params->param0 = *action_value;
            break;
        case bcmFieldActionDscpPreserve:
        case bcmFieldActionDot1pPreserve:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            if (*action_value) {
                params->param0 = 0;
            }
            break;
        case bcmFieldActionRpCosMapNew:
        case bcmFieldActionYpCosMapNew:
        case bcmFieldActionGpCosMapNew:
        case bcmFieldActionCosMapNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            /* retrieve field profile index from classifier ID */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_classifier_field_set(unit, *action_value, 0,
                                                     (int *)&(params->param0)));
            break;
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
            for (idx = 0;
                    (idx < lt_map_ref->num_maps)
                    && (bcmiFieldActionParamNone == lt_map_ref->map[idx].param);
                    idx++) {
                    dst_ofset_current +=  lt_map_ref->map[idx].width[0];
            }
            val = ((*action_value) >> dst_ofset_current);
            if (val) {
                params->param0 = 1;
                params->param1 = val;
            }
            break;
        case bcmFieldActionServicePoolIdPrecedenceNew:
            for (idx = 0;
                    (idx < lt_map_ref->num_maps)
                    && (bcmiFieldActionParamNone == lt_map_ref->map[idx].param);
                    idx++) {
                    dst_ofset_current +=  lt_map_ref->map[idx].width[0];
            }
            params->param0 = *action_value & ((1 << dst_ofset_current) - 1);
            val = ((*action_value) >> dst_ofset_current);
            switch (val) {
                case 0:
                    params->param1 = BCM_FIELD_COLOR_GREEN;
                    break;
                case 3:
                    params->param1 = BCM_FIELD_COLOR_YELLOW;
                    break;
                case 1:
                    params->param1 = BCM_FIELD_COLOR_RED;
                    break;
                default:
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
            break;
        case bcmFieldActionMatchPbmpRedirect:
            params->param0 = ((*action_value) >> lt_map_ref->map[0].width[0]);
            val = (((1 << lt_map_ref->map[0].width[0]) - 1) & (*action_value));
            *action_value = val;
            SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_field_action_profile_get(unit, stage,
                                                action,
                                                action_value,
                                                params,
                                                match_config));
            break;
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionCosQNew:
        case bcmFieldActionUcastCosQNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_UCAST_COS) {
                BCM_GPORT_UCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionYpUcastCosQNew:
            dst_ofset_current =  lt_map_ref->map[0].width[0];
            *action_value &= ((1 << dst_ofset_current) - 1);
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_YP_UCAST_COS) {
                BCM_GPORT_UCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
        case bcmFieldActionRpUcastCosQNew:
        case bcmFieldActionRpCosQNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_RP_UCAST_COS) {
                BCM_GPORT_UCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_MCAST_COS) {
                BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
        case bcmFieldActionYpMcastCosQNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_YP_MCAST_COS) {
                BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
        case bcmFieldActionRpMcastCosQNew:
            if (lt_map_ref->map[0].width[0]) {
                dst_ofset_current =  lt_map_ref->map[0].width[0];
                *action_value &= ((1 << dst_ofset_current) - 1);
            }
            val = *action_value;
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_RP_MCAST_COS) {
                BCM_GPORT_MCAST_QUEUE_GROUP_SET(gport, val);
                params->param0 = gport;
            } else {
                params->param0 = val;
            }
            break;
       case bcmFieldActionUnmodifiedPacketRedirectPort:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT) {
                gport_info.port = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT) {
                params->param1 = *action_value;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_TRUNK) {
                gport_info.tgid = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            if (!(entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_construct(unit,
                                                    &gport_info,
                                                    &dest_gport));
                params->param0 = dest_gport;
            }
            break;
       case bcmFieldActionRedirectPort:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS) {
                gport_info.mpls_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN) {
                gport_info.wlan_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN) {
                gport_info.vxlan_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN) {
                gport_info.vlan_vp_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV) {
                gport_info.niv_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM) {
                gport_info.mim_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW) {
                gport_info.flow_id = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
            } else if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT) {
                gport_info.port = *action_value;
                gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
            } else {
                if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT) {
                    params->param1 = *action_value;
                    SHR_ERR_EXIT(BCM_E_NONE);
                } else {
                    SHR_ERR_EXIT(BCM_E_NOT_FOUND);
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_construct(unit, &gport_info, &dest_gport));
            params->param0 = dest_gport;
            break;
        case bcmFieldActionL3Switch:
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionL3SwitchAndResetOverlayNextHop:
        case bcmFieldActionRedirectEgrNextHopAndResetOverlayNextHop:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                            BCMI_LTSW_L3_EGR_OBJ_T_UL, &if_id));

            } else if (entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                        BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL, &if_id));

            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
            params->param0 = if_id;
            break;
        case bcmFieldActionChangeL2Fields:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_id_construct(unit, *action_value,
                                            BCMI_LTSW_L3_EGR_OBJ_T_UL, &if_id));
            params->param0 = if_id;
            break;
        case bcmFieldActionRedirectMcast:
            _BCM_MULTICAST_GROUP_SET(params->param0,
                    _BCM_MULTICAST_TYPE_L2, *action_value);
            break;
        case bcmFieldActionRedirectIpmc:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_REDIRECT_IPMC_FLOW) {
                _BCM_MULTICAST_GROUP_SET(params->param0,
                        _BCM_MULTICAST_TYPE_FLOW, *action_value);
            } else {
                _BCM_MULTICAST_GROUP_SET(params->param0,
                        _BCM_MULTICAST_TYPE_L3, *action_value);
            }
            break;
        case bcmFieldActionLoopbackSrcPortGportNew:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_gport_get(unit, *action_value,
                                         &gport));
            params->param0 = gport;
            break;
        case bcmFieldActionOuterTpidNew:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_int_ltsw_field_tpid_hw_decode(unit,
                                                   *action_value,
                                                   &tpid));
            params->param0 = tpid;
            break;
        case bcmFieldActionGpStatGroup:
        case bcmFieldActionYpStatGroup:
        case bcmFieldActionRpStatGroup:
            params->param0 = *action_value - 1;
            break;

        default:
            SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_valid_invalid_index_set(
       int unit,
       int index,
       uint16_t lt_map_flags,
       bool cond_matched,
       bool *lt_field_map)
{
    SHR_FUNC_ENTER(unit);
    if (lt_map_flags & BCMINT_FIELD_ENTRY_ACTION_VALID_LT_FIELDS_SET) {
        lt_field_map[index] = cond_matched;
    } else if (lt_map_flags & BCMINT_FIELD_ENTRY_ACTION_INVALID_LT_FIELDS_SET) {
        lt_field_map[index] = !cond_matched;
    } else {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_lt_map_valid_info_get(
       int unit,
       bcmi_ltsw_field_stage_t stage,
       bcm_field_entry_t entry_id,
       bcm_field_action_t action,
       uint16_t lt_map_flags,
       uint16_t map_count,
       bool *lt_field_map)
{
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    char *nhop_fid = NULL;
    char *ecmp_fid = NULL;
    char *action_fid = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    int iter = 0;
    SHR_FUNC_ENTER(unit);

    /* Get stage info from stage ID . */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    if (map_count > lt_map_ref->num_maps) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }
    switch (action) {
        case bcmFieldActionL3Switch:
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionL3SwitchAndResetOverlayNextHop:
        case bcmFieldActionRedirectEgrNextHopAndResetOverlayNextHop:
            if ((action == bcmFieldActionL3Switch)||
                (action == bcmFieldActionL3SwitchAndResetOverlayNextHop)) {
                nhop_fid = stage_info->tbls_info->action_l3_switch_nhop_fid;
                ecmp_fid = stage_info->tbls_info->action_l3_switch_ecmp_fid;
            } else if ((action == bcmFieldActionRedirectEgrNextHop) ||
                      (action ==
                       bcmFieldActionRedirectEgrNextHopAndResetOverlayNextHop)) {
                nhop_fid = stage_info->tbls_info->action_redirect_egr_nhop_fid;
                ecmp_fid = stage_info->tbls_info->action_redirect_egr_ecmp_fid;
            }
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_OVERLAY) {
                action_fid = nhop_fid;
            } else if (entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_EGR_OBJ_ECMP_GROUP_OVERLAY) {
                action_fid = ecmp_fid;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case bcmFieldActionUnmodifiedPacketRedirectPort:
            if (entry_oper->action_flags &
                    BCMINT_FIELD_ENTRY_ACTION_GPORT_TRUNK) {
                action_fid =
                    stage_info->tbls_info->action_unmod_redirect_to_trunk_fid;
            } else if (entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_GPORT_MODPORT) {
                action_fid =
                    stage_info->tbls_info->action_unmod_redirect_to_port_fid;
            } else if (entry_oper->action_flags &
                        BCMINT_FIELD_ENTRY_ACTION_GPORT_PORT) {
                action_fid =
                    stage_info->tbls_info->action_unmod_redirect_to_port_fid;
            } else {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        default:
            SHR_EXIT();
    }

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (0 == (sal_strcmp((lt_map_ref->map[iter]).lt_field_name,
                        action_fid))) {
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcmint_ltsw_field_valid_invalid_index_set(unit,
                        iter,
                        lt_map_flags,
                        TRUE,
                        lt_field_map));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(
                    bcmint_ltsw_field_valid_invalid_index_set(unit,
                        iter,
                        lt_map_flags,
                        FALSE,
                        lt_field_map));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_mirror_cont_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry_id,
    bcm_field_action_t action,
    int mirror_cont,
    char **cont_name)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cont_name, SHR_E_PARAM);

    if (stage_info->tbls_info->action_mirror_cont_id_fid == NULL) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    *cont_name = stage_info->tbls_info->action_mirror_cont_id_fid;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_field_mirror_idx_to_fp_cont_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry_id,
    const bcm_field_action_map_t *lt_map_ref,
    int mtp_idx,
    int *mirror_cont)
{
    int iter = 0;
    uint64_t val = 0;
    uint32 num_element = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    bcmlt_field_def_t fld_defs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_map_ref, SHR_E_PARAM);
    SHR_NULL_CHECK(mirror_cont, SHR_E_PARAM);

    if (stage_info->stage_id != bcmiFieldStageIngress) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Get the entry hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, stage_info->tbls_info->policy_sid,
                               (lt_map_ref->map[0]).lt_field_name, &fld_defs));

    for (iter = 0; (iter < fld_defs.elements); iter++) {
        /* Lookup the policy table */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &policy_template,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->policy_sid,
                                          stage_info->tbls_info->policy_key_fid,
                                          0, entry_id & BCM_FIELD_ID_MASK, NULL,
                                          NULL,
                                          0,
                                          0,
                                          0,
                                          0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(policy_template,
                                         (lt_map_ref->map[0]).lt_field_name,
                                         iter,
                                         &val,
                                         1, &num_element));
        if (val == mtp_idx) {
            break;
        }
    }

    if (iter == fld_defs.elements) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Return FP container ID */
    *mirror_cont = iter;

exit:
    if (BCMLT_INVALID_HDL != policy_template) {
       (void) bcmlt_entry_free(policy_template);
       policy_template = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_mirror_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry_id,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    int iter = 0;
    int flags = 0x0;
    int mirror_cont = -1;
    char *cont_field = NULL;
    bcmi_ltsw_mirror_info_t mirror_info;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    /* action validation */
    if (action == bcmFieldActionMirrorIngress) {
        flags = BCMI_LTSW_MIRROR_INGRESS;
    } else if (action == bcmFieldActionMirrorEgress) {
        flags = BCMI_LTSW_MIRROR_EGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action, stage_info, &lt_map_ref));

    sal_memset(&mirror_info, 0x0, sizeof(mirror_info));
    /* Mirror dest container alloc */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_add(unit, entry_id, flags,
                                         mirror_gport,
                                         &mirror_info));

    mirror_cont = mirror_info.mirror_cont;

    /* Retrieve Policy Mirror Container */
    if (stage_info->tbls_info->action_mirror_cont_id_fid == NULL) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Lookup the policy table */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_lt_entry_commit(unit, &policy_template,
                                     BCMLT_OPCODE_LOOKUP,
                                     BCMLT_PRIORITY_NORMAL,
                                     stage_info->tbls_info->policy_sid,
                                     stage_info->tbls_info->policy_key_fid,
                                     0, entry_id & BCM_FIELD_ID_MASK, NULL,
                                     NULL,
                                     0,
                                     0,
                                     0,
                                     0));

    cont_field = stage_info->tbls_info->action_mirror_cont_id_fid;

    /* set mirror index in the policy table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &policy_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->policy_sid,
                                      stage_info->tbls_info->policy_key_fid,
                                      0, entry_id & BCM_FIELD_ID_MASK, NULL,
                                      cont_field,
                                      (uint64_t)(mirror_info.mirror_index),
                                      mirror_cont, 1,
                                      0));

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        /* Set internal maps associated if any. */
        if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_internal_map_set(
                                 unit,
                                 1, action, policy_template,
                                 policy_template,
                                 stage_info,
                                 &(lt_map_ref->map[iter]),
                                 mirror_cont, 1, 0, NULL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, policy_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
        }
    }

exit:
    if (BCMLT_INVALID_HDL != policy_template) {
       (void) bcmlt_entry_free(policy_template);
       policy_template = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_mirror_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_gport_t mirror_gport)
{
    int iter = 0;
    int flags = 0x0;
    char *cont_field;
    int mirror_cont = -1;
    bcmi_ltsw_mirror_info_t mirror_info;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* action validation */
    if (action == bcmFieldActionMirrorIngress) {
        flags = BCMI_LTSW_MIRROR_INGRESS;
    } else if (action == bcmFieldActionMirrorEgress) {
        flags = BCMI_LTSW_MIRROR_EGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&mirror_info, 0x0, sizeof(mirror_info));

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action, stage_info, &lt_map_ref));

    /* Get Mirror dest container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_get(unit, entry, flags,
                                         mirror_gport,
                                         &mirror_info));

    /* Delete Mirror dest container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_field_dest_delete(unit, entry, flags,
                                            mirror_gport));

    /* Retrieve Policy Mirror Container */
    if (stage_info->tbls_info->action_mirror_cont_id_fid == NULL) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    cont_field = stage_info->tbls_info->action_mirror_cont_id_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_field_mirror_idx_to_fp_cont_get(unit,
                                                  stage_info,
                                                  entry & BCM_FIELD_ID_MASK,
                                                  lt_map_ref,
                                                  (int)mirror_info.mirror_index,
                                                  &mirror_cont));

    /* set mirror index to 0 in the policy table */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_lt_entry_commit(unit, &policy_template,
                                     BCMLT_OPCODE_UPDATE,
                                     BCMLT_PRIORITY_NORMAL,
                                     stage_info->tbls_info->policy_sid,
                                     stage_info->tbls_info->policy_key_fid,
                                     0, entry & BCM_FIELD_ID_MASK, NULL,
                                     cont_field,
                                     0, mirror_cont,
                                     1, 0));

    if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_field_lt_entry_commit(unit, &policy_template,
                                         BCMLT_OPCODE_LOOKUP,
                                         BCMLT_PRIORITY_NORMAL,
                                         stage_info->tbls_info->policy_sid,
                                         stage_info->tbls_info->policy_key_fid,
                                         0, entry & BCM_FIELD_ID_MASK, NULL,
                                         NULL,
                                         0,
                                         0, 0,
                                         0));
    }

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        /* Set internal maps associated if any. */
        if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_internal_map_set(
                                      unit,
                                      0, action, policy_template,
                                      policy_template,
                                      stage_info,
                                      &(lt_map_ref->map[iter]),
                                      mirror_cont, 1, 0, NULL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, policy_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
        }
    }

exit:
    if (BCMLT_INVALID_HDL != policy_template) {
       (void) bcmlt_entry_free(policy_template);
       policy_template = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_mirror_index_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index)
{
    char *cont_field;
    int fp_mtp_cont = -1;
    uint64_t mirror_ena = 0;
    uint64_t mirror_idx = 0;
    uint32_t num_element = 0;
    bcmlt_field_def_t fld_defs;
    bcmlt_entry_handle_t policy_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mirror_index, SHR_E_PARAM);

    /* Retrieve Policy Mirror Container */
    if (stage_info->tbls_info->action_mirror_cont_id_fid == NULL) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    cont_field = stage_info->tbls_info->action_mirror_cont_enable_fid;

    fp_mtp_cont = mirror_cont;

    /* Perform policy lookup */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &policy_template,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->policy_sid,
                                      stage_info->tbls_info->policy_key_fid,
                                      0, entry & BCM_FIELD_ID_MASK, NULL,
                                      NULL,
                                      0, 0, 0,
                                      0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, stage_info->tbls_info->policy_sid,
                               cont_field, &fld_defs));

    if (fld_defs.elements > 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(policy_template,
                                         cont_field,
                                         fp_mtp_cont,
                                         &mirror_ena,
                                         1, &num_element));
        if (COMPILER_64_LO(mirror_ena) == 1) {
            *enable = true;
            cont_field = stage_info->tbls_info->action_mirror_cont_id_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(policy_template,
                                             cont_field,
                                             fp_mtp_cont,
                                             &mirror_idx,
                                             1, &num_element));
        }
    } else {
        /* Get Mirror index for a given mirror container */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(policy_template,
                                   cont_field,
                                   &mirror_idx));
    }

    *mirror_index = (int)(COMPILER_64_LO(mirror_idx));

exit:
    if (policy_template != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(policy_template);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_action_default_value_get(
        int unit, bcm_field_entry_t entry_id,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out)
{
    const bcm_field_action_map_t *lt_map_ref = NULL;
    uint8_t idx = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bool found = 0;
    bcm_field_action_t cpu_cancel_act_arr[2][8] = {
        {
            bcmFieldActionGpCopyToCpuCancel,
            bcmFieldActionYpCopyToCpuCancel,
            bcmFieldActionRpCopyToCpuCancel,
            bcmFieldActionCopyToCpuCancel,
            bcmFieldActionGpSwitchToCpuCancel,
            bcmFieldActionYpSwitchToCpuCancel,
            bcmFieldActionRpSwitchToCpuCancel,
            bcmFieldActionSwitchToCpuCancel
        },
        {
            bcmFieldActionGpSwitchToCpuCancel,
            bcmFieldActionYpSwitchToCpuCancel,
            bcmFieldActionRpSwitchToCpuCancel,
            bcmFieldActionSwitchToCpuCancel,
            bcmFieldActionGpCopyToCpuCancel,
            bcmFieldActionYpCopyToCpuCancel,
            bcmFieldActionRpCopyToCpuCancel,
            bcmFieldActionCopyToCpuCancel
        }
    };

    SHR_FUNC_ENTER(unit);

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info, &entry_oper));

    /*
     * convert the sdk6 action values to sdklt action values
     * Do switch case based on
     * Misc actions which needs convertions or chip specific.
     */
    switch (action) {
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionYpSwitchToCpuCancel:
        case bcmFieldActionRpSwitchToCpuCancel:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
            if (lt_map_ref->num_maps > 1) {
                /* Find the mapping of the CPU Cancel action in the array . */
                for (idx = 0;
                     (idx < (sizeof(cpu_cancel_act_arr[0])/sizeof(cpu_cancel_act_arr[0][0])))
                        && (cpu_cancel_act_arr[0][idx] != action); idx++) {
                    /* Dont do anything here. Loop ends when the action is found. */
                }

                if (idx == (sizeof(cpu_cancel_act_arr[0])/sizeof(cpu_cancel_act_arr[0][0]))) {
                    /* End of the array reached. Action map is not found. */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_field_entry_action_present(unit,
                                                cpu_cancel_act_arr[1][idx],
                                                entry_oper, &found));
                if (del_action && found) {
                    for (idx = lt_map_ref->num_maps - (lt_map_ref->num_maps/3);
                            idx < lt_map_ref->num_maps; idx++) {
                        fld_defs_out[idx].def = 1;
                    }
                }
            }
            break;
        default:
            SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    int num_bits_for_port;
    uint32 mod_port_data = 0;   /* concatenated modid and port */
    uint32 mod_port_mask = 0;   /* concatenated modid and port */
    uint32 *mod_port_data_get = &mod_port_data;   /* concatenated modid and port */
    uint32 *mod_port_mask_get = &mod_port_mask;   /* concatenated modid and port */

    bcm_qual_field_t qual_info;
    bcm_gport_t gport;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    if ((qual != bcmFieldQualifySrcPort) &&
        (qual != bcmFieldQualifyDstPort) &&
        (qual != bcmFieldQualifySrcTrunkMemberGport) &&
        (qual != bcmFieldQualifySrcModPortGports) &&
        (qual != bcmFieldQualifySrcModPortGport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &mod_port_data_get;
    qual_info.qual_mask = (void *) &mod_port_mask_get;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));
    num_bits_for_port = _shr_popcount(
                        (unsigned int)bcmi_ltsw_dev_max_port_addr(unit));
    *data_modid =  (*mod_port_data_get) >> num_bits_for_port;
    *mask_modid =  (*mod_port_mask_get) >> num_bits_for_port;

    *data_port = (*mod_port_data_get) & ((1 << num_bits_for_port) - 1);
    *mask_port = (*mod_port_mask_get) & ((1 << num_bits_for_port) - 1);
    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_stk_modmap_map(unit,
            BCM_STK_MODMAP_GET, *data_modid, *data_port,
            data_modid, data_port));
    if (*mask_modid == bcmi_ltsw_dev_max_modid(unit)) {
        *mask_modid = BCM_FIELD_EXACT_MATCH_MASK;
    }
    if (*mask_port == bcmi_ltsw_dev_max_port_addr(unit)) {
        *mask_port = BCM_FIELD_EXACT_MATCH_MASK;
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info,
                                        &entry_oper));

    if (((qual == bcmFieldQualifySrcPort) &&
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT)) ||
        ((qual == bcmFieldQualifyDstPort) &&
        (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT))) {
        BCM_GPORT_MODPORT_SET(gport, *data_modid, *data_port);
        *data_port = gport;
        *data_modid = 0;
    }
exit:
    SHR_FUNC_EXIT()
}

int
xgs_ltsw_field_qualify_port_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    bcm_qual_field_t qual_info;
    bcm_gport_t gport; /*temp gport and mask */
    int num_bits_for_port;
    int id =0;
    bcm_trunk_t trunk_id = 0;
    uint32 mod_port_data = 0;   /* concatenated modid and port */
    uint32 mod_port_mask = 0;   /* concatenated modid and port */

    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bool is_gport = false;


    SHR_FUNC_ENTER(unit);

    if ((qual != bcmFieldQualifySrcPort) &&
        (qual != bcmFieldQualifyDstPort) &&
        (qual != bcmFieldQualifySrcTrunkMemberGport) &&
        (qual != bcmFieldQualifySrcModPortGports) &&
        (qual != bcmFieldQualifySrcModPortGport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info,
                                        &entry_oper));

    if (BCM_GPORT_IS_SET(data_port)) {
        if (!(BCM_GPORT_IS_MODPORT(data_port))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        mask_modid = BCM_FIELD_EXACT_MATCH_MASK;
        gport = data_port;
        is_gport = true;
    } else {
        BCM_GPORT_MODPORT_SET(gport, data_modid, data_port);
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmi_ltsw_port_gport_resolve(unit, gport,
                                  &data_modid, &data_port, &trunk_id, &id));

    /* Set masks to all ones if exact match is specified. */
    if (mask_modid == BCM_FIELD_EXACT_MATCH_MASK) {
        mask_modid = bcmi_ltsw_dev_max_modid(unit);
    }
    if (mask_port == BCM_FIELD_EXACT_MATCH_MASK) {
        mask_port = bcmi_ltsw_dev_max_port_addr(unit);
    }
    num_bits_for_port = _shr_popcount((unsigned int)bcmi_ltsw_dev_max_port_addr(unit));
    mod_port_data = (data_modid << num_bits_for_port) | data_port;
    mod_port_mask = (mask_modid << num_bits_for_port) | mask_port;

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_PORT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &mod_port_data;
    qual_info.qual_mask = (void *) &mod_port_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));
    if (is_gport) {
        if (qual == bcmFieldQualifySrcPort) {
            entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT;
        } else {
            entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT;
        }
    } else {
        if (qual == bcmFieldQualifySrcPort) {
            entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_SRC_PORT_TYPE_GPORT;
        } else {
            entry_oper->entry_flags &= ~BCMINT_FIELD_ENTRY_DST_PORT_TYPE_GPORT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_entry_compr_qual_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        uint32_t grp_flags,
        bcm_field_qset_t *qset,
        uint32_t *compr_flags)
{
    bool all_enabled = false;
    SHR_FUNC_ENTER(unit);

    if (!(grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED) &&
        !(grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED)) {
        *compr_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        all_enabled = TRUE;
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
        if ((all_enabled == TRUE) ||
            (grp_flags & BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED)) {
            *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_field_group_compr_qual_validate(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        uint32_t grp_flags,
        bcm_field_qset_t *qset,
        uint32_t *compr_flags)
{
    SHR_FUNC_ENTER(unit);

    if (!(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP6_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP_COMPRESSION) &&
        !(grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP6_COMPRESSION)) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) &&
            !BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        *compr_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT |
                       BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        SHR_EXIT();
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_SRC_IP6_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
    }

    if (grp_flags & BCM_FIELD_GROUP_CREATE_WITH_DST_IP6_COMPRESSION) {
        if (!BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        *compr_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_compression_set(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bcm_field_qset_t *qset,
        uint32_t group_flags,
        bcmlt_entry_handle_t templ,
        uint32_t *c_flags)
{
    int iter;
    uint8_t num_types = 0;
    uint32_t c_valid_flags = 0x0;
    bcm_field_qualify_t qual[4] = {bcmFieldQualifySrcIp,
                                   bcmFieldQualifyDstIp,
                                   bcmFieldQualifySrcIp6,
                                   bcmFieldQualifyDstIp6};
    int compr_flags[4] = {BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT,
                          BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT};
    const char *comp_data[4] = {0};
    char *comp_type[4] = {"SRC_IPV4", "DST_IPV4", "SRC_IPV6", "DST_IPV6"};

    SHR_FUNC_ENTER(unit);

    if (templ == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (entry_oper == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_group_compr_qual_validate(unit, stage_info,
                                                  group_flags,
                                                  qset, &c_valid_flags));
    } else {
        bcmi_field_ha_group_oper_t *group_oper;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                            stage_info,
                                            &group_oper));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_entry_compr_qual_validate(unit, stage_info,
                                                  group_oper->group_flags,
                                                  qset, &c_valid_flags));
    }


    for (iter = 0; iter < 4; iter++) {
        if (BCM_FIELD_QSET_TEST(*qset, qual[iter])) {
            if (!(c_valid_flags & compr_flags[iter])) {
                continue;
            }
            comp_data[num_types++] = comp_type[iter];
            /* Update flags as per qset */
            *c_flags |= compr_flags[iter];
        } else if ((entry_oper != NULL) &&
                   (entry_oper->entry_flags & compr_flags[iter])) {
            comp_data[num_types++] = comp_type[iter];
            /* Update flags as per qset */
            *c_flags |= compr_flags[iter];
        }
    }

    if (num_types > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(
                  templ, stage_info->tbls_info->compression_type_fid,
                  0, comp_data, num_types));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(templ,
                stage_info->tbls_info->compression_type_cnt_fid,
                num_types));
    }
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_compression_del(
        int unit,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_qset_t *qset,
        bcmlt_entry_handle_t templ)
{
    int iter;
    int cnt = 0;
    uint64_t n_types = 0;
    uint32_t num_types = 0;
    bcm_field_qualify_t qual[4] = {bcmFieldQualifySrcIp,
                                   bcmFieldQualifyDstIp,
                                   bcmFieldQualifySrcIp6,
                                   bcmFieldQualifyDstIp6};
    const char *comp_data[4] = {0};
    char *comp_field = NULL;
    char *comp_type[4] = {"SRC_IPV4", "DST_IPV4", "SRC_IPV6", "DST_IPV6"};

    SHR_FUNC_ENTER(unit);

    if (templ == BCMLT_INVALID_HDL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (iter = 0; iter < 4; iter++) {
        if (BCM_FIELD_QSET_TEST(*qset, qual[iter])) {
            comp_field = comp_type[iter];
            break;
        }
    }

    if (comp_field == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(templ,
            stage_info->tbls_info->compression_type_cnt_fid,
            &n_types));

    if (n_types == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_get(
              templ,
              stage_info->tbls_info->compression_type_fid,
              0,
              comp_data,
              4,
              &num_types));
    cnt = 0;
    for (iter = 0; iter < n_types; iter++) {
         comp_data[cnt] = comp_data[iter];
         if (sal_strcmp(comp_data[iter], comp_field)) {
             cnt++;
         }
    }
    n_types = cnt;

    if (cnt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(
                templ,
                stage_info->tbls_info->compression_type_fid,
                0,
                comp_data,
                cnt));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(templ,
                               stage_info->tbls_info->compression_type_cnt_fid,
                               n_types));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qualify_dstl3egress_set(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t if_id)
{
    int mask = 0;
    int egr_obj_idx = 0;
    bcm_qual_field_t qual_info;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    mask = -1;

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, if_id,
                                        &egr_obj_idx, &egr_obj_type));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3Egress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_OL) {
        entry_oper->entry_flags |=
                              BCMINT_FIELD_ENTRY_EGR_OBJ_OVERLAY;
    } else if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL){
        entry_oper->entry_flags |=
                    BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY;
    } else if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_UNDERLAY;
    } else if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qualify_dstl3egress_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_if_t *if_id)
{
    int mask = 0;
    int *mask_get = &mask;
    int egr_obj_idx = 0;
    int *egr_obj_idx_get = &egr_obj_idx;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3Egress;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_OBJ_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                                    BCMI_LTSW_L3_EGR_OBJ_T_OL, if_id));
    } else if (entry_oper->entry_flags &
                       BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                               BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL, if_id));
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_OBJ_UNDERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                                      BCMI_LTSW_L3_EGR_OBJ_T_UL, if_id));
    } else if (entry_oper->entry_flags &
                       BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, egr_obj_idx,
                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL, if_id));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qual_info_lt_name_get(
    int unit,
    const char *lt_field_name,
    char *num_offset_field_name,
    char *offset_field_name,
    char *width_field_name) {

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(num_offset_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(offset_field_name, SHR_E_INTERNAL);
    SHR_NULL_CHECK(width_field_name, SHR_E_INTERNAL);

    sal_strcpy(num_offset_field_name, "NUM_");
    sal_strcat(num_offset_field_name, lt_field_name);
    sal_strcat(num_offset_field_name, "_INFO");

    sal_strcat(offset_field_name, lt_field_name);
    sal_strcat(offset_field_name, "_OFFSET");

    sal_strcat(width_field_name, lt_field_name);
    sal_strcat(width_field_name, "_WIDTH");

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_field_qualify_SrcTrunk(int unit, bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t data, bcm_trunk_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    if (BCM_GPORT_IS_MODPORT(data)) {
        data = BCM_GPORT_MODPORT_PORT_GET(data);
        if (mask != BCM_FIELD_EXACT_MATCH_MASK) {
            mask = BCM_GPORT_MODPORT_PORT_GET(mask);
        }
    } else {
        data = (uint32_t)(data) | (1 << _XGS_FIELD_TRUNK_BIT_POS);
    }
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_qualify_SrcTrunk_get(int unit, bcm_field_entry_t entry,
                                    bcm_field_qualify_t qual, bool *is_trunk,
                                    bcm_trunk_t *data, bcm_trunk_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcTrunk;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_TRUNK;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    if (NULL != is_trunk) {
        *is_trunk = (((uint32_t)(*data)) &
                       (1 << _XGS_FIELD_TRUNK_BIT_POS)) ? true : false;
    }

    *data = (uint32_t)(*data) & ((1 << _XGS_FIELD_TRUNK_BIT_POS) - 1);

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_field_aacl_class_id_get(
        int unit,
        bcm_field_qualify_t qid,
        bcmint_field_stage_info_t *stage_info,
        bcmi_field_ha_entry_oper_t *entry_oper,
        int *class_id,
        bcm_field_qualify_t *pair_qual)
{
    bool is_ip6 = 0;
    bool is_ip = 0;
    bool is_pair_valid = 0;
    bool is_qual_valid = 0;
    int l4port = 0;
    int l4port_mask = 0;
    bcm_ip_t ip = 0x0;
    bcm_ip_t ip_mask = 0x0;
    bcm_ip6_t ip6;
    bcm_ip6_t ip6_mask;
    bcm_qual_field_t qual_info;
    bcm_qual_field_t pair_qual_info;
    bcmlt_entry_handle_t aacl_template_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t aacl_control_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    char *aacl_table_name = NULL;
    uint64_t class_id_val = 0;
    uint64_t ip6_val = 0, mac_val = 0;
    bool src_nonip_valid = FALSE;
    bool dst_nonip_valid = FALSE;
    uint32_t vrf = 0;
    uint32_t vrf_mask = 0;
    uint8_t ipproto = 0, ipproto_mask = 0;
    uint8_t ipflags = 0, ipflags_mask = 0;
    uint8_t tcp_control = 0, tcp_control_mask = 0;
    bcm_mac_t mac = {0}, mac_mask = {0};
    uint64_t comp_mode = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(entry_oper, SHR_E_INTERNAL);

    /* Lookup the aacl control table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &aacl_control_hdl,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      "L3_ALPM_CONTROL",
                                      0, 0, 0, NULL,
                                      NULL,
                                      0,
                                      0,
                                      0,
                                      0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(aacl_control_hdl,
                           "COMP_KEY_TYPE",
                           &comp_mode));

    if ((qid == bcmFieldQualifySrcIp) ||
         (qid == bcmFieldQualifySrcIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) &&
                  (comp_mode != 2) && (qid == bcmFieldQualifySrcIp)) {
            is_ip = 1;
        } else if ((stage_info->flags &
                      BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY) &&
                      (comp_mode != 2) && (qid == bcmFieldQualifySrcIp6)) {
            is_ip6 = 1;
        } else {
            if (qid == bcmFieldQualifySrcIp) {
                is_ip = 1;
                src_nonip_valid = TRUE;
            } else {
                is_ip6 = 1;
                src_nonip_valid = TRUE;
            }
        }
    } else if ((qid == bcmFieldQualifyDstIp) ||
               (qid == bcmFieldQualifyDstIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) &&
                  (comp_mode != 2) && (qid == bcmFieldQualifyDstIp)) {
            is_ip = 1;
        } else if ((stage_info->flags &
                      BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY) &&
                      (comp_mode != 2) && (qid == bcmFieldQualifyDstIp6)) {
            is_ip6 = 1;
        } else {
            if (qid == bcmFieldQualifyDstIp) {
                is_ip = 1;
                dst_nonip_valid = TRUE;
            } else {
                is_ip6 = 1;
                dst_nonip_valid = TRUE;
            }
        }
    }

    sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* retrieve qualifier mask */
    if (src_nonip_valid || dst_nonip_valid) {
        if (comp_mode == 0) {
            int *l4port_t = &l4port;
            int *l4port_t_mask = &l4port_mask;

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));
            if (src_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPort;
            } else if (dst_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPort;
            }
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &l4port_t;
            pair_qual_info.qual_mask = (void *) &l4port_t_mask;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));
            if (l4port_mask != 0) {
                is_pair_valid = 1;
            }
        } else if (comp_mode == 2) {
            int *l4port_t = &l4port;
            int *l4port_t_mask = &l4port_mask;
            uint32_t *vrf_t = &vrf;
            uint32_t *vrf_mask_t = &vrf_mask;
            uint8_t *ipproto_t = &ipproto;
            uint8_t *ipproto_mask_t = &ipproto_mask;
            uint8_t *ipflags_t = &ipflags;
            uint8_t *ipflags_mask_t = &ipflags_mask;
            uint8_t *tcp_control_t = &tcp_control;
            uint8_t *tcp_control_mask_t = &tcp_control_mask;
            bcm_mac_t *mac_t = &mac;
            bcm_mac_t *mac_mask_t = &mac_mask;

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            if (src_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifyL4SrcPort;
            } else if (dst_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifyL4DstPort;
            }
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &l4port_t;
            pair_qual_info.qual_mask = (void *) &l4port_t_mask;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));


            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            pair_qual_info.sdk6_qual_enum = bcmFieldQualifyVrf;
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &vrf_t;
            pair_qual_info.qual_mask = (void *) &vrf_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            pair_qual_info.sdk6_qual_enum = bcmFieldQualifyIpProtocol;
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &ipproto_t;
            pair_qual_info.qual_mask = (void *) &ipproto_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            pair_qual_info.sdk6_qual_enum = bcmFieldQualifyIpFlags;
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &ipflags_t;
            pair_qual_info.qual_mask = (void *) &ipflags_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            pair_qual_info.sdk6_qual_enum = bcmFieldQualifyTcpControl;
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &tcp_control_t;
            pair_qual_info.qual_mask = (void *) &tcp_control_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));

            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            if (src_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifySrcMac;
            } else if (dst_nonip_valid) {
                pair_qual_info.sdk6_qual_enum = bcmFieldQualifyDstMac;
            }

            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_MAC;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &mac_t;
            pair_qual_info.qual_mask = (void *) &mac_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));

            if ((l4port_mask != 0) && (vrf_mask != 0) && (ipproto_mask != 0) &&
                (ipflags_mask != 0) && (tcp_control_mask != 0)
                && BCM_MAC_IS_ZERO(mac_mask)) {
                is_pair_valid = 1;
            }
        } else { /* Default is comp_mode = 1 */
            uint32_t *vrf_t = &vrf;
            uint32_t *vrf_mask_t = &vrf_mask;
            sal_memset(&pair_qual_info, 0, sizeof(bcm_qual_field_t));

            pair_qual_info.sdk6_qual_enum = bcmFieldQualifyVrf;
            pair_qual_info.depth = 1;
            pair_qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
            pair_qual_info.flags = 0;

            pair_qual_info.qual_data = (void *) &vrf_t;
            pair_qual_info.qual_mask = (void *) &vrf_mask_t;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_field_qualifier_get(unit,
                      entry_oper->entry_id, &pair_qual_info));
            if (vrf_mask != 0) {
                is_pair_valid = 1;
            }
        }
    }

    if (is_ip == 1) {
        bcm_ip_t *ip_t = &ip;
        bcm_ip_t *ip_t_mask = &ip_mask;

        qual_info.sdk6_qual_enum = qid;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
        qual_info.flags = 0;

        qual_info.qual_data = (void *)&ip_t;
        qual_info.qual_mask = (void *)&ip_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit,
                  entry_oper->entry_id, &qual_info));
        if (ip_mask != 0) {
            is_qual_valid = 1;
        }
    } else if (is_ip6 == 1) {
        bcm_ip6_t null_ip6;
        bcm_ip6_t *ip6_t = &ip6;
        bcm_ip6_t *ip6_t_mask = &ip6_mask;

        sal_memset(&null_ip6, 0x0, sizeof(bcm_ip6_t));
        qual_info.sdk6_qual_enum = qid;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
        qual_info.flags = 0;
        qual_info.qual_data = (void *)&ip6_t;
        qual_info.qual_mask = (void *)&ip6_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit,
                  entry_oper->entry_id, &qual_info));
        if (sal_memcmp(&ip6_mask, &null_ip6, sizeof(bcm_ip6_t))) {
            is_qual_valid = 1;
        }
    }

    /* retreive aacl class id */
    if (is_qual_valid) {

        dunit = bcmi_ltsw_dev_dunit(unit);
        switch (qual_info.sdk6_qual_enum) {
            case bcmFieldQualifySrcIp:
                aacl_table_name = "L3_IPV4_COMP_SRC";
                break;
            case bcmFieldQualifySrcIp6:
                aacl_table_name = "L3_IPV6_COMP_SRC";
                break;
            case bcmFieldQualifyDstIp:
                aacl_table_name = "L3_IPV4_COMP_DST";
                break;
            case bcmFieldQualifyDstIp6:
                aacl_table_name = "L3_IPV6_COMP_DST";
                break;
            default:
               SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        /* Entry handle allocate for action template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                              aacl_table_name,
                              &aacl_template_hdl));
        if ((qual_info.sdk6_qual_enum == bcmFieldQualifySrcIp) ||
            (qual_info.sdk6_qual_enum == bcmFieldQualifyDstIp)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV4",ip));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV4_MASK", ip_mask));
        } else {
            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6, 1);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_LOWER", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6, 0);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_UPPER", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6_mask, 1);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_LOWER_MASK", ip6_val));

            bcmi_ltsw_util_ip6_half_to_uint64(&ip6_val,
                 &ip6_mask, 0);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                               "IPV6_UPPER_MASK", ip6_val));
        }

        if (((qid == bcmFieldQualifySrcIp) &&
             (stage_info->flags &
              BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY)) ||
            ((qid == bcmFieldQualifySrcIp6) &&
             (stage_info->flags &
              BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) ||
            ((qid = bcmFieldQualifyDstIp) &&
             (stage_info->flags &
              BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY)) ||
            ((qid == bcmFieldQualifyDstIp6) &&
             (stage_info->flags &
              BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY))) {
            is_pair_valid=false;
            l4port = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "L4_PORT",
                    l4port));
            l4port_mask = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "L4_PORT_MASK",
                    l4port_mask));
            vrf = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "VRF",
                    vrf));
            vrf_mask = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "VRF_MASK",
                    vrf_mask));
            ipproto = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "IP_PROTOCOL",
                    ipproto));
            tcp_control = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "TCP_FLAGS",
                    tcp_control));
            ipflags = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "IP_FLAGS",
                    ipflags));
            bcmi_ltsw_util_mac_to_uint64(&mac_val, mac);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(aacl_template_hdl,
                    "MAC",
                    mac_val));
        } else {
            if (is_pair_valid) {
                if (comp_mode == 0) {
                    vrf = 0;
                    vrf_mask = 0;
                    ipproto = 0;
                    tcp_control = 0;
                    ipflags = 0;
                    mac_val = 0;
                } else if (comp_mode == 1) {
                    vrf_mask = (0x1FFF & vrf_mask);
                    l4port = 0;
                    l4port_mask = 0;
                    ipproto = 0;
                    tcp_control = 0;
                    ipflags = 0;
                    mac_val = 0;
                } else {
                    vrf_mask = (0x1FFF & vrf_mask);
                    bcmi_ltsw_util_mac_to_uint64(&mac_val, mac);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "L4_PORT",
                        l4port));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "L4_PORT_MASK",
                        l4port_mask));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "VRF",
                        vrf));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "VRF_MASK",
                        vrf_mask));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "IP_PROTOCOL",
                        ipproto));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "TCP_FLAGS",
                        tcp_control));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "IP_FLAGS",
                        ipflags));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(aacl_template_hdl,
                        "MAC",
                        mac_val));

            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, aacl_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(aacl_template_hdl,
                               "CLASS_ID",
                               &class_id_val));
        *class_id = (int)class_id_val;

        if ((*class_id != -1) && is_pair_valid && (comp_mode != 2)) {
            *pair_qual = pair_qual_info.sdk6_qual_enum;
        }
    }

exit:
    if (BCMLT_INVALID_HDL != aacl_template_hdl) {
        (void) bcmlt_entry_free(aacl_template_hdl);
    }
    if (BCMLT_INVALID_HDL != aacl_control_hdl) {
        (void) bcmlt_entry_free(aacl_control_hdl);
    }
    SHR_FUNC_EXIT();
}
