
/*
 * $Id: memory.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * New access data structures test functions
 */

#include <soc/drv.h>

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#endif /* BCM_ACCESS_SUPPORT */

#define BSL_LOG_MODULE BSL_LS_SOC_ACCESS
#include <soc/sand/shrextend/shrextend_debug.h>

#include "../testlist.h"


/* Generic function that calls functions for testing new access */
int access_test(int unit, args_t *a, void *pa) {
#ifdef BCM_ACCESS_SUPPORT
    int rv;

    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
    
    if (runtime_info == NULL) {
        return -1;
    }
    COMPILER_REFERENCE(pa);

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "New access consistency test:\n")));
    rv = access_data_structure_consistency_test(runtime_info);

    return rv;
#else
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The new access consistency test is NOT relevant for this device!\n")));
    return -1;
#endif /* BCM_ACCESS_SUPPORT */
}

#ifdef BCM_ACCESS_SUPPORT
/* Test the consistency of the access_runtime_info_t data structure */
int access_data_structure_consistency_test(
    access_runtime_info_t *runtime_info        /* Access runtime info of the device */
) {
    const access_device_type_info_t *device_info; /* Constant access information for the device type */
    const access_device_regmem_t *rm_info, *rm_info_of_prev_field, *rm_info_of_curr_field, *reg_info, *mem_info, *l_regmem;
    const access_device_block_t *block_info, *l_block;
    const access_block_instance_t *block_instance_info;
    const access_device_field_t *field_info, *field_before, *field_after, *l_field;
    access_local_regmem_id_t nof_regmems, local_regmem_id;
    access_local_field_id_t local_field_id;
    access_local_block_id_t local_block_id, prev_local_block_id = -1;
    const uint64 *default_values_ptr;
    unsigned regmem_i, field_i, block_i, instance_i, reg_i, mem_i, regmem_map_i, field_map_i, block_map_i, regmems_pointed_to = 0, fields_pointed_to = 0, blocks_pointed_to = 0;
    unsigned this_is_a_mem, this_is_the_1st_mem, did_we_see_a_mem = 0;
    unsigned last_alias_mem = -2, last_non_alias_mem = -2, have_error = 0;
    uint8 *memory = NULL;
    int unit = 0;

    if (runtime_info == NULL) {
        return -1;
    }
    
    unit = runtime_info->unit;
    device_info = runtime_info->device_type_info;
    default_values_ptr = device_info->default_values;

    nof_regmems = device_info->nof_regs + device_info->nof_mems;

    /* Loop over all regmems */
    for (regmem_i = 0; regmem_i < nof_regmems; regmem_i++) {
        /* The register or memory to access */
        rm_info = device_info->local_regs + regmem_i;
        this_is_the_1st_mem = 0;
        if ((this_is_a_mem = rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY)) {
            if (!did_we_see_a_mem) {
                did_we_see_a_mem = 1;
                this_is_the_1st_mem = 1;
            }
        }
        /* Loop over the fields of the regmem */
        for (field_i = rm_info->fields_index; field_i < rm_info->fields_index + rm_info->nof_fields; field_i++) {
            field_info = device_info->local_fields + field_i;
            /* Check that the regmem is marked as the regemem of the field */
            if (field_info->regmem != regmem_i) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Regmem %u %s field %u %s is marked as belonging to a different regmem: local regmem ID %u.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : "", field_i, global_field_name_indices[field_info->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_info->global_field_id]] : "", field_info->regmem));
                have_error = 1;
            }
        }
        /* Check that the field before and the field after the regmem fields belong to a different regmem */
        field_before = (device_info->local_fields + rm_info->fields_index) - 1;
        field_after = device_info->local_fields + (rm_info->fields_index + rm_info->nof_fields);
        if ((device_info->nof_fields != 0 && regmem_i && !this_is_the_1st_mem && field_before->regmem >= regmem_i) ||
             (regmem_i != (nof_regmems - 1) && field_after->regmem <= regmem_i)) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Fields %u %s or %u %s belong to wrong regmem %u %s.\n"), (unsigned)(field_before - device_info->local_fields), global_field_name_indices[field_before->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_before->global_field_id]] : "", (unsigned)(field_after - device_info->local_fields), global_field_name_indices[field_after->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_after->global_field_id]] : "", regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
            have_error = 1;
        }
        /* Check that we first have all the registers and afterwards have all the memories */
        if (regmem_i != 0 && !this_is_the_1st_mem && ((rm_info - 1)->flags & ACCESS_REGMEM_FLAG_MEMORY) != this_is_a_mem) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Incorrect memory flag for regmem %u %s.\n"), regmem_i - 1, global_regmem_name_indices[(rm_info-1)->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[(rm_info-1)->global_regmem_id]] : ""));
            have_error = 1;
        }
        /* Check that default_value_pointer points to the correct current location in device_info->default_values */
        if (!this_is_a_mem && rm_info->width_in_bits > 64) {
            if (rm_info->u.default_value_pointer != default_values_ptr) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Incorrect default value pointer for register %u %s.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
                have_error = 1;
            }
            default_values_ptr += (rm_info->width_in_bits + 63) / 64;
        }
        /* If a memory alias, then check that the memory aliases immediately follow the memory that they are an alias of */
        if (this_is_a_mem) {
            if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0) {
                if ((last_non_alias_mem + 1 != regmem_i && last_alias_mem + 1 != regmem_i) ||
                     rm_info->u.mem.alias_regmem != last_non_alias_mem) {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Incorrect memory alias for %u %s.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
                    have_error = 1;
                }
                last_alias_mem = regmem_i;
            } else {
                last_non_alias_mem = regmem_i;
            }
        }
        /* Check that base_address is bigger than the previous regmem (or equal if this is a memory alias), or that this is the first regmem of a new block */
        if ((rm_info->flags & (ACCESS_REGMEM_FLAG_MEMORY | ACCESS_REGMEM_FLAG_IS_ALIAS)) == 
              (ACCESS_REGMEM_FLAG_MEMORY | ACCESS_REGMEM_FLAG_IS_ALIAS)) {
            if (rm_info->base_address != (rm_info - 1)->base_address) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "There is an error at the base_address of regmem %u %s.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
                have_error = 1;
            }
        /* Check that the block ID is the same and the address increased, or that the local block ID increased due to moving to a new block. */
        } else if (prev_local_block_id == rm_info->local_block_id) {
            if (rm_info->base_address <= (rm_info - 1)->base_address) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "There is an error at the base_address of regmem %u %s.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
                have_error = 1;
            }
        /* Check that local_block_id is not smaller than in the previous regmem  */
        } else if (rm_info->local_block_id < prev_local_block_id && regmem_i !=0 && !this_is_the_1st_mem) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Regmem %u %s has a smaller local block ID than the previous regmem\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
            have_error = 1;
        }
        /* Check that name_index is below the nof global names */
        if (global_regmem_name_indices[rm_info->global_regmem_id] >= ACCESS_NOF_GLOBAL_NAMES) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The name_index %u of a regmem %u is above nof global names.\n"), global_regmem_name_indices[rm_info->global_regmem_id], regmem_i));
            have_error = 1;
        }
        /* Check that each regmem has a none NULL description */
        if (rm_info->description == NULL) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The description of a regmem %u %s is empty.\n"), regmem_i, global_regmem_name_indices[rm_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]] : ""));
            have_error = 1;
        }
        prev_local_block_id = rm_info->local_block_id;
    }

    /* Loop over all fields */
    for (field_i = 0; field_i < device_info->nof_fields; field_i++) {
        field_info = device_info->local_fields + field_i;
        /* Check that name_index is below the nof global names */
        if (global_regmem_name_indices[field_info->global_field_id] >= ACCESS_NOF_GLOBAL_NAMES) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The name_index %u of a field %u is above nof global names.\n"), global_field_name_indices[field_info->global_field_id], field_i));
            have_error = 1;
        }
        /* Get the current and the previous regmem of a field */
        rm_info_of_curr_field = device_info->local_regs + field_info->regmem;
        rm_info_of_prev_field = device_info->local_regs + (field_info->regmem - 1);
		/* Check if the field belongs to the same regmem as the previous field */
        if (field_i && field_info->regmem == (field_info - 1)->regmem) {
            /* Check that the start of the field is greater than or equal to the start of the previous field + the nof bits of the previous field unless both fields are marked with a flags to be overllaping (ACCESS_FIELD_FLAG_COLLISION). */
            if ((field_info->flags & ACCESS_FIELD_FLAG_COLLISION) == 0  || ((field_info - 1)->flags & ACCESS_FIELD_FLAG_COLLISION) == 0) {
                if (field_info->start_bit < (field_info - 1)->start_bit + (field_info - 1)->size_in_bits) {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The field %u %s and the next one are overllaping although there are not marked with flag ACCESS_FIELD_FLAG_COLLISION.\n"), field_i - 1, global_field_name_indices[(field_info - 1)->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[(field_info - 1)->global_field_id]] : ""
                    ));
                    have_error = 1;
                }
            }
        }
        /* Check that the local regmem ID of the current field is bigger than that of the previous field or this is the single place where we moved from register fields (regmem of previous field is a register) to memory fields (regmem of the current field is a memory). */
        else if (field_i > 0 && !((rm_info_of_prev_field->flags & ACCESS_REGMEM_FLAG_MEMORY) == 0 && (rm_info_of_curr_field->flags & ACCESS_REGMEM_FLAG_MEMORY) != 0)) {
            if (field_info->regmem < (field_info - 1)->regmem) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The fields %u %s and %u %s point to wrong regmems.\n"), field_i, global_field_name_indices[field_info->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_info->global_field_id]] : "", field_i - 1, global_field_name_indices[(field_info - 1)->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[(field_info - 1)->global_field_id]] : ""));
                have_error = 1;
            }
        }
    }

    /* Loop over all local blocks */
    for (block_i = 0; block_i < device_info->nof_blocks; block_i++) {
        block_info = device_info->blocks + block_i;
        /* Loop over the block_instances of the block */
        for (instance_i = block_info->instances_start_index; instance_i < block_info->instances_start_index +  block_info->nof_instances; instance_i++) {
            /* Check that the block_instance is marked as the block_instance of the block */
            block_instance_info = device_info->block_instances + instance_i;
            if (block_instance_info->block_id != block_i) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The block instance with id %u is not marked as an instance of a block %s.\n"), instance_i, access_global_block_types[block_info->block_type].block_name));
                have_error = 1;
            }
        }
        /* Check that the local_block id equals to the previous one + 1 */
        if (block_i && (block_info - 1)->instances_start_index + (block_info - 1)->nof_instances != block_info->instances_start_index) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The instances of blocks %u and %u are not consecutive.\n"), (unsigned)block_i - 1, (unsigned)block_i));
            have_error = 1;
        } 
        /* Loop over the registers of the block */
        for (reg_i = block_info->registers_start; reg_i < block_info->registers_start + block_info->nof_registers; reg_i++) {
            /* Verify that points back to the same block */
            reg_info = device_info->local_regs + reg_i;
            if (reg_info->local_block_id != block_i) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The block register %u %s with local_block_id %u is not marked as an register of a block %s.\n"), reg_i, global_regmem_name_indices[reg_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[reg_info->global_regmem_id]] : "", (unsigned)reg_info->local_block_id, access_global_block_types[block_info->block_type].block_name));
                have_error = 1;
            }
        }
        /* Loop over the memories of the block */
        for (mem_i = block_info->memories_start; mem_i < block_info->memories_start + block_info->nof_memories; mem_i++) {
            /* Verify that points back to the same block */
            mem_info = device_info->local_mems + mem_i;
            if (mem_info->local_block_id != block_i) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The block memory %u %s with local_block_id %u is not marked as an memory of a block %s.\n"), mem_i, global_regmem_name_indices[mem_info->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[mem_info->global_regmem_id]] : "", (unsigned)mem_info->local_block_id, access_global_block_types[block_info->block_type].block_name));
                have_error = 1;
            }
        }
        /* Check that each local block points to the right global block by converting it using blocks_global2local_map */
        if (block_i != device_info->blocks_global2local_map[block_info->block_type]) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The local block %s does not point to the right global block.\n"), access_global_block_types[block_info->block_type].block_name));
            have_error = 1;
        }
    }

    memory = sal_alloc2(device_info->nof_blocks * sizeof(uint8), SAL_ALLOC_F_ZERO, "MC for regmem_mappings testing");
    if (memory == NULL)
    { 
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Failed to allocate memory for regmem_mappings testing.\n")));
    } else {
        /* Loop over all regmem_mappings */
        for (regmem_map_i = 0; regmem_map_i < device_info->nof_regmem_mappings; regmem_map_i++) {
            local_regmem_id = device_info->regmems_global2local_map[regmem_map_i];
            /* Check that if we have something that is not -1, it's in range - the value is between 0 and the max size of the array */
            if (local_regmem_id >= nof_regmems) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Regmem global2local_map entry %u is invalid.\n"), local_regmem_id));
                have_error = 1;
            }
            /* Check that the name_index of the global regmem is equal to the name index of the local regmem pointed to */
            /* Get the local regmem */
            l_regmem = device_info->local_regs + local_regmem_id;
            if (global_regmem_name_indices[l_regmem->global_regmem_id] != global_regmem_name_indices[regmem_map_i]) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The name_index of the global regmem %u %s is not equal to the name_index of the local regmem %u %s pointed to.\n"), regmem_map_i, global_regmem_name_indices[regmem_map_i] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[regmem_map_i]] : "", local_regmem_id, global_regmem_name_indices[l_regmem->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[l_regmem->global_regmem_id]] : ""));
                have_error = 1;
            }
            /* Check that each global points to exactly one local */
            /* Check that point to 0 and mark with 1, unless it's not the first time we point to it */
            if (memory[regmem_map_i] == 0) {
                memory[regmem_map_i] = 1;
                regmems_pointed_to++;
            } else {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "It is the second time that the global regmem %u %s points to the local regmem %u %s.\n"), regmem_map_i, global_regmem_name_indices[regmem_map_i] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[regmem_map_i]] : "", local_regmem_id, global_regmem_name_indices[l_regmem->global_regmem_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_regmem_name_indices[l_regmem->global_regmem_id]] : ""));
                have_error = 1;
            }
        }
        sal_free(memory);
    }
    
    /* Check that the number of local regmems pointed to is equals to the number of local regmems */
    if (nof_regmems != regmems_pointed_to) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The number of local regmems pointed to is not equals to the number of local regmems.\n")));
        have_error = 1;
    }

    memory = sal_alloc2(device_info->nof_blocks * sizeof(uint8), SAL_ALLOC_F_ZERO, "MC for field_mappings testing");
    if (memory == NULL)
    { 
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Failed to allocate memory for field_mappings testing.\n")));
    } else {
        /* Loop over all field_mappings */
        for (field_map_i = 0; field_map_i < device_info->nof_field_mappings; field_map_i++) {
            local_field_id = device_info->fields_global2local_map[field_map_i];
            /* Check that if we have something that is not -1, it's in range - the value is between 0 and the max size of the array */
            if (local_field_id != -1) {
                if (local_field_id >= device_info->nof_fields) {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Field global2local_map %u is invalid.\n"), local_field_id));
                    have_error = 1;
                }
                /* Check that the name_index of the global field is equal to the name index of the local field pointed to */
                /* Get the local field */
                l_field = device_info->local_fields + local_field_id;
                if (global_field_name_indices[l_field->global_field_id] != global_field_name_indices[field_map_i]) {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The name_index of the global field %u %s is not equal to the name_index of the local field %u %s pointed to.\n"), field_map_i, global_field_name_indices[field_map_i] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_map_i]] : "", local_field_id, global_field_name_indices[l_field->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[l_field->global_field_id]] : ""));
                    have_error = 1;
                }
                /* Check that each global points to exactly one local */
                /* Check that point to 0 and mark with 1, unless it's not the first time we point to it */
                if (memory[field_map_i] == 0) {
                    memory[field_map_i] = 1;
                    fields_pointed_to++;
                } else {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "It is the second time that the global field %u %s points to the local field %u %s.\n"), field_map_i, global_field_name_indices[field_map_i] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[field_map_i]] : "", local_field_id, global_field_name_indices[l_field->global_field_id] < ACCESS_NOF_GLOBAL_NAMES ? access_global_names[global_field_name_indices[l_field->global_field_id]] : ""));
                    have_error = 1;
                }
            }
        }
        sal_free(memory);
    }

    /* Check that the number of local fields pointed to is equals to the number of local fields */
    if (device_info->nof_fields != fields_pointed_to) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The number of local fields pointed to is not equals to the number of local fields.\n")));
        have_error = 1;
    }

    memory = sal_alloc2(device_info->nof_blocks * sizeof(uint8), SAL_ALLOC_F_ZERO, "MC for block_mappings testing");
    if (memory == NULL)
    { 
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Failed to allocate memory for block_mappings testing.\n")));
    } else {
        /* Loop over all block_mappings */
        for (block_map_i = 0; block_map_i < device_info->nof_block_mappings; block_map_i++) {
            local_block_id = device_info->blocks_global2local_map[block_map_i];
            /* Check that if we have something that is not -1, it's in range - the value is between 0 and the max size of the array */
            if (local_block_id >= device_info->nof_blocks) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Block global2local_map %d is invalid.\n"), local_block_id));
                have_error = 1;
            }
            /* Check that the name_index of the global block is equal to the name index of the local block pointed to */
            /* Get the local block */
            l_block = device_info->blocks + local_block_id;
            if (l_block->block_type != block_map_i) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The block_type of the global block %s is not equal to the block_type of the local block %s pointed to.\n"), access_global_block_types[block_map_i].block_name, access_global_block_types[l_block->block_type].block_name));
                have_error = 1;
            }
            /* Check that each global points to exactly one local */
            /* Check that point to 0 and mark with 1, unless it's not the first time we point to it */
            if (memory[block_map_i] == 0) {
                memory[block_map_i] = 1;
                blocks_pointed_to++;
            } else {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "It is the second time that the global block %s points to the local block %s.\n"), access_global_block_types[block_map_i].block_name, access_global_block_types[l_block->block_type].block_name));
                have_error = 1;
            }
        }
        sal_free(memory);
    }

    /* Check that the number of local blocks pointed to is equals to the number of local blocks */
    if (device_info->nof_blocks != blocks_pointed_to) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "The number of local blocks pointed to is not equals to the number of local blocks.\n")));
        have_error = 1;
    }

    

    if (have_error) {
        test_error(unit, "New Access consistency test failed (TR 190)!\n");
        return -1;
    } else {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "New Access consistency test finished successfully!\n")));
        return 0;
    }
}
#endif /* BCM_ACCESS_SUPPORT */
