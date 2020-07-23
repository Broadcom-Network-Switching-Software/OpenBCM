/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Purpose: examples of trap management BCM API
 * To Activate Following Settings Run:     
 * BCM> cint examples/dpp/cint_trap_mgmt.c
 *
 * Examples:
 *     index 0 - Trap packets with SA==DA
 *                   using reserved trap code.
 *     index 1 - Trap packets with specific MAC address 
 *                   using user defined trap code.
 *
 * Steps:
 * 1. Get trap handle per trap type
 *     Call bcm_rx_trap_type_create()
 * 2. Set trap config
 *     Call bcm_rx_trap_set()
 * Extra steps for l2 frame with specific MAC address
 * 3. Set trap type gport with trap code, snoop and trap strength.
 *     Call BCM_GPORT_TRAP_SET
 * 4. Add a static mac address entry with vlan_id, mac address
 *     and destination port (trap type gport).  
 *     Call bcm_l2_addr_add()
 * 5. Destroy a trap type by trap handle
 *     Call bcm_rx_trap_type_destroy()
 *
 * Traffics:
 * 1. (0x5, 0x5, 0x8100, 1) for SA==DA
 * 2. (0xff, 0x5, 0x8100, 1) for l2 frame trap
 */

/* Array to keep existing trap ids. Supports 4 units each with 1024 trap types.
   index (unit * 1024 + trap_type) will keep the existing trap id of specific unit and type */
int trap_ids[4096];
sal_memset(trap_ids,-1,4096*4);

struct trap_mgmt_info_s
{
    bcm_rx_trap_t type; /* trap management type*/
    int trap_id; /* trap id*/
	int snoop_id; /*snoop command*/
};

static trap_mgmt_info_s trap_mgmt_info[2];

void 
trap_mgmt_type_init() {
    trap_mgmt_info[0].type = bcmRxTrapSaEqualsDa;
	trap_mgmt_info[1].type = bcmRxTrapUserDefine;
}

int 
trap_management_snoop_set(int unit, int index, int dest_port) {   
    int rv = BCM_E_NONE;
    bcm_rx_snoop_config_t snoop_config;
    int snoop_cmd;
    int flags = 0;
    int idx;

    if ((index < 0) || (index > 2)) {
        rv = BCM_E_PARAM; 
		return rv; 
    }
    /*Creat snoop handler*/
    rv = bcm_rx_snoop_create(unit,flags,&snoop_cmd);
    printf("bcm_rx_snoop_create, rv %d, snoop cmd %d \n", rv, snoop_cmd);
    if (rv != BCM_E_NONE) {
	    printf("Error in: bcm_rx_snoop_create rv %d\n",rv);
	    return rv;
    }

    trap_mgmt_info[index].snoop_id = snoop_cmd;

    sal_memset(&snoop_config, 0, sizeof(snoop_config));
    snoop_config.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_REPLACE);
    snoop_config.dest_port = dest_port;
    snoop_config.size = 64;
    snoop_config.probability = 100000;

    rv = bcm_rx_snoop_set(unit, snoop_cmd, &snoop_config);
    if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_snoop_set rv %d\n",rv);
        return rv;
    }

    return rv;
}


int 
trap_management_trap_set(int unit, int index, int trap_strength, int snoop_strength) {   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t type;
    int trap_id;
    int idx;

    if ((index < 0) || (index > 2)) {
        rv = BCM_E_PARAM; 
		return rv; 
    }

    if ((trap_strength < 0) || (trap_strength > 8)) {
        rv = BCM_E_PARAM; 
		return rv; 
    }

    type = trap_mgmt_info[index].type;
    rv = bcm_rx_trap_type_create(unit, 0, type, &trap_id);
    printf("bcm_rx_trap_type_create, rv %d, trap id %d \n", rv, trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_trap_type_create rv %d\n", rv);
        return rv;
    }
    trap_mgmt_info[index].trap_id = trap_id;
    
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = trap_strength;

    /*1<= snoop_strength<=3 for setting snoop command*/
    if ((snoop_strength >= 0)&&(snoop_strength <= 3)) {
	    trap_config.snoop_cmnd = trap_mgmt_info[index].snoop_id;	
        trap_config.snoop_strength = snoop_strength;
	}

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned %d\n", rv);
        return rv;
    }

    return rv;
}



/*
 * configure trap to trap packet to given destination 
 * trap_dest: trap destination, can be
 *   - CPU: use BCM_GPORT_LOCAL_CPU
 *   - Drop:  BCM_GPORT_BLACK_HOLE,
 *   - any other gport
 */
int
trap_type_set(int unit, int type, int trap_strength,int trap_dest) {   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    int trap_id;

    /* create trap (if not already exist) */
	if (trap_ids[unit*1024+type] >= 0) {
        trap_id = trap_ids[unit*1024+type];
	} else {
        rv = bcm_rx_trap_type_create(unit, 0, type, &trap_id);
        trap_ids[unit*1024+type] = trap_id;
        printf("created trap %d \n", trap_id);
        if (rv == BCM_E_RESOURCE) {
            printf("trap already created continue... \n");
        }
        else if (rv != BCM_E_NONE) {
            printf("Error in: bcm_rx_trap_type_create rv %d\n", rv);
        }
    }
    
    /* configure trap attribute tot update destination */
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = trap_strength;
    trap_config.dest_port = trap_dest;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned %d\n", rv);
        return rv;
    }

    return rv;
}



int
trap_mac_addr(int unit, int gport, bcm_mac_t mac, bcm_vlan_t vlan){
    int rv;
    bcm_l2_addr_t l2_addr;
      
    bcm_l2_addr_t_init(&l2_addr,mac,vlan);
    /* static mac address entry */
    l2_addr.flags = 0x00000020;
    l2_addr.port = gport;
    rv = bcm_l2_addr_add(unit,&l2_addr);
	
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_l2_addr_add returned %d\n", rv);
        return rv;
    }
	
    return rv;
}

int 
trap_mgmt_run(int unit) {
    bcm_mac_t mac;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    int rv;

    /*Initialize trap types*/
    trap_mgmt_type_init();

    /*for bcmRxTrapSaEqualsDa*/
    rv = trap_management_trap_set(unit, 0, 7, -1);
    if (rv != BCM_E_NONE) {
        printf("Error in : trap_management_trap_set for sa==da, returned %d\n", rv);
        return rv;
    }
	
    /*for bcmRxTrapUserDefine*/
    trap_management_trap_set(unit, 1, 0, -1);
    if (rv != BCM_E_NONE) {
        printf("Error in : trap_management_trap_set for user defined trap, returned %d\n", rv);
        return rv;
    }
		
    /*Trap specific mac address to cpu*/
    BCM_GPORT_TRAP_SET(gport, trap_mgmt_info[1].trap_id, 7, 0);
    printf("trap gport 0x%x \n", gport);
    mac[5] = 0xff;
    vlan = 1;
    rv = trap_mac_addr(unit, gport, mac, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error in : trap_mac_addr returned %d\n", rv);
        return rv;
    }
	

    return rv;
}

int
trap_mgmt_stop(int unit) {
    bcm_mac_t mac;
	int rv;

    /*delete trap for bcmRxTrapSaEqualsDa*/
	rv = bcm_rx_trap_type_destroy(unit,trap_mgmt_info[0].trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_type_destroy, index 0, returned %d\n", rv);
        return rv;
    }

    /*delete static mac for trap*/
    mac[5] = 0xff;
    rv = bcm_l2_addr_delete_by_mac(unit, mac, BCM_L2_DELETE_STATIC);
	
    /*for bcmRxTrapUserDefine*/
	rv = bcm_rx_trap_type_destroy(unit,trap_mgmt_info[1].trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_type_destroy, index 1, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Sets the trap to be used in the AT_Cint_non_ip_packet DVAPI
 */
int
run_with_non_ip_packet_dvapi(int unit, int port) {
    int rv;
    rv = trap_type_set(unit, bcmRxTrapMyMacAndUnknownL3, 7, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap_type_set\n");
    }
    return rv;
}

/*
 * Destroys the trap that was used in the AT_Cint_non_ip_packet DVAPI for cleanup.
 */
int
non_ip_packet_dvapi_restore(int unit, int port){
    int rv;
    rv = trap_type_set(unit, bcmRxTrapMyMacAndUnknownL3, 0, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap_type_set\n");
    }
    return rv;
}
