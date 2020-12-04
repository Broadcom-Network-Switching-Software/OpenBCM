/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides Fabric diagnostics example
 */


int
cint_fabric_connectivity_get(
    int unit,
    int nof_links,
    int fabric_port_base,
    bcm_fabric_link_connectivity_t *links_connectivity_array)
{
    int rv = BCM_E_NONE;

    int link_index;
    int connected_link_index = 0;
    int connectivity_result_size;
    bcm_fabric_link_connectivity_t links_connectivity_result[nof_links];
    bcm_port_config_t config;

    rv = bcm_fabric_link_connectivity_status_get(unit, nof_links, links_connectivity_result, &connectivity_result_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error: problem with bcm_fabric_link_connectivity_status_get; \n");
        return rv;
    }

    printf("nof links arg: %d\n", nof_links);
    printf("nof links result: %d\n", connectivity_result_size);
    bcm_port_config_get(unit, &config);


    for( link_index = 0; link_index < nof_links; link_index++ )
    {
        if (BCM_PBMP_MEMBER(config.sfi, fabric_port_base + link_index))
        {
            links_connectivity_array[link_index].module_id = links_connectivity_result[connected_link_index].module_id;
            links_connectivity_array[link_index].link_id = links_connectivity_result[connected_link_index].link_id;
            links_connectivity_array[link_index].device_type = links_connectivity_result[connected_link_index].device_type;
            connected_link_index++;
        }
        else
        {
            links_connectivity_array[link_index].module_id = 0;
            links_connectivity_array[link_index].link_id = 0;
            links_connectivity_array[link_index].device_type = bcmFabricDeviceTypeUnknown;
        }
    }

    return rv;
}
