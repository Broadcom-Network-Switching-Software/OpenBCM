/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY_PHYMOD_CTRL_H_
#define _PHY_PHYMOD_CTRL_H_

#include <soc/types.h>
#include <phymod/phymod.h>
#include <phymod/phymod_symbols.h>

/*
 * Generic object for managing cores and lanes.
 */
typedef struct soc_phy_obj_s {
    struct soc_phy_obj_s *next;
    int obj_id;
    void *owner;
} soc_phy_obj_t;


/*
 *  Ardon core init flags
 *  per core: core_init was called at each stage
 */ 
#define ARDON_CORE_INIT_CALLED_1 0x1 /** Ardon core init stage 1 done */
#define ARDON_CORE_INIT_CALLED_2 0x2 /** Ardon core init stage 2 done  */


/*
 * Core object
 *
 * The core object is a physical entity which is independent of other
 * physical entities.
 */
typedef struct soc_phymod_core_s {
    soc_phy_obj_t obj;
    phymod_core_access_t pm_core;
    phymod_bus_t pm_bus;
    int ref_cnt;
    int init;
    int unit;
    int port;
    int (*read)(int, uint32, uint32, uint16*);
    int (*write)(int, uint32, uint32, uint16);
    int (*wrmask)(int, uint32, uint32, uint16, uint16);
    void *device_aux_modes ;
    phymod_core_init_config_t init_config;
    uint32_t flags;
} soc_phymod_core_t;

/*
 * PHY object
 *
 * A PHY is one or more lanes belonging to the same core object. Most
 * PHY hardware is capable of configuring multiple lanes
 * simultaneously, so we do not create an object for each lane of a
 * logical port.
 */
typedef struct soc_phymod_phy_s {
    soc_phy_obj_t obj;
    phymod_phy_access_t pm_phy;
    soc_phymod_core_t *core;
    phymod_phy_init_config_t  init_config;
} soc_phymod_phy_t;

/*
 * Logical port object
 *
 * This object represents a logical port as seen from the MAC (and
 * usually also the application). It consists of one or more lane
 * groups, depending on how many core objects the logical port spans.
 */
typedef struct soc_phymod_ctrl_s {
    int unit;
    int num_phys;
    int main_phy;
    soc_phymod_phy_t *phy[PHYMOD_CONFIG_MAX_CORES_PER_PORT];
    phymod_symbols_t *symbols;
    void (*cleanup)(struct soc_phymod_ctrl_s*);
} soc_phymod_ctrl_t;


extern int
soc_phymod_miim_bus_read(int unit, uint32_t addr, uint32_t reg, uint32_t *data);

extern int
soc_phymod_miim_bus_write(int unit, uint32_t addr, uint32_t reg, uint32_t data);

extern int
soc_phymod_core_create(int unit, int core_id, soc_phymod_core_t **core);

extern int
soc_phymod_core_destroy(int unit, soc_phymod_core_t *core);

extern int
soc_phymod_core_find_by_id(int unit, int core_id, soc_phymod_core_t **core);

extern int
soc_phymod_phy_create(int unit, int phy_id, soc_phymod_phy_t **phy);

extern int
soc_phymod_phy_destroy(int unit, soc_phymod_phy_t *phy);

extern int
soc_phymod_phy_find_by_id(int unit, int phy_id, soc_phymod_phy_t **phy);

#endif /* _PHY_PHYMOD_CTRL_H_ */
