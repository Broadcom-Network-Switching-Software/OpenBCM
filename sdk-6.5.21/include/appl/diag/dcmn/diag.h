/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag.h
 * Purpose:     Device diagnostics commands.
 */


#ifndef _DIAG_H_INCLUDED_
#define _DIAG_H_INCLUDED_

#include <soc/chip.h>
#include <appl/diag/diag.h>

#if (!defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT))
#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#endif

/* 
 *DNX Diag pack infrastructure 
 */
#define DNX_DIAG_ID_UNAVAIL     (-1)

typedef cmd_result_t (*diag_dnx_action_func_ptr_t)(int unit, args_t* a);
typedef void (*diag_dnx_usage_func_ptr_t)(int unit);
typedef int (*diag_dnx_diag_id_supported_func_ptr_t)(int unit, int diag_id);

typedef struct diag_dnx_table_s {
    char*                       module_char;    /*Command name*/
    diag_dnx_action_func_ptr_t  action_func;    /*Callback to command fuction*/
    diag_dnx_usage_func_ptr_t   usage_func;     /*Callback to command usage function*/
    int                         diag_id;        /*diag id - used in order to make sure this command is supported by the device*/
} diag_dnx_table_t;

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      print the usage of all the supported commands by this unit. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 */
void diag_dnx_usage_print(int unit,
                          const diag_dnx_table_t *diag_pack, 
                          diag_dnx_diag_id_supported_func_ptr_t diag_id_support);

/*
 * Function: 
 *      diag_dnx_usage_print 
 * Purpose: 
 *      Dispatch according to command name and if the diag_id is supported. 
 * Parameters: 
 *      unit            - (IN) Unit number.
 *      diag_pack       - (IN) Diag pack table.
 *      diag_id_support - (IN) Callback to a function that checks if diag_id supported.
 *      cmd_name        - (IN) Command full name.
 *      args            - (IN) args required by the command.
 */
cmd_result_t diag_dnx_dispatch(int unit,
                  const diag_dnx_table_t *diag_pack,
                  diag_dnx_diag_id_supported_func_ptr_t diag_id_support,
                  char *cmd_name,
                  args_t *args);

int diag_dcmn_phy_measure_port(int unit, bcm_port_t port, bcm_stat_val_t type,uint32* rate_int, uint32* rate_remainder);

cmd_result_t dpp_do_dump_table(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, int count, int flags);
/* read or write wide memory, supports ARAD only */
cmd_result_t
   _soc_arad_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,
                   unsigned operation)  ;/* operation should be 1 for read and 0 for write */

/* Get the root node for the diags list and the top node in the file(creates the file if does not exist)  */
int dnx_diag_xml_root_get(int unit, char* xml_file, void  **curTop, void **curRoot);

#if (!defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT))
/*************************************** 
* Writes last packet information 
* to info parameter
 */
    cmd_result_t
dpp_diag_last_packet_info_get(int unit, int core, SOC_TMC_DIAG_LAST_PACKET_INFO *info);
#endif

#define DIAG_COUNTERS_F_ARADPLUS_ONLY 0x1

#define DIAG_COUNTERS_F_JERICHO_ONLY 0x2
#define DIAG_COUNTERS_F_JERICHOPLUS_ONLY 0x4

typedef struct diag_counter_data_s {
  char*             reg_name;
  soc_block_type_t  block;
  soc_reg_t         reg;
  unsigned          reg_index; /* register index, should be 0 when not a register array. */
  soc_field_t       cnt_field;
  soc_field_t       overflow_field;
  uint32            flags;
} diag_counter_data_t;

typedef enum jericho_graphic_counters {

    EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER,
    EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER,
    EGQ_ERPP_LAG_PRUNING_DISCARDS_COUNTER,
    EGQ_ERPP_PMF_DISCARDS_COUNTER,
    EGQ_ERPP_VLAN_MEMBERSHIP_DISCARDS_COUNTER,
    EGQ_EHP_DISCARD_PACKET_COUNTER, 	
    EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER, 	
    EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER, 	
    EGQ_EHP_UNICAST_PACKET_COUNTER, 	
    EGQ_FQP_PACKET_COUNTER, 	
    EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER, 	
    EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER, 	
    EGQ_PQP_MULTICAST_BYTES_COUNTER, 	
    EGQ_PQP_MULTICAST_PACKET_COUNTER, 	
    EGQ_PQP_UNICAST_BYTES_COUNTER, 	
    EGQ_PQP_UNICAST_PACKET_COUNTER,
    EPNI_EPE_BYTES_COUNTER, 	
    EPNI_EPE_DISCARDED_PACKETS_COUNTER, 	
    EPNI_EPE_PACKET_COUNTER,
    FDA_EGQ_N_CELLS_IN_CNT_IPT0,
    FDA_EGQ_N_CELLS_IN_CNT_IPT1,
    FDA_EGQ_N_CELLS_IN_CNT_MESHMC0,
    FDA_EGQ_N_CELLS_IN_CNT_MESHMC1,
    FDA_EGQ_N_CELLS_IN_CNT_TDM0,
    FDA_EGQ_N_CELLS_IN_CNT_TDM1,
    FDA_EGQ_N_CELLS_OUT_CNT_IPT0,
    FDA_EGQ_N_CELLS_OUT_CNT_IPT1,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_10,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_11,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_00,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_01,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_20,
    FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_21,
    FDA_EGQ_N_CELLS_OUT_CNT_TDM0,
    FDA_EGQ_N_CELLS_OUT_CNT_TDM1,
    FDA_EGQ_N_CELLS_OUT_CNT_MESHMC0,
    FDA_EGQ_N_CELLS_OUT_CNT_MESHMC1,
    FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_0_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_1_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_FABFIF_2_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT1,
    FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT0,
    FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT1,
    FDA_EGQ_N_TDM_OVF_DROP_CNT0,
    FDA_EGQ_N_TDM_OVF_DROP_CNT1,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_1,
    FDA_FAB_48_SCH_CELLS_IN_CNT_P_1,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_2,
    FDA_FAB_48_SCH_CELLS_IN_CNT_P_2,
    FDA_FAB_36_SCH_CELLS_IN_CNT_P_3,
    FDA_FAB_48_SCH_CELLS_IN_CNT_P_3,
    FDR_CELL_IN_CNT,
    FDR_P_1_CELL_IN_CNT,
    FDR_P_2_CELL_IN_CNT,
    FDR_P_3_CELL_IN_CNT,
    FDT_DESC_CELL_CNT,
    FDT_IRE_DESC_CELL_CNT,
    FDT_DATA_CELL_CNT,
    IDR_MMU_INTERFACE_COUNTER,
    IDR_OCB_INTERFACE_COUNTER,
    IPT_CRC_ERR_CNT,
    IPT_CFG_EVNT_CNT,
    IPT_CFG_BYT_CNT,
    IPT_EGQ_0_PKT_CNT,
    IPT_EGQ_1_PKT_CNT,
    IPT_ENQ_0_PKT_CNT,
    IPT_ENQ_1_PKT_CNT,
    IPT_FDT_0_PKT_CNT,
    IPT_FDT_1_PKT_CNT,
    IQM_DEQUEUE_PACKET_COUNTER,
    IQM_ENQUEUE_PACKET_COUNTER,
    IQM_QUEUE_DELETED_PACKET_COUNTER,
    IQM_QUEUE_ENQ_DISCARDED_PACKET_COUNTER,
    IRE_CPU_PACKET_COUNTER,
    IRE_FDT_INTERFACE_COUNTER,
    IRE_NIF_N_PACKET_COUNTER0,
    IRE_NIF_N_PACKET_COUNTER1,
    IRE_OAMP_PACKET_COUNTER,
    IRE_OLP_PACKET_COUNTER,
    IRE_RCY_N_PACKET_COUNTER0,
    IRE_RCY_N_PACKET_COUNTER1,
    IRE_RCY_N_PACKET_COUNTER2,
    IRE_RCY_N_PACKET_COUNTER3,
    MMU_IDR_PACKET_COUNTER,
    NBIH_RX_TOTAL_BYTE_COUNTER,
    NBIH_RX_TOTAL_PKT_COUNTER,
    NBIH_RX_NUM_TOTAL_DROPPED_EOPS,
    NBIH_TX_TOTAL_BYTE_COUNTER,
    NBIH_TX_TOTAL_PKT_COUNTER,
    NBIL_RX_NUM_TOTAL_DROPPED_EOPS,
    NOF_GRAPHIC_COUNTERS    /*always last*/

} jericho_graphic_counters_e;

typedef enum jericho_rqp_graphic_counters {
    EGQ_PRP_PACKET_GOOD_UC_CNT0,
    EGQ_PRP_PACKET_GOOD_UC_CNT1,
    EGQ_PRP_PACKET_GOOD_MC_CNT0,
    EGQ_PRP_PACKET_GOOD_MC_CNT1,
    EGQ_PRP_PACKET_GOOD_TDM_CNT0,
    EGQ_PRP_PACKET_GOOD_TDM_CNT1,
    EGQ_PRP_PACKET_DISCARD_UC_CNT0,
    EGQ_PRP_PACKET_DISCARD_UC_CNT1,
    EGQ_PRP_PACKET_DISCARD_MC_CNT0,
    EGQ_PRP_PACKET_DISCARD_MC_CNT1,
    EGQ_PRP_PACKET_DISCARD_TDM_CNT0,
    EGQ_PRP_PACKET_DISCARD_TDM_CNT1,
    EGQ_PRP_SOP_DISCARD_UC_CNT0,
    EGQ_PRP_SOP_DISCARD_UC_CNT1,
    EGQ_PRP_SOP_DISCARD_MC_CNT0,
    EGQ_PRP_SOP_DISCARD_MC_CNT1,
    EGQ_PRP_SOP_DISCARD_TDM_CNT0,
    EGQ_PRP_SOP_DISCARD_TDM_CNT1,
    NOF_RQP_GRAPHIC_COUNTERS    /*always last*/
} jericho_rqp_graphic_counters_e;

typedef enum qax_graphic_counters {

    QAX_EGQ_EHP_DISCARD_PACKET_COUNTER, 	
    QAX_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER, 	
    QAX_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER, 	
    QAX_EGQ_EHP_UNICAST_PACKET_COUNTER, 	
    QAX_EGQ_FQP_PACKET_COUNTER, 	
    QAX_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER, 	
    QAX_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER, 	
    QAX_EGQ_PQP_MULTICAST_BYTES_COUNTER, 	
    QAX_EGQ_PQP_MULTICAST_PACKET_COUNTER, 	
    QAX_EGQ_PQP_UNICAST_BYTES_COUNTER, 	
    QAX_EGQ_PQP_UNICAST_PACKET_COUNTER,
    QAX_EPNI_EPE_BYTES_COUNTER, 	
    QAX_EPNI_EPE_DISCARDED_PACKETS_COUNTER, 	
    QAX_EPNI_EPE_PACKET_COUNTER,
    QAX_FDA_EGQ_N_CELLS_IN_CNT_IPT,
    QAX_FDA_EGQ_N_CELLS_IN_CNT_MESHMC,
    QAX_FDA_EGQ_N_CELLS_IN_CNT_TDM,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_IPT,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_1,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_FAB_PIPE_2,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_TDM,
    QAX_FDA_EGQ_N_CELLS_OUT_CNT_MESHMC,
    QAX_FDA_EGQ_N_MESHMC_PRIO_0_DROP_CNT,
    QAX_FDA_EGQ_N_MESHMC_PRIO_1_DROP_CNT,
    QAX_FDA_EGQ_N_MESHMC_PRIO_2_DROP_CNT,
    QAX_FDA_EGQ_N_TDM_OVF_DROP_CNT,
    QAX_FDR_CELL_IN_CNT,
    QAX_FDR_P_1_CELL_IN_CNT,
    QAX_FDR_P_2_CELL_IN_CNT,
    QAX_FDR_P_3_CELL_IN_CNT,
    QAX_FDT_DESC_CELL_CNT,
    QAX_FDT_IRE_DESC_CELL_CNT,
    QAX_FDT_DATA_CELL_CNT,
    /*QAX_IDR_MMU_INTERFACE_COUNTER,*/
    /*QAX_IDR_OCB_INTERFACE_COUNTER,*/
    /*QAX_IPT_CRC_ERR_CNT,*/
    /*QAX_IPT_CFG_EVNT_CNT,
    QAX_IPT_CFG_BYT_CNT,
    QAX_IPT_FDT_0_PKT_CNT,
    QAX_IPT_FDT_1_PKT_CNT,*/
    QAX_CGM_VOQ_SRAM_DEQ_PACKET_COUNTER,
    QAX_CGM_VOQ_SRAM_ENQ_PACKET_COUNTER,
    QAX_CGM_VOQ_SRAM_DEL_PACKET_COUNTER,
    QAX_CGM_VOQ_SRAM_ENQ_RJCT_PACKET_COUNTER,
    QAX_CGM_VOQ_DRAM_DEQ_PACKET_COUNTER,
    QAX_CGM_VOQ_DRAM_ENQ_PACKET_COUNTER,
    QAX_CGM_VOQ_DRAM_DEL_PACKET_COUNTER,
    QAX_IRE_CPU_PACKET_COUNTER,
    QAX_IRE_FDT_INTERFACE_COUNTER,
    QAX_IRE_NIF_PACKET_COUNTER,
    QAX_IRE_OAMP_PACKET_COUNTER,
    QAX_IRE_OLP_PACKET_COUNTER,
    QAX_IRE_RCY_PACKET_COUNTER,
    QAX_MMU_IDR_PACKET_COUNTER,
    QAX_SPB_ALMOST_FULL_0_SRAM_REJECT_COUNTER,
    QAX_SPB_ALMOST_FULL_1_SRAM_REJECT_COUNTER,
    QAX_SPB_ALMOST_FULL_2_SRAM_REJECT_COUNTER,
    QAX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER,
    QAX_NBIH_RX_TOTAL_BYTE_COUNTER,
    QAX_NBIH_RX_TOTAL_PKT_COUNTER,
    QAX_NBIH_RX_NUM_TOTAL_DROPPED_EOPS,
    QAX_NBIH_TX_TOTAL_BYTE_COUNTER,
    QAX_NBIH_TX_TOTAL_PKT_COUNTER,
    QAX_NBIL_RX_NUM_TOTAL_DROPPED_EOPS,
    QAX_TXQ_EGQ_SRAM_COUNTER,
    QAX_TXQ_EGQ_DRAM_COUNTER,
    QAX_TXQ_FDT_SRAM_COUNTER,
    QAX_TXQ_FDT_DRAM_COUNTER,
    QAX_NOF_GRAPHIC_COUNTERS    /*always last*/

} qax_graphic_counters_e;

typedef enum qux_graphic_counters {
    /** EHP */
    QUX_EGQ_EHP_DISCARD_PACKET_COUNTER,
    QUX_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER,
    QUX_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER,
    QUX_EGQ_EHP_UNICAST_PACKET_COUNTER,
    QUX_EGQ_EHP_MULTICAST_HIGH_DISCARDS_COUNTER,
    QUX_EGQ_EHP_MULTICAST_LOW_DISCARDS_COUNTER,

    /** FQP */
	QUX_EGQ_FQP_CELL_COUNTER,
    QUX_EGQ_FQP_PACKET_COUNTER,
    /** PQP */
    QUX_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER,
    QUX_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER,
    QUX_EGQ_PQP_MULTICAST_BYTES_COUNTER,
    QUX_EGQ_PQP_MULTICAST_PACKET_COUNTER,
    QUX_EGQ_PQP_UNICAST_BYTES_COUNTER,
    QUX_EGQ_PQP_UNICAST_PACKET_COUNTER,

    QUX_EPNI_EPE_BYTES_COUNTER,
    QUX_EPNI_EPE_DISCARDED_PACKETS_COUNTER,
    QUX_EPNI_EPE_PACKET_COUNTER,
    QUX_EPNI_EPE_MIRRORED_PACKETS_COUNTER,

    QUX_CGM_VOQ_SRAM_DEQ_PACKET_COUNTER,
    QUX_CGM_VOQ_SRAM_ENQ_PACKET_COUNTER,
    QUX_CGM_VOQ_SRAM_DEL_PACKET_COUNTER,
    QUX_CGM_VOQ_SRAM_ENQ_RJCT_PACKET_COUNTER,
    QUX_CGM_VOQ_DRAM_DEQ_PACKET_COUNTER,
    QUX_CGM_VOQ_DRAM_ENQ_PACKET_COUNTER,
    QUX_CGM_VOQ_DRAM_DEL_PACKET_COUNTER,
    QUX_CGM_QNUM_NOT_VALID_PACKETS,

    QUX_IRE_CPU_PACKET_COUNTER,
    QUX_IRE_NIF_PACKET_COUNTER,
    QUX_IRE_OAMP_PACKET_COUNTER,
    QUX_IRE_OLP_PACKET_COUNTER,
    QUX_IRE_RCY_PACKET_COUNTER,
    QUX_IRE_IPSEC_PACKET_COUNTER,
    QUX_IRE_SAT_PACKET_COUNTER,

    QUX_MMU_RD_REQ_COUNTER,
    QUX_MMU_WR_REQ_COUNTER,
    QUX_MMU_SPB_GOOD_PACKET_CNT,
    QUX_MMU_SPB_PACKET_ERROR_CNT,
    QUX_MMU_DRAM_GOOD_PACKET_CNT,
    QUX_MMU_DRAM_PACKET_CRC_ERROR_CNT,
    QUX_MMU_DRAM_PACKET_PKUP_DISCARD_CNT,

    QUX_SPB_ALMOST_FULL_0_SRAM_REJECT_COUNTER,
    QUX_SPB_ALMOST_FULL_1_SRAM_REJECT_COUNTER,
    QUX_SPB_ALMOST_FULL_2_SRAM_REJECT_COUNTER,
    QUX_SPB_ALMOST_FULL_SRAM_REJECT_COUNTER,
    QUX_SPB_FULL_SRAM_REJECT_COUNTER,
    QUX_SPB_DISCARDED_PACKETS_CNT_0,
    QUX_SPB_DISCARDED_PACKETS_CNT_1,
    QUX_SPB_DISCARDED_PACKETS_CNT_2,
    QUX_SPB_DISCARDED_PACKETS_CNT_3,

    QUX_NIF_RX_TOTAL_BYTE_COUNTER,
    QUX_NIF_RX_TOTAL_PKT_COUNTER,
    QUX_NIF_RX_NUM_TOTAL_DROPPED_EOPS,
    QUX_NIF_TX_TOTAL_BYTE_COUNTER,
    QUX_NIF_TX_TOTAL_PKT_COUNTER,
    QUX_TXQ_EGQ_SRAM_COUNTER,
    QUX_TXQ_EGQ_DRAM_COUNTER,
    QUX_NOF_GRAPHIC_COUNTERS    /*always last*/
} qux_graphic_counters_e;

typedef enum qux_rqp_graphic_counters {
    QUX_EGQ_PRP_PACKET_GOOD_UC_CNT,
    QUX_EGQ_PRP_PACKET_GOOD_MC_CNT,
    QUX_EGQ_PRP_PACKET_DISCARD_UC_CNT,
    QUX_EGQ_PRP_PACKET_DISCARD_MC_CNT,
    QUX_EGQ_PRP_SOP_DISCARD_UC_CNT,
    QUX_EGQ_PRP_SOP_DISCARD_MC_CNT,
    QUX_EGQ_PRP_PACKET_IN_CNT,
    QUX_EGQ_PQP_CELL_IN_CNT,
    NOF_QUX_RQP_GRAPHIC_COUNTERS    /*always last*/
} qux_rqp_graphic_counters_e;


typedef enum qax_rqp_graphic_counters {
    ASLALSLSALSA
} qax_rqp_graphic_counters_e;

#endif /*_DIAG_H_INCLUDED_*/
