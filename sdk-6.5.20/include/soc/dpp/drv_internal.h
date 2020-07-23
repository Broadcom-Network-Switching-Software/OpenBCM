#ifndef __DRV_INTERNAL_H__
#define __DRV_INTERNAL_H__
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
typedef struct {
  uint32 start;
  uint32 end;
}SOC_PB_MGMT_DBUFF_BOUNDARIES;

typedef struct {
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  uc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_uc;
} SOC_PB_INIT_DBUFFS_BDRY;



extern uint32
soc_pb_init_dram_nof_buffs_calc(uint32                   dram_size_total_mbyte,
                            SOC_PETRA_ITM_DBUFF_SIZE_BYTES dbuff_size,
                            SOC_PETRA_HW_QDR_PROTECT_TYPE  qdr_protection_mode,
                            SOC_PETRA_HW_QDR_SIZE_MBIT     qdr_total_size_mbit,
                            uint32                   *nof_dram_buffs);
extern uint32
soc_pb_init_dram_buff_boudaries_calc(uint32                     total_buffs,
                                 SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
                                 SOC_PB_INIT_DBUFFS_BDRY       *dbuffs);
extern uint32
soc_pb_mgmt_init_before_blocks_oor(int                    unit,
                               SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
                               SOC_PB_INIT_DBUFFS_BDRY         *dbuffs_bdries,
                               SOC_PB_HW_ADJUSTMENTS           *hw_adj);
extern uint32
soc_pb_mgmt_blocks_init_unsafe(int unit);

extern uint32
soc_pb_mgmt_init_after_blocks_oor(int                         unit, 
                              SOC_PETRA_ITM_DBUFF_SIZE_BYTES  dbuff_size, 
                              SOC_PB_INIT_DBUFFS_BDRY      *dbuffs_bdries);
extern uint32
soc_pb_mgmt_tbls_init(int unit, int silent);

extern uint32
soc_pb_pp_mgmt_tbls_init_unsafe(int unit, int silent);

extern uint32 
soc_pb_mgmt_hw_set_defaults(int unit);

extern uint32
soc_pb_mgmt_hw_adjust_ddr(int unit, SOC_PETRA_HW_ADJ_DDR *hw_adjust);

extern uint32
soc_pb_mgmt_functional_init(int unit, SOC_PB_HW_ADJUSTMENTS *hw_adjust, int sildent);

extern uint32
soc_pb_mgmt_hw_adjust_qdr(int unit, SOC_PB_HW_ADJ_QDR *hw_adjust);

extern uint32
soc_pb_mgmt_init_finalize(int unit);

#endif 
