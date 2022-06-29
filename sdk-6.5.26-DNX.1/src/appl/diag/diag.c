/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 * Purpose: Common device register/mem diagnostic functions.
 */
#if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT)
#include <appl/diag/system.h>
#include <appl/diag/diag.h>

#include <shared/bsl.h>
#include <sdk_config.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <soc/defs.h>
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif /* BCM_ACCESS_SUPPORT */

/* *INDENT-OFF* */

#ifdef BCM_ACCESS_SUPPORT
/* Print the details of the regmem to be accessed: name, array index, block instance, memory index */
void diag_print_regmem_info(access_runtime_info_t *runtime_info, const access_device_regmem_t *rm_info, access_block_instance_num_t inst_i, uint16 arr_ind, uint32 mem_ind)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
    const access_device_block_t *local_block = device_info->blocks + rm_info->local_block_id;
    
    /* Print name */
    cli_out("%s", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
    /* Print array index */
    if (rm_info->nof_array_indices > 1)
    {
        cli_out("[%u]", arr_ind);
    }
    /* Print block instance */
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT)
    {
        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            cli_out(".*");
        }
        else
        {
            cli_out(".%s", SOC_CONTROL(runtime_info->unit)->info.port_name[inst_i]);
        }
    }
    else
    {
        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            cli_out(".%s*", access_global_block_types[local_block->block_type].block_name);
        }
        else
        {
            cli_out(".%s%u", access_global_block_types[local_block->block_type].block_name, inst_i);
        }
    }
    /* Print memory index */
    if (rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY && mem_ind != (uint32)(-1))
    {
        cli_out("[%u]", mem_ind);
    }
    cli_out("\n");
}
#endif /* BCM_ACCESS_SUPPORT */

/* Return the width in bits of the register */
int diag_reg_bits(int unit, char* reg_name)
{
    int reg_bits = 0;

    if (!SOC_UNIT_VALID(unit)) 
    {
        cli_out("Invalid unit.\n");
        return reg_bits;
    }
    if (reg_name == NULL) 
    {
        cli_out("NULL reg_name given.\n");
        return reg_bits;
    }
    if (*reg_name == '$') 
    {
        reg_name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;

        parse_register_name_rv = access_parse_register_name(runtime_info, reg_name, 0, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return reg_bits;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", reg_name);
            return reg_bits;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;
        
        reg_bits = rm_info->width_in_bits;
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;

        if (soc_regaddrlist_alloc(&alist) < 0) 
        {
            cli_out("Could not allocate address list.  Memory error.\n");
            return reg_bits;
        }
        if (parse_symbolic_reference(unit, &alist, reg_name) < 0) 
        {
            cli_out("Syntax error parsing \"%s\"\n", reg_name);
        } 
        else 
        {
            if (alist.count > 1) 
            {
                /* a list is not supported, only a single address */
                cli_out("Only a single address can be get bits %s.\n", reg_name);
            }
            else
            {
                ainfo = &alist.ainfo[0];
                reg_bits = soc_reg_bits(unit, ainfo->reg);
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return reg_bits;
}

/* Return the number of bits of a given register field */
int diag_reg_field_bits(int unit, char *reg_name, char* field_name)
{
    int field_bits = 0;

    if (!SOC_UNIT_VALID(unit)) 
    {
        cli_out("Invalid unit.\n");
        return field_bits;
    }
    if (reg_name == NULL) 
    {
        cli_out("NULL reg_name given.\n");
        return field_bits;
    }
    if (field_name == NULL) 
    {
        cli_out("NULL field_name given.\n");
        return field_bits;
    }
    if (*reg_name == '$') 
    {
        reg_name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;

        parse_register_name_rv = access_parse_register_name(runtime_info, reg_name, 0, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return field_bits;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", reg_name);
            return field_bits;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                return field_info->size_in_bits;
            }
        }
        cli_out("The specified field %s was not found in the register %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        soc_reg_info_t *reginfo;
        int f;

        if (soc_regaddrlist_alloc(&alist) < 0)
        {
            cli_out("Could not allocate address list.  Memory error.\n");
            return field_bits;
        }
        if (parse_symbolic_reference(unit, &alist, reg_name) < 0) 
        {
            cli_out("Syntax error parsing \"%s\"\n", reg_name);
        } 
        else 
        {
            if (alist.count >1) 
            {
                /* a list is not supported, only a single address */
                cli_out("Only a single address can be get field bits %s.\n", reg_name);
            }
            else 
            {
                ainfo = &alist.ainfo[0];
                reginfo = &SOC_REG_INFO(unit, ainfo->reg);

                /* search field by name*/
                for (f = reginfo->nFields - 1; f >= 0; f--) 
                {
                    soc_field_info_t *fld = &reginfo->fields[f];
                    if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field), field_name)) 
                    {
                        continue;
                    }
                    field_bits = soc_reg_field_length(unit, ainfo->reg, fld->field);
                    break;
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }   
    return field_bits;
}

/* return the register ID, block instance ID,
 * block instance number and array index
 * of the register given as a string input.
 *
 * @param [in] unit - the relevant unit
 * @param [in] name - the register name
 * @param [out] reg_id - the returned register ID
 * @param [out] block_instance_id - the block instance
 * @param [out] block_instance_number - the block instance number
 * @param [out] array_index - the array index
 *
 * Example: "CGM_INTERRUPT_REGISTER.CGM1"
 */

int diag_reg_info_get(int unit, char *name, int *reg_id, int *block_instance_id, unsigned int  *block_instance_number, unsigned int *array_index)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit))
    {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL)
    {
        cli_out("NULL name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$')
    {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;
        uint16 arr_ind = -1;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            if (device_info->blocks[rm_info->local_block_id].nof_instances > 1)
            {
                cli_out("A single block instance of the register %s must be specified.\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM; 
            }
            *block_instance_number = 0;
        }
        else
        {
            *block_instance_number = inst_i;
        }
        if (arr_ind == (uint16)(-1))
        {
            if (rm_info->nof_array_indices == 1)
            {
                *array_index = 0;
            }
            else
            {
                cli_out("ERROR: must specify an array index for register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }
        else
        {
            *array_index = arr_ind;
        }

        *reg_id = regmem_i;
        *block_instance_id = device_info->blocks[rm_info->local_block_id].instances_start_index + *block_instance_number;
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            if (alist.count > 1)
            {
                /* a list is not supported, only a single address */
                cli_out("A single block instance of the register must be specified  %s.\n", name);
                rv = SOC_E_PARAM;
            }
            else
            {
                ainfo = &alist.ainfo[0];
                *reg_id = ainfo->reg;
                *array_index = ainfo->idx;
                *block_instance_id = ainfo->block;
                *block_instance_number = ainfo->port;
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Get register value */
int diag_reg_get(int unit, char *name, reg_val value) 
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit))
    {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$')
    {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;
        uint16 arr_ind = -1;
        bcm_gport_t port;
        bcm_port_config_t port_config;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;
       
        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (rm_info->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        if (((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT) && inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            rv = bcm_port_config_get(unit, &port_config);
            cli_out("WARNING: a port of %s was not specified, the first found port will be used\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        if ((rv = access_local_regmem(runtime_info, 0, regmem_i, port, arr_ind, 0, value)))
                        {
                            cli_out("ERROR: Failed to read from register ");
                            diag_print_regmem_info(runtime_info, rm_info, port, arr_ind, 0);
                            return rv;
                        }
                        return SOC_E_NONE;
                    }
                }
            }
            cli_out("ERROR: Did not find a port to read from\n");
            return SOC_E_PARAM;
        }
        else if ((rv = access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, 0, value)))
        {
            cli_out("ERROR: Failed to read from register ");
            diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
            return rv;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            if (alist.count > 1) {
                /* a list is not supported, only a single address */
                cli_out("Only a single address can be read %s.\n", name);
                rv = SOC_E_PARAM;
            } else {
                ainfo = &alist.ainfo[0];
                if (soc_cpureg == SOC_REG_INFO(unit, ainfo->reg).regtype) {
                    SOC_REG_ABOVE_64_CLEAR(value);
                    value[0] = soc_pci_read(unit, SOC_REG_INFO(unit, ainfo->reg).offset);
                    rv = BCM_E_NONE;
    #ifdef BCM_IPROC_SUPPORT
                } else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    SOC_REG_ABOVE_64_CLEAR(value);
                    rv = soc_iproc_getreg(unit, SOC_REG_INFO(unit, ainfo->reg).offset, &(value[0]));
    #endif
                } else if (soc_customreg == SOC_REG_INFO(unit, ainfo->reg).regtype) {
                    rv = soc_custom_reg_above_64_get(unit, ainfo->reg, ainfo->port, ainfo->idx, value); 
                    if (rv) {
                        char buf[80]; 
                        soc_reg_sprint_addr(unit, buf, ainfo);
                        cli_out("ERROR: read from register %s failed: %s\n", buf, soc_errmsg(rv));
                    }
                } else if ((rv = soc_reg_above_64_get(unit, ainfo->reg, ainfo->port, ainfo->idx, value)) < 0) {
                    char buf[80];
                    soc_reg_sprint_addr(unit, buf, ainfo);
                    cli_out("ERROR: read from register %s failed: %s\n",
                        buf, soc_errmsg(rv));
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Get a register field value by reading the register, and extracting the field from the read register. */
int diag_reg_field_get(int unit, char *name, char* field_name, reg_val value)
{
    int rv = SOC_E_NONE;
    reg_val value_all;

    if (!SOC_UNIT_VALID(unit))
    {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (field_name == NULL) 
    {
        cli_out("NULL field_name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$')
    {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;
        bcm_gport_t port;
        bcm_port_config_t port_config;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;
       
        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (rm_info->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the register %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if (((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT) && inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            rv = bcm_port_config_get(unit, &port_config);
            cli_out("WARNING: a port of %s was not specified, the first found port will be used\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        if ((rv = access_local_regmem(runtime_info, 0, regmem_i, port, arr_ind, 0, value_all)))
                        {
                            cli_out("ERROR: Failed to read from register ");
                            diag_print_regmem_info(runtime_info, rm_info, port, arr_ind, 0);
                            return rv;
                        }
                        else if ((rv = access_local_field_get(runtime_info, fld, value_all, value)))
                        {
                            cli_out("ERROR: Failed to get field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                            return rv;
                        }
                        return SOC_E_NONE;
                    }
                }
            }
            cli_out("ERROR: Did not find a port to read from\n");
            return SOC_E_PARAM;
        }
        else
        {
            if ((rv = access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, 0, value_all)))
            {
                cli_out("ERROR: Failed to read from register ");
                diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                return rv;
            }
            else if ((rv = access_local_field_get(runtime_info, fld, value_all, value)))
            {
                cli_out("ERROR: Failed to get field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                return rv;
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        soc_reg_info_t *reginfo;
        int field_found = 0, f;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            if (alist.count >1) {
                /* a list is not supported, only a single address */
                cli_out("Only a single address can be read %s.\n", name);
                rv = SOC_E_PARAM;
            }
            else {
                ainfo = &alist.ainfo[0];
                reginfo = &SOC_REG_INFO(unit, ainfo->reg);
                rv = BCM_E_NONE;
                if (soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    SOC_REG_ABOVE_64_CLEAR(value);
                    value_all[0] = soc_pci_read(unit, SOC_REG_INFO(unit,ainfo->reg).offset);
    #ifdef BCM_IPROC_SUPPORT
                } else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    SOC_REG_ABOVE_64_CLEAR(value);
                    rv = soc_iproc_getreg(unit, SOC_REG_INFO(unit, ainfo->reg).offset, value_all);
    #endif
                } else {
                    rv = soc_reg_above_64_get(unit, ainfo->reg, ainfo->port, ainfo->idx, value_all);
                }
                if (rv < 0) {
                    char buf[80];
                    soc_reg_sprint_addr(unit, buf, ainfo);
                    cli_out("ERROR: read from register %s failed: %s\n",
                            buf, soc_errmsg(rv));
                }

                /* search field by name*/
                for (f = reginfo->nFields - 1; f >= 0; f--) {
                    soc_field_info_t *fld = &reginfo->fields[f];
                    if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field),field_name)) {
                        continue;
                    }
                    field_found = 1;
                    soc_reg_above_64_field_get(unit, ainfo->reg, value_all, fld->field, value);
                    break;
                }
                if (field_found == 0) {
                    rv = SOC_E_NOT_FOUND;
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Get a register field from a register value in memory. The register is not read. */
int diag_reg_field_only_get(int unit, char *name, reg_val reg_value, char* field_name, reg_val field_value)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit))
    {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (field_name == NULL) 
    {
        cli_out("NULL field_name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$')
    {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the register %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if ((rv = access_local_field_get(runtime_info, fld, reg_value, field_value)))
        {
            cli_out("ERROR: Failed to get field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
            return rv;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        soc_reg_info_t *reginfo;
        int  field_found = 0, f;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            if (alist.count >1) {
                /* a list is not supported, only a single address */
                cli_out("Only a single address can be read %s.\n", name);
                rv = SOC_E_PARAM;
            }
            else {
                ainfo = &alist.ainfo[0];
                reginfo = &SOC_REG_INFO(unit, ainfo->reg);
                rv = BCM_E_NONE;
                /* search field by name*/
                for (f = reginfo->nFields - 1; f >= 0; f--) {
                    soc_field_info_t *fld = &reginfo->fields[f];
                    if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field),field_name)) {
                        continue;
                    }
                    field_found = 1;
                    soc_reg_above_64_field_get(unit, ainfo->reg, reg_value, fld->field, field_value);
                    break;
                }
                if (field_found == 0) {
                    rv = SOC_E_NOT_FOUND;
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Set register value */
int diag_reg_set(int unit, char *name, reg_val value)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$') {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv, first_array_ind, last_array_ind;
        uint16 arr_ind = -1;
        bcm_gport_t port;
        bcm_port_config_t port_config;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        if (arr_ind == (uint16)(-1) && rm_info->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        if (((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT) && inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            rv = bcm_port_config_get(unit, &port_config);
            cli_out("WARNING: a port of %s was not specified, the first found port will be used\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        /* Loop over array indices to write the register to each one */
                        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
                        {
                            if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, port, arr_ind, 0, value)))
                            {
                                cli_out("ERROR: Failed to write register ");
                                diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                                return rv;
                            }
                        }
                        return SOC_E_NONE;
                    }
                }
            }
            cli_out("ERROR: Did not find a port to read from\n");
            return SOC_E_PARAM;
        }
        else 
        {
            /* Loop over array indices to write the register to each one */
            for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
            {
                if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, 0, value)))
                {
                    cli_out("ERROR: Failed to write register ");
                    diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        int i;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            for(i=0 ; i<alist.count && SOC_E_NONE == rv ; i++) {
                ainfo = &alist.ainfo[0];
                if (soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    soc_pci_write(unit, SOC_REG_INFO(unit,ainfo->reg).offset, value[0]);
                    rv = BCM_E_NONE;
#ifdef BCM_IPROC_SUPPORT
                } else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    rv = soc_iproc_setreg(unit, SOC_REG_INFO(unit,ainfo->reg).offset, value[0]);
 #endif
                } else if (( rv = soc_reg_above_64_set(unit, ainfo->reg, ainfo->port, ainfo->idx, value)) < 0) {
                    char buf[80];
                    soc_reg_sprint_addr(unit, buf, ainfo);
                    cli_out("ERROR: write to register %s failed: %s\n",
                            buf, soc_errmsg(rv));
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Set a register field by reading the register, changing the field value in memory, and writing back the register. */
int diag_reg_field_set(int unit, char *name, char* field_name, reg_val value)
{
    int rv = SOC_E_NONE;
    reg_val value_all;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (field_name == NULL) 
    {
        cli_out("NULL field_name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$') {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        const access_device_block_t *local_block = NULL;
        int parse_register_name_rv, first_array_ind, last_array_ind, first_inst_ind, last_inst_ind;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;
        const uint8 *block_instances_flags;
        bcm_gport_t port;
        bcm_port_config_t port_config;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        if (arr_ind == (uint16)(-1) && rm_info->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

        local_block = device_info->blocks + rm_info->local_block_id;

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the register %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if (((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT) && inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            rv = bcm_port_config_get(unit, &port_config);
            cli_out("WARNING: a port of %s was not specified, the first found port will be used\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
                        {
                            if ((rv = access_local_regmem(runtime_info, 0, regmem_i, port, arr_ind, 0, value_all)))
                            {
                                cli_out("ERROR: Failed to read from register ");
                                diag_print_regmem_info(runtime_info, rm_info, port, arr_ind, 0);
                                return rv;
                            }
                            else if ((rv = access_local_field_set(runtime_info, fld, value_all, value)))
                            {
                                cli_out("ERROR: Failed to set field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                                return rv;
                            }
                            else if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, port, arr_ind, 0, value_all)))
                            {
                                cli_out("ERROR: Failed to write to register ");
                                diag_print_regmem_info(runtime_info, rm_info, port, arr_ind, 0);
                                return rv;
                            }
                        }
                        return SOC_E_NONE;
                    }
                }
            }
            cli_out("ERROR: Did not find a port to read from\n");
            return SOC_E_PARAM;
        }
        else
        {
            if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
            {
                if (local_block->nof_instances > 1)
                {
                    cli_out("WARNING: writing to all instances of register %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                }
                first_inst_ind = 0;
                last_inst_ind = local_block->nof_instances - 1;
            }
            else
            {
                first_inst_ind = last_inst_ind = inst_i;
            }
            /* Loop over instances and array indices to write the register to each one */
            block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
            for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i++)
            {
                /* Check if a block instance is enabled */
                if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
                {
                    continue;
                }
                for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
                {
                    if ((rv = access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, 0, value_all)))
                    {
                        cli_out("ERROR: Failed to read from register ");
                        diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                        return rv;
                    }
                    else if ((rv = access_local_field_set(runtime_info, fld, value_all, value)))
                    {
                        cli_out("ERROR: Failed to set field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                        return rv;
                    }
                    else if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, 0, value_all)))
                    {
                        cli_out("ERROR: Failed to write to register ");
                        diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                        return rv;
                    }
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        soc_reg_info_t *reginfo;
        int i, f, field_found = 0;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            for(i=0 ; i<alist.count && SOC_E_NONE == rv ; i++) {
                ainfo = &alist.ainfo[0];
                reginfo = &SOC_REG_INFO(unit, ainfo->reg);
                rv = BCM_E_NONE;
                if (soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                    SOC_REG_ABOVE_64_CLEAR(value_all);
                    value_all[0] = soc_pci_read(unit, SOC_REG_INFO(unit,ainfo->reg).offset);
#ifdef BCM_IPROC_SUPPORT
                } else if (soc_iprocreg == SOC_REG_INFO(unit, ainfo->reg).regtype) {
                    rv = soc_iproc_getreg(unit, SOC_REG_INFO(unit, ainfo->reg).offset, value_all);

#endif
                } else {
                    rv = soc_reg_above_64_get(unit, ainfo->reg, ainfo->port, ainfo->idx, value_all);
                }
                if (rv < 0) {
                    char buf[80];
                    soc_reg_sprint_addr(unit, buf, ainfo);
                    cli_out("ERROR: read from register %s failed: %s\n",
                            buf, soc_errmsg(rv));
                }

                /*search field by name*/
                for (f = reginfo->nFields - 1; f >= 0; f--) {
                    soc_field_info_t *fld = &reginfo->fields[f];
                    if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field),field_name)) {
                        continue;
                    }
                    field_found = 1;
                    soc_reg_above_64_field_set(unit, ainfo->reg, value_all, fld->field, value);
                    break;
                }
                if (field_found == 0) {
                    rv = SOC_E_NOT_FOUND;
                } else {
                    rv = BCM_E_NONE;
                    if (soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                        soc_pci_write(unit, SOC_REG_INFO(unit,ainfo->reg).offset, value_all[0]);
#ifdef BCM_IPROC_SUPPORT
                    } else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                        rv = soc_iproc_setreg(unit, SOC_REG_INFO(unit,ainfo->reg).offset, value_all[0]);
#endif
                    } else {
                        rv = soc_reg_above_64_set(unit, ainfo->reg, ainfo->port, ainfo->idx, value_all);
                    }
                    if (rv < 0) {
                        char buf[80];
                        soc_reg_sprint_addr(unit, buf, ainfo);
                        cli_out("ERROR: write to register %s failed: %s\n",
                                buf, soc_errmsg(rv));
                    }
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* Set a field in the given register value, and then write it to the register. No read from the register performed */
int diag_reg_field_only_set(int unit, char *name, reg_val reg_value, char* field_name, reg_val field_value)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL reg name given.\n");
        return SOC_E_PARAM;
    }
    if (field_name == NULL) 
    {
        cli_out("NULL field_name given.\n");
        return SOC_E_PARAM;
    }
    if (*name == '$') {
        name++;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_register_name_rv, first_array_ind, last_array_ind;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;
        bcm_gport_t port;
        bcm_port_config_t port_config;

        parse_register_name_rv = access_parse_register_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_register_name returns -1, there is an error/failure from the user input */
        if (parse_register_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_register_name returns 1, there is no exact match of register name */
        if (parse_register_name_rv == 1)
        {
            cli_out("Unknown register %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Register to access */
        rm_info = device_info->local_regs + regmem_i;

        if (arr_ind == (uint16)(-1) && rm_info->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of register array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the register %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if ((rv = access_local_field_set(runtime_info, fld, reg_value, field_value)))
        {
            cli_out("ERROR: Failed to set field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
            return rv;
        }

        if (((rm_info->flags & ACCESS_REGMEM_FLAG_ACCESS_TYPE_MASK) == ACCESS_REGMEM_FLAG_ACCESS_TYPE_PER_PORT) && inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            rv = bcm_port_config_get(unit, &port_config);
            cli_out("WARNING: a port of %s was not specified, the first found port will be used\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            BCM_PBMP_ITER(port_config.nif, port)
            {
                uint32 phy_port = 0;
                int block_inst_id = -1;
                access_local_block_id_t local_block_id;
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                block_inst_id = device_info->port_info[phy_port].blk; /* get the local block instance ID for phy_port */
                if (block_inst_id >= 0 && block_inst_id < device_info->nof_block_instances) { /* if found valid port_info data */
                    local_block_id = device_info->block_instances[block_inst_id].block_id;
                    if (rm_info->local_block_id == local_block_id || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CDMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CDPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_CLMAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_CLPORT]) || (rm_info->local_block_id == device_info->blocks_global2local_map[BLOCK_DC3MAC] && local_block_id == device_info->blocks_global2local_map[BLOCK_DC3PORT])) /* may need another blocktypes for future devices */
                    {
                        /* Loop over array indices to write the register to each one */
                        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
                        {
                            if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, port, arr_ind, 0, reg_value)))
                            {
                                cli_out("ERROR: Failed to write to register ");
                                diag_print_regmem_info(runtime_info, rm_info, port, arr_ind, 0);
                                return rv;
                            }
                        }
                        return SOC_E_NONE;
                    }
                }
            }
            cli_out("ERROR: Did not find a port to read from\n");
            return SOC_E_PARAM;
        }
        else
        {
            /* Loop over array indices to write the register to each one */
            for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
            {
                if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, 0, reg_value)))
                {
                    cli_out("ERROR: Failed to write to register ");
                    diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, 0);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_regaddrlist_t alist;
        soc_regaddrinfo_t *ainfo;
        soc_reg_info_t *reginfo;
        int i, f, field_found = 0;

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return SOC_E_PARAM;
        }
        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = SOC_E_PARAM;
        } else {
            for(i=0 ; i<alist.count && SOC_E_NONE == rv ; i++) {
                ainfo = &alist.ainfo[0];
                reginfo = &SOC_REG_INFO(unit, ainfo->reg);
                rv = BCM_E_NONE;
                    /*search field by name*/
                for (f = reginfo->nFields - 1; f >= 0; f--) {
                    soc_field_info_t *fld = &reginfo->fields[f];
                    if (sal_strcasecmp(SOC_FIELD_NAME(unit, fld->field),field_name)) {
                        continue;
                    }
                    field_found = 1;
                    soc_reg_above_64_field_set(unit, ainfo->reg, reg_value, fld->field, field_value);
                    break;
                }
                if (field_found == 0) {
                    rv = SOC_E_NOT_FOUND;
                } else {
                    rv = BCM_E_NONE;
                    if (soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                        soc_pci_write(unit, SOC_REG_INFO(unit,ainfo->reg).offset, reg_value[0]);
    #ifdef BCM_IPROC_SUPPORT
                    } else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
                        rv = soc_iproc_setreg(unit, SOC_REG_INFO(unit,ainfo->reg).offset, reg_value[0]);
    #endif
                    } else {
                        rv = soc_reg_above_64_set(unit, ainfo->reg, ainfo->port, ainfo->idx, reg_value);
                    }
                    if (rv < 0) {
                        char buf[80];
                        soc_reg_sprint_addr(unit, buf, ainfo);
                        cli_out("ERROR: write to register %s failed: %s\n",
                                buf, soc_errmsg(rv));
                    }
                }
            }
        }
        soc_regaddrlist_free(&alist);
    }
    return rv;
}

/* return the memory ID, block instance ID,
 * block instance number and array index
 * of the memory given as a string input.
 *
 * @param [in] unit - the relevant unit
 * @param [in] name - the memory name
 * @param [out] mem_id - the returned memory ID
 * @param [out] block_instance_id - the block instance
 * @param [out] block_instance_number - the block instance number
 * @param [out] array_index - the array index
 *
 * Example: "CGM_INSTRUMENTATION_MEMORY[0]"
 */
int diag_mem_info_get(int unit, char *name, int *mem_id, int *block_instance_id, unsigned int *block_instance_number, unsigned int *array_index)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            rm_info = device_info->local_regs + regmem_i;
        }

        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            if (device_info->blocks[rm_info->local_block_id].nof_instances > 1)
            {
                cli_out("A single block instance of the memory %s must be specified.\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
            *block_instance_number = 0;
        }
        else
        {
            *block_instance_number = inst_i;
        }
        if (arr_ind == (uint16)(-1))
        {
            if (rm_info->nof_array_indices == 1)
            {
                *array_index = 0;
            }
            else
            {
                cli_out("ERROR: must specify an array index for memory array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }
        else
        {
            *array_index = arr_ind;
        }

        *mem_id = regmem_i;
        *block_instance_id = device_info->blocks[rm_info->local_block_id].instances_start_index + *block_instance_number;
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        if (parse_memory_name(unit, mem_id, name, block_instance_id, array_index) < 0) {
            cli_out("ERROR: unknown memory \"%s\"\n",name);
            return SOC_E_PARAM;
        }
        if ((*block_instance_id < SOC_MEM_BLOCK_MIN(unit,*mem_id)) || (*block_instance_id > SOC_MEM_BLOCK_MAX(unit,*mem_id)))
        {
            cli_out("Invalid block instance ID %d. Min ID: %d, Max ID: %d.\n",
                    *block_instance_id, SOC_MEM_BLOCK_MIN(unit,*mem_id), SOC_MEM_BLOCK_MAX(unit,*mem_id));
        }

        *block_instance_number = *block_instance_id - SOC_MEM_BLOCK_MIN(unit,*mem_id);
    }
    return rv;
}

/* Get memory value */
int diag_mem_get(int unit, char *name, int index, mem_val value)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            rm_info = device_info->local_regs + regmem_i;
        }

        /* Check index range */
        if (index < 0 || index >= rm_info->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d do not specify a legal sub range of indices 0..%u\n", index, rm_info->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (rm_info->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for memory array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        if (access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, index, value))
        {
            cli_out("ERROR: Failed to read from memory ");
            diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, index);
            return SOC_E_PARAM;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_mem_t mem;
        unsigned array_index;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        /* Created entry, now read it */
        if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, value)) < 0) {
            cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index,
                    soc_errmsg(rv));
        }
    }
    return rv;
}


/* Get a memory field value by reading the memory, and extracting the field from the read memory. */
int diag_mem_field_get(int unit, char *name, char* field_name, int index, mem_val fval)
{
    int rv = SOC_E_NONE;
    mem_val value;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        int parse_memory_name_rv;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            mem_aliased_to = device_info->local_regs + regmem_i;
        }
        else
        {
            mem_aliased_to = rm_info;
        }

        /* Check index range */
        if (index < 0 || index >= mem_aliased_to->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d do not specify a legal sub range of indices 0..%u\n", index, mem_aliased_to->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (mem_aliased_to->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for memory array %s:\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the memory %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if ((rv = access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, index, value)))
        {
            cli_out("ERROR: Failed to read from memory ");
            diag_print_regmem_info(runtime_info, mem_aliased_to, inst_i, arr_ind, index);
            return rv;
        }
        else if ((rv = access_local_field_get(runtime_info, fld, value, fval)))
        {
            cli_out("ERROR: Failed to get field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
            return rv;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_field_info_t *fieldp;
        soc_mem_t mem;
        soc_mem_info_t *memp;
        int f;
        unsigned array_index;
        char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        memp = &SOC_MEM_INFO(unit, mem);

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        sal_memset(tmp, 0, sizeof(char) * ((SOC_MAX_MEM_FIELD_WORDS * 8) + 3) );

        if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, value)) < 0) {
            cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index,
                    soc_errmsg(rv));
        }
        for (f = memp->nFields - 1; f >= 0; f--) {
            fieldp = &memp->fields[f];
            if (!(sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name))) {
                soc_mem_field_get(unit, mem, value, fieldp->field, fval);
                _shr_format_long_integer(tmp, fval, SOC_MAX_MEM_FIELD_WORDS);
#if !defined(SOC_NO_NAMES)
                LOG_VERBOSE(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit,
                                        "%s="),
                            soc_fieldnames[fieldp->field]));
#endif
                LOG_VERBOSE(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit,
                                        "%s\n"),
                            tmp));
                break;
            }
        }
    }
    return rv;
}

/* Get a memory field from a memory value without first reading the memory */
int diag_mem_field_only_get(int unit, char *name, mem_val value, char* field_name, int index, mem_val fval)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        int parse_memory_name_rv;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            mem_aliased_to = device_info->local_regs + regmem_i;
        }
        else
        {
            mem_aliased_to = rm_info;
        }

        /* Check index range */
        if (index < 0 || index >= mem_aliased_to->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d do not specify a legal sub range of indices 0..%u\n", index, mem_aliased_to->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (mem_aliased_to->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for memory array %s:\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the memory %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }
        else if ((rv = access_local_field_get(runtime_info, fld, value, fval)))
        {
            cli_out("ERROR: Failed to get field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
            return rv;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_field_info_t *fieldp;
        soc_mem_t mem;
        soc_mem_info_t *memp;
        int f;
        unsigned array_index;
        char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        memp = &SOC_MEM_INFO(unit, mem);

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        sal_memset(fval, 0, (sizeof(uint32)) * SOC_MAX_MEM_FIELD_WORDS);
        sal_memset(tmp, 0, sizeof(char) * ((SOC_MAX_MEM_FIELD_WORDS * 8) + 3) );

        for (f = memp->nFields - 1; f >= 0; f--) {
            fieldp = &memp->fields[f];
            if (!(sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name))) {
                soc_mem_field_get(unit, mem, value, fieldp->field, fval);
                _shr_format_long_integer(tmp, fval, SOC_MAX_MEM_FIELD_WORDS);
    #if !defined(SOC_NO_NAMES)
                LOG_VERBOSE(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit,
                                        "%s="),
                            soc_fieldnames[fieldp->field]));
    #endif
                LOG_VERBOSE(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit,
                                        "%s\n"),
                            tmp));
                break;
            }
        }
    }
    return rv;
}

/* Read the memory in given renge of indices using sbus DMA if possible */
int diag_soc_mem_read_range(int unit, char *name, int index1, int index2, void *buffer)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv, index = 0;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            rm_info = device_info->local_regs + regmem_i;
        }

        /* Check index range */
        if (index1 < 0 || index2 < index1 || index2 >= rm_info->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index1 %d or index2 %d do not specify a legal sub range of indices 0..%u\n", index1, index2, rm_info->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1))
        {
            arr_ind = 0;
            if (rm_info->nof_array_indices > 1)
            {
                cli_out("ERROR: must specify an array index for memory array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
                return SOC_E_PARAM;
            }
        }

        if ((rv = access_local_regmem_sbusdma(runtime_info, 0, regmem_i, inst_i, arr_ind, index1, index2 - index1 + 1, 0, buffer)))
        {
            cli_out("ERROR: Failed to read memory ");
            diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, index);
            return rv;
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_mem_t mem;
        unsigned array_index;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        /* Created entry, now read it */
        if ((rv = soc_mem_array_read_range(unit, mem, array_index, copyno, index1, index2, buffer)) < 0) {
            cli_out("Read ERROR: table %s.%d[%d-%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index1, index2,
                    soc_errmsg(rv));
        }
    }
    return rv;
}

/* Set a memory */
int diag_mem_set(int unit, char *name, int start, int count, mem_val value)
{
    int index = 0;
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv, first_array_ind, last_array_ind;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            rm_info = device_info->local_regs + regmem_i;
        }

        if (rm_info->flags & ACCESS_REGMEM_FLAG_RO)
        {
            cli_out("ERROR: Table %s is read-only\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_PARAM;
        }

        /* Check index range */
        if (start < 0 || count <= 0 || start + count >= rm_info->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d or count %d do not specify a legal sub range of indices 0..%u\n", start, count, rm_info->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1) && rm_info->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of memory array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        /* Loop over array indices and memory indices to write the memory to each one */
        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
        {
            for (index = start; index < start + count; index++)
            {
                if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, index, value)))
                {
                    cli_out("ERROR: Failed to write memory ");
                    diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, index);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_mem_t mem;
        unsigned array_index;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        if (soc_mem_is_readonly(unit, mem)) {
            cli_out("ERROR: Table %s is read-only\n",
                    SOC_MEM_UFNAME(unit, mem));
            return SOC_E_PARAM;
        }

        /* Created entry, now write it */
        for (index = start; index < start + count && SOC_E_NONE == rv; index++)
        {
            if ((rv = soc_mem_array_write(unit, mem, array_index, copyno, index, value)) < 0) {
                cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                        0 : copyno, index,
                        soc_errmsg(rv));
            }
        }
    }
    return rv;
}

/* Set a memory field by reading the memory, changing the field value in memory, and writing back the memory. */
int diag_mem_field_set(int unit, char *name, char* field_name, int start, int count, mem_val fval)
{
    int index = 0;
    mem_val value;
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        const access_device_block_t *local_block = NULL;
        int parse_memory_name_rv, first_array_ind, last_array_ind, first_inst_ind, last_inst_ind;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;
        const uint8 *block_instances_flags;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            mem_aliased_to = device_info->local_regs + regmem_i;
        }
        else
        {
            mem_aliased_to = rm_info;
        }

        if (mem_aliased_to->flags & ACCESS_REGMEM_FLAG_RO)
        {
            cli_out("ERROR: Table %s is read-only\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
            return SOC_E_PARAM;
        }

        /* Check index range */
        if (start < 0 || count <= 0 || start + count >= mem_aliased_to->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d or count %d do not specify a legal sub range of indices 0..%u\n", start, count, mem_aliased_to->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1) && mem_aliased_to->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of memory array %s:\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
        }

        local_block = device_info->blocks + mem_aliased_to->local_block_id;

        if (inst_i == ACCESS_ALL_BLOCK_INSTANCES)
        {
            if (local_block->nof_instances > 1)
            {
                cli_out("WARNING: writing to all instances of memory %s:\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
            }
            first_inst_ind = 0;
            last_inst_ind = local_block->nof_instances - 1;
        }
        else
        {
            first_inst_ind = last_inst_ind = inst_i;
        }
        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = mem_aliased_to->first_array_index;
            last_array_ind = mem_aliased_to->nof_array_indices + mem_aliased_to->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the memory %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        /* Loop over instances, array indices and memory indices to write the memory to each one */
        block_instances_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
        for (inst_i = first_inst_ind; inst_i <= last_inst_ind; inst_i++)
        {
            /* Check if a block instance is enabled */
            if ((block_instances_flags[inst_i] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED))
            {
                continue;
            }
            for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
            {
                for (index = start; index < start + count; index++)
                {
                    if ((rv = access_local_regmem(runtime_info, 0, regmem_i, inst_i, arr_ind, index, value)))
                    {
                        cli_out("ERROR: Failed to read from memory ");
                        diag_print_regmem_info(runtime_info, mem_aliased_to, inst_i, arr_ind, index);
                        return rv;
                    }
                    else if ((rv = access_local_field_set(runtime_info, fld, value, fval)))
                    {
                        cli_out("ERROR: Failed to set field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
                        return rv;
                    }
                    else if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, index, value)))
                    {
                        cli_out("ERROR: Failed to write to memory ");
                        diag_print_regmem_info(runtime_info, mem_aliased_to, inst_i, arr_ind, index);
                        return rv;
                    }
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_mem_t mem;
        soc_field_info_t *fieldp;
        soc_mem_info_t *memp;
        int f;
        unsigned array_index;
        int copyno;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        if (soc_mem_is_readonly(unit, mem)) {
            cli_out("ERROR: Table %s is read-only\n",
                    SOC_MEM_UFNAME(unit, mem));
            return SOC_E_PARAM;
        }
        memp = &SOC_MEM_INFO(unit, mem);
        /* Created entry, now write it */
        for (index = start; index < start + count && SOC_E_NONE == rv; index++)
        {
            sal_memset(value, 0, sizeof (mem_val));

            if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, value)) < 0) {
                cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                        0 : copyno, index,
                        soc_errmsg(rv));
            }
            for (f = memp->nFields - 1; f >= 0; f--) {
                fieldp = &memp->fields[f];
                if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name)) {
                    continue;
                }
                soc_mem_field_set(unit, mem, value, fieldp->field, fval);
            }

            if ((rv = soc_mem_array_write(unit, mem, array_index, copyno, index, value)) < 0) {
                cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                        0 : copyno, index,
                        soc_errmsg(rv));
            }
        }
    }
    return rv;
}

/* Set a memory field in the given memory value, and then write it to the memory. No read from the memory performed */
int diag_mem_field_only_set(int unit, char *name, mem_val value, char* field_name, int start, int count, mem_val fval)
{
    int index = 0;
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL, *mem_aliased_to;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv, first_array_ind, last_array_ind;
        const access_device_field_t *field_info = NULL;
        access_local_field_id_t fld;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            mem_aliased_to = device_info->local_regs + regmem_i;
        }
        else
        {
            mem_aliased_to = rm_info;
        }

        if (mem_aliased_to->flags & ACCESS_REGMEM_FLAG_RO)
        {
            cli_out("ERROR: Table %s is read-only\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
            return SOC_E_PARAM;
        }

        /* Check index range */
        if (start < 0 || count <= 0 || start + count >= mem_aliased_to->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index %d or count %d do not specify a legal sub range of indices 0..%u\n", start, count, mem_aliased_to->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1) && mem_aliased_to->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of memory array %s:\n", access_global_names[global_regmem_name_indices[mem_aliased_to->global_regmem_id]]);
        }

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = mem_aliased_to->first_array_index;
            last_array_ind = mem_aliased_to->nof_array_indices + mem_aliased_to->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        for (fld = rm_info->fields_index; fld < rm_info->fields_index + rm_info->nof_fields; fld++)
        {
            /* Field to access */
            field_info = device_info->local_fields + fld;
            /* Search field by name*/
            if (!sal_strcasecmp(field_name, access_global_names[global_field_name_indices[field_info->global_field_id]]))
            {
                break;
            }
        }
        if (fld >= rm_info->fields_index + rm_info->nof_fields)
        {
            cli_out("The specified field %s was not found in the memory %s.\n", field_name, access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_NOT_FOUND;
        }

        if ((rv = access_local_field_set(runtime_info, fld, value, fval)))
        {
            cli_out("ERROR: Failed to set field %s\n", access_global_names[global_field_name_indices[field_info->global_field_id]]);
            return rv;
        }

        /* Loop over array indices and memory indices to write the memory to each one */
        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
        {
            for (index = start; index < start + count; index++)
            {
                if ((rv = access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, index, value)))
                {
                    cli_out("ERROR: Failed to write to memory ");
                    diag_print_regmem_info(runtime_info, mem_aliased_to, inst_i, arr_ind, index);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        soc_mem_t mem;
        soc_field_info_t *fieldp;
        soc_mem_info_t *memp;
        int f, copyno;
        unsigned array_index;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        if (soc_mem_is_readonly(unit, mem)) {
            cli_out("ERROR: Table %s is read-only\n",
                    SOC_MEM_UFNAME(unit, mem));
            return SOC_E_PARAM;
        }
        memp = &SOC_MEM_INFO(unit, mem);
        /* Created entry, now write it */
        for (index = start; index < start + count && SOC_E_NONE == rv; index++)
        {
            for (f = memp->nFields - 1; f >= 0; f--) {
                fieldp = &memp->fields[f];
                if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name)) {
                    continue;
                }
                soc_mem_field_set(unit, mem, value, fieldp->field, fval);
            }

            if ((rv = soc_mem_array_write(unit, mem, array_index, copyno, index, value)) < 0) {
                cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                        0 : copyno, index,
                        soc_errmsg(rv));
            }
        }
    }
    return rv;
}

/* Write the memory in given renge of indices using sbus DMA if possible */
int diag_soc_mem_write_range(int unit, char *name, int index1, int index2, void *buffer)
{
    int rv = SOC_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }
    if (name == NULL) {
        cli_out("NULL mem name given.\n");
        return SOC_E_PARAM;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit); /* Access information for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;  /* Constant access information for the device type */
        const access_device_regmem_t *rm_info = NULL;
        access_block_instance_num_t inst_i = ACCESS_ALL_BLOCK_INSTANCES;
        access_local_regmem_id_t regmem_i;
        int parse_memory_name_rv, first_array_ind, last_array_ind, index = 0;
        uint16 arr_ind = -1;

        parse_memory_name_rv = access_parse_memory_name(runtime_info, name, &arr_ind, &inst_i, &regmem_i);
        /* If access_parse_memory_name returns -1, there is an error/failure from the user input */
        if (parse_memory_name_rv < 0)
        {
            return SOC_E_PARAM;
        }
        /* If access_parse_memory_name returns 1, there is no exact match of memory name */
        if (parse_memory_name_rv == 1)
        {
            cli_out("Unknown memory %s:\n", name);
            return SOC_E_PARAM;
        }
        /* Memory to access */
        rm_info = device_info->local_regs + regmem_i;

        if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS) != 0)
        {
            regmem_i = rm_info->u.mem.alias_regmem;
            rm_info = device_info->local_regs + regmem_i;
        }

        if (rm_info->flags & ACCESS_REGMEM_FLAG_RO)
        {
            cli_out("ERROR: Table %s is read-only\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
            return SOC_E_PARAM;
        }

        /* Check index range */
        if (index1 < 0 || index2 < index1 || index2 >= rm_info->u.mem.mem_nof_elements)
        {
            cli_out("Error: Memory index1 %d or index2 %d do not specify a legal sub range of indices 0..%u\n", index1, index2, rm_info->u.mem.mem_nof_elements);
            return SOC_E_PARAM;
        }

        if (arr_ind == (uint16)(-1) && rm_info->nof_array_indices > 1)
        {
            cli_out("WARNING: writing to all array indices of memory array %s:\n", access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
        }

        if (arr_ind == (uint16)(-1))
        {
            first_array_ind = rm_info->first_array_index;
            last_array_ind = rm_info->nof_array_indices + rm_info->first_array_index - 1;
        }
        else
        {
            first_array_ind = last_array_ind = arr_ind;
        }

        /* Loop over array indices and memory indices to write the memory to each one */
        for (arr_ind = first_array_ind; arr_ind <= last_array_ind; arr_ind++)
        {
            if ((rv = access_local_regmem_sbusdma(runtime_info, FLAG_ACCESS_IS_WRITE, regmem_i, inst_i, arr_ind, index1, index2 - index1 + 1, 0, buffer)))
            {
                cli_out("ERROR: Failed to write memory ");
                diag_print_regmem_info(runtime_info, rm_info, inst_i, arr_ind, index);
                return rv;
            }
        }
    }
#endif /* BCM_ACCESS_SUPPORT */
    else
    {
        int copyno;
        soc_mem_t mem;
        unsigned array_index;

        if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n",name);
            return SOC_E_PARAM;
        }

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            return SOC_E_PARAM;
        }

        if (soc_mem_is_readonly(unit, mem)) {
            cli_out("ERROR: Table %s is read-only\n",
                    SOC_MEM_UFNAME(unit, mem));
            return SOC_E_PARAM;
        }

        /* Created entry, now write it */
        if ((rv = soc_mem_array_write_range(unit, 0, mem, array_index, copyno, index1, index2, buffer)) < 0) {
            cli_out("Write ERROR: table %s.%d[%d-%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index1, index2,
                    soc_errmsg(rv));
        }
    }
    return rv;
}
#else
/* To avoid empty file warning in not supported architectures */
int appl_diag_diag_anchor;
#endif /* #if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT) */
