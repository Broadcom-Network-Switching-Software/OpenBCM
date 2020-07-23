/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __DPP_MULTICAST_H__
#define __DPP_MULTICAST_H__



#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/types.h>
#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <shared/swstate/sw_state.h>









#define MCDS_GET_COMMON_MEMBER(mcds, m) (((dpp_mcds_common_t*)(mcds))->m)

#define MCDS_INGRESS_LINK_END(mcds) MCDS_GET_COMMON_MEMBER(mcds, ingress_link_end)
#define MCDS_GET_MCDB_ENTRY(mcds, index) MCDS_GET_COMMON_MEMBER((mcds), get_mcdb_entry_from_mcds)((mcds), (index))

#define MCDS_GET_MCDB_ENTRY_TYPE(mcds, entry) MCDS_GET_COMMON_MEMBER(mcds, get_mcdb_entry_type)(entry)
#define MCDS_SET_MCDB_ENTRY_TYPE(mcds, entry, type_value) MCDS_GET_COMMON_MEMBER(mcds, set_mcdb_entry_type)((entry), (type_value))
#define MCDS_GET_MCDB_ENTRY_TYPE_FROM_MCDS(mcds, index) MCDS_GET_MCDB_ENTRY_TYPE(mcds, MCDS_GET_MCDB_ENTRY((mcds), (index)))
#define MCDS_SET_MCDB_ENTRY_TYPE_FROM_MCDS(mcds, index, type_value) MCDS_SET_MCDB_ENTRY_TYPE(mcds, MCDS_GET_MCDB_ENTRY((mcds), (index)), (type_value))

#define MCDS_GET_NEXT_POINTER(mcds, unit, entry, entry_type, next_entry) MCDS_GET_COMMON_MEMBER(mcds, get_next_pointer)((unit), (entry), (entry_type), (next_entry))
#define MCDS_SET_NEXT_POINTER(mcds, unit, entry, entry_type, next_entry) MCDS_GET_COMMON_MEMBER(mcds, set_next_pointer)((unit), (entry), (entry_type), (next_entry))



#define JER_SWDB_ENTRY_GET_TYPE(entry) ((entry)->word1 >> JER_SWDB_MCDB_TYPE_SHIFT) 
#define JER_SWDB_ENTRY_SET_TYPE(entry, type_value)  \
    do {(entry)->word1 = ((entry)->word1 & ~(JER_SWDB_MCDB_TYPE_MASK << JER_SWDB_MCDB_TYPE_SHIFT)) | \
      ((type_value) << JER_SWDB_MCDB_TYPE_SHIFT); } while (0)
#define JER_SWDB_MCDB_GET_TYPE(mcds, index) JER_SWDB_ENTRY_GET_TYPE((mcds)->mcdb + (index)) 
#define JER_SWDB_MCDB_SET_TYPE(mcds, index, type_value) JER_SWDB_ENTRY_SET_TYPE((mcds)->mcdb + (index), (type_value)) 








typedef void dpp_mcds_t;       
typedef void dpp_mcdb_entry_t; 

typedef uint32 dpp_mc_id_t;
 



typedef dpp_mcdb_entry_t* (*dpp_get_mcdb_entry_from_mcds_f)(
    SOC_SAND_IN  dpp_mcds_t* mcds,
    SOC_SAND_IN  uint32 mcdb_index
  );

typedef uint32 (*dpp_get_mcdb_entry_type_f)(
    SOC_SAND_IN  dpp_mcdb_entry_t* entry
  );

typedef void (*dpp_set_mcdb_entry_type_f)(
    SOC_SAND_INOUT  dpp_mcdb_entry_t* entry,
    SOC_SAND_IN     uint32 type_value
  );



typedef uint32 (*dpp_get_next_pointer_f)(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry,      
    SOC_SAND_IN  uint32  entry_type, 
    SOC_SAND_OUT uint32  *next_entry 
  );
  

typedef uint32 (*dpp_set_next_pointer_f)(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_to_set, 
    SOC_SAND_IN  uint32  entry_type,   
    SOC_SAND_IN  uint32  next_entry    
  );


typedef struct {
    
    dpp_get_mcdb_entry_from_mcds_f get_mcdb_entry_from_mcds;
    dpp_get_mcdb_entry_type_f get_mcdb_entry_type;
    dpp_set_mcdb_entry_type_f set_mcdb_entry_type;
    dpp_get_next_pointer_f get_next_pointer;
    dpp_set_next_pointer_f set_next_pointer;

    uint32 ingress_link_end; 
    uint32 flags;
} dpp_mcds_common_t;

typedef struct
{
  PARSER_HINT_ARR SHR_BITDCL *egress_groups_open_data; 
  PARSER_HINT_ARR SHR_BITDCL *cud2core;           
} ARAD_MULTICAST;



dpp_mcds_t *dpp_get_mcds(
    SOC_SAND_IN  int unit
  );


uint32 dpp_alloc_mcds(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  unsigned   size_of_mcds, 
    SOC_SAND_OUT dpp_mcds_t **mcds_out    
);


uint32 dpp_dealloc_mcds(
    SOC_SAND_IN  int        unit
);




#ifndef _DPP_NO_MC_ASSERTS
#define DPP_MC_ASSERT(cond) do {if (!(cond)) dpp_perform_mc_assert(__FILE__, __LINE__);} while (0)
void dpp_perform_mc_assert(const char *file_name, unsigned line_number);
EXTERN int arad_mcds_asserts_enabled;
#else
#define DPP_MC_ASSERT(cond)
#endif




uint32 dpp_mcds_get_nof_asserts(void);
uint8 dpp_mcds_get_mc_asserts_enabled(void);
void dpp_mcds_set_mc_asserts_enabled(uint8 enabled);
uint8 dpp_mcds_get_mc_asserts_real(void);
void dpp_mcds_set_mc_asserts_real(uint8 real);

#endif 

