/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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


int diag_reg_bits(int unit, char* reg_name)
{
    int reg_bits = 0;
    soc_regaddrlist_t    alist;
    soc_regaddrinfo_t*   ainfo;

    if (!SOC_UNIT_VALID(unit)) 
    {
        cli_out("Invalid unit.\n");
        return reg_bits;
    }

    if (reg_name == NULL) 
    {
        return reg_bits;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) 
    {
        cli_out("Could not allocate address list.  Memory error.\n");
        return reg_bits;
    }

    if (*reg_name == '$') 
    {
        reg_name++;
    }
    /* Symbolic name given, print all or some values ... */
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
    return reg_bits;
}


int diag_reg_field_bits(int unit, char *reg_name, char* field_name)
{
    int                 field_bits = 0;
    soc_regaddrlist_t   alist;
    soc_regaddrinfo_t*  ainfo;
    soc_reg_info_t*     reginfo;
    int                 f;

    if (!SOC_UNIT_VALID(unit)) 
    {
        cli_out("Invalid unit.\n");
        return field_bits;
    }
    if (reg_name == NULL) 
    {
        return field_bits;
    }

    if (soc_regaddrlist_alloc(&alist) < 0)
    {
        cli_out("Could not allocate address list.  Memory error.\n");
        return field_bits;
    }

    if (*reg_name == '$') 
    {
        reg_name++;
    }

    /* Symbolic name given, print all or some values ... */
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

            /*search field by name*/
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
    return field_bits;
}

/* return the register ID and block instance ID
 * of the register given as a string input.
 *
 * @param [in] unit - the relevant unit
 * @param [in] name - the register name
 * #param [out] reg_id - the returned register ID
 * @param [out] block_instance_id - the block instance
 *
 * Example: "CGM_INTERRUPT_REGISTER.CGM1"
 */

int diag_reg_info_get(int unit, char *name, soc_reg_t *reg_id, soc_block_t *block_instance_id)
{
    int rv = SOC_E_NONE;
    soc_regaddrlist_t alist;
    soc_regaddrinfo_t    *ainfo;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }
    /* Symbolic name given, print all or some values ... */
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
        }
    }
    soc_regaddrlist_free(&alist);
    return rv;
}

int diag_reg_get(int unit, char *name, reg_val value) 
{
   int rv = SOC_E_NONE;
   soc_regaddrlist_t alist;
   soc_regaddrinfo_t    *ainfo;

   if (!SOC_UNIT_VALID(unit)) {
      cli_out("Invalid unit.\n");
      return SOC_E_UNIT;
   }

   if (name == NULL) {
      return SOC_E_PARAM;
   }

   if (soc_regaddrlist_alloc(&alist) < 0) {
      cli_out("Could not allocate address list.  Memory error.\n");
      return SOC_E_PARAM;
   }

   if (*name == '$') {
      name++;
   }
      /* Symbolic name given, print all or some values ... */
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
                   char            buf[80]; 
                   soc_reg_sprint_addr(unit, buf, ainfo);
                   cli_out("ERROR: read from register %s failed: %s\n", buf, soc_errmsg(rv));
               }
            } else if ((rv = soc_reg_above_64_get(unit, ainfo->reg, ainfo->port, ainfo->idx, value)) < 0) {
               char            buf[80];
               soc_reg_sprint_addr(unit, buf, ainfo);
               cli_out("ERROR: read from register %s failed: %s\n",
                       buf, soc_errmsg(rv));
            }
         }
      }
   soc_regaddrlist_free(&alist);
   return rv;
}

/* Get a register field value by reading the register, and extracting the field from the read register. */
int diag_reg_field_get(int unit, char *name, char* field_name, reg_val value)
{
    int             rv = SOC_E_NONE;
    soc_regaddrlist_t alist;
    soc_regaddrinfo_t    *ainfo;
    soc_reg_info_t *reginfo;
    int             f;
    reg_val value_all;
    int field_found = 0;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, print all or some values ... */
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
                char            buf[80];
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
                soc_reg_above_64_field_get(unit, ainfo->reg, value_all, fld->field, value);
                break;
            }
            if (field_found == 0) {
                rv = SOC_E_NOT_FOUND;
            }


        }
    }
    soc_regaddrlist_free(&alist);
    return rv;
}

/* Get a register field from a register value in memory. The register is not read. */
int diag_reg_field_only_get(int unit, char *name, reg_val reg_value, char* field_name, reg_val field_value)
{
    int             rv = SOC_E_NONE;
    soc_regaddrlist_t alist;
    soc_regaddrinfo_t    *ainfo;
    soc_reg_info_t *reginfo;
    int             f;
    int field_found = 0;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, print all or some values ... */
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
            /*search field by name*/
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
    return rv;
}

int diag_reg_set(int unit, char *name, reg_val value)
{
    int rv = SOC_E_NONE, i;
    soc_regaddrlist_t   alist;
    soc_regaddrinfo_t   *ainfo;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, print all or some values ... */
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
                char            buf[80];
                soc_reg_sprint_addr(unit, buf, ainfo);
                cli_out("ERROR: write to register %s failed: %s\n",
                        buf, soc_errmsg(rv));
            }
        }
    }
    soc_regaddrlist_free(&alist);
    return rv;
}

/* Set a register field by reading the register, changing the field value in memory, and writing back the register. */
int diag_reg_field_set(int unit, char *name, char* field_name,reg_val value)
{
    int rv = SOC_E_NONE, i;
    soc_regaddrlist_t   alist;
    soc_regaddrinfo_t   *ainfo;
    soc_reg_info_t *reginfo;
    int             f;
    reg_val value_all;
    int field_found = 0;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, print all or some values ... */
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
                char            buf[80];
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
                        char            buf[80];
                        soc_reg_sprint_addr(unit, buf, ainfo);
                        cli_out("ERROR: write to register %s failed: %s\n",
                                buf, soc_errmsg(rv));
                    }

                }
            }
    }
    soc_regaddrlist_free(&alist);
    return rv;
}

/* Set a register field in a register value in memory. The register is not read or written */
int diag_reg_field_only_set(int unit, char *name, reg_val reg_value, char* field_name,reg_val field_value)
{
    int rv = SOC_E_NONE, i;
    soc_regaddrlist_t   alist;
    soc_regaddrinfo_t   *ainfo;
    soc_reg_info_t *reginfo;
    int             f;
    int field_found = 0;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return SOC_E_PARAM;
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, print all or some values ... */
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
    return rv;
}

/* return the memory ID and block instance ID
 * of the register given as a string input.
 *
 * @param [in] unit - the relevant unit
 * @param [in] name - the memory name
 * #param [out] mem_id - the returned memory ID
 * @param [out] block_instance_id - the block instance
 *
 * Example: "CGM_INSTRUMENTATION_MEMORY[0]"
 */
int diag_mem_info_get(int unit, char *name, soc_mem_t *mem_id, soc_block_t *block_instance_id)
{
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, mem_id, name, block_instance_id, &array_index) < 0) {
        cli_out("ERROR: unknown memory \"%s\"\n",name);
        return SOC_E_PARAM;
    }

    return rv;
}

int diag_mem_get(int unit, char *name, int index, mem_val value)
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return SOC_E_PARAM;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        return SOC_E_PARAM;
    }

    /* Created entry, now write it */
    if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, value)) < 0) {
        cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                0 : copyno, index,
                soc_errmsg(rv));
    }


    return rv;
}


/* Set a memory field in a memory value stored in memory. The device memory is not read or written */
int diag_mem_field_get(int unit, char *name, char* field_name, int index, mem_val value)
{
    int         copyno;
    soc_field_info_t *fieldp;
    soc_mem_t   mem;
    soc_mem_info_t *memp;
    int f;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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
    sal_memset(value, 0, sizeof (mem_val));
    sal_memset(tmp, 0, sizeof(char) * ((SOC_MAX_MEM_FIELD_WORDS * 8) + 3) );

    if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, fval)) < 0) {
        cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                0 : copyno, index,
                soc_errmsg(rv));
    }
    for (f = memp->nFields - 1; f >= 0; f--) {
        fieldp = &memp->fields[f];
        if (!(sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name))) {
            soc_mem_field_get(unit, mem, fval, fieldp->field, value);
            _shr_format_long_integer(tmp, value, SOC_MAX_MEM_FIELD_WORDS);
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
    return rv;
}

/* The funciton diag_mem_field_only_get is getting only the field without first getting the memory value */
int diag_mem_field_only_get(int unit, char *name, char* field_name, int index, mem_val value)
{
    int         copyno;
    soc_field_info_t *fieldp;
    soc_mem_t   mem;
    soc_mem_info_t *memp;
    int f;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;  
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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
    sal_memset(value, 0, sizeof (mem_val));
    sal_memset(tmp, 0, sizeof(char) * ((SOC_MAX_MEM_FIELD_WORDS * 8) + 3) );

    if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, fval)) < 0) {
        cli_out("Read ERROR: table %s.%d[%d]: %s\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                0 : copyno, index,
                soc_errmsg(rv));
    }
    for (f = memp->nFields - 1; f >= 0; f--) {
        fieldp = &memp->fields[f];
        if (!(sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name))) {
            soc_mem_field_get(unit, mem, fval, fieldp->field, value);
            _shr_format_long_integer(tmp, value, SOC_MAX_MEM_FIELD_WORDS);
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
    return rv;
}

int diag_soc_mem_read_range(int unit, char *name, int index1, int index2, void *buffer)
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

    if (parse_memory_name(unit, &mem, name, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",name);
        return SOC_E_PARAM;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        return SOC_E_PARAM;
    }

    /* Created entry, now write it */
    if ((rv = soc_mem_array_read_range(unit, mem, array_index, copyno, index1, index2, buffer)) < 0) {
        cli_out("Read ERROR: table %s.%d[%d-%d]: %s\n",
                SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                0 : copyno, index1, index2,
                soc_errmsg(rv));
    }

    return rv;
}

int diag_mem_set(int unit, char *name, int start, int count, mem_val value)
{
    int         index, copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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

    return rv;
}

int diag_mem_field_set(int unit, char *name, char* field_name, int start, int count, mem_val value)
{
    int         index, copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    soc_field_info_t *fieldp;
    soc_mem_info_t *memp;
    int f;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    unsigned    array_index;



    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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
        sal_memset(fval, 0, sizeof (fval));

        if ((rv = soc_mem_array_read(unit, mem, array_index, copyno, index, fval)) < 0) {
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
            soc_mem_field_set(unit, mem, fval, fieldp->field, value);
        }

        if ((rv = soc_mem_array_write(unit, mem, array_index, copyno, index, fval)) < 0) {
            cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index,
                    soc_errmsg(rv));
        }
    }

    return rv;
}

/* Get a memory field from a memory value stored in memory. The device memory is not read. */
int diag_mem_field_only_set(int unit, char *name, char* field_name, int start, int count, mem_val value)
{
    int         index, copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    soc_field_info_t *fieldp;
    soc_mem_info_t *memp;
    int f;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    unsigned    array_index;



    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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
        sal_memset(fval, 0, sizeof (fval));
        for (f = memp->nFields - 1; f >= 0; f--) {
            fieldp = &memp->fields[f];
            if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name)) {
                continue;
            }
            soc_mem_field_set(unit, mem, fval, fieldp->field, value);
        }

        if ((rv = soc_mem_array_write(unit, mem, array_index, copyno, index, fval)) < 0) {
            cli_out("Write ERROR: table %s.%d[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ?
                    0 : copyno, index,
                    soc_errmsg(rv));
        }
    }

    return rv;
}


int diag_soc_mem_write_range(int unit, char *name, int index1, int index2, void *buffer)
{
    int         copyno;
    soc_mem_t   mem;
    int         rv = SOC_E_NONE;
    unsigned    array_index;

    if (!SOC_UNIT_VALID(unit)) {
        cli_out("Invalid unit.\n");
        return SOC_E_UNIT;
    }

    if (name == NULL) {
        return SOC_E_PARAM;
    }

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

    return rv;
}
#else
/* To avoid empty file warning in not supported architectures */
int appl_diag_diag_anchor;
#endif /* #if defined(BCM_SAND_SUPPORT) || defined(BCM_ESW_SUPPORT) */
