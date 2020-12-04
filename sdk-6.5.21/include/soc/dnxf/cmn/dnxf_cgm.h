/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF CGM H
 */

#ifndef _SOC_DNXF_CGM_H_
#define _SOC_DNXF_CGM_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <shared/fabric.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod.h>

#define SOC_DNXF_CGM_MAX_NOF_TH_REG_FIELDS           (9)




_shr_error_t soc_dnxf_cgm_profile_threshold_set(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int threshold_value);
_shr_error_t soc_dnxf_cgm_profile_threshold_get(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int *threshold_value);

_shr_error_t soc_dnxf_cgm_link_profile_set(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count,
    soc_port_t * links);
_shr_error_t soc_dnxf_cgm_link_profile_get(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count_max,
    uint32 *links_count,
    soc_port_t * links);

_shr_error_t soc_dnxf_cgm_pipe_level_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *level,
    int *pipe);
_shr_error_t soc_dnxf_cgm_cast_prio_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *cast,
    int *priority);

typedef enum soc_dnxf_cgm_reg_structure_e
{
    soc_dnxf_cgm_reg_structure_1,       
    soc_dnxf_cgm_reg_structure_2,       
    soc_dnxf_cgm_reg_structure_3,       
    soc_dnxf_cgm_reg_structure_4        
} soc_dnxf_cgm_reg_structure_t;


typedef struct soc_dnxf_cgm_registers_table_s
{
    soc_reg_t register_name;
    soc_field_t field_name_array[SOC_DNXF_CGM_MAX_NOF_TH_REG_FIELDS];
} soc_dnxf_cgm_registers_table_t;


typedef struct soc_dnxf_cgm_threshold_to_reg_binding_s
{
    _shr_dnxf_threshold_type_t threshold_type;
    soc_dnxf_cgm_reg_structure_t th_handle_case;
    uint32 nof_regs_to_write_to;
    const soc_dnxf_cgm_registers_table_t *reg_description_ptr;
} soc_dnxf_cgm_threshold_to_reg_binding_t;

#endif 
