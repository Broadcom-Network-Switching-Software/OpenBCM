/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3




#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>





#define JER_PP_VRID_TO_IP_VERSION_PROTOCOL_MASK    (3)
#define JER_PP_VRID_TO_IP_VERSION_MASK_NOF_BITS    (2)
#define JER_PP_VRID_TO_IP_VERSION_LOWEST_INDEX     (16)
#define JER_PP_VRID_TO_IP_VERSION_HIGHEST_INDEX    (30)


#define JER_PP_MYMAC_VRRP_TCAM_PROTOCOL_GROUP_MASK_BITS (3)






#define JER_PP_VRID_TO_IP_VERSION_PROTOCOL_MASK_CREATE(_index_shift) \
            (JER_PP_VRID_TO_IP_VERSION_PROTOCOL_MASK << (_index_shift))


#define JER_PP_VRID_TO_IP_VERSION_TRAVERSE(_index_shift)                                       \
    for (_index_shift = JER_PP_VRID_TO_IP_VERSION_LOWEST_INDEX ;                               \
                _index_shift <= JER_PP_VRID_TO_IP_VERSION_HIGHEST_INDEX ;                      \
                            _index_shift += JER_PP_VRID_TO_IP_VERSION_MASK_NOF_BITS)  


#define JER_PP_VRID_TO_IP_VERSION_INDEX_IS_IN_GROUP(_reg_val, _index_shift, _group)                  \
            (JER_PP_VRID_TO_IP_VERSION_GET_GROUP_BY_INDEX((_reg_val), (_index_shift)) == (_group))


#define JER_PP_VRID_TO_IP_VERSION_GET_GROUP_BY_INDEX(_reg_val, _index_shift)                           \
            ((_reg_val & JER_PP_VRID_TO_IP_VERSION_PROTOCOL_MASK_CREATE(_index_shift)) >> _index_shift)


#define JER_PP_VRID_TO_IP_VERSION_RESET_PROTOCOL(_reg_val, _index_shift)                      \
            ((_reg_val) &= ~JER_PP_VRID_TO_IP_VERSION_PROTOCOL_MASK_CREATE(_index_shift));


#define MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PARSED_L2_NEXT_PROTOCOL(_index_shift) \
            ((_index_shift) / JER_PP_VRID_TO_IP_VERSION_MASK_NOF_BITS)


#define MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PROTOCOL_FLAG(_index_shift) \
    ((_index_shift % JER_PP_VRID_TO_IP_VERSION_MASK_NOF_BITS == 0) ?    \
    SOC_PPC_PARSED_L2_NEXT_PROTOCOL_TO_L3_PROTOCOL_FLAG(MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PARSED_L2_NEXT_PROTOCOL(_index_shift))    \
    :0)



















#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS 2 

#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_MY_MAC_LSB         (0)
#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_SKIP_ETHERNET_LSB  (SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_MY_MAC_LSB         + 1)
#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_ROUTING_ENABLE_LSB (SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_SKIP_ETHERNET_LSB  + 1)
#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_COMPATIBLE_MC_LSB  (SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_ROUTING_ENABLE_LSB + 1)
#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_PTC_PROFILE_LSB    (SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_COMPATIBLE_MC_LSB  + 1)


#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INDEX_GET(ptc_profile, compatible_mc, routing_enable, skip_ethernet, my_mac) \
    (((my_mac) << SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_MY_MAC_LSB)                   \
     | ((skip_ethernet) << SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_SKIP_ETHERNET_LSB)   \
     | ((routing_enable) << SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_ROUTING_ENABLE_LSB) \
     | ((compatible_mc) << SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_COMPATIBLE_MC_LSB)   \
     | ((ptc_profile) << SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_PTC_PROFILE_LSB)       \
    )

#define SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, compatible_mc, routing_enable, skip_ethernet, my_mac) \
    (SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS * SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INDEX_GET(ptc_profile, compatible_mc, routing_enable, skip_ethernet, my_mac))





#define SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_IS_MC_LSB          (0)
#define SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_NEXT_PROTOCOL_LSB  (SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_IS_MC_LSB +1)

#define SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_INDEX_GET(next_protocol, is_mc) \
    (((is_mc) << SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_IS_MC_LSB) \
     | ((next_protocol) << SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_NEXT_PROTOCOL_LSB) \
    )

 
#define SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(next_protocol, is_mc) \
     (ARAD_PP_FWD_CODE_NOF_BITS * SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_INDEX_GET(next_protocol, is_mc))


soc_error_t 
soc_jer_mymac_2nd_mymac_init(int unit){
    int rv;
    soc_reg_above_64_val_t
        reg_above64_val;
    uint32 value; 
     
    int ptc_profile; 
    int is_skip_ethernet; 
    int is_my_mac; 
    int fwd_code; 
    int is_mc; 

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    


    for (ptc_profile=0; ptc_profile<ARAD_PORTS_NOF_FLP_PROFILES; ptc_profile++) {
        for (is_skip_ethernet=0; is_skip_ethernet<2; is_skip_ethernet++) {
            
            value = 1; 
            SHR_BITCOPY_RANGE(reg_above64_val, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 0, 0, is_skip_ethernet, 1), 
                              &value, 
                              0, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                              ); 

            
            value = 0; 
            SHR_BITCOPY_RANGE(reg_above64_val, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 0, 0, is_skip_ethernet, 0), 
                              &value, 
                              0, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                              ); 

            
            value = 0; 
            SHR_BITCOPY_RANGE(reg_above64_val, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 0, 1, is_skip_ethernet, 0), 
                              &value, 
                              0, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                              ); 

            
            value = 2; 
            SHR_BITCOPY_RANGE(reg_above64_val, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 0, 1, is_skip_ethernet, 1), 
                              &value, 
                              0, 
                              SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                              ); 

            
            value = 0; 
            for (is_my_mac=0; is_my_mac<2; is_my_mac++) {
                SHR_BITCOPY_RANGE(reg_above64_val, 
                                  SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 1, 0, is_skip_ethernet, is_my_mac), 
                                  &value, 
                                  0, 
                                  SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                                  ); 

            }

            
            value = 2; 
            for (is_my_mac=0; is_my_mac<2; is_my_mac++) {
                SHR_BITCOPY_RANGE(reg_above64_val, 
                                  SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_OFFSET_GET(ptc_profile, 1, 1, is_skip_ethernet, is_my_mac), 
                                  &value, 
                                  0, 
                                  SOC_JER_PP_MYMAC_INNER_ETHERNET_TERMINTATION_ACTION_INNER_ETHERNET_TERMINATION_NOF_BITS
                                  ); 
            }
        }
    }

    rv = WRITE_IHP_VTT_INNER_ETHERNET_TERMINATION_ACTIONr(unit, SOC_CORE_ALL, reg_above64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    rv = WRITE_IHP_INNER_ETHERNET_TRAP_ACTION_PROFILE_VECTORr(unit, SOC_CORE_ALL, reg_above64_val);
    SOCDNX_IF_ERR_EXIT(rv);


    

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    value = ARAD_PP_FWD_CODE_IPV4_UC; 

    
    for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
        for (is_mc=0;is_mc<2; is_mc++) {
            SHR_BITCOPY_RANGE(reg_above64_val, 
                              SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(fwd_code, is_mc), 
                              &value, 
                              0, 
                              ARAD_PP_FWD_CODE_NOF_BITS
                              ); 
        }
    }

    
    SHR_BITCOPY_RANGE(reg_above64_val, 
                      SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(SOC_PPC_FP_PARSED_ETHERTYPE_IPV4, 0), 
                      &value, 
                      0, 
                      ARAD_PP_FWD_CODE_NOF_BITS
                      ); 

    
    value = ARAD_PP_FWD_CODE_IPV4_MC; 
    SHR_BITCOPY_RANGE(reg_above64_val, 
                      SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(SOC_PPC_FP_PARSED_ETHERTYPE_IPV4 , 1), 
                      &value, 
                      0, 
                      ARAD_PP_FWD_CODE_NOF_BITS
                      ); 
    
    value = ARAD_PP_FWD_CODE_IPV6_UC; 
    SHR_BITCOPY_RANGE(reg_above64_val, 
                      SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(SOC_PPC_FP_PARSED_ETHERTYPE_IPV6, 0), 
                      &value, 
                      0, 
                      ARAD_PP_FWD_CODE_NOF_BITS
                      ); 

    
    value = ARAD_PP_FWD_CODE_IPV6_MC; 
    SHR_BITCOPY_RANGE(reg_above64_val, 
                      SOC_JER_PP_MYMAC_MAP_NEXT_PROTOCOL_TO_FWD_CODE_OFFSET_GET(SOC_PPC_FP_PARSED_ETHERTYPE_IPV6, 1), 
                      &value, 
                      0, 
                      ARAD_PP_FWD_CODE_NOF_BITS
                      ); 
    



    rv = WRITE_IHP_MAP_NEXT_PROTOCOL_TO_FWD_CODEr(unit, SOC_CORE_ALL, reg_above64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_jer_mymac_protocol_group_set(int unit, uint32 protocols, uint32 group){
    int rv;
    uint32 reg_val;
    int index_shift;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (group >= SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Given protocol group too high")));
    }

    
    rv = READ_IHP_MAP_VRID_TO_IP_VERSIONr(unit, SOC_CORE_ALL, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    JER_PP_VRID_TO_IP_VERSION_TRAVERSE(index_shift) {

        
        if (JER_PP_VRID_TO_IP_VERSION_INDEX_IS_IN_GROUP(reg_val, index_shift, group)) {
            JER_PP_VRID_TO_IP_VERSION_RESET_PROTOCOL(reg_val, index_shift);
        }

        
        if (MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PROTOCOL_FLAG(index_shift) & protocols) {
            JER_PP_VRID_TO_IP_VERSION_RESET_PROTOCOL(reg_val, index_shift);
            reg_val |= group << index_shift;
        }
    }

    
    rv = WRITE_IHP_MAP_VRID_TO_IP_VERSIONr(unit, SOC_CORE_ALL, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_EXIT;
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_jer_mymac_protocol_group_get_protocol_by_group(int unit, uint32 group, uint32 *protocols){
    int rv;
    uint32 reg_val;
    int index_shift;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (group >= SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Given protocol group too high")));
    }

    SOCDNX_NULL_CHECK(protocols);


    
    rv = READ_IHP_MAP_VRID_TO_IP_VERSIONr(unit, SOC_CORE_ALL, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);


    
    JER_PP_VRID_TO_IP_VERSION_TRAVERSE(index_shift) {

        
        if (JER_PP_VRID_TO_IP_VERSION_INDEX_IS_IN_GROUP(reg_val, index_shift, group)) {
            *protocols |= MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PROTOCOL_FLAG(index_shift);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_jer_mymac_protocol_group_get_group_by_protocols(int unit, uint32 protocols, uint32 *group){
    int rv;
    uint32 reg_val;
    uint32 curr_group;
    int index_shift;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (protocols == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("No protocols given")));
    }

    SOCDNX_NULL_CHECK(group);


    
    rv = READ_IHP_MAP_VRID_TO_IP_VERSIONr(unit, SOC_CORE_ALL, &reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    *group = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID;

    
    JER_PP_VRID_TO_IP_VERSION_TRAVERSE(index_shift) {

        
        if (MYMAC_VRID_TO_IP_VERSION_INDEX_TO_PROTOCOL_FLAG(index_shift) & protocols) {
            curr_group = JER_PP_VRID_TO_IP_VERSION_GET_GROUP_BY_INDEX(reg_val,index_shift);

            
            if (*group < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) && curr_group != *group) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("Given protocols are in more than one group")));
            } else {
                *group = curr_group;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_mymac_vrrp_tcam_info_set(int unit, SOC_PPC_VRRP_CAM_INFO *info){
    int rv;
    SOC_SAND_PP_MAC_ADDRESS not_mac_addr;
    uint32
      mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] = {0},
      data[4] = {0},
      entry_offset = info->vrrp_cam_index,
      not_protocol_group_mask = 0,
      i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    rv = soc_sand_pp_mac_address_struct_to_long(&info->mac_addr, mac_in_longs);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    soc_IHP_VRID_MY_MAC_TCAMm_field_set(unit, data, DAf, mac_in_longs);
    soc_IHP_VRID_MY_MAC_TCAMm_field32_set(unit, data, IP_VERSIONf, info->protocol_group);

    


    sal_memset(&not_mac_addr, 0, sizeof(not_mac_addr));

    
    for (i = 0 ; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8 ; i++) {
        not_mac_addr.address[i] = ~info->mac_mask.address[i];
    }

    
    not_protocol_group_mask = ~(info->protocol_group_mask) & JER_PP_MYMAC_VRRP_TCAM_PROTOCOL_GROUP_MASK_BITS;

    rv = soc_sand_pp_mac_address_struct_to_long(&not_mac_addr, mac_in_longs);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    soc_IHP_VRID_MY_MAC_TCAMm_field_set(unit, data, DA_MASKf, mac_in_longs);
    soc_IHP_VRID_MY_MAC_TCAMm_field32_set(unit, data, IP_VERSION_MASKf, not_protocol_group_mask); 

    
    soc_IHP_VRID_MY_MAC_TCAMm_field32_set(unit, data, VALIDf, 1);      

    soc_IHP_VRID_MY_MAC_TCAMm_field32_set(unit, data, INDEXf, entry_offset);

    
    rv = WRITE_IHP_VRID_MY_MAC_TCAMm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_mymac_vrrp_tcam_info_delete(int unit, uint8 cam_index){
    int rv;
    uint32
      empty_data[4] = {0},
      entry_offset = cam_index;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = WRITE_IHP_VRID_MY_MAC_TCAMm(unit, MEM_BLOCK_ANY, entry_offset, empty_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>



