/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PMF_LOW_LEVEL_CE_INCLUDED__

#define __ARAD_PMF_LOW_LEVEL_CE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>
#include <soc/dpp/PPC/ppc_api_fp.h>






#define ARAD_PMF_CE_HW_BUFFER_SUB_HEADER_BIT             (2)
#define ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB        (2)
#define ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB        (0)
#define ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB        (3)
#define ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB        (3)
#define ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB            (15)
#define ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16        (12)
#define ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32        (11)
#define ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16        (11)
#define ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32        (10)
#define ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB            (4)

#define ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_HEADER                (0)
#define ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_INTERNAL_FIELD        (1)

#define ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES            (8)
#define ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES        (4)
#define ARAD_PMF_CE_INSTR_RESOLUTION_32B_NOF_BITS        (32)
#define ARAD_PMF_CE_INSTR_RESOLUTION_16B_NOF_BITS        (16)
#define ARAD_PMF_CE_INSTR_2S_COMP_VALUE_32B_BYTES_BIT_NDX    (6)
#define ARAD_PMF_CE_INSTR_2S_COMP_VALUE_16B_NIBBLES_BIT_NDX    (7)

typedef SOC_TMC_PMF_CE_PACKET_HEADER_INFO                      ARAD_PMF_CE_PACKET_HEADER_INFO;


#define ARAD_PMF_ETH_CE_NOF_INSTR_ZONES  (5)


#define ARAD_PMF_CE_NOF_GROUP_PER_LSB    (2)



















uint32
  arad_pmf_ce_internal_field_offset_qual_find(
	  SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_IN  uint32		                   nof_bits,
	  SOC_SAND_IN  uint32						   ce_offset,
	  SOC_SAND_IN  uint8		                   is_32b_ce,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         irpp_field_in,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT  SOC_PPC_FP_QUAL_TYPE           *irpp_field,
	  SOC_SAND_OUT  uint32		                   *qual_lsb,
	  SOC_SAND_OUT  uint32		                   *qual_nof_bits
 );

soc_field_t
  arad_pmf_ce_instruction_fld_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
    SOC_SAND_IN  uint32                           ce_ndx
  );



uint32
  arad_pmf_ce_nof_real_bits_compute_header(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info, 
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_IN  uint8		                   is_32b_ce,
	  SOC_SAND_OUT uint32						   *nof_bits_for_ce 
  );

uint32
  arad_pmf_ce_internal_field_offset_compute(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE        irpp_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_IN  uint32		                   nof_bits,
      SOC_SAND_IN  uint32		                   qual_lsb,
      SOC_SAND_IN  uint32                             lost_bits, 
      SOC_SAND_IN  uint8		                   is_32b_ce,
      SOC_SAND_OUT uint8                           *is_found,
      SOC_SAND_OUT uint32						   *ce_offset,
      SOC_SAND_OUT uint32						   *nof_bits_for_ce,
      SOC_SAND_OUT uint32                          *hw_buffer_jericho 
  );


uint8
  arad_pmf_low_level_ce_is_32b_ce(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  );

uint8
  arad_pmf_low_level_ce_is_second_group(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  );

uint32
  arad_pmf_ce_header_info_find(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            header_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT ARAD_PMF_CE_HEADER_QUAL_INFO	      *qual_info
  );


uint32
  arad_pmf_ce_internal_field_info_find(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            irpp_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
	  SOC_SAND_IN  uint8                           is_msb,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT ARAD_PMF_CE_IRPP_QUALIFIER_INFO     *qual_info
  );

soc_field_t
  arad_pmf_ce_valid_fld_group_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint32                    ce_group_ndx,
    SOC_SAND_IN  uint32                    pmf_key
  );

soc_mem_t
  arad_pmf_ce_table_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint8                     is_ce_in_msb,
    SOC_SAND_IN  uint8                     is_second_lookup,
    SOC_SAND_IN  uint8                     is_update_key,
    SOC_SAND_IN  uint8                     ce_ndx
  );


uint32
  arad_pmf_low_level_ce_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pmf_ce_entry_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     pmf_pgm_ndx,
    SOC_SAND_IN  uint32                     pmf_key,
    SOC_SAND_IN  uint32                     ce_ndx,
    SOC_SAND_IN  uint8                      is_ce_in_msb,
    SOC_SAND_IN  uint8                      is_second_lookup
  );



uint32
  arad_pmf_ce_packet_header_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  );


uint32
  arad_pmf_ce_for_key_get_unsafe(
    SOC_SAND_IN    int                            unit,
    SOC_SAND_IN    SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN    uint32                         pmf_pgm_ndx,
    SOC_SAND_IN    uint32                         ce_ndx,
    SOC_SAND_IN    uint8                          is_second_lookup,
    SOC_SAND_OUT   SOC_PPC_FP_CE_INFO             *info
  );


uint32
  arad_pmf_ce_packet_header_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  );

uint32
  arad_pmf_ce_for_key_dump(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup
  );

uint32
  arad_pmf_ce_internal_info_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  uint8                            is_update_key,
	SOC_SAND_IN  uint32                             qual_lsb,
    SOC_SAND_IN  uint32                             lost_bits, 
	SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            irpp_field
  );



uint32
  arad_pmf_ce_internal_info_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         irpp_field_in,
	SOC_SAND_OUT  uint32                            *qual_lsb,
	SOC_SAND_OUT uint32                             *nof_bits,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE             *irpp_field
  );


uint32
  arad_pmf_ce_nop_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
	SOC_SAND_IN  uint8                            is_ce_not_valid
  );



uint32
  arad_pmf_ce_nop_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
	SOC_SAND_IN  uint8                            is_second_lookup,
	SOC_SAND_OUT uint8                            *is_ce_not_valid,
    SOC_SAND_OUT uint32                             *loc
  );

uint32
  ARAD_PMF_CE_PACKET_HEADER_INFO_verify(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info,
    SOC_SAND_IN  uint32						ce_ndx
  );

void
  ARAD_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  ARAD_PMF_CE_SUB_HEADER enum_val
  );


void
  ARAD_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif





