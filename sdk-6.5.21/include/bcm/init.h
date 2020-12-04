/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_INIT_H__
#define __BCM_INIT_H__

#include <bcm/types.h>
#include <bcm/module.h>

/* BCM Information structure. */
typedef struct bcm_info_s {
    uint32 vendor;      /* PCI values used usually. */
    uint32 device;      /* PCI values used usually. */
    uint32 revision;    /* PCI values used usually. */
    uint32 capability; 
    int num_pipes;      /* Number of pipes in the device. */
    int num_pp_pipes;   /* Number of packet processing pipes in the device. */
    int num_buffers;    /* Number of packet buffers. */
} bcm_info_t;

/* Capability flags. */
#define BCM_INFO_SWITCH         0x00000001 /* Network switch chip. */
#define BCM_INFO_FABRIC         0x00000002 /* Fabric chip. */
#define BCM_INFO_L3             0x00000004 /* Chip has layer 3. */
#define BCM_INFO_IPMC           0x00000008 /* Chip has IP multicast. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM software layer for a device. */
extern int bcm_init(
    int unit);

/* Initialize the BCM software layer for a device. */
extern int bcm_init_selective(
    int unit, 
    uint32 flags);

/* Check the return value from system_init() operation. */
extern int bcm_init_check(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Attach a device as a BCM unit. */
extern int bcm_attach(
    int unit, 
    char *type, 
    char *subtype, 
    int remunit);

/* Attach only tx and rx modules to a device as a BCM unit. */
extern int bcm_attach_early_txrx(
    int unit, 
    char *type, 
    char *subtype, 
    int remunit);

/* Detach a device as a BCM unit. */
extern int bcm_detach(
    int unit);

/* Detach all modules excluding tx and rx from BCM unit. */
extern int bcm_detach_late_txrx(
    int unit);

/* Find a matching BCM unit. */
extern int bcm_find(
    char *type, 
    char *subtype, 
    int remunit);

/* Determine if a BCM unit is attached. */
extern int bcm_attach_check(
    int unit);

/* Determine the highest BCM unit currently attached. */
extern int bcm_attach_max(
    int *max_units);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get information about a BCM unit. */
extern int bcm_info_get(
    int unit, 
    bcm_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Groups of devices */
typedef enum bcm_device_member_e {
    bcmDeviceMemberDNX = 0,     /* DNX FAP device */
    bcmDeviceMemberDNXF = 1,    /* DNX Fabric element device */
    bcmDeviceMemberDPP = 2,     /* DPP FAP device */
    bcmDeviceMemberDFE = 3,     /* DFE Fabric element device */
    bcmDeviceMemberDNXC = 4     /* DNX FAP or Fabric element device */
} bcm_device_member_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Check if device is member in group 'member_type' */
extern int bcm_device_member_get(
    int unit, 
    uint32 flags, 
    bcm_device_member_t member_type, 
    int *is_member);

/* Initialize a device excluding stacking functionality. */
extern int bcm_clear(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize the BCM Information structure. */
extern void bcm_info_t_init(
    bcm_info_t *info);

#if !defined(BCM_WARM_BOOT_SUPPORT)
/* Define as empty just to be safe. */
#define _bcm_shutdown(unit)     BCM_E_NONE 
#endif

#ifndef BCM_HIDE_DISPATCHABLE

#if defined(BCM_WARM_BOOT_SUPPORT)
/* _bcm_shutdown */
extern int _bcm_shutdown(
    int unit);
#endif

#endif /* BCM_HIDE_DISPATCHABLE */

/* Device State. */
typedef enum bcm_device_state_e {
    bcmDeviceStateAttach = 0,   /* Device attach. */
    bcmDeviceStateDetach = 1    /* Device detach. */
} bcm_device_state_t;

/* Attach/detach information. */
typedef struct bcm_attach_info_s {
    int unit;               /* Attach/detach unit. */
    const char *type;       /* Attach/detach type. */
    const char *subtype;    /* Attach/detach subtype. */
    int remunit;            /* Attach/detach remunit. */
} bcm_attach_info_t;

/* Initialize a BCM Attach Information structure. */
extern void bcm_attach_info_t_init(
    bcm_attach_info_t *info);

/* Attach/detach callback */
typedef int (*bcm_attach_cb_t)(
    int unit, 
    bcm_device_state_t state, 
    bcm_attach_info_t *info, 
    void *user_data);

/* 
 * Register/unregister a callback function to be called before
 * BCM device attachment and after BCM device detachment.
 */
extern int bcm_attach_register(
    int unit, 
    bcm_attach_cb_t cb, 
    void *user_data);

/* 
 * Register/unregister a callback function to be called before
 * BCM device attachment and after BCM device detachment.
 */
extern int bcm_attach_unregister(
    int unit, 
    bcm_attach_cb_t cb, 
    void *user_data);

/* Detach retry. */
typedef struct bcm_detach_retry_s {
    uint32 poll_usecs;  /* Time poll interval in usecs. */
    int num_retries;    /* Number of retries. */
} bcm_detach_retry_t;

/* Initialize a BCM Detach Retry structure. */
extern void bcm_detach_retry_t_init(
    bcm_detach_retry_t *retry);

/* Set/get the poll interval and number of retries before detach exits. */
extern int bcm_detach_retry_set(
    int unit, 
    bcm_detach_retry_t *retry);

/* Set/get the poll interval and number of retries before detach exits. */
extern int bcm_detach_retry_get(
    int unit, 
    bcm_detach_retry_t *retry);

/* This structure should contain user's information. */
typedef struct bcm_init_advanced_info_s {
    uint32 debug_flags; /* Holds flags,  such as print time stamp,  print memory
                           utilization, test memory leak, etc. */
} bcm_init_advanced_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * API for initialization, caller can decide himself on flags and init
 * info
 */
extern int bcm_init_advanced(
    int unit, 
    bcm_init_advanced_info_t *init_advanced_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT 0x1        /* Memory Leak detection
                                                          Activated. */
#define BCM_INIT_ADVANCED_F_MEM_LOG         0x2        /* Memory Logging. */
#define BCM_INIT_ADVANCED_F_TIME_STAMP      0x4        /* Time Stamp saving
                                                          Activated. */
#define BCM_INIT_ADVANCED_F_TIME_LOG        0x8        /* Time Stamp saving
                                                          Activated. */
#define BCM_INIT_ADVANCED_F_SWSTATE_LOG     0x10       /* SW state resources
                                                          Logging. */
#define BCM_INIT_ADVANCED_F_ACCESS_ONLY     0x20       /* Access only booting.
                                                          Will run just steps
                                                          marked with
                                                          UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY
                                                          and
                                                          UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY. */
#define BCM_INIT_ADVANCED_F_TIME_ANALYZER_LOG 0x40       /* Display time analyzer
                                                          logging. */
#define BCM_INIT_ADVANCED_F_MULTITHREAD     0x80       /* Multithreading
                                                          activated. */
#define BCM_INIT_ADVANCED_F_KBP_MULTITHREAD 0x100      /* KBP Multithreading
                                                          activated. */
#define BCM_INIT_ADVANCED_F_HEAT_UP         0x200      /* Heat production
                                                          activated. Will run
                                                          minimal startup to
                                                          cause the device to
                                                          produce more heat. */

/* Set/get the warmboot state in BCM Layer. */
extern int bcm_warmboot_set(
    int unit, 
    int warmboot);

/* Set/get the warmboot state in BCM Layer. */
extern int bcm_warmboot_get(
    int unit, 
    int *warmboot);

#endif /* __BCM_INIT_H__ */
