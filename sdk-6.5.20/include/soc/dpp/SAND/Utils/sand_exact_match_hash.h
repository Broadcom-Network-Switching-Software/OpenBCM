/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef SOC_SAND_EXACT_MATCH_HASH_H_INCLUDED

#define SOC_SAND_EXACT_MATCH_HASH_H_INCLUDED



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_exact_match.h>

























void 
  soc_sand_exact_match_hash_key_to_verifier_31_19(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_31_19(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );

void 
  soc_sand_exact_match_hash_key_to_verifier_31_20(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_31_20(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );

void 
  soc_sand_exact_match_hash_key_to_verifier_25_12(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id, 
    SOC_SAND_OUT SOC_SAND_EXACT_MATCH_VERIFIER verifier
  );

SOC_SAND_EXACT_MATCH_HASH_VAL 
  soc_sand_exact_match_hash_key_to_hash_25_13(
    SOC_SAND_IN SOC_SAND_EXACT_MATCH_KEY key, 
    SOC_SAND_IN uint32 table_id
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



