/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane4_tdm.h
 */

#ifndef _SOC_HURRICANE4_TDM_H_
#define _SOC_HURRICANE4_TDM_H_

#include <soc/esw/port.h>

/*** START SDK API COMMON CODE ***/

/* HR4 specific defines */
#define HURRICANE4_TDM_PORTS_PER_PBLK         4
#define HURRICANE4_TDM_PBLKS_PER_PIPE         16
#define HURRICANE4_TDM_PBLKS_PER_HPIPE        8
#define HURRICANE4_TDM_PIPES_PER_DEV          1
#define HURRICANE4_TDM_HPIPES_PER_PIPE        1

#define HURRICANE4_TDM_PBLKS_PER_DEV          \
                 (HURRICANE4_TDM_PBLKS_PER_PIPE * HURRICANE4_TDM_PIPES_PER_DEV)
#define HURRICANE4_TDM_GPORTS_PER_HPIPE       \
                 (HURRICANE4_TDM_PORTS_PER_PBLK * HURRICANE4_TDM_PBLKS_PER_HPIPE)
#define HURRICANE4_TDM_GPORTS_PER_PIPE        \
                 (HURRICANE4_TDM_PORTS_PER_PBLK * HURRICANE4_TDM_PBLKS_PER_PIPE)
#define HURRICANE4_TDM_GPORTS_PER_DEV         \
                 (HURRICANE4_TDM_GPORTS_PER_PIPE * HURRICANE4_TDM_PIPES_PER_DEV)

/* Physical port */
/* 64 General device port + 1 CPU + 1 FAE + 1 Loopback */
#define HURRICANE4_TDM_PHY_PORTS_PER_PIPE     \
                 (HURRICANE4_TDM_GPORTS_PER_PIPE + 7) /* 64 FP ports + cpu, lpbk, broadscan, 4 Macsec ports */
#define HURRICANE4_TDM_PHY_PORTS_PER_DEV      \
                 (HURRICANE4_TDM_PHY_PORTS_PER_PIPE * HURRICANE4_TDM_PIPES_PER_DEV)
#define HURRICANE4_TDM_PHY_MGMT0              999 /* unused */
#define HURRICANE4_TDM_PHY_MGMT1              999 /* unused */
#define HURRICANE4_TDM_DEV_MGMT0              999 /* unused */
#define HURRICANE4_TDM_DEV_MGMT1              999 /* unused */

/* MMU ancillary port number */
#define HR4_NUM_EXT_PORTS 71
#define HURRICANE4_TDM_MMU_OVSB_TOKEN         0x48
#define HURRICANE4_TDM_MMU_NULL_TOKEN         0x49
#define HURRICANE4_TDM_MMU_PURG_TOKEN         0x4a
#define HURRICANE4_TDM_MMU_UNUSED_TOKEN       0xFF
#define HURRICANE4_TDM_MMU_MGMT1_TOKEN        0x3f

/* IDB ancillary port number */
#define HURRICANE4_TDM_IDB_OVSB_TOKEN         0x48
#define HURRICANE4_TDM_IDB_NULL_TOKEN         0x49
#define HURRICANE4_TDM_IDB_SBUS_TOKEN         0x4A
#define HURRICANE4_TDM_IDB_OPT1_TOKEN         0x4A
#define HURRICANE4_TDM_IDB_OPT2_TOKEN         0x4B
#define HURRICANE4_TDM_IDB_UNUSED_TOKEN       0xFF
#define HURRICANE4_TDM_IDB_FAE_TOKEN          0x45

/* Oversub groups */
#define HURRICANE4_TDM_OVS_GROUPS_PER_PIPE    12
#define HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE   6
#define HURRICANE4_TDM_OVS_GROUP_LENGTH       12
#define HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE   2

/*** START SDK API COMMON CODE ***/
extern void _soc_hr4_tdm_print_port_map(int unit, soc_port_map_type_t *port_map);
extern void _soc_hr4_tdm_print_tdm_info(int unit, soc_tdm_schedule_t *tdm_info,
                                        int hpipe, int is_ing);
extern void _soc_hr4_tdm_print_port_resource(int unit,
                                 soc_port_resource_t *port_info, int entry_num);
extern void _soc_hr4_tdm_print_schedule_state(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_calculation(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_idb_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_idb_hsp(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_idb_pkt_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_idb_ppe_credit(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int full_credit_threshold_0,
                                 int opp_credit_threshold_0,
                                 int full_credit_threshold_1,
                                 int opp_credit_threshold_1 );
extern int _soc_hr4_tdm_set_idb_opportunistic(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int cpu_op_en, int lb_opp_en, int opp1_port_en,
                                 int opp2_port_en, int opp_ovr_sub_en);
extern int _soc_hr4_tdm_set_ovs_group_sel(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int set_idb, int set_mmu);
extern int _soc_hr4_tdm_set_ovs_group(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_mmu_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_mmu_hsp(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_hr4_tdm_set_mmu_opportunistic(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int cpu_op_en, int lb_opp_en, int opp1_port_en,
                                 int opp2_port_en, int opp_ovr_sub_en);

extern int soc_hr4_tdm_set_idb_dpp_ctrl(int unit,
                                 soc_port_schedule_state_t *schedule_state,
                                 int credits);
extern int soc_hr4_tdm_init(int unit, soc_port_schedule_state_t *schedule_state);

extern int _soc_hr4_tdm_get_mport_2_hp_num( uint32 mmu_port_num);

extern int _soc_hr4_tdm_get_cal_id( uint32 phy_num);

extern int _soc_hr4_tdm_get_phy_port_base ( uint32 pm_num);

extern int _soc_hr4_tdm_get_sis_port_spacing_e(int unit, int phy_port);

extern void soc_hr4_tdm_get_port_ratio(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int pm_num, int *pm_mode, int prev_or_new);
extern void _soc_hr4_tdm_get_pipe_map(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 uint32 *pipe_map);
extern void _soc_hr4_tdm_get_ovs_pipe_map(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 uint32 *ovs_pipe_map);
extern int _soc_hr4_tdm_get_speed_ovs_class(int unit, int speed);
extern int _soc_hr4_tdm_get_same_port_spacing_e(int unit, int speed);

extern void soc_hurricane4_port_schedule_speed_remap(int unit,
                                 soc_port_schedule_state_t *port_schedule_state);

/*** END SDK API COMMON CODE ***/

#endif	/* !_SOC_HURRICANE4_TDM_H_   */
