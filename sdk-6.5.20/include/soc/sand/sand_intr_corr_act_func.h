/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header for ser correction.
 */

#ifndef _SAND_INTR_CORR_ACT_FUNC_H_
#define _SAND_INTR_CORR_ACT_FUNC_H_


#include <soc/defs.h>
#include <shared/shrextend/shrextend_debug.h>



#define SAND_INTERRUPT_PRINT_MSG_SIZE               1000
#define SAND_INTERRUPT_COR_ACT_MSG_SIZE             36
#define SAND_INTERRUPT_SPECIAL_MSG_SIZE             512



typedef enum {
    SAND_INT_CORR_ACT_CLEAR_CHECK,
    SAND_INT_CORR_ACT_CONFIG_DRAM,
    SAND_INT_CORR_ACT_ECC_1B_FIX,
    SAND_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY,
    SAND_INT_CORR_ACT_FORCE,
    SAND_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO,
    SAND_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO,
    SAND_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO,
    SAND_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO,
    SAND_INT_CORR_ACT_HARD_RESET,
    SAND_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC,
    SAND_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY,
    SAND_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY,
    SAND_INT_CORR_ACT_INGRESS_HARD_RESET,
    SAND_INT_CORR_ACT_IPS_QDESC,
    SAND_INT_CORR_ACT_NONE,
    SAND_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY,
    SAND_INT_CORR_ACT_PRINT,
    SAND_INT_CORR_ACT_REPROGRAM_RESOURCE,
    SAND_INT_CORR_ACT_RTP_LINK_MASK_CHANGE,
    SAND_INT_CORR_ACT_RX_LOS_HANDLE,
    SAND_INT_CORR_ACT_SHADOW,
    SAND_INT_CORR_ACT_SHADOW_AND_SOFT_RESET,
    SAND_INT_CORR_ACT_SHUTDOWN_FBR_LINKS,
    SAND_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION,
    SAND_INT_CORR_ACT_SOFT_RESET,
    SAND_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB,
    SAND_INT_CORR_ACT_RTP_SLSCT,
    SAND_INT_CORR_ACT_SHUTDOWN_LINKS,
    SAND_INT_CORR_ACT_MC_RTP_CORRECT,
    SAND_INT_CORR_ACT_UC_RTP_CORRECT,
    SAND_INT_CORR_ACT_ALL_REACHABLE_FIX,
    SAND_INT_CORR_ACT_EVENT_READY,
    SAND_INT_CORR_ACT_IPS_QSZ_CORRECT,
    SAND_INT_CORR_ACT_EM_SOFT_RECOVERY,
    SAND_INT_CORR_ACT_XOR_FIX,
    SAND_INT_CORR_ACT_DYNAMIC_CALIBRATION,
    SAND_INT_CORR_ACT_EM_ECC_1B_FIX,
    SAND_INT_CORR_ACT_INGRESS_SOFT_RESET,
    SAND_INT_CORR_ACT_XOR_ECC_1B_FIX,
    SAND_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET,
    SAND_INT_CORR_ACT_MDB_ECC_1B_FIX,
    SAND_INT_CORR_ACT_MDB_ECC_2B_FIX,
    SAND_INT_CORR_ACT_DELETE_BDB_FIFO_FULL,
    SAND_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY,
    SAND_INT_CORR_ACT_HBC_ECC_1B_FIX,
    SAND_INT_CORR_ACT_HBC_ECC_2B_FIX,
    SAND_INT_CORR_ACT_SOFT_INIT,
    SAND_INT_PFC_DEADLOCK_BREAKING_MECHANISM,
    SAND_INT_CORR_ACT_ILKN_RX_PORT_STATUS_CHANGE,
    SAND_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID,
    SAND_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR,
    SAND_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING,
    SAND_INT_CORR_ACT_TCAM_MOVE_ECC_VALID,
    SAND_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT,
    SAND_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT,
    SAND_INT_CORR_ACT_PON_RESET,
    SAND_INT_CORR_ACT_BTC_HEADER_SIZE_EXCEED,
    SAND_INT_CORR_ACT_BTC_NOF_INSTRUCTIONS_EXCEED,
    SAND_INT_CORR_ACT_BTC_HEADER_SIZE_NOT_BYTE_ALIGNED,
    SAND_INT_CORR_ACT_EPNI_ETPP_DEC_ABOVE_TH,
    SAND_INT_CORR_ACT_EPNI_ETPP_SOP_DEC_ABOVE_TH,
    SAND_INT_CORR_ACT_EPNI_ETPP_EOP_DEC_ABOVE_TH,
    SAND_INT_CORR_ACT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH,
    SAND_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID,
    SAND_INT_CORR_ACT_TCAM_ECC2BITS_HANDLE_DONE,
	SAND_INT_CORR_ACT_MACSEC_SA_EXPIRY,
    SAND_INT_CORR_ACT_KAPS_TECC,
    SAND_INT_CORR_ACT_MAX
} sand_int_corr_act_type;

typedef enum sand_memory_dc_e
{
    SAND_INVALID_DC=-1,
    SAND_ECC_ECC2B_DC,
    SAND_ECC_ECC1B_DC,
    SAND_P_1_ECC_ECC2B_DC,
    SAND_P_1_ECC_ECC1B_DC,
    SAND_P_2_ECC_ECC2B_DC,
    SAND_P_2_ECC_ECC1B_DC,
    SAND_P_3_ECC_ECC2B_DC,
    SAND_P_3_ECC_ECC1B_DC,
    SAND_ECC_PARITY_DC
} sand_memory_dc_t;


 
typedef struct
{
  
  uint32 bank_id;
  
  uint32 entry;
} sand_intr_tcam_location;

typedef struct sand_intr_action_s
{
    soc_handle_interrupt_func   func_arr;
    sand_int_corr_act_type      corr_action;
} sand_intr_action_t;

typedef struct sand_block_control_info_s
{
    int ecc1_int;
    int ecc2_int;
    int parity_int;
    soc_reg_t gmo_reg;                  

    soc_reg_t ecc1_monitor_mem_reg;     
    soc_reg_t ecc2_monitor_mem_reg;     
    soc_reg_t parity_monitor_mem_reg;   

    soc_reg_t ecc1_cnt_reg;
    soc_reg_t ecc2_cnt_reg;
    soc_reg_t parity_cnt_reg;
} sand_block_control_info_t;



void sand_intr_action_info_set(int unit, void* sand_intr_action);
sand_intr_action_t* sand_intr_action_info_get(int unit);
int sand_is_ser_action_support(int unit);
char* sand_intr_action_name_by_func(int unit, soc_handle_interrupt_func intr_func);
sand_memory_dc_t sand_get_cnt_reg_type(int unit, soc_reg_t cnt_reg);
int sand_get_cnt_reg_values(
        int unit,
        sand_memory_dc_t type,
        soc_reg_t cnt_reg,
        int copyno,
        uint32 *cntf, 
        uint32 *cnt_overflowf, 
        uint32 *addrf, 
        uint32 *addr_validf);
int sand_is_ser_intr_cnt_reg(int unit, soc_reg_t reg);



int
sand_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    sand_int_corr_act_type corr_act,
    char *special_msg);

int sand_blktype_to_index(soc_block_t blktype);
int sand_set_block_control_regs_info_init(int unit, void* data);
int sand_collect_blocks_control_info(int unit);
int sand_dump_block_control_info(int unit, sand_block_control_info_t* map);


int sand_read_mem_gmo_reg(int unit, soc_mem_t mem, int copyno, uint32* regval);


int sand_write_mem_gmo_reg(int unit, soc_mem_t mem, int copyno, uint32 regval);


int sand_disable_block_mem_monitor(
        int unit,
        soc_mem_t mem,
        int copyno,
        soc_reg_above_64_val_t* monitor_mask);


int sand_re_enable_block_mem_monitor(int unit, int interrupt_id, soc_mem_t mem, int copyno, soc_reg_above_64_val_t* monitor_mask);

sand_block_control_info_t* sand_get_block_control_info(int unit);

int
sand_single_interrupt_info_dump(
    int unit,
    int intr);

#endif 

