/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt6_tdm.h
 */

#ifndef _SOC_FIREBOLT6_TDM_H_
#define _SOC_FIREBOLT6_TDM_H_

/*
#include <imports_from_sw.h>
#include <dv_common.h>

#include <all_reg_enums.h>
#include <all_mem_enums.h>
#include <all_fld_enums.h>
#include <all_regs_ext.i>
#include <all_mems_ext.i>

#include <exports_to_sw.h>


#include <tdm_fb6_defines.h>
#include <tdm_defines.h>
#include <tdm_soc.h>
#include <tdm_top.h>
#include <firebolt6_tdm_shim.h>
*/

#include <soc/esw/port.h>
#include <assert.h>

/* HX5 specific defines */
#define FIREBOLT6_TDM_PORTS_PER_PBLK         4
#define FIREBOLT6_TDM_PBLKS_PER_PIPE         17
#define FIREBOLT6_TDM_PBLKS_PER_HPIPE        8
#define FIREBOLT6_TDM_PIPES_PER_DEV          1
#define FIREBOLT6_TDM_HPIPES_PER_PIPE        2

#define FIREBOLT6_TDM_PBLKS_PER_DEV          \
                 (FIREBOLT6_TDM_PBLKS_PER_PIPE * FIREBOLT6_TDM_PIPES_PER_DEV)
#define FIREBOLT6_TDM_GPORTS_PER_HPIPE       \
                 (FIREBOLT6_TDM_PORTS_PER_PBLK * FIREBOLT6_TDM_PBLKS_PER_HPIPE)
#define FIREBOLT6_TDM_GPORTS_PER_PIPE        \
                 (FIREBOLT6_TDM_PORTS_PER_PBLK * FIREBOLT6_TDM_PBLKS_PER_PIPE)
#define FIREBOLT6_TDM_GPORTS_PER_DEV         \
                 (FIREBOLT6_TDM_GPORTS_PER_PIPE * FIREBOLT6_TDM_PIPES_PER_DEV)

/* Physical port */
/* 64 General device port + 1 CPU + 1 FAE + 1 Loopback */
#define FIREBOLT6_TDM_PHY_PORTS_PER_PIPE     \
                 (FIREBOLT6_TDM_GPORTS_PER_PIPE + 3)
#define FIREBOLT6_TDM_PHY_PORTS_PER_DEV      \
                 (FIREBOLT6_TDM_PHY_PORTS_PER_PIPE * FIREBOLT6_TDM_PIPES_PER_DEV)
#define FIREBOLT6_TDM_PHY_MGMT0              999 /* unused */
#define FIREBOLT6_TDM_PHY_MGMT1              999 /* unused */
#define FIREBOLT6_TDM_DEV_MGMT0              999 /* unused */
#define FIREBOLT6_TDM_DEV_MGMT1              999 /* unused */

/* MMU ancillary port number */
/*#define FB6_NUM_EXT_PORTS 72*/
#define FIREBOLT6_TDM_MMU_OVSB_TOKEN         0x48
#define FIREBOLT6_TDM_MMU_NULL_TOKEN         0x49
#define FIREBOLT6_TDM_MMU_PURG_TOKEN         0x4a
#define FIREBOLT6_TDM_MMU_UNUSED_TOKEN       0xFF
#define FIREBOLT6_TDM_MMU_MGMT1_TOKEN        0x40

/* IDB ancillary port number */
#define FIREBOLT6_TDM_IDB_OVSB_TOKEN         0x48
#define FIREBOLT6_TDM_IDB_NULL_TOKEN         0x49
#define FIREBOLT6_TDM_IDB_SBUS_TOKEN         0x4A
#define FIREBOLT6_TDM_IDB_OPT1_TOKEN         0x4A
#define FIREBOLT6_TDM_IDB_OPT2_TOKEN         0x4B
#define FIREBOLT6_TDM_IDB_UNUSED_TOKEN       0xFF
#define FIREBOLT6_TDM_IDB_FAE_TOKEN          0x45
#define FIREBOLT6_TDM_IDB_MGMT1_TOKEN        0x40
#define FIREBOLT6_TDM_IDB_MGMT2_TOKEN        0x41
#define FIREBOLT6_TDM_IDB_MGMT3_TOKEN        0x42
#define FIREBOLT6_TDM_IDB_MGMT4_TOKEN        0x43

/* Oversub groups */
#define FIREBOLT6_TDM_OVS_GROUPS_PER_PIPE    12
#define FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE   6
#define FIREBOLT6_TDM_OVS_GROUP_LENGTH       12
#define FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE   2

/*** START SDK API COMMON CODE ***/
extern void _soc_fb6_tdm_print_port_map(int unit, soc_port_map_type_t *port_map);
extern void _soc_fb6_tdm_print_tdm_info(int unit, soc_tdm_schedule_t *tdm_info,
                                        int hpipe, int is_ing);
extern void _soc_fb6_tdm_print_port_resource(int unit,
                                 soc_port_resource_t *port_info, int entry_num);
extern void _soc_fb6_tdm_print_schedule_state(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_calculation(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_idb_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_idb_hsp(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_idb_pkt_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_idb_ppe_credit(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int full_credit_threshold_0,
                                 int opp_credit_threshold_0,
                                 int full_credit_threshold_1,
                                 int opp_credit_threshold_1 );
extern int _soc_fb6_tdm_set_idb_opportunistic(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int cpu_op_en, int lb_opp_en, int opp1_port_en,
                                 int opp2_port_en, int opp_ovr_sub_en);
extern int _soc_fb6_tdm_set_ovs_group_sel(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int set_idb, int set_mmu);
extern int _soc_fb6_tdm_set_ovs_group(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_mmu_calendar(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_mmu_hsp(int unit,
                                 soc_port_schedule_state_t *sch_info);
extern int _soc_fb6_tdm_set_mmu_opportunistic(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int cpu_op_en, int lb_opp_en, int opp1_port_en,
                                 int opp2_port_en, int opp_ovr_sub_en);

extern int soc_fb6_tdm_set_idb_dpp_ctrl(int unit,
                                 soc_port_schedule_state_t *schedule_state,
                                 int credits);
extern int soc_fb6_tdm_init(int unit, soc_port_schedule_state_t *schedule_state);

extern int _soc_fb6_tdm_get_mport_2_hp_num( uint32 mmu_port_num);

extern int _soc_fb6_tdm_get_cal_id( uint32 phy_num);

extern int _soc_fb6_tdm_get_phy_port_base ( uint32 pm_num);

extern int _soc_fb6_tdm_get_sis_port_spacing_e(int unit, int phy_port);

extern void soc_fb6_tdm_get_port_ratio(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 int pm_num, int *pm_mode, int prev_or_new);
extern void _soc_fb6_tdm_get_pipe_map(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 uint32 *pipe_map);
extern void _soc_fb6_tdm_get_ovs_pipe_map(int unit,
                                 soc_port_schedule_state_t *sch_info,
                                 uint32 *ovs_pipe_map);
extern int _soc_fb6_tdm_get_speed_ovs_class(int unit, int speed);
extern int _soc_fb6_tdm_get_same_port_spacing_e(int unit, int speed);

extern void soc_firebolt6_port_schedule_speed_remap(int unit,
                                 soc_port_schedule_state_t *port_schedule_state);
/*** END SDK API COMMON CODE ***/


#endif	/* !_SOC_FIREBOLT6_TDM_H_ */
