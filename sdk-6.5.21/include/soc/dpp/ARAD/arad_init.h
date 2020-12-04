/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_INIT_INCLUDED__

#define __ARAD_INIT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_mgmt.h>
























uint32 arad_mgmt_init_pll_reset(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_MGMT_INIT *init);

uint32 arad_mgmt_pon_init(
    SOC_SAND_IN  int                                unit);

uint32
  arad_mgmt_ihb_tbls_init(
    SOC_SAND_IN  int                 unit
  );

uint32 
arad_init_pdm_nof_entries_calc(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_INIT_PDM_MODE         pdm_mode,
    SOC_SAND_OUT uint32                     *pdm_nof_entries
   );


uint32
  arad_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN     ARAD_MGMT_INIT           *init,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  arad_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN     ARAD_MGMT_INIT           *init
  );


uint32
arad_ser_init(int unit);

uint32 arad_mgmt_ipt_init(
    SOC_SAND_IN  int                 unit
);

uint32
  arad_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

int _arad_mgmt_irr_tbls_init_dma_callback(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int copyno, 
    SOC_SAND_IN int array_index, 
    SOC_SAND_IN int index, 
    SOC_SAND_OUT uint32 *value, 
    SOC_SAND_IN int entry_sz, 
    SOC_SAND_IN void *opaque);

uint32
  arad_mgmt_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );



uint32
  arad_mgmt_all_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  arad_mgmt_ips_tbls_init(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint8                 silent
    );

uint32
  arad_mgmt_ipt_tbls_init(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint8                 silent
    );

uint32
  arad_init_dram_fbc_buffs_get(
    SOC_SAND_IN  uint32  buffs_without_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *fbc_nof_bufs
  );


uint32
  arad_init_mesh_topology(
    SOC_SAND_IN int             unit
  );


uint32
  arad_mgmt_stk_init(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_MGMT_INIT               *init
  );


int soc_bist_all_arad(const int unit, const int skip_errors);
int soc_bist_irdp_arad(const int unit, const int skip_errors);
int soc_bist_arad_ser_test(const int unit, const int skip_errors, uint32 nof_repeats, uint32 time_to_wait, uint32 ser_test_num);
int soc_bist_all_ardon(const int unit, const int skip_errors);
int soc_bist_irfc_ardon(const int unit, const int skip_errors);


uint32
arad_mgmt_nbi_ecc_mask_get(
      SOC_SAND_IN int                unit,
      SOC_SAND_OUT uint64            *mask
      );

uint32      arad_iqm_workaround(SOC_SAND_IN  int    unit);
int         arad_activate_power_savings(int unit);
uint32      arad_mgmt_init_finalize(SOC_SAND_IN int unit);


int soc_dpp_arad_dma_init(int unit);
int soc_arad_tcam_bist_check(int unit);


#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


