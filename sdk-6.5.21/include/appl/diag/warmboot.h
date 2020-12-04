/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	sh.h
 * Purpose:	Types and structures for command table entires and
 *		command return values for cshell commands.
 */

#ifndef _DIAG_WARMBOOT_H
#define _DIAG_WARMBOOT_H

#ifdef BCM_WARM_BOOT_SUPPORT
extern int appl_scache_file_close(int unit);
extern int appl_scache_file_open(int unit, int warm_boot, char *filename);
extern void appl_warm_boot_event_handler_cb(int unit, soc_switch_event_t event,
                                            uint32 arg1, uint32 arg2, uint32 arg3,
                                            void* userdata);
#ifdef BCM_PETRA_SUPPORT
extern int appl_scache_user_buffer(int unit);
#endif

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _DIAG_WARMBOOT_H */
