/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * This file provides VLAN-Port creation and modification functions that supprt various types of ACs 
 * by supplying flags that represent the required AC type.                                           */ 

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */

uint32 DNX_FAILOVER_VAL_MASK=0x1FFFFFFF;

/* Default values for fields */
bcm_vlan_t DEFAULT_VLAN=100;
bcm_vlan_port_match_t DEFAULT_CRITERIA=BCM_VLAN_PORT_MATCH_PORT_VLAN;
bcm_failover_t DEFAULT_INGRESS_FAILOVER_ID=5;
bcm_failover_t DEFAULT_FEC_FAILOVER_ID=25;
bcm_multicast_t DEFAULT_INGRESS_MC_GROUP=22;
uint32 DEFAULT_MACT_GROUP=3;
bcm_if_t ALLOCATED_FEC_ID=0;

/* Flags for AC Types */
uint32 AC_TYPE_WIDE_DATA=0x00000001;
uint32 AC_TYPE_STATS=0x00000002;
uint32 AC_TYPE_INGRESS_PROTECTION=0x00000004;
uint32 AC_TYPE_FEC_PROTECTION=0x00000008;
uint32 AC_TYPE_INGRESS_MC=0x00000010;
uint32 AC_TYPE_MACT_GROUP=0x00000020;

/*
 * Wrapper for the AC creation according to unique AC type configuration that may
 * result in specific result types.
 *
 * INPUT: 
 *   port: phy port
 *   vlan: Outer VID
 *   vsi: VSI to associate
 *   direction_flags: BCM_VLAN_PORT_CREATE_INGRESS_ONLY / EGRESS_ONLY
 *   ac_type_flags - The unique attributes of the AC: WIDE_DATA / STATS / INGRESS_PROTECTION etc
 *   type_data - Additional data that the a specific ac_type may require
 *   vlan_port - The return VLAN-Port structure of the created AC
 */
int vlan_port__create(int unit,
                      bcm_gport_t port,
                      bcm_vlan_t vlan,
                      bcm_vlan_t vsi,
                      uint32 direction_flags,
                      uint32 ac_type_flags,
                      void *type_data,
                      bcm_vlan_port_t *vlan_port)
{
    int rv;
    bcm_l3_egress_t l3_egress;
    bcm_if_t fec_standby_id, fec_primay_id;

    bcm_vlan_port_t_init(vlan_port);

    /*
     * Configure default match 
     */
    vlan_port->port = port;
    vlan_port->criteria = DEFAULT_CRITERIA;
    vlan_port->match_vlan = vlan;
    vlan_port->vsi = vsi;

    /*
     * Configure direction flags
     */
    vlan_port->flags |= direction_flags;

    /*
     * Configure specific type fields
     */
    if (ac_type_flags & AC_TYPE_WIDE_DATA) {
        vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }
    if (ac_type_flags & AC_TYPE_STATS) {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }
    if (ac_type_flags & AC_TYPE_INGRESS_PROTECTION) {
        bcm_failover_t failover_id = DEFAULT_INGRESS_FAILOVER_ID;

        rv = bcm_failover_create(unit, BCM_FAILOVER_INGRESS | BCM_FAILOVER_WITH_ID, &failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_create failed for Ingress Protection with failover_id - %d, rv - %d\n", failover_id, rv);
            return rv;
        }
        vlan_port->ingress_failover_id = failover_id;

        rv = bcm_failover_set(unit, failover_id, FALSE);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_set failed for Ingress failover_id - %d, rv - %d\n", failover_id, rv);
            return rv;
        }
    }
    if (ac_type_flags & AC_TYPE_FEC_PROTECTION) {
        bcm_failover_t failover_id = DEFAULT_FEC_FAILOVER_ID;

        rv = bcm_failover_create(unit, BCM_FAILOVER_FEC | BCM_FAILOVER_WITH_ID, &failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_create failed for Ingress Protection with failover_id - %d, rv - %d\n", failover_id, rv);
            return rv;
        }
        rv = bcm_failover_set(unit, failover_id, TRUE);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_failover_set failed for FEC failover_id - %d, rv - %d\n", failover_id, rv);
            return rv;
        }

        /*
         * Create a Secondary FEC
         */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.port = port;
        l3_egress.failover_id = failover_id;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egress, &fec_standby_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_create failed for Secondary FEC with ID - %d, rv - %d\n", fec_standby_id, rv);
            return rv;
        }
        ALLOCATED_FEC_ID = fec_standby_id;
        printf("fec_standby_id = %d\n", fec_standby_id);

        /*
         * Create a Primary FEC
         */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.port = port;
        l3_egress.failover_id = failover_id;
        l3_egress.failover_if_id = fec_standby_id;
        fec_primay_id = fec_standby_id - 1;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_primay_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_create failed for Primary FEC with ID - %d, rv - %d\n", fec_primay_id, rv);
            return rv;
        }
        printf("fec_primay_id = %d\n", fec_primay_id);
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(vlan_port->failover_port_id, fec_primay_id);
    }
    if (ac_type_flags & AC_TYPE_INGRESS_MC) {
        bcm_multicast_t mc_group_id;
        /* 
         * Allocate an Ingress protection MC group
         */
        mc_group_id = DEFAULT_INGRESS_MC_GROUP;
        rv = multicast__open_mc_group(unit, &mc_group_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__open_mc_group failed for group %d, rv - %d\n", mc_group_id, rc);
            return rv;
        }
        vlan_port->failover_mc_group = mc_group_id;
    }
    if (ac_type_flags & AC_TYPE_MACT_GROUP) {
        vlan_port->group = DEFAULT_MACT_GROUP;
    }

    /* 
     * Create the VLAN-Port
     */
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create failed, rv - %d\n", rv);
        return rv;
    }
    printf("vlan_port->vlan_port_id = %d\n", vlan_port->vlan_port_id);

    /*
     * Adjustments following the VLAN-Port creation
     */
    if (ac_type_flags & AC_TYPE_FEC_PROTECTION) {

        /* Set the Out-LIF in the Primary FEC entry */
        l3_egress.intf = vlan_port->vlan_port_id;
        l3_egress.flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_primay_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_create failed when adding Out-AC to the Primary FEC, rv - %d\n", fec_primay_id, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}


/*
 * The function retrieves unique ac_type configurations that were applied on a
 * given gport.
 *
 * INPUT: 
 *   gport: AC gport
 *   ac_type_flags - The returned unique attributes of the AC: WIDE_DATA / STATS / INGRESS_PROTECTION etc
 */
int vlan_port__ac_type_get(int unit,
                           bcm_gport_t gport,
                           uint32 *ac_type_flags)
{
    bcm_vlan_port_t vlan_port;
    int rv;

    /*
     * Retrieve the VLAN-Port structure
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = gport;

    rv = bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_find failed for gport - %d, rv - %d\n", gport, rv);
        return rv;
    }

    /*
     * Check what ac_type are represented by the VLAN-Port
     */
    *ac_type_flags = 0;
    if (vlan_port.flags & BCM_VLAN_PORT_INGRESS_WIDE) {
        *ac_type_flags |= AC_TYPE_WIDE_DATA;
    }
    if (vlan_port.flags & BCM_VLAN_PORT_STAT_INGRESS_ENABLE) {
        *ac_type_flags |= AC_TYPE_STATS;
    }
    if ((vlan_port.ingress_failover_id & DNX_FAILOVER_VAL_MASK) == DEFAULT_INGRESS_FAILOVER_ID) {
        *ac_type_flags |= AC_TYPE_INGRESS_PROTECTION;
    }
    if (vlan_port.failover_port_id) {
        bcm_if_t fec_id;
        bcm_l3_egress_t l3_egress;

        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(fec_id, vlan_port.failover_port_id);
        printf("fec_id = %d\n", fec_id);

        /* Retrieve the FEC that is used for learning in order to verify it contains the gport as the Out-LIF */
        rv = bcm_l3_egress_get(unit, fec_id, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_egress_get failed for FEC-ID - %d, rv - %d\n", fec_id, rv);
            return rv;
        }

        if (BCM_FORWARD_ENCAP_ID_VAL_GET(l3_egress.intf) == BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(gport))) {
            *ac_type_flags |= AC_TYPE_FEC_PROTECTION;
        }
    }
    if (vlan_port.failover_mc_group == DEFAULT_INGRESS_MC_GROUP) {
        *ac_type_flags |= AC_TYPE_INGRESS_MC;
    }
    if (vlan_port.group == DEFAULT_MACT_GROUP) {
        *ac_type_flags |= AC_TYPE_MACT_GROUP;
    }

    return BCM_E_NONE;
}

