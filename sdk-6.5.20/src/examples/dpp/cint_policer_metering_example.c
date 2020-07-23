/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Note- run the following first:
 * cint examples/sand/utility/cint_sand_utils_global.c
 * 1) metering_example() - create several meter entries, in different modes.
 * get each entry, to see the rates and bursts that were actually set (might be a little different).
 *
 * 2) traffic_example() - create a meter according to configuration (single/two rate, color blind or not) and map traffic to this meter.
 * traffic with priority=1 will be arrive with a green color and with priority=4 will be arrive with a yellow color.
 * for each configuration and packet priority, a different traffic rate will be received in out-port.
 *
 * 3) two_meter_per_packet_example() - create 2 meters from group=0 and 1 meter from group=1.
 * each packet is mapped to 2 meters, a meter from group=0 and a meter from group=1.
 * both packets share the same meter from group=1, but have different meters from group=0.
 * run both streams together and get the resolved traffic rate.
 *
 * 4) metering_reset() - destroys all the meter entries.
 *
 * how to run each example is explained bellow.
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

struct policer_metering_info_s {

    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_policer_config_t pol_cfg_get;

    bcm_policer_config_t pol2_cfg;
    bcm_policer_t policer_id2;
    bcm_policer_config_t pol2_cfg_get;

    bcm_policer_config_t pol3_cfg;
    bcm_policer_t policer_id3;
    bcm_policer_config_t pol3_cfg_get;

    bcm_policer_config_t pol4_cfg;
    bcm_policer_t policer_id4;
    bcm_policer_config_t pol4_cfg_get;

    bcm_policer_config_t pol5_cfg;
    bcm_policer_t policer_id5;
    bcm_policer_config_t pol5_cfg_get;

    bcm_policer_config_t pol5_1_cfg;

	bcm_policer_config_t pol6_cfg;
    bcm_policer_t policer_id6;
    bcm_policer_config_t pol6_cfg_get;

    bcm_policer_config_t same_prf_1_pol_cfg;
    bcm_policer_t        same_prf_1_policer_id;
    bcm_policer_config_t same_prf_1_pol_cfg_get;

    bcm_policer_config_t same_prf_2_pol_cfg;
    bcm_policer_t        same_prf_2_policer_id;
    bcm_policer_config_t same_prf_2_pol_cfg_get;

	uint8 with_check;
};

policer_metering_info_s metering_info;

metering_info.policer_id2 = 0;
metering_info.with_check = 0;
float five_percent = 5;
float twenty_percent = 20;




/*
 * Set header compensation to be used by the metering and ethernet policing.
 * Header compensation changes the effective size of the packet
 * for metering purposes to compensate for changes in header size (such
 * as the CRC removal) when
 * considering the packet for metering.
 *
 * Header compensation is typically -2 = (-4 [for
 * ethernet CRC removal] +2 [for DRAM CRC addition]).
 *
 * IPG compensation is assumed to be 20 bytes.
 *
 * @unit
 * @port The port to set a policer on.
 * @ipg_compensation_enabled If set then IPG computation in header
 * compensation is enabled.
 *
 * @note This is only supported in Arad+.
 *
 * @return int The status.
 */
int header_compensation_example(int unit, int port, int ipg_compensation_enabled)
{
    int rv;

    /* Set IPG if necessary (this is global) */
	if (ipg_compensation_enabled) {
		rv = bcm_switch_control_set(unit, bcmSwitchMeterAdjustInterframeGap, 20);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_switch_control_set\n");
			print rv;
			return rv;
		}
	}

	/* Set header compensation to -2 on the port. */
	rv = bcm_switch_control_port_set(unit, port, bcmSwitchMeterAdjust, -2);
	if (rv != BCM_E_NONE) {
		printf("Error in bcm_port_control_set while setting header compensation to -2\n");
		print rv;
		return rv;
	}

    return 0;
}

/* two_meters_groups: if policer_ingress_count=64 then there is only one group of 64K meters per core
 *                    if policer_ingress_count=32 then there are 2 groups of meters, each with 32K meters core
 *
 * run:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/cint_policer_metering_example
 * cint
 * metering_example(unit, <is_arad>, <two_meters_groups>);
 */
int
metering_example(int unit, int is_arad, int is_jer, int two_meters_groups) {

    int rv, i;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    if (is_arad) {

        /* set global Fairness (randomize the bucket level): each meter bucket level will add/subtract a randomized value of 8K
           this will lower the posibility that in case several streams are associated with the same policer,
           one stream may starve as another stream consumes all the credits */
        rv = bcm_switch_control_set(unit, bcmSwitchPolicerFairness, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            print rv;
            return rv;
        }
    }

    /* create policer 1 */
    bcm_policer_config_t_init(&(metering_info.pol_cfg));
    metering_info.pol_cfg.mode = bcmPolicerModeTrTcmDs; /* two rates three colors mode */
    /* two rates: ckbits (sec+burst) and pkbits (sec+burst) are both used
       if (is_arad) then max_pkbits_sec are also used
       three colors: green, yellow and red */
	if (is_jer) {
		metering_info.pol_cfg.flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
	}
    metering_info.pol_cfg.pkbits_sec = 100000;
    metering_info.pol_cfg.pkbits_burst = 1000;
    metering_info.pol_cfg.ckbits_sec = 100000;
    metering_info.pol_cfg.ckbits_burst = 1000;
    if (is_arad) {
        metering_info.pol_cfg.max_pkbits_sec = 100000; /* maximal excess rate, including tokens transferred from other buckets */
    }

    rv = bcm_policer_create(unit, &(metering_info.pol_cfg), &(metering_info.policer_id));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 1\n");
        print rv;
        return rv;
    }
    printf("policer_id1: %d\n", metering_info.policer_id);

    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 1\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 1. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }
    print metering_info.pol_cfg_get;

    /* create policer 2 with same configuration */
    bcm_policer_config_t_init(&(metering_info.pol2_cfg));
    metering_info.pol2_cfg.mode = bcmPolicerModeTrTcmDs; /* two rates three colors mode */
    metering_info.pol2_cfg.pkbits_sec = 100000;
    metering_info.pol2_cfg.pkbits_burst = 1000;
    metering_info.pol2_cfg.ckbits_sec = 100000;
    metering_info.pol2_cfg.ckbits_burst = 1000;
    if (is_arad) {
        metering_info.pol2_cfg.max_pkbits_sec = 100000;
    }

    rv = bcm_policer_create(unit, &(metering_info.pol2_cfg), &(metering_info.policer_id2));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 2\n");
        print rv;
        return rv;
    }
    printf("policer_id2: %d\n", metering_info.policer_id2);

    rv = bcm_policer_get(unit, metering_info.policer_id2, &(metering_info.pol2_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 2\n");
        print rv;
        return rv;
    }
    print metering_info.pol_cfg_get;
    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol2_cfg, metering_info.pol2_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 2. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    /* create policer 3 with different configuration */
    bcm_policer_config_t_init(&(metering_info.pol3_cfg));
    metering_info.pol3_cfg.mode = bcmPolicerModeCoupledTrTcmDs; /* two rates three colors with coupling */
    metering_info.pol3_cfg.pkbits_sec = 10000;
    metering_info.pol3_cfg.pkbits_burst = 500;
    metering_info.pol3_cfg.ckbits_sec = 500000;
    metering_info.pol3_cfg.ckbits_burst = 500;
    if (is_arad) {
        metering_info.pol3_cfg.max_pkbits_sec = 10000;
    }

    rv = bcm_policer_create(unit, &(metering_info.pol3_cfg), &(metering_info.policer_id3));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 3\n");
        print rv;
        return rv;
    }
    printf("policer_id3: %d\n", metering_info.policer_id3);

    rv = bcm_policer_get(unit, metering_info.policer_id3, &(metering_info.pol3_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 3\n");
        print rv;
        return rv;
    }
    print metering_info.pol3_cfg_get;
    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol3_cfg, metering_info.pol3_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 3. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    /* update meter config, using policer_set API:
       update policer 1 with policer 3 information */
    rv = bcm_policer_set(unit, metering_info.policer_id, &(metering_info.pol3_cfg));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set\n");
        print rv;
        return rv;
    }
    printf("policer_id1 after set: %d\n", metering_info.policer_id);

    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 1\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol3_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 3. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    if (is_arad) {
        /* now using modes that are only supported in ARAD */

        /* create policer 4 with different configuration */
        bcm_policer_config_t_init(&(metering_info.pol4_cfg));
        if (two_meters_groups) {
            metering_info.pol4_cfg.flags = BCM_POLICER_MACRO; /* if MACRO flag is set, meter will be added to group 1, otherwise to group 0 */
            /* group has meaning only in 32K mode, when meters are divided into 2 groups, with 32K meters each */
        }
        metering_info.pol4_cfg.mode = bcmPolicerModeCoupledCascade; /* cascade mode with coupling */
        /* cascade mode: 4 meters are allocated and work in hierarchical mode, with sharing.
           the returned policer_id is the lowest id of the 4.
           coupling: bandwidth tokens are transferred from the committed rate bucket to the excess rate bucket */
        metering_info.pol4_cfg.ckbits_sec = (info.device == 0x8270 || info.device == 0x8272 || \
            info.device == 0x8273 || info.device == 0x8278) ? 200000000 : 300000000; /* max rate 200GB on QUX, 300GB on others */
        metering_info.pol4_cfg.pkbits_sec = (info.device == 0x8270 || info.device == 0x8272 || \
            info.device == 0x8273 || info.device == 0x8278) ? 160000000 : 260000000;
        metering_info.pol4_cfg.max_ckbits_sec = (info.device == 0x8270 || info.device == 0x8272 || \
            info.device == 0x8273 || info.device == 0x8278) ? 200000000 : 300000000; /* when sharing is enabled, max_ckbits_sec is also used */
        /* maximal committed rate including tokens transferred from a higher class buckets (in case of sharing) */
        metering_info.pol4_cfg.max_pkbits_sec = (info.device == 0x8270 || info.device == 0x8272 || \
            info.device == 0x8273 || info.device == 0x8278) ? 170000000 : 270000000; /* in ARAD, max_pkbits_sec is always used */
        metering_info.pol4_cfg.pkbits_burst = 33292;
        metering_info.pol4_cfg.ckbits_burst = 33000;

        rv = bcm_policer_create(unit, &metering_info.pol4_cfg, &metering_info.policer_id4);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create no. 4\n");
            print rv;
            return rv;
        }
        printf("policer_id4: %d\n", metering_info.policer_id4);

        rv = bcm_policer_get(unit, metering_info.policer_id4, &metering_info.pol4_cfg_get);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 4\n");
            print rv;
            return rv;
        }
        print metering_info.pol4_cfg_get;


        if (metering_info.with_check){
            for (i = 0 ; i < 4 ; i++) {
                rv = bcm_policer_get(unit, metering_info.policer_id4 + i, &metering_info.pol4_cfg_get);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_policer_get no. 4\n");
                    print rv;
                    return rv;
                }
                rv = compare_policers(metering_info.pol4_cfg, metering_info.pol4_cfg_get, five_percent);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_policer_get no. 4. Did not get the same policer.\n");
                    print i;
                    print rv;
                    return rv;
                }
            }
        }

        /* create policer 5 with different config */
        bcm_policer_config_t_init(&(metering_info.pol5_cfg));
        if (two_meters_groups) {
            metering_info.pol5_cfg.flags = BCM_POLICER_MACRO; /* if MACRO flag is set, meter will be added to group 1, otherwise to group 0 */
        }
        metering_info.pol5_cfg.mode = bcmPolicerModeCascade; /* cascade mode */
        /* cascade mode: 4 meters are allocated and work in hierarchical mode, with sharing.
           the returned policer_id is the lowest id of the 4 */
        metering_info.pol5_cfg.ckbits_sec = 50000000;
        metering_info.pol5_cfg.pkbits_sec = 30000000;
        metering_info.pol5_cfg.max_ckbits_sec = 60000000; /* when sharing is enabled, max_ckbits_sec is also used */
        metering_info.pol5_cfg.max_pkbits_sec = 40000000; /* in ARAD, max_pkbits_sec is always used */
        metering_info.pol5_cfg.pkbits_burst = 6666;
        metering_info.pol5_cfg.ckbits_burst = 5555;

        rv = bcm_policer_create(unit, &metering_info.pol5_cfg, &metering_info.policer_id5);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create no. 5\n");
            print rv;
            return rv;
        }
        printf("policer_id5: %d\n", metering_info.policer_id5);

        rv = bcm_policer_get(unit, metering_info.policer_id5, &metering_info.pol5_cfg_get);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get no. 5\n");
            print rv;
            return rv;
        }
        print metering_info.pol5_cfg_get;

        /* change the rate of one of the 4 meters group */
        int policer_id5_1 = metering_info.policer_id5 + 2;

        bcm_policer_config_t_init(&(metering_info.pol5_1_cfg));
        if (two_meters_groups) {
            metering_info.pol5_1_cfg.flags = BCM_POLICER_MACRO; /* if MACRO flag is set, meter will be added to group 1, otherwise to group 0 */
        }
        metering_info.pol5_1_cfg.mode = bcmPolicerModeCascade; /* cascade mode */
        metering_info.pol5_1_cfg.ckbits_sec = 15000;
        metering_info.pol5_1_cfg.pkbits_sec = 13000;
        metering_info.pol5_1_cfg.max_ckbits_sec = 16000;
        metering_info.pol5_1_cfg.max_pkbits_sec = 14000;
        metering_info.pol5_1_cfg.pkbits_burst = 88;
        metering_info.pol5_1_cfg.ckbits_burst = 77;

        /* bcm_policer_set() is called for replace, so no need for flags BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID */
        rv = bcm_policer_set(unit, policer_id5_1, &metering_info.pol5_1_cfg);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_set no. 5-1\n");
            print rv;
            return rv;
        }
        printf("policer_id5-1: %d\n",  policer_id5_1);


        if (metering_info.with_check){
            for (i = 0 ; i < 4 ; i++) {
                rv = bcm_policer_get(unit, metering_info.policer_id5 + i, &metering_info.pol5_cfg_get);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_policer_get no. 5\n");
                    print rv;
                    return rv;
                }
                /* the range of rates is very big. since the rev_exp must be the same, the accuracy will be damage  - up to 20 %*/
                if (i == 2) {
                    /* only no. 2 of the 4 should have different rates */
                    rv = compare_policers(metering_info.pol5_1_cfg, metering_info.pol5_cfg_get, twenty_percent);
                }
                else {
                    rv = compare_policers(metering_info.pol5_cfg, metering_info.pol5_cfg_get, twenty_percent);
                }
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_policer_get no. 5. Did not get the same policer.\n");
                    print rv;
                    if (i == 2) {
                        print metering_info.pol5_1_cfg;
                    }
                    else {
                        print metering_info.pol5_cfg;
                    }
                    print metering_info.pol5_cfg_get;
                    return rv;
                }
            }
        }

        /* Profile management example. */
        /* 1) Create meter 1 and point it to profile 512, configure with cfg A. */
        /* 2) Next create meter 2 also to profile 512, get the cfg and make sure it is A. */

        /* 3) Change the configuration of the profile to cfg B using meter 2 and the change cfg flag. */
        /*    After this the profile configuration should be changed. */
        /* 4) Get the cfg of meter 1, and check it equals cfg B (since both meters use the same profile). */
        bcm_policer_config_t_init(&(metering_info.same_prf_1_pol_cfg));
        metering_info.same_prf_1_pol_cfg.mode = bcmPolicerModeTrTcmDs; /* two rates three colors mode */
        /* two rates: ckbits (sec+burst) and pkbits (sec+burst) are both used
           if (is_arad) then max_pkbits_sec are also used
           three colors: green, yellow and red */
        metering_info.same_prf_1_pol_cfg.pkbits_sec = 123000;
        metering_info.same_prf_1_pol_cfg.pkbits_burst = 1000;
        metering_info.same_prf_1_pol_cfg.ckbits_sec = 123000;
        metering_info.same_prf_1_pol_cfg.ckbits_burst = 1000;
        metering_info.same_prf_1_pol_cfg.max_pkbits_sec = 123000; /* maximal excess rate, including tokens transferred from other buckets */
        /* Use profile id 512*/
        metering_info.same_prf_1_pol_cfg.entropy_id = 512;
        /* Make sure the configuration is changed (even if the profile exists) */
        metering_info.same_prf_1_pol_cfg.flags = BCM_POLICER_REPLACE_SHARED;

        rv = bcm_policer_create(unit, &(metering_info.same_prf_1_pol_cfg), &(metering_info.same_prf_1_policer_id));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create for same_prf_1_pol_cfg\n");
            print rv;
            return rv;
        }
        printf("same_prf_1_policer_id: %d\n", metering_info.same_prf_1_policer_id);

        metering_info.same_prf_1_pol_cfg_get.flags = BCM_POLICER_REPLACE_SHARED;
        rv = bcm_policer_get(unit, metering_info.same_prf_1_policer_id, &(metering_info.same_prf_1_pol_cfg_get));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get for same_prf_1_policer_id\n");
            print rv;
            return rv;
        }

        if (metering_info.with_check) {
            rv = compare_policers(metering_info.same_prf_1_pol_cfg, metering_info.same_prf_1_pol_cfg_get, five_percent);
            if (rv != BCM_E_NONE) {
                printf("Error in same_prf_1_pol_cfg_get - did not get the same policer.\n");
                print rv;
                print metering_info.same_prf_1_pol_cfg;
                print metering_info.same_prf_1_pol_cfg_get;
                return rv;
            }
        }

        /* Allocate another meter with the same profile. */
        metering_info.same_prf_2_pol_cfg = metering_info.same_prf_1_pol_cfg;
        if (is_arad) {
            /* No need for BCM_POLICER_SHARE_REPLACE - we don't want to replace the config. */
            metering_info.same_prf_2_pol_cfg.flags = 0;
        }
        rv = bcm_policer_create(unit, &(metering_info.same_prf_2_pol_cfg), &(metering_info.same_prf_2_policer_id));
        if (rv != BCM_E_NONE) {
            printf("Error in same_prf_2_pol_cfg create\n");
            print rv;
            return rv;
        }
        printf("same_prf_2_policer_id: %d\n", metering_info.same_prf_2_policer_id);

        /* Get the entropy_id as well as the configuration. */
        metering_info.same_prf_2_pol_cfg_get.flags |= BCM_POLICER_REPLACE_SHARED;
        rv = bcm_policer_get(unit, metering_info.same_prf_2_policer_id, &(metering_info.same_prf_2_pol_cfg_get));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get for same_prf_2_policer_id \n");
            print rv;
            return rv;
        }

        if (metering_info.with_check) {
            rv = compare_policers(metering_info.same_prf_2_pol_cfg, metering_info.same_prf_2_pol_cfg_get, five_percent);
            if (rv != BCM_E_NONE) {
                printf("Error in same_prf_2_pol_cfg_get - did not get the same policer.\n");
                print rv;
                print metering_info.same_prf_2_pol_cfg;
                print metering_info.same_prf_2_pol_cfg_get;
                return rv;
            }
        }

        /* Change the profile config */
        metering_info.same_prf_2_pol_cfg.pkbits_sec = 123123123;
        metering_info.same_prf_2_pol_cfg.max_pkbits_sec = 123123123;
        metering_info.same_prf_2_pol_cfg.flags |= BCM_POLICER_REPLACE_SHARED;

        rv = bcm_policer_set(unit, metering_info.same_prf_2_policer_id, &(metering_info.same_prf_2_pol_cfg));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_set for same_prf_2_pol_cfg\n");
            print rv;
            return rv;
        }

        /* Get the entropy_id as well as the config. */
        metering_info.same_prf_1_pol_cfg_get.flags |= BCM_POLICER_REPLACE_SHARED;
        rv = bcm_policer_get(unit, metering_info.same_prf_1_policer_id, &(metering_info.same_prf_1_pol_cfg_get));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get for same_prf_1_policer_id \n");
            print rv;
            return rv;
        }

        if (metering_info.with_check) {
            /* It is now expected that the config of policer 1 be equal to the config that was set to policer 2, */
            /* since both use the same profile and since the profile config has been changed. */
            rv = compare_policers(metering_info.same_prf_2_pol_cfg, metering_info.same_prf_1_pol_cfg_get, five_percent);
            if (rv != BCM_E_NONE) {
                printf("Error in same_prf_1_pol_cfg_get - did not get the same policer as same_prf_2_pol_cfg.\n");
                printf("Expected to see the same configuration since both point to the same profile.\n");
                print rv;
                print metering_info.same_prf_2_pol_cfg;
                print metering_info.same_prf_1_pol_cfg_get;
                return rv;
            }
        }
    }

    /* re-create policer 3 with different configuration */
    bcm_policer_config_t_init(&(metering_info.pol3_cfg));
    metering_info.pol3_cfg.mode = bcmPolicerModeCoupledTrTcmDs; /* two rates three colors with coupling */
    metering_info.pol3_cfg.pkbits_sec = 11111;
    metering_info.pol3_cfg.pkbits_burst = 0;
    metering_info.pol3_cfg.ckbits_sec = 555555;
    metering_info.pol3_cfg.ckbits_burst = 64;
    if (is_arad) {
        metering_info.pol3_cfg.max_pkbits_sec = 12222;
    }

    rv = bcm_policer_create(unit, &(metering_info.pol3_cfg), &(metering_info.policer_id3));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 3\n");
        print rv;
        return rv;
    }
    printf("policer_id3: %d\n", metering_info.policer_id3);

    rv = bcm_policer_get(unit, metering_info.policer_id3, &(metering_info.pol3_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 3\n");
        print rv;
        return rv;
    }
    print metering_info.pol3_cfg_get;

    rv = compare_policers(metering_info.pol3_cfg, metering_info.pol3_cfg_get, five_percent);
    if (rv != BCM_E_NONE) {
         printf("Error in bcm_policer_get no. 3. Did not get the same policer.\n");
         print rv;
         return rv;
    }

	/* create policer 6 with different configuration */
    bcm_policer_config_t_init(&(metering_info.pol6_cfg));
    metering_info.pol6_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors */
	/* in this mode coupling is always enabled.
	   the excess bucket has no credits of its own, it only receives excess credits from the committed bucket */
    metering_info.pol6_cfg.pkbits_sec = 0; /* must be 0 because excess bucket has no credits */
    metering_info.pol6_cfg.pkbits_burst = 1000;
    metering_info.pol6_cfg.ckbits_sec = 30000; /* 30MB */
    metering_info.pol6_cfg.ckbits_burst = 1000;
    if (is_arad) {
		/* although pkbits_sec = 0, max_pkbits_sec must be configured so that excess bucket could receive
		   excess credits from the comitted bucket up to that limit */
        metering_info.pol6_cfg.max_pkbits_sec = 20000;
    }

    rv = bcm_policer_create(unit, &(metering_info.pol6_cfg), &(metering_info.policer_id6));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 6\n");
        print rv;
        return rv;
    }
    printf("policer_id6: %d\n", metering_info.policer_id6);

    rv = bcm_policer_get(unit, metering_info.policer_id6, &(metering_info.pol6_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 6\n");
        print rv;
        return rv;
    }
    print metering_info.pol6_cfg_get;

    rv = compare_policers(metering_info.pol6_cfg, metering_info.pol6_cfg_get, five_percent);
    if (rv != BCM_E_NONE) {
         printf("Error in bcm_policer_get no. 6. Did not get the same policer.\n");
         print rv;
         return rv;
    }

    return rv;
}

/* if policer_ingress_count=64 then there is only one group of 64K meters per core
 * if policer_ingress_count=32 then there are 2 groups of meters, each with 32K meters per core
 *
 * run:
 * cint ../../../../src/examples/dpp/cint_policer_metering_example.c
 * cint
 * metering_replace_example(unit, <is_arad>);
 */
int
metering_replace_example(int unit, int is_arad) {

    int rv, i;
    int policer_id = 0;
    int policer_nof = 0;
    bcm_policer_config_t eth_pol_cfg;
    bcm_policer_config_t eth_pol_cfg_get;

    /* Replaced fields for bcm_policer_create and bcm_policer_set when policer isn't ethernet policer.
        *    mode
        *    ckbits_sec
        *    ckbits_burst
        *    pkbits_sec
        *    pkbits_burst
        *    max_pkbits_sec in arad chip
        *    flags with/without BCM_POLICER_COLOR_BLIND
        *    flags with/without  BCM_POLICER_MODE_PACKETS
        *    entropy_id when replaced flag is BCM_POLICER_REPLACE
        *
        * we can use bcm_policer_set or bcm_policer_create with BCM_POLICER_WITH_ID | BCM_POLICER_REPLACE
        * to replace some fields. in this example, use bcm_policer_set.
        * Please note we don't want to support replaced function about flags with/without  BCM_POLICER_MACRO because
        * BCM_POLICER_MACRO is used to allocate meter id in group 0 or group 1. after that the policer ID will be change.
 */

	/* create policer 1 */
    bcm_policer_config_t_init(&(metering_info.pol_cfg));
    metering_info.pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors */
	/* in this mode coupling is always enabled.
	 * the excess bucket has no credits of its own, it only receives excess credits from the committed bucket
	 */
    metering_info.pol_cfg.pkbits_sec = 0; /* must be 0 because excess bucket has no credits */
    metering_info.pol_cfg.pkbits_burst = 1000;
    metering_info.pol_cfg.ckbits_sec = 30000; /* 30MB */
    metering_info.pol_cfg.ckbits_burst = 1000;
    if (is_arad) {
		/* Although pkbits_sec = 0, max_pkbits_sec must be configured so that excess bucket could receive
		  * excess credits from the comitted bucket up to that limit
		  */
        metering_info.pol_cfg.max_pkbits_sec = 20000;
    }

    rv = bcm_policer_create(unit, &(metering_info.pol_cfg), &(metering_info.policer_id));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create no. 1\n");
        print rv;
        return rv;
    }

    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 1\n");
        print rv;
        return rv;
    }

    rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
    if (rv != BCM_E_NONE) {
         printf("Error in bcm_policer_get mode is bcmPolicerModeSrTcm. Did not get the same policer.\n");
         print rv;
         return rv;
    }

    /* Change followed fields
        *  field                                  old value                          new value
        *  mode                      single rates three colors mode          two rates three colors mode
        *  ckbits_sec                         30Mb                                    100Mb
        *  ckbits_burst                       10KB                                    20KB
        *  pkbits_sec                         0Mb                                     100Mb
        *  pkbits_burst                       10KB                                    20KB
        *  max_pkbits_sec                  20Mb                                    100Mb
        *  flag                      without BCM_POLICER_COLOR_BLIND          with BCM_POLICER_COLOR_BLIND
        *                             and BCM_POLICER_MODE_PACKETS            and BCM_POLICER_MODE_PACKETS
        *  entropy_id
        */
    /* two rate three colors */
    metering_info.pol_cfg.mode = bcmPolicerModeTrTcmDs;
    /* two rates: ckbits (sec+burst) and pkbits (sec+burst) are both used
        * if (is_arad) then max_pkbits_sec are also used
        * three colors: green, yellow and red
        */
    metering_info.pol_cfg.pkbits_sec = 100000;
    metering_info.pol_cfg.pkbits_burst = 2000;
    metering_info.pol_cfg.ckbits_sec = 100000;
    metering_info.pol_cfg.ckbits_burst = 2000;
    if (is_arad) {
        metering_info.pol_cfg.max_pkbits_sec = 100000; /* maximal excess rate, including tokens transferred from other buckets */
    }
    metering_info.pol_cfg.flags |= (BCM_POLICER_COLOR_BLIND);

    /* suppose the device is arad plus */
    if (!is_arad) {
        metering_info.pol_cfg.flags |= (BCM_POLICER_MODE_PACKETS);

    }

    rv = bcm_policer_set(unit, metering_info.policer_id, &(metering_info.pol_cfg));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set, mode is bcmPolicerModeTrTcmDs\n");
        print rv;
        return rv;
    }

    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get mode is bcmPolicerModeTrTcmDs\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get, mode is bcmPolicerModeTrTcmDs. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    /* Other replaced modes:
        *   bcmPolicerModeCommitted
        *   bcmPolicerModeCoupledTrTcmDs
        *   bcmPolicerModeSrTcm
 */
    /* new mode is bcmPolicerModeCommitted */
    bcm_policer_config_t_init(&(metering_info.pol_cfg));
    metering_info.pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    metering_info.pol_cfg.mode = bcmPolicerModeCommitted;
    metering_info.pol_cfg.ckbits_sec = 15000; /* 15M */
    metering_info.pol_cfg.ckbits_burst = 3000;
    metering_info.pol_cfg.pkbits_burst = 0;
    rv = bcm_policer_set(unit, metering_info.policer_id, &(metering_info.pol_cfg));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set, mode is bcmPolicerModeCommitted\n");
        print rv;
        return rv;
    }
    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get, mode is bcmPolicerModeCommitted\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get, mode is bcmPolicerModeCommitted. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    /* new mode is bcmPolicerModeCoupledTrTcmDs */
    bcm_policer_config_t_init(&(metering_info.pol_cfg));
    metering_info.pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    metering_info.pol_cfg.mode = bcmPolicerModeCoupledTrTcmDs;
    metering_info.pol_cfg.pkbits_sec = 10000;
    metering_info.pol_cfg.pkbits_burst = 500;
    metering_info.pol_cfg.ckbits_sec = 500000;
    metering_info.pol_cfg.ckbits_burst = 500;
    if (is_arad) {
        metering_info.pol_cfg.max_pkbits_sec = 10000;
    }
    rv = bcm_policer_set(unit, metering_info.policer_id, &(metering_info.pol_cfg));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set, mode is bcmPolicerModeCoupledTrTcmDs\n");
        print rv;
        return rv;
    }
    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get, mode is bcmPolicerModeCoupledTrTcmDs\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get, mode is bcmPolicerModeCoupledTrTcmDs. Did not get the same policer.\n");
            print rv;
            return rv;
        }
    }

    /* new mode is bcmPolicerModeSrTcm */
    bcm_policer_config_t_init(&(metering_info.pol_cfg));
    metering_info.pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    metering_info.pol_cfg.mode = bcmPolicerModeSrTcm;
    metering_info.pol_cfg.pkbits_sec = 0; /* must be 0 because excess bucket has no credits */
    metering_info.pol_cfg.pkbits_burst = 1000;
    metering_info.pol_cfg.ckbits_sec = 30000; /* 30MB */
    metering_info.pol_cfg.ckbits_burst = 1000;
    if (is_arad) {
		/* although pkbits_sec = 0, max_pkbits_sec must be configured so that excess bucket could receive
		   excess credits from the comitted bucket up to that limit */
        metering_info.pol_cfg.max_pkbits_sec = 20000;
    }
    rv = bcm_policer_set(unit, metering_info.policer_id, &(metering_info.pol_cfg));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set, mode is bcmPolicerModeSrTcm\n");
        print rv;
        return rv;
    }
    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get, mode is bcmPolicerModeSrTcm\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg, metering_info.pol_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_get, mode is bcmPolicerModeSrTcm. Did not get the same policer\n");
            print rv;
            return rv;
        }
    }

    /* Use BCM_POLICER_REPLACE_SHARED, so meter profile(entropy_id) will be not changed;
        * But meter configuration will be changed.
        * 1. create 1st policer with  meter profile 512
        * 2. create 2nd policer with  meter profile 512, it will also update the meter configuration with 1 policer id
        * 3. verify another meter configuration whether changed.
        */
    metering_info.pol_cfg.mode = bcmPolicerModeTrTcmDs;
    metering_info.pol_cfg.flags = (BCM_POLICER_REPLACE_SHARED | BCM_POLICER_COLOR_BLIND);
    metering_info.pol_cfg.entropy_id = 512;
    metering_info.pol_cfg.pkbits_sec = 50000;
    metering_info.pol_cfg.ckbits_sec = 50000;
    metering_info.pol_cfg.pkbits_burst = 2000;
    metering_info.pol_cfg.ckbits_burst = 2000;
    if (is_arad) {
        metering_info.pol_cfg.max_pkbits_sec = 100000; /* maximal excess rate, including tokens transferred from other buckets */
    }
    rv = bcm_policer_create(unit, &(metering_info.pol_cfg), &(metering_info.policer_id));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create\n");
        print rv;
        return rv;
    }

    metering_info.pol_cfg.pkbits_sec = 100000;
    metering_info.pol_cfg.ckbits_sec = 100000;
    rv = bcm_policer_create(unit, &(metering_info.pol_cfg), &(metering_info.policer_id2));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create\n");
        print rv;
        return rv;
    }

    /*get 1st policer meter configuration*/
    /* set flags as BCM_POLICER_REPLACE_SHARED so can get meter profile value */
    metering_info.pol_cfg_get.flags = BCM_POLICER_REPLACE_SHARED;
    rv = bcm_policer_get(unit, metering_info.policer_id, &(metering_info.pol_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 1\n");
        print rv;
        return rv;
    }

    /*get 2nd policer meter configuration*/
    metering_info.pol2_cfg_get.flags = BCM_POLICER_REPLACE_SHARED;
    rv = bcm_policer_get(unit, metering_info.policer_id2, &(metering_info.pol2_cfg_get));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get no. 1\n");
        print rv;
        return rv;
    }

    if (metering_info.with_check){
        rv = compare_policers(metering_info.pol_cfg_get, metering_info.pol2_cfg_get, five_percent);
        if (rv != BCM_E_NONE) {
            printf("Error, meter configuration about 2 policer with  BCM_POLICER_REPLACE_SHARED isn't same\n");
            print rv;
            print metering_info.pol_cfg_get;
            print metering_info.pol2_cfg_get;
            return rv;
        }
    }

	if (!is_device_or_above(unit, JERICHO)){

		/* Replaced fields  for bcm_policer_set when policer is eth policer
			*  ckbits_sec,
			*  ckbits_burst,
			*  flags with/without BCM_POLICER_MODE_PACKETS in arad plus
			*/
		rv = bcm_policer_group_create(unit, bcmPolicerGroupModeTrafficType, &policer_id, &policer_nof);
		if (rv != BCM_E_NONE) {
			printf("Error in bcm_policer_group_create no\n");
			print rv;
			return rv;
		}
		printf("ethernet policer id:%d\n", policer_id);
		/* Update the meter entry  configuration */
		for (i = 0; i < policer_nof; i++) {
			bcm_policer_config_t_init(&eth_pol_cfg);
			eth_pol_cfg.ckbits_sec = 100000 + (i * 50000);
			eth_pol_cfg.ckbits_burst = 1000;
			if (!is_arad) {
				eth_pol_cfg.is_packet_mode = (i%2);
			}
			rv = bcm_policer_set(unit, (policer_id+i), &eth_pol_cfg);
			if (rv != BCM_E_NONE) {
				printf("Error in bcm_policer_set ethernet policer :%d\n", i);
				print rv;
				return rv;
			}
			bcm_policer_config_t_init(&eth_pol_cfg_get);
			rv = bcm_policer_get(unit, (policer_id+i), &eth_pol_cfg_get);
			if (rv != BCM_E_NONE) {
				printf("Error in bcm_policer_get ethernet policer :%d\n", i);
				print rv;
				return rv;
			}
			if (metering_info.with_check){
				rv = compare_policers(eth_pol_cfg, eth_pol_cfg_get, five_percent);
				if (rv != BCM_E_NONE) {
					printf("Error, ehternet policer bcm_policer_get. Did not get the same policer\n");
					print rv;
					return rv;
				}
			}
		}

		/* Please call bcm_port_policer_set to associate In-Ports to an aggregate policer */
	}

    return rv;
}


/* single_rate: used only if is_arad=1
 * if single_rate=1 - meter mode is bcmPolicerModeSrTcm (single rate 3 colors), the excess bucket has no credits of its own and
 *                    it only receives excess credits from the committed bucket
 * if single_rate=1 - meter mode is bcmPolicerModeTrTcmDs (2 rates 3 colors), the committed bucaket and the excess bucket
 *                    each has credits of its own
 *
 * run:
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/cint_policer_metering_example
 * cint
 * traffic_example(unit, <in_port>, <out_port>, <is_arad>, <single_rate>, <color_blind>);
 *
 * traffic to run:
 * for Arad (is_arad=1):
 * run ethernet packet with DA 1 and vlan tag id 1 from in_port, with:
 * 1) priority = 1:
 *     the stream will go through the committed bucket and afterwards also through the excess bucket
 *     if single_rate=0: the stream will arrive at out_port with 50M rate (30M CIR + 20M EIR).
 *     if single_rate=1: the stream will arrive at out_port with 30M rate (EIR has no credits).
 *
 * 2) priority = 4:
 *     if color_blind=0: the stream will go straight to the excess bucket and arrive at out_port with 20M rate
 *                       in both cases (single_rate=1/0).
 *     if color_blind=1: the packet will go to the CIR and will behave the same as priority=1.
 */
int
traffic_example(int unit, int in_port, int out_port, int is_arad, int mode, int color_blind) {

    int rv;
    int trap_id;
    bcm_rx_trap_config_t trap_cfg;

    int group_priority = 6;
    bcm_field_group_t grp = 1;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int inPortValue = in_port; /* incoming port */
    int inPortMasdk = 0xffffffff;
    bcm_field_stat_t stats[2];
    int statId;
    int DstPortModValue = 0;
    int DstPortModMask = 0xffffffff;
    int DstPortValue = out_port; /* outgoing port */
    int DstPortMask  = 0xffffffff;
    int core_id; /*core matching the in_port*/
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_policer_t entropy_id;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};

    if (!is_arad) {

        /* allocate a trap_id for unknown_da */
        bcm_rx_trap_type_create(unit, 0, bcmRxTrapVlanUnknownDa, &trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_rx_trap_type_create\n");
            print rv;
            return rv;
        }
        printf("trap id: %d\n", trap_id);

        /* configure the trap properties */
        bcm_rx_trap_config_t_init(&trap_cfg);
        trap_cfg.trap_strength = 5;
        trap_cfg.flags |= BCM_RX_TRAP_REPLACE;

        /* send to port 2 as destination */
        trap_cfg.flags |= BCM_RX_TRAP_UPDATE_DEST;
        trap_cfg.dest_port = out_port;

        /* map packet to meter_id 1 */

        trap_cfg.flags |= BCM_RX_TRAP_UPDATE_POLICER;
        trap_cfg.policer_id = metering_info.policer_id;


        rv = bcm_rx_trap_set(unit, trap_id, &trap_cfg);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_rx_trap_set\n");
            print rv;
            return rv;
        }
    }
    else { /* Arad */
        bcm_policer_config_t_init(&pol_cfg);
        pol_cfg.flags |= BCM_POLICER_WITH_ID;

        if (color_blind) {
            pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
        }

        pol_cfg.ckbits_sec = 30000; /* 30Mbps */
        pol_cfg.ckbits_burst = 1000;
        pol_cfg.pkbits_burst = 1000;
        pol_cfg.max_pkbits_sec = 20000; /* 20Mbps max */

        switch (mode) {
            case bcmPolicerModeSrTcm:
                pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */
                /* in this mode coupling is always enabled and the excess bucket has no credits of its own,
                                it only receives excess credits from the committed bucket, up to max_pkbits_sec */
                break;
            case bcmPolicerModeTrTcmDs:
                pol_cfg.mode = bcmPolicerModeTrTcmDs; /* two rates three colors mode */
                pol_cfg.pkbits_sec = 20000;   /* 20Mbps */
                break;
            case bcmPolicerModeCoupledTrTcmDs:
                pol_cfg.mode = bcmPolicerModeCoupledTrTcmDs; /* two rates three colors in couple mode */
                pol_cfg.pkbits_sec = 20000;   /* 20Mbps */
                pol_cfg.max_pkbits_sec = 30000;     /* 30Mbps max */
                break;
            case bcmPolicerModeCommitted:
                pol_cfg.mode = bcmPolicerModeCommitted;
                pol_cfg.pkbits_burst = 0;
                pol_cfg.max_pkbits_sec = 0;
                break;
            case bcmPolicerModeCascade:
                pol_cfg.mode = bcmPolicerModeCascade;
                pol_cfg.ckbits_sec = 50000;
                pol_cfg.pkbits_sec = 30000;
                pol_cfg.max_ckbits_sec = 60000; /* when sharing is enabled, max_ckbits_sec is also used */
                pol_cfg.max_pkbits_sec = 40000; /* in ARAD, max_pkbits_sec is always used */
                pol_cfg.pkbits_burst = 6666;
                pol_cfg.ckbits_burst = 5555;
                break;
            case bcmPolicerModeCoupledCascade:
                pol_cfg.mode = bcmPolicerModeCoupledCascade;
                pol_cfg.ckbits_sec = 50000;
                pol_cfg.pkbits_sec = 30000;
                pol_cfg.max_ckbits_sec = 60000; /* when sharing is enabled, max_ckbits_sec is also used */
                pol_cfg.max_pkbits_sec = 40000; /* in ARAD, max_pkbits_sec is always used */
                pol_cfg.pkbits_burst = 6666;
                pol_cfg.ckbits_burst = 5555;
                break;
            default:
                return BCM_E_PARAM;
                break;
        }

        /* meter should be allocated in the core in which the ports are allocated*/
        rv = get_core_from_port(unit, inPortValue, &core_id);
        if (rv != BCM_E_NONE) {
            print rv;
            return rv;
        }
        pol_cfg.core_id = core_id;

        if (mode == bcmPolicerModeCascade || mode == bcmPolicerModeCoupledCascade) {
            policer_id = 32;
        } else {
            policer_id = 30;
        }

        rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
            print rv;
            return rv;
        }

        printf("The created policer_id is %d\n", policer_id);

        entropy_id = get_policer_entropy_id(unit, policer_id);
        printf("The created meter profile id is %d\n", entropy_id);

        /* map packet to meter_id, according to in-port and dst-port */
        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        rv = bcm_field_group_create_mode_id(unit, qset, group_priority, bcmFieldGroupModeAuto, grp);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_create_mode_id\n");
            print rv;
            return rv;
        }

        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat0);
        rv = bcm_field_group_action_set(unit, grp, aset);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_action_set\n");
            print rv;
            return rv;
        }

        rv = bcm_field_entry_create(unit, grp, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create\n");
            print rv;
            return rv;
        }
        rv = bcm_field_qualify_InPort(unit, ent, inPortValue, inPortMasdk);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort with in_port %d\n", inPortValue);
            print rv;
            return rv;
        }
        DstPortModValue = core_id;
        rv = bcm_field_qualify_DstPort(unit, ent, DstPortModValue, DstPortModMask, DstPortValue, DstPortMask);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_DstPort with dest_port (mod) %d (port) %d\n", DstPortModValue, DstPortValue);
            print rv;
            return rv;
        }

        stats[0] = bcmFieldStatAcceptedPackets;
        stats[1] = bcmFieldStatDroppedPackets;
        rv = bcm_field_stat_create(unit, grp, 2, &(stats[0]), &statId);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_stat_create\n");
            print rv;
            return rv;
        }
        rv = bcm_field_entry_stat_attach(unit, ent, statId);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_stat_attach with ent %d\n", ent);
            print rv;
            return rv;
        }
        rv = bcm_field_entry_policer_attach(unit, ent, 0, (policer_id & 0xffff));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id);
            print rv;
            return rv;
        }
        rv = bcm_field_group_install(unit, grp);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_install\n");
            print rv;
            return rv;
        }

        /* send traffic with DA=1 and Vlan tag id 100 to out_port */
        bcm_l2_addr_t_init(&l2addr, mac, 100);
        l2addr.port = out_port;
        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add with vid 1\n");
            print rv;
            return rv;
        }

        /*
         * map Vlan tag proirity=4 and cfi=0 to yellow color:
         * this will mean that the packet will arrive with a yellow color instead of green
         * if meter mode is not COLOR_BLIND, the packet will go straight to the excess bucket
         * (without going through the committed bucket)
         */
        rv = bcm_port_vlan_priority_map_set(unit, in_port, 4 /* priority */, 0 /* cfi */,
                                                  1 /* internal priority */, bcmColorYellow /* color */);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
            print rv;
            return rv;
        }

        /*
         * map Vlan tag proirity=1 and cfi=0 to green color:
         * packet that will arrive with a green color will go through the committed bucket
         * and the excess bucket
         */
        rv = bcm_port_vlan_priority_map_set(unit, in_port, 1 /* priority */, 0 /* cfi */,
                                                  0 /* internal priority */, bcmColorGreen /* color */);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_vlan_priority_map_set with prio 1\n");
            print rv;
            return rv;
        }
    }

    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

int
mef_10_2_traffic_example(int unit, int in_port, int out_port, int is_arad, int mode, int color_blind) {

    int rv;
    int trap_id;
    bcm_rx_trap_config_t trap_cfg;

    int group_priority = 6;
    bcm_field_group_t grp = 1;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent1;
    bcm_field_entry_t ent2;
    bcm_field_entry_t ent3;
    bcm_field_entry_t ent4;
    int inPortValue = in_port; /* incoming port */
    int inPortMasdk = 0xffffffff;
    bcm_field_stat_t stats[2];
    int statId;
    int DstPortModValue = 0;
    int DstPortModMask = 0xffffffff;
    int DstPortValue = out_port; /* outgoing port */
    int DstPortMask  = 0xffffffff;
    int core_id; /*core matching the in_port*/
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};

    if (mode != bcmPolicerModeCascade && mode != bcmPolicerModeCoupledCascade) {
        return -1;
    }
    if (!is_arad) {

        /* allocate a trap_id for unknown_da */
        bcm_rx_trap_type_create(unit, 0, bcmRxTrapVlanUnknownDa, &trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_rx_trap_type_create\n");
            print rv;
            return rv;
        }
        printf("trap id: %d\n", trap_id);

        /* configure the trap properties */
        bcm_rx_trap_config_t_init(&trap_cfg);
        trap_cfg.trap_strength = 5;
        trap_cfg.flags |= BCM_RX_TRAP_REPLACE;

        /* send to port 2 as destination */
        trap_cfg.flags |= BCM_RX_TRAP_UPDATE_DEST;
        trap_cfg.dest_port = out_port;

        /* map packet to meter_id 1 */

        trap_cfg.flags |= BCM_RX_TRAP_UPDATE_POLICER;
        trap_cfg.policer_id = metering_info.policer_id;


        rv = bcm_rx_trap_set(unit, trap_id, &trap_cfg);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_rx_trap_set\n");
            print rv;
            return rv;
        }
    }
    else { /* Arad */
        bcm_policer_config_t_init(&pol_cfg);
        pol_cfg.flags |= BCM_POLICER_WITH_ID;

        if (color_blind) {
            pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
        }

        pol_cfg.ckbits_sec = 30000; /* 30Mbps */
        pol_cfg.ckbits_burst = 1000;
        pol_cfg.pkbits_burst = 1000;
        pol_cfg.max_pkbits_sec = 20000; /* 20Mbps max */

        switch (mode) {
            case bcmPolicerModeCascade:
                pol_cfg.mode = bcmPolicerModeCascade;
                pol_cfg.ckbits_sec = 50000;
                pol_cfg.pkbits_sec = 30000;
                pol_cfg.max_ckbits_sec = -1; /* when sharing is enabled, max_ckbits_sec is also used */
                pol_cfg.max_pkbits_sec = -1; /* in ARAD, max_pkbits_sec is always used */
                pol_cfg.pkbits_burst = 6666;
                pol_cfg.ckbits_burst = 5555;
                break;
            case bcmPolicerModeCoupledCascade:
                pol_cfg.mode = bcmPolicerModeCoupledCascade;
                pol_cfg.ckbits_sec = 50000;
                pol_cfg.pkbits_sec = 30000;
                pol_cfg.max_ckbits_sec = 60000; /* when sharing is enabled, max_ckbits_sec is also used */
                pol_cfg.max_pkbits_sec = 40000; /* in ARAD, max_pkbits_sec is always used */
                pol_cfg.pkbits_burst = 6666;
                pol_cfg.ckbits_burst = 5555;
                break;
            default:
                return BCM_E_PARAM;
                break;
        }

        /* meter should be allocated in the core in which the ports are allocated*/
        rv = get_core_from_port(unit, inPortValue, &core_id);
        if (rv != BCM_E_NONE) {
            print rv;
            return rv;
        }
        pol_cfg.core_id = core_id;

        if (mode == bcmPolicerModeCascade || mode == bcmPolicerModeCoupledCascade) {
            policer_id = 32;
        } else {
            policer_id = 30;
        }

        rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
            print rv;
            return rv;
        }


        BCM_FIELD_QSET_INIT(qset);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        rv = bcm_field_group_create_mode_id(unit, qset, group_priority, bcmFieldGroupModeAuto, grp);

        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
        rv = bcm_field_group_action_set(unit, grp, aset);

        rv = bcm_field_entry_create(unit, grp, &ent1);
        rv = bcm_field_qualify_OuterVlanId(unit, ent1, 100, 0xfff);
        rv = bcm_field_entry_policer_attach(unit, ent1, 0, (policer_id & 0xffff));

        rv = bcm_field_entry_create(unit, grp, &ent2);
        rv = bcm_field_qualify_OuterVlanId(unit, ent2, 300, 0xfff);
        rv = bcm_field_entry_policer_attach(unit, ent2, 0, ((policer_id + 1 )& 0xffff));

        rv = bcm_field_entry_create(unit, grp, &ent3);
        rv = bcm_field_qualify_OuterVlanId(unit, ent3, 500, 0xfff);
        rv = bcm_field_entry_policer_attach(unit, ent3, 0, ((policer_id + 2 )& 0xffff));

        rv = bcm_field_entry_create(unit, grp, &ent4);
        rv = bcm_field_qualify_OuterVlanId(unit, ent4, 700, 0xfff);
        rv = bcm_field_entry_policer_attach(unit, ent4, 0, ((policer_id + 3 )& 0xffff));
        rv = bcm_field_group_install(unit, grp);

        rv = bcm_vlan_create(unit, 100);
        rv = bcm_vlan_create(unit, 300);
        rv = bcm_vlan_create(unit, 500);
        rv = bcm_vlan_create(unit, 700);


        bcm_l2_addr_t_init(&l2addr, mac, 100);
        l2addr.port = out_port;
        rv = bcm_l2_addr_add(unit, l2addr);

        l2addr.vid = 300;
        rv = bcm_l2_addr_add(unit, l2addr);

        l2addr.vid = 500;
        rv = bcm_l2_addr_add(unit, l2addr);

        l2addr.vid = 700;
        rv = bcm_l2_addr_add(unit, l2addr);

        /*
         * map Vlan tag proirity=4 and cfi=0 to yellow color:
         * this will mean that the packet will arrive with a yellow color instead of green
         * if meter mode is not COLOR_BLIND, the packet will go straight to the excess bucket
         * (without going through the committed bucket)
         */
        rv = bcm_port_vlan_priority_map_set(unit, in_port, 4 /* priority */, 0 /* cfi */,
                                                  1 /* internal priority */, bcmColorYellow /* color */);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
            print rv;
            return rv;
        }

        /*
         * map Vlan tag proirity=1 and cfi=0 to green color:
         * packet that will arrive with a green color will go through the committed bucket
         * and the excess bucket
         */
        rv = bcm_port_vlan_priority_map_set(unit, in_port, 1 /* priority */, 0 /* cfi */,
                                                  0 /* internal priority */, bcmColorGreen /* color */);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_vlan_priority_map_set with prio 1\n");
            print rv;
            return rv;
        }
    }

    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

/*
 * to run this example, the following soc properties must be set:
 *      policer_ingress_count=32 (to allow 2 meters per-packet)
 *      policer_ingress_sharing_mode=1/2 (SERIAL/PARALLEL)
 * SERIAL - The result of the first meter is fed into the second meter
 * PARALLEL - The final meter result is the worst-case result of the two meters
 *
 * run:
 * cint examples/cint_policer_metering_example
 * cint
 * two_meterw_per_packet_example(unit, <out_port>);
 *
 * run traffic with DA=1 and Vlan tag id 100 at 1G rate, together with traffic with DA=1 and Vlan tag id 300 at 1G rate:
 *      both streams will share the same meter in group=1 and have different meters in group=0.
 *      meter in group=1 has a 15M rate and the 2 meters in group=0 both have 25M rate
 *      (one is single rate of 25M and the other is 2 rates of 20M + 5M).
 *
 * in both cases (sharing_mode = SERIAL/PARALLEL) traffic will arrive at out_port with 15M rate (combined for the 2 streams).
 * if sharing_mode is SERIAL: group=0 meter will return a 25M rate and group=1 meter will lower this rate to 15M.
 * if sharing_mode is PARALLEL: the worst case of the 2 meters (which is 15M) will be the result.
 */
int
two_meter_per_packet_example(int unit, int out_port) {

    int rv, group_priority = 1;
    bcm_field_group_t grp = 1;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent1;
    bcm_field_entry_t ent2;
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_policer_config_t pol_cfg_share;
    bcm_policer_t policer_id_share;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
	int core_id;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_group_create_mode_id(unit, qset, group_priority, bcmFieldGroupModeAuto, grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_mode_id\n");
        print rv;
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel1);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set\n");
        print rv;
        return rv;
    }

    /* meters should be allocated in the core in which the ports are allocated*/
    rv = get_core_from_port(unit, out_port, &core_id);
    if (rv != BCM_E_NONE) {
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=1 with CIR = 15M */
    bcm_policer_config_t_init(&pol_cfg_share);
    pol_cfg_share.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_MACRO + BCM_POLICER_COLOR_BLIND;
    pol_cfg_share.mode = bcmPolicerModeCommitted;
    pol_cfg_share.ckbits_sec = 15000; /* 15M */
    pol_cfg_share.ckbits_burst = 3000;
    pol_cfg_share.pkbits_burst = 0;
    pol_cfg_share.max_pkbits_sec = 15000;
    pol_cfg_share.core_id = core_id;
    policer_id_share = 10;
    rv = bcm_policer_create(unit, &pol_cfg_share, &policer_id_share);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_share);
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=0, with CIR = 5M and EIR = 20M */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeTrTcmDs;
    pol_cfg.ckbits_sec = 5000; /* 5M */
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_sec = 20000; /* 20M */
    pol_cfg.pkbits_burst = 5000;
    pol_cfg.max_pkbits_sec = 25000;
    pol_cfg.core_id = core_id;
    policer_id = 1;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    /* map traffic with vlan=100 to meter-id=1 in group=0 and to meter-id=10 in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent1\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent1, 100, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 100\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent1, 0, (policer_id & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent1, 1, (policer_id_share & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_share);
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=0 with CIR = 25M (single rate) */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeSrTcm;
    pol_cfg.ckbits_sec = 25000;
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.max_pkbits_sec = 25000;
    pol_cfg.core_id = core_id;
    policer_id = 3;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    /* map traffic with vlan=300 to meter-id=3 in group=0 and to the same meter (id=10) in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent2);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent3\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent2, 300, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 300\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent2,  0, (policer_id & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent2,  1, (policer_id_share & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_share);
        print rv;
        return rv;
    }

    rv = bcm_field_group_install(unit, grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_install\n");
        print rv;
        return rv;
    }

    /* create the 2 Vlans */
    rv = bcm_vlan_create(unit, 100);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 100\n");
        print rv;
        return rv;
    }
    rv = bcm_vlan_create(unit, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 300\n");
        print rv;
        return rv;
    }

    /* send all traffic with DA=1 and one of the above Vlans to out_port */
    bcm_l2_addr_t_init(&l2addr, mac, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 100\n");
        print rv;
        return rv;
    }
    l2addr.vid = 300;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 300\n");
        print rv;
        return rv;
    }

    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

/*
 * this is a example of policer configuration for testing "advanced_color_resolution_example"
 * policer_ingress_sharing_mode is either SERIAL or PARALLEL

  * run:
 * cint examples/cint_policer_metering_example
 * cint
 * two_meter_per_packet_adv_color_resolution_example(unit, <out_port>);
 * run traffic with DA=1 and Vlan tag id 100/200/300/400 at 10G rate
 * VLAN 100:
 * Policer 1(trtcm,CIR 5M, EIR 20M) in group 0 & policer 11(committed 15M) in group 1
 * VLAN 200:
 * Policer 1(trtcm,CIR 5M, EIR 20M) in group 0 & policer 12(committed 35M) in group 1
 * VLAN 300:
 * Policer 2(srtcm,CIR 25M) in group 0 & policer 11(committed 15M) in group 1
 * VLAN 400:
 * Policer 2(srtcm,CIR 25M) in group 0 & policer 12(committed 35M) in group 1
 * Note: mark black is for QUX
 */

int
two_meter_per_packet_adv_color_resolution_example(int unit, int out_port, int mark_black) {

    int rv, group_priority = 1;
    bcm_field_group_t grp = 1;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent1,ent2,ent3,ent4;
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id_01,policer_id_02,policer_id_11,policer_id_12;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
    int core_id;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_group_create_mode_id(unit, qset, group_priority, bcmFieldGroupModeAuto, grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_mode_id\n");
        print rv;
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel1);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set\n");
        print rv;
        return rv;
    }

    /* meters should be allocated in the core in which the ports are allocated*/
    rv = get_core_from_port(unit, out_port, &core_id);
    if (rv != BCM_E_NONE) {
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=1 with CIR = 15M */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_MACRO + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeCommitted;
    pol_cfg.ckbits_sec = 15000; /* 15M */
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_burst = 0;
    pol_cfg.max_pkbits_sec = 15000;
    pol_cfg.core_id = core_id;
    policer_id_11 = 11;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id_11);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_11);
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=1 with CIR = 35M */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_MACRO + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeCommitted;
    pol_cfg.ckbits_sec = 35000; /* 35M */
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_burst = 0;
    pol_cfg.max_pkbits_sec = 15000;
    pol_cfg.core_id = core_id;
    policer_id_12 = 12;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id_12);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_12);
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=0, with CIR = 5M and EIR = 20M */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeTrTcmDs;
    pol_cfg.ckbits_sec = 5000; /* 5M */
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_sec = 20000; /* 20M */
    pol_cfg.pkbits_burst = 5000;
    pol_cfg.max_pkbits_sec = 25000;
    if (mark_black) {
        pol_cfg.mark_drop_as_black = 1;
    }
    pol_cfg.core_id = core_id;
    policer_id_01 = 1;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id_01);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_01);
        print rv;
        return rv;
    }

    /* create COLOR_BLIND meter from group=0 with CIR = 25M (single rate) */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID + BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeSrTcm;
    pol_cfg.ckbits_sec = 25000;
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.max_pkbits_sec = 25000;
    if (mark_black) {
        pol_cfg.mark_drop_as_black = 1;
    }
    pol_cfg.core_id = core_id;
    policer_id_02 = 2;
    rv = bcm_policer_create(unit, &pol_cfg, &policer_id_02);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_02);
        print rv;
        return rv;
    }

    /* map traffic with vlan=100 to meter-id=1 in group=0 and to meter-id=11 in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent1\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent1, 100, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 100\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent1, 0, (policer_id_01 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_01);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent1, 1, (policer_id_11 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_11);
        print rv;
        return rv;
    }

    /* map traffic with vlan=200 to meter-id=1 in group=0 and to meter-id=12 in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent2);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent3\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent2, 200, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 300\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent2,  0, (policer_id_01 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_01);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent2,  1, (policer_id_12 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_12);
        print rv;
        return rv;
    }

    /* map traffic with vlan=300 to meter-id=2 in group=0 and to meter-id=11 in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent3);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent1\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent3, 300, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 100\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent3, 0, (policer_id_02 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_02);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent3, 1, (policer_id_11 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_11);
        print rv;
        return rv;
    }

    /* map traffic with vlan=400 to meter-id=1 in group=0 and to meter-id=12 in group=1 */
    rv = bcm_field_entry_create(unit, grp, &ent4);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create with ent3\n");
        print rv;
        return rv;
    }
    rv = bcm_field_qualify_OuterVlanId(unit, ent4, 400, 0xfff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_OuterVlanId with vlan 300\n");
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent4,  0, (policer_id_02 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_02);
        print rv;
        return rv;
    }
    rv = bcm_field_entry_policer_attach(unit, ent4,  1, (policer_id_12 & 0xffff));
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_policer_attach with policer_id %d\n", policer_id_12);
        print rv;
        return rv;
    }

    rv = bcm_field_group_install(unit, grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_install\n");
        print rv;
        return rv;
    }

    /* create the 4 Vlans */
    rv = bcm_vlan_create(unit, 100);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 100\n");
        print rv;
        return rv;
    }
    rv = bcm_vlan_create(unit, 200);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 200\n");
        print rv;
        return rv;
    }

    rv = bcm_vlan_create(unit, 300);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 300\n");
        print rv;
        return rv;
    }
    rv = bcm_vlan_create(unit, 400);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_create with vlan 400\n");
        print rv;
        return rv;
    }

    /* send all traffic with DA=1 and one of the above Vlans to out_port */
    bcm_l2_addr_t_init(&l2addr, mac, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 100\n");
        print rv;
        return rv;
    }
    l2addr.vid = 200;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 200\n");
        print rv;
        return rv;
    }

    l2addr.vid = 300;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 300\n");
        print rv;
        return rv;
    }

    l2addr.vid = 400;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 400\n");
        print rv;
        return rv;
    }

    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

/* after each one of test above, run:
 * metering_reset(unit);
 * to destroy all the meters created
 */
int
metering_reset(int unit) {

    int rv;

    if (metering_info.policer_id2 != 0) {

        /* destroy policer 2 */
        rv = bcm_policer_destroy(unit, metering_info.policer_id2);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_destroy\n");
            print rv;
            return rv;
        }

        /* get policer 2 */
        rv = bcm_policer_get(unit, metering_info.policer_id2, &(metering_info.pol2_cfg_get));
        if (rv != BCM_E_NOT_FOUND) {
            printf("Error, bcm_policer_destroy did not destroy the entry\n");
            print rv;
            return rv;
        }
    }

    /* destroy all the policers */
    rv = bcm_policer_destroy_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_destroy_all\n");
        print rv;
    }

    /* set discard back to not drop any packets */
    rv = bcm_cosq_discard_set(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

/* discard all packets that arrive with
 *
 * run:
 * cint examples/cint_policer_metering_example
 * cint
 * discard_traffic_by_dp(unit, <in_port>, <out_port>);
 *
 * traffic to run:
 * 1)run ethernet packet with DA 1 and vlan tag id 1 from in_port, with priority 4
 *      all packets will be dropped because the packet's DP is 1 and the device
 *      is condfigured to drop all packets with DP above 0
 *
 * 2) run ethernet packet with DA 1 and vlan tag id 1 from in_port, with priority 1
 *      the packet's DP is 0 and all the packets will arrive at the out_port
 */
int
discard_traffic_by_dp(int unit, int in_port, int out_port) {

    int rv;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};

    /* send traffic with DA=1 and Vlan tag id 1 to out_port */
    bcm_l2_addr_t_init(&l2addr, mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }

    /*
     * map Vlan tag proirity=4 and cfi=0 to yellow color:
     * this will mean that the packet will arrive with a yellow color instead of green
     * if meter mode is not COLOR_BLIND, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     */
    rv = bcm_port_vlan_priority_map_set(unit, in_port, 4 /* priority */, 0 /* cfi */,
                                              1 /* internal priority */, bcmColorYellow /* color */);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
        print rv;
        return rv;
    }

    /*
     * map Vlan tag proirity=1 and cfi=0 to green color:
     * packet that will arrive with a green color will go through the committed bucket
     * and the excess bucket
     */
    rv = bcm_port_vlan_priority_map_set(unit, in_port, 1 /* priority */, 0 /* cfi */,
                                              0 /* internal priority */, bcmColorGreen /* color */);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_priority_map_set with prio 1\n");
        print rv;
        return rv;
    }

    /* set discard DP to drop all packets with DP > 1 (colored yellow, red or black) */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_COLOR_YELLOW);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}

/* Use this function instead of metering_example if you want to test that every created policer is saved properly */
int metering_example_with_check(int unit, int is_arad, int is_jer, int two_meters_groups) {
    int rv = BCM_E_NONE;
    metering_info.with_check = 1;
    rv = metering_example(unit,is_arad,is_jer,two_meters_groups);
    if (rv != BCM_E_NONE) {
        printf("Error %d, in metering_example", rv);
        return rv;
    }
    metering_info.with_check = 0;
    return rv;
}

/* Compares two policers. Since there's a slight variation between input values and stored values, a 5% variation is allowed. */
/* some cases which share the same rev_exp, the accuracy will be reduce.
    example:cascade mode: the rev_exp is the same for all rates in the group.
    So if there is huge difference between the rates, the accuracy will reduce.
*/
int compare_policers(bcm_policer_config_t original, bcm_policer_config_t other, float accuracy_percentage){
        float accuracy;
        if (original.mode == bcmPolicerModeCoupledCascade
            && other.mode == bcmPolicerModeCascade){
            /* OK */
        } else if (original.mode != other.mode){
            printf("Error: Created policer doesn't have the same mode.\n Original is:");
            print original.mode;
            printf("Other is: ");
            print other.mode;
            return -1;
        }

        accuracy = accuracy_percentage / 100;
        if (check_percent_variation(original.ckbits_sec, other.ckbits_sec, accuracy)){
            printf("Error: Created policer doesn't have the same ckbits_sec. Original was %d, other was %d\n", original.ckbits_sec, other.ckbits_sec);
            return -1;
        }
        if (check_percent_variation(original.pkbits_sec, other.pkbits_sec, accuracy)){
            printf("Error: Created policer doesn't have the same pkbits_sec. Original was %d, other was %d\n", original.pkbits_sec, other.pkbits_sec);
            return -1;
        }
        if (check_percent_variation(original.max_ckbits_sec, other.max_ckbits_sec, accuracy)){
            printf("Error: Created policer doesn't have the same max_ckbits_sec. Original was %d, other was %d\n", original.max_ckbits_sec, other.max_ckbits_sec);
            return -1;
        }
        if (check_percent_variation(original.max_pkbits_sec, other.max_pkbits_sec, accuracy)){
            printf("Error: Created policer doesn't have the same max_pkbits_sec. Original was %d, other was %d\n", original.max_pkbits_sec, other.max_pkbits_sec);
            return -1;
        }
	/*        if (check_percent_variation(original.pkbits_burst, other.pkbits_burst)){
            printf("Error: Created policer doesn't have the same pkbits_burst. Original was %d, other was %d\n", original.pkbits_burst, other.pkbits_burst);
            return -1;
        }
        if (check_percent_variation(original.ckbits_burst, other.ckbits_burst)){
            printf("Error: Created policer doesn't have the same ckbits_burst. Original was %d, other was %d\n", original.ckbits_burst, other.ckbits_burst);
            return -1;
	    }*/
        if (original.entropy_id != other.entropy_id) {
            printf("Error: entropy_ids differ: original.entropy_id=%d != %d=new.entropy_id.\n",original.entropy_id,other.entropy_id);
            return -1;
        }
		if (original.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE != other.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE) {
            printf("Error: BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE differ: original=%d != new=%d\n",original.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE,other.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE);
            return -1;
        }

        return 0;
}

int check_percent_variation(int original, int other, float accuracy){
    if (original != 0 &&
        (other < original * (1 - accuracy) ||
         other > original * (1 + accuracy))){
        return -1;
    }
    return 0;
}

/* Return <0 on error, and the entropy_id of policer on success. */
/* This is for use in the DVAPI. */
int get_policer_entropy_id(int unit, bcm_policer_t policer)
{
    bcm_policer_config_t cfg;
    int rv = BCM_E_NONE;

    bcm_policer_config_t_init(&cfg);
    cfg.flags |= BCM_POLICER_REPLACE_SHARED;

    bcm_policer_get(unit, policer, &cfg);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_get for same_prf_1_policer_id \n");
        print rv;
        return rv;
    }

    return cfg.entropy_id;
}


/*
 * Extract core from input local port. On fail, return '-1'
 */
int get_core_from_port(int unit, int port, int * core)
{
    bcm_error_t rc ;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;
	bcm_gport_t gport = port;

    rc = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit,"bcm_port_get failed. Error:%d (%s)\n"),
                                                    rc, bcm_errmsg(rc)));
        return -1;
    } else {
        *core = mapping_info.core ;
    }
    return 0;
}

/*
   Should be used with either Serial or Parallel metering configured.
   This function configures advacnce metering color resolution, in which meter-0 is set to mark exceeding packets with Black
   color and meter-1 is set to mark exceeding packets Black if policer action is set to 1 or 3 and Red if policer action is 0 or 2.
*/
int advanced_color_resolution_example(int unit)
{
    bcm_policer_color_decision_t dec, dec1;
    bcm_policer_color_resolution_t res, res1;
    bcm_policer_color_t pol0_color, pol1_color;
    bcm_color_t pol_color, eth_color, in_color, eth_max_color;
    uint8 pol0_other_bucket, pol1_other_bucket;
    uint32 pol_act;
    int rv = BCM_E_NONE;
    int is_jericho, is_qux;
    int pol0_mark_black, pol1_mark_black;

    if (is_device_or_above(unit,ARAD_PLUS) == 0){
        printf("metering advance color resolution isn't supported on devices prior to Arad+\n");
        return rv;
    }

    is_jericho = is_device_or_above(unit, JERICHO);
    is_qux = is_device_or_above(unit, QUMRAN_UX);

    /*Set Policer0 to Resovle {Green, Yellow, Black} and Policer1 to Resolve {Green, Yellow, Red}*/
    for (pol0_color = bcmPolicerColorGreen; pol0_color <= bcmPolicerColorRed; pol0_color = pol0_color + 1) {
        for (pol0_other_bucket = 0; pol0_other_bucket <= 1; pol0_other_bucket++) {
            for (pol1_color = bcmPolicerColorGreen; pol1_color <= bcmPolicerColorRed; pol1_color = pol1_color + 1) {
                for (pol1_other_bucket = 0; pol1_other_bucket <= 1; pol1_other_bucket++) {
                    for (pol0_mark_black = 0; pol0_mark_black <= 1; pol0_mark_black++) {
                        for (pol1_mark_black = 0; pol1_mark_black <= 1; pol1_mark_black++) {
                            /*In Arad+ table is used only when both meters are valid, therefore the
                              bcmPolicerColorInvalid color isn't applicable.*/
                            if (!is_jericho && (pol0_color == bcmPolicerColorInvalid || pol1_color == bcmPolicerColorInvalid)){
                                continue;
                            }
                            if((is_qux == FALSE) && (pol0_mark_black != 0 || pol1_mark_black != 0))
                            {
                                continue;
                            }
                            /*Set the table key*/
                            dec.policer0_color = pol0_color;
                            dec.policer0_other_bucket_has_credits = pol0_other_bucket;
                            dec.policer1_color = pol1_color;
                            dec.policer1_other_bucket_has_credits = pol1_other_bucket;
                            dec.policer0_mark_drop_as_black = pol0_mark_black;
                            dec.policer1_mark_drop_as_black = pol1_mark_black;

                            /*Set the value*/
                            if (pol0_color == bcmPolicerColorRed) {
                                /*Policer 0 Red resolves to Black*/
                                dec.policer0_update_bucket = bcmPolicerColorRed;
                                if (pol1_color == bcmPolicerColorInvalid) {
                                    dec.policer1_update_bucket = bcmPolicerColorInvalid;
                                } else {
                                    dec.policer1_update_bucket = bcmPolicerColorRed;
                                }
                                if (is_qux == FALSE){
                                    dec.policer_color = bcmColorBlack;
                                }
                                else if ((is_qux == TRUE) && (pol0_mark_black != 0)){
                                    dec.policer_color = bcmColorBlack;
                                }
                                else {
                                    dec.policer_color = bcmColorRed;
                               }
                            }
                            else if (pol1_color == bcmPolicerColorRed) {
                                /*Policer 1 Red resolves to Red*/
                                if (pol0_color == bcmPolicerColorInvalid) {
                                    dec.policer0_update_bucket = bcmPolicerColorInvalid;
                                } else {
                                    dec.policer0_update_bucket = bcmPolicerColorRed;
                                }
                                dec.policer1_update_bucket = bcmPolicerColorRed;
                                dec.policer_color = bcmColorRed;
                            }
                            else if (pol0_color == bcmPolicerColorInvalid || pol1_color == bcmPolicerColorInvalid) {
                                /*One of the meters is invalid, the other isn't Red*/
                                dec.policer0_update_bucket = pol0_color;
                                dec.policer1_update_bucket = pol1_color;
                                if (pol0_color == bcmPolicerColorYellow || pol1_color == bcmPolicerColorYellow) {
                                    dec.policer_color = bcmColorYellow;
                                }
                                else{
                                    dec.policer_color = bcmColorGreen;
                                }
                            }
                            else if (pol0_color == pol1_color){
                                /*Both meters resolve to the same color*/
                                dec.policer0_update_bucket = pol1_color;
                                dec.policer1_update_bucket = pol1_color;
                                if (pol1_color == bcmPolicerColorYellow) {
                                    dec.policer_color = bcmColorYellow;
                                }
                                else {
                                    dec.policer_color = bcmColorGreen;
                                }
                            }
                            else {
                                /*One resolves Green the other Yellow, output is Yellow anyway*/
                                dec.policer_color = bcmColorYellow;
                                if (pol0_color == bcmPolicerColorYellow) {
                                    dec.policer0_update_bucket = bcmPolicerColorYellow;
                                    if (pol1_other_bucket) {
                                        dec.policer1_update_bucket = bcmPolicerColorYellow;

                                    } else {
                                        dec.policer1_update_bucket = bcmPolicerColorGreen;
                                    }
                                }
                                else {
                                    dec.policer1_update_bucket = bcmPolicerColorYellow;
                                    if (pol0_other_bucket) {
                                        dec.policer0_update_bucket = bcmPolicerColorYellow;
                                    } else {
                                        dec.policer0_update_bucket = bcmPolicerColorGreen;
                                    }
                                }
                            }

                            rv = bcm_policer_color_decision_set(unit, &dec);
                            if (rv != BCM_E_NONE) {
                                printf("Error in bcm_policer_color_decision_set\n");
                                print rv;
                                return rv;
                            }

                            if (metering_info.with_check) {
                                dec1.policer0_color = dec.policer0_color;
                                dec1.policer0_other_bucket_has_credits = dec.policer0_other_bucket_has_credits;
                                dec1.policer1_color = dec.policer1_color;
                                dec1.policer1_other_bucket_has_credits = dec.policer1_other_bucket_has_credits;
                                dec1.policer0_update_bucket = 0;
                                dec1.policer1_update_bucket = 0;
                                dec1.policer_color =  0;

                                rv = bcm_policer_color_decision_get(unit, &dec1);
                                if (rv != BCM_E_NONE) {
                                    printf("Error in bcm_policer_color_decision_get\n");
                                    print rv;
                                    return rv;
                                }
                                if (dec.policer0_update_bucket != dec1.policer0_update_bucket ||
                                    dec.policer1_update_bucket != dec1.policer1_update_bucket ||
                                    dec.policer_color != dec1.policer_color ) {
                                    printf("Confiugred and received color decision mis-match.\n");
                                    printf("Configured:\n");
                                    print dec;
                                    printf("Received:\n");
                                    print dec1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

	/*Ethenet policer is part of the key for the color resolution table only in Jericho
	  In Arad+, Ethernet Policer color is reflected in Policer Color, this will be Black if Ethernet Policer
	  decides to drop the packet*/
	eth_max_color = is_jericho ? bcmColorBlack : bcmColorGreen;

	/*Set color resolution, when (policer_action % 2 == 1) policer_1 Red also resolves to Black*/
	for (pol_color = bcmColorGreen; pol_color <= bcmColorBlack; pol_color = pol_color + 1) {
		for (eth_color = bcmColorGreen; eth_color <= eth_max_color; eth_color = eth_color + 1) {
			for (in_color = bcmColorGreen; in_color <= bcmColorBlack; in_color = in_color + 1) {
				for (pol_act = 0; pol_act <= 3; pol_act++) {
					res.policer_color = pol_color;
					res.ethernet_policer_color = eth_color;
					res.incoming_color = in_color;
					res.policer_action = pol_act;

					if (eth_color == bcmColorBlack) {
						res.egress_color = res.ingress_color = bcmColorBlack;
					}
					else {
						res.egress_color = res.ingress_color = pol_color;
						if ((pol_act %2 == 1) && (pol_color == bcmColorRed)){
							res.egress_color = res.ingress_color = bcmColorBlack;
						}
					}

					rv = bcm_policer_color_resolution_set(unit, &res);
					if (rv != BCM_E_NONE) {
						printf("Error in bcm_policer_color_resolution_set\n");
						print rv;
						return rv;
					}

					if (metering_info.with_check) {
						res1.policer_color = res.policer_color;
						res1.ethernet_policer_color = res.ethernet_policer_color;
						res1.incoming_color = res.incoming_color;
						res1.policer_action = res.policer_action;
						res1.egress_color = 0;
						res1.ingress_color = 0;

						rv = bcm_policer_color_resolution_get(unit, &res1);
						if (rv != BCM_E_NONE) {
							printf("Error in bcm_policer_color_decision_get\n");
							print rv;
							return rv;
						}
						if (res.ingress_color != res1.ingress_color ||
							res.egress_color != res1.egress_color) {
							printf("Confiugred and received color resolution mis-match.\n");
							printf("Configured:\n");
							print res;
							printf("Received:\n");
							print res1;
						}
					}
				}
			}
		}
	}

    return rv;
}

/* enable or disable global config MEF 10.3.
 * used when the meter mode=bcmPolicerModeCoupledCascade
 * If disable, the configuration is MEF 10.2
 * supported only in Jericho devices and above
 */
int
meter_mode_mef_10_3_enable(
    int unit,
    int enable)
{

    int rv = BCM_E_NONE;
    int enable_get = 0;

    rv = bcm_switch_control_set(unit, bcmSwitchMeterMef10dot3Enable, enable);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_switch_control_set\n");
        print rv;
        return rv;
	}

    rv = bcm_switch_control_get(unit, bcmSwitchMeterMef10dot3Enable, &enable_get);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_switch_control_get\n");
        print rv;
        return rv;
    }

    if(enable != enable_get)
    {
        printf("enable_get=%d, but enable_set=%d\n", enable_get, enable);
        return BCM_E_FAIL;
    }
    return rv;
}

/* example to test min CIR/EIR */
int
meter_min_rate_example(int unit, int in_port, int out_port, int mode, int color_blind, int step) {

    int rv;

    int group_priority = 6;
    bcm_field_group_t grp = 1;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent1;
    bcm_field_entry_t ent2;
    bcm_field_entry_t ent3;
    bcm_field_entry_t ent4;
    int inPortValue = in_port; /* incoming port */
    int inPortMasdk = 0xffffffff;
    bcm_field_stat_t stats[2];
    int statId;
    int DstPortModValue = 0;
    int DstPortModMask = 0xffffffff;
    int DstPortValue = out_port; /* outgoing port */
    int DstPortMask  = 0xffffffff;
    int core_id; /*core matching the in_port*/
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};

    if (mode != bcmPolicerModeCascade && mode != bcmPolicerModeCoupledCascade) {
        return -1;
    }

    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID;

    if (color_blind) {
        pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    }

    switch (mode) {
        case bcmPolicerModeCascade:
            pol_cfg.mode = bcmPolicerModeCascade;
            pol_cfg.ckbits_sec = 100;
            pol_cfg.pkbits_sec = 100;
            pol_cfg.max_ckbits_sec = 100;
            pol_cfg.max_pkbits_sec = 100;
            pol_cfg.pkbits_burst = 100;
            pol_cfg.ckbits_burst = 100;
            break;
        case bcmPolicerModeCoupledCascade:
            pol_cfg.mode = bcmPolicerModeCoupledCascade;
            pol_cfg.ckbits_sec = 100;
            pol_cfg.pkbits_sec = 100;
            pol_cfg.max_ckbits_sec = 100;
            pol_cfg.max_pkbits_sec = 100;
            pol_cfg.pkbits_burst = 100;
            pol_cfg.ckbits_burst = 100;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }

    /* meter should be allocated in the core in which the ports are allocated*/
    rv = get_core_from_port(unit, inPortValue, &core_id);
    if (rv != BCM_E_NONE) {
        print rv;
        return rv;
    }
    pol_cfg.core_id = core_id;

    if (mode == bcmPolicerModeCascade || mode == bcmPolicerModeCoupledCascade) {
        policer_id = 32;
    } else {
        policer_id = 30;
    }

    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    /* step to set different rate for a single policer in cascade */
    policer_id = 33;
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_WITH_ID;
    pol_cfg.mode = mode;

    /*
     * step 1, set CIR = 0, maxCIR = 0
     * step 2, set EIR = 0, maxEIR = 0
     * step 3, set EIR = 0, maxEIR != 0
     */
    switch (step) {
        case 1:
            pol_cfg.ckbits_sec = 0;
            pol_cfg.pkbits_sec = 22;
            pol_cfg.max_ckbits_sec = 0;
            pol_cfg.max_pkbits_sec = 22;
            pol_cfg.ckbits_burst = 0;
            pol_cfg.pkbits_burst = 100;
            break;
        case 2:
            pol_cfg.ckbits_sec = 22;
            pol_cfg.pkbits_sec = 0;
            pol_cfg.max_ckbits_sec = 22;
            pol_cfg.max_pkbits_sec = 0;
            pol_cfg.ckbits_burst = 100;
            pol_cfg.pkbits_burst = 0;
            break;
        case 3:
            pol_cfg.ckbits_sec = 22;
            pol_cfg.pkbits_sec = 0;
            pol_cfg.max_ckbits_sec = 100;
            pol_cfg.max_pkbits_sec = 88;
            pol_cfg.ckbits_burst = 100;
            pol_cfg.pkbits_burst = 0;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }

    rv = bcm_policer_set(unit, policer_id, &pol_cfg);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_set with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    policer_id = 32;
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_group_create_mode_id(unit, qset, group_priority, bcmFieldGroupModeAuto, grp);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
    rv = bcm_field_group_action_set(unit, grp, aset);

    rv = bcm_field_entry_create(unit, grp, &ent1);
    rv = bcm_field_qualify_OuterVlanId(unit, ent1, 100, 0xfff);
    rv = bcm_field_entry_policer_attach(unit, ent1, 0, (policer_id & 0xffff));

    rv = bcm_field_entry_create(unit, grp, &ent2);
    rv = bcm_field_qualify_OuterVlanId(unit, ent2, 300, 0xfff);
    rv = bcm_field_entry_policer_attach(unit, ent2, 0, ((policer_id + 1 )& 0xffff));
    rv = bcm_field_entry_create(unit, grp, &ent3);

    rv = bcm_field_qualify_OuterVlanId(unit, ent3, 500, 0xfff);
    rv = bcm_field_entry_policer_attach(unit, ent3, 0, ((policer_id + 2 )& 0xffff));
    rv = bcm_field_entry_create(unit, grp, &ent4);

    rv = bcm_field_qualify_OuterVlanId(unit, ent4, 700, 0xfff);
    rv = bcm_field_entry_policer_attach(unit, ent4, 0, ((policer_id + 3 )& 0xffff));
    rv = bcm_field_group_install(unit, grp);

    rv = bcm_vlan_create(unit, 100);
    rv = bcm_vlan_create(unit, 300);
    rv = bcm_vlan_create(unit, 500);
    rv = bcm_vlan_create(unit, 700);

    bcm_l2_addr_t_init(&l2addr, mac, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    l2addr.vid = 300;
    rv = bcm_l2_addr_add(unit, l2addr);

    l2addr.vid = 500;
    rv = bcm_l2_addr_add(unit, l2addr);

    l2addr.vid = 700;
    rv = bcm_l2_addr_add(unit, l2addr);

    /*
     * map Vlan tag proirity=4 and cfi=0 to yellow color:
     * this will mean that the packet will arrive with a yellow color instead of green
     * if meter mode is not COLOR_BLIND, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     */
    rv = bcm_port_vlan_priority_map_set(unit, in_port, 4 /* priority */, 0 /* cfi */,
                                              1 /* internal priority */, bcmColorYellow /* color */);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_priority_map_set with prio 4\n");
        print rv;
        return rv;
    }

    /*
     * map Vlan tag proirity=1 and cfi=0 to green color:
     * packet that will arrive with a green color will go through the committed bucket
     * and the excess bucket
     */
    rv = bcm_port_vlan_priority_map_set(unit, in_port, 1 /* priority */, 0 /* cfi */,
                                              0 /* internal priority */, bcmColorGreen /* color */);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_priority_map_set with prio 1\n");
        print rv;
        return rv;
    }

    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    return rv;
}
