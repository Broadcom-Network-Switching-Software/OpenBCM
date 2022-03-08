
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#include "pemladrv_physical_access.h"
#include "pemladrv_meminfo_access.h"
#include "pemladrv_logical_access.h"

#ifdef BCM_DNX_SUPPORT
#include "soc/register.h"
#include "soc/mem.h"
#include "soc/drv.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>

ApiInfo api_info[MAX_NOF_UNITS];




void db_mapping_info_init(const int unit, int nof_dbs)
{
  int db_ndx;
  api_info[unit].db_direct_container.nof_direct_dbs = nof_dbs;
  api_info[unit].db_direct_container.db_direct_info_arr = (LogicalDirectInfo*)sal_alloc(nof_dbs * sizeof(LogicalDirectInfo), "");
  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx)
  {
      for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES; ++core_id) {
          api_info[unit].db_direct_container.db_direct_info_arr[db_ndx].db_direct_info_per_core_arr[core_id].result_chunk_mapper_matrix_arr = NULL;
          sal_strncpy(api_info[unit].db_direct_container.db_direct_info_arr[db_ndx].db_direct_info_per_core_arr[core_id].name, "/0", MAX_NAME_LENGTH);
      }
  }
}

void reg_mapping_info_init(const int unit, int nof_reg)
{
  int reg_ndx;
  api_info[unit].reg_container.nof_registers = nof_reg;
  api_info[unit].reg_container.reg_info_arr = (LogicalRegInfo*)sal_alloc(nof_reg * sizeof(LogicalRegInfo), "");
  
  for (reg_ndx = 0; reg_ndx < nof_reg; ++reg_ndx)
  {
    api_info[unit].reg_container.reg_info_arr[reg_ndx].is_mapped = 0;
  }
}

void tcam_mapping_info_init(const int unit, int nof_dbs)
{
  int db_ndx;
  api_info[unit].db_tcam_container.nof_tcam_dbs = nof_dbs;
  api_info[unit].db_tcam_container.db_tcam_info_arr = (LogicalTcamInfo*)sal_alloc(nof_dbs * sizeof(LogicalTcamInfo), "");
  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx)
  {
      for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES; ++core_id) {
          api_info[unit].db_tcam_container.db_tcam_info_arr[db_ndx].db_tcam_per_core_arr[core_id].key_chunk_mapper_matrix_arr = NULL;
          api_info[unit].db_tcam_container.db_tcam_info_arr[db_ndx].db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr = NULL;
          sal_strncpy(api_info[unit].db_tcam_container.db_tcam_info_arr[db_ndx].db_tcam_per_core_arr[core_id].name, "/0", MAX_NAME_LENGTH);
      }
  }
}

void em_mapping_info_init(const int unit, int nof_dbs)
{
  int db_ndx;
  api_info[unit].db_em_container.nof_em_dbs = nof_dbs;
  api_info[unit].db_em_container.db_em_info_arr = (LogicalEmInfo*)sal_alloc(nof_dbs * sizeof(LogicalEmInfo), "");
  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx)
  {
      for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES; ++core_id) {
          api_info[unit].db_em_container.db_em_info_arr[db_ndx].logical_em_info.db_tcam_per_core_arr[core_id].key_chunk_mapper_matrix_arr = NULL;
          api_info[unit].db_em_container.db_em_info_arr[db_ndx].logical_em_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr = NULL;
          sal_strncpy(api_info[unit].db_em_container.db_em_info_arr[db_ndx].logical_em_info.db_tcam_per_core_arr[core_id].name, "/0", MAX_NAME_LENGTH);
      }
  }
}

void lpm_mapping_info_init(const int unit, int nof_dbs)
{
  int db_ndx;
  api_info[unit].db_lpm_container.nof_lpm_dbs = nof_dbs;
  api_info[unit].db_lpm_container.db_lpm_info_arr = (LogicalLpmInfo*)sal_alloc(nof_dbs * sizeof(LogicalLpmInfo), "");
  
  for (db_ndx = 0; db_ndx < nof_dbs; ++db_ndx)
  {
      for (int core_id=0; core_id<PEM_CFG_API_NOF_CORES; ++core_id) {
          api_info[unit].db_lpm_container.db_lpm_info_arr[db_ndx].logical_lpm_info.db_tcam_per_core_arr[core_id].key_chunk_mapper_matrix_arr = NULL;
          api_info[unit].db_lpm_container.db_lpm_info_arr[db_ndx].logical_lpm_info.db_tcam_per_core_arr[core_id].result_chunk_mapper_matrix_arr = NULL;
          sal_strncpy(api_info[unit].db_lpm_container.db_lpm_info_arr[db_ndx].logical_lpm_info.db_tcam_per_core_arr[core_id].name, "/0", MAX_NAME_LENGTH);
      }
  }
}

void vr_mapping_info_init(const int unit, int nof_vws)
{
  int vw_ndx;
  api_info[unit].vw_container.nof_virtual_wires = nof_vws;
  api_info[unit].vw_container.vw_info_arr = (VirtualWireInfo*)sal_alloc(nof_vws * sizeof(VirtualWireInfo), "");
  
  sal_memset(api_info[unit].vw_container.vw_info_arr,0,nof_vws * sizeof(VirtualWireInfo));


  
  for (vw_ndx = 0; vw_ndx < nof_vws; ++vw_ndx)
  {
    api_info[unit].vw_container.vw_info_arr[vw_ndx].vw_mappings_arr = NULL;
  }
}

