/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __DPP_MULTICAST_IMP_H__
#define __DPP_MULTICAST_IMP_H__



#include <soc/dpp/multicast.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/types.h>
#include <soc/dpp/dpp_config_defs.h>











#define DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START 0  
#define DPP_MCDS_TYPE_VALUE_FREE_BLOCK       1  
#define DPP_MCDS_TYPE_VALUE_INGRESS_START    2  
#define DPP_MCDS_TYPE_VALUE_INGRESS          3  
#define DPP_MCDS_TYPE_VALUE_EGRESS_START     4  
#define DPP_MCDS_TYPE_VALUE_EGRESS           5  
#define DPP_MCDS_TYPE_VALUE_EGRESS_TDM_START 6  
#define DPP_MCDS_TYPE_VALUE_EGRESS_TDM       7  


#define DPP_MCDS_TYPE_IS_FREE(type) (!((type) & 6))                  
#define DPP_MCDS_TYPE_IS_USED(type) ((type) & 6)                     
#define DPP_MCDS_TYPE_IS_INGRESS(type) (((type) & 6) == 2)           
#define DPP_MCDS_TYPE_IS_EGRESS(type) ((type) & 4)                   
#define DPP_MCDS_TYPE_IS_EGRESS_NORMAL(type) (((type) & 6) == 4)     
#define DPP_MCDS_TYPE_IS_NORMAL(type) (((type) ^ ((type) >> 1)) & 2) 
#define DPP_MCDS_TYPE_IS_TDM(type) (((type) & 6) == 6)               
#define DPP_MCDS_TYPE_IS_EGRESS_START(type) (((type) & 5) == 4)      
#define DPP_MCDS_TYPE_IS_EGRESS_NOT_START(type) (((type) & 5) == 5)  
#define DPP_MCDS_TYPE_IS_START(type) (((type) & 1) == 0)             
#define DPP_MCDS_TYPE_IS_INGRESS_OR_TDM(type) ((type) & 2)           
#define DPP_MCDS_TYPES_ARE_THE_SAME(t1, t2) ((((t1)^(t2)) & 6) == 0) 
#define DPP_MCDS_TYPE_GET_START(type) ((type) & 6)                   
#define DPP_MCDS_TYPE_GET_NONE_START(type) ((type) | 1)              




#define DPP_MCDS_MAX_FREE_BLOCK_SIZE_ALLOCED 8

#define DPP_MCDS_MAX_FREE_BLOCK_SIZE_GENERAL 8

#define DPP_MCDS_MAX_FREE_BLOCK_SIZE 8



#define DPP_MCDS_GET_FREE_BLOCKS_PREFER_SMALL   1  
#define DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE    2  
#define DPP_MCDS_GET_FREE_BLOCKS_NO_UPDATES     4  
                                                         
                                                         
#define DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS 8  
#define DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL      16 

#define ARAD_MCDS_NOF_REGIONS 3 







#define DPP_MC_ENTRY_SIZE 2                

#define ARAD_MC_ENTRY_MASK_VAL 0x7ffff
#define JER_MC_ENTRY_MASK_VAL 0xffffff

#define ARAD_MC_ENTRY_MASK_WORD(mcds, index) ((mcds)->mcdb[index].word1) 

#define DPP_MCDS_TYPE_SHIFT 29 
#define DPP_MAX_NOF_MCDB_INDEX_BITS 18 
#define DPP_NOF_REMAINDER_PREV_BITS (DPP_MAX_NOF_MCDB_INDEX_BITS - 16) 
#define DPP_NOF_REMAINDER_PREV_MASK ((1 << DPP_NOF_REMAINDER_PREV_BITS) - 1)
#define DPP_MCDS_MSB_PREV_BIT_SHIFT (DPP_MCDS_TYPE_SHIFT - DPP_NOF_REMAINDER_PREV_BITS) 
#define DPP_MCDS_BITMAP_OPEN_SHIFT (DPP_MCDS_MSB_PREV_BIT_SHIFT - 1) 
#define DPP_MCDS_TEST_BIT_SHIFT (DPP_MCDS_BITMAP_OPEN_SHIFT -1) 


#define DPP_MCDS_WORD1_KEEP_BITS_MASK  \
  ((1 << DPP_MCDS_BITMAP_OPEN_SHIFT) | (1 << DPP_MCDS_TEST_BIT_SHIFT))


#define DPP_MCDS_TYPE_MASK 7   

#define DPP_MCDS_ENTRY_GET_TYPE(entry) ((entry)->word1 >> DPP_MCDS_TYPE_SHIFT) 
#define DPP_MCDS_ENTRY_SET_TYPE(entry, type_value)  \
    do {(entry)->word1 = ((entry)->word1 & ~(DPP_MCDS_TYPE_MASK << DPP_MCDS_TYPE_SHIFT)) | \
      ((type_value) << DPP_MCDS_TYPE_SHIFT); } while (0)
#define DPP_MCDS_GET_TYPE(mcds, index) DPP_MCDS_ENTRY_GET_TYPE((mcds)->mcdb + (index)) 
#define DPP_MCDS_SET_TYPE(mcds, index, type_value) DPP_MCDS_ENTRY_SET_TYPE((mcds)->mcdb + (index), (type_value)) 


#define DPP_MCDS_MSB_PREV_BIT_MASK (DPP_NOF_REMAINDER_PREV_MASK << 16) 

#define DPP_MCDS_GET_PREV_ENTRY(mcds, index) \
  (((ARAD_MC_ENTRY_MASK_WORD((mcds), (index)) >> (DPP_MCDS_MSB_PREV_BIT_SHIFT - 16)) & \
    DPP_MCDS_MSB_PREV_BIT_MASK) | (mcds)->prev_entries[index])

#define DPP_MCDS_SET_PREV_ENTRY(mcds, index, prev_entry) \
  do { \
    ARAD_MC_ENTRY_MASK_WORD((mcds), (index)) = (ARAD_MC_ENTRY_MASK_WORD((mcds), (index)) & \
      ~(DPP_NOF_REMAINDER_PREV_MASK << DPP_MCDS_MSB_PREV_BIT_SHIFT)) | \
      (((prev_entry) & DPP_MCDS_MSB_PREV_BIT_MASK) << (DPP_MCDS_MSB_PREV_BIT_SHIFT - 16)); \
    (mcds)->prev_entries[index] = (prev_entry) & 0xffff; \
  } while(0);

#define DPP_MCDS_ENTRY_GET_PREV_ENTRY(entry, mcds, index) \
  ((((entry)->word1 >> (DPP_MCDS_MSB_PREV_BIT_SHIFT - 16)) & \
    DPP_MCDS_MSB_PREV_BIT_MASK) | (mcds)->prev_entries[index])
#define DPP_MCDS_ENTRY_SET_PREV_ENTRY(entry, mcds, index, prev_entry) \
  do { \
    (entry)->word1 = ((entry)->word1 & \
      ~(1 << DPP_MCDS_MSB_PREV_BIT_SHIFT)) | \
      (((prev_entry) & DPP_MCDS_MSB_PREV_BIT_MASK) << (DPP_MCDS_MSB_PREV_BIT_SHIFT - 16)); \
    (mcds)->prev_entries[index] = (prev_entry) & 0xffff; \
  } while(0);

#define DPP_MCDS_FREE_NEXT_PREV_MASK 0x3ffff
#define DPP_MCDS_FREE_BLOCK_SIZE_MASK 0xf 
#define DPP_MCDS_FREE_BLOCK_SIZE_SHIFT 19
#define DPP_MCDS_GET_FREE_NEXT_ENTRY(mcds, index)  \
  (ARAD_MC_ENTRY_MASK_WORD(mcds, index) & DPP_MCDS_FREE_NEXT_PREV_MASK)
#define DPP_MCDS_SET_FREE_NEXT_ENTRY(mcds, index, next_entry)  \
  ARAD_MC_ENTRY_MASK_WORD(mcds, index) = (ARAD_MC_ENTRY_MASK_WORD(mcds, index) & ~DPP_MCDS_FREE_NEXT_PREV_MASK) | (next_entry)
#define DPP_MCDS_ENTRY_GET_FREE_PREV_ENTRY(entry)  \
  ((entry)->word0 & DPP_MCDS_FREE_NEXT_PREV_MASK)
#define DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, index)  \
  ((mcds)->mcdb[index].word0 & DPP_MCDS_FREE_NEXT_PREV_MASK)
#define DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, index, prev_entry)  \
  (mcds)->mcdb[index].word0 = ((mcds)->mcdb[index].word0 & ~DPP_MCDS_FREE_NEXT_PREV_MASK) | (prev_entry)
#define DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, index)  \
  (((mcds)->mcdb[index].word0 >> DPP_MCDS_FREE_BLOCK_SIZE_SHIFT) & DPP_MCDS_FREE_BLOCK_SIZE_MASK)
#define DPP_MCDS_SET_FREE_BLOCK_SIZE(mcds, index, size)  \
  (mcds)->mcdb[index].word0 = ((mcds)->mcdb[index].word0 & ~((uint32)DPP_MCDS_FREE_NEXT_PREV_MASK << DPP_MCDS_FREE_BLOCK_SIZE_SHIFT)) | \
  ((size) << DPP_MCDS_FREE_BLOCK_SIZE_SHIFT)


#define DPP_MCDS_ENTRY_IS_BITMAP_OPEN(   entry)    (((entry)->word1 >> DPP_MCDS_BITMAP_OPEN_SHIFT) & 1) 
#define DPP_MCDS_ENTRY_SET_BITMAP_OPEN(  entry) do {(entry)->word1 |=  (1 << DPP_MCDS_BITMAP_OPEN_SHIFT);} while (0) 
#define DPP_MCDS_ENTRY_SET_BITMAP_CLOSED(entry) do {(entry)->word1 &= ~(1 << DPP_MCDS_BITMAP_OPEN_SHIFT);} while (0) 


#define DPP_MCDS_ENTRY_GET_TEST_BIT(     entry)    (((entry)->word1 >> DPP_MCDS_TEST_BIT_SHIFT) & 1)
#define DPP_MCDS_ENTRY_SET_TEST_BIT_ON(  entry) do {(entry)->word1 |=  (1 << DPP_MCDS_TEST_BIT_SHIFT);} while (0)
#define DPP_MCDS_ENTRY_SET_TEST_BIT_OFF( entry) do {(entry)->word1 &= ~(1 << DPP_MCDS_TEST_BIT_SHIFT);} while (0)
#define DPP_MCDS_ENTRY_SET_TEST_BIT(entry, val) do {(entry)->word1 &= ~(1 << DPP_MCDS_TEST_BIT_SHIFT); (entry)->word1 |= (((val) & 1) << DPP_MCDS_TEST_BIT_SHIFT);} while (0)



#define DPP_MCDS_GET_EGRESS_FORMAT(mcds, index) (SOC_IS_JERICHO(unit) ? JER_MCDS_GET_EGRESS_FORMAT(mcds, index): ARAD_MCDS_GET_EGRESS_FORMAT(mcds, index))
#define DPP_MCDS_IS_EGRESS_FORMAT_CONSECUTIVE(format) ((format) & 4)
#define DPP_MCDS_IS_EGRESS_FORMAT_CONSECUTIVE_END(format) (((format) & 5) == 4)
#define ARAD_MCDS_IS_EGRESS_FORMAT_CONSECUTIVE_NEXT(format) (((format) & 5) == 5)

#define ARAD_MCDS_GET_EGRESS_FORMAT(mcds, index) (((mcds)->mcdb[index].word1 & 0x70000) >> 16)
#define JER_MCDS_GET_EGRESS_FORMAT(mcds, index) (((mcds)->mcdb[index].word1 & 0xF00000) >> 20)

#define DPP_MULT_MAX_REPLICATIONS 4095 
#define DPP_MULT_MAX_INGRESS_REPLICATIONS DPP_MULT_MAX_REPLICATIONS 
#define DPP_MULT_MAX_EGRESS_REPLICATIONS DPP_MULT_MAX_REPLICATIONS  


#define DPP_MC_ING_DESTINATION_DISABLED 0x3ffff 
#define DPP_MC_EGR_OUTLIF_DISABLED 0            
#define DPP_MC_EGR_BITMAP_DISABLED 0            
#define DPP_MC_NOF_EGRESS_PORTS 256             
#define DPP_MULT_EGRESS_PORT_INVALID (DPP_MC_NOF_EGRESS_PORTS-1) 
#define ARAD_MULT_EGRESS_SMALL_PORT_INVALID 127 
#define DPP_MC_NOF_EGRESS_BITMAP_WORDS (SOC_TMC_NOF_FAP_PORTS_JERICHO/SOC_SAND_NOF_BITS_IN_UINT32) 

#define DPP_MC_EGRESS_LINK_PTR_END 0
#define DPP_MC_INGRESS_1ST_ENTRY 0
#define DPP_MC_EGR_CUD_INVALID 0 
#define IRDB_TABLE_ENTRY_WORDS 2

#define DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY ((uint32)(-1))

#define DPP_MC_EGR_BITMAP_ID_BITS 13  
#define DPP_MC_EGR_NOF_BITMAPS (1 <<  DPP_MC_EGR_BITMAP_ID_BITS) 
#define DPP_MC_EGR_MAX_BITMAP_ID (DPP_MC_EGR_NOF_BITMAPS - 1)

#define DPP_LAST_MCDB_ENTRY(mcds) MCDS_INGRESS_LINK_END(mcds)








typedef uint8 dpp_free_entries_block_size_t; 

typedef struct { 
  uint32 first; 
} dpp_free_entries_block_list_t;

typedef struct { 
  dpp_free_entries_block_size_t max_size; 
  uint32 range_start;
  uint32 range_end; 
  dpp_free_entries_block_list_t lists[DPP_MCDS_MAX_FREE_BLOCK_SIZE]; 
} dpp_free_entries_blocks_region_t;



typedef struct
{
  uint32 word0;
  uint32 word1;
} arad_mcdb_entry_t;

typedef struct {
    uint32 base;  
    uint32 extra;    
} dpp_rep_data_t; 





#define DPP_MCDS_REP_DATA_PORT_NOF_BITS 8
#define DPP_MCDS_REP_DATA_CUD_NOF_BITS 19
#define DPP_MCDS_REP_DATA_BM_ID_NOF_BITS DPP_MC_EGR_BITMAP_ID_BITS
#define DPP_MCDS_REP_DATA_TYPE_NOF_BITS 3
#define DPP_MCDS_REP_DATA_TYPE_OFFSET 10
#define DPP_MCDS_REP_DATA_PORT_MASK ((1 << DPP_MCDS_REP_DATA_PORT_NOF_BITS) - 1)
#define DPP_MCDS_REP_DATA_BM_ID_MASK ((1 << DPP_MCDS_REP_DATA_BM_ID_NOF_BITS) - 1)
#define DPP_MCDS_REP_DATA_TYPE_MASK ((1 << DPP_MCDS_REP_DATA_TYPE_NOF_BITS) - 1)
#define DPP_MCDS_REP_DATA_CUD_MASK ((1 << DPP_MCDS_REP_DATA_CUD_NOF_BITS) - 1)
#define DPP_MCDS_REP_DATA_EGR_CUD_SHIFT 13
#define DPP_MCDS_REP_DATA_EXTRA_CUD_SHIFT 13
#define DPP_MCDS_REP_DATA_INGR_DEST_EXTRA_BITS (18 - DPP_MCDS_REP_DATA_BM_ID_NOF_BITS)
#define DPP_MCDS_REP_DATA_INGR_DEST_EXTRA_BITS_MASK ((1 << DPP_MCDS_REP_DATA_INGR_DEST_EXTRA_BITS) - 1)

#define DPP_MCDS_REP_TYPE_INGRESS 0
#define DPP_MCDS_REP_TYPE_EGR_PORT_CUD 3
#define DPP_MCDS_REP_TYPE_EGR_CUD 2
#define DPP_MCDS_REP_TYPE_EGR_BM_CUD 1

#define DPP_MCDS_REP_DATA_SET_TYPE(data, type) do {(data)->extra |= ((type) << DPP_MCDS_REP_DATA_TYPE_OFFSET);} while (0)
#define DPP_MCDS_REP_DATA_RESET_TYPE(data, type) do {(data)->extra &= ~(((uint32)DPP_MCDS_REP_DATA_TYPE_MASK) << DPP_MCDS_REP_DATA_TYPE_OFFSET); \
  (data)->extra |= ((type) << DPP_MCDS_REP_DATA_TYPE_OFFSET);} while (0)
#define DPP_MCDS_REP_DATA_GET_TYPE(data) (((data)->extra >> DPP_MCDS_REP_DATA_TYPE_OFFSET) & DPP_MCDS_REP_DATA_TYPE_MASK)

#define DPP_MCDS_REP_DATA_SET_EXTRA_CUD(data, cud) do {(data)->extra |= (cud) << DPP_MCDS_REP_DATA_EXTRA_CUD_SHIFT;} while (0)
#define DPP_MCDS_REP_DATA_RESET_EXTRA_CUD(data, cud) do {(data)->extra &= ~(((uint32)DPP_MCDS_REP_DATA_CUD_MASK) << DPP_MCDS_REP_DATA_EXTRA_CUD_SHIFT); \
                                                         (data)->extra |= (cud) << DPP_MCDS_REP_DATA_EXTRA_CUD_SHIFT;} while (0)
#define DPP_MCDS_REP_DATA_GET_EXTRA_CUD(data) ((data)->extra >> DPP_MCDS_REP_DATA_EXTRA_CUD_SHIFT)

#define DPP_MCDS_REP_DATA_SET_EGR_PORT(data, port) do {(data)->base |= (port);} while (0)
#define DPP_MCDS_REP_DATA_RESET_EGR_PORT(data, port) do {(data)->base &= ~(uint32)DPP_MCDS_REP_DATA_PORT_MASK; (data)->base |= (port);} while (0)
#define DPP_MCDS_REP_DATA_GET_EGR_PORT(data) ((data)->base & DPP_MCDS_REP_DATA_PORT_MASK)

#define DPP_MCDS_REP_DATA_SET_EGR_BM_ID(data, bm_id) do {(data)->base |= (bm_id);} while (0)
#define DPP_MCDS_REP_DATA_RESET_EGR_BM_ID(data, bm_id) do {(data)->base &= ~(uint32)DPP_MCDS_REP_DATA_BM_ID_MASK; (data)->base |= (bm_id);} while (0)
#define DPP_MCDS_REP_DATA_GET_EGR_BM_ID(data) ((data)->base & DPP_MCDS_REP_DATA_BM_ID_MASK)

#define DPP_MCDS_REP_DATA_SET_EGR_CUD(data, cud) do {(data)->base |= (cud) << DPP_MCDS_REP_DATA_EGR_CUD_SHIFT;} while (0)
#define DPP_MCDS_REP_DATA_RESET_EGR_CUD(data, cud) do {(data)->base &= ~(((uint32)DPP_MCDS_REP_DATA_CUD) << DPP_MCDS_REP_DATA_EGR_CUD_SHIFT); \
                                                       (data)->base |= (cud) << DPP_MCDS_REP_DATA_EGR_CUD_SHIFT;} while (0)
#define DPP_MCDS_REP_DATA_GET_EGR_CUD(data) ((data)->base >> DPP_MCDS_REP_DATA_EGR_CUD_SHIFT)

#define DPP_MCDS_REP_DATA_SET_INGR_CUD(data, cud) DPP_MCDS_REP_DATA_SET_EGR_CUD(data, cud)
#define DPP_MCDS_REP_DATA_RESET_INGR_CUD(data, cud) DPP_MCDS_REP_DATA_RESET_EGR_CUD(data, cud)
#define DPP_MCDS_REP_DATA_GET_INGR_CUD(data) DPP_MCDS_REP_DATA_GET_EGR_CUD(data)

#define DPP_MCDS_REP_DATA_SET_INGR_DEST(data, dest) do {DPP_MCDS_REP_DATA_SET_EGR_BM_ID((data), (dest) & DPP_MCDS_REP_DATA_BM_ID_MASK); (data)->extra |= \
  (dest) >> DPP_MCDS_REP_DATA_BM_ID_NOF_BITS;} while (0)
#define DPP_MCDS_REP_DATA_RESET_INGR_DEST(data, dest) do {DPP_MCDS_REP_DATA_RESET_EGR_BM_ID((data), (dest) & DPP_MCDS_REP_DATA_BM_ID_MASK); \
  (data)->extra &= ~DPP_MCDS_REP_DATA_INGR_DEST_EXTRA_BITS_MASK; (data)->extra |= (dest) >> DPP_MCDS_REP_DATA_BM_ID_NOF_BITS;} while (0)
#define DPP_MCDS_REP_DATA_GET_INGR_DEST(data) (DPP_MCDS_REP_DATA_GET_EGR_BM_ID(data) | \
  (((data)->extra & DPP_MCDS_REP_DATA_INGR_DEST_EXTRA_BITS_MASK) << DPP_MCDS_REP_DATA_BM_ID_NOF_BITS))


#define DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core_id) ((mcds)->egress_mcdb_offset + (mcds)->nof_egr_ll_groups * (core_id) + (mcid))


#define DPP_MCDS_GET_REP_INDEX(core_id, i) ((DPP_MULT_MAX_REPLICATIONS * (core_id)) + (i))

#define DPP_CUD2CORE_BITS_PER_CUD 2
#define DPP_CUD2CORE_CUD_MASK ((1 << DPP_CUD2CORE_BITS_PER_CUD) - 1)
#define DPP_CUD2CORE_CUDS_PER_WORD (SOC_SAND_NOF_BITS_IN_UINT32 / DPP_CUD2CORE_BITS_PER_CUD)
#define DPP_CUD2CORE_UNDEF_VALUE DPP_CUD2CORE_CUD_MASK
#define DPP_CUD2CORE_GET_CORE(_unit, _cud, _core) do { \
        uint32 _lcl_core = 0;\
        SOCDNX_IF_ERR_EXIT( \
            sw_state_access[(_unit)].dpp.soc.arad.tm.arad_multicast.cud2core.bit_range_read( \
                (_unit), \
                (_cud) * DPP_CUD2CORE_BITS_PER_CUD, \
                0, \
                DPP_CUD2CORE_BITS_PER_CUD, \
                &_lcl_core) \
        ); \
        (_core) = _lcl_core; \
    } while (0)
#define DPP_CUD2CORE_SET_CORE(_unit, _cud, _core) do { \
        uint32 _lcl_core = (_core); \
        SOCDNX_IF_ERR_EXIT( \
            sw_state_access[(_unit)].dpp.soc.arad.tm.arad_multicast.cud2core.bit_range_write( \
                (_unit), \
                (_cud) * DPP_CUD2CORE_BITS_PER_CUD, \
                0, \
                DPP_CUD2CORE_BITS_PER_CUD,\
                &_lcl_core)\
        );\
  } while (0)


typedef enum
{
  McdsFreeBuildBlocksAdd_AllMustBeUsed = 0, 
  McdsFreeBuildBlocksAddAll,                
  McdsFreeBuildBlocksAddOnlyFree            
} mcds_free_build_option_t;

typedef enum
{ 
  irdb_value_group_closed     = 0, 
  irdb_value_group_open       = 1, 
  irdb_value_group_open_mmc   = 3, 
  irdb_value_group_max_value  = 3  
} irdb_value_t;


typedef uint32 dpp_mc_egr_bitmap_t[DPP_MC_NOF_EGRESS_BITMAP_WORDS + 1];




typedef uint32 dpp_mc_ingress_dest_t; 
typedef uint32 dpp_mc_outlif_t;       
typedef uint8  dpp_mc_local_port_t;   
typedef uint16 dpp_mc_bitmap_id_t;    

typedef uint32 dpp_mc_core_bitmap_t; 
#define DPP_MC_CORE_BITAMAP_NO_CORES ((dpp_mc_core_bitmap_t)0)
#define DPP_MC_CORE_BITAMAP_CORE_0 ((dpp_mc_core_bitmap_t)1)
#define DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit) ((DPP_MC_CORE_BITAMAP_CORE_0 << SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) - 1)
#define DPP_MC_FOREACH_CORE(core_bitmap, core_bitmap_temp, core) \
    for ((core) = 0, (core_bitmap_temp) = (core_bitmap); (core_bitmap_temp); ++core, (core_bitmap_temp) = (core_bitmap_temp) >> 1) \
        if (core_bitmap_temp & DPP_MC_CORE_BITAMAP_CORE_0)

#define DPP_MC_NO_2ND_CUD 0 
#define DPP_MC_2ND_CUD_IS_EEI 0x80000000 

#define DPP_MC_2ND_CUD_TYPE_MCDS2REP(type) (((uint32)(type)) << 31) 
#define DPP_MC_2ND_CUD_TYPE_REP2MCDS(type) (2-(uint8)(((uint32)(type)) >> 31)) 

typedef enum 
{
  dpp_mc_group_2nd_cud_none = 0,   
  dpp_mc_group_2nd_cud_outrif = 1, 
  dpp_mc_group_2nd_cud_eei = 2     
} dpp_mc_group_2nd_cud_type_t;

typedef struct {
    uint32 dest;           
    uint32 cud;            
    uint32 additional_cud; 
                           
} dpp_mc_replication_t; 





typedef void (*dpp_egr_mc_write_entry_port_cud_f)(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
);


typedef void (*dpp_egr_mc_write_entry_port_cud_noptr_f)(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint8             use_next     
                                                  
);


typedef void (*dpp_egr_mc_write_entry_cud_f)(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
);


typedef void (*dpp_egr_mc_write_entry_cud_noptr_f)(
    SOC_SAND_IN     int               unit,
    SOC_SAND_INOUT  arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN     dpp_rep_data_t    *rep1,       
    SOC_SAND_IN     dpp_rep_data_t    *rep2,       
    SOC_SAND_IN     dpp_rep_data_t    *rep3,       
    SOC_SAND_IN     uint8             use_next     
                                                   
);


typedef void (*dpp_egr_mc_write_entry_bm_cud_f)(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep,        
    SOC_SAND_IN    uint32            next_entry   
);


typedef uint32 (*dpp_get_replications_from_entry_f)(
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


typedef uint32 (*dpp_convert_ingress_replication_hw2api_f)(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       cud,            
    SOC_SAND_IN  uint32       dest,           
    SOC_SAND_OUT soc_gport_t  *port_array,    
    SOC_SAND_OUT soc_if_t     *encap_id_array 
);


typedef uint32 (*dpp_set_egress_linked_list_f)(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         is_group_start,      
    SOC_SAND_IN  uint32                        group_id,            
    SOC_SAND_IN  uint32                        list_end,            
    SOC_SAND_IN  uint32                        alloced_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t alloced_block_size,  
    SOC_SAND_IN  dpp_mc_core_bitmap_t          cores_to_set,        
    SOC_SAND_OUT uint32                        *list_start,         
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err             
);

typedef struct {
    dpp_mcds_common_t common; 
    uint32 nof_unoccupied; 

    
    arad_mcdb_entry_t *mcdb;    
    uint16 *prev_entries;       
    uint32 free_value[2];       
    uint32 empty_ingr_value[2]; 
    uint32 msb_word_mask      ; 
    uint32 ingr_word1_replication_mask; 
    uint32 max_ingr_cud_field;    
    uint32 max_egr_cud_field;   
    uint32 egress_mcdb_offset;  
    uint32 nof_egr_ll_groups;   
    int unit; 
    dpp_free_entries_blocks_region_t free_general; 
    dpp_free_entries_blocks_region_t ingress_alloc_free; 
    dpp_free_entries_blocks_region_t egress_alloc_free;

    dpp_egr_mc_write_entry_port_cud_f egr_mc_write_entry_port_cud;
    dpp_egr_mc_write_entry_port_cud_noptr_f egr_mc_write_entry_port_cud_noptr;
    dpp_egr_mc_write_entry_cud_f egr_mc_write_entry_cud;
    dpp_egr_mc_write_entry_cud_noptr_f egr_mc_write_entry_cud_noptr;
    dpp_egr_mc_write_entry_bm_cud_f egr_mc_write_entry_bm_cud;
    dpp_get_replications_from_entry_f get_replications_from_entry;
    dpp_convert_ingress_replication_hw2api_f convert_ingress_replication_hw2api;
    dpp_set_egress_linked_list_f set_egress_linked_list;

#ifdef DONOT_USE_SW_DB_FOR_MULTICAST
    uint32 *egress_groups_open_data; 
#endif

    
    
    dpp_rep_data_t reps[DPP_MULT_MAX_REPLICATIONS * SOC_DPP_DEFS_MAX(NOF_CORES)]; 

    uint16 nof_reps[SOC_DPP_DEFS_MAX(NOF_CORES)]; 
    uint16 nof_ingr_reps;            
    uint16 nof_egr_bitmap_reps[SOC_DPP_DEFS_MAX(NOF_CORES)];      
    uint16 nof_egr_port_outlif_reps[SOC_DPP_DEFS_MAX(NOF_CORES)]; 
    uint16 nof_egr_outlif_reps[SOC_DPP_DEFS_MAX(NOF_CORES)];      
    uint8 info_2nd_cud; 

    uint16 max_nof_ingr_replications; 
    uint16 max_nof_mmc_replications; 

} dpp_mcds_base_t; 



#define DPP_MC_GROUP_2ND_CUD_NONE   0 
#define DPP_MC_GROUP_2ND_CUD_EEI    1 
#define DPP_MC_GROUP_2ND_CUD_OUTRIF 2 






dpp_mcdb_entry_t* dpp_mcds_get_mcdb_entry(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mcdb_index
);



int dpp_rep_data_t_compare(void *a, void *b);



uint32 dpp_mcds_write_entry(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mcdb_index 
);


uint32 dpp_mcdb_copy_write(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 src_index, 
    SOC_SAND_IN uint32 dst_index  
);


void dpp_mcds_clear_replications(dpp_mcds_base_t *mcds, const uint32 group_type);


uint32 dpp_mcds_copy_replications_to_arrays(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8        is_egress,           
    SOC_SAND_IN  uint32       arrays_size,         
    SOC_SAND_OUT soc_gport_t  *port_array,         
    SOC_SAND_OUT soc_if_t     *encap_id_array,     
    SOC_SAND_OUT soc_multicast_replication_t *reps 
);


uint32 dpp_mcds_copy_replications_from_arrays(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8     is_egress,       
    SOC_SAND_IN  uint8     do_clear,        
    SOC_SAND_IN  uint32    arrays_size,     
    SOC_SAND_IN  dpp_mc_replication_t *reps 
);


uint32
  dpp_mcds_copy_replications_from_egress_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         do_clear,    
    SOC_SAND_IN  uint32                        block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t block_size,  
    SOC_SAND_INOUT uint32                      *cud2,       
    SOC_SAND_OUT uint32                        *next_entry  
);


void dpp_add_ingress_replication(
  dpp_mcds_base_t *mcds,
  const uint32     cud,
  const uint32     dest
);

void dpp_add_egress_replication_port_cud(
  dpp_mcds_base_t *mcds,
  unsigned         core,
  const uint32     cud,
  const uint32     cud2,
  const uint32     port
);

void dpp_add_egress_replication_cud(
  dpp_mcds_base_t *mcds,
  unsigned         core,
  const uint32     cud,
  const uint32     cud2
);

void dpp_add_egress_replication_bitmap(
  dpp_mcds_base_t *mcds,
  uint32           core,
  const uint32     cud,
  const uint32     cud2,
  const uint32     bm_id
);





void dpp_mcds_init_region(dpp_free_entries_blocks_region_t *region, dpp_free_entries_block_size_t max_size, uint32 range_start, uint32 range_end);


uint32 dpp_mcds_build_free_blocks(
    SOC_SAND_IN    int                              unit,   
    SOC_SAND_INOUT dpp_mcds_base_t                  *mcds,
    SOC_SAND_IN    uint32                           start_index, 
    SOC_SAND_IN    uint32                           end_index,   
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    mcds_free_build_option_t         entry_option 
);


uint32 dpp_mcds_reserve_group_start(
    SOC_SAND_INOUT dpp_mcds_base_t *mcds,
    SOC_SAND_IN    uint32           mcdb_index 
);


uint32 dpp_mcds_get_free_entries_block(
    SOC_SAND_INOUT dpp_mcds_base_t              *mcds,
    SOC_SAND_IN    uint32                        flags,        
    SOC_SAND_IN    dpp_free_entries_block_size_t wanted_size,  
    SOC_SAND_IN    dpp_free_entries_block_size_t max_size,     
    SOC_SAND_OUT   uint32                        *block_start, 
    SOC_SAND_OUT   dpp_free_entries_block_size_t *block_size   
);


dpp_free_entries_blocks_region_t* dpp_mcds_get_region(dpp_mcds_base_t *mcds, uint32 mcdb_index);


dpp_free_entries_blocks_region_t* dpp_mcds_get_region_and_consec_range(dpp_mcds_base_t *mcds, uint32 mcdb_index, uint32 *range_start, uint32 *range_end);




uint32 dpp_mcdb_relocate_entries(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        mcdb_index,             
    SOC_SAND_IN  uint32                        relocation_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t relocation_block_size,  
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err                
);

uint32 dpp_mcds_unoccupied_get(
    SOC_SAND_IN dpp_mcds_base_t *mcds
);





uint32 dpp_mcdb_free_linked_list(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       first_index, 
    SOC_SAND_IN  uint32                       entries_type 
);



uint32
  dpp_mcdb_free_linked_list_till_my_end(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    first_index,  
    SOC_SAND_IN  uint32    entries_type, 
    SOC_SAND_IN  uint32    end_index     
);






uint32 dpp_mult_properties_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  mcid,
    SOC_SAND_IN  irdb_value_t value 
);


int dpp_mult_does_group_exist(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  mcid,       
    SOC_SAND_IN  int     is_egress,  
    SOC_SAND_OUT uint8   *does_exit
);


uint32 dpp_egress_group_open_set(
    SOC_SAND_IN  int     unit, 
    SOC_SAND_IN  uint32  group_id,  
    SOC_SAND_IN  uint8   is_open    
);


uint32 dpp_mcds_get_group(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_core_bitmap_t cores_to_get, 
    SOC_SAND_IN  uint8                do_clear,     
    SOC_SAND_IN  uint8                get_bm_reps,  
    SOC_SAND_IN  uint32               group_id,     
    SOC_SAND_IN  uint32               group_type,   
    SOC_SAND_IN  uint16               max_size,     
    SOC_SAND_OUT uint16               *group_size   
);


uint32 dpp_mult_remove_replication(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               group_type, 
    SOC_SAND_IN  uint32               dest,       
    SOC_SAND_IN  soc_if_t             cud,        
    SOC_SAND_IN  soc_if_t             cud2,       
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err,   
    SOC_SAND_OUT dpp_mc_core_bitmap_t *cores      
);






uint32 dpp_mult_ing_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY  *ing_entry,       
    SOC_SAND_IN  uint32               next_entry,       
    SOC_SAND_IN  uint32               prev_entry        
);






int dpp_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id, 
    SOC_SAND_OUT uint8 *is_open
);



uint32 dpp_mcds_set_egress_linked_list(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         is_group_start,      
    SOC_SAND_IN  uint32                        group_id,            
    SOC_SAND_IN  uint32                        list_end,            
    SOC_SAND_IN  uint32                        alloced_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t alloced_block_size,  
    SOC_SAND_IN  dpp_mc_core_bitmap_t          cores_to_set,        
    SOC_SAND_OUT uint32                        *list_start,         
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err             
);





uint32 dpp_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int unit
);


uint32
  dpp_mcds_get_relocation_block(
    SOC_SAND_IN  dpp_mcds_base_t              *mcds,
    SOC_SAND_IN  uint32                        mcdb_index,              
    SOC_SAND_OUT uint32                        *relocation_block_start, 
    SOC_SAND_OUT dpp_free_entries_block_size_t *relocation_block_size   
);





uint32 dpp_mcds_multicast_init(
    SOC_SAND_IN int      unit
);


uint32 dpp_mcds_multicast_init2(
    SOC_SAND_IN int      unit
);


uint32 dpp_mcds_multicast_terminate(
    SOC_SAND_IN int unit
);


uint32 dpp_mc_init(
    SOC_SAND_IN  int                 unit
);


uint32 dpp_mult_does_group_exist_ext( 
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,      
    SOC_SAND_IN  int             is_egress, 
    SOC_SAND_OUT uint8           *is_open   
);





uint32 dpp_mult_ing_group_open(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,        
    SOC_SAND_IN  uint32                 mc_group_size,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
);
 

uint32 dpp_mult_ing_group_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t      multicast_id_ndx 
);


uint32 dpp_mult_ing_group_update(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  dpp_mc_id_t          multicast_id_ndx,    
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY   *mc_group,           
    SOC_SAND_IN  uint32                mc_group_size,       
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             
);

uint32 dpp_mult_ing_traffic_class_map_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_TMC_MULT_ING_TR_CLS_MAP *map
);
uint32 dpp_mult_ing_traffic_class_map_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *map
);


uint32 dpp_mult_ing_destination_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *replication,     
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
);


uint32 dpp_mult_ing_destination_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *entry,           
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
);


uint32 dpp_mult_ing_group_size_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx,
    SOC_SAND_OUT uint32      *mc_group_size
);


uint32 dpp_mult_ing_get_group(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t group_mcid,           
    SOC_SAND_IN  uint32      mc_group_size,        
    SOC_SAND_OUT soc_gport_t *ports,               
    SOC_SAND_OUT soc_if_t    *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,
    SOC_SAND_OUT uint32      *exact_mc_group_size, 
    SOC_SAND_OUT uint8       *is_open              
);


uint32 dpp_mult_ing_all_groups_close(
    SOC_SAND_IN  int unit
);





uint32 dpp_mult_eg_bitmap_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
);


uint32 dpp_mult_eg_bitmap_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
);


uint32 dpp_mult_eg_bitmap_group_create(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx 
);


uint32 dpp_mult_eg_bitmap_group_close(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx
);


uint32 dpp_mult_eg_bitmap_group_update(
                  int                                   unit,
                  dpp_mc_id_t                           multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
);


uint32 dpp_mult_eg_bitmap_group_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           bitmap_id, 
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     
);


uint32 dpp_mult_eg_bitmap_group_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      
);

uint32 dpp_mult_eg_bitmap_group_bm_add(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap, 
                  SOC_TMC_ERROR                         *out_err 
);


uint32 dpp_mult_eg_bitmap_group_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      
);

uint32 dpp_mult_eg_bitmap_group_bm_remove(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap, 
                  SOC_TMC_ERROR                         *out_err 
);

uint32 dpp_mult_eg_bitmap_group_close_all_groups(
    SOC_SAND_IN  int                 unit
);


uint32 dpp_mult_eg_group_close(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx
);


uint32 dpp_mult_eg_group_size_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t           multicast_id_ndx,
    SOC_SAND_OUT uint32                 *mc_group_size
);


uint32 dpp_mult_eg_get_group(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    group_mcid,           
    SOC_SAND_IN  uint32         mc_group_size,        
    SOC_SAND_OUT soc_gport_t    *ports,               
    SOC_SAND_OUT soc_if_t       *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32         *exact_mc_group_size, 
    SOC_SAND_OUT uint8          *is_open              
);


uint32 dpp_mult_eg_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_id_t          mcid,         
    SOC_SAND_IN  uint8                allow_create, 
    SOC_SAND_IN  uint32               group_size,   
    SOC_SAND_IN  dpp_mc_replication_t *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err      
);


uint32 dpp_mult_eg_reps_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_IN  uint32                nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t  *reps,      
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err    
);


uint32 dpp_mult_eg_reps_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   
    SOC_SAND_IN  uint32                nof_reps,     
    SOC_SAND_IN  dpp_mc_replication_t  *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      
);



uint32 dpp_mult_cud_to_port_map_set(
    SOC_SAND_IN int                 unit, 
    SOC_SAND_IN uint32              flags,
    SOC_SAND_IN uint32              cud,  
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port  
);


uint32 dpp_mult_cud_to_port_map_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32              cud,  
    SOC_SAND_OUT SOC_TMC_FAP_PORT_ID *port 
);



soc_error_t dpp_mult_get_entry(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 mcdb_index,
    SOC_SAND_OUT uint32 *entry 
);


uint32
dpp_mcdb_index_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 group_id, 
    SOC_SAND_IN int is_egress, 
    SOC_SAND_OUT uint16 *group_size, 
    SOC_SAND_OUT uint32 *index_core0, 
    SOC_SAND_OUT uint32 *index_core1 
);
#endif 

