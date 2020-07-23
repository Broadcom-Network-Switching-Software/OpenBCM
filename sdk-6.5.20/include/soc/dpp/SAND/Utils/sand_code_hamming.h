/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#ifndef __SOC_SAND_CODE_HAMMING_H_INCLUDED__

#define __SOC_SAND_CODE_HAMMING_H_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>




#include <soc/dpp/SAND/Utils/sand_framework.h>




























SOC_SAND_RET
  soc_sand_code_hamming_check_data_no_check(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  );


SOC_SAND_RET
  soc_sand_code_hamming_check_data(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  );


SOC_SAND_RET
  soc_sand_code_hamming_get_p(
    SOC_SAND_IN  uint32  data[],
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_IN  uint32  generation_matrix[],
    SOC_SAND_IN  uint32  m,
    SOC_SAND_OUT uint32* s
  );


SOC_SAND_RET
  soc_sand_code_hamming_get_p_no_check(
    SOC_SAND_IN    uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  generation_matrix[],
    SOC_SAND_IN    uint32  p,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* s
  );


SOC_SAND_RET
  soc_sand_code_hamming_get_p_bit_wide(
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_OUT uint32* p
  );



SOC_SAND_RET
  soc_sand_code_hamming_generate_gen_matrix(
    SOC_SAND_IN  uint32 data_bit_wide,
    SOC_SAND_IN  uint32 search_gen_mat_nof_entires,
    SOC_SAND_OUT uint32 gen_mat[],
    SOC_SAND_OUT uint32 search_gen_mat[]
  );




#if defined (SOC_SAND_DEBUG)




uint32
  soc_sand_code_hamming_TEST(
    SOC_SAND_IN uint32 silent
  );


void
  soc_sand_code_hamming_print_gen_matrix(
    SOC_SAND_IN uint32 data_bit_wide
  );



#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
