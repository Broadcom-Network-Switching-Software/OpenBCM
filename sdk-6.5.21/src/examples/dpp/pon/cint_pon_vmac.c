/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_pon_vmac.c
 * Purpose: An example of how to use vmac function in PON application. 
 *          
 * Calling sequence:
 *
 * Initialization:
 *  1. Add the following port configureations to config-sand.bcm
 *     
 *     # Map bcm local port to Network-Interface[.channel] interfaces - TBD
 *         ucode_port_128.BCM88650=10GBase-R15
 *         ucode_port_1.BCM88650=10GBase-R14
 *     # pon application
 *         pon_application_support_enabled_1.BCM88650=TRUE
 *         mcs_load_uc0=pon to enable loading ukernel core 0
 *         vmac_enable=1 to enable VMAC function in PON application.
 *     Set VMAC const data:
 *        VMAC encoding include 48 bits, 22 LSBs is make up of LLID-MAC-Indx[6:0], 
 *        reserved 0[7], PON tunnel ID[17:8] Pon-Port attribute[21:18].
 *        26 MSBs is configured by soc property. it's presenting slot, vmac format and so on.
 *   MAC Address	  Example value  	  Description
 *	 Bit 47...45      0x0             GPON vMAC format DSLAM ID part 1
 *	 Bit 44           0x0             ISAM xPON vMAC set to 1
 *	 Bit 43           0x0             vMAC (0) / CFM MAC (1)
 *	 Bit 42           0x0             For vMAC always 0 / for CFM MAC (future; PON Identifier: OLT / ONT)
 *	 Bit 41           0x0             U/L field set to 1 (local MAC address validity)
 *	 Bit 40           0x0             I/G field set to 0 (unicast address)
 *   Bit 39...27      0x2A            GPON vMAC format DSLAM ID part 2 [1-64K] 
 *                                    A unique DSLAM ID within an EMAN connected to the same IP edges
 *   Bit 26...22      0x14            Slot ID of the line board [0-31]
 *        vmac_encoding_value=0x000155000000
 *        vmac_encoding_mask=0xFFFFFFC00000
 *  2. Add followed configuration to config-sand.bcm if AC isolation is needed
 *         bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
 *  3. Add followed configuration to config-sand.bcm to set user-header size
 *      field_class_id_size_0.BCM88650=16
 *      field_class_id_size_1.BCM88650=0
 *      field_class_id_size_2.BCM88650=16
 *      field_class_id_size_3.BCM88650=32
 *  4. Add followed configuration to config-sand.bcm to disable ARP (next hop mac extension) feature
 *      bcm886xx_next_hop_mac_extension_enable.BCM88650=0  
 *
 *
 * Set up sequence:
 *
 * Service Model:
 * 1:1 Service:
 *     PON Port 1   <------------------------------->  VSI  <---> NNI Port 128
 *     Pon 1 Tunnel-ID 1 clvan 10 ------------------------------|---4096  |----------- SVLAN 100 cvlan 10
 *     Pon 1 Tunnel-ID xxx cvlan 10 ----------------------------|
 *     Pon 1 Tunnel-ID max_tunnel_num cvlan 10 -----------------|
 *
 * Traffic: set MAC limiting number in tuunel 10 is 5, and send traffic with 10 different SAs. 
 *          System can only learn 5 SAs and only the packets with these SAs will be passed.
 *
 *  1. PON Port 1 Tunnel-ID 10 CVLAN 10 <-CrossConnect-> NNI Port 128 SVLAN 100 CVLAN 10
 *        - From PON port:
 *              -   ethernet header with any DA, and any SA(it's OMAC) and will receive a packet with DA, SA(it's VMAC).
 *                  For example: send packet with DA - 00:22:33:44:55:66 SA(OMAC) - 00:02:03:04:05:06
 *                               receive packet with DA - 00:22:33:44:55:66 SA(VMAC) - 00:01:55:04:01:00
 *              -   Tunnel ID:10
 *              -   VLAN tag: VLAN tag type 0x9100, VID = 10
 *        - From NNI port:
 *              -   ethernet header with DA(it's VMAC), and SA and will receive a packet with DA(it's OMAC), SA.
 *                  For example: send packet with DA(VMAC) - 00:01:55:04:01:00 SA - 00:22:33:44:55:66
 *                               receive packet with DA(OMAC) - 00:02:03:04:05:06 SA - 00:22:33:44:55:66
 *              -   VLAN tag: VLAN tag type 0x8100, VID = 100 and 0x9100, VID = 10
 *
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_vmac.c
 *      BCM> cint examples/dpp/pon/cint_pon_field_vmac.c
 *      BCM> cint
 *      cint> print pon_vmac_init(unit, pon_port, nni_port, max_tunnel_num, vmac_class_id);
 *      cint> print pon_vmac_app(unit, cpu_learn);
 *      cint> print pon_vmac_limit_set(unit, pon_port, tunnel_id, limit_num);
 *      cint> print vmac_upstream_user_header(unit, pon_port);
 *      cint> print vmac_downstream_user_header(unit, nni_port);
 */

struct pon_vmac_info_s{
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport[16384]; /*2048*8*/
    bcm_gport_t nni_gport[1];
    bcm_port_t pon_port[8];
    bcm_port_t nni_port[8];
    int nni_svlan;/* outer vlan number at NNI port*/
    int pon_cvlan;
    int age_seconds;
    int max_tunnel_num; /*max tunnel id number per pon port*/
    int lif_offset; /* 16*1024, Offset for downstream flooding multicast group(MC = VSI + lif_offset)*/
};

pon_vmac_info_s pon_vmac_info;

int pon_vmac_run(int unit, int pon_port, int nni_port)
{
    pon_vmac_init(unit, pon_port, nni_port, 3, 70977);
    pon_vmac_app(unit, 1);
    pon_vmac_limit_set(unit, pon_port, 1, 10);
    return 0;
}

/* Initialize PON application configurations.
 * Example: pon_mact_limit_init(0, 1, 128, 16, 70977);
 * vmac class id is make up with 
 * "pon_port[3:0] + vmac_encoding[9:0] + reserve bits[1:0] + vmac_encoding[25:10];"
 * vmac_class_id(70977) is make up with 1 - pon_port[3:0], 0x154 - vmac_encoding[9:0], 
 * 0 - reserve bits[1:0], 0x1 - vmac_encoding[25:10]
 */
int pon_vmac_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port, int max_tunnel_num, int vmac_class_id)
{
    int i = 0;
    int j = 0;
    int rv;

    /*Enable the l2 learn limit feature for PON MACT application*/
    rv = bcm_l2_learn_limit_enable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_enable failed $rv\n");
        return rv;
    }
 
    pon_vmac_info.vsi = 0;
    pon_vmac_info.max_tunnel_num = max_tunnel_num;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < pon_vmac_info.max_tunnel_num; j++)
        {
            pon_vmac_info.pon_gport[i*2048 + j] = 0;
        }
    }
    pon_vmac_info.nni_gport[0] = 0;

    for (i = 0; i < 8; i++)
    {
        pon_vmac_info.pon_port[i] = -1;
        pon_vmac_info.nni_port[i] = -1;
    }

    pon_vmac_info.age_seconds = 60;

    /* use 1 pon port: 1 , 1 nni port: 128 */
    pon_vmac_info.nni_svlan = 100;
    pon_vmac_info.pon_cvlan = 10;
    pon_vmac_info.nni_port[0] = nni_port;
    pon_vmac_info.pon_port[0] = pon_port;

    pon_app_init(unit, pon_port, nni_port, 0, 0);

    /* Set PON-Port VMAC attribute */
    rv = bcm_port_class_set(unit, pon_port, bcmPortClassFieldIngressPacketProcessing, vmac_class_id);
    if (BCM_FAILURE(rv)) {
        printf("BCM bcm_port_control_set FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Setup pon vmac test model if cpu_learn isn't 0; otherwise, it's a normal PP model.
*     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 1 clvan 10 ------------------------------|---4096  |----------- SVLAN 100 cvlan 10
*     Pon 1 Tunnel-ID xxx cvlan 10 ----------------------------|
*     Pon 1 Tunnel-ID max_tunnel_num cvlan 10 -----------------|
*                                                 
* steps
 */
int pon_vmac_app(int unit, int cpu_learn)
{
    int rv = 0;
    int i = 0;
    bcm_if_t encap_id = 0;
    int tunnel_id = 0;
    int pon_port_idx = 0;
    int action_id = 0;

    if (cpu_learn) {
        rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_CENT|BCM_L2_LEARN_CPU);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = vswitch_create(unit, &(pon_vmac_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (pon_port_idx = 0; pon_port_idx < 8; pon_port_idx++)
    {
        if (pon_vmac_info.pon_port[pon_port_idx] != (-1))
        {
            for (tunnel_id = 0; tunnel_id < pon_vmac_info.max_tunnel_num; tunnel_id++)
            {
                /* Create PON LIF */
                rv = pon_lif_create(unit,
                                    pon_vmac_info.pon_port[pon_port_idx], 
                                    match_otag,
                                    0, 0,
                                    tunnel_id,
                                    tunnel_id,
                                    pon_vmac_info.pon_cvlan,
                                    0, 
                                    0, 0, 
                                    &(pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id]));
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_lif_create returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }

                /* Add PON LIF to vswitch */
                rv = bcm_vswitch_port_add(unit,
                                          pon_vmac_info.vsi, 
                                          pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_vswitch_port_add returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }
                /*For downstream*/
                rv = multicast_vlan_port_add(unit, 
                                             pon_vmac_info.vsi+lif_offset, 
                                             pon_vmac_info.pon_port[pon_port_idx], 
                                             pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id]);

                /* Set PON LIF ingress VLAN editor */
                rv = pon_port_ingress_vt_set(unit, 
                                             otag_to_o_i2_tag,
                                             pon_vmac_info.nni_svlan, 
                                             pon_vmac_info.pon_cvlan,
                                             pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id], 
                                             0,
                                             &action_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_port_ingress_vt_set returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }                                             
                /* Set PON LIF egress VLAN editor */
                rv = pon_port_egress_vt_set(unit,
                                            otag_to_o_i2_tag,
                                            tunnel_id,
                                            pon_vmac_info.pon_cvlan, 0,
                                            pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, pon_port_egress_vt_set returned %s idx:%d tunnel ID:%d\n", bcm_errmsg(rv), pon_port_idx, tunnel_id);
                    return rv;
                }
                /* Enable VMAC attirbutes for PON ports. */
                rv = pon_vmac_port_cos_profile_set(unit, pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id], 1);
                if (rv != BCM_E_NONE) {
                    printf("Error, pon_vmac_port_cos_profile_set returned %s pon_gport\n", bcm_errmsg(rv));
                    return rv;
                }

                rv = pon_vmac_port_lif_profile_set(unit, pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id], 1);
                if (rv != BCM_E_NONE) {
                    printf("Error, pon_vmac_port_lif_profile_set returned %s pon_gport\n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

     /* Create NNI LIF */
     rv = nni_lif_create(unit, 
                         pon_vmac_info.nni_port[0], 
                         match_o_i_tag, 0, 
                         pon_vmac_info.nni_svlan,
                         pon_vmac_info.pon_cvlan,
                         0,
                         &(pon_vmac_info.nni_gport[0]), &encap_id);
     if (rv != BCM_E_NONE) {
         printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
         return rv;
     }

     /*For upstream*/
     rv = multicast_vlan_port_add(unit, pon_vmac_info.vsi, pon_vmac_info.nni_port[0],
                                  pon_vmac_info.nni_gport[0]);
     if (rv != BCM_E_NONE) {
         printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }

     /* Add to vswitch */
     rv = bcm_vswitch_port_add(unit, pon_vmac_info.vsi, pon_vmac_info.nni_gport[0]);
     if (rv != BCM_E_NONE) {
         printf("Error, bcm_vswitch_port_add returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }
     
     /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
	   bcm_port_control_set(unit, pon_vmac_info.nni_gport[0], bcmPortControlFloodUnknownUcastGroup, lif_offset);
	   bcm_port_control_set(unit, pon_vmac_info.nni_gport[0], bcmPortControlFloodUnknownMcastGroup, lif_offset);
	   bcm_port_control_set(unit, pon_vmac_info.nni_gport[0], bcmPortControlFloodBroadcastGroup, lif_offset);

     /* Enable VMAC attributes for NNI ports */
     rv = pon_vmac_port_cos_profile_set(unit, pon_vmac_info.nni_gport[0], 1);
     if (rv != BCM_E_NONE) {
         printf("Error, pon_vmac_port_cos_profile_set returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }

     rv = pon_vmac_port_lif_profile_set(unit, pon_vmac_info.nni_gport[0], 1);
     if (rv != BCM_E_NONE) {
         printf("Error, pon_vmac_port_lif_profile_set returned %s nni_gport\n", bcm_errmsg(rv));
         return rv;
     }

    return rv;
}

/************************************************************
* Set LIF profile by gport
*/
int pon_vmac_port_lif_profile_set(int unit, bcm_gport_t gport, int enable)
{
    int rv = 0;

    rv = bcm_port_class_set(unit, gport, bcmPortClassFieldIngress, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set returned bcmPortClassFieldIngress %s gport:%08x\n", bcm_errmsg(rv), gport);
        return rv;
    }

    return rv;
}

/************************************************************
* Set cos profile by gport. if input parameter enable is 1, it's
* presenting this gport is vmac enable.
*/
int pon_vmac_port_cos_profile_set(int unit, bcm_gport_t gport, int enable)
{
    int rv = 0;

    /* Set COS profile as 1 to indicate VMAC is enabled */
    rv = bcm_port_control_set(unit, gport, bcmPortControlVMac, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set bcmPortControlVMac returned %s gport:%08x\n", bcm_errmsg(rv), gport);
        return rv;
    }

    return rv;
}


/*********************************************************************************
* Setup the VMAC limit number for per tunnel id: 
*/
int pon_vmac_limit_set(int unit, int port, int tunnel_id, int limit_num) 
{
    int rv = 0;
    bcm_l2_learn_limit_t limit;

    /* Set VMAC limit number */
    bcm_l2_learn_limit_t_init(&limit);
    limit.flags |= BCM_L2_LEARN_LIMIT_TUNNEL;
    limit.port = port;
    limit.tunnel_id = tunnel_id;
    limit.limit = limit_num;

    rv = bcm_l2_learn_limit_set(unit, &limit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_limit_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/*********************************************************************************
* Clean  pon vmac application configuration: 
*/
int pon_vmac_app_clean(int unit)
{
    int rv;
    int tunnel_id = 0;
    int pon_port_idx = 0;

    /* Delete pon gport */
    for (pon_port_idx = 0; pon_port_idx < 8; pon_port_idx++)
    {
        if (pon_vmac_info.pon_port[pon_port_idx] != (-1))
        {
            for (tunnel_id = 0; tunnel_id < pon_vmac_info.max_tunnel_num; tunnel_id++)
            {
                if (pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id] != 0)
                {
                    rv = vswitch_delete_port(unit, pon_vmac_info.vsi, pon_vmac_info.pon_gport[pon_port_idx*2048+tunnel_id]);
                    if (rv != BCM_E_NONE) 
                    {
                        printf("Error, vswitch_delete_port() returned %s remove pon gport[%d] failed\n",
                               bcm_errmsg(rv), pon_port_idx*2048+tunnel_id);
                        return rv;
                    }
                }
            }
        }
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_vmac_info.vsi, pon_vmac_info.nni_gport[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s, remove nni gport[0] failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_vmac_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

