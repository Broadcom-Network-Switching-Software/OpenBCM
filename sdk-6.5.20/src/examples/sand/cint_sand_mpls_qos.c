/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cint QOS Setup example code for MPLS
 * In the CINT example we present 2 different functions of QOS in MPLS world:
 * 1. Mapping from Incoming MPLS-EXP label to Internal TC+DP. It will allow the user to set the traffic
 *    management COS according to MPLS label.
 * 2. Mapping from Internal nwk_qos to Outgoing MPLS label. Allow to signal nwk_qos mapping into the
 *    MPLS label.
 *
 * copy to /usr/local/sbin location, run bcm.user
 *
 * Run script:
 *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/sand/cint_sand_mpls_qos.c
 * cint ../../../../src/examples/dpp/cint_mpls_lsr.c
 * c
 * int unit = 0;
 * int inP = 201;
 * int outP = 202;
 * dnx_qos_run(unit,inP,outP);
 *
 * Traffic example, MPLS LSR:
 *  The formation of the packet and the received packet are determined by the activation of qos_run.
 *  Ethernet DA: 00:00:00:00:00:11 , SA: 00:00:63:DE:1A:13 , VLAN: TPID 0x8100 , VID 100;   MPLS: Label:5000, EXP:0
 *  Expect:
 *  MPLS LABEL 8000, EXP 2
 *  Ethernet DA 00:00:00:00:00:22, SA 00:00:00:00:00:11
 *
 * Please note the the in-EXP can be substituted by any of the values in the following table.
 * Expect the matching out-EXP.
 */

   /*
    * Map ingress and egress as follows Ingress EXP INTERNAL TC/DP Egress EXP 0 0/GREEN 2 1 0/YELLOW 2 2 1/GREEN 3 3
    * 1/YELLOW 4 4 2/GREEN 5 5 2/RED 6 6 3/YELLOW 7 7 3/RED 7
    */
int qos_map_mpls_internal_pri[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };

bcm_color_t qos_map_mpls_internal_color[8] = { bcmColorGreen,
    bcmColorYellow,
    bcmColorGreen,
    bcmColorYellow,
    bcmColorGreen,
    bcmColorYellow,
    bcmColorYellow,
    bcmColorRed
};

int qos_map_mpls_remark_pri[8] = { 2, 2, 3, 4, 5, 6, 7, 7 };
static int mpls_in_map_id = 3;
static int mpls_eg_map_id = 1;
static int l3_in_map_id = 2;
static int l3_qos_in_internal_map_id = 200;
static int l3_qos_in_internal_map_id_phb = 201;
static int l3_qos_in_internal_map_id_remark = 202;
static int mpls_qos_in_internal_map_id = 100;
static int mpls_qos_eg_internal_map_id = 10;

/*  Map: Incoming-EXP (exp) -> TC (int_pri), DP (color)*/
int
dnx_qos_map_mpls_ingress_profile(
    int unit)
{
    bcm_qos_map_t mpls_in_map, l3_ing_map;
    int flags = 0;
    int mpls_exp;
    int rv = BCM_E_NONE;
    int opcode = mpls_qos_in_internal_map_id;

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);

    /** Create QOS profile */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID,
                            &mpls_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress mpls bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** Create opcode profile */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode);
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress mpls opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);
    mpls_in_map.opcode = opcode;
    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls ingress bcm_qos_map_add():REMARK OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    for (mpls_exp = 0; mpls_exp < 8; mpls_exp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&mpls_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color)
         */
        mpls_in_map.exp = mpls_exp;
        mpls_in_map.remark_int_pri = qos_map_mpls_remark_pri[mpls_exp];

        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_add(unit, flags, &mpls_in_map, opcode);
        if (rv != BCM_E_NONE)
        {
            printf("error in mpls ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
   }

    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
      /** Create opcode profile */
    opcode = mpls_qos_in_internal_map_id;
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB  | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode);
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress mpls bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);
    mpls_in_map.opcode = opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls ingress bcm_qos_map_add():PHB OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    for (mpls_exp = 0; mpls_exp < 8; mpls_exp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&mpls_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color), NWK_WOS
         */
        mpls_in_map.exp = mpls_exp;
        mpls_in_map.int_pri = qos_map_mpls_internal_pri[mpls_exp];
        mpls_in_map.color = qos_map_mpls_internal_color[mpls_exp];

        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_add(unit, flags, &mpls_in_map, opcode);
        if (rv != BCM_E_NONE)
        {
            printf("error in mpls ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* MAp: COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)*/
int
dnx_qos_map_mpls_egress_profile(
    int unit)
{
    bcm_qos_map_t mpls_eg_map;
    int flags = 0;
    int mpls_exp;
    int rv;
    bcm_color_t color;
    int exp = 0;

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    /** Create QOS profile */
    if (mpls_eg_map_id !=0)
    {
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &mpls_eg_map_id);
    }
    else
    {
        rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, mpls_eg_map_id, &mpls_eg_map_id);
    }

    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

        /** Create QOS profile */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &mpls_qos_eg_internal_map_id);

    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    mpls_eg_map.opcode = mpls_qos_eg_internal_map_id;
    rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP) */
    int color_index;
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 256; exp++)
    {

        for (color_index = 0; color_index < 2; color_index++)
        {
            mpls_eg_map.color = color_index;
            mpls_eg_map.int_pri = exp;
            /*
             * Egress mapping is:
             * qos_var - 1:1
             * Only yellow is mapped.
             */
            mpls_eg_map.exp = (exp + ((color_index + 1) % 2)) % 8;
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
            rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_qos_eg_internal_map_id);
            if (rv != BCM_E_NONE)
            {
                printf("error in mpls egress bcm_qos_map_add()\n");
                printf("rv is: $rv \n");
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /*
     * set second exp mapping, mpls_0_1[exp] uses qos_variable[2:0], mpls_0_2[exp] uses qos_variable[5:3]
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP)
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 255; exp++)
    {
        mpls_eg_map.color = 1;
        mpls_eg_map.int_pri = exp;
        /*
         * Egress mapping is:
         * qos_var - 1:1
         * Only yellow is mapped.
         */
        mpls_eg_map.exp = (exp+1) & 0x7;
        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_MPLS_SECOND;
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_qos_eg_internal_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    /*
     * set inheritance mapping,
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP)
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 255; exp++)
    {
        mpls_eg_map.color = 1;
        mpls_eg_map.int_pri = exp;
        /*
         * Egress inheritance mapping is:
         * Only yellow is mapped.
         * next stage qos is (qos-1)&7
         */
        mpls_eg_map.remark_color = 1;
        mpls_eg_map.remark_int_pri = (exp - 1) & 0x7;
        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_MPLS_SECOND;
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/*  Map: Incoming-EXP (exp) -> TC (int_pri), DP (color)*/
int
dpp_qos_map_mpls_ingress_profile(int unit)
{
    bcm_qos_map_t mpls_in_map, l3_ing_map;
    int flags = 0;
    int mpls_exp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&mpls_in_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &mpls_in_map_id);

    if (rv != BCM_E_NONE) {
	printf("error in ingress mpls bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
	return rv;
    }

    flags = BCM_QOS_MAP_MPLS;

    for (mpls_exp=0; mpls_exp<8; mpls_exp++) {

        /*
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color)
         */
        mpls_in_map.exp = mpls_exp;
        mpls_in_map.int_pri = qos_map_mpls_internal_pri[mpls_exp];
        mpls_in_map.color = qos_map_mpls_internal_color[mpls_exp];

        if (is_device_or_above(unit, ARAD)) {
            /* Incoming-EXP (exp) -> In-DSCP-EXP (remark_int_pri) */
            mpls_in_map.remark_int_pri = mpls_exp;
        }

        rv = bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in mpls ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.int_pri = mpls_exp; /* TC */
        l3_ing_map.color = qos_map_mpls_internal_color[mpls_exp]; /* DP */
        l3_ing_map.inherited_dscp_exp = 1;
        l3_ing_map.exp = mpls_exp;
        l3_ing_map.dscp = 100;
        l3_ing_map.remark_int_pri = 100; /* packet DSCP (TOS) */
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, mpls_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, rv is:%s\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* MAp: COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)*/
int
dpp_qos_map_mpls_egress_profile(int unit)
{
    bcm_qos_map_t mpls_eg_map;
    int flags = 0;
    int mpls_exp;
    int rv;
    bcm_color_t color;
    int exp = 0;
    int is_policer_color = soc_property_get(unit , "policer_color_resolution_mode",0);

    /* Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit, JERICHO)) {
        mpls_eg_map_id = 7;
    } else if (is_device_or_above(unit, ARAD)) {
        mpls_eg_map_id = 3;
    }

    rv = bcm_qos_map_create(unit, flags, &mpls_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in mpls egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    flags = BCM_QOS_MAP_MPLS;

    /* COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)
    */
    for (exp=0; exp<8; exp++) {
        mpls_eg_map.color = qos_map_mpls_internal_color[exp];
        if(is_policer_color && mpls_eg_map.color == bcmColorRed){
            mpls_eg_map.color = bcmColorYellow;
        }
        if (!is_device_or_above(unit, ARAD)) {
            /* In PetraB the color is one bit. */
            mpls_eg_map.color = mpls_eg_map.color & 1;
        }
        mpls_eg_map.int_pri = exp;
        mpls_eg_map.remark_int_pri = qos_map_mpls_remark_pri[exp];
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    printf("mpls_eg_map_id is: $mpls_eg_map_id \n");

    return rv;
}

/* Return the map id set up for egress - called from cint_mpls_lsr.c */
/* to set up tunnel.                                                 */
int
qos_map_id_mpls_egress_get(int unit)
{
  return mpls_eg_map_id;
}

int
qos_map_id_mpls_ingress_get(int unit)
{
  return mpls_in_map_id;
}

/* perform full mapping of: Incoming-EXP -> Outgoing-EXP */
int
dnx_qos_map_mpls(
    int unit,
    int is_ingress,
    int is_egress)
{
    int rv = BCM_E_NONE;

    if (is_ingress)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            rv = dnx_qos_map_mpls_ingress_profile(unit);
        }
        else
        {
            rv = dpp_qos_map_mpls_ingress_profile(unit);
        }
        if (rv)
        {
            printf("error setting up ingress qos profile\n");
            return rv;
        }
    }

    if (is_egress)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            rv = dnx_qos_map_mpls_egress_profile(unit);
        }
        else
        {
            rv = dpp_qos_map_mpls_egress_profile(unit);
        }
        if (rv)
        {
            printf("error setting up egress qos profile\n");
            return rv;
        }
    }
    return rv;
}
/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 * Need to load cint file: cint_vpls_mp_basic.c
 */
int
mpls_qos_map_gport(
    int unit,
    int map_ingress,
    int ingress_gport,
    int map_egress,
    int egress_gport,
    int use_gport_from_cint)
{
    int rv = BCM_E_NONE;
    int ingress_gport_set, egress_gport_set;

    if (use_gport_from_cint == 1)
    {
        ingress_gport_set = cint_vpls_basic_info.mpls_tunnel_id_ingress;
        egress_gport_set = cint_vpls_basic_info.mpls_tunnel_id;
    } else if (use_gport_from_cint == 2)
    {
        ingress_gport_set = cint_vpls_roo_basic_info.core_overlay_mpls_port_id_ing;
        egress_gport_set = cint_vpls_roo_basic_info.core_overlay_mpls_port_id_eg;
    }
    else
    {
        ingress_gport_set = ingress_gport;
        egress_gport_set = egress_gport;
    }
    if (map_ingress)
    {
        rv = bcm_qos_port_map_set(unit, ingress_gport_set, mpls_in_map_id, -1);
        if (rv != BCM_E_NONE)
        {
            printf("error bcm_qos_port_map_set setting up ingress gport(0x%08x) to map\n", ingress_gport_set);
            return rv;
        }
    }

    if (map_egress)
    {
        rv = bcm_qos_port_map_set(unit, egress_gport_set, -1, mpls_eg_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("error bcm_qos_port_map_set setting up egress gport(0x%08x) to map\n", egress_gport_set);
            return rv;
        }
    }

    return rv;
}

/* Main function of this Cint:
   * Perform full mapping of Incoming-EXP -> Outgoing-EXP according to the above specifications
   * Dump MPLS QOS mapping information relevant for this file
   * Activate the main function of the mpls application
   * Attach the QOS profile to the inRIF
*/
int
dnx_qos_run(int unit, int inPort, int outPort)
{
    bcm_l3_ingress_t l3i;
    int rv;
    int in_vlan = 100;
    bcm_vlan_port_t vlan_port;

    dnx_qos_map_mpls(unit, 1, 1);

    mpls_lsr_init(outPort,outPort, 0x11, 0x22, 5000, 8000, in_vlan, 200,0);

    mpls_lsr_config(&unit, 1, 0);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = inPort;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = in_vlan;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, in_vlan, inPort, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vlan_gport_add \n");
        return rv;
    }
    /* Get the configuration of the InRIF. */
    rv = bcm_l3_ingress_get(unit, in_vlan, &l3i);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n");
        printf("bcm_l3_ingress_get(%d, %d, l3i)\n", unit, in_vlan);
        print l3i;
        print rv;
        return rv;
    }

    /* Attch the ingress QoS profile. */
    l3i.qos_map_id = mpls_in_map_id;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3i, in_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n");
        printf("bcm_l3_ingress_create(%d, l3i, %d)\n", unit, in_vlan);
        print l3i;
        print rv;
        return rv;
    }

    return 0;
}
/* For JR2 devices
 * L3 ingress map:
 * Incoming-DSCP-> TC (int_pri), DP (color)
 */
int
dnx_qos_map_l3_ingress_profile(
    int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;
    int opcode = l3_qos_in_internal_map_id_remark;

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /** Create QOS profile */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID,
                            &l3_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** Create opcode profile */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode);
    l3_qos_in_internal_map_id_remark = opcode;
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = opcode;
    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in l3 ingress bcm_qos_map_add():REMARK OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    for (dscp = 0; dscp < 256; dscp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&l3_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming (dscp) -> TC (int_pri), DP (color)
         */
        l3_in_map.dscp = dscp  ;
        l3_in_map.remark_int_pri = dscp ;

        flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, opcode);
        if (rv != BCM_E_NONE)
        {
            printf("error in l3 ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
   }

    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
      /** Create opcode profile */
    opcode = l3_qos_in_internal_map_id_phb;
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB  | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode);
    l3_qos_in_internal_map_id_phb = opcode;
    if (rv != BCM_E_NONE)
    {
        printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in l3 ingress bcm_qos_map_add():PHB OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    for (dscp = 0; dscp < 256; dscp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&l3_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-DSCP -> TC (int_pri), DP (color), NWK_WOS
         */
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = dscp % 8;
        l3_in_map.color = 1;

        flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, opcode);
        if (rv != BCM_E_NONE)
        {
            printf("error in l3 ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}
/* for JR1 devices
 * ingress L3 map:
 * Incoming-DSCP -> TC (int_pri), DP (color)
 */
int
dpp_qos_map_l3_ingress_profile(int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&l3_in_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &l3_in_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;

    for (dscp=0; dscp<256; dscp++) {

        /*
         * Ingress COS profile Maps:
         * Incoming (dscp) -> TC (int_pri), DP (color)
         */
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = dscp % 8;
        l3_in_map.color = 1;

        if (is_device_or_above(unit, ARAD)) {
            /* Incoming-DSCP -> In-DSCP-EXP (remark_int_pri) */
            l3_in_map.remark_int_pri = dscp;
        }

        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in l3 ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6;

    for (dscp=0; dscp<256; dscp++) {

        /*
         * Ingress COS profile Maps:
         * Incoming (dscp) -> TC (int_pri), DP (color)
         */
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = dscp % 8;
        l3_in_map.color = 1;

        if (is_device_or_above(unit, ARAD)) {
            /* Incoming-DSCP -> In-DSCP-EXP (remark_int_pri) */
            l3_in_map.remark_int_pri = dscp;
        }

        rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in l3 ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

 /* for JR1 devices
  * egress mpls marking profile: COLOR,
  * INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)
  */

int
dpp_qos_map_mpls_egress_encap_profile(int unit)
{
    bcm_qos_map_t mpls_eg_map;
    int flags = 0;
    int rv;
    int i = 0;


    /* Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;

    rv = bcm_qos_map_create(unit, flags, &mpls_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in mpls egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    flags = BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_IPV4;

    /* INT_PRI(IN-DSCP_EXP) -> marking exp
    */
    for (i=0; i<256; i++) {

        mpls_eg_map.exp = (i + 1) % 8;
        mpls_eg_map.remark_int_pri = i;
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    flags = BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6;

    /* INT_PRI(IN-DSCP_EXP) -> marking exp
    */
    for (i=0; i<256; i++) {

        mpls_eg_map.exp = (i + 1) % 8;
        mpls_eg_map.remark_int_pri = i;
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    printf("mpls_eg_map_id is: %d \n", mpls_eg_map_id);

    return rv;
}

/** Set egress qos remark profile for mpls ingress swap case.*/
int
mpls_ingress_swap_remark_profile_set(int unit, int remark_profile)
{
    int rv = BCM_E_NONE;
    int qos_map_id;

    if (remark_profile == -1) {
        qos_map_id = mpls_eg_map_id;
    } else {
        rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, remark_profile, &qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_qos_map_id_get_by_profile\n");
            return rv;
        }
    }

    if (is_device_or_above(unit, JERICHO2)) {
        int multi_swap_command = *(dnxc_data_get(unit,  "headers", "feature", "fhei_mpls_cmd_from_eei", NULL));
        int is_jr2_mode  = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
        if (multi_swap_command && is_jr2_mode)
        {
            bcm_mpls_tunnel_switch_t entry;
            /**In lsr configure : mpls_lsr_info_1.in_label++*/
            entry.label = mpls_lsr_info_1.in_label - 1;
            rv= bcm_mpls_tunnel_switch_get(unit, &entry);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcmQosControlMplsIngressSwapRemarkProfile\n");
                return rv;
            }
            entry.qos_map_id = qos_map_id;
            entry.flags |= (BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_REPLACE);
            rv= bcm_mpls_tunnel_switch_create(unit, &entry);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcmQosControlMplsIngressSwapRemarkProfile\n");
                return rv;
            }
        }
        else
        {
            rv = bcm_qos_control_set(unit, 0, bcmQosControlMplsIngressSwapRemarkProfile, qos_map_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcmQosControlMplsIngressSwapRemarkProfile\n");
                return rv;
            }
        }
    } else {
        printf("Error, bcmQosControlMplsIngressSwapRemarkProfile is unavailable\n");
        return BCM_E_UNAVAIL;
    }

    return rv;
}

/** Set mpls explicit NULL label qos profile.*/
int
dnx_qos_mpls_explicit_null_label_profile_set(int unit, int map_id, int is_ipv6)
{
    int rv = BCM_E_NONE;

    if (is_ipv6)
    {
        rv = bcm_qos_control_set(unit, BCM_QOS_MAP_IPV6, bcmQosControlMplsExplicitNullIngressQosProfile, map_id); 
    }
    else
    {
        rv = bcm_qos_control_set(unit, BCM_QOS_MAP_IPV4, bcmQosControlMplsExplicitNullIngressQosProfile, map_id); 
    }
    
    return rv;
}

