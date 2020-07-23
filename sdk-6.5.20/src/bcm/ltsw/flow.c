/*! \file flow.c
 *
 * Flex flow management.
 * This file contains the management for flex flow.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/flow.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/flow_int.h>
#include <bcm_int/ltsw/generated/flow_ha.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/mbcm/flow.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/vlan.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_types.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FLOW

/*!
 * \brief Defines Software bookkeeping for flow related information.
 */
typedef struct ltsw_flow_bookkeeping_s {

    /* Data structure initialized or not. */
    uint8_t initialized;

    /* Protection mutex. */
    sal_mutex_t flow_mutex;

    /* Tunnel ID mapping for DVP. */
    uint32_t *tnl_initiator_id_map;

    /* Match info for VP. */
    bcmint_flow_vp_match_t *vp_match;

} ltsw_flow_bookkeeping_t;

/*!
 * \Flow book keeping information.
 */
static ltsw_flow_bookkeeping_t  ltsw_flow_bk_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
* Private functions
 */

/*!
 * \brief Check if flow feature in enabled.
 *
 * This function is used to check if flow feature is eanbled.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
flow_feature_check(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check if flow module is initialized.
 *
 * This function is used to check if flow module is initialized.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
flow_init_check(int unit)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];

    SHR_FUNC_ENTER(unit);

    if ((flow_info == NULL) || (flow_info->initialized == FALSE)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Take flow lock sempahore.
 *
 * This function is used to take flow lock sempahore.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
flow_lock(int unit)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    sal_mutex_take(flow_info->flow_mutex, SAL_MUTEX_FOREVER);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Release flow lock sempahore.
 *
 * This function is used to release flow lock sempahore.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval NONE.
 */
static void
flow_unlock(int unit)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];

    sal_mutex_give(flow_info->flow_mutex);
}

/*!
 * \brief Initialize the flow information structures.
 *
 * This function is used to initialize the flow information structures.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
flow_info_init(int unit)
{
   ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
   uint32_t table_num;
   uint32_t ha_alloc_size = 0, ha_req_size = 0;
   uint32_t ha_instance_size = 0, ha_array_size = 0;
   int rv = 0;
   int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    /* Create FLOW protection mutex. */
    if (flow_info->flow_mutex == NULL) {
        flow_info->flow_mutex = sal_mutex_create("flowMutex");
        SHR_NULL_CHECK(flow_info->flow_mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_num_get(unit, &table_num));
    ha_req_size = table_num * sizeof(uint32_t);
    ha_alloc_size = ha_req_size;
    flow_info->tnl_initiator_id_map = bcmi_ltsw_ha_mem_alloc(unit,
                                          BCMI_HA_COMP_ID_FLOW,
                                          bcmintFlowHaTnlInitIdMap,
                                          "bcmFlowTnlInitIdMap",
                                          &ha_alloc_size);
    SHR_NULL_CHECK(flow_info->tnl_initiator_id_map, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(flow_info->tnl_initiator_id_map, 0, ha_alloc_size);
    }

    ha_array_size = table_num;
    ha_instance_size = sizeof(bcmint_flow_vp_match_t);
    ha_req_size = ha_instance_size * ha_array_size;
    ha_alloc_size = ha_req_size;
    flow_info->vp_match = bcmi_ltsw_ha_mem_alloc(unit,
                              BCMI_HA_COMP_ID_FLOW, bcmintFlowHaVpMatch,
                              "bcmFlowVpMatch",
                              &ha_alloc_size);
    SHR_NULL_CHECK(flow_info->vp_match, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(flow_info->vp_match, 0, ha_alloc_size);
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                 BCMI_HA_COMP_ID_FLOW,
                                 bcmintFlowHaVpMatch,
                                 0, ha_instance_size, ha_array_size,
                                 BCMINT_FLOW_VP_MATCH_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm && (flow_info->tnl_initiator_id_map)) {
            (void)bcmi_ltsw_ha_mem_free(unit, flow_info->tnl_initiator_id_map);
        }

        if (!warm && (flow_info->vp_match)) {
            (void)bcmi_ltsw_ha_mem_free(unit, flow_info->vp_match);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all flow port fomr a VPN.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vpn       VPN ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
flow_port_delete_all(int unit, bcm_vpn_t vpn)
{
    uint32_t vp_num = 0;
    uint32_t vp_idx = 0;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    bcm_gport_t flow_port_id;

    SHR_FUNC_ENTER(unit);

    if (vpn != BCM_FLOW_VPN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, vpn));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_num_get(unit, &vp_num));

     for (vp_idx = 0; vp_idx < vp_num; vp_idx++) {
         bcmi_ltsw_virtual_vp_info_init(&vp_info);
         if (bcmi_ltsw_virtual_vp_info_get(unit, vp_idx, &vp_info)) {
             continue;
         }
         if (!(vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW)) {
             continue;
         }
         if (vp_info.vpn != vpn) {
             continue;
         }

         BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, vp_idx);
         SHR_IF_ERR_VERBOSE_EXIT
             (bcm_ltsw_flow_port_destroy(unit, vpn, flow_port_id));
     }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all flow ports related to VLAN.
 *
 * \param [in]   unit      Unit number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
flow_port_vlan_delete_all(int unit)
{
    bcm_vpn_t vlan;
    int exist = 0;

    SHR_FUNC_ENTER(unit);

    for (vlan = BCM_VLAN_MIN + 1; vlan <= BCM_VLAN_MAX; vlan++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_exist_check(unit, vlan, &exist));

        if (exist) {
            SHR_IF_ERR_VERBOSE_EXIT
                (flow_port_delete_all(unit, vlan));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the match info for a flow port.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   info      Match info for the flow port.
 *
 * \retval NONE.
 */
static void
flow_port_match_info_set(int unit, bcm_flow_match_config_t *info)
{
    int vp = 0;
    bcmint_flow_vp_match_t *match;

    if (!(info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID)) {
        return;
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    match                 = &(ltsw_flow_bk_info[unit].vp_match[vp]);
    match->valid          = 1;
    match->criteria       = info->criteria;
    match->sip            = info->sip;
    match->valid_elements = info->valid_elements;
    match->port           = info->port;
    match->vlan           = info->vlan;
    match->inner_vlan     = info->inner_vlan;
    sal_memcpy(match->sip6, info->sip6, sizeof(bcm_ip6_t));

    return;
}

/*!
 * \brief Reset the match info for a flow port.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   info      Match info for the flow port.
 *
 * \retval NONE.
 */
static void
flow_port_match_info_reset(int unit, bcm_flow_match_config_t *info)
{
    int vp = 0;
    uint32_t vp_num = 0;
    int found = 0;
    bcmint_flow_vp_match_t *match;

    if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        match = &(ltsw_flow_bk_info[unit].vp_match[vp]);
        sal_memset(match, 0, sizeof(bcmint_flow_vp_match_t));
        return;
    }

    (void)bcmi_ltsw_virtual_vp_num_get(unit, &vp_num);

    for (vp = 0; vp < vp_num; vp++) {
        match = &(ltsw_flow_bk_info[unit].vp_match[vp]);
        if (match->valid == 0) {
            continue;
        }

        if (match->criteria != info->criteria) {
            continue;
        }

        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
            if (info->valid_elements & BCM_FLOW_MATCH_SIP_VALID) {
                if (info->sip != match->sip) {
                    continue;
                }
                found = 1;
                break;
            } else {
                if (sal_memcmp(info->sip6, match->sip6, sizeof(bcm_ip6_t))) {
                    continue;
                }
                found = 1;
                break;
            }
        }
        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN) {
            if (info->port == match->port &&
                info->vlan == match->vlan) {
                found = 1;
                break;
            }
        }
        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_INNER_VLAN) {
            if (info->port == match->port &&
                info->inner_vlan == match->inner_vlan) {
                found = 1;
                break;
            }
        }
        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN_STACKED) {
            if (info->port == match->port &&
                info->vlan == match->vlan &&
                info->inner_vlan == match->inner_vlan) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        return;
    }

    sal_memset(match, 0, sizeof(bcmint_flow_vp_match_t));
    return;
}

/*!
 * \brief Retrive the match config of a flow port.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   flow_port_id     Flow port ID.
 * \param [out]  info     Flow port match config info.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
flow_port_match_info_get(int unit,
                         bcm_gport_t  flow_port_id,
                         bcm_flow_match_config_t *info)
{
    bcmint_flow_vp_match_t *match = NULL;
    int vp = 0;

    SHR_FUNC_ENTER(unit);

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    match = &(ltsw_flow_bk_info[unit].vp_match[vp]);
    if (!match->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    info->criteria       = match->criteria;
    info->sip            = match->sip;
    info->valid_elements = match->valid_elements;
    info->port           = match->port;
    info->vlan           = match->vlan;
    info->inner_vlan     = match->inner_vlan;
    sal_memcpy(info->sip6, match->sip6, sizeof(bcm_ip6_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Notify the other modules egress if of a flow port is changed.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   info      Flow port encap info.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
flow_port_encap_egress_if_notify(int unit, bcm_flow_port_encap_t *info)
{
    int vp_lag_vp = 0;
    int rv;
    bcmi_ltsw_l2_virtual_info_t v_info;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    int vp = 0;

    SHR_FUNC_ENTER(unit);

    /* Notify TRUNK module about the update. */
    rv = bcmi_ltsw_flow_port_source_vp_lag_get(unit, info->flow_port,
                                               &vp_lag_vp);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_vp_lag_member_update(unit,
                                                  vp_lag_vp,
                                                  info->flow_port));
    }

    /* Notify L2 module about the update. */
    vp = BCM_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info));
    if (!(vp_info.flags & BCMI_LTSW_VIRTUAL_VP_CASCADED)) {
        sal_memset(&v_info, 0, sizeof(bcmi_ltsw_l2_virtual_info_t));
        v_info.egress_if = info->egress_if;
        v_info.gport     = info->flow_port;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_virtual_info_update(unit, &v_info));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
flow_match_assigned_to_flow_port(int unit, bcm_flow_match_config_t *info)
{
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_INNER_VLAN ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN_STACKED) {
        return TRUE;
    }

    return FALSE;
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize flow module.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcm_ltsw_flow_init(int unit)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    if (flow_info->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flow_cleanup(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_info_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_init(unit));

    flow_info->initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        bcm_ltsw_flow_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flow module.
 *
 * \param [in]  unit               Unit number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
bcm_ltsw_flow_cleanup(int unit)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    if (flow_info->initialized == FALSE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = 1;

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flow_vpn_destroy_all(unit));

        if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW_VLAN_DOMAIN)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (flow_port_vlan_delete_all(unit));
        }
    }

    SHR_IF_ERR_CONT
        (mbcm_ltsw_flow_detach(unit));

    flow_info->initialized = FALSE;

    flow_unlock(unit);

    locked = 0;

    /* Destroy protection mutex. */
    if (flow_info->flow_mutex) {
        sal_mutex_destroy(flow_info->flow_mutex);
        flow_info->flow_mutex = NULL;
    }

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the handle for the flow name.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   flow_name    Flow name.
 * \param [out]  handle       Flow handle.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_handle_get(int unit, const char *flow_name,
                         bcm_flow_handle_t *handle)
{
    bcmint_flow_handle_rec_t *handle_rec_entry = NULL;
    uint32_t rec_num, rec_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_name, SHR_E_PARAM);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_HANDLE,
                                         (uint8_t **)&handle_rec_entry,
                                         &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        if (sal_strncmp(flow_name, handle_rec_entry[rec_idx].flow_name,
                        sal_strlen(handle_rec_entry[rec_idx].flow_name))== 0) {
            *handle = handle_rec_entry[rec_idx].flow_handle;
            SHR_EXIT();
        }
    }

    /* Must be not found */
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get the option id for flow option name.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   handle            Flow handle.
 * \param [in]   flow_option_name  Flow option name.
 * \param [out]  option_id         Flow option ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_option_id_get(int unit, bcm_flow_handle_t flow_handle,
                            const char *flow_option_name,
                            bcm_flow_option_id_t *option_id)
{
    bcmint_flow_op_map_rec_t *op_rec_entry = NULL;
    uint32_t rec_num, rec_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_option_name, SHR_E_PARAM);
    SHR_NULL_CHECK(option_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_OP,
                                         (uint8_t **)&op_rec_entry, &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        if (sal_strcmp(flow_option_name, op_rec_entry[rec_idx].op_name) == 0) {
            *option_id = op_rec_entry[rec_idx].op_id;
            SHR_EXIT();
        }
    }

    /* Must be not found */
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
   SHR_FUNC_EXIT();
}

int
bcmint_flow_lt_get(int unit,
                   bcmint_flow_lt_id_t lt_id,
                   const bcmint_flow_lt_t **lt_info)
{
    bcmint_flow_lt_db_t lt_db;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lt_db, 0, sizeof(lt_db));
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_flow_lt_db_get(unit, &lt_db), SHR_E_UNAVAIL);

    if (!(lt_db.lt_bmp & (1 << lt_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *lt_info = &lt_db.lts[lt_id];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump flow handle info.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_handle_dump(int unit)
{
    bcmint_flow_handle_rec_t *handle_rec_entry = NULL;
    uint32_t rec_num, rec_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    cli_out("\n    FLOW HANDLE \n");
    cli_out("=========================================="\
            "======================================\n");
    cli_out("%-48s  %-10s \n", "FLOW_NAME", "FLOW_HANDLE");
    cli_out("========================================"\
            "========================================\n");

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_HANDLE,
                                         (uint8_t **)&handle_rec_entry,
                                         &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        cli_out("%-48s  %-10d \n",
                handle_rec_entry[rec_idx].flow_name,
                handle_rec_entry[rec_idx].flow_handle);
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Dump flow option info.
 *
 * \param [in]   unit              Unit number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_option_dump(int unit)
{
    bcmint_flow_op_map_rec_t *op_rec_entry = NULL;
    uint32_t rec_num, rec_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_OP,
                                         (uint8_t **)&op_rec_entry, &rec_num));

    cli_out("\n    FLOW OPTION  \n");
    cli_out("========================================="\
            "===================================================\n");
    cli_out("%-64s %-10s \n","OPTION_NAME", "OPTION_ID");
    cli_out("========================================="\
            "===================================================\n");

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        cli_out("%-64s %-10d \n",
                op_rec_entry[rec_idx].op_name,
                op_rec_entry[rec_idx].op_id);

    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field ID for logical field name.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   flow_handle       Flow handle.
 * \param [in]   field_name        Logical field name.
 * \param [out]  field_id          Logical field ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_logical_field_id_get(int unit, bcm_flow_handle_t flow_handle,
                                   const char *field_name,
                                   bcm_flow_field_id_t *field_id)
{
    uint32_t rec_idx, rec_num;
    bcmint_flow_lf_map_rec_t *lf_map_entry = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(field_name, SHR_E_PARAM);
    SHR_NULL_CHECK(field_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_LOGICAL_FIELD,
                                         (uint8_t **)&lf_map_entry, &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        /* Find logical field ID based on name. */
        if ((sal_strncmp(field_name, lf_map_entry[rec_idx].lf_name,
                         sal_strlen(lf_map_entry[rec_idx].lf_name))== 0)) {
            *field_id = lf_map_entry[rec_idx].lf_id;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field name for logical field ID.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   flow_handle       Flow handle.
 * \param [in]   field_id          Logical field ID.
 * \param [out]  field_name        Logical field name.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_logical_field_name_get(int unit, bcm_flow_handle_t flow_handle,
                                      uint32_t field_id,
                                      char *field_name)
{
    uint32_t rec_idx, rec_num;
    bcmint_flow_lf_map_rec_t *lf_map_entry = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(field_name, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_LOGICAL_FIELD,
                                         (uint8_t **)&lf_map_entry, &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        /* Find logical field name based on ID. */
        if (lf_map_entry[rec_idx].lf_id == field_id) {
            /* Return new field if different field used. */
            if (lf_map_entry[rec_idx].new_lf_name != NULL) {
                sal_strncpy(field_name, lf_map_entry[rec_idx].new_lf_name,
                            sal_strlen(lf_map_entry[rec_idx].new_lf_name));
            } else {
                sal_strncpy(field_name, lf_map_entry[rec_idx].lf_name,
                            sal_strlen(lf_map_entry[rec_idx].lf_name));
            }
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the logical table name for flow option name.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   flow_handle       Flow handle.
 * \param [in]   option_id         Flow option ID.
 * \param [out]  table_name        Logical table name.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_option_logical_table_name_get(
    int unit,
    bcm_flow_handle_t flow_handle,
    bcm_flow_option_id_t option_id,
    char *table_name)
{
    uint32_t rec_idx, rec_num;
    bcmint_flow_op_lt_map_rec_t *op_lt_map_entry = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_mapping_info_get(unit, INFO_SEL_OP_LOGICAL_TABLE,
                                         (uint8_t **)&op_lt_map_entry,
                                         &rec_num));

    for (rec_idx = 0; rec_idx < rec_num; rec_idx++) {
        if (op_lt_map_entry[rec_idx].op_id == option_id) {
            sal_strncpy(table_name, op_lt_map_entry[rec_idx].lt_name,
                        sal_strlen(op_lt_map_entry[rec_idx].lt_name));
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an L2/L3 VPN.
 *
 * \param [in]       unit      Unit number.
 * \param [in-out]   vpn       VPN ID.
 * \param [out]      info      VPN structure.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_vpn_create(int unit, bcm_vpn_t *vpn, bcm_flow_vpn_config_t *info)
{
    int vfi_index = 0;
    uint8_t vpn_alloc_flag = 0;
    int local_rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(vpn, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    if (info->flags & BCM_FLOW_VPN_ELINE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Allocate VFI */
    if (info->flags & BCM_FLOW_VPN_REPLACE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, *vpn));
    } else if (info->flags & BCM_FLOW_VPN_WITH_ID) {
        local_rv = bcmint_flow_vpn_is_valid(unit, *vpn);
        if (local_rv == SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else if (local_rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(local_rv);
        }
        BCMI_LTSW_VIRTUAL_VPN_GET(vfi_index, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                  *vpn);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_alloc_with_id(unit, BCMI_LTSW_VFI_TYPE_FLOW,
                                                 vfi_index));
        vpn_alloc_flag = 1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_alloc(unit, BCMI_LTSW_VFI_TYPE_FLOW,
                                         &vfi_index));
        BCMI_LTSW_VIRTUAL_VPN_SET(*vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                  vfi_index);
        vpn_alloc_flag = 1;
    }

    /* Update HW VFI table */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vpn_hw_opt(unit, HW_OPT_SET, *vpn, info));

exit:
    if (SHR_FUNC_ERR() && vpn_alloc_flag) {
        (void)bcmi_ltsw_virtual_vfi_free(unit, BCMI_LTSW_VFI_TYPE_FLOW,
                                         vfi_index);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VPN.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  vpn       VPN ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_vpn_destroy(int unit, bcm_vpn_t vpn)
{
    uint32_t vfi_index = 0;
    uint32_t vfi_start = 0, vfi_end = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_range_get(unit, &vfi_start, &vfi_end));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_port_delete_all(unit, vpn));

    BCMI_LTSW_VIRTUAL_VPN_GET(vfi_index, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, vpn);
    if (vfi_index >= vfi_start) {
        (void)bcmi_ltsw_virtual_vfi_free(unit, BCMI_LTSW_VFI_TYPE_FLOW,
                                         vfi_index);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VPNs.
 *
 * \param [in]  unit     Unit number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_vpn_destroy_all(int unit)
{
    uint32_t vfi_index = 0;
    uint32_t vfi_start = 0, vfi_end = 0;
    bcm_vpn_t vpn = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_range_get(unit, &vfi_start, &vfi_end));

    for (vfi_index = vfi_start; vfi_index <= vfi_end; vfi_index++) {
        if (bcmi_ltsw_virtual_vfi_used_get(unit, vfi_index,
                                           BCMI_LTSW_VFI_TYPE_FLOW)) {
            BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                      vfi_index);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_flow_vpn_destroy(unit, vpn));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get VPN properties.
 *
 * \param [in]   unit      Unit number.
 * \param [out]  info      VPN structure.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_vpn_get(int unit, bcm_vpn_t vpn, bcm_flow_vpn_config_t *info)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    bcm_flow_vpn_config_t_init(info);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vpn_hw_opt(unit, HW_OPT_GET, vpn, info));

    info->flags =  BCM_FLOW_VPN_ELAN;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Traverse VPNs.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  cb         Callback function.
 * \param [in]  user_data  User context data.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_vpn_traverse(int unit, bcm_flow_vpn_traverse_cb cb,
                           void *user_data)
{
    uint32_t vfi_index = 0;
    uint32_t vfi_start = 0, vfi_end = 0;
    bcm_vpn_t vpn = 0;
    bcm_flow_vpn_config_t info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_range_get(unit, &vfi_start, &vfi_end));

    for (vfi_index = vfi_start; vfi_index <= vfi_end; vfi_index++) {
        if (bcmi_ltsw_virtual_vfi_used_get(unit, vfi_index,
                                           BCMI_LTSW_VFI_TYPE_FLOW)) {
            BCMI_LTSW_VIRTUAL_VPN_SET(vpn, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI,
                                      vfi_index);
            bcm_flow_vpn_config_t_init(&info);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_flow_vpn_get(unit, vpn, &info));
            SHR_IF_ERR_VERBOSE_EXIT
                (cb(unit, vpn, &info, user_data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create and add an access/network flow port to L2 VPN.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  vpn       VPN ID.
 * \param [in]  flow_port Flow port properties.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_create(int unit, bcm_vpn_t vpn, bcm_flow_port_t *flow_port)
{
    uint32_t num_vp = 0;
    bcmi_ltsw_virtual_vp_info_t vp_info;
    int vp = -1;
    uint8_t vp_alloc_flag = 0;
    uint32_t vp_start = 0, vp_end = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_port, SHR_E_PARAM);

    if (vpn != BCM_FLOW_VPN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, vpn));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_num_get(unit, &num_vp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_range_get(unit, &vp_start, &vp_end));

    if (flow_port->flags & BCM_FLOW_PORT_REPLACE) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
        if (vp == -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                          BCMI_LTSW_VP_TYPE_FLOW)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        bcmi_ltsw_virtual_vp_info_init(&vp_info);

    } else if (flow_port->flags & BCM_FLOW_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_FLOW_PORT(flow_port->flow_port_id)) {
             SHR_ERR_EXIT(SHR_E_BADID);
         }
         vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
         if (vp == -1) {
             SHR_ERR_EXIT(SHR_E_PARAM);
         }
         if (vp >= num_vp) {
             SHR_ERR_EXIT(SHR_E_BADID);
         }

         if (vp < vp_start || vp > vp_end) {
             SHR_ERR_EXIT(SHR_E_BADID);
         }

         if (!bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                            BCMI_LTSW_VP_TYPE_FLOW)) {
             SHR_ERR_EXIT(SHR_E_EXISTS);
         }
         bcmi_ltsw_virtual_vp_info_init(&vp_info);
         vp_info.flags |= BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW;
         if (flow_port->flags & BCM_FLOW_PORT_NETWORK) {
             vp_info.flags |= BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT;
         }
         vp_info.vpn = vpn;

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_ltsw_virtual_vp_used_set(unit, vp, vp_info));
         vp_alloc_flag = 1;
    } else {
        bcmi_ltsw_virtual_vp_info_init(&vp_info);
        vp_info.flags |= BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW;
        if (flow_port->flags & BCM_FLOW_PORT_NETWORK) {
            vp_info.flags |= BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT;
        }
        vp_info.vpn = vpn;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_alloc(unit, vp_start, (int)vp_end,
                                        1, vp_info, &vp));

        vp_alloc_flag = 1;
        BCM_GPORT_FLOW_PORT_ID_SET(flow_port->flow_port_id, vp);
    }

    /* Update HW table */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vp_hw_opt(unit, HW_OPT_SET, flow_port));

exit:
    if (SHR_FUNC_ERR() && vp_alloc_flag) {
        (void) bcmi_ltsw_virtual_vp_free(unit, BCMI_LTSW_VP_TYPE_FLOW,
                                         1, vp);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an access/network flow port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vpn           VPN ID.
 * \param [in]  flow_port_id  Flow Gport ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_destroy(int unit, bcm_vpn_t vpn, bcm_gport_t flow_port_id)
{
    int vp = -1;
    bcm_flow_port_t flow_port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    if (vpn != BCM_FLOW_VPN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (vp == -1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                      BCMI_LTSW_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    BCM_GPORT_FLOW_PORT_ID_SET(flow_port_id, vp);
    bcm_flow_port_t_init(&flow_port);
    flow_port.flow_port_id = flow_port_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flow_port_get(unit, vpn, &flow_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vp_hw_opt(unit, HW_OPT_CLR, &flow_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_free(unit, BCMI_LTSW_VFI_TYPE_FLOW, 1,
                                   vp));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flow port properties.
 *
 * \param [in]   unit          Unit number.
 * \param [in]   vpn           VPN ID.
 * \param [out]  flow_port     Flow port properties.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_get(int unit, bcm_vpn_t vpn, bcm_flow_port_t *flow_port)
{
    int vp = -1;
    bcmi_ltsw_virtual_vp_info_t vp_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_port, SHR_E_PARAM);

    if (vpn != BCM_FLOW_VPN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, vpn));
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port->flow_port_id);
    if (vp == -1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    bcmi_ltsw_virtual_vp_info_init(&vp_info);
    if (bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (!(vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT) {
        flow_port->flags |= BCM_FLOW_PORT_NETWORK;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vp_hw_opt(unit, HW_OPT_GET, flow_port));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multiple flow port properties.
 *
 * \param [in]   unit           Unit number.
 * \param [in]   vpn            VPN ID.
 * \param [in]   port_max       Maximum number of flow ports in array.
 * \param [out]  port_array     Array of flow ports.
 * \param [out]  port_count     Number of FLOW ports returned in array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                           bcm_flow_port_t *port_array,
                           int *port_count)
{
    uint32_t vp_num = 0;
    uint32_t vp_idx = 0;
    bcmi_ltsw_virtual_vp_info_t vp_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(port_array, SHR_E_PARAM);
    SHR_NULL_CHECK(port_count, SHR_E_PARAM);

    if (vpn != BCM_FLOW_VPN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_flow_vpn_is_valid(unit, vpn));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vp_num_get(unit, &vp_num));

    *port_count = 0;

     for (vp_idx = 0; vp_idx < vp_num; vp_idx++) {
         bcmi_ltsw_virtual_vp_info_init(&vp_info);
         if (bcmi_ltsw_virtual_vp_info_get(unit, vp_idx, &vp_info)) {
             continue;
         }
         if (!(vp_info.flags & BCMI_LTSW_VIRTUAL_VP_TYPE_FLOW)) {
             continue;
         }
         if (vp_info.vpn != vpn) {
             continue;
         }
         if (port_max == 0) {
             /* return port count only. */
             (*port_count)++;
             continue;
         } else if (*port_count >= port_max) {
             SHR_EXIT();
         } else {
             /* do nothing. */
         }

         BCM_GPORT_FLOW_PORT_ID_SET(port_array[*port_count].flow_port_id,
                                    vp_idx);
         SHR_IF_ERR_VERBOSE_EXIT
             (bcm_ltsw_flow_port_get(unit, vpn, &port_array[*port_count]));

         (*port_count)++;
     }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flow port type.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   port     Port number.
 * \param [out]  type     Flow port type, TRUE for access, FALSE for network.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_port_type(int unit, bcm_port_t port, uint8_t *type)
{
    int vp = -1;
    bcmi_ltsw_virtual_vp_info_t vp_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(type, SHR_E_PARAM);

    vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
    if (vp == -1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    bcmi_ltsw_virtual_vp_info_init(&vp_info);
    if (bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT) {
        *type = FALSE;
    } else {
        *type = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whehter flow port is network port.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   port     Port number.
 *
 * \retval TRUE   Flow port exist and is network port.
 * \retval FALSE  Flow port doesn't exist or is access port.
 */
int
bcmi_ltsw_flow_port_is_network(int unit, bcm_port_t port)
{
    int vp = -1;
    bcmi_ltsw_virtual_vp_info_t vp_info;

    if (!ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        return FALSE;
    }

    vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
    if (vp == -1) {
        return FALSE;
    }
    bcmi_ltsw_virtual_vp_info_init(&vp_info);
    if (bcmi_ltsw_virtual_vp_info_get(unit, vp, &vp_info)) {
        return FALSE;
    }
    if (vp_info.flags & BCMI_LTSW_VIRTUAL_VP_NETWORK_PORT) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*!
 * \brief Get flow port from L2 interface.
 *
 * \param [in]  unit  Unit number.
 * \param [in]  l2_if L2 interface.
 * \param [out] port  Flow gport.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_l2_if_to_port(int unit, int l2_if, bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(port, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_l2_if_to_port(unit, l2_if, port));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get L2 interface from flow gport.
 *
 * \param [in]  unit  Unit number.
 * \param [in]  port  Flow gport.
 * \param [out] l2_if L2 interface.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_port_to_l2_if(int unit, bcm_port_t port, int *l2_if)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(l2_if, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_to_l2_if(unit, port, l2_if));

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a match rule to identify traffic flow with certain objects.
 *
 * The function is used to get the internal objects. These objects generally
 * will be used in the switchs forwarding process.
 * The matching rules comprise of packet fields and/or the port number the
 * packet comes in. The identified objects can be VFI,virtual port, interface
 * id, flexible objects specified by logical fields, or a combination of them.
 *
 * \param [in]   unit           Unit number.
 * \param [in]   info           Match info structure.
 * \param [in]   num_of_fields  Number of logical fields.
 * \param [in]   field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_match_add(int unit, bcm_flow_match_config_t *info,
                        uint32_t num_of_fields,
                        bcm_flow_logical_field_t *field)
{
    int locked = false;
    int vp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                          BCMI_LTSW_VP_TYPE_FLOW)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_SET, info));

    if (flow_match_assigned_to_flow_port(unit, info)) {
        flow_port_match_info_set(unit, info);
    }

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a match rule for the given match_criteria.
 *
 * \param [in]   unit           Unit number.
 * \param [in]   info           Match info structure.
 * \param [in]   num_of_fields  Number of logical fields.
 * \param [in]   field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_match_delete(int unit, bcm_flow_match_config_t *info,
                           uint32_t num_of_fields,
                           bcm_flow_logical_field_t *field)
{
    int locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_CLR, info));

    if (flow_match_assigned_to_flow_port(unit, info)) {
        flow_port_match_info_reset(unit, info);
    }

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the match objects and attributes for the given match_criteria.
 *
 * \param [in]   unit           Unit number.
 * \param [out]  info           Match info structure.
 * \param [in]   num_of_fields  Number of logical fields.
 * \param [out]  field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_match_get(int unit, bcm_flow_match_config_t *info,
                        uint32_t num_of_fields,
                        bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_GET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse match rules.
 *
 * \param [in]   unit        Unit number.
 * \param [in]   cb          User callback function.
 * \param [in]   user_data   User context data.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_match_traverse(int unit, bcm_flow_match_traverse_cb cb,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add the object(key) based encapsulation data to the packet.
 *
 * This function is used to configure encapsulation to object. The objects
 * can be VP,VFI,VRF,interface ID, specified by logical fields, or combination
 * of them. The encapsulation data comprises of fields in the
 * bcm_flow_encap_config_t and/or logical fields.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Encap configuration info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_encap_add(int unit, bcm_flow_encap_config_t *info,
                        uint32_t num_of_fields,
                        bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_hw_opt(unit, HW_OPT_SET, info, num_of_fields,
                                     field));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the object(key) based encapsulation data.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Encap configuration info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_encap_delete(int unit, bcm_flow_encap_config_t *info,
                           uint32_t num_of_fields,
                           bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_hw_opt(unit, HW_OPT_CLR, info, num_of_fields,
                                     field));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the object(key) based encapsulation data.
 *
 * \param [in]    unit           Unit number.
 * \param [out]   info           Encap configuration info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [out]   field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_encap_get(int unit, bcm_flow_encap_config_t *info,
                        uint32_t num_of_fields,
                        bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_hw_opt(unit, HW_OPT_GET, info, num_of_fields,
                                     field));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse the object(key) based encapsulation data entries.
 *
 * \param [in]    unit         Unit number.
 * \param [in]    cb           User callback function.
 * \param [in]    user_data    User context data.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_encap_traverse(int unit, bcm_flow_encap_traverse_cb cb,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Bind the egress object and tunnel initiator with the DVP.
 *
 * This function is used to bind DVP with egress object or tunnel initiator. It
 * also program DVP related encap data for L2 tunnel.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Egress encap info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_encap_set(int unit, bcm_flow_port_encap_t *info,
                             uint32_t num_of_fields,
                             bcm_flow_logical_field_t *field)
{
    bcm_flow_port_encap_t cur_info;
    int rv;
    int egress_if_updated = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    bcm_flow_port_encap_t_init(&cur_info);
    cur_info.flow_port = info->flow_port;
    rv = mbcm_ltsw_flow_port_encap_hw_opt(unit, HW_OPT_GET, &cur_info);
    if(SHR_SUCCESS(rv)) {
        if ((info->valid_elements & BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID) &&
            (info->egress_if != cur_info.egress_if)) {
            egress_if_updated = 1;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_encap_hw_opt(unit, HW_OPT_SET, info));

    if (egress_if_updated) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (flow_port_encap_egress_if_notify(unit, info), SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the encap configuration info for the given DVP.
 *
 * \param [in]    unit           Unit number.
 * \param [out]   info           Egress encap info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [out]   field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_port_encap_get(int unit, bcm_flow_port_encap_t *info,
                             uint32_t num_of_fields,
                             bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_encap_hw_opt(unit, HW_OPT_GET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a tunnel header for packet encapsulation.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Tunnel config info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_initiator_create(int unit,
                                      bcm_flow_tunnel_initiator_t *info,
                                      uint32_t num_of_fields,
                                      bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_hw_opt(unit, HW_OPT_SET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Felete a tunnel header entry.
 *
 * \param [in]    unit               Unit number.
 * \param [in]    flow_tunnel_id     Tunnel ID Gport.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_initiator_destroy(int unit, bcm_gport_t flow_tunnel_id)
{
    bcm_flow_tunnel_initiator_t info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    bcm_flow_tunnel_initiator_t_init(&info);
    info.tunnel_id = flow_tunnel_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_hw_opt(unit, HW_OPT_CLR, &info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a tunnel header entry.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Tunnel config info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_initiator_get(int unit, bcm_flow_tunnel_initiator_t *info,
                                   uint32_t num_of_fields,
                                   bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_hw_opt(unit, HW_OPT_GET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse tunnel header entries.
 *
 * \param [in]    unit         Unit number.
 * \param [in]    cb           User callback function.
 * \param [in]    user_data    User context datas.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_initiator_traverse(int unit,
                                        bcm_flow_tunnel_initiator_traverse_cb cb,
                                        void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if given FLOW VPN is valid.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vpn       VPN ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmint_flow_vpn_is_valid(int unit, bcm_vpn_t vpn)
{
    bcm_vpn_t flow_vpn_min = 1;
    uint32_t vfi_index = -1, num_vfi = 0;
    int exist = 0;

    SHR_FUNC_ENTER(unit);

    if(!BCMI_LTSW_VIRTUAL_VPN_IS_SET(vpn) &&
       ltsw_feature(unit, LTSW_FT_FLEX_FLOW_VLAN_DOMAIN)) {

       SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_vlan_exist_check(unit, vpn, &exist));
       if (!exist) {
           SHR_ERR_EXIT(SHR_E_NOT_FOUND);
       }
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_num_get(unit, &num_vfi));

    BCMI_LTSW_VIRTUAL_VPN_SET(flow_vpn_min, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, 0);

    if (vpn < flow_vpn_min || vpn > (flow_vpn_min + num_vfi - 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    BCMI_LTSW_VIRTUAL_VPN_GET(vfi_index, BCMI_LTSW_VIRTUAL_VPN_TYPE_VFI, vpn);
    if (!bcmi_ltsw_virtual_vfi_used_get(unit, vfi_index,
                                        BCMI_LTSW_VFI_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get physical tunnel index based on logical tunnel index.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    soft_tnl_idx   Logical tnuuel index.
 * \param [out]   tnl_idx        Physical tunnel index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmint_flow_tunnel_idx_get(int unit, uint32_t soft_tnl_idx,
                           uint32_t *tnl_idx)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
    uint32_t start = 0;
    uint32_t end = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(tnl_idx, SHR_E_PARAM);
    *tnl_idx = -1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_tnl_range_get(unit, &start, &end));
    if ((soft_tnl_idx < start) || (soft_tnl_idx > end)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));

    *tnl_idx = flow_info->tnl_initiator_id_map[soft_tnl_idx];

    flow_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set physical tunnel index to logical tunnel index mapping.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    soft_tnl_idx   Logical tnuuel index.
 * \param [in]    tnl_idx        Physical tunnel index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmint_flow_tunnel_idx_set(int unit, uint32_t soft_tnl_idx,
                           uint32_t tnl_idx)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
    uint32_t start = 0;
    uint32_t end = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_tnl_range_get(unit, &start, &end));
    if ((soft_tnl_idx < start) || (soft_tnl_idx > end)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));

    flow_info->tnl_initiator_id_map[soft_tnl_idx] = tnl_idx;

    flow_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get physical tunnel index based on logical tunnel index during
 * warmboot.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    soft_tnl_idx   Logical tnuuel index.
 * \param [out]   tnl_idx        Physical tunnel index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmint_flow_warmboot_tunnel_idx_get(int unit, uint32_t soft_tnl_idx,
                                    uint32_t *tnl_idx)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
    uint32_t start = 0;
    uint32_t end = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * This interface is only for WB during initialization.
     * bcmint_flow_tunnel_idx_get should be used after initialization.
    */
    if (flow_info->initialized) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_NULL_CHECK(tnl_idx, SHR_E_PARAM);
    *tnl_idx = -1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_tnl_range_get(unit, &start, &end));
    if ((soft_tnl_idx < start) || (soft_tnl_idx > end)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *tnl_idx = flow_info->tnl_initiator_id_map[soft_tnl_idx];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get logical tunnel index based on physical tunnel index.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    tnl_idx        Physical tunnel index.
 * \param [out]   soft_tnl_idx   Logical tnuuel index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmint_flow_tunnel_sw_idx_get(int unit, uint32_t tnl_idx,
                              uint32_t *soft_tnl_idx)
{
    ltsw_flow_bookkeeping_t *flow_info = &ltsw_flow_bk_info[unit];
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t tmp_soft_tnl_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(soft_tnl_idx, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_tnl_range_get(unit, &start, &end));

    SHR_IF_ERR_VERBOSE_EXIT(flow_lock(unit));
    for (tmp_soft_tnl_idx = start; tmp_soft_tnl_idx < end; tmp_soft_tnl_idx++) {
        if (flow_info->tnl_initiator_id_map[tmp_soft_tnl_idx] == tnl_idx) {
            break;
        }
    }
    flow_unlock(unit);

    if (tmp_soft_tnl_idx >= end) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        *soft_tnl_idx = tmp_soft_tnl_idx;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flow tunnel terminator match.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Tunnel config info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_terminator_create(int unit,
                                       bcm_flow_tunnel_terminator_t *info,
                                       uint32_t num_of_fields,
                                       bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_term_hw_opt(unit, HW_OPT_SET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a tunnel termination entry.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    info           Tunnel config info structure.
 * \param [in]    num_of_fields  Number of logical fields.
 * \param [in]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int bcm_ltsw_flow_tunnel_terminator_destroy(int unit,
                                            bcm_flow_tunnel_terminator_t *info,
                                            uint32_t num_of_fields,
                                            bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_term_hw_opt(unit, HW_OPT_CLR, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a tunnel termination entry.
 *
 * \param [in]    unit           Unit number.
 * \param [out]    info           Tunnel config info structure.
 * \param [out]    num_of_fields  Number of logical fields.
 * \param [out]    field          Logical field array.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_terminator_get(int unit,
                                    bcm_flow_tunnel_terminator_t *info,
                                    uint32_t num_of_fields,
                                    bcm_flow_logical_field_t *field)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_term_hw_opt(unit, HW_OPT_GET, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse tunnel termination entries.
 *
 * \param [in]    unit         Unit number.
 * \param [in]    cb           User callback function.
 * \param [in]    user_data    User context datas.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_tunnel_terminator_traverse(int unit,
                                         bcm_flow_tunnel_terminator_traverse_cb cb,
                                         void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_term_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the accounting stat object associated with the table.
 *
 * \param [in]    unit               Unit number.
 * \param [in]    flow_handle        Flow handle.
 * \param [in]    flow_option_id     Flow option ID.
 * \param [in]    function_type      Flow function type.
 * \param [out]   stat_object        Stat accounting object.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int bcm_ltsw_flow_stat_object_get(int unit, bcm_flow_handle_t flow_handle,
                                  bcm_flow_option_id_t flow_option_id,
                                  bcm_flow_function_type_t function_type,
                                  bcm_stat_object_t *stat_object)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Attach counter entries to the given table.
 *
 * \param [in]    unit               Unit number.
 * \param [in]    flow_stat_info     Flow stat config structure.
 * \param [in]    num_of_fields      Number of logical fields.
 * \param [in]    field              Logical fields.
 * \param [in]    stat_counter_id    Stat counter id.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_stat_attach(int unit, bcm_flow_stat_info_t *flow_stat_info,
                          uint32_t num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_stat_info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_stat_attach(unit, flow_stat_info, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries from the given table.
 *
 * \param [in]    unit               Unit number.
 * \param [in]    flow_stat_info     Flow stat config structure.
 * \param [in]    num_of_fields      Number of logical fields.
 * \param [in]    field              Logical fields.
 * \param [in]    stat_counter_id    Stat counter id.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_stat_detach(int unit, bcm_flow_stat_info_t *flow_stat_info,
                          uint32_t num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_NULL_CHECK(flow_stat_info, SHR_E_PARAM);
    if (num_of_fields) {
        SHR_NULL_CHECK(field, SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_stat_detach(unit, flow_stat_info, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Stat counter of associated with table.
 *
 * \param [in]    unit               Unit number.
 * \param [in]    flow_stat_info     Flow stat config structure.
 * \param [in]    num_of_fields      Number of logical fields.
 * \param [in]    field              Logical fields.
 * \param [in]    stat_object        Stat object.
 * \param [Out]   stat_counter_id    Stat counter id.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcm_ltsw_flow_stat_id_get(int unit, bcm_flow_stat_info_t *flow_stat_info,
                          uint32_t num_of_fields,
                          bcm_flow_logical_field_t *field,
                          bcm_stat_object_t stat_object,
                          uint32_t *stat_counter_id)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Helper funtion to get modid, port, and trunk_id from a flow port.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   gport     Flow port.
 * \param [out]  modid     Module ID.
 * \param [out]  port      Port number.
 * \param [out]  trunk_id  Trunk ID.
 * \param [out]  id        HW ID.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_gport_resolve(int unit, bcm_gport_t flow_port,
                             bcm_module_t *modid, bcm_port_t *port,
                             bcm_trunk_t *trunk_id, int *id)
{
    int vp = -1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_NULL_CHECK(modid, SHR_E_PARAM);
    SHR_NULL_CHECK(port, SHR_E_PARAM);
    SHR_NULL_CHECK(trunk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    *modid = -1;
    *port = -1;
    *trunk_id = -1;
    *id = -1;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port);
    if (vp == -1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                       BCMI_LTSW_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (bcmi_ltsw_flow_port_is_network(unit, flow_port)) {
        bcm_flow_port_encap_t info;

        bcm_flow_port_encap_t_init(&info);
        info.flow_port = flow_port;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flow_port_encap_hw_opt(unit, HW_OPT_GET, &info));

    }

    *id = vp;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Display Flow software structure information.
 *
 * This function is used to display Flow software structure information.
 *
 * \retval None
 */
void
bcmi_ltsw_flow_sw_dump(int unit)
{
    #define IP6_STR_LEN  (64 + 1)
    ltsw_flow_bookkeeping_t   *fi = &(ltsw_flow_bk_info[unit]);
    bcmint_flow_vp_match_t  *vp_match;
    char ip_str[IP6_STR_LEN];
    int vp = 0;
    uint32_t vp_num = 0;
    uint32_t tnl_idx;
    int prints_per_line;

    if (!fi->initialized) {
        return;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information FLOW - Unit %d\n"), unit));

    if (SHR_FAILURE(bcmi_ltsw_virtual_vp_num_get(unit, &vp_num))) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nVP and Tunnel index mapping:\n")));
    prints_per_line = 0;
    for (vp = 0; vp < vp_num; vp++) {
        tnl_idx = fi->tnl_initiator_id_map[vp];
        if (tnl_idx == 0) {
            continue;
        }
        prints_per_line++;
        if(prints_per_line % 2 == 0) {
            prints_per_line = 0;
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "      VP %#x: tunnel index %#x"), vp, tnl_idx));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

    LOG_CLI((BSL_META_U(unit,
                        "\nVP and Match info:\n")));
    prints_per_line = 0;
    for (vp = 0; vp < vp_num; vp++) {
        vp_match = &(fi->vp_match[vp]);
        if (!vp_match->valid) {
            continue;
        }
        prints_per_line++;
        if(prints_per_line % 1 == 0) {
            prints_per_line = 0;
            LOG_CLI((BSL_META_U(unit, "\n")));
        }

        LOG_CLI((BSL_META_U(unit,
                            "      VP %#x:"), vp));
        LOG_CLI((BSL_META_U(unit, "\t criteria %#x"), vp_match->criteria));
        LOG_CLI((BSL_META_U(unit, "\t sip %#x"), vp_match->sip));
        if ((vp_match->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) &&
            !(vp_match->valid_elements & BCM_FLOW_MATCH_SIP_VALID)) {
            sal_memset(ip_str, 0, IP6_STR_LEN);
            bcmi_ltsw_util_ip6_to_str(ip_str, vp_match->sip6);
            LOG_CLI((BSL_META_U(unit, "\t sip6 0x%-s"), vp_match->sip6));
        }
        LOG_CLI((BSL_META_U(unit, "\t elements %#x"),
                                                     vp_match->valid_elements));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

    /* Dump device-specific SW FLOW info. */
    (void)mbcm_ltsw_flow_sw_dump(unit);

    return;
}

/*!
 * \brief Resolve the tunnel ID into the tunnel logical table index.
 *
 * \param [in]    unit           Unit number.
 * \param [in]    tunnel_id      Tunnel gport ID.
 * \param [out]   tnl_idx        Tunnel logical table index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
bcmi_ltsw_flow_tunnel_id_resolve(int unit, bcm_gport_t tunnel_id,
                                 uint32_t *tnl_idx)
{
    bcmi_ltsw_tunnel_type_t tnl_type;
    uint32_t soft_tnl_idx = 0;

    SHR_FUNC_ENTER(unit);

    if (!BCM_GPORT_IS_SET(tunnel_id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCMI_LTSW_GPORT_TUNNEL_ID_GET(tunnel_id, tnl_type, soft_tnl_idx);
    if (tnl_type != bcmiTunnelTypeFlexFlow) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_flow_tunnel_idx_get(unit, soft_tnl_idx, tnl_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the match of flow gport to the vp_lag vp.
 *
 * \param [in] unit Unit Number.
 * \param [in] flow_port_id Flow gport ID.
 * \param [in] vp_lag_vp VPLAG vp Id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_flow_port_source_vp_lag_set(
    int unit,
    bcm_gport_t flow_port_id,
    int vp_lag_vp)
{
    int vp = -1;
    bcm_flow_match_config_t info;
    int locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = true;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                      BCMI_LTSW_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (bcmi_ltsw_virtual_vp_used_get(unit, vp_lag_vp,
                                      BCMI_LTSW_VP_TYPE_VPLAG)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    bcm_flow_match_config_t_init(&info);
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_port_match_info_get(unit, flow_port_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_GET, &info));

    info.valid_elements |= BCM_FLOW_MATCH_FLOW_PORT_VALID;
    BCM_GPORT_FLOW_PORT_ID_SET(info.flow_port, vp_lag_vp);
    info.options |= BCM_FLOW_MATCH_OPTION_REPLACE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_SET, &info));

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the vp_lag vp matched by the criteria of the flow gport.
 *
 * \param [in] unit Unit Number.
 * \param [in] flow_port_id Flow gport ID.
 * \param [out] vp_lag_vp VPLAG vp Id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_flow_port_source_vp_lag_get(
    int unit,
    bcm_gport_t flow_port_id,
    int *vp_lag_vp)
{
    int vp = -1;
    bcm_flow_match_config_t info;
    int locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = true;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                      BCMI_LTSW_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    bcm_flow_match_config_t_init(&info);
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_port_match_info_get(unit, flow_port_id, &info));
    info.valid_elements |= BCM_FLOW_MATCH_FLOW_PORT_VALID;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_GET, &info));

    if (info.flow_port == flow_port_id) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (vp_lag_vp) {
        vp = BCM_GPORT_FLOW_PORT_ID_GET(info.flow_port);
        if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                          BCMI_LTSW_VP_TYPE_VPLAG)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        *vp_lag_vp = vp;
    }

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief clear the vp_lag vp matched by the criteria of the flow gport.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport The flow gport.
 * \param [in] vp_lag_vp The VPLAG vp Id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_flow_port_source_vp_lag_clr(
    int unit,
    bcm_gport_t flow_port_id,
    int vp_lag_vp)
{
    int vp = -1;
    bcm_flow_match_config_t info;
    int locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_feature_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_init_check(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (flow_lock(unit));
    locked = true;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(flow_port_id);
    if (bcmi_ltsw_virtual_vp_used_get(unit, vp,
                                      BCMI_LTSW_VP_TYPE_FLOW)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    bcm_flow_match_config_t_init(&info);
    SHR_IF_ERR_VERBOSE_EXIT
        (flow_port_match_info_get(unit, flow_port_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_GET, &info));

    info.valid_elements |= BCM_FLOW_MATCH_FLOW_PORT_VALID;
    info.flow_port       = flow_port_id;
    info.options |= BCM_FLOW_MATCH_OPTION_REPLACE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_hw_opt(unit, HW_OPT_SET, &info));

exit:
    if (locked) {
        flow_unlock(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flow_evpn_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_evpn_enable_set(unit, enable));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flow_evpn_enable_get(
    int unit,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_evpn_enable_get(unit, enable));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flow_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_switch_control_set(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flow_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_switch_control_get(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_set(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t es_id,
    uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_set(unit, flow_port_id, es_id, flags));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_get(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t es_id,
    uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_get(unit, flow_port_id, es_id, flags));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_flexctr_object_set(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_flexctr_object_set(unit, flow_port_id, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_flexctr_object_get(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_flexctr_object_get(unit, flow_port_id, value));

exit:
    SHR_FUNC_EXIT();
}


int bcm_ltsw_flow_es_filter_stat_attach(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_stat_attach(unit, flow_port_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_stat_detach(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_stat_detach(unit, flow_port_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_es_filter_stat_id_get(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_es_filter_stat_id_get(unit, flow_port_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}


int bcm_ltsw_flow_vpn_stat_attach(
    int unit,
    bcm_vpn_t vpn,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vpn_stat_attach(unit, vpn, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_vpn_stat_detach(
    int unit,
    bcm_vpn_t vpn,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vpn_stat_detach(unit, vpn, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_vpn_stat_id_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_flexctr_direction_t direction,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_vpn_stat_id_get(unit, vpn, direction, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_port_stat_attach(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_stat_attach(unit, flow_port_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_port_stat_detach(
    int unit,
    bcm_gport_t flow_port_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_stat_detach(unit, flow_port_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_port_stat_id_get(
    int unit,
    bcm_gport_t flow_port_id,
    bcm_flexctr_direction_t direction,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_port_stat_id_get(unit, flow_port_id, direction, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_initiator_stat_attach(
    int unit,
    bcm_gport_t tunnel_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_stat_attach(unit, tunnel_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_initiator_stat_detach(
    int unit,
    bcm_gport_t tunnel_id,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_stat_detach(unit, tunnel_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_initiator_stat_id_get(
    int unit,
    bcm_gport_t tunnel_id,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_initiator_stat_id_get(unit, tunnel_id, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_encap_flexctr_object_set(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_flexctr_object_set(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_encap_flexctr_object_get(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_flexctr_object_get(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_encap_stat_attach(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_stat_attach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_encap_stat_detach(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_stat_detach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_encap_stat_id_get(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_encap_stat_id_get(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_match_flexctr_object_set(
    int unit,
    bcm_flow_match_config_t *info,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_flexctr_object_set(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_match_flexctr_object_get(
    int unit,
    bcm_flow_match_config_t *info,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_flexctr_object_get(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_match_stat_attach(
    int unit,
    bcm_flow_match_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_stat_attach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_match_stat_detach(
    int unit,
    bcm_flow_match_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_stat_detach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_match_stat_id_get(
    int unit,
    bcm_flow_match_config_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_match_stat_id_get(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_terminator_flexctr_object_set(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_terminator_flexctr_object_get(unit, info, value));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_terminator_stat_attach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_terminator_stat_detach(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32_t num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_tunnel_terminator_stat_id_get(unit, info, num_of_fields, field, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}
