/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header correction action functions for jericho interrupts.
 */

#ifndef _DCMN_INTR_CORR_ACT_FUNC_H_
#define _DCMN_INTR_CORR_ACT_FUNC_H_


#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/ARAD/arad_tcam.h>
#endif
#include <soc/dcmn/dcmn_port.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/sand/sand_intr_corr_act_func.h>


#define DCMN_INTERRUPT_PRINT_MSG_SIZE               512
#define DCMN_INTERRUPT_COR_ACT_MSG_SIZE             36
#define DCMN_INTERRUPT_SPECIAL_MSG_SIZE             1000

#define DCMN_INT_BIT_PER_MAC_INT_TYPE               4
#define DCMN_INT_BIT_PER_FSRD_QUAD_INT_TYPE         4
#define DCMN_INT_REGS_PER_FSRD_QUAD_INT_TYPE        3

typedef enum {
    DCMN_INT_CORR_ACT_CLEAR_CHECK                   =   SAND_INT_CORR_ACT_CLEAR_CHECK,
    DCMN_INT_CORR_ACT_CONFIG_DRAM                   =   SAND_INT_CORR_ACT_CONFIG_DRAM,
    DCMN_INT_CORR_ACT_ECC_1B_FIX                    =   SAND_INT_CORR_ACT_ECC_1B_FIX,
    DCMN_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY         =   SAND_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_FORCE                         =   SAND_INT_CORR_ACT_FORCE,
    DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO       =   SAND_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO        =   SAND_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO        =   SAND_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO   =   SAND_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO,
    DCMN_INT_CORR_ACT_HARD_RESET                    =   SAND_INT_CORR_ACT_HARD_RESET,
    DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC     =   SAND_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC,
    DCMN_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY          =   SAND_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY          =   SAND_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_INGRESS_HARD_RESET            =   SAND_INT_CORR_ACT_INGRESS_HARD_RESET,
    DCMN_INT_CORR_ACT_IPS_QDESC                     =   SAND_INT_CORR_ACT_IPS_QDESC,
    DCMN_INT_CORR_ACT_NONE                          =   SAND_INT_CORR_ACT_NONE,
    DCMN_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY         =   SAND_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_PRINT                         =   SAND_INT_CORR_ACT_PRINT,
    DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE            =   SAND_INT_CORR_ACT_REPROGRAM_RESOURCE,
    DCMN_INT_CORR_ACT_RTP_LINK_MASK_CHANGE          =   SAND_INT_CORR_ACT_RTP_LINK_MASK_CHANGE,
    DCMN_INT_CORR_ACT_RX_LOS_HANDLE                 =   SAND_INT_CORR_ACT_RX_LOS_HANDLE,
    DCMN_INT_CORR_ACT_SHADOW                        =   SAND_INT_CORR_ACT_SHADOW,
    DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET         =   SAND_INT_CORR_ACT_SHADOW_AND_SOFT_RESET,
    DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS            =   SAND_INT_CORR_ACT_SHUTDOWN_FBR_LINKS,
    DCMN_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION  =   SAND_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION,
    DCMN_INT_CORR_ACT_SOFT_RESET                    =   SAND_INT_CORR_ACT_SOFT_RESET,
    DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB        =   SAND_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB,
    DCMN_INT_CORR_ACT_RTP_SLSCT                     =   SAND_INT_CORR_ACT_RTP_SLSCT,
    DCMN_INT_CORR_ACT_SHUTDOWN_LINKS                =   SAND_INT_CORR_ACT_SHUTDOWN_LINKS,
    DCMN_INT_CORR_ACT_MC_RTP_CORRECT                =   SAND_INT_CORR_ACT_MC_RTP_CORRECT,
    DCMN_INT_CORR_ACT_UC_RTP_CORRECT                =   SAND_INT_CORR_ACT_UC_RTP_CORRECT,
    DCMN_INT_CORR_ACT_ALL_REACHABLE_FIX             =   SAND_INT_CORR_ACT_ALL_REACHABLE_FIX,
    DCMN_INT_CORR_ACT_EVENT_READY                   =   SAND_INT_CORR_ACT_EVENT_READY,
    DCMN_INT_CORR_ACT_IPS_QSZ_CORRECT               =   SAND_INT_CORR_ACT_IPS_QSZ_CORRECT,
    DCMN_INT_CORR_ACT_EM_SOFT_RECOVERY              =   SAND_INT_CORR_ACT_EM_SOFT_RECOVERY,
    DCMN_INT_CORR_ACT_XOR_FIX                       =   SAND_INT_CORR_ACT_XOR_FIX,
    DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION           =   SAND_INT_CORR_ACT_DYNAMIC_CALIBRATION,
    DCMN_INT_CORR_ACT_EM_ECC_1B_FIX                 =   SAND_INT_CORR_ACT_EM_ECC_1B_FIX,
    DCMN_INT_CORR_ACT_INGRESS_SOFT_RESET            =   SAND_INT_CORR_ACT_INGRESS_SOFT_RESET,
    DCMN_INT_CORR_ACT_XOR_ECC_1B_FIX                =   SAND_INT_CORR_ACT_XOR_ECC_1B_FIX,
    DCMN_INT_CORR_ACT_MAX                           =   SAND_INT_CORR_ACT_MAX
} dcmn_int_corr_act_type;


#define DCMN_INTERRUPT_IPT_MAX_CRC_DELETED_FIFO_DEPTH   16


typedef struct {
    soc_mem_t mem;
    unsigned int array_index;
    int copyno;
    int min_index;
    int max_index;
} dcmn_interrupt_mem_err_info;

typedef struct dcmn_intr_action_s
{
    soc_handle_interrupt_func func_arr;
    dcmn_int_corr_act_type corr_action;
} dcmn_intr_action_t;


    
int dcmn_mem_decide_corrective_action(int unit,sand_memory_dc_t type,soc_mem_t mem,int copyno, dcmn_int_corr_act_type *action_type, char* special_msg);

char* dcmn_mem_ser_ecc_action_info_get(int unit, soc_mem_t mem, int isEcc1b);


int 
dcmn_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);

int
dcmn_interrupt_handles_corrective_action_for_xor(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* xor_correct_info,
    char* msg,
    int clear,
    int ecc1bit);

int
dcmn_interrupt_handles_corrective_action_for_em_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* xor_correct_info,
    char* msg);

int 
dcmn_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg);
int
dcmn_interrupt_handles_corrective_action_do_nothing (
  int unit,
  int block_instance,
  uint32 interrupt_id,
  char *msg);
    
int dcmn_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);

int dcmn_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);        

int
dcmn_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    dcmn_int_corr_act_type corr_act,
    char *special_msg);

int
dcmn_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char* msg_print,
    char* msg);

#ifdef BCM_DPP_SUPPORT
int dcmn_interrupt_fmac_link_get(
    int unit,
    int fmac_block_instance,
    int bit_in_field,
    int *p_link);
int dcmn_interrupt_data_collection_for_mac_oof_int(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    dcmn_int_corr_act_type* p_corrective_action,
    char* special_msg);
int dcmn_interrupt_data_collection_for_mac_decerr_int(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int is_recurring,
    dcmn_int_corr_act_type* p_corrective_action,
    char* special_msg);
int dcmn_interrupt_data_collection_for_mac_transmit_err_int(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char* special_msg,
    dcmn_int_corr_act_type* p_corrective_action);
int
dcmn_interrupt_handles_corrective_action_for_ips_qsz(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);

int
dcmn_interrupt_data_collection_for_tcamprotectionerror(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    ARAD_TCAM_LOCATION *location,
    dcmn_int_corr_act_type* corrective_action);

int
dcmn_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    ARAD_TCAM_LOCATION* location,
    char* msg);
#endif

#ifdef BCM_DFE_SUPPORT
int
dcmn_interrupt_handles_corrective_action_for_rtp_slsct(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);
#endif

int
dcmn_interrupt_data_collection_for_shadowing(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char* special_msg,
    dcmn_int_corr_act_type* p_corrective_action,
    dcmn_interrupt_mem_err_info* shadow_correct_info);
int
dcmn_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg,
    dcmn_int_corr_act_type corr_act,
    void *param1,
    void *param2);
int dcmn_interrupt_handles_corrective_action_ingress_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg);
int dcmn_interrupt_fsrd_link_get(int unit,
                                int block_instance,
                                int bit_in_field,
                                int reg_index,
                                int *p_link);

#endif 
