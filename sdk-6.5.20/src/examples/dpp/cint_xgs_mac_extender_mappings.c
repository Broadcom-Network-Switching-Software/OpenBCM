/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_xgs_mac_extender_mappings.c
 * Purpose: Provide an example of MAC extender application between ARAD and XGS devices.
 * In this system ARAD is in device-mode PP and connected to several XGS devices usually as 1G Port extedner.
 * CINT provides an example of setting for ARAD
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |        ----------------         ----------------                                              |
 *  |        | XGS Modid 0  |         | ARAD         |                                              |
 *  |        |              |         |   Modid 0    |                                              |
 *  |XGS Port|0             |         |1             |                                              |
 *  |        |1             |   HG    |21            |                                              |
 *  |        |2             |---------|22            |                                              |
 *  |        |3             |         |23            |                                              |
 *  |        |.             |         |              |                                              |
 *  |        |.             |         |              |                                              |
 *  |        |9             |         |28            |                                              |
 *  |        ---------------|         |              |                                              |
 *  |        ----------------         |              |                                              |
 *  |        | XGS Modid 1  |         |              |                                              |
 *  |        |              |         |              |                                              |
 *  |XGS Port|0             |         |2             |                                              |
 *  |        |1             |   HG    |31            |                                              |
 *  |        |2             |---------|32            |                                              |
 *  |        |3             |         |33            |                                              |
 *  |        |.             |         |.             |                                              |
 *  |        |.             |         |.             |                                              |
 *  |        |9             |         |38            |                                              |
 *  |        ---------------|         ---------------|                                              |
 *  |                                                                                               |
 *  |                                                                                               |
 *  |                               +~~~~~~~~~~~~~~~~~~~~~~~~~~+                                    |
 *  |                               | Figure : XGS MAC extender|                                    |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                                                                                  
 * Explanation:                                                                             
 *  *   ARAD is used as PP device with connectivity to 1G Port Extender XGS Devices.
 *  *   Connectivity is done using HG headers and Logical channelized interface.      
 *  *   Referring to figure above:
 *      * Each XGS Device (Modid0,Modid1) has 10 ports
 *      * Each Device is connected in one interface to ARAD
 *      * Packets between XGS<->ARAD are passed with additional HG header
 *      * When packet goes from XGS to BCM86xx packet includes HG header with HG.SRC_PORT and HG.SRC_MODID
 *        to indicate which XGS Port the packet came from.      
 *      * When packet goes from BCM86xx to XGS packet includes HG header with HG.DST_PORT and HG.DST_MODID
 *        to indicate which XGS Port the packet goes to.
 *      * All PP capability are done in ARAD, 1G extender devices only responsible on passing traffic to correct 1G port.
 *      * Same-interface and LAG resolution are done in BCM86XX.
 *      * ARAD can have ports that are connected to XGS MAC extender and other ports in the system (Regular Ethernet ports,
 *        CPU, Recycle and so on...).
 *    
 *  Notes:
 *      * ARAD first port in each HG interface (channel 0) must be between 0-15.
 *      Example: In figure above there are two interfaces. Each interface ARAD first local port (channel 0)
 *      Must be between 0-15. In figure above first interface ARAD local port channel 0 is 1. Second interface
 *      ARAD local port channel 0 is 2.
 * 
 *  Main settings for ARAD:
 *  1. Configure ports in the device to be XGS MAC extender
 *  2. Set for each XGS.Port, XGS.Modid its ARAD local port
 *  3. Usual TM and PP settings regardless of XGS HG interface between ARAD and XGS.
 * 
 * 
 *  How-to-run:
 *  Define or replace the following SOC properties than the default Negev application:
 *   - ucode_port_0=CPU.0
 *   - ucode_port_1=XLGE0.0
 *   - ucode_port_21=XLGE0.1
 *   - ucode_port_22=XLGE0.2
 *   - ucode_port_23=XLGE0.3
 *   - ucode_port_24=XLGE0.4
 *   - ucode_port_25=XLGE0.5
 *   - ucode_port_26=XLGE0.6
 *   - ucode_port_27=XLGE0.7
 *   - ucode_port_28=XLGE0.8
 *   - ucode_port_29=XLGE0.9
 *   - ucode_port_2=XLGE1.0
 *   - ucode_port_31=XLGE1.1
 *   - ucode_port_32=XLGE1.2
 *   - ucode_port_33=XLGE1.3
 *   - ucode_port_34=XLGE1.4
 *   - ucode_port_35=XLGE1.5
 *   - ucode_port_36=XLGE1.6
 *   - ucode_port_37=XLGE1.7
 *   - ucode_port_38=XLGE1.8
 *   - ucode_port_39=XLGE1.9
 *   - diag_disable=1
 *   - tm_port_header_type_in_0.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_out_0.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_in_1.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_out_1.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_in_21.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_21.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_22.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_22.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_23.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_23.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_24.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_24.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_25.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_25.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_26.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_26.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_27.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_27.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_28.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_28.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_29.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_29.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_2.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_out_2.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_in_31.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_31.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_32.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_32.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_33.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_33.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_34.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_34.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_35.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_35.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_36.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_36.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_37.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_37.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_38.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_38.BCM88650=XGS_MAC_EXT
*    - tm_port_header_type_in_39.BCM88650=XGS_MAC_EXT 
*    - tm_port_header_type_out_39.BCM88650=XGS_MAC_EXT
*    - port_priorities.BCM88650=2 
* 
* 
*    - cint ../../../../src/examples/dpp/cint_xgs_mac_extender_mappings.c 
*    - cint ../../../../src/examples/dpp/cint_mact.c 
*    - cint 
*    - int unit = 0; 
*    - xgs_mac_extender_main(unit); 
*    - example_new_vlan_entries(unit,1,2,31,1); 
* 
*    Example traffic: Send traffic from XLGE1 
*    HG.SRC_PORT = 0, HG.SRC_MODID = 1. 
*    Ethernet: DA 00:00:00:00:00:ab VLAN=1 
*    Expect: 
*    Unicast Packet will be send to from Src-port 2 to Destination-port 31. 
*    No changes to Ethernet packet. 
*    HG.DST_PORT = 1, HG.DST_MODID=1 
*    All other PP applications can work as usual. 
* 
*    Note: Assuming the application is bring-up with the default Negev application (appl_dpp). 
*    In this example, we try to simplify the settings so it is done after the default Negev application is up. 
*    It is recommended to set XGS MAC extender mappings before setting system ports in the system. 
*/

/* 
 * Decide if port is XGS MAC extender
 */
int xgs_mac_extender_is_xgs_mac_extender_port(int unit, int port)
{
  /* 
   * Example have static decision. Other way is to decide according to given soc properties header type. 
   * See appl_dpp_is_xgs_mac_extender_port in src/appl/diag/dcmn/init.c 
   */
  if (port == 1 || port == 2 || (port >= 21 && port <= 29) || (port >= 31 && port <= 39)) {
      return 1;
  }

  return 0;
}

/* 
 * Static mapping between port to its remote modid and remote port
 */
void xgs_mac_extender_convert_port_to_remote_modid_port(int unit, int local_port, int* remote_modid, int* remote_port)
{
    int tmp = 0;

    if (local_port == 1 || (local_port >= 21 && local_port <= 29))  {
      *remote_modid = 0;      
    } else {
      *remote_modid = 1;
    }

    if (local_port == 1) {      
      tmp = 1;      
    } else if (local_port == 2) {
      tmp = 2;
    }
    *remote_port = (local_port - tmp) % 10;
    
}

/* This function create system ports of the system assuming one device */
int
stk_init(int unit, int mymodid)
{
    int rv = BCM_E_NONE;
    int idx, port, sys_port, intern_idx, modid, index;
    bcm_gport_t internal_gport[80];
    bcm_gport_t modport_gport,sysport_gport;
    int int_flags,dummy;

    /* Set modid */
    rv = bcm_stk_my_modid_set(unit, mymodid);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_my_modid_set failed. Error:%d (%s)\n",  
                 rv, bcm_errmsg(rv));
        return rv;
    }

    /* 
     * Creation of system ports in the system 
     * Iterate over port + internal ports. After the iteration, num_ports will
     *  be incremented by internal_num_ports 
     * Assuming each device has the same num ports + internal ports  
     */    
    for (modid = 0; modid < (g_info.nof_devices); modid++) {       
        for (idx = 0; idx < g_info.num_ports; idx++) {          
            port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
            /* System port id depends on modid + port id */ 
            sys_port = convert_modport_to_sysport_id(modid,port);    
                
            BCM_GPORT_MODPORT_SET(modport_gport, modid, port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);

            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, 
                                               modport_gport);
            if (BCM_FAILURE(rv)) {
               printf("bcm_stk_sysport_gport_set failed. Error:%d (%s)\n",  
               rv, bcm_errmsg(rv));
               return rv;
            }
        }
    }

    /* Now set up internal ports ERP/OLP/OAMP */
    /* Get number of internal ports */
    /* Assuming all devices have the same amount internal ports */    
    int_flags =
        BCM_PORT_INTERNAL_EGRESS_REPLICATION |
        BCM_PORT_INTERNAL_OAMP |
        BCM_PORT_INTERNAL_OLP;
    rv = bcm_port_internal_get(unit, int_flags, 80,
                   internal_gport, &dummy);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_internal_get failed. Error:%d (%s)\n",  
               rv, bcm_errmsg(rv));
        return rv;
    }
    
    for (index = 0; index < dummy; ++index) {
        
        if (BCM_GPORT_IS_LOCAL(internal_gport[index])) {
        port = BCM_GPORT_LOCAL_GET(internal_gport[index]);        
        } else if (BCM_GPORT_IS_MODPORT(internal_gport[index])) {
        port = BCM_GPORT_MODPORT_PORT_GET(internal_gport[index]);        
        } else {
         return BCM_E_PARAM;
        }
        
        /* System port id depends on modid + port id */ 
        sys_port = convert_modport_to_sysport_id(modid, port);                                  
        BCM_GPORT_MODPORT_SET(modport_gport, modid, port);
        BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);

        rv = bcm_stk_sysport_gport_set(unit, sysport_gport, 
                       modport_gport);
        if (BCM_FAILURE(rv)) {
           printf("bcm_stk_sysport_gport_set failed. Error:%d (%s)\n",  
                   rv, bcm_errmsg(rv));
            return rv;
        }
    }
        

    rv = bcm_stk_module_enable(unit, mymodid, g_info.num_ports+g_info.num_internal_ports, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_petra_stk_module_enable failed. Error:%d (%s)\n",  
                 rv, bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}

int xgs_mac_extender_main(int unit)
{
  int idx, port, remote_modid, remote_port;
  bcm_error_t rv;
  bcm_port_config_t port_config;
  bcm_port_t port_i;
  bcm_pbmp_t pbmp;

  /* 
   * All XGS MAC extender ports, set mappings according to: 
   * Port 1, 21-29 Modid 0 Port 0-10 
   * Port 2, 31-39 Modid 1 Port 0-10
   */
  rv = bcm_port_config_get(unit, &port_config);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_port_config_get $rv \n");
    return rv;
  }    
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_OR(pbmp, port_config.all);
  BCM_PBMP_REMOVE(pbmp, port_config.sfi);  

  BCM_PBMP_ITER(pbmp, port_i) {        
    if (xgs_mac_extender_is_xgs_mac_extender_port(unit, port_i)) {
        xgs_mac_extender_convert_port_to_remote_modid_port(unit, port_i, &remote_modid, &remote_port);

        rv = bcm_stk_modport_remote_map_set(unit, port_i, 0,
                                         remote_modid, remote_port);
        if (BCM_FAILURE(rv)) {
            printf("bcm_stk_modport_remote_map_set: failed. Error:%d (%s) \n", 
                     rv, bcm_errmsg(rv));
            return rv;
        }
    }
  }

  return 0;
}
