/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        fec_performance.h
 * Purpose:     fec allocation performance test
 */


#include <appl/dpp/FecPerformance/fec_performance.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/mpls.h>
#include <bcm/types.h>
#include <bcm/failover.h>
#include <sal/core/libc.h> 
#include <sal/appl/io.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#define FEC_PERFORMACE_DEFAULT_ECMP_GROUP_SIZE 256

static int fec_performance_l1_port_create(int unit, uint8 pp_port_id);
static int fec_performance_l2_port_create(int unit, uint8 pp_port_id, uint16 rif, bcm_gport_t lif);
static int fec_performance_l3_eedb_create(int unit, int print_dbg, uint8 pp_port_id, uint16 rif, bcm_mac_t mac_l3_egress, bcm_if_t *encap_id);
static int fec_performance_mpls_tunnels(int unit, int print_dbg, int num_of_tunnels, uint16 rif, bcm_if_t ll_encap_id, bcm_if_t *first_fec_id, bcm_mpls_label_t egress_start_label);
static int fec_performance_l3_ecmp_create(int unit, int print_dbg, bcm_if_t ecmp_id, bcm_if_t first_fec_id);
static int fec_performance_mpls_label_route_create(int unit, bcm_if_t ecmp_id, bcm_mpls_label_t ingress_label);

int fec_performance_main(int unit, int print_dbg, int ecmp_count, int cleanup)
{
	uint8 pp_port_id = 14;
	uint16 rif = 1000;
	bcm_gport_t lif = 5000;
	bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
	int rc = BCM_E_NONE;
	bcm_if_t encap_id, first_fec_id, ecmp_id;
	bcm_mpls_label_t ingress_label, egress_start_label;

	ingress_label = 1;
	egress_start_label = 1;

	soc_sand_ll_timer_clear();

	soc_sand_ll_timer_set("Total Time", 1);

	rc = fec_performance_l1_port_create(unit, pp_port_id);
	if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_l1_port_create has failed")));
         goto exit;
     }

	rc = fec_performance_l2_port_create(unit, pp_port_id, rif, lif);
	if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_l2_port_create has failed")));
         goto exit;
     }

	rc = fec_performance_l3_eedb_create(unit, print_dbg, pp_port_id, rif, mac_l3_egress, &encap_id);
	if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_l3_eedb_create has failed")));
         goto exit;
     }

	first_fec_id = 4096;
	rc = fec_performance_mpls_tunnels(unit, print_dbg, ecmp_count * FEC_PERFORMACE_DEFAULT_ECMP_GROUP_SIZE, rif, encap_id, &first_fec_id, egress_start_label);
	if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_mpls_tunnels has failed")));
         goto exit;
     }
	
	for (ecmp_id = 1; ecmp_id <= ecmp_count; ecmp_id++) {
		rc = fec_performance_l3_ecmp_create(unit, print_dbg, ecmp_id, first_fec_id);
		if (rc != BCM_E_NONE)
        {
			LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_l3_ecmp_create has failed")));
			goto exit;
		}

		rc = fec_performance_mpls_label_route_create(unit, ecmp_id, ingress_label);
		if (rc != BCM_E_NONE)
		{
			LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"fec_performance_mpls_label_route_create has failed")));
			goto exit;
		}

		first_fec_id += FEC_PERFORMACE_DEFAULT_ECMP_GROUP_SIZE;
		ingress_label += 1;
	}
	
	soc_sand_ll_timer_stop(1);
	soc_sand_ll_timer_print_all();

exit:
   return rc;
}

int fec_performance_l1_port_create(int unit, uint8 pp_port_id)
{
    int rc = BCM_E_NONE;
    uint32 pp_port_flags; 
    bcm_port_interface_info_t nif_info;
    bcm_port_mapping_info_t   port_mapping_info;
    bcm_port_info_t           port_l1_info;

    sal_memset(&nif_info, 0, sizeof(nif_info));
    sal_memset(&port_mapping_info, 0, sizeof(port_mapping_info));
    sal_memset(&port_l1_info, 0, sizeof(port_l1_info));
    /*
     * It is assumed that the SOC file has already setup the NIF 
     * and allocated PP port, call BCM SDK to get the nif and pp port info
     * info and populte the info in the L1 port's table object.
     */
    rc = bcm_port_get(unit, 
                      pp_port_id,
                      &pp_port_flags,
                      &nif_info, 
                      &port_mapping_info);
    if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_port_get has failed")));
         goto exit;
     }


    rc = bcm_port_info_get(unit,
                           pp_port_id,
                           &port_l1_info);
    if (rc != BCM_E_NONE)
     {
         LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_port_info_get has failed")));
         goto exit;
     }

exit:
   return rc;
}

int fec_performance_l2_port_create (int unit, uint8 pp_port_id, uint16 rif, bcm_gport_t lif)
{

    int rc = BCM_E_NONE;
    bcm_vlan_port_t  vlan_port;
    bcm_l3_intf_t    l3if;
    bcm_l3_ingress_t l3_ing_if;
    uint8 bia_mac[6] = {0x0, 0xa, 0xb, 0xc, 0xd, 0xe};

    /* 
     * Program the rule IRPP ucode will use to Assign the pp port to a vlan domain  : 
     * The bcmPortClassId rule means:
     * If RX packet is tagged, then vlan domain = VLAN ID from packet's MAC header
     * If RX packet is untagged, then vlan domain = Initial VLAN ID which is
     * assigned to the port using bcm_port_untagged_vlan_set. 
     */
    rc = bcm_port_class_set(unit,
                            pp_port_id,  
                            bcmPortClassId,
                            pp_port_id);
	if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_port_class_set has failed")));
        goto exit;
    }

    /* 
     * create the RIF = VSI = vlan domain. IRPP ucode 
     * will assign port's RX untagged traffic to this vlan domain.
 */
    bcm_l3_intf_t_init(&l3if);
    bcm_l3_ingress_t_init(&l3_ing_if);
    sal_memcpy(&(l3if.l3a_mac_addr), bia_mac, 6);
    l3if.l3a_flags |= BCM_L3_UNTAG |  BCM_L3_WITH_ID;
    l3if.l3a_vid = rif; 
    l3if.l3a_ttl = 31; 
    l3if.l3a_mtu = 1524;
    l3if.l3a_vrf = 0;
    l3if.l3a_intf_id = rif;
    rc = bcm_l3_intf_create(unit, &l3if);
    if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_intf_create has failed")));
        goto exit;
    }

     /*  
      * Set Initial vlan assignment  RIF = VSI for 
      * untagged traffic received on the port.
      * IRRP link layer ucode will first insert this VLAN ID into the packet
      * Then IRRP vlan translation code will classify port+initial vlan to the port's LIF
      */
	rc = bcm_port_untagged_vlan_set(unit,
                                     pp_port_id,
                                     rif);
    if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_port_untagged_vlan_set has failed")));
        goto exit;
    }

     /*
      * Now create the LIF (AKA vlan port) and associated with the RIF.
      */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
    vlan_port.flags =  BCM_VLAN_PORT_EGRESS_UNTAGGED;
    vlan_port.match_vlan = pp_port_id;
    vlan_port.vsi = rif; 
    vlan_port.port = pp_port_id;
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, lif);
    rc = bcm_vlan_port_create(unit, &vlan_port);
    if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_vlan_port_create has failed")));
        goto exit;
    }

    /*
     * L3 and L2 setup is now complete. so now enable port's L1 TX and RX links
     */
    rc = bcm_port_enable_set(unit, pp_port_id,  1); 
    if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_port_enable_set has failed")));
        goto exit;
    }

exit:
   return rc;
}


int fec_performance_l3_eedb_create(int unit, int print_dbg, uint8 pp_port_id, uint16 rif,bcm_mac_t mac_l3_egress, bcm_if_t *encap_id)
{
	bcm_l3_egress_t l3eg;
	bcm_if_t l3egid;
	uint32 flags;
	int rc = BCM_E_NONE;

	bcm_l3_egress_t_init(&l3eg);
	sal_memcpy(l3eg.mac_addr, mac_l3_egress, 6); /*Destination MAC address*/
	l3eg.vlan = rif;
	flags = BCM_L3_EGRESS_ONLY;
	l3egid = 0;

	rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
	if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_egress_create has failed")));
        goto exit;
    }

	if (print_dbg == 1) {
		LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_l3_egress_create created EEDB entry with encap_id: %x\n"), l3eg.encap_id));
	}

	*encap_id = l3eg.encap_id;

exit:
   return rc;
}

int fec_performance_mpls_tunnels(int unit, int print_dbg, int num_of_tunnels, uint16 rif, bcm_if_t ll_encap_id, bcm_if_t *first_fec_id, bcm_mpls_label_t egress_start_label){

	bcm_mpls_egress_label_t label_array[1];
	bcm_l3_egress_t l3eg;
	bcm_if_t l3egid, fec_id;
	uint32 flags;
	bcm_failover_t failover_id_fec, primary_tunnel_id;
	int i, rc = BCM_E_NONE;

	fec_id = *first_fec_id;
	primary_tunnel_id = failover_id_fec = 0;
	for (i = 0; i < num_of_tunnels; i++) {
		bcm_mpls_egress_label_t_init(&label_array[0]);
		label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT); 
		label_array[0].label =  egress_start_label;
		label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
		label_array[0].ttl = 30;
        label_array[0].l3_intf_id = rif;

		rc = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
		if (rc != BCM_E_NONE)
		{
			LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_mpls_tunnel_initiator_create has failed")));
			goto exit;
		}

		if (print_dbg == 1) {
			LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_mpls_tunnel_initiator created tunnel %x, with label %d, pointing to Out-Rif %d\n"),
				   label_array[0].tunnel_id, 
				   label_array[0].label,
				   label_array[0].l3_intf_id));
		}

		bcm_l3_egress_t_init(&l3eg);
		l3eg.intf = label_array[0].tunnel_id;
		l3eg.encap_id = ll_encap_id;
		flags = (BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE);
		l3egid = 0;
		rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
		if (rc != BCM_E_NONE)
		{
			LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_egress_create has failed")));
			goto exit;
		}

		if (print_dbg == 1) {
			LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_l3_egress_create updated EEDB entry with encap_id: %x to point to MPLS tunnel %x\n"),
				   l3eg.encap_id, label_array[0].tunnel_id));
		}

		if (i % 2 == 1) {

			rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
			if (rc != BCM_E_NONE)
			{
				LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_failover_create has failed")));
				goto exit;
			}

			/*Protecton fec*/
			bcm_l3_egress_t_init(&l3eg);
			l3eg.intf = label_array[0].tunnel_id;
			l3eg.failover_id = failover_id_fec;
			l3eg.failover_if_id = 0;
			l3egid = fec_id;
			flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;

			soc_sand_ll_timer_set("Protection FEC- Create BCM", 2);

			rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);

			soc_sand_ll_timer_stop(2);

			if (rc != BCM_E_NONE)
			{
				LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_egress_create has failed")));
				goto exit;
			}

			/*Primary FEC*/
			bcm_l3_egress_t_init(&l3eg);
			l3eg.intf = primary_tunnel_id;
			l3eg.failover_id = failover_id_fec;
			l3eg.failover_if_id = l3egid;
			flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
			l3egid = fec_id - 1;

			soc_sand_ll_timer_set("Primary FEC- Create BCM", 3);

			rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);

			soc_sand_ll_timer_stop(3);

			if (rc != BCM_E_NONE)
			{
				LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_egress_create has failed")));
				goto exit;
			}

			if (print_dbg == 1) {
				LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_l3_egress_create Primary-FEC %x, Pointing to tunnel: %x\n"),fec_id, primary_tunnel_id));
				LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_l3_egress_create Secondary-FEC %x, Pointing to tunnel: %x\n"), fec_id - 1, label_array[0].tunnel_id));
			}

			if (i == 1) {
				/*In the first round, save the id returned from the API*/
				*first_fec_id = l3egid;
			}
		} else {
			primary_tunnel_id = label_array[0].tunnel_id;
		}

		/*Increment label and fec-id, as preperation for next tuunel*/
		egress_start_label++;
		fec_id++;
	}

exit:
   return rc;
}

int fec_performance_l3_ecmp_create(int unit, int print_dbg, bcm_if_t ecmp_id, bcm_if_t first_fec_id){
	bcm_l3_egress_ecmp_t ecmp;
	bcm_if_t ecmp_inf[FEC_PERFORMACE_DEFAULT_ECMP_GROUP_SIZE / 2];
	int i, ecmp_group_size, rc = BCM_E_NONE;

	/*Initilize FEC-ID array*/
	ecmp_group_size = FEC_PERFORMACE_DEFAULT_ECMP_GROUP_SIZE / 2;
	for (i = 0; i < ecmp_group_size; i++) {
		ecmp_inf[i] = first_fec_id;
		first_fec_id += 2;
	}

	bcm_l3_egress_ecmp_t_init(&ecmp);
	ecmp.ecmp_intf = ecmp_id;
	ecmp.max_paths = ecmp_group_size;
	ecmp.flags = BCM_L3_WITH_ID;

	soc_sand_ll_timer_set("ECMP- Create BCM", 4);

	rc = bcm_l3_egress_ecmp_create(unit, &ecmp, ecmp_group_size, ecmp_inf);

	soc_sand_ll_timer_stop(4);

	if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_l3_egress_ecmp_create has failed")));
		goto exit;
	}

	if (print_dbg  == 1) {
		LOG_INFO(BSL_LS_APPL_OTHER,(BSL_META_U(unit,"L3: bcm_l3_egress_ecmp_create Created ECMP %x, group size: %d\n"),ecmp_id, ecmp_group_size));
	}

exit:
   return rc;
}

int fec_performance_mpls_label_route_create(int unit, bcm_if_t ecmp_id, bcm_mpls_label_t ingress_label){
	int rc = BCM_E_NONE;
	bcm_mpls_tunnel_switch_t tunnel_switch;


    bcm_mpls_tunnel_switch_t_init(&tunnel_switch);
    tunnel_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.label = ingress_label;
    tunnel_switch.flags = (BCM_MPLS_SWITCH_TTL_DECREMENT|BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP);
    tunnel_switch.egress_if = ecmp_id;

    rc = bcm_mpls_tunnel_switch_create(unit, &tunnel_switch);
	if (rc != BCM_E_NONE)
	{
		LOG_ERROR(BSL_LS_APPL_OTHER, (BSL_META_U(unit,"bcm_mpls_tunnel_switch_create has failed")));
		goto exit;
	}

exit:
   return rc;
}

