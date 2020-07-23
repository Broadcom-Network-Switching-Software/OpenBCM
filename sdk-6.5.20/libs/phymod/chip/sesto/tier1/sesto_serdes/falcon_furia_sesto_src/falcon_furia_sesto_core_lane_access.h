/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */


/* 
 */

/* @file core_lane_access.h
 * Generic interface to access individual cores and lanes. The implementation
 * for the functions is chip dependent.
 */

#ifndef FALCON_FURIA_CORE_LANE_ACCESS_H_
#define FALOCN_FURIA_CORE_LANE_ACCESS_H_

#include <phymod/phymod_types.h>
/** Get the number of cores in the current chip.
 * @return the number of PMD IP cores in the current chip.
 */
uint8_t get_num_cores(void);

/** Get the index of the current PMD IP core.
 * @return the index of the current core.
 */
uint8_t get_core(void);

/** Set the index of the current PMD IP core.
 * @param core_index the index for the newly selected core
 * @return 0 if core_index is valid, 1 otherwise.
 */
uint8_t set_core(uint8_t core_index);

/** Get printable information of the lane and core.
 * @return a string with printable information of the lane and core.
 */
const char* get_core_lane_info(void);

/** Get number of lanes in the current PMD IP core.
 * @return the number of lanes in the current core.
 */
uint8_t get_num_lanes(void);

/** Get the index of the current lane in the current PMD IP core.
 * @return the index of the current lane.
 */
uint8_t get_lane(void);

/** Set the index of the current quad-lane core.
 * @param lane_index the index for the newly selected core
 * @return 0 if core_index is valid, 1 otherwise.
 */
uint8_t set_lane(uint8_t lane_index);

uint16_t ip_rd_reg(uint16_t address);
void ip_wr_reg(uint16_t address, uint16_t val);

#endif

