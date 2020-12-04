
 
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/JER/JER_PP/jer_pp_lbp_init.h>

#include <soc/dpp/drv.h>






















#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VRF          0
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NATIVE_VSI   1
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_OUTLIF       2
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_MINUS_1      3
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_INRIF        4
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI          5

#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS     3


#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB (0)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB            (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB + 1)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB + 1)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_FWD_CODE_LSB         (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB + 1)


#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif) \
         (((outlif_is_outrif) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB)) \
         | ((is_mc) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB)) \
         | ((stamp_native_vsi) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB)) \
         | ((fwd_code) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_FWD_CODE_LSB))) 



#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif) \
         (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS * SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif))



soc_error_t
soc_jer_lbp_init(SOC_SAND_IN int unit) {
    uint32
       res = SOC_SAND_OK, 
       system_header_mode; 

    
    soc_reg_above_64_val_t pph_vsi_source; 

    
    uint32 fwd_codes_IPvX_XC[4] = {ARAD_PP_FWD_CODE_IPV4_UC, ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_UC, ARAD_PP_FWD_CODE_IPV6_MC}; 
    uint32 boolean_values[2] = {0,1}; 
    uint32 fwd_codes_index, is_mc_index, outlif_is_outrif_index; 
    uint32 fwd_code; 

    uint32 value; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    SOC_REG_ABOVE_64_CLEAR(pph_vsi_source);

    


        
    res = soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr), REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf, &system_header_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO) {
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NATIVE_VSI; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                  SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                    SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 1, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                    &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                }
            }
        }
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_OUTLIF;

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
          SHR_BITCOPY_RANGE(pph_vsi_source,                                                                            
                            SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 0, 1), 
                            &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                             
        }

        if (SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0)) {
            value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VRF;
        } else {
            value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_MINUS_1;
        }


        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
          SHR_BITCOPY_RANGE(pph_vsi_source,                                                                            
                            SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 0, 0), 
                            &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                             
        }
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_INRIF; 

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
            for (outlif_is_outrif_index = 0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
              SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 1, boolean_values[outlif_is_outrif_index]), 
                                &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
            }
        }
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI; 


        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            if (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC && fwd_code != ARAD_PP_FWD_CODE_IPV4_MC
                && fwd_code != ARAD_PP_FWD_CODE_IPV6_UC && fwd_code != ARAD_PP_FWD_CODE_IPV6_MC
                && fwd_code != ARAD_PP_FWD_CODE_CPU_TRAP) {
               for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                   for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                     SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                       SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 0, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                       &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                   }
               }
            }
        }
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_INRIF; 
        for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
            for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
              SHR_BITCOPY_RANGE(pph_vsi_source,
                          SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(ARAD_PP_FWD_CODE_CPU_TRAP, 0, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]),
                          &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);
            }
        }
    } 
    else if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_ARAD) {

        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NATIVE_VSI; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                  SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                    SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 1, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                    &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                }
            }
        }



        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            if (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC && fwd_code != ARAD_PP_FWD_CODE_IPV4_MC
                && fwd_code != ARAD_PP_FWD_CODE_IPV6_UC && fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) {
               for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                   for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                     SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                       SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 0, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                       &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                   }
               }
            }
        }


        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VRF; 


        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
            for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                  SHR_BITCOPY_RANGE(pph_vsi_source,                                                                       
                                    SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index],      
                                                                                 0,  
                                                                                 boolean_values[is_mc_index],             
                                                                                 boolean_values[outlif_is_outrif_index]), 
                                    &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                        
                }
            }
        }
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHB_PPH_VSI_SOURCEr(unit, REG_PORT_ANY, pph_vsi_source)); 


    
     
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OUT_RIF_RANGEr(unit, REG_PORT_ANY, SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1)); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_lpb_init()", 0, 0); 
 
}


#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_NOF_BITS (3)  
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_NOF_BITS    (1)

#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX      ((1 << SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_NOF_BITS) - 1) 

#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE             (0)
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB    (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE + 4)
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_LSB (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB + 1)


#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_GET(fwd_offset_index, inLif_equal_0, fwd_code) \
    (((fwd_offset_index) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_LSB)) \
    | ((inLif_equal_0) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB)) \
    | ((fwd_code) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE)))




#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(fwd_offset_index, inLif_equal_0, fwd_code) \
         (ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS * SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_GET(fwd_offset_index, inLif_equal_0, fwd_code))

soc_error_t
soc_jer_php_fhei_fwd_size_init(SOC_SAND_IN int unit) {
     uint32
        res = SOC_SAND_OK,
        system_header_mode; 

     
     soc_reg_above_64_val_t pph_fhei_fwd_size; 

     
     uint32 fwd_code_index, inLif_equal_0_index, fwd_offset_index; 
     uint32 fwd_codes_Eth_IPvX_XC_Trill_Trap_Eth_After_Term[8] = {
         ARAD_PP_FWD_CODE_ETHERNET,ARAD_PP_FWD_CODE_IPV4_UC, ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_UC, 
         ARAD_PP_FWD_CODE_IPV6_MC, ARAD_PP_FWD_CODE_TRILL, ARAD_PP_FWD_CODE_CPU_TRAP, ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION}; 
     uint32 fwd_codes_IPvX_MC[2] = {ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_MC}; 
     uint32 boolean_values[2] = {0,1}; 
     uint32 fwd_code; 

     uint32 value; 

     SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

     SOC_REG_ABOVE_64_CLEAR(pph_fhei_fwd_size); 


     
     res = soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr), REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf, &system_header_mode);
     SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

     value = ARAD_IHB_PPH_FHEI_FWD_SIZE_3B; 

     for (fwd_code_index=0; fwd_code_index<8; fwd_code_index++) {
         for(inLif_equal_0_index = 0; inLif_equal_0_index < 2; inLif_equal_0_index++) {
             for (fwd_offset_index=0;fwd_offset_index <= SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX; fwd_offset_index++) {
                 SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                   SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                        fwd_offset_index,
                                        boolean_values[inLif_equal_0_index], 
                                        fwd_codes_Eth_IPvX_XC_Trill_Trap_Eth_After_Term[fwd_code_index]), 
                                   &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
             }
         }
     }
     value = ARAD_IHB_PPH_FHEI_FWD_SIZE_0B; 

     for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
         if ((fwd_code != ARAD_PP_FWD_CODE_ETHERNET) && (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC) 
             && (fwd_code != ARAD_PP_FWD_CODE_IPV4_MC) && (fwd_code != ARAD_PP_FWD_CODE_IPV6_UC)
             && (fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) && (fwd_code != ARAD_PP_FWD_CODE_TRILL)
             && (fwd_code != ARAD_PP_FWD_CODE_CPU_TRAP) && (fwd_code != ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION)) {
            for(inLif_equal_0_index = 0; inLif_equal_0_index < 2; inLif_equal_0_index++) {
                for (fwd_offset_index=0;fwd_offset_index < SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX; fwd_offset_index++) {
                    SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                      SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                         fwd_offset_index, boolean_values[inLif_equal_0_index], fwd_code), 
                                      &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
                }
            }
         }
     }
     if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO) {
         for (fwd_code_index=0;fwd_code_index<2;fwd_code_index++) {
             value = ARAD_IHB_PPH_FHEI_FWD_SIZE_8B; 
             for (inLif_equal_0_index=0;inLif_equal_0_index<2;inLif_equal_0_index++) {
                 SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                   SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                      4, boolean_values[inLif_equal_0_index], fwd_codes_IPvX_MC[fwd_code_index]), 
                                   &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
             }
             SHR_BITCOPY_RANGE(pph_fhei_fwd_size,
                                  SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                  2, 0, fwd_codes_IPvX_MC[fwd_code_index]),
                               &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS);
         }
         
         value = ARAD_IHB_PPH_FHEI_FWD_SIZE_8B; 
         for (inLif_equal_0_index = 0; inLif_equal_0_index < 2; inLif_equal_0_index++)
         {
             for (fwd_offset_index = 0; fwd_offset_index <= SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX; fwd_offset_index++)
             {
                 SHR_BITCOPY_RANGE(pph_fhei_fwd_size,
                    SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(fwd_offset_index, boolean_values[inLif_equal_0_index], 
                    ARAD_PP_FWD_CODE_MPLS), 
                    &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS);
             }
         }
     }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHB_PPH_FHEI_FWD_SIZEr(unit, REG_PORT_ANY, pph_fhei_fwd_size));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_php_fhei_fwd_size_init()", 0, 0); 
}




#include <soc/dpp/SAND/Utils/sand_footer.h>
