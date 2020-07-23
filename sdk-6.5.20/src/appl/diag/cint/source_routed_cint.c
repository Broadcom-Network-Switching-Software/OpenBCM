/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hand-coded support for soc_dpp routines. 
 */

typedef int dfe_cint_data_not_empty; /* Make ISO compilers happy. */
#include <sdk_config.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/register.h>
#include <appl/diag/system.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/vsc256_fabric_cell.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(INCLUDE_LIB_CINT) && (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/dcmn_fabric_cell.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#define SEPERATOR ' '

/* soc fabric cell  { */

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_inband_route_set,
                         int,int,unit,0,0,
                         int,int,route_id,0,0,
                         soc_fabric_inband_route_t*,soc_fabric_inband_route_t,route,1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         soc_inband_route_get,
                         int,int,unit,0,0,
                         int,int,route_id,0,0,
                         soc_fabric_inband_route_t*,soc_fabric_inband_route_t,route,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_inband_route_group_set,
                         int,int,unit,0,0,
                         int,int,group_id,0,0,
                         int,int,flags,0,0,
                         int,int,route_count,0,0,
                         int*,int,route_ids,1,0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                         soc_inband_route_group_get,
                         int,int,unit,0,0,
                         int,int,group_id,0,0,
                         int,int,flags,0,0,
                         int,int,route_count_max,0,0,
                         int*,int,route_count,1,0,
                         int*,int,route_ids,1,0);

typedef soc_reg_above_64_val_t reg_val;
typedef uint32                 mem_val[SOC_MAX_MEM_WORDS];

/* The function allocates arraies in size nof_entries each one.
   The firset parameter is a pointer soc_reg_t or soc_memt_t; the parameter is_regs determine wheater it is registers or memories.
   The second parameter is ports or copynos pointer                                                                              .
   the parameters indecies and vals are not allocated in a case of memories because it is not necessary.                                                                                                                               .
   The parameter is_above_64 determines if the pointer vals will be allocated to uint64 entries or soc_reg_above_64_val_t .                                                                                                                                                                                                                                    .
 */
static int
alloc_arraies(void **regs_or_mems, int **ports_or_copy_nos, int **indecies, void **vals, int nof_entries, uint8 is_regs, uint8 is_above_64)
{
  if(is_regs)
  {
    *regs_or_mems = sal_alloc(nof_entries*sizeof(soc_reg_t), 0);
  }
  else
  {
    *regs_or_mems = sal_alloc(nof_entries*sizeof(soc_mem_t), 0);
  }
  if(!(*regs_or_mems)) {
    cli_out("Memory allocation failed\n");
    return SOC_E_FAIL;
  }
  *ports_or_copy_nos = sal_alloc(nof_entries*sizeof(int), 0);
  if(!(*ports_or_copy_nos)) {
    cli_out("Memory allocation failed\n");
    sal_free(*regs_or_mems);
    return SOC_E_FAIL;
  }

  if(!is_regs)
  {/*no need to allocate indecies and values*/
    return SOC_E_NONE;
  }

  *indecies = sal_alloc(nof_entries*sizeof(int), 0);
  if(!(*indecies)) {
    cli_out("Memory allocation failed\n");
    sal_free(*ports_or_copy_nos);
    sal_free(*regs_or_mems);
    return SOC_E_FAIL;
  }

  if(is_above_64) {
    *vals = sal_alloc(nof_entries*sizeof(soc_reg_above_64_val_t), 0);
  }
  else
  {
    *vals = sal_alloc(nof_entries*sizeof(uint64), 0);
  }
  if(!(*vals)) {
    cli_out("Memory allocation failed\n");
    sal_free(*indecies);
    sal_free(*ports_or_copy_nos);
    sal_free(*regs_or_mems);
    return SOC_E_FAIL;
  }

  return SOC_E_NONE;
}

/* The function handles the reading of the registers. It sould be called by the function inband_reg_handle*/
static int inband_reg_read_handle(const soc_regaddrlist_t* alist, int nof_regs, int unit, int flags, int route_id, reg_val *value, uint8 is_above_64)
{
  int i, rv;
  soc_regaddrinfo_t	*ainfo;
  int junk_array_count;
  void* vals = NULL;
  soc_reg_t *regs = NULL;
  bcm_port_t *ports = NULL;
  int *indecies = NULL;
  void *void_regs = NULL;

  for(i=0; i< nof_regs; ++i) {
    if (alist[i].count >1) { 
    /* a list is not supported, only a single address */
  
      cli_out("Only a single address can be read.\n");
      return SOC_E_PARAM;
    }
  }
  
  rv = alloc_arraies(&void_regs, &ports, &indecies, &vals, nof_regs, TRUE, is_above_64);
  if(rv)
  {
    return rv;
  }
  regs = void_regs;

  for(i=0; i < nof_regs; ++i) {
    ainfo = &alist[i].ainfo[0];
    regs[i] = ainfo->reg;
    ports[i] = ainfo->port;
    indecies[i] = ainfo->idx;
  }

  if(is_above_64)
  {
    rv = soc_inband_reg_above_64_read(unit, flags, route_id, nof_regs, regs, ports, indecies, vals, &junk_array_count);   
  }
  else
  {
    rv = soc_inband_reg_read(unit, flags, route_id, nof_regs, regs, ports, indecies, vals, &junk_array_count);
  }

  if(rv < 0)
  {
    cli_out("ERROR: read from registers failed: %s\n",
            soc_errmsg(rv));
  }

  else
  {
    for(i=0; i < nof_regs; ++i) {
      if(is_above_64){
        sal_memcpy(value[i], ((soc_reg_above_64_val_t*)vals)[i], 128/8);
      }
      else{
        sal_memcpy(value[i], (uint64*)vals+i , 64/8);
      }
    }
  }

  sal_free(vals);
  sal_free(indecies);
  sal_free(ports);
  sal_free(regs);
  return rv;
}

/* The function handles the writing of the registers. It sould be called by the function inband_reg_handle*/
static int inband_reg_write_handle(const soc_regaddrlist_t* alist, int nof_regs, int unit, int flags, int route_id, reg_val *value, uint8 is_above_64)
{
  int rv, i, j, index;
  soc_regaddrinfo_t	*ainfo;
  int array_count;
  soc_reg_t *regs = NULL;
  bcm_port_t *ports = NULL;
  int *indecies = NULL;
  void *vals = NULL;
  void *void_regs = NULL;

  array_count = 0;
  for(i=0; i < nof_regs; ++i) {
    array_count += alist[i].count;
  }

  rv = alloc_arraies(&void_regs, &ports, &indecies, &vals, array_count, TRUE, is_above_64);
  if(rv) {
    return rv;
  }
  regs = void_regs;

  index = 0;
  for(i=0; i < nof_regs; ++i)
  {
    for(j=0; j < alist[i].count; ++j)
    {
      ainfo = &alist[i].ainfo[j];
      regs[index] = ainfo->reg;
      ports[index] = ainfo->port;
      indecies[index] = ainfo->idx;
      if(is_above_64){
        sal_memcpy(((soc_reg_above_64_val_t*)vals)[index++], value[i], 128/8);
      }
      else{
        sal_memcpy(&((uint64*)vals)[index++], value[i], 64/8);
      }
    }
  }

  if(is_above_64)
  {
    rv = soc_inband_reg_above_64_write(unit, flags, route_id, array_count, regs, ports, indecies, vals);
  }
  else
  {
    rv = soc_inband_reg_write(unit, flags, route_id, array_count, regs, ports, indecies, vals);
  }
  if(rv)
  { 
    cli_out("ERROR: write to registers failed: %s\n",
            soc_errmsg(rv));
  }

  sal_free(vals);
  sal_free(indecies);
  sal_free(ports);
  sal_free(regs);
  return rv;
}


/* The function handles the transactions of reading/writing from/to registers 64 bits or above 64 bits.
   The function get a pointer to the function that sould do the desired action read/write and a parametr if the registers are above 64 bits or not.
   The function gets teh names of the regosters seperated by the SEPERATOR symbol */ 
static int inband_reg_handle(
  int unit,
  int route_id,
  const char *names,
  int(*handle_func)(const soc_regaddrlist_t*, int nof_regs, int unit, int flags, int route_id, reg_val *val, uint8 is_above_64),
  reg_val *value,
  uint8 is_above_64
)
{
  int             rv = SOC_E_NONE;
  soc_regaddrlist_t *alist;
  char *reg_beg, *reg_end = NULL;
  const char *end;
  int i, flags = 0, nof_regs;
  char *reg_names = NULL;
  int len_names = 0;

  if (!SOC_UNIT_VALID(unit)) {
    cli_out("Invalid unit.\n");
    return SOC_E_UNIT;
  }

  if (names == NULL) {
    return SOC_E_PARAM;
  }

  end = names;
  /* Symbolic name given, print all or some values ... */
  for(nof_regs = 0; end; nof_regs++) {
    end = strchr(end, SEPERATOR);
    if(end) {
      ++end;
    }
  }

  alist = sal_alloc(nof_regs*sizeof(soc_regaddrlist_t), 0);
  if(!alist) {
    cli_out("Memory aloocation failed\n");
    return SOC_E_FAIL;
  }

  len_names = sal_strlen(names);
  reg_names = sal_alloc(len_names+1, 0);
  if(!reg_names) {
    cli_out("Memory allocation failed\n");
    sal_free(alist);
    return SOC_E_FAIL;
  }
  sal_strncpy(reg_names, names,len_names);
  if (len_names)
      reg_names[len_names] = '\0';

  reg_beg = reg_names;
  for(i=0; i<nof_regs; ++i) {
    if (*reg_beg == '$') {
      ++reg_beg;
    }
    if( i < nof_regs - 1)/*i.e. skeep the last register*/
    {
      reg_end = strchr(reg_beg, SEPERATOR);
      if (reg_end) {
          *reg_end = 0;
      }
    }
    if (soc_regaddrlist_alloc(&alist[i]) < 0) {
      cli_out("Could not allocate address list.  Memory error.\n");
      rv = SOC_E_PARAM;
      break;
    }
    
    if(isint(reg_beg))
    {
      if(i > 0)
      {
        if(!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
        {
          cli_out("All the registers should be addresses or names\n");
          rv = SOC_E_PARAM;
          break;
        }
      }
      alist[i].count = 1;
      alist[i].ainfo[0].reg = atoi(reg_beg);
      alist[i].ainfo[0].port = 0;
      alist[i].ainfo[0].idx = 0;
    }
    else
    {
      if(i > 0) 
      {
          cli_out("All the registers should be addresses or names\n");
          rv = SOC_E_PARAM;
          break;
      }

      if (parse_symbolic_reference(unit, &alist[i], reg_beg) < 0) {
        cli_out("Syntax error parsing \"%s\"\n", reg_beg);
        rv = SOC_E_PARAM;
        break;
      }
    }
    if (reg_end) {
        reg_beg = reg_end + 1;
    }
  }

  sal_free(reg_names);
  if(i < nof_regs)/*i.e. the loop was broken */
  {
    nof_regs = i;/*And then will be allocations free */
  }
  else
  {
    rv = handle_func(alist, nof_regs, unit, flags, route_id, value, is_above_64);
  }

  for(i=0; i < nof_regs; ++i) {
    soc_regaddrlist_free(&alist[i]);
  }
  sal_free(alist);
  return rv;
}

/* The function handles the reading of the memories. It sould be called by the function inband_mem_handle*/
static int inband_mem_read_handle(
  int unit, 
  uint32 flags, 
  int route_id, 
  int nof_mems,
  soc_mem_t *mems, 
  int *copynos,
  int *indecies,
  mem_val *values 
)
{
  int rv;
  int junk_array_count;
  int i;
  void **ptr;
  ptr = sal_alloc(nof_mems*sizeof(void*), 0);
  if(!ptr) {
    cli_out("Memory allocation failed\n");
    return SOC_E_FAIL;
  }
  for(i=0; i< nof_mems; ++i) {
    ptr[i] = &values[i];
  }
  rv = soc_inband_mem_read(unit, flags, route_id, nof_mems, mems, copynos, indecies, ptr, &junk_array_count);
  sal_free(ptr);
  if(rv < 0)
  {
    cli_out("Read ERROR: %s\n",
            soc_errmsg(rv));
  }

  return rv;
}

/* The function handles the writing of the memories. It sould be called by the function inband_mem_handle*/
static int inband_mem_write_handle(
  int unit, 
  uint32 flags, 
  int route_id, 
  int nof_mems,
  soc_mem_t *mems, 
  int *copynos,
  int *indecies,
  mem_val *values 
)
{
  int rv;
  int i;
  void **ptr;
  ptr = sal_alloc(nof_mems*sizeof(void*), 0);
  if(!ptr) {
    cli_out("Memory allocation failed\n");
    return SOC_E_FAIL;
  }
  for(i=0; i< nof_mems; ++i) {
    ptr[i] = &values[i];
  }
  
  rv = soc_inband_mem_write(unit, flags, route_id, nof_mems, mems, copynos, indecies, ptr);
  sal_free(ptr);
  if(rv < 0)
  { 
    cli_out("Write ERROR: %s\n",
            soc_errmsg(rv));
  }

  return rv;
}

/* The function handles the transactions of reading/writing from/to memories.
   The function get a pointer to the function that sould do the desired action read/write.
   The function gets the names of the regosters seperated by the SEPERATOR symbol */ 
static int 
inband_mem_handle(
  int unit,
  int route_id,
  const char *names,
  int* indecies,
  int(*handle_func)(
    int unit, 
    uint32 flags, 
    int route_id, 
    int nof_mems,
    soc_mem_t *mem, 
    int *copynos,
    int *indecies,
    mem_val*  values
  ),
  mem_val *value
)
{
  int         *copynos = NULL;
  soc_mem_t   *mems = NULL;
  int         rv = SOC_E_NONE;
  char *mem_beg, *mem_end = NULL;
  const char *end;
  int i, flags = 0, nof_mems;
  void *void_mems = NULL;
  char *mem_names = NULL;
  int len_names = 0;

  if(!SOC_UNIT_VALID(unit)) {
    cli_out("Invalid unit.\n");
	return SOC_E_UNIT;
  }

  if (names == NULL) {
	return SOC_E_PARAM;
  }

  end = names;
  /* Symbolic name given, print all or some values ... */
  for(nof_mems = 0; end; nof_mems++) {
    end = strchr(end, SEPERATOR);
    if(end) {
      ++end;
    }
  }

  len_names = sal_strlen(names);
  mem_names = sal_alloc(len_names+1, 0);
  if(!mem_names) {
    cli_out("Memory allocation failed\n");
    return SOC_E_FAIL;
  }
  sal_strncpy(mem_names, names,len_names);
  if (len_names)
      mem_names[len_names] = '\0';

  rv = alloc_arraies(&void_mems, &copynos, NULL, NULL, nof_mems, FALSE, FALSE);
  if(rv)
  {
    sal_free(mem_names);
    return rv;
  }
  mems = void_mems;

  mem_beg = mem_names;
  for(i=0; i < nof_mems; ++i)
  {
    if(i < nof_mems - 1)/* i.e. skeep in the last memory */
    {
      mem_end = strchr(mem_beg, SEPERATOR);
      if (mem_end) {
          *mem_end = 0;
      }
    }

    if(isint(mem_beg))
    {
      if(i > 0)
      {
        if(!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
        {
          cli_out("All the memories should be addresses or names\n");
          break;
        }
      }

      mems[i] = atoi(mem_beg);
      copynos[i] = 0;
    }
    else
    {
      if(i > 0)
      {
        if(!(flags & SOC_DCMN_SR_CELL_FLAG_FE600_FORMAT))
        {
          cli_out("All the memories should be addresses or names\n");
          break;
        }
      }

      if (parse_memory_name(unit, &mems[i], mem_beg, &copynos[i], 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",mem_beg);
        break;
      }
    }
    if (!SOC_MEM_IS_VALID(unit, mems[i])) {
      cli_out("Error: Memory %s not valid for chip %s.\n",
              SOC_MEM_UFNAME(unit, mems[i]), SOC_UNIT_NAME(unit));
      break;
    }
    if (mem_end) {
        mem_beg = mem_end + 1;
    }
  }

  sal_free(mem_names);
  if(i < nof_mems)/*i.e. the loop has been broken*/
  {
    sal_free(mems);
    sal_free(copynos);
    return SOC_E_PARAM;
  }

  rv = handle_func(unit, flags, route_id, nof_mems, mems, copynos, indecies, value);
  sal_free(mems);
  sal_free(copynos);
  return rv;
}

static int inband_reg_read(int unit, int route_id, const char *names, reg_val* values)
{
  return inband_reg_handle(unit, route_id, names, inband_reg_read_handle, values, FALSE);
}

static int inband_reg_above_64_read(int unit, int route_id, const char *names, reg_val *values)
{
  return inband_reg_handle(unit, route_id, names, inband_reg_read_handle, values, TRUE);
}

static int inband_reg_write(int unit, int route_id, const char *names, reg_val *values)
{
  return inband_reg_handle(unit, route_id, names, inband_reg_write_handle, values, FALSE);
}

static int inband_reg_above_64_write(int unit, int route_id, const char *names, reg_val *values)
{
  return inband_reg_handle(unit, route_id, names, inband_reg_write_handle, values, TRUE);
}

static int inband_mem_read(int unit, int route_id, const char *names, int* indecies, mem_val* values)
{
  return inband_mem_handle(unit, route_id, names, indecies, inband_mem_read_handle, values);
}

static int inband_mem_write(int unit, int route_id, const char *names, int* indecies, mem_val* values)
{
  return inband_mem_handle(unit, route_id, names, indecies, inband_mem_write_handle, values);
}

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         inband_reg_read,
                         int,int,unit,0,0,
                         int, int, route_id, 0, 0,
                         char*,char,names,1,0,
                         reg_val*, reg_val, values, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         inband_reg_write,
                         int,int,unit,0,0,
                         int, int, route_id, 0, 0,
                         char*,char,names,1,0,
                         reg_val*, reg_val, values, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         inband_reg_above_64_read,
                         int,int,unit,0,0,
                         int, int, route_id, 0, 0,
                         char*,char,names,1,0,
                         reg_val*, reg_val, values, 1, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         inband_reg_above_64_write,
                         int,int,unit,0,0,
                         int, int, route_id, 0, 0,
                         char*,char,names,1,0,
                         reg_val*, reg_val, values, 1, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         inband_mem_read,
                         int, int, unit, 0, 0,
                         int, int, route_id, 0, 0,
                         char*, char, names, 1, 0,
                         int*, int, indecies,1,0,
                         mem_val*,mem_val,values,1,0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         inband_mem_write,
                         int, int, unit, 0, 0,
                         int, int, route_id, 0, 0,
                         char*, char, names, 1, 0,
                         int*, int, indecies,1,0,
                         mem_val*,mem_val,values,1,0);

static cint_function_t __cint_source_routed_functions[] = 
{
  CINT_FWRAPPER_ENTRY(soc_inband_route_set),
  CINT_FWRAPPER_ENTRY(soc_inband_route_get),
  CINT_FWRAPPER_ENTRY(soc_inband_route_group_set),
  CINT_FWRAPPER_ENTRY(soc_inband_route_group_get),
  CINT_FWRAPPER_ENTRY(inband_reg_read),
  CINT_FWRAPPER_ENTRY(inband_reg_write),
  CINT_FWRAPPER_ENTRY(inband_reg_above_64_read),
  CINT_FWRAPPER_ENTRY(inband_reg_above_64_write),
  CINT_FWRAPPER_ENTRY(inband_mem_read),
  CINT_FWRAPPER_ENTRY(inband_mem_write),

  CINT_ENTRY_LAST
};  

static cint_parameter_desc_t __cint_struct_members__soc_fabric_inband_route_t[] =
{
  {
    "uint32",
      "number_of_hops",
      0,
      0
  },
  {
    "int",
      "link_ids",
      0,
      FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS
    },
    { NULL }
};

static cint_parameter_desc_t __cint_typedefs[] = 
{
  {
    "uint32",
      "reg_val",
      0,
      SOC_REG_ABOVE_64_MAX_SIZE_U32
  },
  {
    "uint32",
      "mem_val",
      0,
      SOC_MAX_MEM_WORDS
  },
  { NULL }
};

static void*
__cint_maddr__soc_fabric_inband_route_t(void* p, int mnum, cint_struct_type_t* parent)
{
  void* rv;
  soc_fabric_inband_route_t* s = (soc_fabric_inband_route_t*) p;

  switch(mnum)
  {
  case 0: rv = &(s->number_of_hops); break;
  case 1: rv = &(s->link_ids); break;
  default: rv = NULL; break;
  }

  return rv;
}

static cint_struct_type_t __cint_source_routed_structures[] =
{   
  {
    "soc_fabric_inband_route_t",
      sizeof(soc_fabric_inband_route_t),
      __cint_struct_members__soc_fabric_inband_route_t,
      __cint_maddr__soc_fabric_inband_route_t
  },
  { NULL }
};

cint_data_t source_routed_cint_data = 
{
  NULL,
  __cint_source_routed_functions,
  __cint_source_routed_structures,
  NULL, 
  __cint_typedefs, 
  NULL, 
  NULL
}; 

#endif /* INCLUDE_LIB_CINT && BCM_DFE_SUPPORT*/


