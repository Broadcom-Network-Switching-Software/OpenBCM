/*! \file field.c
 *
 * Field Destination Driver for XFS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/field.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/field_destination.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/xfs/field_destination.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FP

#define DEST_TYPE_L2_MC   (1 << 0)
#define DEST_TYPE_L3_MC   (1 << 1)
#define DEST_TYPE_L2_OIF  (1 << 2)

#define DEST_TYPE_MASK  0xf

#define BCM_FIELD_DESTINATION_ACTION_COS 8

#define OPAQUE_CTRL_ID_COS_MAP_VALID   1 << 0

/* Entry priority 0 is used for the default dump entry. */
#define ENTRY_PRIORITY_LOWEST 0
#define ENTRY_PRIORITY_BASE 1
#define ENTRY_NUM_RSVD      2

#define DEVICE_PORT_TYPE_FRONT_PANEL   (1 << 0)
#define DEVICE_PORT_TYPE_HIGIG3        (1 << 1)

/* ETRAP_CONTROL format. */
#define ETRAP_CTRL_ELEPHANT    (1 << 1)
#define ETRAP_CTRL_NOTIFY      (1 << 2)


typedef struct fp_dest_cos_map_entry_s {
    uint8_t    uc_cos;

    uint8_t    mc_cos;

    uint8_t    rqe_cos;

    uint8_t    cpu_cos;
} fp_dest_cos_map_entry_t;

typedef enum fp_dest_cng_redirect_action_e {
    NO_CNG_REDIRECT    = 0,
    CNG_ONLY           = 1,
    REDIRECT_ONLY      = 2,
    CNG_REDIRECT       = 3,
    CNG_REDIRECT_ACTION_COUNT
} fp_dest_cng_redirect_action_t;

/* Field destination information. */
typedef struct xfs_field_dest_info_s {

    /* Initialized flag. */
    int                    initialized;

    /* Max priority of DESTINATION_FP_TABLE. */
    int max_pri;

    /* Min priority of DESTINATION_FP_TABLE. */
    int min_pri;

    /* SBR of DESTINATION_FP_TABLE. */
    int                    sbr_index[CNG_REDIRECT_ACTION_COUNT];

} xfs_field_dest_info_t;

static xfs_field_dest_info_t dfp_info[BCM_MAX_NUM_UNITS];
#define DFP_INFO(_u_)           (&(dfp_info[_u_]))
#define MAX_PRI(_u_)            (DFP_INFO(_u_)->max_pri)
#define MIN_PRI(_u_)            (DFP_INFO(_u_)->min_pri)
#define SBR_INDX(_u_, _i_)      (DFP_INFO(_u_)->sbr_index[_i_])

/******************************************************************************
 * Private functions
 */
/*
 * \brief Get an entry from FP_DESTINATION_COS_Q_MAP.
 *
 * This function is used to get an entry from FP_DESTINATION_COS_Q_MAP.
 *
 * \param [in]  unit           Unit Number.
 * \param [in]  index          FP_DESTINATION_COS_Q_MAP entry index.
 * \param [out] entry          FP_DESTINATION_COS_Q_MAP entry.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
fp_dest_cos_map_entry_get(
    int unit,
    int index,
    fp_dest_cos_map_entry_t *entry)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t value;
    int dunit = 0;
    int  num_ucq, num_mcq;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_DESTINATION_COS_Q_MAPs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FP_DESTINATION_COS_Q_MAP_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, UC_COSs, &value));
    entry->uc_cos = (uint8_t)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    if (num_mcq != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, MC_COSs, &value));
        entry->mc_cos = (uint8_t)value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, RQE_COSs, &value));
        entry->rqe_cos = (uint8_t)value;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, CPU_COSs, &value));
    entry->cpu_cos = (uint8_t)value;


exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry from FP_DESTINATION_COS_Q_MAP.
 *
 * This function is used to add an entry from FP_DESTINATION_COS_Q_MAP.
 *
 * \param [in]  unit           Unit Number.
 * \param [in]  index          FP_DESTINATION_COS_Q_MAP entry index.
 * \param [out] entry          FP_DESTINATION_COS_Q_MAP entry.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
fp_dest_cos_map_entry_add(
    int unit,
    int index,
    fp_dest_cos_map_entry_t *entry)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    int  num_ucq, num_mcq;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_DESTINATION_COS_Q_MAPs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FP_DESTINATION_COS_Q_MAP_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, UC_COSs, entry->uc_cos));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    if (num_mcq != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, MC_COSs, entry->mc_cos));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, RQE_COSs, entry->rqe_cos));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, CPU_COSs, entry->cpu_cos));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}


/*
 * \brief Delete an entry of FP_DESTINATION_COS_Q_MAP.
 *
 * This function is used to delete an entry of FP_DESTINATION_COS_Q_MAP.
 *
 * \param [in]  unit           Unit Number.
 * \param [in]  index          FP_DESTINATION_COS_Q_MAP entry index.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
fp_dest_cos_map_entry_delete(
    int unit,
    int index)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, FP_DESTINATION_COS_Q_MAPs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FP_DESTINATION_COS_Q_MAP_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for FP_DESTINATION_COS_Q_MAP profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] entries FP_DESTINATION_COS_Q_MAP profile entry.
 * \param [in] entries_per_set Number of entries in a profile set.
 * \param [out] hash Hash signature.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fp_dest_cos_map_profile_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (!entries || (entries_per_set != 1) || !hash) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *hash = _shr_crc16(0, (uint8 *)entries, sizeof(fp_dest_cos_map_entry_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT FP_DESTINATION_COS_Q_MAP.
 *
 * This function is used to compare profile set of LT FP_DESTINATION_COS_Q_MAP.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              FP_DESTINATION_COS_Q_MAP profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
fp_dest_cos_map_profile_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int index,
    int *cmp)
{
    fp_dest_cos_map_entry_t cos_map_entry;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fp_dest_cos_map_entry_get(unit, index, &cos_map_entry));

    *cmp = sal_memcmp(entries, (void *)&cos_map_entry,
                      sizeof(fp_dest_cos_map_entry_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Register LT FP_DESTINATION_COS_Q_MAP into profile manager.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
fp_dest_cos_map_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, FP_DESTINATION_COS_Q_MAPs,
                                       FP_DESTINATION_COS_Q_MAP_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    profile_hdl = BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP;

    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit, profile_hdl));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    fp_dest_cos_map_profile_hash_cb,
                                    fp_dest_cos_map_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Recover hash and reference count of then entry of PORT_COS_QUEUE_MAP.
 *
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
fp_dest_cos_map_entry_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl;
    fp_dest_cos_map_entry_t cos_map_entry;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index;

    profile_hdl = BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl,
                                         profile_id, &ref_count));
    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fp_dest_cos_map_entry_get(unit, profile_id, &cos_map_entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &cos_map_entry,
                                           1, profile_id));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              1, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Recover LT FP_DESTINATION_COS_Q_MAP.
 *
 * This function is used to recover hash and reference count of the LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
fp_dest_cos_map_recover(int unit)
{
    int rv = SHR_E_NONE;
    int dunit = 0;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t index, value;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(dunit, ent_hdl, BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, OPAQUE_CTRL_IDs, &value));
        if (!(value & OPAQUE_CTRL_ID_COS_MAP_VALID)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, COS_MAP_2_INDEXs, &index));

        SHR_IF_ERR_VERBOSE_EXIT
            (fp_dest_cos_map_entry_recover(unit, (int)index));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the LT PORT_COS_QUEUE_MAP.
 *
 * This function is used to initialize the LT PORT_COS_QUEUE_MAP
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
fp_dest_cos_map_profile_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (fp_dest_cos_map_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, FP_DESTINATION_COS_Q_MAPs));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (fp_dest_cos_map_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the match to handle.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 * \param [out] ent_hdl    Entry handle for DESTINATION_FP_TABLE.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
field_destination_match_to_hdl(
    int unit,
    bcm_field_destination_match_t *match,
    bcmlt_entry_handle_t ent_hdl)
{
    int l2_if;
    uint32_t ipmc_id;
    int device_port_type = 0;
    int cng;
    int etrap_ctrl = 0, etrap_ctrl_mask = 0;
    uint64_t ifp_opaque_ctrl_id;
    uint64_t ifp_opaque_ctrl_id_mask;

    SHR_FUNC_ENTER(unit);

    if (match->gport_mask != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_to_l2_if(unit, match->gport, &l2_if));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATIONs, l2_if));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATION_MASKs,
                                   match->gport_mask));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                   DEST_TYPE_L2_OIF));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALID_MASKs,
                                   DEST_TYPE_MASK));
    } else if (match->mc_group_mask != 0) {
        if (_BCM_MULTICAST_IS_SET(match->mc_group)) {
            ipmc_id = _BCM_MULTICAST_ID_GET(match->mc_group);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATIONs, ipmc_id));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATION_MASKs,
                                       match->mc_group_mask));

            if (_BCM_MULTICAST_IS_L2(match->mc_group)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(ent_hdl,
                                           KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                           DEST_TYPE_L2_MC));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(ent_hdl,
                                           KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                           DEST_TYPE_L3_MC));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl,
                                       KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALID_MASKs,
                                       DEST_TYPE_MASK));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATIONs, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, KEY_DESTINATION_MASKs, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALID_MASKs,
                                   0));
    }

    if (match->port_encap == BCM_PORT_ENCAP_IEEE) {
        device_port_type = DEVICE_PORT_TYPE_FRONT_PANEL;
    } else if (match->port_encap == BCM_PORT_ENCAP_HIGIG3) {
        device_port_type = DEVICE_PORT_TYPE_HIGIG3;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, DEVICE_PORT_TYPEs,
                               device_port_type));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, DEVICE_PORT_TYPE_MASKs,
                               match->port_encap_mask));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(match->color);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, KEY_CNGs, cng));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, KEY_CNG_MASKs, match->color_mask));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, INT_PRIs, match->int_pri));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, INT_PRI_MASKs, match->int_pri_mask));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, IFP_OPAQUE_OBJECTs,
                               match->opaque_object_2));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, IFP_OPAQUE_OBJECT_MASKs,
                               match->opaque_object_2_mask));

    cng = BCMI_LTSW_QOS_COLOR_ENCODING(match->elephant_color);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ETRAP_CNGs, cng));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ETRAP_CNG_MASKs,
                               match->elephant_color_mask));

    etrap_ctrl = 0;
    etrap_ctrl_mask = 0;
    if (match->elephant_mask == 1) {
        etrap_ctrl_mask |= ETRAP_CTRL_ELEPHANT;
    }
    if (match->elephant) {
        etrap_ctrl |= ETRAP_CTRL_ELEPHANT;
    }
    if (match->elephant_notify_mask == 1) {
        etrap_ctrl_mask |= ETRAP_CTRL_NOTIFY;
    }
    if (match->elephant_notify) {
        etrap_ctrl |= ETRAP_CTRL_NOTIFY;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ETRAP_CONTROLs, etrap_ctrl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ETRAP_CONTROL_MASKs, etrap_ctrl_mask));

    ifp_opaque_ctrl_id = match->ifp_opaque_ctrl_id;
    ifp_opaque_ctrl_id_mask = match->ifp_opaque_ctrl_id_mask;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, KEY_IFP_OPAQUE_CTRL_IDs, ifp_opaque_ctrl_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, KEY_IFP_OPAQUE_CTRL_ID_MASKs,ifp_opaque_ctrl_id_mask));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the action to handle.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 * \param [in]  action     Action info.
 * \param [out] ent_hdl    Entry handle for DESTINATION_FP_TABLE.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
field_destination_action_to_hdl(
    int unit,
    bcm_field_destination_match_t *match,
    bcm_field_destination_action_t *action,
    bcmlt_entry_handle_t ent_hdl)
{
    int l2_if;
    uint32_t ipmc_id;
    bcm_field_redirect_destination_type_t dst_type;
    int assign_color = 0, redirect = 0;
    int sbr_idx;
    int rv;
    bcmi_ltsw_profile_hdl_t profile_hdl;
    fp_dest_cos_map_entry_t cos_map_entry;
    int profile_idx = -1;
    fp_dest_cng_redirect_action_t cng_redirect;
    int mirror_index = 0;
    bcmi_ltsw_mirror_info_t info;
    int cng;

    SHR_FUNC_ENTER(unit);

    if (action->flags & BCM_FIELD_DESTINATION_ACTION_COS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, OPAQUE_CTRL_IDs,
                                   OPAQUE_CTRL_ID_COS_MAP_VALID));

        profile_hdl = BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP;
        cos_map_entry.uc_cos  = action->uc_cos;
        cos_map_entry.mc_cos  = action->mc_cos;
        cos_map_entry.rqe_cos = action->rqe_cos;
        cos_map_entry.cpu_cos = action->cpu_cos;
        rv = bcmi_ltsw_profile_index_allocate(unit, profile_hdl,
                                              &cos_map_entry, 0, 1, &profile_idx);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_EXISTS);
        if (rv == SHR_E_NONE) {
            /* A new entry. */
            SHR_IF_ERR_VERBOSE_EXIT
                (fp_dest_cos_map_entry_add(unit, profile_idx, &cos_map_entry));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, COS_MAP_2_INDEXs, profile_idx));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, DROPs, action->drop));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, COPY_TO_CPUs, action->copy_to_cpu));

    if (action->flags & BCM_FIELD_DESTINATION_ACTION_COLOR) {
        cng = BCMI_LTSW_QOS_COLOR_ENCODING(action->color);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, CNGs, cng));
        assign_color = 1;
    }

    if (action->flags & BCM_FIELD_DESTINATION_ACTION_REDIRECT) {
        dst_type = action->destination_type;
        if ((dst_type == bcmFieldRedirectDestinationPort) ||
            (dst_type == bcmFieldRedirectDestinationTrunk)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_to_l2_if(unit, action->gport, &l2_if));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, DESTINATIONs, l2_if));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl,
                                       L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                       DEST_TYPE_L2_OIF));
        } else if ((dst_type == bcmFieldRedirectDestinationMCast) ||
                   (dst_type == bcmFieldRedirectDestinationL3Multicast)) {
            if (!_BCM_MULTICAST_IS_SET(action->mc_group)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            ipmc_id = _BCM_MULTICAST_ID_GET(action->mc_group);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, DESTINATIONs, ipmc_id));

            if (_BCM_MULTICAST_IS_L2(action->mc_group)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(ent_hdl,
                                           L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                           DEST_TYPE_L2_MC));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(ent_hdl,
                                           L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                           DEST_TYPE_L3_MC));
            }
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        redirect = 1;
    }

    cng_redirect = NO_CNG_REDIRECT;
    if (assign_color && redirect) {
        cng_redirect = CNG_REDIRECT;
    } else if (assign_color && !redirect) {
        cng_redirect = CNG_ONLY;
    } else if (!assign_color && redirect) {
        cng_redirect = REDIRECT_ONLY;
    }
    sbr_idx = SBR_INDX(unit, cng_redirect);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, STRENGTH_PROFILE_INDEXs, sbr_idx));

    if (action->flags & BCM_FIELD_DESTINATION_ACTION_MIRROR) {
        sal_memset((void *)&info, 0, sizeof(bcmi_ltsw_mirror_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_field_destination_add(unit,
                                                    match,
                                                    action->mirror_dest_id,
                                                    &info));
       mirror_index = info.mirror_index;
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(ent_hdl, MIRROR_INDEXs, mirror_index));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (profile_idx != -1) {
            (void)bcmi_ltsw_profile_index_free(unit, profile_hdl, profile_idx);
        }

        if (mirror_index != -0) {
            (void)bcmi_ltsw_mirror_field_destination_delete(unit, match);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the handle to match.
 *
 * \param [in]  unit       Unit number.
 * \param [out] ent_hdl    Entry handle for DESTINATION_FP_TABLE.
 * \param [in]  match      Match info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
field_destination_hdl_to_match(
    int unit,
    bcmlt_entry_handle_t ent_hdl,
    bcm_field_destination_match_t *match)
{
    int device_port_type = 0;
    uint64_t value, dest;
    uint64_t etrap_ctrl = 0, etrap_ctrl_mask = 0;
    uint64_t ifp_opaque_ctrl_id, ifp_opaque_ctrl_id_mask;

    SHR_FUNC_ENTER(unit);

    bcm_field_destination_match_t_init(match);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KEY_DESTINATION_MASKs, &value));
    if (value != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl,
                                   KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                                   &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, KEY_DESTINATIONs, &dest));
        if (value == DEST_TYPE_L2_OIF) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_if_to_port(unit, (int)dest, &match->gport));
            match->gport_mask = (uint32_t)value;
        } else if (value == DEST_TYPE_L2_MC) {
            _BCM_MULTICAST_GROUP_SET(match->mc_group,
                                     _BCM_MULTICAST_TYPE_L2,
                                     (int)dest);
            match->mc_group_mask = (uint32_t)value;
        } else if (value == DEST_TYPE_L3_MC) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_multicast_l3_grp_id_get(unit,
                                                   (uint32_t)dest,
                                                   &match->mc_group));
            match->mc_group_mask = (uint32_t)value;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, DEVICE_PORT_TYPE_MASKs, &value));
    match->port_encap_mask = (uint32_t)value;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, DEVICE_PORT_TYPEs, &value));
    if (value == DEVICE_PORT_TYPE_FRONT_PANEL) {
        match->port_encap = BCM_PORT_ENCAP_IEEE;
    } else if (device_port_type == DEVICE_PORT_TYPE_HIGIG3) {
        match->port_encap = BCM_PORT_ENCAP_HIGIG3;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KEY_CNG_MASKs, &value));
    match->color_mask = (uint32_t)value;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KEY_CNGs, &value));
    match->color = BCMI_LTSW_QOS_COLOR_DECODING(value);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, INT_PRI_MASKs, &value));
    match->int_pri_mask = (uint32_t)value;
        (bcmlt_entry_field_get(ent_hdl, INT_PRIs, &value));
    match->int_pri = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, IFP_OPAQUE_OBJECT_MASKs, &value));
    match->opaque_object_2_mask = (uint32_t)value;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, IFP_OPAQUE_OBJECTs, &value));
    match->opaque_object_2 = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, ETRAP_CNG_MASKs, &value));
    match->elephant_color_mask = (uint8_t)value;
    if (match->elephant_color_mask != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, ETRAP_CNGs, &value));
        match->elephant_color = BCMI_LTSW_QOS_COLOR_DECODING(value);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, ETRAP_CONTROLs, &value));
    etrap_ctrl = value;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, ETRAP_CONTROL_MASKs, &value));
    etrap_ctrl_mask = value;
    if (etrap_ctrl & ETRAP_CTRL_ELEPHANT) {
        match->elephant = 1;
    }
    if (etrap_ctrl_mask & ETRAP_CTRL_ELEPHANT) {
        match->elephant_mask = 1;
    }
    if (etrap_ctrl & ETRAP_CTRL_NOTIFY) {
        match->elephant_notify = 1;
    }
    if (etrap_ctrl_mask & ETRAP_CTRL_NOTIFY) {
        match->elephant_notify_mask = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KEY_IFP_OPAQUE_CTRL_IDs, &ifp_opaque_ctrl_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KEY_IFP_OPAQUE_CTRL_ID_MASKs, &ifp_opaque_ctrl_id_mask));
    match->ifp_opaque_ctrl_id = (uint32_t)ifp_opaque_ctrl_id;
    match->ifp_opaque_ctrl_id_mask = (uint32_t)ifp_opaque_ctrl_id_mask;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the action to handle.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 * \param [out] ent_hdl    Entry handle for DESTINATION_FP_TABLE.
 * \param [in]  action     Action info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
field_destination_hdl_to_action(
    int unit,
    bcm_field_destination_match_t *match,
    bcmlt_entry_handle_t ent_hdl,
    bcm_field_destination_action_t *action)
{
    uint64_t   value;
    uint64_t   dest;
    int i;
    fp_dest_cos_map_entry_t  cos_map_entry;

    SHR_FUNC_ENTER(unit);

    bcm_field_destination_action_t_init(action);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, DROPs, &value));
    action->drop = (uint8_t)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, COPY_TO_CPUs, &value));
    action->copy_to_cpu = (uint8_t)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, OPAQUE_CTRL_IDs, &value));
    if (value & OPAQUE_CTRL_ID_COS_MAP_VALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, COS_MAP_2_INDEXs, &value));

        SHR_IF_ERR_VERBOSE_EXIT
            (fp_dest_cos_map_entry_get(unit, (int)value, &cos_map_entry));
        action->uc_cos   = cos_map_entry.uc_cos;
        action->mc_cos   = cos_map_entry.mc_cos;
        action->rqe_cos  = cos_map_entry.rqe_cos;
        action->cpu_cos  = cos_map_entry.cpu_cos;
        action->flags |= BCM_FIELD_DESTINATION_ACTION_COS;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, STRENGTH_PROFILE_INDEXs, &value));
    for (i = 0; i < CNG_REDIRECT_ACTION_COUNT; i++) {
        if (SBR_INDX(unit, i) == (int)value) {
            break;
        }
    }
    if (i == CNG_REDIRECT_ACTION_COUNT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if ((i == CNG_ONLY) || (i == CNG_REDIRECT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, CNGs, &value));
        action->color = BCMI_LTSW_QOS_COLOR_DECODING(value);
        action->flags |= BCM_FIELD_DESTINATION_ACTION_COLOR;
    }

    if ((i == REDIRECT_ONLY) || (i == CNG_REDIRECT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl,
                                   L2MC_L3MC_L2_OIF_SYS_DST_VALIDs, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, DESTINATIONs, &dest));
        if (value == DEST_TYPE_L2_OIF) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_if_to_port(unit, (int)dest, &action->gport));
            if (BCM_GPORT_IS_TRUNK(action->gport)) {
                action->destination_type = bcmFieldRedirectDestinationTrunk;
            } else {
                action->destination_type = bcmFieldRedirectDestinationPort;
            }
        } else if (value == DEST_TYPE_L2_MC) {
            _BCM_MULTICAST_GROUP_SET(action->mc_group,
                                     _BCM_MULTICAST_TYPE_L2,
                                     (int)dest);
            action->destination_type = bcmFieldRedirectDestinationMCast;
        } else if (value == DEST_TYPE_L3_MC) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_multicast_l3_grp_id_get(unit,
                                                   (uint32_t)dest,
                                                   &action->mc_group));
            action->destination_type = bcmFieldRedirectDestinationL3Multicast;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        action->flags |= BCM_FIELD_DESTINATION_ACTION_REDIRECT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, MIRROR_INDEXs, &value));
    if (value != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_field_destination_get(unit, match,
                                                    &action->mirror_dest_id));
        action->flags |= BCM_FIELD_DESTINATION_ACTION_MIRROR;
    }
exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief Traverse DESTINATION_FP_TABLE and call the callback.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  cb_err_abort  Abort when callback returns error.
 * \param [in]  callback      callback.
 * \param [in]  user_data     User data.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
field_destination_entry_traverse(
    int unit,
    int cb_err_abort,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv = SHR_E_NONE;
    bcm_field_destination_match_t   match;
    bcm_field_destination_action_t  action;
    uint64_t  value;
    int trav_cnt = 0, cb_cnt = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, ENTRY_PRIORITYs, &value));
        if (value < MIN_PRI(unit) + ENTRY_PRIORITY_BASE) {
            continue;
        }

        if (value > MAX_PRI(unit) + ENTRY_PRIORITY_BASE) {
            continue;
        }

        trav_cnt++;
        if (trav_cnt > 1) {
            /*
             * Not the first entry.
             * Call the callback with the previous match/action.
             * Then convert the current handle to match/action.
             */
            cb_cnt++;
            rv = callback(unit, &match, &action, user_data);
            if (SHR_FAILURE(rv) && cb_err_abort) {
                SHR_ERR_EXIT(rv);
            }
        }

        bcm_field_destination_match_t_init(&match);
        bcm_field_destination_action_t_init(&action);

        SHR_IF_ERR_VERBOSE_EXIT
            (field_destination_hdl_to_match(unit, ent_hdl, &match));
        match.priority = (int)value - ENTRY_PRIORITY_BASE;

        SHR_IF_ERR_VERBOSE_EXIT
            (field_destination_hdl_to_action(unit, &match,
                                             ent_hdl, &action));
    }

    if (cb_cnt != trav_cnt) {
        rv = callback(unit, &match, &action, user_data);
        if (SHR_FAILURE(rv) && cb_err_abort) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
* \brief Callback for Clearing all entries of DESTINATION_FP_TABLE LT.
*
* \param [in]  unit        Unit Number.
* \param [in]  match       Match info.
* \param [in]  action      Action info.
* \param [in]  User_data   User data.
*
* \retval SHR_E_NONE     No errors.
* \retval !SHR_E_NONE    Failure.
*/
static int
field_destination_trav_cb(
    int unit,
    bcm_field_destination_match_t *match,
    bcm_field_destination_action_t *action,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

     /* Delete the flex counter. */
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmint_ltsw_field_destination_stat_detach(unit, match));

     /* Delete entry. */
     SHR_IF_ERR_VERBOSE_EXIT
         (xfs_ltsw_field_destination_entry_delete(unit, match));

exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief Set a default entry of DESTINATION_FP_TABLE LT.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  priority    Entry priority.
 * \param [in]  cleanup     clean up.
 * \param [in]  match_port  Match port.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
destination_fp_table_default_set(
    int unit,
    int priority,
    int cleanup,
    bcm_gport_t match_port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_DESTINATION_FP;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    int strength;
    uint64_t key_dest_msk = 0, key_dst_valid = 0, key_dst_valid_msk = 0;
    int l2_if = 0;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &entry_hdl));
    /* Keys. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_CNGs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_CNG_MASKs, 0));
    if (match_port == BCM_GPORT_BLACK_HOLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_to_l2_if(unit, match_port, &l2_if));
        key_dest_msk = 0xffff;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DROPs, 1));
        key_dst_valid     = DEST_TYPE_L2_OIF;
        key_dst_valid_msk = DEST_TYPE_MASK;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_IFP_OPAQUE_CTRL_IDs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_IFP_OPAQUE_CTRL_ID_MASKs,0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_DESTINATIONs, l2_if));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, KEY_DESTINATION_MASKs, key_dest_msk));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEVICE_PORT_TYPEs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, DEVICE_PORT_TYPE_MASKs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
                               key_dst_valid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               KEY_L2MC_L3MC_L2_OIF_SYS_DST_VALID_MASKs,
                               key_dst_valid_msk));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IFP_OPAQUE_OBJECTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IFP_OPAQUE_OBJECT_MASKs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ETRAP_CONTROLs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ETRAP_CONTROL_MASKs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ETRAP_CNGs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ETRAP_CNG_MASKs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRIs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, INT_PRI_MASKs, 0));

    if (cleanup) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL),
                                  SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ENTRY_PRIORITYs, priority));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &strength));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, STRENGTH_PROFILE_INDEXs, strength));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT( );
}

/*!
* \brief Clear all entries of DESTINATION_FP_TABLE LT.
*
* \param [in]  unit        Unit Number.
*
* \retval SHR_E_NONE     No errors.
* \retval !SHR_E_NONE    Failure.
*/
static int
field_destination_lt_clear(int unit) {
    int warmboot = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (warmboot) {
        SHR_EXIT();
    }

     /* Delete Dummy entry. */
     SHR_IF_ERR_VERBOSE_EXIT
         (destination_fp_table_default_set(unit, 0, 1,
                                           BCM_GPORT_INVALID));

     /* Delete entry for blackhole. */
     SHR_IF_ERR_VERBOSE_EXIT
         (destination_fp_table_default_set(unit, 0, 1,
                                           BCM_GPORT_BLACK_HOLE));

    /* Delete entries programmed via destination field APIs. */
    SHR_IF_ERR_VERBOSE_EXIT
       (field_destination_entry_traverse(unit, 0,
                                         field_destination_trav_cb, NULL));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
xfs_ltsw_field_destination_deinit(int unit) {
    bcmi_ltsw_profile_hdl_t profile_hdl;
    int i;
    xfs_field_dest_info_t *di = &dfp_info[unit];

    SHR_FUNC_ENTER(unit);

    if (di->initialized == FALSE) {
        SHR_EXIT();
    }

    profile_hdl = BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    for (i = 0; i < CNG_REDIRECT_ACTION_COUNT; i++) {
        SBR_INDX(unit, i) = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_lt_clear(unit));

    di->initialized = FALSE;
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_init(int unit)
{
    int i;
    int sbr_idx;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_DESTINATION_FP;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
    int cold_boot = !bcmi_warmboot_get(unit);
    uint64_t min, max;
    int priority;
    xfs_field_dest_info_t *di = &dfp_info[unit];

    SHR_FUNC_ENTER(unit);

    if (di->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_field_destination_deinit(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fp_dest_cos_map_profile_init(unit));

    for (i = 0; i < CNG_REDIRECT_ACTION_COUNT; i++) {
        if (i == NO_CNG_REDIRECT) {
            ent_type = BCMI_LTSW_SBR_PET_NONE;
        } else if (i == CNG_ONLY) {
            ent_type =
            BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_IFP_OPAQUE_CTRL_ID_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID;
        } else if (i == REDIRECT_ONLY) {
            ent_type = BCMI_LTSW_SBR_PET_NO_CNG_NO_IFP_OPAQUE_CTRL_ID;
        } else {
            ent_type = BCMI_LTSW_SBR_PET_DEF;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &sbr_idx));
        SBR_INDX(unit, i) = sbr_idx;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, DESTINATION_FP_TABLEs,
                                       ENTRY_PRIORITYs, &min, &max));
    MAX_PRI(unit) = max - ENTRY_NUM_RSVD;
    MIN_PRI(unit) = min;

    if (cold_boot) {
        /* Priority ENTRY_PRIORITY_LOWEST only used in dummy entry. */
        priority = ENTRY_PRIORITY_LOWEST;
        SHR_IF_ERR_VERBOSE_EXIT
            (destination_fp_table_default_set(unit, priority, 0,
                                              BCM_GPORT_INVALID));

        /* Priority ENTRY_PRIORITY_HIGHEST only used for blackhole. */
        priority = MAX_PRI(unit) + ENTRY_NUM_RSVD;
        SHR_IF_ERR_VERBOSE_EXIT
            (destination_fp_table_default_set(unit, priority, 0,
                                              BCM_GPORT_BLACK_HOLE));
    }

    di->initialized = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t  *match,
    int *mirror_index)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    if (mirror_index == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, MIRROR_INDEXs, &value));
    *mirror_index = (int)value;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_entry_add(
    int unit,
    uint32   options,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bcmlt_opcode_t opcode;

    SHR_FUNC_ENTER(unit);

    /*
     * Entry priority 0 is reserved for the default dump entry.
     * The priority programmed into LT will be
     * match->priority + ENTRY_PRIORITY_BASE.
     */
    if (match->priority < MIN_PRI(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (match->color_mask != 0) {
        if ((match->color != bcmColorGreen) &&
            (match->color != bcmColorYellow) &&
            (match->color != bcmColorRed)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (match->elephant_color_mask != 0) {
        if ((match->elephant_color != bcmColorGreen) &&
            (match->elephant_color != bcmColorYellow) &&
            (match->elephant_color != bcmColorRed)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (action->flags & BCM_FIELD_DESTINATION_ACTION_COLOR) {
        if ((action->color != bcmColorGreen) &&
            (action->color != bcmColorYellow) &&
            (action->color != bcmColorRed)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (match->priority > MAX_PRI(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ENTRY_PRIORITYs,
                               match->priority + ENTRY_PRIORITY_BASE));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, VIEW_Ts, FLEX_CTRs));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    if (options & BCM_FIELD_DESTINATION_OPTIONS_REPLACE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, ent_hdl,
                                  BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_action_to_hdl(unit, match, action, ent_hdl));

    if (options & BCM_FIELD_DESTINATION_OPTIONS_REPLACE) {
        opcode = BCMLT_OPCODE_UPDATE;
    } else {
        opcode = BCMLT_OPCODE_INSERT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl, opcode, BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_entry_get(
    int unit,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, ENTRY_PRIORITYs, &value));
    match->priority = (int)value - ENTRY_PRIORITY_BASE;

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_hdl_to_action(unit, match, ent_hdl, action));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_entry_traverse(
    int unit,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data)
{
    int cb_err_abort = bcmi_ltsw_dev_cb_err_abort(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_entry_traverse(unit, cb_err_abort,
                                          callback, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_field_destination_entry_delete(
    int unit,
    bcm_field_destination_match_t  *match)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bcmi_ltsw_profile_hdl_t profile_hdl;
    int profile_idx = -1;
    uint64_t  value;
    int rv;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, OPAQUE_CTRL_IDs, &value));
    if (value & OPAQUE_CTRL_ID_COS_MAP_VALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, COS_MAP_2_INDEXs, &value));
        profile_idx = (int)value;
        profile_hdl = BCMI_LTSW_PROFILE_FP_DESTINATION_COS_Q_MAP;
        rv = bcmi_ltsw_profile_index_free(unit, profile_hdl, profile_idx);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fp_dest_cos_map_entry_delete(unit, profile_idx));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, MIRROR_INDEXs, &value));
    if (value != 0) {
        (void)bcmi_ltsw_mirror_field_destination_delete(unit, match);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_field_destination_flexctr_attach(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t flex_ctr_action;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, FLEX_CTR_ACTIONs, &flex_ctr_action));

    if (info->direction != BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Already attached a flex counter action. */
    if (flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    flex_ctr_action = (uint64_t)info->action_index;
    /* Attach flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FLEX_CTR_ACTIONs, flex_ctr_action));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_field_destination_flexctr_info_get(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t flex_ctr_action;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, FLEX_CTR_ACTIONs, &flex_ctr_action));

    info->action_index = flex_ctr_action;
    info->direction    = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    info->stage        = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    info->table_name   = DESTINATION_FP_TABLEs;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_field_destination_flexctr_detach(
    int unit,
    bcm_field_destination_match_t *match)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FLEX_CTR_ACTIONs,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_field_destination_flexctr_object_set(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t value)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, FLEX_CTR_INDEXs, value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_field_destination_flexctr_object_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *value)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value_64;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DESTINATION_FP_TABLEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (field_destination_match_to_hdl(unit, match, ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, FLEX_CTR_INDEXs, &value_64));

    *value = (uint32_t)value_64;
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}
