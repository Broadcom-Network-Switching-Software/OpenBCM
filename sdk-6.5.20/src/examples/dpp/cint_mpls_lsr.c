/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_lsr.c
 * Purpose: Example of the MPLS label switching router configuration.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                  . . .   . . .      . . .   . . .   . . .          /|         |
 *  |                         . . .   .     . .     .   .       .       .    +--------+  /          |
 *  |                  . .   .      .                 .                     /        /| /+-------+  |
 *  |                .     .                      +--------+               +--------+ |/ |Eth Pkt|  |
 *  |              .                             /        /|_ _ _ _ _ _ _ _|  LER   | |  +-------+  |
 *  |            .                              +--------+ |- - - - - - - -|        |/              |
 *  |             +--------+                    |  LSR   | |               +--------+               |
 *  |            /        /|                    |        |/ +---------------+      .                |
 *  | +-------+ +--------+ |                    +--------+  |Eth Pkt|Label 5|      .                |
 *  | |Eth Pkt| |  LER   | |  +---------------+    / /      +---------------+        .              |
 *  | +-------+ |        |/\  |Eth Pkt|Label 1|  / /                                   .            |
 *  | --------> +--------+ \ \+---------------+ / /                                    .            |
 *  |           .           \ \                / /                                    .             |
 *  |           .            \ \              / / +---------------+           .      .              |
 *  |          .              \ \ +--------+ / /  |Eth Pkt|Label 3|          .  . . .               |
 *  |          .               \/        /|/ /    +---------------+         .                       |
 *  |           . . .          +--------+ |/                               .                        |
 *  |                .         |  LSR   | |                   .           .                         |
 *  |                 .     .  |        |/                  .   .       .                           |
 *  |                   . .   .+--------+       .          .      . . .                             |
 *  |                          . . .    .     .   .       .                                         |
 *  |                               . .   . .       . . .                                           |
 *  |  +-----------+                                                                                |
 *  |  +-----------+ LSP            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                               |
 *  |                               | Figure 22: MPLS Network Model |                               |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Explanation:
 *  *   For incoming packet, the Ethernet header is terminated.
 *  *   Labels are swapped and forwarded to egress port with new lable and Ethernet encapsulations.
 *
 * Calling Sequence:
 *  *   Port TPIDs setting
 *      -   Assuming default TPID setting, that is, all ports configured with TPID 0x8100.
 *  *   Create MPLS L3 interface.
 *      -   Set interface with my-MAC 00:00:00:00:00:11 and VID 200.
 *      -   Packet routed to this L3 interface will be set with this MAC.
 *      -   Calls bcm_l3_intf_create().
 *  *   Create egress object points to the above l3-interface.
 *      -   Packet routed to this egress-object will be forwarded to egress port and encapsulated
 *          with Ethernet Header.
 *          *   DA: next-hop-DA.
 *          *   SA/VID: according to interface definition.
 *      -   Calls bcm_l3_egress_create().
 *  *  Add ILM (ingress label mapping) entry (refer to mpls_add_switch_entry).
 *      -   Scenario 1: Maps incoming label (5000) to egress label (8000) and points to egress object created above.
 *      -   Scenario 2: Maps incoming labels (400,1000) to egress labels (200,2000) .
 *      -   Flags.
 *          *   BCM_MPLS_SWITCH_TTL_DECREMENT has to be present.
 *          *   BCM_MPLS_SWITCH_ACTION_SWAP: for LSR functionality.
 *      -   Calls bcm_mpls_tunnel_switch_add().
 *
 * To Activate Above Settings Run (example of call):
 *  	BCM> cint ../sand/utility/cint_sand_utils_global.c
 *  	BCM> cint ../sand/utility/cint_sand_utils_mpls.c
 *      BCM> cint cint_qos.c
 *      BCM> cint cint_multi_device_utils.c
 *      BCM> cint utility/cint_utils_l2.c
 *      BCM> cint utility/cint_utils_l3.c
 *      BCM> cint cint_mpls_lsr.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> int nof_units = <nof_units>;
 *      cint> int units[nof_units] = {<unit1>, <unit2>,...};
 *      cint> int outP = 13;
 *      cint> int inP = 13;
 *      cint> int outSysport, inSysport;
 *      cint> port_to_system_port(unit1, outP, &outSysport);
 *      cint> port_to_system_port(unit2, inP, &inSysport);
 *  Scenario 1:
 *      cint> rv = mpls_lsr_run_with_defaults_multi_device(units, nof_units, inSysport, outSysport);
 *  Scenario 2:
 *      cint> rv = lsr_basic_example(&units,nof_units,in_Sysport,out_Sysport);
 * Traffic:
 *   Scenario 1:
 *   Sending packet from egress port 1:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
 *   |    | DA |SA||TIPD1 |Prio|VID||   MPLS   || Data |    |
 *   |    |0:11|  ||0x8100|    |100||Label:5000||      |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
 *   |   | Figure 23: Sending Packet from Egress Port 1 |   |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * * The packet will be received in egress port 1 with following header
 *   (label swapped, Ethernet header updated).
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
 *   |    |DA| SA ||TIPD1 |Prio|VID||   MPLS   || Data |    |
 *   |    |  |0:11||0x8100|    |   ||Label:8000||      |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
 *   |   | Figure 24: Packets Received on Egress Port 1 |   |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *   Scenario 2:
 *   Sending packet from egress port 1:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |    | DA |SA||TIPD1 |Prio|VID||   MPLS   ||   MPLS   || Data |    |
 *   |    |0:11|  ||0x8100|    |10 ||Label:400 ||Label:1000||      |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+    |
 *   |   | Figure 25: Sending Packet from Egress Port 1            |    |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * * The packet will be received in egress port 1 with following header
 *   (First MPLS is terminated (400), Second MPLS is swapped (1000->2000), add additional MPLS header (200)
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |    |DA|SA  ||TIPD1 |Prio|VID||   MPLS   ||   MPLS   || Data |    |
 *   |    |  |0:11||0x8100|    |10 ||Label:200 ||Label:2000||      |    |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+    |
 *   |   | Figure 26: Packet Received on Egress Port 1             |    |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Mpls Raw processing:
 * You can send and/or receive an mpls raw packet, (e.g without the preceding ethernet header)
 * by configuring mpls raw processing on the relevant port.
 * First, set relevant soc property: port_raw_mpls_enable = 1; Then, call:
 * enable_mpls_raw_processing(port);
 *
 * Remarks:
 *  *   You can add more label mappings using mpls_add_switch_entry().
 *  *   In EA1, building the Vlan-tag of  the new Ethernet header is not fully correct.
 *
 * Script adjustment:
 *  You can adjust the following attributes of the application:
 *  -  Vid, my_mac:     ethernet attribute, MPLS packets arrived on this VLAN with this MAD as DA,
 *                      will be L2 terminated and forwarded to MPLS engine
 *  -  in_label:        incoming label to Swap
 *  -  eg_label:        egress label
 *  -  eg_port:         physical port to forward packet to
 *  -  eg_vid:          VLAN id to stamp in new Ethernet header
 */


uint8 test_fec_format_c = 0;
uint8 test_fec_format_c_swap = 0;
int ingress_tunnel_id;          /* Set by mpls_add_term_entry */
int ingress_tunnel_id_indexed[3];       /* Set by mpls_add_term_entry in case of mpls_termination_label_index_enable */
int mpls_tunnel_label_out = 0;
int inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
int add_ll_label = 0;

void
mpls_lsr_init_aux(
    int in_sysport,
    int out_sysport,
    int my_mac_lsb,
    int next_hop_lsb,
    int in_label,
    int out_label,
    int in_vlan,
    int out_vlan,
    int out_to_tunnel,
    int pipe_mode_exp,
    int expected)
{

    int rv = BCM_E_NONE;

    mpls_pipe_mode_exp_set = pipe_mode_exp;

    mpls_exp = expected;
    ext_mpls_exp = expected;

    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vlan, out_vlan,
                  out_to_tunnel);
}

/*
 * Change the values of the access indexes from their defaults.
 * MUST be called before the main function
 */
int
mpls_set_access_indexes(int index_1, int index_2, int index_3)
{
	int rv = BCM_E_NONE;

	mpls_lsr_info_1.use_custom_index_1 = 0;
	mpls_lsr_info_1.use_custom_index_2 = 0;
	mpls_lsr_info_1.use_custom_index_3 = 0;

	if (index_1 > 0) {
		mpls_lsr_info_1.access_index_1 =  index_1;
		mpls_lsr_info_1.use_custom_index_1 = 1;
	}
	if (index_2 > 0) {
		mpls_lsr_info_1.access_index_2 =  index_2;
		mpls_lsr_info_1.use_custom_index_2 = 1;
	}
	if (index_3 > 0) {
		mpls_lsr_info_1.access_index_3 =  index_3;
		mpls_lsr_info_1.use_custom_index_3 = 1;
	}

	return rv;
}


/* Add switch entry to perform swap
 * Swap in_label with eg_label
 * Point to egress-object: egress_intf, returned by create_l3_egress
 */
int
mpls_add_switch_entry(
    int unit,
    int in_label,
    int eg_label,
    bcm_if_t egress_intf)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    int mpls_termination_label_index_enable;
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    if (test_fec_format_c && !is_device_or_above(unit, JERICHO2))
    {
        entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    }
    /*
     * TTL decrement has to be present 
     */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /*
     * Uniform: inherit TTL and EXP, in general valid options: both present (uniform) or none of them (Pipe) 
     */
    /*
     * On JR2, BCM_MPLS_SWITCH_OUTER_TTL and BCM_MPLS_SWITCH_OUTER_EXP are invalid
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }

    /*
     * incomming label 
     */
    entry.label = in_label;
    if (mpls_termination_label_index_enable)
    {
        BCM_MPLS_INDEXED_LABEL_SET(entry.label, in_label, 1);
    }

    /*
     * egress attribures
     */
    entry.egress_label.label = eg_label;

    entry.egress_if = egress_intf;
    entry.port = mpls_lsr_info_1.in_port;       /* This is relevant only in case of mpls context port */

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* Add switch entry to perform pop
*/
int
mpls_add_frr_switch_entry(int unit, int in_label)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    int mpls_termination_label_index_enable;
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);


    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;

    entry.flags |= BCM_MPLS_SWITCH_FRR;

    /* incomming label */
    entry.label = in_label;
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(entry.label, in_label, 4);
    }

    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* Add switch entry to perform PUSH
 * Push egress label.
 * Point to egress-object: egress_intf, returned by create_l3_egress
 * Push can be done in ingress (set is_ingress_push = 1) or egress (set set is_ingress_push = 0)
 */
int
mpls_add_switch_push_entry(
    int unit,
    int in_label,
    int eg_label,
    bcm_if_t egress_intf,
    int is_ingress_push)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);

    if (is_ingress_push)
    {   /* push is done in ingress */
        entry.action = BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH;

        entry.flags = 0;

        /*
         * PIPE: Set TTL and EXP,
         * in general valid options:
         * both present (uniform) or none of them (Pipe)
         */
        entry.egress_label.flags |= (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        if (!is_device_or_above(unit, ARAD_PLUS) || mpls_pipe_mode_exp_set)
        {
            entry.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
        else
        {
            entry.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
        entry.egress_label.ttl = 20;
        entry.egress_label.exp = mpls_exp;

        /*
         * egress attributes
         */
        entry.egress_label.label = eg_label;
    }
    else
    {   /* push is done in egress */
        entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
        entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    }

    /*
     * incoming label 
     */
    entry.label = in_label;
    entry.egress_if = egress_intf;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

int
mpls_add_term_entry_multi_device(
    int *units_id,
    int nof_units,
    int term_label,
    uint32 next_prtcl)
{
    int unit, i, rv;
    for (i = 0; i < nof_units; i++)
    {
        unit = units_id[i];
        rv = mpls_add_term_entry(unit, term_label, next_prtcl);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_term_entry in unit %d\n", unit);
            return rv;
        }
    }
    return rv;
}

/* Add term entry to perform pop
 */
int
mpls_add_term_entry_info(
    int unit,
    int term_label,
    int flags,
    uint32 next_prtcl,
    int *tunnel_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    int mpls_termination_label_index_enable;
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);

    entry.flags |= flags;
    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    /*
     * Next protocol indication:
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or
     * 0 - unset(MPLS)
     */

    if (is_device_or_above(unit, ARAD_PLUS))
    {
        if (next_prtcl & BCM_MPLS_SWITCH_EXPECT_BOS)
        {
            /*
             * In ARAD+ we dont use next protocol because it is extracted from next nibble, but we can use expect bos
             * indication 
             */
            entry.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
        }
    }
    else
    {
        entry.flags |= next_prtcl;
    }

    /*
     * incomming label 
     */
    entry.label = term_label;
    if (mpls_termination_label_index_enable)
    {
        BCM_MPLS_INDEXED_LABEL_SET(entry.label, term_label, 1);
    }

    /*
     * Enable when testing egress QOS, need to source cint_qos.c This remarks the mpls egress packet 
     */
    entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);

    entry.tunnel_id = *tunnel_id;
    entry.inlif_counting_profile = inlif_counting_profile;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    *tunnel_id = entry.tunnel_id;
    /*
     * In JR2 each tunnel will be assigned RIF properties
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * Set Incoming Tunnel-RIF properties 
         */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
        bcm_l3_ingress_t l3_ing_if;
        l3_ing_if.flags = BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
        rv = bcm_l3_ingress_create(unit, l3_ing_if, l3_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), intf_ingress_rif_set\n", rv);
            return rv;
        }
    }
    return rv;
}

int
mpls_add_term_entry(
    int unit,
    int term_label,
    uint32 next_prtcl)
{
    int rv;
    int flags = 0;
    int tunnel_id;
    int mpls_termination_label_index_enable;

    /*
     * read mpls index soc property 
     */
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);
    if (!mpls_termination_label_index_enable)
    {
        /*
         * if not set 
         */
        rv = mpls_add_term_entry_info(unit, term_label, flags, next_prtcl, &tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_term_entry_info\n");
            return rv;
        }
    }
    else
    {
        /*
         * else add entries for label index 1 and 2 
         */
        rv = mpls_add_index_term_entry_info(unit, mpls_lsr_info_1.access_index_1, term_label, flags, next_prtcl,
                                            &tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_index_term_entry_info\n");
            return rv;
        }
        ingress_tunnel_id_indexed[0] = tunnel_id;
        rv = mpls_add_index_term_entry_info(unit, mpls_lsr_info_1.access_index_2, term_label, flags, next_prtcl,
                                            &tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_index_term_entry_info\n");
            return rv;
        }
        ingress_tunnel_id_indexed[1] = tunnel_id;
        if (mpls_lsr_info_1.access_index_3 != mpls_lsr_info_1.access_index_2)
        {
            rv = mpls_add_index_term_entry_info(unit, mpls_lsr_info_1.access_index_3, term_label, flags, next_prtcl,
                                                &tunnel_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in mpls_add_index_term_entry_info\n");
                return rv;
            }
            ingress_tunnel_id_indexed[2] = tunnel_id;
        }
    }
    ingress_tunnel_id = tunnel_id;
    return rv;
}

/* Add the router alert label as a reserved label.
 * This label must NOT be in the bottom of stack, and its label is 1.                                                                                                                         .
 * This is only available for Arad+.                                                                                        .
 *                                                                                                                          .
 * @term_label the label to use. 0-15.
 * @expect_bos If set then packets WITH BOS are expected to be received on this tunnel.
 *             If unset then packets WITHOUT BOS are expected to be received on this tunnel.
 */
int
mpls_add_router_alert_label(
    int unit,
    uint32 next_prtcl)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    if (!is_device_or_above(unit, ARAD_PLUS))
    {
        printf("Error - mpls_add_router_alert_label can only run on Arad plus / Jericho.\n");
        return BCM_E_PARAM;
    }

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    /*
     * Next protocol indication:
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or
     * 0 - unset(MPLS)
     */

    /*
     * We always expect no BOS for this label. Therefore we do not use the BCM_MPLS_SWITCH_EXPECT_BOS flag. 
     */

    /*
     * incomming label 
     */
    entry.label = 1;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* Add term entry to perform pop by index
 */
int
mpls_add_index_term_entry_info(
    int unit,
    int mpls_index,
    int term_label,
    int flags,
    uint32 next_prtcl,
    int *tunnel_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    entry.flags |= flags;

    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    /*
     * Next protocol indication:
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or
     * 0 - unset(MPLS)
     */

    if (is_device_or_above(unit, ARAD_PLUS))
    {
        if (next_prtcl & BCM_MPLS_SWITCH_EXPECT_BOS)
        {
            /*
             * In ARAD+ we dont use next protocol because it is extracted from next nibble, but we can use expect bos
             * indication 
             */
            entry.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
        }
    }
    else
    {
        entry.flags |= next_prtcl;
    }

    /*
     * incomming label 
     */
    /*
     * index must be between 1-3 
     */
    if (mpls_index <= 0 || mpls_index > 3)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create index is out of range %d\n", mpls_index);
        return BCM_E_PARAM;
    }

    BCM_MPLS_INDEXED_LABEL_SET(entry.label, term_label, mpls_index);

    /*
     * Enable when testing egress QOS, need to source cint_qos.c This remarks the mpls egress packet 
     */
    entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    *tunnel_id = entry.tunnel_id;
    return rv;
}

int
mpls_add_index_term_entry(
    int unit,
    int mpls_index,
    int term_label,
    uint32 next_prtcl)
{
    int flags = 0;
    int dummy_tunnel_id;
    return mpls_add_index_term_entry_info(unit, mpls_index, term_label, flags, next_prtcl, &dummy_tunnel_id);
}

/* Add PHP entry */
int
mpls_add_php_entry(
    int unit,
    int php_label,
    uint32 next_prtcl,
    int pipe,
    bcm_if_t egress_intf)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;

    /*
     * Next protocol indication:
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or
     * 0 - unset(MPLS)
     */
    entry.flags |= next_prtcl;

    /*
     * TTL decrement has to be present 
     */
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    /*
     * Uniform: inherit TTL and EXP, in general valid options: both present (uniform) or none of them (Pipe) 
     */
    if (pipe == 0)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
            entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
            entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        }
        else
        {
            entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
        }
    }

    /*
     * incoming label 
     */
    entry.label = php_label;

    /*
     * egress attribures
     */
    entry.egress_if = egress_intf;

    /*
     * Enable when testing egress QOS, need to source cint_qos.c This remarks the mpls egress packet 
     */
    entry.qos_map_id = qos_map_id_mpls_egress_get(unit);

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* Add FRR Termination entry */
int
mpls_add_frr_term_entry(
    int unit,
    int frr_label,
    int term_label,
    uint32 next_prtcl)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    /*
     * Next protocol indication:
     * BCM_MPLS_SWITCH_NEXT_HEADER_IPV4 | BCM_MPLS_SWITCH_NEXT_HEADER_IPV6, or
     * BCM_MPLS_SWITCH_NEXT_HEADER_L2, or
     * 0 - unset(MPLS)
     */
    entry.flags |= next_prtcl;

    /*
     * incomming label 
     */
    entry.label = frr_label;

    /*
     * FRR : add two flags: BCM_MPLS_SWITCH_FRR, BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL 
     */
    entry.flags |= BCM_MPLS_SWITCH_FRR | BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;

    entry.second_label = term_label;

    /*
     * Enable when testing egress QOS, need to source cint_qos.c This remarks the mpls egress packet 
     */
    entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* to run a two-device system call with second_unit >= 0 */
int
mpls_lsr_config_init(
    int *units_ids,
    int nof_units,
    int extend_example)
{

    int ingress_intf1, ingress_intf2, ingress_intf3;
    int egress_intf;
    int encap_id;
    int rv, i;
    int unit;
    int flags;
    int is_Arad_Jericho;
    create_l3_egress_s l3_egress_arp, l3_egress_fec;
    bcm_pbmp_t pbmp;

    is_Arad_Jericho = device_is_Arad_Jericho(units_ids, nof_units);

    /*
     * Set pipe_mode_exp_set for all units 
     */
    /*
     * JR2 Does not need the switch property, Uniform and PIPE for TTL and EXP are controled by BCM API 
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        for (i = 0; i < nof_units; i++)
        {
            rv = mpls__mpls_pipe_mode_exp_set(units_ids[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
                return rv;
            }
        }
    }
    
    /* configure the AC lif for double tag packets - this function relevant for termination mode 22-23 only */
	rv = mpls_lsr_double_tag_ac_lif_configuration(units_ids,nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_lsr_double_tag_ac_lif_configuration\n");
        return rv;
    }
	
    /* Create the vlans in all units*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_vlan_destroy(unit, mpls_lsr_info_1.in_vid);
        rv = bcm_vlan_create(unit, mpls_lsr_info_1.in_vid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create with vid = %d and unit %d\n", mpls_lsr_info_1.in_vid, unit);
            return rv;
        }

        rv = bcm_vlan_destroy(unit, mpls_lsr_info_1.eg_vid);
        rv = bcm_vlan_create(unit, mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create with vid = %d and unit %d\n", mpls_lsr_info_1.eg_vid, unit);
            return rv;
        }
    }

    /*
     * Create ingress and egress vlan gports in all units. Order of units is irrelevant
     */

    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        /*
         * the flag BCM_VLAN_GPORT_ADD_UNTAGGED can be used only with BCM_VLAN_GPORT_ADD_EGRESS_ONLY and not with
         * BCM_VLAN_GPORT_ADD_INGRESS_ONLY indication. The functionality is strictly related to the egress pipe. This
         * was instabe in JR1 and is now fixed for JR2 
         */
        if (!is_device_or_above(unit, JERICHO2))
        {
            flags = BCM_VLAN_GPORT_ADD_UNTAGGED;
        }
        else
        {
            flags = BCM_VLAN_GPORT_ADD_INGRESS_ONLY;
        }
        rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.in_vid, mpls_lsr_info_1.in_port, flags);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in bcm_vlan_create with vid = %d, port = 0x%x and unit %d\n", mpls_lsr_info_1.in_vid,
                   mpls_lsr_info_1.in_port, unit);
            return rv;
        }
        if (!is_device_or_above(unit, JERICHO2))
        {
            flags = BCM_VLAN_GPORT_ADD_UNTAGGED;
        }
        else
        {
            flags = BCM_VLAN_GPORT_ADD_UNTAGGED | BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
        }
        rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.eg_vid, mpls_lsr_info_1.eg_port, flags);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in bcm_vlan_create with vid = %d, port = 0x%x and unit %d\n", mpls_lsr_info_1.eg_vid,
                   mpls_lsr_info_1.eg_port, unit);
            return rv;
        }
        /*
         * vlan_gport_add does not need WITH_ID flag 
         */
    }

    if (mpls_lsr_info_1.use_multiple_mymac)
    {
        /*
         * Set a specific myamac for mpls, using multiple mymac termination, rather than a global mymac 
         */
        l2__mymac_properties_s mymac_properties;

        sal_memset(&mymac_properties, 0, sizeof(mymac_properties));

        sal_memcpy(mymac_properties.mymac, mpls_lsr_info_1.my_mac, 6);
        mymac_properties.mymac_type = l2__mymac_type_multiple_mymac;
        mymac_properties.vsi = mpls_lsr_info_1.in_vid;
        mymac_properties.flags = BCM_L2_STATION_MPLS;

        for (i = 0; i < nof_units; i++)
        {
            unit = units_ids[i];
            rv = l2__mymac__set(unit, &mymac_properties);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in l2__mymac__set\n");
                return rv;
            }
        }
        /*
         * Save the station id so it can be removed at cleanup. 
         */
        mpls_lsr_info_1.mymac_l2_station_id = mymac_properties.station_id;
    }

    /*
     * L3 interface for mpls rounting 
     */
    flags = 0;
    ingress_intf1 = 0;
    units_array_make_local_first(units_ids, nof_units, mpls_lsr_info_1.in_port);
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        create_l3_intf_s intf;
        intf.vsi = mpls_lsr_info_1.in_vid;
        intf.my_global_mac = mpls_lsr_info_1.my_mac;
        intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
        intf.skip_mymac = mpls_lsr_info_1.use_multiple_mymac;
        intf.mtu_valid = 1;
        intf.mtu = 0;
        intf.mtu_forwarding = 0;

        rv = l3__intf_rif__create(unit, &intf);
        ingress_intf1 = intf.rif;
        if (rv != BCM_E_NONE)
        {
            printf("Error, in l3__intf_rif__create\n");
            return rv;
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*
     * create ingress object, packet will be routed to 
     */
    flags = 0;
    ingress_intf2 = 0;
    units_array_make_local_first(units_ids, nof_units, mpls_lsr_info_1.in_port);
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        create_l3_intf_s intf;
        intf.vsi = mpls_lsr_info_1.eg_vid;
        intf.my_global_mac = mpls_lsr_info_1.my_mac;
        intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
        intf.skip_mymac = mpls_lsr_info_1.use_multiple_mymac;
        intf.mtu_valid = 1;
        intf.mtu = 0;
        intf.mtu_forwarding = 0;

        rv = l3__intf_rif__create(unit, &intf);
        ingress_intf2 = intf.rif;
        if (rv != BCM_E_NONE)
        {
            printf("Error, in l3__intf_rif__create\n");
            return rv;
        }
        flags |= BCM_L3_WITH_ID;
    }

    units_array_make_local_first(units_ids, nof_units, mpls_lsr_info_1.eg_port);

    if (mpls_lsr_info_1.out_to_tunnel)
    {
        for (i = 0; i < nof_units; i++)
        {
            mpls__egress_tunnel_utils_s mpls_tunnel_properties;
            unit = units_ids[i];
            mpls_tunnel_properties.label_in = 200;
            mpls_tunnel_properties.label_out = mpls_tunnel_label_out;

            if (!is_device_or_above(unit, JERICHO2))
            {
                mpls_tunnel_properties.next_pointer_intf = ingress_intf2;
            }

            if ((mpls_tunnel_label_out == 0) && extend_example)
            {
                mpls_tunnel_properties.label_out = 400;
            }
            printf("mpls_tunnel_properties.label_out: %d\n", mpls_tunnel_properties.label_out);

            ext_mpls_ttl = 40;
            printf("Trying to create tunnel initiator\n");
            rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in mpls__create_tunnel_initiator__set\n");
                return rv;
            }

            ingress_intf3 = mpls_tunnel_properties.tunnel_id;
        }
    }
    else
    {
        ingress_intf3 = ingress_intf2;
    }

    mpls_lsr_info_1.ingress_intf = ingress_intf3;

    /*
     * create egress object 
     */
    if (is_Arad_Jericho)
    {
        encap_id = 0x40002000;
    }
    else
    {
        encap_id = 0;
    }
    flags = 0;

    /*
     * We're creating the l3_egress with eg_port. The local unit for eg_port has been advanced before the above if
     * block
     */
     
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        if (test_fec_format_c)
        {
            create_l3_egress_with_mpls_s l3eg_fec;
            bcm_failover_t failover_fec = 0;
            int gport_forward_fec = 0;

            l3_egress_fec.l3_flags = BCM_L3_CASCADED;
            if(!is_device_or_above(unit, JERICHO2))
            {
                /* BCM_L3_CASCADED not supported on allocation_flags for JR2 */
                l3_egress_fec.allocation_flags = BCM_L3_CASCADED;
            }
            l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
            l3_egress_fec.vlan = mpls_lsr_info_1.eg_vid;
            l3_egress_fec.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;

            rv = l3__egress__create(unit, &l3_egress_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in l3__egress__create\n");
                return rv;
            }
            
            BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, l3_egress_fec.fec_id);
            if (flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)
            {
                l3eg_fec.l3_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
                flags &= ~BCM_L3_ENCAP_SPACE_OPTIMIZED;
            }
            l3eg_fec.allocation_flags = flags;
            l3eg_fec.out_tunnel_or_rif = ingress_intf3;
            if (is_device_or_above(unit, JERICHO))
            {
                l3eg_fec.out_gport = gport_forward_fec;
            }
            else
            {
                l3eg_fec.out_gport = mpls_lsr_info_1.eg_port;
            }
            l3eg_fec.fec_id = egress_intf;
            add_test_fec_format_c_fields(unit, &l3eg_fec);

            if (!is_device_or_above(unit, JERICHO_B0))
            {
                /*
                 * create failover id 
                 */
                rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_fec);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_failover_create for fec protection, rv - %d\n", rv);
                    return rv;
                }
                printf("Ingress Failover id: 0x%x\n", failover_fec);
                /*
                 * Create protected FEC 
                 */
                l3eg_fec.failover_id = failover_fec;
                rv = l3_egress__mpls_push_command__create(unit, &l3eg_fec);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, l3_egress__mpls_push_command__create\n");
                    return rv;
                }
                l3eg_fec.failover_if_id = l3eg_fec.fec_id;
                l3eg_fec.arp_encap_id = 0;
            }
            /*
             * Create primary FEC 
             */
            rv = l3_egress__mpls_push_command__create(unit, &l3eg_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error, l3_egress__mpls_push_command__create\n");
                return rv;
            }
            egress_intf = l3eg_fec.fec_id;
            flags |= BCM_L3_WITH_ID;
        }
        else
        {
            printf("inside the else\n");
            l3_egress_fec.out_gport = mpls_lsr_info_1.eg_port;
            l3_egress_arp.vlan = mpls_lsr_info_1.eg_vid;
            l3_egress_arp.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
            l3_egress_arp.out_tunnel_or_rif = mpls_lsr_info_1.ingress_intf;
            l3_egress_arp.arp_encap_id = encap_id;
            l3_egress_arp.fec_id = egress_intf;
            l3_egress_arp.qos_map_id = qos_map_id_mpls_egress_get(unit);
            if (add_ll_label)
            {
                l3_egress_arp.l3_flags2 |= BCM_L3_FLAGS2_EGRESS_WIDE;
            }

            rv = l3__egress_only_encap__create(unit, &l3_egress_arp);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in l3__egress_only_encap__create\n");
                return rv;
            }
            print l3_egress_arp;

            encap_id = l3_egress_arp.arp_encap_id;
            /*
             * FEC must be format B, either tunnel or ARP is pointed from FEC 
             */
            if (mpls_lsr_info_1.out_to_tunnel)
            {
                l3_egress_fec.out_tunnel_or_rif = ingress_intf3;
                l3_egress_fec.arp_encap_id = 0;
            }
            else
            {
                l3_egress_fec.out_tunnel_or_rif = 0;
                l3_egress_fec.arp_encap_id = encap_id;
            }
            rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in l3__egress_only_fec__create\n");
                return rv;
            }

            l3_egress_fec.allocation_flags |= BCM_L3_WITH_ID;
            egress_intf = l3_egress_fec.fec_id;
        }
    }

    mpls_lsr_info_1.encap_id = encap_id;
    if (test_fec_format_c)
    {
        mpls_lsr_info_1.egress_intf = egress_intf;
    }
    else
    {
        mpls_lsr_info_1.egress_intf = l3_egress_fec.fec_id;
    }
    return rv;
}

/* to run a two-device system call with second_unit >= 0 */
int
mpls_lsr_config(
    int *units_ids,
    int nof_units,
    int extend_example)
{

    int rv, i;
    int unit;

    rv = mpls_lsr_config_init(units_ids, nof_units, extend_example);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_config_init\n");
        return rv;
    }
    /*
     * add switch entry to perform LSR 
     */
    /*
     * swap, in-label, with egress label, and point to egress object 
     */
    /*
     * Since the switch entry is not local in any unit, the units order is irrelevant 
     */
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        rv = mpls_add_switch_entry(unit, mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label,
                                   mpls_lsr_info_1.egress_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_switch_entry\n");
            return rv;
        }
    }
    return rv;
}

/* Activate default settings */
int
mpls_lsr_run_with_defaults_multi_device(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    mpls_lsr_init(in_sysport, out_sysport, 0x11, 0x22, 5000, 8000, 100, 200, 0);
    return mpls_lsr_config(units_ids, nof_units, 0);
}

/* Kept for backwards compatibilty. Does not support second unit. If you want to run with a second unit, call mpls_lsr_run_with_defaults_multi_device instead*/
int
mpls_lsr_run_with_defaults(
    int unit,
    int second_unit,
    int out_port)
{
    int rv, nof_units = 1;
    int out_sysport;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = port_to_system_port(unit, out_port, &out_sysport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in port_to_system_port\n");
        return rv;
    }

    mpls_lsr_init(out_sysport, out_sysport, 0x11, 0x22, 5000, 8000, 100, 200, 0);
    rv = mpls_lsr_config(&unit, nof_units, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_config\n");
        return rv;
    }
    return rv;
}

/*
 * LSR Application includes Tunnel initiator and Tunnel termination
 */
int
mpls_lsr_tunnel_example(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    bcm_error_t rv;
    int term_label = 1000;
    int flags, ingress_intf, encap_id, egress_intf;
    int unit, i;

    mpls_lsr_init(in_sysport, out_sysport, 0x11, 0x22, 5000, 8000, 100, 200, 1 /* out to tunnel example */ );

    rv = mpls_lsr_config(units_ids, nof_units, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_config\n");
        return rv;
    }

    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        /*
         * Add termination label, next protocol 0 because is taken from next nibble 
         */
        rv = mpls_add_term_entry(unit, term_label, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_term_entry\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Create second lsr tunnel includes Tunnel initiator and Tunnel termination
 */
int
mpls_second_lsr_tunnel_create(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    bcm_error_t rv;
    int term_label = 2000;
    int flags, ingress_intf, encap_id, egress_intf;
    int unit, i;

    mpls_lsr_init(in_sysport, out_sysport, 0xAA, 0xBB, 6000, 9000, 1000, 2000, 1 /* out to tunnel example */ );

    rv = mpls_lsr_config(units_ids, nof_units, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_config\n");
        return rv;
    }

    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        /*
         * Add termination label, next protocol 0 because is taken from next nibble 
         */
        rv = mpls_add_term_entry(unit, term_label, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_term_entry\n");
            return rv;
        }
    }

    return rv;
}

/*
 * LSR Application includes LSR with PHP command
 */
int
mpls_lsr_php_example(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    int egress_intf;
    int flags, ingress_intf, encap_id;
    int rv, i;
    int unit;
    uint32 next_prtcl;

    mpls_lsr_init(in_sysport, out_sysport, 0x11, 0x22, 0x1111, 0x2222, 100, 200, 0);
    rv = mpls_lsr_config(units_ids, nof_units, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_config\n");
        return rv;
    }

    /*
     * create egress object 
     */
    encap_id = 0;
    flags = 0;

    /*
     * Egress unit is first anyway
     */
    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        create_l3_egress_s l3eg_fec;
        sal_memcpy(l3eg_fec.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac, 6);
        l3eg_fec.allocation_flags = flags;
        l3eg_fec.out_tunnel_or_rif = ingress_intf;
        l3eg_fec.out_gport = mpls_lsr_info_1.eg_port;
        l3eg_fec.vlan = mpls_lsr_info_1.eg_vid;
        l3eg_fec.fec_id = egress_intf;
        l3eg_fec.arp_encap_id = encap_id;

        rv = l3__egress__create(unit, &l3eg_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, l3__egress__create\n");
            return rv;
        }
        egress_intf = l3eg_fec.fec_id;
        encap_id = l3eg_fec.arp_encap_id;
        flags |= BCM_L3_WITH_ID;
    }

    for (i = 0; i < nof_units; i++)
    {
        unit = units_ids[i];
        /* JR2 not supporting BCM_MPLS_SWITCH_NEXT_HEADER_IPVx */
        next_prtcl = is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
        rv = mpls_add_php_entry(unit, 333, next_prtcl, 0, egress_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_add_php_entry\n");
            return rv;
        }
    }

    return rv;
}

/* packet comes in with IPv4oMPLS1000oMPLS400oEth
   First MPLS is terminated (400)
   Second MPLS is swapped (1000->2000)
   Then encapsulation add additional MPLS header (200)
   Packet goes out as IPv4oMPLS2000oMPLS200oEth
    */

int
lsr_basic_example(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    int my_mac_lsb = 0x11;      /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22;    /* set MAC to 00:00:00:00:00:22 */
    int in_label = 1000;
    int out_label = 2000;
    int in_vid = 10;
    int eg_vid = 10;
    int out_to_tunnel = 1;
    int term_label = 400;
    uint32 next_header_flags = 0;       /* indicate next protocol is MPLS */

    mpls_lsr_init(in_sysport, out_sysport, my_mac_lsb, next_hop_lsb, in_label, out_label, in_vid, eg_vid,
                  out_to_tunnel);

    /*
     * create vlan
     */
    mpls_lsr_config(units_ids, nof_units, 0);
    mpls_add_term_entry_multi_device(units_ids, nof_units, term_label, next_header_flags);

}

/*
 *  Runs the regular lsr_basic_example on one unit instead of an array of units
 *  Used to call this functionality from Dvapi tests
 */
int
lsr_basic_example_single_unit(
    int unit,
    int in_port,
    int out_port)
{
    int units_ids[1];
    units_ids[0] = unit;
    return lsr_basic_example(units_ids, 1, in_port, out_port);
}

/* aux function to create ingress/egress interfaces */

int
add_test_fec_format_c_fields(
    int unit,
    create_l3_egress_with_mpls_s * l3eg)
{
    int rv;
    if (test_fec_format_c)
    {

        l3eg->mpls_label = 1234;

        if (test_fec_format_c_swap)
        {
            l3eg->mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP;
        }
        else
        {
            l3eg->mpls_action = BCM_MPLS_EGRESS_ACTION_PUSH;
            l3eg->mpls_flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
            l3eg->mpls_ttl = 123;
            l3eg->mpls_exp = 5;

            if (!is_device_or_above(unit, ARAD_PLUS) || mpls_pipe_mode_exp_set)
            {
                l3eg->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }
            else
            {
                l3eg->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            }
        }
    }
    return rv;
}

/*
 * l3 interface replace - egress
 * Used to check that the api with given encap_id and flags does not allocate new ll
 */
int
l3_egress_replace(
    int unit)
{

    int rv;
    int flags;
    uint8 mac_test[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    create_l3_egress_s l3eg;

    bcm_if_t l3egid;

    int test_failover_id = 0;
    int test_failover_intf_id = 0;

    l3eg.allocation_flags =
        BCM_L3_WITH_ID | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE | BCM_L3_INGRESS_ONLY;
    if (is_device_or_above(unit, JERICHO2))
    {
        l3eg.allocation_flags &= ~BCM_L3_KEEP_DSTMAC & ~BCM_L3_KEEP_VLAN;
    }
    l3eg.out_tunnel_or_rif = mpls_lsr_info_1.ingress_intf;

    l3eg.out_gport = mpls_lsr_info_1.eg_port;
    l3eg.failover_id = test_failover_id;
    l3eg.failover_if_id = test_failover_intf_id;
    l3eg.arp_encap_id = mpls_lsr_info_1.encap_id;
    if (is_device_or_above(unit, JERICHO2) && l3eg.out_tunnel_or_rif)
    {
        l3eg.arp_encap_id = 0;
    }
    l3eg.qos_map_id = qos_map_id_mpls_egress_get(unit);
    l3eg.fec_id = mpls_lsr_info_1.egress_intf;

    rv = l3__egress_only_fec__create(unit, &l3eg);
    if (rv != BCM_E_NONE)
    {
        printf("error, in bcm_l3_egress_create with REPLACE encap_id\n");
        return rv;
    }

    return rv;
}

/* Activate deafult settings, using multiple mymac termination instead of global mymac. */
int
mpls_lsr_run_with_mutliple_mymac(
    int *units_ids,
    int nof_units,
    int in_sysport,
    int out_sysport)
{
    int rv;

    mpls_lsr_init(in_sysport, out_sysport, 0x11, 0x22, 5000, 8000, 100, 200, 0);

    rv = mpls_lsr_multiple_mymac_init(units_ids, nof_units);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_multiple_mymac_init\n");
        return rv;
    }
    return mpls_lsr_config(units_ids, nof_units, 0);
}

/*
 * mpls_lsr_multiple_mymac_init
 *
 *  Used for multiple mymac termination, sets the mpls protocol to be in protcol group 2, and enables it
 *      for protocol based termination.
 */
int
mpls_lsr_multiple_mymac_init(
    int *units_ids,
    int nof_units)
{
    int rv = BCM_E_NONE, i, unit;

    mpls_lsr_info_1.use_multiple_mymac = 1;

    for (i = 0; i < nof_units; i++)
    {

        unit = units_ids[i];

        if (!is_device_or_above(unit, JERICHO))
        {
            print("Mpls specific multiple mymac only supported in Jericho and above");
            return BCM_E_UNAVAIL;
        }

        rv = bcm_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_MPLS, 0x2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }
    }

    return rv;
}

/*
 * mpls_lsr_multiple_mymac_destroy
 *
 * Given a unit, deinitializes the protocol group and unsets the mymac.
 */
int
mpls_lsr_multiple_mymac_destroy(
    int *units_ids,
    int nof_units)
{
    int rv = BCM_E_NONE, i, unit;

    mpls_lsr_info_1.use_multiple_mymac = 0;

    for (i = 0; i < nof_units; i++)
    {

        unit = units_ids[i];
        rv = bcm_l2_station_delete(unit, mpls_lsr_info_1.mymac_l2_station_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l2_station_delete\n");
            return rv;
        }

        rv = bcm_switch_l3_protocol_group_set(unit, 0, 0x2);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_switch_l3_protocol_group_set\n");
            return rv;
        }
    }

    return rv;
}

/*
*  LSR COUNTER
*  * (1) SOC property should be added
*      config add counter_engine_replicated_packets=ONE_COPY
*      config add counter_engine_source_0=INGRESS_FIELD_0_1_0
*      config add counter_engine_statistics_0=FULL_COLOR
*  * (2) How to run
*      BCM> cint ../sand/utility/cint_sand_utils_global.c
*      BCM> cint ../sand/utility/cint_sand_utils_mpls.c
*      BCM> cint cint_qos.c
*      BCM> cint cint_multi_device_utils.c
*      BCM> cint utility/cint_utils_l2.c
*      BCM> cint utility/cint_utils_l3.c
*      BCM> cint cint_mpls_lsr.c
*      BCM> cint cint_queue_tests.c
*      cint> int outP = 13;
*      cint> int inP = 13;
*      For ingress
*      cint> rv = lsr_counter_main(unit,inP, outP);
*      cint> rv = lsr_field_ingress_counter_set(unit);
*      For egress
*            --> need to add soc property"custom_feature_lsr_cnt_egress"
*      cint> rv = lsr_egress_counter_config_set(unit);
*      cint> rv = lsr_counter_main(unit,inP, outP);
*      cint> rv = lsr_field_egress_counter_set(unit);
*      cint> rv = lsr_counter_show(unit);
 */

int
lsr_counter_main(
    int unit,
    int inP,
    int outP)
{
    int rv;
    int nof_units = 1;
    int units[nof_units] = { 0 };
    uint16 ingress_count_id = 100;
    uint16 egress_count_id = 200;
    bcm_mpls_label_t label = 5000;
    int outSysport, inSysport;
    port_to_system_port(unit, outP, &outSysport);
    port_to_system_port(unit, inP, &inSysport);

    rv = mpls_lsr_run_with_defaults_multi_device(units, nof_units, inSysport, outSysport);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_data_qualifier_create - udp_qualifier-oam_restore Err %x\n", result);
        return rv;
    }
    /*
     * Low 16bit is ingress count ID, High 16bits is egress count ID
     */
    rv = bcm_mpls_label_stat_attach(unit, 5000, inP, (egress_count_id << 16) | ingress_count_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_field_data_qualifier_create - udp_qualifier-oam_restore Err %x\n", result);
        return rv;
    }
    return rv;
}

int
lsr_field_ingress_counter_set(
    int unit)
{
    int result = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_field[2];
    bcm_field_group_t group = 1;
    bcm_field_data_qualifier_t lsr_qualifier;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_group_config_t_init(&grp);
    grp.group = group;

    /*
     * Define the QSET - use InLIF as qualifier.
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyForwardingType);

    /*
     *  Define the ASET - update counter ID.
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    /*
     * Qualify lem payload result is counter ID 
     */
    bcm_field_data_qualifier_t_init(&lsr_qualifier);
    lsr_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;
    lsr_qualifier.offset = 0;
    lsr_qualifier.length = 16;
    lsr_qualifier.qualifier = bcmFieldQualifyL2SrcValue;
    result = bcm_field_data_qualifier_create(unit, &lsr_qualifier);
    /*
     * Using LEM 1st lookup result as qualifier 
     */
    result = bcm_field_qset_data_qualifier_add(unit, grp.qset, lsr_qualifier.qual_id);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_data_qualifier_create - udp_qualifier-oam_restore Err %x\n", result);
        return result;
    }

    /*
     * Create the Field group with type Direct Extraction 
     */
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_create\n");
        return result;
    }

    /*
     * Attach the action set 
     */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /*
     * qualify lsr packet 
     */
    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeMpls);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_field[0]);
    bcm_field_extraction_field_t_init(&ext_field[1]);

    extract.action = bcmFieldActionStat;
    extract.bias = 0;

    /*
     * First extraction structure indicates action is valid 
     */
    ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_field[0].bits = 1;
    ext_field[0].value = 1;

    ext_field[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_field[1].bits = 16;
    ext_field[1].lsb = 0;       /* XIAO: if egress, then lsb should be 16 ??? */
    ext_field[1].qualifier = lsr_qualifier.qual_id;

    result = bcm_field_direct_extraction_action_add(unit, ent, extract, 2 /* count */ ,
                                                    &ext_field);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}

int
lsr_egress_counter_config_set(
    int unit)
{
    bcm_stat_counter_engine_t counter_engine;
    bcm_stat_counter_config_t counter_config;
    bcm_color_t colors[4] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
    int index1, index2;
    uint8 drop_fwd[2] = { 0, 1 };
    int rv;

    counter_config.format.format_type = bcmStatCounterFormatPacketsAndBytes;
    counter_config.format.counter_set_mapping.counter_set_size = 1;
    counter_config.format.counter_set_mapping.num_of_mapping_entries = 8;

    /*
     * Counter configuration is independent on the color, drop precedence.
     */
    counter_engine.flags = BCM_STAT_COUNTER_IGNORE_COLORS | BCM_STAT_COUNTER_IGNORE_DISPOSITION;
/*
    for ( index1  = bcmColorGreen; index1 < 4; index1++) {
        for (index2 = 0; index2 < 2; index2++) {
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0;
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
            counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
        }
    }
*/
    counter_config.source.core_id = 0;
    counter_config.source.pointer_range.start = (1 / 0x4000) * 0x4000;  /* NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE ; */
    counter_config.source.pointer_range.end = 1024 - 1;

    counter_config.source.engine_source = bcmStatCounterSourceEgressField;
    counter_engine.engine_id = 3;
    rv = bcm_stat_counter_config_set(0, &counter_engine, &counter_config);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_stat_counter_config_set\n");
        return rv;
    }
    return rv;
}

int
lsr_field_egress_counter_set(
    int unit,
    int outP)
{
    /*
     * modify vsi with egress counter ID form LEM 2nd lookup result 
     */
    int result = 0;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_field[2];
    bcm_field_group_t group = 1;
    bcm_field_data_qualifier_t lsr_qualifier;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_qset_t eg_qset;
    bcm_field_aset_t eg_aset;
    bcm_field_group_t eg_grp = 3;
    bcm_gport_t sysport_gport = 0x1024;
    bcm_field_entry_t eg_ent;
    int e_group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_group_config_t_init(&grp);
    grp.group = group;
    int stat_id = 1;            /* Actually, don't need to care of stat_id, just use a valid value */
    int is_qax = FALSE;

    /*
     * Ingress PMF use to copy counter-ID to VSI, counter-ID is from 1st lookup result in LEM
     */
    /*
     * Define the QSET - use InLIF as qualifier.
     */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyForwardingType);

    /*
     *  Define the ASET - update counter ID.
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVSwitchNew);

    /*
     * Qualify lem payload result is counter ID 
     */
    bcm_field_data_qualifier_t_init(&lsr_qualifier);
    lsr_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;
    lsr_qualifier.offset = 19;
    lsr_qualifier.length = 2;
    lsr_qualifier.qualifier = bcmFieldQualifyL2SrcValue;
    result = bcm_field_data_qualifier_create(unit, &lsr_qualifier);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    /*
     * Using LEM 1st lookup result as qualifier 
     */
    result = bcm_field_qset_data_qualifier_add(unit, grp.qset, lsr_qualifier.qual_id);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }

    /*
     * Create the Field group with type Direct Extraction 
     */
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }
    /*
     * Attach the action set 
     */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    /*
     * qualify lsr packet 
     */
    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeMpls);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_field[0]);
    bcm_field_extraction_field_t_init(&ext_field[1]);

    extract.action = bcmFieldActionVSwitchNew;
    extract.bias = 0;

    ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_field[0].bits = 16;
    ext_field[0].lsb = 0;
    ext_field[0].qualifier = lsr_qualifier.qual_id;

    bcm_field_direct_extraction_action_add(unit, ent, extract, 1 /* count */ ,
                                           &ext_field);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }
    bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    /*
     * modify egress counter pointer with VSI(3) 
     */

    BCM_FIELD_QSET_INIT(eg_qset);

    BCM_FIELD_QSET_ADD(eg_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(eg_qset, bcmFieldQualifyForwardingType);

    BCM_FIELD_ASET_INIT(eg_aset);
    BCM_FIELD_ASET_ADD(eg_aset, bcmFieldActionStat);

    result = bcm_field_group_create_id(unit, eg_qset, e_group_priority, eg_grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_create_id\n");
        return result;
    }
    /*
     * rv = bcm_field_group_create(unit, eg_qset, e_group_priority, &eg_grp);
     */
    result = bcm_field_group_action_set(unit, eg_grp, eg_aset);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }
    result = bcm_field_entry_create(unit, eg_grp, &eg_ent);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }
    result = bcm_field_qualify_ForwardingType(unit, eg_ent, bcmFieldForwardingTypeMpls);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, outP);
    /*
     * all params must be 0 for lsr egress counter
     */
    is_qumran_ax(unit, &is_qax);

    if (!is_qax)
    {
        /*
         * action config for stat: para 0 is stat id para 1 is system port para 2 is outlif
         */
        bcm_field_action_core_config_t core_config_jericho[2] = { {0, stat_id, sysport_gport, 0}
        , {0, stat_id, sysport_gport, 0}
        };

        /*
         * Para 0 is 3, means setting Counter pointer with VSI value (Counter-Profile is '3' ?
         * System-Headers-Record.PPH.VSI :)
         */
        result = bcm_field_action_config_add(unit, eg_ent, bcmFieldActionStat, 2, core_config_jericho);
    }
    else
    {
        bcm_field_action_core_config_t core_config = { 0, stat_id, sysport_gport, 0 };

        /*
         * Para 0 is 3, means setting Counter pointer with VSI value (Counter-Profile is '3' ?
         * System-Headers-Record.PPH.VSI :)
         */
        result = bcm_field_action_config_add(unit, eg_ent, bcmFieldActionStat, 1, &core_config);
    }

    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    result = bcm_field_group_install(unit, eg_grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install\n");
        return result;
    }
    return result;
}

int
lsr_counter_show(
    int unit)
{
    uint32 numOfpackets = 0;
    uint64 numOfpackets_64;
    uint64 numofbytes_64;
    int counter_proc = 3;       /* counter engine */
    int stat_id = 0;
    int countId = 0x456;        /* counter ID */

    for (countId = 0; countId < 0xfff; countId++)
    {
        BCM_FIELD_STAT_ID_SET(stat_id, counter_proc, countId);
        bcm_field_stat_get32(unit, stat_id, bcmFieldStatPackets, numOfpackets);
        bcm_field_stat_get(unit, stat_id, bcmFieldStatBytes, numofbytes_64);
        if (numOfpackets > 0)
        {
            print countId;
            print numOfpackets;
            print numofbytes_64;
            print stat_id;
            break;
        }
    }
    print countId;
    return 0;
}

int mpls_lsr_double_tag_ac_lif_configuration(int *units_ids, int nof_units){

	int i;
	int unit;
	int flags;
    int mpls_termination_label_index_enable;
	int mpls_termination_label_index_database_mode;
    int rv=0;

	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);

    if ((mpls_termination_label_index_enable == 1) &&
    	((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
    	bcm_vlan_port_t vlan_port_1;
    	bcm_gport_t vlan_port_id = 0;

    	units_array_make_local_first(units_ids, nof_units, mpls_lsr_info_1.in_port);

		for (i = 0 ; i < nof_units ; i++){
			bcm_port_t local_in_port;

			unit = units_ids[i];
			/* set ports to identify double tags packets */
			rv = bcm_vlan_control_port_set(unit, mpls_lsr_info_1.in_port, bcmVlanPortDoubleLookupEnable, 1);
			if (rv != BCM_E_NONE) {
				printf("(%s) \n",bcm_errmsg(rv));
				return rv;
			}

			rv = bcm_port_local_get(unit, mpls_lsr_info_1.in_port, &local_in_port);
			if (rv != BCM_E_NONE) {
				printf("Error, bcm_port_class_set %d\n", rv);
				return rv;
			}
			/* when a port is configured with  "bcmVlanPortDoubleLookupEnable" the VLAN domain must be unique in the device */
			rv = bcm_port_class_set(unit, local_in_port, bcmPortClassId, local_in_port);
			if (rv != BCM_E_NONE) {
				printf("Error, bcm_port_class_set %d\n", rv);
				return rv;
			}

			/* set up tag structure */
			port_tpid_init(mpls_lsr_info_1.in_port,1,1);
			rv = port_tpid_set(unit);
			if (rv != BCM_E_NONE) {
				printf("Error, port_tpid_set\n");
				return rv;
			}

			/* In advanced vlan translation mode, the default ingress/ egress actions and mapping
				are not configured. This is here to compensate. */
			if (advanced_vlan_translation_mode) {
				rv = vlan_translation_default_mode_init(unit);
				if (rv != BCM_E_NONE) {
					printf("Error, in vlan_translation_default_mode_init\n");
					return rv;
				}
			}

			/* add port, according to port_vlan_vlan */
			bcm_vlan_port_t_init(&vlan_port_1);

			/* set port attributes, key <port-vlan-vlan>*/
			vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
			vlan_port_1.vsi = mpls_lsr_info_1.in_vid;
			vlan_port_1.port = mpls_lsr_info_1.in_port;
			vlan_port_1.match_vlan = mpls_lsr_info_1.in_vid;
			vlan_port_1.match_inner_vlan = 40;
			vlan_port_1.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
			vlan_port_1.vlan_port_id = vlan_port_id;

			if(i > 0)
				vlan_port_1.flags |= BCM_VLAN_PORT_WITH_ID;

			rv = bcm_vlan_port_create(unit, &vlan_port_1);
			if (rv != BCM_E_NONE) {
				printf("Error, bcm_vlan_port_create\n");
				return rv;
			}

			vlan_port_id = vlan_port_1.vlan_port_id;

			printf("vlan_port_1.vlan_port_id 0x%08x vlan_port_encap_id1: 0x%08x\n\r",vlan_port_1.vlan_port_id, vlan_port_1.encap_id);

			/* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
			   action mapping. This is here to compensate. */
			if (advanced_vlan_translation_mode) {
				rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
				if (rv != BCM_E_NONE) {
					printf("Error: vlan_translation_vlan_port_create_to_translation\n");
				}
			}

			/* egress VLAN */
			rv = bcm_vlan_destroy(unit, mpls_lsr_info_1.eg_vid);
			rv = bcm_vlan_create(unit,mpls_lsr_info_1.eg_vid);
			if (rv != BCM_E_NONE) {
				printf("Error, in bcm_vlan_create with vid = %d and unit %d\n", mpls_lsr_info_1.eg_vid, unit);
				return rv;
			}

			/* Create egress vlan gports in all units. Order of units is irrelevant*/
			flags = BCM_VLAN_GPORT_ADD_UNTAGGED;
			rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.eg_vid, mpls_lsr_info_1.eg_port, flags);
			if (BCM_FAILURE(rv)) {
				printf("Error, in bcm_vlan_create with vid = %d, port = 0x%x and unit %d\n",  mpls_lsr_info_1.eg_vid, mpls_lsr_info_1.eg_port, unit);
				return rv;
			}
		}
    }
    return rv;
}

int mpls_drop_unmatch_ces_packet(int unit){
    int rv=0;
    bcm_l2_station_t station;
    int station_id;
    bcm_vswitch_cross_connect_t gports;

    /* nni port 13  */
    int nniVlan1 = 200;
    int nniPort1 = 13;
    int outLabel1 = 1000;
    int outpwlabel1 = 5000;
    int inLabel1 = 2000;
    int inpwlabel1 = 6000;
    bcm_l3_intf_t l3if;
    bcm_mpls_port_t mpls_port;

    bcm_mac_t my_mac={0x0,0x11,0x11,0x11,0x11,0x11};
    sal_memcpy(station.dst_mac, my_mac, 6);
    sal_memset(station.dst_mac_mask, 0xff, 6);
    station.flags = 0;
    station.src_port_mask = 0;
    station.vlan_mask = 0;
    bcm_l2_station_add(unit, &station_id, &station);

    /* uni port 14  */
    int ac_port=14;
    int ac_vlan=100;
    bcm_pbmp_t pbmp, ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_create(unit, ac_vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_create");
        return rv;
    }
    BCM_PBMP_PORT_ADD(pbmp, ac_port);
    rv = bcm_vlan_port_add(unit, ac_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_port_add");
        return rv;
    }

    bcm_vlan_port_t vlan_port_uni;
    bcm_vlan_port_t_init(&vlan_port_uni);
    vlan_port_uni.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_uni.port = ac_port;
    vlan_port_uni.match_vlan = ac_vlan;
    vlan_port_uni.egress_vlan = ac_vlan;
    vlan_port_uni.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port_uni);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vlan_create");
        return rv;
    }
    printf("vlan_port_uni.vlan_port_id 0x%08x, AC global lif 0x%x\n", (vlan_port_uni.vlan_port_id),(vlan_port_uni.vlan_port_id)&0x3FFFF);

    bcm_vlan_create(0, nniVlan1);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, nniPort1);
    bcm_vlan_port_add(0, nniVlan1, pbmp, ubmp);

    /*
    Create VSI RIF for MPLS tunnel application Used for MyMAC check
    If Packet Destination MAC=RIF MAC, termination is done
    */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_vid = nniVlan1;
    l3if.l3a_ttl = 31;
    l3if.l3a_mtu = 1524;
    l3if.l3a_intf_id = nniVlan1;
    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    bcm_l3_intf_create(0, &l3if);

    bcm_mpls_egress_label_t label_array[2];
    bcm_mpls_egress_label_t_init(&label_array[0]);
    label_array[0].tunnel_id = 0;
    label_array[0].label = outLabel1;
    label_array[0].exp = 4;
    label_array[0].ttl = 30;
    label_array[0].l3_intf_id = l3if.l3a_intf_id;
    label_array[0].flags =BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    bcm_mpls_tunnel_initiator_create(0,0,1,label_array);
    printf("tunnel id 0x%08x, global lif 0x%x\n", (label_array[0].tunnel_id),(label_array[0].tunnel_id) & 0x1FFFFFFF);

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);
    uint8  g_nhop_mac1[6]={0x0,0x0,0x0,0x0,0x0,0x22};
    l3eg.intf = label_array[0].tunnel_id;
    l3eg.port = nniPort1;
    l3eg.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    sal_memcpy(l3eg.mac_addr, g_nhop_mac1, 6);
    l3eg.vlan = nniVlan1;
    bcm_l3_egress_create(0, 0, &l3eg, &l3egid);
    printf("Egress obj id 0x%08x, Fec entry index:%d\n", (l3egid),(l3egid) & 0x1FFFFFFF);
    printf("LL global lif 0x%x\n", (l3eg.encap_id) & 0x3FFFFFFF);

    /*
    Add entry for MPLS Termination
    Termination is done on the packet based on its MPLS label
    */
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = inLabel1;
    entry.port = nniPort1;
    rv = bcm_mpls_tunnel_switch_create(0, &entry);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_mpls_tunnel_switch_create");
        return rv;
    }
    printf("tunnel id 0x%08x, global lif 0x%0x\n", (entry.tunnel_id),(entry.tunnel_id) & 0x3FFFFFF);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = inpwlabel1;
    mpls_port.egress_tunnel_if = l3egid;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_COUNTED;
    mpls_port.port = nniPort1;
    mpls_port.egress_label.label = outpwlabel1;
    mpls_port.egress_label.ttl=40;
    rv = bcm_mpls_port_add(0, 0, &mpls_port);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_mpls_port_add");
        return rv;
    }
    printf("encap id (global lif) 0x%x\n", mpls_port.encap_id);
    printf("mpls_port_id 0x%0x\n", mpls_port.mpls_port_id);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = vlan_port_uni.vlan_port_id;
    gports.port2 = mpls_port.mpls_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_vswitch_cross_connect_add");
        return rv;
    }
    return rv;
}
