/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON





#include <shared/bsl.h>
#include <soc/dpp/drv.h>


#include <shared/swstate/access/sw_state_access.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_pat_tree.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/drv.h>






#define SOC_SAND_PAT_TREE_NOT_PREFIX 0XFF






#define SOC_SAND_PAT_TREE_CACHE_INST(_inst) (SOC_SAND_BIT(31)|(_inst))

#define SOC_SAND_PAT_TREE_ACTIVE_INST(_arr_mem_info, _inst) \
  ((_arr_mem_info->pat_tree_data.cache_enabled)?SOC_SAND_PAT_TREE_CACHE_INST(_inst):(_inst))


#define SOC_SAND_PAT_TREE_NODE_INFO_GET(_tree_info_ndx, _place, _node, _node_ref,exit_num)   \
  if(_tree_info_ndx->node_ref_get_fun == NULL){  \
    _tree_info_ndx->node_get_fun(  \
            _tree_info_ndx->prime_handle,  \
            SOC_SAND_PAT_TREE_ACTIVE_INST(_tree_info_ndx,_tree_info_ndx->sec_handle),  \
            _place, \
            _node \
          ); \
    _node_ref = _node; \
  }\
  else  \
  { \
    _tree_info_ndx->node_ref_get_fun(  \
            _tree_info_ndx->prime_handle,  \
            SOC_SAND_PAT_TREE_ACTIVE_INST(_tree_info_ndx,_tree_info_ndx->sec_handle),  \
            _place, \
            &_node_ref \
          ); \
  } \



















STATIC  uint8
  soc_sand_pat_tree_compare_key(
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key1,
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key2,
    SOC_SAND_IN uint8             prefix
 );

STATIC  uint8
  soc_sand_pat_tree_get_bit(
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key,
    SOC_SAND_IN uint8             prefix
  );



STATIC  uint8
  soc_sand_pat_tree_matching_differing_bit(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key1,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key2
  );



STATIC  uint8
  soc_sand_pat_tree_matching_bits(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key1,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key2
  );


STATIC  uint8
  soc_sand_pat_tree_matching_differing_bit_in_prefix(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY    *key1_p,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY    *key2_p
  );

STATIC uint32
  soc_sand_pat_tree_alloc_node(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_PLACE    *place
  );


STATIC uint32 soc_sand_pat_tree_free_node(
  SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
  SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   place
);

STATIC SOC_SAND_PAT_TREE_NODE_PLACE
  soc_sand_pat_tree_lpm_query(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO         *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  uint8                  identical_payload
  );

STATIC uint32
  soc_sand_pat_tree_get_block_ip_prefix_order(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO           *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE     current_node_place,
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_NODE_PLACE  *iter,
    SOC_SAND_INOUT  uint32                *entries_to_scan,
    SOC_SAND_INOUT  uint32                *entries_to_get,
    SOC_SAND_INOUT  uint8               *get_flag,
    SOC_SAND_INOUT  uint8                 prefix,
    SOC_SAND_INOUT  uint32                *readen,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *nodes,
    SOC_SAND_OUT uint32                   *nof_entries
  );

STATIC uint8
  soc_sand_pat_tree_default_node_data_is_identical_fun(
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
    SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  );

STATIC uint32
   soc_sand_pat_tree_foreach_mark_invalid(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO         *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   node_place
  );

uint32 
  soc_sand_pat_tree_create(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  )
{
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    res;
  int 
    counter;
  int
    tree_unit;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx->node_set_fun);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx->node_get_fun);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx->root_set_fun);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx->root_get_fun);

  if(tree_info_ndx->node_data_is_identical_fun == NULL)
  {
    tree_info_ndx->node_data_is_identical_fun = soc_sand_pat_tree_default_node_data_is_identical_fun;
  }

  if (tree_info_ndx->tree_size == 0 || tree_info_ndx->tree_size == 0xFFFFFFFF )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  if(!tree_info_ndx->pat_tree_data.tree_memory) {

    soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);

    btmp_init_info.size = tree_info_ndx->tree_size;
    btmp_init_info.support_cache = tree_info_ndx->support_cache;
    tree_unit = tree_info_ndx->pat_tree_data.memory_use_unit;

    
    
    if (!SOC_WARM_BOOT(tree_unit)) {
            res = soc_sand_occ_bm_create(
                tree_unit,
                &btmp_init_info,
                &(tree_info_ndx->pat_tree_data.memory_use)
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.counter.get(tree_unit, &counter);
            sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.occ_bm_hndl.set(tree_unit, counter, tree_info_ndx->pat_tree_data.memory_use);
            counter++;
            sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.counter.set(tree_unit, counter);
    }
    else {
        sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.counter.get(tree_unit, &counter);
        sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.occ_bm_hndl.get(tree_unit, counter, &(tree_info_ndx->pat_tree_data.memory_use));
        counter++;
        sw_state_access[tree_unit].dpp.soc.arad.pp.ipv4_pat.counter.set(tree_unit, counter);
    }

    res =
        soc_sand_occ_bm_get_ptr_from_handle(
            tree_unit,
            tree_info_ndx->pat_tree_data.memory_use,
            &(tree_info_ndx->pat_tree_data.memory_use_ptr)) ;
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit) ;

    tree_info_ndx->pat_tree_data.tree_memory = (SOC_SAND_PAT_TREE_NODE*) soc_sand_os_malloc_any_size(tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE), "tree_info_ndx->pat_tree_data.tree_memory");
    if (!(tree_info_ndx->pat_tree_data.tree_memory))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
    }
    soc_sand_os_memset(tree_info_ndx->pat_tree_data.tree_memory, 0, tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
    if (tree_info_ndx->support_cache)
    {
      tree_info_ndx->pat_tree_data.tree_memory_cache = (SOC_SAND_PAT_TREE_NODE*) soc_sand_os_malloc_any_size(tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE), "tree_info_ndx->pat_tree_data.tree_memory_cache");
      if (!(tree_info_ndx->pat_tree_data.tree_memory_cache))
      {
        soc_sand_os_free_any_size(tree_info_ndx->pat_tree_data.tree_memory);
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
      }
      soc_sand_os_memset(tree_info_ndx->pat_tree_data.tree_memory_cache, 0, tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
    }
  }
  tree_info_ndx->pat_tree_data.root = SOC_SAND_PAT_TREE_NULL;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_create()",0,0);
}



uint32
  soc_sand_pat_tree_destroy(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  )
{
  uint32
    res;
  uint8
    in_use ;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_DESTROY);

  soc_sand_os_free_any_size(tree_info_ndx->pat_tree_data.tree_memory);
  if (tree_info_ndx->support_cache)
  {
    soc_sand_os_free_any_size(tree_info_ndx->pat_tree_data.tree_memory_cache);
  }
  res = soc_sand_occ_is_bitmap_active(tree_info_ndx->pat_tree_data.memory_use_unit,tree_info_ndx->pat_tree_data.memory_use,&in_use) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  if (!SOC_IS_DETACHING(tree_info_ndx->pat_tree_data.memory_use_unit)) {
      
      if (in_use) {
          res = soc_sand_occ_bm_destroy(
                  tree_info_ndx->pat_tree_data.memory_use_unit,
                  tree_info_ndx->pat_tree_data.memory_use
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

          tree_info_ndx->pat_tree_data.memory_use = soc_sand_occ_bm_get_illegal_bitmap_handle() ;
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_destroy()",0,0);

}


uint32
  soc_sand_pat_tree_clear(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_CLEAR);

  res = soc_sand_occ_bm_clear(
          tree_info_ndx->pat_tree_data.memory_use_unit,
          tree_info_ndx->pat_tree_data.memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = tree_info_ndx->root_set_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          SOC_SAND_PAT_TREE_NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_clear()",0,0);

}


uint32
  soc_sand_pat_tree_clear_nodes(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  )
{
  uint32
    res;
  SOC_SAND_PAT_TREE_NODE_PLACE
    root_node_place;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_CLEAR);


  res = tree_info_ndx->root_get_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          &root_node_place
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(root_node_place == SOC_SAND_PAT_TREE_NULL)
  {
    goto exit;
  }

  res = soc_sand_pat_tree_foreach_mark_invalid(tree_info_ndx, root_node_place);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = tree_info_ndx->root_set_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          SOC_SAND_PAT_TREE_NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_clear()",0,0);
}



uint32
  soc_sand_pat_tree_root_reset(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_CLEAR);

  res = tree_info_ndx->root_set_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          SOC_SAND_PAT_TREE_NULL
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_root_reset()",0,0);
}

uint32
  soc_sand_pat_tree_node_add(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO      *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_DATA    *node_data,
    SOC_SAND_OUT  uint8                 *success
  )
{
  uint8
    match;
  uint8
    next_node,
    parent_to_curr;
  SOC_SAND_PAT_TREE_NODE_PLACE
    current_node_place,
    parent_node_place,
    new_node_place,
    glue_node_place;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref=NULL,
    *parent_node_ref=NULL;
  SOC_SAND_PAT_TREE_NODE
    current_node,
    parent_node;
  SOC_SAND_PAT_TREE_NODE
    new_node,
    glue_node;
  uint8
    differ_bit;
  SOC_SAND_PAT_TREE_KEY
    key;
  uint8
    prefix,
    cached;
  uint32
    data;
  SOC_SAND_PAT_TREE_NODE_KEY
    *head_change_ptr;
  SOC_SAND_PAT_TREE_NODE_KEY
    head_change_cpy;
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PAT_TREE_NODE_ADD);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx);
  SOC_SAND_CHECK_NULL_INPUT(node_key);
  SOC_SAND_CHECK_NULL_INPUT(node_data);
  SOC_SAND_CHECK_NULL_INPUT(success);



  *success = TRUE;

  prefix = node_key->prefix;
  key = node_key->val;
  data = node_data->payload;

  cached = tree_info_ndx->pat_tree_data.cache_enabled;

  
  if (cached) {

    head_change_ptr = &(tree_info_ndx->pat_tree_data.cache_change_head);

    if (head_change_ptr->prefix == SOC_SAND_PAT_TREE_PREFIX_INVALID)  
    {
        if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
        {
            soc_sand_os_memcpy(head_change_ptr, node_key, sizeof(SOC_SAND_PAT_TREE_NODE_KEY));
        }
        else
        {
            unit = (tree_info_ndx->prime_handle);

            res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
                                            node_key, 
                                            tree_info_ndx->sec_handle);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
        }
    }
    else {
      
      differ_bit = soc_sand_pat_tree_matching_differing_bit_in_prefix(head_change_ptr,node_key);
      
      if (differ_bit < SOC_SAND_PAT_TREE_PREFIX_MAX_LEN) {
          if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
          {
              
              head_change_ptr->prefix = differ_bit;
              
              head_change_ptr->val &= ~(((uint32) -1) >> differ_bit);
          }
          else
          {

            unit = (tree_info_ndx->prime_handle);

            head_change_cpy = tree_info_ndx->pat_tree_data.cache_change_head;

            head_change_cpy.prefix = differ_bit;

            
            head_change_cpy.val &= ~(((uint32) -1) >> differ_bit);

            res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
                                            &(head_change_cpy), 
                                            tree_info_ndx->sec_handle);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
              
          }
      }
    }
    cached = 0;
  }

 
  parent_to_curr = 0;
  soc_sand_os_memset(&current_node,0x0, sizeof(SOC_SAND_PAT_TREE_NODE));


  parent_node_place = SOC_SAND_PAT_TREE_NULL;
  res = tree_info_ndx->root_get_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          &current_node_place
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  next_node = 0;
 
  while ( current_node_place != SOC_SAND_PAT_TREE_NULL)
  {

    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, current_node_place, &current_node, current_node_ref,10);
    parent_to_curr = next_node;
    if (current_node_ref->prefix >= prefix)
    {
      break;
    }
    match = soc_sand_pat_tree_compare_key(
              current_node_ref->key,
              key,
              current_node_ref->prefix
            );
    if (!match)
    {
      break;
    }

    next_node = soc_sand_pat_tree_get_bit(key, current_node_ref->prefix);
    parent_node_place = current_node_place;
    current_node_place = current_node_ref->child[next_node];
  }
  if (parent_node_place != SOC_SAND_PAT_TREE_NULL)
  {
    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, parent_node_place, &parent_node, parent_node_ref,30);
  }

 
  if(current_node_place == SOC_SAND_PAT_TREE_NULL)
  {
    res = soc_sand_pat_tree_alloc_node(
            tree_info_ndx,
            &new_node_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   
    if (new_node_place == SOC_SAND_PAT_TREE_NULL)
    {
      *success = FALSE;
      goto exit;
    }

    soc_sand_os_memset(&new_node, 0, sizeof(new_node));

    new_node.child[0] = SOC_SAND_PAT_TREE_NULL;
    new_node.child[1] = SOC_SAND_PAT_TREE_NULL;
    new_node.key = key;
    new_node.prefix = prefix;
    new_node.is_prefix = SAND_PAT_TREE_NODE_FLAGS_PREFIX;
    new_node.data = data;

    res = tree_info_ndx->node_set_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            new_node_place,
            &new_node
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

   
   
    if (parent_node_place == SOC_SAND_PAT_TREE_NULL)
    {
      res = tree_info_ndx->root_set_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              new_node_place
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
    else
    {
      parent_node_ref->child[next_node] = new_node_place;
      res = tree_info_ndx->node_set_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              parent_node_place,
              parent_node_ref
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    }

    goto exit;
  }

 
  differ_bit = soc_sand_pat_tree_matching_bits(
                 current_node_ref->key,
                 key
               );

 
  if(current_node_ref->prefix >= prefix && differ_bit >= prefix)
  {
    if(current_node_ref->prefix == prefix)
    {
      
      current_node_ref->is_prefix = TRUE;
      current_node_ref->key = (uint32)(current_node_ref->key & (~((((uint32)1) << (32 -current_node_ref->prefix)) - 1)));
      current_node_ref->data = data;
      res = tree_info_ndx->node_set_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              current_node_place,
              current_node_ref
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);


      goto exit;
    }
    else
    {
     

      next_node = soc_sand_pat_tree_get_bit(current_node_ref->key, prefix);

      res = soc_sand_pat_tree_alloc_node(
              tree_info_ndx,
              &new_node_place
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
     
      if (new_node_place == SOC_SAND_PAT_TREE_NULL)
      {
        *success = FALSE;
        goto exit;
      }
      new_node.child[next_node] = current_node_place;
      new_node.child[1 - next_node] = SOC_SAND_PAT_TREE_NULL;
      new_node.key = key;
      new_node.prefix = prefix;
      new_node.is_prefix = SAND_PAT_TREE_NODE_FLAGS_PREFIX;
      new_node.data = data;
     
      res = tree_info_ndx->node_set_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              new_node_place,
              &new_node
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

     
      parent_node_ref->child[parent_to_curr] = new_node_place;
      res = tree_info_ndx->node_set_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              parent_node_place,
              parent_node_ref
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      goto exit;
    }
  }
  else
  {
   
    res = soc_sand_pat_tree_alloc_node(
            tree_info_ndx,
            &new_node_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    if (new_node_place == SOC_SAND_PAT_TREE_NULL)
    {
      *success = FALSE;
      goto exit;
    }
    new_node.child[0] = SOC_SAND_PAT_TREE_NULL;
    new_node.child[1] = SOC_SAND_PAT_TREE_NULL;
    new_node.key = key;
    new_node.prefix = prefix;
    new_node.is_prefix = SAND_PAT_TREE_NODE_FLAGS_PREFIX;
    new_node.data = data;

    res = soc_sand_pat_tree_alloc_node(
            tree_info_ndx,
            &glue_node_place
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    if (glue_node_place == SOC_SAND_PAT_TREE_NULL)
    {
      *success = FALSE;
      goto exit;
    }

    next_node = soc_sand_pat_tree_get_bit(current_node_ref->key, differ_bit);
    glue_node.child[next_node] = current_node_place;
    next_node = soc_sand_pat_tree_get_bit(key, differ_bit);
    glue_node.child[next_node] = new_node_place;
    glue_node.key = key;
    glue_node.prefix = differ_bit;
    glue_node.is_prefix = FALSE; 
    
    parent_node_ref->child[parent_to_curr] = glue_node_place;
    res = tree_info_ndx->node_set_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            current_node_place,
            current_node_ref
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);


    res = tree_info_ndx->node_set_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            glue_node_place,
            &glue_node
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);


    res = tree_info_ndx->node_set_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            parent_node_place,
            parent_node_ref
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);


    res = tree_info_ndx->node_set_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            new_node_place,
            &new_node
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_node_add()",0,0);
}




uint32
  soc_sand_pat_tree_node_remove(
    SOC_SAND_INOUT SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key
  )
{
  uint8
    match;
  uint8
    next_node,
    parent_to_curr,
    gr_to_par;
  SOC_SAND_PAT_TREE_NODE_PLACE
    current_node_place,
    valid_node,
    parent_node_place,
    grand_node_place;
  SOC_SAND_PAT_TREE_NODE
    current_node,
    parent_node,
    grand_node;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref=NULL,
    *parent_node_ref=NULL,
    *grand_node_ref=NULL;
  SOC_SAND_PAT_TREE_KEY
    key;
  uint8
    prefix,
    cached;
  uint32
    differ_bit;
  SOC_SAND_PAT_TREE_NODE_KEY
    *head_change_ptr;
  SOC_SAND_PAT_TREE_NODE_KEY
    head_change_cpy;
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PAT_TREE_NODE_REMOVE);

  soc_sand_os_memset(&current_node,0x0, sizeof(SOC_SAND_PAT_TREE_NODE));
  key = node_key->val;
  prefix = node_key->prefix;

  cached = tree_info_ndx->pat_tree_data.cache_enabled;

  
  if (cached) {

    head_change_ptr = &(tree_info_ndx->pat_tree_data.cache_change_head);

    if (head_change_ptr->prefix == SOC_SAND_PAT_TREE_PREFIX_INVALID) 
    {
        if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
        {
            soc_sand_os_memcpy(head_change_ptr,node_key,sizeof(SOC_SAND_PAT_TREE_NODE_KEY));
        }
        else
        {
            unit = (tree_info_ndx->prime_handle);

            res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
                                            node_key, 
                                            tree_info_ndx->sec_handle);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
        }
    }
    else {
      
      differ_bit = soc_sand_pat_tree_matching_differing_bit_in_prefix(head_change_ptr,node_key);
      
      if (differ_bit < SOC_SAND_PAT_TREE_PREFIX_MAX_LEN) {
          if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
          {
              
              head_change_ptr->prefix = differ_bit;
              
              head_change_ptr->val &= ~(((uint32) -1) >> differ_bit);
          }
          else
          {

              unit = (tree_info_ndx->prime_handle);

              head_change_cpy = tree_info_ndx->pat_tree_data.cache_change_head;

              head_change_cpy.prefix = differ_bit;

              
              head_change_cpy.val &= ~(((uint32) -1) >> differ_bit);

              res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
                                              &(head_change_cpy), 
                                              tree_info_ndx->sec_handle);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
              
          }
      }
    }
    cached = 0;
  }


  parent_node_place = SOC_SAND_PAT_TREE_NULL;
  grand_node_place = SOC_SAND_PAT_TREE_NULL;
  res = tree_info_ndx->root_get_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          &current_node_place
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  gr_to_par = 0;
  parent_to_curr = 0;
  next_node = 0;

 
  while ( current_node_place != SOC_SAND_PAT_TREE_NULL)
  {
    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, current_node_place, &current_node, current_node_ref,20);

    gr_to_par = parent_to_curr;
    parent_to_curr = next_node;
    next_node = soc_sand_pat_tree_get_bit(key, current_node_ref->prefix);
    if (current_node_ref->prefix >= prefix)
    {
      break;
    }
    match = soc_sand_pat_tree_compare_key(
              current_node_ref->key,
              key,
              current_node_ref->prefix
            );
    if (!match)
    {
      break;
    }
    grand_node_place = parent_node_place;
    parent_node_place = current_node_place;
    current_node_place = current_node_ref->child[next_node];
  }

  
  if (parent_node_place != SOC_SAND_PAT_TREE_NULL)
  {
    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, parent_node_place, &parent_node, parent_node_ref,30);
  }

  if (grand_node_place != SOC_SAND_PAT_TREE_NULL)
  {
    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, grand_node_place, &grand_node, grand_node_ref,40);
  }

 

 
  if ( (current_node_place != SOC_SAND_PAT_TREE_NULL) && current_node_ref->prefix == prefix)
  {
    match = soc_sand_pat_tree_compare_key(
              current_node_ref->key,
              key,
              current_node_ref->prefix
            );
    if (!match)
    {
      goto exit;
    }
   
    if((current_node_ref->child[0] != SOC_SAND_PAT_TREE_NULL) && (current_node_ref->child[1] != SOC_SAND_PAT_TREE_NULL))
    {
     current_node_ref->is_prefix = 0;
     res = tree_info_ndx->node_set_fun(
             tree_info_ndx->prime_handle,
             SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
             current_node_place,
             current_node_ref
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


     
    }
   
    else if((current_node_ref->child[0] == SOC_SAND_PAT_TREE_NULL) && (current_node_ref->child[1] == SOC_SAND_PAT_TREE_NULL))
    {
      res = soc_sand_pat_tree_free_node(
              tree_info_ndx,
              current_node_place
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

     
      if (parent_node_place == SOC_SAND_PAT_TREE_NULL)
      {
        res = tree_info_ndx->root_set_fun(
                tree_info_ndx->prime_handle,
                SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
                SOC_SAND_PAT_TREE_NULL
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
      else
      {
        parent_node_ref->child[parent_to_curr] = SOC_SAND_PAT_TREE_NULL;

        res = tree_info_ndx->node_set_fun(
                tree_info_ndx->prime_handle,
                SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
                parent_node_place,
                parent_node_ref
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);


       
        if(!(parent_node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX))
        {
         
          grand_node_ref->child[gr_to_par] = parent_node_ref->child[1-parent_to_curr];

          res = soc_sand_pat_tree_free_node(
                  tree_info_ndx,
                  parent_node_place
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


          res = tree_info_ndx->node_set_fun(
                  tree_info_ndx->prime_handle,
                  SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
                  grand_node_place,
                  grand_node_ref
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        }
      }
    }
   
   
    else
    {
      if (current_node_ref->child[0] != SOC_SAND_PAT_TREE_NULL)
      {
        valid_node = current_node_ref->child[0];
      }
      else
      {
        valid_node = current_node_ref->child[1];
      }

      if (parent_node_place == SOC_SAND_PAT_TREE_NULL)
      {
        res = tree_info_ndx->root_set_fun(
                tree_info_ndx->prime_handle,
                SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
                valid_node
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
      }
      else
      {
        parent_node_ref->child[parent_to_curr] = valid_node;

        res = tree_info_ndx->node_set_fun(
                tree_info_ndx->prime_handle,
                SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
                parent_node_place,
                parent_node_ref
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      }
      res = soc_sand_pat_tree_free_node(
              tree_info_ndx,
              current_node_place
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_node_remove()",0,0);
}



uint32
  soc_sand_pat_tree_lpm_get(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  uint8                  identical_payload,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *lpm_info,
    SOC_SAND_OUT  uint8                 *found

  )
{
  SOC_SAND_PAT_TREE_NODE
    node;
  SOC_SAND_PAT_TREE_NODE
    *node_ref=NULL;
  SOC_SAND_PAT_TREE_NODE_PLACE
    node_place;


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_LPM_GET);

  node_place = soc_sand_pat_tree_lpm_query(
                tree_info_ndx,
                node_key,
                identical_payload
              );
  if (node_place == SOC_SAND_PAT_TREE_NULL)
  {
    *found = FALSE;
    goto exit;
  }

  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_place, &node, node_ref,10);

  lpm_info->data.payload = node_ref->data;
  lpm_info->key.val = node_ref->key;
  lpm_info->key.prefix = node_ref->prefix;
  lpm_info->node_place = node_place;
  *found = TRUE;

exit:

    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_lpm_get()",0,0);
}




uint32
  soc_sand_pat_tree_node_at_place(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE  node_place,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *lpm_info,
    SOC_SAND_OUT  uint8                 *exist
  )
{
  SOC_SAND_PAT_TREE_NODE
    node;
  SOC_SAND_PAT_TREE_NODE
    *node_ref=NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_LPM_GET);

  if (node_place == SOC_SAND_PAT_TREE_NULL)
  {
    *exist = FALSE;
    goto exit;
  }
  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_place, &node, node_ref,10);

  
  lpm_info->data.payload = node_ref->data;
  lpm_info->key.val = node_ref->key;
  lpm_info->key.prefix = node_ref->prefix;
  lpm_info->node_place = node_place;

  *exist = TRUE;

exit:

    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_node_at_place()",0,0);
}
uint32
  soc_sand_pat_tree_get_block(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO         *tree_info_ndx,
    SOC_SAND_IN SOC_SAND_PAT_TREE_ITER_TYPE     iter_type,
    SOC_SAND_INOUT  SOC_SAND_U64                *iter,
    SOC_SAND_IN  uint32                   entries_to_scan,
    SOC_SAND_IN  uint32                   entries_to_get,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *nodes,
    SOC_SAND_OUT uint32                   *nof_entries
  )
{
  SOC_SAND_PAT_TREE_NODE_PLACE
    node_indx,
    tree_root;
  SOC_SAND_PAT_TREE_NODE
    current_node;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref=NULL;
  uint8
    prefix;
  uint32
    to_scan,
    to_get,
    scanned,
    readen;
  uint8
    used,
    get_falg;
  SOC_SAND_PAT_TREE_NODE_PLACE
    rec_iter;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_GET_BLOCK);

  scanned = 0;
  readen = 0;
  *nof_entries = 0;


  if (iter_type == SOC_SAND_PAT_TREE_ITER_TYPE_FAST)
  {
    if (iter->arr[0] == SOC_SAND_PAT_TREE_NULL)
    {
      goto exit;
    }
    node_indx = iter->arr[0];
    for (; scanned < entries_to_scan && readen < entries_to_get &&
           node_indx < tree_info_ndx->tree_size; ++node_indx)
    {
      res = soc_sand_occ_bm_is_occupied(
              tree_info_ndx->pat_tree_data.memory_use_unit,
              tree_info_ndx->pat_tree_data.memory_use,
              node_indx,
              &used
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      ++scanned;
      if (used)
      {
        SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_indx, &current_node, current_node_ref,20);
        if (current_node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX)
        {
          nodes[readen].data.payload = current_node_ref->data;
          nodes[readen].key.val = current_node_ref->key;
          nodes[readen].key.prefix = current_node_ref->prefix;
          ++readen;
        }
      }
    }
    if (node_indx < tree_info_ndx->tree_size)
    {
      iter->arr[0] = node_indx;
    }
    else
    {
      iter->arr[0] = SOC_SAND_PAT_TREE_NULL;
      iter->arr[1] = SOC_SAND_PAT_TREE_NULL;
    }
    *nof_entries = readen;
    goto exit;
  }
  if (iter_type == SOC_SAND_PAT_TREE_ITER_TYPE_KEY_PREFIX_ORDERED)
  {
    if (iter->arr[0] == SOC_SAND_PAT_TREE_NULL)
    {
      goto exit;
    }

    if (iter->arr[0] == 0)
    {
      res = tree_info_ndx->root_get_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              &rec_iter
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      get_falg = TRUE;
    }
    else
    {
      rec_iter = iter->arr[0];
      get_falg = FALSE;
    }
    to_scan = entries_to_scan;
    to_get = entries_to_get;


    res = tree_info_ndx->root_get_fun(
            tree_info_ndx->prime_handle,
            SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
            &tree_root
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    soc_sand_pat_tree_get_block_ip_prefix_order(
      tree_info_ndx,
      tree_root,
      &rec_iter,
      &to_scan,
      &to_get,
      &get_falg,
      SOC_SAND_PAT_TREE_NOT_PREFIX,
      &readen,
      nodes,
      nof_entries
    );
    if (to_get == 0 || to_scan == 0)
    {
      iter->arr[0] = rec_iter;
      iter->arr[1] = 0;
    }
    else
    {
      iter->arr[0] = SOC_SAND_PAT_TREE_NULL;
      iter->arr[1] = SOC_SAND_PAT_TREE_NULL;
    }
    goto exit;
  }

  if (iter_type == SOC_SAND_PAT_TREE_ITER_TYPE_PREFIX_KEY_ORDERED)
  {
    if (iter->arr[1] == SOC_SAND_PAT_TREE_NULL)
    {
      goto exit;
    }

    if (iter->arr[0] == 0)
    {
      res = tree_info_ndx->root_get_fun(
              tree_info_ndx->prime_handle,
              SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
              &rec_iter
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      get_falg = TRUE;
    }
    else
    {
      rec_iter = iter->arr[0];
      get_falg = FALSE;
    }
    prefix = (uint8)iter->arr[1];

    to_scan = entries_to_scan;
    to_get = entries_to_get;

    for (;prefix <= SOC_SAND_PAT_TREE_PREFIX_MAX_LEN && to_scan && to_get; ++prefix)
    {
       res = tree_info_ndx->root_get_fun(
               tree_info_ndx->prime_handle,
               SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
               &tree_root
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      res = soc_sand_pat_tree_get_block_ip_prefix_order(
              tree_info_ndx,
              tree_root,
              &rec_iter,
              &to_scan,
              &to_get,
              &get_falg,
              prefix,
              &readen,
              nodes,
              nof_entries
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    }
    if (to_get == 0 || to_scan == 0)
    {
      iter->arr[0] = rec_iter;
      iter->arr[1] = prefix - 1;
    }
    else
    {
      iter->arr[0] = SOC_SAND_PAT_TREE_NULL;
      iter->arr[1] = SOC_SAND_PAT_TREE_NULL;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_get_block()",0,0);
}




uint32
  soc_sand_pat_tree_get_size(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_OUT  uint32                    *used_nodes,
    SOC_SAND_OUT  uint32                    *prefix_nodes,
    SOC_SAND_OUT  uint32                    *free_nodes
  )
{
  SOC_SAND_PAT_TREE_NODE
    current_node;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref=NULL;
  SOC_SAND_PAT_TREE_NODE_PLACE
    indx;
  uint8
    used;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_PAT_TREE_GET_SIZE);

  *prefix_nodes = 0;
  *used_nodes = 0;

  for (indx  = 0; indx < tree_info_ndx->tree_size; ++indx )
  {
      res = soc_sand_occ_bm_is_occupied(
              tree_info_ndx->pat_tree_data.memory_use_unit,
              tree_info_ndx->pat_tree_data.memory_use,
              indx,
              &used
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (used)
    {
      ++(*used_nodes);
      SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, indx, &current_node, current_node_ref,20);

      if (current_node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX)
      {
        ++(*prefix_nodes);
      }
    }
  }

  *free_nodes = tree_info_ndx->tree_size - *used_nodes;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_get_size()",0,0);
}


#if SOC_SAND_DEBUG

uint32
  soc_sand_pat_tree_print(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   node_place,
    SOC_SAND_IN  uint8                  including_glue,
    SOC_SAND_IN  uint8                  print_subtree
  )
{
  SOC_SAND_PAT_TREE_NODE
    node;
  SOC_SAND_PAT_TREE_NODE
    *node_ref=NULL;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  if(node_place == SOC_SAND_PAT_TREE_NULL)
  {
     goto exit;
  }
  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_place, &node, node_ref,10);

 
  if (node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX|| including_glue)
  {
    uint32 masked_key = (uint32)(node_ref->key & (~((((uint32)1) << (32 -node_ref->prefix)) - 1)));
    LOG_CLI((BSL_META_U(unit,
                        " %08x/%d %#4x\n"), masked_key, node_ref->prefix, (uint32)node_ref->data));
  }
  if (print_subtree)
  {
    res = soc_sand_pat_tree_print(
            tree_info_ndx,
            node_ref->child[0],
            including_glue,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_pat_tree_print(
            tree_info_ndx,
            node_ref->child[1],
            including_glue,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_print()",0,0);
}





#endif 



STATIC uint32
  soc_sand_pat_tree_get_block_ip_prefix_order(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO           *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE     current_node_place,
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_NODE_PLACE  *iter,
    SOC_SAND_INOUT  uint32                *entries_to_scan,
    SOC_SAND_INOUT  uint32                *entries_to_get,
    SOC_SAND_INOUT  uint8               *get_flag,
    SOC_SAND_INOUT  uint8                 prefix,
    SOC_SAND_INOUT  uint32                *readen,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_INFO   *nodes,
    SOC_SAND_OUT uint32                   *nof_entries
  )
{
  SOC_SAND_PAT_TREE_NODE
    current_node;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref=NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  if (current_node_place == SOC_SAND_PAT_TREE_NULL || *entries_to_scan == 0 || *entries_to_get == 0)
  {
    goto exit;
  }
  soc_sand_os_memset(&current_node,0x0, sizeof(SOC_SAND_PAT_TREE_NODE));

  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, current_node_place, &current_node, current_node_ref,10);

  if (*get_flag)
  {
    if (current_node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX)
    {
      if (prefix == SOC_SAND_PAT_TREE_NOT_PREFIX || prefix == current_node_ref->prefix )
      {
        nodes[*readen].data.payload = current_node_ref->data;
        nodes[*readen].key.val = current_node_ref->key;
        nodes[*readen].key.prefix = current_node_ref->prefix;
        ++(*readen);
        --(*entries_to_get);
        ++(*nof_entries);
      }
    }
    --(*entries_to_scan);
   
    *iter = current_node_place;
  }
 
  if (!*get_flag && *iter == current_node_place)
  {
    *get_flag = TRUE;
  }

  soc_sand_pat_tree_get_block_ip_prefix_order(
    tree_info_ndx,
    current_node_ref->child[0],
    iter,
    entries_to_scan,
    entries_to_get,
    get_flag,
    prefix,
    readen,
    nodes,
    nof_entries
  );

  soc_sand_pat_tree_get_block_ip_prefix_order(
    tree_info_ndx,
    current_node_ref->child[1],
    iter,
    entries_to_scan,
    entries_to_get,
    get_flag,
    prefix,
    readen,
    nodes,
    nof_entries
  );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_get_block_ip_prefix_order()",0,0);

}




STATIC SOC_SAND_PAT_TREE_NODE_PLACE
  soc_sand_pat_tree_lpm_query(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY     *node_key,
    SOC_SAND_IN  uint8                  identical_payload
  )
{
  uint8
    match;
  uint8
    next_node;
  SOC_SAND_PAT_TREE_NODE_PLACE
    current_node_place,
    best_node_place;
  SOC_SAND_PAT_TREE_NODE
    current_node,
    best_node;
  SOC_SAND_PAT_TREE_NODE
    *current_node_ref,
    *best_node_ref=NULL;
  SOC_SAND_PAT_TREE_KEY
    key;
  SOC_SAND_PAT_TREE_NODE_PLACE
      *glbl_current_node_place;
  uint8
    prefix;
  int unit;
  uint32 res;


  best_node_place = 0;
  prefix = node_key->prefix;
  key = node_key->val;


  glbl_current_node_place = (SOC_SAND_PAT_TREE_NODE_PLACE*)&(tree_info_ndx->pat_tree_data.current_node_place);
  
  if(identical_payload&SOC_SAND_PAT_TREE_FIND_LOAD_NODE){
       if(*glbl_current_node_place == SOC_SAND_PAT_TREE_NULL) {
           match = 0;
       }
       else{
           current_node_place = *glbl_current_node_place;
           SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, current_node_place, &current_node, current_node_ref,10);
           match = soc_sand_pat_tree_compare_key(
                     current_node_ref->key,
                     key,
                     current_node_ref->prefix
                   );
       }
       if(!match) {
           tree_info_ndx->root_get_fun(
             tree_info_ndx->prime_handle,
             SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
             &current_node_place
           );
       }
    }
    else{
        tree_info_ndx->root_get_fun(
          tree_info_ndx->prime_handle,
          SOC_SAND_PAT_TREE_ACTIVE_INST(tree_info_ndx,tree_info_ndx->sec_handle),
          &current_node_place
        );
    }


 
  while ( current_node_place != SOC_SAND_PAT_TREE_NULL)
  {
    SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, current_node_place, &current_node, current_node_ref,10);
   
    if ((current_node_ref->is_prefix  & SAND_PAT_TREE_NODE_FLAGS_PREFIX) && (prefix >= current_node_ref->prefix))
    {
     
      match = soc_sand_pat_tree_compare_key(
                current_node_ref->key,
                key,
                current_node_ref->prefix
              );
      if (match)
      {
        
        best_node_place = current_node_place;
        if(identical_payload & SOC_SAND_PAT_TREE_FIND_STORE_NODE) 
        {
            if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
            {
                *glbl_current_node_place = best_node_place;
            }
            else
            {
                unit = (tree_info_ndx->prime_handle);

                res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CURRENT_NODE_PLACE, 
                                                &(best_node_place), 
                                                tree_info_ndx->sec_handle);
                if(SOC_SAND_OK != res)
                    LOG_INFO(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "ERROR: SOC_DPP_WB_ENGINE_SET_ARR res =  %d\n\r"), res));
            }   
        }
      }
    }
   
    if((current_node_ref->prefix >= prefix))
    {
     

      
      if ((identical_payload & SOC_SAND_PAT_TREE_FIND_EXACT))
      {
        if(current_node_ref->is_prefix & SAND_PAT_TREE_NODE_FLAGS_PREFIX && current_node_ref->prefix == prefix && soc_sand_pat_tree_compare_key(current_node_ref->key, key, current_node_ref->prefix)){
            return best_node_place;
        }
        else{
            return SOC_SAND_PAT_TREE_NULL;
        }
      }
      
      if (!(identical_payload & SOC_SAND_PAT_TREE_FIND_IDENTICAL))
      {
        return best_node_place;
      }

      
      
      if(soc_sand_pat_tree_compare_key(current_node_ref->key, key, prefix))
      {
       

        SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, best_node_place, &best_node, best_node_ref,10);


        if (best_node_ref->child[0] != SOC_SAND_PAT_TREE_NULL || best_node_ref->child[1] != SOC_SAND_PAT_TREE_NULL )
        {
            return SOC_SAND_PAT_TREE_NULL;
        }
        else{
            return best_node_place;
        }

      }
      else
      {
        
        return best_node_place;
      }
    }
   
    else
    {
      
      if(soc_sand_pat_tree_compare_key(current_node_ref->key, key, current_node_ref->prefix))
      {
        next_node = soc_sand_pat_tree_get_bit(key, current_node_ref->prefix);
        current_node_place = current_node_ref->child[next_node]; 
      }
      else 
      {
        
        if ((identical_payload & SOC_SAND_PAT_TREE_FIND_EXACT))
        {
           return SOC_SAND_PAT_TREE_NULL;
        }
        else{
            return(best_node_place);
        }

      }
    }
  }
    if (identical_payload & SOC_SAND_PAT_TREE_FIND_EXACT)
    {
       return SOC_SAND_PAT_TREE_NULL;
    }
    else{
        return(best_node_place);
    }
}

uint32
  soc_sand_pat_tree_head_key_of_changes(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO     *tree_info_ndx,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_KEY     *cached_head
  ){

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);


  soc_sand_os_memcpy(cached_head,&(tree_info_ndx->pat_tree_data.cache_change_head),sizeof(SOC_SAND_PAT_TREE_NODE_KEY));

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_head_key_of_changes()",0,0);
}


STATIC uint32
  soc_sand_pat_tree_clear_changes(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO     *tree_info_ndx
  )
{
    SOC_SAND_PAT_TREE_NODE_KEY
        head_change_cpy;
    int unit = BSL_UNIT_UNKNOWN;
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
    {
        tree_info_ndx->pat_tree_data.cache_change_head.prefix = SOC_SAND_PAT_TREE_PREFIX_INVALID;
    }
    else
    {
        unit = (tree_info_ndx->prime_handle);

        head_change_cpy = tree_info_ndx->pat_tree_data.cache_change_head;

        head_change_cpy.prefix = SOC_SAND_PAT_TREE_PREFIX_INVALID;

        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_CHANGE_HEAD, 
                                        &(head_change_cpy), 
                                        tree_info_ndx->sec_handle);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_clear_changes()",0,0);
}





STATIC uint32
  soc_sand_pat_tree_alloc_node(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
    SOC_SAND_OUT  SOC_SAND_PAT_TREE_NODE_PLACE    *place
  )
{
  uint8
    found;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  res = soc_sand_occ_bm_get_next(
          tree_info_ndx->pat_tree_data.memory_use_unit,
          tree_info_ndx->pat_tree_data.memory_use,
          place,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!found)
  {
    *place = SOC_SAND_PAT_TREE_NULL;
  }
  else
  {
    res = soc_sand_occ_bm_occup_status_set(
            tree_info_ndx->pat_tree_data.memory_use_unit,
            tree_info_ndx->pat_tree_data.memory_use,
            *place,
            1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_alloc_node()",0,0);
}


STATIC uint32 soc_sand_pat_tree_free_node(
  SOC_SAND_PAT_TREE_INFO       *tree_info_ndx,
  SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   place
)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  if (place > tree_info_ndx->tree_size)
  {
    goto exit;
  }
  res = soc_sand_occ_bm_occup_status_set(
          tree_info_ndx->pat_tree_data.memory_use_unit,
          tree_info_ndx->pat_tree_data.memory_use,
          place,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_free_node()",0,0);
}

STATIC  uint8
  soc_sand_pat_tree_compare_key(
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key1,
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key2,
    SOC_SAND_IN uint8             prefix
  )
{
  if(prefix == 0)
  {
    return TRUE;
  }

  return(uint8)(((key1^key2) >> (32-prefix))==0);
}



STATIC  uint8
  soc_sand_pat_tree_get_bit(
    SOC_SAND_IN SOC_SAND_PAT_TREE_KEY   key,
    SOC_SAND_IN uint8             prefix
  )
{
  uint8
    val,
    bit_place;

  bit_place = sizeof(SOC_SAND_PAT_TREE_KEY) * SOC_SAND_NOF_BITS_IN_CHAR - 1 - prefix;

  val = (key >> bit_place) & 1;


  return val;
}



STATIC  uint8
  soc_sand_pat_tree_matching_differing_bit(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key1,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key2
  )
{

  uint32
    bit_val;
  uint8
    bit_place,
    lsb,
    nof_bits = sizeof(SOC_SAND_PAT_TREE_KEY) * SOC_SAND_NOF_BITS_IN_CHAR,
    msb;
  uint8
    accuracy;
  SOC_SAND_PAT_TREE_KEY xor_key;


  accuracy = nof_bits / 2;

  bit_place = 0;
  xor_key = key1 ^ key2;
  msb = nof_bits - 1;
  while(accuracy != 0)
  {
    lsb = nof_bits - accuracy - bit_place;

    
    bit_val = ((xor_key & ((1 << msb) - 1)) >> lsb);

    
    if(!bit_val)
    {
      bit_place = (uint8)bit_place + accuracy;
    }
    accuracy /= 2;
  }
  return bit_place;

}



STATIC  uint8
  soc_sand_pat_tree_matching_differing_bit_in_prefix(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY    *key1_p,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_KEY    *key2_p
  )
{

  uint32
    bit_val,
    key1 = key1_p->val,
    key2 = key2_p->val;
  uint8
    bit_place,
    prefix_bits,
    lsb,
    nof_bits=32;
  uint8
    accuracy,
    left;
  SOC_SAND_PAT_TREE_KEY 
    xor_key;


  
  if (key1_p->prefix < key2_p->prefix) {
    prefix_bits = key1_p->prefix;
  }
  else{
    prefix_bits = key2_p->prefix;
  }

  
  if (key1_p->val == key2_p->val) {
    return prefix_bits;
  }

  accuracy = (prefix_bits+1) / 2;

  bit_place = 0; 
  xor_key = key1 ^ key2;
  left = prefix_bits - accuracy;

  while(accuracy != 0)
  {
    lsb = nof_bits - accuracy - bit_place;

    bit_val = ((xor_key) >> lsb);

    
    



    
    if(!bit_val)
    {
      bit_place = (uint8)bit_place + accuracy;
    }

    
    if (left == 0) {
        break;
    }

    
    accuracy = (left + 1)/2;
    left -= accuracy;

  }

  return bit_place;
}



STATIC  uint8
  soc_sand_pat_tree_matching_bits(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key1,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_KEY    key2
  )
{
  if(key1 == key2)
  {
    return sizeof(SOC_SAND_PAT_TREE_KEY) * SOC_SAND_NOF_BITS_IN_CHAR;
  }
  return soc_sand_pat_tree_matching_differing_bit(key1, key2);
}

STATIC uint8
  soc_sand_pat_tree_default_node_data_is_identical_fun(
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_0,
  SOC_SAND_IN SOC_SAND_PAT_TREE_NODE       *node_info_1
  )
{
  return (uint8)(node_info_1->data == node_info_0->data && node_info_0->key == node_info_1->key && 
	  node_info_0->prefix == node_info_1->prefix);
}

STATIC uint32
   soc_sand_pat_tree_foreach_imp(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO         *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   node_place,
    SOC_SAND_IN     SOC_SAND_PAT_TREE_FOREACH_FUN  fun,
    SOC_SAND_INOUT     void                        *param
  )
{
  SOC_SAND_PAT_TREE_NODE
    node;
  SOC_SAND_PAT_TREE_NODE
    *node_ref=NULL;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  if(node_place == SOC_SAND_PAT_TREE_NULL)
  {
     goto exit; 
  }

  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_place, &node, node_ref,10);

 
  res = soc_sand_pat_tree_foreach_imp(
          tree_info_ndx,
          node_ref->child[0],
          fun,
          param
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_pat_tree_foreach_imp(
          tree_info_ndx,
          node_ref->child[1],
          fun,
          param
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  fun(tree_info_ndx, node_ref->key, node_ref->prefix, &node_ref->data, node_ref->is_prefix, param);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_foreach()",0,0);
}


STATIC uint32
   soc_sand_pat_tree_foreach_mark_invalid(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO         *tree_info_ndx,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_NODE_PLACE   node_place
  )
{
  SOC_SAND_PAT_TREE_NODE
    node;
  SOC_SAND_PAT_TREE_NODE
    *node_ref=NULL;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  if(node_place == SOC_SAND_PAT_TREE_NULL)
  {
     goto exit; 
  }

  SOC_SAND_PAT_TREE_NODE_INFO_GET(tree_info_ndx, node_place, &node, node_ref,10);
 
  if(node_ref->child[0] != SOC_SAND_PAT_TREE_NULL)
  {
    res = soc_sand_pat_tree_foreach_mark_invalid(
            tree_info_ndx,
            node_ref->child[0]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if(node_ref->child[1] != SOC_SAND_PAT_TREE_NULL)
  {
    res = soc_sand_pat_tree_foreach_mark_invalid(
            tree_info_ndx,
            node_ref->child[1]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = soc_sand_pat_tree_free_node(
        tree_info_ndx,
        node_place
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_foreach_mark_invalid()",0,0);
}


uint32
   soc_sand_pat_tree_foreach(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO         *tree_info,
    SOC_SAND_IN     SOC_SAND_PAT_TREE_FOREACH_FUN  fun,
    SOC_SAND_INOUT     void                        *param
  )
{
  return soc_sand_pat_tree_foreach_imp(tree_info, 0, fun, param);
}

uint32
  soc_sand_pat_tree_cache_set(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint8                 enable_cache
  )
{
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx);

  if (!tree_info_ndx->support_cache && enable_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  if (tree_info_ndx->pat_tree_data.cache_enabled == enable_cache)
  {
    goto exit;
  }




  if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
  {
      tree_info_ndx->pat_tree_data.cache_enabled = enable_cache;
  }
  else
  {
      unit = (tree_info_ndx->prime_handle);

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_CACHE_ENABLED, 
                                      &(enable_cache), 
                                      tree_info_ndx->sec_handle);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
  }



 
  res = soc_sand_occ_bm_cache_set(tree_info_ndx->pat_tree_data.memory_use_unit, tree_info_ndx->pat_tree_data.memory_use,enable_cache);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  if (enable_cache)
  {

      if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
      {
          soc_sand_os_memcpy(tree_info_ndx->pat_tree_data.tree_memory_cache,tree_info_ndx->pat_tree_data.tree_memory,tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
          tree_info_ndx->pat_tree_data.root_cache = tree_info_ndx->pat_tree_data.root;
      }
      else
      {
          unit = (tree_info_ndx->prime_handle);


          res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, 
                                             tree_info_ndx->pat_tree_data.tree_memory,
                                             0,
                                             tree_info_ndx->tree_size);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);      

          res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, 
                                          &(tree_info_ndx->pat_tree_data.root), 
                                          tree_info_ndx->sec_handle);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);      
      }

      
      soc_sand_pat_tree_clear_changes(tree_info_ndx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_cache_set()",0,0);
}

uint32
  soc_sand_pat_tree_cache_commit(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint32               flags
  )
{
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx);

  if (!tree_info_ndx->support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
 
  if (!tree_info_ndx->pat_tree_data.cache_enabled)
  {
    goto exit;
  }
 
  res = soc_sand_occ_bm_cache_commit(tree_info_ndx->pat_tree_data.memory_use_unit, tree_info_ndx->pat_tree_data.memory_use,flags);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
  if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
  {
      soc_sand_os_memcpy(tree_info_ndx->pat_tree_data.tree_memory,tree_info_ndx->pat_tree_data.tree_memory_cache,tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
      tree_info_ndx->pat_tree_data.root = tree_info_ndx->pat_tree_data.root_cache;
  }
  else
  {
      unit = (tree_info_ndx->prime_handle);


      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY, 
                                         tree_info_ndx->pat_tree_data.tree_memory_cache,
                                         0,
                                         tree_info_ndx->tree_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_ROOT, 
                                      &(tree_info_ndx->pat_tree_data.root_cache), 
                                      tree_info_ndx->sec_handle);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      

  }
  
  soc_sand_pat_tree_clear_changes(tree_info_ndx);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_cache_commit()",0,0);
}

uint32
  soc_sand_pat_tree_cache_rollback(
    SOC_SAND_INOUT  SOC_SAND_PAT_TREE_INFO        *tree_info_ndx,
    SOC_SAND_IN  uint32              flags
  )
{
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(tree_info_ndx);

  if (!tree_info_ndx->support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
 
  if (!tree_info_ndx->pat_tree_data.cache_enabled)
  {
    goto exit;
  }
 
  res = soc_sand_occ_bm_cache_rollback(tree_info_ndx->pat_tree_data.memory_use_unit, tree_info_ndx->pat_tree_data.memory_use,flags);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
  if(SOC_DPP_WB_ENGINE_VAR_NONE == tree_info_ndx->wb_var_index)
  {
      soc_sand_os_memcpy(tree_info_ndx->pat_tree_data.tree_memory_cache,tree_info_ndx->pat_tree_data.tree_memory,tree_info_ndx->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
      tree_info_ndx->pat_tree_data.root_cache = tree_info_ndx->pat_tree_data.root;
  }
  else
  {
      unit = (tree_info_ndx->prime_handle);


      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_TREE_MEMORY_CACHE, 
                                         tree_info_ndx->pat_tree_data.tree_memory, 
                                         0,
                                         tree_info_ndx->tree_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, tree_info_ndx->wb_var_index + WB_ENGINE_PAT_TREE_AGREGATION_ROOT_CACHE, 
                                      &(tree_info_ndx->pat_tree_data.root), 
                                      tree_info_ndx->sec_handle);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      
  }

  
  soc_sand_pat_tree_clear_changes(tree_info_ndx);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_cache_rollback()",0,0);
}

uint32
  soc_sand_pat_tree_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO              *tree_info,
    SOC_SAND_IN  uint32                        flags,
    SOC_SAND_OUT  uint32                       *size
  )
{
  const SOC_SAND_PAT_TREE_T
    *tree_data;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(tree_info);
  SOC_SAND_CHECK_NULL_INPUT(size);

  
  tree_data = &(tree_info->pat_tree_data);

  cur_size = sizeof(SOC_SAND_PAT_TREE_INFO);
  total_size += cur_size;

  
  cur_size = tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE);
  total_size += cur_size;

  if (tree_info->pat_tree_data.cache_enabled)
  {
    cur_size = tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE);
    total_size += cur_size;
  }

  
  res = soc_sand_occ_bm_get_size_for_save(
            tree_data->memory_use_unit,
            tree_data->memory_use,
            &cur_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  total_size += cur_size;

  *size= total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_get_size_for_save()",0,0);
}


uint32
  soc_sand_pat_tree_save(
    SOC_SAND_IN  SOC_SAND_PAT_TREE_INFO     *tree_info,
    SOC_SAND_IN  uint32                flags,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  )
{
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_PAT_TREE_INFO
    *info;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(tree_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,tree_info,SOC_SAND_PAT_TREE_INFO,1);

  
  info = (SOC_SAND_PAT_TREE_INFO *) (cur_ptr - sizeof(SOC_SAND_PAT_TREE_INFO));
  soc_sand_os_memset(&info->pat_tree_data.tree_memory, 0x0, sizeof(SOC_SAND_PAT_TREE_NODE *));
  soc_sand_os_memset(&info->pat_tree_data.tree_memory_cache, 0x0, sizeof(SOC_SAND_PAT_TREE_NODE *));
  
  info->pat_tree_data.memory_use = soc_sand_occ_bm_get_illegal_bitmap_handle() ;
  soc_sand_os_memset(&info->node_get_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_GET));
  soc_sand_os_memset(&info->node_set_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_INFO_SET));
  soc_sand_os_memset(&info->node_data_is_identical_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_NODE_DATA_IS_IDENTICAL_FUN));
  soc_sand_os_memset(&info->node_is_skip_in_lpm_identical_data_query_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_NODE_IS_SKIP_IN_LPM_IDENTICAL_DATA_QUERY_FUN));
  soc_sand_os_memset(&info->node_ref_get_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_SW_DB_TREE_NODE_REF_GET));
  soc_sand_os_memset(&info->root_get_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_GET));
  soc_sand_os_memset(&info->root_set_fun, 0x0, sizeof(SOC_SAND_PAT_TREE_SW_DB_TREE_ROOT_SET));  

  
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,tree_info->pat_tree_data.tree_memory,SOC_SAND_PAT_TREE_NODE,tree_info->tree_size);
  
  if (tree_info->pat_tree_data.cache_enabled)
  {
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,tree_info->pat_tree_data.tree_memory_cache,SOC_SAND_PAT_TREE_NODE,tree_info->tree_size);
  }
  
  res = soc_sand_occ_bm_save(
          tree_info->pat_tree_data.memory_use_unit,
          tree_info->pat_tree_data.memory_use,
          cur_ptr,
          buffer_size_bytes - total_size,
          &cur_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  cur_ptr += cur_size;
  total_size += cur_size;

  *actual_size_bytes = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_save()",0,0);
}



uint32
  soc_sand_pat_tree_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_PAT_TREE_LOAD_INFO           *load_info,
    SOC_SAND_OUT SOC_SAND_PAT_TREE_INFO               *tree_info
  )
{
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    res;
  SOC_SAND_PAT_TREE_NODE_PLACE 
    root_holder;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(tree_info);
  SOC_SAND_CHECK_NULL_INPUT(load_info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  
  soc_sand_os_memcpy(tree_info, cur_ptr, sizeof(SOC_SAND_PAT_TREE_INFO));
  cur_ptr += sizeof(SOC_SAND_PAT_TREE_INFO);

  
  root_holder = tree_info->pat_tree_data.root;

  if (tree_info->tree_size == 0 || tree_info->tree_size == 0xFFFFFFFF )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  tree_info->node_set_fun = load_info->node_set_fun;
  tree_info->node_get_fun= load_info->node_get_fun;
  tree_info->node_ref_get_fun= load_info->node_ref_get_fun;
  tree_info->root_set_fun= load_info->root_set_fun;
  tree_info->root_get_fun= load_info->root_get_fun;
  tree_info->node_data_is_identical_fun=load_info->node_data_is_identical_fun;
  tree_info->node_is_skip_in_lpm_identical_data_query_fun= load_info->node_is_skip_in_lpm_identical_data_query_fun;

  
  res = soc_sand_pat_tree_create(tree_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  
  tree_info->pat_tree_data.root = root_holder;
  
  
  soc_sand_os_memcpy(tree_info->pat_tree_data.tree_memory, cur_ptr, tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
  cur_ptr += tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE);
  
  if (tree_info->pat_tree_data.cache_enabled)
  {
    soc_sand_os_memcpy(tree_info->pat_tree_data.tree_memory_cache, cur_ptr, tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE));
    cur_ptr += tree_info->tree_size * sizeof(SOC_SAND_PAT_TREE_NODE);
  }

  
  
  res = soc_sand_occ_bm_destroy(
          tree_info->pat_tree_data.memory_use_unit,
          tree_info->pat_tree_data.memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  
  res = soc_sand_occ_bm_load(
          tree_info->pat_tree_data.memory_use_unit,
          &cur_ptr,
          &tree_info->pat_tree_data.memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,40, exit);

  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pat_tree_load()",0,0);
}


void
  soc_sand_SAND_PAT_TREE_LOAD_INFO_clear(
    SOC_SAND_PAT_TREE_LOAD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PAT_TREE_LOAD_INFO));
   
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PAT_TREE_INFO_clear(
    SOC_SAND_PAT_TREE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PAT_TREE_INFO));
   
  info->wb_var_index     = SOC_DPP_WB_ENGINE_VAR_NONE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
