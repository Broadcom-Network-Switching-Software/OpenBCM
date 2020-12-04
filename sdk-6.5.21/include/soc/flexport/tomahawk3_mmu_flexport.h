/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk3_mmu_flexport.h
*/



#ifndef _SOC_TOMAHAWK3_MMU_FLEXPORT_H_
#define _SOC_TOMAHAWK3_MMU_FLEXPORT_H_


#include <soc/flexport/flexport_common.h>


/*** START SDK API COMMON CODE ***/

/*! @file tomahawk3_mmu_flexport.h
 *  @brief Macros used in APIs of MMU for Tomahawk3.
 *  Details are shown below.
 */

#define TH3_MMU_NUM_L0_NODES_PER_HSP_PORT 12
#define TH3_MMU_NUM_L0_NODES_FOR_CPU_PORT 12
#define TH3_MMU_NUM_L1_NODES_PER_HSP_PORT 2
#define TH3_MMU_NUM_L1_NODES_FOR_CPU_PORT 48
#define TH3_MMU_NUM_Q_PER_HSP_PORT 12
#define TH3_MMU_NUM_Q_FOR_CPU_PORT 48

#define TH3_MMU_NUM_PORTS_PER_PIPE 32
#define TH3_MMU_NUM_PORTS_PER_PIPE_L 20
#define TH3_MMU_MAX_PORTS_PER_PIPE 32
#define TH3_MMU_NUM_HSP_PORTS_PER_PIPE 32
#define TH3_THDO_INVALID_DEV_PORT 255
#define TH3_INVALID_MMU_PORT 255
#define TH3_INVALID_DEV_PORT 160
#define TH3_MMU_PORT_PIPE_OFFSET 32
#define TH3_PHY_PORT_PIPE_OFFSET 32

#define TH3_MMU_CPU_PORT_NUM 19
#define TH3_MMU_MGMT0_PORT_NUM 51
#define TH3_MMU_MGMT1_PORT_NUM 179
#define TH3_MMU_LB0_PORT_NUM ((0*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB1_PORT_NUM ((1*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB2_PORT_NUM ((2*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB3_PORT_NUM ((3*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB4_PORT_NUM ((4*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB5_PORT_NUM ((5*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB6_PORT_NUM ((6*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))
#define TH3_MMU_LB7_PORT_NUM ((7*TH3_MMU_NUM_PORTS_PER_PIPE) + \
                                (TH3_MMU_NUM_PORTS_PER_PIPE_L-2))

#define TH3_MMU_BST_TRACK_MODE 0
#define TH3_MMU_BST_WMARK_MODE 1
#define TH3_MMU_NUM_PGS 8
#define TH3_MMU_NUM_SPS 4
#define TH3_MMU_NUM_COS 12
#define TH3_MMU_THDI_PG_MIN_LIMIT_LOSSLESS 7
#define TH3_MMU_THDI_PG_MIN_LIMIT_LOSSY 0
#define TH3_MMU_THDI_PG_HDRM_LIMIT_LOSSY 0
#define TH3_MMU_NUM_SPEEDS 7
#define TH3_MMU_TCT_ON_TO_CT_OFFSET 5

#define TH3_MMU_PAUSE_PG 7  /*PG7-Used for Initial PG_Min/hdrm + for Pause */

typedef enum pipe_s
{
    TH3_IPIPE,
    TH3_EPIPE
} pipe_t;

#define TH3_MMU_FLUSH_OFF 0
#define TH3_MMU_FLUSH_ON  1
#define TH3_MMU_THD_RX_ENABLED 1
#define TH3_MMU_THD_RX_DISABLED 0

#define TH3_MMU_IS_CPU_PORT(pipe, port) ((pipe == 0) && \
                                         (port == TH3_MMU_CPU_PORT_NUM))
#define TH3_MMU_IS_MGMT_PORT(pipe, port) \
                               ( ((pipe == 1) && (port == TH3_MMU_MGMT0_PORT_NUM)) || \
                                 ((pipe == 5) && (port == TH3_MMU_MGMT1_PORT_NUM)))
#define TH3_MMU_IS_LPBK_PORT(port) (port == TH3_MMU_LB0_PORT_NUM \
                                    || port == TH3_MMU_LB1_PORT_NUM \
                                    || port == TH3_MMU_LB2_PORT_NUM \
                                    || port == TH3_MMU_LB3_PORT_NUM \
                                    || port == TH3_MMU_LB4_PORT_NUM \
                                    || port == TH3_MMU_LB5_PORT_NUM \
                                    || port == TH3_MMU_LB6_PORT_NUM \
                                    || port == TH3_MMU_LB7_PORT_NUM \
                                    )
#define TH3_MMU_GET_PIPE_NUM(port, pipe) \
    pipe = port/TH3_MMU_PORT_PIPE_OFFSET
#define TH3_MMU_PIPE_NUM(port) port/TH3_MMU_PORT_PIPE_OFFSET



/* Some functions from MMU_CONFIG */
extern int soc_tomahawk3_mmu_get_num_queues_for_port(int unit, int pipe,
    int lcl_port,
    int *number_of_queues);

extern int soc_tomahawk3_mmu_get_valid_pipes_for_itm(int unit, int itm,
    int *pipes);

extern int soc_tomahawk3_mmu_get_itm_from_pipe_num(int unit, int pipe,
    int *itm_num);

extern void soc_tomahawk3_mmu_get_q_base_num_for_pipe(int unit, int pipe,
    int *pipe_q_base_num);

extern int soc_tomahawk3_mmu_get_chip_q_num_base_for_port(int unit, int pipe,
    int local_mmu_port,
    int *chip_q_base_num_for_port);


extern int soc_tomahawk3_mmu_get_thdi_bst_mode(int unit,
    int *thdi_bst_mode);

extern int soc_tomahawk3_mmu_get_thdo_bst_mode(int unit,
    int *thdo_bst_mode);


extern void soc_tomahawk3_mmu_get_pipe_flexed_status(
    int unit, soc_port_schedule_state_t *port_schedule_state_t, 
    int pipe,
    int *pipe_flexed);

extern int soc_tomahawk3_mmu_get_itm_flexed_status(
    int unit, soc_port_schedule_state_t *port_schedule_state_t,
    int itm,
    int *itm_flexed);


extern int soc_tomahawk3_mmu_thdi_get_pg_hdrm_setting(int unit, int speed,
    int line_rate,
    uint32 *setting);


/*PORT FLUSH Functions*/

extern int soc_tomahawk3_mmu_mtro_port_metering_config(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val_mtro);

extern int soc_tomahawk3_mmu_qsch_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val);

extern int soc_tomahawk3_mmu_ebqs_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val);

extern int soc_tomahawk3_mmu_rqe_port_flush(int unit,
    int itm,
    int call_iter);

extern int soc_tomahawk3_mmu_wait_ebctm_empty(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe);

/* THDI/WRED Clear Functions */
extern int soc_tomahawk3_mmu_thdi_bst_clr(int unit,
    soc_port_resource_t *port_resource_t);


extern int soc_tomahawk3_mmu_wred_clr(int unit,
    soc_port_resource_t *port_resource_t);


/* THDI Setup function */
extern int soc_tomahawk3_mmu_thdi_setup(int unit,
    soc_port_resource_t *port_resource_t,
    int lossy);


/*PORT MAPPING */
extern int soc_tomahawk3_mmu_set_mmu_port_mappings(int unit, 
    soc_port_schedule_state_t *port_schedule_state_t, 
    soc_port_resource_t *port_resource_t);

/*QSCH Credit Clear Functions */
extern int soc_tomahawk3_mmu_clear_qsch_credit_memories(int unit,
    soc_port_resource_t *port_resource_t);


/* MTRO Credit Clear Functions */
extern int soc_tomahawk3_mmu_clear_mtro_bucket_mems(int unit,
    soc_port_resource_t *port_resource_t);


/* Port Flush Sequence */
extern int soc_tomahawk3_flex_mmu_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);

/* FlexPort Port Down Sequence */
extern int soc_tomahawk3_flex_mmu_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);

/* Port Down Sequence */
extern int soc_tomahawk3_mmu_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);

/* Port Up Sequence */
extern int soc_tomahawk3_mmu_port_up(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);

/* Reconfigure Sequence Phase 1 Phase 2 */
extern int soc_tomahawk3_flex_mmu_reconfigure_phase1(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);


extern int soc_tomahawk3_flex_mmu_reconfigure_phase2(int unit,
    soc_port_schedule_state_t *port_schedule_state_t);


/* Per-block reconfigure functions */

extern int soc_tomahawk3_mmu_crb_reconfig(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_scb_reconfig(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_oqs_reconfig(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_ebctm_reconfig(int unit,
    soc_port_resource_t  *port_resource_t,
    soc_asf_mode_e        ct_mode,
    int mmu_port);

extern int soc_tomahawk3_mmu_ebpcc_reconfig(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_eb_sch_reconfig(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_intfo_clr_port_en(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe);


extern int soc_tomahawk3_mmu_set_crb_port_mapping(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_set_rqe_port_mapping(int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_set_ebcfp_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_set_intfi_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_set_intfo_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_set_thdo_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t);


/* THDO functions */
extern int soc_tomahawk3_mmu_thdo_bst_clr(
    int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_thdo_pktstat_clr(
    int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_mmu_thdo_config(
    int unit,
    soc_port_resource_t *port_resource_t);

extern int soc_tomahawk3_get_max_src_ct_speed(int unit, int dst_speed,
    int ct_mode,
    uint32 *max_speed);

extern void soc_tomahawk3_mmu_get_speed_decode(int unit, int speed,
    uint32 *speed_decode);


/*** END SDK API COMMON CODE ***/

#endif /* _SOC_TOMAHAWK3_MMU_FLEXPORT_H_ */
