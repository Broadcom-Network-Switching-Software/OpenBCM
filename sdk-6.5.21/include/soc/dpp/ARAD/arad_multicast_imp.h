/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_MULTICAST_IMP_INCLUDED__
#define __ARAD_MULTICAST_IMP_INCLUDED__



#include <soc/types.h>
#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>



























uint32 arad_mcdb_get_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry,      
    SOC_SAND_IN  uint32  entry_type, 
    SOC_SAND_OUT uint32  *next_entry 
);


uint32 arad_mcdb_set_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_to_set, 
    SOC_SAND_IN  uint32  entry_type,   
    SOC_SAND_IN  uint32  next_entry    
);


uint32 arad_mcds_get_replications_from_entry(
    SOC_SAND_IN    int     unit,
    SOC_SAND_IN    int     core,        
    SOC_SAND_IN    uint8   get_bm_reps, 
    SOC_SAND_IN    uint32  entry_index, 
    SOC_SAND_IN    uint32  entry_type,  
    SOC_SAND_INOUT uint32  *cud2,       
    SOC_SAND_INOUT uint16  *max_size,   
    SOC_SAND_INOUT uint16  *group_size, 
    SOC_SAND_OUT   uint32  *next_entry  
);


uint32 arad_convert_ingress_replication_hw2api(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       cud,            
    SOC_SAND_IN  uint32       dest,           
    SOC_SAND_OUT soc_gport_t  *port_array,    
    SOC_SAND_OUT soc_if_t     *encap_id_array 
);


uint32 arad_mult_ing_encode_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_OUT   uint32                 *out_destination, 
    SOC_SAND_OUT   uint32                 *out_cud          
  );




#endif 


