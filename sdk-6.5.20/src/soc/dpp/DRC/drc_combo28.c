/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM

 
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/mbcm.h>

 
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_dram.h>

 
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>
#include <soc/dpp/DRC/drc_combo28_init.h>

 
#include <shared/swstate/access/sw_state_access.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif


#define DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY   SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE*SHMOO_COMBO28_BYTE
#define DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE 256
#define DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE 256
#define DRC_COMBO28_VAL_STR_BUFFER_SIZE 1024
#define DRC_COMBO28_DDR_PHY_REF_CLK 100
#define DRC_COMBO28_DYNAMIC_CALIBRATION_CYCLES_PER_ITERATION 1
#define SAMAR_DRAM_INTERFACES_NUM_MAX 5


#define DRC_COMBO28_TRANSLATE_DRAM_TEMPERATURE_VALUE_ACCORDING_TO_JEDEC_SPEC(val) (-40 + (val) * 2)


#define SWSTATE_JER_TM_ACCESS   sw_state_access[unit].dpp.soc.jericho.tm


#define MAX_NOF_BITS_GET(number, max_nof_bits)              for((max_nof_bits) = 0; (number) != 0; ++(max_nof_bits), (number) >>=1); \
                                                            if ((max_nof_bits != 0)) { --(max_nof_bits); }

#ifdef DNX_TEST_BOARD
#define MAX_NOF_HW_DRAM_INTERFACES(unit) (SOC_IS_DNX_TEST_DEVICE(unit) ? SAMAR_DRAM_INTERFACES_NUM_MAX : \
    SOC_DPP_DEFS_GET((unit), hw_dram_interfaces_max))
#else
#define MAX_NOF_HW_DRAM_INTERFACES(unit) SOC_DPP_DEFS_GET((unit), hw_dram_interfaces_max)
#endif



int soc_dpp_drc_combo28_util_calm_shell_bits_swap(int unit, uint32 in_buf, uint32 *out_buf)
{
    uint32 shadow_mr_addr_lcl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(out_buf);
    
    shadow_mr_addr_lcl  = 0x0;
    
    if (SOC_IS_ARDON(unit)) {
        shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 0, 0), 0,  0);    
        shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 1, 1), 1,  1);    
    } else {
        shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 1, 1), 0,  0);    
        shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 0, 0), 1,  1);    
    }
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 2, 2), 2,  2);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 4, 4), 3,  3);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 3, 3), 4,  4);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 6, 6), 5,  5);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 5, 5), 6,  6);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 8, 8), 7,  7);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 7, 7), 8,  8);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 9, 9), 9,  9);    
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 10, 10), 10, 10); 
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 13, 13), 11, 11); 
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 12, 12), 12, 12); 
    shadow_mr_addr_lcl |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(in_buf, 11, 11), 13, 13); 

    *out_buf = shadow_mr_addr_lcl;

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): mr=%d, *shadow_mr_addr=%d.\n"),FUNCTION_NAME(), in_buf, *out_buf));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_dram_cpu_command(int unit, int drc_ndx, uint32 ras_n, uint32 cas_n, uint32 we_n, uint32 act_n, uint32 bank, uint32 address)
{
    uint32 
        abi_n=0,
        abi_n_odd=0,
        parity,
        addr,
        addr_odd,
        bank_odd,
        bus_for_abi,
        parity_bus,
        dram_type,
        field_data,
        clam_shell_mode,
        i;
    uint64 
        reg64_val,
        field64_data,
        field64_val;
    soc_reg_above_64_val_t reg_above_64;
    int cnt;
    static soc_reg_t
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr},
        DRC_CPU_COMMANDSl[] = {DRCA_CPU_COMMANDSr, DRCB_CPU_COMMANDSr, DRCC_CPU_COMMANDSr, 
                               DRCD_CPU_COMMANDSr, DRCE_CPU_COMMANDSr, DRCF_CPU_COMMANDSr, 
                               DRCG_CPU_COMMANDSr, DRCH_CPU_COMMANDSr},
        DRC_CLAM_SHELLl[] = {DRCA_CLAM_SHELLr, DRCB_CLAM_SHELLr, DRCC_CLAM_SHELLr,
                             DRCD_CLAM_SHELLr, DRCE_CLAM_SHELLr, DRCF_CLAM_SHELLr,
                             DRCG_CLAM_SHELLr, DRCH_CLAM_SHELLr},
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr,
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, ras_n=%d, cas_n=%d, we_n=%d, act_n=%d, bank=0x%x, address=0x%x\n"), 
                            FUNCTION_NAME(), drc_ndx, ras_n, cas_n, we_n, act_n, bank, address)); 

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);
    if (SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4 == dram_type) {
        addr = SOC_SAND_SET_BITS_RANGE(address, 13, 0);

        
        soc_dpp_drc_combo28_util_calm_shell_bits_swap(unit, addr, &addr_odd);

        bank_odd = bank;
        bank_odd &= SOC_SAND_ZERO_BITS_MASK(1,0);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(bank, 0, 0), 1,  1);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(bank, 1, 1), 0,  0);

    } else {
        addr = SOC_SAND_GET_BITS_RANGE(address, 14, 8);
  
        addr_odd = 0;
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 7, 7), 0,  0);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 1, 1), 1,  1);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 0, 0), 2,  2);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 6, 6), 3,  3);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 13, 13), 4,  4);
        addr_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 15, 15), 5,  5);

        bank_odd = 0;
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 2, 2), 0,  0);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 5, 5), 1,  1);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 4, 4), 2,  2);
        bank_odd |= SOC_SAND_SET_BITS_RANGE(SOC_SAND_GET_BITS_RANGE(address, 3, 3), 3,  3);

        
        cnt = 0;
        abi_n = 1;
        bus_for_abi = 0;
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(addr, 13, 0);
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(bank, 17, 14);

        SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
        field64_val = soc_reg64_field_get(unit, DRCA_DRAM_SPECIAL_FEATURESr, reg64_val, ABIf);
        field_data = COMPILER_64_LO(field64_val);

        for (i = 0; i < 10; i++) {
            cnt = cnt + SOC_SAND_GET_BITS_RANGE(bus_for_abi,i ,i);
        }
        if ((cnt < 5) && (1 == field_data)) {
            addr = SOC_SAND_GET_BITS_RANGE(~addr, 13, 0);
            bank = SOC_SAND_GET_BITS_RANGE(~bank, 3, 0);
            abi_n = 0;
        }

        
        cnt = 0;
        abi_n_odd = 1;
        bus_for_abi = 0;
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(addr_odd, 13, 0);
        bus_for_abi |= SOC_SAND_SET_BITS_RANGE(bank_odd, 17, 14);
        for (i = 0; i < 10; i++) {
            cnt = cnt + SOC_SAND_GET_BITS_RANGE(bus_for_abi,i ,i);
        }
        if ((cnt < 5) && (1 == field_data)) {
            addr_odd = SOC_SAND_GET_BITS_RANGE(~addr_odd, 13, 0);
            bank_odd = SOC_SAND_GET_BITS_RANGE(~bank_odd, 3, 0);
            abi_n_odd = 0;
        } 
    }

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): addr=0x%x, addr_odd=0x%x, bank_odd=0x%x, abi_n_odd=%d, addr=0x%x, bank=0x%x, abi_n=%d\n"), 
                            FUNCTION_NAME(), addr, addr_odd, bank_odd, abi_n_odd, addr, bank, abi_n)); 

    
    parity_bus = 0;
    parity_bus |= SOC_SAND_SET_BITS_RANGE(ras_n, 0, 0);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(cas_n, 1, 1);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(we_n, 2, 2);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(act_n, 3, 3);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(addr, 17, 4);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(bank, 21, 18);
    parity = 0;
    for (i=0 ; i < 23 ; i++) {
        parity ^= SOC_SAND_GET_BITS_RANGE(parity_bus, i ,i);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, BANK_ADDRESSf, bank);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ADDRESSf, addr);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_Nf, we_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_Nf,  cas_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_Nf, ras_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, PARITYf,  parity);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_Nf, ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? act_n : abi_n));
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, BANK_ADDRESS_ODDf,  bank_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ADDRESS_ODDf,  addr_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_N_ODDf,  we_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_N_ODDf,  cas_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_N_ODDf,  ras_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, PARITY_ODDf,  parity);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_N_ODDf,  ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? act_n : abi_n_odd));

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_CLAM_SHELLl[drc_ndx], REG_PORT_ANY, 0, &field_data));
    clam_shell_mode = soc_reg_field_get(unit, DRCA_CLAM_SHELLr, field_data, CLAM_SHELL_MODEf);
    if (1 == clam_shell_mode) {
        field_data = soc_reg_field_get(unit, DRCA_CLAM_SHELLr, field_data, USE_CLAM_SHELL_DDR_0f);
        soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_Nf, ((0x1 == field_data) ? 0x1 : 0x2));
        soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_N_ODDf, ((0x1 == field_data) ? 0x2 : 0x1));
    } else { 
        
        soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_Nf, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &field64_data));
        field_data = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, field64_data, DRC_IN_HALF_DRAM_CMD_FREQ_MODEf);
        soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_N_ODDf, ((0x1 == field_data) ? 0x3 : 0x0));
    }

    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CKEf, ((dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) ? 0x1 : 0x0));
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RESf, 0x1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODTf, 0x0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODT_ODDf, 0x0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CONST_VALf, 0x0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CMD_CONST_VALf, 0x0);

     
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));

    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CPU_CMD_VLDf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
  
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CPU_CMD_VLDf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 

    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Sent CPU command to DRAM.\n"), FUNCTION_NAME(), drc_ndx));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_load_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 mrs_opcode)
{
    int 
        address,
        rv;
    uint32 reg_val, field_val;
    static soc_reg_t
        DRC_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
            {{DRCA_DRAM_MR_0r, DRCB_DRAM_MR_0r, DRCC_DRAM_MR_0r, DRCD_DRAM_MR_0r, DRCE_DRAM_MR_0r, DRCF_DRAM_MR_0r, DRCG_DRAM_MR_0r, DRCH_DRAM_MR_0r},
             {DRCA_DRAM_MR_1r, DRCB_DRAM_MR_1r, DRCC_DRAM_MR_1r, DRCD_DRAM_MR_1r, DRCE_DRAM_MR_1r, DRCF_DRAM_MR_1r, DRCG_DRAM_MR_1r, DRCH_DRAM_MR_1r},
             {DRCA_DRAM_MR_2r, DRCB_DRAM_MR_2r, DRCC_DRAM_MR_2r, DRCD_DRAM_MR_2r, DRCE_DRAM_MR_2r, DRCF_DRAM_MR_2r, DRCG_DRAM_MR_2r, DRCH_DRAM_MR_2r},
             {DRCA_DRAM_MR_3r, DRCB_DRAM_MR_3r, DRCC_DRAM_MR_3r, DRCD_DRAM_MR_3r, DRCE_DRAM_MR_3r, DRCF_DRAM_MR_3r, DRCG_DRAM_MR_3r, DRCH_DRAM_MR_3r},
             {DRCA_DRAM_MR_4r, DRCB_DRAM_MR_4r, DRCC_DRAM_MR_4r, DRCD_DRAM_MR_4r, DRCE_DRAM_MR_4r, DRCF_DRAM_MR_4r, DRCG_DRAM_MR_4r, DRCH_DRAM_MR_4r},
             {DRCA_DRAM_MR_5r, DRCB_DRAM_MR_5r, DRCC_DRAM_MR_5r, DRCD_DRAM_MR_5r, DRCE_DRAM_MR_5r, DRCF_DRAM_MR_5r, DRCG_DRAM_MR_5r, DRCH_DRAM_MR_5r},
             {DRCA_DRAM_MR_6r, DRCB_DRAM_MR_6r, DRCC_DRAM_MR_6r, DRCD_DRAM_MR_6r, DRCE_DRAM_MR_6r, DRCF_DRAM_MR_6r, DRCG_DRAM_MR_6r, DRCH_DRAM_MR_6r},
             {DRCA_DRAM_MR_7r, DRCB_DRAM_MR_7r, DRCC_DRAM_MR_7r, DRCD_DRAM_MR_7r, DRCE_DRAM_MR_7r, DRCF_DRAM_MR_7r, DRCG_DRAM_MR_7r, DRCH_DRAM_MR_7r},
             {DRCA_DRAM_MR_8r, DRCB_DRAM_MR_8r, DRCC_DRAM_MR_8r, DRCD_DRAM_MR_8r, DRCE_DRAM_MR_8r, DRCF_DRAM_MR_8r, DRCG_DRAM_MR_8r, DRCH_DRAM_MR_8r},
             {DRCA_DRAM_MR_9r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_10r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_11r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_12r, DRCB_DRAM_MR_12r, DRCC_DRAM_MR_12r, DRCD_DRAM_MR_12r, DRCE_DRAM_MR_12r, DRCF_DRAM_MR_12r, DRCG_DRAM_MR_12r, DRCH_DRAM_MR_12r},
             {DRCA_DRAM_MR_13r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_DRAM_MR_14r, DRCB_DRAM_MR_14r, DRCC_DRAM_MR_14r, DRCD_DRAM_MR_14r, DRCE_DRAM_MR_14r, DRCF_DRAM_MR_14r, DRCG_DRAM_MR_14r, DRCH_DRAM_MR_14r},
             {DRCA_DRAM_MR_15r, DRCB_DRAM_MR_15r, DRCC_DRAM_MR_15r, DRCD_DRAM_MR_15r, DRCE_DRAM_MR_15r, DRCF_DRAM_MR_15r, DRCG_DRAM_MR_15r, DRCH_DRAM_MR_15r}},
        DRC_SHADOW_DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = 
             {{DRCA_SHADOW_DRAM_MR_0r, DRCB_SHADOW_DRAM_MR_0r, DRCC_SHADOW_DRAM_MR_0r, DRCD_SHADOW_DRAM_MR_0r, DRCE_SHADOW_DRAM_MR_0r, DRCF_SHADOW_DRAM_MR_0r, DRCG_SHADOW_DRAM_MR_0r, DRCH_SHADOW_DRAM_MR_0r},
             {DRCA_SHADOW_DRAM_MR_1r, DRCB_SHADOW_DRAM_MR_1r, DRCC_SHADOW_DRAM_MR_1r, DRCD_SHADOW_DRAM_MR_1r, DRCE_SHADOW_DRAM_MR_1r, DRCF_SHADOW_DRAM_MR_1r, DRCG_SHADOW_DRAM_MR_1r, DRCH_SHADOW_DRAM_MR_1r},
             {DRCA_SHADOW_DRAM_MR_2r, DRCB_SHADOW_DRAM_MR_2r, DRCC_SHADOW_DRAM_MR_2r, DRCD_SHADOW_DRAM_MR_2r, DRCE_SHADOW_DRAM_MR_2r, DRCF_SHADOW_DRAM_MR_2r, DRCG_SHADOW_DRAM_MR_2r, DRCH_SHADOW_DRAM_MR_2r},
             {DRCA_SHADOW_DRAM_MR_3r, DRCB_SHADOW_DRAM_MR_3r, DRCC_SHADOW_DRAM_MR_3r, DRCD_SHADOW_DRAM_MR_3r, DRCE_SHADOW_DRAM_MR_3r, DRCF_SHADOW_DRAM_MR_3r, DRCG_SHADOW_DRAM_MR_3r, DRCH_SHADOW_DRAM_MR_3r},
             {DRCA_SHADOW_DRAM_MR_4r, DRCB_SHADOW_DRAM_MR_4r, DRCC_SHADOW_DRAM_MR_4r, DRCD_SHADOW_DRAM_MR_4r, DRCE_SHADOW_DRAM_MR_4r, DRCF_SHADOW_DRAM_MR_4r, DRCG_SHADOW_DRAM_MR_4r, DRCH_SHADOW_DRAM_MR_4r},
             {DRCA_SHADOW_DRAM_MR_5r, DRCB_SHADOW_DRAM_MR_5r, DRCC_SHADOW_DRAM_MR_5r, DRCD_SHADOW_DRAM_MR_5r, DRCE_SHADOW_DRAM_MR_5r, DRCF_SHADOW_DRAM_MR_5r, DRCG_SHADOW_DRAM_MR_5r, DRCH_SHADOW_DRAM_MR_5r},
             {DRCA_SHADOW_DRAM_MR_6r, DRCB_SHADOW_DRAM_MR_6r, DRCC_SHADOW_DRAM_MR_6r, DRCD_SHADOW_DRAM_MR_6r, DRCE_SHADOW_DRAM_MR_6r, DRCF_SHADOW_DRAM_MR_6r, DRCG_SHADOW_DRAM_MR_6r, DRCH_SHADOW_DRAM_MR_6r},
             {DRCA_SHADOW_DRAM_MR_7r, DRCB_SHADOW_DRAM_MR_7r, DRCC_SHADOW_DRAM_MR_7r, DRCD_SHADOW_DRAM_MR_7r, DRCE_SHADOW_DRAM_MR_7r, DRCF_SHADOW_DRAM_MR_7r, DRCG_SHADOW_DRAM_MR_7r, DRCH_SHADOW_DRAM_MR_7r},
             {DRCA_SHADOW_DRAM_MR_8r, DRCB_SHADOW_DRAM_MR_8r, DRCC_SHADOW_DRAM_MR_8r, DRCD_SHADOW_DRAM_MR_8r, DRCE_SHADOW_DRAM_MR_8r, DRCF_SHADOW_DRAM_MR_8r, DRCG_SHADOW_DRAM_MR_8r, DRCH_SHADOW_DRAM_MR_8r},
             {DRCA_SHADOW_DRAM_MR_9r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_10r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_11r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_12r, DRCB_SHADOW_DRAM_MR_12r, DRCC_SHADOW_DRAM_MR_12r, DRCD_SHADOW_DRAM_MR_12r, DRCE_SHADOW_DRAM_MR_12r, DRCF_SHADOW_DRAM_MR_12r, DRCG_SHADOW_DRAM_MR_12r, DRCH_SHADOW_DRAM_MR_12r},
             {DRCA_SHADOW_DRAM_MR_13r, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr, INVALIDr},
             {DRCA_SHADOW_DRAM_MR_14r, DRCB_SHADOW_DRAM_MR_14r, DRCC_SHADOW_DRAM_MR_14r, DRCD_SHADOW_DRAM_MR_14r, DRCE_SHADOW_DRAM_MR_14r, DRCF_SHADOW_DRAM_MR_14r, DRCG_SHADOW_DRAM_MR_14r, DRCH_SHADOW_DRAM_MR_14r},
             {DRCA_SHADOW_DRAM_MR_15r, DRCB_SHADOW_DRAM_MR_15r, DRCC_SHADOW_DRAM_MR_15r, DRCD_SHADOW_DRAM_MR_15r, DRCE_SHADOW_DRAM_MR_15r, DRCF_SHADOW_DRAM_MR_15r, DRCG_SHADOW_DRAM_MR_15r, DRCH_SHADOW_DRAM_MR_15r}};
    static soc_field_t DRAM_MRl[SOC_DPP_DRC_COMBO28_MRS_NUM_MAX] = 
         {DRAM_MR_0f, DRAM_MR_1f, DRAM_MR_2f, DRAM_MR_3f, DRAM_MR_4f, DRAM_MR_5f, DRAM_MR_6f, DRAM_MR_7f,
          DRAM_MR_8f, DRAM_MR_9f, DRAM_MR_10f, DRAM_MR_11f, DRAM_MR_12f, DRAM_MR_13f, DRAM_MR_14f, DRAM_MR_15f};

    static soc_field_t 
         SHADOW_DRAM_MR_ADDRl[] = {SHADOW_DRAM_MR_0_ADDRf,  SHADOW_DRAM_MR_1_ADDRf,  SHADOW_DRAM_MR_2_ADDRf,  SHADOW_DRAM_MR_3_ADDRf, 
                                   SHADOW_DRAM_MR_4_ADDRf,  SHADOW_DRAM_MR_5_ADDRf,  SHADOW_DRAM_MR_6_ADDRf,  SHADOW_DRAM_MR_7_ADDRf, 
                                   SHADOW_DRAM_MR_8_ADDRf,  SHADOW_DRAM_MR_9_ADDRf,  SHADOW_DRAM_MR_10_ADDRf, SHADOW_DRAM_MR_11_ADDRf, 
                                   SHADOW_DRAM_MR_12_ADDRf, SHADOW_DRAM_MR_13_ADDRf, SHADOW_DRAM_MR_14_ADDRf, SHADOW_DRAM_MR_15_ADDRf};

    SOCDNX_INIT_FUNC_DEFS;

    if ((mr_ndx == 9) || (mr_ndx == 10) || (mr_ndx == 11) || (mr_ndx == 13)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mr_ndx=%d is not used.\n"), mr_ndx));
    }
    
    if ((mrs_opcode > 0x3ffff)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mrs_opcode=0x%x are out 18bit size.\n"), mrs_opcode));
    }
    
    address = SOC_SAND_GET_BITS_RANGE(mrs_opcode, 15, 0);
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): drc_ndx=%d, Load MRS %d with opcode 0x%x, address=0x%x.\n"), FUNCTION_NAME(), drc_ndx, mr_ndx, mrs_opcode, address));

    
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], &reg_val, DRAM_MRl[mr_ndx], mrs_opcode);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg_val));
    
    
    rv = soc_dpp_drc_combo28_util_calm_shell_bits_swap(unit, mrs_opcode, &field_val);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], &reg_val, SHADOW_DRAM_MR_ADDRl[mr_ndx], field_val); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_SHADOW_DRAM_MRl[mr_ndx][drc_ndx], REG_PORT_ANY, 0, reg_val));     

    
    rv = soc_dpp_drc_combo28_dram_cpu_command(unit, drc_ndx, 0 , 0 , 0 , 1 , mr_ndx , address );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}




int soc_dpp_drc_combo28_info_config_default(int unit, soc_dpp_drc_combo28_info_t *drc_info)
{
    int max_nof_dram_interfaces;
    SOCDNX_INIT_FUNC_DEFS;

    max_nof_dram_interfaces = MAX_NOF_HW_DRAM_INTERFACES(unit);

    
    SOCDNX_NULL_CHECK(drc_info);

    
    sal_memset(drc_info, 0x0, sizeof (soc_dpp_drc_combo28_info_t));

    SHR_BITCLR_RANGE(drc_info->dram_bitmap, 0, max_nof_dram_interfaces);

    SHR_BITCLR_RANGE(drc_info->ref_clk_bitmap, 0, max_nof_dram_interfaces);

exit:
    SOCDNX_FUNC_RETURN;
}

int _soc_dpp_drc_combo28_str_prop_ext_ram_type(int unit, uint32 *dram_type)
{
    int rv = SOC_E_NONE;
    char 
        *propkey, 
        *propval;

    propkey = spn_EXT_RAM_TYPE;
    propval = soc_property_get_str(unit, propkey);    

    if (propval == NULL) {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Missing property Key %s\n"), propkey));
        return SOC_E_FAIL;
    }

    if (sal_strcmp(propval, "DDR4") == 0) {
        *dram_type = SHMOO_COMBO28_DRAM_TYPE_DDR4;
    } else if (sal_strcmp(propval, "GDDR5") == 0) {
        *dram_type = SHMOO_COMBO28_DRAM_TYPE_GDDR5;      
    } else {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
        return SOC_E_FAIL;
    }

    return rv;
}

int _soc_dpp_drc_combo28_dram_param_get(int unit, char *prop, uint32 *p_val)
{
    char 
      *propval, 
      *s;
    int val;

    SOCDNX_INIT_FUNC_DEFS;

    propval = soc_property_get_str(unit, prop);

    if (propval) {
        val = sal_ctoi(propval, &s);
        if ((*s == 'c') && (*(s+1) == '\0')) {
            
             *p_val = val | SOC_SAND_BIT(SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT);
        } else if (*s == '\0') {
            
            *p_val = val;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Property %s is in a unknown format. Should be a number, or a number followed by c (e.g. 1000c)\n"), prop));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_check_swap(int unit, int nof_items, uint32 data_array[])
{
    uint32 values_bitmap, cells_bitmap;
    int item, nof_allowed_items = sizeof(uint32) * SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE;
    SOCDNX_INIT_FUNC_DEFS;

    
    if(nof_items > nof_allowed_items) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("%s - number of items to check (%d) is greater than allowed number (%d)\n"), FUNCTION_NAME(), nof_items, nof_allowed_items));  
    }

    
    values_bitmap = 0;
    cells_bitmap = 0;

    
    for( item = 0; item < nof_items; ++item) {
        if(data_array[item] != item) {
            values_bitmap |= 1 << data_array[item];
            cells_bitmap  |= 1 << item;
        }
    }

    
    if( (values_bitmap ^ cells_bitmap) != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("\nSwap Doesn't make sence. swapped cells(BitMap): 0x%x\t swapped values(BitMap): 0x%x should match\n"), cells_bitmap, values_bitmap));
    }

exit:
    SOCDNX_FUNC_RETURN;        
}


int _soc_dpp_drc_combo28_check_ram_dq_swap(int unit, int drc_ndx, uint32 dq_swap[8][4][8])
{
    int byte;

    SOCDNX_INIT_FUNC_DEFS;
    for(byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; ++byte) {
        SOCDNX_IF_ERR_EXIT(_soc_dpp_drc_combo28_check_swap(unit, SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE, dq_swap[drc_ndx][byte]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_check_ram_addr_bank_swap(int unit, int drc_ndx, uint32 addr_bank_swap[8][18])
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(_soc_dpp_drc_combo28_check_swap(unit, SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE, addr_bank_swap[drc_ndx]));

exit:
    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_check_ram_dq_swap_bytes(int unit, int drc_ndx, uint32 dq_swap_bytes[8][4])
{
    uint32 first_byte_pair_sum = 0;
    uint32 second_byte_pair_sum = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(_soc_dpp_drc_combo28_check_swap(unit, SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES, dq_swap_bytes[drc_ndx]));

    
    first_byte_pair_sum  = dq_swap_bytes[drc_ndx][0] + dq_swap_bytes[drc_ndx][1];
    second_byte_pair_sum = dq_swap_bytes[drc_ndx][2] + dq_swap_bytes[drc_ndx][3];

    if (((first_byte_pair_sum  != 5) && (first_byte_pair_sum  != 1)) || 
        ((second_byte_pair_sum != 5) && (second_byte_pair_sum != 1))) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("\nSwap Doesn't make sence. byte0-->%d byte1-->%d byte2-->%d byte3-->%d\nValid swaps can be only inside byte pair, two byte pairs, or both\n"), 
                                                         dq_swap_bytes[drc_ndx][0], dq_swap_bytes[drc_ndx][1], dq_swap_bytes[drc_ndx][2], dq_swap_bytes[drc_ndx][3]));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_str_prop_ext_ram_addr_bank_swap(int unit, int drc_ndx, uint32 addr_bank_swap[8][18])
{
    int bit;
    char suffix[DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

     
    for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_ADDR_BANK_BITS_PER_INTERFACE; bit++) {
        sal_snprintf(suffix, DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_bit%d", drc_ndx, bit );
        addr_bank_swap[drc_ndx][bit] = soc_property_suffix_num_get(unit, 9,  spn_EXT_RAM_ADDR_BANK_SWAP, suffix, bit);
        LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): suffix=%s=%d.\n"),FUNCTION_NAME(), suffix, addr_bank_swap[drc_ndx][bit]));
    }

    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap(int unit, int drc_ndx, uint32 dq_swap[8][4][8])
{
    int byte, bit;
    char suffix[DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

     
    for (byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; byte++) {
        for (bit = 0; bit < SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE; bit++) {
            sal_snprintf(suffix, DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_byte%d_bit%d", drc_ndx, byte, bit);
            dq_swap[drc_ndx][byte][bit] = soc_property_suffix_num_get(unit, 0,  spn_EXT_RAM_DQ_SWAP, suffix, bit);
            LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): suffix=%s=%d.\n"),FUNCTION_NAME(), suffix, dq_swap[drc_ndx][byte][bit]));
        }
    }

    SOCDNX_FUNC_RETURN;
}


int _soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap_bytes(int unit, int drc_ndx, uint32 dq_swap_bytes[8][4])
{
    int byte;
    char suffix[DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

     
    for (byte = 0; byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; byte++) {
        sal_snprintf(suffix, DRC_COMBO28_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_byte%d", drc_ndx, byte );
        dq_swap_bytes[drc_ndx][byte] = soc_property_suffix_num_get(unit, 0,  spn_EXT_RAM_DQ_SWAP, suffix, byte);
        LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): suffix=%s=%d.\n"),FUNCTION_NAME(), suffix, dq_swap_bytes[drc_ndx][byte]));
    }

    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_transform_value_based_on_byte_swaps(int unit, int drc_ndx, uint32 orig_val, uint32* new_val)
{

    soc_dpp_drc_combo28_info_t *drc_info;
    int current_byte;
    int current_swap;
    int shifts_needed;
    uint32 current_byte_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(new_val);

    *new_val = 0;

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;

    for (current_byte = 0; current_byte < SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES; ++current_byte) 
    {
        
        current_swap = drc_info->dram_dq_swap_bytes[drc_ndx][current_byte];

        
        current_byte_val = orig_val & (0xff << (current_byte * SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE));

        
        shifts_needed =  (current_swap - current_byte) * SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE;
        if (shifts_needed > 0) {
            current_byte_val <<= shifts_needed;
        } else if (shifts_needed < 0) {
            shifts_needed *= -1;
            current_byte_val >>= shifts_needed;
        } 

        
        *new_val |= current_byte_val; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_info_config(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int i = 0x0;
    uint32 max_nof_dram_interfaces = MAX_NOF_HW_DRAM_INTERFACES(unit);
    uint32 printable_bmp = 0;

    SOCDNX_INIT_FUNC_DEFS;

    

    SHR_BITCOUNT_RANGE(drc_info->dram_bitmap, drc_info->dram_num, 0, max_nof_dram_interfaces);

    
    if (drc_info->dram_num > MAX_NOF_HW_DRAM_INTERFACES(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Nubmber of dram is above max")));  
    }

    drc_info->enable = (drc_info->dram_num == 0) ? FALSE : TRUE;

    
    drc_info->auto_tune = soc_property_get(unit, spn_EXT_RAM_AUTO_TUNE, -1); 
    if (-1 == drc_info->auto_tune) {
        drc_info->auto_tune = soc_property_get(unit, spn_DDR3_AUTO_TUNE, 0x2);
    }

    
    drc_info->ref_clk_mhz = DRC_COMBO28_DDR_PHY_REF_CLK;

    drc_info->dram_freq = soc_property_get(unit, spn_EXT_RAM_FREQ, 0);
    drc_info->data_rate_mbps = (2 * drc_info->dram_freq);

    SHR_BITCOPY_RANGE(&printable_bmp, 0, drc_info->dram_bitmap, 0, max_nof_dram_interfaces);
    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s: dram_bitmap=0x%x, dram_num=%d, enable=%d, auto_tune=%d, dram_freq=%d, data_rate=%d\n"), 
                 FUNCTION_NAME(), printable_bmp, drc_info->dram_num, drc_info->enable, drc_info->auto_tune, drc_info->dram_freq, drc_info->data_rate_mbps));


    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_type(unit, &drc_info->dram_type));

    drc_info->nof_columns = soc_property_get(unit, spn_EXT_RAM_COLUMNS, 1024);
    drc_info->nof_rows    = soc_property_get(unit, spn_EXT_RAM_ROWS, 0x0);
    drc_info->nof_banks   = soc_property_get(unit, spn_EXT_RAM_BANKS, 0x0);

    
    drc_info->dram_param.c_cas_latency    = soc_property_get(unit, spn_EXT_RAM_C_CAS_LATENCY, 0x0);
    drc_info->dram_param.c_wr_latency     = soc_property_get(unit, spn_EXT_RAM_C_WR_LATENCY, 0x0);
    drc_info->dram_param.init_wait_period = soc_property_get(unit, spn_EXT_RAM_INIT_WAIT_PERIOD, 0x3ff);

    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RC,             &(drc_info->dram_param.t_rc)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RFC,            &(drc_info->dram_param.t_rfc)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RCD_RD,         &(drc_info->dram_param.t_rcd_rd)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RCD_WR,         &(drc_info->dram_param.t_rcd_wr)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RRD_L,          &(drc_info->dram_param.t_rrd_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RRD_S,          &(drc_info->dram_param.t_rrd_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RAS,            &(drc_info->dram_param.t_ras)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RP,             &(drc_info->dram_param.t_rp)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WR,             &(drc_info->dram_param.t_wr)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_FAW,            &(drc_info->dram_param.t_faw)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_32AW,           &(drc_info->dram_param.t_32aw)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RTP_L,          &(drc_info->dram_param.t_rtp_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WTR_L,          &(drc_info->dram_param.t_wtr_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_WTR_S,          &(drc_info->dram_param.t_wtr_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CCD_L,          &(drc_info->dram_param.t_ccd_l)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CCD_S,          &(drc_info->dram_param.t_ccd_s)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_ZQCS,           &(drc_info->dram_param.t_zqcs)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_REF,            &(drc_info->dram_param.t_ref)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_ALERT,      &(drc_info->dram_param.t_crc_alert)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_RST,            &(drc_info->dram_param.t_rst)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_AL,             &(drc_info->dram_param.t_al)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_RD_LATENCY, &(drc_info->dram_param.t_crc_rd_latency)));
    SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_dram_param_get(unit, spn_EXT_RAM_T_CRC_WR_LATENCY, &(drc_info->dram_param.t_crc_wr_latency)));


    
    drc_info->gear_down_mode                        = soc_property_get(unit, spn_EXT_RAM_GEAR_DOWN_MODE, 0x0);
    drc_info->abi                                   = soc_property_get(unit, spn_EXT_RAM_ABI, 0x0);
    drc_info->write_dbi                             = soc_property_get(unit, spn_EXT_RAM_WRITE_DBI, 0x0);
    drc_info->read_dbi                              = soc_property_get(unit, spn_EXT_RAM_READ_DBI, 0x0);
    drc_info->cmd_par_latency                       = soc_property_get(unit, spn_EXT_RAM_CMD_PAR_LATENCY, 0x0);    
    drc_info->write_crc                             = soc_property_get(unit, spn_EXT_RAM_WRITE_CRC, 0x0);
    drc_info->read_crc                              = soc_property_get(unit, spn_EXT_RAM_READ_CRC, 0x0);
    drc_info->t_ras_enable                          = soc_property_get(unit, spn_EXT_RAM_T_RAS_ENABLE, 0x0);
    drc_info->dram_auto_calibration_update_disable  = (soc_property_get(unit, spn_DRAM_AUTO_CALIBRATION_UPDATE_ENABLE, 0x1)) ? 0 : 1;
    
    if ((drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) && (drc_info->read_crc==1 )) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram type DDR4 does not support read CRC ")));  
    }

    
    SOC_DPP_CONFIG(unit)->arad->init.drc_info.appl_max_buffer_crc_err = soc_property_get(unit, spn_DRAM_CRC_DEL_BUFFER_MAX_RECLAIMS, 0x0);    

    
    SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, i) {
        if(soc_property_port_get(unit, i, spn_DRAM0_CLAMSHELL_ENABLE, FALSE) &&
                drc_info->dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
            if(soc_property_port_get(unit, i, spn_DRAM1_CLAMSHELL_ENABLE, FALSE)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOC_MSG("Wrong Dram ClamShell configuration. only one of the dram should be enabled")));
            }
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_0;
        } else if (soc_property_port_get(unit, i, spn_DRAM1_CLAMSHELL_ENABLE, FALSE) &&
                drc_info->dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_1;
        } else {
            if(drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5 &&
                (soc_property_port_get(unit, i, spn_DRAM0_CLAMSHELL_ENABLE, FALSE) ||
                 soc_property_port_get(unit, i, spn_DRAM1_CLAMSHELL_ENABLE, FALSE))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram type GDDR5 does not support Clamshell!\n ")));
            }
            drc_info->dram_clam_shell_mode[i] = DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED;
        }
    }

    
    SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, i) {
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap(unit, i, drc_info->dram_dq_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_check_ram_dq_swap(unit, i, drc_info->dram_dq_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_dq_swap_bytes(unit, i, drc_info->dram_dq_swap_bytes));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_check_ram_dq_swap_bytes(unit, i, drc_info->dram_dq_swap_bytes));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_addr_bank_swap(unit, i, drc_info->dram_addr_bank_swap));
        SOCDNX_IF_ERR_RETURN(_soc_dpp_drc_combo28_check_ram_addr_bank_swap(unit, i, drc_info->dram_addr_bank_swap));
    }
    
    
    drc_info->bist_enable = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "dram", 0x1);

    if ((drc_info->bist_enable == 1) && (drc_info->auto_tune == 0x3)) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram Bist was enabled without auto tuning - Invalid soc properties configuration")));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data)
{
    uint32
        soc_sand_rv,
        reg_val;
    const static soc_reg_t 
        DRC_REGISTER_CONTROLl[] = {DRCA_REGISTER_CONTROLr, DRCB_REGISTER_CONTROLr, DRCC_REGISTER_CONTROLr,
                                   DRCD_REGISTER_CONTROLr, DRCE_REGISTER_CONTROLr, DRCF_REGISTER_CONTROLr,
                                   DRCG_REGISTER_CONTROLr, DRCH_REGISTER_CONTROLr},
        DRC_REGISTER_CONTROL_WDATAl[] = {DRCA_REGISTER_CONTROL_WDATAr, DRCB_REGISTER_CONTROL_WDATAr, DRCC_REGISTER_CONTROL_WDATAr,
                                         DRCD_REGISTER_CONTROL_WDATAr, DRCE_REGISTER_CONTROL_WDATAr, DRCF_REGISTER_CONTROL_WDATAr,
                                         DRCG_REGISTER_CONTROL_WDATAr, DRCH_REGISTER_CONTROL_WDATAr};
   
    SOCDNX_INIT_FUNC_DEFS; 

    
    if (drc_ndx >= MAX_NOF_HW_DRAM_INTERFACES(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), MAX_NOF_HW_DRAM_INTERFACES(unit)));
    }
    
    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, addr=0x%08x, data=0x%08x\n"), FUNCTION_NAME(), drc_ndx, addr, data));

    reg_val = 0;
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TYPEf, 0x0);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ADDRESSf, addr);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROL_WDATAl[drc_ndx], REG_PORT_ANY, 0, data));

    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_sand_rv = arad_polling( unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, REGISTER_ACCESS_TRIGf, 0 );

#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
          
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data)
{
    uint32
        soc_sand_rv,
        reg_val;
    const static soc_reg_t 
        DRC_REGISTER_CONTROLl[] = {DRCA_REGISTER_CONTROLr, DRCB_REGISTER_CONTROLr, DRCC_REGISTER_CONTROLr,
                                   DRCD_REGISTER_CONTROLr, DRCE_REGISTER_CONTROLr, DRCF_REGISTER_CONTROLr,
                                   DRCG_REGISTER_CONTROLr, DRCH_REGISTER_CONTROLr},
        DRC_REGISTER_ACCESS_RD_DATA_VALIDl[] = {DRCA_REGISTER_ACCESS_RD_DATA_VALIDr, DRCB_REGISTER_ACCESS_RD_DATA_VALIDr, DRCC_REGISTER_ACCESS_RD_DATA_VALIDr,
                                                DRCD_REGISTER_ACCESS_RD_DATA_VALIDr, DRCE_REGISTER_ACCESS_RD_DATA_VALIDr, DRCF_REGISTER_ACCESS_RD_DATA_VALIDr,
                                                DRCG_REGISTER_ACCESS_RD_DATA_VALIDr, DRCH_REGISTER_ACCESS_RD_DATA_VALIDr},
        DRC_REGISTER_CONTROL_RDATAl[] = {DRCA_REGISTER_CONTROL_RDATAr, DRCB_REGISTER_CONTROL_RDATAr, DRCC_REGISTER_CONTROL_RDATAr,
                                         DRCD_REGISTER_CONTROL_RDATAr, DRCE_REGISTER_CONTROL_RDATAr, DRCF_REGISTER_CONTROL_RDATAr,
                                         DRCG_REGISTER_CONTROL_RDATAr, DRCH_REGISTER_CONTROL_RDATAr};
   
    SOCDNX_INIT_FUNC_DEFS; 

    
    SOCDNX_NULL_CHECK(data);

    
    if (drc_ndx >= MAX_NOF_HW_DRAM_INTERFACES(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), MAX_NOF_HW_DRAM_INTERFACES(unit)));
    }

    
    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, addr=0x%08x.\n"), FUNCTION_NAME(), drc_ndx, addr));

    reg_val = 0;
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TYPEf, 0x1);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ADDRESSf, addr);
    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_reg_field_set(unit, DRCA_REGISTER_CONTROLr, &reg_val, REGISTER_ACCESS_TRIGf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, reg_val));

    soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_REGISTER_CONTROLl[drc_ndx], REG_PORT_ANY, 0, REGISTER_ACCESS_TRIGf, 0 );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    soc_sand_rv = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_REGISTER_ACCESS_RD_DATA_VALIDl[drc_ndx], REG_PORT_ANY, 0, REGISTER_ACCESS_RD_DATA_VALIDf, 1 );
#if defined(PLISIM)
    if (!SAL_BOOT_PLISIM)
#endif
    {
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_REGISTER_CONTROL_RDATAl[drc_ndx], REG_PORT_ANY, 0, data));
    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): *data=0x%08x\n"), FUNCTION_NAME(), *data));
      
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask)
{
    int 
        rv = SOC_E_NONE;
    uint32
        tmp, otmp,
        masked_data;
   
    SOCDNX_INIT_FUNC_DEFS; 

    
    if (drc_ndx >= MAX_NOF_HW_DRAM_INTERFACES(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Dram index is above max in device(%d)\n"), MAX_NOF_HW_DRAM_INTERFACES(unit)));
    }

    
    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, addr=0x%08x, data=0x%08x, mask0x%08x\n"), FUNCTION_NAME(), drc_ndx, addr, data, mask));

    masked_data = data & mask;

    rv = soc_dpp_drc_combo28_phy_reg_read(unit, drc_ndx, addr, &tmp);
    SOCDNX_IF_ERR_EXIT(rv);

    otmp = tmp;
    tmp &= ~(mask);
    tmp |= masked_data;

    if (otmp != tmp) {
        rv = soc_dpp_drc_combo28_phy_reg_write(unit, drc_ndx, addr, tmp);
        SOCDNX_IF_ERR_EXIT(rv);
    }
      
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_shmoo_cfg_get(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param)
{
    int 
        byte,
        bit,
        var_array[DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY];
        
    char name_str[DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE];

    SOCDNX_INIT_FUNC_DEFS; 

    
    SOCDNX_NULL_CHECK(shmoo_config_param);
                
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        
        sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY * sizeof(int));    
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL, byte);
        if (soc_property_ci_get_csv(unit, drc_ndx, name_str, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
        }
        
        for (bit  =0; bit < SHMOO_COMBO28_BYTE; ++bit) {
            shmoo_config_param->dq_byte_wr_min_vdl_bit[byte][bit] = var_array[bit];     
        }
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MIN_VDL_DBI, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_min_vdl_dbi[byte] = var_array[byte];
    }

    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MIN_VDL_EDC, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) { 
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_min_vdl_edc[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));    
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DATA, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_max_vdl_data[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DQS, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_wr_max_vdl_dqs[byte] = var_array[byte];
    }    
   
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        
        sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));     
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL, byte);
        if(soc_property_ci_get_csv(unit, drc_ndx, name_str, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
        }

        for (bit = 0; bit < SHMOO_COMBO28_BYTE; ++bit) {
            shmoo_config_param->dq_byte_rd_min_vdl_bit[byte][bit] = var_array[bit];     
        }
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MIN_VDL_DBI, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_min_vdl_dbi[byte] = var_array[byte];
    }

    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MIN_VDL_EDC, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_min_vdl_edc[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSP, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_dqsp[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSN, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_dqsn[byte] = var_array[byte];
    }    

    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_REN_FIFO_CONFIG, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_ren_fifo_config[byte] = var_array[byte];
    } 

       
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));    
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_EDCEN_FIFO_CONFIG, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_edcen_fifo_config[byte] = var_array[byte];
    }
     
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_READ_MAX_VDL_FSM, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_rd_max_vdl_fsm[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_VREF_DAC_CONFIG, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_vref_dac_config[byte] = var_array[byte];
    }
    
    
    sal_memset(var_array, 0, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY*sizeof(int));
    if(soc_property_ci_get_csv(unit, drc_ndx, spn_COMBO28_TUNE_DQ_MACRO_RESERVED_REG, DRC_COMBO28_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY, var_array) == 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("Cannot find full tuning restoration data - Stopping tune parameters restore action\n")));
    }
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        shmoo_config_param->dq_byte_macro_reserved_reg[byte] = var_array[byte];
    }        
   
    
    shmoo_config_param->aq_l_max_vdl_addr = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MAX_VDL_ADDR, 0x0);
    
    
    shmoo_config_param->aq_l_max_vdl_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MAX_VDL_CTRL, 0x0);    

    
    shmoo_config_param->aq_l_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_L_MACRO_RESERVED_REG, 0x0);    

    
    shmoo_config_param->aq_u_max_vdl_addr = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MAX_VDL_ADDR, 0x0);

    
    shmoo_config_param->aq_u_max_vdl_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MAX_VDL_CTRL, 0x0);

    
    shmoo_config_param->aq_u_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_AQ_U_MACRO_RESERVED_REG, 0x0);

    
    shmoo_config_param->common_macro_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_COMMON_MACRO_RESERVED_REG, 0x0);
    
    
    shmoo_config_param->control_regs_read_clock_config = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_READ_CLOCK_CONFIG, 0x0);
    
    
    shmoo_config_param->control_regs_input_shift_ctrl = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_INPUT_SHIFT_CTRL, 0x0);   

    
    shmoo_config_param->control_regs_ren_fifo_central_init = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_REN_FIFO_CENTRAL_INITIALIZER, 0x0);
    
    
    shmoo_config_param->control_regs_edcen_fifo_central_init = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INIT, 0x0);
        
    
    shmoo_config_param->control_regs_shared_vref_dac_config = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_SHARED_VREF_DAC_CONFIG, 0x0);

     
    shmoo_config_param->control_regs_reserved_reg = soc_property_ci_get(unit, drc_ndx, spn_COMBO28_TUNE_CONTROL_REGS_RESERVED_REG, 0x0);

exit:

    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_cfg_set(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param)
{
    int 
        written = 0,
        byte,
        bit;
    char name_str[DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE];
    char val_str[DRC_COMBO28_VAL_STR_BUFFER_SIZE];    

    SOCDNX_INIT_FUNC_DEFS; 
        
    
    SOCDNX_NULL_CHECK(shmoo_config_param);
            
    for (byte=0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL, byte, drc_ndx, unit);

        written = 0;
        val_str[0]=0;
        for(bit=0; bit<SHMOO_COMBO28_BYTE; ++bit){
            written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_bit[byte][bit] );    
        }
        
        if (soc_mem_config_set(name_str, val_str) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_DQ_WR_MIN_VDL configuration")));
        }
    }
                 
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL_DBI, drc_ndx, unit);     
        
    written = 0;
    val_str[0]='\0';
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_dbi[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MIN_VDL_DBI configuration")));
    }

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MIN_VDL_EDC, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_min_vdl_edc[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MIN_VDL_EDC configuration")));
    }
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DATA, drc_ndx, unit);

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_max_vdl_data[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MAX_VDL_DATA configuration")));
    }
        
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_WR_MAX_VDL_DQS, drc_ndx, unit);

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_wr_max_vdl_dqs[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_WR_MAX_VDL_DQS configuration")));
    }
       
    for (byte = 0; byte < SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte) {
        
        sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_byte%d_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL, byte, drc_ndx, unit);

        written = 0;
        val_str[0]=0; 
        for(bit=0; bit<SHMOO_COMBO28_BYTE; ++bit){
            written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_bit[byte][bit] );    
        }
        
        if (soc_mem_config_set(name_str, val_str) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_DQ_READ_MIN_VDL configuration")));
        }
    }
                 
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL_DBI, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_dbi[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_READ_MIN_VDL_DBI configuration")));
    }

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MIN_VDL_EDC, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_min_vdl_edc[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MIN_VDL_EDC configuration")));
    }
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSP, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_dqsp[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MAX_VDL_DQSP configuration")));
    }
        
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_RD_MAX_VDL_DQSN, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;    
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_dqsn[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_RD_MAX_VDL_DQSN configuration")));
    }

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_REN_FIFO_CONFIG, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_ren_fifo_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_REN_FIFO_CONFIG configuration")));
    }
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_EDCEN_FIFO_CONFIG, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_edcen_fifo_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_EDCEN_FIFO_CONFIG configuration")));
    }
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_READ_MAX_VDL_FSM, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_rd_max_vdl_fsm[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_READ_MAX_VDL_FSM configuration")));
    }
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_VREF_DAC_CONFIG, drc_ndx, unit);     
    
    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_vref_dac_config[byte] );    
    }
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_VREF_DAC_CONFIG configuration")));
    }  

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_DQ_MACRO_RESERVED_REG, drc_ndx, unit);     

    written = 0;
    val_str[0]=0;
    for(byte=0; byte<SHMOO_COMBO28_DQ_CONFIG_NOF_BYTE; ++byte){
        written += sal_snprintf(val_str + written, DRC_COMBO28_VAL_STR_BUFFER_SIZE - written, "0x%08x,", shmoo_config_param->dq_byte_macro_reserved_reg[byte] );    
    }

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_DQ_BYTE_MACRO_RESERVED_REG configuration")));
    }  
              
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MAX_VDL_ADDR, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_max_vdl_addr );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_TUNE_aq_l_max_vdl_addr configuration")));
    }       
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MAX_VDL_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_max_vdl_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_l_max_vdl_ctrl configuration")));
    } 

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_L_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_l_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("FaileD to save combo28_tune_aq_l_macro_reserved_reg configuration")));
    } 

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MAX_VDL_ADDR, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_max_vdl_addr );    
        
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_max_vdl_addr configuration")));
    }       
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MAX_VDL_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_max_vdl_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_max_vdl_ctrl configuration")));
    }    

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_AQ_U_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->aq_u_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_aq_u_macro_reserved_reg configuration")));
    }    

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_COMMON_MACRO_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->common_macro_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_common_macro_reserved_reg configuration")));
    } 

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_READ_CLOCK_CONFIG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_read_clock_config );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_read_clock_config configuration")));
    }     
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_INPUT_SHIFT_CTRL, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_input_shift_ctrl );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_input_shift_ctrl configuration")));
    } 

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_REN_FIFO_CENTRAL_INITIALIZER, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_ren_fifo_central_init );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_ren_fifo_central_init configuration")));
    }
  
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_EDCEN_FIFO_CENTRAL_INIT, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_edcen_fifo_central_init );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_edcen_fifo_central_init configuration")));
    }  
    
    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_SHARED_VREF_DAC_CONFIG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_shared_vref_dac_config );    
    
    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Failed to save combo28_tune_control_regs_shared_vref_dac_config configuration")));
    }     

    
    sal_snprintf(name_str, DRC_COMBO28_SOC_PROPERTY_STR_BUFFER_SIZE, "%s_ci%d.%d", spn_COMBO28_TUNE_CONTROL_REGS_RESERVED_REG, drc_ndx, unit);

    sal_snprintf(val_str, DRC_COMBO28_VAL_STR_BUFFER_SIZE, "0x%08x,", shmoo_config_param->control_regs_reserved_reg );    

    if (soc_mem_config_set(name_str, val_str) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOC_MSG("Faile to save combo28_tune_control_regs_reserved_reg configuration")));
    }     

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_enable_wr_crc(int unit, int drc_ndx, int enable)
{   
    int 
    rv = SOC_E_NONE,
    mrs_opcode=0,
    mrs_mask=0,
    mrs_num;
    uint32 
    dram_type;
    uint64 reg64_val;
   
    static soc_reg_t 
        DRC_DRAM_SPECIAL_FEATURESl[] = {DRCA_DRAM_SPECIAL_FEATURESr, DRCB_DRAM_SPECIAL_FEATURESr, DRCC_DRAM_SPECIAL_FEATURESr, 
                                        DRCD_DRAM_SPECIAL_FEATURESr, DRCE_DRAM_SPECIAL_FEATURESr, DRCF_DRAM_SPECIAL_FEATURESr,
                                        DRCG_DRAM_SPECIAL_FEATURESr, DRCH_DRAM_SPECIAL_FEATURESr},
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, 
                                                         DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};
     
    SOCDNX_INIT_FUNC_DEFS;
 
    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, DRCA_DRAM_SPECIAL_FEATURESr, &reg64_val, WRITE_CRCf, (enable == 0x0) ? FALSE : TRUE);
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, DRC_DRAM_SPECIAL_FEATURESl[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    dram_type = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg64_val, DRAM_TYPEf);

    if (dram_type == SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4) {
    
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0) ? FALSE : TRUE), 12);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,12);        
        mrs_num = 2;
         
    } else {
    
        SOC_SAND_SET_BIT(mrs_opcode, ((enable == 0x0) ? TRUE : FALSE), 10);
        SOC_SAND_SET_BIT(mrs_mask, 0x1 ,10);       
        mrs_num = 4;
        
    }

    rv = soc_dpp_drc_combo28_shmoo_modify_mrs(unit, drc_ndx, mrs_num, mrs_opcode, mrs_mask);
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(int unit, soc_dpp_drc_combo28_info_t* drc_info, int drc_ndx)
{
    uint32 curr_refi = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) 
    {
        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_enable_refresh(unit, drc_ndx , 0, 0, &curr_refi));

        if (drc_info->write_dbi == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_wr_dbi(unit, drc_ndx, 1));
        }

        if(drc_info->read_dbi == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_rd_dbi(unit, drc_ndx, 1));
        }

        if(drc_info->write_crc == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_wr_crc(unit, drc_ndx, 1));
        }

        if(drc_info->read_crc == 1) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_shmoo_enable_rd_crc(unit, drc_ndx, 1));
        }

        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_enable_refresh(unit, drc_ndx , 1, curr_refi, &curr_refi));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_dynamic_calibration_calculate_vdl_shift(int unit, uint32 orig_vdl_value, int vdl_shift, uint32 vdl_addr, uint32* new_vdl_value) 
{
    int orig_ui_shift, orig_vdl_step, orig_total_position;
    int shifted_vdl_value, shifted_ui_shift, shifted_vdl_step, shifted_total_position;
    uint32 has_ui_shift = (vdl_addr & 0xf) == 0xa ? 1 : 0;
    soc_dpp_drc_combo28_info_t *drc_info;
    uint32 ui_size;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: orig_vdl_value=%u, vdl_shift=%d, vdl_addr=0x%x\n"), FUNCTION_NAME(), orig_vdl_value, vdl_shift, vdl_addr));

    SOCDNX_NULL_CHECK(new_vdl_value);

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    ui_size = drc_info->ui_size;

    if (has_ui_shift == 1) 
    {
        orig_ui_shift = orig_vdl_value >> 9;
        orig_vdl_step = orig_vdl_value & 0x1ff;
        orig_total_position = orig_ui_shift * ui_size + orig_vdl_step;
        shifted_total_position = orig_total_position + vdl_shift;
        shifted_ui_shift = shifted_total_position / ui_size;
        shifted_vdl_step = shifted_total_position % ui_size;
        shifted_vdl_value = (shifted_ui_shift << 9) | shifted_vdl_step;
        *new_vdl_value = shifted_vdl_value;
    } else {
        *new_vdl_value = orig_vdl_value + vdl_shift;
    }

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() results: new_vdl_value=%u\n"), FUNCTION_NAME(), *new_vdl_value));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_run_dynamic_calibration_iterations(int unit, int drc_ndx, uint32 vdl_addr, int vdl_shift, uint32 nof_iterations, int* err_occurred) 
{
    int i;
    uint32 reg_val;
    uint32 orig_vdl_value;
    uint32 new_vdl_value;
    soc_reg_above_64_val_t reg_above_64;

    static soc_reg_t
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr },
        DRC_PHY_CALIB_FINISHEDr[] = {DRCA_PHY_CALIB_FINISHEDr, DRCB_PHY_CALIB_FINISHEDr, DRCC_PHY_CALIB_FINISHEDr, DRCD_PHY_CALIB_FINISHEDr, 
                                     DRCE_PHY_CALIB_FINISHEDr, DRCF_PHY_CALIB_FINISHEDr, DRCG_PHY_CALIB_FINISHEDr, DRCH_PHY_CALIB_FINISHEDr },
        DRC_CALIB_BIST_ERROR_OCCURREDr[] = {DRCA_CALIB_BIST_ERROR_OCCURREDr, DRCB_CALIB_BIST_ERROR_OCCURREDr, DRCC_CALIB_BIST_ERROR_OCCURREDr, DRCD_CALIB_BIST_ERROR_OCCURREDr, 
                                            DRCE_CALIB_BIST_ERROR_OCCURREDr, DRCF_CALIB_BIST_ERROR_OCCURREDr, DRCG_CALIB_BIST_ERROR_OCCURREDr, DRCH_CALIB_BIST_ERROR_OCCURREDr };    
    
    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, vdl_addr=0x%x, vdl_shift=%d, nof_iterations=%u\n"), FUNCTION_NAME(), drc_ndx, vdl_addr, vdl_shift, nof_iterations));

    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, vdl_addr, &orig_vdl_value));

    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_calculate_vdl_shift(unit, orig_vdl_value, vdl_shift, vdl_addr, &new_vdl_value));

    *err_occurred = 0;

    for (i = 0; (i < nof_iterations) && (*err_occurred != 1); ++i)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_PHY_REG_ADDRf, vdl_addr);
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_PHY_REG_CURR_VALf, orig_vdl_value);
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, CALIB_PHY_REG_OVRD_VALf, new_vdl_value);
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], reg_above_64, PHY_CALIB_ENf, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
        
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_PHY_CALIB_FINISHEDr[drc_ndx], REG_PORT_ANY, 0, PHY_CALIB_FINISHEDf, 0x1));

        SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, PHY_CALIB_ENf, 0));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_CALIB_BIST_ERROR_OCCURREDr[drc_ndx], REG_PORT_ANY, 0, &reg_val));
        *err_occurred = soc_reg_field_get(unit, DRC_CALIB_BIST_ERROR_OCCURREDr[drc_ndx], reg_val, CALIB_BIST_ERR_OCCURREDf) ? 1 : 0;

        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() results: err_occurred=%u\n"), FUNCTION_NAME(), *err_occurred));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_configure_dynamic_calibration_iteration(int unit, int drc_ndx, uint32 cycles_per_iteration) 
{
    static soc_reg_t
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr },
        DRC_GDDR_5_BIST_CONFIGURATIONSr[] = {DRCA_GDDR_5_BIST_CONFIGURATIONSr, DRCB_GDDR_5_BIST_CONFIGURATIONSr, DRCC_GDDR_5_BIST_CONFIGURATIONSr, DRCD_GDDR_5_BIST_CONFIGURATIONSr, 
                                             DRCE_GDDR_5_BIST_CONFIGURATIONSr, DRCF_GDDR_5_BIST_CONFIGURATIONSr, DRCG_GDDR_5_BIST_CONFIGURATIONSr, DRCH_GDDR_5_BIST_CONFIGURATIONSr },
        DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr[] = {DRCA_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCB_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCD_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, 
                                                       DRCE_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCF_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCG_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr, DRCH_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr };

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, cycles_per_iteration=%u\n"), FUNCTION_NAME(), drc_ndx, cycles_per_iteration));

    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, CALIB_OVRD_MODEf, 0));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_GDDR_5_BIST_CONFIGURATIONSr[drc_ndx], REG_PORT_ANY, GDDR_5_BIST_MODEf, 3));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSr[drc_ndx], REG_PORT_ANY, GDDR_5_BIST_TOTAL_NUMBER_OF_COMMANDSf, cycles_per_iteration * 12)); 

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_dynamic_calibration_change_vdl(int unit, int drc_ndx, uint32 vdl_addr, int vdl_shift)
{
    int err_occurred;
    static soc_reg_t
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr };
    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, vdl_addr=0x%x, vdl_shift=%d\n"), FUNCTION_NAME(), drc_ndx, vdl_addr, vdl_shift));

    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, CALIB_OVRD_MODEf, 1));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_run_dynamic_calibration_iterations(unit, drc_ndx, vdl_addr, vdl_shift, 1, &err_occurred));

exit:
    SOCDNX_FUNC_RETURN;
}





int soc_dpp_drc_combo28_dynamic_calibration_find_vdl_shift_border(int unit, int drc_ndx, uint32 vdl_addr, uint32 iterations_per_position, uint32 range, int direction, int* found_border) 
{
    int border_position = 0;
    int next_search_position = 0;
    int err_occured = 0;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, vdl_addr=0x%x, iterations_per_position=%u, range=%u, direction=%d\n"), 
                                                 FUNCTION_NAME(),    drc_ndx,    vdl_addr,      iterations_per_position,    range,    direction));

    
    if (direction * direction != 1)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("direction = %d, allowed directions are -1 or 1\n"), direction));
    }

    
    SOCDNX_NULL_CHECK(found_border);

    while (border_position + 1 < range) 
    {
        next_search_position = (border_position + range) / 2;
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_run_dynamic_calibration_iterations(unit, drc_ndx, vdl_addr, next_search_position * direction, iterations_per_position, &err_occured));
        if(err_occured == 0) 
        {
            border_position = next_search_position;
        } else
        {
            range = next_search_position;
        }
    }
    *found_border = border_position * direction;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() results: found_border=%d\n"), FUNCTION_NAME(), *found_border));

exit:
    SOCDNX_FUNC_RETURN;
}





int soc_dpp_drc_combo28_dynamic_calibrate_write_path(int unit, int drc_ndx, uint32 vdl_addr)
{
    int left_vdl_shift_border;
    int right_vdl_shift_border;
    int new_vdl_shift;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, vdl_addr=0x%x\n"), FUNCTION_NAME(),    drc_ndx,    vdl_addr));

    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_configure_dynamic_calibration_iteration( unit, drc_ndx, DRC_COMBO28_DYNAMIC_CALIBRATION_CYCLES_PER_ITERATION));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_find_vdl_shift_border(unit, drc_ndx, vdl_addr, 10, 35, -1, &left_vdl_shift_border));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_find_vdl_shift_border(unit, drc_ndx, vdl_addr, 10, 35,  1, &right_vdl_shift_border));
    new_vdl_shift = (left_vdl_shift_border + right_vdl_shift_border) / 2;
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_change_vdl(unit, drc_ndx, vdl_addr, new_vdl_shift));

exit:
    SOCDNX_FUNC_RETURN;    
}



uint32 soc_dpp_drc_combo28_dynamic_calibration_subtract_rd_2g_delay (uint32 data) 
{
    uint32 new_data = 0;

    new_data = ((data & 0x1f) == 0) ? data : (data & 0xfffffc00) | ( (data & (0x1f << 5) ) + (1 << 5) ) | ( (data & 0x1f) - 1);
    return new_data;
}


uint32 soc_dpp_drc_combo28_dynamic_calibration_check_and_fix_fsm_vdl_wrap_around(int ui_size, int new_fsm_vdl_position)
{
    if (new_fsm_vdl_position > 2 * ui_size) {
        
        return new_fsm_vdl_position - 2 * ui_size;
    } else if (new_fsm_vdl_position < 0) {
        
        return new_fsm_vdl_position + 2 * ui_size;
    } else {
        
        return new_fsm_vdl_position;
    }
}



int soc_dpp_drc_combo28_dynamic_calibration_change_fsm_vdl(int unit, int drc_ndx, int shifts[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES]) 
{
    int i;
    uint32 current_fsm_vdl_positions[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];
    uint32 dq_byte_read_max_vdl_fsm_addr[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0x01d, 0x11d, 0x21d, 0x31d};
    uint32 current_ren_fifo_config[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];
    uint32 dq_byte_ren_fifo_config_addr[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0x01a, 0x11a, 0x21a, 0x31a};
    uint32 current_edcen_fifo_config[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];
    uint32 dq_byte_edcen_fifo_config_addr[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0x01b, 0x11b, 0x21b, 0x31b};
    uint32 reset_ctrl_value;
    uint32 ren_fifo_central_initializer_value;
    uint32 edcen_fifo_central_initializer_value;
    uint32 field_val;
    uint32 ui_size;
    soc_reg_above_64_val_t reg_above_64;    
    soc_dpp_drc_combo28_info_t *drc_info;

    static soc_reg_t
        DRC_PHY_CALIBRATION_CTRLr[] = {DRCA_PHY_CALIBRATION_CTRLr, DRCB_PHY_CALIBRATION_CTRLr, DRCC_PHY_CALIBRATION_CTRLr, DRCD_PHY_CALIBRATION_CTRLr, 
                                       DRCE_PHY_CALIBRATION_CTRLr, DRCF_PHY_CALIBRATION_CTRLr, DRCG_PHY_CALIBRATION_CTRLr, DRCH_PHY_CALIBRATION_CTRLr },
        DRC_PHY_CALIBRATION_RES_FIFO_CFGr[] = {DRCA_PHY_CALIBRATION_RES_FIFO_CFGr, DRCB_PHY_CALIBRATION_RES_FIFO_CFGr, DRCC_PHY_CALIBRATION_RES_FIFO_CFGr, DRCD_PHY_CALIBRATION_RES_FIFO_CFGr, 
                                               DRCE_PHY_CALIBRATION_RES_FIFO_CFGr, DRCF_PHY_CALIBRATION_RES_FIFO_CFGr, DRCG_PHY_CALIBRATION_RES_FIFO_CFGr, DRCH_PHY_CALIBRATION_RES_FIFO_CFGr },
        DRC_PHY_CALIB_FINISHEDr[] = {DRCA_PHY_CALIB_FINISHEDr, DRCB_PHY_CALIB_FINISHEDr, DRCC_PHY_CALIB_FINISHEDr, DRCD_PHY_CALIB_FINISHEDr, 
                                     DRCE_PHY_CALIB_FINISHEDr, DRCF_PHY_CALIB_FINISHEDr, DRCG_PHY_CALIB_FINISHEDr, DRCH_PHY_CALIB_FINISHEDr };
    static soc_field_t
        CALIB_PHY_RES_FSM_VDL_VAL_BYTEf[] = {CALIB_PHY_RES_FSM_VDL_VAL_BYTE_0f, CALIB_PHY_RES_FSM_VDL_VAL_BYTE_1f, CALIB_PHY_RES_FSM_VDL_VAL_BYTE_2f, CALIB_PHY_RES_FSM_VDL_VAL_BYTE_3f},
        CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTEf[] = {CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTE_0f, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTE_1f, 
                                                          CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTE_2f, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTE_3f},
        CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTEf[] = {CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTE_0f, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTE_1f, 
                                                              CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTE_2f, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTE_3f};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, shifts[0]=%d, shifts[1]=%d, shifts[2]=%d, shifts[3]=%d\n"), 
                                                 FUNCTION_NAME(),    drc_ndx,    shifts[0],    shifts[1],    shifts[2],    shifts[3]));

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    ui_size = drc_info->ui_size;

    for (i = 0; i < SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES; ++i)
    {
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, dq_byte_read_max_vdl_fsm_addr[i],  &current_fsm_vdl_positions[i]));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, dq_byte_ren_fifo_config_addr[i],   &current_ren_fifo_config[i]));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, dq_byte_edcen_fifo_config_addr[i], &current_edcen_fifo_config[i]));
    }
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, current_fsm_vdl_positions[0]=%d, current_fsm_vdl_positions[1]=%d, current_fsm_vdl_positions[2]=%d, current_fsm_vdl_positions[3]=%d\n"), 
                                                 FUNCTION_NAME(),    drc_ndx,    current_fsm_vdl_positions[0],    current_fsm_vdl_positions[1],    current_fsm_vdl_positions[2],    current_fsm_vdl_positions[3]));
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, current_ren_fifo_config[0]=%d, current_ren_fifo_config[1]=%d, current_ren_fifo_config[2]=%d, current_ren_fifo_config[3]=%d\n"), 
                                                 FUNCTION_NAME(),    drc_ndx,    current_ren_fifo_config[0],    current_ren_fifo_config[1],    current_ren_fifo_config[2],    current_ren_fifo_config[3]));
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, dq_byte_edcen_fifo_config_addr[0]=%d, dq_byte_edcen_fifo_config_addr[1]=%d, dq_byte_edcen_fifo_config_addr[2]=%d, dq_byte_edcen_fifo_config_addr[3]=%d\n"), 
                                                 FUNCTION_NAME(),    drc_ndx,    dq_byte_edcen_fifo_config_addr[0],    dq_byte_edcen_fifo_config_addr[1],    dq_byte_edcen_fifo_config_addr[2],    dq_byte_edcen_fifo_config_addr[3]));                                            
                                                
    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, 0xf01, &reset_ctrl_value));
    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, 0xf06, &ren_fifo_central_initializer_value));
    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, 0xf08, &edcen_fifo_central_initializer_value));

    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, CALIB_OVRD_MODEf, 2));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64)); 
    for (i = 0; i < SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES; ++i)
    {
        field_val = soc_dpp_drc_combo28_dynamic_calibration_check_and_fix_fsm_vdl_wrap_around(ui_size, current_fsm_vdl_positions[i] + shifts[i]);
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, new_fsm_vdl_position[%d]=%d\n"), FUNCTION_NAME(), drc_ndx, i, field_val));
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_FSM_VDL_VAL_BYTEf[i], field_val); 
        field_val = soc_dpp_drc_combo28_dynamic_calibration_subtract_rd_2g_delay(current_ren_fifo_config[i]);
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, current_ren_fifo_config[%d]=%d\n"), FUNCTION_NAME(), drc_ndx, i, field_val));
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_BYTEf[i], field_val);
        field_val = soc_dpp_drc_combo28_dynamic_calibration_subtract_rd_2g_delay(current_edcen_fifo_config[i]);
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, current_edcen_fifo_config[%d]=%d\n"), FUNCTION_NAME(), drc_ndx, i, field_val));
        soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTEf[i], field_val);        
    }
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_WRITE_ENABLE_RESET_VALf, (ren_fifo_central_initializer_value & 0xfffffff0));
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_REN_FIFO_CFG_INITIAL_VAL_EDC_BYTE_3f, field_val);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_WRITE_ENABLE_RESET_VAL_EDCf, (edcen_fifo_central_initializer_value & 0xfffffff0));
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_PHY_RESET_RESET_VALf, (reset_ctrl_value & 0xfffffff8));
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_REG_ACCESS_PRDf, 8);
    soc_reg_above_64_field32_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], reg_above_64, CALIB_PHY_RES_RD_2G_DELAY_RANGEf, 3);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_PHY_CALIBRATION_RES_FIFO_CFGr[drc_ndx], REG_PORT_ANY, 0, reg_above_64));


    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, PHY_CALIB_ENf, 1));
    SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_PHY_CALIB_FINISHEDr[drc_ndx], REG_PORT_ANY, 0, PHY_CALIB_FINISHEDf, 0x1));
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, DRC_PHY_CALIBRATION_CTRLr[drc_ndx], REG_PORT_ANY, 0, PHY_CALIB_ENf, 0));

exit:
    SOCDNX_FUNC_RETURN;    
}


int soc_dpp_drc_combo28_cdr_monitor_enable_set(int unit, int drc_ndx, int switch_on)
{
    uint32 field32_val;
    static soc_reg_t 
        DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[] = {DRCA_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCB_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCC_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCD_PERIODIC_PHY_CDR_MONITOR_CFGr,
                                               DRCE_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCF_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCG_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCH_PERIODIC_PHY_CDR_MONITOR_CFGr };
    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, switch_on=%d\n"), FUNCTION_NAME(),    drc_ndx,    switch_on));

    field32_val = switch_on ? 1 : 0;
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], REG_PORT_ANY, PHY_CDR_MONITOR_ENf, field32_val));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_drc_combo28_cdr_monitor_enable_get(int unit, int drc_ndx, int* is_enabled)
{
    uint32 field32_val;
    uint64 reg64_val;
    static soc_reg_t DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[] = {DRCA_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCB_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCC_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCD_PERIODIC_PHY_CDR_MONITOR_CFGr,
                                                            DRCE_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCF_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCG_PERIODIC_PHY_CDR_MONITOR_CFGr, DRCH_PERIODIC_PHY_CDR_MONITOR_CFGr };
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    field32_val = soc_reg64_field32_get(unit, DRC_PERIODIC_PHY_CDR_MONITOR_CFGr[drc_ndx], reg64_val, PHY_CDR_MONITOR_ENf);
    *is_enabled = field32_val;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d is_enabled=%d\n"), FUNCTION_NAME(), drc_ndx, *is_enabled));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_cdr_ctl(int unit, int drc_ndx, int auto_copy, int reset_n, int enable)
{
    combo28_cdr_config_param_t cdr_config_params;
    combo16_cdr_config_param_t cdr_config_params_16;

    SOCDNX_INIT_FUNC_DEFS;

    cdr_config_params.auto_copy = auto_copy;
    cdr_config_params.byte = -1;
    cdr_config_params.reset_n = reset_n;
    cdr_config_params.enable = enable;
    cdr_config_params.p = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.n = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.i = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.q = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.ib = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.data_source = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.init_lock_transition = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.accu_pos_threshold = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.update_gap = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;
    cdr_config_params.update_mode = SHMOO_COMBO28_CDR_UNDEFINED_VALUE;

    if (SOC_IS_SINAI(unit))
    {
        sal_memcpy(&cdr_config_params_16,&cdr_config_params,sizeof(cdr_config_params_16));
        SOCDNX_IF_ERR_EXIT(soc_combo16_cdr_ctl(unit, drc_ndx, 0, &cdr_config_params_16));    
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_combo28_cdr_ctl(unit, drc_ndx, 0, &cdr_config_params));    
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_update_dram_reference_fsm_vdl(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t* drc_info, int fsm_vdl_total_drift[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES])
{
    int i;
    uint32 new_reference_fsm_vdl[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0};
    uint32 dq_byte_status_cdr_pn_vdl_addr[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0x27, 0x127, 0x227, 0x327};

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d, drc_info*=0x%p\n"), FUNCTION_NAME(), drc_ndx, (void*)drc_info));

    for (i = 0; i < SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES; ++i) 
    {
        
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_shmoo_phy_reg_read (unit, drc_ndx, dq_byte_status_cdr_pn_vdl_addr[i], &new_reference_fsm_vdl[i]));
        
        new_reference_fsm_vdl[i] &=0x1ff;
        
        fsm_vdl_total_drift[i] = new_reference_fsm_vdl[i] - drc_info->dram_reference_fsm_vdl[drc_ndx][i];
        
        drc_info->dram_reference_fsm_vdl[drc_ndx][i] = new_reference_fsm_vdl[i];
    }
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() results:    fsm_vdl_total_drift[0]=%d, fsm_vdl_total_drift[1]=%d, fsm_vdl_total_drift[2]=%d, fsm_vdl_total_drift[3]=%d\n"), 
                                                 FUNCTION_NAME(), fsm_vdl_total_drift[0],    fsm_vdl_total_drift[1],    fsm_vdl_total_drift[2],    fsm_vdl_total_drift[3]));    
        
exit:
    SOCDNX_FUNC_RETURN;
}


static int soc_dpp_drc_combo28_mini_shmoo_run(int unit, int drc_ndx)
{
    int cdr_is_enabled;
    combo28_shmoo_config_param_t *config_param;
    soc_dpp_drc_combo28_info_t *drc_info;    
        
    SOCDNX_INIT_FUNC_DEFS;

    drc_info = &(SOC_DPP_CONFIG(unit)->arad->init.drc_info);
    config_param = &(SOC_DPP_CONFIG(unit)->arad->init.drc_info.shmoo_config_param[drc_ndx]);    
    
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_cdr_monitor_enable_get(unit, drc_ndx, &cdr_is_enabled));
    if (cdr_is_enabled)
    {
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() Disable cdr\n"), FUNCTION_NAME()));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 0));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_cdr_ctl(unit, drc_ndx, 1, 0, 0));
    }

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() Running shmoo on interface %d\n"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT( soc_combo28_shmoo_ctl(unit, drc_ndx, -1, 0, SHMOO_COMBO28_ACTION_RESTORE, config_param));

    if (cdr_is_enabled)
    {
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() Enable cdr\n"), FUNCTION_NAME()));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_cdr_ctl(unit, drc_ndx, 1, 1, 1));
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 1));
    }
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() restore dbi and crc\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_gddr5_restore_dbi_and_crc(unit, drc_info, drc_ndx));
        
exit:
    SOCDNX_FUNC_RETURN;
}


static int soc_dpp_drc_combo28_rd_wr_crc_clear(int unit, int drc_ndx)
{
    soc_reg_t DRC_INTERRUPT_REGISTERl[] = { DRCA_INTERRUPT_REGISTERr, DRCB_INTERRUPT_REGISTERr, DRCC_INTERRUPT_REGISTERr, DRCD_INTERRUPT_REGISTERr,
                                            DRCE_INTERRUPT_REGISTERr, DRCF_INTERRUPT_REGISTERr, DRCG_INTERRUPT_REGISTERr, DRCH_INTERRUPT_REGISTERr};
    uint32 reg32_val = 0;
    
    SOCDNX_INIT_FUNC_DEFS;
     
    
    soc_reg_field_set(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], &reg32_val, DRAM_WR_CRC_ERRf, 1);
    soc_reg_field_set(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], &reg32_val, DRAM_RD_CRC_ERRf, 1);
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], REG_PORT_ANY, 0, reg32_val));
    
exit:
    SOCDNX_FUNC_RETURN;    
}




static int soc_dpp_drc_combo28_rd_wr_crc_check(int unit, int drc_ndx, int *crc_happend)
{
    soc_reg_t DRC_INTERRUPT_REGISTERl[] = { DRCA_INTERRUPT_REGISTERr, DRCB_INTERRUPT_REGISTERr, DRCC_INTERRUPT_REGISTERr, DRCD_INTERRUPT_REGISTERr,
                                            DRCE_INTERRUPT_REGISTERr, DRCF_INTERRUPT_REGISTERr, DRCG_INTERRUPT_REGISTERr, DRCH_INTERRUPT_REGISTERr};
    uint32 reg32_val;
    int wr_crc;
    int rd_crc;

    SOCDNX_INIT_FUNC_DEFS;
    
    *crc_happend = 0;
    
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &reg32_val));
    wr_crc = soc_reg_field_get(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], reg32_val, DRAM_WR_CRC_ERRf);
    rd_crc = soc_reg_field_get(unit, DRC_INTERRUPT_REGISTERl[drc_ndx], reg32_val, DRAM_RD_CRC_ERRf);
    
    *crc_happend = wr_crc || rd_crc;
    
exit:
    SOCDNX_FUNC_RETURN;    
}


static int soc_dpp_drc_combo28_rd_wr_crc_corrective_action(int unit, int drc_ndx, int* soft_reset_is_requiered)
{
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 dynmic_memory_access_value;
    int fbc_fmc_0_min_index = 0;
    int fbc_fmc_0_max_index = 0;
    int fbc_fmc_1_min_index = 0;
    int fbc_fmc_1_max_index = 0;
    int fbc_mnmc_0_min_index = 0;
    int fbc_mnmc_0_max_index = 0;
    int fbc_mnmc_1_min_index = 0;
    int fbc_mnmc_1_max_index = 0;
    int dbuff_size;
    int shifts_for_mmu_read_write_action;
    int crc_happend_during_read;
    int crc_happend_during_write;
    soc_jer_ocb_dram_dbuffs_bound_t  *dbuffs;
    uint8 *fbc_fmc_0 = NULL;
    uint8 *fbc_fmc_1 = NULL;
    uint8 *fbc_mnmc_0 = NULL;
    uint8 *fbc_mnmc_1 = NULL;
    uint64 reg64_val;
    int fmc_0_start_original;
    int fmc_0_start_new;
    int fmc_1_start_original;
    int fmc_1_start_new;    
        
    SOCDNX_INIT_FUNC_DEFS;

    *soft_reset_is_requiered = 0;
    dbuffs = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    dbuff_size = SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size;
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, EGQ_GLOBAL_GENERAL_CFG_3r, REG_PORT_ANY, 0, &reg32_val));
    shifts_for_mmu_read_write_action = soc_reg_field_get(unit, EGQ_GLOBAL_GENERAL_CFG_3r, reg32_val, DRAM_BUFF_SIZEf);
        
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() check fbc size, and check buffer size to allocate accordingly\n"), FUNCTION_NAME()));
    SOCDNX_ALLOC(fbc_fmc_0, uint8, dbuffs->fbc_fmc_0.size * dbuff_size, "fbc_fmc_0");
    SOCDNX_ALLOC(fbc_fmc_1, uint8, dbuffs->fbc_fmc_1.size * dbuff_size, "fbc_fmc_1");
    SOCDNX_ALLOC(fbc_mnmc_0, uint8, dbuffs->fbc_mnmc_0.size * dbuff_size, "fbc_mnmc_0");
    SOCDNX_ALLOC(fbc_mnmc_1, uint8, dbuffs->fbc_mnmc_1.size * dbuff_size, "fbc_mnmc_1");

    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() get range of fbc buffers\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above64_val));
    fbc_fmc_0_min_index = (soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf)) << shifts_for_mmu_read_write_action;
    fbc_fmc_0_max_index = (((soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf)) + 1) << shifts_for_mmu_read_write_action) - 1;
    fbc_mnmc_0_min_index = (soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf)) << shifts_for_mmu_read_write_action;
    fbc_mnmc_0_max_index = (((soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf)) + 1) << shifts_for_mmu_read_write_action) - 1;    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, REG_PORT_ANY, 1, reg_above64_val));
    fbc_fmc_1_min_index = (soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_LOWf)) << shifts_for_mmu_read_write_action;
    fbc_fmc_1_max_index = (((soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_FULL_MULTICAST_N_LIMIT_HIGHf)) + 1) << shifts_for_mmu_read_write_action) - 1;
    fbc_mnmc_1_min_index = (soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_LOWf)) << shifts_for_mmu_read_write_action;
    fbc_mnmc_1_max_index = (((soc_reg_above_64_field32_get(unit, IDR_FBC_EXTERNAL_CONFIGURATIONr, reg_above64_val, FBC_EXTERNAL_MINI_MULTICAST_N_LIMIT_HIGHf)) + 1) << shifts_for_mmu_read_write_action) - 1;
    
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() change FMC buffer ranges to invalid to prevent trashing of the LBM\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, &reg64_val));
    fmc_0_start_original = soc_reg64_field32_get(unit,MMU_GLOBAL_FULL_MC_DB_RANGE_0r,reg64_val,FULL_MULTICAST_0_DB_PTR_STARTf);
    fmc_0_start_new = 1 + soc_reg64_field32_get(unit,MMU_GLOBAL_FULL_MC_DB_RANGE_0r,reg64_val,FULL_MULTICAST_0_DB_PTR_ENDf);
    soc_reg64_field32_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, &reg64_val, FULL_MULTICAST_0_DB_PTR_STARTf, fmc_0_start_new);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, reg64_val));
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, &reg64_val));
    fmc_1_start_original = soc_reg64_field32_get(unit,MMU_GLOBAL_FULL_MC_DB_RANGE_1r,reg64_val,FULL_MULTICAST_1_DB_PTR_STARTf);
    fmc_1_start_new = 1 + soc_reg64_field32_get(unit,MMU_GLOBAL_FULL_MC_DB_RANGE_1r,reg64_val,FULL_MULTICAST_1_DB_PTR_ENDf);
    soc_reg64_field32_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, &reg64_val, FULL_MULTICAST_1_DB_PTR_STARTf, fmc_1_start_new);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, reg64_val));
    
    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, MMU_PDC_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, LBM_USE_ONLY_FULL_MCf, 1));
        
    
    READ_MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, &dynmic_memory_access_value);
    WRITE_MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1);

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_clear(unit, drc_ndx));
    
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, INDIRECT_LOGICAL_MODEf, 1));
    SOCDNX_IF_ERR_EXIT( soc_mem_read_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ANY, fbc_fmc_0_min_index, fbc_fmc_0_max_index, fbc_fmc_0));
    SOCDNX_IF_ERR_EXIT( soc_mem_read_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ANY, fbc_fmc_1_min_index, fbc_fmc_1_max_index, fbc_fmc_1));
    SOCDNX_IF_ERR_EXIT( soc_mem_read_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ANY, fbc_mnmc_0_min_index, fbc_mnmc_0_max_index, fbc_mnmc_0));
    SOCDNX_IF_ERR_EXIT( soc_mem_read_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ANY, fbc_mnmc_1_min_index, fbc_mnmc_1_max_index, fbc_mnmc_1));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_check(unit, drc_ndx, &crc_happend_during_read));
    
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() Run Mini Shmoo\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_mini_shmoo_run(unit, drc_ndx));
    
    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_clear(unit, drc_ndx));    
    
    
    SOCDNX_IF_ERR_EXIT( soc_mem_write_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ALL, fbc_fmc_0_min_index, fbc_fmc_0_max_index, fbc_fmc_0));
    SOCDNX_IF_ERR_EXIT( soc_mem_write_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ALL, fbc_fmc_1_min_index, fbc_fmc_1_max_index, fbc_fmc_1));
    SOCDNX_IF_ERR_EXIT( soc_mem_write_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ALL, fbc_mnmc_0_min_index, fbc_mnmc_0_max_index, fbc_mnmc_0));
    SOCDNX_IF_ERR_EXIT( soc_mem_write_range(unit, MMU_DRAM_ADDRESS_SPACEm, SOC_BLOCK_ALL, fbc_mnmc_1_min_index, fbc_mnmc_1_max_index, fbc_mnmc_1));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_check(unit, drc_ndx, &crc_happend_during_write));    
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, INDIRECT_LOGICAL_MODEf, 0));
    WRITE_MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, dynmic_memory_access_value);        
                
    
    SOCDNX_IF_ERR_EXIT( soc_reg_above_64_field32_modify(unit, MMU_PDC_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, LBM_USE_ONLY_FULL_MCf, 0));
        
    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, &reg64_val, FULL_MULTICAST_0_DB_PTR_STARTf, fmc_0_start_original);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_0r, REG_PORT_ANY, 0, reg64_val));
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, &reg64_val, FULL_MULTICAST_1_DB_PTR_STARTf, fmc_1_start_original);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, MMU_GLOBAL_FULL_MC_DB_RANGE_1r, REG_PORT_ANY, 0, reg64_val));

    if (crc_happend_during_read || crc_happend_during_write)
    {
        *soft_reset_is_requiered = 1;
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() CRC error ouccured when trying to read/write FBC, interface is beyond repair, other action is requiered\n"), FUNCTION_NAME()));
    }
            
exit:
    
    SOCDNX_FREE(fbc_fmc_0);
    SOCDNX_FREE(fbc_fmc_1);
    SOCDNX_FREE(fbc_mnmc_0);
    SOCDNX_FREE(fbc_mnmc_1);
    SOCDNX_FUNC_RETURN;
}  



int soc_dpp_drc_combo28_rd_wr_crc_handle_interrupt(int unit, int drc_ndx)
{
    int soft_reset_is_requiered = 0;
    int time_to_wait_for_dram_clearing = 0;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d\n"), FUNCTION_NAME(), drc_ndx));
    
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() forcing traffic into OCB\n"), FUNCTION_NAME()));
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, FORCE_OCB_FOR_NON_ELIGIBLEf, 1));
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, IDR_BUFFER_CHANGE_CONFIGURATIONr, REG_PORT_ANY, ENABLE_BUFFER_CHANGEf, 0));
    
    
    time_to_wait_for_dram_clearing = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mili_seconds_to_wait_for_dram_clearing", 10000);
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() waiting %d mili seconds for dram to empty\n"), FUNCTION_NAME(), time_to_wait_for_dram_clearing));
    sal_msleep(time_to_wait_for_dram_clearing);
    
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() trying corrective action\n"), FUNCTION_NAME()));
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_rd_wr_crc_corrective_action(unit, drc_ndx, &soft_reset_is_requiered));
    if (soft_reset_is_requiered)
    {
        
        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() Run Soft reset\n"), FUNCTION_NAME()));
        SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_device_reset,(unit, SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET)));
    }
    
    
    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() enable traffic back to Dram\n"), FUNCTION_NAME()));
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, FORCE_OCB_FOR_NON_ELIGIBLEf, 0));
    
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, IDR_BUFFER_CHANGE_CONFIGURATIONr, REG_PORT_ANY, ENABLE_BUFFER_CHANGEf, 1));    
        
exit:
    SOCDNX_FUNC_RETURN;
}




int soc_dpp_drc_combo28_dynamic_calibration_handle_cdr_interrupt(int unit, int drc_ndx)
{
    int i;
    soc_dpp_drc_combo28_info_t *drc_info;
    uint32 dq_byte_write_max_vdl_data_addr[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES] = {0x00a, 0x10a, 0x20a, 0x30a};
    int fsm_vdl_total_drift[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];

    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d\n"), FUNCTION_NAME(), drc_ndx));

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;

    
    soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 0);

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_update_dram_reference_fsm_vdl(unit, drc_ndx, drc_info, fsm_vdl_total_drift));

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_read_direction", 0)) {
        SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_change_fsm_vdl(unit, drc_ndx, fsm_vdl_total_drift));
    }

    
    soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 1);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_dynamic_calibration_write_direction", 0)) {
        for (i = 0; i < SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES; ++i) 
        {            
            SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibrate_write_path(unit, drc_ndx, dq_byte_write_max_vdl_data_addr[i])); 
        } 
    }
exit:
    SOCDNX_FUNC_RETURN;
}





int soc_dpp_drc_combo28_gddr5_temp_get (int unit, int drc_ndx, soc_dpp_drc_gddr5_temp_t *gddr5_temp)
{    
    int current_temp;
    int max_temp;
    int min_temp;
    uint64  reg64_val;
    soc_dpp_drc_combo28_info_t* drc_info;

    static soc_reg_t DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[] = {DRCA_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCB_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCC_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCD_PERIODIC_DRAM_TEMP_SENS_CFGr,
                                                           DRCE_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCF_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCG_PERIODIC_DRAM_TEMP_SENS_CFGr, DRCH_PERIODIC_DRAM_TEMP_SENS_CFGr };
    static soc_reg_t DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[] = {DRCA_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCB_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCC_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCD_PERIODIC_DRAM_TEMP_SENS_STATUSr,
                                                              DRCE_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCF_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCG_PERIODIC_DRAM_TEMP_SENS_STATUSr, DRCH_PERIODIC_DRAM_TEMP_SENS_STATUSr };
    SOCDNX_INIT_FUNC_DEFS;

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved Args: drc_ndx=%d\n"), FUNCTION_NAME(), drc_ndx));

    SOCDNX_NULL_CHECK(gddr5_temp);

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;

    
    if (drc_info->dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dram type is not GDDR5 -> not supported by this function\n")));
    }

    
    if ( !SHR_BITGET(drc_info->dram_bitmap, drc_ndx) ){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dram index %d is not configured\n"), drc_ndx));
    }

    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], &reg64_val, TEMP_SENS_GEN_PRDf, 0x1fff);
#ifdef CRASH_RECOVERY_SUPPORT
    
    soc_hw_set_immediate_hw_access(unit);
#endif
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], REG_PORT_ANY, 0, reg64_val));

    
    SOCDNX_SAND_IF_ERR_EXIT( arad_polling( unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[drc_ndx], REG_PORT_ANY, 0, TEMP_SENS_DONEf, 1 ));
#ifdef CRASH_RECOVERY_SUPPORT
    
    soc_hw_restore_immediate_hw_access(unit);
#endif 

    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    current_temp = soc_reg64_field32_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[drc_ndx], reg64_val, LAST_TEMP_SENSf);
    max_temp = soc_reg64_field32_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[drc_ndx], reg64_val, MAX_TEMP_SENSf);
    min_temp = soc_reg64_field32_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_STATUSr[drc_ndx], reg64_val, MIN_TEMP_SENSf);


    
    
    gddr5_temp->current_temp = DRC_COMBO28_TRANSLATE_DRAM_TEMPERATURE_VALUE_ACCORDING_TO_JEDEC_SPEC(current_temp);
    gddr5_temp->max_temp     = DRC_COMBO28_TRANSLATE_DRAM_TEMPERATURE_VALUE_ACCORDING_TO_JEDEC_SPEC(max_temp);
    gddr5_temp->min_temp     = DRC_COMBO28_TRANSLATE_DRAM_TEMPERATURE_VALUE_ACCORDING_TO_JEDEC_SPEC(min_temp);

    
    SOCDNX_IF_ERR_EXIT( soc_reg64_get(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], REG_PORT_ANY, 0, &reg64_val));
    soc_reg64_field32_set(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], &reg64_val, TEMP_SENS_GEN_PRDf, 0x0);
    SOCDNX_IF_ERR_EXIT( soc_reg64_set(unit, DRC_PERIODIC_DRAM_TEMP_SENS_CFGr[drc_ndx], REG_PORT_ANY, 0, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}




int soc_dpp_drc_combo28_validate_dram_address (int unit, uint32 addr)
{
    uint32 nof_bank_bits;
    uint32 nof_dram_interface_bits;
    uint32 nof_columns;
    uint32 nof_column_bits;
    uint32 nof_row_bits;
    uint32 nof_rows;
    uint32 nof_bits_in_mask;
    uint32 bank;
    uint32 dram_interface;
    uint32 column;
    uint32 row;
    uint32 first_bit;
    soc_dpp_drc_combo28_info_t* drc_info;
    SOCDNX_INIT_FUNC_DEFS;

    drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;

    
    nof_bank_bits = 4;
    
    nof_dram_interface_bits = 3;
    
    nof_columns = drc_info->nof_columns;
    MAX_NOF_BITS_GET(nof_columns, nof_column_bits);
    nof_column_bits -= 6;
    
    nof_rows = drc_info->nof_rows;
    MAX_NOF_BITS_GET(nof_rows, nof_row_bits);
    nof_bits_in_mask = nof_bank_bits + nof_dram_interface_bits + nof_column_bits + nof_row_bits;

    if ((addr & SOC_SAND_ZERO_BITS_MASK(nof_bits_in_mask, 0)) != 0) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x is invalid - too long\n"), addr));
    }

    first_bit = 0;
    bank = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_bank_bits - 1, first_bit);
    if (bank >= drc_info->nof_banks) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid bank (bank %d)\n"), addr, bank));
    }

    first_bit += nof_bank_bits;
    dram_interface = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_dram_interface_bits - 1, first_bit);
    if (!SHR_BITGET(drc_info->dram_bitmap, dram_interface)) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid dram interface (dram interface %c)\n"), addr, 'A' + dram_interface));
    }

    first_bit += nof_dram_interface_bits;
    column = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_column_bits - 1, first_bit);
    if (column >= drc_info->nof_columns) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid column\n"), addr));
    }

    first_bit += nof_column_bits;
    row = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_row_bits - 1, first_bit);
    if (row >= drc_info->nof_rows) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid row\n"), addr));
    }

    LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved address: 0x%x directs to dram interface %c, bank number %d, column %d, row %d\n"), FUNCTION_NAME(), addr, 'A' + dram_interface, bank, column<<6, row));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_rd_wr_crc_interrupt_control (
    int         unit, 
    int         drc_index, 
    int         restore_saved_state,
    int         mask_interrupt )
{
    static soc_reg_t DRC_INTERRUPT_MASK_REGISTERl[] = { DRCA_INTERRUPT_MASK_REGISTERr, DRCB_INTERRUPT_MASK_REGISTERr, DRCC_INTERRUPT_MASK_REGISTERr, DRCD_INTERRUPT_MASK_REGISTERr,
                                                        DRCE_INTERRUPT_MASK_REGISTERr, DRCF_INTERRUPT_MASK_REGISTERr, DRCG_INTERRUPT_MASK_REGISTERr, DRCH_INTERRUPT_MASK_REGISTERr };
    static soc_reg_t DRC_INTERRUPT_REGISTERl[] = {  DRCA_INTERRUPT_REGISTERr, DRCB_INTERRUPT_REGISTERr, DRCC_INTERRUPT_REGISTERr, DRCD_INTERRUPT_REGISTERr, 
                                                    DRCE_INTERRUPT_REGISTERr, DRCF_INTERRUPT_REGISTERr, DRCG_INTERRUPT_REGISTERr, DRCH_INTERRUPT_REGISTERr };
    uint32 reg32_val;
    int wr_crc_mask;
    int rd_crc_mask;
    
    SOCDNX_INIT_FUNC_DEFS;
        
    if (mask_interrupt)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], REG_PORT_ANY, 0, &reg32_val));
        wr_crc_mask = soc_reg_field_get(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], reg32_val, DRAM_WR_CRC_ERR_MASKf);
        rd_crc_mask = soc_reg_field_get(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], reg32_val, DRAM_RD_CRC_ERR_MASKf);
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_rd_crc_interrupt_state.set(unit, drc_index, rd_crc_mask));
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_wr_crc_interrupt_state.set(unit, drc_index, wr_crc_mask));

        
        soc_reg_field_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], &reg32_val, DRAM_WR_CRC_ERR_MASKf, 0);
        soc_reg_field_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], &reg32_val, DRAM_RD_CRC_ERR_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], REG_PORT_ANY, 0, reg32_val));
    } 
    else if (restore_saved_state)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTERRUPT_REGISTERl[drc_index], REG_PORT_ANY, 0, &reg32_val));
        soc_reg_field_set(unit, DRC_INTERRUPT_REGISTERl[drc_index], &reg32_val, DRAM_WR_CRC_ERRf, 1);
        soc_reg_field_set(unit, DRC_INTERRUPT_REGISTERl[drc_index], &reg32_val, DRAM_RD_CRC_ERRf, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTERRUPT_REGISTERl[drc_index], REG_PORT_ANY, 0, reg32_val));
        
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], REG_PORT_ANY, 0, &reg32_val));
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_rd_crc_interrupt_state.get(unit, drc_index, &rd_crc_mask));
        SOCDNX_IF_ERR_EXIT(SWSTATE_JER_TM_ACCESS.dram_wr_crc_interrupt_state.get(unit, drc_index, &wr_crc_mask));
        soc_reg_field_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], &reg32_val, DRAM_WR_CRC_ERR_MASKf, wr_crc_mask);
        soc_reg_field_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], &reg32_val, DRAM_RD_CRC_ERR_MASKf, rd_crc_mask);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_INTERRUPT_MASK_REGISTERl[drc_index], REG_PORT_ANY, 0, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



static int soc_dpp_drc_combo28_ddr4_cpu_command(int unit, int drc_ndx, uint32 cpu_addr, uint32 bank, uint32 cs_n, uint32 we_n, uint32 cas_n, uint32 ras_n)
{
    static soc_reg_t
        DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                         DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                         DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
                                                         DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr},
        DRC_CPU_COMMANDSl[] = {DRCA_CPU_COMMANDSr, DRCB_CPU_COMMANDSr, DRCC_CPU_COMMANDSr,
                               DRCD_CPU_COMMANDSr, DRCE_CPU_COMMANDSr, DRCF_CPU_COMMANDSr,
                               DRCG_CPU_COMMANDSr, DRCH_CPU_COMMANDSr};

    uint32 cs_n_odd, we_n_odd, cas_n_odd, ras_n_odd, bank_odd, addr, addr_odd, act_n_abi, act_n_abi_odd;
    uint32 DrcInHalfDramCmdFreqMode;
    uint64 field64_data;
    soc_reg_above_64_val_t reg_above_64;
    uint32 parity_bus, parity, parity_bus_odd, parity_odd, i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, DRC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERl[drc_ndx], REG_PORT_ANY, 0, &field64_data));
    DrcInHalfDramCmdFreqMode = soc_reg64_field32_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, field64_data, DRC_IN_HALF_DRAM_CMD_FREQ_MODEf);
    if (DrcInHalfDramCmdFreqMode)
    {
        
        we_n_odd  = 1;
        cas_n_odd = 1;
        ras_n_odd = 1;
        cs_n_odd  = 3;
    } else {
        we_n_odd  = we_n;
        cas_n_odd = cas_n;
        ras_n_odd = ras_n;
        cs_n_odd  = cs_n;
    }
    
    addr = cpu_addr & 0x3FFF;
    addr_odd = addr;
    bank_odd = bank;
    act_n_abi = 1;
    act_n_abi_odd = 1;

    
    parity_bus = 0;
    parity_bus |= SOC_SAND_SET_BITS_RANGE(ras_n, 0, 0);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(cas_n, 1, 1);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(we_n, 2, 2);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(act_n_abi, 3, 3);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(addr, 17, 4);
    parity_bus |= SOC_SAND_SET_BITS_RANGE(bank, 21, 18);
    parity = 0;
    for (i=0 ; i < 23; i++) {
        parity ^= SOC_SAND_GET_BITS_RANGE(parity_bus, i ,i);
    }

    parity_bus_odd = 0;
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(ras_n_odd, 0, 0);
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(cas_n_odd, 1, 1);
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(we_n_odd, 2, 2);
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(act_n_abi_odd, 3, 3);
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(addr_odd, 17, 4);
    parity_bus_odd |= SOC_SAND_SET_BITS_RANGE(bank_odd, 21, 18);
    parity_odd = 0;
    for (i=0 ; i < 23; i++) {
        parity_odd ^= SOC_SAND_GET_BITS_RANGE(parity_bus_odd, i ,i);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, BANK_ADDRESSf, bank);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ADDRESSf, addr);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_Nf, we_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_Nf,  cas_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_Nf, ras_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, PARITYf,  parity);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_Nf, act_n_abi);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, BANK_ADDRESS_ODDf,  bank_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ADDRESS_ODDf,	addr_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_N_ODDf,  we_n_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_N_ODDf,  cas_n_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_N_ODDf,  ras_n_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, PARITY_ODDf,  parity_odd);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_N_ODDf, act_n_abi_odd);
    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CKEf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RESf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODTf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODT_ODDf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CONST_VALf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CMD_CONST_VALf, 0);
    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_Nf, cs_n);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_N_ODDf, cs_n_odd);
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));

    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CPU_CMD_VLDf, 0x0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));

    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CPU_CMD_VLDf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}


static int soc_dpp_drc_combo28_ddr4_zqcl(int unit, int drc_ndx)
{
    soc_reg_t DRC_GENERAL_CONFIGURATIONSl[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr,
                                               DRCD_GENERAL_CONFIGURATIONSr, DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr,
                                               DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr};
    uint32 cpu_addr = 0, reg_val, ap_bit_pos;
    uint32 cs_n;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GENERAL_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    ap_bit_pos = soc_reg_field_get(unit, DRCA_GENERAL_CONFIGURATIONSr, reg_val, AP_BIT_POSf);
    if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_10) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  10, 10);
    } else if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_8) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  8, 8);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Undefined Ap bit.\n")));
    }

    
    cs_n  = 2;
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_cpu_command(unit, drc_ndx, cpu_addr, 0, cs_n, 0, 1, 1));
    
    sal_usleep(15);
    
    cs_n  = 1;
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_cpu_command(unit, drc_ndx, cpu_addr, 0, cs_n, 0, 1, 1));
    
    sal_usleep(15);

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_dpp_drc_combo28_ddr4_precharge_all(int unit, int drc_ndx)
{
    soc_reg_t DRC_GENERAL_CONFIGURATIONSl[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr, DRCC_GENERAL_CONFIGURATIONSr,
                                               DRCD_GENERAL_CONFIGURATIONSr, DRCE_GENERAL_CONFIGURATIONSr, DRCF_GENERAL_CONFIGURATIONSr,
                                               DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr};
    uint32 cpu_addr = 0, reg_val, ap_bit_pos;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_GENERAL_CONFIGURATIONSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    ap_bit_pos = soc_reg_field_get(unit, DRCA_GENERAL_CONFIGURATIONSr, reg_val, AP_BIT_POSf);
    if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_10) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  10, 10);
    } else if (ap_bit_pos == SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_8) {
        cpu_addr |= SOC_SAND_SET_BITS_RANGE(0x1,  8, 8);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Undefined Ap bit.\n")));
    }

    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_cpu_command(unit, drc_ndx, cpu_addr, 0, 0, 0, 1, 0));

exit:
    SOCDNX_FUNC_RETURN;
}


static int soc_dpp_drc_combo28_ddr4_load_mrs(int unit, int drc_ndx, uint32 mr_index, uint32 mrs_opcode)
{
    uint32 cpu_addr, bank;
    SOCDNX_INIT_FUNC_DEFS;

    cpu_addr = mrs_opcode & 0xFFFF;
    bank = mr_index;

    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_cpu_command(unit, drc_ndx, cpu_addr, bank, 0, 0, 0, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_dpp_drc_combo28_ddr4_auto_refresh(int unit, int drc_ndx)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_cpu_command(unit, drc_ndx, 0, 0, 0, 1, 0, 0));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_drc_combo28_shmoo_init_ddr4(int unit, int drc_ndx)
{
    uint32 reg_val;
    static soc_reg_t
        DRC_CPU_COMMANDSl[] = {DRCA_CPU_COMMANDSr, DRCB_CPU_COMMANDSr, DRCC_CPU_COMMANDSr, DRCD_CPU_COMMANDSr, 
                               DRCE_CPU_COMMANDSr, DRCF_CPU_COMMANDSr,DRCG_CPU_COMMANDSr, DRCH_CPU_COMMANDSr},
        DRC_TRAIN_INIT_TRIGERSl[] = {DRCA_TRAIN_INIT_TRIGERSr, DRCB_TRAIN_INIT_TRIGERSr, DRCC_TRAIN_INIT_TRIGERSr, DRCD_TRAIN_INIT_TRIGERSr,
                                     DRCE_TRAIN_INIT_TRIGERSr, DRCF_TRAIN_INIT_TRIGERSr, DRCG_TRAIN_INIT_TRIGERSr, DRCH_TRAIN_INIT_TRIGERSr};

    soc_reg_above_64_val_t reg_above_64;
    soc_dpp_drc_combo28_info_t *drc_info = &SOC_DPP_CONFIG(unit)->arad->init.drc_info;
    SOCDNX_INIT_FUNC_DEFS;


    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CKEf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RESf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_Nf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_Nf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_Nf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_Nf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_Nf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODTf, 0);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, WE_N_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CAS_N_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RAS_N_OR_REF_N_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CS_N_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, PARITY_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ACT_N_ABI_N_ODDf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, ODT_ODDf, 0);
     
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CONST_VALf, 1);
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CMD_CONST_VALf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, cpu_ddr4_init RES low\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(500);

    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, RESf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, cpu_ddr4_init RES high CKE low\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(500);
    
    soc_reg_above_64_field32_set(unit, DRCA_CPU_COMMANDSr, reg_above_64, CKEf, 1);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, DRC_CPU_COMMANDSl[drc_ndx], REG_PORT_ANY, 0, reg_above_64));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, CKE high\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(1);

    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Load MRs\n"), FUNCTION_NAME(), drc_ndx));
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 3, drc_info->mr[drc_ndx][3]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 6, drc_info->mr[drc_ndx][6]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 5, drc_info->mr[drc_ndx][5]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 4, drc_info->mr[drc_ndx][4]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 2, drc_info->mr[drc_ndx][2]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 1, drc_info->mr[drc_ndx][1]));
    sal_usleep(1);
    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_load_mrs(unit, drc_ndx, 0, drc_info->mr[drc_ndx][0]));
    sal_usleep(1);

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_zqcl(unit, drc_ndx));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Issue ZQCL\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(1);

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_precharge_all(unit, drc_ndx));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Pre-Charge All\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(1);

    
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_ddr4_auto_refresh(unit, drc_ndx));
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, Auto Refresh\n"), FUNCTION_NAME(), drc_ndx));
    sal_usleep(1);

    
    LOG_VERBOSE(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): drc_ndx=%d, force init finished\n"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_TRAIN_INIT_TRIGERSr, &reg_val, FORCE_DRAM_INIT_DONEf, 0x1);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, DRC_TRAIN_INIT_TRIGERSl[drc_ndx], REG_PORT_ANY, 0, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

