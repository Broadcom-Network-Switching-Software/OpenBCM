/*
* $Id: compat_6526.c,v 1.0 2021/11/18
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.26 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6526.h>


/*
 * Function:
 *      _bcm_compat6526in_port_phy_fec_error_mask_t
 * Purpose:
 *      Convert the bcm_port_phy_fec_error_mask_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_port_phy_fec_error_mask_t(
    bcm_compat6526_port_phy_fec_error_mask_t *from,
    bcm_port_phy_fec_error_mask_t *to)
{
    to->error_mask_bit_31_0 = from->error_mask_bit_31_0;
    to->error_mask_bit_63_32 = from->error_mask_bit_63_32;
    to->error_mask_bit_79_64 = from->error_mask_bit_79_64;
}

/*
 * Function:
 *      _bcm_compat6526out_port_phy_fec_error_mask_t
 * Purpose:
 *      Convert the bcm_port_phy_fec_error_mask_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_port_phy_fec_error_mask_t(
    bcm_port_phy_fec_error_mask_t *from,
    bcm_compat6526_port_phy_fec_error_mask_t *to)
{
    to->error_mask_bit_31_0 = from->error_mask_bit_31_0;
    to->error_mask_bit_63_32 = from->error_mask_bit_63_32;
    to->error_mask_bit_79_64 = from->error_mask_bit_79_64;
}

/*
 * Function:
 *      bcm_compat6526_port_phy_fec_error_inject_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_phy_fec_error_inject_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) 
 *      error_control_map - (IN) 
 *      bit_error_mask - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_port_phy_fec_error_inject_set(
    int unit,
    bcm_port_t port,
    uint16 error_control_map,
    bcm_compat6526_port_phy_fec_error_mask_t bit_error_mask)
{
    int rv = BCM_E_NONE;
    bcm_port_phy_fec_error_mask_t new_bit_error_mask;
    sal_memset(&new_bit_error_mask, 0, sizeof(bcm_port_phy_fec_error_mask_t));

    /* Transform the entry from the old format to new one */
    _bcm_compat6526in_port_phy_fec_error_mask_t(&bit_error_mask, &new_bit_error_mask);
    /* Call the BCM API with new format */
    rv = bcm_port_phy_fec_error_inject_set(unit, port, error_control_map, new_bit_error_mask);


    return rv;
}

/*
 * Function:
 *      bcm_compat6526_port_phy_fec_error_inject_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_phy_fec_error_inject_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) 
 *      error_control_map - (OUT) 
 *      bit_error_mask - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_port_phy_fec_error_inject_get(
    int unit,
    bcm_port_t port,
    uint16 *error_control_map,
    bcm_compat6526_port_phy_fec_error_mask_t *bit_error_mask)
{
    int rv = BCM_E_NONE;
    bcm_port_phy_fec_error_mask_t *new_bit_error_mask = NULL;

    if (bit_error_mask != NULL) {
        new_bit_error_mask = (bcm_port_phy_fec_error_mask_t *)
                     sal_alloc(sizeof(bcm_port_phy_fec_error_mask_t),
                     "New bit_error_mask");
        if (new_bit_error_mask == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_phy_fec_error_inject_get(unit, port, error_control_map, new_bit_error_mask);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_port_phy_fec_error_mask_t(new_bit_error_mask, bit_error_mask);

    /* Deallocate memory*/
    sal_free(new_bit_error_mask);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6526_port_config_t *to)
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
    to->crps = from->crps;
    to->lb = from->lb;
    to->d3c = from->d3c;
    to->otn = from->otn;
}

/*
 * Function:
 *      bcm_compat6526_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_port_config_get(
    int unit,
    bcm_compat6526_port_config_t *config)
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
    _bcm_compat6526out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_tunnel_terminator_t(
    bcm_compat6526_tunnel_terminator_t *from,
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
    to->default_vrf = from->default_vrf;
    to->sci = from->sci;
    to->default_tunnel_id = from->default_tunnel_id;
}

/*
 * Function:
 *      _bcm_compat6526out_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_tunnel_terminator_t(
    bcm_tunnel_terminator_t *from,
    bcm_compat6526_tunnel_terminator_t *to)
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
    to->default_vrf = from->default_vrf;
    to->sci = from->sci;
    to->default_tunnel_id = from->default_tunnel_id;
}

/*
 * Function:
 *      bcm_compat6526_tunnel_terminator_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_tunnel_terminator_add(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6526_tunnel_terminator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_tunnel_terminator_delete(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6526_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6526_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vxlan_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vxlan_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vxlan_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2gre_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2gre_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2gre_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6526_tunnel_terminator_t *info)
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
        _bcm_compat6526in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}
#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6526in_trunk_member_t
 * Purpose:
 *      Convert the bcm_trunk_member_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_trunk_member_t(
    bcm_compat6526_trunk_member_t *from,
    bcm_trunk_member_t *to)
{
    bcm_trunk_member_t_init(to);
    to->flags = from->flags;
    to->gport = from->gport;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->spa_id = from->spa_id;
}

/*
 * Function:
 *      _bcm_compat6526out_trunk_member_t
 * Purpose:
 *      Convert the bcm_trunk_member_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_trunk_member_t(
    bcm_trunk_member_t *from,
    bcm_compat6526_trunk_member_t *to)
{
    to->flags = from->flags;
    to->gport = from->gport;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->spa_id = from->spa_id;
}

/*
 * Function:
 *      bcm_compat6526_trunk_member_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_member_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk ID.
 *      member - (IN) Trunk member to add.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_trunk_member_add(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member = NULL;

    if (member != NULL) {
        new_member = (bcm_trunk_member_t *)
                     sal_alloc(sizeof(bcm_trunk_member_t),
                     "New member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_trunk_member_t(member, new_member);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_member_add(unit, tid, new_member);


    /* Deallocate memory*/
    sal_free(new_member);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_trunk_member_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_member_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk ID.
 *      member - (IN) Trunk member to delete.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_trunk_member_delete(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member = NULL;

    if (member != NULL) {
        new_member = (bcm_trunk_member_t *)
                     sal_alloc(sizeof(bcm_trunk_member_t),
                     "New member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_trunk_member_t(member, new_member);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_member_delete(unit, tid, new_member);


    /* Deallocate memory*/
    sal_free(new_member);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_trunk_psc_profile_info_t
 * Purpose:
 *      Convert the bcm_trunk_psc_profile_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_trunk_psc_profile_info_t(
    bcm_compat6526_trunk_psc_profile_info_t *from,
    bcm_trunk_psc_profile_info_t *to)
{
    int i1 = 0;

    for (i1 = 0; i1 < 16; i1++) {
        to->weight_array[i1] = from->weight_array[i1];
    }
    to->weight_array_size = from->weight_array_size;
    to->psc_flags = from->psc_flags;
    to->max_nof_members_in_profile = from->max_nof_members_in_profile;
}

/*
 * Function:
 *      _bcm_compat6526out_trunk_psc_profile_info_t
 * Purpose:
 *      Convert the bcm_trunk_psc_profile_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_trunk_psc_profile_info_t(
    bcm_trunk_psc_profile_info_t *from,
    bcm_compat6526_trunk_psc_profile_info_t *to)
{
    int i1 = 0;

    for (i1 = 0; i1 < 16; i1++) {
        to->weight_array[i1] = from->weight_array[i1];
    }
    to->weight_array_size = from->weight_array_size;
    to->psc_flags = from->psc_flags;
    to->max_nof_members_in_profile = from->max_nof_members_in_profile;
}

/*
 * Function:
 *      _bcm_compat6526in_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_trunk_info_t(
    bcm_compat6526_trunk_info_t *from,
    bcm_trunk_info_t *to)
{
    bcm_trunk_info_t_init(to);
    to->flags = from->flags;
    to->psc = from->psc;
    _bcm_compat6526in_trunk_psc_profile_info_t(&from->psc_info, &to->psc_info);
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_trunk_dgm_t));
    to->rh_random_seed = from->rh_random_seed;
}

/*
 * Function:
 *      _bcm_compat6526out_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_trunk_info_t(
    bcm_trunk_info_t *from,
    bcm_compat6526_trunk_info_t *to)
{
    to->flags = from->flags;
    to->psc = from->psc;
    _bcm_compat6526out_trunk_psc_profile_info_t(&from->psc_info, &to->psc_info);
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_trunk_dgm_t));
    to->rh_random_seed = from->rh_random_seed;
}

/*
 * Function:
 *      _bcm_compat6526in_trunk_multiset_info_t
 * Purpose:
 *      Convert the bcm_trunk_multiset_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_trunk_multiset_info_t(
    bcm_compat6526_trunk_multiset_info_t *from,
    bcm_trunk_multiset_info_t *to)
{
    bcm_trunk_multiset_info_t_init(to);
    to->tid = from->tid;
    _bcm_compat6526in_trunk_info_t(&from->trunk_info, &to->trunk_info);
    to->member_count = from->member_count;
    to->set_result = from->set_result;
}

/*
 * Function:
 *      _bcm_compat6526out_trunk_multiset_info_t
 * Purpose:
 *      Convert the bcm_trunk_multiset_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_trunk_multiset_info_t(
    bcm_trunk_multiset_info_t *from,
    bcm_compat6526_trunk_multiset_info_t *to)
{
    to->tid = from->tid;
    _bcm_compat6526out_trunk_info_t(&from->trunk_info, &to->trunk_info);
    to->member_count = from->member_count;
    to->set_result = from->set_result;
}

/*
 * Function:
 *      bcm_compat6526_trunk_multi_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_multi_set.
 * Parameters:
 *      unit - (IN) BCM device number
 *      trunk_count - (IN) Number of trunk groups in trunk_array.
 *      trunk_array - (INOUT) Trunk list (see \ref bcm_trunk_multiset_info_t)
 *      tot_member_count - (IN) Total number of members in member_array from trunk list.
 *      member_array - (IN) Chained member list from trunk list.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_trunk_multi_set(
    int unit,
    int trunk_count,
    bcm_compat6526_trunk_multiset_info_t *trunk_array,
    int tot_member_count,
    bcm_compat6526_trunk_member_t *member_array)
{
    int rv = BCM_E_NONE;
    bcm_trunk_multiset_info_t *new_trunk_array = NULL;
    bcm_trunk_member_t *new_member_array = NULL;
    int i = 0;

    if (trunk_array != NULL && trunk_count > 0 && member_array != NULL && tot_member_count > 0) {
        new_trunk_array = (bcm_trunk_multiset_info_t *)
                     sal_alloc(trunk_count * sizeof(bcm_trunk_multiset_info_t),
                     "New trunk_array");
        if (new_trunk_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < trunk_count; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6526in_trunk_multiset_info_t(&trunk_array[i], &new_trunk_array[i]);
        }
        new_member_array = (bcm_trunk_member_t *)
                     sal_alloc(tot_member_count * sizeof(bcm_trunk_member_t),
                     "New member_array");
        if (new_member_array == NULL) {
            sal_free(new_trunk_array);
            return BCM_E_MEMORY;
        }
        for (i = 0; i < tot_member_count; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6526in_trunk_member_t(&member_array[i], &new_member_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_multi_set(unit, trunk_count, new_trunk_array, tot_member_count, new_member_array);

    for (i = 0; i < trunk_count; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_trunk_multiset_info_t(&new_trunk_array[i], &trunk_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_trunk_array);
    sal_free(new_member_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_trunk_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      tid - (IN) Trunk ID.
 *      t_data - (OUT) 
 *      member_max - (IN) Maximum number of members in provided member_array.
 *      member_array - (OUT) Member list.
 *      member_count - (OUT) Number of members returned in member_array.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_info_t *t_data,
    int member_max,
    bcm_compat6526_trunk_member_t *member_array,
    int *member_count)
{
    int rv = BCM_E_NONE;
    bcm_trunk_info_t *new_t_data = NULL;
    bcm_trunk_member_t *new_member_array = NULL;
    int i = 0;

    if (t_data != NULL && member_array != NULL && member_max > 0) {
        new_t_data = (bcm_trunk_info_t *)
                     sal_alloc(sizeof(bcm_trunk_info_t),
                     "New t_data");
        if (new_t_data == NULL) {
            return BCM_E_MEMORY;
        }
        new_member_array = (bcm_trunk_member_t *)
                     sal_alloc(member_max * sizeof(bcm_trunk_member_t),
                     "New member_array");
        if (new_member_array == NULL) {
            sal_free(new_t_data);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_get(unit, tid, new_t_data, member_max, new_member_array, member_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_trunk_info_t(new_t_data, t_data);
    for (i = 0; i < member_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_trunk_member_t(&new_member_array[i], &member_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_t_data);
    sal_free(new_member_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_trunk_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_set.
 * Parameters:
 *      unit - (IN) BCM device number
 *      tid - (IN) Trunk ID
 *      trunk_info - (IN) Pointer to trunk info structure (see \ref bcm_trunk_info_t)
 *      member_count - (IN) Number of members in member_array.
 *      member_array - (IN) Member list.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6526_trunk_info_t *trunk_info,
    int member_count,
    bcm_compat6526_trunk_member_t *member_array)
{
    int rv = BCM_E_NONE;
    bcm_trunk_info_t *new_trunk_info = NULL;
    bcm_trunk_member_t *new_member_array = NULL;
    int i = 0;

    if (trunk_info != NULL && member_array != NULL && member_count > 0) {
        new_trunk_info = (bcm_trunk_info_t *)
                     sal_alloc(sizeof(bcm_trunk_info_t),
                     "New trunk_info");
        if (new_trunk_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_trunk_info_t(trunk_info, new_trunk_info);
        new_member_array = (bcm_trunk_member_t *)
                     sal_alloc(member_count * sizeof(bcm_trunk_member_t),
                     "New member_array");
        if (new_member_array == NULL) {
            sal_free(new_trunk_info);
            return BCM_E_MEMORY;
        }
        for (i = 0; i < member_count; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6526in_trunk_member_t(&member_array[i], &new_member_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_set(unit, tid, new_trunk_info, member_count, new_member_array);


    /* Deallocate memory*/
    sal_free(new_trunk_info);
    sal_free(new_member_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_policer_config_t
 * Purpose:
 *      Convert the bcm_policer_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_policer_config_t(
    bcm_compat6526_policer_config_t *from,
    bcm_policer_config_t *to)
{
    bcm_policer_config_t_init(to);
    to->flags = from->flags;
    to->mode = from->mode;
    to->ckbits_sec = from->ckbits_sec;
    to->cbits_sec_lower = from->cbits_sec_lower;
    to->max_ckbits_sec = from->max_ckbits_sec;
    to->max_cbits_sec_lower = from->max_cbits_sec_lower;
    to->ckbits_burst = from->ckbits_burst;
    to->cbits_burst_lower = from->cbits_burst_lower;
    to->pkbits_sec = from->pkbits_sec;
    to->pbits_sec_lower = from->pbits_sec_lower;
    to->max_pkbits_sec = from->max_pkbits_sec;
    to->max_pbits_sec_lower = from->max_pbits_sec_lower;
    to->pkbits_burst = from->pkbits_burst;
    to->pbits_burst_lower = from->pbits_burst_lower;
    to->kbits_current = from->kbits_current;
    to->bits_current_lower = from->bits_current_lower;
    to->action_id = from->action_id;
    to->sharing_mode = from->sharing_mode;
    to->entropy_id = from->entropy_id;
    to->pool_id = from->pool_id;
    to->bucket_width = from->bucket_width;
    to->token_gran = from->token_gran;
    to->bucket_init_val = from->bucket_init_val;
    to->bucket_rollover_val = from->bucket_rollover_val;
    to->core_id = from->core_id;
    to->ncoflow_policer_id = from->ncoflow_policer_id;
    to->npoflow_policer_id = from->npoflow_policer_id;
    to->actual_ckbits_sec = from->actual_ckbits_sec;
    to->actual_cbits_sec_lower = from->actual_cbits_sec_lower;
    to->actual_ckbits_burst = from->actual_ckbits_burst;
    to->actual_cbits_burst_lower = from->actual_cbits_burst_lower;
    to->actual_pkbits_sec = from->actual_pkbits_sec;
    to->actual_pbits_sec_lower = from->actual_pbits_sec_lower;
    to->actual_pkbits_burst = from->actual_pkbits_burst;
    to->actual_pbits_burst_lower = from->actual_pbits_burst_lower;
    to->average_pkt_size = from->average_pkt_size;
    to->mark_drop_as_black = from->mark_drop_as_black;
    to->color_resolve_profile = from->color_resolve_profile;
    to->meter_pair_index = from->meter_pair_index;
    to->ports = from->ports;
}

/*
 * Function:
 *      _bcm_compat6526out_policer_config_t
 * Purpose:
 *      Convert the bcm_policer_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_policer_config_t(
    bcm_policer_config_t *from,
    bcm_compat6526_policer_config_t *to)
{
    to->flags = from->flags;
    to->mode = from->mode;
    to->ckbits_sec = from->ckbits_sec;
    to->cbits_sec_lower = from->cbits_sec_lower;
    to->max_ckbits_sec = from->max_ckbits_sec;
    to->max_cbits_sec_lower = from->max_cbits_sec_lower;
    to->ckbits_burst = from->ckbits_burst;
    to->cbits_burst_lower = from->cbits_burst_lower;
    to->pkbits_sec = from->pkbits_sec;
    to->pbits_sec_lower = from->pbits_sec_lower;
    to->max_pkbits_sec = from->max_pkbits_sec;
    to->max_pbits_sec_lower = from->max_pbits_sec_lower;
    to->pkbits_burst = from->pkbits_burst;
    to->pbits_burst_lower = from->pbits_burst_lower;
    to->kbits_current = from->kbits_current;
    to->bits_current_lower = from->bits_current_lower;
    to->action_id = from->action_id;
    to->sharing_mode = from->sharing_mode;
    to->entropy_id = from->entropy_id;
    to->pool_id = from->pool_id;
    to->bucket_width = from->bucket_width;
    to->token_gran = from->token_gran;
    to->bucket_init_val = from->bucket_init_val;
    to->bucket_rollover_val = from->bucket_rollover_val;
    to->core_id = from->core_id;
    to->ncoflow_policer_id = from->ncoflow_policer_id;
    to->npoflow_policer_id = from->npoflow_policer_id;
    to->actual_ckbits_sec = from->actual_ckbits_sec;
    to->actual_cbits_sec_lower = from->actual_cbits_sec_lower;
    to->actual_ckbits_burst = from->actual_ckbits_burst;
    to->actual_cbits_burst_lower = from->actual_cbits_burst_lower;
    to->actual_pkbits_sec = from->actual_pkbits_sec;
    to->actual_pbits_sec_lower = from->actual_pbits_sec_lower;
    to->actual_pkbits_burst = from->actual_pkbits_burst;
    to->actual_pbits_burst_lower = from->actual_pbits_burst_lower;
    to->average_pkt_size = from->average_pkt_size;
    to->mark_drop_as_black = from->mark_drop_as_black;
    to->color_resolve_profile = from->color_resolve_profile;
    to->meter_pair_index = from->meter_pair_index;
    to->ports = from->ports;
}

/*
 * Function:
 *      bcm_compat6526_policer_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pol_cfg - (IN) Policer config
 *      policer_id - (INOUT) Policer ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_policer_create(
    int unit,
    bcm_compat6526_policer_config_t *pol_cfg,
    bcm_policer_t *policer_id)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New pol_cfg");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_policer_config_t(pol_cfg, new_pol_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_create(unit, new_pol_cfg, policer_id);


    /* Deallocate memory*/
    sal_free(new_pol_cfg);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_policer_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 *      pol_cfg - (IN) Policer configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6526_policer_config_t *pol_cfg)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New pol_cfg");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_policer_config_t(pol_cfg, new_pol_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_set(unit, policer_id, new_pol_cfg);


    /* Deallocate memory*/
    sal_free(new_pol_cfg);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_policer_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 *      pol_cfg - (OUT) Policer configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6526_policer_config_t *pol_cfg)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New pol_cfg");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_get(unit, policer_id, new_pol_cfg);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_policer_config_t(new_pol_cfg, pol_cfg);

    /* Deallocate memory*/
    sal_free(new_pol_cfg);

    return rv;
}

STATIC void _bcm_compat6526in_flexstate_action_t(
                                    bcm_compat6526_flexstate_action_t *from,
                                    bcm_flexstate_action_t *to)
{
    to->flags = from->flags;
    to->source = from->source;
    sal_memcpy(&to->ports, &from->ports, sizeof(bcm_pbmp_t));
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    sal_memcpy(&to->index_operation, &from->index_operation, sizeof(bcm_flexstate_index_operation_t));

    to->operation_a.update_compare = from->operation_a.update_compare;
    to->operation_a.update_compare_opd = bcmFlexstateUpdateCompareOpdUseStoredValA;
    sal_memcpy(&to->operation_a.compare_operation, &from->operation_a.compare_operation, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_a.operation_true, &from->operation_a.operation_true, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_a.operation_false, &from->operation_a.operation_false, sizeof(bcm_flexstate_value_operation_t));
    to->operation_a.update_pool = from->operation_a.update_pool;
    to->operation_a.update_bus = from->operation_a.update_bus;

    to->operation_b.update_compare = from->operation_b.update_compare;
    to->operation_b.update_compare_opd = bcmFlexstateUpdateCompareOpdUseStoredValB;
    sal_memcpy(&to->operation_b.compare_operation, &from->operation_b.compare_operation, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_b.operation_true, &from->operation_b.operation_true, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_b.operation_false, &from->operation_b.operation_false, sizeof(bcm_flexstate_value_operation_t));
    to->operation_b.update_pool = from->operation_b.update_pool;
    to->operation_b.update_bus = from->operation_b.update_bus;

    sal_memcpy(&to->trigger, &from->trigger, sizeof(bcm_flexstate_trigger_t));
}

STATIC void _bcm_compat6526out_flexstate_action_t(
                                    bcm_flexstate_action_t *from,
                                    bcm_compat6526_flexstate_action_t *to)
{
    to->flags = from->flags;
    to->source = from->source;
    sal_memcpy(&to->ports, &from->ports, sizeof(bcm_pbmp_t));
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    sal_memcpy(&to->index_operation, &from->index_operation, sizeof(bcm_flexstate_index_operation_t));

    to->operation_a.update_compare = from->operation_a.update_compare;
    sal_memcpy(&to->operation_a.compare_operation, &from->operation_a.compare_operation, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_a.operation_true, &from->operation_a.operation_true, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_a.operation_false, &from->operation_a.operation_false, sizeof(bcm_flexstate_value_operation_t));
    to->operation_a.update_pool = from->operation_a.update_pool;
    to->operation_a.update_bus = from->operation_a.update_bus;

    to->operation_b.update_compare = from->operation_b.update_compare;
    sal_memcpy(&to->operation_b.compare_operation, &from->operation_b.compare_operation, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_b.operation_true, &from->operation_b.operation_true, sizeof(bcm_flexstate_value_operation_t));
    sal_memcpy(&to->operation_b.operation_false, &from->operation_b.operation_false, sizeof(bcm_flexstate_value_operation_t));
    to->operation_b.update_pool = from->operation_b.update_pool;
    to->operation_b.update_bus = from->operation_b.update_bus;

    sal_memcpy(&to->trigger, &from->trigger, sizeof(bcm_flexstate_trigger_t));
}

int bcm_compat6526_flexstate_action_create(
    int unit, 
    int options, 
    bcm_compat6526_flexstate_action_t *action, 
    uint32 *action_id)
{
    bcm_flexstate_action_t new_action;

    if (action == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexstate_action_t_init(&new_action);
    _bcm_compat6526in_flexstate_action_t(action, &new_action);

    return bcm_flexstate_action_create(unit, options, &new_action, action_id);

}

int bcm_compat6526_flexstate_action_get(
    int unit, 
    uint32 action_id, 
    bcm_compat6526_flexstate_action_t *action)
{
    bcm_flexstate_action_t new_action;
    int rv;

    if (action == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexstate_action_t_init(&new_action);

    rv = bcm_flexstate_action_get(unit, action_id, &new_action);

   _bcm_compat6526out_flexstate_action_t(&new_action, action);

    return rv;
}

STATIC void _bcm_compat6526in_flexstate_quantization_t(
                                    bcm_compat6526_flexstate_quantization_t *from,
                                    bcm_flexstate_quantization_t *to)
{
    int i;

    to->object = from->object;
    to->object_upper = from->object_upper;
    to->ports = from->ports;

    for (i = 0; i < BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }

    to->range_num = from->range_num;
    to->flags = from->flags;
    to->object_mask = 0xffff;
    to->object_mask_upper = 0xffff;
}

STATIC void _bcm_compat6526out_flexstate_quantization_t(
                                    bcm_flexstate_quantization_t *from,
                                    bcm_compat6526_flexstate_quantization_t *to)
{
    int i;

    to->object = from->object;
    to->object_upper = from->object_upper;
    to->ports = from->ports;

    for (i = 0; i < BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }

    to->range_num = from->range_num;
    to->flags = from->flags;
}

int bcm_compat6526_flexstate_quantization_create(
                            int unit,
                            int options,
                            bcm_compat6526_flexstate_quantization_t *quantization,
                            uint32 *quant_id)
{
    bcm_flexstate_quantization_t new_quantization;

    if (quantization == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexstate_quantization_t_init(&new_quantization);
    _bcm_compat6526in_flexstate_quantization_t(quantization, &new_quantization);

    return bcm_flexstate_quantization_create(unit, options, &new_quantization, quant_id);

}

int bcm_compat6526_flexstate_quantization_get(
                            int unit,
                            uint32 quant_id,
                            bcm_compat6526_flexstate_quantization_t *quantization)
{
    bcm_flexstate_quantization_t new_quantization;
    int rv;

    if (quantization == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexstate_quantization_t_init(&new_quantization);

    rv = bcm_flexstate_quantization_get(unit, quant_id, &new_quantization);

   _bcm_compat6526out_flexstate_quantization_t(&new_quantization, quantization);

    return rv;
}

STATIC void _bcm_compat6526in_flexctr_quantization_t(
                                    bcm_compat6526_flexctr_quantization_t *from,
                                    bcm_flexctr_quantization_t *to)
{
    int i;

    to->object = from->object;
    to->object_upper = from->object_upper;
    to->ports = from->ports;

    for (i = 0; i < BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }

    to->range_num = from->range_num;
    to->object_id = from->object_id;
    to->object_id_upper = from->object_id_upper;
    to->flags = from->flags;
    to->object_mask = 0xffff;
    to->object_mask_upper = 0xffff;
}

STATIC void _bcm_compat6526out_flexctr_quantization_t(
                                    bcm_flexctr_quantization_t *from,
                                    bcm_compat6526_flexctr_quantization_t *to)
{
    int i;

    to->object = from->object;
    to->object_upper = from->object_upper;
    to->ports = from->ports;

    for (i = 0; i < BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }

    to->range_num = from->range_num;
    to->object_id = from->object_id;
    to->object_id_upper = from->object_id_upper;
    to->flags = from->flags;
}

int bcm_compat6526_flexctr_quantization_create(
                            int unit,
                            int options,
                            bcm_compat6526_flexctr_quantization_t *quantization,
                            uint32 *quant_id) {
    bcm_flexctr_quantization_t new_quantization;

    if (quantization == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexctr_quantization_t_init(&new_quantization);
    _bcm_compat6526in_flexctr_quantization_t(quantization, &new_quantization);

    return bcm_flexctr_quantization_create(unit, options, &new_quantization, quant_id);

}

int bcm_compat6526_flexctr_quantization_get(
                            int unit,
                            uint32 quant_id,
                            bcm_compat6526_flexctr_quantization_t *quantization)
{
    bcm_flexctr_quantization_t new_quantization;
    int rv;

    if (quantization == NULL) {
        return BCM_E_PARAM;
    }

    bcm_flexctr_quantization_t_init(&new_quantization);

    rv = bcm_flexctr_quantization_get(unit, quant_id, &new_quantization);

   _bcm_compat6526out_flexctr_quantization_t(&new_quantization, quantization);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6526in_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from <=6.5.26 to
 *   SDK 6.5.26+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_info_t(
        bcm_compat6526_flowtracker_check_info_t *from,
        bcm_flowtracker_check_info_t *to)
{
    bcm_flowtracker_check_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->mask_value = from->mask_value;
    to->shift_value = from->shift_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);

    to->custom_id = from->custom_id;
    to->stage = from->stage;
}

/*
 * Function:
 *   _bcm_compat6526out_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from 6.5.26+ to
 *   <=6.5.26
 * Parameters:
 *   from        - (IN) The 6.5.26+ version of the datatype
 *   to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_info_t(
        bcm_flowtracker_check_info_t *from,
        bcm_compat6526_flowtracker_check_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->mask_value = from->mask_value;
    to->shift_value = from->shift_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);

    to->custom_id = from->custom_id;
    to->stage = from->stage;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_create
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
int bcm_compat6526_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6526_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id)
{
    int rv = 0;
    bcm_flowtracker_check_info_t new_check_info;

    _bcm_compat6526in_flowtracker_check_info_t(&check_info, &new_check_info);

    rv = bcm_flowtracker_check_create(unit, options, new_check_info, check_id);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_check_info_t(&new_check_info, &check_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   check_info - (OUT) check info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6526_flowtracker_check_info_t *check_info)
{
    int rv = 0;
    bcm_flowtracker_check_info_t *new_check_info = NULL;

    if (check_info != NULL) {
        new_check_info = (bcm_flowtracker_check_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_info_t), "New check info");
        if (new_check_info == NULL) {
            return BCM_E_MEMORY;
        }
        bcm_flowtracker_check_info_t_init(new_check_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_get(unit, check_id, new_check_info);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_check_info_t(new_check_info, check_info);
    }

    sal_free(new_check_info);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype
 *      from <=6.5.26 to SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_action_info_t(
    bcm_compat6526_flowtracker_check_action_info_t *from,
    bcm_flowtracker_check_action_info_t *to)
{
    bcm_flowtracker_check_action_info_t_init(to);
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    to->weight                   = from->weight;
    to->direction                = from->direction;
    to->custom_id                = from->custom_id;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype from
 *      6.5.26+ to SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_action_info_t(
    bcm_flowtracker_check_action_info_t *from,
    bcm_compat6526_flowtracker_check_action_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    to->weight                   = from->weight;
    to->direction                = from->direction;
    to->custom_id                = from->custom_id;

    return;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_action_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (IN)    flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t info)
{
    int rv;
    bcm_flowtracker_check_action_info_t info_new;

    /* Transform the entry from the old format to new one */
    _bcm_compat6526in_flowtracker_check_action_info_t(&info, &info_new);

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_set(unit, check_id, info_new);

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_action_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (OUT)   flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t *info)
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
        _bcm_compat6526in_flowtracker_check_action_info_t(info, info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_get(unit, check_id, info_new);

    if (info_new != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flowtracker_check_action_info_t(info_new, info);

        sal_free(info_new);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flowtracker_check_export_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_export_info_t datatype
 *      from <=6.5.26 to SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_export_info_t(
    bcm_compat6526_flowtracker_check_export_info_t *from,
    bcm_flowtracker_check_export_info_t *to)
{
    bcm_flowtracker_check_export_info_t_init(to);
    to->export_check_threshold = from->export_check_threshold;
    to->operation              = from->operation;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_flowtracker_check_export_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_export_info_t datatype from
 *      6.5.26+ to SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_export_info_t(
    bcm_flowtracker_check_export_info_t *from,
    bcm_compat6526_flowtracker_check_export_info_t *to)
{
    to->export_check_threshold = from->export_check_threshold;
    to->operation              = from->operation;

    return;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_export_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_export_info_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (IN)    flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_export_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t info)
{
    int rv;
    bcm_flowtracker_check_export_info_t info_new;

    /* Transform the entry from the old format to new one */
    _bcm_compat6526in_flowtracker_check_export_info_t(&info, &info_new);

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_export_info_set(unit, check_id, info_new);

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_export_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_export_info_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (OUT)   flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_export_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t *info)
{
    int rv;
    bcm_flowtracker_check_export_info_t *info_new = NULL;

    if (info != NULL) {
        info_new = sal_alloc(sizeof(bcm_flowtracker_check_export_info_t),
                "New check export info");
        if (info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flowtracker_check_export_info_t(info, info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_export_info_get(unit, check_id, info_new);

    if (info_new != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flowtracker_check_export_info_t(info_new, info);

        sal_free(info_new);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6526in_flowtracker_group_flow_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_group_flow_action_info_t datatype from <=6.5.26 to
 *   SDK 6.5.26+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_group_flow_action_info_t(
        bcm_compat6526_flowtracker_group_flow_action_info_t *from,
        bcm_flowtracker_group_flow_action_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));
    to->pipe_idx = from->pipe_idx;
    to->mode = from->mode;
    to->exact_match_idx = from->exact_match_idx;
}

/*
 * Function:
 *   _bcm_compat6526out_flowtracker_group_flow_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_group_flow_action_info_t datatype from 6.5.26+ to
 *   <=6.5.26
 * Parameters:
 *   from        - (IN) The 6.5.26+ version of the datatype
 *   to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_group_flow_action_info_t(
        bcm_flowtracker_group_flow_action_info_t *from,
        bcm_compat6526_flowtracker_group_flow_action_info_t *to)
{
    to->pipe_idx = from->pipe_idx;
    to->mode = from->mode;
    to->exact_match_idx = from->exact_match_idx;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_group_flow_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_flow_delete
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for delete action.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_compat6526_flowtracker_group_flow_delete(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_compat6526_flowtracker_group_flow_action_info_t *action_info)
{
    int rv = 0;
    bcm_flowtracker_group_flow_action_info_t *new_action_info = NULL;

    if (action_info != NULL) {
        new_action_info = (bcm_flowtracker_group_flow_action_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_group_flow_action_info_t), "New action info");
        if (new_action_info == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_flowtracker_group_flow_action_info_t(action_info, new_action_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_flow_delete(unit, flow_group_id, new_action_info);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_group_flow_action_info_t(new_action_info, action_info);
    }

    sal_free(new_action_info);
    return rv;
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_action_update
 * Purpose:
 *      Update actions of a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for update action.
 *      num_actions             - (IN)  Number of actions to update
 *      action_list             - (IN)  List of actions to update.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_compat6526_flowtracker_group_flow_action_update(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_compat6526_flowtracker_group_flow_action_info_t *action_info,
        int num_actions,
        bcm_flowtracker_group_action_info_t *action_list)
{
    int rv = 0;
    bcm_flowtracker_group_flow_action_info_t *new_action_info = NULL;

    if (action_info != NULL) {
        new_action_info = (bcm_flowtracker_group_flow_action_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_group_flow_action_info_t), "New action info");
        if (new_action_info == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_flowtracker_group_flow_action_info_t(action_info, new_action_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_flow_action_update(unit, flow_group_id, new_action_info, num_actions, action_list);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_group_flow_action_info_t(new_action_info, action_info);
    }

    sal_free(new_action_info);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from <=6.5.26 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l2_addr_t(
    bcm_compat6526_l2_addr_t *from,
    bcm_l2_addr_t *to)
{
    int i1 = 0;

    sal_memset(to, 0, sizeof(*to));
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
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
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->learn_strength = from->learn_strength;
    to->age_profile = from->age_profile;
}

/*
 * Function:
 *      _bcm_compat6526out_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from SDK 6.5.26+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l2_addr_t(
    bcm_l2_addr_t *from,
    bcm_compat6526_l2_addr_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
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
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->learn_strength = from->learn_strength;
    to->age_profile = from->age_profile;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) Input MAC address to search
 *      vid - (IN) Input VLAN ID to search
 *      l2addr - (OUT) Pointer to L2 address structure to receive results
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_conflict_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_conflict_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      addr - (IN) L2 address to search for conflicts
 *      cf_array - (OUT) List of L2 addresses conflicting with addr
 *      cf_max - (IN) Number of entries allocated to cf_array
 *      cf_count - (OUT) Actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_conflict_get(
    int unit,
    bcm_compat6526_l2_addr_t *addr,
    bcm_compat6526_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_addr = NULL;
    bcm_l2_addr_t *new_cf_array = NULL;
    int i = 0;

    if (addr != NULL && cf_array != NULL && cf_max > 0) {
        new_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New addr");
        if (new_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(addr, new_addr);
        new_cf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                     "New cf_array");
        if (new_cf_array == NULL) {
            sal_free(new_addr);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, new_addr, new_cf_array, cf_max, cf_count);

    for (i = 0; i < cf_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l2_addr_t(&new_cf_array[i], &cf_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_addr);
    sal_free(new_cf_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_get32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_get32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_set32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_set32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_enable_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_enable_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) pointer to an L2 address structure.
 *      enable - (IN) Zero to disable, anything else to enable.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_enable_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, new_l2_addr, enable);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace.
 * Parameters:
 *      unit - (IN) BCM device number
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      new_module - (IN) New module ID for a replace
 *      new_port - (IN) New port for a replace
 *      new_trunk - (IN) New trunk ID for a replace
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;

    if (match_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(match_addr, new_match_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, new_match_addr, new_module, new_port, new_trunk);


    /* Deallocate memory*/
    sal_free(new_match_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_replace_match
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace_match.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      mask_addr - (IN) L2 mask to match on delete/replace
 *      replace_addr - (IN) value to replace match entries (not relevant in case of delete)
 *      replace_mask_addr - (IN) sets which fields/bits from replace_addr are relevant for replace. Unmasked fields/bit will not be affected. (not relevant for delete)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_compat6526_l2_addr_t *mask_addr,
    bcm_compat6526_l2_addr_t *replace_addr,
    bcm_compat6526_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;
    bcm_l2_addr_t *new_mask_addr = NULL;
    bcm_l2_addr_t *new_replace_addr = NULL;
    bcm_l2_addr_t *new_replace_mask_addr = NULL;

    if (match_addr != NULL && mask_addr != NULL && replace_addr != NULL && replace_mask_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(match_addr, new_match_addr);
        new_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New mask_addr");
        if (new_mask_addr == NULL) {
            sal_free(new_match_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(mask_addr, new_mask_addr);
        new_replace_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_addr");
        if (new_replace_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(replace_addr, new_replace_addr);
        new_replace_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_mask_addr");
        if (new_replace_mask_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            sal_free(new_replace_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(replace_mask_addr, new_replace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, new_match_addr, new_mask_addr, new_replace_addr, new_replace_mask_addr);


    /* Deallocate memory*/
    sal_free(new_match_addr);
    sal_free(new_mask_addr);
    sal_free(new_replace_addr);
    sal_free(new_replace_mask_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_multi_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_multi_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_multi_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type<br>L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_multi_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_by_struct_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (INOUT) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_by_struct_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_get(unit, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_by_struct_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_by_struct_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_delete(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6526in_l3_ipmc_t
 * Purpose:
 *      Convert the bcm_l3_ipmc_t datatype from <=6.5.26 to 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_ipmc_addr_t(
    bcm_compat6526_ipmc_addr_t *from,
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
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6526out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.26+ to <=6.5.26
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6526_ipmc_addr_t *to)
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
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

int bcm_compat6526_ipmc_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
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
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
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
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
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
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_remove(unit, new_data);

    if (NULL != new_data) {
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_ipmc_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) IPMC route information.
 *      value - (IN) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_ipmc_flexctr_object_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t *)
                     sal_alloc(sizeof(bcm_ipmc_addr_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_ipmc_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) IPMC route information.
 *      value - (OUT) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_ipmc_flexctr_object_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t *)
                     sal_alloc(sizeof(bcm_ipmc_addr_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

int bcm_compat6526_ipmc_stat_attach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_attach(unit, new_data, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_detach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info)
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_detach(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_get(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_sync_get(unit, new_data, stat, num_entries,
                                        counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_set(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_id_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
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
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_id_get(unit, new_data, stat, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
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
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
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
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
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
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_remove(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from <=6.5.26 to 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_host_t(
    bcm_compat6526_l3_host_t *from,
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
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
    to->l3a_traffic_class = from->l3a_traffic_class;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from 6.5.26+ to <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l3_host_t(
    bcm_l3_host_t *from,
    bcm_compat6526_l3_host_t *to)
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
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
    to->l3a_traffic_class = from->l3a_traffic_class;
}

int bcm_compat6526_l3_host_find(
    int unit,
    bcm_compat6526_l3_host_t *info)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_find(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_add(
    int unit,
    bcm_compat6526_l3_host_t *info)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_add(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete(
    int unit,
    bcm_compat6526_l3_host_t *ip_addr)
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
        _bcm_compat6526in_l3_host_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete_by_interface(
    int unit,
    bcm_compat6526_l3_host_t *info)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete_all(
    int unit,
    bcm_compat6526_l3_host_t *info)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_attach(
    int unit,
    bcm_compat6526_l3_host_t *info,
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_detach(
    int unit,
    bcm_compat6526_l3_host_t *info)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_flexctr_object_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 value)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_set(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_flexctr_object_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 *value)
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_get(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_get(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                           counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_set(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_id_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
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
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_route_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_route_find.
 * Parameters:
 *      unit - (IN) BCM device number
 *      host - (IN) Pointer to bcm_l3_host_t specifying the IP address.
 *      route - (INOUT) Pointer to bcm_l3_route_t specifying the network number. \ref L3_ROUTE_FIND_FIELDS_table
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_route_find(
    int unit,
    bcm_compat6526_l3_host_t *host,
    bcm_l3_route_t *route)
{
    int rv = BCM_E_NONE;
    bcm_l3_host_t *new_host = NULL;

    if (host != NULL) {
        new_host = (bcm_l3_host_t *)
                     sal_alloc(sizeof(bcm_l3_host_t),
                     "New host");
        if (new_host == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(host, new_host);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_find(unit, new_host, route);


    /* Deallocate memory*/
    sal_free(new_host);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <= 6.5.26 to SDK 6.5.27+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.26 version of the datatype.
 *      to   - (OUT) SDK 6.5.27+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526in_l3_egress_ecmp_t(bcm_compat6526_l3_egress_ecmp_t *from,
                                   bcm_l3_egress_ecmp_t *to)
{
    if (from && to) {
        bcm_l3_egress_ecmp_t_init(to);
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.27+ to <=6.5.26.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.27+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.26 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526out_l3_egress_ecmp_t(bcm_l3_egress_ecmp_t *from,
                                    bcm_compat6526_l3_egress_ecmp_t *to)
{
    if (from && to) {
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_egress_ecmp_resilient_entry_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <= 6.5.26
 *      to SDK 6.5.27+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.26 version of the datatype.
 *      to   - (OUT) SDK 6.5.27+ version of the datatype.
 *
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    if (from && to) {
        to->hash_key = from->hash_key;
        (void)_bcm_compat6526in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
        to->intf = from->intf;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_create.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      options           - (IN) L3_ECMP_O_xxx flags.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_create(int unit,
                              uint32 options,
                              bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
                              int ecmp_member_count,
                              bcm_l3_ecmp_member_t *ecmp_member_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_create(unit, options, new, ecmp_member_count,
                            ecmp_member_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_get.
 *
 * Parameters:
 *      unit              - (IN) Unit number.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_size  - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of entries of ecmp_member_count
 *                                actually filled in. This will be a value less
 *                                than or equal to the value passed in as
 *                                ecmp_member_size unless ecmp_member_size is 0.
 *                                If ecmp_member_size is 0 then
 *                                ecmp_member_array is ignored and
 *                                ecmp_member_count is filled in with the number
 *                                of entries that would have been filled into
 *                                ecmp_member_array if ecmp_member_size was
 *                                arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_get(int unit,
                           bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
                           int ecmp_member_size,
                           bcm_l3_ecmp_member_t *ecmp_member_array,
                           int *ecmp_member_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_get(unit, new, ecmp_member_size, ecmp_member_array,
                         ecmp_member_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_find.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      ecmp_member_count - (IN) Number of member in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *      ecmp_info         - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_find(int unit,
                            int ecmp_member_count,
                            bcm_l3_ecmp_member_t *ecmp_member_array,
                            bcm_compat6526_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_create.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_create(int unit,
                                     bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                     int intf_count,
                                     bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_create(unit, new, intf_count, intf_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_destroy
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_destroy.
 *
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_destroy(int unit,
                                      bcm_compat6526_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_destroy(unit, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_get.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_size  - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in.
 *                         This will be a value less than or equal to the value
 *                         passed in as intf_size unless intf_size is 0. If
 *                         intf_size is 0 then intf_array is ignored and
 *                         intf_count is filled in with the number of entries
 *                         that would have been filled into intf_array if
 *                         intf_size was arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_get(int unit,
                                  bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                  int intf_size,
                                  bcm_if_t *intf_array,
                                  int *intf_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_get(unit, new, intf_size, intf_array, intf_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_add.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_add(int unit,
                                  bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                  bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_add(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_delete.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_delete(int unit,
                                     bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                     bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_delete(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_find.
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp       - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_find(int unit,
                                   int intf_count,
                                   bcm_if_t *intf_array,
                                   bcm_compat6526_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the structure members. */
    bcm_l3_egress_ecmp_t_init(new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, intf_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_tunnel_priority_set
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_tunnel_priority_set.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit, new, intf_count,
                                                intf_array);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_replace
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_replace.
 *
 * Parameters:
 *      unit          - (IN) Unit number.
 *      flags         - (IN) BCM_TRUNK_RESILIENT flags.
 *      match_entry   - (IN) Matching criteria
 *      num_entries   - (OUT) Number of entries matched.
 *      replace_entry - (IN) Replacing entry when the action is replace.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_match = NULL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_replace = NULL;

    /* Validate input parameters for NULL ptr. */
    if (match_entry == NULL || replace_entry == NULL) {
        return (BCM_E_PARAM);
    }

    new_match = (bcm_l3_egress_ecmp_resilient_entry_t *)
                    sal_alloc(sizeof(*new_match),
                              "bcmcompat6526L3EgrEcmpRhNewMatchEnt");
    if (new_match == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new_match, 0, sizeof(*new_match));
    new_replace = (bcm_l3_egress_ecmp_resilient_entry_t *)
                        sal_alloc(sizeof(*new_replace),
                                  "bcmcompat6526L3EgrEcmpRhNewReplaceEnt");
    if (new_replace == NULL) {
        sal_free(new_match);
        return (BCM_E_MEMORY);
    }
    sal_memset(new_replace, 0, sizeof(*new_replace));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(match_entry,
                                                             new_match);
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(replace_entry,
                                                             new_replace);
    rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, new_match,
                                              num_entries, new_replace);
    if (new_match) {
        sal_free(new_match);
    }
    if (new_replace) {
        sal_free(new_replace);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_add.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6526L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_delete.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6526L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_match_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_match_config_t(
    bcm_compat6526_flow_match_config_t *from,
    bcm_flow_match_config_t *to)
{
    int i1 = 0;

    bcm_flow_match_config_t_init(to);
    to->vnid = from->vnid;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->port = from->port;
    to->sip = from->sip;
    to->sip_mask = from->sip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    to->dip = from->dip;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->protocol = from->protocol;
    to->mpls_label = from->mpls_label;
    to->flow_port = from->flow_port;
    to->vpn = from->vpn;
    to->intf_id = from->intf_id;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->flags = from->flags;
    to->default_vlan = from->default_vlan;
    to->default_vpn = from->default_vpn;
    to->class_id = from->class_id;
    to->next_hdr = from->next_hdr;
    to->tunnel_action = from->tunnel_action;
    to->etag_vlan = from->etag_vlan;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_match_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_match_config_t(
    bcm_flow_match_config_t *from,
    bcm_compat6526_flow_match_config_t *to)
{
    int i1 = 0;

    to->vnid = from->vnid;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->port = from->port;
    to->sip = from->sip;
    to->sip_mask = from->sip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    to->dip = from->dip;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->protocol = from->protocol;
    to->mpls_label = from->mpls_label;
    to->flow_port = from->flow_port;
    to->vpn = from->vpn;
    to->intf_id = from->intf_id;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->flags = from->flags;
    to->default_vlan = from->default_vlan;
    to->default_vpn = from->default_vpn;
    to->class_id = from->class_id;
    to->next_hdr = from->next_hdr;
    to->tunnel_action = from->tunnel_action;
    to->etag_vlan = from->etag_vlan;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_add(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_add(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_delete(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_delete(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) (IN/OUT) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (INOUT) (IN/OUT) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_match_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_attach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_detach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_id_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_tunnel_terminator_t(
    bcm_compat6526_flow_tunnel_terminator_t *from,
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
    to->vlan_mask = from->vlan_mask;
    to->class_id = from->class_id;
    to->term_pbmp = from->term_pbmp;
    to->next_hdr = from->next_hdr;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->vpn = from->vpn;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_tunnel_terminator_t(
    bcm_flow_tunnel_terminator_t *from,
    bcm_compat6526_flow_tunnel_terminator_t *to)
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
    to->vlan_mask = from->vlan_mask;
    to->class_id = from->class_id;
    to->term_pbmp = from->term_pbmp;
    to->next_hdr = from->next_hdr;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->vpn = from->vpn;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) tunnel config info structure
 *      num_of_fields - (IN) Number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_create(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_destroy
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_destroy.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) tunnel config info structure
 *      num_of_fields - (IN) number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_destroy(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) tunnel config info structure
 *      num_of_fields - (IN) number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
/*
 * Function:
 *      _bcm_compat6526in_cosq_mod_profile_t
 * Purpose:
 *      Convert the bcm_cosq_mod_profile_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_cosq_mod_profile_t(
    bcm_compat6526_cosq_mod_profile_t *from,
    bcm_cosq_mod_profile_t *to)
{
    bcm_cosq_mod_profile_t_init(to);
    to->percent_0_25 = from->percent_0_25;
    to->percent_25_50 = from->percent_25_50;
    to->percent_50_75 = from->percent_50_75;
    to->percent_75_100 = from->percent_75_100;
    to->enable = from->enable;
}

/*
 * Function:
 *      _bcm_compat6526out_cosq_mod_profile_t
 * Purpose:
 *      Convert the bcm_cosq_mod_profile_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_cosq_mod_profile_t(
    bcm_cosq_mod_profile_t *from,
    bcm_compat6526_cosq_mod_profile_t *to)
{
    to->percent_0_25 = from->percent_0_25;
    to->percent_25_50 = from->percent_25_50;
    to->percent_50_75 = from->percent_50_75;
    to->percent_75_100 = from->percent_75_100;
    to->enable = from->enable;
}

/*
 * Function:
 *      bcm_compat6526_cosq_mod_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_mod_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile_id - (IN) Profile id
 *      profile - (IN) (IN/OUT) Mirror-on-drop profile pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_cosq_mod_profile_set(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_mod_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_mod_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_mod_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_cosq_mod_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_mod_profile_set(unit, profile_id, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_cosq_mod_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_mod_profile_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      profile_id - (IN) Profile id
 *      profile - (OUT) (IN/OUT) Mirror-on-drop profile pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_cosq_mod_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_mod_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_mod_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_mod_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_mod_profile_get(unit, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_cosq_mod_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_mirror_ingress_mod_event_profile_t
 * Purpose:
 *      Convert the bcm_mirror_ingress_mod_event_profile_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_mirror_ingress_mod_event_profile_t(
    bcm_compat6526_mirror_ingress_mod_event_profile_t *from,
    bcm_mirror_ingress_mod_event_profile_t *to)
{
    bcm_mirror_ingress_mod_event_profile_t_init(to);
    to->reason_code = from->reason_code;
    to->sample_rate = from->sample_rate;
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6526out_mirror_ingress_mod_event_profile_t
 * Purpose:
 *      Convert the bcm_mirror_ingress_mod_event_profile_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_mirror_ingress_mod_event_profile_t(
    bcm_mirror_ingress_mod_event_profile_t *from,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *to)
{
    to->reason_code = from->reason_code;
    to->sample_rate = from->sample_rate;
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6526_mirror_ingress_mod_event_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_ingress_mod_event_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile - (IN) Ingress mirror-on-drop event profile
 *      profile_id - (OUT) Profile id returned on success
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_ingress_mod_event_profile_create(
    int unit,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile,
    int *profile_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_ingress_mod_event_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_ingress_mod_event_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_ingress_mod_event_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_mirror_ingress_mod_event_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_ingress_mod_event_profile_create(unit, new_profile, profile_id);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_mirror_ingress_mod_event_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_ingress_mod_event_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile_id - (IN) Event profile id
 *      profile - (OUT) Event profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_ingress_mod_event_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_ingress_mod_event_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_ingress_mod_event_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_ingress_mod_event_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_ingress_mod_event_profile_get(unit, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_mirror_ingress_mod_event_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_rx_cosq_mapping_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_rx_cosq_mapping_t(
    bcm_compat6526_rx_cosq_mapping_t *from,
    bcm_rx_cosq_mapping_t *to)
{
    bcm_rx_cosq_mapping_t_init(to);
    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
    to->flex_key1 = from->flex_key1;
    to->flex_key1_mask = from->flex_key1_mask;
    to->flex_key2 = from->flex_key2;
    to->flex_key2_mask = from->flex_key2_mask;
    sal_memcpy(&to->trace_reasons, &from->trace_reasons, sizeof(bcm_rx_pkt_trace_reasons_t));
    sal_memcpy(&to->trace_reasons_mask, &from->trace_reasons_mask, sizeof(bcm_rx_pkt_trace_reasons_t));
    to->drop_event = from->drop_event;
    to->drop_event_mask = from->drop_event_mask;
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6526out_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_rx_cosq_mapping_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_rx_cosq_mapping_t(
    bcm_rx_cosq_mapping_t *from,
    bcm_compat6526_rx_cosq_mapping_t *to)
{
    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
    to->flex_key1 = from->flex_key1;
    to->flex_key1_mask = from->flex_key1_mask;
    to->flex_key2 = from->flex_key2;
    to->flex_key2_mask = from->flex_key2_mask;
    sal_memcpy(&to->trace_reasons, &from->trace_reasons, sizeof(bcm_rx_pkt_trace_reasons_t));
    sal_memcpy(&to->trace_reasons_mask, &from->trace_reasons_mask, sizeof(bcm_rx_pkt_trace_reasons_t));
    to->drop_event = from->drop_event;
    to->drop_event_mask = from->drop_event_mask;
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) 
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_set(unit, options, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      rx_cosq_mapping - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_get(unit, new_rx_cosq_mapping);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_rx_cosq_mapping_t(new_rx_cosq_mapping, rx_cosq_mapping);

    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) 
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_add(unit, options, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_delete(unit, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_vlan_port_t(
    bcm_compat6526_vlan_port_t *from,
    bcm_vlan_port_t *to)
{
    bcm_vlan_port_t_init(to);
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
    to->learn_strength = from->learn_strength;
}

/*
 * Function:
 *      _bcm_compat6526out_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_vlan_port_t(
    bcm_vlan_port_t *from,
    bcm_compat6526_vlan_port_t *to)
{
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
    to->learn_strength = from->learn_strength;
}

/*
 * Function:
 *      bcm_compat6526_vlan_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 Logical port.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_port_create(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_create(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_port_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 logical port
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_port_find(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_find(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_port_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_port_t(
    bcm_compat6526_flow_port_t *from,
    bcm_flow_port_t *to)
{
    bcm_flow_port_t_init(to);
    to->flow_port_id = from->flow_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->network_group_id = from->network_group_id;
    to->egress_service_tpid = from->egress_service_tpid;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->drop_mode = from->drop_mode;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_port_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_port_t(
    bcm_flow_port_t *from,
    bcm_compat6526_flow_port_t *to)
{
    to->flow_port_id = from->flow_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->network_group_id = from->network_group_id;
    to->egress_service_tpid = from->egress_service_tpid;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->drop_mode = from->drop_mode;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}


/*
 * Function:
 *      _bcm_compat6526in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_encap_config_t(
    bcm_compat6526_flow_encap_config_t *from,
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
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->function = from->function;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_encap_config_t(
    bcm_flow_encap_config_t *from,
    bcm_compat6526_flow_encap_config_t *to)
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
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->function = from->function;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_add(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_add(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) Flow encap configuration parameters to specify the key
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_delete(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_delete(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (INOUT) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_encap_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_attach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_detach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_id_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vpn - (IN) BCM default vpn
 *      flow_port - (INOUT) (IN/OUT) FLOW port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_create(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;

    if (flow_port != NULL) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_port_t(flow_port, new_flow_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_create(unit, vpn, new_flow_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_port_t(new_flow_port, flow_port);

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) Vpn Instance
 *      flow_port - (INOUT) (IN/OUT) FLOW port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;

    if (flow_port != NULL) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_port_t(flow_port, new_flow_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_get(unit, vpn, new_flow_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_port_t(new_flow_port, flow_port);

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN Instance
 *      port_max - (IN) Maximum number of FLOW ports in array
 *      flow_port - (OUT) 
 *      port_count - (OUT) Number of FLOW ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6526_flow_port_t *flow_port,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;
    int i = 0;

    if (flow_port != NULL && port_max > 0) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(port_max * sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_get_all(unit, vpn, port_max, new_flow_port, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flow_port_t(&new_flow_port[i], &flow_port[i]);
    }

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_BFD)
/*
 * Function:
 *      _bcm_compat6526in_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_bfd_endpoint_info_t(
    bcm_compat6526_bfd_endpoint_info_t *from,
    bcm_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    bcm_bfd_endpoint_info_t_init(to);
    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->pkt_inner_vlan_id = from->pkt_inner_vlan_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    to->mpls_hdr = from->mpls_hdr;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
    to->vxlan_vnid = from->vxlan_vnid;
    to->vlan_tpid = from->vlan_tpid;
    to->inner_vlan_tpid = from->inner_vlan_tpid;
    to->injected_network_qos = from->injected_network_qos;
}

/*
 * Function:
 *      _bcm_compat6526out_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_bfd_endpoint_info_t(
    bcm_bfd_endpoint_info_t *from,
    bcm_compat6526_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->pkt_inner_vlan_id = from->pkt_inner_vlan_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    to->mpls_hdr = from->mpls_hdr;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
    to->vxlan_vnid = from->vxlan_vnid;
    to->vlan_tpid = from->vlan_tpid;
    to->inner_vlan_tpid = from->inner_vlan_tpid;
    to->injected_network_qos = from->injected_network_qos;
}

/*
 * Function:
 *      bcm_compat6526_bfd_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_bfd_endpoint_create(
    int unit,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_bfd_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_bfd_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}
#endif /* defined(INCLUDE_BFD) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_egress_t(
    bcm_compat6526_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i1 = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->flags3 = from->flags3;
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
    to->encap_access          = from->encap_access;
    to->ep_recirc_profile_id  = from->ep_recirc_profile_id;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6526_l3_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->flags3 = from->flags3;
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
    to->encap_access          = from->encap_access;
    to->ep_recirc_profile_id  = from->ep_recirc_profile_id;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_create
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
int bcm_compat6526_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_t *egr,
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
        _bcm_compat6526in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      intf - (IN) L3 interface ID pointing to Egress object.
 *      egr - (OUT) Egress forwarding path properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *egr)
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
    _bcm_compat6526out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      egr - (IN) Egress forwarding path properties.
 *      intf - (OUT) L3 interface ID pointing to Egress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_egress_find(
    int unit,
    bcm_compat6526_l3_egress_t *egr,
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
        _bcm_compat6526in_l3_egress_t(egr, new_egr);
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
 *      bcm_compat6526_l3_egress_allocation_get
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
int bcm_compat6526_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_t *egr,
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
        _bcm_compat6526in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_allocation_get(unit, flags, new_egr, nof_members, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_failover_egress_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *failover_egr)
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
        _bcm_compat6526in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_failover_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_egress_t *failover_egr)
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
        _bcm_compat6526in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_ingress_t(
    bcm_compat6526_l3_ingress_t *from,
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
    sal_memcpy(&to->int_cosq_stat[0], &from->int_cosq_stat[0],
               sizeof (bcm_l3_int_cosq_stat_t) * BCM_L3_INT_MAX_COSQ_STAT);
}

/*
 * Function:
 *      _bcm_compat6526out_l3_ingress_t
 * Purpose:
 *      Convert the bcm_l3_ingress_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l3_ingress_t(
    bcm_l3_ingress_t *from,
    bcm_compat6526_l3_ingress_t *to)
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
    sal_memcpy(&to->int_cosq_stat[0], &from->int_cosq_stat[0],
               sizeof (bcm_l3_int_cosq_stat_t) * BCM_L3_INT_MAX_COSQ_STAT);
}

/*
 * Function:
 *      bcm_compat6526_l3_ingress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ing_intf - (IN) Egress forwarding destination.
 *      intf_id - (INOUT) (IN/OUT) L3 Ingress Interface ID pointing to L3 IIF object. This is an IN argument if either BCM_L3_INGRESS_REPLACE or BCM_L3_INGRESS_WITH_ID are given in flags.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_ingress_create(
    int unit,
    bcm_compat6526_l3_ingress_t *ing_intf,
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
        _bcm_compat6526in_l3_ingress_t(ing_intf, new_ing_intf);
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
 *      bcm_compat6526_l3_ingress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      ing_intf - (OUT) Ingress Interface object properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_ingress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6526_l3_ingress_t *ing_intf)
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
    _bcm_compat6526out_l3_ingress_t(new_ing_intf, ing_intf);

    /* Deallocate memory*/
    sal_free(new_ing_intf);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_ingress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ingress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ing_intf - (IN) Ingress Interface object properties.
 *      intf_id - (OUT) L3-IIF ID pointing to Ingress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l3_ingress_find(
    int unit,
    bcm_compat6526_l3_ingress_t *ing_intf,
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
        _bcm_compat6526in_l3_ingress_t(ing_intf, new_ing_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ingress_find(unit, new_ing_intf, intf_id);


    /* Deallocate memory*/
    sal_free(new_ing_intf);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6526in_l3_nat_egress_t
 * Purpose:
 *   Convert the bcm_l3_nat_egress_t datatype from <=6.5.26 to
 *   SDK 6.5.27+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_l3_nat_egress_t(
        bcm_compat6526_l3_nat_egress_t *from,
        bcm_l3_nat_egress_t *to)
{
    bcm_l3_nat_egress_t_init(to);
    to->flags = from->flags;
    to->nat_id = from->nat_id;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->src_port = from->src_port;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    to->dst_port = from->dst_port;
    to->snat_id = from->snat_id;
    to->dnat_id = from->dnat_id;
}

/*
 * Function:
 *   _bcm_compat6526out_l3_nat_egress_t
 * Purpose:
 *   Convert the bcm_l3_nat_egress_t datatype from 6.5.27+ to
 *   <=6.5.26
 * Parameters:
 *   from     - (IN) The 6.5.26+ version of the datatype
 *   to       - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_l3_nat_egress_t(
        bcm_l3_nat_egress_t *from,
        bcm_compat6526_l3_nat_egress_t *to)
{
    to->flags = from->flags;
    to->nat_id = from->nat_id;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->src_port = from->src_port;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    to->dst_port = from->dst_port;
    to->snat_id = from->snat_id;
    to->dnat_id = from->dnat_id;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_egress_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_egress_add
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Egress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_egress_add(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_add(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_egress_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_egress_get
 * Parameters:
 *   unit         - (IN) BCM device unit number.
 *   nat_info     - (OUT) Egress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_egress_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(nat_info, new_nat_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_get(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_egress_destroy
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_egress_destroy
 * Parameters:
 *   unit         - (IN) BCM device unit number.
 *   nat_info     - (OUT) Egress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_egress_destroy(
        int unit,
        bcm_compat6526_l3_nat_egress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(nat_info, new_nat_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_destroy(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_attach(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        uint32 stat_counter_id)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_attach(unit, new_info, stat_counter_id);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_detach(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_detach(unit, new_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_counter_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_counter_get(unit, new_info, stat, num_entries, counter_indexes, counter_values);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_counter_sync_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_counter_sync_get(unit, new_info, stat, num_entries, counter_indexes, counter_values);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_counter_set(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 num_entries,
        uint32 *counter_indexes,
        bcm_stat_value_t *counter_values)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_counter_set(unit, new_info, stat, num_entries, counter_indexes, counter_values);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_multi_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint64 *value_arr)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_multi_get(unit, new_info, nstat, stat_arr, value_arr);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_multi_get32(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint32 *value_arr)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_multi_get32(unit, new_info, nstat, stat_arr, value_arr);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_multi_set(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint64 *value_arr)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_multi_set(unit, new_info, nstat, stat_arr, value_arr);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_multi_set32(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        int nstat,
        bcm_l3_nat_egress_stat_t *stat_arr,
        uint32 *value_arr)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_multi_set32(unit, new_info, nstat, stat_arr, value_arr);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

int bcm_compat6526_l3_nat_egress_stat_id_get(
        int unit,
        bcm_compat6526_l3_nat_egress_t *info,
        bcm_l3_nat_egress_stat_t stat,
        uint32 *stat_counter_id)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_egress_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_egress_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6526in_l3_nat_ingress_t
 * Purpose:
 *   Convert the bcm_l3_nat_ingress_t datatype from <=6.5.26 to
 *   SDK 6.5.27+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_l3_nat_ingress_t(
        bcm_compat6526_l3_nat_ingress_t *from,
        bcm_l3_nat_ingress_t *to)
{
    bcm_l3_nat_ingress_t_init(to);
    to->flags = from->flags;
    to->ip_addr = from->ip_addr;
    to->vrf = from->vrf;
    to->l4_port = from->l4_port;
    to->ip_proto = from->ip_proto;
    to->nat_id = from->nat_id;
    to->pri = from->pri;
    to->class_id = from->class_id;
    to->nexthop = from->nexthop;
    to->realm_id = from->realm_id;
}

/*
 * Function:
 *   _bcm_compat6526out_l3_nat_ingress_t
 * Purpose:
 *   Convert the bcm_l3_nat_ingress_t datatype from 6.5.27+ to
 *   <=6.5.26
 * Parameters:
 *   from     - (IN) The 6.5.27+ version of the datatype
 *   to       - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_l3_nat_ingress_t(
        bcm_l3_nat_ingress_t *from,
        bcm_compat6526_l3_nat_ingress_t *to)
{
    to->flags = from->flags;
    to->ip_addr = from->ip_addr;
    to->vrf = from->vrf;
    to->l4_port = from->l4_port;
    to->ip_proto = from->ip_proto;
    to->nat_id = from->nat_id;
    to->pri = from->pri;
    to->class_id = from->class_id;
    to->nexthop = from->nexthop;
    to->realm_id = from->realm_id;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_ingress_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_add
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_ingress_add(
        int unit,
        bcm_compat6526_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_add(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_ingress_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_delete
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_ingress_delete(
        int unit,
        bcm_compat6526_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_delete(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_ingress_find
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_find
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_ingress_find(
        int unit,
        bcm_compat6526_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_find(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6526_l3_nat_ingress_delete_all
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_delete_all
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_l3_nat_ingress_delete_all(
        int unit,
        bcm_compat6526_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6526in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_delete_all(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6526out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_intf_t(
    bcm_compat6526_l3_intf_t *from,
    bcm_l3_intf_t *to)
{
    int i = 0;

    bcm_l3_intf_t_init(to);
    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    to->l3a_ttl_dec                = from->l3a_ttl_dec;
    to->opaque_ctrl_id             = from->opaque_ctrl_id;
    to->l3a_mpls_flags             = from->l3a_mpls_flags;
    sal_memcpy(&to->l3a_mpls_label, &from->l3a_mpls_label, sizeof(bcm_mpls_label_t));
    to->l3a_mpls_ttl               = from->l3a_mpls_ttl;
    to->l3a_mpls_exp               = from->l3a_mpls_exp;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from 6.5.26+ to
 *      SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The 6.5.27+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l3_intf_t(
    bcm_l3_intf_t *from,
    bcm_compat6526_l3_intf_t *to)
{
    int i = 0;

    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    to->l3a_ttl_dec                = from->l3a_ttl_dec;
    to->opaque_ctrl_id             = from->opaque_ctrl_id;
    to->l3a_mpls_flags             = from->l3a_mpls_flags;
    sal_memcpy(&to->l3a_mpls_label, &from->l3a_mpls_label, sizeof(bcm_mpls_label_t));
    to->l3a_mpls_ttl               = from->l3a_mpls_ttl;
    to->l3a_mpls_exp               = from->l3a_mpls_exp;
    return;
}

int
bcm_compat6526_l3_intf_create(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_create(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6526_l3_intf_delete(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_delete(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6526_l3_intf_find(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6526_l3_intf_find_vlan(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find_vlan(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6526_l3_intf_get(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_get(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int bcm_compat6526_tunnel_initiator_set(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_set(unit, new_intf, tunnel);


    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6526_tunnel_initiator_create(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_create(unit, new_intf, tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l3_intf_t(new_intf, intf);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6526_tunnel_initiator_clear(
    int unit,
    bcm_compat6526_l3_intf_t *intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_clear(unit, new_intf);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6526_tunnel_initiator_get(
    int unit,
    bcm_compat6526_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_get(unit, new_intf, tunnel);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_stat_info_t
 * Purpose:
 *      Convert the bcm_flow_stat_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_stat_info_t(
    bcm_compat6526_flow_stat_info_t *from,
    bcm_flow_stat_info_t *to)
{
    int i1 = 0;

    bcm_flow_stat_info_t_init(to);
    to->flow_port = from->flow_port;
    to->port = from->port;
    to->vpn = from->vpn;
    to->inner_vlan = from->inner_vlan;
    to->vlan = from->vlan;
    to->vnid = from->vnid;
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
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->protocol = from->protocol;
    to->tunnel_id = from->tunnel_id;
    to->egress_if = from->egress_if;
    to->intf_id = from->intf_id;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
    to->l3a_flags = from->l3a_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->oif_group = from->oif_group;
    to->dvp_group = from->dvp_group;
    to->match_criteria = from->match_criteria;
    to->encap_criteria = from->encap_criteria;
    to->mpls_label = from->mpls_label;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->function_type = from->function_type;
}

/*
 * Function:
 *      bcm_compat6526_flow_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_stat_attach.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      flow_stat_info - (IN) Flow stat info.
 *      num_of_fields - (IN) Number of logical fields.
 *      field - (IN) Logical fields.
 *      stat_counter_id - (IN) Stat counter id.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_stat_attach(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_stat_info_t *new_flow_stat_info = NULL;

    if (flow_stat_info != NULL) {
        new_flow_stat_info = (bcm_flow_stat_info_t *)
                     sal_alloc(sizeof(bcm_flow_stat_info_t),
                     "New flow_stat_info");
        if (new_flow_stat_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_stat_info_t(flow_stat_info, new_flow_stat_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_stat_attach(unit, new_flow_stat_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_flow_stat_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_stat_info - (IN) 
 *      num_of_fields - (IN) 
 *      field - (IN) 
 *      stat_counter_id - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_stat_detach(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_stat_info_t *new_flow_stat_info = NULL;

    if (flow_stat_info != NULL) {
        new_flow_stat_info = (bcm_flow_stat_info_t *)
                     sal_alloc(sizeof(bcm_flow_stat_info_t),
                     "New flow_stat_info");
        if (new_flow_stat_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_stat_info_t(flow_stat_info, new_flow_stat_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_stat_detach(unit, new_flow_stat_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_flow_stat_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_stat_info - (IN) Flow stat info.
 *      num_of_fields - (IN) Number of logical fields.
 *      field - (IN) Logical fields.
 *      stat_object - (IN) 
 *      stat_counter_id - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_stat_id_get(
    int unit,
    bcm_compat6526_flow_stat_info_t *flow_stat_info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    bcm_stat_object_t stat_object,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_stat_info_t *new_flow_stat_info = NULL;

    if (flow_stat_info != NULL) {
        new_flow_stat_info = (bcm_flow_stat_info_t *)
                     sal_alloc(sizeof(bcm_flow_stat_info_t),
                     "New flow_stat_info");
        if (new_flow_stat_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_stat_info_t(flow_stat_info, new_flow_stat_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_stat_id_get(unit, new_flow_stat_info, num_of_fields, field, stat_object, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_flow_stat_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_tunnel_srv6_srh_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_srv6_srh_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_tunnel_srv6_srh_t(
    bcm_compat6526_flow_tunnel_srv6_srh_t *from,
    bcm_flow_tunnel_srv6_srh_t *to)
{
    bcm_flow_tunnel_srv6_srh_t_init(to);
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->next_hdr = from->next_hdr;
    to->tag = from->tag;
    to->egr_obj = from->egr_obj;
    to->valid_elements = from->valid_elements;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_tunnel_srv6_srh_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_srv6_srh_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_tunnel_srv6_srh_t(
    bcm_flow_tunnel_srv6_srh_t *from,
    bcm_compat6526_flow_tunnel_srv6_srh_t *to)
{
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->next_hdr = from->next_hdr;
    to->tag = from->tag;
    to->egr_obj = from->egr_obj;
    to->valid_elements = from->valid_elements;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_srv6_srh_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_srv6_srh_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      options - (IN) Configuration option. Use See BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_xxx
 *      info - (INOUT) (IN/OUT) SRV6 header configuration parameters
 *      nofs - (IN) Number of SID in the SID list
 *      sid_list - (IN) SID list in the SRH header
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_srv6_srh_add(
    int unit,
    uint32 options,
    bcm_compat6526_flow_tunnel_srv6_srh_t *info,
    int nofs,
    bcm_ip6_t *sid_list)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_srv6_srh_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_srv6_srh_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_srv6_srh_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_srv6_srh_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_srv6_srh_add(unit, options, new_info, nofs, sid_list);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_srv6_srh_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_srv6_srh_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_srv6_srh_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) (IN/OUT) Tunnel initiator configuration parameters
 *      max_nofs - (IN) Max size of the sid_list array
 *      sid_list - (OUT) sid_list array
 *      count - (OUT) Actual size of the retrieved sid_list array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_srv6_srh_get(
    int unit,
    bcm_compat6526_flow_tunnel_srv6_srh_t *info,
    int max_nofs,
    bcm_ip6_t *sid_list,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_srv6_srh_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_srv6_srh_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_srv6_srh_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_srv6_srh_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_srv6_srh_get(unit, new_info, max_nofs, sid_list, count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_srv6_srh_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6526in_switch_pkt_protocol_match_t
 *
 * Purpose:
 *      Convert the bcm_switch_pkt_protocol_match_t datatype
 *      from <= 6.5.26 to SDK 6.5.27+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.26 version of the datatype.
 *      to   - (OUT) SDK 6.5.27+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526in_switch_pkt_protocol_match_t(
    bcm_compat6526_switch_pkt_protocol_match_t *from,
    bcm_switch_pkt_protocol_match_t *to)
{
    if (from && to) {
        bcm_switch_pkt_protocol_match_t_init(to);
        to->l2_iif_opaque_ctrl_id        = from->l2_iif_opaque_ctrl_id;
        to->l2_iif_opaque_ctrl_id_mask   = from->l2_iif_opaque_ctrl_id_mask;
        to->vfi                          = from->vfi;
        to->vfi_mask                     = from->vfi_mask;
        sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
        sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
        to->ethertype                    = from->ethertype;
        to->ethertype_mask               = from->ethertype_mask;
        to->arp_rarp_opcode              = from->arp_rarp_opcode;
        to->arp_rarp_opcode_mask         = from->arp_rarp_opcode_mask;
        to->icmp_type                    = from->icmp_type;
        to->icmp_type_mask               = from->icmp_type_mask;
        to->igmp_type                    = from->igmp_type;
        to->igmp_type_mask               = from->igmp_type_mask;
        to->l4_valid                     = from->l4_valid;
        to->l4_valid_mask                = from->l4_valid_mask;
        to->l4_src_port                  = from->l4_src_port;
        to->l4_src_port_mask             = from->l4_src_port_mask;
        to->l4_dst_port                  = from->l4_dst_port;
        to->l4_dst_port_mask             = from->l4_dst_port_mask;
        to->l5_bytes_0_1                 = from->l5_bytes_0_1;
        to->l5_bytes_0_1_mask            = from->l5_bytes_0_1_mask;
        to->outer_l5_bytes_0_1           = from->outer_l5_bytes_0_1;
        to->outer_l5_bytes_0_1_mask      = from->outer_l5_bytes_0_1_mask;
        to->ip_last_protocol             = from->ip_last_protocol;
        to->ip_last_protocol_mask        = from->ip_last_protocol_mask;
        to->fixed_hve_result_set_1       = from->fixed_hve_result_set_1;
        to->fixed_hve_result_set_1_mask  = from->fixed_hve_result_set_1_mask;
        to->fixed_hve_result_set_5       = from->fixed_hve_result_set_5;
        to->fixed_hve_result_set_5_mask  = from->fixed_hve_result_set_5_mask;
        to->flex_hve_result_set_1        = from->flex_hve_result_set_1;
        to->flex_hve_result_set_1_mask   = from->flex_hve_result_set_1_mask;
        to->tunnel_processing_results_1 =
            from->tunnel_processing_results_1;
        to->tunnel_processing_results_1_mask =
            from->tunnel_processing_results_1_mask;
        to->vfp_opaque_ctrl_id           = from->vfp_opaque_ctrl_id;
        to->vfp_opaque_ctrl_id_mask      = from->vfp_opaque_ctrl_id_mask;
        to->vlan_xlate_opaque_ctrl_id =
            from->vlan_xlate_opaque_ctrl_id;
        to->vlan_xlate_opaque_ctrl_id_mask =
            from->vlan_xlate_opaque_ctrl_id_mask;
        to->vlan_xlate_opaque_ctrl_id_1 =
            from->vlan_xlate_opaque_ctrl_id_1;
        to->vlan_xlate_opaque_ctrl_id_1_mask =
            from->vlan_xlate_opaque_ctrl_id_1_mask;
        to->l2_iif_opaque_ctrl_id_1      = from->l2_iif_opaque_ctrl_id_1;
        to->l2_iif_opaque_ctrl_id_1_mask = from->l2_iif_opaque_ctrl_id_1_mask;
        to->fixed_hve_result_set_4       = from->fixed_hve_result_set_4;
        to->fixed_hve_result_set_4_mask  = from->fixed_hve_result_set_4_mask;
        to->outer_l2_valid_otag_present =
            from->outer_l2_valid_otag_present;
        to->outer_l2_valid_otag_present_mask =
            from->outer_l2_valid_otag_present_mask;
        to->ovid                         = from->ovid;
        to->ovid_mask                    = from->ovid_mask;
        to->ip_hdr_protocol              = from->ip_hdr_protocol;
        to->ip_hdr_protocol_mask         = from->ip_hdr_protocol_mask;
        to->ingress_pp_port              = from->ingress_pp_port;
        to->ingress_pp_port_mask         = from->ingress_pp_port_mask;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_switch_pkt_protocol_match_t
 *
 * Purpose:
 *      Convert the bcm_switch_pkt_protocol_match_t datatype
 *      from SDK 6.5.27+ to <=6.5.26.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.27+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.26 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526out_switch_pkt_protocol_match_t(
    bcm_switch_pkt_protocol_match_t *from,
    bcm_compat6526_switch_pkt_protocol_match_t *to)
{
    if (from && to) {
        to->l2_iif_opaque_ctrl_id        = from->l2_iif_opaque_ctrl_id;
        to->l2_iif_opaque_ctrl_id_mask   = from->l2_iif_opaque_ctrl_id_mask;
        to->vfi                          = from->vfi;
        to->vfi_mask                     = from->vfi_mask;
        sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
        sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
        to->ethertype                    = from->ethertype;
        to->ethertype_mask               = from->ethertype_mask;
        to->arp_rarp_opcode              = from->arp_rarp_opcode;
        to->arp_rarp_opcode_mask         = from->arp_rarp_opcode_mask;
        to->icmp_type                    = from->icmp_type;
        to->icmp_type_mask               = from->icmp_type_mask;
        to->igmp_type                    = from->igmp_type;
        to->igmp_type_mask               = from->igmp_type_mask;
        to->l4_valid                     = from->l4_valid;
        to->l4_valid_mask                = from->l4_valid_mask;
        to->l4_src_port                  = from->l4_src_port;
        to->l4_src_port_mask             = from->l4_src_port_mask;
        to->l4_dst_port                  = from->l4_dst_port;
        to->l4_dst_port_mask             = from->l4_dst_port_mask;
        to->l5_bytes_0_1                 = from->l5_bytes_0_1;
        to->l5_bytes_0_1_mask            = from->l5_bytes_0_1_mask;
        to->outer_l5_bytes_0_1           = from->outer_l5_bytes_0_1;
        to->outer_l5_bytes_0_1_mask      = from->outer_l5_bytes_0_1_mask;
        to->ip_last_protocol             = from->ip_last_protocol;
        to->ip_last_protocol_mask        = from->ip_last_protocol_mask;
        to->fixed_hve_result_set_1       = from->fixed_hve_result_set_1;
        to->fixed_hve_result_set_1_mask  = from->fixed_hve_result_set_1_mask;
        to->fixed_hve_result_set_5       = from->fixed_hve_result_set_5;
        to->fixed_hve_result_set_5_mask  = from->fixed_hve_result_set_5_mask;
        to->flex_hve_result_set_1        = from->flex_hve_result_set_1;
        to->flex_hve_result_set_1_mask   = from->flex_hve_result_set_1_mask;
        to->tunnel_processing_results_1 =
            from->tunnel_processing_results_1;
        to->tunnel_processing_results_1_mask =
            from->tunnel_processing_results_1_mask;
        to->vfp_opaque_ctrl_id           = from->vfp_opaque_ctrl_id;
        to->vfp_opaque_ctrl_id_mask      = from->vfp_opaque_ctrl_id_mask;
        to->vlan_xlate_opaque_ctrl_id =
            from->vlan_xlate_opaque_ctrl_id;
        to->vlan_xlate_opaque_ctrl_id_mask =
            from->vlan_xlate_opaque_ctrl_id_mask;
        to->vlan_xlate_opaque_ctrl_id_1 =
            from->vlan_xlate_opaque_ctrl_id_1;
        to->vlan_xlate_opaque_ctrl_id_1_mask =
            from->vlan_xlate_opaque_ctrl_id_1_mask;
        to->l2_iif_opaque_ctrl_id_1      = from->l2_iif_opaque_ctrl_id_1;
        to->l2_iif_opaque_ctrl_id_1_mask = from->l2_iif_opaque_ctrl_id_1_mask;
        to->fixed_hve_result_set_4       = from->fixed_hve_result_set_4;
        to->fixed_hve_result_set_4_mask  = from->fixed_hve_result_set_4_mask;
        to->outer_l2_valid_otag_present =
            from->outer_l2_valid_otag_present;
        to->outer_l2_valid_otag_present_mask =
            from->outer_l2_valid_otag_present_mask;
        to->ovid                         = from->ovid;
        to->ovid_mask                    = from->ovid_mask;
        to->ip_hdr_protocol              = from->ip_hdr_protocol;
        to->ip_hdr_protocol_mask         = from->ip_hdr_protocol_mask;
        to->ingress_pp_port              = from->ingress_pp_port;
        to->ingress_pp_port_mask         = from->ingress_pp_port_mask;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6526_switch_pkt_protocol_control_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_pkt_protocol_control_add.
 *
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      options  - (IN) Options flags.
 *      match    - (IN) Packet protocol match info.
 *      action   - (IN) Packet control action info.
 *      priority - (IN) Priority.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6526_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = BCM_E_INTERNAL;
    bcm_switch_pkt_protocol_match_t *new = NULL;

    /* Validate input parameter. */
    if (match == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_switch_pkt_protocol_match_t *)sal_alloc(
              sizeof(*new), "bcmcompat6526SwitchPktProtocolMatchNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_switch_pkt_protocol_match_t ->
     *   bcm_switch_pkt_protocol_match_t.
     */
    (void)_bcm_compat6526in_switch_pkt_protocol_match_t(match, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_switch_pkt_protocol_control_add(unit, options, new, action,
                                             priority);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as match is of INOUT parameter type for this API.
         * - bcm_switch_pkt_protocol_match_t ->
         *   bcm_compat6526_switch_pkt_protocol_match_t
         */
        (void)_bcm_compat6526out_switch_pkt_protocol_match_t(new, match);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_switch_pkt_protocol_control_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_pkt_protocol_control_get.
 *
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      match    - (IN) Packet protocol match info.
 *      action   - (OUT) Packet control action info.
 *      priority - (OUT) Priority.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6526_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = BCM_E_INTERNAL;
    bcm_switch_pkt_protocol_match_t *new = NULL;

    /* Validate input parameter. */
    if (match == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_switch_pkt_protocol_match_t *)sal_alloc(
              sizeof(*new), "bcmcompat6526SwitchPktProtocolMatchNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_switch_pkt_protocol_match_t ->
     *   bcm_switch_pkt_protocol_match_t.
     */
    (void)_bcm_compat6526in_switch_pkt_protocol_match_t(match, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_switch_pkt_protocol_control_get(unit, new, action, priority);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as match is of INOUT parameter type for this API.
         * - bcm_switch_pkt_protocol_match_t ->
         *   bcm_compat6526_switch_pkt_protocol_match_t
         */
        (void)_bcm_compat6526out_switch_pkt_protocol_match_t(new, match);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_switch_pkt_protocol_control_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_pkt_protocol_control_delete.
 *
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      match    - (IN) Packet protocol match info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6526_switch_pkt_protocol_match_t *match)
{
    int rv = BCM_E_INTERNAL;
    bcm_switch_pkt_protocol_match_t *new = NULL;

    /* Validate input parameter. */
    if (match == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_switch_pkt_protocol_match_t *)sal_alloc(
              sizeof(*new), "bcmcompat6526SwitchPktProtocolMatchNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_switch_pkt_protocol_match_t ->
     *   bcm_switch_pkt_protocol_match_t.
     */
    (void)_bcm_compat6526in_switch_pkt_protocol_match_t(match, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_switch_pkt_protocol_control_delete(unit, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_vlan_translate_egress_key_config_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_key_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_vlan_translate_egress_key_config_t(
    bcm_compat6526_vlan_translate_egress_key_config_t *from,
    bcm_vlan_translate_egress_key_config_t *to)
{
    bcm_vlan_translate_egress_key_config_t_init(to);
    to->flags = from->flags;
    to->key_type = from->key_type;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->port = from->port;
    to->otag_preserve = from->otag_preserve;
    to->itag_preserve = from->itag_preserve;
    to->ingress_port = from->ingress_port;
    to->ingress_outer_vlan = from->ingress_outer_vlan;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_action_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 *      action - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_add(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_action_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_delete(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_action_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 *      action - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_get(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_flexctr_extended_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_attach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_flexctr_extended_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_flexctr_extended_detach_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach_all(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_translate_egress_flexctr_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      array_size - (IN) Size of allocated entries in config array.
 *      config_array - (OUT) Array of flex counter configurations.
 *      count - (OUT) Number of elements in config array. If array_size is 0, config array will be ignored, and count will be filled with the actual number.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6526_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_get(unit, new_key_config, array_size, config_array, count);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}
/*
 * Function:
 *      _bcm_compat6526in_switch_reflector_data_t
 * Purpose:
 *      Convert the bcm_switch_reflector_data_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_switch_reflector_data_t(
    bcm_compat6526_switch_reflector_data_t *from,
    bcm_switch_reflector_data_t *to)
{
    int i1 = 0;

    bcm_switch_reflector_data_t_init(to);
    to->type = from->type;
    for (i1 = 0; i1 < 6; i1++) {
        to->mc_reflector_my_mac[i1] = from->mc_reflector_my_mac[i1];
    }
    to->error_estimate = from->error_estimate;
    to->next_encap_id = from->next_encap_id;
    to->reflector_member_link_id = from->reflector_member_link_id;
}

/*
 * Function:
 *      _bcm_compat6526out_switch_reflector_data_t
 * Purpose:
 *      Convert the bcm_switch_reflector_data_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_switch_reflector_data_t(
    bcm_switch_reflector_data_t *from,
    bcm_compat6526_switch_reflector_data_t *to)
{
    int i1 = 0;

    to->type = from->type;
    for (i1 = 0; i1 < 6; i1++) {
        to->mc_reflector_my_mac[i1] = from->mc_reflector_my_mac[i1];
    }
    to->error_estimate = from->error_estimate;
    to->next_encap_id = from->next_encap_id;
    to->reflector_member_link_id = from->reflector_member_link_id;
}

/*
 * Function:
 *      bcm_compat6526_switch_reflector_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_reflector_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      flags - (IN) see BCM_SWITCH_REFLECTOR_X
 *      encap_id - (INOUT) the allocated destroyed encap ID
 *      data - (INOUT) data used for the allocated encap ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_switch_reflector_create(
    int unit,
    uint32 flags,
    bcm_if_t *encap_id,
    bcm_compat6526_switch_reflector_data_t *data)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t *new_data = NULL;

    if (data != NULL) {
        new_data = (bcm_switch_reflector_data_t *)
                     sal_alloc(sizeof(bcm_switch_reflector_data_t),
                     "New data");
        if (new_data == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_switch_reflector_data_t(data, new_data);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_reflector_create(unit, flags, encap_id, new_data);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_switch_reflector_data_t(new_data, data);

    /* Deallocate memory*/
    sal_free(new_data);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_switch_reflector_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_reflector_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) see BCM_SWITCH_REFLECTOR_X
 *      encap_id - (IN) the allocated destroyed encap ID
 *      data - (INOUT) data used for the allocated encap ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_switch_reflector_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_compat6526_switch_reflector_data_t *data)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t *new_data = NULL;

    if (data != NULL) {
        new_data = (bcm_switch_reflector_data_t *)
                     sal_alloc(sizeof(bcm_switch_reflector_data_t),
                     "New data");
        if (new_data == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_switch_reflector_data_t(data, new_data);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_reflector_get(unit, flags, encap_id, new_data);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_switch_reflector_data_t(new_data, data);

    /* Deallocate memory*/
    sal_free(new_data);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_pkt_dnx_udh_t
 * Purpose:
 *      Convert the bcm_pkt_dnx_udh_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_pkt_dnx_udh_t(
    bcm_compat6526_pkt_dnx_udh_t *from,
    bcm_pkt_dnx_udh_t *to)
{
    to->size = from->size;
    to->data = from->data;
}

/*
 * Function:
 *      _bcm_compat6526out_pkt_dnx_udh_t
 * Purpose:
 *      Convert the bcm_pkt_dnx_udh_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_pkt_dnx_udh_t(
    bcm_pkt_dnx_udh_t *from,
    bcm_compat6526_pkt_dnx_udh_t *to)
{
    to->size = from->size;
    to->data = from->data;
}

/*
 * Function:
 *      _bcm_compat6526in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_mirror_header_info_t(
    bcm_compat6526_mirror_header_info_t *from,
    bcm_mirror_header_info_t *to)
{
    int i1 = 0;

    bcm_mirror_header_info_t_init(to);
    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    sal_memcpy(&to->pp, &from->pp, sizeof(bcm_mirror_pkt_dnx_pp_header_t));
    for (i1 = 0; i1 < 4; i1++) {
        _bcm_compat6526in_pkt_dnx_udh_t(&from->udh[i1], &to->udh[i1]);
    }
}

/*
 * Function:
 *      _bcm_compat6526out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_mirror_header_info_t(
    bcm_mirror_header_info_t *from,
    bcm_compat6526_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    sal_memcpy(&to->pp, &from->pp, sizeof(bcm_mirror_pkt_dnx_pp_header_t));
    for (i1 = 0; i1 < 4; i1++) {
        _bcm_compat6526out_pkt_dnx_udh_t(&from->udh[i1], &to->udh[i1]);
    }
}

/*
 * Function:
 *      bcm_compat6526_mirror_header_info_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_MIRROR_DEST_* flags
 *      mirror_dest_id - (IN) (IN/OUT) Mirrored destination ID
 *      mirror_header_info - (IN) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6526_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_set(unit, flags, mirror_dest_id, new_mirror_header_info);


    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_mirror_header_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mirror_dest_id - (IN) Mirrored destination ID
 *      flags - (INOUT) (IN/OUT) BCM_MIRROR_DEST_* flags
 *      mirror_header_info - (OUT) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6526_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_get(unit, mirror_dest_id, flags, new_mirror_header_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_switch_drop_event_mon_t
 * Purpose:
 *      Convert the bcm_switch_drop_event_mon_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_switch_drop_event_mon_t(
    bcm_compat6526_switch_drop_event_mon_t *from,
    bcm_switch_drop_event_mon_t *to)
{
    bcm_switch_drop_event_mon_t_init(to);
    to->drop_event = from->drop_event;
    to->actions = from->actions;
    to->flags = from->flags;
}

/*
 * Function:
 *      _bcm_compat6526out_switch_drop_event_mon_t
 * Purpose:
 *      Convert the bcm_switch_drop_event_mon_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_switch_drop_event_mon_t(
    bcm_switch_drop_event_mon_t *from,
    bcm_compat6526_switch_drop_event_mon_t *to)
{
    to->drop_event = from->drop_event;
    to->actions = from->actions;
    to->flags = from->flags;
}

/*
 * Function:
 *      bcm_compat6526_switch_drop_event_mon_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_drop_event_mon_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      monitor - (IN) (for set) Pointer to a switch drop event monitor structure.<br>(IN/OUT) (for get) Pointer to a switch drop event monitor structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_switch_drop_event_mon_set(
    int unit,
    bcm_compat6526_switch_drop_event_mon_t *monitor)
{
    int rv = BCM_E_NONE;
    bcm_switch_drop_event_mon_t *new_monitor = NULL;

    if (monitor != NULL) {
        new_monitor = (bcm_switch_drop_event_mon_t *)
                     sal_alloc(sizeof(bcm_switch_drop_event_mon_t),
                     "New monitor");
        if (new_monitor == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_switch_drop_event_mon_t(monitor, new_monitor);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_drop_event_mon_set(unit, new_monitor);


    /* Deallocate memory*/
    sal_free(new_monitor);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_switch_drop_event_mon_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_drop_event_mon_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      monitor - (INOUT) (for set) Pointer to a switch drop event monitor structure.<br>(IN/OUT) (for get) Pointer to a switch drop event monitor structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_switch_drop_event_mon_get(
    int unit,
    bcm_compat6526_switch_drop_event_mon_t *monitor)
{
    int rv = BCM_E_NONE;
    bcm_switch_drop_event_mon_t *new_monitor = NULL;

    if (monitor != NULL) {
        new_monitor = (bcm_switch_drop_event_mon_t *)
                     sal_alloc(sizeof(bcm_switch_drop_event_mon_t),
                     "New monitor");
        if (new_monitor == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_switch_drop_event_mon_t(monitor, new_monitor);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_drop_event_mon_get(unit, new_monitor);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_switch_drop_event_mon_t(new_monitor, monitor);

    /* Deallocate memory*/
    sal_free(new_monitor);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_sat_config_t
 * Purpose:
 *      Convert the bcm_sat_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_sat_config_t(
    bcm_compat6526_sat_config_t *from,
    bcm_sat_config_t *to)
{
    bcm_sat_config_t_init(to);
    to->timestamp_format = from->timestamp_format;
    to->config_flags = from->config_flags;
    to->seq_number_wrap_around = from->seq_number_wrap_around;
}

/*
 * Function:
 *      _bcm_compat6526out_sat_config_t
 * Purpose:
 *      Convert the bcm_sat_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_sat_config_t(
    bcm_sat_config_t *from,
    bcm_compat6526_sat_config_t *to)
{
    to->timestamp_format = from->timestamp_format;
    to->config_flags = from->config_flags;
    to->seq_number_wrap_around = from->seq_number_wrap_around;
}

/*
 * Function:
 *      bcm_compat6526_sat_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_sat_config_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      conf - (IN) Pointer to SAT config structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_sat_config_set(
    int unit,
    bcm_compat6526_sat_config_t *conf)
{
    int rv = BCM_E_NONE;
    bcm_sat_config_t *new_conf = NULL;

    if (conf != NULL) {
        new_conf = (bcm_sat_config_t *)
                     sal_alloc(sizeof(bcm_sat_config_t),
                     "New conf");
        if (new_conf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_sat_config_t(conf, new_conf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_sat_config_set(unit, new_conf);


    /* Deallocate memory*/
    sal_free(new_conf);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_sat_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_sat_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      conf - (OUT) Pointer to SAT config structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_sat_config_get(
    int unit,
    bcm_compat6526_sat_config_t *conf)
{
    int rv = BCM_E_NONE;
    bcm_sat_config_t *new_conf = NULL;

    if (conf != NULL) {
        new_conf = (bcm_sat_config_t *)
                     sal_alloc(sizeof(bcm_sat_config_t),
                     "New conf");
        if (new_conf == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_sat_config_get(unit, new_conf);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_sat_config_t(new_conf, conf);

    /* Deallocate memory*/
    sal_free(new_conf);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from SDK <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_udf_pkt_format_info_t(
    bcm_compat6526_udf_pkt_format_info_t *from,
    bcm_udf_pkt_format_info_t *to)
{
    bcm_udf_pkt_format_info_t_init(to);
    to->prio = from->prio;
    to->ethertype = from->ethertype;
    to->ethertype_mask = from->ethertype_mask;
    to->ip_protocol = from->ip_protocol;
    to->ip_protocol_mask = from->ip_protocol_mask;
    to->l2 = from->l2;
    to->vlan_tag = from->vlan_tag;
    to->outer_ip = from->outer_ip;
    to->inner_ip = from->inner_ip;
    to->tunnel = from->tunnel;
    to->mpls = from->mpls;
    to->fibre_chan_outer = from->fibre_chan_outer;
    to->fibre_chan_inner = from->fibre_chan_inner;
    to->higig = from->higig;
    to->vntag = from->vntag;
    to->etag = from->etag;
    to->cntag = from->cntag;
    to->icnm = from->icnm;
    to->subport_tag = from->subport_tag;
    to->class_id = from->class_id;
    to->inner_protocol = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port = from->l4_dst_port;
    to->l4_dst_port_mask = from->l4_dst_port_mask;
    to->opaque_tag_type = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt = from->int_pkt;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->lb_pkt_type = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    to->ip_gre_first_2bytes = from->ip_gre_first_2bytes;
    to->ip_gre_first_2bytes_mask = from->ip_gre_first_2bytes_mask;
    to->dcn_pkt = from->dcn_pkt;
    to->dcn_pkt_mask = from->dcn_pkt_mask;
    to->byte_after_bos_label = from->byte_after_bos_label;
    to->byte_after_bos_label_mask = from->byte_after_bos_label_mask;
    to->dcn_shim_extn_byte = from->dcn_shim_extn_byte;
    to->dcn_shim_extn_byte_mask = from->dcn_shim_extn_byte_mask;
    to->dcn_shim_flags = from->dcn_shim_flags;
    to->dcn_shim_flags_mask = from->dcn_shim_flags_mask;
    to->gsh_header_type = from->gsh_header_type;
    to->module_header_type = from->module_header_type;
    to->svtag_present  = from->svtag_present;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from SDK 6.5.27+ to
 *      SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_udf_pkt_format_info_t(
    bcm_udf_pkt_format_info_t *from,
    bcm_compat6526_udf_pkt_format_info_t *to)
{
    to->prio = from->prio;
    to->ethertype = from->ethertype;
    to->ethertype_mask = from->ethertype_mask;
    to->ip_protocol = from->ip_protocol;
    to->ip_protocol_mask = from->ip_protocol_mask;
    to->l2 = from->l2;
    to->vlan_tag = from->vlan_tag;
    to->outer_ip = from->outer_ip;
    to->inner_ip = from->inner_ip;
    to->tunnel = from->tunnel;
    to->mpls = from->mpls;
    to->fibre_chan_outer = from->fibre_chan_outer;
    to->fibre_chan_inner = from->fibre_chan_inner;
    to->higig = from->higig;
    to->vntag = from->vntag;
    to->etag = from->etag;
    to->cntag = from->cntag;
    to->icnm = from->icnm;
    to->subport_tag = from->subport_tag;
    to->class_id = from->class_id;
    to->inner_protocol = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port = from->l4_dst_port;
    to->l4_dst_port_mask = from->l4_dst_port_mask;
    to->opaque_tag_type = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt = from->int_pkt;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->lb_pkt_type = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    to->ip_gre_first_2bytes = from->ip_gre_first_2bytes;
    to->ip_gre_first_2bytes_mask = from->ip_gre_first_2bytes_mask;
    to->dcn_pkt = from->dcn_pkt;
    to->dcn_pkt_mask = from->dcn_pkt_mask;
    to->byte_after_bos_label = from->byte_after_bos_label;
    to->byte_after_bos_label_mask = from->byte_after_bos_label_mask;
    to->dcn_shim_extn_byte = from->dcn_shim_extn_byte;
    to->dcn_shim_extn_byte_mask = from->dcn_shim_extn_byte_mask;
    to->dcn_shim_flags = from->dcn_shim_flags;
    to->dcn_shim_flags_mask = from->dcn_shim_flags_mask;
    to->gsh_header_type = from->gsh_header_type;
    to->module_header_type = from->module_header_type;
    to->svtag_present  = from->svtag_present;
    return;
}

int
bcm_compat6526_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6526_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_create(unit, options,
            new_pkt_format, pkt_format_id);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6526out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

int
bcm_compat6526_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6526_udf_pkt_format_info_t *pkt_format)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_info_get(unit, pkt_format_id, new_pkt_format);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6526out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

STATIC void
_bcm_compat6526in_field_action_params_t(
    bcm_compat6526_field_action_params_t *from,
    bcm_field_action_params_t *to)
{
    to->param0 = from->param0;
    to->param1 = from->param1;
    to->param2 = from->param2;
    to->pbmp = from->pbmp;
}

STATIC void
_bcm_compat6526out_field_action_params_t(
    bcm_field_action_params_t *from,
    bcm_compat6526_field_action_params_t *to)
{
    to->param0 = from->param0;
    to->param1 = from->param1;
    to->param2 = from->param2;
    to->pbmp = from->pbmp;
}

/* Add an action to a field entry */
int bcm_compat6526_field_action_config_info_add(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_compat6526_field_action_params_t *params, 
    bcm_field_action_match_config_t *match_config)
{
    int rv = BCM_E_NONE;
    bcm_field_action_params_t *new_params = NULL;

    if (params != NULL) {
        new_params = (bcm_field_action_params_t *)
                     sal_alloc(sizeof(bcm_field_action_params_t),
                     "New params");
        if (new_params == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_field_action_params_t(params, new_params);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_config_info_add(unit, entry, action, new_params, match_config);


    /* Deallocate memory*/
    sal_free(new_params);

    return rv;
}

/* 
 * Retrieve the parameters for an action previously added to a field
 * entry
 */
int bcm_compat6526_field_action_config_info_get(
    int unit, 
    bcm_field_entry_t entry, 
    bcm_field_action_t action, 
    bcm_compat6526_field_action_params_t *params, 
    bcm_field_action_match_config_t *match_config)
{
    int rv = BCM_E_NONE;
    bcm_field_action_params_t *new_params = NULL;

    if (params != NULL) {
        new_params = (bcm_field_action_params_t *)
                     sal_alloc(sizeof(bcm_field_action_params_t),
                     "New params");
        if (new_params == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_config_info_get(unit, entry, action, new_params, match_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_field_action_params_t(new_params, params);

    /* Deallocate memory*/
    sal_free(new_params);

    return rv;
}
#endif /* BCM_RPC_SUPPORT */
