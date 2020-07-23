#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_kbp_entry_mgmt.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
























STATIC
  void
    arad_pp_ipv6_prefix_to_mask(
      SOC_SAND_IN  uint32 prefix,
      SOC_SAND_OUT uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S]
    )
{
  int32
    counter = prefix,
    word = 3;

  mask[0] = mask[1] = mask[2] = mask[3] = 0;

  while (counter >= 32)
  {
    mask[word--] = 0xffffffff;
    counter -= 32;
  }
  if (counter > 0)
  {
    mask[word] = SOC_SAND_BITS_MASK(31, 32 - counter);
  }
}


STATIC uint32
  arad_pp_ip_tcam_route_to_prio(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *key
  )
{
  uint32
    prio = SOC_SAND_U32_MAX;

  switch(key->type)
  {

  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)){
      prio = prio - (uint32) key->key.ipv6_static_src_bind.prefix_len;
    } else {
  	  
      prio = 0;
    }
    break;
  case ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION:
    prio = 0;
    break;

  default:

    break;
  }
   
  return prio;
}

STATIC
  uint32
    arad_pp_ip_tcam_key_to_stream(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *key,
      SOC_SAND_OUT uint8                       stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES]
  )
{
  uint32 tmp, long_stream[ARAD_PP_IP_TCAM_KEY_LEN_LONGS];
  uint8 start_bit;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  start_bit = 0;
  tmp = 0;

  res = SOC_SAND_OK; 
  sal_memset(long_stream, 0x0, (sizeof(uint32) * ARAD_PP_IP_TCAM_KEY_LEN_LONGS));
  SOC_SAND_CHECK_FUNC_RESULT(res,  20, exit);

  switch(key->type)
  {
      case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(key->key.ipv6_static_src_bind.sip6.address,
                                          128,
                                          long_stream);
      if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)) {
          tmp = key->key.ipv6_static_src_bind.prefix_len;
          ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_SAND_NOF_BITS_IN_CHAR, long_stream);
      }
      
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(&(key->key.ipv6_static_src_bind.lif_ndx), 
										   16,
										   long_stream);
      break;
    
    case ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION:
      ARAD_PP_IP_BIT_STREAM_SET_ANY_FIELD(key->key.ip6_compression.ip6.ipv6_address.address,
                                            128,
                                            long_stream);
      break;
    
    default:
#if defined(INCLUDE_KBP)
        {
            uint32 
                table_size_in_bytes,
                table_payload_in_bytes;

            res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(key->type), &table_size_in_bytes, &table_payload_in_bytes);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            
            sal_memcpy(stream, key->key.elk_fwd.m_data, table_size_in_bytes);
            sal_memcpy(&(stream[table_size_in_bytes]), key->key.elk_fwd.m_mask, table_size_in_bytes);
        }
#endif 
      break;
  }

  
  if (!ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(key->type)) {
      tmp = key->type;
      soc_sand_bitstream_set_any_field(&tmp, 157, 3, long_stream);

      res = soc_sand_U32_to_U8(
              long_stream,
              ARAD_PP_IP_TCAM_KEY_LEN_BYTES,
              stream
            );
      SOC_SAND_CHECK_FUNC_RESULT(res,  100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_ip_tcam_key_to_stream()",0,0);
}

STATIC
  uint32
    arad_pp_ip_tcam_stream_to_key(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  uint8                 stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES],
      SOC_SAND_IN  ARAD_IP_TCAM_ENTRY_TYPE   tcam_key_type,
      SOC_SAND_OUT ARAD_PP_IP_TCAM_ENTRY_KEY *key
    )
{
  uint32 long_stream[ARAD_PP_IP_TCAM_KEY_LEN_LONGS], tmp = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memset(key, 0x0, sizeof(*key));

#if defined(INCLUDE_KBP)
  if (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(tcam_key_type)) {
      uint32 
          res = SOC_SAND_OK,
          table_size_in_bytes,
          table_payload_in_bytes;

      res = arad_kbp_table_size_get(unit, ARAD_IP_TCAM_TO_KBP_FRWRD_IP_TBL_ID(tcam_key_type), &table_size_in_bytes, &table_payload_in_bytes);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      key->type = tcam_key_type;
      sal_memcpy(key->key.elk_fwd.m_data, stream, table_size_in_bytes);
      sal_memcpy(key->key.elk_fwd.m_mask, &(stream[table_size_in_bytes]), table_size_in_bytes);

      ARAD_DO_NOTHING_AND_EXIT;
  }
#endif 
 


  soc_sand_U8_to_U32(
    stream,
    ARAD_PP_IP_TCAM_KEY_LEN_BYTES,
    long_stream
  );

  soc_sand_bitstream_get_any_field(long_stream, 157, 3, &tmp);
  key->type = tmp;
  switch (key->type)
  {
  
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
  	soc_sand_bitstream_get_any_field(long_stream, 0, 128, key->key.ipv6_static_src_bind.sip6.address);
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)) {
	    soc_sand_bitstream_get_any_field(long_stream, 128, SOC_SAND_NOF_BITS_IN_CHAR, &tmp);
        key->key.ipv6_static_src_bind.prefix_len = (uint8) tmp;
	    soc_sand_bitstream_get_any_field(long_stream, 136, 16, &(key->key.ipv6_static_src_bind.lif_ndx));
    } else {
        
	    soc_sand_bitstream_get_any_field(long_stream, 128, 16, &(key->key.ipv6_static_src_bind.lif_ndx));
    }
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION:
    soc_sand_bitstream_get_any_field(long_stream, 0, 128, key->key.ip6_compression.ip6.ipv6_address.address);
    break;
  default:
    break;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_stream_to_key()", 0, 0);
}

STATIC
  void
    arad_pp_ip_tcam_entry_build(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY *key,
      SOC_SAND_OUT ARAD_TCAM_ENTRY           *entry
  )
{
  uint32 mask, ipv6_mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  ARAD_TCAM_ENTRY_clear(entry);
  
  entry->valid = TRUE;

  switch(key->type)
  {
  
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
    if (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)){
      arad_pp_ipv6_prefix_to_mask(key->key.ipv6_static_src_bind.prefix_len, ipv6_mask);
      soc_sand_bitstream_set_any_field(key->key.ipv6_static_src_bind.sip6.address, 0, 128, entry->value);
      soc_sand_bitstream_set_any_field(ipv6_mask,                                  0, 128, entry->mask);
      mask = SOC_SAND_BITS_MASK(15, 0);
      soc_sand_bitstream_set_any_field(&key->key.ipv6_static_src_bind.lif_ndx, 128, 16, entry->value);
      soc_sand_bitstream_set_any_field(&mask,                                  128, 16, entry->mask);
    } else {
      soc_sand_bitstream_set_any_field(key->key.ipv6_static_src_bind.sip6.address, 0, 128, entry->value);
      
      soc_sand_bitstream_set_any_field(&(key->key.ipv6_static_src_bind.lif_ndx), 128, 16, entry->value);
      
      soc_sand_bitstream_set_bit_range(entry->mask, 0, 143); 
    }
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION:
    soc_sand_bitstream_set_any_field(key->key.ipv6_static_src_bind.sip6.address, 0, 128, entry->value);
    
    soc_sand_bitstream_set_bit_range(entry->mask, 0, 127); 
    break;
  
  default:
    break;
  }
}

uint32
    arad_pp_frwrd_ip_tcam_ip_db_id_get(
      SOC_SAND_IN ARAD_IP_TCAM_ENTRY_TYPE entry_type
    )
{
  uint32
    tcam_db_id;

  switch (entry_type)
  {
  case ARAD_IP_TCAM_ENTRY_TYPE_IPV6_STATIC_SRC_BIND:
  	tcam_db_id = ARAD_PP_TCAM_STATIC_ACCESS_ID_SRC_BIND_IPV6;
    break;

  case ARAD_IP_TCAM_ENTRY_TYPE_SIPV6_COMPRESSION:
    tcam_db_id = ARAD_PP_TCAM_STATIC_ACCESS_ID_SIP;
    break;

  default:
    tcam_db_id = ARAD_TCAM_MAX_NOF_LISTS;
    break;
  }

  return tcam_db_id;
}


uint32
  arad_pp_ip_tcam_entry_hw_add(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key,
    SOC_SAND_IN  uint8                       is_for_update,
    SOC_SAND_IN  ARAD_TCAM_ACTION           *action,
    SOC_SAND_IN  uint32                     data_indx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
    uint32
      res = SOC_SAND_OK,
      tcam_db_id,
      priority;
    ARAD_TCAM_ENTRY
      entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(success);

  tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(route_key->type);
  priority = arad_pp_ip_tcam_route_to_prio(unit, route_key);
  arad_pp_ip_tcam_entry_build(unit, route_key, &entry);
  entry.is_for_update = is_for_update;
  res = arad_tcam_managed_db_entry_add_unsafe(
          unit,
          tcam_db_id,
          data_indx,
          FALSE,
          priority,
          &entry,
          action,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_entry_hw_add()", 0, 0);
}

#if defined(INCLUDE_KBP)
uint32
  arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(
     SOC_SAND_IN  uint8     *mask,
     SOC_SAND_IN  uint32    table_size_in_bytes,
     SOC_SAND_OUT uint32    *prefix_len
     )
{
  uint32 length;
  uint8 bytes_idx, bits_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(mask);
  SOC_SAND_CHECK_NULL_INPUT(prefix_len);

   
  length = table_size_in_bytes * SOC_SAND_NOF_BITS_IN_BYTE;

  for (bytes_idx = (table_size_in_bytes -1); bytes_idx > 0; bytes_idx--) {
       if (mask[bytes_idx] == SOC_SAND_U8_MAX) {
           length -= SOC_SAND_NOF_BITS_IN_BYTE;
       }
       else {
           break;
       }
  }
  for (bits_idx = 0; bits_idx < SOC_SAND_NOF_BITS_IN_BYTE; bits_idx++)
  {
      if ((mask[bytes_idx] >> bits_idx) == 0)
      {
          length -= bits_idx;
          break;
      }
  }

  *prefix_len = length;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_lpm_prefix_len_get()", 0, 0);
}


uint32
  arad_pp_frwrd_ip_tcam_lpm_data_mask_get(
     SOC_SAND_IN  uint32    prefix_len,
     SOC_SAND_IN  uint32    table_size_in_bytes,
     SOC_SAND_OUT uint8     *mask
  )
{
  uint32 length;
  int index;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(mask);

  
  sal_memset(mask, SOC_SAND_U8_MAX, table_size_in_bytes);
  length = prefix_len;
  index = 0;

  while(length >= SOC_SAND_NOF_BITS_IN_BYTE) {
    mask[index++] = 0;
    length -= SOC_SAND_NOF_BITS_IN_BYTE;
  }
  if(length > 0) {
    mask[index] = SOC_SAND_U8_MAX >> length;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_lpm_prefix_len_get()", 0, 0);
}

#endif 

 
uint32
  arad_pp_frwrd_ip_tcam_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  ARAD_TCAM_ACTION                                *action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
    uint32 res = SOC_SAND_OK, data_indx;
    uint8 stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};
    SOC_SAND_HASH_TABLE_PTR hash_tbl;
    uint8 found, entry_added = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(route_key);
    SOC_SAND_CHECK_NULL_INPUT(success);

    *success = SOC_SAND_SUCCESS;

    
    res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          stream,
          &data_indx,
          &found
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (!found)
    {
        
        res = soc_sand_hash_table_entry_add(
                unit,
                hash_tbl,
                stream,
                &data_indx,
                &entry_added
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else {
        entry_added = TRUE;
    }

    if (entry_added) {
        
        res = arad_pp_ip_tcam_entry_hw_add(
            unit,
                route_key,
                found,
                action,
                data_indx,
                success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        if ((*success != SOC_SAND_SUCCESS) && (!found))
        {
          res = soc_sand_hash_table_entry_remove_by_index(
                  unit,
                  hash_tbl,
                  data_indx
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
    }
    else {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_add_unsafe()", 0, 0);
}



 
uint32
  arad_pp_frwrd_ip_tcam_route_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY *route_key,
    SOC_SAND_IN  uint8               exact_match,
    SOC_SAND_OUT ARAD_TCAM_ACTION    *action,
    SOC_SAND_OUT uint8               *found,
    SOC_SAND_OUT uint8               *hit_bit
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
      stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};
  uint32
    entry_id;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl ;
  uint32
    tcam_db_id;
  ARAD_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  if ((exact_match & SOC_PPC_FRWRD_IP_EXACT_MATCH) 
      
      || (ARAD_IP_TCAM_ENTRY_TYPE_IS_KBP(route_key->type)))
  {
    
      res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_hash_table_entry_lookup(
            unit,
            hash_tbl,
            stream,
            &data_indx,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    entry_id = data_indx;
  }
  else
  {
      
      tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(
                     route_key->type
                   );

    
    arad_pp_ip_tcam_entry_build(
      unit,
      route_key,
      &entry
    );

    res = arad_tcam_db_entry_search_unsafe(
            unit,
            tcam_db_id,
            &entry,
            &entry_id,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (*found)
  {
      uint32 tcam_db_id, priority;
      ARAD_TCAM_ENTRY entry;

      tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(route_key->type);

      res = arad_tcam_db_entry_get_unsafe(
            unit,
            tcam_db_id,
            entry_id,
            exact_match & SOC_PPC_FRWRD_IP_CLEAR_ON_GET,
            &priority,
			&entry,
			action,
            found,
            hit_bit
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_get_unsafe()", 0, 0);
}


 
uint32
  arad_pp_frwrd_ip_tcam_route_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  SOC_PPC_IP_ROUTING_TABLE_RANGE           *block_range_key,
    SOC_SAND_OUT  ARAD_PP_IP_TCAM_ENTRY_KEY                  *route_key,
    SOC_SAND_OUT ARAD_TCAM_ACTION                            *action,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    arr_indx,
    data_indx;
  uint32
    nof_scanned,
    nof_retrieved;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};
  ARAD_PP_IP_TCAM_ENTRY_KEY
    key;
  uint8
      hit_bit,            
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  if ((block_range_key->entries_to_scan == 0) || (block_range_key->entries_to_act == 0)) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  iter = block_range_key->start.payload.arr[0];
  nof_scanned   = 0;
  nof_retrieved = 0;
  arr_indx      = 0;
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)
          && (nof_scanned < block_range_key->entries_to_scan)
          && (nof_retrieved < block_range_key->entries_to_act))
  {
      
      res = soc_sand_hash_table_get_next(
              unit,
              hash_tbl,
              &iter,
              stream,
              &data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if(SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)) {
          break;
      }

    ++nof_scanned;
    res = arad_pp_ip_tcam_stream_to_key(
            unit,
            stream,
            route_key[0].type, 
            &key
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    if (key.type == route_key[arr_indx].type)
    {
      
      if (key.vrf_ndx == route_key[arr_indx].vrf_ndx)
      {
          uint32 tcam_db_id, priority;
          ARAD_TCAM_ENTRY entry;

          route_key[arr_indx] = key;        
          tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(key.type);

          res = arad_tcam_db_entry_get_unsafe(
                unit,
                tcam_db_id,
                data_indx,
                TRUE,
                &priority,
				&entry,
				&action[arr_indx],
                &found,
                &hit_bit
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);       
        
        

        ++nof_retrieved;
        ++arr_indx;
      }
    }
  }

  *nof_entries = arr_indx;

  if(SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
    block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
  }
  else
  {
    block_range_key->start.payload.arr[0] = iter;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_route_get_block_unsafe()",0,0);
}


 
uint32
  arad_pp_frwrd_ip_tcam_route_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY  *route_key
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
    found;
  uint8
	  stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
      
  res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_ip_tcam_key_to_stream(unit, route_key, stream);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = soc_sand_hash_table_entry_lookup(
              unit,
              hash_tbl,
              stream,
              &data_indx,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (!found)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR, 30, exit);
      }

      res = soc_sand_hash_table_entry_remove_by_index(
              unit,
              hash_tbl,
              data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = arad_tcam_managed_db_entry_remove_unsafe(unit, arad_pp_frwrd_ip_tcam_ip_db_id_get(route_key->type), data_indx );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);      

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_route_remove_unsafe()", 0, 0);
}


 
uint32
  arad_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  ARAD_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                    clear_all_vrf
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};
  ARAD_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
  res = soc_sand_hash_table_get_next(
          unit,
          hash_tbl,
          &iter,
          stream,
          &data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
      res = arad_pp_ip_tcam_stream_to_key(
              unit,
              stream,
              route_key->type, 
              &key
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    if (key.type == route_key->type)
    {
      if (clear_all_vrf || key.vrf_ndx == route_key->vrf_ndx)
      {
        res = arad_pp_frwrd_ip_tcam_route_remove_unsafe(
                unit,
                &key
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }

    res = soc_sand_hash_table_get_next(
            unit,
            hash_tbl,
            &iter,
            stream,
            &data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ip_tcam_routing_table_clear_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_ip_tcam_rewrite_entry(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  uint8                      entry_exists,
     SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION  *global_location,
     SOC_SAND_IN  ARAD_TCAM_LOCATION         *location
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_HASH_TABLE_PTR
        hash_tbl;
    uint8
        found;
    uint8
      stream[ARAD_PP_IP_TCAM_KEY_LEN_BYTES] = {0};

    ARAD_PP_IP_TCAM_ENTRY_KEY
        route_key;
    ARAD_IP_TCAM_ENTRY_TYPE
        tcam_key_type;
    ARAD_TCAM_ENTRY
        entry;
    ARAD_TCAM_BANK_ENTRY_SIZE
        entry_size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(global_location);
    SOC_SAND_CHECK_NULL_INPUT(location);

    

    found = FALSE;
    ARAD_TCAM_ENTRY_clear(&entry);

    if(entry_exists)
    {
        
        tcam_key_type = global_location->tcam_db_id;

        
        res = sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.route_key_to_entry_id.get(unit, &hash_tbl);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

        
        res = soc_sand_hash_table_get_by_index(unit, hash_tbl, global_location->entry_id, stream, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

        if(found)
        {
            
            res = arad_pp_ip_tcam_stream_to_key(unit, stream, tcam_key_type, &route_key);
            SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

            
            arad_pp_ip_tcam_entry_build(unit, &route_key, &entry);
            entry.is_for_update = FALSE;
        }
        else
        {
            
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }
    }
    

    res = sw_state_access[unit].dpp.soc.arad.tm.tcam.bank.entry_size.get(unit, location->bank_id, &entry_size);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

    
    res = arad_tcam_entry_rewrite(unit, entry_exists, global_location->tcam_db_id, location, entry_size, &entry);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ip_tcam_rewrite_entry()", 0, 0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


