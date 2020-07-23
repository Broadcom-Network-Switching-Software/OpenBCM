/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cint Knet example code
 *
 * Run script:
 * cd ../../../src/examples/sand
 * cint cint_knet.c
 * cint
 * knet_example(unit, port);
 */

/* function to add a knet interface.
 * bcm_knet_netif_create/get API were used in function
 */
int
knet_netif_create(int unit,int port)
{
    int rc;
    bcm_mac_t mac = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07};
    bcm_knet_netif_t netif, net_if_get;
    int netif_id;

    /* Create virtual network interface by bcm_knet_netif_create*/
    bcm_knet_netif_t_init(&netif);
    netif.type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
    netif.flags = BCM_KNET_NETIF_F_ADD_TAG;
    netif.mac_addr = mac;
    netif.vlan = 2000;
    netif.port = port;
    sal_memcpy(netif.name, "bcm_eth_1",9);
    rc = bcm_knet_netif_create(unit, &netif);
    if (rc != BCM_E_NONE) {
            printf("Error, in function bcm_knet_netif_create. \n");
            return -1;
    }
    netif_id = netif.id;

    /* Get virtual network interface by bcm_knet_netif_get*/
    bcm_knet_netif_t_init(&net_if_get);
    rc = bcm_knet_netif_get(unit, netif_id, &net_if_get);
    if (rc != BCM_E_NONE) {
            printf("Error, Can not get the created netif by bcm_knet_netif_get. \n");
            return -1;
    }

    return netif_id;
}


/* function to delete knet interface.
 * bcm_knet_netif_destroy API was used in function
 */
int
knet_netif_destroy(int unit,int netif_id)
{
    int rc;

    /* Delete virtual network interface by bcm_knet_netif_destroy*/
    rc = bcm_knet_netif_destroy(unit, netif_id);
    if (rc != BCM_E_NONE) {
        printf("Error, in function bcm_knet_netif_destroy. \n");
    }
    return rc;
}

/* function to add a knet filter, it will send packet to a knet interface by match vlan=200.
 * bcm_knet_filter_create/get API were used in function
 */
int
knet_filter_create(int unit,int netif_id)
{
    int rc;
    bcm_knet_filter_t filter, filter_get;
    int filter_id;

    /* Create filter by bcm_knet_netif_create, match packet vlan 200, and send to Knet interface netif_id*/
    bcm_knet_filter_t_init(&filter);
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    filter.flags = BCM_KNET_FILTER_F_STRIP_TAG;
    filter.priority = 30;
    filter.dest_type = BCM_KNET_DEST_T_NETIF;
    filter.dest_id = netif_id;
    filter.match_flags = BCM_KNET_FILTER_M_VLAN;
    filter.m_vlan = 200;
    rc = bcm_knet_filter_create(unit, &filter);
    if (rc != BCM_E_NONE) {
            printf("Error, in function bcm_knet_filter_create. \n");
            return -1;
    }
    filter_id = filter.id;

    /* Get filter by bcm_knet_netif_get*/
    bcm_knet_filter_t_init(&filter_get);
    rc = bcm_knet_filter_get(unit, netif_id, &filter_get);
    if (rc != BCM_E_NONE) {
            printf("Error, Can not get the created filter by bcm_knet_filter_get. \n");
            return -1;
    }

    return filter_id;
}


/* function to delete knet filter.
 * bcm_knet_filter_destroy API was used in function
 */
int
knet_filter_destroy(int unit,int filter_id)
{
    int rc;

    /* Delete virtual network interface by bcm_knet_netif_destroy*/
    rc = bcm_knet_filter_destroy(unit, filter_id);
    if (rc != BCM_E_NONE) {
        printf("Error, in function bcm_knet_filter_destroy. \n");
    }
    return rc;
}

/* An example to the add a knet interface, and add knet filter,
 *     if packet's vlan is 200, then send packet to the knet interface.
 */
int _netif_id, _filter_id;
int
knet_example(int unit,int port)
{
    int rc;
    printf("Create Knet interface and filter. \n");

    /* Create virtual network interface*/
    _netif_id = knet_netif_create(unit, port);
    if (_netif_id == -1) {
            printf("Error, can not create knet interface. \n");
            return -1;
    }
    printf("create knet interface id %d. \n",_netif_id);

    /* Create filter that send packet to knet interface by match vlan*/
    _filter_id = knet_filter_create(unit, _netif_id);
    if (_filter_id == -1) {
            printf("Error, can not create knet filter. \n");
            return -1;
    }
    printf("create knet filter id %d. \n",_filter_id);

    return rc;
}

/* clean configure.
 */
int
knet_example_clean(int unit)
{
    int rc;
    printf("Delete Knet interface and filter. \n");

    /* delete knet filter*/
    rc = knet_filter_destroy(unit, _filter_id);
    if (rc != BCM_E_NONE) {
            printf("Error, can not delete knet filter. \n");
            return rc;
    }
    printf("delete knet filter id %d. \n",_filter_id);

    /* delete knet interface*/
    rc = knet_netif_destroy(unit, _netif_id);
    if (rc != BCM_E_NONE) {
            printf("Error, can not delete knet interface. \n");
            return rc;
    }
    printf("delete knet interface id %d. \n",_netif_id);

    return rc;
}

