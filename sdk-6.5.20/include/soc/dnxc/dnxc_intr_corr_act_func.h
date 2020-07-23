/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header correction action functions for jr2_jericho interrupts.
 */

#ifndef _DNXC_INTR_CORR_ACT_FUNC_H_
#define _DNXC_INTR_CORR_ACT_FUNC_H_

#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/sand/sand_intr_corr_act_func.h>

#define DNXC_INTERRUPT_PRINT_MSG_SIZE               1000
#define DNXC_INTERRUPT_COR_ACT_MSG_SIZE             36
#define DNXC_INTERRUPT_SPECIAL_MSG_SIZE             512

typedef enum
{
    DNXC_INT_CORR_ACT_CLEAR_CHECK = SAND_INT_CORR_ACT_CLEAR_CHECK,
    DNXC_INT_CORR_ACT_CONFIG_DRAM = SAND_INT_CORR_ACT_CONFIG_DRAM,
    DNXC_INT_CORR_ACT_ECC_1B_FIX = SAND_INT_CORR_ACT_ECC_1B_FIX,
    DNXC_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY = SAND_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY,
    DNXC_INT_CORR_ACT_FORCE = SAND_INT_CORR_ACT_FORCE,
    DNXC_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO = SAND_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO,
    DNXC_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO = SAND_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO,
    DNXC_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO = SAND_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO,
    DNXC_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO = SAND_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO,
    DNXC_INT_CORR_ACT_HARD_RESET = SAND_INT_CORR_ACT_HARD_RESET,
    DNXC_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC = SAND_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC,
    DNXC_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY = SAND_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY,
    DNXC_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY = SAND_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY,
    DNXC_INT_CORR_ACT_INGRESS_HARD_RESET = SAND_INT_CORR_ACT_INGRESS_HARD_RESET,
    DNXC_INT_CORR_ACT_IPS_QDESC = SAND_INT_CORR_ACT_IPS_QDESC,
    DNXC_INT_CORR_ACT_NONE = SAND_INT_CORR_ACT_NONE,
    DNXC_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY = SAND_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY,
    DNXC_INT_CORR_ACT_PRINT = SAND_INT_CORR_ACT_PRINT,
    DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE = SAND_INT_CORR_ACT_REPROGRAM_RESOURCE,
    DNXC_INT_CORR_ACT_RTP_LINK_MASK_CHANGE = SAND_INT_CORR_ACT_RTP_LINK_MASK_CHANGE,
    DNXC_INT_CORR_ACT_RX_LOS_HANDLE = SAND_INT_CORR_ACT_RX_LOS_HANDLE,
    DNXC_INT_CORR_ACT_SHADOW = SAND_INT_CORR_ACT_SHADOW,
    DNXC_INT_CORR_ACT_SHADOW_AND_SOFT_RESET = SAND_INT_CORR_ACT_SHADOW_AND_SOFT_RESET,
    DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS = SAND_INT_CORR_ACT_SHUTDOWN_FBR_LINKS,
    DNXC_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION = SAND_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION,
    DNXC_INT_CORR_ACT_SOFT_RESET = SAND_INT_CORR_ACT_SOFT_RESET,
    DNXC_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB = SAND_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB,
    DNXC_INT_CORR_ACT_RTP_SLSCT = SAND_INT_CORR_ACT_RTP_SLSCT,
    DNXC_INT_CORR_ACT_SHUTDOWN_LINKS = SAND_INT_CORR_ACT_SHUTDOWN_LINKS,
    DNXC_INT_CORR_ACT_MC_RTP_CORRECT = SAND_INT_CORR_ACT_MC_RTP_CORRECT,
    DNXC_INT_CORR_ACT_UC_RTP_CORRECT = SAND_INT_CORR_ACT_UC_RTP_CORRECT,
    DNXC_INT_CORR_ACT_ALL_REACHABLE_FIX = SAND_INT_CORR_ACT_ALL_REACHABLE_FIX,
    DNXC_INT_CORR_ACT_EVENT_READY = SAND_INT_CORR_ACT_EVENT_READY,
    DNXC_INT_CORR_ACT_IPS_QSZ_CORRECT = SAND_INT_CORR_ACT_IPS_QSZ_CORRECT,
    DNXC_INT_CORR_ACT_EM_SOFT_RECOVERY = SAND_INT_CORR_ACT_EM_SOFT_RECOVERY,
    DNXC_INT_CORR_ACT_XOR_FIX = SAND_INT_CORR_ACT_XOR_FIX,
    DNXC_INT_CORR_ACT_DYNAMIC_CALIBRATION = SAND_INT_CORR_ACT_DYNAMIC_CALIBRATION,
    DNXC_INT_CORR_ACT_EM_ECC_1B_FIX = SAND_INT_CORR_ACT_EM_ECC_1B_FIX,
    DNXC_INT_CORR_ACT_INGRESS_SOFT_RESET = SAND_INT_CORR_ACT_INGRESS_SOFT_RESET,
    DNXC_INT_CORR_ACT_XOR_ECC_1BIT_FIX = SAND_INT_CORR_ACT_XOR_ECC_1B_FIX,
    DNXC_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET = SAND_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET,
    DNXC_INT_CORR_ACT_MDB_ECC_1B_FIX = SAND_INT_CORR_ACT_MDB_ECC_1B_FIX,
    DNXC_INT_CORR_ACT_MDB_ECC_2B_FIX = SAND_INT_CORR_ACT_MDB_ECC_2B_FIX,
    DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_FULL = SAND_INT_CORR_ACT_DELETE_BDB_FIFO_FULL,
    DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY = SAND_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY,
    DNXC_INT_CORR_ACT_HBC_ECC_1B_FIX = SAND_INT_CORR_ACT_HBC_ECC_1B_FIX,
    DNXC_INT_CORR_ACT_HBC_ECC_2B_FIX = SAND_INT_CORR_ACT_HBC_ECC_2B_FIX,
    DNXC_INT_CORR_ACT_SOFT_INIT = SAND_INT_CORR_ACT_SOFT_INIT,
    DNXC_INT_PFC_DEADLOCK_BREAKING_MECHANISM = SAND_INT_PFC_DEADLOCK_BREAKING_MECHANISM,
    DNXC_INT_CORR_ACT_ILKN_RX_PORT_STATUS_CHANGE = SAND_INT_CORR_ACT_ILKN_RX_PORT_STATUS_CHANGE,
    DNXC_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID = SAND_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID,
    DNXC_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR = SAND_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR,
    DNXC_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING = SAND_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING,
    DNXC_INT_CORR_ACT_TCAM_MOVE_ECC_VALID = SAND_INT_CORR_ACT_TCAM_MOVE_ECC_VALID,
    DNXC_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT = SAND_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT,
    DNXC_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT = SAND_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT,
    DNXC_INT_CORR_ACT_PON_RESET = SAND_INT_CORR_ACT_PON_RESET,
    DNXC_INT_CORR_ACT_BTC_HEADER_SIZE_EXCEED = SAND_INT_CORR_ACT_BTC_HEADER_SIZE_EXCEED,
    DNXC_INT_CORR_ACT_BTC_NOF_INSTRUCTIONS_EXCEED = SAND_INT_CORR_ACT_BTC_NOF_INSTRUCTIONS_EXCEED,
    DNXC_INT_CORR_ACT_BTC_HEADER_SIZE_NOT_BYTE_ALIGNED = SAND_INT_CORR_ACT_BTC_HEADER_SIZE_NOT_BYTE_ALIGNED,
    DNXC_INT_CORR_ACT_EPNI_ETPP_DEC_ABOVE_TH = SAND_INT_CORR_ACT_EPNI_ETPP_DEC_ABOVE_TH,
    DNXC_INT_CORR_ACT_EPNI_ETPP_SOP_DEC_ABOVE_TH = SAND_INT_CORR_ACT_EPNI_ETPP_SOP_DEC_ABOVE_TH,
    DNXC_INT_CORR_ACT_EPNI_ETPP_EOP_DEC_ABOVE_TH = SAND_INT_CORR_ACT_EPNI_ETPP_EOP_DEC_ABOVE_TH,
    DNXC_INT_CORR_ACT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH = SAND_INT_CORR_ACT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH,
    DNXC_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID = SAND_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID,
    DNXC_INT_CORR_ACT_TCAM_ECC2BITS_HANDLE_DONE = SAND_INT_CORR_ACT_TCAM_ECC2BITS_HANDLE_DONE,
    DNXC_INT_CORR_ACT_MACSEC_SA_EXPIRY = SAND_INT_CORR_ACT_MACSEC_SA_EXPIRY,
    DNXC_INT_CORR_ACT_KAPS_TECC = SAND_INT_CORR_ACT_KAPS_TECC,
    DNXC_INT_CORR_ACT_MAX
} dnxc_int_corr_act_type;

#define DNXC_XOR_MEMORY_NOT_RECOVER_XOR_BANK_FLAGS  0x1
#define DNXC_XOR_MEMORY_FORCE_SEVERITY_FLAGS        0x2

typedef struct
{
    soc_mem_t mem;
    int sram_banks_bits;
    int entry_used_bits;
    int flags;
} dnxc_xor_mem_info;

typedef struct
{
    soc_mem_t mem;
    unsigned int array_index;
    int copyno;
    int min_index;
    int max_index;
} dnxc_interrupt_mem_err_info;

typedef struct dnxc_intr_action_s
{
    soc_handle_interrupt_func func_arr;
    dnxc_int_corr_act_type corr_action;
} dnxc_intr_action_t;

int dnxc_mem_decide_corrective_action(
    int unit,
    sand_memory_dc_t type,
    soc_mem_t mem,
    int copyno,
    dnxc_int_corr_act_type * action_type,
    char *special_msg);
char *dnxc_mem_ser_ecc_action_info_get(
    int unit,
    soc_mem_t mem,
    int isEcc1b);

int dnxc_xor_mem_info_get(
    int unit,
    soc_mem_t mem,
    dnxc_xor_mem_info * xor_mem_info);

int dnxc_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * shadow_correct_info_p,
    char *msg);

int dnxc_interrupt_handles_corrective_action_for_xor(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * xor_correct_info,
    int ecc1bit,
    char *msg);

int dnxc_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * ecc_1b_correct_info_p,
    char *msg);
int dnxc_interrupt_handles_corrective_action_do_nothing(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

int dnxc_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

int dnxc_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

#ifdef BCM_DNX_SUPPORT

int dnxc_interrupt_handles_corrective_action_for_mdb_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * ecc_1b_correct_info_p,
    char *msg);

int dnxc_interrupt_handles_corrective_action_for_mdb_2b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * ecc_1b_correct_info_p,
    char *msg);

int dnxc_interrupt_data_collection_for_tcam_protection_error(
    int unit,
    int block_instance,
    char *special_msg,
    sand_intr_tcam_location * location);

int dnxc_interrupt_tcam_protection_ecc_2bits_handle_done(
    int unit,
    int block_instance);

int dnxc_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(
    int unit,
    int block_instance,
    sand_intr_tcam_location * location);

int dnx_interrupt_data_collection_for_kaps_tcam_error(
    int unit,
    int block_instance,
    int is_ecc_2bits,
    int is_rpb_a,
    int index,
    char *special_msg,
    sand_intr_tcam_location * location);
#endif

int dnxc_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    dnxc_int_corr_act_type corr_act,
    char *special_msg);

int dnxc_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg_print,
    char *msg);

#ifdef BCM_DNXF_SUPPORT
int dnxc_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

int dnxc_interrupt_handles_corrective_action_for_rtp_slsct(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * shadow_correct_info_p,
    char *msg);
#endif

int dnxc_interrupt_data_collection_for_shadowing(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *special_msg,
    dnxc_int_corr_act_type * p_corrective_action,
    dnxc_interrupt_mem_err_info * shadow_correct_info);
int dnxc_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg,
    dnxc_int_corr_act_type corr_act,
    void *param1,
    void *param2);
int dnxc_interrupt_handles_corrective_action_tcam_query_failure(
    int unit,
    int block_instance,
    char *msg);
int dnxc_decide_interrupt_action_fsrd_SyncStatusChanged(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg);
int dnxc_decide_interrupt_action_fsrd_RxLockChanged(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg);
int dnxc_decide_interrupt_action_fmac_tx_ilkn_overflow(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg);
int dnxc_decide_interrupt_action_fmac_tx_ilkn_underrun(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg);
int dnxc_decide_interrupt_action_fmac_print_link(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg,
    const char *msg);

int dnxc_decide_interrupt_action_fmac_tuning_serdes(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg);

#endif
