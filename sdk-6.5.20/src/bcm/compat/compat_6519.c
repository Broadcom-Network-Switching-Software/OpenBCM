/*
 * $Id: compat_6519.c,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.19 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6519.h>

/*
 * Function:
 *   _bcm_compat6519in_cosq_burst_monitor_flow_view_info_t
 * Purpose:
 *   Convert the bcm_cosq_burst_monitor_flow_view_info_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN)  The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_cosq_burst_monitor_flow_view_info_t(
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *from,
        bcm_cosq_burst_monitor_flow_view_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    COMPILER_64_SET(to->export_param.export_threshold, 0,
                    from->export_param.export_threshold);
    to->export_param.weights[1] = from->export_param.rx_weight;
    to->export_param.weights[5] = from->export_param.drop_weight;
    COMPILER_64_COPY(to->start_timer_usecs, from->start_timer_usecs);
    COMPILER_64_COPY(to->age_timer_usecs, from->age_timer_usecs);
    to->scan_interval_usecs = from->scan_interval_usecs;
    to->age_threshold = from->age_threshold;
    to->num_samples = from->num_samples;
    to->num_flows = from->num_flows;
    to->num_flows_clear = from->num_flows_clear;
}

/*
 * Function:
 *   _bcm_compat6519out_cosq_burst_monitor_flow_view_info_t
 * Purpose:
 *   Convert the bcm_cosq_burst_monitor_flow_view_info_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN)  The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_cosq_burst_monitor_flow_view_info_t(
        bcm_cosq_burst_monitor_flow_view_info_t *from,
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *to)
{
    sal_memcpy(&to->export_param, &from->export_param, sizeof(bcm_cosq_burst_monitor_export_trigger_t));
    to->scan_interval_usecs = from->scan_interval_usecs;
    to->age_threshold = from->age_threshold;
    COMPILER_64_COPY(to->start_timer_usecs, from->start_timer_usecs);
    to->num_samples = from->num_samples;

    COMPILER_64_TO_32_LO(to->export_param.export_threshold,
                         from->export_param.export_threshold);
    to->export_param.rx_weight = from->export_param.weights[1];
    to->export_param.drop_weight = from->export_param.weights[5];
    COMPILER_64_COPY(to->start_timer_usecs, from->start_timer_usecs);
    COMPILER_64_COPY(to->age_timer_usecs, from->age_timer_usecs);
    to->scan_interval_usecs = from->scan_interval_usecs;
    to->age_threshold = from->age_threshold;
    to->num_samples = from->num_samples;
    to->num_flows = from->num_flows;
    to->num_flows_clear = from->num_flows_clear;
}

/*
 * Function:
 *   bcm_compat6519_cosq_burst_monitor_flow_view_config_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_cosq_burst_monitor_flow_view_config_set.
 * Parameters:
 *     unit                 - (IN) BCM device number
 *     options              - (IN) Update and clear options
 *     flow_view            - (IN) QCM flow view information.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_cosq_burst_monitor_flow_view_config_set(int unit,
        uint32 options,
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *flow_view)
{
    int rv = 0;
    bcm_cosq_burst_monitor_flow_view_info_t *new_flow_view = NULL;

    if (flow_view != NULL) {
        new_flow_view = (bcm_cosq_burst_monitor_flow_view_info_t *)
            sal_alloc(sizeof(bcm_cosq_burst_monitor_flow_view_info_t), "New Flow View");
        if (new_flow_view == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_cosq_burst_monitor_flow_view_info_t(flow_view, new_flow_view);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_cosq_burst_monitor_flow_view_config_set(unit, options, new_flow_view);

    if (rv >= 0) {
        _bcm_compat6519out_cosq_burst_monitor_flow_view_info_t(new_flow_view, flow_view);
    }

    sal_free(new_flow_view);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_cosq_burst_monitor_flow_view_config_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_cosq_burst_monitor_flow_view_config_get.
 * Parameters:
 *   unit           - (IN)  BCM device number.
 *   flow_view      - (OUT) QCM flow view information.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_cosq_burst_monitor_flow_view_config_get(int unit,
        bcm_compat6519_cosq_burst_monitor_flow_view_info_t *flow_view)
{
    int rv = 0;
    bcm_cosq_burst_monitor_flow_view_info_t *new_flow_view = NULL;

    if (flow_view != NULL) {
        new_flow_view = (bcm_cosq_burst_monitor_flow_view_info_t *)
            sal_alloc(sizeof(bcm_cosq_burst_monitor_flow_view_info_t), "New Flow View");
        if (new_flow_view == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_cosq_burst_monitor_flow_view_config_get(unit, new_flow_view);

    if (rv >= 0) {
        _bcm_compat6519out_cosq_burst_monitor_flow_view_info_t(new_flow_view, flow_view);
    }

    sal_free(new_flow_view);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_time_tod_t
 * Purpose:
 *      Convert the bcm_time_tod_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_time_tod_t(
    bcm_compat6519_time_tod_t *from,
    bcm_time_tod_t *to)
{
    bcm_time_tod_t_init(to);
    to->time_format = from->time_format;
    sal_memcpy(&to->ts, &from->ts, sizeof(bcm_time_spec_t));
    to->ts_adjustment_counter_ns = from->ts_adjustment_counter_ns;
}

/*
 * Function:
 *      _bcm_compat6519out_time_tod_t
 * Purpose:
 *      Convert the bcm_time_tod_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_time_tod_t(
    bcm_time_tod_t *from,
    bcm_compat6519_time_tod_t *to)
{
    to->time_format = from->time_format;
    sal_memcpy(&to->ts, &from->ts, sizeof(bcm_time_spec_t));
    to->ts_adjustment_counter_ns = from->ts_adjustment_counter_ns;
}

/*
 * Function:
 *      bcm_compat6519_time_tod_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_time_tod_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      stages - (IN) ASIC stage where tod value is set
 *      tod - (IN) Time of the day values.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_time_tod_set(
    int unit,
    uint32 stages,
    bcm_compat6519_time_tod_t *tod)
{
    int rv = BCM_E_NONE;
    bcm_time_tod_t *new_tod = NULL;

    if (tod != NULL) {
        new_tod = (bcm_time_tod_t *)
                     sal_alloc(sizeof(bcm_time_tod_t),
                     "New tod");
        if (new_tod == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_time_tod_t(tod, new_tod);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_time_tod_set(unit, stages, new_tod);

    /* Deallocate memory */
    sal_free(new_tod);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_time_tod_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_time_tod_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      stages - (IN) ASIC stage where tod value is set
 *      tod - (OUT) Time of the day values.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_time_tod_get(
    int unit,
    uint32 stages,
    bcm_compat6519_time_tod_t *tod)
{
    int rv = BCM_E_NONE;
    bcm_time_tod_t *new_tod = NULL;

    if (tod != NULL) {
        new_tod = (bcm_time_tod_t *)
                     sal_alloc(sizeof(bcm_time_tod_t),
                     "New tod");
        if (new_tod == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_time_tod_get(unit, stages, new_tod);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_time_tod_t(new_tod, tod);

    /* Deallocate memory */
    sal_free(new_tod);

    return rv;
}

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6519in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.19 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flow_encap_config_t(bcm_compat6519_flow_encap_config_t *from,
                                      bcm_flow_encap_config_t *to)
{
    bcm_flow_encap_config_t_init(to);
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;
 }

/*
 * Function:
 *      _bcm_compat6519out_ecn_map_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from 6.5.19+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flow_encap_config_t(bcm_flow_encap_config_t *from,
                            bcm_compat6519_flow_encap_config_t *to)
{
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;

}

/*
 * Function: bcm_compat6519_flow_encap_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_get.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6519_flow_encap_get(int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6519_flow_encap_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_flow_encap_delete.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6519_flow_encap_delete(int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_delete(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6519_flow_encap_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_flow_encap_add.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6519_flow_encap_add(int unit,
    bcm_compat6519_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_add(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flow_port_encap_t(
    bcm_compat6519_flow_port_encap_t *from,
    bcm_flow_port_encap_t *to)
{
    sal_memset(to, 0, sizeof(*to));
    to->flow_port       = from->flow_port;
    to->options         = from->options;
    to->flags           = from->flags;
    to->class_id        = from->class_id;
    to->mtu             = from->mtu;
    to->network_group   = from->network_group;
    to->dvp_group       = from->dvp_group;
    to->pri             = from->pri;
    to->cfi             = from->cfi;
    to->tpid            = from->tpid;
    to->vlan            = from->vlan;
    to->egress_if       = from->egress_if;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    to->ip4_id          = from->ip4_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->dst_port        = from->dst_port;

}

/*
 * Function:
 *      _bcm_compat6519out_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype f from 6.5.19+ to <=6.5.19.
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flow_port_encap_t(
    bcm_flow_port_encap_t *from,
    bcm_compat6519_flow_port_encap_t *to)
{
    to->flow_port       = from->flow_port;
    to->options         = from->options;
    to->flags           = from->flags;
    to->class_id        = from->class_id;
    to->mtu             = from->mtu;
    to->network_group   = from->network_group;
    to->dvp_group       = from->dvp_group;
    to->pri             = from->pri;
    to->cfi             = from->cfi;
    to->tpid            = from->tpid;
    to->vlan            = from->vlan;
    to->egress_if       = from->egress_if;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    to->ip4_id          = from->ip4_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->dst_port        = from->dst_port;
}

int
bcm_compat6519_flow_port_encap_set(
    int unit,
    bcm_compat6519_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_set(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6519_flow_port_encap_get(
    int unit,
    bcm_compat6519_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flow_tunnel_terminator_t(
    bcm_compat6519_flow_tunnel_terminator_t *from,
    bcm_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    bcm_flow_tunnel_terminator_t_init(to);
    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
}

/*
 * Function:
 *      _bcm_compat6519out_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype f from 6.5.19+ to <=6.5.19.
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flow_tunnel_terminator_t(
    bcm_flow_tunnel_terminator_t *from,
    bcm_compat6519_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
}

int
bcm_compat6519_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_create(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6519_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_destroy(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;

}


int
bcm_compat6519_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6519_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;

}

/*
 * Function:
 *      _bcm_compat6519in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.19 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_mpls_port_t(
    bcm_compat6519_mpls_port_t *from,
    bcm_mpls_port_t *to)
{
    bcm_mpls_port_t_init(to);

    to->mpls_port_id                 = from->mpls_port_id;
    to->flags                        = from->flags;
    to->flags2                       = from->flags2;
    to->if_class                     = from->if_class;
    to->exp_map                      = from->exp_map;
    to->int_pri                      = from->int_pri;
    to->pkt_pri                      = from->pkt_pri;
    to->pkt_cfi                      = from->pkt_cfi;
    to->service_tpid                 = from->service_tpid;
    to->port                         = from->port;
    to->criteria                     = from->criteria;
    to->match_vlan                   = from->match_vlan;
    to->match_inner_vlan             = from->match_inner_vlan;
    to->egress_tunnel_if             = from->egress_tunnel_if;
    to->egress_label                 = from->egress_label;
    to->mtu                          = from->mtu;
    to->egress_service_vlan          = from->egress_service_vlan;
    to->pw_seq_number                = from->pw_seq_number;
    to->encap_id                     = from->encap_id;
    to->ingress_failover_id          = from->ingress_failover_id;
    to->ingress_failover_port_id     = from->ingress_failover_port_id;
    to->failover_id                  = from->failover_id;
    to->failover_port_id             = from->failover_port_id;
    to->policer_id                   = from->policer_id;
    to->failover_mc_group            = from->failover_mc_group;
    to->pw_failover_id               = from->pw_failover_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->vccv_type                    = from->vccv_type;
    to->network_group_id             = from->network_group_id;
    to->match_subport_pkt_vid        = from->match_subport_pkt_vid;
    to->tunnel_id                    = from->tunnel_id;
    to->per_flow_queue_base          = from->per_flow_queue_base;
    to->qos_map_id                   = from->qos_map_id;
    to->egress_failover_id           = from->egress_failover_id;
    to->egress_failover_port_id      = from->egress_failover_port_id;
    to->ecn_map_id                   = from->ecn_map_id;
    to->class_id                     = from->class_id;
    to->egress_class_id              = from->egress_class_id;
    to->inlif_counting_profile       = from->inlif_counting_profile;
    to->egress_tunnel_label          = from->egress_tunnel_label;
    to->nof_service_tags             = from->nof_service_tags;
    to->ingress_qos_model            = from->ingress_qos_model;
    to->context_label                = from->context_label;
    to->ingress_if                   = from->ingress_if;
    to->port_group                   = from->port_group;

    return;
}

/*
 * Function:
 *      _bcm_compat6519out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from SDK 6.5.19+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6519_mpls_port_t *to)
{
    to->mpls_port_id                 = from->mpls_port_id;
    to->flags                        = from->flags;
    to->flags2                       = from->flags2;
    to->if_class                     = from->if_class;
    to->exp_map                      = from->exp_map;
    to->int_pri                      = from->int_pri;
    to->pkt_pri                      = from->pkt_pri;
    to->pkt_cfi                      = from->pkt_cfi;
    to->service_tpid                 = from->service_tpid;
    to->port                         = from->port;
    to->criteria                     = from->criteria;
    to->match_vlan                   = from->match_vlan;
    to->match_inner_vlan             = from->match_inner_vlan;
    to->egress_tunnel_if             = from->egress_tunnel_if;
    to->egress_label                 = from->egress_label;
    to->mtu                          = from->mtu;
    to->egress_service_vlan          = from->egress_service_vlan;
    to->pw_seq_number                = from->pw_seq_number;
    to->encap_id                     = from->encap_id;
    to->ingress_failover_id          = from->ingress_failover_id;
    to->ingress_failover_port_id     = from->ingress_failover_port_id;
    to->failover_id                  = from->failover_id;
    to->failover_port_id             = from->failover_port_id;
    to->policer_id                   = from->policer_id;
    to->failover_mc_group            = from->failover_mc_group;
    to->pw_failover_id               = from->pw_failover_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->vccv_type                    = from->vccv_type;
    to->network_group_id             = from->network_group_id;
    to->match_subport_pkt_vid        = from->match_subport_pkt_vid;
    to->tunnel_id                    = from->tunnel_id;
    to->per_flow_queue_base          = from->per_flow_queue_base;
    to->qos_map_id                   = from->qos_map_id;
    to->egress_failover_id           = from->egress_failover_id;
    to->egress_failover_port_id      = from->egress_failover_port_id;
    to->ecn_map_id                   = from->ecn_map_id;
    to->class_id                     = from->class_id;
    to->egress_class_id              = from->egress_class_id;
    to->inlif_counting_profile       = from->inlif_counting_profile;
    to->egress_tunnel_label          = from->egress_tunnel_label;
    to->nof_service_tags             = from->nof_service_tags;
    to->ingress_qos_model            = from->ingress_qos_model;
    to->context_label                = from->context_label;
    to->ingress_if                   = from->ingress_if;
    to->port_group                   = from->port_group;

    return;
}

int bcm_compat6519_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6519_mpls_port_t *mpls_port)
{
    int rv;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (NULL != mpls_port) {
        /* Create from heap to avoid the stack to bloat */
        new_mpls_port = sal_alloc(sizeof(bcm_mpls_port_t), "New mpls port");
        if (NULL == new_mpls_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mpls_port_t(mpls_port, new_mpls_port);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);
    if (NULL != new_mpls_port) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_mpls_port_t(new_mpls_port, mpls_port);
        /* Deallocate memory */
        sal_free(new_mpls_port);
    }

    return rv;
}

int bcm_compat6519_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6519_mpls_port_t *mpls_port)
{
    int rv;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (NULL != mpls_port) {
        /* Create from heap to avoid the stack to bloat */
        new_mpls_port = sal_alloc(sizeof(bcm_mpls_port_t), "New mpls port");
        if (NULL == new_mpls_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mpls_port_t(mpls_port, new_mpls_port);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);
    if (NULL != new_mpls_port) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_mpls_port_t(new_mpls_port, mpls_port);
        /* Deallocate memory */
        sal_free(new_mpls_port);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_mpls_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      port_max - (IN) Maximum number of ports in array
 *      port_array - (OUT) Array of MPLS ports
 *      port_count - (OUT) Number of ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6519_mpls_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_mpls_port_t *)
                         sal_alloc(port_max * sizeof(bcm_mpls_port_t),
                         "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get_all(unit, vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory */
    sal_free(new_port_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l3_host_t(
    bcm_compat6519_l3_host_t *from,
    bcm_l3_host_t *to)
{
    int i = 0;

    bcm_l3_host_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
	to->l3a_mtu = from->l3a_mtu;
}

/*
 * Function:
 *      _bcm_compat6519out_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from 6.5.19+ to <=6.5.19
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_l3_host_t(
    bcm_l3_host_t *from,
    bcm_compat6519_l3_host_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
}

int bcm_compat6519_l3_host_find(
    int unit,
    bcm_compat6519_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_find(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_add(
    int unit,
    bcm_compat6519_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_add(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_delete(
    int unit,
    bcm_compat6519_l3_host_t *ip_addr)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_delete_by_interface(
    int unit,
    bcm_compat6519_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_delete_all(
    int unit,
    bcm_compat6519_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_attach(
    int unit,
    bcm_compat6519_l3_host_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_detach(
    int unit,
    bcm_compat6519_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_counter_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_get(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                           counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_counter_set(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_set(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_host_stat_id_get(
    int unit,
    bcm_compat6519_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l3_route_t(
    bcm_compat6519_l3_route_t *from,
    bcm_l3_route_t *to)
{
    int i = 0;

    bcm_l3_route_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
}

/*
 * Function:
 *      _bcm_compat6519out_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from 6.5.19+ to <=6.5.19
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_l3_route_t(
    bcm_l3_route_t *from,
    bcm_compat6519_l3_route_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
}

int bcm_compat6519_l3_host_delete_by_network(
    int unit,
    bcm_compat6519_l3_route_t *ip_addr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_network(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_add(
    int unit,
    bcm_compat6519_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_add(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_delete(
    int unit,
    bcm_compat6519_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_delete_by_interface(
    int unit,
    bcm_compat6519_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_delete_all(
    int unit,
    bcm_compat6519_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_get(
    int unit,
    bcm_compat6519_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_multipath_get(
    int unit,
    bcm_compat6519_l3_route_t *the_route,
    bcm_compat6519_l3_route_t *path_array,
    int max_path,
    int *path_count)
{
    int rv, i;
    bcm_l3_route_t *new_info = NULL;
    bcm_l3_route_t *new_paths = NULL;

    if (NULL != the_route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(the_route, new_info);
    }

    if (NULL != path_array) {
        /* Create from heap to avoid the stack to bloat */
        new_paths = sal_alloc((sizeof(bcm_l3_route_t) * max_path), "New Route");
        if (NULL == new_paths) {
            if (NULL != new_info) {
                sal_free(new_info);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_multipath_get(unit, new_info, new_paths, max_path,
                                    path_count);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, the_route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    if (NULL != new_paths) {
        for (i = 0; i < *path_count && i < max_path; i++) {
            /* Transform the entry from the new format to old one */
            _bcm_compat6519out_l3_route_t(&new_paths[i], &path_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_paths);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_get(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_get32(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_set(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_set32(
    int unit,
    bcm_compat6519_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_enable_set(
    int unit,
    bcm_compat6519_l3_route_t *route,
    int enable)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_enable_set(unit, new_info, enable);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_attach(
    int unit,
    bcm_compat6519_l3_route_t *route,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_detach(
    int unit,
    bcm_compat6519_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_counter_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_get(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                            counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_counter_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_set(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_multi_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_multi_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6519_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_stat_id_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6519_l3_route_find(
    int unit,
    bcm_compat6519_l3_host_t *host,
    bcm_compat6519_l3_route_t *route)
{
    int rv;
    bcm_l3_host_t *new_host = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != host) {
        /* Create from heap to avoid the stack to bloat */
        new_host = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_host) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_host_t(host, new_host);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_host) {
                sal_free(new_host);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_find(unit, new_host, new_route);

    if (NULL != new_host) {
        /* Deallocate memory*/
        sal_free(new_host);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

int bcm_compat6519_l3_subnet_route_find(
    int unit,
    bcm_compat6519_l3_route_t *input,
    bcm_compat6519_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_input = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != input) {
        /* Create from heap to avoid the stack to bloat */
        new_input = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_input) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(input, new_input);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_input) {
                sal_free(new_input);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_subnet_route_find(unit, new_input, new_route);

    if (NULL != new_input) {
        /* Deallocate memory*/
        sal_free(new_input);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_route_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_route_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 route information.
 *      value - (IN) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_route_flexctr_object_set(
    int unit,
    bcm_compat6519_l3_route_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_l3_route_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_route_t *)
                     sal_alloc(sizeof(bcm_l3_route_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_route_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_route_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 route information.
 *      value - (OUT) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_route_flexctr_object_get(
    int unit,
    bcm_compat6519_l3_route_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_l3_route_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_route_t *)
                     sal_alloc(sizeof(bcm_l3_route_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_route_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l3_ipmc_t
 * Purpose:
 *      Convert the bcm_l3_ipmc_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_ipmc_addr_t(
    bcm_compat6519_ipmc_addr_t *from,
    bcm_ipmc_addr_t *to)
{
    int i = 0;

    bcm_ipmc_addr_t_init(to);
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
	to->mtu = from->mtu;
}

/*
 * Function:
 *      _bcm_compat6519out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.19+ to <=6.5.19
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6519_ipmc_addr_t *to)
{
    int i = 0;

    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
	to->mtu = from->mtu;
}

int bcm_compat6519_ipmc_add(
    int unit,
    bcm_compat6519_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_find(
    int unit,
    bcm_compat6519_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_remove(
    int unit,
    bcm_compat6519_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_remove(unit, new_data);

    if (NULL != new_data) {
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_attach(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_attach(unit, new_data, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_detach(
    int unit,
    bcm_compat6519_ipmc_addr_t *info)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_detach(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_counter_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_get(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_sync_get(unit, new_data, stat, num_entries,
                                        counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_counter_set(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_set(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_multi_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_multi_set(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_stat_id_get(
    int unit,
    bcm_compat6519_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_id_get(unit, new_data, stat, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_config_add(
    int unit,
    bcm_compat6519_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_config_find(
    int unit,
    bcm_compat6519_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6519_ipmc_config_remove(
    int unit,
    bcm_compat6519_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_remove(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l3_egress_t(
    bcm_compat6519_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i1 = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac_addr[i1] = from->mac_addr[i1];
    }
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    sal_memcpy(&to->vntag, &from->vntag, sizeof(bcm_vntag_t));
    to->vntag_action = from->vntag_action;
    sal_memcpy(&to->etag, &from->etag, sizeof(bcm_etag_t));
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_addr[i1] = from->src_mac_addr[i1];
    }
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
    to->nat_realm_id = from->nat_realm_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6519out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6519_l3_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac_addr[i1] = from->mac_addr[i1];
    }
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    sal_memcpy(&to->vntag, &from->vntag, sizeof(bcm_vntag_t));
    to->vntag_action = from->vntag_action;
    sal_memcpy(&to->etag, &from->etag, sizeof(bcm_etag_t));
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_addr[i1] = from->src_mac_addr[i1];
    }
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
    to->nat_realm_id = from->nat_realm_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
}

/*
 * Function:
 *      bcm_compat6519_l3_egress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_REPLACE: replace existing. BCM_L3_WITH_ID: intf argument is given.
 *      egr - (INOUT) (IN/OUT) Egress forwarding destination.
 *      if_id - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6519_l3_egress_t *egr,
    bcm_if_t *if_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      intf - (IN) L3 interface ID pointing to Egress object.
 *      egr - (OUT) Egress forwarding path properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      egr - (IN) Egress forwarding path properties.
 *      intf - (OUT) L3 interface ID pointing to Egress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_egress_find(
    int unit,
    bcm_compat6519_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);


    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_egress_allocation_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_allocation_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN)
 *      egr - (INOUT)
 *      nof_members - (IN)
 *      if_id - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6519_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_allocation_get(unit, flags, new_egr, nof_members, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_failover_egress_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_failover_egr = NULL;

    if (failover_egr != NULL) {
        new_failover_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New failover_egr");
        if (new_failover_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_failover_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_failover_egr = NULL;

    if (failover_egr != NULL) {
        new_failover_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New failover_egr");
        if (new_failover_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l3_ingress_t(
    bcm_compat6519_l3_ingress_t *from,
    bcm_l3_ingress_t *to)
{
    bcm_l3_ingress_t_init(to);
    to->flags = from->flags;
    to->vrf = from->vrf;
    to->urpf_mode = from->urpf_mode;
    to->intf_class = from->intf_class;
    to->ipmc_intf_id = from->ipmc_intf_id;
    to->qos_map_id = from->qos_map_id;
    to->ip4_options_profile_id = from->ip4_options_profile_id;
    to->nat_realm_id = from->nat_realm_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    to->intf_class_route_disable = from->intf_class_route_disable;
    to->oam_default_profile = from->oam_default_profile;
    to->hash_layers_disable = from->hash_layers_disable;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6519out_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_l3_ingress_t(
    bcm_l3_ingress_t *from,
    bcm_compat6519_l3_ingress_t *to)
{
    to->flags = from->flags;
    to->vrf = from->vrf;
    to->urpf_mode = from->urpf_mode;
    to->intf_class = from->intf_class;
    to->ipmc_intf_id = from->ipmc_intf_id;
    to->qos_map_id = from->qos_map_id;
    to->ip4_options_profile_id = from->ip4_options_profile_id;
    to->nat_realm_id = from->nat_realm_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    to->intf_class_route_disable = from->intf_class_route_disable;
    to->oam_default_profile = from->oam_default_profile;
    to->hash_layers_disable = from->hash_layers_disable;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      bcm_compat6519_l3_ingress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ing_intf - (IN) Egress forwarding destination.
 *      intf_id - (INOUT) (IN/OUT) L3 Ingress Interface ID pointing to L3 IIF object. This is an IN argument if either BCM_L3_INGRESS_REPLACE or BCM_L3_INGRESS_WITH_ID are given in flags.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_ingress_create(
    int unit,
    bcm_compat6519_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (ing_intf != NULL) {
        new_ing_intf = (bcm_l3_ingress_t *)
                     sal_alloc(sizeof(bcm_l3_ingress_t),
                     "New ing_intf");
        if (new_ing_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_ingress_t(ing_intf, new_ing_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_create(unit, new_ing_intf, intf_id);


    /* Deallocate memory*/
    sal_free(new_ing_intf);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_ingress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      ing_intf - (OUT) Ingress Interface object properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6519_l3_ingress_t *ing_intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (ing_intf != NULL) {
        new_ing_intf = (bcm_l3_ingress_t *)
                     sal_alloc(sizeof(bcm_l3_ingress_t),
                     "New ing_intf");
        if (new_ing_intf == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_get(unit, intf, new_ing_intf);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l3_ingress_t(new_ing_intf, ing_intf);

    /* Deallocate memory*/
    sal_free(new_ing_intf);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l3_ingress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ing_intf - (IN) Ingress Interface object properties.
 *      intf_id - (OUT) L3-IIF ID pointing to Ingress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l3_ingress_find(
    int unit,
    bcm_compat6519_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t *new_ing_intf = NULL;

    if (ing_intf != NULL) {
        new_ing_intf = (bcm_l3_ingress_t *)
                     sal_alloc(sizeof(bcm_l3_ingress_t),
                     "New ing_intf");
        if (new_ing_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l3_ingress_t(ing_intf, new_ing_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_find(unit, new_ing_intf, intf_id);


    /* Deallocate memory*/
    sal_free(new_ing_intf);

    return rv;
}


#endif /* INCLUDE_L3 */

/*
 * Function:
 *   _bcm_compat6519in_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_compat6519_switch_pkt_protocol_match_t datatype from
 *   <=6.5.19 to SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_switch_pkt_protocol_match_t(
    bcm_compat6519_switch_pkt_protocol_match_t *from,
    bcm_switch_pkt_protocol_match_t *to)
{
    bcm_switch_pkt_protocol_match_t_init(to);
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    to->vfp_opaque_ctrl_id               = from->vfp_opaque_ctrl_id;
    to->vfp_opaque_ctrl_id_mask          = from->vfp_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id        = from->vlan_xlate_opaque_ctrl_id;
    to->vlan_xlate_opaque_ctrl_id_mask   = from->vlan_xlate_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id_1      = from->vlan_xlate_opaque_ctrl_id_1;
    to->vlan_xlate_opaque_ctrl_id_1_mask = from->vlan_xlate_opaque_ctrl_id_1_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   _bcm_compat6519out_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_protocol_match_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_switch_pkt_protocol_match_t(
    bcm_switch_pkt_protocol_match_t *from,
    bcm_compat6519_switch_pkt_protocol_match_t *to)
{
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    to->vfp_opaque_ctrl_id               = from->vfp_opaque_ctrl_id;
    to->vfp_opaque_ctrl_id_mask          = from->vfp_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id        = from->vlan_xlate_opaque_ctrl_id;
    to->vlan_xlate_opaque_ctrl_id_mask   = from->vlan_xlate_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id_1      = from->vlan_xlate_opaque_ctrl_id_1;
    to->vlan_xlate_opaque_ctrl_id_1_mask = from->vlan_xlate_opaque_ctrl_id_1_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   bcm_compat6519_switch_pkt_protocol_control_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options flags.
 *   match          - (IN) Protocol match structure.
 *   action         - (IN) Packet control action.
 *   priority       - (IN) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6519_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6519in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_add(unit, options, new_match,
                                             action, priority);

    if (rv >= 0) {
        _bcm_compat6519out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_switch_pkt_protocol_control_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 *   action         - (OUT) Packet control action.
 *   priority       - (OUT) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6519_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6519in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_get(unit, new_match, action, priority);

    if (rv >= 0) {
        _bcm_compat6519out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_switch_pkt_protocol_control_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6519_switch_pkt_protocol_match_t *match)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_delete(unit, new_match);

    if (rv >= 0) {
        _bcm_compat6519out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_l2_addr_distribute_t
 * Purpose:
 *      Convert the bcm_l2_addr_distribute_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l2_addr_distribute_t(
    bcm_compat6519_l2_addr_distribute_t *from,
    bcm_l2_addr_distribute_t *to)
{
    bcm_l2_addr_distribute_t_init(to);
    to->flags = from->flags;
    to->vid = from->vid;
}

/*
 * Function:
 *      _bcm_compat6519out_l2_addr_distribute_t
 * Purpose:
 *      Convert the bcm_l2_addr_distribute_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_l2_addr_distribute_t(
    bcm_l2_addr_distribute_t *from,
    bcm_compat6519_l2_addr_distribute_t *to)
{
    to->flags = from->flags;
    to->vid = from->vid;
}

/*
 * Function:
 *      bcm_compat6519_l2_addr_msg_distribute_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_msg_distribute_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      distribution - (IN) Learning events distribution
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2_addr_msg_distribute_set(
    int unit,
    bcm_compat6519_l2_addr_distribute_t *distribution)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_distribute_t *new_distribution = NULL;

    if (distribution != NULL) {
        new_distribution = (bcm_l2_addr_distribute_t *)
                     sal_alloc(sizeof(bcm_l2_addr_distribute_t),
                     "New distribution");
        if (new_distribution == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_distribute_t(distribution, new_distribution);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_msg_distribute_set(unit, new_distribution);


    /* Deallocate memory*/
    sal_free(new_distribution);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2_addr_msg_distribute_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_msg_distribute_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      distribution - (INOUT) (IN/OUT) Learning events distribution
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2_addr_msg_distribute_get(
    int unit,
    bcm_compat6519_l2_addr_distribute_t *distribution)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_distribute_t *new_distribution = NULL;

    if (distribution != NULL) {
        new_distribution = (bcm_l2_addr_distribute_t *)
                     sal_alloc(sizeof(bcm_l2_addr_distribute_t),
                     "New distribution");
        if (new_distribution == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_distribute_t(distribution, new_distribution);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_msg_distribute_get(unit, new_distribution);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_distribute_t(new_distribution, distribution);

    /* Deallocate memory*/
    sal_free(new_distribution);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_mirror_destination_t(
    bcm_compat6519_mirror_destination_t *from,
    bcm_mirror_destination_t *to)
{
    bcm_mirror_destination_t_init(to);
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->ipfix_version = from->ipfix_version;
    to->psamp_epoch = from->psamp_epoch;
    to->cosq = from->cosq;
    to->cfi = from->cfi;
    to->timestamp_mode = from->timestamp_mode;
    to->multi_dip_group = from->multi_dip_group;
    to->drop_group_bmp = from->drop_group_bmp;
    to->second_pass_src_port = from->second_pass_src_port;
    to->encap_truncate_mode = from->encap_truncate_mode;
    to->encap_truncate_profile_id = from->encap_truncate_profile_id;
}

/*
 * Function:
 *   _bcm_compat6519out_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_mirror_destination_t(
    bcm_mirror_destination_t *from,
    bcm_compat6519_mirror_destination_t *to)
{
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->ipfix_version = from->ipfix_version;
    to->psamp_epoch = from->psamp_epoch;
    to->cosq = from->cosq;
    to->cfi = from->cfi;
    to->timestamp_mode = from->timestamp_mode;
    to->multi_dip_group = from->multi_dip_group;
    to->drop_group_bmp = from->drop_group_bmp;
    to->second_pass_src_port = from->second_pass_src_port;
    to->encap_truncate_mode = from->encap_truncate_mode;
    to->encap_truncate_profile_id = from->encap_truncate_profile_id;
}

/*
 * Function:
 *   bcm_compat6519_mirror_destination_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   mirror_dest    - (INOUT) Mirrored destination and encapsulation.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_mirror_destination_create(
    int unit,
    bcm_compat6519_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6519out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_mirror_destination_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_get.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   mirror_dest_id   - (IN) Mirrored destination ID.
 *   mirror_dest      - (INOUT) Matching Mirrored destination descriptor.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6519_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6519out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_flowtracker_tracking_param_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_tracking_param_info_t datatype
 *      from <=6.5.19 to SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_tracking_param_info_t(
    bcm_compat6519_flowtracker_tracking_param_info_t *from,
    bcm_flowtracker_tracking_param_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->check_id                 = from->check_id;
    to->src_ts                   = from->src_ts;
    to->udf_id                   = from->udf_id;
    sal_memcpy(to->mask, from->mask, BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    sal_memcpy(to->tracking_data, from->tracking_data,
                    BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    to->direction                = bcmFlowtrackerFlowDirectionNone;
    to->custom_id                = 0;
    return;
}

/*
 * Function:
 *      _bcm_compat6519out_flowtracker_tracking_param_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_tracking_param_info_t datatype from
 *      6.5.19+ to SDK <=6.5.19
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_tracking_param_info_t(
    bcm_flowtracker_tracking_param_info_t *from,
    bcm_compat6519_flowtracker_tracking_param_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->check_id                 = from->check_id;
    to->src_ts                   = from->src_ts;
    to->udf_id                   = from->udf_id;
    sal_memcpy(to->mask, from->mask, BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    sal_memcpy(to->tracking_data, from->tracking_data,
                    BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    return;
}

/*
 * Function:
 *      _bcm_compat6519in_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype
 *      from <=6.5.19 to SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_check_action_info_t(
    bcm_compat6519_flowtracker_check_action_info_t *from,
    bcm_flowtracker_check_action_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    to->weight                   = from->weight;
    to->direction                = bcmFlowtrackerFlowDirectionNone;
    to->custom_id                = 0;
    return;
}

/*
 * Function:
 *      _bcm_compat6519out_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype from
 *      6.5.19+ to SDK <=6.5.19
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_check_action_info_t(
    bcm_flowtracker_check_action_info_t *from,
    bcm_compat6519_flowtracker_check_action_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    return;
}

/*
 * Function:
 *      _bcm_compat6519in_flowtracker_group_stat_t
 * Purpose:
 *      Convert the bcm_flowtracker_group_stat_t datatype
 *      from <=6.5.19 to SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_group_stat_t(
    bcm_compat6519_flowtracker_group_stat_t *from,
    bcm_flowtracker_group_stat_t *to)
{
    COMPILER_64_COPY(to->flow_missed_count, from->flow_missed_count);
    COMPILER_64_COPY(to->flow_missed_count, from->flow_missed_count);
    COMPILER_64_COPY(to->flow_aged_out_count, from->flow_aged_out_count);
    COMPILER_64_COPY(to->flow_learnt_count, from->flow_learnt_count);
    COMPILER_64_COPY(to->flow_meter_exceeded_count, from->flow_meter_exceeded_count);
    COMPILER_64_SET(to->flow_start_count, 0, 0);
    COMPILER_64_SET(to->flow_end_count, 0, 0);
    return;
}

/*
 * Function:
 *      _bcm_compat6519out_flowtracker_group_stat_t
 * Purpose:
 *      Convert the bcm_flowtracker_group_stat_t datatype from
 *      6.5.19+ to SDK <=6.5.19
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_group_stat_t(
    bcm_flowtracker_group_stat_t *from,
    bcm_compat6519_flowtracker_group_stat_t *to)
{
    COMPILER_64_COPY(to->flow_missed_count, from->flow_missed_count);
    COMPILER_64_COPY(to->flow_missed_count, from->flow_missed_count);
    COMPILER_64_COPY(to->flow_aged_out_count, from->flow_aged_out_count);
    COMPILER_64_COPY(to->flow_learnt_count, from->flow_learnt_count);
    COMPILER_64_COPY(to->flow_meter_exceeded_count, from->flow_meter_exceeded_count);
    return;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_tracking_params_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   num_tracking_params - (IN)  Numer of tracking params
 *   list_of_tracking_params - (IN) List of tracking params
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_tracking_params_set(
    int unit,
    bcm_flowtracker_group_t id,
    int num_tracking_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_tp_list = NULL;

    if (list_of_tracking_params != NULL) {
        new_tp_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_tracking_params), "new tracking param list");
        if (new_tp_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_tracking_params; i++) {
            _bcm_compat6519in_flowtracker_tracking_param_info_t(
                    &list_of_tracking_params[i], &new_tp_list[i]);
        }
    }

    rv = bcm_flowtracker_group_tracking_params_set(unit, id,
            num_tracking_params, new_tp_list);

    if (new_tp_list != NULL) {
        sal_free(new_tp_list);
    }

    return rv;

}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_tracking_params_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   max_size          - (IN)    size of list_of_tracking_params
 *   list_of_tracking_params - (OUT) List of tracking params
 *   list_size         - (OUT)   Actual number of entries
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_tracking_params_get(
    int unit,
    bcm_flowtracker_group_t id,
    int max_size,
    bcm_compat6519_flowtracker_tracking_param_info_t *list_of_tracking_params,
    int *list_size)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_tp_list = NULL;

    if (list_of_tracking_params != NULL) {
        new_tp_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    max_size), "new tracking param list");
        if (new_tp_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < max_size; i++) {
            _bcm_compat6519in_flowtracker_tracking_param_info_t(
                    &list_of_tracking_params[i], &new_tp_list[i]);
        }
    }

    rv = bcm_flowtracker_group_tracking_params_get(unit, id,
            max_size, new_tp_list, list_size);

    if (new_tp_list != NULL) {
        for (i = 0; i < *list_size; i++) {
            _bcm_compat6519out_flowtracker_tracking_param_info_t(
                    &new_tp_list[i], &list_of_tracking_params[i]);
        }
        sal_free(new_tp_list);
    }

    return rv;

}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_stat_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_stat_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   group_stats       - (IN)    Group stats
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_stat_set(
    int unit,
    bcm_flowtracker_group_t group_id,
    bcm_compat6519_flowtracker_group_stat_t *group_stats)
{
    int rv;
    bcm_flowtracker_group_stat_t *group_stats_new = NULL;

    if (group_stats != NULL) {
        group_stats_new = sal_alloc(sizeof(bcm_flowtracker_group_stat_t),
                "new group stats");
        if (group_stats_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flowtracker_group_stat_t(
                group_stats, group_stats_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_stat_set(unit, group_id, group_stats_new);

    if (group_stats_new != NULL) {
        _bcm_compat6519out_flowtracker_group_stat_t(
                group_stats_new, group_stats);
        sal_free(group_stats_new);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_stat_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_stat_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   group_stats       - (OUT)   Group stats
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_stat_get(
    int unit,
    bcm_flowtracker_group_t group_id,
    bcm_compat6519_flowtracker_group_stat_t *group_stats)
{
    int rv;
    bcm_flowtracker_group_stat_t *group_stats_new = NULL;

    if (group_stats != NULL) {
        group_stats_new = sal_alloc(sizeof(bcm_flowtracker_group_stat_t),
                "new group stats");
        if (group_stats_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flowtracker_group_stat_t(
                group_stats, group_stats_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_stat_get(unit, group_id, group_stats_new);

    if (group_stats_new != NULL) {
        _bcm_compat6519out_flowtracker_group_stat_t(
                group_stats_new, group_stats);
        sal_free(group_stats_new);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_check_action_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (IN)    flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6519_flowtracker_check_action_info_t info)
{
    int rv;
    bcm_flowtracker_check_action_info_t info_new;

    /* Transform the entry from the old format to new one */
    _bcm_compat6519in_flowtracker_check_action_info_t(&info, &info_new);

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_set(unit, check_id, info_new);

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_check_action_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (OUT)   flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6519_flowtracker_check_action_info_t *info)
{
    int rv;
    bcm_flowtracker_check_action_info_t *info_new = NULL;

    if (info != NULL) {
        info_new = sal_alloc(sizeof(bcm_flowtracker_check_action_info_t),
                "New check action info");
        if (info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flowtracker_check_action_info_t(info, info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_get(unit, check_id, info_new);

    if (info_new != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_flowtracker_check_action_info_t(info_new, info);

        sal_free(info_new);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_user_entry_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_add
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   flow_group_id           - (IN) Flowtracker group Id.
 *   options                 - (IN) Options to create user entry.
 *   num_user_entry_params   - (IN) Number of user entry params.
 *   user_entry_param_list   - (IN) List of user entry params.
 *   entry_handle            - (IN) User entry handle.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_ep_list = NULL;

    if (user_entry_param_list != NULL) {
        new_ep_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_user_entry_params), "new tracking param list");
        if (new_ep_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_user_entry_params; i++) {
            _bcm_compat6519in_flowtracker_tracking_param_info_t(
                    &user_entry_param_list[i], &new_ep_list[i]);
        }
    }

    rv = bcm_flowtracker_user_entry_add(unit, flow_group_id,
            options, num_user_entry_params, new_ep_list, entry_handle);

    if (new_ep_list != NULL) {
        sal_free(new_ep_list);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_user_entry_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_get
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   entry_handle            - (IN) User entry handle.
 *   num_user_entry_params   - (IN) Maximum Number of user entry params.
 *   user_entry_param_list   - (OUT) List of user entry params.
 *   actual_user_entry_params   - (OUT) Number of user entry params.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_compat6519_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_ep_list = NULL;

    if (user_entry_param_list != NULL) {
        new_ep_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_user_entry_params), "new tracking param list");
        if (new_ep_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_user_entry_params; i++) {
            _bcm_compat6519in_flowtracker_tracking_param_info_t(
                    &user_entry_param_list[i], &new_ep_list[i]);
        }
    }

    rv = bcm_flowtracker_user_entry_get(unit, entry_handle,
            num_user_entry_params, new_ep_list, actual_user_entry_params);

    if (new_ep_list != NULL) {
        for (i = 0; i < *actual_user_entry_params; i++) {
            _bcm_compat6519out_flowtracker_tracking_param_info_t(
                    &new_ep_list[i], &user_entry_param_list[i]);
        }
        sal_free(new_ep_list);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_flowtracker_export_trigger_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_export_trigger_info_t datatype
 *      from <=6.5.19 to SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_export_trigger_info_t(
    bcm_compat6519_flowtracker_export_trigger_info_t *from,
    bcm_flowtracker_export_trigger_info_t *to)
{
    sal_memcpy(to->trigger_bmp, from->trigger_bmp,
                sizeof(SHR_BITDCL) * (_SHR_BITDCLSIZE(6)));
    to->interval_usecs = from->interval_usecs;
    to->flags = 0;
    return;
}

/*
 * Function:
 *      _bcm_compat6519out_flowtracker_export_trigger_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_export_trigger_info_t datatype from
 *      6.5.19+ to SDK <=6.5.19
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_export_trigger_info_t(
    bcm_flowtracker_export_trigger_info_t *from,
    bcm_compat6519_flowtracker_export_trigger_info_t *to)
{
    sal_memcpy(to->trigger_bmp, from->trigger_bmp,
                sizeof(SHR_BITDCL) * (_SHR_BITDCLSIZE(6)));
    to->interval_usecs = from->interval_usecs;
    return;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_export_trigger_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_export_trigger_set
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   id                      - (IN) Flowtracker Group Id
 *   export_trigger_info     - (IN) Export triggers info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_export_trigger_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6519_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int rv;
    bcm_flowtracker_export_trigger_info_t *trig_info_new = NULL;

    if (export_trigger_info != NULL) {
        trig_info_new = sal_alloc(sizeof(bcm_flowtracker_export_trigger_info_t),
                "new export trigger info");
        if (trig_info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flowtracker_export_trigger_info_t(
                export_trigger_info, trig_info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_export_trigger_set(unit, id, trig_info_new);

    if (trig_info_new != NULL) {
        _bcm_compat6519out_flowtracker_export_trigger_info_t(
                trig_info_new, export_trigger_info);
        sal_free(trig_info_new);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_group_export_trigger_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_export_trigger_get
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   id                      - (IN) Flowtracker Group Id
 *   export_trigger_info     - (OUT) Export triggers info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_group_export_trigger_get(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_compat6519_flowtracker_export_trigger_info_t *export_trigger_info)
{
    int rv;
    bcm_flowtracker_export_trigger_info_t *trig_info_new = NULL;

    if (export_trigger_info != NULL) {
        trig_info_new = sal_alloc(sizeof(bcm_flowtracker_export_trigger_info_t),
                "new export trigger info");
        if (trig_info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flowtracker_export_trigger_info_t(
                export_trigger_info, trig_info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_export_trigger_get(unit, id, trig_info_new);

    if (trig_info_new != NULL) {
        _bcm_compat6519out_flowtracker_export_trigger_info_t(
                trig_info_new, export_trigger_info);
        sal_free(trig_info_new);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_collector_info_t(
        bcm_compat6519_collector_info_t *from,
        bcm_collector_info_t *to)
{
    bcm_collector_info_t_init(to);
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_compat6519_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    sal_memcpy(&to->src_ts, &from->src_ts, sizeof(bcm_collector_timestamp_source_t));
    to->max_records_reserve = 16;
}

/*
 * Function:
 *   _bcm_compat6519out_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_collector_info_t(
        bcm_collector_info_t *from,
        bcm_compat6519_collector_info_t *to)
{
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_compat6519_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    sal_memcpy(&to->src_ts, &from->src_ts, sizeof(bcm_collector_timestamp_source_t));
}

/*
 * Function:
 *   bcm_compat6519_collector_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_create
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Collector create options
 *   id             - (INOUT) Collector Id.
 *   collector_info - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6519_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_create(unit, options, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6519out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_collector_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_get
 * Parameters:
 *   unit    - (IN) BCM device number.
 *   id      - (IN) Collector Id.
 *   info    - (OUT) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_collector_get(
        int unit,
        bcm_collector_t collector_id,
        bcm_compat6519_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_get(unit, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6519out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *  _bcm_compat6519in_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from <=6.5.19 to
 *  SDK 6.5.19+
 * Parameters:
 *  from        - (IN) The <=6.5.19 version of the datatype
 *  to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_l2_addr_t(bcm_compat6519_l2_addr_t *from,
                            bcm_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
}

/*
 * Function:
 *  _bcm_compat6519out_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from 6.5.19+ to
 *  <=6.5.19
 * Parameters:
 *  from        - (IN) The SDK 6.5.19+ version of the datatype
 *  to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *  Nothing
 */
STATIC void
_bcm_compat6519out_l2_addr_t(bcm_l2_addr_t *from,
                             bcm_compat6519_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
}

/*
 * Function: bcm_compat6519_l2_addr_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_add(int unit,
                           bcm_compat6519_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_addr_multi_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  count  - (IN) count of l2 address to add.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_multi_add(int unit,
                                 bcm_compat6519_l2_addr_t *l2addr,
                                 int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_addr_multi_delete
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) L2 address entry with old format.
 *  count  - (IN) count of l2 address to delete.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_multi_delete(int unit,
                                    bcm_compat6519_l2_addr_t *l2addr,
                                    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_addr_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  mac_addr - (IN)  Input mac address entry to search.
 *  vid      - (IN)  Input VLAN ID to search.
 *  l2addr   - (OUT) l2 address entry with old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_get(int unit,
                           bcm_mac_t mac_addr, bcm_vlan_t vid,
                           bcm_compat6519_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_conflict_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_conflict_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  addr     - (IN)  l2 address entry with old format.
 *  cf_array - (OUT) conflicting address array.
 *  cf_max   - (IN)  maximum conflicting address.
 *  cf_count - (OUT) actual count of conflicting address found.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_conflict_get(int unit, bcm_compat6519_l2_addr_t *addr,
                               bcm_compat6519_l2_addr_t *cf_array, int cf_max,
                               int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t naddr;
    bcm_l2_addr_t *ncf_array = NULL;
    int i = 0;

    if ((NULL != addr) && (NULL != cf_array) && (0 < cf_max)) {
        /* Create from heap to avoid the stack to bloat */
        ncf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                    "compat6519_l2_conflict_get");
        if (NULL == ncf_array) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6519in_l2_addr_t(addr, &naddr);

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, &naddr, ncf_array, cf_max, cf_count);

    if (rv >= 0) {
        for(i = 0; i < *cf_count; i++) {
            _bcm_compat6519out_l2_addr_t(&ncf_array[i], &cf_array[i]);
        }
    }

    /* Deallocate memory*/
    sal_free(ncf_array);
    return rv;
}

/*
 * Function: bcm_compat6519_l2_replace
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace.
 *
 * Parameters:
 *  unit       - (IN) BCM device number.
 *  flags      - (IN) L2 replace flags.
 *  match_addr - (IN) L2 address entry with old format.
 *  new_module - (IN) module id.
 *  new_port   - (IN) port.
 *  new_trunk  - (IN) trunk group id.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_replace(int unit, uint32 flags,
                          bcm_compat6519_l2_addr_t *match_addr,
                          bcm_module_t new_module,
                          bcm_port_t new_port, bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;

    if (NULL != match_addr) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(match_addr, newmatch_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, newmatch_addr, new_module, new_port, new_trunk);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newmatch_addr, match_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_replace_match
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace_match.
 *
 * Parameters:
 *  unit              - (IN) BCM device number.
 *  match_addr        - (IN) l2 address entry with old format.
 *  mask_addr         - (IN) mask l2 address entry with old format.
 *  replace_addr      - (IN) replace l2 address entry in old format.
 *  replace_mask_addr - (IN) replace l2 mask address entry in old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_replace_match(int unit, uint32 flags,
                                bcm_compat6519_l2_addr_t *match_addr,
                                bcm_compat6519_l2_addr_t *mask_addr,
                                bcm_compat6519_l2_addr_t *replace_addr,
                                bcm_compat6519_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;
    bcm_l2_addr_t *newmask_addr = NULL;
    bcm_l2_addr_t *newreplace_addr = NULL;
    bcm_l2_addr_t *newreplace_mask_addr = NULL;

    if ((NULL != match_addr) && (NULL != mask_addr) &&
            (NULL != replace_addr) && (NULL != replace_mask_addr)) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        newmask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr mask");
        if (NULL == newmask_addr) {
            sal_free(newmatch_addr);
            return BCM_E_MEMORY;
        }

        newreplace_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr");
        if (NULL == newreplace_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            return BCM_E_MEMORY;
        }

        newreplace_mask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr mask");
        if (NULL == newreplace_mask_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            sal_free(newreplace_addr);
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(match_addr, newmatch_addr);
        _bcm_compat6519in_l2_addr_t(mask_addr, newmask_addr);
        _bcm_compat6519in_l2_addr_t(replace_addr, newreplace_addr);
        _bcm_compat6519in_l2_addr_t(replace_mask_addr, newreplace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, newmatch_addr, newmask_addr,
            newreplace_addr, newreplace_mask_addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newmatch_addr, match_addr);
    _bcm_compat6519out_l2_addr_t(newmask_addr, mask_addr);
    _bcm_compat6519out_l2_addr_t(newreplace_addr, replace_addr);
    _bcm_compat6519out_l2_addr_t(newreplace_mask_addr, replace_mask_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);
    sal_free(newmask_addr);
    sal_free(newreplace_addr);
    sal_free(newreplace_mask_addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_stat_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_stat_get(int unit,
                           bcm_compat6519_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_stat_get32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get32.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_stat_get32(int unit,
                             bcm_compat6519_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_stat_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_stat_set(int unit,
                           bcm_compat6519_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_stat_set32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set32.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_stat_set32(int unit,
                             bcm_compat6519_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_stat_enable_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_enable_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  enable - (IN) enable value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_stat_enable_set(int unit,
                                  bcm_compat6519_l2_addr_t *l2addr,
                                  int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, newl2addr, enable);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_addr_by_struct_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_by_struct_get.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (INOUT) l2 address entry with old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_by_struct_get(int unit, bcm_compat6519_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_get(unit, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6519_l2_addr_by_struct_delete
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_by_struct_delete.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6519_l2_addr_by_struct_delete(int unit, bcm_compat6519_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_delete(unit, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_flexctr_index_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_index_operation_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flexctr_index_operation_t(
    bcm_compat6519_flexctr_index_operation_t *from,
    bcm_flexctr_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
        to->object_id[i] = from->object_id[i];
    }
}

/*
 * Function:
 *   _bcm_compat6519out_flexctr_index_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_index_operation_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flexctr_index_operation_t(
    bcm_flexctr_index_operation_t *from,
    bcm_compat6519_flexctr_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
        to->object_id[i] = from->object_id[i];
    }
}

/*
 * Function:
 *   _bcm_compat6519in_flexctr_value_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_value_operation_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flexctr_value_operation_t(
    bcm_compat6519_flexctr_value_operation_t *from,
    bcm_flexctr_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
        to->object_id[i] = from->object_id[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6519out_flexctr_value_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_value_operation_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flexctr_value_operation_t(
    bcm_flexctr_value_operation_t *from,
    bcm_compat6519_flexctr_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_COMPAT6519_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
        to->object_id[i] = from->object_id[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6519in_flexctr_trigger_t
 * Purpose:
 *   Convert the bcm_flexctr_trigger_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flexctr_trigger_t(
    bcm_compat6519_flexctr_trigger_t *from,
    bcm_flexctr_trigger_t *to)
{
    to->trigger_type = from->trigger_type;
    to->period_num = from->period_num;
    to->interval = from->interval;
    to->object = from->object;
    to->object_value_start = from->object_value_start;
    to->object_value_stop = from->object_value_stop;
    to->object_value_mask = from->object_value_mask;
    to->interval_shift = from->interval_shift;
    to->interval_size = from->interval_size;
    to->object_id = from->object_id;
}

/*
 * Function:
 *   _bcm_compat6519out_flexctr_trigger_t
 * Purpose:
 *   Convert the bcm_flexctr_trigger_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flexctr_trigger_t(
    bcm_flexctr_trigger_t *from,
    bcm_compat6519_flexctr_trigger_t *to)
{
    to->trigger_type = from->trigger_type;
    to->period_num = from->period_num;
    to->interval = from->interval;
    to->object = from->object;
    to->object_value_start = from->object_value_start;
    to->object_value_stop = from->object_value_stop;
    to->object_value_mask = from->object_value_mask;
    to->interval_shift = from->interval_shift;
    to->interval_size = from->interval_size;
    to->object_id = from->object_id;
}

/*
 * Function:
 *   _bcm_compat6519in_flexctr_action_t
 * Purpose:
 *   Convert the bcm_flexctr_action_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flexctr_action_t(
    bcm_compat6519_flexctr_action_t *from,
    bcm_flexctr_action_t *to)
{
    bcm_flexctr_action_t_init(to);
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    _bcm_compat6519in_flexctr_index_operation_t(&from->index_operation,
                                                &to->index_operation);
    _bcm_compat6519in_flexctr_value_operation_t(&from->operation_a,
                                                &to->operation_a);
    _bcm_compat6519in_flexctr_value_operation_t(&from->operation_b,
                                                &to->operation_b);
    _bcm_compat6519in_flexctr_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   _bcm_compat6519out_flexctr_action_t
 * Purpose:
 *   Convert the bcm_flexctr_action_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flexctr_action_t(
    bcm_flexctr_action_t *from,
    bcm_compat6519_flexctr_action_t *to)
{
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    _bcm_compat6519out_flexctr_index_operation_t(&from->index_operation,
                                                 &to->index_operation);
    _bcm_compat6519out_flexctr_value_operation_t(&from->operation_a,
                                                 &to->operation_a);
    _bcm_compat6519out_flexctr_value_operation_t(&from->operation_b,
                                                 &to->operation_b);
    _bcm_compat6519out_flexctr_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   bcm_compat6519_flexctr_action_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_action_create.
 * Parameters:
 *   unit              - (IN) Unit number.
 *   options           - (IN) BCM_FLEXCTR_OPTIONS_XXX options.
 *   action            - (IN) Flex counter action data structure.
 *   stat_counter_id   - (IN/OUT) Flex counter ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6519_flexctr_action_t *action,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexctr_action_t *)
                     sal_alloc(sizeof(bcm_flexctr_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flexctr_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_create(unit, options, new_action, stat_counter_id);

    if (rv >= 0) {
        _bcm_compat6519out_flexctr_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flexctr_action_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_action_get.
 * Parameters:
 *   unit              - (IN) Unit number.
 *   stat_counter_id   - (IN) Flex counter ID.
 *   action            - (OUT) Flex counter action data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6519_flexctr_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexctr_action_t *)
                     sal_alloc(sizeof(bcm_flexctr_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_flexctr_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_get(unit, stat_counter_id, new_action);

    if (rv >= 0) {
        _bcm_compat6519out_flexctr_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_mirror_payload_zero_offsets_t
 * Purpose:
 *   Convert the bcm_mirror_payload_zero_offsets_t datatype from <=6.5.19 to
 *   SDK 6.5.19+.
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_mirror_payload_zero_offsets_t(
    bcm_compat6519_mirror_payload_zero_offsets_t *from,
    bcm_mirror_payload_zero_offsets_t *to)
{
    bcm_mirror_payload_zero_offsets_t_init(to);
    to->l2_offset = from->l2_offset;
    to->l3_offset = from->l3_offset;
    to->udp_offset = from->udp_offset;
}

/*
 * Function:
 *   _bcm_compat6519out_mirror_payload_zero_offsets_t
 * Purpose:
 *   Convert the bcm_mirror_payload_zero_offsets_t datatype from 6.5.19+ to
 *   <=6.5.19.
 * Parameters:
 *   from     - (IN) The 6.5.19+ version of the datatype.
 *   to       - (OUT) The <=6.5.19 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_mirror_payload_zero_offsets_t(
    bcm_mirror_payload_zero_offsets_t *from,
    bcm_compat6519_mirror_payload_zero_offsets_t *to)
{
    to->l2_offset= from->l2_offset;
    to->l3_offset = from->l3_offset;
    to->udp_offset = from->udp_offset;
}

/*
 * Function:
 *   bcm_compat6519_mirror_payload_zero_profile_create
 * Purpose:
 *   Compatibility function for RPC call to
 *   bcm_mirror_payload_zero_profile_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   mirror_payload_offset_info - (IN) Mirror zero payload offset info.
 *   flags          - (IN) - To denote the module using the profile.
 *   profile_index  - (OUT) - Profile index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6519_mirror_payload_zero_profile_create(
        int unit,
        bcm_compat6519_mirror_payload_zero_offsets_t *mirror_payload_offset_info,
        uint32 flags,
        uint32 *profile_index)
{
    int rv = 0;
    bcm_mirror_payload_zero_offsets_t *new_config = NULL;

    if (mirror_payload_offset_info != NULL) {
        new_config = (bcm_mirror_payload_zero_offsets_t*)
            sal_alloc(sizeof(bcm_mirror_payload_zero_offsets_t),
                    " Mirror Payload Zero Offsets");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_mirror_payload_zero_offsets_t(
                mirror_payload_offset_info,
                new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_payload_zero_profile_create(unit,
                                                new_config,
                                                flags,
                                                profile_index);
    if (new_config != NULL) {
        _bcm_compat6519out_mirror_payload_zero_offsets_t(
                new_config,
                mirror_payload_offset_info);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_mirror_source_t
 * Purpose:
 *      Convert the bcm_mirror_source_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_mirror_source_t(
    bcm_compat6519_mirror_source_t *from,
    bcm_mirror_source_t *to)
{
    bcm_mirror_source_t_init(to);
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
    to->mod_profile_id = from->mod_profile_id;
    to->sample_profile_id = from->sample_profile_id;
}

/*
 * Function:
 *      bcm_compat6519_mirror_source_dest_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_source_dest_add(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_add(unit, new_source, mirror_dest_id);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_mirror_source_dest_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_source_dest_delete(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete(unit, new_source, mirror_dest_id);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_mirror_source_dest_delete_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6519_mirror_source_t *source)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete_all(unit, new_source);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_mirror_source_dest_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      array_size - (IN) Size of allocated entries in mirror_dest array
 *      mirror_dest - (OUT) Array for mirror destinations information
 *      count - (OUT) Actual number of mirror destinations
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_source_dest_get_all(
    int unit,
    bcm_compat6519_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_get_all(unit, new_source, array_size, mirror_dest, count);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_flowtracker_export_element_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_element_info_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_export_element_info_t(
        bcm_compat6519_flowtracker_export_element_info_t *from,
        bcm_flowtracker_export_element_info_t *to)
{
    to->flags = from->flags;
    to->element = from->element;
    to->data_size = from->data_size;
    to->info_elem_id = from->info_elem_id;
    to->check_id = from->check_id;
    to->direction = bcmFlowtrackerFlowDirectionNone;
    to->custom_id = 0;
}

/*
 * Function:
 *   _bcm_compat6519out_flowtracker_export_element_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_element_info_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_export_element_info_t(
        bcm_flowtracker_export_element_info_t *from,
        bcm_compat6519_flowtracker_export_element_info_t *to)
{
    to->flags = from->flags;
    to->element = from->element;
    to->data_size = from->data_size;
    to->info_elem_id = from->info_elem_id;
    to->check_id = from->check_id;
}

/*
 * Function:
 *      bcm_compat6519_flowtracker_export_template_validate
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_flowtracker_export_template_validate
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_compat6519_flowtracker_export_template_validate(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_in_export_elements,
        bcm_compat6519_flowtracker_export_element_info_t *in_export_elements,
        int max_out_export_elements,
        bcm_compat6519_flowtracker_export_element_info_t *out_export_elements,
        int *actual_out_export_elements)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_in_info = NULL;
    bcm_flowtracker_export_element_info_t *new_out_info = NULL;

    if ((in_export_elements != NULL) && (max_in_export_elements > 0)) {
        new_in_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              max_in_export_elements), "New in info");
        if (new_in_info == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < max_in_export_elements; iter++) {
            _bcm_compat6519in_flowtracker_export_element_info_t
                                        (&in_export_elements[iter],
                                         &new_in_info[iter]);
        }
    }
    if ((out_export_elements != NULL) && (max_out_export_elements != 0)) {
        new_out_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              max_out_export_elements), "New out info");
        if (new_out_info == NULL) {
            if (new_in_info != NULL) {
                sal_free(new_in_info);
            }
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < max_out_export_elements; iter++) {
            _bcm_compat6519in_flowtracker_export_element_info_t
                                        (&out_export_elements[iter],
                                         &new_out_info[iter]);
        }
    }

    rv = bcm_flowtracker_export_template_validate(unit, flow_group_id,
                    max_in_export_elements, new_in_info,
                    max_out_export_elements, new_out_info,
                    actual_out_export_elements);

    if (new_in_info != NULL) {
        sal_free(new_in_info);
    }

    if (rv >= 0) {
        if (new_out_info != NULL) {
            for (iter = 0; iter < *actual_out_export_elements; iter++) {
                _bcm_compat6519out_flowtracker_export_element_info_t
                                    (&new_out_info[iter],
                                     &out_export_elements[iter]);
            }
            sal_free(new_out_info);
        }
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_export_template_create
 * Purpose:
 *   Compatibility function for RPC call to
 *   bcm_flowtracker_export_template_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options
 *   id             - (INOUT) Template Id
 *   set_id         - (IN) Set id
 *   max_size       - (IN) Size of array 'list_of_export_elements'
 *   list_of_export_elements - (IN) Array of export element info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_export_template_create(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_compat6519_flowtracker_export_element_info_t *list_of_export_elements)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_info = NULL;

    if ((list_of_export_elements != NULL) && (num_export_elements > 0)) {
        new_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              num_export_elements), "New export elements info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < num_export_elements; iter++) {
            _bcm_compat6519in_flowtracker_export_element_info_t
                                        (&list_of_export_elements[iter],
                                         &new_info[iter]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_export_template_create(unit, options, id,
                                set_id, num_export_elements, new_info);

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_export_template_get
 * Purpose:
 *   Compatibility function for RPC call to
 *   bcm_flowtracker_export_template_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   id             - (IN) Template Id
 *   set_id         - (OUT) Set id
 *   max_size       - (IN) Size of array 'list_of_export_elements'
 *   list_of_export_elements - (OUT) Array of export element info
 *   list_size      - (OUT) Actual number of valid elements in
                           'list_of_export_elements'
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_export_template_get(
    int unit,
    bcm_flowtracker_export_template_t id,
    uint16 *set_id,
    int max_size,
    bcm_compat6519_flowtracker_export_element_info_t *list_of_export_elements,
    int *list_size)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_info = NULL;

    if ((list_of_export_elements != NULL) && (list_size != NULL) && (max_size > 0)) {
        new_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                                        max_size), "New export element info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_export_template_get(unit, id, set_id,
                                max_size, new_info, list_size);

    if (new_info != NULL) {
        for (iter = 0; iter < max_size; iter++) {
            _bcm_compat6519out_flowtracker_export_element_info_t
                (&new_info[iter],
                 &list_of_export_elements[iter]);
        }
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_mirror_truncate_length_profile_t
 * Purpose:
 *      Convert the bcm_mirror_truncate_length_profile_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_mirror_truncate_length_profile_t(
    bcm_compat6519_mirror_truncate_length_profile_t *from,
    bcm_mirror_truncate_length_profile_t *to)
{
    bcm_mirror_truncate_length_profile_t_init(to);
    to->adjustment_length = from->adjustment_length;
}

/*
 * Function:
 *      _bcm_compat6519out_mirror_truncate_length_profile_t
 * Purpose:
 *      Convert the bcm_mirror_truncate_length_profile_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_mirror_truncate_length_profile_t(
    bcm_mirror_truncate_length_profile_t *from,
    bcm_compat6519_mirror_truncate_length_profile_t *to)
{
    to->adjustment_length = from->adjustment_length;
}

/*
 * Function:
 *      bcm_compat6519_mirror_truncate_length_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_truncate_length_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Options flags.
 *      profile_id - (INOUT) Profile id.
 *      profile - (IN) Truncate length profile pointer.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_truncate_length_profile_create(
    int unit,
    uint32 options,
    int *profile_id,
    bcm_compat6519_mirror_truncate_length_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_truncate_length_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_truncate_length_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_truncate_length_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_mirror_truncate_length_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_truncate_length_profile_create(unit, options, profile_id, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_mirror_truncate_length_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_truncate_length_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile_id - (IN) Profile id.
 *      profile - (OUT) Truncate length profile pointer.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_mirror_truncate_length_profile_get(
    int unit,
    int profile_id,
    bcm_compat6519_mirror_truncate_length_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_truncate_length_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_truncate_length_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_truncate_length_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_truncate_length_profile_get(unit, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_mirror_truncate_length_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_oam_delay_t
 * Purpose:
 *      Convert the bcm_oam_delay_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_oam_delay_t(
    bcm_compat6519_oam_delay_t *from,
    bcm_oam_delay_t *to)
{
    int i1 = 0;

    bcm_oam_delay_t_init(to);
    to->flags = from->flags;
    to->delay_id = from->delay_id;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->period = from->period;
    sal_memcpy(&to->delay_min, &from->delay_min, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->delay_max, &from->delay_max, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->delay, &from->delay, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->txf, &from->txf, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->rxf, &from->rxf, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->txb, &from->txb, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->rxb, &from->rxb, sizeof(bcm_time_spec_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_pri_bitmap = from->pkt_pri_bitmap;
    to->int_pri = from->int_pri;
    to->rx_oam_packets = from->rx_oam_packets;
    to->tx_oam_packets = from->tx_oam_packets;
    to->timestamp_format = from->timestamp_format;
    for (i1 = 0; i1 < 6; i1++) {
        to->peer_da_mac_address[i1] = from->peer_da_mac_address[i1];
    }
    to->pm_id = from->pm_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_base_id[i1] = from->dm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_offset[i1] = from->dm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_byte_offset[i1] = from->dm_tx_update_lm_counter_byte_offset[i1];
    }
    to->dm_tx_update_lm_counter_size = from->dm_tx_update_lm_counter_size;
    for (i1 = 0; i1 < BCM_OAM_MAX_NUM_TLVS_FOR_DM; i1++) {
        sal_memcpy(&to->tlvs[i1], &from->tlvs[i1], sizeof(bcm_oam_tlv_t));
    }
}

/*
 * Function:
 *      _bcm_compat6519out_oam_delay_t
 * Purpose:
 *      Convert the bcm_oam_delay_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_oam_delay_t(
    bcm_oam_delay_t *from,
    bcm_compat6519_oam_delay_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->delay_id = from->delay_id;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->period = from->period;
    sal_memcpy(&to->delay_min, &from->delay_min, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->delay_max, &from->delay_max, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->delay, &from->delay, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->txf, &from->txf, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->rxf, &from->rxf, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->txb, &from->txb, sizeof(bcm_time_spec_t));
    sal_memcpy(&to->rxb, &from->rxb, sizeof(bcm_time_spec_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_pri_bitmap = from->pkt_pri_bitmap;
    to->int_pri = from->int_pri;
    to->rx_oam_packets = from->rx_oam_packets;
    to->tx_oam_packets = from->tx_oam_packets;
    to->timestamp_format = from->timestamp_format;
    for (i1 = 0; i1 < 6; i1++) {
        to->peer_da_mac_address[i1] = from->peer_da_mac_address[i1];
    }
    to->pm_id = from->pm_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_base_id[i1] = from->dm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_offset[i1] = from->dm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->dm_tx_update_lm_counter_byte_offset[i1] = from->dm_tx_update_lm_counter_byte_offset[i1];
    }
    to->dm_tx_update_lm_counter_size = from->dm_tx_update_lm_counter_size;
    for (i1 = 0; i1 < BCM_OAM_MAX_NUM_TLVS_FOR_DM; i1++) {
        sal_memcpy(&to->tlvs[i1], &from->tlvs[i1], sizeof(bcm_oam_tlv_t));
    }
}

/*
 * Function:
 *      bcm_compat6519_oam_delay_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_delay_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      delay_ptr - (INOUT) The OAM delay object to add
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_oam_delay_add(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr)
{
    int rv = BCM_E_NONE;
    bcm_oam_delay_t *new_delay_ptr = NULL;

    if (delay_ptr != NULL) {
        new_delay_ptr = (bcm_oam_delay_t *)
                     sal_alloc(sizeof(bcm_oam_delay_t),
                     "New delay_ptr");
        if (new_delay_ptr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_oam_delay_t(delay_ptr, new_delay_ptr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_delay_add(unit, new_delay_ptr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_oam_delay_t(new_delay_ptr, delay_ptr);

    /* Deallocate memory*/
    sal_free(new_delay_ptr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_oam_delay_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_delay_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      delay_ptr - (INOUT) The OAM delay object to get
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_oam_delay_get(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr)
{
    int rv = BCM_E_NONE;
    bcm_oam_delay_t *new_delay_ptr = NULL;

    if (delay_ptr != NULL) {
        new_delay_ptr = (bcm_oam_delay_t *)
                     sal_alloc(sizeof(bcm_oam_delay_t),
                     "New delay_ptr");
        if (new_delay_ptr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_oam_delay_t(delay_ptr, new_delay_ptr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_delay_get(unit, new_delay_ptr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_oam_delay_t(new_delay_ptr, delay_ptr);

    /* Deallocate memory*/
    sal_free(new_delay_ptr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_oam_delay_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_delay_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      delay_ptr - (IN) The OAM delay object to delete
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_oam_delay_delete(
    int unit,
    bcm_compat6519_oam_delay_t *delay_ptr)
{
    int rv = BCM_E_NONE;
    bcm_oam_delay_t *new_delay_ptr = NULL;

    if (delay_ptr != NULL) {
        new_delay_ptr = (bcm_oam_delay_t *)
                     sal_alloc(sizeof(bcm_oam_delay_t),
                     "New delay_ptr");
        if (new_delay_ptr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_oam_delay_t(delay_ptr, new_delay_ptr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_delay_delete(unit, new_delay_ptr);


    /* Deallocate memory*/
    sal_free(new_delay_ptr);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6519in_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_tunnel_initiator_t(
    bcm_compat6519_tunnel_initiator_t *from,
    bcm_tunnel_initiator_t *to)
{
    int i1 = 0;

    bcm_tunnel_initiator_t_init(to);
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
}

/*
 * Function:
 *      _bcm_compat6519out_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_tunnel_initiator_t(
    bcm_tunnel_initiator_t *from,
    bcm_compat6519_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_set(unit, intf, new_tunnel);


    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (INOUT) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_create(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_get(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_vxlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_vxlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_wlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_wlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2gre_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2gre_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6519_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_tunnel_terminator_t(
    bcm_compat6519_tunnel_terminator_t *from,
    bcm_tunnel_terminator_t *to)
{
    int i1 = 0;

    bcm_tunnel_terminator_t_init(to);
    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->pbmp = from->pbmp;
    to->vlan = from->vlan;
    to->fwd_vlan = from->fwd_vlan;
    to->remote_port = from->remote_port;
    to->tunnel_id = from->tunnel_id;
    to->if_class = from->if_class;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->failover_id = from->failover_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->tunnel_class = from->tunnel_class;
    to->qos_map_id = from->qos_map_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->priority = from->priority;
    to->subport_pbmp_profile_id = from->subport_pbmp_profile_id;
    to->egress_if = from->egress_if;
}

/*
 * Function:
 *      _bcm_compat6519out_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_tunnel_terminator_t(
    bcm_tunnel_terminator_t *from,
    bcm_compat6519_tunnel_terminator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->pbmp = from->pbmp;
    to->vlan = from->vlan;
    to->fwd_vlan = from->fwd_vlan;
    to->remote_port = from->remote_port;
    to->tunnel_id = from->tunnel_id;
    to->if_class = from->if_class;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->failover_id = from->failover_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->tunnel_class = from->tunnel_class;
    to->qos_map_id = from->qos_map_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->priority = from->priority;
    to->subport_pbmp_profile_id = from->subport_pbmp_profile_id;
    to->egress_if = from->egress_if;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_add(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_add(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_delete(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_update(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_attach(unit, new_terminator, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_detach(unit, new_terminator);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_id_get(unit, new_terminator, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      value - (IN) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_flexctr_object_set(unit, new_terminator, value);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_tunnel_terminator_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      value - (OUT) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *terminator,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_flexctr_object_get(unit, new_terminator, value);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_vxlan_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_vxlan_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_vxlan_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2gre_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2gre_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_l2gre_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6519_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

#endif /* defined(INCLUDE_L3) */


/*
 * Function:
 *      _bcm_compat6519in_stat_pp_profile_info_t
 * Purpose:
 *      Convert the bcm_stat_pp_profile_info_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_stat_pp_profile_info_t(
    bcm_compat6519_stat_pp_profile_info_t *from,
    bcm_stat_pp_profile_info_t *to)
{
    bcm_stat_pp_profile_info_t_init(to);
    to->counter_command_id = from->counter_command_id;
    to->stat_object_type = from->stat_object_type;
    to->meter_command_id = from->meter_command_id;
    to->meter_qos_map_id = from->meter_qos_map_id;
    to->is_meter_enable = from->is_meter_enable;
    to->is_fp_cs_var = from->is_fp_cs_var;
    to->ingress_tunnel_metadata_size = from->ingress_tunnel_metadata_size;
    to->ingress_tunnel_metadata_start_bit = from->ingress_tunnel_metadata_start_bit;
    to->ingress_forward_metadata_size = from->ingress_forward_metadata_size;
    to->ingress_forward_metadata_start_bit = from->ingress_forward_metadata_start_bit;
    to->ingress_fwd_plus_one_metadata_size = from->ingress_fwd_plus_one_metadata_size;
    to->ingress_fwd_plus_one_metadata_start_bit = from->ingress_fwd_plus_one_metadata_start_bit;
}

/*
 * Function:
 *      _bcm_compat6519out_stat_pp_profile_info_t
 * Purpose:
 *      Convert the bcm_stat_pp_profile_info_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_stat_pp_profile_info_t(
    bcm_stat_pp_profile_info_t *from,
    bcm_compat6519_stat_pp_profile_info_t *to)
{
    to->counter_command_id = from->counter_command_id;
    to->stat_object_type = from->stat_object_type;
    to->meter_command_id = from->meter_command_id;
    to->meter_qos_map_id = from->meter_qos_map_id;
    to->is_meter_enable = from->is_meter_enable;
    to->is_fp_cs_var = from->is_fp_cs_var;
    to->ingress_tunnel_metadata_size = from->ingress_tunnel_metadata_size;
    to->ingress_tunnel_metadata_start_bit = from->ingress_tunnel_metadata_start_bit;
    to->ingress_forward_metadata_size = from->ingress_forward_metadata_size;
    to->ingress_forward_metadata_start_bit = from->ingress_forward_metadata_start_bit;
    to->ingress_fwd_plus_one_metadata_size = from->ingress_fwd_plus_one_metadata_size;
    to->ingress_fwd_plus_one_metadata_start_bit = from->ingress_fwd_plus_one_metadata_start_bit;
}

/*
 * Function:
 *      bcm_compat6519_stat_pp_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_stat_pp_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Flags
 *      engine_source - (IN) counting source, Legal values: ingressReceivePp, EgressReceivePp, EgressTransmitPp
 *      stat_pp_profile - (INOUT)
 *      stat_pp_profile_info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_compat6519_stat_pp_profile_info_t *stat_pp_profile_info)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_profile_info_t *new_stat_pp_profile_info = NULL;

    if (stat_pp_profile_info != NULL) {
        new_stat_pp_profile_info = (bcm_stat_pp_profile_info_t *)
                     sal_alloc(sizeof(bcm_stat_pp_profile_info_t),
                     "New stat_pp_profile_info");
        if (new_stat_pp_profile_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_stat_pp_profile_info_t(stat_pp_profile_info, new_stat_pp_profile_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_stat_pp_profile_create(unit, flags, engine_source, stat_pp_profile, new_stat_pp_profile_info);


    /* Deallocate memory*/
    sal_free(new_stat_pp_profile_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_stat_pp_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_stat_pp_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      stat_pp_profile - (IN) pp profile statistics ID
 *      stat_pp_profile_info - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_compat6519_stat_pp_profile_info_t *stat_pp_profile_info)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_profile_info_t *new_stat_pp_profile_info = NULL;

    if (stat_pp_profile_info != NULL) {
        new_stat_pp_profile_info = (bcm_stat_pp_profile_info_t *)
                     sal_alloc(sizeof(bcm_stat_pp_profile_info_t),
                     "New stat_pp_profile_info");
        if (new_stat_pp_profile_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_stat_pp_profile_get(unit, stat_pp_profile, new_stat_pp_profile_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_stat_pp_profile_info_t(new_stat_pp_profile_info, stat_pp_profile_info);

    /* Deallocate memory*/
    sal_free(new_stat_pp_profile_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6519out_cosq_max_latency_pkts_t
 * Purpose:
 *      Convert the bcm_cosq_max_latency_pkts_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_cosq_max_latency_pkts_t(
    bcm_cosq_max_latency_pkts_t *from,
    bcm_compat6519_cosq_max_latency_pkts_t *to)
{
    to->dest_gport = from->dest_gport;
    to->cosq = from->cosq;
    to->latency = from->latency;
    to->latency_flow = from->latency_flow;
}

/*
 * Function:
 *      bcm_compat6519_cosq_max_latency_pkts_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_max_latency_pkts_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) gport
 *      flags - (IN) flags
 *      max_count - (IN) max_count
 *      max_latency_pkts - (OUT) max_latency_pkts
 *      actual_count - (OUT) actual_count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_cosq_max_latency_pkts_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_compat6519_cosq_max_latency_pkts_t *max_latency_pkts,
    int *actual_count)
{
    int rv = BCM_E_NONE;
    bcm_cosq_max_latency_pkts_t *new_max_latency_pkts = NULL;

    if (max_latency_pkts != NULL) {
        new_max_latency_pkts = (bcm_cosq_max_latency_pkts_t *)
                     sal_alloc(sizeof(bcm_cosq_max_latency_pkts_t),
                     "New max_latency_pkts");
        if (new_max_latency_pkts == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_max_latency_pkts_get(unit, gport, flags, max_count, new_max_latency_pkts, actual_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_cosq_max_latency_pkts_t(new_max_latency_pkts, max_latency_pkts);

    /* Deallocate memory*/
    sal_free(new_max_latency_pkts);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519in_policer_database_config_t
 * Purpose:
 *      Convert the bcm_policer_database_config_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_policer_database_config_t(
    bcm_compat6519_policer_database_config_t *from,
    bcm_policer_database_config_t *to)
{
    bcm_policer_database_config_t_init(to);
    to->expansion_enable = from->expansion_enable;
    to->is_single_bucket = from->is_single_bucket;
}

/*
 * Function:
 *      bcm_compat6519_policer_database_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_database_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN)
 *      policer_database_handle - (IN) Handle decode the policer database key: core, scope, database_id.
 *      config - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_policer_database_create(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_compat6519_policer_database_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_policer_database_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_policer_database_config_t *)
                     sal_alloc(sizeof(bcm_policer_database_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_policer_database_config_t(config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_database_create(unit, flags, policer_database_handle, new_config);


    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6519in_policer_expansion_group_t
 * Purpose:
 *      Convert the bcm_policer_expansion_group_t datatype from <=6.5.19 to
 *      SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_policer_expansion_group_t(
    bcm_compat6519_policer_expansion_group_t *from,
    bcm_policer_expansion_group_t *to)
{
    int i1 = 0;

    bcm_policer_expansion_group_t_init(to);
    for (i1 = 0; i1 < bcmPolicerFwdTypeMax; i1++) {
        sal_memcpy(&to->config[i1], &from->config[i1], sizeof(bcm_policer_expansion_group_types_config_t));
    }
}

/*
 * Function:
 *      _bcm_compat6519out_policer_expansion_group_t
 * Purpose:
 *      Convert the bcm_policer_expansion_group_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_policer_expansion_group_t(
    bcm_policer_expansion_group_t *from,
    bcm_compat6519_policer_expansion_group_t *to)
{
    int i1 = 0;

    for (i1 = 0; i1 < bcmPolicerFwdTypeMax; i1++) {
        sal_memcpy(&to->config[i1], &from->config[i1], sizeof(bcm_policer_expansion_group_types_config_t));
    }
}

/*
 * Function:
 *      bcm_compat6519_policer_expansion_groups_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_expansion_groups_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN)
 *      core_id - (IN)
 *      expansion_group - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_policer_expansion_groups_set(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_compat6519_policer_expansion_group_t *expansion_group)
{
    int rv = BCM_E_NONE;
    bcm_policer_expansion_group_t *new_expansion_group = NULL;

    if (expansion_group != NULL) {
        new_expansion_group = (bcm_policer_expansion_group_t *)
                     sal_alloc(sizeof(bcm_policer_expansion_group_t),
                     "New expansion_group");
        if (new_expansion_group == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_policer_expansion_group_t(expansion_group, new_expansion_group);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_expansion_groups_set(unit, flags, core_id, new_expansion_group);


    /* Deallocate memory*/
    sal_free(new_expansion_group);

    return rv;
}

/*
 * Function:
 *      bcm_compat6519_policer_expansion_groups_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_expansion_groups_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN)
 *      core_id - (IN)
 *      expansion_group - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_policer_expansion_groups_get(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_compat6519_policer_expansion_group_t *expansion_group)
{
    int rv = BCM_E_NONE;
    bcm_policer_expansion_group_t *new_expansion_group = NULL;

    if (expansion_group != NULL) {
        new_expansion_group = (bcm_policer_expansion_group_t *)
                     sal_alloc(sizeof(bcm_policer_expansion_group_t),
                     "New expansion_group");
        if (new_expansion_group == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_expansion_groups_get(unit, flags, core_id, new_expansion_group);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_policer_expansion_group_t(new_expansion_group, expansion_group);

    /* Deallocate memory*/
    sal_free(new_expansion_group);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6519in_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from <=6.5.19 to
 *   SDK 6.5.19+
 * Parameters:
 *   from        - (IN) The <=6.5.19 version of the datatype
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519in_flowtracker_check_info_t(
        bcm_compat6519_flowtracker_check_info_t *from,
        bcm_flowtracker_check_info_t *to)
{
    bcm_flowtracker_check_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);

    to->custom_id = 0;
}

/*
 * Function:
 *   _bcm_compat6519out_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from 6.5.19+ to
 *   <=6.5.19
 * Parameters:
 *   from        - (IN) The 6.5.19+ version of the datatype
 *   to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6519out_flowtracker_check_info_t(
        bcm_flowtracker_check_info_t *from,
        bcm_compat6519_flowtracker_check_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_check_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_create
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   options    - (IN) Options to create check.
 *   check_info - (IN) check info.
 *   check_id   - (INOUT) Software id of check.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6519_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id)
{
    int rv = 0;
    bcm_flowtracker_check_info_t new_check_info;

    _bcm_compat6519in_flowtracker_check_info_t(&check_info, &new_check_info);

    rv = bcm_flowtracker_check_create(unit, options, new_check_info, check_id);

    if (rv >= 0) {
        _bcm_compat6519out_flowtracker_check_info_t(&new_check_info, &check_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6519_flowtracker_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   check_info - (OUT) check info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6519_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6519_flowtracker_check_info_t *check_info)
{
    int rv = 0;
    bcm_flowtracker_check_info_t *new_check_info = NULL;

    if (check_info != NULL) {
        new_check_info = (bcm_flowtracker_check_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_info_t), "New check info");
        if (new_check_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6519in_flowtracker_check_info_t(check_info, new_check_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_get(unit, check_id, new_check_info);

    if (rv >= 0) {
        _bcm_compat6519out_flowtracker_check_info_t(new_check_info, check_info);
    }

    sal_free(new_check_info);
    return rv;
}


/*
 * Function:
 *      _bcm_compat6519in_field_group_config_t
 * Purpose:
 *      Convert the bcm_compat6519_field_group_config_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519in_field_group_config_t(
    bcm_compat6519_field_group_config_t *from,
    bcm_field_group_config_t *to)
{
    bcm_field_group_config_t_init(to);

    to->flags = from->flags;
    to->qset = from->qset;
    to->priority = from->priority;
    to->mode = from->mode;
    to->ports = from->ports;
    to->group = from->group;
    to->aset = from->aset;
    to->preselset = from->preselset;
    to->group_ref = from->group_ref;
    to->max_entry_priorities = from->max_entry_priorities;
    to->hintid  = from->hintid;
    to->action_res_id = from->action_res_id;
    sal_memcpy(to->name, from->name,
               (sizeof(uint8) *  BCM_FIELD_MAX_NAME_LEN));
    to->cycle = from->cycle;
    to->pgm_bmp = from->pgm_bmp;
    to->share_id = from->share_id;
    to->presel_group = from->presel_group;
}

/*
 * Function:
 *      _bcm_compat6519out_field_group_config_t
 * Purpose:
 *      Convert the bcm_compat6519_field_group_config_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_field_group_config_t(
    bcm_field_group_config_t *from,
    bcm_compat6519_field_group_config_t *to)
{
    to->flags = from->flags;
    to->qset = from->qset;
    to->priority = from->priority;
    to->mode = from->mode;
    to->ports = from->ports;
    to->group = from->group;
    to->aset = from->aset;
    to->preselset = from->preselset;
    to->group_ref = from->group_ref;
    to->max_entry_priorities = from->max_entry_priorities;
    to->hintid  = from->hintid;
    to->action_res_id = from->action_res_id;
    sal_memcpy(to->name, from->name,
               (sizeof(uint8) *  BCM_FIELD_MAX_NAME_LEN));
    to->cycle = from->cycle;
    to->pgm_bmp = from->pgm_bmp;
    to->share_id = from->share_id;
    to->presel_group = from->presel_group;

}

/*
 * Function: bcm_compat6519_field_group_config_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_config_create.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      group_config - bcm_compat6519_field_group_config_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6519_field_group_config_create(
    int unit,
    bcm_compat6519_field_group_config_t *group_config)
{
    int rv;
    bcm_field_group_config_t *new_group_config = NULL;

    if (NULL != group_config) {
        /* Create from heap to avoid the stack to bloat */
        new_group_config  = (bcm_field_group_config_t *)
            sal_alloc(sizeof(bcm_field_group_config_t), "New field group config");
        if (NULL == new_group_config) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_field_group_config_t(group_config, new_group_config);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_group_config_create(unit, new_group_config);

    if (BCM_SUCCESS(rv) && new_group_config != NULL && group_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6519out_field_group_config_t(new_group_config, group_config);
    }

    if (NULL != new_group_config) {
        /* Deallocate memory*/
        sal_free(new_group_config);
    }

    return rv;
}

/*
 * Function: bcm_compat6519_field_group_config_validate
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_config_validate.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      group_config - bcm_compat6519_field_group_config_t.
 *      mode - Reference to group mode.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6519_field_group_config_validate(
    int unit,
    bcm_compat6519_field_group_config_t *group_config,
    bcm_field_group_mode_t *mode)
{
    int rv;
    bcm_field_group_config_t *new_group_config = NULL;

    if (NULL != group_config) {
        /* Create from heap to avoid the stack to bloat */
        new_group_config  = (bcm_field_group_config_t *)
            sal_alloc(sizeof(bcm_field_group_config_t), "New field group config");
        if (NULL == new_group_config) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6519in_field_group_config_t(group_config, new_group_config);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_group_config_validate(unit, new_group_config, mode);

    if (NULL != new_group_config) {
        /* Deallocate memory*/
        sal_free(new_group_config);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6519out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.20+ to
 *      <=6.5.19
 * Parameters:
 *      from        - (IN) The SDK 6.5.20+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6519out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6519_port_config_t *to)
{
    int i1 = 0;

    to->fe = from->fe;
    to->ge = from->ge;
    to->xe = from->xe;
    to->ce = from->ce;
    to->le = from->le;
    to->cc = from->cc;
    to->cd = from->cd;
    to->e = from->e;
    to->hg = from->hg;
    to->sci = from->sci;
    to->sfi = from->sfi;
    to->spi = from->spi;
    to->spi_subport = from->spi_subport;
    to->port = from->port;
    to->cpu = from->cpu;
    to->all = from->all;
    to->stack_int = from->stack_int;
    to->stack_ext = from->stack_ext;
    to->tdm = from->tdm;
    to->pon = from->pon;
    to->llid = from->llid;
    to->il = from->il;
    to->xl = from->xl;
    to->rcy = from->rcy;
    to->sat = from->sat;
    to->ipsec = from->ipsec;
    for (i1 = 0; i1 < BCM_PIPES_MAX; i1++) {
        to->per_pipe[i1] = from->per_pipe[i1];
    }
    to->nif = from->nif;
    to->control = from->control;
    to->eventor = from->eventor;
    to->olp = from->olp;
    to->oamp = from->oamp;
    to->erp = from->erp;
    to->roe = from->roe;
    to->rcy_mirror = from->rcy_mirror;
    for (i1 = 0; i1 < BCM_PP_PIPES_MAX; i1++) {
        to->per_pp_pipe[i1] = from->per_pp_pipe[i1];
    }
    to->stat = from->stat;
}

/*
 * Function:
 *      bcm_compat6519_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6519_port_config_get(
    int unit,
    bcm_compat6519_port_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_port_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_port_config_t *)
                     sal_alloc(sizeof(bcm_port_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_config_get(unit, new_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6519out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}


#endif /* BCM_RPC_SUPPORT*/
