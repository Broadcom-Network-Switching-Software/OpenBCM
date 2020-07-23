/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>


#include <soc/mcm/memregs.h>




#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>





























uint32 dpp_ing_mc_group_entry_to_mcdb_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_INOUT dpp_mcdb_entry_t       *mcdb_entry, 
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,  
    SOC_SAND_IN    uint32                 next_entry   
);


uint32 dpp_mult_properties_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  mcid,
    SOC_SAND_IN  irdb_value_t value 
)
{
  uint32 entry[2] = {0};
  const unsigned bit_offset = 2 * (mcid % 16);
  const int table_index = mcid / 16;
  irdb_value_t value_internal;

  SOCDNX_INIT_FUNC_DEFS;


  if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID out of range")));
  }

  SOCDNX_IF_ERR_EXIT(READ_IDR_IRDBm(unit, MEM_BLOCK_ANY, table_index, entry));
  value_internal = (*entry >> bit_offset) & irdb_value_group_max_value;

  
  if (value_internal != irdb_value_group_closed && value != irdb_value_group_closed)
  {
      if ((value_internal == irdb_value_group_open_mmc) && (value == irdb_value_group_open))
      {
          SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("mini multicast group (%d) exceeded max mini mc replication number"),mcid));
      }
  }
  else
  {
      if (value_internal != value) { 
          *entry &= ~(((uint32)irdb_value_group_max_value) << bit_offset);
          *entry |= (((uint32)value) << bit_offset);
          SOCDNX_IF_ERR_EXIT(WRITE_IDR_IRDBm(unit, MEM_BLOCK_ANY, table_index, entry));
      }
  }

exit:
  SOCDNX_FUNC_RETURN;
}




uint32 dpp_mult_properties_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  mcid,
    SOC_SAND_OUT irdb_value_t *value 
)
{
  uint32 entry[2];
  const unsigned bit_offset = 2 * (mcid % 16);
  const int table_index = mcid / 16;

  SOCDNX_INIT_FUNC_DEFS;
  if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID out of range")));
  }
  SOCDNX_NULL_CHECK(value);

  SOCDNX_IF_ERR_EXIT(READ_IDR_IRDBm(unit, MEM_BLOCK_ANY, table_index, entry));

  *value = ((*entry) >> bit_offset) & irdb_value_group_max_value;

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_group_set(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY  *mc_group,
    SOC_SAND_IN  uint32               mc_group_size,
    SOC_SAND_IN  uint8                allow_create, 
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err      
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  int group_exists = 0, group_start_alloced = 0, failed = 1;
  uint32 group_entry_type;
  uint32 nof_entries_needed, remaining_group_entries = mc_group_size;
  uint32 old_group_entries = MCDS_INGRESS_LINK_END(mcds), linked_list = MCDS_INGRESS_LINK_END(mcds);
  uint32 block_start = 0;
  dpp_free_entries_block_size_t block_size = 0;
  arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, multicast_id_ndx);
  arad_mcdb_entry_t start_entry = {0};
  irdb_value_t prev_prop_value, new_prop_value;

  SOCDNX_INIT_FUNC_DEFS;
  if (mc_group_size) {
    SOCDNX_NULL_CHECK(mc_group);
    if (mc_group_size > mcds->max_nof_ingr_replications) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ingress MC group size is too big")));
    }
  }
  SOCDNX_NULL_CHECK(out_err);
  DPP_MC_ASSERT(multicast_id_ndx < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids);


  group_entry_type = DPP_MCDS_GET_TYPE(mcds, multicast_id_ndx);
  group_exists = group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS_START;
  if (!group_exists && !allow_create) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
  }

  
  if (!group_exists && multicast_id_ndx != DPP_MC_EGRESS_LINK_PTR_END) {
    if (DPP_MCDS_TYPE_IS_USED(group_entry_type)) { 
      SOCDNX_IF_ERR_EXIT(dpp_mcdb_relocate_entries(unit, multicast_id_ndx, 0, 0, out_err));
      if (*out_err) { 
        SOC_EXIT;
      }
    } else { 
      SOCDNX_IF_ERR_EXIT(dpp_mcds_reserve_group_start(mcds, multicast_id_ndx));
      DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS_START); 
    }
    group_start_alloced = 1;
  }

  
  nof_entries_needed = mc_group_size;
  if (mc_group_size) { 
    --nof_entries_needed;
  }
  
  if (nof_entries_needed > dpp_mcds_unoccupied_get(mcds)) { 
    *out_err = _SHR_E_FULL;
    SOC_EXIT;
  }
  *out_err = _SHR_E_NONE;

  
  if (mc_group_size) { 
    SOCDNX_IF_ERR_EXIT(dpp_ing_mc_group_entry_to_mcdb_entry(unit, &start_entry, mc_group, 0));
    --remaining_group_entries;
  } else { 
    start_entry.word0 = mcds->empty_ingr_value[0];
    start_entry.word1 = mcds->empty_ingr_value[1];
  }

  
  while (remaining_group_entries) { 
    dpp_free_entries_block_size_t max_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE;
    uint32 cur_entry, next_entry = linked_list;
    if (remaining_group_entries < DPP_MCDS_MAX_FREE_BLOCK_SIZE) {
      max_block_size = remaining_group_entries;
    }
    SOCDNX_IF_ERR_EXIT( 
      dpp_mcds_get_free_entries_block(mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_SMALL,
        1, max_block_size, &block_start, &block_size)); 
    DPP_MC_ASSERT(block_size > 0 && block_size <= max_block_size && remaining_group_entries >= block_size);

    for (cur_entry = block_start + block_size; cur_entry > block_start; ) { 
      --cur_entry;
      DPP_MC_ASSERT(remaining_group_entries);
      SOCDNX_IF_ERR_EXIT(dpp_mult_ing_multicast_group_entry_to_tbl( 
        unit, cur_entry, mc_group + (remaining_group_entries--), next_entry,
        (cur_entry == block_start ? multicast_id_ndx : cur_entry - 1)));
      next_entry = cur_entry;
    } 

    if (linked_list != MCDS_INGRESS_LINK_END(mcds)) { 
      DPP_MCDS_SET_PREV_ENTRY(mcds, linked_list, block_start + block_size - 1);
    }
    linked_list = block_start; 
    block_size = 0; 
  }

  

  if (group_exists) { 
    old_group_entries = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);
  }

  
  mcdb_entry->word0 = start_entry.word0; 
  mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
  mcdb_entry->word1 |= start_entry.word1;
  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, multicast_id_ndx); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS_START); 

  
  new_prop_value = mc_group_size > mcds->max_nof_mmc_replications ? irdb_value_group_open : irdb_value_group_open_mmc;
  if (group_exists) {
    SOCDNX_IF_ERR_EXIT(dpp_mult_properties_get_unsafe(unit, multicast_id_ndx, &prev_prop_value));
    if (mc_group_size > mcds->max_nof_mmc_replications && prev_prop_value != irdb_value_group_open) { 
      DPP_MC_ASSERT(new_prop_value == irdb_value_group_open);
      SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, new_prop_value));
      prev_prop_value = new_prop_value;
    }
  } else {
    prev_prop_value = irdb_value_group_closed;
  }

  SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
    mcds, unit, multicast_id_ndx, DPP_MCDS_TYPE_VALUE_INGRESS_START, linked_list));
  linked_list = MCDS_INGRESS_LINK_END(mcds); 

  if (old_group_entries != MCDS_INGRESS_LINK_END(mcds)) { 
    SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list(
      unit, old_group_entries, DPP_MCDS_TYPE_VALUE_INGRESS));
  }

  if (prev_prop_value != new_prop_value) { 
    SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, new_prop_value));
  }

  failed = 0;
exit:
  if (linked_list != MCDS_INGRESS_LINK_END(mcds)) { 
    DPP_MC_ASSERT(failed);
    dpp_mcdb_free_linked_list(unit, linked_list, DPP_MCDS_TYPE_VALUE_INGRESS);
  }
  if (block_size) { 
    DPP_MC_ASSERT(failed);
    dpp_mcds_build_free_blocks(unit, mcds,
      block_start, block_start + block_size - 1, dpp_mcds_get_region(mcds, block_start), McdsFreeBuildBlocksAdd_AllMustBeUsed);
  }
  if (group_start_alloced && failed) { 
    DPP_MC_ASSERT(!group_exists);
    dpp_mcds_build_free_blocks(unit, mcds,
      multicast_id_ndx, multicast_id_ndx, dpp_mcds_get_region(mcds, multicast_id_ndx), McdsFreeBuildBlocksAdd_AllMustBeUsed);
  }

  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_group_open(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,        
    SOC_SAND_IN  uint32                 mc_group_size,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
)
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(dpp_mult_ing_group_set(
    unit, multicast_id_ndx, mc_group, mc_group_size, TRUE, out_err));
exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_get_group(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t group_mcid,           
    SOC_SAND_IN  uint32      mc_group_size,        
    SOC_SAND_OUT soc_gport_t *ports,               
    SOC_SAND_OUT soc_if_t    *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32      *exact_mc_group_size, 
    SOC_SAND_OUT uint8       *is_open              
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(exact_mc_group_size);
  SOCDNX_NULL_CHECK(is_open);
  if (mc_group_size && (!ports || !cuds)) { 
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("NULL pointer")));
  }


  SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, group_mcid, FALSE, is_open));
  if (*is_open) {
    uint16 group_size;
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group(
      unit, DPP_MC_CORE_BITAMAP_CORE_0, TRUE, TRUE, group_mcid, DPP_MCDS_TYPE_VALUE_INGRESS, mc_group_size, &group_size));
    *exact_mc_group_size = group_size;
    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_to_arrays(unit, 0, mc_group_size, ports, cuds, NULL));
  } else { 
    *exact_mc_group_size = 0;
  }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_group_update(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  dpp_mc_id_t          multicast_id_ndx,    
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY   *mc_group,           
    SOC_SAND_IN  uint32                mc_group_size,       
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err             
)
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(dpp_mult_ing_group_set(
    unit, multicast_id_ndx, mc_group, mc_group_size, FALSE, out_err));
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_ing_group_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t      multicast_id_ndx 
)
{
  uint8 does_exist;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, multicast_id_ndx, FALSE, &does_exist));
  if (does_exist) { 
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    SOC_TMC_ERROR out_err;

    SOCDNX_IF_ERR_EXIT(dpp_mult_ing_group_set( 
      unit, multicast_id_ndx, 0, 0, FALSE, &out_err));
    if (out_err) { 
      SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
    }
    SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, irdb_value_group_closed));

    if (multicast_id_ndx != DPP_MC_EGRESS_LINK_PTR_END) { 
      SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(
        unit, mcds, multicast_id_ndx, multicast_id_ndx, dpp_mcds_get_region(mcds, multicast_id_ndx), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    } else { 
      arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, DPP_MC_EGRESS_LINK_PTR_END);
      mcdb_entry->word0 = ((dpp_mcds_base_t*)mcds)->free_value[0];
      mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
      mcdb_entry->word1 |= ((dpp_mcds_base_t*)mcds)->free_value[1];
      SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, DPP_MC_EGRESS_LINK_PTR_END)); 
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}




uint32 dpp_mult_traverse_ingress_list(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32            list_start,   
    SOC_SAND_IN  arad_mcdb_entry_t *replication, 
    SOC_SAND_IN  uint16            max_size,     
    SOC_SAND_OUT uint16            *found_size,  
    SOC_SAND_OUT uint8             *was_found,   
    SOC_SAND_OUT uint32            *found_entry  
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 index = list_start;
  uint16 list_size = 1;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(replication);
  SOCDNX_NULL_CHECK(found_size);
  SOCDNX_NULL_CHECK(found_entry);
  DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_INGRESS(DPP_MCDS_GET_TYPE(mcds, list_start)) || list_start == MCDS_INGRESS_LINK_END(mcds));

  for (;; ++list_size) {
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, index);
    uint32 next_entry;

    if (list_size > max_size) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("passed max allowed entries number to traverse")));
    }
    
    if (replication->word0 ==  mcdb_entry->word0 &&
        replication->word1 == (mcdb_entry->word1 & mcds->ingr_word1_replication_mask)) {
      *was_found = TRUE;
      break;
    }
    next_entry = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);
    if (next_entry == MCDS_INGRESS_LINK_END(mcds)) { 
      *was_found = FALSE;
      break;
    }
    index = next_entry;
    DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_entry) == DPP_MCDS_TYPE_VALUE_INGRESS);
  }
  *found_size = list_size;
  *found_entry = index;

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_ing_destination_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *replication,     
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
)
{

  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *work_entry = MCDS_GET_MCDB_ENTRY(mcds, multicast_id_ndx);
  uint32 next_index, add_index, block_start;
  uint16 found_size;
  dpp_free_entries_block_size_t block_size;
  uint8 does_exist;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(out_err);

  SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, multicast_id_ndx, FALSE, &does_exist));
  if (!does_exist) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
  }
  DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(work_entry) == DPP_MCDS_TYPE_VALUE_INGRESS_START);

  *out_err = _SHR_E_NONE;
  next_index = soc_mem_field32_get(unit, IRR_MCDBm, work_entry, LINK_PTRf);
  if (soc_mem_field32_get(unit, IRR_MCDBm, work_entry, DESTINATIONf) == DPP_MC_ING_DESTINATION_DISABLED) { 
    DPP_MC_ASSERT(next_index == MCDS_INGRESS_LINK_END(mcds));
    
#ifdef DPP_INGR_MC_PERFORM_UNNEEDED_UPDATES
    SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, irdb_value_group_open_mmc)); 
#endif
    SOCDNX_IF_ERR_EXIT(dpp_mult_ing_multicast_group_entry_to_tbl( 
      unit, multicast_id_ndx, replication, next_index, multicast_id_ndx));
    SOC_EXIT;
  }

  if (!(SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_ALLOW_DUPLICATES_MODE)) { 

    arad_mcdb_entry_t replication_entry = {0};
    uint8 was_found = 0;
    SOCDNX_IF_ERR_EXIT(dpp_ing_mc_group_entry_to_mcdb_entry(unit, &replication_entry, replication, 0));

    
    SOCDNX_IF_ERR_EXIT(dpp_mult_traverse_ingress_list(unit, multicast_id_ndx, &replication_entry,
      mcds->max_nof_ingr_replications, &found_size, &was_found, &add_index));
    if (was_found) {
      *out_err = _SHR_E_EXISTS; 
      SOC_EXIT;
    }

  } else { 
    
    for (found_size = 1; found_size < mcds->max_nof_ingr_replications && next_index != MCDS_INGRESS_LINK_END(mcds); ++found_size) {
      next_index = soc_mem_field32_get(unit, IRR_MCDBm, MCDS_GET_MCDB_ENTRY(mcds, next_index), LINK_PTRf);
    }
  }
    if (found_size >= mcds->max_nof_ingr_replications) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ingress MC group size is too big to add to")));
    }

  add_index = multicast_id_ndx; 

  
  work_entry = MCDS_GET_MCDB_ENTRY(mcds, add_index);
  next_index = soc_mem_field32_get(unit, IRR_MCDBm, work_entry, LINK_PTRf);
  SOCDNX_IF_ERR_EXIT(dpp_mcds_get_free_entries_block( 
    mcds, DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
    1, 1, &block_start, &block_size));
  if (block_size != 1) { 
    *out_err = _SHR_E_FULL;
    DPP_MC_ASSERT(!block_size);
    SOC_EXIT;
  }

  SOCDNX_IF_ERR_EXIT(dpp_mult_ing_multicast_group_entry_to_tbl( 
    unit, block_start, replication, next_index, add_index));

  if (next_index != MCDS_INGRESS_LINK_END(mcds)) {
    DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_index) == DPP_MCDS_TYPE_VALUE_INGRESS);
    DPP_MCDS_SET_PREV_ENTRY(mcds, next_index, block_start); 
  }

  SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe( 
    unit, multicast_id_ndx, found_size >= mcds->max_nof_mmc_replications ? irdb_value_group_open : irdb_value_group_open_mmc));

  SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
    mcds, unit, add_index, DPP_MCDS_TYPE_VALUE_INGRESS, block_start));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_ing_destination_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *entry,           
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t replication_entry = {0}, *found_entry;
  uint16 found_size;
  uint32 found_index, next_index, prev_index, freed_index;
  uint8 was_found;
  uint8 does_exist;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(entry);

  SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, multicast_id_ndx, FALSE, &does_exist));
  if (!does_exist) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
  }

  SOCDNX_IF_ERR_EXIT(dpp_ing_mc_group_entry_to_mcdb_entry(unit, &replication_entry, entry, 0));

  SOCDNX_IF_ERR_EXIT(dpp_mult_traverse_ingress_list(
    unit, multicast_id_ndx, &replication_entry, mcds->max_nof_ingr_replications,
    &found_size, &was_found, &found_index));
  if (!was_found) {
    *out_err = _SHR_E_NOT_FOUND; 
    SOC_EXIT;
  }
  *out_err = _SHR_E_NONE;

  found_entry = MCDS_GET_MCDB_ENTRY(mcds, found_index);
  next_index = soc_mem_field32_get(unit, IRR_MCDBm, found_entry, LINK_PTRf);
  prev_index = DPP_MCDS_GET_PREV_ENTRY(mcds, found_index);

  if (found_index == multicast_id_ndx) { 

    DPP_MC_ASSERT(prev_index == multicast_id_ndx && found_size == 1);
    if (next_index == MCDS_INGRESS_LINK_END(mcds)) { 
      found_entry->word0 = ((dpp_mcds_base_t*)mcds)->empty_ingr_value[0];
      found_entry->word1 &= ~mcds->msb_word_mask;
      found_entry->word1 |= ((dpp_mcds_base_t*)mcds)->empty_ingr_value[1];
      SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, found_index)); 
#ifdef DPP_INGR_MC_PERFORM_UNNEEDED_UPDATES 
      SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, irdb_value_group_open_mmc));
#endif
      SOC_EXIT;
    } else { 
      arad_mcdb_entry_t *entry2 = MCDS_GET_MCDB_ENTRY(mcds, next_index);
      DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(entry2) == DPP_MCDS_TYPE_VALUE_INGRESS);
      found_entry->word0 = entry2->word0;
      found_entry->word1 &= ~mcds->msb_word_mask;
      found_entry->word1 |= (entry2->word1 & mcds->msb_word_mask);
      freed_index = next_index;
      next_index = soc_mem_field32_get(unit, IRR_MCDBm, entry2, LINK_PTRf); 
      SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, found_index)); 
    }

  } else { 

    DPP_MC_ASSERT(found_size > 2  ? (prev_index != multicast_id_ndx) : (found_size == 2 && prev_index == multicast_id_ndx));
    
    SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
      mcds, unit, prev_index, DPP_MCDS_TYPE_VALUE_INGRESS, next_index));
    freed_index = found_index;
    --found_size; 

  }

  if (next_index != MCDS_INGRESS_LINK_END(mcds)) {
    DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_index) == DPP_MCDS_TYPE_VALUE_INGRESS);
    DPP_MCDS_SET_PREV_ENTRY(mcds, next_index, prev_index); 
    
    for (; found_size <= mcds->max_nof_mmc_replications && next_index != MCDS_INGRESS_LINK_END(mcds); ++found_size) {
      next_index = soc_mem_field32_get(unit, IRR_MCDBm, MCDS_GET_MCDB_ENTRY(mcds, next_index), LINK_PTRf);
    }
  }

  if (found_size == mcds->max_nof_mmc_replications) { 
    SOCDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id_ndx, irdb_value_group_open_mmc));
  }

  SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks( 
    unit, mcds, freed_index, freed_index, dpp_mcds_get_region(mcds, freed_index), McdsFreeBuildBlocksAdd_AllMustBeUsed));

  

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_ing_group_size_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx,
    SOC_SAND_OUT uint32      *mc_group_size
)
{
  uint8 is_open;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mc_group_size);

  SOCDNX_IF_ERR_EXIT(dpp_mult_ing_get_group(
    unit, multicast_id_ndx, 0, 0, 0, NULL, mc_group_size, &is_open));

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_multicast_set_entry(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx, 
    SOC_SAND_IN  uint32               outlif,           
    SOC_SAND_IN  uint32               destination,      
    SOC_SAND_IN  uint32               next_entry,       
    SOC_SAND_IN  uint32               prev_entry        
)

{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, OUT_LIFf, outlif);
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, DESTINATIONf, destination);
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf, next_entry);
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_INGRESS_START : DPP_MCDS_TYPE_VALUE_INGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_ing_mc_group_entry_to_mcdb_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_INOUT dpp_mcdb_entry_t       *mcdb_entry, 
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,  
    SOC_SAND_IN    uint32                 next_entry   
)
{
  SOCDNX_INIT_FUNC_DEFS;

  
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, OUT_LIFf, ing_entry->cud);
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, DESTINATIONf, ing_entry->destination.id);
  soc_mem_field32_set(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf, next_entry);

  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_IN  uint32                 next_entry,       
    SOC_SAND_IN  uint32                 prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(ing_entry);

  
  SOCDNX_IF_ERR_EXIT(dpp_ing_mc_group_entry_to_mcdb_entry(unit, mcdb_entry, ing_entry, next_entry));

  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_INGRESS_START : DPP_MCDS_TYPE_VALUE_INGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_ing_traffic_class_map_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_TMC_MULT_ING_TR_CLS_MAP *map
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported for the device")));
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_ing_traffic_class_map_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *map
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported for the device")));
exit:
  SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

