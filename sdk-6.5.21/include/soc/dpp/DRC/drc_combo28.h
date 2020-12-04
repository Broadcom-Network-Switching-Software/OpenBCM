/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains DPP DRC main structure and routine declarations for the Dram operation using PHY Combo28.
 */
#ifndef _SOC_DPP_DRC_COMBO28_H
#define _SOC_DPP_DRC_COMBO28_H



#include <soc/dpp/dpp_config_defs.h>


#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h> 


#include <soc/dpp/TMC/tmc_api_dram.h>


#include <soc/shmoo_combo28.h>
#include <soc/shmoo_combo16.h>



#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED          SOC_TMC_DRAM_CLAM_SHELL_MODE_DISABLED
#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_0            SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_0
#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_1            SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_1
typedef SOC_TMC_DRAM_CLAM_SHELL_MODE                       DRC_COMBO28_DRAM_CLAM_SHELL_MODE;




#define SOC_DPP_DRC_COMBO28_MRS_NUM_MAX (16)

#define SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE (SOC_SAND_NOF_BITS_IN_BYTE) 
#define SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BITS (32)
#define SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES (SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BITS / SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE) 
#define SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES (4)


#define SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT (31)


#define SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE (18)


#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4 0
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_GDDR5 1


#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_10 2
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_8 0





typedef struct soc_dpp_drc_combo28_info_dram_param_s {

    
    uint32 t_rfc;

    
    uint32 t_rc;

    
    uint32 t_rcd_rd;
    
    uint32 t_rcd_wr;

    
    uint32 t_rrd_l;

    
    uint32 t_rrd_s;

    
    uint32 t_ras;

    
    uint32 t_rp;

    
    uint32 t_wr;

    
    uint32 t_faw;

    
    uint32 t_32aw;

    
    uint32 t_rtp_l;

    
    uint32 t_rtp_s;

    
    uint32 t_wtr_l;

    
    uint32 t_wtr_s;

    
    uint32 t_ccd_l;

    
    uint32 t_ccd_s;

    
    uint32 t_zqcs;

    
    uint32 t_ref;

    
    uint32 t_crc_alert;

    
    uint32 t_rst;

    
    uint32 t_al;

    
    uint32 t_crc_rd_latency;

    
    uint32 t_crc_wr_latency;

    
    uint32 c_cas_latency;

    
    uint32 c_wr_latency;

    
    uint32 init_wait_period;

} soc_dpp_drc_combo28_info_dram_param_t;


typedef struct soc_dpp_drc_combo28_info_dram_internal_param_s {

    
    uint32 t_ck;

    
    uint32 c_wl;

    
    uint32 c_refi;

    
    uint32 c_pcb;

    
    uint32 c_rfc_f;

    
    uint32 c_ras;

    
    uint32 c_mr_ras;

    
    uint32 c_rp;

    
    uint32 c_wr;

    
    uint32 c_mr_wr;

    
    uint32 c_rtp_l;

    
    uint32 c_wtr_l;

    
    uint32 c_wtr_s;

    
    uint32 c_ccd_l;

    
    uint32 c_ccd_s;

    
    uint32 c_al;

    
    uint32 c_crc_rd_latency;

    
    uint32 c_crc_wr_latency;

    
    uint32 bl;

    
    uint32 dqs_prem_1_ck;

    
    uint32 wr_recovery_op;

    
    uint32 burst_chop_en;

} soc_dpp_drc_combo28_info_dram_internal_param_t;


typedef struct soc_dpp_drc_combo28_info_s {

     

    
    uint32 device_core_freq;

    
    uint8 enable;

    
    SHR_BITDCLNAME(dram_bitmap, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));

    
    SHR_BITDCLNAME(ref_clk_bitmap, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));

    
    int dram_num;

    
    int dram_freq;

    
    int data_rate_mbps;

    
    int ref_clk_mhz;

    
    uint32 dram_type;

    
    uint32 nof_banks;

    
    uint32 nof_columns;

    
    uint32 nof_rows;
    
    
    soc_dpp_drc_combo28_info_dram_param_t dram_param;

    
    soc_dpp_drc_combo28_info_dram_internal_param_t dram_int_param;

    
    uint32 mr[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_MRS_NUM_MAX];

    
    DRC_COMBO28_DRAM_CLAM_SHELL_MODE dram_clam_shell_mode[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

    
    uint8 zq_calib_map[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

    
    uint32 ui_size;

    
    uint32 dram_dq_swap[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES][SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE]; 

    
    uint32 dram_dq_swap_bytes[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES];

    
    uint32 dram_addr_bank_swap[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE];

    
    uint32 dram_reference_fsm_vdl[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];

    
    combo28_shmoo_dram_info_t shmoo_info;

    
    combo28_vendor_info_t vendor_info;
    
     
    
    uint32 dram_auto_calibration_update_disable;
         
    
    uint32 gear_down_mode;

    
    uint32 abi;

    
    uint32 write_dbi;

    
    uint32 read_dbi;

    
    uint32 write_crc;
    
    
    uint32 read_crc;

    
    uint32 cmd_par_latency;

    
    int auto_tune;

    combo28_shmoo_config_param_t shmoo_config_param[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

    
    uint8 bist_enable;

    
    int sim_system_mode;

    
    uint32 appl_max_buffer_crc_err;

    
    uint8 t_ras_enable;

} soc_dpp_drc_combo28_info_t;

typedef struct soc_dpp_drc_gddr5_temp_s {
    int max_temp;
    int min_temp;
    int current_temp;
} soc_dpp_drc_gddr5_temp_t;


 

int soc_dpp_drc_combo28_util_calm_shell_bits_swap(int unit, uint32 in_buf, uint32 *out_buf);


int soc_dpp_drc_combo28_dram_cpu_command(int unit, int drc_ndx, uint32 ras_n, uint32 cas_n, uint32 we_n, uint32 act_n, uint32 bank, uint32 address);
int soc_dpp_drc_combo28_shmoo_load_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 mrs_opcode);

int soc_dpp_drc_combo28_validate_dram_address (int unit, uint32 addr);


int soc_dpp_drc_combo28_info_config_default(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_info_config(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_shmoo_cfg_get(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param);
int soc_dpp_drc_combo28_shmoo_cfg_set(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param);
int soc_dpp_drc_combo28_enable_wr_crc(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(int unit, soc_dpp_drc_combo28_info_t* drc_info, int drc_ndx);
int soc_dpp_drc_combo28_update_dram_reference_fsm_vdl(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t* drc_info, int fsm_vdl_total_drift[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES]);
int soc_dpp_drc_combo28_cdr_monitor_enable_set(int unit, int drc_ndx, int switch_on);
int soc_dpp_drc_combo28_cdr_monitor_enable_get(int unit, int drc_ndx, int* is_enabled);
int soc_dpp_drc_combo28_cdr_ctl(int unit, int drc_ndx, int auto_copy, int reset_n, int enable);
int soc_dpp_drc_combo28_gddr5_temp_get(int unit, int drc_ndx, soc_dpp_drc_gddr5_temp_t *gddr5_temp);


int soc_dpp_drc_combo28_transform_value_based_on_byte_swaps(int unit, int drc_ndx, uint32 orig_val, uint32* new_val);


int soc_dpp_drc_combo28_dynamic_calibration_handle_cdr_interrupt(int unit, int drc_ndx);
int soc_dpp_drc_combo28_rd_wr_crc_handle_interrupt(int unit, int drc_ndx);
int soc_dpp_drc_combo28_rd_wr_crc_interrupt_control(int unit, int drc_index, int restore_saved_state, int mask_interrupt);



int soc_dpp_drc_combo28_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data);
int soc_dpp_drc_combo28_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data);
int soc_dpp_drc_combo28_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);
int soc_dpp_drc_combo28_shmoo_init_ddr4(int unit, int drc_ndx);



#endif 
