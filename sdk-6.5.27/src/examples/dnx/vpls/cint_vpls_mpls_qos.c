/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_vpls_mpls_qos.c Purpose: utility for MPLS TANDEM over VPLS with QoS.
 */

/*
 *
 * Configuration:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
  cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_mpls_qos.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port = 201;
  int mode = 0;
  rv = vpls_mpls_qos_main(unit,in_port,out_port,mode);
  print rv;
 * Test Scenario - end
 *

 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined 'mode'
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 0 - basic with 1 MPLS label
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 1 MPLS and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5     ||Exp:6            ||                          ||                      ||
 *   |    |                         |                       ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 1 - basic with 2 MPLS labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 2 MPLS and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5         ||Exp:5     ||Exp:6             ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 2 - basic with 1 MPLS with 1 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 1 MPLS, 1 TANDEM and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x5656||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5         ||Exp:5         ||Exp:6           ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 */

int MPLS_1_TANDEM_0_MODE = 0;
int MPLS_2_TANDEM_0_MODE = 1;
int MPLS_1_TANDEM_1_MODE = 2;
int native_vlan_port_id;

int
vpls_mpls_qos_main(
    int unit,
    int port1,
    int port2,
    int mode)
{

    init_default_vpls_params(unit);

    /** set EXP to UNIFORM mode in egress */
    mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelUniform;

    /** set EXP Pipe mode in ingress */
    pwe_term.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    pwe_term.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    pwe_term.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    switch (mode)
    {
        case MPLS_1_TANDEM_0_MODE:
            outer_mpls_tunnel_index = 0;
            break;
        case MPLS_2_TANDEM_0_MODE:
            mpls_encap_tunnel[0].num_labels = 2;
            mpls_encap_tunnel[0].label[1] = 0x4444;

            outer_mpls_tunnel_index = 0;
            break;
        case MPLS_1_TANDEM_1_MODE:
            mpls_encap_tunnel[1].label[0] = 0x5656;
            mpls_encap_tunnel[1].num_labels = 1;
            mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel3;;
            mpls_encap_tunnel[1].tunnel_id = 8196;
            mpls_encap_tunnel[1].flags = mpls_encap_tunnel[0].flags | BCM_MPLS_EGRESS_LABEL_TANDEM;

            outer_mpls_tunnel_index = 1;
            break;
        default:
            printf("Error(%d), invalid test mode(%d) - valid range is %d-%d \n", rv, mode, MPLS_1_TANDEM_0_MODE,
                   MPLS_1_TANDEM_1_MODE);
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN_MSG(vpls_main(unit, port1, port2), "");

    /* Add VLAN translation for out AC to test AC PCP remark */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, ac_port.vlan_port_id,          /* outlif                  */
                                                                      0x9100,                        /* outer_tpid              */
                                                                      0x8100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionNone,         /* Inner tpid action none  */
                                                                      bcmVlanActionReplace,          /* outer_action            */
                                                                      bcmVlanActionNone,             /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionNone,             /* inner_pri_action        */
                                                                      100,                           /* new_outer_vid           */
                                                                      0,                             /* new_inner_vid           */
                                                                      3,                             /* vlan_edit_profile       */
                                                                      4,                             /* tag_format - here is stag */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    return BCM_E_NONE;
}

/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 */
int
vpls_mpls_qos_map_gport(
    int unit,
    int ingress_qos_profile,
    int egress_qos_profile)
{
    char error_msg[200]={0,};
    int i;
    int flags;
    bcm_gport_t gport;
    bcm_l3_egress_t l3eg;
    int ingress_qos_map_id;
    int egress_qos_map_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,
                                       ingress_qos_profile, &ingress_qos_map_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,
                                       egress_qos_profile, &egress_qos_map_id), "");

    if (ingress_qos_profile != -1)
    {
        snprintf(error_msg, sizeof(error_msg), "setting up ingress gport(0x%08x) to map", ac_port.vlan_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, ac_port.vlan_port_id, ingress_qos_map_id, -1), error_msg);
    }

    if (egress_qos_profile != -1)
    {
        /** set remark profile to pwe*/
        snprintf(error_msg, sizeof(error_msg), "setting up egress gport(0x%08x) to map", pwe_encap.mpls_port_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, pwe_encap.mpls_port_id, -1, egress_qos_map_id), error_msg);

        /** update tunnel with egress qos map id*/
        for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
        {
            mpls_encap_tunnel[i].qos_map_id = egress_qos_map_id;
            mpls_encap_tunnel[i].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        }

        BCM_IF_ERROR_RETURN_MSG(vpls_create_mpls_tunnel(unit, mpls_encap_tunnel), "");

        /** update link layer with egress qos map id*/
        for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
        {
            gport = mpls_encap_tunnel[i].l3_intf_id;
            print gport;
            if(BCM_L3_ITF_TYPE_IS_LIF(gport))
            {
                int flags;
                bcm_l3_egress_t l3eg;
                bcm_l3_egress_t_init(l3eg);
                BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit,gport,&l3eg), "");
                l3eg.qos_map_id = egress_qos_map_id;
                flags=BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY;
                BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit,flags,&l3eg,gport), "");
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class:
 * 2. Set a vlan translation command (replace outer) with the
 * following parameters: edit_profile=5, tag_format=1,
 * new_vlan=1111
 */
int
vpls_mpls_qos_ve_swap(
    int unit,
    int port,
    int is_ive)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    /*
     * configure ive swap
     */
    BCM_IF_ERROR_RETURN_MSG(ive_eve_translation_set(unit, ac_port.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone, 1111, 0, 5     /* edit_profile
                                  */ , 4 /* tag_format */ , is_ive), "");
    return BCM_E_NONE;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class
 * 2. Set a vlan translation command (replace outer, and add
 * priority parameters) with the following parameters:
 * edit_profile=5, tag_format=4, new_vlan=1111
 */
int
vpls_mpls_qos_ve_pri_action_swap(
    int unit,
    int port,
    int is_ive)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    /*
     * configure ive swap
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, ac_port.vlan_port_id, 0x8100, 0x09100, bcmVlanTpidActionNone, bcmVlanTpidActionNone, bcmVlanActionReplace, bcmVlanActionNone, bcmVlanActionAdd, bcmVlanActionNone, 1111, 0, 5     /* edit_profile
                                                                 */ , 4 /* tag_format */ ,
                                                                is_ive), "");
    return BCM_E_NONE;
}

/**create native ac for vlan translation*/
int
vpls_create_egress_native_ac(
    int unit,
    bcm_vpn_t vsi,
    bcm_gport_t mpls_port_id,
    bcm_gport_t * vlan_port_id)
{
    bcm_vlan_port_t vlan_port;
    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.vsi = vsi;
    vlan_port.port = mpls_port_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    /*
     * Save the created vlan_port_id
     */
    *vlan_port_id = vlan_port.vlan_port_id;

    printf("core native ac = 0x%x\n", *vlan_port_id);

    return BCM_E_NONE;
}

/*
 * create L2 egress qos profile, inheritance qos var 
 */
int vpls_l2_qos_egress_profile(
    int unit,
    int remark_profile,
    int remark_opcode)
{
    int flags;
    bcm_qos_map_t l2_eg_map;
    int qos_var;
    int dp;

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if (remark_profile != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &remark_profile), "");

    /*next layer*/
    flags = BCM_QOS_MAP_REMARK;
    for (dp = 0; dp < 4; dp++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&l2_eg_map);
            l2_eg_map.int_pri = qos_var;
            l2_eg_map.color = dp;
            if (qos_var >= 6 )
            {
                l2_eg_map.remark_int_pri = 5;
            }
            else
            {
                l2_eg_map.remark_int_pri = 6;
            }
            l2_eg_map.remark_color = dp;
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l2_eg_map, remark_profile), "");
        }
    }

    return BCM_E_NONE;
}


/*all egress remark profile and add mapping, used in case pipeNextNameSpace qos model*/
int vpls_mpls_qos_egress_profile(int unit, int profile_id)
{
    int egress_qos_profile;
    bcm_qos_map_t mpls_eg_map;
    int map_opcode;
    int flags = 0;
    int qos_var;
    int color;
    int exp = 0;

    /** Create QOS profile */
    if (profile_id !=0)
    {
        egress_qos_profile = profile_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &egress_qos_profile), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, 0, &egress_qos_profile), "");
    }
    /*save egress qos map id*/
    mpls_encap_tunnel[0].qos_map_id = egress_qos_profile;

    /** Create QOS opcode */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &map_opcode), "");

    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */
    bcm_qos_map_t_init(&mpls_eg_map);
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    mpls_eg_map.opcode = map_opcode;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, egress_qos_profile), "");

    /*
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-EXP) 
     * 6, 7 are exp which set in pw and tunnel separately 
     * remark out exp to 5
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (qos_var = 0; qos_var < 256; qos_var++)
    {
        for (color = 0; color < 4; color++)
        {
            /** Clear structure */
            bcm_qos_map_t_init(&mpls_eg_map);
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
            mpls_eg_map.color = color;
            mpls_eg_map.int_pri = qos_var;
            if (qos_var >=6 )
            {
                mpls_eg_map.exp = 5;
            }
            else
            {
                mpls_eg_map.exp = 6;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, map_opcode), "");
        }
    }

    /** next layer nwk_qos input
     *  0~5 inheritance to 6
     *  6~255 inheritance to 5
     */
    for (color = 0; color < 4; color++)
    {
        for (qos_var = 0; qos_var < 256; qos_var++)
        {
            bcm_qos_map_t_init(&mpls_eg_map);
            mpls_eg_map.int_pri = qos_var;
            mpls_eg_map.color = color;
            if (qos_var >= 6 )
            {
                mpls_eg_map.remark_int_pri = 5;
            }
            else
            {
                mpls_eg_map.remark_int_pri = 6;
            }
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, egress_qos_profile), "");
        }
    }
    return BCM_E_NONE;
}

/**qos pipe model test main function*/
int vpls_mpls_qos_model_main(
    int unit,
    int port1,
    int port2)
{

    init_default_vpls_params(unit);

    /** set EXP to UNIFORM mode in egress */
    mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    mpls_encap_tunnel[0].exp = 6;
    pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    pwe_encap.exp = 7;

    outer_mpls_tunnel_index = 0;

    BCM_IF_ERROR_RETURN_MSG(vpls_main(unit, port1, port2), "");

    /*vpn is 5 in cint_dnx_utils_vpls.c*/
    BCM_IF_ERROR_RETURN_MSG(vpls_create_egress_native_ac(unit, 5, pwe_encap.mpls_port_id, native_vlan_port_id), "");
    return BCM_E_NONE;
}

/*update pw qos model*/
int vpls_mpls_pwe_qos_model_update(int unit, int qos_model)
{
    int i;

    pwe_encap.egress_qos_model.egress_qos = qos_model;
    if ((qos_model != bcmQosEgressModelPipeNextNameSpace) &&
        (qos_model != bcmQosEgressModelPipeMyNameSpace))
    {
        pwe_encap.exp = 0;
    }
    pwe_encap.flags |= (BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_REPLACE);
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_encapsulation(unit, &pwe_encap), "");
    return BCM_E_NONE;
}

/*update tunnel qos model */
int vpls_mpls_tunnel_qos_model_update(int unit, int qos_model)
{
    int i;

    for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
    {
        mpls_encap_tunnel[i].egress_qos_model.egress_qos = qos_model;
        if ((qos_model != bcmQosEgressModelPipeNextNameSpace) &&
            (qos_model != bcmQosEgressModelPipeMyNameSpace))
        {
            mpls_encap_tunnel[i].exp = 0;
        }
        mpls_encap_tunnel[i].flags |= (BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE);
    }

    BCM_IF_ERROR_RETURN_MSG(vpls_create_mpls_tunnel(unit, mpls_encap_tunnel), "");
    return BCM_E_NONE;
}
