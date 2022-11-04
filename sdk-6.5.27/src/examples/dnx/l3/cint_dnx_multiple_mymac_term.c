/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Multiple mymac mode allows l2 terminating all l3 protocol packets, according to the mac address.
 *
 * Use bcm_l2_station_add, as shown in multiple_mymac_termination_example.
 *
 * To run sample script:
 * cd ../../../src/examples/dnx
 * cint cint_dnx_ip_route_basic.c
 * cint cint_dnx_l3_vrrp.c
 * cint cint_dnx_multiple_mymac_term.c
 * cint
 * multiple_mymac_termination_example(<unit>, <in_port>, <out_port>);
 */


 /*
 *  IPV4 packet to send from in_port:
 *  - vlan = in_vlan (= 50).
 *  - DA = {0x00, 0x00, 0x6b, 0x6b, 0x01, 0x75}
 *  - IPV4 DIP = 0x7fffff00 (127.255.255.0)
 *  packet will be routed to out_port
 *  there will be termination (although DA is not MyMac)
 *
 * Any l3 protocol packet to send from in_port:
 *  - vlan = in_vlan (= 50).
 *  - DA =  {0x00, 0x00, 0x6b, 0x6b, 0x08, 0x75}
 *
 *  Packets will be routed according to the mapping in create_traffic_mapping
 *
 */
int multiple_mymac_termination_station_id[2] = {0,0};

int multiple_mymac_termination_example(int unit, int in_port, int out_port){
    int rv;
    int station_id;
    bcm_l2_station_t station;
    bcm_mac_t mutliple_mac = {0x00, 0x00, 0x6b, 0x6b, 0x01, 0x75};
    bcm_mac_t mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int in_vlan = 50;

    rv = create_traffic_mapping(unit, in_port, out_port, in_vlan, 0);
    if (rv != BCM_E_NONE) {
       printf("Error, in create_traffic_mapping");
       return rv;
    }

    bcm_l2_station_t_init(&station);
    sal_memcpy(station.dst_mac, mutliple_mac, 6);
    sal_memcpy(station.dst_mac_mask, mac_mask, 6);
    station.flags = BCM_L2_STATION_IPV4;
    station.vlan = in_vlan;
    station.vlan_mask = 0xffff;
    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_station_add\n");
        return rv;
    }

    if (verbose) {
        printf("Done creating first l2_station. station_id=0x%x\n", station_id);
        print station;
    }
    multiple_mymac_termination_station_id[0] = station_id;

    station.dst_mac[4] = 0x08;
    station.flags = 0;

    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_station_add\n");
        return rv;
    }

    if (verbose) {
        printf("Done creating second l2_station. station_id=0x%x\n", station_id);
        print station;
    }
    multiple_mymac_termination_station_id[1] = station_id;

    return rv;
}

/*
 * multiple_mymac_termination_destroy
 *
 * Given a unit, unsets the mymac.
 */
int
multiple_mymac_termination_destroy(int unit){
    int rv = BCM_E_NONE, i, station_id;

    for (i = 0 ; i < 2 ; i++) {
        station_id = multiple_mymac_termination_station_id[i];
        if (station_id != 0) {
            rv = bcm_l2_station_delete(unit, station_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_l2_station_delete. station_id=0x%x\n", station_id);
            }
            multiple_mymac_termination_station_id[i] =0;
        }
    }

    return rv;
}


