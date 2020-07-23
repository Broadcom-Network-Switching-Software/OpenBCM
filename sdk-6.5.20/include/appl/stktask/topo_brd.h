/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        topo_brd.h
 * Purpose:     
 */

#ifndef   _STKTASK_TOPO_BRD_H_
#define   _STKTASK_TOPO_BRD_H_

#include <sal/types.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/stktask.h>

typedef int (*bcm_board_reset_f)(topo_cpu_t *tp_cpu, uint32 flags);
typedef int (*bcm_board_program_f)(topo_cpu_t *topo_cpu, cpudb_ref_t db_ref);
typedef int (*bcm_board_trunk_cb_f)(int unit, int tid,
                                    bcm_trunk_info_t *info,
                                    int member_count,
                                    bcm_trunk_member_t *member_array,
                                    void *cookie);

/* In topo_brd.c */

extern int topo_board_program(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu);
extern int bcm_board_topo_set(topo_cpu_t *tp_cpu);
extern int bcm_board_topo_get(topo_cpu_t **tp_cpu);

extern int topo_board_setup(cpudb_ref_t db_ref);
extern int topo_board_initialize(cpudb_ref_t db_ref);

/* Rapid Recovery */
extern int topo_board_rapid_recovery(cpudb_ref_t db_ref,
                                     int unit, bcm_port_t fail_port);

typedef struct xgs3devinfo_s {
    int unit;           /* Device unit number */
    int modid;          /* Device modid */
    int intport[2];     /* Internal port number */
} xgs3devinfo_t;

typedef struct bcm_board_port_connection_s {
    bcm_port_t from;    /* usually switch */
    bcm_port_t to;      /* usually fabric */
} bcm_board_port_connection_t;

extern int bcm_board_topo_24g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_24g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_12g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_48g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_48f_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_24g2x_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_48g2x_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_8h(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_lm2x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_lm6x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_lm_xgs2_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_cfm_xgs2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_cfm_xgs3(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_sdk_xgs3_24g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_lm_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs2_12g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs3_48f(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs3_20x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs3_48g5g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_xgs3_48g2(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_lm_56800_12x(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);

#define bcm_board_topo_lm_xgs1_6x bcm_board_topo_lm6x
#define bcm_board_topo_lm_xgs2_6x bcm_board_topo_lm6x
#define bcm_board_topo_sdk_xgs3_12g bcm_board_topo_sdk_xgs3_24g
#define bcm_board_topo_sdk_xgs3_16h bcm_board_topo_sdk_xgs3_20x

/* Strata specific code in brd_strata.c */
extern int bcm_board_topo_24f2g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_48f4g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);
extern int bcm_board_topo_48f2g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref);

/* XGS End to end flow control code in brd_e2e.c */
extern int bcm_board_e2e_config_set(int pkt_rate, int cell_limit,
				    int pkt_limit);
extern int bcm_board_e2e_config_get(int *pkt_rate, int *cell_limit,
				    int *pkt_limit);
extern int bcm_board_e2e_set(void);

extern int _bcm_board_dual_xgs3_e2e_set(int unit0, bcm_module_t modid0,
                                        int unit1, bcm_module_t modid1);

/* Trunking */

#define BCM_BOARD_TRUNK_NORMAL          0x0001 /* Default trunk */
#define BCM_BOARD_TRUNK_DESTROY         0x0002 /* Destroy trunks only */
#define BCM_BOARD_TRUNK_ALL             0x0004 /* Trunk all external ports */

extern int bcm_board_auto_trunk_get(int *flag);
extern int bcm_board_auto_trunk_set(int flag);
extern int bcm_board_trunk_local(cpudb_entry_t *entry, int flags);
extern int bcm_board_trunk_system(cpudb_entry_t *entry, cpudb_ref_t db_ref,
                                  int flags);
extern int bcm_board_trunk_destroy(cpudb_entry_t *entry);
extern int bcm_board_trunk_callback_add(bcm_board_trunk_cb_f func,
                                        void *cookie);
extern int bcm_board_trunk_callback_remove(bcm_board_trunk_cb_f func,
                                        void *cookie);

/* Compatibility */
#define bcm_board_trunk(entry, create) bcm_board_trunk_local((entry), (create))

/* Board Programming common functions */
/* Internal stack flags */
#define BCM_BOARD_INT_STK_FLAGS \
   (BCM_STK_ENABLE | BCM_STK_DUPLEX | BCM_STK_INTERNAL)

/* Previous SDK releases used BCM_BOARD_AUTO_TRUNK to enable
   auto-trunking, and now it's a runtime flag. For backwards
   compatibility, check to see if BCM_BOARD_AUTO_TRUNK flag is
   defined, and if it is, change the default auto-trunking state to
   TRUE.
*/

/*
 * Check if MOD ID of _unit matches dest_mod.  If so, set
 * exit_port to _dest_port and return success.
 */
#define BCM_BOARD_MOD_CHECK(_unit, _dest_port, _dest_mod, _exit_port) do { \
        int local_mod;                                                  \
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(_unit, &local_mod));   \
        if ((_dest_mod) == local_mod) { /* Local destination */         \
            *(_exit_port) = _dest_port;                                 \
            return BCM_E_NONE;                                          \
        }                                                               \
    } while (0)


extern int bcm_board_unknown_src_modid_setup(cpudb_entry_t *entry);
extern int bcm_board_module_filter(int unit,
                                   topo_cpu_t *tp_cpu,
                                   cpudb_ref_t db_ref,
                                   bcm_port_config_t *config);
extern int bcm_board_internal_stk_port_add(int unit, int port);
extern int bcm_board_fab_mod_map(int funit, topo_cpu_t *tp_cpu,
                                 cpudb_entry_t *l_entry);
extern int bcm_board_xgs_common(int funit, topo_cpu_t *tp_cpu,
                                cpudb_ref_t db_ref, int *unitport);
extern int bcm_board_topomap_stk(int src_unit, int dest_modid,
                                 bcm_port_t *exit_port);
extern int bcm_board_untrunkable_stack_ports(int unit, topo_cpu_t *tp_cpu);

/* Rapid Recovery */
extern int bcm_board_topo_trunk_failover(topo_cpu_t *topo_cpu,
                                         int unit, bcm_port_t fail_sp,
                                         int fail_sp_idx,
                                         bcm_trunk_t tid,
                                         bcm_trunk_info_t *trunk,
                                         int member_count,
                                         bcm_trunk_member_t *member_array);

extern int bcm_board_num_modid_set(int num);
extern int bcm_board_num_modid_get(int *num);
extern int bcm_board_xgs_local_switch(int funit,
                                      topo_cpu_t *tp_cpu,
                                      bcm_port_config_t *config,
                                      bcm_board_port_connection_t *conn);
extern int bcm_board_xgs_local_fabric(int funit,
                                      topo_cpu_t *tp_cpu,
                                      bcm_port_config_t *config);
     
#if defined(INCLUDE_ATPTRANS_SOCKET) && defined(INCLUDE_CHASSIS)
extern int
bcm_board_chassis_atptrans_socket_transition(bcm_st_state_t from,
                                             bcm_st_event_t event,
                                             bcm_st_state_t to,
                                             cpudb_ref_t disc_db,
                                             cpudb_ref_t cur_db);
#endif

/* Callback registration */

typedef enum topo_board_control_e {
    topoBoardInit,
    topoBoardProgram,
    topoBoardDeinit,
    topoBoardCount
} topo_board_control_t;

typedef int (*topo_board_callback_t)(topo_board_control_t control,
                                     cpudb_ref_t db_ref,
                                     topo_cpu_t *topo_cpu,
                                     void *user_data);

extern int topo_board_register(topo_board_callback_t func, void *user_data);

extern int topo_board_unregister(topo_board_callback_t func, void *user_data);

extern int topo_board_default_board_program(topo_board_control_t op,
                                            cpudb_ref_t db_ref,
                                            topo_cpu_t *topo_cpu,
                                            void *user_data);

#endif /* _STKTASK_TOPO_BRD_H_ */
