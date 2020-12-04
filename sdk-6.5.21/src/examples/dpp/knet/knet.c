/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *File: knet.c
 * Purpose: This CINT script demonstrate how to send and receive packets over KNET interface, including:
 *
 * 1. Example of KNET interface of type BCM_KNET_NETIF_T_TX_CPU_INGRESS
 * 2. Example of KNET interface of type BCM_KNET_NETIF_T_TX_LOCAL_PORT
 * 3. Example of KNET interface of type BCM_KNET_NETIF_T_TX_META_DATA
 *
 *    Prerequistes:
 *    =============
 *      a) Build SDK with KNET feature enabled
 *      b) Build $SDK/src/examples/dpp/knet/knet_tx.c and knet_rx.c
 *         source files for customer target CPU
 *      c) Insert knet kernel module before launching SDK.
 *      d) Requried SOC proterties:
 *           tm_port_header_type_in_200.0=INJECTED_2_PP
 *           tm_port_header_type_out_202.0=CPU
 *
 * 1. Example of KNET interface of type BCM_KNET_NETIF_T_TX_CPU_INGRESS
 *
 *    In TX direction, KNET interface of type "CPU INGRESS" supports only "Rx Port Injection".
 *    In RX direction, raw Ethernet packets are sent to KNET interface
 *
 * Set up sequence:
 *    cint ../../../../src/examples/dpp/knet/knet.c
 *    cint
 *    knet_interface_example_cpu_ingress(0);
 *    exit;
 *
 * 2. Example of KNET interface of type BCM_KNET_NETIF_T_TX_LOCAL_PORT
 *
 *    In TX direction, KNET interface of type "LOCAL PORT" supports only "Tx Wire Injection".
 *    In RX direction, raw Ethernet packets are sent to KNET interface
 *
 * Set up sequence:
 *    cint ../../../../src/examples/dpp/knet/knet.c
 *    cint
 *    knet_interface_example_local_port(0);
 *    exit;
 *
 * 3. Example of KNET interface of type BCM_KNET_NETIF_T_TX_META_DATA
 *
 *    In TX direction, KNET interface in RCPU mode supports various kinds of injections, typically:
 *      a) Rx Port Injection
 *      b) Tx Wire Injection
 *      c) Egress PP Injection
 *    In RX direction, this exampls shows how to decode "RCPU L2(18) + RCPU(14) + Metadata(64)"
 *
 * Set up sequence:
 *    cint ../../../../src/examples/dpp/knet/knet.c
 *    cint
 *    knet_interface_example_rcpu(0);
 *    exit;
 */

cint_reset();

bcm_port_t in_port  = 200;
bcm_port_t out_port = 202;

bcm_error_t
knet_rx_port_injection_info_dump(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int core;
    uint32 tm_port;
    uint32 cpu_channel;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t config;

    rv = bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_get() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_config_get(unit, &config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (BCM_PBMP_MEMBER(config.cpu, in_port))
    {
        core = mapping_info.core;
        tm_port = mapping_info.tm_port;
        cpu_channel = mapping_info.channel;;
    }
    else
    {
        int port_ndx;
        int has_cpu_port = FALSE;

        core = mapping_info.core;

        BCM_PBMP_ITER(config.cpu, port_ndx)
        {
            rv = bcm_port_get(unit, port_ndx, &flags, &interface_info, &mapping_info);
            if(BCM_FAILURE(rv)) {
                printf("\nError in bcm_port_get() : %s\n", bcm_errmsg(rv));
                return rv;
            }
            if (mapping_info.core == core)
            {
                /*
                 * Set source tm port as first CPU port which is on the same core
                 */
                tm_port = mapping_info.tm_port;
                cpu_channel = mapping_info.channel;
                has_cpu_port = TRUE;
                break;
            }
        }
        if (!has_cpu_port)
        {
            printf("\nNo cpu port\n");
            return BCM_E_INTERNAL;
        }
    }

    printf("\n KNET Rx Port Injection:\n");
    printf("PTCH.SSP %u(0x%x)\n", tm_port, tm_port);
    printf("CPU Channel %u(0x%x)\n", cpu_channel, cpu_channel);

    return rv;
}

bcm_error_t
knet_tx_wire_injection_info_dump(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int core;
    uint32 tm_port;
    uint32 cpu_channel;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t config;
    bcm_gport_t sysport_gport;

    int cpu_port;
    int port_ndx;
    int has_cpu_port = FALSE;

    rv = bcm_port_config_get(unit, &config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_ITER(config.cpu, port_ndx)
    {
        has_cpu_port = TRUE;
        rv = bcm_port_get(unit, port_ndx, &flags, &interface_info, &mapping_info);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_port_get() : %s\n", bcm_errmsg(rv));
            return rv;
        }
        tm_port = mapping_info.tm_port;
        core = mapping_info.core;
        cpu_channel = mapping_info.channel;
        break;
    }
    if (!has_cpu_port)
    {
        printf("\nNo cpu port\n");
        return BCM_E_INTERNAL;
    }

    rv = bcm_stk_gport_sysport_get(unit, out_port, &sysport_gport);
    if(BCM_FAILURE(rv)) {
        printf("\nError bcm_stk_gport_sysport_get() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\n KNET Tx Wire Injection:\n");
    printf("PTCH.SSP %u(0x%x)\n", tm_port, tm_port);
    printf("CPU Channel %u(0x%x)\n", cpu_channel, cpu_channel);
    printf("IMTH.Destination %d(0x%x)\n", (sysport_gport & 0xffff), (sysport_gport & 0xffff));

    return rv;
}

bcm_error_t
knet_egress_pp_injection_info_dump(int unit)
{
    return knet_tx_wire_injection_info_dump(unit);
}

int knet_trav_netif(
    int unit,
    bcm_knet_netif_t *netif,
    void *user_data)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_knet_netif_destroy(unit, netif->id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
};

int knet_trav_filter(
    int unit,
    bcm_knet_filter_t *filter,
    void *user_data)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_knet_filter_destroy(unit, filter->id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_destroy() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
};

bcm_error_t
knet_clear(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    printf("Delete existed KNET interfaces\n");
    rv = bcm_knet_netif_traverse(unit, knet_trav_netif, NULL);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_traverse() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Delete existed KNET filters\n");
    rv = bcm_knet_filter_traverse(unit, knet_trav_filter, NULL);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_traverse() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

bcm_error_t
test_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Forward traffic from in_port to out_port*/
    rv = bcm_port_force_forward_set(unit, in_port, out_port, 1);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_force_forward_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


bcm_error_t
test_clear(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Forward traffic from in_port to out_port*/
    rv = bcm_port_force_forward_set(unit, in_port, out_port, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_force_forward_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t knet_interface_example_cpu_ingress(int unit)
{
    bcm_error_t rv;
    bcm_knet_netif_t netif;
    bcm_knet_filter_t filter;
    const bcm_mac_t local_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE };
    const char *device_name = "virt-intf0";

    if (BCM_FAILURE((rv = test_clear(unit)))) {
        printf("test_clear() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* Clear KNET setup */
    if (BCM_FAILURE((rv = knet_clear(unit)))) {
        printf("knet_clear() failed.\n");
        return -1;
    }

    printf("Create a KNET interface in CPU Ingress mode\n");
    /* Send packets through ingress logic */
    bcm_knet_netif_t_init(&netif);
    netif.type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
    netif.flags = BCM_KNET_NETIF_F_ADD_TAG;
    netif.port = in_port;
    netif.vlan = 1;
    /* Set device name */
    sal_strcpy(netif.name, device_name);
    /* Set MAC address associated with this interface */
    netif.mac_addr = local_mac;
    rv = bcm_knet_netif_create(unit, &netif);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_netif_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create a KNET filter\n");
    /* Add filter to catch source port */
    bcm_knet_filter_t_init(&filter);
    sal_strcpy(filter.desc, "MatchSPA");
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    /*filter1.flags = BCM_KNET_FILTER_F_STRIP_TAG;*/
    filter.priority = 50;
    /* Send packet to network interface */
    filter.dest_type = BCM_KNET_DEST_T_NETIF;
    filter.dest_id = netif.id;
    filter.match_flags = BCM_KNET_FILTER_M_INGPORT;
    filter.m_ingport = in_port;
    rv = bcm_knet_filter_create(unit, &filter);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    knet_rx_port_injection_info_dump(unit);
    /* Provided steps to user for sending and receiving packets through knet interface */
    printf("\nUser has to follow below steps to verify KNET functionality\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx in backround \"./knet_rx -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx to send 5 packets \"./knet_tx -vv virt-intf0 5\" \n");
    printf("=====================================================================================\n");
    printf("Above 4 steps will demonstrate transmit and receive functionality over KNET interface\n");

    return BCM_E_NONE;
}

bcm_error_t knet_interface_example_local_port(int unit)
{
    bcm_error_t rv;
    bcm_knet_netif_t netif;
    bcm_knet_filter_t filter;
    const bcm_mac_t local_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE };
    const char *device_name = "virt-intf0";

    if (BCM_FAILURE((rv = test_clear(unit)))) {
        printf("test_clear() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* Clear KNET setup */
    if (BCM_FAILURE((rv = knet_clear(unit)))) {
        printf("knet_clear() failed.\n");
        return -1;
    }

    printf("Create a KNET interface in Local port mode\n");
    /* Send packets to local port */
    bcm_knet_netif_t_init(&netif);
    netif.type = BCM_KNET_NETIF_T_TX_LOCAL_PORT;
    netif.flags = BCM_KNET_NETIF_F_ADD_TAG;
    netif.port = out_port;
    netif.vlan = 1;
    /* Set device name */
    sal_strcpy(netif.name, device_name);
    /* Set MAC address associated with this interface */
    netif.mac_addr = local_mac;
    rv = bcm_knet_netif_create(unit, &netif);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_netif_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add default filter to KNET interface */
    printf("Create a KNET filter\n");
    bcm_knet_filter_t_init(&filter);
    sal_strcpy(filter.desc, "DefaultToNetif");
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    filter.priority = 51;
    /* Send packet to network interface */
    filter.dest_type = BCM_KNET_DEST_T_NETIF;
    filter.dest_id = netif.id;
    rv = bcm_knet_filter_create(unit, &filter);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    knet_tx_wire_injection_info_dump(unit);
    /* Provided steps to user for sending and receiving packets through knet interface */
    printf("\nUser has to follow below steps to verify KNET functionality\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx in backround \"./knet_rx -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx to send 5 packets \"./knet_tx -vv virt-intf0 5\" \n");
    printf("=====================================================================================\n");
    printf("Above 4 steps will demonstrate transmit and receive functionality over KNET interface\n");

    return BCM_E_NONE;
}

bcm_error_t knet_interface_example_rcpu(int unit)
{
    bcm_error_t rv;
    bcm_knet_netif_t netif;
    bcm_knet_filter_t filter1;
    bcm_knet_filter_t filter2;
    const bcm_mac_t local_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE };
    const char *device_name = "virt-intf0";

    if (BCM_FAILURE((rv = test_clear(unit)))) {
        printf("test_clear() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* Clear KNET setup */
    if (BCM_FAILURE((rv = knet_clear(unit)))) {
        printf("knet_clear() failed.\n");
        return -1;
    }

    printf("Create a KNET interface in RCPU mode\n");
    bcm_knet_netif_t_init(&netif);
    netif.type = BCM_KNET_NETIF_T_TX_META_DATA;
    /* Set RCPU mode */
    netif.flags = BCM_KNET_NETIF_F_RCPU_ENCAP;
    /* Set device name */
    sal_strcpy(netif.name, device_name);
    /* Set MAC address associated with this interface */
    netif.mac_addr = local_mac;
    rv = bcm_knet_netif_create(unit, &netif);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_netif_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create KNET filters\n");
    /* Add filter to catch source port */
    bcm_knet_filter_t_init(&filter1);
    sal_strcpy(filter1.desc, "MatchSPA");
    filter1.type = BCM_KNET_FILTER_T_RX_PKT;
    /*filter1.flags = BCM_KNET_FILTER_F_STRIP_TAG;*/
    filter1.priority = 50;
    /* Send packet to network interface */
    filter1.dest_type = BCM_KNET_DEST_T_NETIF;
    filter1.dest_id = netif.id;
    filter1.match_flags = BCM_KNET_FILTER_M_INGPORT;
    filter1.m_ingport = in_port;
    rv = bcm_knet_filter_create(unit, &filter1);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add default filter to KNET interface */
    bcm_knet_filter_t_init(&filter2);
    sal_strcpy(filter2.desc, "DefaultToNetif");
    filter2.type = BCM_KNET_FILTER_T_RX_PKT;
    filter2.priority = 51;
    /* Send packet to network interface */
    filter2.dest_type = BCM_KNET_DEST_T_NETIF;
    filter2.dest_id = netif.id;
    rv = bcm_knet_filter_create(unit, &filter2);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Rx Port Injection */
    knet_rx_port_injection_info_dump(unit);
    printf("\nRx Port Injection:\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx in backround \"./knet_rx -r -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx to send 5 packets \"./knet_tx -vv virt-intf0 5 -r -m RxPort -ssp 200 -channel 8\" \n");
    printf("=====================================================================================\n");

    /* Tx Wire Injection */
    knet_tx_wire_injection_info_dump(unit);
    printf("\nTx Wire Injection\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx in backround \"./knet_rx -r -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx to send 5 packets \"./knet_tx -vv virt-intf0 5 -r -m TxWire -dest 202 -channel 0 -ssp 0\" \n");
    printf("=====================================================================================\n");

    /* Egress PP Injection */
    knet_egress_pp_injection_info_dump(unit);
    printf("\nEgress PP Injection:\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx in backround \"./knet_rx -r -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx to send 5 packets \"./knet_tx -vv virt-intf0 5 -r -m EgressPP -dest 202 -channel 0 -ssp 0\" \n");
    printf("=====================================================================================\n");

    return BCM_E_NONE;
}
