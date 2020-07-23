/*! \file field_api_manual.c
 *
 * Field Module Manual Qualify Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/types.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>

#define BSL_LOG_MODULE BSL_LS_BCM_FP
/******************************************************************************
 * Private functions
 */
static int
bcmint_ltsw_field_qualify_macro_val_set(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_qual_field_t *qual_info)
{
    int i = 0, shift_val = 0;
    uint32 tmp_data_val = 0, tmp_mask_val = 0;
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_values_t qual_info_values;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info_values, 0, sizeof(bcm_qual_field_values_t));

    if ((qual_info == NULL) ||
        (qual_info->data_type != BCM_QUAL_DATA_TYPE_UINT32)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Retreive macro values supported by qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_macro_values_get(unit, 1, qual_info,
                                                 &qual_info_values));
    if (qual_info_values.num == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Modify data/mask values to be set as per qual info values returned. */
    data_local_val = mask_local_val = 0;
    for (i = 0; i < qual_info_values.num; i++) {
        tmp_data_val = tmp_mask_val = 0;
        if ((*(uint32 *)qual_info->qual_mask) &
                        qual_info_values.macro_flags[i]) {
            tmp_mask_val = qual_info_values.macro_flags_mask_value[i];
        }
        if ((*(uint32 *)qual_info->qual_data) &
                        qual_info_values.macro_flags[i]) {
            tmp_data_val = qual_info_values.macro_flags_value[i];
        }
        data_local_val |= tmp_data_val << shift_val;
        mask_local_val |= tmp_mask_val << shift_val;
        shift_val += qual_info_values.macro_flags_mask_width[i];
    }

    *(uint32 *)qual_info->qual_data = data_local_val;
    *(uint32 *)qual_info->qual_mask = mask_local_val;

    /* Set modified values in LT field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, qual_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_qualify_macro_val_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_qual_field_t *qual_info)
{
    int i = 0;
    uint32 shift_val = 0, flag_tmp_mask = 0;
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_values_t qual_info_values;

    SHR_FUNC_ENTER(unit);
    sal_memset(&qual_info_values, 0, sizeof(bcm_qual_field_values_t));

    if ((qual_info == NULL) ||
        (qual_info->data_type != BCM_QUAL_DATA_TYPE_UINT32)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Retreive macro values supported by qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_macro_values_get(unit, 0, qual_info,
                                                 &qual_info_values));
    if (qual_info_values.num == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get modified values from LT field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, qual_info));
    data_local_val = **(uint32 **)qual_info->qual_data;
    mask_local_val = **(uint32 **)qual_info->qual_mask;

    /* Return values to user as per macro flags set. */
    **(uint32 **)qual_info->qual_data = 0;
    **(uint32 **)qual_info->qual_mask = 0;
    for (i = 0; i < qual_info_values.num; i++) {
        flag_tmp_mask = qual_info_values.macro_flags_mask_value[i] << shift_val;
        if ((data_local_val & flag_tmp_mask) ==
            (qual_info_values.macro_flags_value[i] << shift_val)) {
            **(uint32 **)qual_info->qual_data |= qual_info_values.macro_flags[i];
        }
        if ((mask_local_val & flag_tmp_mask) ==
            (qual_info_values.macro_flags_mask_value[i] << shift_val)) {
            **(uint32 **)qual_info->qual_mask |= qual_info_values.macro_flags[i];
        }
        shift_val += qual_info_values.macro_flags_mask_width[i];
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_qualify_parameters_sanitize(int unit, bcm_field_qualify_t qual,
        void *param0, void *param1)
{
    bcm_range_config_t range_config;

    SHR_FUNC_ENTER(unit);

    switch (qual) {
        case bcmFieldQualifyRangeCheck:
            range_config.rid = *(bcm_range_t *)param0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_range_get(unit, &range_config));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
bcm_ltsw_field_qualify_InnerTpid(int unit,
                                  bcm_field_entry_t entry,
                                  uint16 tpid)
{
    uint16 tpid_mask = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    tpid_mask = 0xFFFF;

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTpid;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &tpid;
    qual_info.qual_mask = (void *) &tpid_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_InnerTpid_get(int unit,
                                     bcm_field_entry_t entry,
                                     uint16 *tpid)
{
    uint16 tpid_mask = 0;
    bcm_qual_field_t qual_info;
    uint16 *tpid_mask_get = &tpid_mask;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerTpid;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &tpid;
    qual_info.qual_mask = (void *) &tpid_mask_get;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterTpid(int unit,
                                 bcm_field_entry_t entry,
                                 uint16 tpid)
{
    uint16 tpid_mask = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    tpid_mask = 0xFFFF;

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterTpid;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &tpid;
    qual_info.qual_mask = (void *) &tpid_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_OuterTpid_get(int unit,
                                     bcm_field_entry_t entry,
                                     uint16 *tpid)
{
    uint16 tpid_mask = 0;
    bcm_qual_field_t qual_info;
    uint16 *tpid_mask_get = &tpid_mask;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterTpid;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &tpid;
    qual_info.qual_mask = (void *) &tpid_mask_get;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcmint_ltsw_field_qualify_port_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_port_get(unit, entry, qual,
                                          data_modid, mask_modid,
                                          data_port, mask_port));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_qualify_port(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_port_set(unit, entry, qual,
                                          data_modid, mask_modid,
                                          data_port, mask_port));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcPort_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_ltsw_field_qualify_port_get(
        unit,
        entry,
        bcmFieldQualifySrcPort,
        data_modid,
        mask_modid,
        data_port,
        mask_port));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT()
}

int
bcm_ltsw_field_qualify_SrcPort(
    int unit,
    bcm_field_entry_t entry,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_ltsw_field_qualify_port(
        unit,
        entry,
        bcmFieldQualifySrcPort,
        data_modid,
        mask_modid,
        data_port,
        mask_port));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT()
}

int
bcm_ltsw_field_qualify_DstPort_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_module_t *data_modid,
    bcm_module_t *mask_modid,
    bcm_port_t *data_port,
    bcm_port_t *mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_ltsw_field_qualify_port_get(
        unit,
        entry,
        bcmFieldQualifyDstPort,
        data_modid,
        mask_modid,
        data_port,
        mask_port));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT()
}

int
bcm_ltsw_field_qualify_DstPort(
    int unit,
    bcm_field_entry_t entry,
    bcm_module_t data_modid,
    bcm_module_t mask_modid,
    bcm_port_t data_port,
    bcm_port_t mask_port)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_ltsw_field_qualify_port(
        unit,
        entry,
        bcmFieldQualifyDstPort,
        data_modid,
        mask_modid,
        data_port,
        mask_port));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT()
}

int
bcm_ltsw_field_qualify_L3EgressIntf(int unit,
                                    bcm_field_entry_t entry,
                                    uint32_t intf_id, uint32_t mask)
{
    int egr_intf_idx = 0;
    bcm_qual_field_t qual_info;
    bcmi_ltsw_l3_egr_intf_type_t egr_intf_type;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egr_intf_to_index(unit, intf_id,
                                        &egr_intf_idx, &egr_intf_type));

    /* Check supported egress interface type */
    if (!(egr_intf_type == BCMI_LTSW_L3_EGR_INTF_T_OL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3EgressIntf;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_intf_idx;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_EGR_INTF_OVERLAY;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L3EgressIntf_get(int unit,
                                        bcm_field_entry_t entry,
                                        uint32_t *intf_id, uint32_t *mask)
{
    bcm_if_t intf_id_get = 0;
    uint32_t mask_egr_intf_idx = 0;
    uint32_t *mask_get = &mask_egr_intf_idx;
    uint32_t egr_intf_idx = 0;
    uint32_t *egr_intf_idx_get = &egr_intf_idx;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL3EgressIntf;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_intf_idx_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_INTF_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egr_index_to_intf(unit, egr_intf_idx,
                                BCMI_LTSW_L3_EGR_INTF_T_OL, &intf_id_get));
        *intf_id = (uint32_t)intf_id_get;
        *mask = mask_egr_intf_idx;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressUnderlay(int unit,
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
    FP_LOCK(unit);

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

    /* Check supported obj type */
    if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) ||
          (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressUnderlay;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_UL) {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_UNDERLAY;
    } else {
        entry_oper->entry_flags |=
            BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3EgressUnderlay_get(int unit,
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
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstL3EgressUnderlay;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_INT;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &egr_obj_idx_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_OBJ_UNDERLAY) {
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
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstType(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_DstType_t dst_type)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch (dst_type) {
        case bcmFieldDstTypeL2EgrIntf:
            data_val = 1;
            break;
        case bcmFieldDstTypeVp:
            data_val = 3;
            break;
        case bcmFieldDstTypeEcmp:
            data_val = 4;
            break;
        case bcmFieldDstTypeL3Egr:
            data_val = 5;
            break;
        case bcmFieldDstTypeL2Mcast:
            data_val = 6;
            break;
        case bcmFieldDstTypeL3Mcast:
            data_val = 7;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstType_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_DstType_t *dst_type)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    switch (data_val) {
        case 1:
            *dst_type = bcmFieldDstTypeL2EgrIntf;
            break;
        case 3:
            *dst_type = bcmFieldDstTypeVp;
            break;
        case 4:
            *dst_type = bcmFieldDstTypeEcmp;
            break;
        case 5:
            *dst_type = bcmFieldDstTypeL3Egr;
            break;
        case 6:
            *dst_type = bcmFieldDstTypeL2Mcast;
            break;
        case 7:
            *dst_type = bcmFieldDstTypeL3Mcast;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackHdrDstType(int unit,
                                       bcm_field_entry_t entry,
                                       bcm_field_DstType_t lb_hdr_dst_type)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch (lb_hdr_dst_type) {
        case bcmFieldDstTypeL2EgrIntf:
            data_val = 1;
            break;
        case bcmFieldDstTypeVp:
            data_val = 3;
            break;
        case bcmFieldDstTypeEcmp:
            data_val = 4;
            break;
        case bcmFieldDstTypeL3Egr:
            data_val = 5;
            break;
        case bcmFieldDstTypeL2Mcast:
            data_val = 6;
            break;
        case bcmFieldDstTypeL3Mcast:
            data_val = 7;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHdrDstType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackHdrDstType_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_DstType_t *lb_hdr_dst_type)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHdrDstType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    switch (data_val) {
        case 1:
            *lb_hdr_dst_type = bcmFieldDstTypeL2EgrIntf;
            break;
        case 3:
            *lb_hdr_dst_type = bcmFieldDstTypeVp;
            break;
        case 4:
            *lb_hdr_dst_type = bcmFieldDstTypeEcmp;
            break;
        case 5:
            *lb_hdr_dst_type = bcmFieldDstTypeL3Egr;
            break;
        case 6:
            *lb_hdr_dst_type = bcmFieldDstTypeL2Mcast;
            break;
        case 7:
            *lb_hdr_dst_type = bcmFieldDstTypeL3Mcast;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMulticastGroup(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_multicast_t group)
{
    uint16_t mc_index = 0;
    uint16_t mask = 0;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    /* Check supported multicast group */
    if ((0 == BCM_MULTICAST_IS_SET(group)) ||
        !((_BCM_MULTICAST_IS_L2(group)) || (_BCM_MULTICAST_IS_L3(group)))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);
    mask = -1;

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &mc_index;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (_BCM_MULTICAST_IS_L2(group)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_L2_MCAST;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_L3_MCAST;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMulticastGroup_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_multicast_t *group)
{
    uint16_t mask = 0;
    uint16_t *mask_get = &mask;
    uint16_t mc_index = 0;
    uint16_t *mc_index_get = &mc_index;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMulticastGroup;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &mc_index_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_L2_MCAST) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, mc_index);
    } else {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, mc_index);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMulticastGroups(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_multicast_t group,
                                   bcm_multicast_t mask)
{
    uint16_t mc_index = 0;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    /* Check supported multicast group */
    if ((0 == BCM_MULTICAST_IS_SET(group)) ||
        !((_BCM_MULTICAST_IS_L2(group)) || (_BCM_MULTICAST_IS_L3(group)))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (mask != BCM_FIELD_EXACT_MATCH_MASK) {
        if (0 == BCM_MULTICAST_IS_SET(mask)) {
            return (BCM_E_PARAM);
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (mask != BCM_FIELD_EXACT_MATCH_MASK) {
        mask = _BCM_MULTICAST_ID_GET(mask);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMulticastGroups;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &mc_index;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (_BCM_MULTICAST_IS_L2(group)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_L2_MCAST;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_L3_MCAST;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMulticastGroups_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_multicast_t *group,
                                   bcm_multicast_t *mask)
{
    int16_t mask_hw = 0;
    int16_t *mask_get = &mask_hw;
    uint16_t mc_index = 0;
    uint16_t *mc_index_get = &mc_index;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMulticastGroups;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &mc_index_get;
    qual_info.qual_mask = (void *) &mask_get;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_L2_MCAST) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, mc_index);
        if (mask_hw != -1) {
            _BCM_MULTICAST_GROUP_SET(*mask, _BCM_MULTICAST_TYPE_L2, *(uint16_t *)mask_get);
        } else {
            *mask = -1;
        }
    } else {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, mc_index);
        if (mask_hw != -1) {
            _BCM_MULTICAST_GROUP_SET(*mask, _BCM_MULTICAST_TYPE_L3, *(uint16_t *)mask_get);
        } else {
            *mask = -1;
        }
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3Egress(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_if_t if_id)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_dstl3egress_set(unit, entry, if_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstL3Egress_get(int unit,
                                       bcm_field_entry_t entry,
                                       bcm_if_t *if_id)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_dstl3egress_get(unit, entry, if_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

STATIC int
bcmint_ltsw_field_qualify_Color(int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint8 color)
{
    uint8 data = 0, mask = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    data = color;
    mask = BCM_FIELD_EXACT_MATCH_MASK;
    qual_info.qual_data = &data;
    qual_info.qual_mask = &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

STATIC int
bcmint_ltsw_field_qualify_Color_get(int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint8 *color)
{
    uint8 *mask = 0, mask_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = qual;

    mask = &mask_val;

    qual_info.qual_data = (void *) &color;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Color(int unit, bcm_field_entry_t entry, uint8 color)
{
    return bcmint_ltsw_field_qualify_Color(unit, entry, bcmFieldQualifyColor, color);
}

int
bcm_ltsw_field_qualify_Color_get(int unit, bcm_field_entry_t entry, uint8 *color)
{
    return bcmint_ltsw_field_qualify_Color_get(unit, entry, bcmFieldQualifyColor, color);
}

int
bcm_ltsw_field_qualify_LoopBackColor(int unit, bcm_field_entry_t entry, uint8 color)
{
    return bcmint_ltsw_field_qualify_Color(unit, entry, bcmFieldQualifyLoopBackColor, color);
}

int
bcm_ltsw_field_qualify_LoopBackColor_get(int unit, bcm_field_entry_t entry, uint8 *color)
{
    return bcmint_ltsw_field_qualify_Color_get(unit, entry, bcmFieldQualifyLoopBackColor, color);
}

int
bcm_ltsw_field_qualify_IngressStpState(int unit, bcm_field_entry_t entry,
                                                  uint8 data, uint8 mask)
{
    uint8_t data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressStpState;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    data_val = data;
    mask_val = mask;

    qual_info.qual_data = &data_val;
    qual_info.qual_mask = &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IngressStpState_get(int unit, bcm_field_entry_t entry,
                                                    uint8 *data, uint8 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIngressStpState;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyHiGig3Flags (Macro based). */
int
bcm_ltsw_field_qualify_HiGig3Flags(int unit, bcm_field_entry_t entry,
                                            uint16 data, uint16 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Flags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyHiGig3Flags (Macro based). */
int
bcm_ltsw_field_qualify_HiGig3Flags_get(int unit, bcm_field_entry_t entry,
                                              uint16 *data, uint16 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyHiGig3Flags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyLoopbackHiGig3Flags (Macro based). */
int
bcm_ltsw_field_qualify_LoopbackHiGig3Flags(int unit, bcm_field_entry_t entry,
                                                    uint16 data, uint16 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHiGig3Flags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyLoopbackHiGig3Flags (Macro based). */
int
bcm_ltsw_field_qualify_LoopbackHiGig3Flags_get(int unit, bcm_field_entry_t entry,
                                                      uint16 *data, uint16 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHiGig3Flags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_L2MacInfo(int unit, bcm_field_entry_t entry,
                                          uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_L2MacInfo_get(int unit, bcm_field_entry_t entry,
                                            uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_IpInfo(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_IpInfo_get(int unit, bcm_field_entry_t entry,
                                          uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyL4Info (Macro based). */
int
bcm_ltsw_field_qualify_L4Info(int unit, bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyL4Info (Macro based). */
int
bcm_ltsw_field_qualify_L4Info_get(int unit, bcm_field_entry_t entry,
                                          uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyL2LearnCommands (Macro based). */
int
bcm_ltsw_field_qualify_L2LearnCommands(int unit, bcm_field_entry_t entry,
                                                uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL2LearnCommands;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyL2LearnCommands (Macro based). */
int
bcm_ltsw_field_qualify_L2LearnCommands_get(int unit, bcm_field_entry_t entry,
                                                  uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL2LearnCommands;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyTunnelTermStatus (Macro based). */
int
bcm_ltsw_field_qualify_TunnelTermStatus(int unit, bcm_field_entry_t entry,
                                                 uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelTermStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyTunnelTermStatus (Macro based). */
int
bcm_ltsw_field_qualify_TunnelTermStatus_get(int unit, bcm_field_entry_t entry,
                                                   uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyTunnelTermStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyLoopbackHdrFlags (Macro based). */
int
bcm_ltsw_field_qualify_LoopbackHdrFlags(int unit, bcm_field_entry_t entry,
                                                uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHdrFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyLoopbackHdrFlags (Macro based). */
int
bcm_ltsw_field_qualify_LoopbackHdrFlags_get(int unit, bcm_field_entry_t entry,
                                                    uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackHdrFlags;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyMHOpcode (Macro value based). */
int
bcm_ltsw_field_qualify_MHOpcode(int unit,
                                bcm_field_entry_t entry,
                                uint8 data, uint8 mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_mhopcode_set(unit, entry, data, mask));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyMHOpcode (Macro based). */
int
bcm_ltsw_field_qualify_MHOpcode_get(int unit,
                                    bcm_field_entry_t entry,
                                    uint8 *data, uint8 *mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_mhopcode_get(unit, entry, data, mask));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyPacketStatus (Macro based). */
int
bcm_ltsw_field_qualify_PacketStatus(int unit,
                                    bcm_field_entry_t entry,
                                    uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyPacketStatus (Macro based). */
int
bcm_ltsw_field_qualify_PacketStatus_get(int unit,
                                        bcm_field_entry_t entry,
                                        uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyPacketStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyForwardingStatus (Macro based). */
int
bcm_ltsw_field_qualify_ForwardingStatus(int unit,
                                        bcm_field_entry_t entry,
                                        uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyForwardingStatus (Macro based). */
int
bcm_ltsw_field_qualify_ForwardingStatus_get(int unit,
                                            bcm_field_entry_t entry,
                                            uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingStatus;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyForwardingEligible (Macro based). */
int
bcm_ltsw_field_qualify_ForwardingEligible(int unit,
                                          bcm_field_entry_t entry,
                                          uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingEligible;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyForwardingEligible (Macro based). */
int
bcm_ltsw_field_qualify_ForwardingEligible_get(int unit,
                                              bcm_field_entry_t entry,
                                              uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyForwardingEligible;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyOuterL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_OuterL2MacInfo(int unit, bcm_field_entry_t entry,
                                               uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyOuterL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_OuterL2MacInfo_get(int unit, bcm_field_entry_t entry,
                                                 uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyOuterIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_OuterIpInfo(int unit, bcm_field_entry_t entry,
                                            uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyOuterIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_OuterIpInfo_get(int unit, bcm_field_entry_t entry,
                                              uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyOuterL4Info (Macro based). */
int
bcm_ltsw_field_qualify_OuterL4Info(int unit, bcm_field_entry_t entry,
                                            uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyOuterL4Info (Macro based). */
int
bcm_ltsw_field_qualify_OuterL4Info_get(int unit, bcm_field_entry_t entry,
                                              uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOuterL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyInnerL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_InnerL2MacInfo(int unit, bcm_field_entry_t entry,
                                                uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyInnerL2MacInfo (Macro based). */
int
bcm_ltsw_field_qualify_InnerL2MacInfo_get(int unit, bcm_field_entry_t entry,
                                                 uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL2MacInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyInnerIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_InnerIpInfo(int unit, bcm_field_entry_t entry,
                                            uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyInnerIpInfo (Macro based). */
int
bcm_ltsw_field_qualify_InnerIpInfo_get(int unit, bcm_field_entry_t entry,
                                               uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerIpInfo;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyInnerL4Info (Macro based). */
int
bcm_ltsw_field_qualify_InnerL4Info(int unit, bcm_field_entry_t entry,
                                            uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyInnerL4Info (Macro based). */
int
bcm_ltsw_field_qualify_InnerL4Info_get(int unit, bcm_field_entry_t entry,
                                              uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyInnerL4Info;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyMirrorPktType (Macro based). */
int
bcm_ltsw_field_qualify_MirrorPktType(int unit, bcm_field_entry_t entry,
                                              uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorPktType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyMirrorPktType (Macro based). */
int
bcm_ltsw_field_qualify_MirrorPktType_get(int unit, bcm_field_entry_t entry,
                                         uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyMirrorPktType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyPtpPktCtrl (Macro based). */
int
bcm_ltsw_field_qualify_PtpPktCtrl(int unit, bcm_field_entry_t entry,
                                            uint16 data, uint16 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpPktCtrl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyPtpPktCtrl (Macro based). */
int
bcm_ltsw_field_qualify_PtpPktCtrl_get(int unit, bcm_field_entry_t entry,
                                             uint16 *data, uint16 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyPtpPktCtrl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyL3Check (Macro based). */
int
bcm_ltsw_field_qualify_L3Check(int unit,
                               bcm_field_entry_t entry,
                               uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Check;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyL3Check (Macro based). */
int
bcm_ltsw_field_qualify_L3Check_get(int unit,
                                   bcm_field_entry_t entry,
                                   uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyL3Check;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_udf(int unit,
        bcm_field_entry_t eid,
        bcm_udf_id_t udf_id,
        int length,
        uint8 *data, uint8 *mask)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate NULL parameters */
    if ((data == NULL) || (mask == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, eid, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, eid,
                                        stage_info,
                                        &entry_oper));

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_udf_qual_set(unit, stage_info, group_oper,
                                      entry_oper, udf_id,
                                      length, data, mask));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_field_qualify_udf_get(int unit,
        bcm_field_entry_t eid,
        bcm_udf_id_t udf_id,
        int max_length,
        uint8 *data, uint8 *mask,
        int *actual_length)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate NULL parameters */
    if ((data == NULL) || (mask == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, eid, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, eid,
                                        stage_info,
                                        &entry_oper));

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_udf_qual_get(unit, stage_info, group_oper,
                                      entry_oper, udf_id,
                                      max_length, data, mask,
                                      actual_length));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RangeCheck(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_range_t range_id,
        int invert)
{
    bcm_qual_field_t qual_info;
    uint32 data_l = 0, mask_l = -1;
    uint32 *data = NULL, *mask = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data = &data_l;
    mask = &mask_l;

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRangeCheck;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = BCM_FIELD_F_BMP;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_parameters_sanitize(unit,
                                                       qual_info.sdk6_qual_enum,
                                                       (void *)&range_id,
                                                       NULL));
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    SHR_BITSET(mask, range_id);
    *data = invert ? SHR_BITCLR(data, range_id) : SHR_BITSET(data, range_id);

    qual_info.qual_data = (void *) data;
    qual_info.qual_mask = (void *) mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RangeCheck_get(
        int unit, bcm_field_entry_t entry,
        int max_count, bcm_field_range_t* range, int* invert, int* count)
{
    bcm_qual_field_t qual_info;
    uint32 data_l = 0, mask_l = 0;
    uint32 *data = NULL, *mask = NULL;
    int ct = 0, idx = 0;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data = &data_l;
    mask = &mask_l;

    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRangeCheck;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = BCM_FIELD_F_BMP;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    for (idx = 0; (idx < (sizeof(data_l) * 8)) && (idx < max_count) ; idx++) {
        if (SHR_BITGET(mask, idx)) {
            range[ct] = idx;
            invert[ct] = !(SHR_BITGET(data, idx));
            ct++;
        }
    }

    *count = ct;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PktFwdType(int unit, bcm_field_entry_t entry,
                                  bcm_field_PktFwdType_t type)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch (type) {
        case bcmFieldPktFwdTypeL2Ucast:
            data_val = 0;
            break;
        case bcmFieldPktFwdTypeL2Mcast:
            data_val = 1;
            break;
        case bcmFieldPktFwdTypeL3Ucast:
            data_val = 2;
            break;
        case bcmFieldPktFwdTypeL3Mcast:
            data_val = 3;
            break;
        case bcmFieldPktFwdTypeCopyToCpu:
            data_val = 4;
            break;
        case bcmFieldPktFwdTypeL3Any:
            data_val = 2;
            mask_val = 2;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktFwdType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PktFwdType_get(int unit, bcm_field_entry_t entry,
                                           bcm_field_PktFwdType_t *type)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktFwdType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    switch (data_val) {
        case 0:
            *type = bcmFieldPktFwdTypeL2Ucast;
            break;
        case 1:
            *type = bcmFieldPktFwdTypeL2Mcast;
            break;
        case 2:
            if (mask_val == 2) {
                *type = bcmFieldPktFwdTypeL3Any;
            } else {
                *type = bcmFieldPktFwdTypeL3Ucast;
            }
            break;
        case 3:
            *type = bcmFieldPktFwdTypeL3Mcast;
            break;
        case 4:
            *type = bcmFieldPktFwdTypeCopyToCpu;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcGport(int unit,
                                bcm_field_entry_t entry,
                                bcm_gport_t src_port)
{
    bcm_port_t lport = -1;
    bcm_qual_field_t qual_info;
    uint16_t data_val = 0, mask_val = -1;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate if given gport is valid for the qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                             bcmFieldQualifySrcGport,
                                             src_port, &lport));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    data_val = (uint16_t)lport;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (BCM_GPORT_IS_MPLS_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MPLS;
    } else if (BCM_GPORT_IS_WLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_WLAN;
    } else if (BCM_GPORT_IS_VXLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VXLAN;
    } else if (BCM_GPORT_IS_VLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_NIV;
    } else if (BCM_GPORT_IS_MIM_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MIM;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_FLOWPORT;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcGport_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_gport_t *src_port)
{
    bcm_qual_field_t qual_info;
    bcmi_ltsw_gport_info_t gport_info;
    uint16_t data_val = 0, mask_val = 0;
    uint16_t *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&gport_info, 0, sizeof(gport_info));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MPLS) {
        gport_info.mpls_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_WLAN) {
        gport_info.wlan_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VXLAN) {
        gport_info.vxlan_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VLAN) {
        gport_info.vlan_vp_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_NIV) {
        gport_info.niv_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MIM) {
        gport_info.mim_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
    } else {
        gport_info.flow_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_construct(unit, &gport_info, src_port));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcGports(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_gport_t src_port,
                                 bcm_gport_t src_port_mask)
{
    bcm_port_t lport = -1;
    bcm_port_t lport_mask = -1;
    bcm_qual_field_t qual_info;
    uint16_t data_val = 0, mask_val = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate if given gport is valid for the qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                             bcmFieldQualifySrcGports,
                                             src_port, &lport));
    /* Validate if given gport is valid for the qualifier. */
    if (src_port_mask != BCM_FIELD_EXACT_MATCH_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                                 bcmFieldQualifySrcGports,
                                                 src_port_mask,
                                                 &lport_mask));
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcGports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    data_val = (uint16_t)lport;
    if (src_port_mask != BCM_FIELD_EXACT_MATCH_MASK) {
        mask_val = (uint16_t)lport_mask;
    } else {
        mask_val = BCM_FIELD_EXACT_MATCH_MASK;
    }

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (BCM_GPORT_IS_MPLS_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MPLS;
    } else if (BCM_GPORT_IS_WLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_WLAN;
    } else if (BCM_GPORT_IS_VXLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VXLAN;
    } else if (BCM_GPORT_IS_VLAN_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_NIV;
    } else if (BCM_GPORT_IS_MIM_PORT(src_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MIM;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_FLOWPORT;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_SrcGports_get(int unit,
                                     bcm_field_entry_t entry,
                                     bcm_gport_t *src_port,
                                     bcm_gport_t *src_port_mask)
{
    bcm_qual_field_t qual_info;
    bcmi_ltsw_gport_info_t gport_info;
    bcmi_ltsw_gport_info_t gport_mask_info;
    uint32_t data_val = 0;
    int mask_val = 0;
    uint32_t *data_tmp = &data_val;
    int *mask_tmp = &mask_val;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&gport_info, 0, sizeof(gport_info));
    sal_memset(&gport_mask_info, 0, sizeof(gport_mask_info));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifySrcGports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MPLS) {
        gport_info.mpls_id = data_val;
        gport_mask_info.mpls_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_WLAN) {
        gport_info.wlan_id = data_val;
        gport_mask_info.wlan_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_WLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VXLAN) {
        gport_info.vxlan_id = data_val;
        gport_mask_info.vxlan_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_VXLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VLAN) {
        gport_info.vlan_vp_id = data_val;
        gport_mask_info.vlan_vp_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_VLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_NIV) {
        gport_info.niv_id = data_val;
        gport_mask_info.niv_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_NIV_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MIM) {
        gport_info.mim_id = data_val;
        gport_mask_info.mim_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_MIM_PORT;
    } else {
        gport_info.flow_id = data_val;
        gport_mask_info.flow_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_FLOW_PORT;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_construct(unit, &gport_info, src_port));
    if (mask_val != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_mask_info, src_port_mask));
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MixedSrcClassId(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t data,
        bcm_field_src_class_t mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_MixedSrcClassId(unit, entry, pbmp,
                                                   data, mask));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_MixedSrcClassId_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_t *data,
        bcm_field_src_class_t *mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_MixedSrcClassId_get(unit, entry, pbmp,
                                                       data, mask));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstGport(int unit,
                                bcm_field_entry_t entry,
                                bcm_gport_t dst_port)
{
    bcm_port_t lport = -1;
    bcm_qual_field_t qual_info;
    uint16_t data_val = 0, mask_val = -1;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate if given gport is valid for the qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                             bcmFieldQualifyDstGport,
                                             dst_port, &lport));
    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    data_val = (uint16_t)lport;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));
    if (BCM_GPORT_IS_MPLS_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MPLS;
    } else if (BCM_GPORT_IS_WLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_WLAN;
    } else if (BCM_GPORT_IS_VXLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VXLAN;
    } else if (BCM_GPORT_IS_VLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_NIV;
    } else if (BCM_GPORT_IS_MIM_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MIM;
    } else if (BCM_GPORT_IS_FLOW_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_FLOWPORT;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MODPORT;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstGport_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_gport_t *dst_port)
{
    bcm_qual_field_t qual_info;
    bcmi_ltsw_gport_info_t gport_info;
    uint16_t data_val = 0, mask_val = 0;
    uint16_t *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&gport_info, 0, sizeof(gport_info));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MPLS) {
        gport_info.mpls_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_WLAN) {
        gport_info.wlan_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VXLAN) {
        gport_info.vxlan_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VLAN) {
        gport_info.vlan_vp_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_NIV) {
        gport_info.niv_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MIM) {
        gport_info.mim_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_FLOWPORT) {
        gport_info.flow_id = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_FLOW_PORT;
    } else {
        gport_info.port = data_val;
        gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MODPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_if_to_port(unit, gport_info.port, dst_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_info, dst_port));
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstGports(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_gport_t dst_port,
                                 bcm_gport_t dst_port_mask)
{
    bcm_port_t lport = -1;
    bcm_port_t lport_mask = -1;
    bcm_qual_field_t qual_info;
    uint16_t data_val = 0, mask_val = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate if given gport is valid for the qualifier. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                             bcmFieldQualifyDstGports,
                                             dst_port, &lport));

    if (dst_port_mask != BCM_FIELD_EXACT_MATCH_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_qual_gport_validate(unit, entry,
                                                 bcmFieldQualifyDstGports,
                                                 dst_port_mask, &lport_mask));
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstGports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    data_val = (uint16_t)lport;
    if (dst_port_mask != BCM_FIELD_EXACT_MATCH_MASK) {
        mask_val = (uint16_t)lport_mask;
    } else {
        mask_val = BCM_FIELD_EXACT_MATCH_MASK;
    }

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (BCM_GPORT_IS_MPLS_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MPLS;
    } else if (BCM_GPORT_IS_WLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_WLAN;
    } else if (BCM_GPORT_IS_VXLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VXLAN;
    } else if (BCM_GPORT_IS_VLAN_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_NIV;
    } else if (BCM_GPORT_IS_MIM_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MIM;
    } else if (BCM_GPORT_IS_FLOW_PORT(dst_port)) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_FLOWPORT;
    } else {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_GPORT_MODPORT;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstGports_get(int unit,
                                bcm_field_entry_t entry,
                                bcm_gport_t *dst_port,
                                bcm_gport_t *dst_port_mask)
{
    bcm_qual_field_t qual_info;
    bcmi_ltsw_gport_info_t gport_info;
    bcmi_ltsw_gport_info_t gport_mask_info;
    uint32_t data_val = 0;
    int mask_val = 0;
    uint32_t *data_tmp = &data_val;
    int *mask_tmp = &mask_val;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
    sal_memset(&gport_info, 0, sizeof(gport_info));
    sal_memset(&gport_mask_info, 0, sizeof(gport_mask_info));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDstGports;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MPLS) {
        gport_info.mpls_id = data_val;
        gport_mask_info.mpls_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_WLAN) {
        gport_info.wlan_id = data_val;
        gport_mask_info.wlan_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_WLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VXLAN) {
        gport_info.vxlan_id = data_val;
        gport_mask_info.vxlan_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_VXLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_VLAN) {
        gport_info.vlan_vp_id = data_val;
        gport_mask_info.vlan_vp_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_VLAN_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_NIV) {
        gport_info.niv_id = data_val;
        gport_mask_info.niv_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_NIV_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MIM) {
        gport_info.mim_id = data_val;
        gport_mask_info.mim_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_MIM_PORT;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_FLOWPORT) {
        gport_info.flow_id = data_val;
        gport_mask_info.flow_id = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type =_SHR_GPORT_TYPE_FLOW_PORT;
    } else {
        gport_info.port = data_val;
        gport_mask_info.port = mask_val;
        gport_info.gport_type =
            gport_mask_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
    }

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_GPORT_MODPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_if_to_port(unit, gport_info.port, dst_port));
        if (mask_val != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_if_to_port(unit, gport_mask_info.port,
                                         dst_port_mask));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_info, dst_port));
        if (mask_val != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_construct(unit, &gport_mask_info,
                                                dst_port_mask));
        }
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyVlanFormat (Macro based). */
int
bcm_ltsw_field_qualify_VlanFormat(int unit, bcm_field_entry_t entry,
                                                 uint8 data, uint8 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyVlanFormat;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyVlanFormat (Macro based). */
int
bcm_ltsw_field_qualify_VlanFormat_get(int unit, bcm_field_entry_t entry,
                                                   uint8 *data, uint8 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyVlanFormat;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyOverlayVlanFormat (Macro based). */
int
bcm_ltsw_field_qualify_OverlayVlanFormat(int unit, bcm_field_entry_t entry,
                                                 uint8 data, uint8 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayVlanFormat;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyOverlayVlanFormat (Macro based). */
int
bcm_ltsw_field_qualify_OverlayVlanFormat_get(int unit, bcm_field_entry_t entry,
                                                   uint8 *data, uint8 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyOverlayVlanFormat;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
int
bcm_ltsw_field_qualify_EgressL3TunnelGport(int unit,
                                bcm_field_entry_t entry,
                                bcm_gport_t tnl_gport)
{
    bcmi_ltsw_tunnel_type_t tnl_type;
    uint32_t tnl_idx;
    bcm_qual_field_t qual_info;
    uint16_t data_val = 0, mask_val = -1;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressL3TunnelGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    BCMI_LTSW_GPORT_TUNNEL_ID_GET(tnl_gport, tnl_type, tnl_idx);

    if (tnl_type == bcmiTunnelTypeMpls ||
        tnl_type == bcmiTunnelTypeMpls ||
        tnl_type == bcmiTunnelTypeIpL3) {
        data_val = (uint16_t)tnl_idx;
    } else if (tnl_type == bcmiTunnelTypeFlexFlow) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_flow_tunnel_id_resolve(unit, tnl_gport, &tnl_idx));
        data_val = (uint16_t)tnl_idx;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

    if (tnl_type == bcmiTunnelTypeMpls) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TUNNEL_TYPE_MPLS;
    } else if (tnl_type == bcmiTunnelTypeIpL3) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TUNNEL_TYPE_IPL3;
    } else if (tnl_type == bcmiTunnelTypeFlexFlow) {
        entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TUNNEL_TYPE_FLEXFLOW;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_EgressL3TunnelGport_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_gport_t *tnl_gport)
{
    bcm_qual_field_t qual_info;
    bcmi_ltsw_tunnel_type_t tnl_type = 0;
    uint32_t tnl_idx;
    uint16_t data_val = 0, mask_val = 0;
    uint16_t *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_TUNNEL_TYPE_MPLS) {
        tnl_type = bcmiTunnelTypeMpls;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_TUNNEL_TYPE_IPL3) {
        tnl_type = bcmiTunnelTypeIpL3;
    } else if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_TUNNEL_TYPE_FLEXFLOW) {
        tnl_type = bcmiTunnelTypeFlexFlow;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyEgressL3TunnelGport;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT16;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    tnl_idx = data_val;

    BCMI_LTSW_GPORT_TUNNEL_ID_SET(*tnl_gport, tnl_type, tnl_idx);
    
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuReasonHigh(int unit, bcm_field_entry_t entry, uint32 data, uint32 mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuReasonHigh;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    /* set copy to cpu reason value */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_copytocpureasonhigh_set(unit, &data, &mask));

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_CopyToCpuReasonHigh_get(int unit, bcm_field_entry_t entry, uint32 *data, uint32 *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyCopyToCpuReasonHigh;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    /* set copy to cpu reason value */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_copytocpureasonhigh_get(unit, data, mask));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntHdrType(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_IntHdr_Type_t type)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch (type) {
        case bcmfieldIntHdrTypeNone:
            data_val = 0;
            break;
        case bcmfieldIntHdrTypeINTDP:
            data_val = 1;
            break;
        case bcmfieldIntHdrTypeIFA:
            data_val = 2;
            break;
        case bcmfieldIntHdrTypeIOAM:
            data_val = 3;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntHdrType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntHdrType_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_IntHdr_Type_t *type)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntHdrType;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    switch (data_val) {
        case 0:
            *type = bcmfieldIntHdrTypeNone;
            break;
        case 1:
            *type = bcmfieldIntHdrTypeINTDP;
            break;
        case 2:
            *type = bcmfieldIntHdrTypeIFA;
            break;
        case 3:
            *type = bcmfieldIntHdrTypeIOAM;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntHdrPresent(int unit,
                               bcm_field_entry_t entry,
                               bcm_field_IntHdr_Location_t location)
{
    uint8 data_val = 0, mask_val = -1;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    switch (location) {
        case bcmfieldIntHdrLocationInner:
            data_val = 0;
            break;
        case bcmfieldIntHdrLocationBoth:
            data_val = 1;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntHdrPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_val;
    qual_info.qual_mask = (void *) &mask_val;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_IntHdrPresent_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_IntHdr_Location_t *location)
{
    uint8 data_val = 0, mask_val = 0;
    uint8 *data_tmp = &data_val, *mask_tmp = &mask_val;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyIntHdrPresent;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT8;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data_tmp;
    qual_info.qual_mask = (void *) &mask_tmp;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

    switch (data_val) {
        case 0:
            *location = bcmfieldIntHdrLocationInner;
            break;
        case 1:
            *location = bcmfieldIntHdrLocationBoth;
            break;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PktMiscCtrl(int unit, bcm_field_entry_t entry, bcm_field_pkt_misc_ctrl_t pkt_misc_ctrl)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktMiscCtrl;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;

    qual_info.qual_data = (void *) &pkt_misc_ctrl;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_field_qualify_PktMiscCtrl_get(int unit, bcm_field_entry_t entry, bcm_field_pkt_misc_ctrl_t *pkt_misc_ctrl)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyPktMiscCtrl;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;

    qual_info.qual_data = (void *) &pkt_misc_ctrl;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Snap(int unit,
        bcm_field_entry_t entry,
        bcm_field_snap_header_t data,
        bcm_field_snap_header_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySnap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_SNAP;
    qual_info.flags = BCM_FIELD_F_STRUCT;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Llc(int unit,
        bcm_field_entry_t entry,
        bcm_field_llc_header_t data,
        bcm_field_llc_header_t mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLlc;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_LLC;
    qual_info.flags = BCM_FIELD_F_STRUCT;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Snap_get(int unit,
        bcm_field_entry_t entry,
        bcm_field_snap_header_t *data,
        bcm_field_snap_header_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifySnap;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_SNAP;
    qual_info.flags = BCM_FIELD_F_STRUCT;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_Llc_get(int unit,
        bcm_field_entry_t entry,
        bcm_field_llc_header_t *data,
        bcm_field_llc_header_t *mask)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLlc;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_LLC;
    qual_info.flags = BCM_FIELD_F_STRUCT;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PacketRes(int unit,
                                 bcm_field_entry_t entry,
                                 uint32 data,
                                 uint32 mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_packetres_set(unit, entry, data, mask));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_PacketRes_get(int unit,
                                     bcm_field_entry_t entry,
                                     uint32 *data,
                                     uint32 *mask)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qualify_packetres_get(unit, entry, data, mask));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PayloadFirstEightBytes(
    int unit,
    bcm_field_entry_t entry,
    uint32 data1,
    uint32 data2,
    uint32 mask1,
    uint32 mask2)
{
    uint64 data = 0;
    uint64 mask = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PayloadFirstEightBytes;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    data = data2;
    data <<=32;
    data |= data1;
    mask = mask2;
    mask <<=32;
    mask |= mask1;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_L2PayloadFirstEightBytes_get(
    int unit,
    bcm_field_entry_t entry,
    uint32 *data1,
    uint32 *data2,
    uint32 *mask1,
    uint32 *mask2)
{
    uint64 *data = NULL, data_l = 0;
    uint64 *mask = NULL, mask_l = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyL2PayloadFirstEightBytes;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT64;
    qual_info.flags = 0;

    data = &data_l;
    mask = &mask_l;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    *data1 = data_l & 0xFFFFFFFF;
    *data2 = (data_l >> 32);
    *mask1 = mask_l & 0xFFFFFFFF;
    *mask2 = (mask_l >> 32);
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier set - bcmFieldQualifyIntCtrl (Macro based). */
int
bcm_ltsw_field_qualify_IntCtrl(int unit, bcm_field_entry_t entry,
                                         uint32 data, uint32 mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    data_local_val = data;
    mask_local_val = mask;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyIntCtrl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data_local_val;
    qual_info.qual_mask = (void *) &mask_local_val;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Implementation for qualifier get - bcmFieldQualifyIntCtrl (Macro based). */
int
bcm_ltsw_field_qualify_IntCtrl_get(int unit, bcm_field_entry_t entry,
                                          uint32 *data, uint32 *mask)
{
    uint32 data_local_val = 0, mask_local_val = 0;
    uint32 *tmp_data_val = NULL, *tmp_mask_val = NULL;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    tmp_data_val = &data_local_val;
    tmp_mask_val = &mask_local_val;
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyIntCtrl;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &tmp_data_val;
    qual_info.qual_mask = (void *) &tmp_mask_val;

    /* Retreive qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_qualify_macro_val_get(unit, entry, &qual_info));

    *data = data_local_val;
    *mask = mask_local_val;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMultipath(int               unit,
                                   bcm_field_entry_t entry,
                                   bcm_if_t          mp_intf)
{
    uint32 data = 0, mask = 0;
    bcm_qual_field_t qual_info;
    int egr_obj_idx = 0;
    bcmi_ltsw_l3_egr_obj_type_t egr_obj_type;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, mp_intf,
                                            &egr_obj_idx, &egr_obj_type));

    /* Check supported obj type */
    if (!((egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) ||
          (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    data = egr_obj_idx;
    mask = BCM_FIELD_EXACT_MATCH_MASK;

    /* Fill up qualifier information. */
    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMultipath;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;
    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    /* Set qualifier values. */
    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

    if (egr_obj_type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) {
        entry_oper->entry_flags |=
                    BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY;
    } else {
        entry_oper->entry_flags |=
                    BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY;
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DstMultipath_get(int               unit,
                                        bcm_field_entry_t entry,
                                        bcm_if_t          *mp_intf)
{
    uint32 data_l = 0, mask_l = 0;
    uint32 *data = NULL, *mask = NULL;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    data = &data_l;
    mask = &mask_l;
    qual_info.sdk6_qual_enum = bcmFieldQualifyDstMultipath;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &data;
    qual_info.qual_mask = (void *) &mask;

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* check if qual is part of group qset */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info, &entry_oper));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_UNDERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, data_l,
                                    BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL, mp_intf));
    } else if (entry_oper->entry_flags &
                       BCMINT_FIELD_ENTRY_EGR_OBJ_ECMP_GROUP_OVERLAY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_obj_id_construct(unit, data_l,
                               BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL, mp_intf));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackType(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_LoopbackType_t loopback_type)
{
    uint16 loopback_type_mask = 0;
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    loopback_type_mask = 0xFFFF;

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackType;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &loopback_type;
    qual_info.qual_mask = (void *) &loopback_type_mask;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_LoopbackType_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_LoopbackType_t* loopback_type)
{
    uint16 loopback_type_mask = 0;
    bcm_qual_field_t qual_info;
    uint16 *loopback_type_mask_get = &loopback_type_mask;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyLoopbackType;
    qual_info.flags = 0;

    qual_info.qual_data = (void *) &loopback_type;
    qual_info.qual_mask = (void *) &loopback_type_mask_get;

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry, &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FirstDropReason(int unit,
                                       bcm_field_entry_t entry,
                                       bcm_field_drop_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFirstDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(unit,
                                    entry,
                                    bcmFieldQualifyFirstDropReason,
                                    (uint32)type,
                                    &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_FirstDropReason_get(int unit,
                                           bcm_field_entry_t entry,
                                           bcm_field_drop_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyFirstDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
         bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
        mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(unit,
                                    entry,
                                    bcmFieldQualifyFirstDropReason,
                                    &qual_info,
                                    (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DropReason(int unit,
                                  bcm_field_entry_t entry,
                                  bcm_field_drop_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(unit,
                entry,
                bcmFieldQualifyDropReason,
                (uint32)type,
                &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_DropReason_get(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_field_drop_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(unit,
                entry,
                bcmFieldQualifyDropReason,
                &qual_info,
                (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrFirstDropReason(int unit,
                                                  bcm_field_entry_t entry,
                                                  bcm_field_drop_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrFirstDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(unit,
                entry,
                bcmFieldQualifyRecircleHdrFirstDropReason,
                (uint32)type,
                &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrFirstDropReason_get(int unit,
                                                      bcm_field_entry_t entry,
                                                      bcm_field_drop_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrFirstDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(unit,
                entry,
                bcmFieldQualifyRecircleHdrFirstDropReason,
                &qual_info,
                (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrDropReason(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_field_drop_reason_t type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_enum_to_lt_enum_value_get(unit,
                entry,
                bcmFieldQualifyRecircleHdrDropReason,
                (uint32)type,
                &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_set(unit, entry,  &qual_info));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qualify_RecircleHdrDropReason_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 bcm_field_drop_reason_t *type)
{
    bcm_qual_field_t qual_info;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

    qual_info.sdk6_qual_enum = bcmFieldQualifyRecircleHdrDropReason;
    qual_info.flags = BCM_FIELD_F_ENUM;
    qual_info.depth = 1;
    qual_info.data_type = BCM_QUAL_DATA_TYPE_UINT32;
    qual_info.qual_data = (void *) &type;
    qual_info.qual_mask = NULL;


    SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_field_qualifier_get(unit, entry,  &qual_info));

    SHR_IF_ERR_VERBOSE_EXIT(
            mbcm_ltsw_field_manual_lt_enum_value_to_enum_get(unit,
                entry,
                bcmFieldQualifyRecircleHdrDropReason,
                &qual_info,
                (uint32 *)type));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
