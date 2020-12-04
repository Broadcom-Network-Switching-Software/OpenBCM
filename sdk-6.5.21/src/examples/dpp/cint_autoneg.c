/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_autoneg.c
* Purpose: An example of auto-negotiation configuration and status check. 
*          The following CINT provides a calling sequence example to execute auto-negotiation on a pair of ports. 
*          The port can be 1-lane, 2-lane or 4-lane port.
* The nogotiated ability in the below table
*AN_Mode   |Ability	   |Speed_Mode	       | Line_Rate (Mb/s)
*--------------------------------------------------------
*CL73	    |FD_Speed	   |100GBASE-CR10       | 100000
*		    |                 |40GBASE-CR4	       | 40000
*		    |                 |40GBASE-KR4	       | 40000
*		    |                 |10GBASE_KX4	       | 10000
*		    |                 |10GBASE_KR	       | 10000
*		    |                 |1000M_KX              | 1000
*	          |FEC		    |                           |
*	          |Pause	    |                           |
*--------------------------------------------------------
*CL73BAM   |FD_Speed	    |20GBASE-CR2	       | 20000
*		    |                  |20GBASE-KR2	       | 20000
*--------------------------------------------------------
*CL37	    |Pause	    |                           |
*		    |FD_Speed     |SGMII	              | 1000
*--------------------------------------------------------
*CL37BAM   |FD Speed	     |40GBASE-X4	       | 40000
*		    |                   |32.7GBASE-X4      | 32700
*		    |                   |31.5GBASE-X4	 | 31500
*		    |                   |25.455GBASE-X4   | 25455
*		    |                   |21GBASE-X4	       | 21000
*		    |                   |20GBASE-X2_CX4  | 20000
*		    |                   |20GBASE-X2	       | 20000
*		    |                   |20GBASE-X4	       | 20000
*		    |                   |20GBASE-X4_CX4  | 20000
*		    |                   |16GBASE-X4	       | 16000
*		    |                   |15.75GBASE-X2	 | 15750
*		    |                   |15GBASE-X4	       | 15000
*		    |                   |13GBASE-X4	       | 13000
*		    |                   |12.7GBASE-X2      | 12700
*		    |                   |12.5GBASE-X4      | 12500
*		    |                   |12GBASE-X4         | 12000
*		    |                   |10.5GBASE-X2       | 10500
*		    |                   |10GBASE-X2_CX4   | 10000
*		    |                   |10GBASE-X2	        | 10000
*		    |                   |10GBASE-X4_CX4   | 10000
*		    |                   |10GBASE-X4          | 10000
*		    |                   |6GBASE-X4	        | 6000
*		    |                   |5GBASE-X4	        | 5000
*		    |                   |2.5GBASE-X	        | 2500	
*		    | FEC             |                   	   | 	
*--------------------------------------------------------
*SGMII	    | FD_Speed     |SGMII	               | 1000
*		    |                   |SGMII	               | 100
*	          |                   |SGMII	               | 10
*/

struct autoneg_port_pair_s{
    int unit_1;
    int unit_2;
    bcm_port_t port_1;
    bcm_port_t port_2;
};

struct autoneg_expected_result_s{
    int speed;
    unsigned int pause;
    unsigned int fec;
};

struct autoneg_info_pair_s{
    bcm_port_ability_t ability[2];
    autoneg_expected_result_s expected_result[2];
    phymod_an_mode_type_t an_mode;
};


/*
* Restart auto-negotiation on this pair of ports with the specified mode
*/
int autoneg_port_pair_restart(autoneg_port_pair_s port_pair, phymod_an_mode_type_t an_mode)
{
    int rv;
    phymod_autoneg_control_t an_control;

    sal_memset(&an_control, 0, sizeof(an_control));
    an_control.an_mode = an_mode;
    /* disable autoneg on this pair of ports */
    rv = bcm_port_autoneg_set(port_pair.unit_1, port_pair.port_1, 0);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_autoneg_set(port_pair.unit_2, port_pair.port_2, 0);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg get fail\n", port_pair.port_2);
        return rv;
    }

    /* enable autoneg on this pair of ports with an_mode */
    rv = bcm_port_autoneg_set(port_pair.unit_1, port_pair.port_1, 1);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_autoneg_set(port_pair.unit_2, port_pair.port_2, 1);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg set fail\n", port_pair.port_2);
        return rv;
    }       

    return rv;
}


/*
* Check auto-negotiation resutl on this pair of ports
*     1, Check auto-negotiation status;
*     2, Check link status;
*     3, Check the negotiated speed;
*     4, Check if the advertised pause ability is recevied correctly by the link partner;
*     5, Check if the advertised FEC ability is recevied correctly by the link partner;
*/
int autoneg_port_pair_result_check(autoneg_port_pair_s port_pair, autoneg_info_pair_s info_pair)
{
    int rv;
    int link_status, speed;
    bcm_port_ability_t ability;
    phymod_autoneg_status_t an_status;

    sal_memset(&ability, 0, sizeof(ability));
    sal_memset(&an_status, 0, sizeof(an_status));

    /* check autoneg status */
    /*rv = portmod_port_autoneg_status_get(port_pair.unit_1, port_pair.port_1, an_status);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg status get fail\n", port_pair.port_1);
        return rv;
    }
    if((1 != an_status.enabled) || (1 != an_status.locked)){
        printf("port %d autoneg not completed, enable=%d, locked=%d\n", \
            port_pair.port_1, an_status.enabled, an_status.locked);
        return BCM_E_FAIL;
    }
    rv = portmod_port_autoneg_status_get(port_pair.unit_2, port_pair.port_2, an_status);
    if (rv != BCM_E_NONE) {
        printf("port %d autoneg status get fail\n", port_pair.port_2);
        return rv;
    }
    if((1 != an_status.enabled) || (1 != an_status.locked)){
        printf("port %d autoneg not completed, enable=%d, locked=%d\n", \
            port_pair.port_1, an_status.enabled, an_status.locked);
        return BCM_E_FAIL;
    }*/

    /* check link status */
    rv = bcm_port_link_status_get(port_pair.unit_1, port_pair.port_1, link_status);
    rv = bcm_port_link_status_get(port_pair.unit_1, port_pair.port_1, link_status);
    if (rv != BCM_E_NONE) {
        printf("port %d link status get fail\n", port_pair.port_1);
        return rv;
    }
    if(1 != link_status){
        printf("port %d link status is down\n", port_pair.port_1);
        return BCM_E_FAIL;
    }
    rv = bcm_port_link_status_get(port_pair.unit_2, port_pair.port_2, link_status);
    rv = bcm_port_link_status_get(port_pair.unit_2, port_pair.port_2, link_status);
    if (rv != BCM_E_NONE) {
        printf("port %d link status get fail\n", port_pair.port_2);
        return rv;
    }
    if(1 != link_status){
        printf("port %d link status is down\n", port_pair.port_2);
        return BCM_E_FAIL;
    }
    
    /* check speed on this pair of ports */
    rv = bcm_port_speed_get(port_pair.unit_1, port_pair.port_1, &speed);
    if (rv != BCM_E_NONE) {
        printf("port %d link speed get fail\n", port_pair.port_1);
        return rv;
    }
    if(info_pair.expected_result[0].speed != speed){
        printf("port %d speed autoneg fail, expected speed is %d and actual speed is %d\n", \
            port_pair.port_1, info_pair.expected_result[0].speed, speed);
        return BCM_E_FAIL;
    }
    rv = bcm_port_speed_get(port_pair.unit_2, port_pair.port_2, &speed);
    if (rv != BCM_E_NONE) {
        printf("port %d link speed get fail\n", port_pair.port_2);
        return rv;
    }
    if(info_pair.expected_result[1].speed != speed){
        printf("port %d speed autoneg fail, expected speed is %d and actual speed is %d\n", \
            port_pair.port_2, info_pair.expected_result[1].speed, speed);
        return BCM_E_FAIL;
    }
    /* check FEC and pause on this pair of ports */
    /*rv = bcm_port_ability_remote_get(port_pair.unit_1, port_pair.port_1, &ability);
    if (rv != BCM_E_NONE) {
        printf("port %d remote ability get fail\n", port_pair.port_1);
        return rv;
    }
    if(info_pair.expected_result[0].pause != ability.pause){
        printf("port %d pause autoneg fail, expected pause is %d and actual pause is %d\n", \
            port_pair.port_1, info_pair.expected_result[0].pause, ability.pause);
        return BCM_E_FAIL;
    }
    if((phymod_AN_MODE_CL73 == info_pair.an_mode) || (phymod_AN_MODE_CL37BAM== info_pair.an_mode)){
        if(info_pair.expected_result[0].fec != ability.fec){
            printf("port %d fec autoneg fail, expected fec is %d and actual fec is %d\n", \
                port_pair.port_1, info_pair.expected_result.fec[0], ability.fec);
            return BCM_E_FAIL;
        }
    }
    rv = bcm_port_ability_remote_get(port_pair.unit_2, port_pair.port_2, &ability);
    if (rv != BCM_E_NONE) {
        printf("port %d remote ability get fail\n", port_pair.port_2);
        return rv;
    }
    if(info_pair.expected_result[1].pause != ability.pause){
        printf("port %d pause autoneg fail, expected pause is %d and actual pause is %d\n", \
            port_pair.port_2, info_pair.expected_result.pause[1], ability.pause);
        return BCM_E_FAIL;
    }
    if((phymod_AN_MODE_CL73 == info_pair.an_mode) || (phymod_AN_MODE_CL37BAM == info_pair.an_mode)){
        if(info_pair.expected_result[1].fec != ability.fec){
            printf("port %d fec autoneg fail, expected fec is %d and actual fec is %d\n", \
                port_pair.port_2, info_pair.expected_result.fec[1], ability.fec);
            return BCM_E_FAIL;
        }
    }*/

    return rv;
}


/*
* Set up 100G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 40G and 100G on this pair of ports;;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 10G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_100G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair, 
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<24) | (1<<26); /*40G and 100G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<7);
    info_pair->ability[1].speed_full_duplex = (1<<24) | (1<<26);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<7);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 100000; /* 100G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 100000; /* 100G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 50G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 50G and 40G on this pair of ports;;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 10G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_50G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair, 
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<24) | (1<<3); /*40G and 50G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<7);
    info_pair->ability[1].speed_full_duplex = (1<<24) | (1<<3);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<7);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 50000; /* 50G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 50000; /* 50G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 40G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 25G and 40G on this pair of ports;;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 10G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_40G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair, 
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<22) | (1<<24); /*25G and 40G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<5);
    info_pair->ability[1].speed_full_duplex = (1<<22) | (1<<24);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<5);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 40000; /* 40G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 40000; /* 40G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 25G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 25G and 40G on port_1, and the advertised speed of 25G on port_2;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 25G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_25G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair,
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<22) | (1<<24); /*25G and 40G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<5);
    info_pair->ability[1].speed_full_duplex = (1<<22);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<5);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 25000; /* 25G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 25000; /* 25G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 20G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 20G and 40G on port_1, and the advertised speed of 25G on port_2;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 20G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_20G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair,
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<18) | (1<<24); /*20G and 40G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<5);
    info_pair->ability[1].speed_full_duplex = (1<<18);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<5);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 20000; /* 20G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 20000; /* 20G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 10G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 1G and 10G on this pair of ports;;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 10G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_10G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair,
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<6) | (1<<11); /*1G and 10G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<5);
    info_pair->ability[1].speed_full_duplex = (1<<6) | (1<<11);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<5);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 10000; /* 10G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 10000; /* 10G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* Set up 1G speed of advertised ability on a pair of ports with one specified auto-negotiation mode
*     1, Set the advertised speed of 1G and 10G on port_1, and the advertised speed of 1G on port_2;
*     2, Set the advertised pause of rx and tx on port_1, and the advertised pause of none on port_2;
*     3, Set the advertised FEC of requested and supported on port_1, and the advertised FEC of none on port_2;
*     4, Set the expected speed of 1G on this pair of ports;
*     5, Set the expected pause of rx and tx on port_2, and the expected pause of none on port_1;
*     6, Set the expected FEC of requested and supported on port_2, and the expected FEC of none on port_1;
*/
int autoneg_ability_1G_setup(autoneg_port_pair_s port_pair, autoneg_info_pair_s *info_pair,
    phymod_an_mode_type_t an_mode)
{
    int rv;

    info_pair->an_mode = an_mode;
    /* set ability on this pair of ports */
    info_pair->ability[0].speed_full_duplex = (1<<6) | (1<<11); /*1G and 10G */
    info_pair->ability[0].fec= (1<<0) | (1<<1); /*FEC supported and requested */
    info_pair->ability[0].pause= (1<<0) | (1<<1); /*pause rx and tx */
    info_pair->ability[0].medium= (1<<1);
    info_pair->ability[0].interface = (1<<5);
    info_pair->ability[1].speed_full_duplex = (1<<6);
    info_pair->ability[1].fec= 0;
    info_pair->ability[1].pause= 0;
    info_pair->ability[1].medium= (1<<1);
    info_pair->ability[1].interface = (1<<5);
    /* set the expected result on this pair of ports */
    info_pair->expected_result[0].speed = 1000; /* 1G */
    info_pair->expected_result[0].pause = 0;
    info_pair->expected_result[0].fec = 0;
    info_pair->expected_result[1].speed = 1000; /* 1G */
    info_pair->expected_result[1].pause = (1<<0) | (1<<1);
    if(phymod_AN_MODE_CL73 == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0) | (1<<1);
    }
    else if(phymod_AN_MODE_CL37BAM == info_pair->an_mode){
        info_pair->expected_result[1].fec = (1<<0);
    }

    rv = bcm_port_ability_advert_set(port_pair.unit_1, port_pair.port_1, &info_pair->ability[0]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_1);
        return rv;
    }
    rv = bcm_port_ability_advert_set(port_pair.unit_2, port_pair.port_2, &info_pair->ability[1]);
    if (rv != BCM_E_NONE) {
        printf("port %d ability set fail\n", port_pair.port_2);
        return rv;
    }
    
    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 1-lane port and their max speed is 10G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 1-lane ports, and mode is CL73
*     1, Verify 10G speed auto-negotiation in CL73 mode;
*     2, Verify 1G speed auto-negotiation in CL73 mode;
*     3, Verify pause in all speed sets of CL73 mode;
*     4, Verify fec in all speed sets of CL73 mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73 via below sentence,
*        1) phy_an_c73_#port_1 = 1
*        2) phy_an_c73_#port_2 = 1
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> cl73_autoneg_1xlane_10G_port(unit);
*/
int autoneg_cl73_1xlane_10G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73, speed=10G on 1xlane_10G port###\n");
    rv = autoneg_ability_10G_setup(port_pair, info_pair, phymod_AN_MODE_CL73);  
    if (rv != BCM_E_NONE) {
        printf("10G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL73, speed=1G on 1xlane_10G port###\n");
    rv = autoneg_ability_1G_setup(port_pair, info_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("1G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 1-lane port and their max speed is 10G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 1-lane ports, and mode is CL37
*     1, Verify 1G speed auto-negotiation in CL37 mode;
*     2, Verify pause in all speed sets of CL37 mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL37 via below sentence,
*        1) phy_an_c73_#port_1 = 0
*        2) phy_an_c73_#port_2 = 0
*        3) phy_an_c37_#port_1 = 2
*        4) phy_an_c37_#port_2 = 2
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl37_1xlane_10G_port(unit);
*/
int autoneg_cl37_1xlane_10G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL37, speed=1G on 1xlane_10G port###\n");
    rv = autoneg_ability_1G_setup(port_pair, info_pair, phymod_AN_MODE_CL37); 
    if (rv != BCM_E_NONE) {
        printf("1G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL37);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 4-lane port and their max speed is 40G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 4-lane ports
*     1, Verify 40G speed auto-negotiation in CL73 mode;
*     2, Verify pause and fec in all speed sets of CL73 mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73 via below sentence,
*        1) phy_an_c73_#port_1 = 1
*        2) phy_an_c73_#port_2 = 1
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl73_4xlane_40G_port(unit);
*/
int autoneg_cl73_4xlane_40G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73, speed=40G on 4xlane_40G port###\n");
    rv = autoneg_ability_40G_setup(port_pair, info_pair, phymod_AN_MODE_CL73); 
    if (rv != BCM_E_NONE) {
        printf("40G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 4-lane port and their max speed is 40G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 4-lane ports
*     1, Verify 40G speed auto-negotiation in CL37BAM mode;
*     2, Verify 20G speed auto-negotiation in CL37BAM mode;
*     3, Verify pause and fec in all speed sets of CL37BAM mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL37BAM via below sentence,
*        1) phy_an_c73_#port_1 = 0
*        2) phy_an_c73_#port_2 = 0
*        3) phy_an_c37_#port_1 = 1
*        4) phy_an_c37_#port_2 = 1
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl73_4xlane_40G_port(unit);
*/
int autoneg_cl37bam_4xlane_40G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL37BAM, speed=40G on 4xlane_40G port###\n");
    rv = autoneg_ability_40G_setup(port_pair, info_pair, phymod_AN_MODE_CL37BAM); 
    if (rv != BCM_E_NONE) {
        printf("40G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL37BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL37BAM, speed=25G on 4xlane_40G port###\n");
    rv = autoneg_ability_25G_setup(port_pair, info_pair, phymod_AN_MODE_CL37BAM); 
    if (rv != BCM_E_NONE) {
        printf("25G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL37BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 1-lane port and their max speed is 25G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 1-lane ports, and mode is CL73
*     1, Verify 10G speed auto-negotiation in CL73 mode;
*     2, Verify 1G speed auto-negotiation in CL73 mode;
*     3, Verify pause in all speed sets of CL73 mode;
*     4, Verify fec in all speed sets of CL73 mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73 via below sentence,
*        1) phy_an_c73_#port_1 = 1
*        2) phy_an_c73_#port_2 = 1
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl73_1xlane_25G_port(unit);
*/
int autoneg_cl73_1xlane_25G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73, speed=10G on 1xlane_25G port###\n");
    rv = autoneg_ability_10G_setup(port_pair, info_pair, phymod_AN_MODE_CL73);  
    if (rv != BCM_E_NONE) {
        printf("10G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("autoneg_port_pair_restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("autoneg_port_pair_result_check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL73, speed=1G on 1xlane_25G port###\n");
    rv = autoneg_ability_1G_setup(port_pair, info_pair, phymod_AN_MODE_CL73); 
    if (rv != BCM_E_NONE) {
        printf("1G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 4-lane port and their max speed is 25G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 1-lane ports and mode is CL73BAM
*     1, Verify 20G speed auto-negotiation in CL73BAM mode;
*     2, Verify 25G speed auto-negotiation in CL73BAM mode;
*     3, Verify pause and FEC in all speed sets of CL73BAM mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73BAM via below sentence,
*        1) phy_an_c73_#port_1 = 2
*        2) phy_an_c73_#port_2 = 2
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl37_1xlane_10G_port(unit);
*/
int autoneg_cl73bam_1xlane_25G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73BAM, speed=25G on 1xlane_25G port###\n");
    rv = autoneg_ability_25G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);    
    if (rv != BCM_E_NONE) {
        printf("20G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    /* 25G AN is not supported by jericho */
    /*printf("###negotiating an_mode=CL73BAM, speed=25G on 1xlane_25G port###\n");
    autoneg_ability_25G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);    
    autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    sal_sleep(1);
    autoneg_port_pair_result_check(port_pair, info_pair);*/

    return rv;
}

/*
* unit_1.port_1 and unit_2.port_2 are 2-lane port and their max speed is 50G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 2-lane ports, and mode is CL73BAM
*     1, Verify 20G speed auto-negotiation in CL73BAM mode;
*     2, Verify 40G speed auto-negotiation in CL73BAM mode;
*     3, Verify 50G speed auto-negotiation in CL73BAM mode;
*     4, Verify pause and FEC in all speed sets of CL73BAM mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73BAM via below sentence,
*        1) phy_an_c73_#port_1 = 2
*        2) phy_an_c73_#port_2 = 2
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl37_1xlane_10G_port(unit);
*/
int autoneg_cl73bam_2xlane_50G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73BAM, speed=20G on 2xlane_50G port###\n");
    rv = autoneg_ability_20G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);    
    if (rv != BCM_E_NONE) {
        printf("20G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL73BAM, speed=40G on 2xlane_50G port###\n");
    rv = autoneg_ability_40G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);  
    if (rv != BCM_E_NONE) {
        printf("40G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL73BAM, speed=50G on 2xlane_50G port###\n");
    rv = autoneg_ability_50G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);  
    if (rv != BCM_E_NONE) {
        printf("50G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 4-lane port and their max speed is 100G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 4-lane ports, and mode is CL73
*     1, Verify 40G speed auto-negotiation in CL73 mode;
*     2, Verify 100G speed auto-negotiation in CL73 mode;
*     4, Verify pause and FEC in all speed sets of CL73 mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73 via below sentence,
*        1) phy_an_c73_#port_1 = 2
*        2) phy_an_c73_#port_2 = 2
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl37_1xlane_10G_port(unit);
*/
int autoneg_cl73_4xlane_100G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73, speed=40G on 4xlane_100G port###\n");
    rv = autoneg_ability_40G_setup(port_pair, info_pair, phymod_AN_MODE_CL73);    
    if (rv != BCM_E_NONE) {
        printf("100G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    printf("###negotiating an_mode=CL73, speed=100G on 4xlane_100G port###\n");
    rv = autoneg_ability_100G_setup(port_pair, info_pair, phymod_AN_MODE_CL73); 
    if (rv != BCM_E_NONE) {
        printf("100G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}


/*
* unit_1.port_1 and unit_2.port_2 are 4-lane port and their max speed is 100G
* unit_1.port_1 and unit_2.port_2 are connected with each other
* Auto-negotiation test on one pair of 4-lane ports, and mode is CL73BAM
*     1, Verify 50G speed auto-negotiation in CL73BAM mode;
*     2, Verify pause and FEC in all speed sets of CL73BAM mode;
* To run this test, please follow the below sequence:
*     1, in config file, set auto negotiation mode to CL73BAM via below sentence,
*        1) phy_an_c73_#port_1 = 2
*        2) phy_an_c73_#port_2 = 2
*     2, in BCM shell, execute the below commands,
*        1) BCM> cd $SDK/src/examples/dpp
*        2) BCM> cint cint_autoneg.c
*        3) BCM> cint
*        4) cint> autoneg_cl37_1xlane_10G_port(unit);
*/
int autoneg_cl73bam_4xlane_100G_port(int unit_1, int unit_2, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv;
    autoneg_port_pair_s port_pair;
    autoneg_info_pair_s info_pair;

    sal_memset(&port_pair, 0, sizeof(port_pair));
    sal_memset(&info_pair, 0, sizeof(info_pair));
    port_pair.unit_1 = unit_1;
    port_pair.unit_2 = unit_2;
    port_pair.port_1 = port_1;
    port_pair.port_2 = port_2;
    printf("###negotiating an_mode=CL73BAM, speed=100G on 4xlane_100G port###\n");
    rv = autoneg_ability_100G_setup(port_pair, info_pair, phymod_AN_MODE_CL73BAM);   
    if (rv != BCM_E_NONE) {
        printf("50G ability setup failed\n");
        return rv;
    }
    rv = autoneg_port_pair_restart(port_pair, phymod_AN_MODE_CL73BAM);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation restart failed");
        return rv;
    }
    sal_sleep(1);
    rv = autoneg_port_pair_result_check(port_pair, info_pair);
    if (rv != BCM_E_NONE) {
        printf("auto negotiation result check failed");
        return rv;
    }

    return rv;
}




