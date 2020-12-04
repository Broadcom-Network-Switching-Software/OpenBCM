/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/portmod/portmod.h>
#include <appl/portmod/portmod_reg_access.h>
#include <soc/phy/phymod_sim.h>
#include <soc/drv.h>


#define MAX_PHYS  (140)
#define MAX_PORTS (256)
#define MAX_PMS (10)

/*read SerDes register function*/
STATIC
int bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){
    portmod_default_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = user_acc;
    
    /*place the read register function here*/
    *val = 0 ;
    LOG_VERBOSE(BSL_LS_SOC_PHYMOD,
                (BSL_META_U(user_data->unit,
                            "phy reg read: core_addr=0x0%8x, reg 0x%08x = 0x%04x\n"),
                            core_addr, reg_addr, *val));
    return SOC_E_NONE;
}

/*write SerDes register function*/
STATIC
int bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){
    portmod_default_user_access_t *user_data;

    if(user_acc == NULL){
        return SOC_E_PARAM;
    }
    user_data = user_acc;

    LOG_VERBOSE(BSL_LS_SOC_PHYMOD,
                (BSL_META_U(user_data->unit,
                            "phy reg write: core_addr=0x0%8x, reg 0x%08x = 0x%04x\n"),
                            core_addr, reg_addr, val));
    return SOC_E_NONE;
}


phymod_bus_t phy_bus = {
    "phy_bus",
    bus_read,
    bus_write,
    NULL,
    NULL, /*mutex take*/
    NULL, /*mutex release*/
    NULL,
    NULL,
    0
};

int portmod_pm12x10_add(int unit, portmod_pbmp_t phys, int core_addrs[3], int blk_ids[4], void **user_data)
{
    int rv;
    int i, is_sim;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t *pm4x10_user_data = NULL;
    portmod_pm4x10_create_info_t *pm4x10_info = NULL;
    phymod_access_t *access = NULL;


    portmod_pm_create_info_t_init(unit, &pm_info);
    /*PM add*/
    pm_info.type = portmodDispatchTypePm12x10;
    pm_info.pm_specific_info.pm12x10.blk_id = blk_ids[0]; 
    /*PM phys*/
    PORTMOD_PBMP_ASSIGN(pm_info.phys, phys);
    /*PM phys polarity*/
    *user_data = NULL;
    pm4x10_user_data = sal_alloc(sizeof(portmod_default_user_access_t)*3, "phymod user data");
    *user_data = pm4x10_user_data;
    if(*user_data == NULL){
        return SOC_E_MEMORY;
    }

    for( i = 0 ; i < 3 ; i++)
    {
        pm4x10_info = &pm_info.pm_specific_info.pm12x10.pm4x10_infos[i];    

        /* PM phys polarity - The following configuration is for No polarity inversion */
        pm4x10_info->polarity.rx_polarity = 0;
        pm4x10_info->polarity.tx_polarity = 0;

        /* PM lane map swap - The following configuration is for No lane swap */
        pm4x10_info->lane_map.num_of_lanes = 4;
        pm4x10_info->lane_map.lane_map_rx[0] = 0;
        pm4x10_info->lane_map.lane_map_rx[1] = 1;
        pm4x10_info->lane_map.lane_map_rx[2] = 2;
        pm4x10_info->lane_map.lane_map_rx[3] = 3;
        pm4x10_info->lane_map.lane_map_tx[0] = 0;
        pm4x10_info->lane_map.lane_map_tx[1] = 1;
        pm4x10_info->lane_map.lane_map_tx[2] = 2;
        pm4x10_info->lane_map.lane_map_tx[3] = 3;

        pm4x10_info->ref_clk = phymodRefClk156Mhz;

        /* phymodFirmwareLoadMethodExternal is the fast firmware load, during bringup we will use other options */
        pm4x10_info->fw_load_method = phymodFirmwareLoadMethodExternal;
        pm4x10_info->external_fw_loader = NULL;

        /*phy access*/
        portmod_default_user_access_t_init(unit, &(pm4x10_user_data[i]));
        pm4x10_user_data[i].unit = unit;
        pm4x10_user_data[i].blk_id = blk_ids[i+1];
        access  = &(pm4x10_info->access);
        PHYMOD_ACC_BUS(access) = NULL; /* &phy_bus; */
        PHYMOD_ACC_ADDR(access) = core_addrs[i]; /*core address*/
        PHYMOD_ACC_USER_ACC(access) = &pm4x10_user_data[i]; 

        soc_physim_check_sim(unit, phymodDispatchTypeTsce, &(pm4x10_info->access), 0, &is_sim);
        if(is_sim) {
            pm4x10_info->access.bus->bus_capabilities |= PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL;;
        }
    }

    rv = portmod_port_macro_add(unit, &pm_info);
    if((rv < 0) && (*user_data != NULL)){
        sal_free(*user_data);
        *user_data = NULL;
         return rv;
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "pm 12x10 added!\n")));
    return rv;
}



int portmod_pm4x25_add(int unit, portmod_pbmp_t phys, int core_addr, int first_blk_id, void **user_data)
{
    int rv, is_sim;
    portmod_pm_create_info_t pm_info;
    portmod_default_user_access_t *pm4x25_user_data = NULL;
    portmod_pm4x25_create_info_t *pm4x25_info = NULL;
    phymod_access_t *access = NULL;


    portmod_pm_create_info_t_init(unit, &pm_info);
    /*PM add*/
    pm_info.type = portmodDispatchTypePm4x25;
    /*PM phys*/
    PORTMOD_PBMP_ASSIGN(pm_info.phys, phys);
    /*PM phys polarity*/
    *user_data = NULL;
    pm4x25_user_data = sal_alloc(sizeof(portmod_default_user_access_t), "phymod user data");
    *user_data = pm4x25_user_data;
    if(*user_data == NULL){
        return SOC_E_MEMORY;
    }
    pm4x25_info = &pm_info.pm_specific_info.pm4x25;

    /* PM phys polarity - The following configuration is for No polarity inversion */
    pm4x25_info->polarity.rx_polarity = 0;
    pm4x25_info->polarity.tx_polarity = 0;

    /* PM lane map swap - The following configuration is for No lane swap */
    pm4x25_info->lane_map.num_of_lanes = 4;
    pm4x25_info->lane_map.lane_map_rx[0] = 0;
    pm4x25_info->lane_map.lane_map_rx[1] = 1;
    pm4x25_info->lane_map.lane_map_rx[2] = 2;
    pm4x25_info->lane_map.lane_map_rx[3] = 3;
    pm4x25_info->lane_map.lane_map_tx[0] = 0;
    pm4x25_info->lane_map.lane_map_tx[1] = 1;
    pm4x25_info->lane_map.lane_map_tx[2] = 2;
    pm4x25_info->lane_map.lane_map_tx[3] = 3;

    pm4x25_info->ref_clk = phymodRefClk156Mhz;

    /* phymodFirmwareLoadMethodExternal is the fast firmware load, during bringup we will use other options */
    pm4x25_info->fw_load_method = phymodFirmwareLoadMethodExternal;
    pm4x25_info->external_fw_loader = NULL;
    

    /*phy access*/
    pm4x25_user_data->blk_id = first_blk_id;
    pm4x25_user_data->mutex = NULL;
    pm4x25_user_data->unit = unit;

    access  = &(pm4x25_info->access);
    PHYMOD_ACC_BUS(access) = NULL;
    PHYMOD_ACC_ADDR(access) = core_addr; /*core address*/
    PHYMOD_ACC_USER_ACC(access) = pm4x25_user_data; 

    soc_physim_check_sim(unit, phymodDispatchTypeTscf, &(pm4x25_info->access), 0, &is_sim);
    if(is_sim) {
        pm4x25_info->access.bus->bus_capabilities |= PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL;
    }

    rv = portmod_port_macro_add(unit, &pm_info);
    if((rv < 0) && (*user_data != NULL)){
        sal_free(*user_data);
        *user_data = NULL;
        return rv;
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "pm 4x25 added!\n")));
    return rv;
}


int portmod_appl_init(int unit, void **user_data){
    int rv = 0;
    int i = 0;
    int core_addrs1[]= {0x80, 0x84, 0x88};
    int core_addrs2[]= {0x60, 0x64, 0x68};
    portmod_pm_instances_t pm_types_and_instances[] = {
        {portmodDispatchTypePm12x10, 2},
        {portmodDispatchTypePm4x25, 1}
    };
    portmod_pbmp_t phys;

    /* static values, in this example according to bcm2801pm.c*/
    int pm4x25_blk_id = 2;
    int pm12x10_blk_ids[2][4] = {
        {3, 5, 6, 7}, /* CXX block id, 3xXLAMCs block ids */
        {4, 6, 7, 8} /* CXX block id, 3xXLAMCs block ids */
    };

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nPortMod Create\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "----------------\n")));
   
    rv = portmod_create(unit, 0, MAX_PORTS, MAX_PHYS, 2, pm_types_and_instances);
    if (rv < 0){
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd 1st PM12x10\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "-----------------\n")));

    /*12x10 add*/
    PORTMOD_PBMP_CLEAR(phys);
    for(i = 0 ; i < 12 ; i++ ){
        PORTMOD_PBMP_PORT_ADD(phys, i);
    }
    rv = portmod_pm12x10_add(unit, phys, core_addrs1, pm12x10_blk_ids[0], &user_data[0]);
    if (rv < 0){
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd 2nd PM12x10\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "-----------------\n")));

    /*another 12x10 add*/
    PORTMOD_PBMP_CLEAR(phys);
    for(i = 12 ; i < 24 ; i++ ){
        PORTMOD_PBMP_PORT_ADD(phys, i);
    }
    rv = portmod_pm12x10_add(unit, phys, core_addrs2, pm12x10_blk_ids[1], &user_data[0]);
    if (rv < 0){
        return rv;
    }
 

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd PM4x25\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "------------\n")));

    PORTMOD_PBMP_CLEAR(phys);
    for(i = 24 ; i < 28 ; i++ ){
        PORTMOD_PBMP_PORT_ADD(phys, i);
    }
    rv = portmod_pm4x25_add(unit, phys , 0x8e, pm4x25_blk_id, &user_data[1]);

    return rv;
}

int
portmod_appl_deinit(int unit, void **user_data)
{
    int i = 0;
    int rv;

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nPortMod Destroy\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "-----------------\n")));

    rv  = portmod_destroy(unit);
    for( i = 0 ; i < MAX_PMS; i++){
        if(user_data[i]){
            sal_free(user_data[i]);
        }
    }
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "portmod destroyed!\n")));
    return rv;
}


int
portmod_appl_add_ports(int unit){
    int next_port = 0;
    int rv = 0, phy;
    portmod_port_add_info_t add_info;

    /* Add 12 x 10G ports*/
    for(phy=0 ; phy<12 ; phy++) {

        LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd port %d to 1st PM12x10\n"), phy));
        LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "----------------------------\n")));

        rv = portmod_reg_access_port_bindex_set(unit, next_port, 3, phy);
        if(rv < 0){
            return rv;
        }
        portmod_port_add_info_t_init(unit, &add_info);
        add_info.interface_config.speed = 100000;
        add_info.interface_config.interface = SOC_PORT_IF_XFI;
        add_info.interface_config.interface_modes = 0;
        PORTMOD_PBMP_PORT_ADD(add_info.phys, phy);

        /* For simulation only */
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&add_info);

        rv = portmod_port_add(unit, next_port, &add_info);
        if(rv < 0){
            return rv;
        }
        rv = portmod_port_enable_set(unit, next_port, 0 , 1);
        if(rv < 0){
            return rv;
        }
        next_port++;
    }


    /*3 is the PM 4x25 block in the PM12X10*/
    /* Add 10 lanes CAUI port*/

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd CGE port to 2nd PM12x10\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "-----------------------------\n")));

    rv = portmod_reg_access_port_bindex_set(unit, next_port, 7, 0);
    if(rv < 0){
        return rv;
    }
    portmod_port_add_info_t_init(unit, &add_info);
    add_info.interface_config.speed = 1000000;
    add_info.interface_config.interface = SOC_PORT_IF_CAUI;
    add_info.interface_config.interface_modes = 0;

    for(phy=0 ; phy<10 ; phy++) {
        PORTMOD_PBMP_PORT_ADD(add_info.phys, 12 + phy);
    }

    /* For simulation only */
    PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&add_info);

    rv = portmod_port_add(unit, next_port, &add_info);
    if(rv < 0){
        return rv;
    }
    rv = portmod_port_enable_set(unit, next_port, 0 , 1);
    if(rv < 0){
        return rv;
    }

    next_port++;

    /* Add CAUI port using 4x25G*/

    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "\nAdd CGE port to PM4x25\n")));
    LOG_INFO(BSL_LS_APPL_PORT, (BSL_META_U(unit, "------------------------\n")));

    rv = portmod_reg_access_port_bindex_set(unit, next_port, 5, 0);
    if(rv < 0){
        return rv;
    }
    portmod_port_add_info_t_init(unit, &add_info);
    add_info.interface_config.speed = 1000000;
    add_info.interface_config.interface = SOC_PORT_IF_CAUI;
    add_info.interface_config.interface_modes = 0;
    PORTMOD_PBMP_PORT_ADD(add_info.phys, 24);
    PORTMOD_PBMP_PORT_ADD(add_info.phys, 25);
    PORTMOD_PBMP_PORT_ADD(add_info.phys, 26);
    PORTMOD_PBMP_PORT_ADD(add_info.phys, 27);

    /* For simulation only */
    PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&add_info);

    rv = portmod_port_add(unit, next_port, &add_info);
    if(rv < 0){
        return rv;
    }
    rv = portmod_port_enable_set(unit, next_port, 0 , 1);
    return rv;
}

int
portmod_appl_main(void)
{
    soc_control_t        *soc;
    void *user_data[SOC_MAX_NUM_DEVICES][MAX_PMS];
    int rv = 0;
    int unit = 0;

    sal_memset(user_data, 0 , sizeof(user_data));

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {  
        soc = sal_alloc(sizeof (soc_control_t), "soc_control");
        if (soc == NULL) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "failed to allocate soc_control\n")));
            return SOC_E_MEMORY;
        }
        sal_memset(soc, 0, sizeof (soc_control_t));
        SOC_CONTROL(unit) = soc;
    }

    /* Register access init */
    rv = portmod_reg_access_init(unit);
    if(rv < 0){
        return rv;
    }

    /* portmod init */
    rv = portmod_appl_init(unit, user_data[unit]);
    if(rv < 0){
        portmod_reg_access_deinit(unit);
        return rv;
    }

    rv = portmod_appl_add_ports(unit);
    if(rv < 0){
        portmod_appl_deinit(unit, user_data[unit]);
        portmod_reg_access_deinit(unit);
        return rv;
    }
    else{
    LOG_INFO(BSL_LS_APPL_PORT,
             (BSL_META_U(unit,
                         "ports added successfully!\n")));
    }

    /* portmod deinit */
    rv = portmod_appl_deinit(unit, user_data[unit]);
    if(rv < 0){
        return rv;
    }

    /* Register access deinit */
    rv = portmod_reg_access_deinit(unit);
    if(rv < 0){
        return rv;
    }

    return 0;
}

