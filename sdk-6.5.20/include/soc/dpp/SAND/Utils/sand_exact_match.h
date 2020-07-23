/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_EXACT_MATCH_H_INCLUDED

#define SOC_SAND_EXACT_MATCH_H_INCLUDED



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>




#define SOC_SAND_EXACT_MATCH_NOF_TABLES 2

#define SOC_SAND_EXACT_MATCH_NULL  SOC_SAND_U32_MAX










typedef enum
{
  SOC_SAND_EXACT_MATCH_USE_AC=0,
  SOC_SAND_EXACT_MATCH_USE_OAM,
  SOC_SAND_EXACT_MATCH_NOF_USES
}SOC_SAND_EXACT_MATCH_USE;

typedef uint8* SOC_SAND_EXACT_MATCH_KEY;
typedef uint8* SOC_SAND_EXACT_MATCH_PAYLOAD;
typedef uint32 SOC_SAND_EXACT_MATCH_HASH_VAL;
typedef uint8* SOC_SAND_EXACT_MATCH_VERIFIER;
typedef uint32 SOC_SAND_EXACT_MATCH_ITER;

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8                    *data
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_HW_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_PAYLOAD        payload, 
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_VERIFIER       verifier
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_HW_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_OUT SOC_SAND_EXACT_MATCH_PAYLOAD        payload, 
      SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER       verifier
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_IN  uint8                  is_valid
    );

typedef
  uint32
    (*SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint32                   tbl_ndx,
      SOC_SAND_IN  uint32                   entry_offset,
      SOC_SAND_OUT uint8                  *is_valid
    );

typedef
  SOC_SAND_EXACT_MATCH_HASH_VAL
    (*SOC_SAND_EXACT_MATCH_KEY_TO_HASH)(
      SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key,
      SOC_SAND_IN uint32 table_id
    );

typedef
  void
    (*SOC_SAND_EXACT_MATCH_KEY_TO_VERIFY)(
      SOC_SAND_IN   SOC_SAND_EXACT_MATCH_KEY key,
      SOC_SAND_IN   uint32 table_ndx,
      SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_VERIFIER verifier
    );


typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
  SOC_SAND_EXACT_MATCH_KEY  key;
  SOC_SAND_EXACT_MATCH_PAYLOAD payload; 
  SOC_SAND_EXACT_MATCH_VERIFIER verifier;
} SOC_SAND_EXACT_MATCH_ENTRY;

typedef struct {

  SOC_SAND_EXACT_MATCH_KEY  key; 
  SOC_SAND_EXACT_MATCH_HASH_VAL  hash_indx;
} SOC_SAND_EXACT_MATCH_STACK_ENTRY;

typedef struct {
  int prime_handle;
  uint32 sec_handle;
  uint32 max_insert_steps;
  uint32 key_bits;
  uint32 hash_bits;
  uint32 verifier_bits;

  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET set_entry_fun;
  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET get_entry_fun;
  SOC_SAND_EXACT_MATCH_HW_ENTRY_SET  hw_set_fun;
  SOC_SAND_EXACT_MATCH_HW_ENTRY_GET  hw_get_fun;
  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET  is_valid_entry_set;
  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET  is_valid_entry_get;

  SOC_SAND_EXACT_MATCH_HASH_VAL (*key_to_hash)(SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, SOC_SAND_IN uint32 table_id); 
  void (*key_to_verifier)(SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, SOC_SAND_IN uint32 table_ndx, SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier); 

} SOC_SAND_EXACT_MATCH_INIT_INFO;

typedef struct
{
  SOC_SAND_EXACT_MATCH_KEY  keys[SOC_SAND_EXACT_MATCH_NOF_TABLES]; 
  uint32 *use_bitmap[SOC_SAND_EXACT_MATCH_NOF_TABLES];
  SOC_SAND_EXACT_MATCH_STACK_ENTRY *stack[SOC_SAND_EXACT_MATCH_NOF_TABLES];
  SOC_SAND_EXACT_MATCH_ENTRY *tmp_entry;
  uint32 table_size;
  uint32 key_size;
  uint32 payload_size;
  uint32  verifier_size;

} SOC_SAND_EXACT_MATCH_T;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_SAND_EXACT_MATCH_INIT_INFO
    init_info;

  SOC_SAND_EXACT_MATCH_T
    mgmt_info;
} SOC_SAND_EXACT_MATCH_INFO;











uint32
  soc_sand_exact_match_create(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_destroy(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_entry_add(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_PAYLOAD     payload,
    SOC_SAND_OUT uint32                     *tbl_id,
    SOC_SAND_OUT uint32                     *entry_indx,
    SOC_SAND_OUT  uint32                    *nof_steps,
    SOC_SAND_OUT  uint8                   *success
  );

uint32
  soc_sand_exact_match_entry_remove(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key
  );

uint32
  soc_sand_exact_match_entry_lookup(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY         key,
    SOC_SAND_OUT  uint32                    *tbl_id,
    SOC_SAND_OUT  uint32                    *entry_indx,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_PAYLOAD    payload,
    SOC_SAND_OUT  uint8                   *found
  );

uint32
  soc_sand_exact_match_clear(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match
  );

uint32
  soc_sand_exact_match_get_block(
    SOC_SAND_INOUT  SOC_SAND_EXACT_MATCH_INFO     *exact_match,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE    *range,
    SOC_SAND_OUT  SOC_SAND_EXACT_MATCH_KEY        keys,
    SOC_SAND_OUT  uint32                    *nof_entries
  );

uint32
  soc_sand_exact_match_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_INFO        *exact_match,
    SOC_SAND_OUT  uint32                    *size
  );

uint32
  soc_sand_exact_match_save(
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_INFO        *exact_match,
    SOC_SAND_OUT uint8                      *buffer,
    SOC_SAND_IN  uint32                     buffer_size_bytes,
    SOC_SAND_OUT uint32                     *actual_size_bytes
  );

uint32
  soc_sand_exact_match_load(
    SOC_SAND_IN  uint8                              **buffer,
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_INFO                *exact_match,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY_TO_HASH         key_to_hash,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_KEY_TO_VERIFY       key_to_verify,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_HW_ENTRY_SET        hw_set_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_HW_ENTRY_GET        hw_get_fun,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_SET  is_valid_entry_set,
    SOC_SAND_IN  SOC_SAND_EXACT_MATCH_IS_ENTRY_VALID_GET  is_valid_entry_get
  );

void
  SOC_SAND_EXACT_MATCH_INFO_clear(
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_INFO        *info
  );

#ifdef SOC_SAND_DEBUG

void
  soc_sand_SAND_EXACT_MATCH_INFO_print(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_INFO         *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

