/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_sw_db.h>


#include <soc/mcm/memregs.h>




#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <soc/dpp/port_sw_db.h>







#define SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_MAX          (8*1024)


#define DPP_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE 9
#define DPP_FDA_MC_ENTRY_SIZE 3
#define FDT_IPT_MESH_MC_ENTRY_SIZE 5
#define EGR_PER_CORE_REP_MAX_ENTRY_SIZE FDT_IPT_MESH_MC_ENTRY_SIZE

#define DPP_FDA_BITS_PER_GROUP 2
#define DPP_FDA_GROUPS_PER_ENTRY 32
#define DPP_FDA_OFFSET_IN_GROUP_BITS 0
#define FDT_IPT_MESH_MC_BITS_PER_GROUP 4
#define FDT_IPT_MESH_MC_GROUPS_PER_ENTRY 32
#define FDT_IPT_MESH_MC_OFFSET_IN_GROUP_BITS 2
#define CORES_BITS_MASK 3


#define JER_CUD2PORT_MAPPING_PORTS_PER_ENTRY 4
#define JER_CUD2PORT_MAPPING_NOF_PORT_BITS 8
#define JER_CUD2PORT_MAPPING_PORT_MASK ((1 << JER_CUD2PORT_MAPPING_NOF_PORT_BITS) - 1)























STATIC uint32 dpp_egress_group_get_core_replication(
    SOC_SAND_IN  int                   unit,       
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_OUT dpp_mc_core_bitmap_t  *out_cores) 
{
    int index, offset; 
    uint32 *entry_word; 
    uint32 data[EGR_PER_CORE_REP_MAX_ENTRY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(group_mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids && out_cores);
    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & (DPP_MC_EGR_CORE_FDA_MODE | DPP_MC_EGR_CORE_MESH_MODE)) {
        
        if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_CORE_FDA_MODE) {
            index = group_mcid / DPP_FDA_GROUPS_PER_ENTRY;
            offset = DPP_FDA_BITS_PER_GROUP * (group_mcid % DPP_FDA_GROUPS_PER_ENTRY);
            entry_word = data + offset / SOC_SAND_NOF_BITS_IN_UINT32;
            offset = offset % SOC_SAND_NOF_BITS_IN_UINT32;
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MCm(unit, MEM_BLOCK_ANY, index, data));
        } else {
            index = group_mcid / FDT_IPT_MESH_MC_GROUPS_PER_ENTRY;
            offset = FDT_IPT_MESH_MC_BITS_PER_GROUP * (group_mcid % FDT_IPT_MESH_MC_GROUPS_PER_ENTRY) + FDT_IPT_MESH_MC_OFFSET_IN_GROUP_BITS;
            entry_word = data + offset / SOC_SAND_NOF_BITS_IN_UINT32;
            offset = offset % SOC_SAND_NOF_BITS_IN_UINT32;
            SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, data));
        }
        *out_cores = (*entry_word >> offset) & CORES_BITS_MASK;
    } else { 
        *out_cores = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
    }
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32 dpp_egress_group_set_core_replication(
    SOC_SAND_IN  int                   unit,       
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_IN  dpp_mc_core_bitmap_t  cores)      
{
    int index, offset; 
    uint32 *word_to_change, orig_word; 
    uint32 i;
    uint32 groups_per_entry, bits_per_group, offset_in_group_bits;
    uint32 nof_full_entries, groups_remainder, uint32s_per_entry;
    soc_mem_t table;
    int mode;
    const static uint32 mode_bit[2] = {DPP_MC_EGR_CORE_FDA_MODE, DPP_MC_EGR_CORE_MESH_MODE};
    uint32 data[EGR_PER_CORE_REP_MAX_ENTRY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;

    DPP_MC_ASSERT(group_mcid <= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids && cores <= DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit));
    for (mode = 0; mode < 2; ++mode) { 
        if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & mode_bit[mode]) == 0) continue;

        
        if (mode_bit[mode] == DPP_MC_EGR_CORE_FDA_MODE) {
            groups_per_entry = DPP_FDA_GROUPS_PER_ENTRY;
            bits_per_group = DPP_FDA_BITS_PER_GROUP;
            offset_in_group_bits = DPP_FDA_OFFSET_IN_GROUP_BITS;
            table = FDA_FDA_MCm;
        } else {
            groups_per_entry = FDT_IPT_MESH_MC_GROUPS_PER_ENTRY;
            bits_per_group = FDT_IPT_MESH_MC_BITS_PER_GROUP;
            offset_in_group_bits = FDT_IPT_MESH_MC_OFFSET_IN_GROUP_BITS;
            table = FDT_IPT_MESH_MCm;
        }

        if (group_mcid == SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) { 
            if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids == 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("no egress MC groups")));
            }

            nof_full_entries = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids / groups_per_entry;
            groups_remainder = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids % groups_per_entry;
            uint32s_per_entry = groups_per_entry * bits_per_group / SOC_SAND_NOF_BITS_IN_UINT32; 

            data[0] = ((uint32)cores) << offset_in_group_bits; 
            for (i = DPP_FDA_BITS_PER_GROUP; i < SOC_SAND_NOF_BITS_IN_UINT32; i *= 2) { 
                data[0] |= data[0] << i;
            }
            for (i = 1; i < uint32s_per_entry; ++i) { 
                data[i] = data[0];
            }
            data[i] = 0; 

            if (nof_full_entries > 0) {
                
                SOCDNX_SAND_IF_ERR_EXIT(arad_fill_partial_table_with_entry(unit, table, 0, 0, MEM_BLOCK_ANY, 0, nof_full_entries - 1, data));
            }
            if (groups_remainder > 0) {
                groups_remainder *= bits_per_group; 
                i = groups_remainder / SOC_SAND_NOF_BITS_IN_UINT32; 
                groups_remainder = groups_remainder % SOC_SAND_NOF_BITS_IN_UINT32; 
    
                data[i] &= ((((uint32)1) << groups_remainder) - 1);
                for (++i; i < uint32s_per_entry; ++i) { 
                    data[i] = 0;
                }
                SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, table, MEM_BLOCK_ANY, nof_full_entries, data));
            }

        } else { 
            index = group_mcid / groups_per_entry;
            offset = bits_per_group * (group_mcid % groups_per_entry) + offset_in_group_bits;
            word_to_change = data + offset / SOC_SAND_NOF_BITS_IN_UINT32;
            offset = offset % SOC_SAND_NOF_BITS_IN_UINT32;

            SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, table, MEM_BLOCK_ANY, index, data));
            orig_word = *word_to_change;
            *word_to_change &= ~(((uint32)CORES_BITS_MASK) << offset);
            *word_to_change |= ((uint32)cores) << offset;
            if (*word_to_change != orig_word) { 
                SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, table, MEM_BLOCK_ALL, index, data));
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_egress_group_open_set(
    SOC_SAND_IN  int     unit, 
    SOC_SAND_IN  uint32  group_id,  
    SOC_SAND_IN  uint8   is_open    
)
{
    uint8 bit_val;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, group_id, &bit_val);
    SOCDNX_IF_ERR_EXIT(rv);
    bit_val = bit_val ? 1: 0;
    if (bit_val != is_open) {
        if(is_open) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_set(unit, group_id);
        } else {
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_clear(unit, group_id);
        }
        SOCDNX_IF_ERR_EXIT(rv);
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int dpp_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id, 
    SOC_SAND_OUT uint8 *is_open
)
{
    uint8 bit_val;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS

    DPP_MC_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_multicast.egress_groups_open_data.bit_get(unit, group_id, &bit_val);
    SOCDNX_IF_ERR_EXIT(rv);
    *is_open = bit_val ? 1: 0;
    
exit:
    SOCDNX_FUNC_RETURN
}



uint32 dpp_mult_cud_to_port_map_set(
    SOC_SAND_IN int                 unit, 
    SOC_SAND_IN uint32              flags,
    SOC_SAND_IN uint32              cud,  
    SOC_SAND_IN SOC_TMC_FAP_PORT_ID port  
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 entry[2] = {0}, shift;
    uint32 tm_port, previous_core;
    int core;
    int index;

    SOCDNX_INIT_FUNC_DEFS;
    if (cud > mcds->max_egr_cud_field) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD too big")));
    }

    if (SOC_IS_JERICHO(unit)) {
        index = cud / JER_CUD2PORT_MAPPING_PORTS_PER_ENTRY;
        shift = JER_CUD2PORT_MAPPING_NOF_PORT_BITS * (cud % JER_CUD2PORT_MAPPING_PORTS_PER_ENTRY);
        if (port == _SHR_GPORT_INVALID) {
            tm_port = DPP_MULT_EGRESS_PORT_INVALID;
            DPP_CUD2CORE_SET_CORE(unit, cud, DPP_CUD2CORE_UNDEF_VALUE);
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
            DPP_CUD2CORE_GET_CORE(unit, cud, previous_core); 
            if (core != previous_core) {
                if (previous_core != DPP_CUD2CORE_UNDEF_VALUE && !flags) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Mapping to a port in a different core than the current mapped port is not allowed without BCM_PORT_ENCAP_MAP_ALLOW_CORE_CHANGE")));
                }
                DPP_CUD2CORE_SET_CORE(unit, cud, core);
            }
        }
        SOCDNX_IF_ERR_EXIT(READ_EDB_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, index, entry));
        entry[0] &= ~((uint32)JER_CUD2PORT_MAPPING_PORT_MASK << shift);
        entry[0] |= tm_port << shift;
        SOCDNX_IF_ERR_EXIT(WRITE_EDB_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, index, entry));
    } else {
        if (port >= DPP_MULT_EGRESS_PORT_INVALID) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid Port")));
        }
        soc_mem_field32_set(unit, EGQ_MAP_OUTLIF_TO_DSPm, entry, DSPf, port); 
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, cud, entry));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_cud_to_port_map_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  uint32              cud,  
    SOC_SAND_OUT SOC_TMC_FAP_PORT_ID *port 
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 entry[2];
    uint32 tm_port;
    soc_port_t logical_port;
    int core;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(port);
    if (cud > mcds->max_egr_cud_field) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD too big")));
    }

    if (SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_EDB_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY,
          cud / JER_CUD2PORT_MAPPING_PORTS_PER_ENTRY, entry));
        tm_port = (entry[0] >> (JER_CUD2PORT_MAPPING_NOF_PORT_BITS * (cud % JER_CUD2PORT_MAPPING_PORTS_PER_ENTRY))) &
          JER_CUD2PORT_MAPPING_PORT_MASK;
        DPP_CUD2CORE_GET_CORE(unit, cud, core); 
        if (core == DPP_CUD2CORE_UNDEF_VALUE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD isn't mapped to port")));
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &logical_port)); 
        *port = logical_port;
    } else {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_OUTLIF_TO_DSPm(unit, MEM_BLOCK_ANY, cud, entry));
        *port = soc_mem_field32_get(unit, EGQ_MAP_OUTLIF_TO_DSPm, entry, DSPf);
    }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_get_group(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    group_mcid,           
    SOC_SAND_IN  uint32         mc_group_size,        
    SOC_SAND_OUT soc_gport_t    *ports,               
    SOC_SAND_OUT soc_if_t       *cuds,                
    SOC_SAND_OUT soc_multicast_replication_t *reps,   
    SOC_SAND_OUT uint32         *exact_mc_group_size, 
    SOC_SAND_OUT uint8          *is_open              
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(exact_mc_group_size);
    SOCDNX_NULL_CHECK(is_open);
    if (mc_group_size && !reps && (!ports || !cuds)) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("NULL pointer")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, group_mcid, TRUE, is_open)) ; 

    if (*is_open) {
        uint16 group_size;
        SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group(
            unit, DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit), TRUE, FALSE, group_mcid, DPP_MCDS_TYPE_VALUE_EGRESS, mc_group_size, &group_size));
        *exact_mc_group_size = group_size;
        SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_to_arrays(unit, 1, mc_group_size,  ports, cuds, reps));
    } else { 
        *exact_mc_group_size = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_egq_vlan_table_tbl_get(
    SOC_SAND_IN   int    unit,
    SOC_SAND_IN   uint32 entry_offset,
    SOC_SAND_OUT  uint32 *vlan_table_tbl_data
)
{
    int core, nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32 *bitmap_ptr = vlan_table_tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    for (core = 0; core < nof_active_cores; ++core) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, core), entry_offset, bitmap_ptr));
        bitmap_ptr += SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_egq_vlan_table_tbl_set(
    SOC_SAND_IN   int     unit,
    SOC_SAND_IN   uint32  entry_offset,
    SOC_SAND_IN   uint32* vlan_table_tbl_data
)
{
    int core, i, nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dpp_mc_core_bitmap_t non_zero_cores = DPP_MC_CORE_BITAMAP_NO_CORES; 
    dpp_mc_core_bitmap_t prev_cores_with_reps, new_cores_with_reps; 
    const uint32 *bitmap_ptr = vlan_table_tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    for (core = 0; core < nof_active_cores; ++core) { 
        for (i = 0; i < SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32; ++i) {
            if (bitmap_ptr[i]) {
                non_zero_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
                break;
            }
        }
        bitmap_ptr += SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    }

    SOCDNX_IF_ERR_EXIT(dpp_egress_group_get_core_replication(unit, entry_offset, &prev_cores_with_reps)); 
    new_cores_with_reps = non_zero_cores & prev_cores_with_reps; 
    if (new_cores_with_reps != prev_cores_with_reps) { 
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, entry_offset, new_cores_with_reps));
        prev_cores_with_reps = new_cores_with_reps;
    }

    
    bitmap_ptr = vlan_table_tbl_data;
    for (core = 0; core < nof_active_cores; ++core) {
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, core), entry_offset, (uint32*)bitmap_ptr));
        bitmap_ptr += SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32;
    }

    if (non_zero_cores != prev_cores_with_reps) { 
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, entry_offset, non_zero_cores));
    }
exit:
    SOCDNX_FUNC_RETURN;
}




uint32 dpp_mult_eg_bitmap_group_get(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  dpp_mc_id_t                           bitmap_id, 
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group     
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, bitmap_id, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}





uint32 dpp_mult_eg_write_entry_port_cud(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  dpp_mc_id_t    multicast_id_ndx, 
    SOC_SAND_IN  dpp_rep_data_t *rep1,            
    SOC_SAND_IN  dpp_rep_data_t *rep2,            
    SOC_SAND_IN  uint32         next_entry,       
    SOC_SAND_IN  uint32         prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_port_cud(unit, mcdb_entry, rep1, rep2, next_entry); 
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_write_entry_port_cud_noptr(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, 
    SOC_SAND_IN dpp_rep_data_t *rep1,            
    SOC_SAND_IN dpp_rep_data_t *rep2,            
    SOC_SAND_IN uint8          use_next,         
                                                 
    SOC_SAND_IN uint32         prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_port_cud_noptr(unit, mcdb_entry, rep1, rep2, use_next); 
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_write_entry_cud(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, 
    SOC_SAND_IN dpp_rep_data_t *rep1,            
    SOC_SAND_IN dpp_rep_data_t *rep2,            
    SOC_SAND_IN uint32         next_entry,       
    SOC_SAND_IN uint32         prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_cud(unit, mcdb_entry, rep1, rep2, next_entry); 
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_write_entry_cud_noptr(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, 
    SOC_SAND_IN dpp_rep_data_t *rep1,            
    SOC_SAND_IN dpp_rep_data_t *rep2,            
    SOC_SAND_IN dpp_rep_data_t *rep3,            
    SOC_SAND_IN uint8          use_next,         
                                                 
    SOC_SAND_IN uint32         prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_cud_noptr(unit, mcdb_entry, rep1, rep2, rep3, use_next); 
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_write_entry_bm_cud(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN dpp_mc_id_t    multicast_id_ndx, 
    SOC_SAND_IN dpp_rep_data_t *rep,             
    SOC_SAND_IN uint32         next_entry,       
    SOC_SAND_IN uint32         prev_entry        
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  arad_mcdb_entry_t *mcdb_entry = dpp_mcds_get_mcdb_entry(unit, multicast_id_ndx);
  SOCDNX_INIT_FUNC_DEFS;

  mcds->egr_mc_write_entry_bm_cud(unit, mcdb_entry, rep, next_entry); 
  SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, multicast_id_ndx)); 

  DPP_MCDS_SET_PREV_ENTRY(mcds, multicast_id_ndx, prev_entry); 
  DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, prev_entry == multicast_id_ndx ? DPP_MCDS_TYPE_VALUE_EGRESS_START : DPP_MCDS_TYPE_VALUE_EGRESS);

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC uint32 dpp_mult_eg_add_group_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  int                           core,               
    SOC_SAND_IN  uint32                        block_start,        
    SOC_SAND_IN  dpp_free_entries_block_size_t block_size,         
    SOC_SAND_IN  dpp_free_entries_block_size_t last_entry_pointer, 
    SOC_SAND_IN  dpp_free_entries_block_size_t port_entries,       
    SOC_SAND_IN  dpp_free_entries_block_size_t outlif_entries,     
    SOC_SAND_IN  dpp_free_entries_block_size_t port_replications,  
    SOC_SAND_IN  dpp_free_entries_block_size_t outlif_replications,
    SOC_SAND_INOUT dpp_rep_data_t              **port_outlif_reps, 
    SOC_SAND_INOUT dpp_rep_data_t              **cud_reps          
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    dpp_rep_data_t *rep, *rep2;
    uint32 last_entry = block_start + block_size + last_entry_pointer - 1; 
    uint32 index = block_start, prev = 0; 
    dpp_free_entries_block_size_t left;
    SOCDNX_INIT_FUNC_DEFS;

    DPP_MC_ASSERT(
      mcds->nof_egr_port_outlif_reps[core] >= port_entries * 2 &&
      mcds->nof_egr_outlif_reps[core] >= outlif_entries * 3 &&
      block_size > 0 && port_replications <= 1 &&
      outlif_replications <= 2 && last_entry_pointer <= 1 &&
      port_entries + outlif_entries + port_replications + (outlif_replications ? 1 : 0) == block_size);
    DPP_MC_ASSERT(port_replications == 0 && outlif_replications == 0); 
    for (left = port_entries; left; --left, ++index) { 
        rep = (*port_outlif_reps)++;
        SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud_noptr(
          unit, index, (*port_outlif_reps)++, rep, last_entry - index, prev)); 
        prev = index;
    }
    for (left = outlif_entries; left; --left, ++index) { 
        rep = (*cud_reps)++;
        rep2 = (*cud_reps)++;
        SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_cud_noptr( unit, index, rep, rep2, (*cud_reps)++, last_entry - index, prev)); 
        prev = index;
    }
    DPP_MC_ASSERT(index == block_start + block_size); 

exit:
    SOCDNX_FUNC_RETURN;
}


static INLINE uint32 dpp_mult_eg_write_cud2_entry_struct(
    SOC_SAND_IN    int               unit,
    SOC_SAND_IN    uint8             cud2_type,   
    SOC_SAND_INOUT arad_mcdb_entry_t *cud2_entry, 
    SOC_SAND_IN    uint32            cud2,        
    SOC_SAND_IN    uint32            next_entry   
) {
    SOCDNX_INIT_FUNC_DEFS;
    cud2_entry->word0 = cud2_type == DPP_MC_GROUP_2ND_CUD_OUTRIF ? 0x80000 : 0x40000;
    cud2_entry->word1 = 0x100001 ;
    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, cud2_entry, OUTLIF_1f, cud2); 
    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, cud2_entry, LINK_PTRf, next_entry);

    SOC_EXIT;
exit:
  SOCDNX_FUNC_RETURN;
}


STATIC uint32 dpp_mult_eg_write_pointer_entry(
    SOC_SAND_IN    int unit,
    SOC_SAND_IN    uint32 index,                    
    SOC_SAND_IN    uint32 prev_entry,               
    SOC_SAND_IN    uint32 next_entry,               
    SOC_SAND_INOUT uint16 *nof_port_couples,        
    SOC_SAND_INOUT uint16 *nof_remaining_ports,     
    SOC_SAND_INOUT uint16 *nof_outlif_replications, 
    SOC_SAND_INOUT uint16 *nof_bitmap_replications, 
    SOC_SAND_INOUT dpp_rep_data_t **couples_reps,   
    SOC_SAND_INOUT dpp_rep_data_t **port_cud_reps,  
    SOC_SAND_INOUT dpp_rep_data_t **cud_only_reps,  
    SOC_SAND_INOUT dpp_rep_data_t **bitmap_reps     
)
{
    dpp_rep_data_t *rep;
    SOCDNX_INIT_FUNC_DEFS;

  
    if (*nof_port_couples) { 
        --*nof_port_couples;
        rep = (*couples_reps)++;
        SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index, (*couples_reps)++, rep, next_entry, prev_entry));

    } else if (*nof_bitmap_replications) { 

         --*nof_bitmap_replications;
        SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_bm_cud(unit, index, (*bitmap_reps)++, next_entry, prev_entry));

    } else if (*nof_outlif_replications) { 

        rep = (*cud_only_reps)++;
        if (*nof_outlif_replications >= 2) {
            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_cud(unit, index, rep,(*cud_only_reps)++, next_entry, prev_entry));
            (*nof_outlif_replications) -= 2;
        } else {
            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_cud(unit, index, rep, 0, next_entry, prev_entry));
            --*nof_outlif_replications;
        }

    } else if (*nof_remaining_ports) { 

        --*nof_remaining_ports;
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && DPP_MCDS_REP_DATA_GET_EGR_PORT(*port_cud_reps) < ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
            
            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index, 0, (*port_cud_reps)++, next_entry, prev_entry));
        } else { 
            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_entry_port_cud(unit, index, (*port_cud_reps)++, 0, next_entry, prev_entry));
        }

    } else { 

      DPP_MC_ASSERT(0);
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("wrong function input")));

    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 dpp_mult_eg_write_pointer_entry_struct(
    SOC_SAND_IN    int unit,
    SOC_SAND_INOUT arad_mcdb_entry_t *entry,        
    SOC_SAND_IN    uint32 next_entry,               
    SOC_SAND_IN    uint16 nof_reps_in_core,         
    SOC_SAND_INOUT uint16 *nof_port_couples,        
    SOC_SAND_INOUT uint16 *nof_remaining_ports,     
    SOC_SAND_INOUT uint16 *nof_outlif_replications, 
    SOC_SAND_INOUT uint16 *nof_bitmap_replications, 
    SOC_SAND_INOUT dpp_rep_data_t **couples_reps,   
    SOC_SAND_INOUT dpp_rep_data_t **port_cud_reps,  
    SOC_SAND_INOUT dpp_rep_data_t **cud_only_reps,  
    SOC_SAND_INOUT dpp_rep_data_t **egr_bitmap_reps 
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    dpp_rep_data_t *rep, *rep2;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (*nof_port_couples) { 

        --*nof_port_couples;
        rep = (*couples_reps)++;
        mcds->egr_mc_write_entry_port_cud(unit, entry, (*couples_reps)++, rep, next_entry);

    } else if (*nof_bitmap_replications) { 

        mcds->egr_mc_write_entry_bm_cud(unit, entry, (*egr_bitmap_reps)++, next_entry);
        --*nof_bitmap_replications;

    } else if (*nof_remaining_ports == 2 && nof_reps_in_core == 2)  {

        
        rep = (*port_cud_reps)++;
        mcds->egr_mc_write_entry_port_cud_noptr(unit, entry, rep, (*port_cud_reps)++, 0);
        *nof_remaining_ports = 0;

    } else if (*nof_outlif_replications == 3 && nof_reps_in_core == 3)  {

        
        rep = (*cud_only_reps)++;
        rep2 = (*cud_only_reps)++;
        mcds->egr_mc_write_entry_cud_noptr(unit, entry, rep, rep2, (*cud_only_reps)++, 0);
        (*nof_outlif_replications) = 0;

    } else if (*nof_outlif_replications) { 

        rep = (*cud_only_reps)++;
        if (*nof_outlif_replications >= 2) {
            mcds->egr_mc_write_entry_cud(unit, entry, rep, (*cud_only_reps)++, next_entry);
            (*nof_outlif_replications) -= 2;
        } else {
            mcds->egr_mc_write_entry_cud(unit, entry, rep, 0, next_entry);
            --*nof_outlif_replications;
        }

    } else if (*nof_remaining_ports) { 

        --*nof_remaining_ports;
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && DPP_MCDS_REP_DATA_GET_EGR_PORT(*port_cud_reps) < ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
            
            mcds->egr_mc_write_entry_port_cud(unit, entry, 0, (*port_cud_reps)++, next_entry);
        } else { 
            mcds->egr_mc_write_entry_port_cud(unit, entry, (*port_cud_reps)++, 0, next_entry);
        }

    } else { 

        DPP_MC_ASSERT(0);
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("wrong function input")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



static INLINE uint32 compare_dpp_rep_data_t(const dpp_rep_data_t *rep1, const dpp_rep_data_t *rep2)
{
    return (rep1->base - rep2->base) | (rep1->extra - rep2->extra);
}


static INLINE void dpp_exchange_dpp_rep_data_t(dpp_rep_data_t *a, dpp_rep_data_t *b)
{
    dpp_rep_data_t temp = *a;
    *a = *b;
    *b = temp;
}




uint32 dpp_mcds_set_egress_linked_list(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint8                         is_group_start,      
    SOC_SAND_IN  uint32                        group_id,            
    SOC_SAND_IN  uint32                        list_end,            
    SOC_SAND_IN  uint32                        alloced_block_start, 
    SOC_SAND_IN  dpp_free_entries_block_size_t alloced_block_size,  
    SOC_SAND_IN  dpp_mc_core_bitmap_t          cores_to_set,        
    SOC_SAND_OUT uint32                        *list_start,         
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err             
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int failed = 1;
    uint32 linked_lists[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0}, linked_list_ends[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0}; 
    uint32 block_start = alloced_block_start;
    uint32 list_prev; 
    uint32 prev_entry; 
    dpp_free_entries_block_size_t block_size = alloced_block_size;
    uint16 pointer_entry; 
    arad_mcdb_entry_t start_entries[SOC_DPP_DEFS_MAX(NOF_CORES)];
    dpp_mc_core_bitmap_t core_bm;
    dpp_mc_core_bitmap_t non_zero_cores = DPP_MC_CORE_BITAMAP_NO_CORES; 
    dpp_mc_core_bitmap_t prev_cores_with_reps, new_cores_with_reps = 0; 
    dpp_rep_data_t *rep, *start_of_cud_reps, *start_of_cud2_reps, *couples_dst, *rep_ptr, *rep_ptr2;
    arad_mcdb_entry_t *cud2_entry; 
    
    dpp_rep_data_t *egr_port_outlif_reps, *couples_reps, *egr_outlif_reps, *egr_bitmap_reps;

    uint16 nof_reps_left_in_core;   
    uint16 i, nof_small_ports;      
    uint16 nof_couples;             
    uint16 nof_couples_in_cud;      
    uint16 nof_remaining_ports;     
    uint16 nof_cud_remaining_ports; 
    uint16 nof_port_cud_reps;       
    uint16 nof_port_cud_reps_in_cud;
    uint16 nof_outlif_replications; 
    uint16 nof_bitmap_replications; 
    uint16 nof_cud_reps_in_core, nof_port_cud_reps_in_core, nof_bitmap_reps_in_core; 
    uint32 core, cud = 0, prev_cud, cud2 = DPP_MC_NO_2ND_CUD;
    uint32 max_low_port_for_couple = SOC_IS_JERICHO(unit) ? DPP_MULT_EGRESS_PORT_INVALID : ARAD_MULT_EGRESS_SMALL_PORT_INVALID;
    int end_of_cud2, is_port_cud_rep, created_linked_list_start;

    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { 
        linked_lists[core] = list_end;
    }
    SOCDNX_NULL_CHECK(out_err);
    DPP_MC_ASSERT(core <= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);
    *out_err = _SHR_E_NONE;

    if (mcds->nof_ingr_reps != 0) {
        DPP_MC_ASSERT(0);
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ingress replications exist")));
    }

    
    
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
        
        linked_list_ends[core] = prev_entry = list_prev = is_group_start ? DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_id, core) : group_id;
        pointer_entry = list_end == DPP_MC_EGRESS_LINK_PTR_END ? 0 : 1; 

        nof_reps_left_in_core = mcds->nof_reps[core];
        rep = mcds->reps + DPP_MCDS_GET_REP_INDEX(core, 0);
        if (nof_reps_left_in_core) {
            non_zero_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core; 
            
            soc_sand_os_qsort(rep, nof_reps_left_in_core, sizeof(dpp_rep_data_t), dpp_rep_data_t_compare);
        }

        if (!(SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_ALLOW_DUPLICATES_MODE)) { 
            for (i = nof_reps_left_in_core; i > 1; --i) {
                rep_ptr = rep+1;
                if (!compare_dpp_rep_data_t(rep, rep_ptr)) {
                    *out_err = _SHR_E_PARAM;
                    SOC_EXIT; 
                }
                rep = rep_ptr;
            }
            rep = mcds->reps + DPP_MCDS_GET_REP_INDEX(core, 0);
        }

        
        rep = mcds->reps + DPP_MCDS_GET_REP_INDEX(core, 0);
        end_of_cud2 = nof_reps_left_in_core == 0;
        nof_cud_reps_in_core = nof_port_cud_reps_in_core = nof_bitmap_reps_in_core = 0;
        do { 

            cud2 = end_of_cud2 ? DPP_MC_NO_2ND_CUD : DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep);
            couples_dst = start_of_cud2_reps = couples_reps = rep;
            nof_couples = nof_remaining_ports = nof_port_cud_reps = 0;

            
            if ((is_port_cud_rep = !end_of_cud2 &&  DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD)) {
                
                for (prev_cud = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep); is_port_cud_rep; prev_cud = cud) {
                    start_of_cud_reps = rep;
                    nof_small_ports = nof_cud_remaining_ports = 0;
                    do { 
                        if (DPP_MCDS_REP_DATA_GET_EGR_PORT(rep) < max_low_port_for_couple) {
                            ++nof_small_ports; 
                        } else {
                            ++nof_cud_remaining_ports; 
                        }
                        ++rep;
                        if (--nof_reps_left_in_core == 0 || DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep) != cud2) {
                            end_of_cud2 = 1;
                            is_port_cud_rep = 0;
                            break;
                        }
                        if (!(is_port_cud_rep = DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_PORT_CUD)) {
                            break;
                        }
                    } while ((cud = DPP_MCDS_REP_DATA_GET_EGR_CUD(rep)) == prev_cud);

                    
                    
                    nof_couples_in_cud = (nof_port_cud_reps_in_cud = nof_small_ports + nof_cud_remaining_ports) / 2;
                    if (nof_couples_in_cud > nof_small_ports) {
                        nof_couples_in_cud = nof_small_ports; 
                    }
                    nof_cud_remaining_ports = nof_port_cud_reps_in_cud - 2 * nof_couples_in_cud;
                    
                    DPP_MC_ASSERT(nof_port_cud_reps_in_cud == rep - start_of_cud_reps);
                    for (rep_ptr = start_of_cud_reps, rep_ptr2 = rep - 1; rep_ptr2 > rep_ptr;) {
                        dpp_exchange_dpp_rep_data_t(rep_ptr++, rep_ptr2--);
                    }
                        
                    
                    i = (nof_small_ports + 1)/ 2; 
                    if (nof_couples_in_cud > i) {
                        i = nof_couples_in_cud - i; 
                        rep_ptr = start_of_cud_reps + 1; 
                        rep_ptr2 = start_of_cud_reps + (nof_couples_in_cud - 1) * 2; 
                        do { 
                            dpp_exchange_dpp_rep_data_t(rep_ptr, rep_ptr2);
                            rep_ptr += 2;
                            rep_ptr2 -= 2;
                        } while (--i);
                        DPP_MC_ASSERT(rep_ptr < rep_ptr2 + 2);
                    }

                    if (couples_dst != start_of_cud_reps) {
                        for (i = nof_couples_in_cud * 2, rep_ptr = start_of_cud_reps; i; --i) { 
                            dpp_exchange_dpp_rep_data_t(couples_dst++, rep_ptr++);
                        }
                    } else {
                        couples_dst += nof_couples_in_cud * 2; 
                    }
                    nof_couples += nof_couples_in_cud;
                    nof_remaining_ports += nof_cud_remaining_ports;
                    nof_port_cud_reps += nof_port_cud_reps_in_cud;
                }

            }
            i = nof_couples * 2;
            egr_port_outlif_reps = nof_remaining_ports ? couples_reps + i : NULL;
            if (nof_couples == 0) {
                couples_reps = NULL;
            }
            DPP_MC_ASSERT(nof_port_cud_reps == rep - start_of_cud2_reps && nof_port_cud_reps == nof_remaining_ports + i && i == couples_dst - start_of_cud2_reps);

            
            rep_ptr = rep;
            while (!end_of_cud2 && DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_CUD) {
                ++rep;
                if (--nof_reps_left_in_core == 0 || DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep) != cud2) {
                    end_of_cud2 = 1;
                }
            }
            nof_outlif_replications = rep - rep_ptr;
            egr_outlif_reps = nof_outlif_replications ? rep_ptr : NULL;

            
            rep_ptr = rep;
            while (!end_of_cud2 && DPP_MCDS_REP_DATA_GET_TYPE(rep) == DPP_MCDS_REP_TYPE_EGR_BM_CUD) {
                ++rep;
                if (--nof_reps_left_in_core == 0 || DPP_MCDS_REP_DATA_GET_EXTRA_CUD(rep) != cud2) {
                    end_of_cud2 = 1;
                }
            }
            nof_bitmap_replications = rep - rep_ptr;
            egr_bitmap_reps = nof_bitmap_replications ? rep_ptr : NULL;

            DPP_MC_ASSERT(end_of_cud2 && nof_outlif_replications + nof_port_cud_reps + nof_bitmap_replications == rep - start_of_cud2_reps);
            nof_cud_reps_in_core += nof_outlif_replications;
            nof_port_cud_reps_in_core += nof_port_cud_reps;
            nof_bitmap_reps_in_core += nof_bitmap_replications;
            

            
            if ((created_linked_list_start = is_group_start && nof_reps_left_in_core == 0)) {
                
                if (cud2 != DPP_MC_NO_2ND_CUD) { 
                    DPP_MC_ASSERT(SOC_IS_JERICHO(unit) && (mcds->info_2nd_cud == DPP_MC_GROUP_2ND_CUD_EEI || mcds->info_2nd_cud == DPP_MC_GROUP_2ND_CUD_OUTRIF));
                    
                    dpp_mult_eg_write_cud2_entry_struct(unit, mcds->info_2nd_cud, start_entries + core, cud2, DPP_MC_EGRESS_LINK_PTR_END);
                } else { 
                    if (nof_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications == 0) { 
                        start_entries[core].word0 = mcds->free_value[0];
                        start_entries[core].word1 = mcds->free_value[1];
                        DPP_MC_ASSERT(mcds->nof_reps[core] == 0);
                    } else { 
                        start_entries[core].word1 = start_entries[core].word0 = 0;
                        SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_pointer_entry_struct(
                          unit, start_entries + core, DPP_MC_EGRESS_LINK_PTR_END, mcds->nof_reps[core],
                          &nof_couples, &nof_remaining_ports, &nof_outlif_replications, &nof_bitmap_replications,
                          &couples_reps, &egr_port_outlif_reps, &egr_outlif_reps, &egr_bitmap_reps));
                    }
                }
            }

            
            while (nof_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications) {
                uint16 port_outlif_full_entries = nof_remaining_ports / 2; 
                uint16 outlif_full_entries = nof_outlif_replications / 3; 
                uint16 net_block_size = port_outlif_full_entries + outlif_full_entries ; 
                uint16 full_block_size; 
                uint32 index = -1;

                if (!net_block_size) { 
                    pointer_entry = 1;
                } else if (pointer_entry && 
                    nof_couples + nof_bitmap_replications == 0 &&                
                    (nof_remaining_ports % 2) + (nof_outlif_replications % 3) == 0) { 
                    
                    --net_block_size;
                    if (outlif_full_entries) {
                        --outlif_full_entries;
                    } else {
                        DPP_MC_ASSERT(port_outlif_full_entries);
                        --port_outlif_full_entries;
                    }
                }

                full_block_size = net_block_size + pointer_entry; 
                if (full_block_size >= DPP_MCDS_MAX_FREE_BLOCK_SIZE) {
                    full_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE;
                    net_block_size = DPP_MCDS_MAX_FREE_BLOCK_SIZE - pointer_entry;
                }
                DPP_MC_ASSERT(full_block_size && full_block_size <= DPP_MCDS_MAX_FREE_BLOCK_SIZE && (net_block_size || full_block_size == 1));

                if (!block_size) { 
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_free_entries_block( 
                      mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
                      full_block_size, full_block_size, &block_start, &block_size));
                    if (!block_size) { 
                        *out_err = _SHR_E_FULL;
                        SOC_EXIT; 
                    }
                    DPP_MC_ASSERT(block_size <= full_block_size);
                }

                net_block_size = block_size - pointer_entry; 
                if (net_block_size) { 
                    uint16 block_port_outlif_entries, block_outlif_entries; 
                    full_block_size = block_size;

                    if (net_block_size < port_outlif_full_entries) { 
                        block_port_outlif_entries = net_block_size;
                        block_outlif_entries = 0;
                    } else {
                        block_port_outlif_entries = port_outlif_full_entries;
                        block_outlif_entries = net_block_size - port_outlif_full_entries;
                  
                    }
                    DPP_MC_ASSERT(block_port_outlif_entries +  block_outlif_entries + pointer_entry == block_size &&
                                  port_outlif_full_entries >= block_port_outlif_entries &&
                                  outlif_full_entries >= block_outlif_entries &&
                                  nof_remaining_ports >= block_port_outlif_entries * 2 &&
                                  nof_outlif_replications >= block_outlif_entries * 3);
                    nof_remaining_ports -= block_port_outlif_entries * 2; 
                    nof_outlif_replications -= block_outlif_entries * 3;
            
                    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_add_group_block( 
                      unit, core, block_start, net_block_size, pointer_entry, block_port_outlif_entries, block_outlif_entries,
                      0, 0, &egr_port_outlif_reps, &egr_outlif_reps));

                    DPP_MC_ASSERT(port_outlif_full_entries >= block_port_outlif_entries && outlif_full_entries >= block_outlif_entries);
                    port_outlif_full_entries -= block_port_outlif_entries;
                    outlif_full_entries -= block_outlif_entries;

                    index = block_start + net_block_size;
                    prev_entry = index - 1;
                    
                } else {
                    full_block_size = 1;
                    index = block_start;
                    DPP_MC_ASSERT(pointer_entry == 1);
                }

                if (pointer_entry) { 
                    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_write_pointer_entry( 
                      unit, index, prev_entry, linked_lists[core],
                      &nof_couples, &nof_remaining_ports, &nof_outlif_replications, &nof_bitmap_replications,
                      &couples_reps, &egr_port_outlif_reps, &egr_outlif_reps, &egr_bitmap_reps));
                } else { 
                    pointer_entry = 1;
                    DPP_MC_ASSERT(linked_lists[core] == list_end);
                }

                DPP_MC_ASSERT(block_size >= full_block_size);
                if (linked_lists[core] != list_end) { 
                    DPP_MCDS_SET_PREV_ENTRY(mcds, linked_lists[core], block_start + full_block_size - 1);
                } else {
                    linked_list_ends[core] = block_start + full_block_size - 1;
                }

                linked_lists[core] = block_start; 
                if (block_size -= full_block_size) { 
                    block_start += full_block_size;
                }

            } 
            DPP_MC_ASSERT(nof_couples + nof_remaining_ports + nof_outlif_replications + nof_bitmap_replications == 0 && !block_size);

            if (!created_linked_list_start && cud2 != DPP_MC_NO_2ND_CUD) {
                
                if (!block_size) { 
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_free_entries_block( 
                      mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_INGRESS | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL, 1, 1, &block_start, &block_size));
                    if (!block_size) { 
                        *out_err = _SHR_E_FULL;
                        SOC_EXIT; 
                    }
                    DPP_MC_ASSERT(block_size == 1);
                }
                cud2_entry = MCDS_GET_MCDB_ENTRY(mcds, block_start);
                dpp_mult_eg_write_cud2_entry_struct(unit, mcds->info_2nd_cud, cud2_entry, cud2, linked_lists[core]); 
                SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, block_start)); 
                DPP_MCDS_ENTRY_SET_TYPE(cud2_entry, DPP_MCDS_TYPE_VALUE_EGRESS);
                if (linked_lists[core] != list_end) { 
                    DPP_MCDS_SET_PREV_ENTRY(mcds, linked_lists[core], block_start);
                } else {
                    linked_list_ends[core] = block_start;
                }
                prev_entry = linked_lists[core] = block_start; 
                --block_size;
                ++block_start;
            }

            end_of_cud2 = 0; 
        } while (nof_reps_left_in_core != 0); 

        DPP_MC_ASSERT( 
          nof_cud_reps_in_core == mcds->nof_egr_outlif_reps[core] && nof_bitmap_reps_in_core == mcds->nof_egr_bitmap_reps[core] &&
          nof_port_cud_reps_in_core == mcds->nof_egr_port_outlif_reps[core] &&
          nof_port_cud_reps_in_core + nof_cud_reps_in_core + nof_bitmap_reps_in_core == mcds->nof_reps[core] &&
          mcds->nof_reps[core] + DPP_MCDS_GET_REP_INDEX(core, 0) == rep - mcds->reps &&
          mcds->nof_reps[core] <= DPP_MULT_MAX_EGRESS_REPLICATIONS);
    }

    
    list_prev = group_id;
    if (is_group_start) { 
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_get_core_replication(unit, group_id, &prev_cores_with_reps)); 
        new_cores_with_reps = (non_zero_cores | ~cores_to_set) & prev_cores_with_reps; 
        if (new_cores_with_reps != prev_cores_with_reps) { 
            SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, group_id, new_cores_with_reps));
            prev_cores_with_reps = new_cores_with_reps;
        }
        new_cores_with_reps |= non_zero_cores; 
    }
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
        
        if (is_group_start) { 
            arad_mcdb_entry_t *mcdb_entry;
            list_prev = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_id, core);
            mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, list_prev);
            mcdb_entry->word0 = start_entries[core].word0; 
            mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
            
            mcdb_entry->word1 |= start_entries[core].word1;

            DPP_MCDS_SET_PREV_ENTRY(mcds, list_prev, list_prev); 
            DPP_MCDS_ENTRY_SET_TYPE(mcdb_entry, DPP_MCDS_TYPE_VALUE_EGRESS_START); 
        }
        if (linked_lists[core] != list_end) { 
            DPP_MCDS_SET_PREV_ENTRY(mcds, linked_lists[core], list_prev);
        }
        SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
          mcds, unit, list_prev, DPP_MCDS_TYPE_VALUE_EGRESS_START, linked_lists[core]));
        linked_lists[core] = list_end; 

        if (list_end != DPP_MC_EGRESS_LINK_PTR_END) { 
            DPP_MCDS_SET_PREV_ENTRY(mcds, list_end, linked_list_ends[core]);
        }
    }

    if (is_group_start && new_cores_with_reps != prev_cores_with_reps) { 
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, group_id, new_cores_with_reps));
    }

    failed = 0;
exit:
    if (failed) { 
        DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
            if (linked_lists[core] != list_end) { 
                  dpp_mcdb_free_linked_list_till_my_end(unit, linked_lists[core], DPP_MCDS_TYPE_VALUE_EGRESS, list_end);
            }
        }
        if (block_size) { 
            DPP_MC_ASSERT(failed || (block_start >= alloced_block_start && block_start < alloced_block_start + alloced_block_size));
            dpp_mcds_build_free_blocks(unit, mcds,
              block_start, block_start + block_size - 1, dpp_mcds_get_region(mcds, block_start),
              block_start == alloced_block_start ? McdsFreeBuildBlocksAddAll : McdsFreeBuildBlocksAdd_AllMustBeUsed);
        }
    }
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  dpp_mc_id_t          mcid,         
    SOC_SAND_IN  uint8                allow_create, 
    SOC_SAND_IN  uint32               group_size,   
    SOC_SAND_IN  dpp_mc_replication_t *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR        *out_err      
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint8 group_exists = 0;
    int failed = 1, core;
    uint32 entry_type, created_list, group_entry_id;
    uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)] = {DPP_MC_EGRESS_LINK_PTR_END}; 
    const dpp_mc_core_bitmap_t cores_to_set = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
    dpp_mc_core_bitmap_t core_bm, group_start_alloced_bm = DPP_MC_CORE_BITAMAP_NO_CORES;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(out_err);


    DPP_MC_ASSERT(mcid < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_get(unit, mcid, &group_exists));
    if (!group_exists && !allow_create) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }
    DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { 
        group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
        entry_type = DPP_MCDS_GET_TYPE(mcds, group_entry_id);
        DPP_MC_ASSERT(group_exists == DPP_MCDS_TYPE_IS_EGRESS_START(entry_type));

        
        if (!group_exists && group_entry_id != MCDS_INGRESS_LINK_END(mcds)) {
            if (DPP_MCDS_TYPE_IS_USED(entry_type)) { 
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_relocate_entries(unit, group_entry_id, 0, 0, out_err));
                if (*out_err) { 
                    SOC_EXIT;
                }
            } else { 
                SOCDNX_IF_ERR_EXIT(dpp_mcds_reserve_group_start(mcds, group_entry_id));
                DPP_MCDS_SET_TYPE(mcds, group_entry_id, DPP_MCDS_TYPE_VALUE_EGRESS_START); 
            }
            group_start_alloced_bm |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
        }

        if (group_exists) { 
            SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, group_entry_id, DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
        }
    }

    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays(unit, 1, 1, group_size, reps)); 

    SOCDNX_IF_ERR_EXIT(mcds->set_egress_linked_list( 
      unit, TRUE, mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, cores_to_set, &created_list, out_err));
    if (*out_err) { 
        SOC_EXIT;
    }

    if (group_exists) { 
        DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) {
            if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) { 
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list(unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
            }
        }
    } else {
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 1));
    }

    failed = 0;
exit:
    if (group_start_alloced_bm && failed) { 
        DPP_MC_ASSERT(!group_exists);
        DPP_MC_FOREACH_CORE(group_start_alloced_bm, core_bm, core) {
            group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
            dpp_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
              dpp_mcds_get_region(mcds, group_entry_id), McdsFreeBuildBlocksAdd_AllMustBeUsed);
        }
    }
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_bitmap_group_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
)
{
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  if (info->mc_id_high >= DPP_MC_EGR_NOF_BITMAPS || info->mc_id_low)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid bitmap range")));
  }

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf,  info->mc_id_high));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  dpp_mult_eg_bitmap_group_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
)
{
  uint32 max_direct_bitmap_group;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(info);

  
  info->mc_id_low = 0;

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf, &max_direct_bitmap_group));
  info->mc_id_high = max_direct_bitmap_group;

exit:
  SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_group_close(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t mcid
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 group_entry_id, core;
    const dpp_mc_core_bitmap_t cores_to_set = DPP_MC_CORE_BITAMAP_ALL_ACTIVE_CORES(unit);
    dpp_mc_core_bitmap_t core_bm;
    SOC_TMC_ERROR internal_err;
    uint8 does_exist;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, mcid, TRUE, &does_exist));
    if (does_exist) { 
        if (mcid <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) { 

            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_close(unit, mcid));

        } else { 

            SOCDNX_IF_ERR_EXIT(dpp_mult_eg_group_set( 
              unit, mcid, FALSE, 0, 0, &internal_err));
            if (internal_err) { 
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("too many entries")));
            }

            DPP_MC_FOREACH_CORE(cores_to_set, core_bm, core) { 
                group_entry_id = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, mcid, core);
                if (group_entry_id != MCDS_INGRESS_LINK_END(mcds)) { 
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, group_entry_id, group_entry_id,
                      dpp_mcds_get_region(mcds, group_entry_id), McdsFreeBuildBlocksAdd_AllMustBeUsed));
                } else { 
                    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, MCDS_INGRESS_LINK_END(mcds));
                    mcdb_entry->word0 = mcds->free_value[0];
                    mcdb_entry->word1 &= DPP_MCDS_WORD1_KEEP_BITS_MASK;
                    mcdb_entry->word1 |= mcds->free_value[1];
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, MCDS_INGRESS_LINK_END(mcds))); 
                }
            }

            SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, mcid, 0));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32 dpp_mcds_remove_replications_from_egress_group(
    SOC_SAND_IN int         unit,        
    SOC_SAND_IN int         core,        
    SOC_SAND_IN dpp_mc_id_t group_mcid,  
    SOC_SAND_IN uint32      dest,        
    SOC_SAND_IN uint32      cud,         
    SOC_SAND_IN uint32      cud2         
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    const uint32 entry_index = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core); 
    uint32 freed_index = entry_index; 
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, entry_index);
    uint32 format = 0, next_index = entry_index, cur_index = 0, prev_index = entry_index, bitmap = -1, cud2_field;
    int is_cud_only_replication = 0, is_port_cud_replication = 0, is_bitmap_cud_replication = 0;
    int remove_entry = 0, found = 0, correct_cud2;
    uint16 entries_left = DPP_MULT_MAX_EGRESS_REPLICATIONS; 
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS_START == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); 

    
    if (dest == _SHR_GPORT_INVALID) { 
        is_cud_only_replication = 1;
        if (cud == DPP_MC_EGR_OUTLIF_DISABLED) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    } else if (dest & ARAD_MC_EGR_IS_BITMAP_BIT) { 
        bitmap = dest & ~ARAD_MC_EGR_IS_BITMAP_BIT;
        is_bitmap_cud_replication = 1;
        if (bitmap == DPP_MC_EGR_OUTLIF_DISABLED) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    } else {
        is_port_cud_replication = 1;
        if (dest == DPP_MULT_EGRESS_PORT_INVALID) { 
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    }

    if (SOC_IS_JERICHO(unit)) { 

        correct_cud2 = (cud2_field = cud2 & ~DPP_MC_2ND_CUD_IS_EEI) == DPP_MC_NO_2ND_CUD;
        for (; entries_left; --entries_left) { 
            cur_index = next_index;
            next_index = DPP_MC_EGRESS_LINK_PTR_END;
            format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf);
            switch (format) {
              
              case 0: 
                next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf);
                if (correct_cud2 && is_port_cud_replication && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f) == cud) {
                    uint32 port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Af);
                    if (dest == port) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Af, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Bf) == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    } else if (dest == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Bf)) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Bf, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (port == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    }
                }
                break;

              case 1: 
                next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, LINK_PTRf);
                if (soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, NEW_FORMATf)) { 
                    correct_cud2 = cud2_field == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f);
                } else if (correct_cud2 && is_bitmap_cud_replication && bitmap == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, BMP_PTRf) &&
                    
                    cud == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f)) {
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, BMP_PTRf, DPP_MC_EGR_BITMAP_DISABLED); 
                    found = remove_entry = 1;
                }
                break;

              case 3: 
                next_index = cur_index + 1;
              case 2:
                if (correct_cud2 && is_port_cud_replication) {
                    uint32 port1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f);
                    uint32 port2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f);
                    if (port1 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_1f) == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (port2 == DPP_MULT_EGRESS_PORT_INVALID) { 
                            remove_entry = 1;
                        }
                    } else if (port2 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f) == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (port1 == DPP_MULT_EGRESS_PORT_INVALID) { 
                            remove_entry = 1;
                        }
                    }
                }
                break;

              default:
                if (format < 8) { 
                    next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, LINK_PTRf);
                    if (correct_cud2 && is_cud_only_replication) {
                        uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_1f);
                        uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_2f);
                        if (outlif1 == cud) {
                            found = 1;
                            soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); 
                            if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                                remove_entry = 1;
                            }
                        } else if (outlif2 == cud) {
                            found = 1;
                            soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); 
                            if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                                remove_entry = 1;
                            }
                        }
                    }
                    break;

                } else { 

                    if (format & 4) {
                        next_index = cur_index + 1;
                    }
                    if (correct_cud2 && is_cud_only_replication) {
                        uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f);
                        uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f);
                        uint32 outlif3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f);
                        if (outlif1 == cud) {
                            found = 1;
                            soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); 
                            if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                                remove_entry = 1;
                            }
                        } else if (outlif2 == cud) {
                            found = 1;
                            soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); 
                            if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                                remove_entry = 1;
                            }
                        } else if (outlif3 == cud) {
                            found = 1;
                            soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f, DPP_MC_EGR_OUTLIF_DISABLED); 
                            if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                                remove_entry = 1;
                            }
                        }
                    }
                }

            } 

            if (found) {
                break;
            }

            if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("replication not found in group")));
            }
            prev_index = cur_index;
            mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
            DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); 
        }

        if (!found) {
            DPP_MC_ASSERT(0);
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("The replication was not found till the legal size of a group")));
        }

        if (remove_entry) { 
            int try_to_copy_the_next_entry = 0;
            if (cur_index == entry_index) { 
                DPP_MC_ASSERT(prev_index == entry_index);
                if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
                    mcdb_entry->word0 = mcds->free_value[0];
                    mcdb_entry->word1 &= ~mcds->msb_word_mask;
                    mcdb_entry->word1 |= mcds->free_value[1];
                    SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, group_mcid, DPP_MC_CORE_BITAMAP_CORE_0 << core));
                } else  { 
                    try_to_copy_the_next_entry = 1;
                }
            } else { 
                arad_mcdb_entry_t *prev_entry = MCDS_GET_MCDB_ENTRY(mcds, prev_index);
                uint32 prev_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, prev_entry, ENTRY_FORMATf), prev_format_next_bit;
                DPP_MC_ASSERT(prev_index != cur_index);
                if (prev_format < 8 && (prev_format & 14) != 2) { 
                    
                    SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
                      mcds, unit, prev_index, DPP_MCDS_TYPE_VALUE_EGRESS, next_index));
                    freed_index = cur_index;
                } else if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
                    prev_format_next_bit = prev_format >= 8 ? 4 : 1;
                    DPP_MC_ASSERT(prev_format & prev_format_next_bit);
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, prev_entry, ENTRY_FORMATf, prev_format & ~prev_format_next_bit); 
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, prev_index));
                    freed_index = cur_index;
                } else {
                    try_to_copy_the_next_entry = 1;
                }
            }

            if (try_to_copy_the_next_entry) { 
                
                arad_mcdb_entry_t *next_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
                uint32 next_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, next_entry, ENTRY_FORMATf);
                DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(next_entry) == DPP_MCDS_TYPE_VALUE_EGRESS);
                if (next_format < 12 && next_format != 3) { 
                    mcdb_entry->word0 = next_entry->word0;
                    mcdb_entry->word1 &= ~mcds->msb_word_mask;
                    mcdb_entry->word1 |= (next_entry->word1 & mcds->msb_word_mask);
                    freed_index = next_index;
                    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, next_index, DPP_MCDS_TYPE_VALUE_EGRESS, &next_index)); 
                }
            }
        }
    } else { 

        for (; entries_left; --entries_left) { 
            cur_index = next_index;

            switch (format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, ENTRY_FORMATf)) {
              
              case 0: 
                next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, LINK_PTRf);
                if (is_port_cud_replication && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, OUTLIF_1f) == cud) {
                    uint32 port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Af);
                    if (dest == port) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Af, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf) == ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    } else if (dest < ARAD_MULT_EGRESS_SMALL_PORT_INVALID && dest == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf)) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf, ARAD_MULT_EGRESS_SMALL_PORT_INVALID); 
                        if (port == DPP_MULT_EGRESS_PORT_INVALID) {
                            remove_entry = 1;
                        }
                    }
                }
                break;

              case 1: 
                next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf);
                if (is_bitmap_cud_replication && bitmap == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, BMP_PTRf) &&
                    cud == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f)) {
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, BMP_PTRf, DPP_MC_EGR_BITMAP_DISABLED); 
                    found = remove_entry = 1;
                }
                break;

              case 2: 
              case 3:
                next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, LINK_PTRf);
                if (is_cud_only_replication) {
                    uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f);
                    uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_2f);
                    if (outlif1 == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); 
                        if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                            remove_entry = 1;
                        }
                    } else if (outlif2 == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); 
                        if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                            remove_entry = 1;
                        }
                    }
                }
                break;

              case 4: 
              case 5:
                if (is_port_cud_replication) {
                    uint32 port1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f);
                    uint32 port2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f);
                    if (port1 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_1f) == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (port2 == DPP_MULT_EGRESS_PORT_INVALID) { 
                            remove_entry = 1;
                        }
                    } else if (port2 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f) == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f, DPP_MULT_EGRESS_PORT_INVALID); 
                        if (port1 == DPP_MULT_EGRESS_PORT_INVALID) { 
                            remove_entry = 1;
                        }
                    }
                }
                break;

              default: 
                if (is_cud_only_replication) {
                    uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f);
                    uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f);
                    uint32 outlif3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f);
                    if (outlif1 == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); 
                        if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                            remove_entry = 1;
                        }
                    } else if (outlif2 == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); 
                        if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                            remove_entry = 1;
                        }
                    } else if (outlif3 == cud) {
                        found = 1;
                        soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f, DPP_MC_EGR_OUTLIF_DISABLED); 
                        if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { 
                            remove_entry = 1;
                        }
                    }
                }
            }

            
            if (format >= 4) {
                if (format & 1) {
                    next_index = cur_index + 1;
                    DPP_MC_ASSERT(next_index < MCDS_INGRESS_LINK_END(mcds));
                } else {
                    next_index = DPP_MC_EGRESS_LINK_PTR_END;
                }
            }
            if (found) {
                break;
            }

            if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("replication not found in group")));
            }
            prev_index = cur_index;
            mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
            DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); 
        }

        if (!found) {
            DPP_MC_ASSERT(0);
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("The replication was not found till the legal size of a group")));
        }

        if (remove_entry) { 
            int try_to_copy_the_next_entry = 0;
            if (cur_index == entry_index) { 
                DPP_MC_ASSERT(prev_index == entry_index);
                if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
                    mcdb_entry->word0 = mcds->free_value[0];
                    mcdb_entry->word1 &= ~mcds->msb_word_mask;
                    mcdb_entry->word1 |= mcds->free_value[1];
                    SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, group_mcid, DPP_MC_CORE_BITAMAP_CORE_0 << core));
                } else  { 
                    try_to_copy_the_next_entry = 1;
                }
            } else { 
                arad_mcdb_entry_t *prev_entry = MCDS_GET_MCDB_ENTRY(mcds, prev_index);
                uint32 prev_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, prev_entry, ENTRY_FORMATf);
                DPP_MC_ASSERT(prev_index != cur_index);
                if (prev_format < 4) { 
                    
                    SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( 
                      mcds, unit, prev_index, DPP_MCDS_TYPE_VALUE_EGRESS, next_index));
                    freed_index = cur_index;
                } else if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { 
                    DPP_MC_ASSERT(prev_format & 1);
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, prev_entry, ENTRY_FORMATf, prev_format & ~1); 
                    SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, prev_index));
                    freed_index = cur_index;
                } else {
                    try_to_copy_the_next_entry = 1;
                }
            }

            if (try_to_copy_the_next_entry) { 
                arad_mcdb_entry_t *next_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
                uint32 next_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, next_entry, ENTRY_FORMATf);
                DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(next_entry) == DPP_MCDS_TYPE_VALUE_EGRESS);
                if (next_format < 4 || !(next_format & 1)) { 
                    mcdb_entry->word0 = next_entry->word0;
                    mcdb_entry->word1 &= ~mcds->msb_word_mask;
                    mcdb_entry->word1 |= (next_entry->word1 & mcds->msb_word_mask);
                    freed_index = next_index;
                    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, next_index, DPP_MCDS_TYPE_VALUE_EGRESS, &next_index)); 
                }
            }
        }
    }

    if (freed_index != cur_index || !remove_entry) { 
        SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, cur_index)); 
    }

    if (freed_index != entry_index) { 
        if (next_index != DPP_MC_EGRESS_LINK_PTR_END) {
            DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_index) == DPP_MCDS_TYPE_VALUE_EGRESS);
            DPP_MCDS_SET_PREV_ENTRY(mcds, next_index, prev_index); 
        }
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks( 
          unit, mcds, freed_index, freed_index, dpp_mcds_get_region(mcds, freed_index), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_reps_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid, 
    SOC_SAND_IN  uint32                nof_reps,   
    SOC_SAND_IN  dpp_mc_replication_t  *reps,      
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err    
)
{
    uint16 group_size;
    uint32 linked_list_start, created_list;
    uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)] = {DPP_MC_EGRESS_LINK_PTR_END}; 
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int core;
    dpp_mc_core_bitmap_t changed_cores = DPP_MC_CORE_BITAMAP_NO_CORES, core_bm;
    uint8 is_open;
    uint32 nof_reps_all_cores = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);
    SOCDNX_NULL_CHECK(out_err);
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_get(unit, group_mcid, &is_open));
    if (!is_open) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }


    
    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays( unit, TRUE, TRUE, nof_reps, reps));

    
    for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
        if (mcds->nof_reps[core] > 0) {
            changed_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
        }
        nof_reps_all_cores += mcds->nof_reps[core]; 

        linked_list_start = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core);
        DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, linked_list_start) == DPP_MCDS_TYPE_VALUE_EGRESS_START);
        SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( 
          mcds, unit, linked_list_start, DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
    }

    
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group( unit, changed_cores, FALSE, TRUE, group_mcid,
      DPP_MCDS_TYPE_VALUE_EGRESS_START, DPP_MULT_MAX_EGRESS_REPLICATIONS - nof_reps, &group_size));
    if (group_size > DPP_MULT_MAX_EGRESS_REPLICATIONS - nof_reps_all_cores) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("too many replications")));
    }

    
    SOCDNX_IF_ERR_EXIT(mcds->set_egress_linked_list(unit, TRUE, group_mcid,
      DPP_MC_EGRESS_LINK_PTR_END, 0, 0, changed_cores, &created_list, out_err));
    if (*out_err) { 
        if (*out_err == _SHR_E_PARAM) {
            *out_err = _SHR_E_EXISTS;
        }
    } else { 
        DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
            if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) {
                SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list( unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_reps_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  dpp_mc_id_t           group_mcid,   
    SOC_SAND_IN  uint32                nof_reps,     
    SOC_SAND_IN  dpp_mc_replication_t  *reps,        
    SOC_SAND_OUT SOC_TMC_ERROR         *out_err      
)
{
    int rep_index;
    uint16 group_size;
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    int core;
    dpp_mc_core_bitmap_t changed_cores = DPP_MC_CORE_BITAMAP_NO_CORES;
    uint8 is_open;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reps);


    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_get(unit, group_mcid, &is_open));
    if (!is_open) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("MC group is not created")));
    }

    
    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_arrays(unit, TRUE, TRUE, nof_reps, reps)); 
    for (core = 0; core < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; ++core) {
        if (mcds->nof_reps[core] > 0) {
            changed_cores |= DPP_MC_CORE_BITAMAP_CORE_0 << core;
        }
    }

    
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_group(unit, changed_cores, TRUE, TRUE, group_mcid,
        DPP_MCDS_TYPE_VALUE_EGRESS_START, DPP_MULT_MAX_EGRESS_REPLICATIONS, &group_size));

    
    for (rep_index = 0; rep_index < nof_reps; ++rep_index)
    {
        SOCDNX_IF_ERR_EXIT(dpp_mult_remove_replication(unit, DPP_MCDS_TYPE_VALUE_EGRESS_START, reps[rep_index].dest, reps[rep_index].cud, reps[rep_index].additional_cud, out_err, &changed_cores)); 
        if (*out_err) { 
            SOC_EXIT;
        }
    }

    
    {
        uint32 old_group_entries[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0}, created_list;
        dpp_mc_core_bitmap_t core_bm;
        DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
            SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER( 
                mcds, unit, DPP_MCDS_GET_EGRESS_GROUP_START(mcds, group_mcid, core), DPP_MCDS_TYPE_VALUE_EGRESS, old_group_entries + core));
        }

        SOCDNX_IF_ERR_EXIT(mcds->set_egress_linked_list( 
            unit, TRUE, group_mcid, DPP_MC_EGRESS_LINK_PTR_END, 0, 0, changed_cores, &created_list, out_err));
        if (*out_err) { 
#ifndef ARAD_EGRESS_MC_DELETE_FAILS_ON_FULL_MCDB
            soc_error_t ret1, ret2 = SOC_E_NONE;
            
            DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
              for (rep_index = 0; rep_index < nof_reps; ++rep_index)
              {
                  ret1 = dpp_mcds_remove_replications_from_egress_group(unit, core, group_mcid, reps[rep_index].dest, reps[rep_index].cud, reps[rep_index].additional_cud); 
                  if (ret2 == SOC_E_NONE) {
                      ret2 = ret1;
                  }
              }
            }
            if (ret2 != SOC_E_NONE) {
                SOCDNX_EXIT_WITH_ERR(ret2, (_BSL_SOCDNX_MSG("failed to delete egress replication")));
            }
            *out_err = _SHR_E_NONE;
#endif 
        } else { 
            DPP_MC_FOREACH_CORE(changed_cores, core_bm, core) {
                if (old_group_entries[core] != DPP_MC_EGRESS_LINK_PTR_END) {
                    SOCDNX_IF_ERR_EXIT(dpp_mcdb_free_linked_list(unit, old_group_entries[core], DPP_MCDS_TYPE_VALUE_EGRESS));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_group_size_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx,
    SOC_SAND_OUT uint32       *mc_group_size
)
{
  uint8 does_exit;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(mc_group_size);


  if (multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) { 

    *mc_group_size = 0;
    SOCDNX_IF_ERR_EXIT(dpp_mult_does_group_exist(unit, multicast_id_ndx, TRUE, &does_exit));
    if (does_exit) { 
      dpp_mc_egr_bitmap_t bitmap;
      uint32 bits, *bits_ptr = bitmap;
      unsigned bits_left, words_left = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;

      SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));
      for (; words_left; --words_left) {
        bits = *(bits_ptr++);
        for (bits_left = SOC_SAND_NOF_BITS_IN_UINT32; bits_left; --bits_left) {
          *mc_group_size += (bits & 1);
          bits >>= 1;
        }
      }
    }

  } else { 

    uint8 is_open;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_get_group(
      unit, multicast_id_ndx, 0, 0, 0, 0, mc_group_size, &is_open));

  }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_bitmap_group_zero(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx, 
    SOC_SAND_IN  uint8       create            
)
{
    uint32 data[DPP_EGQ_VLAN_TABLE_TBL_ENTRY_SIZE] = {0};
    dpp_mc_core_bitmap_t old_cores, new_cores = DPP_MC_CORE_BITAMAP_NO_CORES;

    SOCDNX_INIT_FUNC_DEFS;
    if (multicast_id_ndx > SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("MCID is not a bitmap group")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_egress_group_get_core_replication(unit, multicast_id_ndx, &old_cores)); 
    if (old_cores != new_cores) {
        SOCDNX_IF_ERR_EXIT(dpp_egress_group_set_core_replication(unit, multicast_id_ndx, new_cores)); 
    }
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, SOC_CORE_ALL), multicast_id_ndx, data)); 
    SOCDNX_IF_ERR_EXIT(dpp_egress_group_open_set(unit, multicast_id_ndx, create)); 

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_bitmap_group_create(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  dpp_mc_id_t multicast_id_ndx 
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_zero(unit, multicast_id_ndx, 1));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_bitmap_group_update(
                  int                                   unit,
                  dpp_mc_id_t                           multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *group 
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(group);
    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, &group->bitmap[0]));
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_bitmap_group_close(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  dpp_mc_id_t  multicast_id_ndx
)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_zero(unit, multicast_id_ndx, 0));
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,          
    SOC_SAND_OUT uint32              *out_bit_index 
)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) {
        uint32 tm_port;
        int core;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        *out_bit_index = tm_port + SOC_TMC_NOF_FAP_PORTS_PER_CORE * core;
    } else {
        *out_bit_index = port;
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_bitmap_group_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,         
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      
)
{
    uint32 bit_index, word_index, bit_mask;
    dpp_mc_egr_bitmap_t bitmap = {0};

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(unit, port, &bit_index));
    word_index = bit_index / SOC_SAND_NOF_BITS_IN_UINT32; 
    bit_mask = (uint32)1 << (bit_index & (SOC_SAND_NOF_BITS_IN_UINT32 - 1));

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    if (bitmap[word_index] & bit_mask) {
        *out_err = _SHR_E_EXISTS; 
    } else {
        bitmap[word_index] |= bit_mask;
        SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
        *out_err = _SHR_E_NONE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_bitmap_group_bm_add(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *add_bm, 
                  SOC_TMC_ERROR                         *out_err 
)
{
    unsigned nof_tm_port_words = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dpp_mc_egr_bitmap_t bitmap = {0};
    const uint32 *changes = &add_bm->bitmap[0];
    uint32 *to_write = bitmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    for (; nof_tm_port_words; --nof_tm_port_words) { 
        if (*to_write & *changes) {
            *out_err = _SHR_E_EXISTS; 
            SOC_EXIT;
        }
        *(to_write++) |= *(changes++);
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
    *out_err = _SHR_E_NONE;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_bitmap_group_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  dpp_mc_id_t         multicast_id_ndx,
    SOC_SAND_IN  SOC_TMC_FAP_PORT_ID port,         
    SOC_SAND_OUT SOC_TMC_ERROR       *out_err      
)
{
    uint32 bit_index, word_index, bit_mask;
    dpp_mc_egr_bitmap_t bitmap = {0};

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_local_port_2_bitmap_bit_index(unit, port, &bit_index));
    word_index = bit_index / SOC_SAND_NOF_BITS_IN_UINT32;
    bit_mask = (uint32)1 << (bit_index & (SOC_SAND_NOF_BITS_IN_UINT32 - 1));

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    if (bitmap[word_index] & bit_mask) {
        bitmap[word_index] &= ~bit_mask;
        SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
        *out_err = _SHR_E_NONE;
    } else {
        *out_err = _SHR_E_NOT_FOUND; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 dpp_mult_eg_bitmap_group_bm_remove(
                  int                 unit,
                  dpp_mc_id_t         multicast_id_ndx,
                  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *rem_bm, 
                  SOC_TMC_ERROR                         *out_err 
)
{
    unsigned nof_tm_port_words = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    dpp_mc_egr_bitmap_t bitmap = {0};
    const uint32 *changes = &rem_bm->bitmap[0];
    uint32 *to_write = bitmap;

    SOCDNX_INIT_FUNC_DEFS;

    if (multicast_id_ndx >= DPP_MC_EGR_NOF_BITMAPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("ID is too high for a multicast bitmap")));
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_get(unit, multicast_id_ndx, bitmap));

    for (; nof_tm_port_words; --nof_tm_port_words) { 
        if (~*to_write & *changes) {
            *out_err = _SHR_E_NOT_FOUND; 
            SOC_EXIT;
        }
        *(to_write++) &= ~*(changes++);
    }

    SOCDNX_IF_ERR_EXIT(dpp_egq_vlan_table_tbl_set(unit, multicast_id_ndx, bitmap));
    *out_err = _SHR_E_NONE;

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

