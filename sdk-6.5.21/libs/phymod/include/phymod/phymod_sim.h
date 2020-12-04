/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __PHYMOD_SIM_H__
#define __PHYMOD_SIM_H__

#include <phymod/phymod_types.h>

typedef enum {
    phymodSimEventNone,
    phymodSimEventLinkUp,
    phymodSimEventLinkDown,
    phymodSimEventAutoneg100,
    phymodSimEventAutoneg1000,
    phymodSimEventAutoneg10000,
    phymodSimEventCount
} phymod_sim_event_t;

typedef struct phymod_sim_entry_s {
    uint32_t flags;
    uint32_t addr;
    uint32_t data;
} phymod_sim_entry_t;

typedef struct phymod_sim_data_s {
    phymod_sim_entry_t *entries;
    int num_entries;
    int entries_used;
} phymod_sim_data_t;

typedef struct phymod_sim_drv_s {
    int (*init)(phymod_sim_data_t *psim, int num_ent, phymod_sim_entry_t *ent);
    int (*reset)(phymod_sim_data_t *psim);
    int (*read)(phymod_sim_data_t *psim, uint32_t addr, uint32_t *data);
    int (*write)(phymod_sim_data_t *psim, uint32_t addr, uint32_t data);
    int (*event)(phymod_sim_data_t *psim, phymod_sim_event_t evt);
} phymod_sim_drv_t;

typedef struct phymod_sim_s {
    phymod_sim_data_t data;
    phymod_sim_drv_t *drv;
} phymod_sim_t;

extern int
phymod_sim_init(phymod_sim_t *pms, int num_ent, phymod_sim_entry_t *ent);

extern int
phymod_sim_reset(phymod_sim_t *pms);

extern int
phymod_sim_read(phymod_sim_t *pms, uint32_t addr, uint32_t *data);

extern int
phymod_sim_write(phymod_sim_t *pms, uint32_t addr, uint32_t data);

extern int
phymod_sim_event(phymod_sim_t *pms, phymod_sim_event_t event);

#endif /* __PHYMOD_SIM_H__ */
