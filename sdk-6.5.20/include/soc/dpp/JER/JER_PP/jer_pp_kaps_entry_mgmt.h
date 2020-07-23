/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PP_KAPS_ENTRY_MGMT_INCLUDED__

#define __JER_PP_KAPS_ENTRY_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>





#define JER_KAPS_AD_BUFFER_NOF_BYTES    4
#define JER_KAPS_TBL_PREFIX_NOF_BITS    2
#define JER_KAPS_DMA_BUFFER_NOF_BYTES    8






















    






uint32 jer_pp_kaps_entry_add_hw(SOC_SAND_IN  int                          unit,
                                SOC_SAND_IN  uint32                       table_id,
                                SOC_SAND_IN  uint32                       prefix_len,
                                SOC_SAND_IN  uint32                      hitbit_enabled,
                                SOC_SAND_OUT  uint8                      *data,
                                SOC_SAND_OUT  uint8                      *assoData,
                                SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success);

uint32 jer_pp_kaps_entry_remove_hw(SOC_SAND_IN  int                          unit,
                                   SOC_SAND_IN  uint32                       table_id,
                                   SOC_SAND_IN  uint32                       prefix_len,
                                   SOC_SAND_IN  uint32                       hitbit_enabled,
                                   SOC_SAND_OUT uint8                       *data);


uint32 jer_pp_kaps_entry_get_hw(SOC_SAND_IN    int          unit,
                                SOC_SAND_IN    uint8        table_id,
                                SOC_SAND_IN    uint32       prefix_len,
                                SOC_SAND_IN    uint32       hitbit_enabled,
                                SOC_SAND_OUT   uint8       *data,
                                SOC_SAND_OUT   uint8       *assoData,
                                SOC_SAND_INOUT uint8       *hit_bit,
                                SOC_SAND_OUT   uint8       *found);

uint32 jer_pp_kaps_get_hit_bit(SOC_SAND_IN    int                unit,
                               SOC_SAND_IN    uint8              table_id,
                               SOC_SAND_INOUT struct kbp_entry  *db_entry,
                               SOC_SAND_IN    uint8              clear,
                               SOC_SAND_OUT   uint8             *hit_bit);

uint32 jer_pp_kaps_entry_get_block_hw(SOC_SAND_IN    int                             unit,
                                      SOC_SAND_IN    SOC_DPP_DBAL_SW_TABLE_IDS       dbal_table_id,
                                      SOC_SAND_IN    uint32                          vrf_ndx,
                                      SOC_SAND_IN    SOC_DPP_DBAL_TABLE_INFO        *dbal_table,
                                      SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key,
                                      SOC_SAND_OUT   SOC_PPC_FP_QUAL_VAL            *qual_vals_array,
                                      SOC_SAND_OUT   void                           *payload,
                                      SOC_SAND_OUT   uint32                         *nof_entries);

uint32 jer_pp_kaps_route_to_kaps_payload_buffer_encode(SOC_SAND_IN  int                         unit,
                                                       SOC_SAND_IN  uint32                     *asso_data_buffer,
                                                       SOC_SAND_OUT uint8                      *asso_data);

uint32 jer_pp_kaps_dma_buffer_to_kaps_payload_buffer_encode(
                                                        SOC_SAND_IN  int                        unit,
                                                        SOC_SAND_IN  uint32                     *asso_data_buffer,
                                                        SOC_SAND_IN  uint32                     size,
                                                        SOC_SAND_OUT uint8                      *asso_data);

uint32 jer_pp_kaps_payload_buffer_to_dma_buffer_decode(SOC_SAND_IN  int                        unit,
                                                       SOC_SAND_IN  uint8                      *asso_data,
                                                       SOC_SAND_IN  uint32                     size,
                                                       SOC_SAND_OUT uint32                     *asso_data_buffer);

uint32 jer_pp_kaps_route_to_kaps_payload_buffer_decode(SOC_SAND_IN  int                         unit,
                                                       SOC_SAND_IN  uint8                      *asso_data,
                                                       SOC_SAND_OUT uint32                     *asso_data_buffer);

uint32 jer_pp_kaps_payload_to_mc_dest_id(SOC_SAND_IN  int                                     unit,
                                         SOC_SAND_IN  uint32                                  payload,
                                         SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID              *dest_id);

uint32 jer_pp_kaps_dbal_table_id_translate(SOC_SAND_IN  int                       unit,
                                           SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                                           SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL       qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                           SOC_SAND_IN  uint8                     private_table,
                                           SOC_SAND_OUT JER_KAPS_IP_TBL_ID       *kaps_table_id);

uint32 jer_pp_kaps_table_id_to_dbal_translate(SOC_SAND_IN  int                        unit,
                                              SOC_SAND_IN  JER_KAPS_IP_TBL_ID         kaps_table_id,
                                              SOC_SAND_OUT SOC_DPP_DBAL_SW_TABLE_IDS *table_id);

uint32 jer_pp_kaps_dma_clear_db(SOC_SAND_IN int       unit,
                                SOC_SAND_IN uint32    db_id);

uint32 jer_pp_kaps_entry_table_clear(SOC_SAND_IN    int unit, 
                                     SOC_SAND_IN    uint8 tbl_id);

void jer_pp_kaps_key_encode(SOC_SAND_IN    uint8 tbl_prefix,
                           SOC_SAND_IN    uint8 nof_quals,
                           SOC_SAND_IN    uint64 *qual_vals,
                           SOC_SAND_IN    uint8 *qual_nof_bits,
                           SOC_SAND_OUT   uint8 *data_bytes);

uint32 jer_pp_kaps_db_enabled(SOC_SAND_IN    int              unit,
                              SOC_SAND_IN    SOC_PPC_VRF_ID   vrf_ndx);

uint32 jer_pp_kaps_db_verify(SOC_SAND_IN    int              unit,
                             SOC_SAND_IN    SOC_PPC_RIF_ID    inrif);

uint32 jer_pp_kaps_dma_entry_add(SOC_SAND_IN  int                         unit,
                                 SOC_SAND_IN  uint32                      key,
                                 SOC_SAND_IN  uint8                      *data,
                                 SOC_SAND_IN  uint32                      buffer_size,
                                 SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success);

uint32 jer_pp_kaps_dma_entry_remove( SOC_SAND_IN  int                         unit,
                                     SOC_SAND_IN  uint32                      key,
                                     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success);

uint32 jer_pp_kaps_dma_entry_get(SOC_SAND_IN  int                         unit,
                                 SOC_SAND_IN  uint32                      key,
                                 SOC_SAND_IN  uint32                      buffer_size,
                                 SOC_SAND_OUT uint8                       *data,
                                 SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success);

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


