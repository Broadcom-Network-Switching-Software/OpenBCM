/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
    int (*read32)(phymod_sim_data_t *psim, uint32_t addr, uint32_t *data);
    int (*write32)(phymod_sim_data_t *psim, uint32_t addr, uint32_t data);
    int (*event)(phymod_sim_data_t *psim, phymod_sim_event_t evt);
} phymod_sim_drv_t;

typedef struct phymod_sim_s {
    phymod_sim_data_t data;
    phymod_sim_drv_t *drv;
} phymod_sim_t;

/*!
 * @brief Function to initialize a SIM instance for a PHY.
 *
 * @param [in] pms - phymod sim instance and driver data.
 * @param [in] num_ent - Number of register entries in SIM instance.
 * @param [in] ent - Array of register entries.
 */
extern int
phymod_sim_init(phymod_sim_t *pms, int num_ent, phymod_sim_entry_t *ent);

/*!
 * @brief Reset the SIM instance.
 *
 * @param [in] pms - phymod sim instance and driver data.
 */
extern int
phymod_sim_reset(phymod_sim_t *pms);

/*!
 * @brief Function to read Serdes register.
 *        The PCS and PMD registers are 16-bit wide.
 *
 * @param [in] pms - phymod sim instance and driver data.
 * @param [in] addr - register address.
 * @param [out] data - 16-bit register value.
 */
extern int
phymod_sim_read(phymod_sim_t *pms, uint32_t addr, uint32_t *data);

/*!
 * @brief Function to write data to 16-bit wide Serdes register.
 *        32-bit data is encoded with 16-bit (lower bits) data
 *        and higher order 16-bits are encoded with the mask.
 *        This function supports maskable writes or register
 *        modify operations.
 *
 * @param [in] pms - phymod sim instance and driver data.
 * @param [in] addr - register address.
 * @param [in] data - 16-bit register value, 16-bit mask.
 */
extern int
phymod_sim_write(phymod_sim_t *pms, uint32_t addr, uint32_t data);

/*!
 * @brief Function to read 32-bit wide Serdes register.
 *
 * @param [in] pms - phymod sim instance and driver data.
 * @param [in] addr - register address.
 * @param [out] data - 32-bit register value.
 */
extern int
phymod_sim_read32(phymod_sim_t *pms, uint32_t addr, uint32_t *data);

/*!
 * @brief Function to write data to 32-bit wide Serdes register.
 *        This does not support maskable writes and should not be
 *        used for register modify operations.
 *
 * @param [in] pms - phymod sim instance and driver data.
 * @param [in] addr - register address.
 * @param [in] data - 32-bit register value.
 */
extern int
phymod_sim_write32(phymod_sim_t *pms, uint32_t addr, uint32_t data);

extern int
phymod_sim_event(phymod_sim_t *pms, phymod_sim_event_t event);

#endif /* __PHYMOD_SIM_H__ */
