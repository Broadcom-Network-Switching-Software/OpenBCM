/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_fec_tests.c
 * cint
 * test_fec_with_max_mgid(0,40960);
 * exit;
 */

/*
 * Get maximum possible id for multicast group either egress or ingress
 * Try to create FEC with this id
 */
int test_fec_with_max_mgid(int unit, int fec_id)
{
    bcm_l3_egress_t l3eg;
    int flags = 0;
    int rv = BCM_E_NONE;
    bcm_if_t l3egid = fec_id;
    int mgid;
    int nof_egress_mcid, nof_ingress_mcid;

    if (is_device_or_above(unit, JERICHO2)) {
        nof_egress_mcid  = *(dnxc_data_get(unit,  "multicast", "params", "max_egr_mc_groups", NULL));
        nof_ingress_mcid = *(dnxc_data_get(unit,  "multicast", "params", "max_ing_mc_groups", NULL));

        /* On JR1, BCM_L3_IPMC is used to decide fec_entry_type, when creating FEC.
         * On JR2, BCM_L3_IPMC is not supported, when creating FEC.
         */
        flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    } else {
        nof_egress_mcid = diag_test_mc(unit, "get_nof_mcids");
        nof_ingress_mcid = diag_test_mc(unit, "get_nof_ingress_mcids");

        flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID | BCM_L3_IPMC;
    }
    printf("Egress MC group num:%d, Ingress MC group num:%d\n", nof_egress_mcid, nof_ingress_mcid);

    mgid = (nof_egress_mcid > nof_ingress_mcid) ? (nof_egress_mcid - 1) : (nof_ingress_mcid - 1);

    bcm_l3_egress_t_init(&l3eg);
    BCM_GPORT_MCAST_SET(l3eg.port, mgid);

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_l3_egress_create (%s) for FEC - 0x%x, MGID - %d\n",bcm_errmsg(rv), fec_id, mgid);
    }
    else
    {
        printf("FEC creation successful for FEC-ID - 0x%x, MGID - %d \n", fec_id, mgid);
        print l3egid;
        bcm_l3_egress_destroy(unit, l3egid);
    }

    return rv;
}

