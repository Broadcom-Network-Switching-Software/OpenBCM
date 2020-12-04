/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Running snake test applicataion
 */

/*
 * flags: set snake test flags: 
 * SOC_DFE_ENABLE_PHY_LOOPBACK      - PHY loopback 
 * SOC_DFE_ENABLE_MAC_LOOPBACK      - MAC loopback 
 * SOC_DFE_ENABLE_EXTERNAL_LOOPBACK - assume External loopback 
 */
int run_snake(int unit, uint32 prepare_flags)
{
    int rv;
    soc_fabric_cell_snake_test_results_t res;
    
    /*first disabe rx_los application - no need to check errors for case rx los application is already disabled*/
    rx_los_unit_detach(unit);    

    rv =  soc_dfe_cell_snake_test_prepare(unit,prepare_flags); 
    if(rv != 0) {
        printf("Error, in soc_dfe_cell_snake_test_prepare, rv=%d, \n", rv);
        return rv;
    }
    
    rv = soc_dfe_cell_snake_test_run(unit, 0, &res);
    if(rv != 0) {
        printf("Error, in soc_dfe_cell_snake_test_run, rv=%d, \n", rv);
        return rv;
    }

    print res;
    if (res.test_failed == 0) {
        printf("run_snake: PASS\n");
    }

    return 0;
};

int is_external_loopback(int unit) {
    int rv;

    int nof_links = SOC_DFE_DEFS_GET_NOF_LINKS(&unit);
    bcm_fabric_link_connectivity_t connectivity_links[nof_links];
    int nof_connectivity_links;
    int i, external_loopback_links;

    rv = bcm_fabric_link_connectivity_status_get(unit, nof_links, connectivity_links, &nof_connectivity_links);

    for (i = 0; i < nof_links; ++i) {
        if (connectivity_links[i].device_type != 0) {
            ++external_loopback_links;
        }
    }

    if (external_loopback_links != nof_links) {
        return BCM_E_FAIL;
    }

    printf("is_external_loopback: PASS\n\n");
    return rv;
}
