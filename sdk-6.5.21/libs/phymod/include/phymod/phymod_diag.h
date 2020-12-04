/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *         
 * Shell diagnostics of Phymod    
 */

#ifndef _PHYMOD_DIAG_H_
#define _PHYMOD_DIAG_H_

#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_symbols.h>


/******************************************************************************
 Typedefs
 */
typedef int (*print_func_f)(const char *, ...);

typedef enum{
    PhymodDiagPrbsClear,
    PhymodDiagPrbsSet,
    PhymodDiagPrbsGet
}phymod_diag_prbs_operation_t;

typedef struct phymod_diag_prbs_set_args_s{
    uint32_t flags;
    phymod_prbs_t prbs_options;
    uint32_t enable;
    uint32_t loopback;
}phymod_diag_prbs_set_args_t;

typedef struct phymod_diag_prbs_get_args_s{
    uint32_t time;
}phymod_diag_prbs_get_args_t;

typedef struct phymod_diag_prbs_clear_args_s{
    uint32_t flags;
}phymod_diag_prbs_clear_args_t;


typedef union phymod_diag_prbs_command_args_u{
    phymod_diag_prbs_set_args_t set_params;
    phymod_diag_prbs_get_args_t get_params;
    phymod_diag_prbs_clear_args_t clear_params;
}phymod_prbs_command_args_t;

typedef struct phymod_diag_prbs_args_s{
    phymod_diag_prbs_operation_t prbs_cmd;
    phymod_prbs_command_args_t args;
}phymod_diag_prbs_args_t;


/******************************************************************************
Functions
 */

extern print_func_f phymod_diag_print_func;

int phymod_diag_symbols_table_get(phymod_phy_access_t *phy, phymod_symbols_t **symbols);
int phymod_diag_firmware_mode_set(phymod_core_access_t *cores, int array_size, phymod_firmware_lane_config_t fw_config);
int phymod_diag_firmware_load(phymod_core_access_t *cores, int array_size, char *firwmware_file);
int phymod_diag_prbs(phymod_phy_access_t *phys, int array_size, phymod_diag_prbs_args_t *prbs_diag_args);
int phymod_diag_reg_write(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr, uint32_t val);
int phymod_diag_reg_read(phymod_phy_access_t *phys, int array_size, uint32_t reg_addr);
int phymod_diag_dsc(phymod_phy_access_t *phys, int array_size);
int phymod_diag_dsc_std(phymod_phy_access_t *phys, int array_size);
int phymod_diag_dsc_config(phymod_phy_access_t *phys, int array_size);


int phymod_diag_eyescan_run( 
    phymod_phy_access_t *phys,
    int unit,
    int* port_ids,
    int* line_rates, 
    int num_phys,
    phymod_eyescan_mode_t mode,
    phymod_phy_eyescan_options_t* eyescan_options);

#endif /*_PHYMOD_DIAG_H_*/
