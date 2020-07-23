/*! \file virtual.c
 *
 * Virtual Driver for XFS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/xfs/virtual.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/sbr.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>


/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Reserved first 4K VFI for identical mapping to VLAN. */
#define VFI_RESERVED   (BCM_VLAN_MAX + 1)

/* EGR_VFI tag action to preserve packet odot1p. */
#define VFI_TAG_ACTION_PRESERVE_DOT1P 4

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
xfs_ltsw_virtual_index_get(int unit, int *vfi_min, int *vfi_max, int *vp_min,
                           int *vp_max, int *l2_iif_min, int *l2_iif_max)
{
    uint64_t min = 0;
    uint64_t max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ING_VFI_TABLEs, VFIs,
                                       &min, &max));
    /*
     * Reserved first 4K VFI for identical mapping to VLAN.
     */
    *vfi_min = VFI_RESERVED;
    *vfi_max = (int)max;

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ING_SVP_TABLEs, SVPs,
                                       &min, &max));
    /*
     * VP index is used for VP validation and 0 is treated as invalid VP.
     */
    *vp_min = min > 1? (int)min:1;
    *vp_max = (int)max;

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ING_L2_IIF_TABLEs,
                                       L2_IIFs, &min, &max));
    *l2_iif_min = (int)min;
    *l2_iif_max = (int)max;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_vfi_init(int unit, int vfi)
{
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hd2 = BCMLT_INVALID_HDL;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    int profile_idx = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_VFI_TABLEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFIs, vfi));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MAPPED_VFIs, vfi));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, MEMBERSHIP_PROFILE_PTRs,
                               BCMI_LTSW_VLAN_PROFILE_ING_MSHP_CHK_IDX_DEF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, BITMAP_PROFILE_INDEXs,
                               BCMI_LTSW_VLAN_PROFILE_ING_BLOCK_MASK_IDX_DEF));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                             &profile_idx));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, STRENGTH_PROFILE_INDEXs,
                               profile_idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_VFIs, &entry_hd2));

    pth = BCMI_LTSW_SBR_PTH_EGR_VFI;
    ent_type = BCMI_LTSW_SBR_PET_NONE;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                             &profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd2, STRENGTH_PRFL_IDXs, profile_idx));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd2, VFIs, vfi));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd2, UNTAG_PROFILE_INDEXs,
                               BCMI_LTSW_VLAN_PROFILE_EGR_UNTAG_IDX_DEF));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd2, MEMBERSHIP_PROFILE_IDXs,
                               BCMI_LTSW_VLAN_PROFILE_EGR_MSHP_CHK_IDX_DEF));
    /* Preserve packet's dot1p by default. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd2, TAG_ACTIONs,
                               VFI_TAG_ACTION_PRESERVE_DOT1P));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd2, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hd2) {
        (void) bcmlt_entry_free(entry_hd2);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_vfi_cleanup(int unit, int vfi)
{
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hd2 = BCMLT_INVALID_HDL;
    uint64_t get_val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_VFI_TABLEs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFIs, vfi));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, MEMBERSHIP_PROFILE_PTRs, &get_val));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit, BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                       (int)get_val));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, BITMAP_PROFILE_INDEXs, &get_val));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit,
                                       BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0,
                                       (int)get_val));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_VFIs, &entry_hd2));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd2, VFIs, vfi));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd2, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd2, UNTAG_PROFILE_INDEXs, &get_val));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit, BCMI_LTSW_PROFILE_EGR_UNTAG,
                                       (int)get_val));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd2, MEMBERSHIP_PROFILE_IDXs, &get_val));
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit, BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                       (int)get_val));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd2,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));


exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hd2) {
        (void) bcmlt_entry_free(entry_hd2);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    int vp, cml = 0;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {SVPs,              BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {CML_FLAGS_NEWs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {CML_FLAGS_MOVEs,   BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    if (!(flags & BCM_PORT_LEARN_FWD)) {
        cml |= BCMI_XFS_CML_FLAGS_DROP;
    }
    if (flags & BCM_PORT_LEARN_CPU) {
        cml |= BCMI_XFS_CML_FLAGS_COPY_TO_CPU;
    }
    if (flags & BCM_PORT_LEARN_ARL) {
        cml |= BCMI_XFS_CML_FLAGS_LEARN;
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    fields[1].u.val = cml;
    fields[2].u.val = cml;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_update(unit, ING_SVP_TABLEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_virtual_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    int vp, cml = 0;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {SVPs,              BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {CML_FLAGS_NEWs,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_get(unit, ING_SVP_TABLEs, &lt_entry, NULL, NULL));
    cml = fields[1].u.val;

    *flags = 0;
    if (!(cml & BCMI_XFS_CML_FLAGS_DROP)) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & BCMI_XFS_CML_FLAGS_COPY_TO_CPU) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & BCMI_XFS_CML_FLAGS_LEARN) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    /*
     * bcmVlanActionNone - Preserve.
     * bcmVlanActionDelete - Not preserve.
     */
    if ((action->outer_tag != bcmVlanActionNone) &&
        (action->outer_tag != bcmVlanActionDelete)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if ((action->inner_tag != bcmVlanActionNone) &&
        (action->inner_tag != bcmVlanActionDelete)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));

    set_val = ((action->inner_tag == bcmVlanActionDelete) ? 0 : 1) << 1 |
              ((action->outer_tag == bcmVlanActionDelete) ? 0 : 1);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_TAG_PRESERVE_CTRLs,
                               set_val));

    set_val = (action->priority << 1) | action->new_outer_cfi;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEFAULT_OTAG_DOT1P_CFIs,
                               set_val));

    set_val = (action->new_inner_pkt_prio << 1) | action->new_inner_cfi;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEFAULT_ITAG_DOT1P_CFIs,
                               set_val));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, VLAN_TAG_PRESERVE_CTRLs,
                               &get_val));
    action->outer_tag =
        (get_val & 0x1) ? bcmVlanActionNone : bcmVlanActionDelete;
    action->inner_tag =
        ((get_val >> 1) & 0x1) ? bcmVlanActionNone : bcmVlanActionDelete;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEFAULT_OTAG_DOT1P_CFIs,
                               &get_val));
    action->new_outer_cfi = get_val & 0x1;

    action->priority = get_val >> 0x1;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DEFAULT_ITAG_DOT1P_CFIs,
                               &get_val));
    action->new_inner_cfi = get_val & 0x1;
    action->new_inner_pkt_prio = get_val >> 0x1;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_vlan_action_reset(int unit, bcm_port_t port)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));


    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));

    set_val = BCMI_XFS_VLAN_TAG_PRESERVE_CTRL_DEF;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_TAG_PRESERVE_CTRLs,
                               set_val));
    set_val = 0;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEFAULT_OTAG_DOT1P_CFIs,
                               set_val));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEFAULT_ITAG_DOT1P_CFIs,
                               set_val));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_class_get(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t *pclass_id)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {DVPs,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {GROUP_IDs,   BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if ((pclass != bcmPortClassEgress) &&
        (pclass != bcmPortClassVlanTranslateEgress)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    if (pclass == bcmPortClassEgress) {
        fields[1].fld_name = CLASS_IDs;
    }
    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_get(unit, EGR_DVPs, &lt_entry, NULL, NULL));
    *pclass_id = fields[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_class_set(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t pclass_id)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[2] =
    {
        /*0 */ {DVPs,        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {GROUP_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if ((pclass != bcmPortClassEgress) &&
        (pclass != bcmPortClassVlanTranslateEgress)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    if (pclass == bcmPortClassEgress) {
        fields[1].fld_name = CLASS_IDs;
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    fields[1].u.val = pclass_id;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_update(unit, EGR_DVPs, &lt_entry, NULL));


exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int *arg)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    uint32_t fld_mask = 0;
    uint8_t process_ing_svp = 0;
    uint8_t process_egr_dvp = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hd2 = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if ((type == bcmVlanPortUseOuterPri) ||
        (type == bcmVlanPortUseInnerPri)) {
        process_ing_svp = 1;
    } else if ((type == bcmVlanPortPayloadTagsDelete) ||
        (type == bcmVlanPortTranslateEgressVnidEnable) ||
        (type == bcmVlanTranslateEgressEnable)) {
        process_egr_dvp = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (process_ing_svp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SVPs, vp));
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (process_egr_dvp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, EGR_DVPs, &entry_hd2));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hd2, DVPs, vp));
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hd2, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
    }

    switch (type) {
    case bcmVlanPortUseOuterPri:
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, USE_OTAG_DOT1P_CFI_FOR_PHBs,
                                   &get_val));
        *arg = get_val? 0x1:0x0;
        break;

    case bcmVlanPortUseInnerPri:
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, USE_ITAG_DOT1P_CFI_FOR_PHBs,
                                   &get_val));
        *arg = get_val? 0x1:0x0;
        break;

    case bcmVlanPortPayloadTagsDelete:
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hd2, PROCESS_CTRLs, &get_val));
        fld_mask = 1 << BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_SFT;
        *arg = (get_val & fld_mask)? 0x1:0x0;
        break;

    case bcmVlanPortTranslateEgressVnidEnable:
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hd2, PROCESS_CTRLs, &get_val));
        fld_mask = 1 << BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_SFT;
        *arg = (get_val & fld_mask)? 0x1:0x0;
        break;

    case bcmVlanTranslateEgressEnable:
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hd2, PROCESS_CTRLs, &get_val));
        fld_mask = 1 << BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_SFT;
        *arg = (get_val & fld_mask)? 0x0:0x1;
        break;

    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hd2) {
        (void) bcmlt_entry_free(entry_hd2);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int arg)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    uint64_t process_ctrl_val = 0;
    uint8_t process_ing_svp = 0;
    uint8_t process_egr_dvp = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hd2 = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if ((type == bcmVlanPortUseOuterPri) ||
        (type == bcmVlanPortUseInnerPri)) {
        process_ing_svp = 1;
    } else if ((type == bcmVlanPortPayloadTagsDelete) ||
        (type == bcmVlanPortTranslateEgressVnidEnable) ||
        (type == bcmVlanTranslateEgressEnable)) {
        process_egr_dvp = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (process_ing_svp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SVPs, vp));
    }
    if (process_egr_dvp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, EGR_DVPs, &entry_hd2));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hd2, DVPs, vp));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hd2, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hd2, PROCESS_CTRLs, &process_ctrl_val));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(entry_hd2));
    }

    switch (type) {
    case bcmVlanPortUseOuterPri:
        set_val = arg ? 0x1: 0x0;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, USE_OTAG_DOT1P_CFI_FOR_PHBs,
                                   set_val));
        break;

    case bcmVlanPortUseInnerPri:
        set_val = arg ? 0x1: 0x0;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, USE_ITAG_DOT1P_CFI_FOR_PHBs,
                                   set_val));
        break;

    case bcmVlanPortPayloadTagsDelete:
        process_ctrl_val &= ~(BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_MSK <<
                             BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_SFT);
        if (arg) {
            process_ctrl_val |= (0x1 & BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_MSK) <<
                                 BCMI_XFS_VP_PROCESS_CTRL_ITAG_DEL_SFT;
        }

        break;

    case bcmVlanPortTranslateEgressVnidEnable:
        process_ctrl_val &= ~(BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_MSK <<
                             BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_SFT);
        if (arg) {
            process_ctrl_val |= (0x1 & BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_MSK) <<
                                 BCMI_XFS_VP_PROCESS_CTRL_EVXLT2_EN_SFT;
        }
        break;

    case bcmVlanTranslateEgressEnable:
        process_ctrl_val &= ~(BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_MSK <<
                              BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_SFT);
        process_ctrl_val &= ~(BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_MSK <<
                              BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_SFT);
        if (arg == 0) {
            process_ctrl_val |= (0x1 & BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_MSK) <<
                                 BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_DISABLE_SFT;
        } else {
            process_ctrl_val |= (0x1 & BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_MSK) <<
                                 BCMI_XFS_VP_PROCESS_CTRL_EVXLT1_SELECT_SFT;
        }
        break;

    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (process_ing_svp) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (process_egr_dvp) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hd2, DVPs, vp));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hd2, PROCESS_CTRLs,
                                   process_ctrl_val));
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, entry_hd2, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    if (BCMLT_INVALID_HDL != entry_hd2) {
        (void) bcmlt_entry_free(entry_hd2);
    }

    SHR_FUNC_EXIT();
}

int xfs_ltsw_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port,
                                          int *enable, int *mirror_idx)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    uint32_t fld_width = 0;
    int i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, MIRROR_ENABLEs, &get_val));
    *enable = (int)get_val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_width_get(unit, ING_SVP_TABLEs,
                                 MIRROR_ENABLEs, &fld_width));
    for (i = 0; i < fld_width; i++) { 
        if (i == 0) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, MIRROR_INDEX_0s, &get_val));
            *mirror_idx = (int)get_val;
        } else if (i == 1) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, MIRROR_INDEX_1s, &get_val));
            *(mirror_idx + 1) = (int)get_val;
        } else if (i == 2) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, MIRROR_INDEX_2s, &get_val));
            *(mirror_idx + 2) = (int)get_val;
        } else if (i == 3) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(entry_hdl, MIRROR_INDEX_3s, &get_val));
            *(mirror_idx + 3) = (int)get_val;
        }
    }
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int xfs_ltsw_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port,
                                          int enable, int *mirror_idx)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    int process_enable_val = 0;
    uint64_t get_val;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, MIRROR_ENABLEs, &get_val));
    process_enable_val = (int)get_val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_clear(entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SVPs, vp));

    if (mirror_idx == NULL) {
        /* Disable Mirror sessions */
        process_enable_val &= enable;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_ENABLEs,
                                   process_enable_val));
    } else {
        /* Enable Mirror sessions and configure mirror_index */
        if (enable & (1 << 0)) {
            set_val = *mirror_idx;
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MIRROR_INDEX_0s, set_val));
        }
        if (enable & (1 << 1)) {
            set_val = *(mirror_idx + 1);
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MIRROR_INDEX_1s, set_val));
        }
        if (enable & (1 << 2)) {
            set_val = *(mirror_idx + 2);
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MIRROR_INDEX_2s, set_val));
        }
        if (enable & (1 << 3)) {
            set_val = *(mirror_idx + 3);
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(entry_hdl, MIRROR_INDEX_3s, set_val));
        }
        process_enable_val |= enable;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_ENABLEs,
                                   process_enable_val));
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    switch (mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            set_val = 0;
            break;

        case BCM_PORT_DSCP_MAP_ALL:
            set_val = 1;
            break;

        case BCM_PORT_DSCP_MAP_ZERO:
        case BCM_PORT_DSCP_MAP_IPV4_ONLY:
        case BCM_PORT_DSCP_MAP_IPV6_ONLY:
        case BCM_PORT_DSCP_MAP_IPV4_NONE:
        case BCM_PORT_DSCP_MAP_IPV6_NONE:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, USE_DSCP_FOR_PHBs, set_val));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, USE_DSCP_FOR_PHBs, &get_val));
    *mode = get_val ? BCM_PORT_DSCP_MAP_ALL : BCM_PORT_DSCP_MAP_NONE;

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, DOT1P_PTRs, &get_val));
    *map_ptr = (uint32_t)get_val;

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int dunit = 0;
    int vp = -1;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, DOT1P_PTRs, map_ptr));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, DSCP_PTRs, &get_val));
    *map_ptr = (uint32_t)get_val;

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int dunit = 0;
    int vp = -1;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_SVP_TABLEs, &entry_hd_svp));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, SVPs, vp));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, DSCP_PTRs, map_ptr));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                              int *map_ptr)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_DVPs, &entry_hd));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd, DVPs, vp));
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hd, DOT1P_REMARK_BASE_PTRs, &get_val));
    *map_ptr = (uint32_t)get_val;

exit:
    if (BCMLT_INVALID_HDL != entry_hd) {
        (void) bcmlt_entry_free(entry_hd);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                              int map_ptr)
{
    int dunit = 0;
    int vp = -1;
    bcmlt_entry_handle_t entry_hd = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_DVPs, &entry_hd));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd, DVPs, vp));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hd, DOT1P_REMARK_BASE_PTRs, map_ptr));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd) {
        (void) bcmlt_entry_free(entry_hd);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_nw_group_num_get(int unit, int *num)
{
    uint32_t fld_width = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(num, SHR_E_PARAM);

    rv = bcmi_lt_field_width_get(unit, ING_SVP_TABLEs,
                                 NETWORK_GROUP_BITMAPs,
                                 &fld_width);
    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_UNAVAIL;
    }

    /* Bit 0 of NETWORK_GROUP_BITMAPs is reserved. */
    *num = fld_width - 1;

    SHR_ERR_EXIT(rv);
exit:

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_init(int unit)
{
    int dunit = 0;
    bcmlt_entry_handle_t ing_dvp_hd = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t egr_dvp_hd = BCMLT_INVALID_HDL;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    int profile_idx = 0;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Strength of ING_DVP_TABLE entry 0 points to NONE profile. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_DVP_TABLEs, &ing_dvp_hd));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ing_dvp_hd, DVPs, 0));

    pth = BCMI_LTSW_SBR_PTH_ING_DVP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                             &profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ing_dvp_hd, STRENGTH_PROFILE_INDEXs,
                               profile_idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, ing_dvp_hd, BCMLT_PRIORITY_NORMAL));

    /* Strength of EGR_DVP entry 0 points to NONE profile. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, EGR_DVPs, &egr_dvp_hd));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(egr_dvp_hd, DVPs, 0));

    pth = BCMI_LTSW_SBR_PTH_EGR_DVP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type,
                                             &profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(egr_dvp_hd, STRENGTH_PRFL_IDXs,
                               profile_idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_set_commit(unit, egr_dvp_hd, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ing_dvp_hd) {
        (void) bcmlt_entry_free(ing_dvp_hd);
    }

    if (BCMLT_INVALID_HDL != egr_dvp_hd) {
        (void) bcmlt_entry_free(egr_dvp_hd);
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_virtual_tnl_index_get(int unit, int *tnl_min, int *tnl_max)
{
    uint64_t min = 0;
    uint64_t max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_value_range_get(unit, ING_SVP_TABLEs, SVPs,
                                       &min, &max));
    /*
     * VP index is used for VP validation and 0 is treated as invalid VP.
     * Tunnel is VP based and shares same range with VP.
     */
    *tnl_min = min > 1? (int)min:1;
    *tnl_max = (int)max;

exit:
    SHR_FUNC_EXIT();
}
