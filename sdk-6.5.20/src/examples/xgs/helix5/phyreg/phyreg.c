/*
 * $Id: phyreg.c$
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * cint script for reading/writing phy registers
 */

/* Feature  : phyreg
 *
 * Usage    : BCM.0>c phyreg.c
 * Config   : phyreg_config.bcm
 *
 * Log file : phyreg_log.txt
 *
 * Test Topology : Nothing in specific
 *
 * Summary:
 * ========
 * This file provides examples how to use the BCM APIs to read and write PHY
 * registers
 *
 * Detailed steps done in the CINT script:
 * =======================================
 * 1) Step1 - Test Setup(Done in test_setup())
 * ===========================================
 *  a) Select 1 port on the switch  
 *
 * 2) Step2 - Configuration(Done in phyreg_config())
 * ===================================================
 * None

 * 3) Step3 - Verification (Done in verify())
 * ==========================================
 *  CLAUSE45
 *  a) Read PHYID2 register using port no, check if retrieved ID is correct
 *  b) Read PHYID2 register using mdio address, check if retrieved ID is correct
 *  c) Write disable remote fault bit of AN_X4_CL73_CFG_REG using port no and 
 *     check if it is written correctly
 *  d) Write disable remote fault bit of AN_X4_CL73_CFG_REG using mdio address 
 *     and check if it is written correctly
 *  CLAUSE22
 *  a) Read PHYID2 register using port no, check if retrieved ID is correct
 *  b) Read PHYID2 register using mdio address, check if retrieved ID is correct
 *  c) Read PHYID2 register using BCM_PORT_PHY_INTERNAL flag, check if retrieved
 *     ID is correct
 *  d) Read first 16 PCS registers
 *  e) Expected Result:
 *  ====================
 *  The scripts runs without failing.
 *  Comparison of phy register read/write is done inside verify.
 *  If any value does not match with the expected value, the script
 *  would fail.
 */


cint_reset();
int IEEE_STD_REG = 0;
int PHY_ID_MSB_REG = 2;
int AN_X4_CL73_CFG_REG = 0xC1C0;
int DIS_REMOTE_FAULT_BIT = 0x2000;
int port;
int mdio_addr = 0x81;
int PHY_ID_MSB_REG_VAL = 0x600D;

/*
 * This function is written so that hardcoding of port
 * numbers in cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t config_p;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &config_p);
    if(BCM_FAILURE(rv)){
        printf("Error in retrieving port configuration on unit %d, %s\n", unit, bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = config_p.xe;
    for(i = 1; i < BCM_PBMP_PORT_MAX; i++){
        if(BCM_PBMP_MEMBER(&ports_pbmp,i) && (port < num_ports)){
            port_list[port]=i;
            port++;
        }
    }

    if((port == 0) || (port != num_ports)){
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}


/*
 * This functions gets the port number to read phy register
 */
bcm_error_t
test_setup(int unit)
{
    if(BCM_E_NONE != port_numbers_get(unit, &port, 1)){
        printf("portNumbersGet() failed\n");
        return -1;
    }

    return BCM_E_NONE;
}


/*
 * Config goes here, nothing in specific for phy register read/write
 */
int
phyreg_config(int unit)
{
    return BCM_E_NONE;
}


/*
 * This function reads/writes phy registers and compares with the expected 
 * values
 */
int
verify(int unit)
{
    int rv = BCM_E_NONE;
    int dev_addr, reg_addr;
    uint32 value;
    int flags;
    uint32 c45_addr;


    /* Clause45 read using port number */
    flags = BCM_PORT_PHY_CLAUSE45;
    value = 0xFFFFFFFF;
    dev_addr = IEEE_STD_REG;
    reg_addr = PHY_ID_MSB_REG;
    c45_addr = BCM_PORT_PHY_CLAUSE45_ADDR(dev_addr, reg_addr);
    rv = bcm_port_phy_get(unit, port, flags, c45_addr, &value);
    if(BCM_FAILURE(rv)){
        printf("Error C45 bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    printf("C45 port register read using port no\n");
    printf("dev_addr:%x, reg_addr:%x, value:%x\n\n", dev_addr, reg_addr, value);
    if(value != PHY_ID_MSB_REG_VAL) {
        printf("C45 port register read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, PHY_ID_MSB_REG_VAL);
        return rv;
    }

    /* Clause45 read using MDIO address */
    flags = BCM_PORT_PHY_CLAUSE45 | BCM_PORT_PHY_NOMAP;
    value = 0xFFFFFFFF;
    dev_addr = IEEE_STD_REG;
    reg_addr = PHY_ID_MSB_REG;
    c45_addr = BCM_PORT_PHY_CLAUSE45_ADDR(dev_addr, reg_addr);
    rv = bcm_port_phy_get(unit, mdio_addr, flags, c45_addr, value);
    if(BCM_FAILURE(rv)){
        printf("Error C45 bcm_port_phy_get on unit %d, mdio address %d %s\n", unit, mdio_addr, bcm_errmsg(rv));
        return rv;
    }
    printf("C45 port register read using mdio addr no\n", value);
    printf("dev_addr:%x, reg_addr:%x, value:%x\n\n", dev_addr, reg_addr, value);
    if(value != PHY_ID_MSB_REG_VAL) {
        printf("C45 mdio addr register read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, PHY_ID_MSB_REG_VAL);
        return rv;
    }


    /* Clause45 write using port number */
    flags = BCM_PORT_PHY_CLAUSE45;
    dev_addr = 0;
    value = 0;
    reg_addr = AN_X4_CL73_CFG_REG;
    c45_addr = BCM_PORT_PHY_CLAUSE45_ADDR(dev_addr, reg_addr);
    /* Set the value to 0 first */
    rv = bcm_port_phy_set(unit, port, flags, c45_addr, value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_set on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_phy_get(unit, port, flags, c45_addr, &value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    printf("c45 port register read using port no before register write\n");
    printf("dev_addr:%x, reg_addr:%x, value:%x\n", dev_addr, reg_addr, value);

    value = DIS_REMOTE_FAULT_BIT;
    rv = bcm_port_phy_set(unit, port, flags, c45_addr, value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    value = 0xFFFFFFFF;
    rv = bcm_port_phy_get(unit, port, flags, c45_addr, &value);
    printf("c45 port register read using port no after register write\n");
    printf("dev_addr:%x, reg_addr:%x, value:%x\n\n", dev_addr, reg_addr, value);
    if(value != DIS_REMOTE_FAULT_BIT) {
        printf("C45 port register write/read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, DIS_REMOTE_FAULT_BIT);
    }


    /* Clause45 write using mdio address */
    flags = BCM_PORT_PHY_CLAUSE45;
    dev_addr = 0;
    reg_addr = AN_X4_CL73_CFG_REG;
    value = 0x0;
    c45_addr = BCM_PORT_PHY_CLAUSE45_ADDR(dev_addr, reg_addr);
    /* Set the value to 0 first */
    rv = bcm_port_phy_set(unit, port, flags, c45_addr, value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_set on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_phy_get(unit, port, flags, c45_addr, &value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, mdio address %x %s\n", unit, mdio_addr, bcm_errmsg(rv));
        return rv;
    }
    printf("c45 port register read using port no before register write\n");
    printf("dev_addr:%x, reg_addr:%x, value:%x\n", dev_addr, reg_addr, value);

    value = DIS_REMOTE_FAULT_BIT;
    rv = bcm_port_phy_set(unit, port, flags, c45_addr, value);

    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, mdio address %d %s\n", unit, mdio_addr, bcm_errmsg(rv));
        return rv;
    }
    value = 0xFFFFFFFF;
    rv = bcm_port_phy_get(unit, port, flags, c45_addr, &value);
    printf("c45 port register read using port no after register write\n");
    printf("dev_addr:%x, reg_addr:%x, value:%x\n\n", dev_addr, reg_addr, value);
    if(value != DIS_REMOTE_FAULT_BIT) {
        printf("C45 mdio addr register write/read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, DIS_REMOTE_FAULT_BIT);
    }


    /* Clause22 read using port number */
    flags = 0;
    value = 0xFFFFFFFF; 
    reg_addr = PHY_ID_MSB_REG;
    rv = bcm_port_phy_get(unit, port, flags, reg_addr, value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    printf("c22 port register read using port no\n"); 
    printf("reg_addr:%x, value:%x\n\n", reg_addr, value);
    if(value != PHY_ID_MSB_REG_VAL) {
        printf("C22 port register read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, PHY_ID_MSB_REG_VAL);
    }
    

    /* Clause22 read using mdio address */
    flags = BCM_PORT_PHY_NOMAP;
    value = 0xFFFFFFFF; 
    reg_addr = PHY_ID_MSB_REG;
    rv = bcm_port_phy_get(unit, mdio_addr, flags, reg_addr, &value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    printf("c22 port register read using mdio address\n");
    printf("reg_addr:%x, value:%x\n\n", reg_addr, value);
    if(value != PHY_ID_MSB_REG_VAL) {
        printf("C22 port register read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, PHY_ID_MSB_REG_VAL);
    }


    /* Clause22 internal phy read using port number */
    flags = BCM_PORT_PHY_INTERNAL;
    value = 0xFFFFFFFF;
    reg_addr = PHY_ID_MSB_REG;
    rv = bcm_port_phy_get(unit, port, flags, reg_addr, &value);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    printf("c22 internal register read using port number\n", value);
    printf("reg_addr:%x, value:%x\n\n", reg_addr, value);
    if(value != PHY_ID_MSB_REG_VAL) {
        printf("C22 BCM_PORT_PHY_INTERNAL read did not return expected val\n");
        printf("value:%x, expected value:%x\n", value, PHY_ID_MSB_REG_VAL);
    }


    /* Clause22 PCS register read */
    flags = 0;
    value = 0xFFFFFFFF;
    printf("PCS regisers,0-16\n");
    for (reg_addr = 0; reg_addr < 16; reg_addr++) {
        if (reg_addr && ((reg_addr % 4) == 0)) {
            printf("\n");
        }
        rv = bcm_port_phy_get(unit, port, flags, reg_addr, &value);
        if(BCM_FAILURE(rv)){
            printf("Error bcm_port_phy_get on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
            return rv;
        }
        printf("\t0x%02x: 0x%04x", reg_addr, value);
    }
    printf("\n");

    return BCM_E_NONE;
}


/* Main function to run the cint script and check the results */
bcm_error_t
execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");
    if(BCM_FAILURE((rv = test_setup(unit)))){
        printf("test_setup() failed\n");
        return -1;
    }

    if(BCM_FAILURE((rv = phyreg_config(unit)))){
        printf("phyreg_config() failed\n");
        return -1;
    }

    if(BCM_FAILURE(rv = verify(unit))){
        printf("verify() failed\n");
        return -1;
    }

    return BCM_E_NONE;
}


const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}





