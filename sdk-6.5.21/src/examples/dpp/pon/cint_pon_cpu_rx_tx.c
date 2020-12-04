/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
* 
* File: cint_pon_cpu_rx_tx.c
*
* Section: pon_cpu_rx
* Purpose: examples of rx trap management BCM API. 
*     Trap PPPOE/DHCP/IGMP/MLD package to CPU, it'll print receive packet on the 
*     command line with the tunnel id by pon_cpu_rx_turnel_id.
*     Trap DHCP/IGMP/MLD are implemented by the bcm_switch_control_port_set BCM API.
*     Trap PPPOE is implemented by the PMF which the bcm_switch_control_port_set  
*     BCM API don't support PPPOE.   
*     You can use pon_cpu_rx_dhcp/igmp/mld/pppoe to enable/disable the specific trap. 
*     PPPOE trap types:
*         0 -> PPPOE Discovery(0x8863)
*         1 -> PPPOE Session(0x8864)
*     DHCP trap types:
*         0 -> bcmRxTrapDhcpv4Client 
*         1 -> bcmRxTrapDhcpv4Server 
*         2 -> bcmRxTrapDhcpv6Client
*         3 -> bcmRxTrapDhcpv6Server
*     IGMP trap types:
*         0 -> bcmRxTrapIgmpMembershipQuery
*         1 -> bcmRxTrapIgmpReportLeaveMsg
*     MLD trap types:
*         0 -> bcmRxTrapIcmpv6MldMcListenerQuery
*         1 -> bcmRxTrapIcmpv6MldReportDone
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/cint_port_tpid.c
*     BCM> cint examples/dpp/pon/cint_pon_utils.c
*     BCM> cint examples/dpp/pon/cint_pon_cpu_rx_tx.c
*     BCM> cint
*     cint> pon_cpu_service_Init(0, 4, 128, 129);
*     cint> pon_cpu_service_setup(0);
*     cint> pon_cpu_rx_init(0,5,1,128,1);
*     cint> pon_cpu_rx_pppoe(0,5,0,1);
*     cint> pon_cpu_rx_dhcp(0,0,1);
*     cint> pon_cpu_rx_igmp(0,0,1);
*     cint> pon_cpu_rx_mld(0,0,1);
*     cint> pon_cpu_rx_pppoe(0,5,0,0);
*     cint> pon_cpu_rx_dhcp(0,0,0);
*     cint> pon_cpu_rx_igmp(0,0,0);
*     cint> pon_cpu_rx_mld(0,0,0);
*     cint> pon_cpu_service_cleanup(0);
*
* Section: pon_cpu_tx
* Purpose: examples of tx management BCM API. 
*     CPU send traffic using header type INJECTED_2 which the header is DATAoITMHoPTCH.
*     PTCH indicates the PP source port that we are going to transmit.
*     ITMH indicates the packet destination. 
*     It support sending the traffic on PON/NNI port by pon_cpu_tx_port.
*     It support sending the traffic on NNI port(Trunk mode) by pon_cpu_tx_trunk.
*
* Trunk configurations:
*     type = 0: 1K SPAs with 16 members.
*     type = 1: 512 SPAs with 32 members.
*     type = 2: 256 SPAs with 64 members.
*     type = 3: 128 SPAs with 128 members.
*     type = 4: 64 SPAs with 256 members.
*
* SOC property config:  
*     tm_port_header_type_in_0.BCM88650=INJECTED_2  
*     tm_port_header_type_out_0.BCM88650=CPU
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/cint_port_tpid.c
*     BCM> cint examples/dpp/pon/cint_pon_utils.c
*     BCM> cint examples/dpp/pon/cint_pon_cpu_rx_tx.c
*     BCM> cint
*     cint> pon_cpu_service_Init(0, 4, 128, 129);
*     cint> pon_cpu_service_setup(0);
*     cint> pon_cpu_tx_port(5,1,100,20);
*     cint> pon_cpu_tx_trunk(0,0,0,20);
*     cint> pon_cpu_service_cleanup(0);
 */


/*******************************************************************************
* CPU RX SECTION
 */
enum pppoe_type_e {
    pppoe_discovery,
    pppoe_session
};

enum dhcp_type_e {
    dhcp_v4_client,
    dhcp_v4_server,
    dhcp_v6_client,
    dhcp_v6_server
};

enum igmp_type_e {
    igmp_query,
    igmp_report_leave
};

enum mld_type_e {
    mld_query,
    mld_report
};
    
struct bcm_cpu_trap_t {
    int                 id;
    bcm_gport_t         gport;
    bcm_field_group_t   group;
    int                 enable[2];
    bcm_field_entry_t   entry[2];
};

int trap_id_dhcp[4];
int trap_id_igmp[4];
int trap_id_mld[4];
bcm_cpu_trap_t trap_pppoe;


/*******************************************************************************
* CPU rx print
 */
int pon_cpu_rx_print(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int                 idx, i, line_len;
    char                buf[64];
    uint8               *pkt_data;    
    char *packet_ptr, *ch_ptr;    

    if (pkt == NULL)
    {
        return BCM_RX_HANDLED;
    }

    printf("Unit:%d CPU RX'ed a packet on port: %d\n", unit, pkt->src_port);
    packet_ptr = sal_alloc(3 * pkt->pkt_len, "Packet print");
    if (packet_ptr != NULL) 
    {
        ch_ptr = packet_ptr;            
        for(i = 0; i < pkt->pkt_len; i++) 
        {
            if ((i%4) == 0)
            {
                sprintf(ch_ptr," ");
                ch_ptr++;
            }
            if ((i%32) == 0)
            {
                sprintf(ch_ptr,"\n");
                ch_ptr++;
            }
            sprintf(ch_ptr,"%02x", (pkt->_pkt_data.data)[i]); 
            ch_ptr++;
            ch_ptr++;
        }
        sprintf(ch_ptr,"\n");
        printf("%s",packet_ptr);
        sal_free(packet_ptr);
    }

    return BCM_RX_HANDLED;
}


/*******************************************************************************
* CPU rx trap set
 */
int pon_cpu_rx_trap_set(int unit, int type, int trap_strength,int trap_dest, int *trap_id) {   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, type, trap_id);
    printf("created trap %d \n", *trap_id);
    if (rv == BCM_E_RESOURCE) {
        printf("trap already created continue... \n");
    }
    else if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_trap_type_create $rv\n");
    }    
    
    /* configure trap attribute tot update destination */
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = trap_strength;
    trap_config.dest_port = trap_dest;

    rv = bcm_rx_trap_set(unit, *trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned $rv\n");
        return rv;
    }    
    
    return rv;
}


/*******************************************************************************
* CPU rx PPPOE
*     type:
*         0 -> PPPOE Discovery(0x8863)
*         1 -> PPPOE Session(0x8864)
 */
int pon_cpu_rx_pppoe(int unit, int port, int type, int enable)
{
    bcm_error_t rv = BCM_E_NONE;
    int trap_type;
    int other_type;
    int trap_strength = 0;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;

    switch(type)
    { 
      case pppoe_discovery:
        trap_type = 0x8863; /* PPPOE Discovery */
        other_type = 1;
        break;
      case pppoe_session:
        trap_type = 0x8864; /* PPPOE Session */
        other_type = 0;
        break;
      default:
        printf("Type must be < 2\n");
        rv = BCM_E_PARAM;
        return rv;
    }  

    if (enable) {
        /* Check current type also don't enable the PPPOE trap */
        if (!trap_pppoe.enable[type])
        {
            /* Check other type also don't enable the PPPOE trap */
            if (!trap_pppoe.enable[other_type])
            {
                pon_cpu_rx_trap_set(unit,bcmRxTrapUserDefine, trap_strength, BCM_GPORT_LOCAL_CPU, &trap_pppoe.id);	
                BCM_FIELD_QSET_INIT(qset);
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);    
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);        
                BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
                BCM_FIELD_ASET_INIT(aset);
                BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

                rv = bcm_field_group_create(unit, qset, 2, &trap_pppoe.group);
                if (rv != BCM_E_NONE)
                {
                    printf("ERROR:: bcm_field_group_create returned $rv\n");
                    return rv;
                }

                rv = bcm_field_group_action_set(unit, trap_pppoe.group, aset);
                if (rv != BCM_E_NONE)
                {
                    printf("ERROR: bcm_field_group_action_set returned $rv\n");
                    return rv;
                }
                
                BCM_GPORT_TRAP_SET(trap_pppoe.gport, trap_pppoe.id, 7, 0);
            }

            rv = bcm_field_entry_create(unit, trap_pppoe.group, &trap_pppoe.entry[type]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_create returned $rv\n");
                return rv;
            }            

            rv = bcm_field_qualify_InPort(unit, trap_pppoe.entry[type], port, 0xffffffff); 
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_qualify_InPort returned $rv\n");
                return rv;
            }

            rv = bcm_field_qualify_EtherType(unit, trap_pppoe.entry[type], trap_type, 0xffff);        
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_qualify_Ethertype returned $rv\n");
                return rv;
            }

            rv = bcm_field_action_add(unit, trap_pppoe.entry[type], bcmFieldActionTrap, trap_pppoe.gport, 0);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_action_add returned $rv\n");
                return rv;
            }

            rv = bcm_field_entry_install(unit, trap_pppoe.entry[type]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_install returned $rv\n");
                return rv;
            }

            trap_pppoe.enable[type] = 1;
        }
        
    } else {
        /* Check current type that has enabled the PPPOE trap */
        if (trap_pppoe.enable[type])
        {        
            rv = bcm_field_entry_remove(unit, trap_pppoe.entry[type]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_remove returned $rv\n");
                return rv;
            }

            rv = bcm_field_entry_destroy(unit, trap_pppoe.entry[type]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_destroy returned $rv\n");
                return rv;
            }   

            trap_pppoe.enable[type] = 0;

            /* Check other type that hasn't enabled the PPPOE trap */
            if (!trap_pppoe.enable[other_type])
            {
                rv = bcm_rx_trap_type_destroy(unit, trap_pppoe.id);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_rx_trap_type_destroy with vlan $rv\n");
                    return rv;
                }
            	rv = bcm_field_group_destroy(unit, trap_pppoe.group);
            	if (rv != BCM_E_NONE)
            	{
            		printf("ERROR: bcm_field_group_destroy returned $rv\n");
            		return rv;
            	}
            }
        }
    }

    return rv;
}


/*******************************************************************************
* CPU rx DHCP
*     type:
*         0 -> bcmRxTrapDhcpv4Client 
*         1 -> bcmRxTrapDhcpv4Server 
*         2 -> bcmRxTrapDhcpv6Client
*         3 -> bcmRxTrapDhcpv6Server
 */
int pon_cpu_rx_dhcp(int unit, int type, int enable){
    bcm_error_t rv = BCM_E_NONE;
    int trap_type;
    int trap_strength = 7;
    
    switch(type)
    { 
      case dhcp_v4_client:
        trap_type = bcmRxTrapDhcpv4Client;
        break;
      case dhcp_v4_server:
        trap_type = bcmRxTrapDhcpv4Server;
        break;
      case dhcp_v6_client:
        trap_type = bcmRxTrapDhcpv6Client;
        break;
      case dhcp_v6_server:
        trap_type = bcmRxTrapDhcpv6Server;
        break;
      default:
        printf("Type must be < 4\n");
        rv = BCM_E_PARAM;
        return rv;
    }      
    
    if (enable) {   	    
        rv = pon_cpu_rx_trap_set(unit, trap_type, trap_strength, BCM_GPORT_LOCAL_CPU, &trap_id_dhcp[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapDhcp, %d $rv\n", type);
            return rv;
        }	   
    } else {    
        rv = bcm_rx_trap_type_destroy(unit, trap_id_dhcp[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapDhcp, %d $rv\n", type);
            return rv;
        }    
    } 	     
    
    return rv;
}


/*******************************************************************************
* CPU rx IGMP
*     type:
*         0 -> bcmRxTrapIgmpMembershipQuery
*         1 -> bcmRxTrapIgmpReportLeaveMsg
 */
int pon_cpu_rx_igmp(int unit, int type, int enable){
    bcm_error_t rv = BCM_E_NONE;
    int trap_type;
    int trap_strength = 7;
    
    switch(type)
    { 
      case igmp_query:
        trap_type = bcmRxTrapIgmpMembershipQuery;
        break;
      case igmp_report_leave:
        trap_type = bcmRxTrapIgmpReportLeaveMsg;
        break;
      default:
        printf("Type must be < 2\n");
        rv = BCM_E_PARAM;
        return rv;
    }      
    
    if (enable) {   	    
        rv = pon_cpu_rx_trap_set(unit, trap_type, trap_strength, BCM_GPORT_LOCAL_CPU, &trap_id_igmp[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapIgmp, %d $rv\n", type);
            return rv;
        }	   
    } else {    
        rv = bcm_rx_trap_type_destroy(unit, trap_id_igmp[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapIgmp, %d $rv\n", type);
            return rv;
        }    
    } 	     
    
    return rv;
}


/*******************************************************************************
* CPU rx MLD
*     type:
*         0 -> bcmRxTrapIcmpv6MldMcListenerQuery
*         1 -> bcmRxTrapIcmpv6MldReportDone
 */
int pon_cpu_rx_mld(int unit, int type, int enable){
    bcm_error_t rv = BCM_E_NONE;
    int trap_type;
    int trap_strength = 7;
    
    switch(type)
    { 
      case mld_query:
        trap_type = bcmRxTrapIcmpv6MldMcListenerQuery;
        break;
      case mld_report:
        trap_type = bcmRxTrapIcmpv6MldReportDone;
        break;
      default:
        printf("Type must be < 2\n");
        rv = BCM_E_PARAM;
        return rv;
    }      
    
    if (enable) {   	    
        rv = pon_cpu_rx_trap_set(unit, trap_type, trap_strength, BCM_GPORT_LOCAL_CPU, &trap_id_mld[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapMld, %d $rv\n", type);
            return rv;
        }	   
    } else {    
        rv = bcm_rx_trap_type_destroy(unit, trap_id_mld[type]);
        if (rv != BCM_E_NONE) {
            printf("Error, setting trap: bcmRxTrapMld, %d $rv\n", type);
            return rv;
        }    
    } 
    
    return rv;
}


/*******************************************************************************
* CPU rx init for DHCP,IGMP and MLD.
 */
int pon_cpu_rx_init(int unit, int pon_port, int pon_enable, int nni_port, int nni_enable)
{        
    bcm_error_t rv = BCM_E_NONE;   
    
    rv = bcm_rx_register(unit, "Protocol Trap", pon_cpu_rx_print, 50, NULL, BCM_RCO_F_INTR|BCM_RCO_F_ALL_COS);
    if (BCM_FAILURE(rv)) {
        printf("Error: bcm_rx_register returned $rv\n");
        return rv;
    } 
	
	  if (pon_enable != 0) {	
	    /* DHCP */
	    rv = bcm_switch_control_port_set( unit, pon_port, bcmSwitchDhcpPktToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }		  
        /* IGMP Query */
        rv = bcm_switch_control_port_set( unit, pon_port, bcmSwitchIgmpQueryToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
        /* IGMP Report and Leave */
        rv = bcm_switch_control_port_set( unit, pon_port, bcmSwitchIgmpReportLeaveToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
        /* MLD */
    	rv = bcm_switch_control_port_set( unit, pon_port, bcmSwitchMldPktToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
    }
  
    if (nni_enable != 0) {	
        /* DHCP */
        rv = bcm_switch_control_port_set( unit, nni_port, bcmSwitchDhcpPktToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }		  
        /* IGMP Query */
        rv = bcm_switch_control_port_set( unit, nni_port, bcmSwitchIgmpQueryToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
        /* IGMP Report and Leave */
        rv = bcm_switch_control_port_set( unit, nni_port, bcmSwitchIgmpReportLeaveToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
        /* MLD */
    	rv = bcm_switch_control_port_set( unit, nni_port, bcmSwitchMldPktToCpu, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set $rv\n");
            return rv;
        }
    }
  
    return rv;
}


/*******************************************************************************
* CPU tx print
 */
int pon_cpu_tx_print (uint8 * pkt_data, int length) 
{
    int i;
    int line_len = 16;
    
    printf ("\n packet dump for length = %d\n", length);
        
    for (i=0; i<length; i++) {
        if (i % line_len == 0) {
            if (i !=0) printf("\n");
                printf("%04x  ", i);
        }
        printf ("%02x ", (pkt_data[i]) & 0xff);
    }
    
    printf ("\n\n");
    return 0;
}


/*******************************************************************************
* CPU tx on port
 */
int pon_cpu_tx_port(bcm_port_t port, int is_pon, bcm_tunnel_id_t tunnel, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    int unit = 0;
    int pkt_flags; 
    int pkt_len = 128;     
    int pkt_header_len = 0;    
    uint8 pkt_header[64];     
    uint8 pkt_tunnel[4];    
    uint8 pkt_tagged_data[64] = {    	               
    	               0x00, 0x01, 0x02, 0x03, 0x04, 0x05, /* DA */
    	               0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, /* SA */
                     0x81, 0x00, 0x00, 0x01, /* TPID + VID */
                     0x08, 0x00, 0x45, 0x10, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x32, 0x06,
                     0x82, 0xab, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
                     0x03, 0x04, 0x00, 0x04, 0x01, 0x00, 0x56, 0xf1, 0x39, 0x00, 0x00,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  
    uint8 pkt_buf[128];  
    bcm_gport_t sysport = 0;

    pkt_flags = BCM_TX_CRC_ALLOC;
    
    bcm_pkt_t *tx_pkt;
    rv = bcm_pkt_alloc(unit,pkt_len, pkt_flags, &tx_pkt);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_pkt_alloc failed $rv\n");
        return rv;
    }

    rv = bcm_stk_gport_sysport_get(unit, port, &sysport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_gport_sysport_get failed $rv\n");
        return rv;
    }

    void * cookie = (void *)1;
    tx_pkt->call_back = 0;
    tx_pkt->blk_count = 1;
    tx_pkt->unit = 0;
    
    /* Dune TM header */
    tx_pkt->_dpp_hdr[3] = 0x00; /*chanel num*/
    tx_pkt->_dpp_hdr[2] = 0x00;
    tx_pkt->_dpp_hdr[1] = 0x00;
    tx_pkt->_dpp_hdr[0] = 0x00;
	
    tx_pkt->_dpp_hdr_type = 1;  /* DPP_HDR_itmh_base */
    
    /* PTCH */
    pkt_header[pkt_header_len++] = 0x50;
    pkt_header[pkt_header_len++] = 0;
    /* ITMH */       
    pkt_header[pkt_header_len++] = 0x00;
    pkt_header[pkt_header_len++] = 0x0c;
    pkt_header[pkt_header_len++] = 00;
    pkt_header[pkt_header_len++] = (sysport&0xff);

    pkt_len =  pkt_header_len + 64;
    /* VID */
    if (is_pon) {
    	pkt_tunnel[0] = 0x81; 
      pkt_tunnel[1] = 0x00; 
      pkt_tunnel[2] = (tunnel&0xff00) >> 8; 
      pkt_tunnel[3] = (tunnel&0x00ff);
    }      
    pkt_tagged_data[14] = (vlan&0xff00) >> 8; 
    pkt_tagged_data[15] = (vlan&0x00ff); 
       
    sal_memset((auto)pkt_buf, 0, 128);   
    sal_memcpy((auto)pkt_buf, (auto)pkt_header, pkt_header_len);    
    if (is_pon) {
      sal_memcpy((auto)&pkt_buf[pkt_header_len], (auto)pkt_tagged_data, 12);
      sal_memcpy((auto)&pkt_buf[pkt_header_len + 12], (auto)pkt_tunnel, 4);
      sal_memcpy((auto)&pkt_buf[pkt_header_len + 16], (auto)pkt_tagged_data + 12, 52);
      pkt_len = pkt_header_len + 64 + 4;
    } else {
      sal_memcpy((auto)&pkt_buf[pkt_header_len], (auto)pkt_tagged_data, 64);
      pkt_len = pkt_header_len + 64;
    }     
    bcm_pkt_memcpy (tx_pkt, 0, (auto)pkt_buf, pkt_len);
    
    rv = bcm_tx (unit, tx_pkt, cookie);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tx failed $rv\n");
        return rv;
    }
        
    printf ("packet tx: %d\n", rv);
    pon_cpu_tx_print ((auto)pkt_buf, pkt_len);

    rv = bcm_pkt_free(unit, tx_pkt);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tx failed $rv\n");
        return rv;
    }

    return rv;
}


/*******************************************************************************
* CPU tx on trunk port
 */
void pon_cpu_tx_trunk(int trunk_type, int trunk, int trunk_member, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    int unit = 0;
    int pkt_flags; 
    int pkt_len = 128;     
    int pkt_header_len = 0;    
    uint8 pkt_header[64]; 
    uint8 pkt_tagged_data[64] = {    	               
    	               0x00, 0x01, 0x02, 0x03, 0x04, 0x05, /* DA */
    	               0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, /* SA */
                     0x81, 0x00, 0x00, 0x01, /* TPID + VID */
                     0x08, 0x00, 0x45, 0x10, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x32, 0x06,
                     0x82, 0xab, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
                     0x03, 0x04, 0x00, 0x04, 0x01, 0x00, 0x56, 0xf1, 0x39, 0x00, 0x00,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66};  
    uint8 pkt_buf[128];  

    pkt_flags = BCM_TX_CRC_ALLOC;
    
    bcm_pkt_t *tx_pkt;
    rv = bcm_pkt_alloc(unit,pkt_len, pkt_flags, &tx_pkt);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_pkt_alloc failed $rv\n");
        return rv;
    }
	
    void * cookie = (void *)1;
    tx_pkt->call_back = 0;
    tx_pkt->blk_count = 1;
    tx_pkt->unit = 0;
    
    /* Dune TM header */
    tx_pkt->_dpp_hdr[3] = 0x00; /*chanel num*/
    tx_pkt->_dpp_hdr[2] = 0x00;
    tx_pkt->_dpp_hdr[1] = 0x00;
    tx_pkt->_dpp_hdr[0] = 0x00;
	
    tx_pkt->_dpp_hdr_type = 1;  /* DPP_HDR_itmh_base */
    
    /* PTCH */
    pkt_header[pkt_header_len++] = 0x50;
    pkt_header[pkt_header_len++] = 0;
    /* ITMH */       
    pkt_header[pkt_header_len++] = 0x00;
    pkt_header[pkt_header_len++] = 0x0C;
    switch(trunk_type)
    {
      case 0:
    		pkt_header[pkt_header_len++] = 0xC0 | (trunk_member & 0x3c) | ((trunk & 0x300) >> 8);  /* 1K SPAs of 16 members */
            pkt_header[pkt_header_len++] = trunk & 0xff;
    		break;
    		
      case 1:
    		pkt_header[pkt_header_len++] = 0xC0 | (trunk_member & 0x3e) | ((trunk & 0x100) >> 8);  /* 512 SPAs of 32 members */
            pkt_header[pkt_header_len++] = trunk & 0xff;
    		break;
      
      case 2:
    		pkt_header[pkt_header_len++] = 0xC0 | (trunk_member & 0x3f);  /* 256 SPAs of 64 members */
            pkt_header[pkt_header_len++] = trunk & 0xff;
    		break;
    		
      case 3:
    		pkt_header[pkt_header_len++] = 0xC0 | ((trunk_member & 0xfe) >> 1);  /* 128 SPAs of 128 members */
            pkt_header[pkt_header_len++] = ((trunk_member & 0x1) << 7) | (trunk & 0x7f);
    		break;
    		
      case 4:
    		pkt_header[pkt_header_len++] = 0xC0 | ((trunk_member & 0xfc) >> 2);  /* 64SPAs of 512 members */
            pkt_header[pkt_header_len++] = ((trunk_member & 0x3) << 6) | (trunk & 0x3f);
    		break;
    }    
         
    pkt_len =  pkt_header_len + 64;
    /* VID */         
    pkt_tagged_data[14] = (vlan&0xff00) >> 8; 
    pkt_tagged_data[15] = (vlan&0x00ff); 
       
    sal_memset((auto)pkt_buf, 0, 128);   
    sal_memcpy((auto)pkt_buf, (auto)pkt_header, pkt_header_len);     
    sal_memcpy((auto)&pkt_buf[pkt_header_len], (auto)pkt_tagged_data, 64);
    pkt_len = pkt_header_len + 64;
    
    bcm_pkt_memcpy (tx_pkt, 0, (auto)pkt_buf, pkt_len);
    
    rv = bcm_tx (unit, tx_pkt, cookie);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tx failed $rv\n");
        return rv;
    }
        
    printf ("packet tx: %d\n", rv);
    pon_cpu_tx_print ((auto)pkt_buf, pkt_len);

    rv = bcm_pkt_free(unit, tx_pkt);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_pkt_free failed $rv\n");
        return rv;
    }

    return rv;
}

/*******************************************************************************
*
*  PP model
 */
struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport[2];
};

bcm_port_t pon_port;
bcm_port_t nni_ports[2];
bcm_gport_t trunk_gport;
bcm_vlan_t tls_service_vsi;
int num_of_tls_services = 2;
pon_service_info_s pon_tls_service_info[2];

/*******************************************************************************
* PP model Init                                                   
 */
int pon_cpu_service_Init(int unit, bcm_port_t port_pon, bcm_port_t port_nni_1, bcm_port_t port_nni_2)
{
    bcm_error_t rv = BCM_E_NONE;  
    
    /* PON Port 4 Tunnel-ID 101 any tag <-> VSI <-> NNI Port 128 any tag.*/
    pon_tls_service_info[0].service_mode = transparent_all_tags;
    pon_tls_service_info[0].tunnel_id    = 100;
    pon_tls_service_info[0].up_lif_type = match_all_tags;
    pon_tls_service_info[0].down_lif_type = match_all_tags;

    /* PON Port 4 Tunnel-ID 102 any tag <-> VSI <-> NNI Port 128 any tag.*/
    pon_tls_service_info[1].service_mode = transparent_all_tags;
    pon_tls_service_info[1].tunnel_id    = 101;
    pon_tls_service_info[1].up_lif_type = match_all_tags;
    pon_tls_service_info[1].down_lif_type = match_all_tags;

    pon_port = port_pon;
    nni_ports[0] = port_nni_1;
    nni_ports[1] = port_nni_2;
    
    rv = pon_app_init(unit, port_pon, port_nni_1, port_nni_2, 1);
    if (rv != BCM_E_NONE) {
        printf("pon_app_init failed $rv\n");
        return rv;
    } 
    BCM_GPORT_TRUNK_SET(trunk_gport, nni_trunk_id);
    return rv;
}

/*******************************************************************************
* PP model setup                                                   
 */
int pon_cpu_service_setup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;  
    int index, j;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;
    int voq_queue_id;
    bcm_gport_t gport_voq_group;

    rv = vswitch_create(unit, &tls_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("vswitch_create failed $rv\n");
        return rv;
    }

    for (index = 0; index < num_of_tls_services; index++)    
    {
        pon_gport = 0;
        service_mode = pon_tls_service_info[index].service_mode;        
        tunnel_id    = pon_tls_service_info[index].tunnel_id;        
        pon_lif_type = pon_tls_service_info[index].up_lif_type;        
        up_ovlan     = pon_tls_service_info[index].up_ovlan;        
        up_ivlan     = pon_tls_service_info[index].up_ivlan;        
        up_pcp       = pon_tls_service_info[index].up_pcp;        
        up_ethertype = pon_tls_service_info[index].up_ethertype;        
        nni_lif_type = pon_tls_service_info[index].down_lif_type;        
        down_ovlan   = pon_tls_service_info[index].down_ovlan;        
        down_ivlan   = pon_tls_service_info[index].down_ivlan; 
        /* Create PON LIF */        
        rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport); 
        if (rv != BCM_E_NONE) {
            printf("pon_lif_create failed (%d) $rv\n", index);
            return rv;
        }
        /* Add PON LIF to vswitch */        
        rv = bcm_vswitch_port_add(unit, tls_service_vsi, pon_gport);        
        if (rv != BCM_E_NONE) {
            printf("bcm_vswitch_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /*For downstream*/
        rv = multicast_vlan_port_add(unit, tls_service_vsi+lif_offset, pon_port, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("multicast_vlan_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF ingress VLAN editor */       
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL); 
        if (rv != BCM_E_NONE) {
            printf("pon_port_ingress_vt_set failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF egress VLAN editor */        
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);   
        if (rv != BCM_E_NONE) {
            printf("pon_port_egress_vt_set failed (%d) $rv\n", index);
            return rv;
        }

        if (index == 0) 
        {
            /* Create LIFs for all ports in trunk */
            for (j = 0; j < 2; j++) {
                /* Create NNI LIF */
                nni_gport = 0;
                rv = nni_lif_create(unit, nni_ports[j], nni_lif_type, 0, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
                if (rv != BCM_E_NONE) {
                    printf("nni_lif_create failed (%d) $rv\n", index);
                    return rv;
                }

                /* add NNI LIF to vswitch */            
                rv = bcm_vswitch_port_add(unit, tls_service_vsi, nni_gport);            
                if (rv != BCM_E_NONE) {
                    printf("bcm_vswitch_port_add failed (%d) $rv\n", index);
                    return rv;
                }            
                /*For upstream*/            
                rv = multicast_vlan_port_add(unit, tls_service_vsi, nni_ports[j], nni_gport); 
                if (rv != BCM_E_NONE) {
                    printf("multicast_vlan_port_add failed (%d) $rv\n", index);
                    return rv;
                }

                /* Set the multicast group offset to flood downstream packets in multicast group(tls_service_vsi+lif_offset) */
                bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
                bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
                bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);
                pon_tls_service_info[index].nni_gport[j] = nni_gport;
            }
        }
        
        pon_tls_service_info[index].pon_gport = pon_gport;        
    }
    
    return rv;
}


/*******************************************************************************
* PP model cleanup                                                   
 */
int pon_cpu_service_cleanup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;
    int index, j;

    for (index = 0; index < num_of_tls_services; index++)
    {
        rv = vswitch_delete_port(unit, tls_service_vsi, pon_tls_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("vswitch_delete_port failed (%d) $rv\n", index);
            return rv;
        }

        if (index == 0) 
        {
            for (j = 0; j < 2; j++) {
                rv = vswitch_delete_port(unit, tls_service_vsi, pon_tls_service_info[index].nni_gport[j]);
                if (rv != BCM_E_NONE) {
                    printf("vswitch_delete_port failed (%d) $rv\n", index);
                    return rv;
                }
            }
        }
    }    
    
    rv = vswitch_delete(unit, tls_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("vswitch_delete failed $rv\n");
        return rv;
    }
    
    return rv;
}

