/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/ARAD/arad_multicast_imp.h>
#include <soc/dpp/JER/jer_multicast_imp.h>


#include <soc/mcm/memregs.h>

#include <shared/bsl.h>




#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/sand/sand_mem.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>






#define ARAD_MC_INGRESS_LINK_PTR_END 0x1ffff 
#define JER_MC_INGRESS_LINK_PTR_END 0x3ffff 
#define JER_MCDB_SIZE (JER_MC_INGRESS_LINK_PTR_END + 1)



#define ARAD_MC_UNOCCUPIED_ENTRY_LOW 0x7fffffff
#define ARAD_MC_UNOCCUPIED_ENTRY_HIGH 0
#define JER_MC_UNOCCUPIED_ENTRY_LOW 0xffffffff
#define JER_MC_UNOCCUPIED_ENTRY_HIGH 3


#define ARAD_MC_ING_EMPTY_ENTRY_LOW  0xffffffff
#define ARAD_MC_ING_EMPTY_ENTRY_HIGH 0x7ffff
#define JER_MC_ING_EMPTY_ENTRY_LOW  0xffffffff
#define JER_MC_ING_EMPTY_ENTRY_HIGH 0x7fffff

#define IRDB_TABLE_GROUPS_PER_ENTRY 16
#define IRDB_TABLE_BITS_PER_GROUP 2

#define DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS 2 

#define JER_MC_SPECIAL_MODE_ENTRIES_PER_ENTRY 4

uint32 dpp_init_mcds(int unit);
uint32 dpp_deinit_mcds(int unit);


















dpp_mcdb_entry_t*
  dpp_mcds_get_mcdb_entry(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mcdb_index
)
{
  return ((dpp_mcds_base_t*)dpp_get_mcds(unit))->mcdb + mcdb_index;
}

dpp_mcdb_entry_t* dpp_mcds_get_mcdb_entry_from_mcds(
    SOC_SAND_IN  dpp_mcds_t* mcds,
    SOC_SAND_IN  uint32 mcdb_index
)
{
  return ((dpp_mcds_base_t*)mcds)->mcdb + mcdb_index;
}



int dpp_rep_data_t_compare(void *a, void *b)
{ 
  const dpp_rep_data_t *ca = b;
  const dpp_rep_data_t *cb = a;
  int32 res = ca->extra - cb->extra;
  return res ? res : (int32)(ca->base - cb->base);
}


void dpp_add_ingress_replication(
  dpp_mcds_base_t *mcds,
  const uint32     cud,
  const uint32     dest
)
{
  dpp_rep_data_t *rep = mcds->reps + mcds->nof_reps[0];
  DPP_MC_ASSERT(mcds->nof_ingr_reps == mcds->nof_reps[0] && mcds->nof_reps[0] < DPP_MULT_MAX_INGRESS_REPLICATIONS &&
    (mcds->nof_egr_port_outlif_reps[0] | mcds->nof_egr_outlif_reps[0] | mcds->nof_egr_bitmap_reps[0]) == 0);
  rep->extra = rep->base = 0;
  DPP_MCDS_REP_DATA_SET_TYPE(rep, DPP_MCDS_REP_TYPE_INGRESS);
  DPP_MCDS_REP_DATA_SET_INGR_CUD(rep, cud);
  DPP_MCDS_REP_DATA_SET_INGR_DEST(rep, dest);
  mcds->nof_ingr_reps = ++mcds->nof_reps[0];
}


STATIC INLINE uint32 compare_dpp_rep_data_t(const dpp_rep_data_t *rep1, const dpp_rep_data_t *rep2)
{
  return (rep1->base - rep2->base) | (rep1->extra - rep2->extra);
}

void dpp_add_egress_replication_port_cud(
  dpp_mcds_base_t *mcds,
  uint32           core,
  const uint32     cud,
  const uint32     cud2,
  const uint32     tm_port
)
{
    dpp_rep_data_t *rep = mcds->reps + core * DPP_MULT_MAX_REPLICATIONS + mcds->nof_reps[core];
    DPP_MC_ASSERT(mcds->nof_ingr_reps == 0 && mcds->nof_reps[core] < DPP_MULT_MAX_EGRESS_REPLICATIONS &&
      mcds->nof_egr_port_outlif_reps[core] + mcds->nof_egr_outlif_reps[core] + mcds->nof_egr_bitmap_reps[core] == mcds->nof_reps[core]);
    rep->extra = rep->base = 0;
    DPP_MCDS_REP_DATA_SET_TYPE(rep, DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
    DPP_MCDS_REP_DATA_SET_EGR_CUD(rep, cud);
    DPP_MCDS_REP_DATA_SET_EXTRA_CUD(rep, cud2);
    DPP_MCDS_REP_DATA_SET_EGR_PORT(rep, tm_port);
    ++mcds->nof_reps[core];
    ++mcds->nof_egr_port_outlif_reps[core];
}

void dpp_add_egress_replication_cud(
  dpp_mcds_base_t *mcds,
  uint32           core,
  const uint32     cud,
  const uint32     cud2
)
{
    dpp_rep_data_t *rep = mcds->reps + core * DPP_MULT_MAX_REPLICATIONS + mcds->nof_reps[core];
    DPP_MC_ASSERT(mcds->nof_ingr_reps == 0 && mcds->nof_reps[core] < DPP_MULT_MAX_EGRESS_REPLICATIONS &&
      mcds->nof_egr_port_outlif_reps[core] + mcds->nof_egr_outlif_reps[core] + mcds->nof_egr_bitmap_reps[core] == mcds->nof_reps[core]);
    rep->extra = rep->base = 0;
    DPP_MCDS_REP_DATA_SET_TYPE(rep, DPP_MCDS_REP_TYPE_EGR_CUD);
    DPP_MCDS_REP_DATA_SET_EGR_CUD(rep, cud);
    DPP_MCDS_REP_DATA_SET_EXTRA_CUD(rep, cud2);
    ++mcds->nof_reps[core];
    ++mcds->nof_egr_outlif_reps[core];
}

void dpp_add_egress_replication_bitmap(
  dpp_mcds_base_t *mcds,
  uint32           core,
  const uint32     cud,
  const uint32     cud2,
  const uint32     bm_id
)
{
    dpp_rep_data_t *rep;
    rep = mcds->reps + core * DPP_MULT_MAX_REPLICATIONS + mcds->nof_reps[core];
    rep->extra = rep->base = 0;
    DPP_MCDS_REP_DATA_SET_TYPE(rep, DPP_MCDS_REP_TYPE_EGR_BM_CUD);
    DPP_MCDS_REP_DATA_SET_EGR_CUD(rep, cud);
    DPP_MCDS_REP_DATA_SET_EXTRA_CUD(rep, cud2);
    DPP_MCDS_REP_DATA_SET_EGR_BM_ID(rep, bm_id);
    ++mcds->nof_reps[core];
    ++mcds->nof_egr_bitmap_reps[core];
    DPP_MC_ASSERT(mcds->nof_ingr_reps == 0 && mcds->nof_reps[core] < DPP_MULT_MAX_EGRESS_REPLICATIONS &&
      mcds->nof_egr_port_outlif_reps[core] + mcds->nof_egr_outlif_reps[core] + mcds->nof_egr_bitmap_reps[core] == mcds->nof_reps[core]);
}




void dpp_mcds_clear_replications(dpp_mcds_base_t *mcds, const uint32 group_type)
{
    int i = 0;
    mcds->nof_ingr_reps = 0;
    mcds->info_2nd_cud = DPP_MC_GROUP_2ND_CUD_NONE;
    for (; i < SOC_DPP_DEFS_MAX(NOF_CORES); ++ i) {
        mcds->nof_egr_bitmap_reps[i] = mcds->nof_egr_outlif_reps[i] = mcds->nof_egr_port_outlif_reps[i] = mcds->nof_reps[i] = 0;
    }
}


uint32 dpp_mcds_copy_replications_to_arrays(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8        is_egress,           
    SOC_SAND_IN  uint32       arrays_size,         
    SOC_SAND_OUT soc_gport_t  *port_array,         
    SOC_SAND_OUT soc_if_t     *encap_id_array,     
    SOC_SAND_OUT soc_multicast_replication_t *reps 
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint16 size;
    dpp_rep_data_t *rep;
    int is_normal_egress = mcds->nof_ingr_reps == 0;
    int swap_cuds = mcds->info_2nd_cud == DPP_MC_GROUP_2ND_CUD_OUTRIF;
    soc_gport_t out_gport;
    soc_if_t out_cud, out_cud2;
    SOCDNX_INIT_FUNC_DEFS;

    
    DPP_MC_ASSERT((is_normal_egress || (mcds->nof_egr_port_outlif_reps[0] | mcds->nof_egr_outlif_reps[0] | mcds->nof_egr_bitmap_reps[0]) == 0) &&
      (SOC_IS_JERICHO(unit) || !swap_cuds));

    
    if (is_egress) {
        int core;
        soc_port_t local_logical_port;
        uint32 total_size = 0;
        for (core = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; core > 0;) {
          --core;
            size = mcds->nof_reps[core];
            total_size += size;
            if (total_size > arrays_size) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many egress replications to return")));
            }

            
            for (rep = mcds->reps + core * DPP_MULT_MAX_REPLICATIONS; size; --size, ++rep) {
                out_cud = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep); 
                switch (DPP_MCDS_REP_DATA_GET_TYPE(rep)) {
                  case DPP_MCDS_REP_TYPE_EGR_PORT_CUD:
                    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                        _SHR_GPORT_LOCAL_SET(out_gport, DPP_MCDS_REP_DATA_GET_EGR_PORT(rep) & ~(1 << 7)); 
                        out_cud |= ((DPP_MCDS_REP_DATA_GET_EGR_PORT(rep) & (soc_if_t)(1 << 7)) << 9); 
                    } else {
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, DPP_MCDS_REP_DATA_GET_EGR_PORT(rep), &local_logical_port));
                        _SHR_GPORT_LOCAL_SET(out_gport, local_logical_port); 
                    }
                    break;
                  case DPP_MCDS_REP_TYPE_EGR_CUD:
                    out_gport = _SHR_GPORT_INVALID ; 
                    break;
                  case DPP_MCDS_REP_TYPE_EGR_BM_CUD:
                    _SHR_GPORT_MCAST_SET(out_gport, DPP_MCDS_REP_DATA_GET_EGR_BM_ID(rep) & DPP_MC_EGR_MAX_BITMAP_ID); 
                    out_cud |= ((DPP_MCDS_REP_DATA_GET_EGR_BM_ID(rep) & (1 << 14)) << 2); 
                    break;
                  default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unexpected replication type")));
                }
                if (reps) {
                    if (swap_cuds) { 
                        if ((out_cud2 = DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep)) == DPP_MC_NO_2ND_CUD) {
                            reps->flags = 0;
                            reps->encap1 = out_cud;
                            reps->encap2 = DPP_MC_NO_2ND_CUD;
                        } else { 
                            reps->flags = SOC_MUTICAST_REPLICATION_ENCAP2_VALID | SOC_MUTICAST_REPLICATION_ENCAP1_L3_INTF;
                            reps->encap1 = out_cud2;
                            reps->encap2 = out_cud;
                        }
                    } else { 
                        reps->encap2 = DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep);
                        reps->encap1 = out_cud;
                        reps->flags = (reps->encap2 == DPP_MC_NO_2ND_CUD) ? 0 : SOC_MUTICAST_REPLICATION_ENCAP2_VALID;
                    }
                    reps++->port = out_gport;   
                } else {
                    *(port_array++) = out_gport;
                    *(encap_id_array++) = out_cud;
                }
            }
        }

    } else { 

        size = mcds->nof_reps[0];
        if (size > arrays_size) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many ingress replications to return")));
        }
        for (rep = mcds->reps; size; --size, ++rep) {
            DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_INGRESS);
            
            SOCDNX_IF_ERR_EXIT(mcds->convert_ingress_replication_hw2api(unit, DPP_MCDS_REP_DATA_GET_INGR_CUD(rep),
              DPP_MCDS_REP_DATA_GET_INGR_DEST(rep), port_array, encap_id_array));
              ++port_array;
            ++encap_id_array;
        }

    } 

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t _get_tm_port_and_core_from_gport(int unit, const uint32 port, uint32 *tm_port, uint32 *port_core)
{
    int core;
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, tm_port, &core));
        if (*tm_port >= DPP_MULT_EGRESS_PORT_INVALID) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid port")));
        }
        DPP_MC_ASSERT(core >= 0 && core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);
        *port_core = core;
    } else { 
        *tm_port = port;
        *port_core = 0;
    }
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mcds_copy_replications_from_arrays(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8     is_egress,       
    SOC_SAND_IN  uint8     do_clear,        
    SOC_SAND_IN  uint32    arrays_size,     
    SOC_SAND_IN  dpp_mc_replication_t *reps 
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 size_left = arrays_size;
    uint32 nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_egress) {

        uint32 core = 0, port;
        uint32 cud, cud2;
        uint8 cud2_type;
        if (do_clear) {
            dpp_mcds_clear_replications(mcds, DPP_MCDS_TYPE_VALUE_EGRESS);
        }
        for (; size_left; --size_left) {
            port = reps->dest;
            cud = reps->cud;
            cud2 = (reps++)->additional_cud;
            cud2_type = DPP_MC_2ND_CUD_TYPE_REP2MCDS(cud2 & DPP_MC_2ND_CUD_IS_EEI);
            if (cud2 != DPP_MC_NO_2ND_CUD) { 
                if (mcds->info_2nd_cud == DPP_MC_GROUP_2ND_CUD_NONE) { 
                    mcds->info_2nd_cud = cud2_type;
                } else if (cud2_type != mcds->info_2nd_cud) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can not mix different types of a 2nd CUD")));
                }
                cud2 &= ~DPP_MC_2ND_CUD_IS_EEI;
            }

            if (port == _SHR_GPORT_INVALID) { 
                if (nof_active_cores > 1) { 
                    DPP_CUD2CORE_GET_CORE(unit, cud, core);
                    if (core == DPP_CUD2CORE_UNDEF_VALUE) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD 0x%x is not mapped to a port/core"), cud));
                    }
                }
                dpp_add_egress_replication_cud(mcds, core, cud, cud2);
            } else if (port & ARAD_MC_EGR_IS_BITMAP_BIT) { 
                for (core = SOC_DPP_CONFIG(mcds->unit)->core_mode.nof_active_cores; core > 0;) {
                    --core;
                    dpp_add_egress_replication_bitmap(mcds, core, cud, cud2, port & ~ARAD_MC_EGR_IS_BITMAP_BIT);
                }
            } else { 
                SOCDNX_IF_ERR_EXIT(_get_tm_port_and_core_from_gport(unit, port, &port, &core));
                dpp_add_egress_replication_port_cud(mcds, core, cud, cud2, port);
            }
        }
        for (core = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; core > 0;) { 
            --core;
            DPP_MC_ASSERT(mcds->nof_reps[core] == (int32)mcds->nof_egr_port_outlif_reps[core] + mcds->nof_egr_outlif_reps[core] + mcds->nof_egr_bitmap_reps[core] && mcds->nof_ingr_reps == 0);
            if (mcds->nof_reps[core] > DPP_MULT_MAX_EGRESS_REPLICATIONS) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
            }
        }

    } else { 

        if (do_clear) {
            dpp_mcds_clear_replications(mcds, DPP_MCDS_TYPE_VALUE_INGRESS);
        }
        DPP_MC_ASSERT(mcds->nof_reps[0] == mcds->nof_ingr_reps && (mcds->nof_egr_port_outlif_reps[0] | mcds->nof_egr_outlif_reps[0] | mcds->nof_egr_bitmap_reps[0]) == 0);
        if (mcds->nof_reps[0] + (int32)arrays_size > DPP_MULT_MAX_INGRESS_REPLICATIONS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
        }

        for (; size_left; --size_left) {
            uint32 dest = reps->dest;
            soc_if_t cud = (reps++)->cud;
            if (dest >= DPP_MC_ING_DESTINATION_DISABLED) { 
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid destination")));
            }
            dpp_add_ingress_replication(mcds, cud, dest);
        }

    } 

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  dpp_mcds_copy_replications_from_egress_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         do_clear,    
    SOC_SAND_IN  uint32                        block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t block_size,  
    SOC_SAND_INOUT uint32                      *cud2,       
    SOC_SAND_OUT uint32                        *next_entry  
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 cur_index = block_start;
  dpp_free_entries_block_size_t entries_remaining = block_size;
  uint16 nof_reps = 0, reps_left;
  SOCDNX_INIT_FUNC_DEFS;

  DPP_MC_ASSERT(block_start + block_size < MCDS_INGRESS_LINK_END(mcds));
  if (do_clear) {
    dpp_mcds_clear_replications(mcds, DPP_MCDS_TYPE_VALUE_EGRESS);
  } else {
    nof_reps = mcds->nof_reps[0];
  }
  DPP_MC_ASSERT(nof_reps == mcds->nof_egr_port_outlif_reps[0] + mcds->nof_egr_outlif_reps[0] +
    mcds->nof_egr_bitmap_reps[0] && mcds->nof_ingr_reps == 0 &&
    nof_reps < DPP_MULT_MAX_EGRESS_REPLICATIONS && mcds->nof_reps[0] == nof_reps);
  reps_left = DPP_MULT_MAX_EGRESS_REPLICATIONS - nof_reps;

  
  while (entries_remaining) {
    SOCDNX_IF_ERR_EXIT(mcds->get_replications_from_entry( 
      unit, 0, TRUE, cur_index, DPP_MCDS_TYPE_VALUE_EGRESS, cud2, &reps_left, &nof_reps, next_entry));
    ++cur_index;
    --entries_remaining;
    if (nof_reps > DPP_MULT_MAX_EGRESS_REPLICATIONS) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    } else if (entries_remaining && *next_entry == DPP_MC_EGRESS_LINK_PTR_END) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("block too small")));
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_remove_replication(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               group_type, 
    SOC_SAND_IN  uint32               dest,       
    SOC_SAND_IN  soc_if_t             cud,        
    SOC_SAND_IN  soc_if_t             cud2,       
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err,   
    SOC_SAND_OUT dpp_mc_core_bitmap_t *cores      
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  dpp_rep_data_t *rep = mcds->reps, rep_to_find = {0};
  uint16 size_left = 0, *specific_nof_reps;
  uint32 core = 0; 
  SOCDNX_INIT_FUNC_DEFS;
  *out_err = _SHR_E_NONE;

  
  if (DPP_MCDS_TYPE_IS_EGRESS(group_type)) { 

    if (dest == _SHR_GPORT_INVALID) { 
      if(SOC_IS_JERICHO(unit)) {
          DPP_CUD2CORE_GET_CORE(unit, cud, core);
          if (core >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD 0x%x to be removed is not mapped to a port/core"), cud));
          }
      }
      DPP_MCDS_REP_DATA_SET_TYPE(&rep_to_find, DPP_MCDS_REP_TYPE_EGR_CUD);
      DPP_MCDS_REP_DATA_SET_EGR_CUD(&rep_to_find, cud);
      DPP_MCDS_REP_DATA_SET_EXTRA_CUD(&rep_to_find, cud2);
      specific_nof_reps = &mcds->nof_egr_outlif_reps[core];
    } else if (dest & ARAD_MC_EGR_IS_BITMAP_BIT) { 
      const uint32 bitmap = dest & ~ARAD_MC_EGR_IS_BITMAP_BIT;
      dpp_rep_data_t *found_reps_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0};
      DPP_MCDS_REP_DATA_SET_TYPE(&rep_to_find, DPP_MCDS_REP_TYPE_EGR_BM_CUD);
      DPP_MCDS_REP_DATA_SET_EGR_CUD(&rep_to_find, cud);
      DPP_MCDS_REP_DATA_SET_EXTRA_CUD(&rep_to_find, cud2);
      DPP_MCDS_REP_DATA_SET_EGR_BM_ID(&rep_to_find, bitmap);
      for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
        
        rep = mcds->reps + DPP_MCDS_GET_REP_INDEX(core, 0);
        for (size_left = mcds->nof_reps[core]; size_left; --size_left, ++rep) {
          DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) != DPP_MCDS_REP_TYPE_INGRESS);
          if (!compare_dpp_rep_data_t(rep, &rep_to_find)) {
            found_reps_per_core[core] = rep;
            break;
          }
        }
        if (!size_left) {
          *out_err = _SHR_E_NOT_FOUND;
          break;
        }
      }
      if (*out_err != _SHR_E_NOT_FOUND) { 
        for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
          --mcds->nof_reps[core];
          --mcds->nof_egr_bitmap_reps[core];
          *found_reps_per_core[core] = mcds->reps[DPP_MCDS_GET_REP_INDEX(core, mcds->nof_reps[core])];
        }
      }
      if (cores != NULL) {
        *cores = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
      }
      SOC_EXIT;
    } else { 
      uint32 tm_port;
      SOCDNX_IF_ERR_EXIT(_get_tm_port_and_core_from_gport(unit, dest, &tm_port, &core));
      DPP_MCDS_REP_DATA_SET_TYPE(&rep_to_find, DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      DPP_MCDS_REP_DATA_SET_EGR_CUD(&rep_to_find, cud);
      DPP_MCDS_REP_DATA_SET_EXTRA_CUD(&rep_to_find, cud2);
      DPP_MCDS_REP_DATA_SET_EGR_PORT(&rep_to_find, tm_port);
      specific_nof_reps = &mcds->nof_egr_port_outlif_reps[core];
    }

    rep = mcds->reps + DPP_MCDS_GET_REP_INDEX(core, 0);
    for (size_left = mcds->nof_reps[core]; size_left; --size_left, ++rep) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) != DPP_MCDS_REP_TYPE_INGRESS);
      if (!compare_dpp_rep_data_t(rep, &rep_to_find)) {
        break;
      }
    }

  } else if (DPP_MCDS_TYPE_IS_INGRESS(group_type)) { 

    DPP_MCDS_REP_DATA_SET_TYPE(&rep_to_find, DPP_MCDS_REP_TYPE_INGRESS);
    DPP_MCDS_REP_DATA_SET_INGR_CUD(&rep_to_find, cud);
    DPP_MCDS_REP_DATA_SET_INGR_DEST(&rep_to_find, dest);
    specific_nof_reps = &mcds->nof_ingr_reps;
    for (size_left = mcds->nof_reps[0]; size_left; --size_left, ++rep) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_INGRESS);
      if (!compare_dpp_rep_data_t(rep, &rep_to_find)) {
        break;
      }
    }

  } else { 
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unexpected entry type")));
  }

  if (!size_left) {
    *out_err = _SHR_E_NOT_FOUND;
  } else { 
    --mcds->nof_reps[core];
    --*specific_nof_reps;
    *rep = mcds->reps[DPP_MCDS_GET_REP_INDEX(core, mcds->nof_reps[core])];
  }
  if (cores != NULL) {
    *cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
  }
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mcds_unoccupied_get(
    SOC_SAND_IN dpp_mcds_base_t *mcds
)
{
  return mcds->nof_unoccupied;
}

void
  dpp_mcds_unoccupied_increase(
    SOC_SAND_INOUT dpp_mcds_base_t *mcds,
    SOC_SAND_IN    uint32               delta
)
{
  mcds->nof_unoccupied += delta;
  DPP_MC_ASSERT(mcds->nof_unoccupied < DPP_LAST_MCDB_ENTRY(mcds));
}

void
  dpp_mcds_unoccupied_decrease(
    SOC_SAND_INOUT dpp_mcds_base_t *mcds,
    SOC_SAND_IN    uint32               delta
)
{
  DPP_MC_ASSERT(mcds->nof_unoccupied >= delta);
  mcds->nof_unoccupied -= delta;
}


dpp_free_entries_blocks_region_t* dpp_mcds_get_region(dpp_mcds_base_t *mcds, uint32 mcdb_index)
{
  DPP_MC_ASSERT(mcds && mcdb_index > 0 && mcdb_index < DPP_LAST_MCDB_ENTRY(mcds));

  if (mcdb_index >= mcds->ingress_alloc_free.range_start && mcdb_index <= mcds->ingress_alloc_free.range_end) {
    return &mcds->ingress_alloc_free;
  }
  if (mcdb_index >= mcds->egress_alloc_free.range_start && mcdb_index <= mcds->egress_alloc_free.range_end) {
    return &mcds->egress_alloc_free;
  }
  return &mcds->free_general;
}



dpp_free_entries_blocks_region_t*
  dpp_mcds_get_region_and_consec_range(dpp_mcds_base_t *mcds, uint32 mcdb_index, uint32 *range_start, uint32 *range_end)
{
  dpp_free_entries_blocks_region_t *range = dpp_mcds_get_region(mcds, mcdb_index);
  *range_start = range->range_start;
  *range_end = range->range_end;
  
  
  if (range == &mcds->free_general) { 
    if (mcdb_index < mcds->ingress_alloc_free.range_start) {
      if (*range_end >= mcds->ingress_alloc_free.range_start) {
        *range_end = mcds->ingress_alloc_free.range_start - 1;
      }
    } else if (mcdb_index > mcds->egress_alloc_free.range_end) {
      if (*range_start <= mcds->egress_alloc_free.range_end) {
        *range_start = mcds->egress_alloc_free.range_end + 1;
      }
    } else if (mcdb_index > mcds->ingress_alloc_free.range_end && mcdb_index < mcds->egress_alloc_free.range_start) {
      if (*range_end >= mcds->egress_alloc_free.range_start) {
        *range_end = mcds->egress_alloc_free.range_start - 1;
      }
      if (*range_start <= mcds->ingress_alloc_free.range_end) {
        *range_start = mcds->ingress_alloc_free.range_end + 1;
      }
    }
  }
  DPP_MC_ASSERT(*range_start <= *range_end && mcdb_index >= *range_start && mcdb_index <= *range_end);
  return range;
}


uint32 dpp_mcdb_copy_write(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 src_index, 
    SOC_SAND_IN uint32 dst_index  
)
{
  uint32 data[DPP_MC_ENTRY_SIZE];
  dpp_mcds_base_t* mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *src_entry = MCDS_GET_MCDB_ENTRY(mcds, src_index);
  arad_mcdb_entry_t *dst_entry = MCDS_GET_MCDB_ENTRY(mcds, dst_index);
  SOCDNX_INIT_FUNC_DEFS;

  data[0] = dst_entry->word0 = src_entry->word0;
  dst_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
  dst_entry->word1 |= src_entry->word1 & ~DPP_MCDS_WORD1_KEEP_BITS_MASK;
  data[1] = src_entry->word1 & mcds->msb_word_mask;
  SOCDNX_IF_ERR_EXIT(WRITE_IRR_MCDBm(unit, MEM_BLOCK_ANY, dst_index, data));
  mcds->prev_entries[dst_index] = mcds->prev_entries[src_index];

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC INLINE void
  dpp_mcds_init_free_entries_block_list(dpp_free_entries_block_list_t *list)
{
  list->first = DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY;
}

void _dpp_mcds_remove_free_entries_block_from_list(dpp_mcds_base_t *mcds, dpp_free_entries_block_list_t *list, uint32 block, const dpp_free_entries_block_size_t block_size)
{
  uint32 next, prev;

  DPP_MC_ASSERT(block < DPP_LAST_MCDB_ENTRY(mcds) && block > 0);
  DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
  DPP_MC_ASSERT(block_size > 0 && block_size == DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, block));
  DPP_MC_ASSERT(list == dpp_mcds_get_region(mcds, block)->lists + (block_size-1));
  next = DPP_MCDS_GET_FREE_NEXT_ENTRY(mcds, block);
  prev = DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, block);
  if (next == block) { 
    DPP_MC_ASSERT(prev == block && list->first == block);
    list->first = DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY; 
  } else { 
    DPP_MC_ASSERT(prev != block && 
      DPP_MCDS_GET_TYPE(mcds, prev) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
      DPP_MCDS_GET_TYPE(mcds, next) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    DPP_MC_ASSERT(DPP_MCDS_GET_FREE_NEXT_ENTRY(mcds, prev) == block &&
                DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, next) == block);
    DPP_MCDS_SET_FREE_NEXT_ENTRY(mcds, prev, next);
    DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, next, prev);
    if (list->first == block) {
      list->first = next; 
    }
  }
  LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
              (BSL_META("removed(%u,%u) "), block, block_size));
  dpp_mcds_unoccupied_decrease(mcds, block_size); 
}


STATIC INLINE void
  dpp_mcds_remove_free_entries_block_from_list(dpp_mcds_base_t *mcds, dpp_free_entries_block_list_t *list, uint32 block)
{
  _dpp_mcds_remove_free_entries_block_from_list(mcds, list, block, DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, block));
}


STATIC INLINE void
  dpp_mcds_remove_free_entries_block_from_region(dpp_mcds_base_t *mcds, dpp_free_entries_blocks_region_t *region, uint32 block, dpp_free_entries_block_size_t block_size)
{
  dpp_free_entries_block_list_t *list = region->lists + (block_size-1);

  DPP_MC_ASSERT(block_size <= region->max_size);
  _dpp_mcds_remove_free_entries_block_from_list(mcds, list, block, block_size);
}


STATIC INLINE int
  dpp_mcds_is_empty_free_entries_block_list(const dpp_mcds_base_t *mcds, const dpp_free_entries_block_list_t *list)
{
  if (list->first == DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY)
    return 1;
  DPP_MC_ASSERT(list->first < DPP_LAST_MCDB_ENTRY(mcds));
  return 0;
}


void
  dpp_mcds_add_free_entries_block_to_list(dpp_mcds_base_t *mcds, dpp_free_entries_block_list_t *list, uint32 block)
{
  uint32 next, prev;
  dpp_free_entries_block_size_t block_size = DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, block);

  if (dpp_mcds_is_empty_free_entries_block_list(mcds, list)) {
    list->first = prev = next = block;
  } else {
    next = list->first;
    prev = DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, next);
    DPP_MC_ASSERT(DPP_MCDS_GET_FREE_NEXT_ENTRY(mcds, prev) == next &&
      DPP_MCDS_GET_TYPE(mcds, next) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, next, block);
    DPP_MCDS_SET_FREE_NEXT_ENTRY(mcds, prev, block);
  }
  DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, block, prev);
  DPP_MCDS_SET_FREE_NEXT_ENTRY(mcds, block, next);
  dpp_mcds_unoccupied_increase(mcds, block_size);
} 


uint32 dpp_mcds_get_free_entries_block_from_list(dpp_mcds_base_t *mcds, dpp_free_entries_block_list_t *list, int to_remove)
{
  uint32 block = list->first;
  if (block == DPP_MC_FREE_ENTRIES_BLOCK_LIST_EMPTY)
    return 0;
  DPP_MC_ASSERT(block < DPP_LAST_MCDB_ENTRY(mcds));
  DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);

  if (to_remove) {
    dpp_mcds_remove_free_entries_block_from_list(mcds, list, block);
  }
  return block;
}


void dpp_mcds_init_region(dpp_free_entries_blocks_region_t *region, dpp_free_entries_block_size_t max_size, uint32 range_start, uint32 range_end)
{
  unsigned i;
  dpp_free_entries_block_list_t *list_p = region->lists;
  region->max_size = max_size;
  region->range_start = range_start;
  region->range_end = range_end;
  for (i = 0; i < max_size; ++i) {
    dpp_mcds_init_free_entries_block_list(list_p);
    ++list_p;
  }
}



#define DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV  1 
#define DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT  2 
#define DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE          3 
STATIC uint32 dpp_mcds_create_free_entries_block(
    SOC_SAND_INOUT dpp_mcds_base_t                 *mcds,
    SOC_SAND_IN    uint32                           flags,             
    SOC_SAND_IN    uint32                           block_start_index, 
    SOC_SAND_IN    dpp_free_entries_block_size_t    block_size,        
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region            
)
{
  int unit = mcds->unit;
  uint32 i, current_block_start_index = block_start_index;
  uint32 block_end = block_start_index + block_size; 
  dpp_free_entries_block_size_t current_block_size = block_size, joint_block_size;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(region);
  if (block_start_index + block_size > MCDS_INGRESS_LINK_END(mcds) || !block_start_index) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("block out of range")));
  }
  if (block_size > region->max_size || block_size < 1) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid block size")));
  }

  
  for (i = block_start_index; i < block_end; ++i) {
    if (DPP_MCDS_TYPE_IS_USED(DPP_MCDS_GET_TYPE(mcds, i))) {
      DPP_MC_ASSERT(0); 
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("attempted to add a used entry number %u to a free block"), i));
    }
  }

  
  if (block_size < region->max_size) {
    const uint32 next_block = block_start_index + block_size;
    uint32 prev_block = block_start_index - 1;
    dpp_free_entries_block_size_t prev_block_size = 0, next_block_size = 0;
    
    if (!(flags & DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV) && 
        prev_block >= region->range_start && prev_block <= region->range_end &&
        region == dpp_mcds_get_region(mcds, prev_block) &&
        DPP_MCDS_TYPE_IS_FREE(i = DPP_MCDS_GET_TYPE(mcds, prev_block))) {
      prev_block_size = 1;
      if (i != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) { 
        prev_block = DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, prev_block);
        prev_block_size = block_start_index - prev_block;
        DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, prev_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
          prev_block < block_start_index - 1 && prev_block_size <= region->max_size);
      }
      DPP_MC_ASSERT(prev_block_size == DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, prev_block));
      if (prev_block_size == region->max_size) {
        prev_block_size = 0; 
      }
    }
 
    if (!(flags & DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT) && 
        next_block >= region->range_start && next_block <= region->range_end &&
        region == dpp_mcds_get_region(mcds, next_block) &&
        DPP_MCDS_GET_TYPE(mcds, next_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
      next_block_size = DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, next_block);
      if (next_block_size == region->max_size) {
        next_block_size = 0; 

      } else if (prev_block_size) { 
        if (block_size + prev_block_size <= region->max_size) { 
        } else if (block_size + next_block_size <= region->max_size) { 
          prev_block_size = 0;
        } else if (prev_block_size > next_block_size) { 
          prev_block_size = 0;
        }
      }
    }

    if (prev_block_size) { 

      joint_block_size = prev_block_size + block_size;
      if (joint_block_size > region->max_size) {
        current_block_size = joint_block_size - region->max_size;
        joint_block_size = region->max_size;
      } else {
        current_block_size = 0;
      }
      current_block_start_index = prev_block + joint_block_size;
      DPP_MC_ASSERT(joint_block_size + current_block_size == prev_block_size + block_size &&
        prev_block + joint_block_size == block_start_index + block_size - current_block_size);
      dpp_mcds_remove_free_entries_block_from_region(mcds, region, prev_block, prev_block_size); 

      LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                  (BSL_META_U(unit,
                              "merge with prev free block: prev:%u,%u  freed:%u,%u\n"), prev_block, prev_block_size, block_start_index, block_size));
      DPP_MCDS_SET_FREE_BLOCK_SIZE(mcds, prev_block, joint_block_size); 
      
      for (i = block_start_index; i < current_block_start_index; ++i) {
        DPP_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
        DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, i, prev_block);
      }
      dpp_mcds_add_free_entries_block_to_list(mcds, region->lists + (joint_block_size-1), prev_block); 

    } else if (next_block_size) { 

      joint_block_size = next_block_size + block_size;
      if (joint_block_size > region->max_size) {
        current_block_size = joint_block_size - region->max_size;
        joint_block_size = region->max_size;
      } else {
        current_block_size = 0;
      }
      current_block_start_index += joint_block_size;
      DPP_MC_ASSERT(joint_block_size + current_block_size == next_block_size + block_size &&
        block_start_index + joint_block_size == next_block + next_block_size - current_block_size);

      LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                  (BSL_META_U(unit,
                              "merge with next free block: next:%u,%u  freed:%u,%u\n"), next_block, next_block_size, block_start_index, block_size));
      dpp_mcds_remove_free_entries_block_from_region(mcds, region, next_block, next_block_size); 
      
      DPP_MCDS_SET_FREE_BLOCK_SIZE(mcds, block_start_index, joint_block_size);  \
      DPP_MCDS_SET_TYPE(mcds, block_start_index, DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
      for (i = block_start_index + 1; i < current_block_start_index; ++i) {
        DPP_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
        DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, i, block_start_index);
      }
      dpp_mcds_add_free_entries_block_to_list(mcds, region->lists + (joint_block_size-1), block_start_index); 
 
    }

  }

  if (current_block_size) {
    
    DPP_MCDS_SET_FREE_BLOCK_SIZE(mcds, current_block_start_index, current_block_size);  \
    LOG_VERBOSE(BSL_LS_SOC_MULTICAST,
                (BSL_META_U(unit,
                            "added free block: %u,%u\n"), current_block_start_index, current_block_size));
    
    DPP_MCDS_SET_TYPE(mcds, current_block_start_index, DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
    block_end = current_block_start_index + current_block_size;
    for (i = current_block_start_index + 1; i < block_end; ++i) {
      DPP_MCDS_SET_TYPE(mcds, i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
      DPP_MCDS_SET_FREE_PREV_ENTRY(mcds, i, current_block_start_index);
    }

    
    dpp_mcds_add_free_entries_block_to_list(mcds, region->lists + (current_block_size-1), current_block_start_index);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mcds_build_free_blocks(
    SOC_SAND_IN    int                              unit,   
    SOC_SAND_INOUT dpp_mcds_base_t                  *mcds,
    SOC_SAND_IN    uint32                           start_index, 
    SOC_SAND_IN    uint32                           end_index,   
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    mcds_free_build_option_t         entry_option 
)
{
  dpp_free_entries_block_size_t max_size, block_size;
  uint32 block_start, cur_entry;
  int check_free = entry_option == McdsFreeBuildBlocksAddOnlyFree;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(region);
  DPP_MC_ASSERT(check_free || mcds == dpp_get_mcds(unit));
  if (start_index > end_index) {
    SOC_EXIT;
  }
  if (end_index >= MCDS_INGRESS_LINK_END(mcds) || !start_index) { 
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("block out of range")));
  }

  max_size = region->max_size;

  for (block_start = start_index; block_start <= end_index; block_start += block_size) { 
    if (check_free) {
      block_size = 0;
      for (; block_start <= end_index && DPP_MCDS_TYPE_IS_USED(DPP_MCDS_GET_TYPE(mcds, block_start));
           ++block_start) {} 
      if (block_start <= end_index) { 
        block_size = 1;
        for (cur_entry = block_start + 1; block_size < max_size && cur_entry <= end_index && 
             DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_GET_TYPE(mcds, cur_entry)); ++cur_entry ) {
          ++block_size;
        }
      }
    } else { 
      block_size = block_start + max_size <= end_index ? max_size : end_index - block_start + 1;
    }
    if (block_size) { 
      DPP_MC_ASSERT(block_size <= max_size);
      if (!check_free) { 
        dpp_free_entries_block_size_t i;
        for (i = 0; i < block_size; ++i) {
          DPP_MC_ASSERT(entry_option != McdsFreeBuildBlocksAdd_AllMustBeUsed ||
            DPP_MCDS_TYPE_IS_USED(DPP_MCDS_GET_TYPE(mcds, block_start + i)));
          DPP_MCDS_SET_TYPE(mcds, block_start + i, DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
          
          SOCDNX_IF_ERR_EXIT(WRITE_IRR_MCDBm(unit, MEM_BLOCK_ANY, block_start + i, ((dpp_mcds_base_t*)mcds)->free_value));
        }
      }
      SOCDNX_IF_ERR_EXIT( 
        dpp_mcds_create_free_entries_block(mcds, 0, block_start, block_size, region));
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  dpp_mcds_multicast_init(
    SOC_SAND_IN int      unit
)
{
  uint32 start, end;
  dpp_mcds_base_t *mcds = NULL;
  int failed = 1;
  uint32 table_size;
  uint32 nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
  uint8 is_allocated;
  soc_error_t rv;

  SOCDNX_INIT_FUNC_DEFS;
  if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCDS not initialized")));
  }
  SOCDNX_IF_ERR_EXIT(dpp_init_mcds(unit)); 
  mcds = dpp_get_mcds(unit);
  table_size = DPP_LAST_MCDB_ENTRY(mcds) + 1;

  
  mcds->nof_unoccupied = 0;
  mcds->mcdb = NULL;
  mcds->prev_entries = NULL;

  start = SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_start;
  end   = SOC_DPP_CONFIG(unit)->tm.ingress_mc_id_alloc_range_end;
  if (start < 1) {
    start = 1;
  }
  if (end >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
    end = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1;
  }
  if (end + 1 <= start)  { 
    start = DPP_LAST_MCDB_ENTRY(mcds) + 1;
    end = DPP_LAST_MCDB_ENTRY(mcds);
  }
  dpp_mcds_init_region(&mcds->ingress_alloc_free, DPP_MCDS_MAX_FREE_BLOCK_SIZE_ALLOCED, start, end);
  dpp_mcds_init_region(&mcds->free_general, DPP_MCDS_MAX_FREE_BLOCK_SIZE_GENERAL, 1, DPP_LAST_MCDB_ENTRY(mcds) - 1); 
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
    start = ARAD_MULT_NOF_MULTICAST_GROUPS + SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_start;
    end   = ARAD_MULT_NOF_MULTICAST_GROUPS + SOC_DPP_CONFIG(unit)->tm.egress_mc_id_alloc_range_end;
    if (end >= DPP_LAST_MCDB_ENTRY(mcds)) {
      end = DPP_LAST_MCDB_ENTRY(mcds) - 1;
    }
  } else {
    start = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1, 0);
    end = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1, nof_active_cores - 1);
    DPP_MC_ASSERT(start == SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids &&
      end == SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + nof_active_cores * mcds->nof_egr_ll_groups - 1 && end < DPP_LAST_MCDB_ENTRY(mcds));
  }
  dpp_mcds_init_region(&mcds->egress_alloc_free, DPP_MCDS_MAX_FREE_BLOCK_SIZE_ALLOCED, start, end);

  assert(sizeof(arad_mcdb_entry_t) == sizeof(uint32)*DPP_MC_ENTRY_SIZE &&
         sizeof(int) >= sizeof(int32)); 

  { 
    SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &mcds->mcdb, sizeof(arad_mcdb_entry_t) * table_size, "mcds-mc-mcdb"));
    SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &mcds->prev_entries, sizeof(uint16) * table_size, "mcds-mc-prev_entries"));
    if(!SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.is_allocated(unit, &is_allocated);
        SOCDNX_IF_ERR_EXIT(rv);
        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.alloc(unit);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.alloc_bitmap(unit, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
        SOCDNX_IF_ERR_EXIT(rv);
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.cud2core.alloc_bitmap(unit, mcds->max_egr_cud_field *DPP_CUD2CORE_BITS_PER_CUD);
        SOCDNX_IF_ERR_EXIT(rv);
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.cud2core.bit_range_set(unit,
            0,
            mcds->max_egr_cud_field *DPP_CUD2CORE_BITS_PER_CUD);
        SOCDNX_IF_ERR_EXIT(rv);
        
    }
  }
#ifdef _ARAD_MC_TEST_EXTRA_INITIALIZATION_0 
  SOCDNX_IF_ERR_EXIT(soc_sand_os_memset(mcds->prev_entries, DPP_CUD2CORE_INITIAL_BTYE_VALUE, sizeof(uint16)*table_size));
#endif

 failed = 0;
exit:
  if (failed && mcds) {
    dpp_mcds_multicast_terminate(unit);
  }
  SOCDNX_FUNC_RETURN;
}


uint32
  dpp_mcds_multicast_init2(
    SOC_SAND_IN int      unit
)
{
  unsigned i;
  uint32 *alloced_mem = NULL; 
  uint32 *dest32;
  arad_mcdb_entry_t *dest;
  dpp_mcds_base_t* mcds = dpp_get_mcds(unit);
  uint32 table_size, irdb_table_nof_entries, r32;
  int do_not_read = !SOC_WARM_BOOT(unit);
  int use_dma = !do_not_read &&
#ifdef PLISIM
    !SAL_BOOT_PLISIM &&
#endif
      soc_mem_dmaable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm)); 
  int failed = 1;
  uint32 range_start, range_end, last_end;

  SOCDNX_INIT_FUNC_DEFS;
  if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
    SOC_EXIT;
  }
  if (!mcds || !mcds->mcdb || !mcds->prev_entries) { 
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCDS not initialized")));
  }
  table_size = DPP_LAST_MCDB_ENTRY(mcds) + 1;
  irdb_table_nof_entries = table_size / (2 * IRDB_TABLE_GROUPS_PER_ENTRY); 

  if (!SOC_WARM_BOOT(unit)) {
    if (!SOC_DPP_CONFIG(unit)->arad->init.pp_enable) {
      SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INVALID_OUTLIFr(unit, REG_PORT_ANY, DPP_MC_EGR_CUD_INVALID));
      SOCDNX_IF_ERR_EXIT(WRITE_EPNI_INVALID_OUTLIFr(unit, REG_PORT_ANY, DPP_MC_EGR_CUD_INVALID));
    }
    if (SOC_IS_JERICHO(unit)) { 
      
      for (i = 0; i < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++i) {
        uint32 offset = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, 0, i);
        if (offset >= JER_MCDB_SIZE ) {
            offset += JER_MCDB_SIZE;
            DPP_MC_ASSERT(i == 0 && offset < JER_MCDB_SIZE);
        }
        SOCDNX_IF_ERR_EXIT(READ_EGQ_MULTICAST_OFFSET_ADDRESSr(unit, i, &r32));
        soc_reg_field_set(unit, EGQ_MULTICAST_OFFSET_ADDRESSr, &r32, MCDB_OFFSETf, offset);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MULTICAST_OFFSET_ADDRESSr(unit, i, r32));
      }
    }
  }

  dest = mcds->mcdb;

  
  
  if (use_dma) { 
    arad_mcdb_entry_t *src;
    
    alloced_mem = soc_cm_salloc(unit, 4 * (table_size * DPP_MC_ENTRY_SIZE), "dma-mc-buffer");
    if (alloced_mem == NULL) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate DMA buffer")));
    }
    SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, IRR_MCDBm, MEM_BLOCK_ANY, 0, DPP_LAST_MCDB_ENTRY(mcds), alloced_mem));

    
    src = (void*)alloced_mem;
    for (i = table_size ; i ; --i) {
      dest->word0 = src->word0;
      dest++->word1 = src++->word1 & mcds->msb_word_mask;
    }

  } else { 
    SOCDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &alloced_mem, sizeof(uint32) * irdb_table_nof_entries * IRDB_TABLE_ENTRY_WORDS, "mcds-irdb-tmp"));

    

    if (do_not_read) { 
      for (i = DPP_LAST_MCDB_ENTRY(mcds) + 1 ; i ; --i) {
        dest->word0 = ((dpp_mcds_base_t*)mcds)->free_value[0];
        dest++->word1 = ((dpp_mcds_base_t*)mcds)->free_value[1];
      }
    } else {
      SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, IRR_MCDBm, MEM_BLOCK_ANY, 0, DPP_LAST_MCDB_ENTRY(mcds), dest));
      for (i = table_size; i; --i) {
        dest++->word1 &= mcds->msb_word_mask;
      }
    }

  }
  if (!do_not_read) {
    SOCDNX_IF_ERR_EXIT(soc_mem_read_range(unit, IDR_IRDBm, MEM_BLOCK_ANY, 0, irdb_table_nof_entries - 1, alloced_mem));
  }

#ifdef BCM_WARM_BOOT_SUPPORT 
  if (SOC_WARM_BOOT(unit)  
     ) {
    
    uint32 mcid = 0, highest_bitmap_group = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;
    unsigned j;
    uint16 group_entries;
    uint32 core_id, nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dest32 = alloced_mem;
    for (i = 0 ; i < irdb_table_nof_entries; ++i) {
      uint32 bits = *dest32;
      for (j = 0 ; j < IRDB_TABLE_GROUPS_PER_ENTRY; ++j) {
        if (bits & 1) { 
          
          arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, mcid);
          uint32 cur_entry = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);
          uint32 prev_entry = mcid;

          if (mcid >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids || DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
            DPP_MC_ASSERT(0); 
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("found invalid hardware table values")));
          }
          DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS_START); 
          DPP_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, mcds, mcid, prev_entry);
          group_entries = 1;
          while (cur_entry != MCDS_INGRESS_LINK_END(mcds)) { 
            mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, cur_entry);
            if (DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START || ++group_entries > DPP_MULT_MAX_INGRESS_REPLICATIONS) {
              DPP_MC_ASSERT(0);
              SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used or too many group entries")));
            }
            DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_INGRESS);
            DPP_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, mcds, cur_entry, prev_entry);
            prev_entry = cur_entry;
            cur_entry = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);
          }

        }
        bits >>= IRDB_TABLE_BITS_PER_GROUP;
        ++mcid;
      }
      dest32 += IRDB_TABLE_ENTRY_WORDS;
    }

    
    for (mcid = 0; mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++mcid) {
        uint8 bit_val;
        SOCDNX_IF_ERR_EXIT(
            sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(
                unit,
                mcid,
                &bit_val)
            );
        if (bit_val) { 
          if (mcid > highest_bitmap_group) {
            for (core_id = 0; core_id < nof_active_cores; ++core_id) {
              
              uint32 prev_entry = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core_id);
              arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, prev_entry);
              uint32 cur_entry;
  
              if (DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
                DPP_MC_ASSERT(0);
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used")));
              }
              DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START); 
              DPP_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, mcds, prev_entry, prev_entry);
              SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, prev_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START, &cur_entry)); 
              group_entries = 1;
              while (cur_entry != DPP_MC_EGRESS_LINK_PTR_END) { 
                mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, cur_entry);
                if (DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry) != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START || ++group_entries > DPP_MULT_MAX_INGRESS_REPLICATIONS) {
                  DPP_MC_ASSERT(0);
                  SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry already used or too many group entries")));
                }
                DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS);
                DPP_MCDS_ENTRY_SET_PREV_ENTRY(mcdb_entry, mcds, cur_entry, prev_entry);
                prev_entry = cur_entry;
                SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, prev_entry, DPP_MCDS_TYPE_VALUE_EGRESS, &cur_entry)); 
              }
            }
        }
      }
    }

  } else
#endif 
  {
    
    dest32 = alloced_mem;
    for (i = 0 ; i < irdb_table_nof_entries; ++i) {
      if (*dest32) {
              DPP_MC_ASSERT(0);
              SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Illegal IRR_MCDB content")));
      }
      dest32 += IRDB_TABLE_ENTRY_WORDS;
    }

  }

  

  {
    
    range_start = mcds->ingress_alloc_free.range_start;
    range_end = mcds->ingress_alloc_free.range_end;
    if (range_start <= DPP_LAST_MCDB_ENTRY(mcds) && range_end >= range_start) { 
      DPP_MC_ASSERT(range_start >= 1);
      
      SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, 1, range_start - 1, &mcds->free_general, McdsFreeBuildBlocksAddOnlyFree));
      
      SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, range_start, range_end, &mcds->ingress_alloc_free, McdsFreeBuildBlocksAddOnlyFree));
      last_end = range_end;
    } else {
      last_end = 0; 
    }
    
    range_start = mcds->egress_alloc_free.range_start;
    range_end = mcds->egress_alloc_free.range_end;
    if (range_start <= DPP_LAST_MCDB_ENTRY(mcds) && range_end >= range_start) { 
      DPP_MC_ASSERT(range_start > last_end);
      
      SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks( unit, mcds, last_end + 1, range_start - 1, &mcds->free_general, McdsFreeBuildBlocksAddOnlyFree));
      
      if (range_end >= DPP_LAST_MCDB_ENTRY(mcds)) {
        range_end = DPP_LAST_MCDB_ENTRY(mcds) - 1;
      }
      
      SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, range_start, range_end, &mcds->egress_alloc_free, McdsFreeBuildBlocksAddOnlyFree));
      last_end = range_end;
    }
    
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, last_end + 1, DPP_LAST_MCDB_ENTRY(mcds) - 1, &mcds->free_general, McdsFreeBuildBlocksAddOnlyFree));
  }
  failed = 0;

exit:
  if (alloced_mem) {
    if (use_dma) {
      soc_cm_sfree(unit, alloced_mem);
    } else {
      sand_free_mem_if_not_null(unit, (void*)&alloced_mem);
    }
  }
  if (failed) {
    dpp_mcds_multicast_terminate(unit);
  }
  SOCDNX_FUNC_RETURN;
}


uint32
  dpp_mcds_split_free_entries_block(
    SOC_SAND_INOUT dpp_mcds_base_t             *mcds,    
    SOC_SAND_IN    uint32                          flags,       
    SOC_SAND_INOUT dpp_free_entries_blocks_region_t *region,     
    SOC_SAND_IN    dpp_free_entries_block_size_t    orig_size,   
    SOC_SAND_IN    dpp_free_entries_block_size_t    new_size,    
    SOC_SAND_INOUT uint32                          *block_start 
)
{
  int unit = mcds->unit;
  uint32 i;
  const uint32 next_block = *block_start + orig_size;
  uint32 prev_block = *block_start - 1;
  const dpp_free_entries_block_size_t remaining_size = orig_size - new_size;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mcds);
  SOCDNX_NULL_CHECK(block_start);
  if (orig_size > region->max_size || new_size < 1 || new_size >= orig_size) {
    DPP_MC_ASSERT(0);
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal size parameters")));
  }

  if (!(flags & DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT) &&  
      next_block >= region->range_start && next_block <= region->range_end &&
      region == dpp_mcds_get_region(mcds, next_block) &&
      DPP_MCDS_GET_TYPE(mcds, next_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) {
    dpp_free_entries_block_size_t merged_block_size = DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, next_block);
    dpp_free_entries_block_size_t joint_block_size = merged_block_size + remaining_size;
    if (joint_block_size <= region->max_size) { 
      DPP_MC_ASSERT(next_block - remaining_size == new_size + *block_start);
      dpp_mcds_remove_free_entries_block_from_region(mcds, region, next_block, merged_block_size);
      SOCDNX_IF_ERR_EXIT(dpp_mcds_create_free_entries_block( 
        mcds, DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, next_block - remaining_size, joint_block_size, region));
      goto exit;
    }
  }
  
  if (!(flags & DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV) &&  
             prev_block >= region->range_start && prev_block <= region->range_end &&
             region == dpp_mcds_get_region(mcds, prev_block) &&
             DPP_MCDS_TYPE_IS_FREE(i = DPP_MCDS_GET_TYPE(mcds, prev_block))) {
    dpp_free_entries_block_size_t merged_block_size = 1, joint_block_size;
    if (i != DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START) { 
      prev_block = DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, prev_block);
      merged_block_size = *block_start - prev_block;
      DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, prev_block) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START &&
        prev_block < *block_start - 1 && merged_block_size <= region->max_size);
    }
    DPP_MC_ASSERT(merged_block_size == DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, prev_block));
    joint_block_size = merged_block_size + remaining_size;
    if (joint_block_size <= region->max_size) { 
      DPP_MC_ASSERT(prev_block + joint_block_size == remaining_size + *block_start);
      dpp_mcds_remove_free_entries_block_from_region(mcds, region, prev_block, merged_block_size);
      SOCDNX_IF_ERR_EXIT(dpp_mcds_create_free_entries_block( 
        mcds, DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT, prev_block, joint_block_size, region));
      *block_start += remaining_size;
      goto exit;
    }
  }
  
  
  SOCDNX_IF_ERR_EXIT(dpp_mcds_create_free_entries_block( 
    mcds, DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, *block_start + new_size, remaining_size, region));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mcds_reserve_group_start(
    SOC_SAND_INOUT dpp_mcds_base_t *mcds,
    SOC_SAND_IN    uint32           mcdb_index 
)
{
  int unit = mcds->unit;
  uint32 entry_type;
  SOCDNX_INIT_FUNC_DEFS;
  DPP_MC_ASSERT(mcdb_index <= DPP_LAST_MCDB_ENTRY(mcds));

  entry_type = DPP_MCDS_GET_TYPE(mcds, mcdb_index);
  if (DPP_MCDS_TYPE_IS_USED(entry_type)) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("entry must be free")));
  }
  if (mcdb_index > 0 && mcdb_index < DPP_LAST_MCDB_ENTRY(mcds)) { 
    dpp_free_entries_blocks_region_t* region = dpp_mcds_get_region(mcds, mcdb_index);
    const uint32 block_start = entry_type == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START ?
      mcdb_index : DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, mcdb_index);
    const dpp_free_entries_block_size_t block_size = DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, block_start);
    const uint32 block_end = block_start + block_size - 1;
    DPP_MC_ASSERT(block_start <= mcdb_index && block_start + region->max_size >= mcdb_index && block_size <= region->max_size);

    dpp_mcds_remove_free_entries_block_from_region(mcds, region, block_start, block_size); 
    if (block_start < mcdb_index) { 
      SOCDNX_IF_ERR_EXIT(dpp_mcds_create_free_entries_block(
        mcds, DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_NEXT, block_start, mcdb_index - block_start, region));
    }
    if (block_end > mcdb_index) { 
      SOCDNX_IF_ERR_EXIT(dpp_mcds_create_free_entries_block(
        mcds, DPP_MCDS_SPLIT_FREE_BLOCKS_DONT_MERGE_TO_PREV, mcdb_index + 1, block_end - mcdb_index, region));
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mcds_get_free_entries_block(
    SOC_SAND_INOUT dpp_mcds_base_t              *mcds,
    SOC_SAND_IN    uint32                        flags,        
    SOC_SAND_IN    dpp_free_entries_block_size_t wanted_size,  
    SOC_SAND_IN    dpp_free_entries_block_size_t max_size,     
    SOC_SAND_OUT   uint32                        *block_start, 
    SOC_SAND_OUT   dpp_free_entries_block_size_t *block_size   
)
{
  int unit = mcds->unit;
  dpp_free_entries_blocks_region_t *regions[ARAD_MCDS_NOF_REGIONS];
  int do_change = !(flags & DPP_MCDS_GET_FREE_BLOCKS_NO_UPDATES);
  uint32 block = 0;
  int r, s, loop_start, loop_end;
  int size_loop1_start, size_loop1_increase;
  int size_loop2_start, size_loop2_increase;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mcds);
  SOCDNX_NULL_CHECK(block_start);
  SOCDNX_NULL_CHECK(block_size);
  if (wanted_size > DPP_MCDS_MAX_FREE_BLOCK_SIZE || wanted_size > max_size || 1 > wanted_size) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal wanted size")));
  }

  regions[0] = &mcds->free_general;
  if (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS) { 
    regions[1] = &mcds->ingress_alloc_free;
    regions[2] = &mcds->egress_alloc_free;
  } else {
    regions[1] = &mcds->egress_alloc_free;
    regions[2] = &mcds->ingress_alloc_free;
  }

  if (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_SMALL) { 
    size_loop1_start = wanted_size; size_loop1_increase = -1;
    size_loop2_start = wanted_size + 1; size_loop2_increase = 1;
  } else {
    size_loop1_start = wanted_size; size_loop1_increase = 1;
    size_loop2_start = wanted_size - 1; size_loop2_increase = -1;
  }

  if (flags & DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE) { 

    
    loop_start = size_loop1_start;
    if (size_loop1_increase >= 0) { 
      loop_end = DPP_MCDS_MAX_FREE_BLOCK_SIZE + 1;
    } else { 
      loop_end = 0;
    }
    for (s = loop_start; s != loop_end; s += size_loop1_increase) {
      for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
        dpp_free_entries_blocks_region_t *region = regions[r];
        if (region->max_size >= s) { 
          if ((block = dpp_mcds_get_free_entries_block_from_list(mcds, region->lists + (s - 1), do_change))) {
            goto found;
          }
        }
      }
    }
    
    loop_start = size_loop2_start;
    if (size_loop2_increase >= 0) { 
      loop_end = DPP_MCDS_MAX_FREE_BLOCK_SIZE + 1;
    } else { 
      loop_end = 0;
    }
    for (s = loop_start; s != loop_end; s += size_loop2_increase) {
      for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
        dpp_free_entries_blocks_region_t *region = regions[r];
        if (region->max_size >= s) { 
          if ((block = dpp_mcds_get_free_entries_block_from_list(mcds, region->lists + (s - 1), do_change))) {
            goto found;
          }
        }
      }
    }

  } else { 

    for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) { 
      dpp_free_entries_blocks_region_t *region = regions[r];
      
      dpp_free_entries_block_list_t *lists = region->lists - 1;

      
      loop_start = size_loop1_start;
      if (size_loop1_increase >= 0) { 
        loop_end = region->max_size + 1;
        if (loop_start > loop_end) loop_start = loop_end;
      } else { 
        loop_end = 0;
        if (loop_start > region->max_size) loop_start = region->max_size;
      }
      for (s = loop_start; s != loop_end; s += size_loop1_increase) {
       if ((block = dpp_mcds_get_free_entries_block_from_list(mcds, lists + s, do_change))) {
         goto found;
       }
      }
      
      loop_start = size_loop2_start;
      if (size_loop2_increase >= 0) { 
        loop_end = region->max_size + 1;
        if (loop_start > loop_end) loop_start = loop_end;
      } else { 
        loop_end = 0;
        if (loop_start > region->max_size) loop_start = region->max_size;
      }
      for (s = loop_start; s != loop_end; s += size_loop2_increase) {
       if ((block = dpp_mcds_get_free_entries_block_from_list(mcds, lists + s, do_change))) {
         goto found;
       }
      }

    } 

  } 

  DPP_MC_ASSERT(!mcds->nof_unoccupied);
  if (flags & DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL) {
    *block_start = 0;
    *block_size = 0;
    SOC_EXIT;
  }
  SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("did not find any free block")));

found:
  if (do_change && s > max_size) {
    DPP_MC_ASSERT(s <= DPP_MCDS_MAX_FREE_BLOCK_SIZE);
    SOCDNX_IF_ERR_EXIT( 
      dpp_mcds_split_free_entries_block(mcds, flags, regions[r], s, max_size, &block));
    s = max_size;
  }

  *block_start = block;
  *block_size = s;
  DPP_MC_ASSERT(block && s);

exit:
  SOCDNX_FUNC_RETURN;
}

STATIC uint32 
    dpp_mcds_is_egress_format_consecutive_next(
        int                      unit,
        uint8                    format,
        uint8                    *is_consecutive
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) {
        if (format == 3 || (format >= 12 && format <= 15)) {
            *is_consecutive = TRUE;
        } else if (format == 0 || format == 1 || format == 2 || format == 3 || (format >= 4 && format <= 7) || (format >= 8 && format <= 11)) {
            *is_consecutive = FALSE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("wrong egress format")));
        }

    } else {
         *is_consecutive = ARAD_MCDS_IS_EGRESS_FORMAT_CONSECUTIVE_NEXT(format);
    }
exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  dpp_mcds_get_relocation_block(
    SOC_SAND_IN  dpp_mcds_base_t             *mcds,
    SOC_SAND_IN  uint32                       mcdb_index,              
    SOC_SAND_OUT uint32                       *relocation_block_start, 
    SOC_SAND_OUT dpp_free_entries_block_size_t *relocation_block_size   
)
{
  int unit = mcds->unit;
  uint32 group_entry_type, start = mcdb_index;
  dpp_free_entries_block_size_t size = 1;
  uint8 is_consecutive;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mcds);
  SOCDNX_NULL_CHECK(relocation_block_size);

  group_entry_type = DPP_MCDS_GET_TYPE(mcds, mcdb_index);
  if (DPP_MCDS_TYPE_IS_USED(group_entry_type) &&  
    !DPP_MCDS_TYPE_IS_START(group_entry_type)) {
    if (DPP_MCDS_TYPE_IS_EGRESS_NORMAL(group_entry_type)) { 

      uint32 entry, next_entry;
      DPP_MC_ASSERT(group_entry_type == DPP_MCDS_TYPE_VALUE_EGRESS);
      for (entry = mcdb_index; ; entry = next_entry) { 
        next_entry = DPP_MCDS_GET_PREV_ENTRY(mcds, entry);
        SOCDNX_IF_ERR_EXIT(dpp_mcds_is_egress_format_consecutive_next(unit,DPP_MCDS_GET_EGRESS_FORMAT(mcds, next_entry), &is_consecutive));
        if (next_entry + 1 != entry || 
            !is_consecutive) {
          break;
        }
        DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_entry) == DPP_MCDS_TYPE_VALUE_EGRESS); 
        ++size;
        DPP_MC_ASSERT(next_entry && size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE);
      }
      start = entry;
      SOCDNX_IF_ERR_EXIT(dpp_mcds_is_egress_format_consecutive_next(unit,DPP_MCDS_GET_EGRESS_FORMAT(mcds, mcdb_index), &is_consecutive));
      for (entry = mcdb_index; is_consecutive;)
      { 
        ++size;
        ++entry;
        DPP_MC_ASSERT(entry <= DPP_LAST_MCDB_ENTRY(mcds) && size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE);
        DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, entry) == DPP_MCDS_TYPE_VALUE_EGRESS); 
        SOCDNX_IF_ERR_EXIT(dpp_mcds_is_egress_format_consecutive_next(unit,DPP_MCDS_GET_EGRESS_FORMAT(mcds, entry), &is_consecutive));
      }
      DPP_MC_ASSERT(entry - start + 1 == size);

    }
  } else { 
    DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_FREE(group_entry_type) || (mcdb_index < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids ? 
      group_entry_type == DPP_MCDS_TYPE_VALUE_INGRESS_START : (DPP_MCDS_TYPE_IS_EGRESS_START(group_entry_type) &&
      mcdb_index < DPP_MCDS_GET_EGRESS_GROUP_START(mcds, 0, SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores))));
  }

  *relocation_block_size = size;
  if (relocation_block_start) {
    *relocation_block_start = start;
  }

exit:
  SOCDNX_FUNC_RETURN;
}





uint32
  dpp_mcds_write_entry(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mcdb_index 
)
{
  uint32 data[DPP_MC_ENTRY_SIZE];
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *entry = MCDS_GET_MCDB_ENTRY(mcds, mcdb_index);

  SOCDNX_INIT_FUNC_DEFS;

  data[0] = entry->word0;
  data[1] = entry->word1 & mcds->msb_word_mask;
  SOCDNX_IF_ERR_EXIT(WRITE_IRR_MCDBm(unit, MEM_BLOCK_ANY, mcdb_index, data));

exit:
  SOCDNX_FUNC_RETURN;
}

int _dpp_mcds_test_free_entries(
    SOC_SAND_IN int unit
)
{
  uint32 nof_unoccupied = 0;
  dpp_mcds_base_t* mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *entry, *entry2;
  ARAD_MULT_ID mcid;


  
  for (mcid = 0; mcid <= DPP_LAST_MCDB_ENTRY(mcds); ++mcid) {
    entry = MCDS_GET_MCDB_ENTRY(mcds, mcid);
    if (DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_ENTRY_GET_TYPE(entry))) {
      DPP_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
      ++nof_unoccupied;
    } else {
      DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
    }
  }
  
  if (DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_ENTRY_GET_TYPE(entry =
      MCDS_GET_MCDB_ENTRY(mcds, MCDS_INGRESS_LINK_END(mcds))))) {
    DPP_MC_ASSERT(nof_unoccupied);
    DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
    --nof_unoccupied;
  }
  if (DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_ENTRY_GET_TYPE(entry =
      MCDS_GET_MCDB_ENTRY(mcds, DPP_MC_EGRESS_LINK_PTR_END)))) {
    DPP_MC_ASSERT(nof_unoccupied);
    DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
    --nof_unoccupied;
  }
  if (nof_unoccupied != mcds->nof_unoccupied) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST,
             (BSL_META_U(unit,
                         "The mcdb has %lu free allocatable entries and in the mcds the value is %lu\n"), (unsigned long)nof_unoccupied, (unsigned long)mcds->nof_unoccupied));
    DPP_MC_ASSERT(0);
    return 10;
  }

  
  nof_unoccupied = 0;
  {
      dpp_free_entries_blocks_region_t *regions[ARAD_MCDS_NOF_REGIONS];
    int r;
    dpp_free_entries_block_size_t size, size_i;
    uint32 block, first_block, prev_block;
    regions[0] = &mcds->free_general;
    regions[1] = &mcds->ingress_alloc_free;
    regions[2] = &mcds->egress_alloc_free;

    
    for (r = 0; r < ARAD_MCDS_NOF_REGIONS; ++r) {
      dpp_free_entries_blocks_region_t *region = regions[r];
      dpp_free_entries_block_list_t *lists = region->lists;
      DPP_MC_ASSERT(region->max_size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE && region->max_size > 0);

      for (size = region->max_size; size; --size) { 
        
        if ((block = dpp_mcds_get_free_entries_block_from_list(mcds, lists + size - 1, 0))) { 
          prev_block = DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, block);
          first_block = block;

          
          do {
            entry = MCDS_GET_MCDB_ENTRY(mcds, block);
            DPP_MC_ASSERT(block >= region->range_start && block + size - 1 <= region->range_end);
            DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(entry) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK_START);
            DPP_MC_ASSERT(DPP_MCDS_GET_FREE_BLOCK_SIZE(mcds, block) == size);
            DPP_MC_ASSERT(prev_block == DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, block));
            if (!DPP_MCDS_ENTRY_GET_TEST_BIT(entry)) {
              LOG_ERROR(BSL_LS_SOC_MULTICAST,
                       (BSL_META_U(unit,
                                   "Free block %lu of size %u appeared previously in a linked list\n"), (unsigned long)block, size));
              DPP_MC_ASSERT(0);
              return 20;
            }
            DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
            entry2 = entry;

            for (size_i = 1; size_i < size;  ++ size_i) { 
              ++entry2;
              DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(entry2) == DPP_MCDS_TYPE_VALUE_FREE_BLOCK);
              DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_FREE_PREV_ENTRY(entry2) == block);
              if (!DPP_MCDS_ENTRY_GET_TEST_BIT(entry2)) {
                LOG_ERROR(BSL_LS_SOC_MULTICAST,
                         (BSL_META_U(unit,
                                     "Free entry %lu of free block %lu of size %u appeared previously in a linked list\n"),
                                     (unsigned long)(block + size ), (unsigned long)block, size));
                DPP_MC_ASSERT(0);
                return 30;
              }
            DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry2);
            }
            nof_unoccupied += size;
            prev_block = block;
            block = DPP_MCDS_GET_FREE_NEXT_ENTRY(mcds, block); 
          } while (block != first_block);
          DPP_MC_ASSERT(prev_block == DPP_MCDS_GET_FREE_PREV_ENTRY(mcds, block));
        }
      }
    }
  }
  if (nof_unoccupied != mcds->nof_unoccupied) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST,
             (BSL_META_U(unit,
                         "The mcdb free block lists contain %lu entries and in the mcds the value is %lu\n"), (unsigned long)nof_unoccupied, (unsigned long)mcds->nof_unoccupied));
    DPP_MC_ASSERT(0);
    return 40;
  }

  return 0;
}


uint32
    dpp_mcds_multicast_terminate(
        SOC_SAND_IN int unit
    )
{
    dpp_mcds_base_t* mcds = dpp_get_mcds(unit);
    SOCDNX_INIT_FUNC_DEFS;
 
    if (mcds != NULL) {
        SOCDNX_IF_ERR_EXIT(sand_free_mem(unit, (void**)&mcds->mcdb));
        SOCDNX_IF_ERR_EXIT(sand_free_mem(unit, (void**)&mcds->prev_entries));
    }

    SOCDNX_IF_ERR_EXIT(dpp_deinit_mcds(unit));

    SOC_EXIT;
exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_get_mcdb_entry_type(
    SOC_SAND_IN  dpp_mcdb_entry_t* entry
)
{
    return DPP_MCDS_ENTRY_GET_TYPE((arad_mcdb_entry_t*)entry);
}

void dpp_set_mcdb_entry_type(
    SOC_SAND_INOUT  dpp_mcdb_entry_t* entry,
    SOC_SAND_IN     uint32 type_value
)
{
    arad_mcdb_entry_t *e = (arad_mcdb_entry_t*)entry;
    DPP_MCDS_ENTRY_SET_TYPE(e, type_value);
}





void arad_mult_egr_write_entry_port_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud1, cud2;
  dpp_mc_local_port_t port1, port2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
      port1 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
      port1 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
      port2 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep2);
      if (!rep1) {
          cud1 = cud2;
      } else {
          DPP_MC_ASSERT(cud1 == cud2);
      }
  } else {
      port2 = ARAD_MULT_EGRESS_SMALL_PORT_INVALID;
  }

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Af, port1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf, port2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, ENTRY_FORMATf, 0);
}


void arad_mult_egr_write_entry_port_cud_noptr(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint8             use_next     
                                                  
)
{
  dpp_mc_outlif_t cud1, cud2;
  dpp_mc_local_port_t port1, port2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
      port1 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
      port1 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
      port2 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
      port2 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f, port1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f, port2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, ENTRY_FORMATf, use_next ? 5 : 4);
}


void arad_mult_egr_write_entry_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud1, cud2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
  }

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, ENTRY_FORMATf, 1);
}


void arad_mult_egr_write_entry_cud_noptr(
    SOC_SAND_IN     int               unit,
    SOC_SAND_INOUT  arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN     dpp_rep_data_t    *rep1,       
    SOC_SAND_IN     dpp_rep_data_t    *rep2,       
    SOC_SAND_IN     dpp_rep_data_t    *rep3,       
    SOC_SAND_IN     uint8             use_next     
                                                   
)
{
  dpp_mc_outlif_t cud1, cud2, cud3;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep3) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep3) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud3 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep3);
  } else {
      cud3 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f, cud3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, ENTRY_FORMATf, use_next ? 7 : 6);
}


void arad_mult_egr_write_entry_bm_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep,        
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud;
  dpp_mc_bitmap_id_t bm_id;
  if (rep) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_BM_CUD);
      cud = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep);
      bm_id = DPP_MCDS_REP_DATA_GET_EGR_BM_ID(rep);
  } else {
      cud = DPP_MC_EGR_OUTLIF_DISABLED;
      bm_id = DPP_MC_EGR_BITMAP_DISABLED;
  }
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f, cud);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, BMP_PTRf, bm_id);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf, 1);
}






void jer_mult_egr_write_entry_port_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud1, cud2;
  dpp_mc_local_port_t port1, port2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
      port1 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
      port1 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
      port2 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep2);
      if (!rep1) {
          cud1 = cud2;
      } else {
          DPP_MC_ASSERT(cud1 == cud2);
      }
  } else {
      port2 = DPP_MULT_EGRESS_PORT_INVALID;
  }

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Af, port1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Bf, port2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf, 0);
}


void jer_mult_egr_write_entry_port_cud_noptr(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint8             use_next     
                                                  
)
{
  dpp_mc_outlif_t cud1, cud2;
  dpp_mc_local_port_t port1, port2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
      port1 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
      port1 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
      port2 = DPP_MCDS_REP_DATA_GET_EGR_PORT(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
      port2 = DPP_MULT_EGRESS_PORT_INVALID;
  }
  
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f, port1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f, port2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, ENTRY_FORMATf, use_next ? 3 : 2);
}


void jer_mult_egr_write_entry_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep1,       
    SOC_SAND_IN    dpp_rep_data_t    *rep2,       
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud1, cud2;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
  }

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, ENTRY_FORMATf, 1);
}


void jer_mult_egr_write_entry_cud_noptr(
    SOC_SAND_IN     int               unit,
    SOC_SAND_INOUT  arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN     dpp_rep_data_t    *rep1,       
    SOC_SAND_IN     dpp_rep_data_t    *rep2,       
    SOC_SAND_IN     dpp_rep_data_t    *rep3,       
    SOC_SAND_IN     uint8             use_next     
                                                   
)
{
  dpp_mc_outlif_t cud1, cud2, cud3;
  if (rep1) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep1) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud1 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep1);
  } else {
      cud1 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep2) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep2) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud2 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep2);
  } else {
      cud2 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  if (rep3) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep3) == DPP_MCDS_REP_TYPE_EGR_CUD);
      cud3 = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep3);
  } else {
      cud3 = DPP_MC_EGR_OUTLIF_DISABLED;
  }
  
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f, cud1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f, cud2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f, cud3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, ENTRY_FORMATf, use_next ? 3 : 2);
}


void jer_mult_egr_write_entry_bm_cud(
    SOC_SAND_IN    int               unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *mcdb_entry, 
    SOC_SAND_IN    dpp_rep_data_t    *rep,        
    SOC_SAND_IN    uint32            next_entry   
)
{
  dpp_mc_outlif_t cud;
  dpp_mc_bitmap_id_t bm_id;
  if (rep) {
      DPP_MC_ASSERT(DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_BM_CUD);
      cud = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep);
      bm_id = DPP_MCDS_REP_DATA_GET_EGR_BM_ID(rep);
  } else {
      cud = DPP_MC_EGR_OUTLIF_DISABLED;
      bm_id = DPP_MC_EGR_BITMAP_DISABLED;
  }
  mcdb_entry->word0 = 0; 
  mcdb_entry->word1 &= ~JER_MC_ENTRY_MASK_VAL;
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f, cud);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, BMP_PTRf, bm_id);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, LINK_PTRf, next_entry);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, ENTRY_FORMATf, 1);
}



uint32 dpp_init_mcds(
    SOC_SAND_IN    int         unit
)
{
    dpp_mcds_base_t *dpp_base;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_alloc_mcds(unit, sizeof(*dpp_base), (void*)&dpp_base));

    dpp_base->unit = unit;
    dpp_base->common.flags = 0;
    dpp_base->nof_egr_ll_groups = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - (SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1); 
    dpp_base->common.get_mcdb_entry_type = dpp_get_mcdb_entry_type;
    dpp_base->common.set_mcdb_entry_type = dpp_set_mcdb_entry_type;
    dpp_base->set_egress_linked_list = dpp_mcds_set_egress_linked_list;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
        dpp_base->common.get_mcdb_entry_from_mcds = dpp_mcds_get_mcdb_entry_from_mcds;
        dpp_base->common.get_next_pointer = arad_mcdb_get_next_pointer;
        dpp_base->common.set_next_pointer = arad_mcdb_set_next_pointer;

        dpp_base->common.ingress_link_end = ARAD_MC_INGRESS_LINK_PTR_END;
        dpp_base->free_value[0] = ARAD_MC_UNOCCUPIED_ENTRY_LOW;
        dpp_base->free_value[1] = ARAD_MC_UNOCCUPIED_ENTRY_HIGH;
        dpp_base->empty_ingr_value[0] = ARAD_MC_ING_EMPTY_ENTRY_LOW;
        dpp_base->empty_ingr_value[1] = ARAD_MC_ING_EMPTY_ENTRY_HIGH;
        dpp_base->msb_word_mask = ARAD_MC_ENTRY_MASK_VAL;
        dpp_base->ingr_word1_replication_mask = 3;
        dpp_base->max_egr_cud_field = dpp_base->max_ingr_cud_field = ((1 << 16) - 1);
        
        dpp_base->egress_mcdb_offset = ARAD_MULT_NOF_MULTICAST_GROUPS;

        dpp_base->egr_mc_write_entry_port_cud = arad_mult_egr_write_entry_port_cud;
        dpp_base->egr_mc_write_entry_port_cud_noptr = arad_mult_egr_write_entry_port_cud_noptr;
        dpp_base->egr_mc_write_entry_cud = arad_mult_egr_write_entry_cud;
        dpp_base->egr_mc_write_entry_cud_noptr = arad_mult_egr_write_entry_cud_noptr;
        dpp_base->egr_mc_write_entry_bm_cud = arad_mult_egr_write_entry_bm_cud;
        dpp_base->get_replications_from_entry = arad_mcds_get_replications_from_entry;
        dpp_base->convert_ingress_replication_hw2api = arad_convert_ingress_replication_hw2api;

        switch (SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode) {
          case ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE:
            dpp_base->max_nof_ingr_replications = 4096 - DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS;
            break;
          case ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE:
            dpp_base->max_nof_ingr_replications = 64 - DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS;
            break;
          default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid buffer mode")));
        }
        dpp_base->max_nof_mmc_replications = 1; 

#ifdef BCM_88675_A0
    } else { 
        dpp_base->common.get_mcdb_entry_from_mcds = dpp_mcds_get_mcdb_entry_from_mcds;
        dpp_base->common.get_next_pointer = jer_mcdb_get_next_pointer;
        dpp_base->common.set_next_pointer = jer_mcdb_set_next_pointer;

        dpp_base->common.ingress_link_end = JER_MC_INGRESS_LINK_PTR_END;
        dpp_base->free_value[0] = JER_MC_UNOCCUPIED_ENTRY_LOW;
        dpp_base->free_value[1] = JER_MC_UNOCCUPIED_ENTRY_HIGH;
        dpp_base->empty_ingr_value[0] = JER_MC_ING_EMPTY_ENTRY_LOW;
        dpp_base->empty_ingr_value[1] = JER_MC_ING_EMPTY_ENTRY_HIGH;
        dpp_base->msb_word_mask = JER_MC_ENTRY_MASK_VAL;
        dpp_base->ingr_word1_replication_mask = 0x1f;
        dpp_base->max_ingr_cud_field = ((1 << 19) - 1);
        dpp_base->max_egr_cud_field = ((1 << 18) - 1);
        
        dpp_base->egress_mcdb_offset = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids -
          (SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high + 1);

        dpp_base->egr_mc_write_entry_port_cud = jer_mult_egr_write_entry_port_cud;
        dpp_base->egr_mc_write_entry_port_cud_noptr = jer_mult_egr_write_entry_port_cud_noptr;
        dpp_base->egr_mc_write_entry_cud = jer_mult_egr_write_entry_cud;
        dpp_base->egr_mc_write_entry_cud_noptr = jer_mult_egr_write_entry_cud_noptr;
        dpp_base->egr_mc_write_entry_bm_cud = jer_mult_egr_write_entry_bm_cud;
        dpp_base->get_replications_from_entry = jer_mcds_get_replications_from_entry;
        dpp_base->convert_ingress_replication_hw2api = jer_convert_ingress_replication_hw2api;

        switch (SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode) {
          case JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE:
            dpp_base->max_nof_ingr_replications = 64 - DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS;
            break;
          default: 
            dpp_base->max_nof_ingr_replications = 4096 - DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS;
        }
        dpp_base->max_nof_mmc_replications = 8 - DPP_INGR_MC_NOF_RESERVED_BUFFER_REPLICATIONS;
#endif 
    }
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_deinit_mcds(
    SOC_SAND_IN    int         unit
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_dealloc_mcds(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

