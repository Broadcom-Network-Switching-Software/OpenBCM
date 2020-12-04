/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#ifdef BCM_88675_A0


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST




#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/JER/jer_multicast_imp.h>
#include <soc/dcmn/error.h>
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_ports.h>







#define ARAD_MC_ING_DESTINATION_FLD_TYPE_OFFSET 16
#define ARAD_MC_ING_DESTINATION_FLD_TYPE 0x30000 
#define ARAD_MC_ING_DESTINATION_ID_MASK_TM_FLOW 0x1ffff
#define ARAD_MC_ING_DESTINATION_ID_MASK_OTHERS 0xffff
#define JER_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW (1 << 17)
#define JER_MC_ING_DESTINATION_FLD_TYPE_SYSPORT (1 << 16)
#define JER_MC_ING_DESTINATION_FLD_TYPE_LAG (3 << 15)
#define ARAD_MC_ING_DESTINATION_FLD_TYPE_PORT 0x00000




#define JER_MC_ING_DESTINATION_SYSPORT_BITS 15
#define JER_MC_ING_DESTINATION_SYSPORT_MASK ((1 << JER_MC_ING_DESTINATION_SYSPORT_BITS) - 1)
#define JER_MC_ING_DESTINATION_ID_MASK_TM_FLOW 0x1ffff






















uint32 jer_mcdb_get_next_pointer(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry,      
    SOC_SAND_IN  uint32  entry_type, 
    SOC_SAND_OUT uint32  *next_entry 
)
{
    dpp_mcds_t *mcds = dpp_get_mcds(unit);
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, entry);
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPES_ARE_THE_SAME(entry_type, DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)));

    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 
        *next_entry = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);
    } else { 
        uint32 format_type = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf);
        switch (format_type >> 1) { 
          case 0:
            *next_entry = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf);
            break;
          case 1:
            *next_entry = (format_type & 1) ? entry + 1 : DPP_MC_EGRESS_LINK_PTR_END;
            break;
          case 2:
          case 3:
            *next_entry = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_3m, mcdb_entry, LINK_PTRf);
            break;
          default:
            *next_entry = (format_type & 4) ? entry + 1 : DPP_MC_EGRESS_LINK_PTR_END;
        }
      DPP_MC_ASSERT(*next_entry < MCDS_INGRESS_LINK_END(mcds));
    }

    SOCDNX_FUNC_RETURN;
}



uint32 jer_mcdb_set_next_pointer(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  entry_to_set, 
    SOC_SAND_IN  uint32  entry_type,   
    SOC_SAND_IN  uint32  next_entry    
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, entry_to_set);
    soc_mem_t mem = 0;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPES_ARE_THE_SAME(entry_type, DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)));

    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 
        mem = IRR_MCDBm;
    } else if (DPP_MCDS_TYPE_IS_EGRESS_NORMAL(entry_type)) { 
        uint32 entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf);
        switch (entry_format >> 1) { 
          case 0:
            mem = IRR_MCDB_EGRESS_FORMAT_1m;
            break;
          case 1:
            if (entry_type != DPP_MCDS_TYPE_VALUE_EGRESS_START || next_entry != DPP_MC_EGRESS_LINK_PTR_END || (entry_format & 1)) {
                mem = -1;
            } 
            break;
          case 2:
          case 3:
            mem = IRR_MCDB_EGRESS_FORMAT_3m;
            break;
          default:
            if (entry_type != DPP_MCDS_TYPE_VALUE_EGRESS_START || next_entry != DPP_MC_EGRESS_LINK_PTR_END || (entry_format & 4)) {
                mem = -1;
            } 
        }
    } else {
        mem = -1;
    }

    if (mem == -1) {
        DPP_MC_ASSERT(0);
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("cannot set pointer in this entry type")));
    } else if (mem) {
        soc_mem_field32_set(unit, mem, mcdb_entry, LINK_PTRf, next_entry); 
    }
    SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, entry_to_set)); 

exit:
    SOCDNX_FUNC_RETURN;
}




 
uint32 jer_mcds_get_replications_from_entry(
    SOC_SAND_IN    int     unit,
    SOC_SAND_IN    int     core,        
    SOC_SAND_IN    uint8   get_bm_reps, 
    SOC_SAND_IN    uint32  entry_index, 
    SOC_SAND_IN    uint32  entry_type,  
    SOC_SAND_INOUT uint32  *cud2,       
    SOC_SAND_INOUT uint16  *max_size,   
    SOC_SAND_INOUT uint16  *group_size, 
    SOC_SAND_OUT   uint32  *next_entry  
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, entry_index);
    soc_mem_t mem = 0;
    uint16 found = 0, max = *max_size;
    uint8 cud2_type = 0;
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(entry_type == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); 

    if (DPP_MCDS_TYPE_IS_INGRESS(entry_type)) { 

        uint32 dest = soc_mem_field32_get(unit, (mem = IRR_MCDBm), mcdb_entry, DESTINATIONf);
        if (dest != DPP_MC_ING_DESTINATION_DISABLED) { 
            if (++found <= max) {
                dpp_add_ingress_replication(mcds, soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, OUT_LIFf), dest);
            }
        }

    } else if (DPP_MCDS_TYPE_IS_EGRESS_NORMAL(entry_type)) { 

        uint32 format_type = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, ENTRY_FORMATf);
        uint32 cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f);
        *next_entry = DPP_MC_EGRESS_LINK_PTR_END;
        switch (format_type) { 
          case 0:
            {
                uint32 port = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_1m), mcdb_entry, PP_DSP_1Af);
                if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_port_cud(mcds, core, cud, *cud2, port);
                    }
                }
                port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, PP_DSP_1Bf);
                if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_port_cud(mcds, core, cud, *cud2, port);
                    }
                }
            }
            break;

          case 1:
            {
                const uint32 bitmap = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_2m), mcdb_entry, BMP_PTRf);
                if (soc_mem_field32_get(unit, mem, mcdb_entry, NEW_FORMATf)) {
                    
                    switch ((mcdb_entry->word0 >> 18) & 3) {
                      case 1: 
                        cud2_type = DPP_MC_GROUP_2ND_CUD_EEI;
                        break;
                      case 2: 
                        cud2_type = DPP_MC_GROUP_2ND_CUD_OUTRIF;
                        break;
                      default:
                        DPP_MC_ASSERT(0);
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported format found")));
                    }
                    *cud2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f);
                    DPP_MC_ASSERT(SOC_IS_JERICHO(unit) && (mcds->info_2nd_cud == DPP_MC_GROUP_2ND_CUD_NONE || mcds->info_2nd_cud == cud2_type)); 
                    mcds->info_2nd_cud = cud2_type;
                } else if (bitmap != DPP_MC_EGR_BITMAP_DISABLED && get_bm_reps && ++found <= max) { 
                    dpp_add_egress_replication_bitmap(mcds, core, cud, *cud2, bitmap);
                }
            }
            break;

          case 3:
            *next_entry = entry_index + 1;
          case 2:
            {
                uint32 port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f);
                if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_port_cud(mcds, core, cud, *cud2, port);
                    }
                }
                port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f);
                if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_port_cud(mcds, core, soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f), *cud2, port);
                    }
                }
            }
            break;

          default:
            if (format_type < 8) { 

                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_cud(mcds, core, cud, *cud2);
                    }
                }
                cud = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_3m), mcdb_entry, OUTLIF_2f);
                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_cud(mcds, core, cud, *cud2);
                    }
                }

            } else { 

                if (format_type & 4) {
                    *next_entry = entry_index + 1;
                }
                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_cud(mcds, core, cud, *cud2);
                    }
                }
                cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f);
                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_cud(mcds, core, cud, *cud2);
                    }
                }
                cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f);
                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { 
                    if (++found <= max) {
                        dpp_add_egress_replication_cud(mcds, core, cud, *cud2);
                    }
                }
            }
        } 

#ifdef EGRESS_MC_TDM_FORMAT_SUPPORT
    } else if (DPP_MCDS_TYPE_IS_TDM(entry_type)) { 
        uint32 port = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_TDM_FORMATm), mcdb_entry, PP_DSP_1f);
        if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
            if (++found <= max) {
                dpp_add_egress_replication_cud(mcds, core, port, *cud2);
            }
        }
        port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_2f);
        if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
            if (++found <= max) {
                dpp_add_egress_replication_cud(mcds, core,  port, *cud2);
            }
        }
        port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_3f);
        if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
            if (++found <= max) {
                dpp_add_egress_replication_cud(mcds, core,  port, *cud2);
            }
        }
        port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, mcdb_entry, PP_DSP_4f);
        if (port != DPP_MULT_EGRESS_PORT_INVALID) { 
            if (++found <= max) {
                dpp_add_egress_replication_cud(mcds, core,  port, *cud2);
            }
        }
#endif 

    }

  
    if (mem) {
        *next_entry = soc_mem_field32_get(unit, mem, mcdb_entry, LINK_PTRf);
    }

    if (found < max) {
        *max_size -= found;
    } else {
        *max_size = 0;
    }
    *group_size += found;

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 jer_mult_ing_encode_entry(
    SOC_SAND_IN    int                    unit,
    SOC_SAND_IN    SOC_TMC_MULT_ING_ENTRY *ing_entry,       
    SOC_SAND_OUT   uint32                 *out_destination, 
    SOC_SAND_OUT   uint32                 *out_cud          
)
{
  uint32 destination, cud;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(ing_entry);
  SOCDNX_NULL_CHECK(out_destination);
  SOCDNX_NULL_CHECK(out_cud);

  if ((cud = ing_entry->cud) > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("CUD out of range")));
  }
  destination = ing_entry->destination.id;
  if (ing_entry->destination.type == SOC_TMC_DEST_TYPE_QUEUE) { 
    if (destination > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("queue out of range")));
    }
    destination |= JER_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW;
  } else if (ing_entry->destination.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) { 
    if (destination >= SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("system port out of range")));
    }
    destination |= JER_MC_ING_DESTINATION_FLD_TYPE_SYSPORT;
  } else if (ing_entry->destination.type == SOC_TMC_DEST_TYPE_LAG) { 
    destination |= JER_MC_ING_DESTINATION_FLD_TYPE_LAG;
  } else {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported ingress multicast destination type")));
  }
  *out_destination = destination;
  *out_cud = cud;

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 jer_convert_ingress_replication_hw2api(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       cud,            
    SOC_SAND_IN  uint32       dest,           
    SOC_SAND_OUT soc_gport_t  *port_array,    
    SOC_SAND_OUT soc_if_t     *encap_id_array 
)
{
    uint32 type_bits = dest >> JER_MC_ING_DESTINATION_SYSPORT_BITS;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (type_bits >= 4) { 
        _SHR_GPORT_UNICAST_QUEUE_GROUP_SET(*port_array, dest & JER_MC_ING_DESTINATION_ID_MASK_TM_FLOW);
    } else if (type_bits == 2) { 
            uint32 fap_id, fap_port_id;
        SOCDNX_SAND_IF_ERR_EXIT(arad_sys_phys_to_local_port_map_get(unit, dest & JER_MC_ING_DESTINATION_SYSPORT_MASK, &fap_id, &fap_port_id));
        _SHR_GPORT_MODPORT_SET(*port_array, fap_id, fap_port_id);
    } else if (type_bits == 3) { 
        _SHR_GPORT_TRUNK_SET(*port_array, dest & JER_MC_ING_DESTINATION_SYSPORT_MASK);
    } else {
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unexpected destination type")));
    }
    *encap_id_array = cud; 

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
