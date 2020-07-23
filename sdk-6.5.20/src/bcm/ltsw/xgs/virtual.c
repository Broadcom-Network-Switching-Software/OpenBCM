/*! \file virtual.c
 *
 * Virtual Driver for XGS chips.
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
#include <bcm_int/ltsw/xgs/virtual.h>
#include <bcm_int/ltsw/xgs/types.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/flexctr.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/*!
 * \brief Mirror container number on virtual port.
 */
#define VP_MIRROR_CONTAINER_NUM_MAX 4


/*! Index to the flex action profile entry which disables action. */
#define PORT_FLEX_ACTION_PROFILE_DEF   (0)

/*
 * \brief Source virtual port flex action profile structure.
 *
 * This data structure is used to identify the profile structure for flex
 * aciton check.
 */
typedef struct port_flex_ctr_profile_s {
    uint16_t action;
} port_flex_ctr_profile_t;

/******************************************************************************
 * Private functions
 */

static int
xgs_ltsw_virtual_port_flex_ctr_profile_entry_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(port_flex_ctr_profile_t));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_entry_get(
    int unit,
    int idx,
    port_flex_ctr_profile_t *prof_entry)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t get_val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_ING_EFLEX_ACTIONs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_SVP_ING_EFLEX_ACTION_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ACTIONs, &get_val));
    prof_entry->action = (uint16_t)get_val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int index,
    int *cmp_result)
{
    port_flex_ctr_profile_t prof_entry;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    sal_memset(&prof_entry, 0, sizeof(prof_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_entry_get(unit, index,
                                                          &prof_entry));

    *cmp_result = sal_memcmp(entries, &prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}


static int
xgs_ltsw_virtual_port_flex_ctr_profile_entry_set(
    int unit,
    int idx,
    port_flex_ctr_profile_t *prof_entry)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_ING_EFLEX_ACTIONs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_SVP_ING_EFLEX_ACTION_IDs, idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ACTIONs, prof_entry->action));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_entry_del(
    int unit,
    int idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_ING_EFLEX_ACTIONs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_SVP_ING_EFLEX_ACTION_IDs, idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_get(
    int unit,
    port_flex_ctr_profile_t *entry,
    int index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_entry_get(unit, index, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_add(
    int unit,
    port_flex_ctr_profile_t *entry,
    int *index)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE,
                                          entry, 0, 1, index);
    if (rv == SHR_E_EXISTS) {
        /* the same profile already exists */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
       SHR_ERR_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_entry_set(unit, *index, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_delete(
    int unit,
    int index)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE,
                                      index);

    if (rv == SHR_E_BUSY) {
        /* profile entry is still inused */
       SHR_EXIT();
    }

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    /* empty the profile entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_entry_del(unit, index));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_recover(
    int unit,
    int index)
{
    uint32_t ref_cnt;
    uint64_t idx_min, idx_max;
    port_flex_ctr_profile_t profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_SVP_ING_EFLEX_ACTIONs,
                                       PORT_SVP_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));

    if ((index < idx_min) || (index > idx_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit,
                                         BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE,
                                         index, &ref_cnt));
    if (ref_cnt == 0) {
        /* If profile entry has not been initialized. */
        sal_memset(&profile, 0, sizeof(profile));
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_virtual_port_flex_ctr_profile_get(unit, &profile, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit,
                 BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE,
                 &profile, 1, index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase
            (unit,
             BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE,
             1, index, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_flex_ctr_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE;
    uint64_t idx_max, idx_min;
    int ent_idx_min, ent_idx_max;
    int idx;
    port_flex_ctr_profile_t profile;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    int dunit;
    uint64_t data;
    uint32_t tbl_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_SVP_ING_EFLEX_ACTIONs,
                                       PORT_SVP_ING_EFLEX_ACTION_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register
            (unit, &profile_hdl,
            &ent_idx_min, &ent_idx_max, 1,
            xgs_ltsw_virtual_port_flex_ctr_profile_entry_hash_cb,
            xgs_ltsw_virtual_port_flex_ctr_profile_entry_cmp_cb));

    if (bcmi_warmboot_get(unit)) {
        /* Recover */
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hdl));
        while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                          BCMLT_OPCODE_TRAVERSE,
                                          BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, PORT_SVP_ING_EFLEX_ACTION_IDs,
                                       &data));
            idx = data;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_flex_ctr_profile_recover(unit, idx));
        }

        if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
            SHR_ERR_EXIT(rv);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, PORT_SVP_ING_EFLEX_ACTIONs));

        /* Reserve profile 0 for default to disable flex action. */
        sal_memset(&profile, 0, sizeof(profile));
        profile.action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_virtual_port_flex_ctr_profile_add(unit, &profile,
                                                        &idx));
        if (idx != PORT_FLEX_ACTION_PROFILE_DEF) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, PORT_SVPs, &tbl_size));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_ref_count_increase(unit,
                 BCMI_LTSW_PROFILE_SVP_FLEX_ACT_PROFILE, 1, idx, tbl_size));
        }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_ing_stat_update(int unit, bcm_port_t port,
                                      bcmi_ltsw_flexctr_counter_info_t *info,
                                      uint8_t flag)
{
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;
    uint64_t flex_ctr_action;
    int dunit = 0;
    int vp;
    int action_profile, old_action_profile;
    port_flex_ctr_profile_t action_entry;
    uint64_t get_val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, PORT_SVP_ING_EFLEX_ACTION_IDs,
                               &get_val));
    old_action_profile = (int)get_val;
    sal_memset(&action_entry, 0, sizeof(action_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_entry_get(unit,
                                                          old_action_profile,
                                                          &action_entry));
    flex_ctr_action = action_entry.action;
    if (flag) {
        /* Flex counter action must not attached. */
        if (flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                                (BSL_META_U(unit, "%d already attached!\n"),
                                            (int)flex_ctr_action));
        }
    } else {
        /* Flex counter action must already attached. */
        if (flex_ctr_action != (uint64_t)info->action_index) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "%d not expected (%d)!\n"),
                                            (int)flex_ctr_action,
                                            (int)info->action_index));
        }
    }

    if (flag) {
        flex_ctr_action = (uint64_t)info->action_index;
    } else {
        flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    }
    sal_memset(&action_entry, 0, sizeof(action_entry));
    action_entry.action = flex_ctr_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_add(unit, &action_entry,
                                                    &action_profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_ING_EFLEX_ACTION_IDs,
                               action_profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    /* Remove the old action profile during detach. */
    if ((!flag) && (old_action_profile != PORT_FLEX_ACTION_PROFILE_DEF)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_virtual_port_flex_ctr_profile_delete(unit,
                                                           old_action_profile));
    }

exit:
    if (entry_hd_svp != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_egr_stat_update(int unit, bcm_port_t port,
                                      bcmi_ltsw_flexctr_counter_info_t *info,
                                      uint8_t flag)
{
    bcmlt_entry_handle_t entry_hd = BCMLT_INVALID_HDL;
    uint64_t flex_ctr_action;
    int dunit = 0;
    int vp;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_DVPs, &entry_hd));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd, PORT_DVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd, CTR_EGR_EFLEX_ACTIONs,
                               &flex_ctr_action));

    if (flag) {
        /* Flex counter action must not attached. */
        if (flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_BUSY,
                                (BSL_META_U(unit, "%d already attached!\n"),
                                            (int)flex_ctr_action));
        }
    } else {
        /* Flex counter action must already attached. */
        if (flex_ctr_action != (uint64_t)info->action_index) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_PARAM,
                                (BSL_META_U(unit, "%d not expected (%d)!\n"),
                                            (int)flex_ctr_action,
                                            (int)info->action_index));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(entry_hd));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd, PORT_DVP_IDs, vp));

    if (flag) {
        flex_ctr_action = (uint64_t)info->action_index;
    } else {
        flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd, CTR_EGR_EFLEX_ACTIONs,
                               &flex_ctr_action));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hd != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hd);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_virtual_port_egress_adapt_key_set(
    int unit,
    bcm_gport_t port,
    int key_type)
{
    int vp;
    int old_key_type;
    int stored = 0;

    SHR_FUNC_ENTER(unit);

    if ((key_type != bcmVlanTranslateEgressKeyInvalid) &&
        (key_type != bcmVlanTranslateEgressKeyVpn) &&
        (key_type != bcmVlanTranslateEgressKeyVpnGportGroup) &&
        (key_type != bcmVlanTranslateEgressKeyVpnPortGroup) &&
        (key_type != bcmVlanTranslateEgressKeySrcGport)
        ) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));


    /*
     * Sync to HW in case egress object has been linkaged to VP.
     * If not, sync to HW when the linkage during port_encap.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_port_egress_adapt_key_get(unit, port, &old_key_type));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_port_egress_adapt_key_set(unit, port, key_type));
    stored = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_port_egress_adapt_key_sync(
            unit, port, BCM_IF_INVALID));

exit:
    if (SHR_FUNC_ERR() && stored) {
        (void)bcmi_ltsw_virtual_port_egress_adapt_key_set(unit, port,
                                                          old_key_type);
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
xgs_ltsw_virtual_index_get(int unit, int *vfi_min, int *vfi_max, int *vp_min,
                           int *vp_max, int *l2_iif_min, int *l2_iif_max)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_vfi_init(int unit, int vfi)
{
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int default_profile_id;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VFIs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFI_IDs, vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, FIDs, vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VP_REPLICATIONs, 0x1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_mshp_default_profile_get(unit,
                                                 BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                                 &default_profile_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFI_ING_MEMBER_PORTS_PROFILE_IDs,
                               default_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_mshp_default_profile_get(unit,
                           BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE,
                           &default_profile_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFI_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               default_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_mshp_default_profile_get(unit,
                                                 BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                                 &default_profile_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFI_EGR_MEMBER_PORTS_PROFILE_IDs,
                               default_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}


int
xgs_ltsw_virtual_vfi_cleanup(int unit, int vfi)
{
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t get_val;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VFIs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VFI_IDs, vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VFI_ING_MEMBER_PORTS_PROFILE_IDs,
                               &get_val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit, BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                       (int)get_val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VFI_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               &get_val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit,
                                       BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE,
                                       (int)get_val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VFI_EGR_MEMBER_PORTS_PROFILE_IDs,
                               &get_val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_profile_delete(unit, BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                       (int)get_val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}


int
xgs_ltsw_virtual_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    int vp = 0;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {MAC_DROPs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {MAC_COPY_TO_CPUs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3 */ {MAC_LEARNs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint32_t valid_flags;

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_ARL;
    if ((flags | valid_flags) != valid_flags) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    fields[1].u.val = (flags & BCM_PORT_LEARN_FWD) ? 0 : 1;
    fields[2].u.val = (flags & BCM_PORT_LEARN_CPU) ? 1 : 0;
    fields[3].u.val = (flags & BCM_PORT_LEARN_ARL) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PORT_SVPs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_virtual_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {MAC_DROPs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2 */ {MAC_COPY_TO_CPUs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3 */ {MAC_LEARNs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PORT_SVPs, &lt_entry, NULL, NULL));

    *flags = 0;
    if (!fields[1].u.val) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (fields[2].u.val) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (fields[3].u.val) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_move_set(int unit, bcm_port_t port, uint32_t flags)
{
    int vp = 0;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {MAC_MOVE_DROPs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {MAC_MOVE_COPY_TO_CPUs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3 */ {MAC_MOVEs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint32_t valid_flags;

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_ARL;
    if ((flags | valid_flags) != valid_flags) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    fields[1].u.val = (flags & BCM_PORT_LEARN_FWD) ? 0 : 1;
    fields[2].u.val = (flags & BCM_PORT_LEARN_CPU) ? 1 : 0;
    fields[3].u.val = (flags & BCM_PORT_LEARN_ARL) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, PORT_SVPs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_virtual_port_move_get(int unit, bcm_port_t port, uint32_t *flags)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {MAC_MOVE_DROPs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2 */ {MAC_MOVE_COPY_TO_CPUs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3 */ {MAC_MOVEs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, PORT_SVPs, &lt_entry, NULL, NULL));

    *flags = 0;
    if (!fields[1].u.val) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (fields[2].u.val) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (fields[3].u.val) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                      bcm_vlan_action_set_t *action)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_port_vlan_action_reset(int unit, bcm_port_t port)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_port_class_get(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t *pclass_id)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[2] =
    {
        /*0 */ {PORT_DVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {DVP_GRPs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if ((pclass != bcmPortClassFieldIngressSourceGport) &&
        (pclass != bcmPortClassVlanTranslateEgress)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    if (pclass == bcmPortClassFieldIngressSourceGport) {
        fields[0].fld_name = PORT_SVP_IDs;
        fields[1].fld_name = INPORT_BITMAP_INDEXs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PORT_SVPs, &lt_entry, NULL, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PORT_DVPs, &lt_entry, NULL, NULL));
    }

    *pclass_id = fields[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_class_set(int unit, bcm_port_t port,
                                bcm_port_class_t pclass,
                                uint32_t pclass_id)
{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[2] =
    {
        /*0 */ {PORT_DVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {DVP_GRPs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if ((pclass != bcmPortClassFieldIngressSourceGport) &&
        (pclass != bcmPortClassVlanTranslateEgress)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = vp;
    fields[1].u.val = pclass_id;
    if (pclass == bcmPortClassFieldIngressSourceGport) {
        fields[0].fld_name = PORT_SVP_IDs;
        fields[1].fld_name = INPORT_BITMAP_INDEXs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PORT_SVPs, &lt_entry, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PORT_DVPs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int *arg)
{
    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmVlanPortTranslateEgressKey:
            /* Value in SW cache returned. No need to inspect HW.*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_port_egress_adapt_key_get(unit, port, arg));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                    bcm_vlan_control_port_t type,
                                    int arg)
{
    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmVlanPortTranslateEgressKey:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_egress_adapt_key_set(unit, port, arg));

            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


exit:
    SHR_FUNC_EXIT();
}


int xgs_ltsw_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port,
                                          int *enable, int *mirror_idx)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    int i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    *enable = 0;
    *mirror_idx = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_ING_MIRRORs, &entry_hdl));
    for (i = 0; i < VP_MIRROR_CONTAINER_NUM_MAX; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(entry_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_SVP_IDs, vp));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_CONTAINER_IDs, i));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, MIRRORs, &get_val));
        if (get_val) {
            *enable |= (1 << i);

        }
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int xgs_ltsw_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port,
                                          int enable, int *mirror_idx)
{
    int dunit = 0;
    int vp = -1;
    int set_val = 0;
    int i = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Mirror session index configuration not supported. */
    if (mirror_idx) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_ING_MIRRORs, &entry_hdl));
    for (i = 0; i < VP_MIRROR_CONTAINER_NUM_MAX; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(entry_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_SVP_IDs, vp));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRROR_CONTAINER_IDs, i));
        set_val = (enable & (1 << i));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MIRRORs, set_val));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, entry_hdl,
                                      BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int dunit = 0;
    int vp = -1;
    int ipv4 = -1;
    int ipv6 = -1;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    switch (mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            ipv4 = 0;
            ipv6 = 0;
            break;
        case BCM_PORT_DSCP_MAP_ALL:
            ipv4 = 1;
            ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_ONLY:
            ipv4 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_ONLY:
            ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_NONE:
            ipv4 = 0;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_NONE:
            ipv6 = 0;
            break;
        case BCM_PORT_DSCP_MAP_ZERO:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    if (ipv4 > -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hd_svp, TRUST_PHB_ING_DSCP_V4s,
                                   ipv4));
    }
    if (ipv6 > -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hd_svp, TRUST_PHB_ING_DSCP_V6s,
                                   ipv6));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    uint32_t ipv4 = 0;
    uint32_t ipv6 = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, TRUST_PHB_ING_DSCP_V4s, &get_val));
    ipv4 = (uint32_t)(get_val);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, TRUST_PHB_ING_DSCP_V6s, &get_val));
    ipv6 = (uint32_t)(get_val);

    if (ipv4 && ipv6) {
        *mode = BCM_PORT_DSCP_MAP_ALL;
    } else if (ipv4) {
        *mode = BCM_PORT_DSCP_MAP_IPV4_ONLY;
    } else if (ipv6) {
        *mode = BCM_PORT_DSCP_MAP_IPV6_ONLY;
    } else {
        *mode = BCM_PORT_DSCP_MAP_NONE;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd_svp, PHB_ING_L2_IDs, &get_val));
    *map_ptr = (uint32_t)get_val;

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int dunit = 0;
    int vp = -1;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PHB_ING_L2_IDs, map_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr)
{
    int dunit = 0;
    int vp = -1;
    uint64_t get_val = 0;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hd_svp,
                                PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs,
                                &get_val));
    *map_ptr = (uint32_t)get_val;

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr)
{
    int dunit = 0;
    int vp = -1;
    bcmlt_entry_handle_t entry_hd_svp = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVPs, &entry_hd_svp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp, PORT_SVP_IDs, vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hd_svp,
                               PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs,
                               map_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hd_svp, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hd_svp) {
        (void) bcmlt_entry_free(entry_hd_svp);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                              int *map_ptr)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                              int map_ptr)
{
    return SHR_E_UNAVAIL;
}

int
xgs_ltsw_virtual_nw_group_num_get(int unit, int *num)
{
    uint32_t fld_width = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(num, SHR_E_PARAM);

    rv = bcmi_lt_field_width_get(unit, PORT_SVPs, SVP_NETWORK_GRPs, &fld_width);
    if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_UNAVAIL;
    }

    *num = 1<<fld_width;

    SHR_ERR_EXIT(rv);
exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* warmboot is handled within the interface. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_port_flex_ctr_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_control_get(int unit, bcm_port_t port,
                                  bcm_port_control_t type, int *value)

{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t svp_fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SKIP_VP_PRUNINGs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_lt_field_t dvp_fields[] =
    {
        /*0 */ {PORT_DVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SKIP_VP_PRUNINGs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    uint32_t svp_read = 0;
    uint32_t dvp_read = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    svp_fields[0].u.val = vp;
    dvp_fields[0].u.val = vp;

    svp_fields[1].u.val = 0;
    switch (type) {
        case bcmPortControlBridge:
            svp_fields[1].fld_name = SKIP_VP_PRUNINGs;
            svp_read = 1;

            break;
        case bcmPortControlIP4:
            svp_fields[1].fld_name = V4L3s;
            svp_read = 1;

            break;
        case bcmPortControlIP6:
            svp_fields[1].fld_name = V6L3s;
            svp_read = 1;

            break;
        case bcmPortControlIP4Mcast:
            svp_fields[1].fld_name = V4IPMCs;
            svp_read = 1;

            break;

        case bcmPortControlIP6Mcast:
            svp_fields[1].fld_name = V6IPMCs;
            svp_read = 1;

            break;

        case bcmPortControlIP4McastL2:
            svp_fields[1].fld_name = L2_FWD_IPMCV4s;
            svp_read = 1;

            break;

        case bcmPortControlIP6McastL2:
            svp_fields[1].fld_name = L2_FWD_IPMCV6s;
            svp_read = 1;

            break;

        case bcmPortControlDoNotCheckVlan:
            svp_fields[1].fld_name = SKIP_VLAN_CHECKs;
            svp_read = 1;

            break;

        case bcmPortControlDoNotCheckVlanEgress:
            dvp_fields[1].fld_name = SKIP_VLAN_CHECKs;
            dvp_read = 1;

            break;

        case bcmPortControlL2Learn:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_learn_get(unit, port, (uint32_t *)value));
            SHR_EXIT();

            break;

        case bcmPortControlL2Move:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_move_get(unit, port, (uint32_t *)value));
            SHR_EXIT();

            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (svp_read) {
        lt_entry.fields = svp_fields;
        lt_entry.nfields = sizeof(svp_fields) / sizeof(svp_fields[0]);
        lt_entry.attr = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PORT_SVPs, &lt_entry, NULL, NULL));
        *value = svp_fields[1].u.val;
    }
    if (dvp_read) {
        lt_entry.fields = dvp_fields;
        lt_entry.nfields = sizeof(dvp_fields) / sizeof(dvp_fields[0]);
        lt_entry.attr = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PORT_DVPs, &lt_entry, NULL, NULL));
        *value = dvp_fields[1].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_control_set(int unit, bcm_port_t port,
                                  bcm_port_control_t type, int value)

{
    int vp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t svp_fields[] =
    {
        /*0 */ {PORT_SVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SKIP_VP_PRUNINGs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t dvp_fields[] =
    {
        /*0 */ {PORT_DVP_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SKIP_VP_PRUNINGs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    uint32_t svp_update = 0;
    uint32_t dvp_update = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_type_vp_get(unit, port, &vp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_ANY));

    svp_fields[0].u.val = vp;
    dvp_fields[0].u.val = vp;
    switch (type) {
        case bcmPortControlBridge:
            svp_fields[1].fld_name = SKIP_VP_PRUNINGs;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            dvp_fields[1].fld_name = SKIP_VP_PRUNINGs;
            dvp_fields[1].u.val = value ? 1 : 0;
            dvp_update = 1;

            break;
        case bcmPortControlIP4:
            svp_fields[1].fld_name = V4L3s;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;
        case bcmPortControlIP6:
            svp_fields[1].fld_name = V6L3s;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;
        case bcmPortControlIP4Mcast:
            svp_fields[1].fld_name = V4IPMCs;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;

        case bcmPortControlIP6Mcast:
            svp_fields[1].fld_name = V6IPMCs;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;

        case bcmPortControlIP4McastL2:
            svp_fields[1].fld_name = L2_FWD_IPMCV4s;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;

        case bcmPortControlIP6McastL2:
            svp_fields[1].fld_name = L2_FWD_IPMCV6s;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;

        case bcmPortControlDoNotCheckVlan:
            svp_fields[1].fld_name = SKIP_VLAN_CHECKs;
            svp_fields[1].u.val = value ? 1 : 0;
            svp_update = 1;

            break;

        case bcmPortControlDoNotCheckVlanEgress:
            dvp_fields[1].fld_name = SKIP_VLAN_CHECKs;
            dvp_fields[1].u.val = value ? 1 : 0;
            dvp_update = 1;

            break;

        case bcmPortControlL2Learn:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_learn_set(unit, port, value));
            SHR_EXIT();

            break;

        case bcmPortControlL2Move:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_virtual_port_move_set(unit, port, value));
            SHR_EXIT();

            break;

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (svp_update) {
        lt_entry.fields = svp_fields;
        lt_entry.nfields = sizeof(svp_fields) / sizeof(svp_fields[0]);
        lt_entry.attr = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PORT_SVPs, &lt_entry, NULL));
    }
    if (dvp_update) {
        lt_entry.fields = dvp_fields;
        lt_entry.nfields = sizeof(dvp_fields) / sizeof(dvp_fields[0]);
        lt_entry.attr = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_update(unit, PORT_DVPs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_stat_attach(int unit, bcm_port_t port,
                                  uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id,
                                               &counter_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));
    /* PORT_SVP supports action profile while PORT_DVP support actions. */
    if (counter_info.direction == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_CONT
            (xgs_ltsw_virtual_port_ing_stat_update(unit, port, &counter_info,
                                                   TRUE));
    } else {
        SHR_IF_ERR_CONT
            (xgs_ltsw_virtual_port_egr_stat_update(unit, port, &counter_info,
                                                   TRUE));
    }

    if (SHR_FUNC_ERR()) {
        (void)
        bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                          stat_counter_id);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                          uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id,
                                               &counter_info));

    /* PORT_SVP supports action profile while PORT_DVP support actions. */
    if (counter_info.direction == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_virtual_port_ing_stat_update(unit, port, &counter_info,
                                                   FALSE));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_virtual_port_egr_stat_update(unit, port, &counter_info,
                                                   FALSE));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    int network_group_num = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t set_val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_nw_group_num_get(unit, &network_group_num));

    if ((source_network_group_id < 0) ||
        (source_network_group_id >= network_group_num)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((config->dest_network_group_id < 0) ||
        (config->dest_network_group_id >= network_group_num)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Do not support ingress pruning and multicast remapping. */
    if (config->config_flags & (BCM_SWITCH_NETWORK_GROUP_MCAST_REMAP_ENABLE |
        BCM_SWITCH_NETWORK_GROUP_INGRESS_PRUNE_ENABLE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_NETWORK_GRP_PRUNEs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_SVP_NETWORK_GRPs,
                               source_network_group_id));
    if (config->config_flags &
        BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE) {
        set_val = 1;
    } else {
        set_val = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, PRUNINGs,
                                     config->dest_network_group_id,
                                     &set_val, 1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    int network_group_num = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t get_val;
    uint32_t r_elem_cnt = 0;
    int local_rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(config, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_virtual_nw_group_num_get(unit, &network_group_num));

    if ((source_network_group_id < 0) ||
        (source_network_group_id >= network_group_num)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((config->dest_network_group_id < 0) ||
        (config->dest_network_group_id >= network_group_num)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    config->config_flags &=
        ~(BCM_SWITCH_NETWORK_GROUP_MCAST_REMAP_ENABLE |
          BCM_SWITCH_NETWORK_GROUP_INGRESS_PRUNE_ENABLE |
          BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE);


    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SVP_NETWORK_GRP_PRUNEs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_SVP_NETWORK_GRPs,
                               source_network_group_id));

    local_rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                    BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL);
    if (local_rv == SHR_E_NOT_FOUND) {
        /* No need to update the flag. */
        SHR_ERR_EXIT(SHR_E_NONE);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl, PRUNINGs,
                                         config->dest_network_group_id,
                                         &get_val, 1, &r_elem_cnt));
        if (get_val) {
            config->config_flags |= BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}
