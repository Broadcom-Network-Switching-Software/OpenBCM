/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        metrolite.h
 */

#ifndef _SOC_METROLITE_H_
#define _SOC_METROLITE_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/shmoo_and28.h>
#include <bcm/types.h>

extern int ml_max_linkphy_subports_per_port;
extern int ml_max_linkphy_streams_per_port;
extern int ml_max_linkphy_streams_per_slice;

#define ML_MAX_LOGICAL_PORTS           14
#define ML_MAX_PHYSICAL_PORTS          12 
#define ML_MAX_PP_PORTS                32
#define ML_LPBK                        13 
#define ML_CMIC                        0
#define ML_IDLE                        ML_MAX_LOGICAL_PORTS
#define ML_IDLE_1                      ML_IDLE + 1
#define ML_MAX_BLOCKS                  3
#define ML_MAX_PORTS_PER_BLOCK         4

/* Total available queues on Metrolite device */
#define ML_MMU_MAX_EGRESS_QUEUE        512

/* Linkphy global variables */
#define SOC_ML_LINKPHY_TX_DATA_BUF_START_ADDR_MAX    3392
#define SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MIN      31
#define SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MAX      3423
#define SOC_ML_LINKPHY_TX_STREAM_START_ADDR_OFFSET   0x80
#define SOC_ML_MAX_LINKPHY_BLOCK                     1
#define SOC_ML_MAX_LINKPHY_PORTS                     4
#define SOC_ML_MAX_LINKPHY_STREAMS_PER_SLICE         ml_max_linkphy_streams_per_slice
#define SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT          ml_max_linkphy_streams_per_port
#define SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT         ml_max_linkphy_subports_per_port
#define SOC_ML_MAX_LINKPHY_STREAMS_PER_SUBPORT       1

#define SOC_ML_SUBPORT_GROUP_MAX                     24
#define SOC_ML_MAX_SUBPORTS                          24
#define SOC_ML_SUBPORT_PP_PORT_INDEX_MIN             0
#define SOC_ML_SUBPORT_PP_PORT_INDEX_MAX             ML_MAX_PP_PORTS - 1

#define ML_PORT_MAC_MAX       32

#define ML_MAX_SERVICE_POOLS            4
#define ML_MAX_PRIORITY_GROUPS          8


#define BCM_ML_COSQ_WRED_DROP_MASK    0x006f
#define BCM_ML_COSQ_TOTAL_DROP_MASK   0x0000
#define BCM_ML_COSQ_DEFAULT_DROP_MASK 0x007f


const static uint32 ml_port_mapping[ML_MAX_LOGICAL_PORTS][2] =
{ {0,0}, {1,15}, {2,23}, {3, 31}, {4, 39}, {5,47}, {6, 55},
  {7,63}, {8, 71}, {9, 79}, {21, 175}, {22, 183}, {23, 191}, {29,239}};

#define ML_GET_THDI_PORT(p) ml_port_mapping[p][0]
#define ML_GET_THDI_PORT_PG(p) ml_port_mapping[p][1]
#define  METROLITE_GET_REG_THDI_PORT(unit, port, reg, thdi_port)                 \
         if  (SOC_IS_METROLITE(unit) && (block == MMU_BLOCK(unit)) &&            \
              ((SOC_REG_STAGE(unit, reg) == 2) ||                                \
              (SOC_REG_STAGE(unit, reg) == 38)|                                  \
              (SOC_REG_STAGE(unit, reg) == 40))) {                               \
               thdi_port = ML_GET_THDI_PORT(port);}  
              
typedef struct ml_pbmp_s {
    soc_pbmp_t *pbmp_gport_stack;
    soc_pbmp_t *pbmp_mxq;
    soc_pbmp_t *pbmp_mxq1g;
    soc_pbmp_t *pbmp_mxq2p5g;
    soc_pbmp_t *pbmp_mxq10g;
    soc_pbmp_t *pbmp_xl;
    soc_pbmp_t *pbmp_xl1g;
    soc_pbmp_t *pbmp_xl2p5g;
    soc_pbmp_t *pbmp_xl10g;
    soc_pbmp_t *pbmp_xport_ge;
    soc_pbmp_t *pbmp_xport_xe;
    soc_pbmp_t *pbmp_valid;
    soc_pbmp_t *pbmp_pp;
    soc_pbmp_t *pbmp_linkphy;
}ml_pbmp_t;

typedef uint32 ml_port_speeds_t[ML_MAX_BLOCKS][ML_MAX_PORTS_PER_BLOCK];
typedef uint32 ml_speed_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_port_to_block_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_port_to_block_subports_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_block_ports_t[ML_MAX_BLOCKS][ML_MAX_PORTS_PER_BLOCK];

extern int soc_ml_linkphy_port_reg_blk_idx_get(
     int unit, int port, int blktype, int *block, int *index);
extern int soc_ml_linkphy_port_blk_idx_get(
     int unit, int port, int *block, int *index);
extern int soc_ml_linkphy_get_portid(int unit, int block, int index);
extern int soc_ml_pp_port_get_port(int unit, soc_port_t pp_port);

extern void _ml_phy_addr_default(int unit, int port,
                                 uint16 *phy_addr, uint16 *phy_addr_int);
extern int soc_ml_mem_config(int unit, int dev_id);
extern int _soc_ml_mmu_reconfigure(int unit, int port);
extern int soc_metrolite_global_param_reconfigure(int unit);
extern void soc_metrolite_pbmp_init(int unit,
                                    ml_pbmp_t ml_pbmp,
                                    soc_pbmp_t *pbmp_pp_unsed);
extern void soc_metrolite_subport_init(int unit,
                                       soc_pbmp_t *pbmp_pp_unsed);
extern int bcm_ml_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp);
extern soc_functions_t soc_metrolite_drv_funs;
extern ml_block_ports_t *ml_block_ports[SOC_MAX_NUM_DEVICES];
extern ml_port_speeds_t *ml_port_speeds[SOC_MAX_NUM_DEVICES];
extern int soc_ml_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_metrolite_num_cosq_init(int unit);
extern void soc_metrolite_sbus_ring_map_config(int unit);
extern soc_error_t soc_metrolite_get_port_block(
       int unit, soc_port_t port,uint8 *block);
extern soc_error_t soc_metrolite_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm);
extern void soc_metrolite_block_reset(int unit, uint8 block,int active_low);
extern int soc_metrolite_xl_port_speed_get(int unit, int port, int *speed);

extern int _soc_ml_flexio_scache_allocate(int unit);
extern int _soc_ml_flexio_scache_retrieve(int unit);
extern int _soc_ml_flexio_scache_sync(int unit);

extern soc_error_t soc_metrolite_get_mxq_phy_port_mode(
        int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode);
extern soc_error_t soc_metrolite_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, bcmXlPhyPortMode_t *phy_mode,
        bcmXlCorePortMode_t *core_mode);
extern int _soc_metrolite_get_cfg_num(int unit, int skip_warmboot, int *new_cfg_num);

typedef struct bcm56270_tdm_info_s {
        uint8  tdm_freq;
        uint32  tdm_size;

        /* Display purpose only */
        uint8  row;
        uint8  col;
}bcm56270_tdm_info_t;


/* Configs for BCM56270 */
#define BCM56270_DEVICE_ID_OFFSET_CFG 0
#define BCM56270_DEVICE_ID_DEFAULT_CFG 1
#define BCM56270_DEVICE_ID_MAX_CFG 1

/* Configs for BCM56271 */
#define BCM56271_DEVICE_ID_OFFSET_CFG 1
#define BCM56271_DEVICE_ID_DEFAULT_CFG 2
#define BCM56271_DEVICE_ID_MAX_CFG 4

/* Configs for BCM56272 */
#define BCM56272_DEVICE_ID_OFFSET_CFG 4
#define BCM56272_DEVICE_ID_DEFAULT_CFG 5
#define BCM56272_DEVICE_ID_MAX_CFG 6

typedef struct  ml_tdm_pos_info_s {
        uint16 total_slots;
        int16  pos[180];
} ml_tdm_pos_info_t;

#define ML_TDM_MAX_SIZE 204

extern uint32 ml_current_tdm[ML_TDM_MAX_SIZE];
extern uint32 ml_current_tdm_size;
extern ml_tdm_pos_info_t ml_tdm_pos_info[ML_MAX_BLOCKS];

extern uint32 ml_current_tdm[ML_TDM_MAX_SIZE];
extern uint32 ml_current_tdm_size;
extern ml_tdm_pos_info_t ml_tdm_pos_info[ML_MAX_BLOCKS];
extern void soc_ml_port_bitmap_add(int unit,soc_port_t port,int block,
                                   int speed, int new_lanes);
extern int soc_metrolite_bond_info_init(int unit);
extern int soc_ml_get_max_buffer_size(int unit, int value);
#endif  /* !_SOC_METROLITE_H_ */
