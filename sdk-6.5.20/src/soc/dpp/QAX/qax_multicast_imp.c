/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST



#include <soc/dpp/drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#include <soc/dcmn/error.h>
#include <soc/sand/sand_mem.h>
#include <shared/swstate/access/sw_state_access.h>







#define QAX_MC_FREE_ENTRY_0 0
#define QAX_MC_FREE_ENTRY_1 0
#define QAX_MC_FREE_ENTRY_2 0


#define JER_MC_ING_EMPTY_ENTRY_0 0x00000000
#define JER_MC_ING_EMPTY_ENTRY_1 0xfffffff0
#define JER_MC_ING_EMPTY_ENTRY_2 0x0000a47f


#define QAX_MC_ING_DESTINATION_ID_BITS 15
#define QAX_MC_ING_DESTINATION_Q_TYPE 0
#define QAX_MC_ING_DESTINATION_SYSPORT_TYPE 2
#define QAX_MC_ING_DESTINATION_SYSPORT_LAG_TYPE 3
#define QAX_MC_ING_DESTINATION_SYSPORT_MASK ((1 << QAX_MC_ING_DESTINATION_ID_BITS) - 1)
#define QAX_MC_ING_DESTINATION_ID_MASK_TM_FLOW 0x1ffff

#define QAX_MC_MCIDS_PER_IS_ING_MC_ENTRY 32

#define FDT_IPT_MESH_MC_ENTRY_SIZE 5
#define EGR_PER_CORE_REP_MAX_ENTRY_SIZE FDT_IPT_MESH_MC_ENTRY_SIZE
#define FDT_IPT_MESH_MC_BITS_PER_GROUP 4
#define FDT_IPT_MESH_MC_GROUPS_PER_ENTRY 32
#define FDT_IPT_MESH_MC_OFFSET_IN_GROUP_BITS 2





uint32 qax_mcds_unoccupied_get(
    SOC_SAND_IN qax_mcds_t *mcds
)
{
  return mcds->nof_unoccupied;
}

STATIC void
  qax_mcds_unoccupied_increase(
    SOC_SAND_INOUT qax_mcds_t *mcds,
    SOC_SAND_IN    uint32               delta
)
{
  mcds->nof_unoccupied += delta;
  DPP_MC_ASSERT(mcds->nof_unoccupied <= QAX_LAST_MCDB_ENTRY(mcds->unit));
}

STATIC void
  qax_mcds_unoccupied_decrease(
    SOC_SAND_INOUT qax_mcds_t *mcds,
    SOC_SAND_IN    uint32               delta
)
{
  DPP_MC_ASSERT(mcds->nof_unoccupied >= delta);
  mcds->nof_unoccupied -= delta;
}

STATIC uint32 
qax_self_replication_set (SOC_SAND_IN  int unit,       
                          SOC_SAND_IN  uint32 group_mcid,
                          SOC_SAND_IN  int on_off){

    int index, offset; 
    uint32 *word_to_change, orig_word; 
    uint32 groups_per_entry, bits_per_group, offset_in_group_bits;
    soc_mem_t table;
    uint32 data[EGR_PER_CORE_REP_MAX_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;
    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOC_EXIT;
    }

    if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_CORE_MESH_MODE) == 0) {
        SOC_EXIT;
    }

    groups_per_entry = FDT_IPT_MESH_MC_GROUPS_PER_ENTRY;
    bits_per_group = FDT_IPT_MESH_MC_BITS_PER_GROUP;
    offset_in_group_bits = FDT_IPT_MESH_MC_OFFSET_IN_GROUP_BITS;
    table = FDT_IPT_MESH_MCm;

    index = group_mcid / groups_per_entry;
    offset = bits_per_group * (group_mcid % groups_per_entry) + offset_in_group_bits;
    word_to_change = data + offset / SOC_SAND_NOF_BITS_IN_UINT32;

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, table, MEM_BLOCK_ANY, index, data));
    orig_word = *word_to_change;

    if (on_off) {
        *word_to_change |= 1 << (offset % SOC_SAND_NOF_BITS_IN_UINT32);
    }
    else{
        *word_to_change &= ~(1 << (offset % SOC_SAND_NOF_BITS_IN_UINT32));
    }

    if (*word_to_change != orig_word) { 
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, table, MEM_BLOCK_ALL, index, data));
    }
	
exit:
    SOCDNX_FUNC_RETURN;
}





dpp_free_entries_blocks_region_t*
  qax_mcds_get_region_and_consec_range(qax_mcds_t *mcds, uint32 mcdb_index, uint32 *range_start, uint32 *range_end)
{
    dpp_free_entries_blocks_region_t *range = qax_mcds_get_region(mcds, mcdb_index);
    *range_start = range->range_start;
    *range_end = range->range_end;
    
    DPP_MC_ASSERT(*range_start <= *range_end && mcdb_index >= *range_start && mcdb_index <= *range_end);
    return range;
}


uint32 qax_mcdb_copy_write(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 src_index, 
    SOC_SAND_IN uint32 dst_index  
)
{
  uint32 data[QAX_MC_ENTRY_SIZE];
  qax_mcds_t* mcds = dpp_get_mcds(unit);
  qax_mcdb_entry_t *src_entry = QAX_GET_MCDB_ENTRY(mcds, src_index);
  qax_mcdb_entry_t *dst_entry = QAX_GET_MCDB_ENTRY(mcds, dst_index);
  SOCDNX_INIT_FUNC_DEFS;

  data[0] = dst_entry->word0 = src_entry->word0;
  data[1] = dst_entry->word1 = src_entry->word1;
  dst_entry->word2 &= QAX_MCDS_LAST_WORD_KEEP_BITS_MASK;
  dst_entry->word2 |= src_entry->word2 & ~QAX_MCDS_LAST_WORD_KEEP_BITS_MASK;
  data[2] = src_entry->word2 & QAX_MC_ENTRY_MASK_VAL;
  SOCDNX_IF_ERR_EXIT(WRITE_TAR_MCDBm(unit, MEM_BLOCK_ANY, dst_index, data));

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC INLINE void qax_mcdb_entry_clear_mcdb_bits(qax_mcdb_entry_t *mcdb_entry)
{
    mcdb_entry->word1 = mcdb_entry->word0 = 0;
    mcdb_entry->word2 &= ~QAX_MC_ENTRY_MASK_VAL;
}



STATIC void _qax_mcds_remove_free_entries_block_from_list(qax_mcds_t *mcds, dpp_free_entries_block_list_t *list, uint32 block, const dpp_free_entries_block_size_t block_size)
{
  uint32 next, prev;

  DPP_MC_ASSERT(block <= QAX_LAST_MCDB_ENTRY(mcds->unit) && block > 0);
  DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
  DPP_MC_ASSERT(block_size > 0 && block_size == QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, block));
  DPP_MC_ASSERT(list == qax_mcds_get_region(mcds, block)->lists + (block_size-1));
  next = QAX_MCDS_GET_FREE_NEXT_ENTRY(mcds, block);
  prev = QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, block);
  if (next == block) { 
    DPP_MC_ASSERT(prev == block && list->first == block);
    list->first = DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY; 
  } else { 
    DPP_MC_ASSERT(prev != block && 
      QAX_MCDS_GET_TYPE(mcds, prev) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
      QAX_MCDS_GET_TYPE(mcds, next) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    DPP_MC_ASSERT(QAX_MCDS_GET_FREE_NEXT_ENTRY(mcds, prev) == block &&
                QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, next) == block);
    QAX_MCDS_SET_FREE_NEXT_ENTRY(mcds, prev, next);
    QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, next, prev);
    if (list->first == block) {
      list->first = next; 
    }
  }
  LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
              (BSL_META("removed(%u,%u) "), block, block_size));
  qax_mcds_unoccupied_decrease(mcds, block_size); 
}


STATIC INLINE void
  qax_mcds_remove_free_entries_block_from_list(qax_mcds_t *mcds, dpp_free_entries_block_list_t *list, uint32 block)
{
  _qax_mcds_remove_free_entries_block_from_list(mcds, list, block, QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, block));
}


dpp_free_entries_blocks_region_t* qax_mcds_get_region(qax_mcds_t *mcds, uint32 mcdb_index)
{
  DPP_MC_ASSERT(mcds && mcdb_index > 0 && mcdb_index <= QAX_LAST_MCDB_ENTRY(mcds->unit));

  if (mcdb_index >= mcds->ingress_starts.range_start && mcdb_index <= mcds->ingress_starts.range_end) {
    return &mcds->ingress_starts;
  }
  if (mcdb_index >= mcds->egress_starts.range_start && mcdb_index <= mcds->egress_starts.range_end) {
    return &mcds->egress_starts;
  }
  return &mcds->no_starts;
}


STATIC INLINE void
  qax_mcds_remove_free_entries_block_from_region(qax_mcds_t *mcds, dpp_free_entries_blocks_region_t *region, uint32 block, dpp_free_entries_block_size_t block_size)
{
  dpp_free_entries_block_list_t *list = region->lists + (block_size-1);

  DPP_MC_ASSERT(block_size <= region->max_size);
  _qax_mcds_remove_free_entries_block_from_list(mcds, list, block, block_size);
}


STATIC INLINE int
  qax_mcds_is_empty_free_entries_block_list(const qax_mcds_t *mcds, const dpp_free_entries_block_list_t *list)
{
  if (list->first == DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY)
    return 1;
  DPP_MC_ASSERT(list->first <= QAX_LAST_MCDB_ENTRY(mcds->unit));
  return 0;
}


STATIC void
  qax_mcds_add_free_entries_block_to_list(qax_mcds_t *mcds, dpp_free_entries_block_list_t *list, uint32 block)
{
  uint32 next, prev;
  dpp_free_entries_block_size_t block_size = QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, block);

  if (qax_mcds_is_empty_free_entries_block_list(mcds, list)) {
    list->first = prev = next = block;
  } else {
    next = list->first;
    prev = QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, next);
    DPP_MC_ASSERT(QAX_MCDS_GET_FREE_NEXT_ENTRY(mcds, prev) == next &&
      QAX_MCDS_GET_TYPE(mcds, next) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, next, block);
    QAX_MCDS_SET_FREE_NEXT_ENTRY(mcds, prev, block);
  }
  QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, block, prev);
  QAX_MCDS_SET_FREE_NEXT_ENTRY(mcds, block, next);
  qax_mcds_unoccupied_increase(mcds, block_size);
} 


uint32 qax_mcds_get_free_entries_block_from_list(qax_mcds_t *mcds, dpp_free_entries_block_list_t *list, int to_remove)
{
  uint32 block = list->first;
  if (block == DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY)
    return 0;
  DPP_MC_ASSERT(block <= QAX_LAST_MCDB_ENTRY(mcds->unit));
  DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);

  if (to_remove) {
    qax_mcds_remove_free_entries_block_from_list(mcds, list, block);
  }
  return block;
}



#define QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV  1 
#define QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT  2 
#define QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE          3 
STATIC uint32 qax_mcds_create_free_entries_block(
    SOC_SAND_INOUT qax_mcds_t                 *mcds,
    SOC_SAND_IN    uint32                           flags,             
    SOC_SAND_IN    uint32                           block_start_index, 
    SOC_SAND_IN    dpp_free_entries_block_size_t    block_size,        
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region            
)
{
  int unit = mcds->unit;
  uint32 i, current_block_start_index = block_start_index;
  uint32 block_end = block_start_index + block_size; 
  dpp_free_entries_block_size_t current_block_size = block_size, joint_block_size;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(region);
  if (block_start_index + block_size > mcds->ingress_bitmap_start || !block_start_index) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("block out of range")));
  }
  if (block_size > region->max_size || block_size < 1) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid block size")));
  }

  
  for (i = block_start_index; i < block_end; ++i) {
    if (DPP_MCDS_TYPE_IS_USED(QAX_MCDS_GET_TYPE(mcds, i))) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("attempted to add a used entry number %u to a free block"), i));
    }
  }

  
  if (block_size < region->max_size) {
    const uint32 next_block = block_start_index + block_size;
    uint32 prev_block = block_start_index - 1;
    dpp_free_entries_block_size_t prev_block_size = 0, next_block_size = 0;
    
    if (!(flags & QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV) && 
        prev_block >= region->range_start && prev_block <= region->range_end &&
        region == qax_mcds_get_region(mcds, prev_block) &&
        DPP_MCDS_TYPE_IS_FREE(i = QAX_MCDS_GET_TYPE(mcds, prev_block))) {
      prev_block_size = 1;
      if (i != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) { 
        prev_block = QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, prev_block);
        prev_block_size = block_start_index - prev_block;
        DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, prev_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
          prev_block < block_start_index - 1 && prev_block_size <= region->max_size);
      }
      DPP_MC_ASSERT(prev_block_size == QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, prev_block));
      if (prev_block_size == region->max_size) {
        prev_block_size = 0; 
      }
    }
 
    if (!(flags & QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT) && 
        next_block >= region->range_start && next_block <= region->range_end &&
        region == qax_mcds_get_region(mcds, next_block) &&
        QAX_MCDS_GET_TYPE(mcds, next_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
      next_block_size = QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, next_block);
      if (next_block_size == region->max_size) {
        next_block_size = 0; 

      } else if (prev_block_size) { 
        if (block_size + prev_block_size <= region->max_size) { 
        } else if (block_size + next_block_size <= region->max_size) { 
          prev_block_size = 0;
        } else if (prev_block_size > next_block_size) { 
          prev_block_size = 0;
        }
      }
    }

    if (prev_block_size) { 

      joint_block_size = prev_block_size + block_size;
      if (joint_block_size > region->max_size) {
        current_block_size = joint_block_size - region->max_size;
        joint_block_size = region->max_size;
      } else {
        current_block_size = 0;
      }
      current_block_start_index = prev_block + joint_block_size;
      DPP_MC_ASSERT(joint_block_size + current_block_size == prev_block_size + block_size &&
        prev_block + joint_block_size == block_start_index + block_size - current_block_size);
      qax_mcds_remove_free_entries_block_from_region(mcds, region, prev_block, prev_block_size); 

      LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                  (BSL_META_U(unit,
                              "merge with prev free block: prev:%u,%u  freed:%u,%u\n"), prev_block, prev_block_size, block_start_index, block_size));
      QAX_MCDS_SET_FREE_BLOCK_SIZE(mcds, prev_block, joint_block_size); 
      
      for (i = block_start_index; i < current_block_start_index; ++i) {
        QAX_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
        QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, i, prev_block);
      }
      qax_mcds_add_free_entries_block_to_list(mcds, region->lists + (joint_block_size-1), prev_block); 

    } else if (next_block_size) { 

      joint_block_size = next_block_size + block_size;
      if (joint_block_size > region->max_size) {
        current_block_size = joint_block_size - region->max_size;
        joint_block_size = region->max_size;
      } else {
        current_block_size = 0;
      }
      current_block_start_index += joint_block_size;
      DPP_MC_ASSERT(joint_block_size + current_block_size == next_block_size + block_size &&
        block_start_index + joint_block_size == next_block + next_block_size - current_block_size);

      LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                  (BSL_META_U(unit,
                              "merge with next free block: next:%u,%u  freed:%u,%u\n"), next_block, next_block_size, block_start_index, block_size));
      qax_mcds_remove_free_entries_block_from_region(mcds, region, next_block, next_block_size); 
      
      QAX_MCDS_SET_FREE_BLOCK_SIZE(mcds, block_start_index, joint_block_size);  \
      QAX_MCDS_SET_TYPE(mcds, block_start_index, DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
      for (i = block_start_index + 1; i < current_block_start_index; ++i) {
        QAX_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
        QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, i, block_start_index);
      }
      qax_mcds_add_free_entries_block_to_list(mcds, region->lists + (joint_block_size-1), block_start_index); 
 
    }

  }

  if (current_block_size) {
    
    QAX_MCDS_SET_FREE_BLOCK_SIZE(mcds, current_block_start_index, current_block_size);  \
    LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                (BSL_META_U(unit,
                            "added free block: %u,%u\n"), current_block_start_index, current_block_size));
    
    QAX_MCDS_SET_TYPE(mcds, current_block_start_index, DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    block_end = current_block_start_index + current_block_size;
    for (i = current_block_start_index + 1; i < block_end; ++i) {
      QAX_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
      QAX_MCDS_SET_FREE_PREV_ENTRY(mcds, i, current_block_start_index);
    }

    
    qax_mcds_add_free_entries_block_to_list(mcds, region->lists + (current_block_size-1), current_block_start_index);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC uint32
  qax_mcds_split_free_entries_block(
    SOC_SAND_INOUT qax_mcds_t                       *mcds,       
    SOC_SAND_IN    uint32                           flags,       
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    dpp_free_entries_block_size_t    orig_size,   
    SOC_SAND_IN    dpp_free_entries_block_size_t    new_size,    
    SOC_SAND_INOUT uint32                           *block_start 
)
{
  int unit = mcds->unit;
  uint32 i;
  const uint32 next_block = *block_start + orig_size;
  uint32 prev_block = *block_start - 1;
  const dpp_free_entries_block_size_t remaining_size = orig_size - new_size;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mcds);
  SOCDNX_NULL_CHECK(block_start);
  if (orig_size > region->max_size || new_size < 1 || new_size >= orig_size) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal size parameters")));
  }

  if (!(flags & QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT) &&  
      next_block >= region->range_start && next_block <= region->range_end &&
      region == qax_mcds_get_region(mcds, next_block) &&
      QAX_MCDS_GET_TYPE(mcds, next_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
    dpp_free_entries_block_size_t merged_block_size = QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, next_block);
    dpp_free_entries_block_size_t joint_block_size = merged_block_size + remaining_size;
    if (joint_block_size <= region->max_size) { 
      DPP_MC_ASSERT(next_block - remaining_size == new_size + *block_start);
      qax_mcds_remove_free_entries_block_from_region(mcds, region, next_block, merged_block_size);
      SOCDNX_IF_ERR_EXIT(qax_mcds_create_free_entries_block( 
        mcds, QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, next_block - remaining_size, joint_block_size, region));
      goto exit;
    }
  }
  
  if (!(flags & QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV) &&  
             prev_block >= region->range_start && prev_block <= region->range_end &&
             region == qax_mcds_get_region(mcds, prev_block) &&
             DPP_MCDS_TYPE_IS_FREE(i = QAX_MCDS_GET_TYPE(mcds, prev_block))) {
    dpp_free_entries_block_size_t merged_block_size = 1, joint_block_size;
    if (i != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) { 
      prev_block = QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, prev_block);
      merged_block_size = *block_start - prev_block;
      DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, prev_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
        prev_block < *block_start - 1 && merged_block_size <= region->max_size);
    }
    DPP_MC_ASSERT(merged_block_size == QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, prev_block));
    joint_block_size = merged_block_size + remaining_size;
    if (joint_block_size <= region->max_size) { 
      DPP_MC_ASSERT(prev_block + joint_block_size == remaining_size + *block_start);
      qax_mcds_remove_free_entries_block_from_region(mcds, region, prev_block, merged_block_size);
      SOCDNX_IF_ERR_EXIT(qax_mcds_create_free_entries_block( 
        mcds, QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT, prev_block, joint_block_size, region));
      *block_start += remaining_size;
      goto exit;
    }
  }
  
  
  SOCDNX_IF_ERR_EXIT(qax_mcds_create_free_entries_block( 
    mcds, QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, *block_start + new_size, remaining_size, region));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 qax_mcds_build_free_blocks(
    SOC_SAND_IN    int                              unit,   
    SOC_SAND_INOUT qax_mcds_t                       *mcds,
    SOC_SAND_IN    uint32                           start_index, 
    SOC_SAND_IN    uint32                           end_index,   
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    mcds_free_build_option_t         entry_option 
)
{
    dpp_free_entries_block_size_t max_size, block_size;
    uint32 block_start, cur_entry;
    int check_free = entry_option == McdsFreeBuildBlocksAddOnlyFree;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(region);
    DPP_MC_ASSERT(check_free || mcds == dpp_get_mcds(unit));
    if (start_index > end_index) {
        SOC_EXIT;
    }
    if (end_index >= mcds->ingress_bitmap_start || !start_index) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("block out of range")));
    }

    max_size = region->max_size;

    for (block_start = start_index; block_start <= end_index; block_start += block_size) { 
        if (check_free) {
            block_size = 0;
            for (; block_start <= end_index && DPP_MCDS_TYPE_IS_USED(QAX_MCDS_GET_TYPE(mcds, block_start));
                ++block_start) {} 
            if (block_start <= end_index) { 
                block_size = 1;
                for (cur_entry = block_start + 1; block_size < max_size && cur_entry <= end_index && 
                  DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_GET_TYPE(mcds, cur_entry)); ++cur_entry ) {
                    ++block_size;
                }
            }
        } else { 
            block_size = block_start + max_size <= end_index ? max_size : end_index - block_start + 1;
        }
        if (block_size) { 
            DPP_MC_ASSERT(block_size <= max_size);
            if (!check_free) { 
                dpp_free_entries_block_size_t i;
                for (i = 0; i < block_size; ++i) {
                    DPP_MC_ASSERT(entry_option != McdsFreeBuildBlocksAdd_AllMustBeUsed ||
                      DPP_MCDS_TYPE_IS_USED(QAX_MCDS_GET_TYPE(mcds, block_start + i)));
                    QAX_MCDS_SET_TYPE(mcds, block_start + i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
                    
                    SOCDNX_IF_ERR_EXIT(WRITE_TAR_MCDBm(unit, MEM_BLOCK_ANY, block_start + i, &((qax_mcds_t*)mcds)->free_value));
                }
            }
            SOCDNX_IF_ERR_EXIT( 
              qax_mcds_create_free_entries_block(mcds, 0, block_start, block_size, region));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_get_mcdb_entry_type(
    SOC_SAND_IN  dpp_mcdb_entry_t* entry
)
{
    return QAX_MCDS_ENTRY_GET_TYPE((qax_mcdb_entry_t*)entry);
}

void qax_set_mcdb_entry_type(
    SOC_SAND_INOUT  dpp_mcdb_entry_t* entry,
    SOC_SAND_IN     uint32 type_value
)
{
    qax_mcdb_entry_t *e = (qax_mcdb_entry_t*)entry;
    QAX_MCDS_ENTRY_SET_TYPE(e, type_value);
}


dpp_mcdb_entry_t*
  qax_mcds_get_mcdb_entry(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mcdb_index
)
{
  return ((qax_mcds_t*)dpp_get_mcds(unit))->mcdb + mcdb_index;
}

dpp_mcdb_entry_t* qax_mcds_get_mcdb_entry_from_mcds(
    SOC_SAND_IN  dpp_mcds_t* mcds,
    SOC_SAND_IN  uint32 mcdb_index
)
{
  return ((qax_mcds_t*)mcds)->mcdb + mcdb_index;
}



uint32 qax_mcdb_get_next_pointer(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry,      
    SOC_SAND_IN  uint32  entry_type, 
    SOC_SAND_OUT uint32  *next_entry 
)
{
    qax_mcds_t* mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry = mcds->mcdb + entry;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPES_ARE_THE_SAME(entry_type, QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry)));

    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 
        if (mcdb_entry->word2 & 0x80) { 
            *next_entry = (mcdb_entry->word2 & 0x40) ? QAX_MC_INGRESS_LINK_PTR_END : entry + 1; 
        } else { 
            if (SOC_IS_QUX(unit)) {
                
                *next_entry = (mcdb_entry->word1 >> 21) | ((mcdb_entry->word2 & 0x1f) << 11); 
            } else {
                
                *next_entry = (mcdb_entry->word1 >> 21) | ((mcdb_entry->word2 & 0x3f) << 11); 
            }            
        }
    } else { 
        *next_entry = (mcdb_entry->word1 >> 20) | ((mcdb_entry->word2 & 0x1f) << 12); 
    }
    DPP_MC_ASSERT(*next_entry < mcds->ingress_bitmap_start || *next_entry == QAX_MC_INGRESS_LINK_PTR_END);

    SOCDNX_FUNC_RETURN;
}



uint32 qax_mcdb_set_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_to_set, 
    SOC_SAND_IN  uint32  entry_type,   
    SOC_SAND_IN  uint32  next_entry    
)
{
    qax_mcds_t* mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry = mcds->mcdb + entry_to_set;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPES_ARE_THE_SAME(entry_type, QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry)));

    DPP_MC_ASSERT(next_entry < mcds->ingress_bitmap_start || next_entry == QAX_MC_INGRESS_LINK_PTR_END);
    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 
        if (mcdb_entry->word2 & 0x80) { 
            mcdb_entry->word2 &= ~(uint32)0x40;
            if (next_entry == QAX_MC_INGRESS_LINK_PTR_END) {
                mcdb_entry->word2 |= 0x40;
            } else if (next_entry != entry_to_set + 1) {
                DPP_MC_ASSERT(0);
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("cannot set pointer in this entry type")));
            }
        } else { 
            mcdb_entry->word1 &= 0x1fffff; 
            mcdb_entry->word1 |= (next_entry << 21);
            if (SOC_IS_QUX(unit)) {
                mcdb_entry->word2 &= 0xffffffe0;
            } else {    
                mcdb_entry->word2 &= 0xffffffc0;
            }
            mcdb_entry->word2 |= (next_entry >> 11);
        }
    } else { 
        mcdb_entry->word1 &= 0xfffff; 
        mcdb_entry->word1 |= (next_entry << 20);
        mcdb_entry->word2 &= 0xffffffe0;
        mcdb_entry->word2 |= (next_entry >> 12);
    }

    SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, entry_to_set)); 

exit:
    SOCDNX_FUNC_RETURN;
}




uint32 qax_init_mcds(
    SOC_SAND_IN    int         unit
)
{
    qax_mcds_t *qax_mcds;
    soc_dpp_config_qax_t *qax_conf = SOC_DPP_CONFIG(unit)->qax;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_alloc_mcds(unit, sizeof(*qax_mcds), (void*)&qax_mcds));

    qax_mcds->unit = unit;
    qax_mcds->common.flags = 0;
    qax_mcds->common.get_mcdb_entry_type = qax_get_mcdb_entry_type;
    qax_mcds->common.set_mcdb_entry_type = qax_set_mcdb_entry_type;
    qax_mcds->common.get_mcdb_entry_from_mcds = qax_mcds_get_mcdb_entry_from_mcds;
    qax_mcds->common.get_next_pointer = qax_mcdb_get_next_pointer;
    qax_mcds->common.set_next_pointer = qax_mcdb_set_next_pointer;
    qax_mcds->common.ingress_link_end = QAX_MC_INGRESS_LINK_PTR_END;
    qax_mcds->free_value.word0 = QAX_MC_FREE_ENTRY_0;
    qax_mcds->free_value.word1 = QAX_MC_FREE_ENTRY_1;
    qax_mcds->free_value.word2 = QAX_MC_FREE_ENTRY_2;
    qax_mcds->empty_ingr_value.word0 = JER_MC_ING_EMPTY_ENTRY_0;
    qax_mcds->empty_ingr_value.word1 = JER_MC_ING_EMPTY_ENTRY_1;
    qax_mcds->empty_ingr_value.word2 = JER_MC_ING_EMPTY_ENTRY_2;
    
    qax_mcds->egress_mcdb_offset = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;

    qax_mcds->egress_bitmap_start = QAX_NOF_MCDB_ENTRIES - qax_conf->nof_egress_bitmaps * qax_conf->nof_egress_bitmap_bytes;
    qax_mcds->ingress_bitmap_start = qax_mcds->egress_bitmap_start - qax_conf->nof_ingress_bitmaps * qax_conf->nof_ingress_bitmap_bytes;
    DPP_MC_ASSERT(qax_mcds->egress_mcdb_offset + SOC_DPP_CONFIG(unit)->tm.nof_mc_ids <= qax_mcds->ingress_bitmap_start &&
      qax_conf->nof_ingress_bitmap_bytes <= 4 && qax_conf->nof_egress_bitmap_bytes <= 4 &&
      qax_conf->nof_ingress_bitmap_bytes && qax_conf->nof_egress_bitmap_bytes);
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_deinit_mcds(
    SOC_SAND_IN    int         unit
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_dealloc_mcds(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    qax_mcds_multicast_terminate(
        SOC_SAND_IN int unit
    )
{
    qax_mcds_t* mcds = dpp_get_mcds(unit);
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sand_free_mem(unit, (void**)&mcds->mcdb));
    SOCDNX_IF_ERR_EXIT(qax_deinit_mcds(unit));

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    qax_mcds_multicast_init(
      SOC_SAND_IN int unit
)
{
    uint32 start, end;
    qax_mcds_t *mcds = NULL;
    int failed = 1;
    uint32 table_size;
    uint8 is_allocated;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCDS not initialized")));
    }
    SOCDNX_IF_ERR_EXIT(qax_init_mcds(unit)); 
    mcds = dpp_get_mcds(unit);
    table_size = QAX_LAST_MCDB_ENTRY(unit) + 1;

    
    mcds->nof_unoccupied = 0;
    mcds->mcdb = NULL;

    
    start = SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start;
    DPP_MC_ASSERT(start <= 0);
    end   = SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end;
    if (start < 1) { 
        start = 1;
    }
    if (end >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
        end = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1;
    }
    if (end < start)  { 
        start = QAX_LAST_MCDB_ENTRY(unit) + 1;
        end = QAX_LAST_MCDB_ENTRY(unit);
    }
    dpp_mcds_init_region(&mcds->ingress_starts, DPP_MCDS_MAX_FREE_BLOCK_SIZE_ALLOCED, start, end);

    dpp_mcds_init_region(&mcds->no_starts, DPP_MCDS_MAX_FREE_BLOCK_SIZE_GENERAL, 1, mcds->ingress_bitmap_start - 1); 

    
    if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids > 0) {
        start = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, 0);
        end = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1);
        DPP_MC_ASSERT(SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end + 1 == start &&
          SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids == start && end < mcds->ingress_bitmap_start);
    } else {
        start = QAX_LAST_MCDB_ENTRY(unit) + 1;
        end = QAX_LAST_MCDB_ENTRY(unit);
    }
    dpp_mcds_init_region(&mcds->egress_starts, DPP_MCDS_MAX_FREE_BLOCK_SIZE_ALLOCED, start, end);

    assert(sizeof(qax_mcdb_entry_t) == sizeof(uint32)*QAX_MC_ENTRY_SIZE &&
           sizeof(int) >= sizeof(int32)); 

    { 
        SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &mcds->mcdb, sizeof(qax_mcdb_entry_t) * table_size, "mcds-mc-mcdb"));
        if(!SOC_WARM_BOOT(unit)) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.is_allocated(unit, &is_allocated);
            SOCDNX_IF_ERR_EXIT(rv);
            if(!is_allocated) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.alloc(unit);
                SOCDNX_IF_ERR_EXIT(rv);
            }
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.alloc_bitmap(unit, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high = -1; 
    failed = 0;
exit:
    if (failed && mcds) {
        qax_mcds_multicast_terminate(unit);
    }
    SOCDNX_FUNC_RETURN;
}


uint32
    qax_mcds_multicast_init2(
      SOC_SAND_IN int unit
)
{
    unsigned i;
    uint32 *alloced_mem = NULL; 
    uint32 *dest32;
    qax_mcdb_entry_t *dest;
    qax_mcds_t* mcds = dpp_get_mcds(unit);
    uint32 table_size, irdb_table_nof_entries, r32;
    int do_not_read = !SOC_WARM_BOOT(unit);
    int use_dma = !do_not_read &&
#ifdef PLISIM
      !SAL_BOOT_PLISIM &&
#endif
      soc_mem_dmaable(unit, TAR_MCDBm, SOC_MEM_BLOCK_ANY(unit, TAR_MCDBm)); 
    int failed = 1;
    uint32 range_start, range_end, last_end;

    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        SOC_EXIT;
    }
    if (!mcds || !mcds->mcdb) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCDS not initialized")));
    }
    table_size = QAX_LAST_MCDB_ENTRY(unit) + 1;
    irdb_table_nof_entries = (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + (QAX_NOF_GROUPS_PER_IRDB_ENTRY -1)) / QAX_NOF_GROUPS_PER_IRDB_ENTRY; 

    if (!SOC_WARM_BOOT(unit)) {
        if (!SOC_DPP_CONFIG(unit)->arad->init.pp_enable) {
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INVALID_OUTLIFr(unit, REG_PORT_ANY, DPP_MC_EGR_CUD_INVALID));
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_INVALID_OUTLIFr(unit, REG_PORT_ANY, DPP_MC_EGR_CUD_INVALID));
        }
        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_MULTICAST_OFFSET_ADDRESSr(unit, 0, &r32));
        soc_reg_field_set(unit, EGQ_MULTICAST_OFFSET_ADDRESSr, &r32, MCDB_OFFSETf, QAX_MCDS_GET_EGRESS_GROUP_START(mcds, 0));
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MULTICAST_OFFSET_ADDRESSr(unit, 0, r32));
    }

    dest = mcds->mcdb;

    
    
    if (use_dma) { 
        qax_mcdb_entry_t *src;
        
        alloced_mem = soc_cm_salloc(unit, 4 * (table_size * QAX_MC_ENTRY_SIZE), "dma-mc-buffer");
        if (alloced_mem == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate DMA buffer")));
        }
        SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, TAR_MCDBm, MEM_BLOCK_ANY, 0, QAX_LAST_MCDB_ENTRY(unit), alloced_mem));

        
        src = (void*)alloced_mem;
        for (i = table_size ; i ; --i) {
            dest->word0 = src->word0;
            dest->word1 = src->word1;
            dest++->word2 = src++->word2 & QAX_MC_ENTRY_MASK_VAL;
        }

    } else { 
        SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &alloced_mem, sizeof(uint32) * irdb_table_nof_entries * IRDB_TABLE_ENTRY_WORDS, "mcds-irdb-tmp"));

        

        if (do_not_read) { 
            for (i = QAX_LAST_MCDB_ENTRY(unit) + 1 ; i ; --i) {
                *dest = mcds->free_value;
                ++dest;
            }
        } else {
            SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, TAR_MCDBm, MEM_BLOCK_ANY, 0, QAX_LAST_MCDB_ENTRY(unit), dest));
            for (i = table_size; i; --i) {
                dest++->word2 &= QAX_MC_ENTRY_MASK_VAL;
            }
        }

    }
    if (!do_not_read && irdb_table_nof_entries > 0) {
        SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, CGM_IS_ING_MCm, MEM_BLOCK_ANY, 0, irdb_table_nof_entries - 1, alloced_mem));
    }

#ifdef BCM_WARM_BOOT_SUPPORT 
    if (SOC_WARM_BOOT(unit)) { 
      
        uint32 mcid = 0;
        unsigned j;
        uint16 group_entries;
        dest32 = alloced_mem;
        for (i = 0 ; i < irdb_table_nof_entries; ++i) {
            uint32 bits = *dest32;
            for (j = 0 ; j < QAX_NOF_GROUPS_PER_IRDB_ENTRY; ++j) {
                if (bits & 1) { 
                    
                    qax_mcdb_entry_t *mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, mcid);
                    uint32 cur_entry = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
                    uint32 prev_entry = mcid;

                    if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids || QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
                        DPP_MC_ASSERT(0); 
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("found invalid hardware table values")));
                    }
                    QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS_START); 
                    QAX_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, prev_entry);
                    group_entries = 1;
                    while (cur_entry < mcds->ingress_bitmap_start) { 
                        mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, cur_entry);
                        if (QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START || ++group_entries > QAX_MULT_MAX_INGRESS_REPLICATIONS) {
                            DPP_MC_ASSERT(0);
                            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used or too many group entries")));
                        }
                        QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS);
                        QAX_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, prev_entry);
                        
                        if (mcdb_entry->word2 & 0x80) { 
                            if (mcdb_entry->word2 & 0x40) { 
                                prev_entry = cur_entry;
                                cur_entry = QAX_MC_INGRESS_LINK_PTR_END;
                                break;
                            } else {
                                prev_entry = cur_entry;
                                cur_entry++;
                            }                            
                        } else { 
                            prev_entry = cur_entry;
                            cur_entry = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
                        }
                    }
                    DPP_MC_ASSERT(cur_entry == QAX_MC_INGRESS_LINK_PTR_END); 
                }
                bits >>= 1;
                ++mcid;
            }
            dest32 += IRDB_TABLE_ENTRY_WORDS;
        }

        
        for (mcid = 0; mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++mcid) {
            uint8 bit_val;
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, mcid, &bit_val));
            if (bit_val) { 
                
                uint32 prev_entry = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, mcid);
                qax_mcdb_entry_t *mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, prev_entry);
                uint32 cur_entry;
 
                if (QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
                    DPP_MC_ASSERT(0);
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used")));
                }
                QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START); 
                QAX_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, prev_entry);
                SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, prev_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START, &cur_entry)); 
                group_entries = 1;
                while (cur_entry != DPP_MC_EGRESS_LINK_PTR_END) { 
                    mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, cur_entry);
                    if (QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START || ++group_entries > QAX_MULT_MAX_INGRESS_REPLICATIONS) {
                        DPP_MC_ASSERT(0);
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used or too many group entries")));
                    }
                    QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS);
                    QAX_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, prev_entry);
                    prev_entry = cur_entry;
                    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, prev_entry, DPP_MCDS_TYPE_VALUE_EGRESS, &cur_entry)); 
                }
            }
        }

    } else
#endif 
    {
        
        dest32 = alloced_mem;
        for (i = 0 ; i < irdb_table_nof_entries; ++i) {
            if (*dest32) {
                DPP_MC_ASSERT(0);
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Illegal TAR_MCDB content")));
            }
        dest32 += IRDB_TABLE_ENTRY_WORDS;
        }

    }

    

    
    range_start = mcds->ingress_starts.range_start;
    range_end = mcds->ingress_starts.range_end;
    if (range_start < mcds->ingress_bitmap_start && range_end >= range_start) { 
        DPP_MC_ASSERT(range_start >= 1);
        
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, 1, range_start - 1, &mcds->no_starts, McdsFreeBuildBlocksAddOnlyFree));
        
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, range_start, range_end, &mcds->ingress_starts, McdsFreeBuildBlocksAddOnlyFree));
        last_end = range_end;
    } else {
        last_end = 0; 
    }
    
    range_start = mcds->egress_starts.range_start;
    range_end = mcds->egress_starts.range_end;
    if (range_start < mcds->ingress_bitmap_start && range_end >= range_start) { 
        DPP_MC_ASSERT(range_start == last_end + 1 && range_end < mcds->ingress_bitmap_start);
        
        if (range_end >= mcds->ingress_bitmap_start) {
            range_end = mcds->ingress_bitmap_start - 1;
        }
        
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, range_start, range_end, &mcds->egress_starts, McdsFreeBuildBlocksAddOnlyFree));
        last_end = range_end;
    }
    
    SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, last_end + 1, mcds->ingress_bitmap_start - 1, &mcds->no_starts, McdsFreeBuildBlocksAddOnlyFree));
    failed = 0;

exit:
    if (alloced_mem) {
        if (use_dma) {
            soc_cm_sfree(unit, alloced_mem);
        } else {
            SOCDNX_IF_ERR_EXIT(sand_free_mem(unit, (void*)&alloced_mem));
        }
    }
    if (failed) {
        qax_mcds_multicast_terminate(unit);
    }
    SOCDNX_FUNC_RETURN;
}







int qax_rep_data_t_compare(void *a, void *b)
{ 
  const qax_rep_data_t *ca = b;
  const qax_rep_data_t *cb = a;
  return COMPILER_64_LT(*cb, *ca) ? 0 : 1;
}




STATIC void qax_add_ingress_replication_dest( 
  qax_mcds_t   *mcds,
  const uint32 dest,
  const uint32 cud,
  const uint32 cud2
)
{
    qax_rep_data_t *rep = mcds->reps + mcds->nof_reps;
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_INGRESS && mcds->nof_reps < QAX_MULT_MAX_INGRESS_REPLICATIONS &&
      mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps == mcds->nof_reps);
    QAX_MCDS_REP_DATA_CLEAR(rep);
    QAX_MCDS_REP_DATA_SET_DEST(rep, dest);
    QAX_MCDS_REP_DATA_SET_CUD1(rep, cud);
    QAX_MCDS_REP_DATA_SET_CUD2(rep, cud2);
    
    ++mcds->nof_reps;
    ++mcds->nof_dest_cud_reps;
}

STATIC void qax_add_ingress_replication_bitmap( 
  qax_mcds_t   *mcds,
  const uint32 bm_id,
  const uint32 cud,
  const uint32 cud2
)
{
    qax_rep_data_t *rep = mcds->reps + mcds->nof_reps;
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_INGRESS && mcds->nof_reps < QAX_MULT_MAX_INGRESS_REPLICATIONS &&
      mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps == mcds->nof_reps);
    QAX_MCDS_REP_DATA_CLEAR(rep);
    QAX_MCDS_REP_DATA_SET_DEST(rep, bm_id);
    QAX_MCDS_REP_DATA_SET_CUD1(rep, cud);
    QAX_MCDS_REP_DATA_SET_CUD2(rep, cud2);
    QAX_MCDS_REP_DATA_SET_TYPE(rep, QAX_MCDS_REP_TYPE_BM);
    ++mcds->nof_reps;
    ++mcds->nof_bitmap_reps;
}


STATIC void qax_add_egress_replication_port( 
  qax_mcds_t *mcds,
  const uint32     tm_port,
  const uint32     cud,
  const uint32     cud2
)
{
    qax_rep_data_t *rep = mcds->reps + mcds->nof_reps;
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_EGRESS && mcds->nof_reps < QAX_MULT_MAX_EGRESS_REPLICATIONS &&
      mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps == mcds->nof_reps);
    QAX_MCDS_REP_DATA_CLEAR(rep);
    QAX_MCDS_REP_DATA_SET_DEST(rep, tm_port);
    QAX_MCDS_REP_DATA_SET_CUD1(rep, cud);
    QAX_MCDS_REP_DATA_SET_CUD2(rep, cud2);
    
    ++mcds->nof_reps;
    ++mcds->nof_dest_cud_reps;
}

STATIC void qax_add_egress_replication_bitmap( 
  qax_mcds_t   *mcds,
  const uint32 bm_id,
  const uint32 cud
)
{
    qax_rep_data_t *rep = mcds->reps + mcds->nof_reps;
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_EGRESS && mcds->nof_reps < QAX_MULT_MAX_EGRESS_REPLICATIONS &&
      mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps == mcds->nof_reps);
    QAX_MCDS_REP_DATA_CLEAR(rep);
    QAX_MCDS_REP_DATA_SET_DEST(rep, bm_id);
    QAX_MCDS_REP_DATA_SET_CUD1(rep, cud);
    
    QAX_MCDS_REP_DATA_SET_TYPE(rep, QAX_MCDS_REP_TYPE_BM);
    ++mcds->nof_reps;
    ++mcds->nof_bitmap_reps;
}



uint32 qax_mcds_get_replications_from_entry(
    SOC_SAND_IN    int     unit,
    SOC_SAND_IN    uint32  entry_index, 
    SOC_SAND_IN    uint32  entry_type,  
    SOC_SAND_INOUT uint16  *max_size,   
    SOC_SAND_INOUT uint16  *group_size, 
    SOC_SAND_OUT   uint32  *next_entry  
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry = mcds->mcdb + entry_index;
    uint16 found = 0, max = *max_size;
    uint32 format, dest, cud1, cud2, temp;
    soc_mem_t mcdb_bmp_repl_mem;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(entry_type == QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry)); 
    mcdb_bmp_repl_mem = TAR_MCDB_BITMAP_REPLICATION_ENTRYm;
    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 

        format = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, FORMATf); 
        cud1 = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_0f); 
        cud2 = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_1f); 
        switch (format) { 
          case 0:
            temp = soc_mem_field32_get(unit, mcdb_bmp_repl_mem, mcdb_entry, BMP_PTRf);
            dest = (temp - mcds->ingress_bitmap_start) / SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes;
            DPP_MC_ASSERT(temp >= mcds->ingress_bitmap_start && temp < mcds->egress_bitmap_start &&
              temp == mcds->ingress_bitmap_start + dest * SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes);
            if (++found <= max) { 
                qax_add_ingress_replication_bitmap(mcds, dest, cud1, cud2);
            }
            *next_entry = soc_mem_field32_get(unit, mcdb_bmp_repl_mem, mcdb_entry, LINK_PTRf);
            break;

          case 1:
            dest = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, DESTINATIONf);
            if (dest != QAX_MC_ING_DESTINATION_DISABLED && ++found <= max) { 
                qax_add_ingress_replication_dest(mcds, dest, cud1, cud2);
            }
            *next_entry = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
            break;

          default:
            
            if (++found <= max) {
                qax_add_ingress_replication_dest(mcds, soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f), cud1, DPP_MC_NO_2ND_CUD);
            }
            if (++found <= max) {
                qax_add_ingress_replication_dest(mcds, soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f), cud2, DPP_MC_NO_2ND_CUD);
            }
            *next_entry = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, LASTf) ? QAX_MC_INGRESS_LINK_PTR_END : entry_index + 1;
        } 

    } else if (DPP_MCDS_TYPE_IS_EGRESS_NORMAL(entry_type)) { 
        format = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, ENTRY_FORMATf); 
        cud1 = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, OUTLIFf); 
        *next_entry = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, LINK_PTRf); 
        switch (format) { 
          case 0:

            temp = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, BMP_PTRf);
            if (temp != DPP_MC_EGR_BITMAP_DISABLED && ++found <= max) { 
            dest = (temp - mcds->egress_bitmap_start) / SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes;
            DPP_MC_ASSERT(temp >= mcds->egress_bitmap_start && temp < QAX_NOF_MCDB_ENTRIES && temp == mcds->egress_bitmap_start + dest * SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes);
                qax_add_egress_replication_bitmap(mcds, dest, cud1);
            }            
            break;

          case 1:
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Af);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, DPP_MC_NO_2ND_CUD);
            }
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Bf);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, soc_mem_field32_get(unit,
                  TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, OUTLIF_Bf), DPP_MC_NO_2ND_CUD);
            }
            break;

          case 2:
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_Bf);
            cud2 = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, OUTLIF_2f);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, cud2);
            }
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_Af);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, cud2);
            }
            break;

          case 5:
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, DPP_MC_NO_2ND_CUD);
            }
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, DPP_MC_NO_2ND_CUD);
            }
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, DPP_MC_NO_2ND_CUD);
            }
            dest = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f);
            if (dest != DPP_MULT_EGRESS_PORT_INVALID && ++found <= max) { 
                qax_add_egress_replication_port(mcds, dest, cud1, DPP_MC_NO_2ND_CUD);
            }
            break;

          default:
            DPP_MC_ASSERT(0);
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported egress format found")));
        } 
    }

    if (found < max) {
        *max_size -= found;
    } else {
        *max_size = 0;
    }
    *group_size += found;

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 qax_convert_ingress_destination_hw2api(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       dest,      
    SOC_SAND_OUT soc_gport_t  *out_gport 
)
{
    uint32 type_bits = dest >> QAX_MC_ING_DESTINATION_ID_BITS;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (type_bits == QAX_MC_ING_DESTINATION_Q_TYPE) { 
        _SHR_GPORT_UNICAST_QUEUE_GROUP_SET(*out_gport, dest);
    } else if (type_bits == QAX_MC_ING_DESTINATION_SYSPORT_TYPE) { 
            uint32 fap_id, fap_port_id;
        SOCDNX_SAND_IF_ERR_EXIT(arad_sys_phys_to_local_port_map_get(unit, dest & QAX_MC_ING_DESTINATION_SYSPORT_MASK, &fap_id, &fap_port_id));
        _SHR_GPORT_MODPORT_SET(*out_gport, fap_id, fap_port_id);
    } else if (type_bits == QAX_MC_ING_DESTINATION_SYSPORT_LAG_TYPE) { 
        _SHR_GPORT_TRUNK_SET(*out_gport, dest & QAX_MC_ING_DESTINATION_SYSPORT_MASK);
    } else {
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unexpected destination type")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC void qax_mcds_clear_replications(int unit, qax_mcds_t *mcds, const uint32 group_type)
{
    mcds->group_type = DPP_MCDS_TYPE_GET_NONE_START(group_type);
    mcds->group_type_start = DPP_MCDS_TYPE_GET_START(group_type);
    mcds->nof_dest_cud_reps = mcds->nof_bitmap_reps = mcds->nof_reps = 0;
    if (mcds->group_type == DPP_MCDS_TYPE_VALUE_EGRESS) {
        mcds->alloc_flags = DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL;
        mcds->hw_end = DPP_MC_EGRESS_LINK_PTR_END;
    } else {
        mcds->alloc_flags = DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL;
        mcds->hw_end = QAX_MC_INGRESS_LINK_PTR_END;
    }
}


uint32 qax_mcds_copy_replications_to_arrays(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8        is_egress,           
    SOC_SAND_IN  uint32       arrays_size,         
    SOC_SAND_OUT soc_gport_t  *port_array,         
    SOC_SAND_OUT soc_if_t     *encap_id_array,     
    SOC_SAND_OUT soc_multicast_replication_t *reps 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 dest, rep_type, cud1, cud2;
    uint16 size;
    qax_rep_data_t *rep;
    soc_gport_t out_gport=0;
    soc_port_t local_logical_port;
    SOCDNX_INIT_FUNC_DEFS;

    
    if ((size = mcds->nof_reps) > arrays_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications to return")));
    }

    
    for (rep = mcds->reps; size; --size, ++rep) {
        QAX_MCDS_REP_DATA_GET_FIELDS(rep, dest, cud1, cud2, rep_type); 
        if ((cud1 == SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) &&
            (!is_egress)) {
            cud1 = 0;
        }
        if (rep_type == QAX_MCDS_REP_TYPE_DEST) {
            if (is_egress) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, 0, dest, &local_logical_port));
                _SHR_GPORT_LOCAL_SET(out_gport, local_logical_port); 
            } else { 
                
                SOCDNX_IF_ERR_EXIT(qax_convert_ingress_destination_hw2api(unit, dest, &out_gport));
            }
        } else { 
            _SHR_GPORT_MCAST_SET(out_gport, dest); 
        }

        if (reps) {
            reps->flags = (cud2 == DPP_MC_NO_2ND_CUD) ? 0 : SOC_MUTICAST_REPLICATION_ENCAP2_VALID;
            reps->port = out_gport;   
            reps->encap2 = cud2;
            reps->encap1 = cud1;
            reps++;
        } else {
            *(port_array++) = out_gport;
            *(encap_id_array++) = cud1;
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t qax_get_tm_port_from_gport(int unit, const uint32 port, uint32 *tm_port)
{
    int core;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, tm_port, &core));
    if (*tm_port >= DPP_MULT_EGRESS_PORT_INVALID) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid port")));
    }
    DPP_MC_ASSERT(core == 0);
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mcds_copy_egress_replications_from_arrays(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8     do_clear,        
    SOC_SAND_IN  uint32    arrays_size,     
    SOC_SAND_IN  dpp_mc_replication_t *reps 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 size_left = arrays_size, dest, cud, cud2;
    SOCDNX_INIT_FUNC_DEFS;

    if (do_clear) {
        qax_mcds_clear_replications(unit, mcds, DPP_MCDS_TYPE_VALUE_EGRESS);
    }
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_EGRESS && mcds->nof_reps == (int32)mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps);
    if (mcds->nof_reps + arrays_size > QAX_MULT_MAX_EGRESS_REPLICATIONS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }
    for (; size_left; --size_left) {
        dest = reps->dest;
        cud = reps->cud;
        cud2 = (reps++)->additional_cud;
        if (dest == _SHR_GPORT_INVALID) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("The device does not support CUD only replications")));
        } else if (dest & ARAD_MC_EGR_IS_BITMAP_BIT) { 
            if (cud2 != DPP_MC_NO_2ND_CUD) { 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("The device does not support egress bitmap replications with a 2nd CUD")));
            }
            qax_add_egress_replication_bitmap(mcds, dest & ~ARAD_MC_EGR_IS_BITMAP_BIT, cud);
        } else { 
            SOCDNX_IF_ERR_EXIT(qax_get_tm_port_from_gport(unit, dest, &dest));
            qax_add_egress_replication_port(mcds, dest, cud, cud2);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  qax_mcds_copy_replications_from_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         do_clear,    
    SOC_SAND_IN  uint32                        block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t block_size,  
    SOC_SAND_INOUT uint32                      *cud2,       
    SOC_SAND_OUT uint32                        *next_entry  
)
{
  qax_mcds_t *mcds = dpp_get_mcds(unit);
  uint32 cur_index = block_start;
  dpp_free_entries_block_size_t entries_remaining = block_size;
  uint16 nof_reps = 0, reps_left;
  SOCDNX_INIT_FUNC_DEFS;

  DPP_MC_ASSERT(block_start + block_size < mcds->ingress_bitmap_start);
  if (do_clear) {
    qax_mcds_clear_replications(unit, mcds, QAX_MCDS_ENTRY_GET_TYPE(QAX_GET_MCDB_ENTRY(mcds,cur_index)));
  } else {
    nof_reps = mcds->nof_reps;
  }
  DPP_MC_ASSERT(nof_reps == mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps &&
    nof_reps < QAX_MULT_MAX_EGRESS_REPLICATIONS && mcds->nof_reps == nof_reps);
  reps_left = QAX_MULT_MAX_EGRESS_REPLICATIONS - nof_reps;

  
  while (entries_remaining) {
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_replications_from_entry( 
      unit, cur_index, QAX_MCDS_ENTRY_GET_TYPE(QAX_GET_MCDB_ENTRY(mcds,cur_index)), &reps_left, &nof_reps, next_entry));
    ++cur_index;
    --entries_remaining;
    if (nof_reps > QAX_MULT_MAX_EGRESS_REPLICATIONS) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    } else if (entries_remaining && *next_entry == DPP_MC_EGRESS_LINK_PTR_END) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("block too small")));
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 qax_mcds_reserve_group_start(
    SOC_SAND_INOUT qax_mcds_t *mcds,
    SOC_SAND_IN    uint32           mcdb_index 
)
{
    int unit = mcds->unit;
    uint32 entry_type;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(mcdb_index < mcds->ingress_bitmap_start);

    entry_type = QAX_MCDS_GET_TYPE(mcds, mcdb_index);
    if (DPP_MCDS_TYPE_IS_USED(entry_type)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("entry must be free")));
    }
    if (mcdb_index > 0 ) { 
        dpp_free_entries_blocks_region_t* region = qax_mcds_get_region(mcds, mcdb_index);
        const uint32 block_start = entry_type == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START ?
          mcdb_index : QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, mcdb_index);
        const dpp_free_entries_block_size_t block_size = QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, block_start);
        const uint32 block_end = block_start + block_size - 1;
        DPP_MC_ASSERT(block_start <= mcdb_index && block_start + region->max_size >= mcdb_index && block_size <= region->max_size);

        qax_mcds_remove_free_entries_block_from_region(mcds, region, block_start, block_size); 
        if (block_start < mcdb_index) { 
            SOCDNX_IF_ERR_EXIT(qax_mcds_create_free_entries_block(
              mcds, QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT, block_start, mcdb_index - block_start, region));
        }
        if (block_end > mcdb_index) { 
            SOCDNX_IF_ERR_EXIT(qax_mcds_create_free_entries_block(
              mcds, QAX_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, mcdb_index + 1, block_end - mcdb_index, region));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mcds_get_free_entries_block(
    SOC_SAND_INOUT qax_mcds_t                    *mcds,
    SOC_SAND_IN    uint32                        flags,        
    SOC_SAND_IN    dpp_free_entries_block_size_t wanted_size,  
    SOC_SAND_IN    dpp_free_entries_block_size_t max_size,     
    SOC_SAND_OUT   uint32                        *block_start, 
    SOC_SAND_OUT   dpp_free_entries_block_size_t *block_size   
)
{
    int unit = mcds->unit;
    dpp_free_entries_blocks_region_t *regions[ARAD_MCDS_NOF_REGIONS];
    int do_change = !(flags & DPP_MCDS_GET_FREE_BLOCKS_NO_UPDATES);
    uint32 block = 0;
    int r, s, loop_start, loop_end;
    int size_loop1_start = wanted_size, size_loop1_increase;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mcds);
    SOCDNX_NULL_CHECK(block_start);
    SOCDNX_NULL_CHECK(block_size);
    if (wanted_size > DPP_MCDS_MAX_FREE_BLOCK_SIZE || wanted_size > max_size || 1 > wanted_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal wanted size")));
    }

    regions[0] = &mcds->no_starts;
    if (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS) { 
        regions[1] = &mcds->ingress_starts;
        regions[2] = &mcds->egress_starts;
    } else {
        regions[1] = &mcds->egress_starts;
        regions[2] = &mcds->ingress_starts;
    }

    
    size_loop1_increase = (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_SMALL) ? -1 : 1;

    if (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE) { 

        
        loop_start = size_loop1_start;
        if (size_loop1_increase >= 0) { 
            loop_end = DPP_MCDS_MAX_FREE_BLOCK_SIZE + 1;
        } else { 
            loop_end = 0;
        }
        for (s = loop_start; s != loop_end; s += size_loop1_increase) {
            for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
                dpp_free_entries_blocks_region_t *region = regions[r];
                if (region->max_size >= s) { 
                    if ((block = qax_mcds_get_free_entries_block_from_list(mcds, region->lists + (s - 1), do_change))) {
                        goto found;
                    }
                }
            }
        }
        
        loop_start = size_loop1_start - size_loop1_increase;
        if (size_loop1_increase <= 0) { 
            loop_end = DPP_MCDS_MAX_FREE_BLOCK_SIZE + 1;
        } else { 
            loop_end = 0;
        }
        for (s = loop_start; s != loop_end; s -= size_loop1_increase) {
            for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
                dpp_free_entries_blocks_region_t *region = regions[r];
                if (region->max_size >= s) { 
                    if ((block = qax_mcds_get_free_entries_block_from_list(mcds, region->lists + (s - 1), do_change))) {
                        goto found;
                    }
                }
            }
        }

    } else { 

        for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
            dpp_free_entries_blocks_region_t *region = regions[r];
            
            dpp_free_entries_block_list_t *lists = region->lists - 1;

            
            loop_start = size_loop1_start;
            if (size_loop1_increase >= 0) { 
                loop_end = region->max_size + 1;
                if (loop_start > loop_end) loop_start = loop_end;
            } else { 
                loop_end = 0;
                if (loop_start > region->max_size) loop_start = region->max_size;
            }
            for (s = loop_start; s != loop_end; s += size_loop1_increase) {
             if ((block = qax_mcds_get_free_entries_block_from_list(mcds, lists + s, do_change))) {
                 goto found;
             }
            }
            
            loop_start = size_loop1_start - size_loop1_increase;
            if (size_loop1_increase <= 0) { 
                loop_end = region->max_size + 1;
                if (loop_start > loop_end) loop_start = loop_end;
            } else { 
                loop_end = 0;
                if (loop_start > region->max_size) loop_start = region->max_size;
            }
            for (s = loop_start; s != loop_end; s -= size_loop1_increase) {
             if ((block = qax_mcds_get_free_entries_block_from_list(mcds, lists + s, do_change))) {
                 goto found;
             }
            }

        } 

    } 

    DPP_MC_ASSERT(!mcds->nof_unoccupied);
    if (flags & DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL) {
        *block_start = 0;
        *block_size = 0;
        SOC_EXIT;
    }
    SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("did not find any free block")));

found:
    if (do_change && s > max_size) {
        DPP_MC_ASSERT(s <= DPP_MCDS_MAX_FREE_BLOCK_SIZE);
        SOCDNX_IF_ERR_EXIT( 
            qax_mcds_split_free_entries_block(mcds, flags, regions[r], s, max_size, &block));
        s = max_size;
    }

    *block_start = block;
    *block_size = s;
    DPP_MC_ASSERT(block && s);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC INLINE int qax_mcds_is_consecutive_format(
    SOC_SAND_IN qax_mcds_t *mcds,
    SOC_SAND_IN uint32     mcdb_index
)
{
    
    
    return ((QAX_GET_MCDB_ENTRY(mcds, mcdb_index)->word2 >> 6) & 3) == 2;
}


uint32
  qax_mcds_get_relocation_block(
    SOC_SAND_IN  qax_mcds_t                    *mcds,
    SOC_SAND_IN  uint32                        mcdb_index,              
    SOC_SAND_OUT uint32                        *relocation_block_start, 
    SOC_SAND_OUT dpp_free_entries_block_size_t *relocation_block_size   
)
{
    int unit = mcds->unit;
    uint32 group_entry_type, start = mcdb_index;
    dpp_free_entries_block_size_t size = 1;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mcds);
    SOCDNX_NULL_CHECK(relocation_block_size);

    group_entry_type = QAX_MCDS_GET_TYPE(mcds, mcdb_index);
    
    if (DPP_MCDS_TYPE_IS_USED(group_entry_type) && !DPP_MCDS_TYPE_IS_START(group_entry_type)) {
        if (group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS) { 

            uint32 entry, next_entry;
            DPP_MC_ASSERT(group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS);
            for (entry = mcdb_index; ; entry = next_entry) { 
                if ((next_entry = QAX_MCDS_GET_PREV_ENTRY(mcds, entry)) + 1 != entry ||
                  !qax_mcds_is_consecutive_format(mcds, next_entry)) { 
                    break;
                }
                ++size;
                DPP_MC_ASSERT(next_entry && size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE &&
                  QAX_MCDS_GET_TYPE(mcds, next_entry) == DPP_MCDS_TYPE_VALUE_INGRESS); 
            }
            start = entry;

            
            for (entry = mcdb_index; qax_mcds_is_consecutive_format(mcds, entry); entry = next_entry) {
                next_entry = entry + 1;
                ++size;
                DPP_MC_ASSERT(QAX_MCDS_GET_PREV_ENTRY(mcds,next_entry) == entry && next_entry < mcds->ingress_bitmap_start &&
                  next_entry && size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE &&
                  QAX_MCDS_GET_TYPE(mcds, next_entry) == DPP_MCDS_TYPE_VALUE_INGRESS); 
            }
            DPP_MC_ASSERT(entry - start + 1 == size);

        }
    } else { 
        DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_FREE(group_entry_type) || (mcdb_index < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids ? 
          group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS_START : (DPP_MCDS_TYPE_IS_EGRESS_START(group_entry_type) &&
          mcdb_index < QAX_MCDS_GET_EGRESS_GROUP_START(mcds, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids))));
    }

    *relocation_block_size = size;
    if (relocation_block_start) {
        *relocation_block_start = start;
    }

exit:
    SOCDNX_FUNC_RETURN;
}





uint32
  qax_mcds_write_entry(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mcdb_index 
)
{
  uint32 data[QAX_MC_ENTRY_SIZE];
  qax_mcds_t *mcds = dpp_get_mcds(unit);
  qax_mcdb_entry_t *entry = QAX_GET_MCDB_ENTRY(mcds, mcdb_index);

  SOCDNX_INIT_FUNC_DEFS;

  data[0] = entry->word0;
  data[1] = entry->word1;
  data[2] = entry->word2 & QAX_MC_ENTRY_MASK_VAL;
  SOCDNX_IF_ERR_EXIT(WRITE_TAR_MCDBm(unit, MEM_BLOCK_ANY, mcdb_index, data));

exit:
  SOCDNX_FUNC_RETURN;
}

int qax_mcds_test_free_entries(
    SOC_SAND_IN int unit
)
{
  uint32 nof_unoccupied = 0;
  qax_mcds_t* mcds = dpp_get_mcds(unit);
  qax_mcdb_entry_t *entry, *entry2;
  ARAD_MULT_ID mcid;
  int32 nof_egr_bitmap_entries = (SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmaps * SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes);

  
  for (mcid = 0; mcid <= QAX_LAST_MCDB_ENTRY(unit); ++mcid) {
    entry = QAX_GET_MCDB_ENTRY(mcds, mcid);
    if (DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_ENTRY_GET_TYPE(entry))) {
      QAX_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
      ++nof_unoccupied;
    } else {
      QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
    }
  }
  
  if (DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_ENTRY_GET_TYPE(entry =
      QAX_GET_MCDB_ENTRY(mcds, DPP_MC_EGRESS_LINK_PTR_END)))) {
    DPP_MC_ASSERT(nof_unoccupied);
    QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
    --nof_unoccupied;
  }
  if (nof_unoccupied - nof_egr_bitmap_entries != mcds->nof_unoccupied) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST,
             (BSL_META_U(unit,
                         "The mcdb has %lu free allocatable entries and in the mcds the value is %lu\n"), (unsigned long)nof_unoccupied, (unsigned long)mcds->nof_unoccupied));
    DPP_MC_ASSERT(0);
    return 10;
  }

  
  nof_unoccupied = 0;
  {
      dpp_free_entries_blocks_region_t *regions[ARAD_MCDS_NOF_REGIONS];
    int r;
    dpp_free_entries_block_size_t size, size_i;
    uint32 block, first_block, prev_block;
    regions[0] = &mcds->no_starts;
    regions[1] = &mcds->ingress_starts;
    regions[2] = &mcds->egress_starts;

    
    for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) {
      dpp_free_entries_blocks_region_t *region = regions[r];
      dpp_free_entries_block_list_t *lists = region->lists;
      DPP_MC_ASSERT(region->max_size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE && region->max_size > 0);

      for (size = region->max_size; size; --size) { 
        
        if ((block = qax_mcds_get_free_entries_block_from_list(mcds, lists + size - 1, 0))) { 
          prev_block = QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, block);
          first_block = block;

          
          do {
            entry = QAX_GET_MCDB_ENTRY(mcds, block);
            DPP_MC_ASSERT(block >= region->range_start && block + size - 1 <= region->range_end);
            DPP_MC_ASSERT(QAX_MCDS_ENTRY_GET_TYPE(entry) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
            DPP_MC_ASSERT(QAX_MCDS_GET_FREE_BLOCK_SIZE(mcds, block) == size);
            DPP_MC_ASSERT(prev_block == QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, block));
            if (!QAX_MCDS_ENTRY_GET_TEST_BIT(entry)) {
              LOG_ERROR(BSL_LS_SOC_MULTICAST,
                       (BSL_META_U(unit,
                                   "Free block %lu of size %u appeared previously in a linked list\n"), (unsigned long)block, size));
              DPP_MC_ASSERT(0);
              return 20;
            }
            QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
            entry2 = entry;

            for (size_i = 1; size_i < size;  ++ size_i) { 
              ++entry2;
              DPP_MC_ASSERT(QAX_MCDS_ENTRY_GET_TYPE(entry2) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
              DPP_MC_ASSERT(QAX_MCDS_ENTRY_GET_FREE_PREV_ENTRY(entry2) == block);
              if (!QAX_MCDS_ENTRY_GET_TEST_BIT(entry2)) {
                LOG_ERROR(BSL_LS_SOC_MULTICAST,
                         (BSL_META_U(unit,
                                     "Free entry %lu of free block %lu of size %u appeared previously in a linked list\n"),
                                     (unsigned long)(block + size ), (unsigned long)block, size));
                DPP_MC_ASSERT(0);
                return 30;
              }
            QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry2);
            }
            nof_unoccupied += size;
            prev_block = block;
            block = QAX_MCDS_GET_FREE_NEXT_ENTRY(mcds, block); 
          } while (block != first_block);
          DPP_MC_ASSERT(prev_block == QAX_MCDS_GET_FREE_PREV_ENTRY(mcds, block));
        }
      }
    }
  }
  if (nof_unoccupied != mcds->nof_unoccupied) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST,
             (BSL_META_U(unit,
                         "The mcdb free block lists contain %lu entries and in the mcds the value is %lu\n"), (unsigned long)nof_unoccupied, (unsigned long)mcds->nof_unoccupied));
    DPP_MC_ASSERT(0);
    return 40;
  }

  return 0;
}





uint32 qax_mult_does_group_exist(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,       
    SOC_SAND_IN  int             is_egress,  
    SOC_SAND_OUT uint8           *does_exist 
)
{
    const qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;

    SOCDNX_INIT_FUNC_DEFS;

    *does_exist = FALSE;
    if (mcid >= nof_mc_ids) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("multicast ID %u is not under the number of multicast IDs: %u"), mcid, nof_mc_ids));
    }
    if (!is_egress) { 
        if (QAX_MCDS_GET_TYPE(mcds, mcid) == DPP_MCDS_TYPE_VALUE_INGRESS_START) {
            DPP_MC_ASSERT(mcid == QAX_MCDS_GET_PREV_ENTRY(mcds, mcid) || mcid == 0); 
            *does_exist = TRUE;
        }
    } else { 
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, mcid, does_exist));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
  qax_mcdb_relocate_entries(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        mcdb_index,             
    SOC_SAND_IN  uint32                        relocation_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t relocation_block_size   
)
{
    uint32 start = relocation_block_start, found_block_start, prev_entry;
    dpp_free_entries_block_size_t size = relocation_block_size, found_block_size;
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 entry_type = QAX_MCDS_GET_TYPE(mcds, mcdb_index), cud2 = DPP_MC_NO_2ND_CUD;
    dpp_free_entries_blocks_region_t *region = 0;
    int free_alloced_block = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!size) { 
        SOCDNX_IF_ERR_EXIT(qax_mcds_get_relocation_block(mcds, mcdb_index, &start, &size));
        if (!size) {
            DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_GET_TYPE(mcds, mcdb_index)));
            SOC_EXIT; 
        }
    }

    
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(entry_type) && (size == 1 || entry_type == DPP_MCDS_TYPE_VALUE_INGRESS));

    SOCDNX_IF_ERR_EXIT( 
        qax_mcds_get_free_entries_block(mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
            size, size, &found_block_start, &found_block_size));
    if (!found_block_size) {
        mcds->out_err = _SHR_E_FULL;
        SOC_EXIT;
    }
    DPP_MC_ASSERT(found_block_size <= size);
    prev_entry = QAX_MCDS_GET_PREV_ENTRY(mcds, start);

    free_alloced_block = 1;
    if (found_block_size == size) { 
        dpp_free_entries_block_size_t s;
        uint32 after_block; 

        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, start + size - 1, entry_type, &after_block));

        for (s = 0; s < size; ++s) {
            SOCDNX_IF_ERR_EXIT( 
                qax_mcdb_copy_write(unit, start + s, found_block_start + s));
            QAX_MCDS_SET_PREV_ENTRY(mcds, found_block_start + s, s ? found_block_start + (s-1) : prev_entry);
        }
        SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
            mcds, unit, prev_entry, entry_type, found_block_start));
        free_alloced_block = 0;
        if (after_block != QAX_MC_INGRESS_LINK_PTR_END) { 
            QAX_MCDS_SET_PREV_ENTRY(mcds, after_block, found_block_start + size - 1);
        }
    } else { 
        uint32 list_next; 
        free_alloced_block = 0;
        SOCDNX_IF_ERR_EXIT(qax_mcds_copy_replications_from_block( 
            unit, TRUE, start, size, &cud2, &list_next));

        SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list( 
            unit, DPP_MCDS_TYPE_VALUE_INGRESS, prev_entry, list_next, found_block_start, found_block_size, FALSE));
        found_block_start = mcds->linked_list;
        if (mcds->out_err) {
            SOC_EXIT;
        }
    }

    
    if (mcdb_index > start) { 
        if (!region) {
            region = qax_mcds_get_region(mcds, mcdb_index);
        }
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, start, mcdb_index - 1, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    if (mcdb_index + 1 < start + size) { 
        if (!region) {
            region = qax_mcds_get_region(mcds, mcdb_index);
        }
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, mcdb_index + 1, start + (size - 1), region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    mcds->out_err = _SHR_E_NONE;

exit:
    if (free_alloced_block) {
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, 
            found_block_start, found_block_start + (found_block_size - 1), qax_mcds_get_region(mcds, found_block_start), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }
    SOCDNX_FUNC_RETURN;
}




uint32
  qax_mcdb_free_linked_list_till_my_end(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 first_index,  
    SOC_SAND_IN uint32 entries_type, 
    SOC_SAND_IN uint32 end_index     
)
{
    uint32 cur_index;
    uint32 block_start = 0, block_end = 0; 
    uint32 range_start = 0, range_end = 0; 
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    dpp_free_entries_blocks_region_t *region = 0;
    SOCDNX_INIT_FUNC_DEFS;

    for (cur_index = first_index; cur_index != end_index; ) { 
        DPP_MC_ASSERT(cur_index > 0 && cur_index < mcds->ingress_bitmap_start);

        if (block_end) { 
            if (cur_index == block_end + 1 && cur_index <= range_end) { 
                block_end = cur_index;
            } else if (cur_index + 1 == block_start && cur_index >= range_start) { 
                block_start = cur_index;
            } else { 
                SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
                region = qax_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); 
                block_start = block_end = cur_index;
            }
        } else { 
            region = qax_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); 
            block_start = block_end = cur_index;
        }
        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, cur_index, entries_type, &cur_index));
    }
    if (block_end) { 
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  qax_mcdb_free_linked_list(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  first_index, 
    SOC_SAND_IN uint32  entries_type 
)
{
    return qax_mcdb_free_linked_list_till_my_end(unit, first_index, entries_type, 
      (DPP_MCDS_TYPE_IS_INGRESS(entries_type) ? QAX_MC_INGRESS_LINK_PTR_END : DPP_MC_EGRESS_LINK_PTR_END));
}




uint32 qax_mcds_get_group(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                do_clear,     
    SOC_SAND_IN  uint32               group_id,     
    SOC_SAND_IN  uint32               group_type,   
    SOC_SAND_IN  uint16               max_size,     
    SOC_SAND_OUT uint16               *group_size   
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 entry_type = DPP_MCDS_TYPE_GET_NONE_START(group_type); 
    const uint16 max_group_size = DPP_MCDS_TYPE_IS_INGRESS(group_type) ? QAX_MULT_MAX_INGRESS_REPLICATIONS : QAX_MULT_MAX_EGRESS_REPLICATIONS;
    uint32 cur_index = group_id;
    uint16 entries_remaining = max_size > max_group_size ? max_group_size : max_size; 
    unsigned is_egress = DPP_MCDS_TYPE_IS_EGRESS(group_type);

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group_size);
    *group_size = 0;

    if (do_clear) {
      qax_mcds_clear_replications(unit, mcds, group_type); 
    }
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(group_type) && mcds->hw_end == (DPP_MCDS_TYPE_IS_INGRESS(group_type) ?
      QAX_MC_INGRESS_LINK_PTR_END : DPP_MC_EGRESS_LINK_PTR_END));

    if (is_egress) { 
        cur_index = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, group_id);
    }
    
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_replications_from_entry(unit, cur_index,
      DPP_MCDS_TYPE_GET_START(group_type), &entries_remaining, group_size, &cur_index));

    
    while (cur_index != mcds->hw_end) {
        DPP_MC_ASSERT(cur_index > 0 && cur_index < mcds->ingress_bitmap_start);
        SOCDNX_IF_ERR_EXIT(qax_mcds_get_replications_from_entry(unit, cur_index, entry_type, &entries_remaining, group_size, &cur_index));
        if (*group_size > max_group_size) { 
            DPP_MC_ASSERT(0);
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("group is too big")));
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 qax_mcds_remove_replications_from_group(
    SOC_SAND_IN int                  unit,       
    SOC_SAND_IN dpp_mc_id_t          group_mcid, 
    SOC_SAND_IN uint32               type,       
    SOC_SAND_IN uint32               nof_reps,   
    SOC_SAND_IN dpp_mc_replication_t *reps       
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    const uint32 entry_index = (type == DPP_MCDS_TYPE_VALUE_EGRESS) ? QAX_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid) : group_mcid; 
    uint32 freed_index = entry_index; 
    qax_mcdb_entry_t *mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, entry_index);
    uint32 format = 0, next_index = entry_index, cur_index = 0, prev_index = entry_index, bitmap = -1;
    int is_port_cud_replication = 0, is_bitmap_cud_replication = 0;
    int remove_entry = 0, found = 0;
    uint16 entries_left = QAX_MULT_MAX_EGRESS_REPLICATIONS; 
    uint32 entry_type = QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry);
    int correct_cud2 = (reps->additional_cud == DPP_MC_NO_2ND_CUD);

    SOCDNX_INIT_FUNC_DEFS;

    DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_INGRESS_START == entry_type || DPP_MCDS_TYPE_VALUE_EGRESS_START == entry_type);
    DPP_MC_ASSERT(nof_reps <= 1); 

    
    if (reps->dest & ARAD_MC_EGR_IS_BITMAP_BIT) { 
        bitmap = reps->dest & ~ARAD_MC_EGR_IS_BITMAP_BIT;
        is_bitmap_cud_replication = 1;
        if (bitmap == DPP_MC_EGR_OUTLIF_DISABLED) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    } else {
        is_port_cud_replication = 1;
        if (reps->dest == DPP_MULT_EGRESS_PORT_INVALID) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    }

    for (; entries_left; --entries_left) {
        cur_index = next_index;

        if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) {
            switch (format = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, FORMATf)) {

                case 0: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, LINK_PTRf);
                  if (correct_cud2 && is_bitmap_cud_replication &&
                      bitmap == soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, BMP_PTRf) &&
                      reps->cud == soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, CUD_0f) &&
                      reps->additional_cud == soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, CUD_1f)) {
                      soc_mem_field32_set(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, BMP_PTRf, DPP_MC_EGR_BITMAP_DISABLED); 
                      found = remove_entry = 1;
                  }
                  break;

                case 1: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
                  if (is_port_cud_replication &&
                      soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_0f) == reps->cud &&
                      soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_1f) == reps->additional_cud &&
                      soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, DESTINATIONf) == reps->dest ) {
                      found = remove_entry = 1;
                  }
                  break;
                case 3: 
                case 2: 
                  if (correct_cud2 && is_port_cud_replication) {
                    if (soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f) == reps->dest &&
                        soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_0f) == reps->cud) {
                        found = 1;
                        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f) == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    } else if (soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f) == reps->dest &&
                        soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_1f) == reps->cud) {
                        found = 1;
                        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f) == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    }
                  }
                  if (format == 3) {
                    next_index = DPP_MC_EGRESS_LINK_PTR_END; 
                  } else {
                    next_index++;
                  }
                  break;

                default:
                  SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported ingress format found %u"), format));

              }
          } else {
              switch (format = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, ENTRY_FORMATf)) {

                case 0: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, LINK_PTRf);
                  if (correct_cud2 && is_bitmap_cud_replication && bitmap == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, BMP_PTRf) &&
                      reps->cud == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, OUTLIF_1f)) {
                      soc_mem_field32_set(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, BMP_PTRf, DPP_MC_EGR_BITMAP_DISABLED); 
                      found = remove_entry = 1;
                  }
                  break;

                case 1: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, LINK_PTRf);
                  if (correct_cud2 && is_port_cud_replication) {
                    if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Af) == reps->dest && 
                        soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, OUTLIF_Af) == reps->cud) {
                        found = 1;
                        soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Af, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Bf) == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    } else if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Bf) == reps->dest && 
                               soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, OUTLIF_Bf) == reps->cud) {
                        found = 1;
                        soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Bf, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Af) == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    }
                  }
                  break;

                case 2: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, LINK_PTRf);
                  if (is_port_cud_replication && 
                      soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, OUTLIF_1f) == reps->cud &&
                      soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, OUTLIF_2f) == reps->additional_cud) {
                      if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Af)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Af, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Bf) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      } else if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Bf)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Bf, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_1Af) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      }
                  }
                  break;

                case 5: 
                  next_index = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, LINK_PTRf);
                  if (correct_cud2 && is_port_cud_replication && soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, OUTLIFf) == reps->cud) {
                      if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      } else if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      } else if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      } else if (reps->dest == soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f)) {
                          found = 1;
                          soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f, DPP_MULT_EGRESS_PORT_INVALID); 
                          if (soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f) == DPP_MULT_EGRESS_PORT_INVALID &&
                              soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f) == DPP_MULT_EGRESS_PORT_INVALID) {
                              remove_entry = 1;
                          }
                      }
                  }
                  break;
              default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported egress format found %u"), format));
           }
        }

        if (found) {
            break;
        }

        if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("replication not found in group")));
        }
        prev_index = cur_index;
        mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, next_index);
        DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS == QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry) || DPP_MCDS_TYPE_VALUE_INGRESS == QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry));
    }

    if (!found) {
        DPP_MC_ASSERT(0);
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("The replication was not found till the legal size of a group")));
    }

    if (remove_entry) { 
        int try_to_copy_the_next_entry = 0;
        if (cur_index == entry_index) { 
            DPP_MC_ASSERT(prev_index == entry_index);
            if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
                mcdb_entry->word0 = mcds->free_value.word0;
                mcdb_entry->word1 = mcds->free_value.word1;
                mcdb_entry->word2 &= ~QAX_MC_ENTRY_MASK_VAL;
                mcdb_entry->word2 |= (mcds->free_value.word2 & QAX_MC_ENTRY_MASK_VAL);
                SOCDNX_IF_ERR_EXIT(qax_self_replication_set(unit, group_mcid, FALSE));
            } else  { 
                try_to_copy_the_next_entry = 1;
            }
        } else { 
            qax_mcdb_entry_t *prev_entry = MCDS_GET_MCDB_ENTRY(mcds, prev_index);
            
            uint32 prev_format = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, prev_entry, FORMATf);
            DPP_MC_ASSERT(prev_index != cur_index);
            format = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, FORMATf);
            
            if (QAX_MCDS_ENTRY_GET_TYPE(prev_entry) == DPP_MCDS_TYPE_VALUE_EGRESS || (prev_format != 1)) { 
                
                SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER(mcds, unit, prev_index, QAX_MCDS_ENTRY_GET_TYPE(prev_entry), next_index));
                freed_index = cur_index;
            } else if (next_index == QAX_MC_INGRESS_LINK_PTR_END || (soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, LASTf) == 1 && format == 1)) { 
                soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, prev_entry, LASTf, 1); 
                SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, prev_index));
                freed_index = cur_index;
            } else {
                try_to_copy_the_next_entry = 1;
            }
        }
        if (try_to_copy_the_next_entry) { 
            
            qax_mcdb_entry_t *next_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
            
            uint32 next_format = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, next_entry, FORMATf);
            
            if (QAX_MCDS_ENTRY_GET_TYPE(next_entry) == DPP_MCDS_TYPE_VALUE_EGRESS || (next_format != 1) || 
                next_index == QAX_MC_INGRESS_LINK_PTR_END || soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, next_entry, LASTf) == 1) {
                mcdb_entry->word0 = next_entry->word0;
                mcdb_entry->word1 = next_entry->word1;
                mcdb_entry->word2 &= ~QAX_MC_ENTRY_MASK_VAL;
                mcdb_entry->word2 |= (next_entry->word2 & QAX_MC_ENTRY_MASK_VAL);
                freed_index = next_index;
                SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, next_index, QAX_MCDS_ENTRY_GET_TYPE(next_entry), &next_index)); 
            }
        }
    }

    if (freed_index != cur_index || !remove_entry) { 
        SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, cur_index)); 
    }

    if (freed_index != entry_index) { 
        if (next_index != DPP_MC_EGRESS_LINK_PTR_END) {
            DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, next_index) == DPP_MCDS_TYPE_VALUE_EGRESS);
            QAX_MCDS_SET_PREV_ENTRY(mcds, next_index, prev_index); 
        }
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks( 
          unit, mcds, freed_index, freed_index, qax_mcds_get_region(mcds, freed_index), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_remove_replications(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               group_type,   
    SOC_SAND_IN  uint32               nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t *reps       
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_rep_data_t *rep = mcds->reps, stack_reps[10], *reps_to_remove = stack_reps, *to_remove;
    uint16 size_left = 0;
    uint32 i, dest;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    mcds->out_err = _SHR_E_NONE;
    if (nof_reps == 0) {
        SOC_EXIT;
    } else if (nof_reps > 10) { 
        reps_to_remove = NULL;
        SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &reps_to_remove, sizeof(*reps_to_remove) * nof_reps, "reps2remove"));
    }
    for (i = 0, to_remove = reps_to_remove; i < nof_reps; ++i, ++to_remove) { 
        COMPILER_64_ZERO(*to_remove);
        dest = reps->dest;
        if (group_type == DPP_MCDS_TYPE_VALUE_EGRESS) {
            (void)MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, dest, &dest, &core));
        }
        if (reps->dest & ARAD_MC_EGR_IS_BITMAP_BIT) { 
            dest &= ~ARAD_MC_EGR_IS_BITMAP_BIT;
            QAX_MCDS_REP_DATA_SET_TYPE(to_remove, QAX_MCDS_REP_TYPE_BM);
        }
        QAX_MCDS_REP_DATA_SET_DEST(to_remove, dest);
        QAX_MCDS_REP_DATA_SET_CUD1(to_remove, reps->cud);
        QAX_MCDS_REP_DATA_SET_CUD2(to_remove, reps->additional_cud);
        ++reps;
    }
    to_remove = reps_to_remove;
        
        
    if (nof_reps > 1) { 
        soc_sand_os_qsort(reps_to_remove, nof_reps, sizeof(*reps_to_remove), qax_rep_data_t_compare); 
        soc_sand_os_qsort(rep, mcds->nof_reps, sizeof(*rep), qax_rep_data_t_compare); 
        SOCDNX_EXIT_WITH_ERR(BCM_E_PARAM, (_BSL_SOCDNX_MSG("nof_replications > 1 not currently supported")));
    } else { 
        for (size_left = mcds->nof_reps; size_left; --size_left, ++rep) {
            if (QAX_EQ_REP_DATA(rep, to_remove)) {
                QAX_MCDS_REP_DATA_GET_TYPE(to_remove, i);
                *rep = mcds->reps[--mcds->nof_reps];
                --*(i == QAX_MCDS_REP_TYPE_DEST ? &mcds->nof_dest_cud_reps : &mcds->nof_bitmap_reps);
                SOC_EXIT;
            }
        }
        mcds->out_err = _SHR_E_NOT_FOUND;
    }

exit:
    if (reps_to_remove != stack_reps) {
        sand_free_mem_if_not_null(unit, (void*)&reps_to_remove);
    }
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int unit
)
{
  const qax_mcds_t *mcds = dpp_get_mcds(unit);
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_PCID_LITE_SKIP(unit);

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM) 
#endif
  {
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, TAR_MCDBm, MEM_BLOCK_ANY, &mcds->free_value));
  }

exit:
  SOCDNX_FUNC_RETURN;
}







STATIC int qax_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id, 
    SOC_SAND_OUT uint8 *is_open
)
{
    SOCDNX_INIT_FUNC_DEFS
    DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, group_id, is_open));
exit:
    SOCDNX_FUNC_RETURN
}



uint32 qax_mult_eg_get_group(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    group_mcid,           
    SOC_SAND_IN  uint32         mc_group_size,        
    SOC_SAND_OUT soc_gport_t    *ports,               
    SOC_SAND_OUT soc_if_t       *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32         *exact_mc_group_size, 
    SOC_SAND_OUT uint8          *is_open              
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(exact_mc_group_size);
    SOCDNX_NULL_CHECK(is_open);
    if (mc_group_size && !reps && (!ports || !cuds)) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("NULL pointer")));
    }

    SOCDNX_IF_ERR_EXIT(qax_mult_does_group_exist(unit, group_mcid, TRUE, is_open)) ; 

    if (*is_open) {
        uint16 group_size;
        SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(
            unit, TRUE, group_mcid, DPP_MCDS_TYPE_VALUE_EGRESS, mc_group_size, &group_size));
        *exact_mc_group_size = group_size;
        SOCDNX_IF_ERR_EXIT(qax_mcds_copy_replications_to_arrays(unit, 1, mc_group_size,  ports, cuds, reps));
    } else { 
        *exact_mc_group_size = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_eg_mcdb_bitmap_get(
                  int    unit,
                  uint32 entry_offset,
                  uint32 *vlan_table_tbl_data
)
{
    int i;
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 bitmap_offset = mcds->egress_bitmap_start + SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes * entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes; i++ ) {
        SOCDNX_IF_ERR_EXIT(READ_TAR_MCDB_BITMAP_REPLICATION_VECTORm(unit, COPYNO_ALL, bitmap_offset + i, vlan_table_tbl_data + (i*2)));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_eg_mcdb_bitmap_set(
                int     unit,
                uint32  entry_offset,
                uint32* vlan_table_tbl_data
)
{
    int i;
    int rep_exist = FALSE;
    uint32 data[QAX_MC_ENTRY_SIZE];
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 bitmap_offset = mcds->egress_bitmap_start + SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes * entry_offset;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32; ++i) {
        if (vlan_table_tbl_data[i]) {
            rep_exist = TRUE;
            break;
        }
    }
    SOCDNX_IF_ERR_EXIT(qax_self_replication_set(unit, entry_offset, rep_exist));

    for (i = 0; i < SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes; i++ ) {
        soc_mem_field_set(unit, TAR_MCDB_BITMAP_REPLICATION_VECTORm, data, BITMAP_VECf, vlan_table_tbl_data + (i*2));
        soc_mem_field32_set(unit, TAR_MCDB_BITMAP_REPLICATION_VECTORm, data, LASTf, (i==SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes-1)); 
        soc_mem_field32_set(unit, TAR_MCDB_BITMAP_REPLICATION_VECTORm, data, FORMATf, 3); 
        soc_mem_field32_set(unit, TAR_MCDB_BITMAP_REPLICATION_VECTORm, data, OFFSETf, i); 
        SOCDNX_IF_ERR_EXIT(WRITE_TAR_MCDB_BITMAP_REPLICATION_VECTORm(unit, MEM_BLOCK_ANY, bitmap_offset + i, data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



void qax_mc_start_linked_list(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                        in_block_start, 
    SOC_SAND_IN dpp_free_entries_block_size_t in_block_size,  
    SOC_SAND_IN uint32                        start,          
    SOC_SAND_IN uint32                        end,            
    SOC_SAND_IN uint8                         list_type       
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_NORMAL(list_type) && ((0 == in_block_size) == DPP_MCDS_TYPE_IS_START(list_type))); 

    mcds->out_err = _SHR_E_NONE; 
    if (in_block_size == 0) { 
        mcds->is_group_start_to_be_filled = mcds->writing_full_list = TRUE;
        mcds->start_entry = mcds->free_value;
    } else { 
        mcds->is_group_start_to_be_filled = mcds->writing_full_list = FALSE;
    }

    mcds->linked_list_end = QAX_NO_MCDB_INDEX; 
    mcds->linked_list = mcds->list_end = end;
    mcds->list_start = list_type == DPP_MCDS_TYPE_VALUE_EGRESS_START ? QAX_MCDS_GET_EGRESS_GROUP_START(mcds, start) : start;
    mcds->nof_possible_reps = 0; 
    mcds->block_start = in_block_start; 
    mcds->block_size = in_block_size;
    mcds->state = QAX_MCDS_STATE_INITED;
}




uint32 qax_mc_end_linked_list(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN uint32   to_delete
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, mcds->list_start);
    qax_mcdb_entry_t old_start_entry = *mcdb_entry; 

    SOCDNX_INIT_FUNC_DEFS;
    

    
    if (mcds->writing_full_list) { 
        DPP_MC_ASSERT(mcds->list_end == mcds->hw_end);
        
        mcdb_entry->word0 = mcds->start_entry.word0; 
        mcdb_entry->word1 = mcds->start_entry.word1;
        mcdb_entry->word2 &= QAX_MCDS_LAST_WORD_KEEP_BITS_MASK;
        
        mcdb_entry->word2 |= mcds->start_entry.word2;

        QAX_MCDS_SET_PREV_ENTRY(mcds, mcds->list_start, mcds->list_start); 
        QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, mcds->group_type_start); 
    }
    
    if (!to_delete) 
    soc_mem_field32_set(unit, mcds->group_type == DPP_MCDS_TYPE_VALUE_INGRESS ? TAR_MCDB_SINGLE_REPLICATIONm : TAR_MCDB_EGRESS_TDM_FORMATm,
      mcdb_entry, LINK_PTRf, mcds->linked_list); 
    SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, mcds->list_start)); 

    if (mcds->linked_list != mcds->list_end) {
        
        QAX_MCDS_SET_PREV_ENTRY(mcds, mcds->linked_list, mcds->list_start);
        if (mcds->list_end != mcds->hw_end) { 
            
            QAX_MCDS_SET_PREV_ENTRY(mcds, mcds->list_end, mcds->linked_list_end);
        }
    } else {
        DPP_MC_ASSERT(mcds->writing_full_list && mcds->linked_list_end == QAX_NO_MCDB_INDEX);
    }
    mcds->state = QAX_MCDS_STATE_FINISHED;
exit:
    if (mcds->state != QAX_MCDS_STATE_FINISHED) { 
        *mcdb_entry = old_start_entry;
    }
    SOCDNX_FUNC_RETURN;
}


STATIC uint32 qax_mc_get_linked_list_entry(
    SOC_SAND_IN  int              unit,
    SOC_SAND_OUT qax_mcdb_entry_t **mcdb_entry 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    SOCDNX_INIT_FUNC_DEFS;

    if (mcds->is_group_start_to_be_filled) { 
        DPP_MC_ASSERT(mcds->writing_full_list && mcds->block_size == 0);
        mcds->cur_entry_index = mcds->list_start;
        *mcdb_entry = mcds->cur_entry = &mcds->start_entry;
        mcds->is_group_start_to_be_filled = FALSE;

    } else { 

        if (mcds->block_size == 0) { 
            SOCDNX_IF_ERR_EXIT(qax_mcds_get_free_entries_block(mcds, mcds->alloc_flags, 
              1, 1, &mcds->block_start, &mcds->block_size));
            if (!mcds->block_size) { 
                mcds->out_err = _SHR_E_FULL;
                SOC_EXIT; 
            }
            DPP_MC_ASSERT(mcds->block_size == 1);
        }
        mcds->cur_entry_index = mcds->block_start;
        *mcdb_entry = mcds->cur_entry = QAX_GET_MCDB_ENTRY(mcds, mcds->block_start);
        --mcds->block_size;
        ++mcds->block_start;
    }
    DPP_MC_ASSERT(mcds->cur_entry_index < mcds->ingress_bitmap_start);
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32 qax_mc_set_linked_list_entry(
    SOC_SAND_IN int  unit
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    SOCDNX_INIT_FUNC_DEFS;

    if (mcds->cur_entry_index == mcds->list_start) { 
        DPP_MC_ASSERT(mcds->writing_full_list && mcds->block_size == 0 && mcds->linked_list == mcds->list_end && mcds->linked_list_end == QAX_NO_MCDB_INDEX && mcds->state == QAX_MCDS_STATE_INITED);
        QAX_MCDS_ENTRY_SET_TYPE(mcds->cur_entry, mcds->group_type_start); 
    } else { 
        DPP_MC_ASSERT(mcds->cur_entry == QAX_GET_MCDB_ENTRY(mcds, mcds->cur_entry_index) && mcds->cur_entry_index + 1 == mcds->block_start);

        if (mcds->linked_list_end == QAX_NO_MCDB_INDEX) { 
            DPP_MC_ASSERT(mcds->list_start != mcds->cur_entry_index && mcds->linked_list == mcds->list_end && mcds->state == QAX_MCDS_STATE_INITED);
            mcds->linked_list_end = mcds->cur_entry_index; 
            mcds->state = QAX_MCDS_STATE_STARTED;
        } else { 
            DPP_MC_ASSERT(mcds->linked_list != mcds->list_end && mcds->linked_list_end != QAX_NO_MCDB_INDEX && mcds->state == QAX_MCDS_STATE_STARTED);
            
            QAX_MCDS_SET_PREV_ENTRY(mcds, mcds->linked_list, mcds->cur_entry_index);
        }
        
        soc_mem_field32_set(unit, mcds->group_type == DPP_MCDS_TYPE_VALUE_INGRESS ? TAR_MCDB_SINGLE_REPLICATIONm : TAR_MCDB_EGRESS_TDM_FORMATm,
            mcds->cur_entry, LINK_PTRf, mcds->linked_list); 

        mcds->linked_list = mcds->cur_entry_index; 
        SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, mcds->cur_entry_index)); 
        QAX_MCDS_ENTRY_SET_TYPE(mcds->cur_entry, mcds->group_type); 

    }
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 qax_ing_no_ptr_block(
    SOC_SAND_IN    int            unit,
    SOC_SAND_INOUT qax_rep_data_t **rep,         
                                                 
    SOC_SAND_INOUT qax_rep_data_t **couples,     
    SOC_SAND_INOUT int            *couples_left, 
    SOC_SAND_INOUT int            *nof_entries   
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    uint32 cur_entry_index, block_end, dest, cud1, cud2, rep_type;
    int couples_used, entries_left; 
    soc_mem_t mcdb_bmp_repl_mem;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(mcds->is_group_start_to_be_filled == 0 && *nof_entries && *couples_left && *couples_left && *nof_entries > 0 && *nof_entries <= DPP_MCDS_MAX_FREE_BLOCK_SIZE);
    mcdb_bmp_repl_mem = TAR_MCDB_BITMAP_REPLICATION_ENTRYm;

    if (mcds->block_size <= 0) { 
        SOCDNX_IF_ERR_EXIT(qax_mcds_get_free_entries_block(mcds, DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
          *nof_entries, DPP_MCDS_MAX_FREE_BLOCK_SIZE, &mcds->block_start, &mcds->block_size)); 
        if (!mcds->block_size) { 
            mcds->out_err = _SHR_E_FULL;
            SOC_EXIT; 
        }
        DPP_MC_ASSERT(mcds->block_size > 0);
    }
    if (mcds->block_size >= *nof_entries) {
        entries_left = *nof_entries;
    } else {
        *nof_entries = entries_left = mcds->block_size;
    }
    couples_used = *nof_entries;

    cur_entry_index = block_end = mcds->block_start + (entries_left - 1);
    mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, cur_entry_index);

    
    if (mcds->linked_list != QAX_MC_INGRESS_LINK_PTR_END) { 
        DPP_MC_ASSERT(mcds->linked_list_end != QAX_NO_MCDB_INDEX && *couples_left + 1 >= entries_left);
        
        --*rep;
        QAX_MCDS_REP_DATA_GET_FIELDS(*rep, dest, cud1, cud2, rep_type);
        qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);
        if (rep_type == QAX_MCDS_REP_TYPE_DEST) {
            soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_0f, cud1); 
            soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_1f, cud2); 
            soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, DESTINATIONf, dest); 
            soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, FORMATf, 1); 
        } else {
            soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, CUD_0f, cud1); 
            soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, CUD_1f, cud2); 
            soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, BMP_PTRf, 
              mcds->ingress_bitmap_start + dest * SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes);
            soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, FORMATf, 0); 
        }
        
        soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf, mcds->linked_list); 
        --couples_used;
    } else {
        DPP_MC_ASSERT(*couples_left >= entries_left);
        QAX_MCDS_REP_DATA_GET_FIELDS(*couples, dest, cud1, cud2, rep_type);
        ++*couples;
        DPP_MC_ASSERT (rep_type == QAX_MCDS_REP_TYPE_DEST && cud2 == DPP_MC_NO_2ND_CUD);
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_0f, cud1); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f, dest); 
        QAX_MCDS_REP_DATA_GET_FIELDS(*couples, dest, cud1, cud2, rep_type);
        ++*couples;
        DPP_MC_ASSERT (rep_type == QAX_MCDS_REP_TYPE_DEST && cud2 == DPP_MC_NO_2ND_CUD);
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_1f, cud1); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f, dest); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, LASTf, 1); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, FORMATf, 1); 
    }
    QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS); 
    SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, cur_entry_index)); 

    for (--entries_left; entries_left > 0; --entries_left) {
        --cur_entry_index;
        QAX_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, cur_entry_index); 
        --mcdb_entry;
        QAX_MCDS_REP_DATA_GET_FIELDS(*couples, dest, cud1, cud2, rep_type);
        ++*couples;
        DPP_MC_ASSERT (rep_type == QAX_MCDS_REP_TYPE_DEST && cud2 == DPP_MC_NO_2ND_CUD);
        qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_0f, cud1); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_0f, dest); 
        QAX_MCDS_REP_DATA_GET_FIELDS(*couples, dest, cud1, cud2, rep_type);
        ++*couples;
        DPP_MC_ASSERT (rep_type == QAX_MCDS_REP_TYPE_DEST && cud2 == DPP_MC_NO_2ND_CUD);
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_1f, cud1); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f, dest); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, LASTf, 0); 
        soc_mem_field32_set(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, FORMATf, 1); 
        SOCDNX_IF_ERR_EXIT(qax_mcds_write_entry(unit, cur_entry_index)); 
        QAX_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS); 

    }

    if (mcds->linked_list_end == QAX_NO_MCDB_INDEX) { 
        DPP_MC_ASSERT(mcds->linked_list == mcds->list_end && mcds->state == QAX_MCDS_STATE_INITED);
        mcds->linked_list_end = block_end; 
        mcds->state = QAX_MCDS_STATE_STARTED;
    } else { 
        DPP_MC_ASSERT(mcds->linked_list != mcds->list_end && mcds->linked_list_end != QAX_NO_MCDB_INDEX && mcds->state == QAX_MCDS_STATE_STARTED);
        
        QAX_MCDS_SET_PREV_ENTRY(mcds, mcds->linked_list, block_end);
    }
    mcds->linked_list = cur_entry_index; 

    DPP_MC_ASSERT(mcds->block_size >= *nof_entries && cur_entry_index == mcds->block_start && mcdb_entry == QAX_GET_MCDB_ENTRY(mcds, cur_entry_index));
    mcds->block_size -= *nof_entries;
    mcds->block_start += *nof_entries;
    *couples_left -= couples_used;
exit:
    SOCDNX_FUNC_RETURN;
}




uint32 qax_eg_mc_write_entry_2ports_cud(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 port1, 
    SOC_SAND_IN  uint32 cud1,  
    SOC_SAND_IN  uint32 port2, 
    SOC_SAND_IN  uint32 cud2   
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_mc_get_linked_list_entry(unit, &mcdb_entry));
    if (mcds->out_err) {
        SOC_EXIT;
    }
    qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);

    
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, OUTLIF_Af, cud1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, OUTLIF_Bf, cud2); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Af, port1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, PP_DSP_Bf, port2); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, mcdb_entry, ENTRY_FORMATf, 1); 

    
    SOCDNX_IF_ERR_EXIT(qax_mc_set_linked_list_entry(unit));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_eg_mc_write_entry_4ports_cud(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 port1, 
    SOC_SAND_IN  uint32 port2, 
    SOC_SAND_IN  uint32 port3, 
    SOC_SAND_IN  uint32 port4, 
    SOC_SAND_IN  uint32 cud   
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_mc_get_linked_list_entry(unit, &mcdb_entry));
    if (mcds->out_err) {
        SOC_EXIT;
    }
    qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);

    
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, OUTLIFf, cud); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_1f, port1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f, port2);
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f, port3);
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f, port4);
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, ENTRY_FORMATf, 5); 

    
    SOCDNX_IF_ERR_EXIT(qax_mc_set_linked_list_entry(unit));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_eg_mc_write_entry_2ports_2cuds(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 port1, 
    SOC_SAND_IN  uint32 port2, 
    SOC_SAND_IN  uint32 cud1,  
    SOC_SAND_IN  uint32 cud2   
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_mc_get_linked_list_entry(unit, &mcdb_entry));
    if (mcds->out_err) {
        SOC_EXIT;
    }
    qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);

    
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, OUTLIF_1f, cud1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, OUTLIF_2f, cud2); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_Af, port1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, PP_DSP_Bf, port2); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm, mcdb_entry, ENTRY_FORMATf, 2); 

    
    SOCDNX_IF_ERR_EXIT(qax_mc_set_linked_list_entry(unit));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_eg_mc_write_entry_bitmap_cud(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bitmap_id, 
    SOC_SAND_IN  uint32 cud1       
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(qax_mc_get_linked_list_entry(unit, &mcdb_entry));
    if (mcds->out_err) {
        SOC_EXIT;
    }
    qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);

    
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, OUTLIF_1f, cud1); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, BMP_PTRf,
      mcds->egress_bitmap_start + bitmap_id * SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes); 
    soc_mem_field32_set(unit, TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm, mcdb_entry, ENTRY_FORMATf, 0); 

    
    SOCDNX_IF_ERR_EXIT(qax_mc_set_linked_list_entry(unit));
exit:
    SOCDNX_FUNC_RETURN;
}




STATIC uint32 qax_ing_mc_one_rep_entry(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  qax_rep_data_t *rep 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry;
    uint32 dest, cud1, cud2, rep_type;
    soc_mem_t mcdb_bmp_repl_mem;
    SOCDNX_INIT_FUNC_DEFS;

    mcdb_bmp_repl_mem = TAR_MCDB_BITMAP_REPLICATION_ENTRYm;
    QAX_MCDS_REP_DATA_GET_FIELDS(rep, dest, cud1, cud2, rep_type);
    
    SOCDNX_IF_ERR_EXIT(qax_mc_get_linked_list_entry(unit, &mcdb_entry));
    if (mcds->out_err) {
        SOC_EXIT;
    }

    qax_mcdb_entry_clear_mcdb_bits(mcdb_entry);
    
    if (rep_type == QAX_MCDS_REP_TYPE_DEST) {
        soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_0f, cud1); 
        soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_1f, cud2); 
        soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, DESTINATIONf, dest); 
        soc_mem_field32_set(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, FORMATf, 1); 
    } else {
        soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, CUD_0f, cud1); 
        soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, CUD_1f, cud2); 
        soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, BMP_PTRf, 
          mcds->ingress_bitmap_start + dest * SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes);
        soc_mem_field32_set(unit, mcdb_bmp_repl_mem, mcdb_entry, FORMATf, 0); 
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mc_set_linked_list_entry(unit));
exit:
    SOCDNX_FUNC_RETURN;
}




uint32 qax_mcds_set_linked_list (
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint8                         list_type,           
    SOC_SAND_IN uint32                        group_id,            
    SOC_SAND_IN uint32                        list_end,            
    SOC_SAND_IN uint32                        alloced_block_start, 
    SOC_SAND_IN dpp_free_entries_block_size_t alloced_block_size,  
    SOC_SAND_IN uint32                        to_delete            
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    qax_rep_data_t *rep, *rep_ptr, *couples_ptr;
    int nof_reps_left_in_core;    
    int i, couples_left;
    uint32 dest, cud1, cud2, rep_type, next_dest, next_cud1, next_cud2, next_rep_type, port2, ports34[2];

    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_NORMAL(list_type) && DPP_MCDS_TYPE_GET_NONE_START(list_type) == mcds->group_type);
    
    qax_mc_start_linked_list(unit, alloced_block_start, alloced_block_size, group_id, list_end, list_type);

    
    nof_reps_left_in_core = mcds->nof_reps;
    rep = mcds->reps;
    if (nof_reps_left_in_core > 0) {
        
        soc_sand_os_qsort(rep, nof_reps_left_in_core, sizeof(qax_rep_data_t), qax_rep_data_t_compare);

        if (!(SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_ALLOW_DUPLICATES_MODE)) { 
            for (i = nof_reps_left_in_core; i > 1; --i) {
                rep_ptr = rep+1;
                if (QAX_EQ_REP_DATA(rep, rep_ptr)) {
                    mcds->out_err = _SHR_E_PARAM;
                    SOC_EXIT; 
                }
                rep = rep_ptr;
            }
            rep = mcds->reps;
        }

        if (DPP_MCDS_TYPE_IS_EGRESS(list_type)) { 
            do { 
                QAX_MCDS_REP_DATA_GET_FIELDS(rep, dest, cud1, cud2, rep_type);
                if (rep_type != QAX_MCDS_REP_TYPE_DEST || cud2 != DPP_MC_NO_2ND_CUD) {
                    break;
                }
                ++rep;
                if (--nof_reps_left_in_core > 0) {
                    QAX_MCDS_REP_DATA_GET_FIELDS(rep, next_dest, next_cud1, next_cud2, next_rep_type);
                    if (next_rep_type == QAX_MCDS_REP_TYPE_DEST && next_cud2 == DPP_MC_NO_2ND_CUD) { 
                        mcds->nof_possible_reps +=2;
                        ++rep;
                        --nof_reps_left_in_core; 
                        if (cud1 != next_cud1) { 
                            SOCDNX_IF_ERR_EXIT(qax_eg_mc_write_entry_2ports_cud(unit, dest, cud1, next_dest, next_cud1));
                            if (mcds->out_err) { 
                                 SOC_EXIT; 
                            }
                        } else { 
                            port2 = next_dest;
                            ports34[1] = ports34[0] = DPP_MULT_EGRESS_PORT_INVALID;
                            for (i = 0; i < 2 && nof_reps_left_in_core > 0; ++i, ++rep, ++mcds->nof_possible_reps, --nof_reps_left_in_core) {
                                QAX_MCDS_REP_DATA_GET_FIELDS(rep, next_dest, next_cud1, cud2, next_rep_type);
                                if (rep_type != QAX_MCDS_REP_TYPE_DEST || cud2 != DPP_MC_NO_2ND_CUD || cud1 != next_cud1) {
                                    break;
                                }
                                ports34[i] = next_dest;
                            }
                            SOCDNX_IF_ERR_EXIT(qax_eg_mc_write_entry_4ports_cud(unit, dest, port2, ports34[0], ports34[1], cud1));
                            if (mcds->out_err) { 
                                 SOC_EXIT; 
                            }
                        }
                        continue;
                    }
                }
                
                ++mcds->nof_possible_reps;
                SOCDNX_IF_ERR_EXIT(qax_eg_mc_write_entry_2ports_cud(unit, dest, cud1, DPP_MULT_EGRESS_PORT_INVALID, DPP_MC_NO_2ND_CUD));
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
            } while (nof_reps_left_in_core > 0);

            
            while (nof_reps_left_in_core > 0) {
                QAX_MCDS_REP_DATA_GET_FIELDS(rep, dest, cud1, cud2, rep_type);
                if (rep_type != QAX_MCDS_REP_TYPE_DEST) {
                    break;
                }
                DPP_MC_ASSERT(cud2 != DPP_MC_NO_2ND_CUD);
                ++rep;
                port2 = DPP_MULT_EGRESS_PORT_INVALID;
                ++mcds->nof_possible_reps;
                if (--nof_reps_left_in_core > 0) {
                    QAX_MCDS_REP_DATA_GET_FIELDS(rep, next_dest, next_cud1, next_cud2, next_rep_type);
                    if (next_rep_type == QAX_MCDS_REP_TYPE_DEST && next_cud1 == cud1 && next_cud2 == cud2) {
                        port2 = next_dest;
                        ++rep;
                        ++mcds->nof_possible_reps;
                        --nof_reps_left_in_core;
                    }
                }
                
                SOCDNX_IF_ERR_EXIT(qax_eg_mc_write_entry_2ports_2cuds(unit, dest, port2, cud1, cud2));
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
            }

            
            while (nof_reps_left_in_core > 0) {
                DPP_MC_ASSERT(rep_type != QAX_MCDS_REP_TYPE_DEST && cud2 == DPP_MC_NO_2ND_CUD);
                ++rep;
                --nof_reps_left_in_core;
                mcds->nof_possible_reps += (DPP_MC_NOF_EGRESS_PORTS-1); 
                
                SOCDNX_IF_ERR_EXIT(qax_eg_mc_write_entry_bitmap_cud(unit, dest, cud1));
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
                QAX_MCDS_REP_DATA_GET_FIELDS(rep, dest, cud1, cud2, rep_type);
            }
            
            SOCDNX_IF_ERR_EXIT(qax_self_replication_set(unit , group_id, TRUE));

        } else { 
            int full_block_size, pointer_entry;
            
            for (couples_ptr = rep; nof_reps_left_in_core > 0; --nof_reps_left_in_core, ++couples_ptr) {
                QAX_MCDS_REP_DATA_GET_FIELDS(couples_ptr, dest, cud1, cud2, rep_type);
                if (rep_type != QAX_MCDS_REP_TYPE_DEST || cud2 != DPP_MC_NO_2ND_CUD) {
                    break;
                }
            }
            couples_left = couples_ptr - mcds->reps;
            DPP_MC_ASSERT(couples_left >= 0 && nof_reps_left_in_core + couples_left == mcds->nof_reps);
            rep = (couples_ptr = mcds->reps) + (nof_reps_left_in_core = mcds->nof_reps);
            couples_left /= 2; 
            nof_reps_left_in_core -= couples_left * 2; 

            if (mcds->is_group_start_to_be_filled) { 
                SOCDNX_IF_ERR_EXIT(qax_ing_mc_one_rep_entry(unit, --rep));
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
                if (nof_reps_left_in_core > 0) {
                    --nof_reps_left_in_core;
                } else {
                    ++nof_reps_left_in_core;
                    --couples_left;
                }
                DPP_MC_ASSERT(!mcds->is_group_start_to_be_filled && couples_left >= 0);
            }
            DPP_MC_ASSERT(nof_reps_left_in_core + couples_left * 2 == rep - couples_ptr && couples_left >= 0);
            while (couples_left > 0) {
                pointer_entry = mcds->linked_list == QAX_MC_INGRESS_LINK_PTR_END ? 0 : 1; 
                if (pointer_entry && nof_reps_left_in_core <= 0) {
                    nof_reps_left_in_core +=2;
                    --couples_left;
                    if (couples_left == 0) {
                        break;
                    }
                }
                full_block_size = couples_left + pointer_entry; 
                if (full_block_size > DPP_MCDS_MAX_FREE_BLOCK_SIZE) {
                    full_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE;
                }
                DPP_MC_ASSERT(full_block_size > pointer_entry && nof_reps_left_in_core >= pointer_entry);
                SOCDNX_IF_ERR_EXIT(qax_ing_no_ptr_block(unit, &rep, &couples_ptr, &couples_left, &full_block_size)); 
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
                nof_reps_left_in_core -= pointer_entry;
                DPP_MC_ASSERT(nof_reps_left_in_core + couples_left * 2 == rep - couples_ptr && couples_left >= 0);
            }

            for (; nof_reps_left_in_core > 0; --nof_reps_left_in_core) { 
                DPP_MC_ASSERT(nof_reps_left_in_core == rep - couples_ptr);
                SOCDNX_IF_ERR_EXIT(qax_ing_mc_one_rep_entry(unit, --rep));
                if (mcds->out_err) { 
                     SOC_EXIT; 
                }
            }
            DPP_MC_ASSERT(couples_ptr == rep && (nof_reps_left_in_core | couples_left) == 0);
        }
    } else if (DPP_MCDS_TYPE_IS_INGRESS(list_type)) {
        
        mcds->start_entry = mcds->empty_ingr_value;
    }
    
    SOCDNX_IF_ERR_EXIT(qax_mc_end_linked_list(unit, to_delete)); 

exit:
    if (mcds->state == QAX_MCDS_STATE_STARTED) {  
        DPP_MC_ASSERT(mcds->linked_list != mcds->list_end);
        qax_mcdb_free_linked_list_till_my_end(unit, mcds->linked_list, QAX_MCDS_ENTRY_GET_TYPE(QAX_GET_MCDB_ENTRY(mcds,mcds->linked_list)), list_end);
    } else {
        DPP_MC_ASSERT(mcds->state == QAX_MCDS_STATE_INITED || mcds->state == QAX_MCDS_STATE_FINISHED);
    }
    mcds->state = 0;
    if (mcds->block_size) { 
        qax_mcds_build_free_blocks(unit, mcds, mcds->block_start, mcds->block_start + mcds->block_size - 1,
          qax_mcds_get_region(mcds, mcds->block_start), McdsFreeBuildBlocksAddAll);
    }
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_eg_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_id_t          mcid,         
    SOC_SAND_IN  uint8                allow_create, 
    SOC_SAND_IN  uint32               group_size,   
    SOC_SAND_IN  dpp_mc_replication_t *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err      
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint8 group_exists = 0;
    int failed = 1, group_start_alloced = 0;
    uint32 entry_type, group_entry_id;
    uint32 old_group_entries = DPP_MC_EGRESS_LINK_PTR_END; 

    SOCDNX_INIT_FUNC_DEFS;

    DPP_MC_ASSERT(mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    SOCDNX_IF_ERR_EXIT(qax_egress_group_open_get(unit, mcid, &group_exists));
    if (!(group_exists | allow_create)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }
    { 
        group_entry_id = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, mcid);
        entry_type = QAX_MCDS_GET_TYPE(mcds, group_entry_id);

        if (group_exists) { 
            DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_EGRESS_START(entry_type));
            SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, group_entry_id, DPP_MCDS_TYPE_VALUE_EGRESS, &old_group_entries));
        } else {
            
            if (DPP_MCDS_TYPE_IS_USED(entry_type)) { 
                DPP_MC_ASSERT(!DPP_MCDS_TYPE_IS_START(entry_type));
                SOCDNX_IF_ERR_EXIT(qax_mcdb_relocate_entries(unit, group_entry_id, 0, 0));
                if (mcds->out_err) { 
                    SOC_EXIT;
                }
            } else { 
                SOCDNX_IF_ERR_EXIT(qax_mcds_reserve_group_start(mcds, group_entry_id));
            }
            group_start_alloced = 1;
        }

    }

    SOCDNX_IF_ERR_EXIT(qax_mcds_copy_egress_replications_from_arrays(unit, TRUE, group_size, reps)); 

    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list( 
      unit, DPP_MCDS_TYPE_VALUE_EGRESS_START, mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, FALSE));
    if (mcds->out_err) { 
        SOC_EXIT;
    }

    if (group_exists) { 
        if (old_group_entries != DPP_MC_EGRESS_LINK_PTR_END) { 
            SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_EGRESS));
        }
    } else {
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 1));
    }

    failed = 0;
exit:
    if (group_start_alloced & failed) { 
        DPP_MC_ASSERT(!group_exists);
        group_entry_id = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, mcid);
        if (qax_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
          qax_mcds_get_region(mcds, group_entry_id), McdsFreeBuildBlocksAdd_AllMustBeUsed) != SOC_E_NONE) {
            cli_out("qax_mcds_build_free_blocks failed\n");
        }
    }
    if (out_err != NULL && _rv == SOC_E_NONE) {
        *out_err = mcds->out_err;
    }
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_eg_group_close(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t mcid
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 group_entry_id = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, mcid);
    SOC_TMC_ERROR internal_err;
    uint8 does_exist;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(qax_mult_does_group_exist(unit, mcid, TRUE, &does_exist));
    if (does_exist) { 
        SOCDNX_IF_ERR_EXIT(qax_mult_eg_group_set( 
          unit, mcid, FALSE, 0, 0, &internal_err));
        if (internal_err) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("too many entries")));
        }

        
        SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
          &mcds->egress_starts, McdsFreeBuildBlocksAdd_AllMustBeUsed));

        SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_eg_reps_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_IN  uint32                nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t  *reps,      
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err    
)
{
    uint16 group_size;
    uint32 linked_list_start;
    uint32 old_group_entries = DPP_MC_EGRESS_LINK_PTR_END; 
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint8 is_open;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);
    SOCDNX_IF_ERR_EXIT(qax_egress_group_open_get(unit, group_mcid, &is_open));
    if (!is_open) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_copy_egress_replications_from_arrays(unit, TRUE, nof_reps, reps));

    
    linked_list_start = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid);
    DPP_MC_ASSERT(QAX_MCDS_GET_TYPE(mcds, linked_list_start) == DPP_MCDS_TYPE_VALUE_EGRESS_START);
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( 
      mcds, unit, linked_list_start, DPP_MCDS_TYPE_VALUE_EGRESS, &old_group_entries));

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(unit, FALSE, group_mcid,
      DPP_MCDS_TYPE_VALUE_EGRESS, QAX_MULT_MAX_EGRESS_REPLICATIONS - nof_reps, &group_size));
    if (group_size > QAX_MULT_MAX_EGRESS_REPLICATIONS - nof_reps) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list(unit, DPP_MCDS_TYPE_VALUE_EGRESS_START, group_mcid,
      DPP_MC_EGRESS_LINK_PTR_END, 0, 0, FALSE));
    if (mcds->out_err) { 
        if (mcds->out_err == _SHR_E_PARAM) {
            mcds->out_err = _SHR_E_EXISTS;
        }
    } else if (old_group_entries != DPP_MC_EGRESS_LINK_PTR_END) {
        
        SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_EGRESS));
    }

exit:
    if (out_err != NULL && _rv == SOC_E_NONE) {
        *out_err = mcds->out_err;
    }
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_reps_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   
    SOC_SAND_IN  uint32                nof_reps,     
    SOC_SAND_IN  dpp_mc_replication_t  *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint16 group_size;
    uint8 is_open;
    uint32 old_group_entries = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);

    SOCDNX_IF_ERR_EXIT(qax_egress_group_open_get(unit, group_mcid, &is_open));
    if (!is_open) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(unit, TRUE, group_mcid,
        DPP_MCDS_TYPE_VALUE_EGRESS, QAX_MULT_MAX_EGRESS_REPLICATIONS, &group_size));

    
    SOCDNX_IF_ERR_EXIT(qax_mult_remove_replications(unit, DPP_MCDS_TYPE_VALUE_EGRESS, nof_reps, reps));
    if (mcds->out_err) { 
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( 
      mcds, unit, QAX_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid), DPP_MCDS_TYPE_VALUE_EGRESS, &old_group_entries));

    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list( 
        unit, DPP_MCDS_TYPE_VALUE_EGRESS_START, group_mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, FALSE));
    if (mcds->out_err) { 
#ifndef QAX_EGRESS_MC_DELETE_FAILS_ON_FULL_MCDB
        
        SOCDNX_IF_ERR_EXIT(qax_mcds_remove_replications_from_group(unit, group_mcid, DPP_MCDS_TYPE_VALUE_EGRESS, nof_reps, reps));
        mcds->out_err = _SHR_E_NONE;
#endif 
    } else if (old_group_entries != DPP_MC_EGRESS_LINK_PTR_END) {
        
        SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_EGRESS));
    }

exit:
    if (out_err != NULL && _rv == SOC_E_NONE) {
        *out_err = mcds->out_err;
    }
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_eg_group_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx,
    SOC_SAND_OUT uint32       *mc_group_size
)
{
    uint8 is_open;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mc_group_size);

    SOCDNX_IF_ERR_EXIT(qax_mult_eg_get_group(
      unit, multicast_id_ndx, 0, 0, 0, 0, mc_group_size, &is_open));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_create(
                 int         unit,
                 dpp_mc_id_t multicast_id_ndx 
)
{
    uint32 data[SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32] = {0};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_set(unit, multicast_id_ndx, data));
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, multicast_id_ndx, 1)); 
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_close(
                  int          unit,
                  dpp_mc_id_t  multicast_id_ndx
)
{
    uint32 data[SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32] = {0};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_set(unit, multicast_id_ndx, data));
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, multicast_id_ndx, 0)); 
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_update(
                  int                                   unit,
                  dpp_mc_id_t                           multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    if (multicast_id_ndx >= SOC_DPP_CONFIG(unit)->tm.nof_mc_bitmap_ids) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_set(unit, multicast_id_ndx, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_add(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *add_bm, 
                  SOC_TMC_ERROR                         *out_err 
)
{
    unsigned nof_tm_port_words = SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    dpp_mc_egr_bitmap_t bitmap = {0};
    const uint32 *changes = &add_bm->bitmap[0];
    uint32 *to_write = bitmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= SOC_DPP_CONFIG(unit)->tm.nof_mc_bitmap_ids) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_get(unit, multicast_id_ndx, bitmap));

    for (; nof_tm_port_words; --nof_tm_port_words) { 
        if (*to_write & *changes) {
            *out_err = _SHR_E_EXISTS; 
            SOC_EXIT;
        }
        *(to_write++) |= *(changes++);
    }

    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_set(unit, multicast_id_ndx, bitmap));
    *out_err = _SHR_E_NONE;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_remove(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *rem_bm, 
                  SOC_TMC_ERROR                         *out_err 
)
{
    unsigned nof_tm_port_words = SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    dpp_mc_egr_bitmap_t bitmap = {0};
    const uint32 *changes = &rem_bm->bitmap[0];
    uint32 *to_write = bitmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= SOC_DPP_CONFIG(unit)->tm.nof_mc_bitmap_ids) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_get(unit, multicast_id_ndx, bitmap));

    for (; nof_tm_port_words; --nof_tm_port_words) { 
        if (~*to_write & *changes) {
            *out_err = _SHR_E_NOT_FOUND; 
            SOC_EXIT;
        }
        *(to_write++) &= ~*(changes++);
    }

    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_set(unit, multicast_id_ndx, bitmap));
    *out_err = _SHR_E_NONE;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 qax_mult_eg_bitmap_group_get(
                  int                                   unit,
                  dpp_mc_id_t                           bitmap_id, 
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    SOCDNX_IF_ERR_EXIT(qax_eg_mcdb_bitmap_get(unit, bitmap_id, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}




uint32 qax_mcid_type_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  mcid,
    SOC_SAND_IN  uint32 is_ingress 
)
{
  uint32 entry[2] = {0};
  const unsigned bit_offset = mcid % QAX_MC_MCIDS_PER_IS_ING_MC_ENTRY;
  const int table_index = mcid / QAX_MC_MCIDS_PER_IS_ING_MC_ENTRY;

  SOCDNX_INIT_FUNC_DEFS;
  if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID out of range")));
  }

  SOCDNX_IF_ERR_EXIT(READ_CGM_IS_ING_MCm(unit, MEM_BLOCK_ANY, table_index, entry));

  if (((*entry >> bit_offset) & 1) != is_ingress) { 
    *entry &= ~(((uint32)1) << bit_offset);
    *entry |= (((uint32)is_ingress) << bit_offset);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_IS_ING_MCm(unit, MEM_BLOCK_ANY, table_index, entry));
  }
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 qax_mcid_type_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  mcid,
    SOC_SAND_OUT uint32 *is_ingress 
)
{
  uint32 entry[2];
  const unsigned bit_offset = mcid % QAX_MC_MCIDS_PER_IS_ING_MC_ENTRY;
  const int table_index = mcid / QAX_MC_MCIDS_PER_IS_ING_MC_ENTRY;

  SOCDNX_INIT_FUNC_DEFS;
  if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID out of range")));
  }
  SOCDNX_NULL_CHECK(is_ingress);

  SOCDNX_IF_ERR_EXIT(READ_CGM_IS_ING_MCm(unit, MEM_BLOCK_ANY, table_index, entry));
  *is_ingress = ((*entry) >> bit_offset) & 1;

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC uint32 qax_gport2ing_mc_dest_encoding(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_gport_t gport,          
    SOC_SAND_OUT uint32     *dest_encoding, 
    SOC_SAND_OUT uint32     *is_bitmap      
)
{
    uint32 dest;
    SOC_TMC_DEST_INFO dest_info = {0};
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(dpp_gport_to_tm_dest_info(unit, gport, &dest_info));
    dest = dest_info.id;
    *is_bitmap = 0;

    
    if (dest_info.type == SOC_TMC_DEST_TYPE_QUEUE) { 
        if (dest > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Queue %u, is over %u."),
              (unsigned)dest, (unsigned)SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue));
        }
        dest |= (QAX_MC_ING_DESTINATION_Q_TYPE << QAX_MC_ING_DESTINATION_ID_BITS);
    } else if (dest_info.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) { 
        if (dest >= SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("system port out of range")));
        }
        dest |= (QAX_MC_ING_DESTINATION_SYSPORT_TYPE << QAX_MC_ING_DESTINATION_ID_BITS);
    } else if (dest_info.type == SOC_TMC_DEST_TYPE_LAG) { 
        dest |= (QAX_MC_ING_DESTINATION_SYSPORT_LAG_TYPE << QAX_MC_ING_DESTINATION_ID_BITS);
    } else if (dest_info.type == SOC_TMC_DEST_TYPE_MULTICAST) { 
        if (dest >= SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmaps) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ingress multicast bitmap ID %u, is not under %u."),
              (unsigned)dest, (unsigned)SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmaps));
        }
        *is_bitmap = 1;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported ingress multicast destination type in given gport")));
    }
    *dest_encoding = dest;
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 qax_mcds_copy_ingress_replications_from_arrays(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint8     do_clear,    
    SOC_SAND_IN uint32    arrays_size, 
    SOC_SAND_IN soc_multicast_replication_t *reps 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint32 size_left = arrays_size, dest, cud1, cud2, is_bitmap = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (do_clear) {
        qax_mcds_clear_replications(unit, mcds, DPP_MCDS_TYPE_VALUE_INGRESS);
    }
    DPP_MC_ASSERT(mcds->group_type == DPP_MCDS_TYPE_VALUE_INGRESS && mcds->nof_reps == (int32)mcds->nof_dest_cud_reps + mcds->nof_bitmap_reps);
    if (mcds->nof_reps + arrays_size > QAX_MULT_MAX_INGRESS_REPLICATIONS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }
    for (; size_left; --size_left, ++reps) {
        cud1 = (reps->encap1 != BCM_IF_INVALID && reps->encap1) ? reps->encap1 : SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
        cud2 = reps->flags & SOC_MUTICAST_REPLICATION_ENCAP2_VALID ? reps->encap2 : DPP_MC_NO_2ND_CUD;
        if (cud1 > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud || cud2 > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(
              "Invalid ingress encapsulation ID, should be between 0 and 0x%x"), SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
        }
        
        
        SOCDNX_IF_ERR_EXIT(qax_gport2ing_mc_dest_encoding(unit, reps->port, &dest, &is_bitmap));
        if (is_bitmap) {
            qax_add_ingress_replication_bitmap(mcds, dest * SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes + mcds->ingress_bitmap_start, cud1, cud2);
        } else {
            qax_add_ingress_replication_dest(mcds, dest, cud1, cud2);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 qax_convert_soc2dpp_rep_array(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      arrays_size, 
    SOC_SAND_IN  soc_multicast_replication_t *reps,       
    SOC_SAND_OUT dpp_mc_replication_t        *oreps       
)
{
    uint32 size_left = arrays_size, dest, is_bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    for (; size_left; --size_left, ++reps) {
        oreps->cud = reps->encap1 ? reps->encap1 : SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
        oreps->additional_cud = reps->flags & SOC_MUTICAST_REPLICATION_ENCAP2_VALID ? reps->encap2 : DPP_MC_NO_2ND_CUD;
        if (oreps->cud > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud || oreps->additional_cud > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(
              "Invalid ingress encapsulation ID, should be between 0 and 0x%x"), SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
        }
        
        
        SOCDNX_IF_ERR_EXIT(qax_gport2ing_mc_dest_encoding(unit, reps->port, &dest, &is_bitmap));
        if (is_bitmap) {
            dest |= ARAD_MC_EGR_IS_BITMAP_BIT; 
        }
        oreps->dest = dest;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_group_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  dpp_mc_id_t                 mcid,
    SOC_SAND_IN  soc_multicast_replication_t *reps,
    SOC_SAND_IN  uint32                      mc_group_size,
    SOC_SAND_IN  uint8                       allow_create, 
    SOC_SAND_IN  uint8                       to_delete,    
    SOC_SAND_OUT SOC_TMC_ERROR               *out_err      
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    int group_exists = 0, group_start_alloced = 0, failed = 1;
    uint32 group_entry_type;
    uint32 old_group_entries = QAX_MC_INGRESS_LINK_PTR_END; 

    SOCDNX_INIT_FUNC_DEFS;
    if (mc_group_size) {
        SOCDNX_NULL_CHECK(reps);
        DPP_MC_ASSERT(!to_delete);
        if (mc_group_size > QAX_MULT_MAX_INGRESS_REPLICATIONS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ingress MC group size is too big")));
        }
    }
    mcds->out_err = _SHR_E_NONE;
    DPP_MC_ASSERT(mcid < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids);

    group_entry_type = QAX_MCDS_GET_TYPE(mcds, mcid);
    group_exists = group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS_START;
    if (!group_exists && !allow_create) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    if (group_exists) { 
        DPP_MC_ASSERT(group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS_START);
        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, mcid, DPP_MCDS_TYPE_VALUE_INGRESS, &old_group_entries));
    } else if (mcid != DPP_MC_EGRESS_LINK_PTR_END) {
        
        if (DPP_MCDS_TYPE_IS_USED(group_entry_type)) {
            DPP_MC_ASSERT(!DPP_MCDS_TYPE_IS_START(group_entry_type));
            SOCDNX_IF_ERR_EXIT(qax_mcdb_relocate_entries(unit, mcid, 0, 0)); 
            if (mcds->out_err) { 
                SOC_EXIT;
            }
        } else { 
            SOCDNX_IF_ERR_EXIT(qax_mcds_reserve_group_start(mcds, mcid));
        }
        group_start_alloced = 1;
    }

    SOCDNX_IF_ERR_EXIT(qax_mcds_copy_ingress_replications_from_arrays(unit, TRUE, mc_group_size, reps)); 
    
    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list(unit, DPP_MCDS_TYPE_VALUE_INGRESS_START, mcid, QAX_MC_INGRESS_LINK_PTR_END, 0, 0, to_delete));
    if (mcds->out_err) { 
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcid_type_set_unsafe(unit, mcid, to_delete ? 0 : 1));

    if (to_delete) {
        
        mcds->start_entry = mcds->free_value;
        SOCDNX_IF_ERR_EXIT(qax_mc_end_linked_list(unit, to_delete));
    }

    if (old_group_entries != QAX_MC_INGRESS_LINK_PTR_END) { 
        SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_INGRESS));
    }

    failed = 0;
exit:
    if (out_err != NULL && _rv == SOC_E_NONE) {
        *out_err = mcds->out_err;
    }
    if (group_start_alloced & failed) { 
        DPP_MC_ASSERT(!group_exists);
        if(qax_mcds_build_free_blocks(unit, mcds, mcid, mcid, qax_mcds_get_region(mcds, mcid), McdsFreeBuildBlocksAdd_AllMustBeUsed) != SOC_E_NONE){
            cli_out("qax_mcds_build_free_blocks failed\n");
        }
    }
    SOCDNX_FUNC_RETURN;
}






uint32 qax_mult_ing_group_open(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            multicast_id_ndx, 
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,        
    SOC_SAND_IN  uint32                 mc_group_size,    
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err          
)
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(qax_mult_ing_group_set(unit, multicast_id_ndx, (SOC_SAND_IN soc_multicast_replication_t *)mc_group, mc_group_size, TRUE, FALSE, out_err));
exit:
  SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_group_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    mcid 
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    SOC_TMC_ERROR out_err;
    uint8 does_exist;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(qax_mult_does_group_exist(unit, mcid, FALSE, &does_exist));
    if (does_exist) { 
        SOCDNX_IF_ERR_EXIT(qax_mult_ing_group_set(unit, mcid, 0, 0, FALSE, TRUE, &out_err)); 
        if (out_err) { 
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
        }

        if (mcid != DPP_MC_EGRESS_LINK_PTR_END) { 
            SOCDNX_IF_ERR_EXIT(qax_mcds_build_free_blocks(
              unit, mcds, mcid, mcid, &mcds->ingress_starts, McdsFreeBuildBlocksAdd_AllMustBeUsed));
        } else {
            QAX_MCDS_SET_TYPE(mcds, DPP_MC_EGRESS_LINK_PTR_END, DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_group_update(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  dpp_mc_id_t            mcid,          
    SOC_SAND_IN  SOC_TMC_MULT_ING_ENTRY *mc_group,     
    SOC_SAND_IN  uint32                 mc_group_size, 
    SOC_SAND_OUT SOC_TMC_ERROR          *out_err       
)
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(qax_mult_ing_group_set(
    unit, mcid, (SOC_SAND_IN soc_multicast_replication_t *)mc_group, mc_group_size, FALSE, FALSE, out_err));
exit:
  SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_add_replications(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  dpp_mc_id_t                 mcid,     
    SOC_SAND_IN  uint32                      nof_reps, 
    SOC_SAND_IN  soc_multicast_replication_t *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR               *out_err  
)
{
    uint16 group_size;
    uint32 old_group_entries = QAX_MC_INGRESS_LINK_PTR_END; 
    qax_mcds_t *mcds = dpp_get_mcds(unit);

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);
    if (QAX_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_copy_ingress_replications_from_arrays(unit, TRUE, nof_reps, reps));

    
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( 
      mcds, unit, mcid, DPP_MCDS_TYPE_VALUE_INGRESS, &old_group_entries));

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(unit, FALSE, mcid,
      DPP_MCDS_TYPE_VALUE_INGRESS, QAX_MULT_MAX_INGRESS_REPLICATIONS - nof_reps, &group_size));
    if (group_size > QAX_MULT_MAX_INGRESS_REPLICATIONS - nof_reps) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }
    DPP_MC_ASSERT(mcds->nof_reps <= QAX_MULT_MAX_INGRESS_REPLICATIONS);

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list(unit, DPP_MCDS_TYPE_VALUE_INGRESS_START, mcid,
      QAX_MC_INGRESS_LINK_PTR_END, 0, 0, FALSE));
    if (mcds->out_err) { 
        if (out_err != NULL) {
            if (mcds->out_err == _SHR_E_PARAM) {
                *out_err = _SHR_E_EXISTS;
            } else {
                *out_err = mcds->out_err;
            }
        }
    } else if (old_group_entries != QAX_MC_INGRESS_LINK_PTR_END) {
        
        SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_INGRESS));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_remove_replications(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  dpp_mc_id_t                 mcid,     
    SOC_SAND_IN  uint32                      nof_reps, 
    SOC_SAND_IN  soc_multicast_replication_t *reps,    
    SOC_SAND_OUT SOC_TMC_ERROR               *out_err  
)
{
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    uint16 group_size;
    uint32 old_group_entries = QAX_MC_INGRESS_LINK_PTR_END;
    dpp_mc_replication_t stack_reps[10], *dpp_reps = stack_reps;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);

    if (QAX_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(unit, TRUE, mcid,
        DPP_MCDS_TYPE_VALUE_INGRESS, QAX_MULT_MAX_INGRESS_REPLICATIONS, &group_size));

    if (nof_reps > 10) { 
        dpp_reps = NULL;
        SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &dpp_reps, sizeof(*dpp_reps) * nof_reps, "dppreps2remove"));
    }
    
    SOCDNX_IF_ERR_EXIT(qax_convert_soc2dpp_rep_array(unit, nof_reps, reps, dpp_reps));

    
    SOCDNX_IF_ERR_EXIT(qax_mult_remove_replications(unit, DPP_MCDS_TYPE_VALUE_INGRESS, nof_reps, dpp_reps));
    if (mcds->out_err) { 
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, mcid, DPP_MCDS_TYPE_VALUE_INGRESS, &old_group_entries)); 

    SOCDNX_IF_ERR_EXIT(qax_mcds_set_linked_list( 
        unit, DPP_MCDS_TYPE_VALUE_INGRESS_START, mcid, QAX_MC_INGRESS_LINK_PTR_END, 0, 0, FALSE));
    if (mcds->out_err) { 
#ifndef QAX_EGRESS_MC_DELETE_FAILS_ON_FULL_MCDB
        
        SOCDNX_IF_ERR_EXIT(qax_mcds_remove_replications_from_group(unit, mcid, DPP_MCDS_TYPE_VALUE_INGRESS, nof_reps, dpp_reps));
        mcds->out_err = _SHR_E_NONE;
#endif 
    } else if (old_group_entries != QAX_MC_INGRESS_LINK_PTR_END) {
        
        SOCDNX_IF_ERR_EXIT(qax_mcdb_free_linked_list(unit, old_group_entries, DPP_MCDS_TYPE_VALUE_INGRESS));
    }

exit:
    if (out_err != NULL && _rv == SOC_E_NONE) {
        *out_err = mcds->out_err;
    }
    if (dpp_reps != stack_reps) {
        sand_free_mem_if_not_null(unit, (void*)&dpp_reps);
    }
    SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_group_size_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx,
    SOC_SAND_OUT uint32      *mc_group_size
)
{
  uint8 is_open;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mc_group_size);

  SOCDNX_IF_ERR_EXIT(qax_mult_ing_get_group(unit, multicast_id_ndx, 0, 0, 0, NULL, mc_group_size, &is_open));

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 qax_mult_ing_get_group(
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


  SOCDNX_IF_ERR_EXIT(qax_mult_does_group_exist(unit, group_mcid, FALSE, is_open));
  if (*is_open) {
    uint16 group_size;
    SOCDNX_IF_ERR_EXIT(qax_mcds_get_group(
      unit, TRUE, group_mcid, DPP_MCDS_TYPE_VALUE_INGRESS, mc_group_size, &group_size));
    *exact_mc_group_size = group_size;
    SOCDNX_IF_ERR_EXIT(qax_mcds_copy_replications_to_arrays(unit, 0, mc_group_size, ports, cuds, reps));
  } else { 
    *exact_mc_group_size = 0;
  }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t qax_mult_get_entry(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 mcdb_index,
    SOC_SAND_OUT uint32 *entry 
)
{
    const qax_mcds_t* mcds = dpp_get_mcds(unit);
    qax_mcdb_entry_t *mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, mcdb_index);
    SOCDNX_INIT_FUNC_DEFS;

    if (mcdb_index >= QAX_NOF_MCDB_ENTRIES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCDB entry %u is out of range"), mcdb_index));
    }
    if (DPP_MCDS_TYPE_IS_USED(QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry))) { 
        entry[0] = mcdb_entry->word0;
        entry[1] = mcdb_entry->word1;
        entry[2] = mcdb_entry->word2 & QAX_MC_ENTRY_MASK_VAL;
    } else {
        entry[0] = QAX_MC_FREE_ENTRY_0;
        entry[1] = QAX_MC_FREE_ENTRY_1;
        entry[2] = QAX_MC_FREE_ENTRY_2;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


