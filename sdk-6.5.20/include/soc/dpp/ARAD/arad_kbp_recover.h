/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_KBP_RECOVER_INCLUDED__

#define __ARAD_KBP_RECOVER_INCLUDED__



int arad_kbp_recover_rx_enable (int unit, int mdio_id);

int arad_kbp_recover_rx_shut_down (int unit, int mdio_id);

int arad_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    ARAD_NIF_ELK_USER_DATA user_data[SOC_DPP_DEFS_MAX(NOF_CORES)],
    uint32 retries,
    void *, int option);

int arad_kbp_op_recover_txpcs_soft_reset(int unit, int core, soc_port_t kbp_port, int mdio_id);

int arad_kbp_op_phmf_fifo_reset(int unit, int core, int mdio_id);

int arad_kbp_op_link_stability_check(int unit, uint32 core, uint32 kbp_mdio_id, uint32 retries);

#endif 
