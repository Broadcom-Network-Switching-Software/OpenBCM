/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DFE_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_
#define _DFE_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_

/*************
 * INCLUDES  *
 */
#include <soc/dcmn/dcmn_intr_corr_act_func.h>

/* 
 *  Defines
  */

#define FE1600_INT_LINK_NUM_IN_SHUTDOWN_LINK_BLOCK    32
#define FE1600_INT_BIT_PER_MAC_INT_TYPE               4

#define FE1600_INT_CRC_THRESHOLD                      10000
#define FE1600_INT_FEC_UNCORRECTABLE_THRESHOLD          1
#define FE1600_INT_CRC_MASK                           0x00000000ffffffff
#define FE1600_INT_FEC_UNCORRECTABLE_MASK             0x0000ffff
#define FE1600_INT_LINKS_NUM_IN_DCH_BLOCK             32
#define FE1600_INT_FMAC_BLOCKS_BITS_NUM_PER_INTERRUPT 4
#define FE1600_INT_BIT_PER_FSRD_QUAD_INT_TYPE         4
#define FE1600_INT_REGS_PER_FSRD_QUAD_INT_TYPE        4
#define FE1600_INT_LINK_INVOLVED                      1
#define FE1600_INT_LINK_SHUTDOWN                      0
#define FE1600_INTERRUPT_SPECIAL_MSG_SIZE             256
#define FE1600_INTERRUPT_COR_ACT_MSDG_SIZE            128
#define FE1600_INTERRUPT_PRINT_MSG_SIZE               512
#define FE1600_INTERRUPT_PRIVATE_STRING_SIZE          20
#define FE1600_INTERRUPT_LINKS_NUM_PER_DCH_BLOCK      32
#define FE1600_INTERRUPT_MAX_RTP_COPIES                       8
#define FE1600_INTERRUPT_NOF_LOOPS_TOO_MANY_DIFF_TABLES       5
#define FE1600_INTERRUPT_MULTICAST_BIT                            19
#define FE1600_INTERRUPT_MAX_TABLES_FOR_RTP_TABLE_NON_REPAIR  2
#define FE1600_INTERRUPT_DESTINATION_ID_MASK            0x7ffff
#define FE1600_INTERRUPT_DEFAULT_TABLES_NUM           8
#define FE1600_INTERRUPT_BALANCING_TABLES_NUM         4
#define FE1600_INTERRUPT_MESSAGE_FOR_CONVERSION       32
#define FE1600_INTERRUPT_RTP_BLOCK                    0x12
#define FE1600_INTERRUPT_MAX_BLOCK_NUM                0x3f
#define FE1600_INTERRUPT_MAX_MEMORY_ADDRESS           0x3ffffff
#define FE1600_INTERRUPT_DIRTY_BIT_VECTOR_SIZE_TO_OVERRIDE_UNICAST_TABLE 32

/* 
 *  Enums
  */
typedef enum {
    FE1600_INT_MULTICAST_TABLES_EQUAL = 0x0,
    FE1600_INT_MULTICAST_TABLES_SHOULD_BE_CORRECTED,
    FE1600_INT_MULTICAST_TABLES_TOO_MANY_TRIALS_WITH_INCORRECT_DIFFERENT_TABLES,
    FE1600_INT_MULTICAST_TABLES_LAST
}fe1600_int_multicast_tables_status_type;


typedef enum {
    FE1600_INT_UNICAST_DEST_MODE = 0x0,
    FE1600_INT_MUTICAST_DEST_MODE,
    FE1600_INT_LAST_DEST_MODE
}fe1600_int_dest_mode_type;

/* 
 *  Structs
  */
typedef struct
{
  int first_link;
  int last_link;
}fe1600_links ;


typedef struct {
    uint32 links_bitmap[4];
} fe1600_interrupt_links_t;



typedef struct {
    /* unreachable destination counter */
    uint32 un_reach_dest_cnt;
    /* overflow of destination counter */
    uint32 un_reach_dest_cnt_o;
    /* dest id */
    uint32 dest_id;
    /* dest mode */
    fe1600_int_dest_mode_type dest_mode;
} fe1600_interrupt_unreachable_dest_info;


typedef struct {
    fe1600_int_multicast_tables_status_type multicast_tables_status;
    int valid_table_id;
} fe1600_interrupt_multicast_tables_info;


typedef struct {
    uint32 multicast_id;
    int tables_num;
    int valid_table_id;
} fe1600_interrupt_mc_rtp_table_correct_info;

/* 
 *  Functions
  */

/* Corrective Action main function */
int fe1600_interrupt_handles_corrective_action(int unit, int block_instance, fe1600_interrupt_type interrupt_id, char *msg, dcmn_int_corr_act_type corr_act, void *param1, void *param2);

/*
 *  Corrective Action functions    
 */
int fe1600_interrupt_handles_corrective_action_shutdown_link(int unit, int block_instance, fe1600_interrupt_type interrupt_id,uint8* array_links,char* msg);
int fe1600_interrupt_handles_corrective_action_force(int unit, int block_instance, fe1600_interrupt_type interrupt_id, char *msg);
int fe1600_interrupt_handles_corrective_action_mc_rtp_correct(int unit, int block_instance, fe1600_interrupt_type interrupt_id, fe1600_interrupt_mc_rtp_table_correct_info* scrub_shadow_write_info_p, char* msg);
int fe1600_interrupt_handles_corrective_action_uc_rtp_correct(int unit, int block_instance, fe1600_interrupt_type interrupt_id, int* route_update_dirty_index, char* msg);
int fe1600_interrupt_handles_corrective_action_all_reachable_fix(int unit, int block_instance, fe1600_interrupt_type interrupt_id, char* msg);
/*
 *  Corrective Action functions    
 */

/*
*  common function utilities for the same interrupts
*/
int fe1600_interrupt_data_collection_for_recuring_action_dch_alto(int unit,int block_instance,fe1600_interrupt_type en_fe1600_interrupt, dcmn_int_corr_act_type * p_corrective_action,uint8* array_links, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_dch_ifm(int unit,int block_instance,fe1600_interrupt_type en_fe1600_interrupt,dcmn_int_corr_act_type * p_corrective_action,char* special_msg);
int fe1600_interrupt_data_collection_for_handle_mac_rxcrcerrn_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, uint8* array_links, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_mac_wrongsize_int(int unit,int block_instance, fe1600_interrupt_type en_fe1600_interrupt,char* special_msg);
int fe1600_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(int unit,int block_instance, fe1600_interrupt_type en_fe1600_interrupt,char* special_msg ,uint8* array_links);
int fe1600_interrupt_data_collection_for_handle_mac_oof_int( int unit,int block_instance,fe1600_interrupt_type en_fe1600_inter, soc_dcmn_port_pcs_t* p_pcs,char* special_msg);
int fe1600_interrupt_data_collection_for_recurring_mac_oof_int( int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, soc_dcmn_port_pcs_t* p_pcs, dcmn_int_corr_act_type* p_corrective_action,char* special_msg, uint8* array_links);
int fe1600_interrupt_data_collection_for_handle_mac_decerr_int( int unit,int block_instance, fe1600_interrupt_type en_fe1600_interrupt, soc_dcmn_port_pcs_t* p_pcs, dcmn_int_corr_act_type* p_corrective_action, char* special_msg, uint8* array_links);
int fe1600_interrupt_data_collection_for_mac_transmit_err_int( int unit,int block_instance,fe1600_interrupt_type en_fe1600_interrupt, char* special_msg, dcmn_int_corr_act_type* p_corrective_action, uint8* array_links);
int fe1600_interrupt_data_collection_for_srd_fsrd_syncstatuschanged( int unit,int block_instance,fe1600_interrupt_type en_fe1600_interrupt, soc_port_t *link, char* special_msg);
int fe1600_interrupt_data_collection_for_mac_lnklvlagen_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_mac_lnklvlagectxbn_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_ccs_cpucaptcellfneint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_mac_los_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_mac_rxlostofsync_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, soc_port_t *link, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_dcl_ccpsrcdcnglink_int(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationacove1kint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationabovemaxbaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_rtp_drh_querydestinationaboveupdatebaseindexint(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_rtp_drhp_queryunreachablemulticast(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_rtp_drhs_queryunreachablemulticast(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg);
int fe1600_interrupt_data_collection_for_handle_unreachable_destination(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg, dcmn_int_corr_act_type* p_corrective_action, int* corr_act_unicast_index, fe1600_interrupt_mc_rtp_table_correct_info* mc_rtp_table_correct_info_p);
int fe1600_interrupt_rtp_table_inconsistent_data_collection(int unit, int block_instance, int tables_num, uint32 multicast_id, fe1600_interrupt_multicast_tables_info* multicast_tables_info);
int fe1600_interrupt_data_collection_for_handle_rtp_err(int unit, int block_instance, fe1600_interrupt_type en_fe1600_interrupt, char* special_msg, dcmn_int_corr_act_type* p_corrective_action, int* corr_act_unicast_index, fe1600_interrupt_mc_rtp_table_correct_info* mc_rtp_table_correct_info_p, dcmn_interrupt_mem_err_info* shadow_correct_info);
int fe1600_interrupt_get_multicast_tables_number(int unit, int* tables_number);
int fe1600_interrupt_get_unreachable_p_dest_info(int unit, int block_instance, fe1600_interrupt_unreachable_dest_info* unreachable_dest_info_p);
int fe1600_interrupt_get_unreachable_s_dest_info(int unit, int block_instance, fe1600_interrupt_unreachable_dest_info* unreachable_dest_info_p);
/* 
*  
*  
*    standard function utilities 
*/
int fe1600_interrupt_mct_addr_to_array_element_and_index(int unit, uint32 address, int* numels, int* index);
int fe1600_interrupt_convert_memory_address(int unit, uint32 block, uint32 address, uint32* converted_address);
int fe1600_interrupt_fmac_link_get(int unit,int fmac_block_instance,int bit_in_field, int *p_link);
int fe1600_interrupt_dch_links_get( int unit,int dch_block_instance,fe1600_links *links );
int fe1600_interrupt_fsrd_link_get(int unit, int block_instance, int bit_in_field, int reg_index, int *p_link);

#endif /* _DFE_INTERRUPT_HANDLER_CORR_ACT_FUNC_H_ */
