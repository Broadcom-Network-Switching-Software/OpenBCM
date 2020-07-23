/*************************************************************************************
 *                                                                                   *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************************************************************************************
 *************************************************************************************
 *                                                                                   *
 *  Revision      :         *
 *                                                                                   *
 *  Description   :  Common types used by Serdes API functions                       *
 */

/** @file srds_api_types.h
 * Common types used by Serdes API functions
 * This file contains types which are common to all serdes cores
 */

#ifndef SRDS_API_TYPES_H
#define SRDS_API_TYPES_H

#include <phymod/phymod.h>
/* Opaque SerDes Access Struct to be defined by Upper level Software */
struct srds_access_s;
/* typedef struct srds_access_s srds_access_t; */

/* Opaque SerDes Mutex Struct to be defined by Upper level Software */
struct srds_mutex_s;
typedef struct srds_mutex_s srdes_mutex_t;
typedef phymod_access_t srds_access_t;

/*------------------------------*/
/** Serdes Info Table Structure */
/*------------------------------*/
typedef struct {
    uint32_t signature;
    uint32_t diag_mem_ram_base;
    uint32_t diag_mem_ram_size;
    uint32_t core_var_ram_base;
    uint32_t core_var_ram_size;
    uint32_t lane_var_ram_base;
    uint32_t lane_var_ram_size;
    uint32_t trace_mem_ram_base;
    uint32_t trace_mem_ram_size;
    uint32_t micro_var_ram_base;
    uint32_t ucode_version;
    uint16_t grp_ram_size;
    uint8_t  lane_count;
    uint8_t  trace_memory_descending_writes;
    uint8_t  micro_count;
    uint8_t  micro_var_ram_size;
    uint8_t  info_table_version;
} srds_info_t;
#endif
