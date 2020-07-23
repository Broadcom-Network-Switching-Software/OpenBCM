/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

int
check_error_by_percent(uint32 original, uint32 computed, float percent)
{
	float high_limit = (1+percent)*original;
	float low_limit  = (1-percent)*original;
	float val = computed;
	printf("val=%f,high_limit=%f,low_limit=%f\n",val,high_limit,low_limit);
	if ((val > high_limit) || (val < low_limit)) {
		return 1;
	}
	return 0;
}

 /* set get basic example -
	check that the quantiaztion error is not greater than 5 percent */
int
policer_set_get_example(int unit, int in_port, int packet_mode, int color_mode, int header_truncate) {

	int rv;
    uint32 kbits_sec, kbits_burst;
        bcm_info_t info;

	int packet_mode_flag = packet_mode == 0 ? 0 : BCM_RATE_MODE_PACKETS;
	int color_mode_flag = color_mode == 0 ? 0 : BCM_RATE_COLOR_BLIND;
	int header_truncate_flag = header_truncate == 0 ? 0: BCM_RATE_PKT_ADJ_HEADER_TRUNCATE;

	/* set 1800MB rate for unknown unicast traffic */
	rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF|packet_mode_flag|header_truncate_flag|color_mode_flag, 1800000, 5500);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_set for BCM_RATE_DLF\n");
		return rv;
	}
	rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_DLF, &kbits_sec, &kbits_burst);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_get for BCM_RATE_DLF\n");
		return rv;
	}
	rv = check_error_by_percent(1800000,kbits_sec,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer rate value for BCM_RATE_DLF, original = %d, computed=%d\n",1800000,kbits_sec);
		return rv;
	}
	rv = check_error_by_percent(5500,kbits_burst,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer burst value for BCM_RATE_DLF, original = %d, computed=%d\n",5500,kbits_burst);
		return rv;
	}

	/* set 250MB rate for unicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST|packet_mode_flag|header_truncate_flag|color_mode_flag, 250000, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UCAST\n");
        return rv;
    }
	rv = check_error_by_percent(250000,kbits_sec,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer rate value for BCM_RATE_UCAST, original = %d, computed=%d\n",250000,kbits_sec);
		return rv;
	}
	rv = check_error_by_percent(300,kbits_burst,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer burst value for BCM_RATE_UCAST, original = %d, computed=%d\n",300,kbits_burst);
		return rv;
	}

    /* set 230MB rate for unknown multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST|packet_mode_flag|header_truncate_flag|color_mode_flag, 230000, 7000);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UNKNOWN_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
	rv = check_error_by_percent(230000,kbits_sec,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer rate value for BCM_RATE_UNKNOWN_MCAST, original = %d, computed=%d\n",230000,kbits_sec);
		return rv;
	}

        rv = bcm_info_get(unit, &info);
	if (info.device == 0x8660 || info.device == 0x8650) {
	    rv = check_error_by_percent(2080,kbits_burst,0.05);
	    if (rv != BCM_E_NONE) {
		    printf("Error in policer burst value for BCM_RATE_UNKNOWN_MCAST, original = %d, computed=%d\n",2080,kbits_burst);
		    return rv;
	    }
	} else {
	    rv = check_error_by_percent(7000,kbits_burst,0.05);
	    if (rv != BCM_E_NONE) {
		    printf("Error in policer burst value for BCM_RATE_UNKNOWN_MCAST, original = %d, computed=%d\n",7000,kbits_burst);
		    return rv;
	    }
	}

    /* set 500MB rate for multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST|packet_mode_flag|header_truncate_flag|color_mode_flag, 500000, 400);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_MCAST\n");
        return rv;
    }
	rv = check_error_by_percent(500000,kbits_sec,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer rate value for BCM_RATE_MCAST, original = %d, computed=%d\n",500000,kbits_sec);
		return rv;
	}
	rv = check_error_by_percent(400,kbits_burst,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer burst value for BCM_RATE_MCAST, original = %d, computed=%d\n",400,kbits_burst);
		return rv;
	}

	/* set 100MB rate for broadcast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST|packet_mode_flag|header_truncate_flag|color_mode_flag, 100000, 200);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_BCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_BCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_BCAST\n");
        return rv;
    }
	rv = check_error_by_percent(100000,kbits_sec,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer rate value for BCM_RATE_BCAST, original = %d, computed=%d\n",100000,kbits_sec);
		return rv;
	}
	rv = check_error_by_percent(200,kbits_burst,0.05);
	if (rv != BCM_E_NONE) {
		printf("Error in policer burst value for BCM_RATE_BCAST, original = %d, computed=%d\n",200,kbits_burst);
		return rv;
	}
    return rv;
}


/*
 * create an Ethernet Policer for each traffic type: UC, unknown UC, MC, unknown MC and BC.
 * get each entry, to see the rates and bursts that were actually set (might be a little different).
 * send traffic and check that rate matches the Policer configuration.
 * Note- these interfaces are not supported for Jericho at the moment
 *
 * run:
 * cint examples/cint_rate_policer_example.c
 * cint
 * print traffic_example(unit, <in_port>, <out_port>);
 *
 * traffic to run:
 * 1) UC: run ethernet packet with DA 1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 250Mbit rate
 *
 * 2) MC: run ethernet packet with DA ff:0:0:0:0:1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 500Mbit rate
 *
 * 3) BC: run ethernet packet with DA ff:ff:ff:ff:ff:ff and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 100Mbit rate
 *
 * Distinguishing meter red from Ethernet policer red:
 * When a packet arrives red at a meter, the meter will output red as well.
 * In this situation there is no way to distinguish whether the meter decided
 * to drop the packet (e.g. the packet arrived green, but the meter decided
 * to drop it) or if it arrived red at the meter (in which case the meter
 * will keep the red color).
 * If the SOC property policer_color_resolution_mode=1 then:
 * If a packet arrives red at the meter, the final DP will be 3.
 * If the packet does not arrive red, but the meter decides to drop it,
 * then the DP will be 2.
 */
int
traffic_example(int unit, int in_port, int out_port, int packet_mode) {

    int rv;
    uint32 kbits_sec, kbits_burst;
    bcm_l2_addr_t l2addr;
    bcm_mac_t uc_mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t mc_mac = {0xff, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t bc_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int packet_mode_flag = packet_mode == 0 ? 0 : BCM_RATE_MODE_PACKETS;

	printf("Set the device to drop all red packets...\n");
	rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_cosq_discard_set for BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK.\n");
		return rv;
	}

	rv = bcm_port_vlan_priority_map_set(unit, in_port, 0 /* priority */, 0 /* cfi */,
											  0 /* internal priority */, bcmColorGreen /* color */);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
		print rv;
		return rv;
	}

	/* set 350MB rate for unknown unicast traffic */
	rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF|BCM_RATE_PKT_ADJ_HEADER_TRUNCATE|packet_mode_flag, 350000, 300);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_set for BCM_RATE_DLF\n");
		return rv;
	}
	rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_DLF, &kbits_sec, &kbits_burst);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_get for BCM_RATE_DLF\n");
		return rv;
	}
	printf("for BCM_RATE_DLF, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 250MB rate for unicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST|packet_mode_flag, 250000, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 230MB rate for unknown multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST|packet_mode_flag, 230000, 7000);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UNKNOWN_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UNKNOWN_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 500MB rate for multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST|packet_mode_flag, 500000, 400);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 100MB rate for broadcast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST|packet_mode_flag, 100000, 200);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_BCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_BCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_BCAST\n");
        return rv;
    }
    printf("for BCM_RATE_BCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* add UC, MC and BC entries (with Vlan 1) and send to out_port */
    bcm_l2_addr_t_init(&l2addr, uc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for UC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, mc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for MC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, bc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for BC mac\n");
        return rv;
    }

    return rv;
}
/*
 *A traffic example with flag BCM_RATE_COLOR_BLIND
 */
int
traffic_example_with_color_mode(int unit, int in_port, int out_port, int packet_mode,int color_blind) {

    int rv;
    uint32 kbits_sec, kbits_burst;
    bcm_l2_addr_t l2addr;
    bcm_mac_t uc_mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t mc_mac = {0xff, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t bc_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int packet_mode_flag = packet_mode == 0 ? 0 : BCM_RATE_MODE_PACKETS;
    int color_blind_flag = color_blind == 0 ? 0 : BCM_RATE_COLOR_BLIND;

    /* Because bcmColorRed is not supported by this API, bcmColorYellow is used here.
     * In Ethernet meter, any non-green packets should be identified as red in color aware mode*/
	rv = bcm_port_vlan_priority_map_set(unit, in_port, 3 /* priority */, 0 /* cfi */,
											0 /* internal priority */, bcmColorYellow /* color */);
	rv = bcm_port_vlan_priority_map_set(unit, in_port, 0 /* priority */, 0 /* cfi */,
											0 /* internal priority */, bcmColorGreen /* color */);
	printf("Set the device to drop all red packets...\n");
	rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE |  BCM_COSQ_DISCARD_COLOR_RED
                              | BCM_COSQ_DISCARD_COLOR_BLACK);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_cosq_discard_set for BCM_COSQ_DISCARD_ENABLE
        | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_BLACK.\n");
		return rv;
	}

	/* set 350MB rate for unknown unicast traffic */
	rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF|BCM_RATE_PKT_ADJ_HEADER_TRUNCATE |
                                packet_mode_flag | color_blind_flag, 350000, 300);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_set for BCM_RATE_DLF\n");
		return rv;
	}
	rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_DLF, &kbits_sec, &kbits_burst);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_get for BCM_RATE_DLF\n");
		return rv;
	}
	printf("for BCM_RATE_DLF, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 250MB rate for unicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST
                                |packet_mode_flag|color_blind_flag, 250000, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 230MB rate for unknown multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST
                                |packet_mode_flag|color_blind_flag, 230000, 7000);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_UNKNOWN_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_UNKNOWN_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* set 500MB rate for multicast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST
                                |packet_mode_flag|color_blind_flag, 500000, 400);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_MCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_MCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_MCAST\n");
        return rv;
    }
    printf("for BCM_RATE_MCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

	/* set 100MB rate for broadcast traffic */
    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST
                                |packet_mode_flag|color_blind_flag, 100000, 200);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set for BCM_RATE_BCAST\n");
        return rv;
    }
    rv = bcm_rate_bandwidth_get(unit, in_port, BCM_RATE_BCAST, &kbits_sec, &kbits_burst);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_get for BCM_RATE_BCAST\n");
        return rv;
    }
    printf("for BCM_RATE_BCAST, set rate %d and burst %d\n", kbits_sec, kbits_burst);

    /* add UC, MC and BC entries (with Vlan 1) and send to out_port */
    bcm_l2_addr_t_init(&l2addr, uc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for UC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, mc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for MC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, bc_mac, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for BC mac\n");
        return rv;
    }

    return rv;
}
/* disable all the Policers set at traffic_example() */
int
policer_reset(int unit, int in_port) {

    int rv;

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_BCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_BCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_MCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_MCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_UCAST\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_DLF, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_DLF\n");
        return rv;
    }

    rv = bcm_rate_bandwidth_set(unit, in_port, BCM_RATE_UNKNOWN_MCAST, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rate_bandwidth_set with 0 for BCM_RATE_UNKNOWN_MCAST\n");
        return rv;
    }

    return rv;
}

/* policer_verify_max_allocation - check that the maximum policers allocation is 31*/
int
 policer_verify_max_allocation(int unit,  int port0, int port1, int port2, int port3, int port4, int port5, int port6)
{
	int i,rv;
	int in_port;
    uint32
	 kbits_sec   = 1000,
	 kbits_burst = 100;
	int max_alloc = 31;
	int ports_arr[7];
	int type_arr[5] = {BCM_RATE_DLF,BCM_RATE_BCAST,BCM_RATE_UCAST,BCM_RATE_MCAST,BCM_RATE_UNKNOWN_MCAST};

	ports_arr[0] = port0;
	ports_arr[1] = port1;
	ports_arr[2] = port2;
	ports_arr[3] = port3;
	ports_arr[4] = port4;
	ports_arr[5] = port5;
	ports_arr[6] = port6;

	for (i=1;i<=max_alloc;i++) {
		in_port = ports_arr[i/5];
		rv = bcm_rate_bandwidth_set(unit, in_port, type_arr[i%5], kbits_sec*i, kbits_burst*i);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_rate_bandwidth_set allocation\n");
		}
	}
	rv = bcm_rate_bandwidth_set(unit, in_port, type_arr[i%5], kbits_sec*i, kbits_burst*i);
	if (rv == BCM_E_NONE) {
		printf("Error in bcm_rate_bandwidth_set allocation : allocat more than the maximum possible allocations, %d\n\n",max_alloc);
		return 1;
	}
	return 0;
}

/* policer_aggregate_policer_example : shows how to create, configure and destroy a group of policers
   *    1. allocate group of policers with mode and core indication
   *    2. add configuration for each sub-policer
   *    3. set port(s) to the group of policers
   *    4. cancel policing of a specific sub-policer, for example policer_id + 1
   *    5. destroy all policers: two steps
   *    	5A. unset the ports from the associated group by passing polcier_id=0
   *    	5B. destoy sub-policers one by one
   *    		(or call bcm_policer_destroy_all(unit))
*/
int
 policer_aggregate_policer_example(int unit, int core, int port0)
{
	int i,rv;

	bcm_policer_aggregate_group_info_t group_info;
	bcm_policer_config_t pol_cfg;
	bcm_policer_t policer_id;
	int nof_pol;

    group_info.mode = bcmPolicerGroupModeTrafficType;
    group_info.core = core;

	/* 1. allocate group of policers with mode and core indication*/
	rv = bcm_policer_aggregate_group_create(unit, &group_info, &policer_id, &nof_pol);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_policer_aggregate_group_create\n");
		return rv;
	}

	/* 2. add configuration for each sub-policer */
	for (i=0;i<nof_pol;i++) {
		bcm_policer_config_t_init(&pol_cfg);
		pol_cfg.ckbits_sec = 100000+i*500000;
		pol_cfg.ckbits_burst = 1000+50*i;
		if (i == 1) {
			pol_cfg.flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
		}
		rv = bcm_policer_set(unit, (policer_id + i), &pol_cfg);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_set\n");
			return rv;
		}
	}

	/* 3. set port(s) to the group of policers*/
	rv = bcm_port_policer_set(unit,port0,policer_id);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port0);
		return rv;
	}
/* optional: set more than one port to the same group

	rv = bcm_port_policer_set(unit,port1,policer_id);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port1);
		return rv;
	}
*/
	/* 4. cancel policing of a specific sub-policer, for example policer_id + 1*/
	bcm_policer_config_t_init(&pol_cfg);
	pol_cfg.ckbits_sec = 0;
	pol_cfg.ckbits_burst = 0;
	rv = bcm_policer_set(unit, (policer_id+1), &pol_cfg);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_policer_set\n");
		return rv;
	}

	/* 5. destroy all policers: two steps*/
	/* 5A. unset the ports from the associated group by passing polcier_id=0*/
	rv = bcm_port_policer_set(unit,port0,0);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port0);
		return rv;
	}
/* optional: set more than one port to the same group

	rv = bcm_port_policer_set(unit,port1,0);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port1);
		return rv;
    }
*/

	/* 5B. destoy sub-policers one by one
	   or call bcm_policer_destroy_all(unit)
	*/
	for (i=0;i<nof_pol;i++) {
		rv = bcm_policer_destroy(unit, policer_id+i);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_petra_policer_destroy\n");
			return rv;
		}
	}
	return 0;
}

int
 policer_aggregate_group_regression_test(int unit, int port0)
{
	int i,rv;
	bcm_policer_aggregate_group_info_t group_info;
	bcm_policer_config_t pol_cfg;
	bcm_policer_t policer_id_0,policer_id_1;
	int nof_pol;
	int dual_core;
	bcm_info_t info;

  /* NOTE:
   * Two aggregate policers are created here, just for test usage.
   * One aggregate group in one core is enough - the user should know in which core it should be created
   */

	rv = bcm_info_get(unit, &info);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_info_get\n");
		return rv;
	}

	if((1 == is_device_or_above(unit,JERICHO)) && (0 == is_device_or_above(unit,QUMRAN_AX))){
		if(sal_strcmp(soc_property_get_str(unit, spn_DEVICE_CORE_MODE),"SINGLE_CORE") == 0){
			dual_core = 0;
		}else{
			dual_core = 1;
		}
	} else {
		dual_core = 0;
	}

    group_info.mode = bcmPolicerGroupModeTrafficType;
    group_info.core = 0;

	/* 1. allocate group of policers with mode and core indication*/
	rv = bcm_policer_aggregate_group_create(unit, &group_info, &policer_id_0, &nof_pol);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_policer_aggregate_group_create\n");
		return rv;
	}

	if (dual_core == 1) {
		group_info.core = 1;
		/* 1. allocate group of policers with mode and core indication*/
		rv = bcm_policer_aggregate_group_create(unit, &group_info, &policer_id_1, &nof_pol);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_aggregate_group_create\n");
			return rv;
		}
	}

	/* 2. add configuration for each sub-policer */
	for (i=0;i<nof_pol;i++) {
		bcm_policer_config_t_init(&pol_cfg);
		pol_cfg.ckbits_sec = 100000+i*500000;
		pol_cfg.ckbits_burst = 1000+50*i;
		rv = bcm_policer_set(unit, (policer_id_0+i), &pol_cfg);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_set\n");
			return rv;
		}
		if(dual_core == 1){
			rv = bcm_policer_set(unit, (policer_id_1+i), &pol_cfg);
			if (rv != BCM_E_NONE) {
				printf("Error in bcm_policer_set\n");
				return rv;
			}
		}
	}

	/* 3. set port(s) to the group of policers*/
	rv = bcm_port_policer_set(unit,port0,policer_id_0);
	if (rv != BCM_E_NONE) {
		if(dual_core == 1){
			rv = bcm_port_policer_set(unit,port0,policer_id_1);
		}
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_port_policer_set, port =%d\n",port0);
			return rv;
		}
	}
	/* 4. cancel policing of a specific sub-policer, for example policer_id + 1*/
	bcm_policer_config_t_init(&pol_cfg);
	pol_cfg.ckbits_sec = 0;
	pol_cfg.ckbits_burst = 0;
	rv = bcm_policer_set(unit, (policer_id_0+1), &pol_cfg);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_policer_set\n");
		return rv;
	}

	if(dual_core == 1){
		rv = bcm_policer_set(unit, (policer_id_1+1), &pol_cfg);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_set\n");
			return rv;
		}
	}

	/* 5. destroy all policers: two steps*/
	/* 5A. unset the ports from the associated group by passing polcier_id=0*/
	rv = bcm_port_policer_set(unit,port0,0);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port0);
		return rv;
	}

	/* 5B.bcm_policer_destroy_all(unit)
	*/
	rv = bcm_policer_destroy_all(unit);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_petra_policer_destroy\n");
		return rv;
	}

	return 0;
}

/*
 * create an Ethernet Policer for each traffic type: UC, unknown UC, MC, unknown MC and BC.
 * get each entry, to see the rates and bursts that were actually set (might be a little different).
 * send traffic and check that rate matches the Policer configuration.
 * Note- these interfaces are supported for Jericho
 *
 * run:
 * cint examples/cint_rate_policer_example.c
 * cint
 * print traffic_example(unit, <in_port>, <out_port>);
 *
 * traffic to run:
 * 1) UC (known or/and unknown): run ethernet packet with DA 1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 100Mbit rate
 *
 * 2) MC (known or/and unknown): run ethernet packet with DA ff:0:0:0:0:1 and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 150Mbit rate
 *
 * 3) BC: run ethernet packet with DA ff:ff:ff:ff:ff:ff and vlan tag id 1 from in_port
 *    traffic will arrive at out_port with 200Mbit rate
 *
 * Distinguishing meter red from Ethernet policer red:
 * When a packet arrives red at a meter, the meter will output red as well.
 * In this situation there is no way to distinguish whether the meter decided
 * to drop the packet (e.g. the packet arrived green, but the meter decided
 * to drop it) or if it arrived red at the meter (in which case the meter
 * will keep the red color).
 * If the SOC property policer_color_resolution_mode=1 then:
 * If a packet arrives red at the meter, the final DP will be 3.
 * If the packet does not arrive red, but the meter decides to drop it,
 * then the DP will be 2.
 */
int
aggregate_policer_traffic_example(int unit, int core ,int port1, int port2, int mode) {

    int rv,i;
	bcm_policer_aggregate_group_info_t group_info;
	bcm_policer_config_t pol_cfg;
	bcm_policer_t policer_id;
	int nof_pol;

    bcm_l2_addr_t l2addr;
    bcm_mac_t uc_mac1 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t mc_mac1 = {0xff, 0x0, 0x0, 0x0, 0x0, 0x1};
    bcm_mac_t bc_mac1 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	bcm_mac_t uc_mac2 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x2};
    bcm_mac_t mc_mac2 = {0xff, 0x0, 0x0, 0x0, 0x0, 0x2};
    bcm_mac_t bc_mac2 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	printf("Set the device to drop all red packets...\n");
	rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_cosq_discard_set for BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK.\n");
		return rv;
	}

	rv = bcm_port_vlan_priority_map_set(unit, port1, 0 /* priority */, 0 /* cfi */,
											  0 /* internal priority */, bcmColorGreen /* color */);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_port_vlan_priority_map_set with prio \n");
		print rv;
		return rv;
	}

	rv = bcm_port_vlan_priority_map_set(unit, port2, 0 /* priority */, 0 /* cfi */,
											  0 /* internal priority */, bcmColorGreen /* color */);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_port_vlan_priority_map_set with prio \n");
		print rv;
		return rv;
	}

    group_info.mode = mode;
    group_info.core = core;

	rv = bcm_policer_aggregate_group_create(unit, &group_info, &policer_id, &nof_pol);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_policer_aggregate_group_create\n");
		return rv;
	}

	/* 2. add configuration for each sub-policer */
	for (i=0;i<nof_pol;i++) {
		bcm_policer_config_t_init(&pol_cfg);
		pol_cfg.ckbits_sec = 100000+i*50000;
		pol_cfg.ckbits_burst = 1000+500*i;
		rv = bcm_policer_set(unit, (policer_id+i), &pol_cfg);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_set\n");
			return rv;
		}
	}
	/* 3.1. set port(s) to the group of policers*/
	rv = bcm_port_policer_set(unit,port1,policer_id);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port1);
		return rv;
	}

	/* 3.2. set port(s) to the group of policers*/
	rv = bcm_port_policer_set(unit,port2,policer_id);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_policer_set, port =%d\n",port2);
		return rv;
	}

    /* add UC, MC and BC entries (with Vlan 1) and send to port1 */
    bcm_l2_addr_t_init(&l2addr, uc_mac1, 1);
    l2addr.port = port1;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for UC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, mc_mac1, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for MC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, bc_mac1, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for BC mac\n");
        return rv;
    }

	/* add UC, MC and BC entries (with Vlan 1) and send to port2 */
    bcm_l2_addr_t_init(&l2addr, uc_mac2, 1);
    l2addr.port = port2;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for UC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, mc_mac2, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for MC mac\n");
        return rv;
    }

    sal_memcpy(l2addr.mac, bc_mac2, 6);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add for BC mac\n");
        return rv;
    }

    return rv;
}


