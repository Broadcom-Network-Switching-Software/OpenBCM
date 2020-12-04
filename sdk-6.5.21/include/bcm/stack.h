/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_STACK_H__
#define __BCM_STACK_H__

#include <bcm/types.h>

#define BCM_STK_MAX_MODULES                 (72)       /* Maximum number of
                                                          modules in stack. */
#define BCM_STK_MODULE_MAX_ALL_REACHABLE    (0x1)      /* All Reachable flag for
                                                          stack module max. */

/* module protocols indicate module communication types */
typedef enum bcm_module_protocol_e {
    bcmModuleProtocolNone = 1, 
    bcmModuleProtocol1 = 2,         /* QE20 protocol - plane A */
    bcmModuleProtocol2 = 3,         /* QE20 protocol - plane B */
    bcmModuleProtocol3 = 4,         /* QE40 protocol - A/B */
    bcmModuleProtocol4 = 5,         /* QE40 protocol - A fic B local */
    bcmModuleProtocol5 = 6,         /* QE40 protocol - A fic */
    bcmModuleProtocolCustom1 = 7    /* Custom Device Protocol - A/B */
} bcm_module_protocol_t;

/* bcm_stk_module_control_t */
typedef enum bcm_stk_module_control_e {
    bcmStkModuleAllReachableIgnore = 0, /* Ignore a MODULE ID during all
                                           reachable vector calculation */
    bcmStkModuleMHPriorityUse = 1       /* Use the Priority/Color from the
                                           Module Header */
} bcm_stk_module_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the Stack Control subsystem. */
extern int bcm_stk_init(
    int unit);

/* Gets the list of ports currently in SL stack mode. */
extern int bcm_stk_pbmp_get(
    int unit, 
    bcm_pbmp_t *cur_pbm, 
    bcm_pbmp_t *inactive_pbm);

/* Set/Get SL stack flags for a port. */
extern int bcm_stk_port_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* Set/Get SL stack flags for a port. */
extern int bcm_stk_port_get(
    int unit, 
    bcm_port_t port, 
    uint32 *flags);

#endif /* BCM_HIDE_DISPATCHABLE */

/* bcm_stk_update_callback function */
typedef void (*bcm_stk_cb_f)(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    void *cookie);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register a function for stack port updates. */
extern int bcm_stk_update_callback_register(
    int unit, 
    bcm_stk_cb_f cb, 
    void *cookie);

/* Unregister a function for stack port updates. */
extern int bcm_stk_update_callback_unregister(
    int unit, 
    bcm_stk_cb_f cb, 
    void *cookie);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_stk_port_set/get. */
#define BCM_STK_CAPABLE         0x00000001 /* The port is stack capable, but may
                                              not be stacked currently (e.g.
                                              Dynamic SL front panel). */
#define BCM_STK_ENABLE          0x00000002 /* The port is in stacking mode. */
#define BCM_STK_INACTIVE        0x00000004 /* Explicitly set inactive to avoid
                                              undirected traffic to flow on this
                                              link. */
#define BCM_STK_DUPLEX          0x00000008 /* This is a duplex connection. */
#define BCM_STK_SIMPLEX         0x00000010 /* This is a simplex connection
                                              (exclusive to _DUPLEX) */
#define BCM_STK_NONE            0x00000020 /* Not a stackable port (currently
                                              ignored). */
#define BCM_STK_SL              0x00000040 /* Port is an SL (front panel) stack
                                              port (currently ignored). */
#define BCM_STK_HG              0x00000080 /* Port is a HiGig stack port
                                              (currently ignored). */
#define BCM_STK_INTERNAL        0x00000100 /* Port is an on-board device
                                              connection (set only; currently
                                              ignored). */
#define BCM_STK_NO_LINK         0x00000200 /* Port does not have link. Not
                                              recorded; provided for callbacks.
                                              (set only; currently ignored). */
#define BCM_STK_CUT             0x00000400 /* Port is a "cut" port that blocks
                                              multicast and broadcast traffic.
                                              (currently ignored). */
#define BCM_STK_FLAGS_MASK      0x000007ff 

#define BCM_STK_FLAGS_STR       \
{ \
    "capable", \
    "enable", \
    "inactive", \
    "duplex", \
    "simplex", \
    "none", \
    "sl", \
    "hg", \
    "internal", \
    "no_link", \
    "cut", \
    "" \
} 

/* bcm_stk_flags_str */
extern char *bcm_stk_flags_str[];

/* 
 * Pass this as the port parameter to set/get to use the unique IPIC_PORT
 * if present.
 */
#define BCM_STK_USE_HG_IF       -1         

#ifndef BCM_HIDE_DISPATCHABLE

/* Set/Get SL stack mode status */
extern int bcm_stk_mode_set(
    int unit, 
    uint32 flags);

/* Set/Get SL stack mode status. */
extern int bcm_stk_mode_get(
    int unit, 
    uint32 *flags);

/* Get or set the module ID of the local device. */
extern int bcm_stk_my_modid_set(
    int unit, 
    int my_modid);

/* Get or set the module ID of the local device. */
extern int bcm_stk_my_modid_get(
    int unit, 
    int *my_modid);

/* Get or set the module ID of the local device. */
extern int bcm_stk_modid_set(
    int unit, 
    int modid);

/* Get or set the module ID of the local device. */
extern int bcm_stk_modid_get(
    int unit, 
    int *modid);

#endif /* BCM_HIDE_DISPATCHABLE */

/* module ID types */
typedef enum bcm_stk_modid_type_e {
    bcmStkModidTypePrimary = 0,         /* primary module IDs */
    bcmStkModidTypeMultiNextHops = 1,   /* multiple next hops module IDs */
    bcmStkModidTypeCoe = 2              /* COE module IDs */
} bcm_stk_modid_type_t;

/* Structure for the configuration of the module. */
typedef struct bcm_stk_modid_config_s {
    uint32 flags; 
    int modid; 
    int num_ports; 
    bcm_stk_modid_type_t modid_type; 
    int num_mods; 
} bcm_stk_modid_config_t;

/* Initialize a bcm_stk_modid_config_t structure. */
extern void bcm_stk_modid_config_t_init(
    bcm_stk_modid_config_t *pconfig);

#ifndef BCM_HIDE_DISPATCHABLE

/* Sets/Gets the module id. */
extern int bcm_stk_modid_config_set(
    int unit, 
    bcm_stk_modid_config_t *modid);

/* Sets/Gets the module id. */
extern int bcm_stk_modid_config_get(
    int unit, 
    bcm_stk_modid_config_t *modid);

/* Adds a desired module local to the system. */
extern int bcm_stk_modid_config_add(
    int unit, 
    bcm_stk_modid_config_t *modid);

/* Deletes a desired module that is local to the system. */
extern int bcm_stk_modid_config_delete(
    int unit, 
    bcm_stk_modid_config_t *modid);

/* Deletes all modules added local to the system. */
extern int bcm_stk_modid_config_delete_all(
    int unit);

/* Gets all modules added local to the system. */
extern int bcm_stk_modid_config_get_all(
    int unit, 
    int modid_max, 
    bcm_stk_modid_config_t *modid_array, 
    int *modid_count);

/* Number of module identifiers used by device */
extern int bcm_stk_modid_count(
    int unit, 
    int *num_modid);

/* Control module steering. */
extern int bcm_stk_modport_set(
    int unit, 
    int modid, 
    int port);

/* Control module steering. */
extern int bcm_stk_modport_get(
    int unit, 
    int modid, 
    int *port);

/* Get module steering */
extern int bcm_stk_modport_get_all(
    int unit, 
    int modid, 
    int port_max, 
    int *port_array, 
    int *port_count);

/* Clear module steering. */
extern int bcm_stk_modport_clear(
    int unit, 
    int modid);

/* Clear module steering. */
extern int bcm_stk_modport_clear_all(
    int unit);

/* Add or delete more than one path for a given destination module. */
extern int bcm_stk_modport_add(
    int unit, 
    int modid, 
    bcm_port_t port);

/* Add or delete more than one path for a given destination module. */
extern int bcm_stk_modport_delete(
    int unit, 
    int modid, 
    bcm_port_t port);

/* Control per ingress port module steering. */
extern int bcm_stk_port_modport_set(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid, 
    bcm_port_t dest_port);

/* 
 * Get a destination path for a given destination module of the ingress
 * port.
 */
extern int bcm_stk_port_modport_get(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid, 
    bcm_port_t *dest_port);

/* Get per ingress port module steering */
extern int bcm_stk_port_modport_get_all(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid, 
    int dest_port_max, 
    bcm_port_t *dest_port_array, 
    int *dest_port_count);

/* Clear per ingress port module steering. */
extern int bcm_stk_port_modport_clear(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid);

/* 
 * Clear destination paths for all destination modules of the ingress
 * port.
 */
extern int bcm_stk_port_modport_clear_all(
    int unit, 
    bcm_port_t ing_port);

/* 
 * Add or delete more than one path for a given destination module of an
 *     ingress port.
 */
extern int bcm_stk_port_modport_add(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid, 
    bcm_port_t dest_port);

/* Delete one path for a given destination module of the ingress port. */
extern int bcm_stk_port_modport_delete(
    int unit, 
    bcm_port_t ing_port, 
    bcm_module_t dest_modid, 
    bcm_port_t dest_port);

/* Set, get, or delete Fabric unicast table entries. */
extern int bcm_stk_ucbitmap_set(
    int unit, 
    int port, 
    int modid, 
    bcm_pbmp_t pbmp);

/* Set, get, or delete Fabric unicast table entries. */
extern int bcm_stk_ucbitmap_get(
    int unit, 
    int port, 
    int modid, 
    bcm_pbmp_t *pbmp);

/* Set, get, or delete Fabric unicast table entries. */
extern int bcm_stk_ucbitmap_del(
    int unit, 
    bcm_port_t port, 
    int modid, 
    bcm_pbmp_t pbmp);

/* Set or get a fabric mapping between fabric and local space. */
extern int bcm_stk_fmod_lmod_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_module_t fmod, 
    bcm_module_t lmod);

/* Set or get a fabric mapping between fabric and local space. */
extern int bcm_stk_fmod_lmod_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_module_t *fmod, 
    bcm_module_t *lmod);

/* Set or get a fabric mapping between fabric and switch space. */
extern int bcm_stk_fmod_smod_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_module_t fmod, 
    bcm_module_t smod, 
    bcm_port_t sport, 
    uint32 nports);

/* Set or get a fabric mapping between fabric and switch space. */
extern int bcm_stk_fmod_smod_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_module_t fmod, 
    bcm_module_t *smod, 
    bcm_port_t *sport, 
    uint32 *nports);

/* Assign or get fabric port to a module mapping group. */
extern int bcm_stk_port_modmap_group_set(
    int unit, 
    bcm_port_t port, 
    int group);

/* Assign or get fabric port to a module mapping group. */
extern int bcm_stk_port_modmap_group_get(
    int unit, 
    bcm_port_t port, 
    int *group);

/* Retrieve the maximum module mapping group on a fabric. */
extern int bcm_stk_port_modmap_group_max_get(
    int unit, 
    int *max_group);

/* Enable/Disable or get module mapping on a fabric port. */
extern int bcm_stk_modmap_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable);

/* Enable/Disable or get module mapping on a fabric port. */
extern int bcm_stk_modmap_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Switch module mapping. */
#define BCM_STK_MODMAP_SET      1          /* Global to local. */
#define BCM_STK_MODMAP_GET      2          /* Local to global. */

/* bcm_stk_modmap_cb_t */
typedef int (*bcm_stk_modmap_cb_t)(
    int unit, 
    int setget, 
    bcm_module_t mod_in, 
    bcm_port_t port_in, 
    bcm_module_t *mod_out, 
    bcm_port_t *port_out);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register a Module Mapping callback. */
extern int bcm_stk_modmap_register(
    int unit, 
    bcm_stk_modmap_cb_t func);

/* Register a Module Mapping callback. */
extern int bcm_stk_modmap_unregister(
    int unit);

/* Map or unmap a module identifier. */
extern int bcm_stk_modmap_map(
    int unit, 
    int setget, 
    bcm_module_t mod_in, 
    bcm_port_t port_in, 
    bcm_module_t *mod_out, 
    bcm_port_t *port_out);

/* Enable or disable a module and specify number of ports. */
extern int bcm_stk_module_enable(
    int unit, 
    bcm_module_t modid, 
    int nports, 
    int enable);

/* Map a SAND System-Port to a Gport */
extern int bcm_stk_sysport_gport_set(
    int unit, 
    bcm_gport_t sysport, 
    bcm_gport_t gport);

/* Given a SAND System-Port, get the Gport */
extern int bcm_stk_sysport_gport_get(
    int unit, 
    bcm_gport_t sysport, 
    bcm_gport_t *gport);

/* Given a Gport get the SAND System-Port */
extern int bcm_stk_gport_sysport_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t *sysport);

/* Add or delete modid from TM-Domain. */
extern int bcm_stk_modid_domain_add(
    int unit, 
    int modid, 
    int tm_domain);

/* Delete modid from TM-domain */
extern int bcm_stk_modid_domain_delete(
    int unit, 
    int modid, 
    int tm_domain);

/* Get array of modid belongs to TM-Domin */
extern int bcm_stk_modid_domain_get(
    int unit, 
    int tm_domain, 
    int mod_max, 
    int *mod_array, 
    int *mod_count);

/* Define via which Stacking-Lag the TM-Domain will be reached. */
extern int bcm_stk_domain_stk_trunk_add(
    int unit, 
    int tm_domain, 
    bcm_trunk_t stk_trunk);

/* Delete a path from stacking trunk to TM-domain */
extern int bcm_stk_domain_stk_trunk_delete(
    int unit, 
    int tm_domain, 
    bcm_trunk_t stk_trunk);

/* Get array of TM-Domains which are reachable by the Stacking-Lag. */
extern int bcm_stk_domain_stk_trunk_get(
    int unit, 
    bcm_trunk_t stk_trunk, 
    int domain_max, 
    int *domain_array, 
    int *domain_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Flags for bcm_stk_system_gport_map_create/get/destroy. */
#define BCM_STK_GPORT_MAP_WITH_ID   0x00000001 /* Add mapping with the specified
                                                  ID. */
#define BCM_STK_GPORT_MAP_REPLACE   0x00000002 /* Replace existing mapping. */

/* Structure for mapping gport to system port ID. */
typedef struct bcm_stk_system_gport_map_s {
    uint32 flags; 
    bcm_module_t modid;         /* Destination module ID. */
    bcm_gport_t dest_gport;     /* Destination gport ID. */
    bcm_gport_t system_gport;   /* System gport ID */
} bcm_stk_system_gport_map_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create mapping from local gport id to system gport id */
extern int bcm_stk_system_gport_map_create(
    int unit, 
    bcm_stk_system_gport_map_t *sys_gport_map);

/* Get mapping from local gport id to system gport id */
extern int bcm_stk_system_gport_map_get(
    int unit, 
    bcm_stk_system_gport_map_t *sys_gport_map);

/* Destroy mapping from local gport id to system gport id */
extern int bcm_stk_system_gport_map_destroy(
    int unit, 
    bcm_gport_t system_gport);

/* Set remote mapping between local port and remote port, remote modid */
extern int bcm_stk_modport_remote_map_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    int remote_modid, 
    bcm_port_t remote_port);

/* Get remote mapping between local port and remote port, remote modid */
extern int bcm_stk_modport_remote_map_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    int *remote_modid, 
    bcm_port_t *remote_port);

/* Maximum group ID set/get */
extern int bcm_stk_module_max_set(
    int unit, 
    uint32 flags, 
    bcm_module_t max_module);

/* Maximum group ID set/get */
extern int bcm_stk_module_max_get(
    int unit, 
    uint32 flags, 
    bcm_module_t *max_module);

/* Set/get control value. */
extern int bcm_stk_module_control_set(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int arg);

/* Set/get control value. */
extern int bcm_stk_module_control_get(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int *arg);

/* 
 * Enable remote ports for the specified module based on the specified
 * port bitmap.
 */
extern int bcm_stk_module_remote_port_enable_set(
    int unit, 
    int modid, 
    bcm_pbmp_t pbmp);

/* Get enabled remote port bitmap for the specified module. */
extern int bcm_stk_module_remote_port_enable_get(
    int unit, 
    int modid, 
    bcm_pbmp_t *pbmp);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_STACK_H__ */
