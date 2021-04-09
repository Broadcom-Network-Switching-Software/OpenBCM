/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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


static int inband_mem_read(int unit, int route_id, const char *names, int* indecies, mem_val* values)
{
  return inband_mem_handle(unit, route_id, names, indecies, inband_mem_read_handle, values);
}

static int inband_mem_write(int unit, int route_id, const char *names, int* indecies, mem_val* values)
{
  return inband_mem_handle(unit, route_id, names, indecies, inband_mem_write_handle, values);
}

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


