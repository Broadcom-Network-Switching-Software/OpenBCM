/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 PORT
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
 
#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#ifdef BCM_88950_A0

#include <soc/dfe/fe3200/fe3200_port.h>
#include <soc/dfe/fe3200/fe3200_drv.h>
#include <soc/dfe/fe3200/fe3200_defs.h>

#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_config_imp_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>

#include <soc/dcmn/dcmn_port.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>

#include <soc/portmod/portmod.h>
#include <phymod/phymod_acc.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/phy/phymod_port_control.h>

void *portmod_user_access_data[SOC_MAX_NUM_DEVICES] = {NULL};

extern unsigned char  tscf_ucode[];
extern unsigned short tscf_ucode_len;


soc_error_t 
soc_fe3200_port_soc_init(int unit)
{
    soc_port_t port_i;
    uint32 lcpll, puc, pll_control;
    int sync_e_master_port, sync_e_slave_port, sync_e_master_divider;
    soc_reg_above_64_val_t reg_above_64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    for (port_i = 0; port_i < SOC_FE3200_NOF_LINKS ; ++port_i) {
        lcpll = INT_DEVIDE(port_i, SOC_FE3200_NOF_PORT_IN_LCPLL);


        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, ECI_POWERUP_CONFIGr, REG_PORT_ANY, 0, &puc));
        if(!SHR_BITGET(&puc,SOC_FE3200_PORT_ECI_POWER_UP_CONFIG_STATIC_PLL_BIT) || SAL_BOOT_PLISIM) {
            
            switch(lcpll) {
            case 0:
                SOCDNX_IF_ERR_EXIT(READ_ECI_MISC_PLL_0_CONFIGr(unit, reg_above_64_val));
                break;
            case 1:
                SOCDNX_IF_ERR_EXIT(READ_ECI_MISC_PLL_1_CONFIGr(unit, reg_above_64_val));
                break;
            case 2:
                SOCDNX_IF_ERR_EXIT(READ_ECI_MISC_PLL_2_CONFIGr(unit, reg_above_64_val));
                break;
            case 3:
            default:
                SOCDNX_IF_ERR_EXIT(READ_ECI_MISC_PLL_3_CONFIGr(unit, reg_above_64_val));
                break;
            }
            SOC_REG_ABOVE_64_WORD_GET(reg_above_64_val, &pll_control, 0);
            if(SOC_FE3200_PORT_PLL_CONFIG_OUT_125_MHZ_WORD_0 == pll_control) { 
                SOC_INFO(unit).port_refclk_int[port_i] = 125;
            } else if(SOC_FE3200_PORT_PLL_CONFIG_OUT_156_25_MHZ_WORD_0 == pll_control){ 
                SOC_INFO(unit).port_refclk_int[port_i] = 156;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Unrecognized LC-PLL value")));
            }
        } else {
            
            switch(lcpll) {
            case 0:
                SOC_INFO(unit).port_refclk_int[port_i] = SHR_BITGET(&puc,0)  ? 125 : 156;
                break;
            case 1:
                SOC_INFO(unit).port_refclk_int[port_i] = SHR_BITGET(&puc,2) ? 125 : 156;
                break;
            case 2:
                SOC_INFO(unit).port_refclk_int[port_i] = SHR_BITGET(&puc,4) ? 125 : 156;
                break;
            case 3:
            default:
                SOC_INFO(unit).port_refclk_int[port_i] = SHR_BITGET(&puc,6) ? 125 : 156;
                break;
            }
        }

        if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) || SOC_DFE_IS_FE2(unit)) 
        {
            if (port_i >= 104 && port_i <= 107 && SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_26 != -1)
            {
                SOC_INFO(unit).port_refclk_int[port_i] = SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_26 ? 156 : 125;
            }
            else if (port_i >=140 && port_i <= 143 && SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_35 != -1)
            {
                SOC_INFO(unit).port_refclk_int[port_i] = SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_35 ? 156 : 125;
            }

        }
    }


    if(!SOC_WARM_BOOT(unit)) {

        

        sync_e_master_port = soc_dfe_property_suffix_num_get(unit, 0, spn_SYNC_ETH_CLK_TO_PORT_ID_CLK, "", 1, SOC_DFE_PROPERTY_UNAVAIL);
        sync_e_slave_port  = soc_dfe_property_suffix_num_get(unit, 1, spn_SYNC_ETH_CLK_TO_PORT_ID_CLK, "", 1, SOC_DFE_PROPERTY_UNAVAIL);
    
        if (sync_e_master_port != SOC_DFE_PROPERTY_UNAVAIL)
        {
            SOCDNX_IF_ERR_EXIT(soc_fe3200_port_sync_e_link_set(unit, 1, sync_e_master_port));
        }
    
        if (sync_e_slave_port != SOC_DFE_PROPERTY_UNAVAIL)
        {
            SOCDNX_IF_ERR_EXIT(soc_fe3200_port_sync_e_link_set(unit, 0, sync_e_slave_port));
        }

        sync_e_master_divider =  soc_dfe_property_suffix_num_get(unit, 0, spn_SYNC_ETH_CLK_DIVIDER, "", 0, SOC_DFE_PROPERTY_UNAVAIL);

        SOCDNX_IF_ERR_EXIT(soc_fe3200_port_sync_e_divider_set(unit, sync_e_master_divider));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_sync_e_link_set(int unit, int is_master, int port)
{
    int fsrd_block;
    uint32 reg_val,reg_val2; 
    SOCDNX_INIT_FUNC_DEFS;

    if ((port < 0) || (port >= SOC_FE3200_NOF_LINKS))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid port for syncE configuration")));
    }

    if (!(SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid port for syncE configuration")));
    }

    fsrd_block = port/SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd);

    SOCDNX_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));

    if (is_master)
    {
        
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, FABRIC_MASTER_SYNC_E_SELf, fsrd_block);
            
        soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val2, MASTER_CLK_SELf,  port % SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd));

    }
    else
    {
        
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, FABRIC_SLAVE_SYNC_E_SELf, fsrd_block);
        
        soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val2, SLAVE_CLK_SELf,  port % SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd));
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, reg_val));
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, fsrd_block, reg_val2));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_sync_e_divider_set(int unit, int divider)
{
    uint32 reg_val;
    int phase1_divider, i;
    int disabled;
    SOCDNX_INIT_FUNC_DEFS;

    if (!((divider >= SOC_FE3200_PORT_SYNC_E_MIN_DIVIDER) && (divider <= SOC_FE3200_PORT_SYNC_E_MAX_DIVIDER)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid divider for syncE configuration, must be between 2 to 16")));
    }

    phase1_divider = divider-1;
 
    
    for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); i++)
    {
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_quad_disabled, (unit, i * SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd), &disabled)));
        if (!disabled)
        {
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, i, &reg_val));
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val, SYNC_E_CLK_DIV_PHASE_1f, phase1_divider);
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val, SYNC_E_CLK_DIV_PHASE_0f, phase1_divider/2);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, i, reg_val));
        }

    }

exit:
    SOCDNX_FUNC_RETURN;    
}

soc_error_t
soc_fe3200_port_sync_e_link_get(int unit, int is_master, int *port)
{
    uint32 reg_val, reg_val2; 
    int fsrd_block;
    int inner_fsrd_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));

    if (is_master)
    {
        fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_MASTER_SYNC_E_SELf);  
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));  
        inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, MASTER_CLK_SELf);   
    }
    else
    {
        fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_SLAVE_SYNC_E_SELf); 
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));  
        inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, SLAVE_CLK_SELf);   
    }

    *port = fsrd_block*SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd) + inner_fsrd_port;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_sync_e_divider_get(int unit, int *divider)
{
    uint32 reg_val, reg_val2;
    int fsrd_block;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));
    fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_MASTER_SYNC_E_SELf);
    SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));
    *divider = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, SYNC_E_CLK_DIV_PHASE_1f) + 1; 

exit:
    SOCDNX_FUNC_RETURN;    

}


STATIC
int soc_fe3200_core_address_get(int unit, int core_index, uint16 *phy_addr){
    uint16 bus_id = 0;
    uint16 port = 0;

    
    bus_id = (core_index / 18) * 3 + 1 ; 
    bus_id += ((core_index % 9) / 3);
    
    port = ((core_index % 18) / 9) * 12 + ((core_index % 3) * 4);
    
    
    *phy_addr = 0x80 | ((bus_id & 0x3)<<PHY_ID_BUS_LOWER_SHIFT) | ((bus_id & 0xc)<<PHY_ID_BUS_UPPER_SHIFT) | port;

    return SOC_E_NONE;
}

STATIC soc_error_t
soc_fe3200_port_config_get(int unit, soc_port_t port, int is_init_sequence, dcmn_port_fabric_init_config_t* port_config)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    port_config->pcs = PORTMOD_PCS_64B66B_RS_FEC;
    port_config->speed = 25000;
    port_config->cl72 = 1;

    
    if(is_init_sequence) {
        SOCDNX_IF_ERR_EXIT(soc_dcmn_port_config_get(unit, port, port_config));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{

    int rc = SOC_E_NONE, is_first_link;
    soc_port_t port;
    dcmn_port_fabric_init_config_t port_config;
    phymod_firmware_load_method_t fw_load_method = phymodFirmwareLoadMethodNone;
    int broadcast_load = 0;
    dcmn_port_init_stage_t stage;
    int fw_verify = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_init_sequence) {
        fw_load_method = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
        if (!SOC_IS_RELOADING(unit)) 
        {
            fw_verify = (fw_load_method & 0xff00 ? 1 : 0); 
        }
        fw_load_method &= 0xff;


        
        if(fw_load_method == phymodFirmwareLoadMethodExternal &&
           !(dcmn_device_block_for_feature(unit,DCMN_FABRIC_12_QUADS_FEATURE)) &&
           !(dcmn_device_block_for_feature(unit,DCMN_FABRIC_24_QUADS_FEATURE)) &&
           !(dcmn_device_block_for_feature(unit,DCMN_FABRIC_18_QUADS_FEATURE))) {
             broadcast_load = 1;
        }

    } else {
        fw_load_method = phymodFirmwareLoadMethodInternal;
        broadcast_load = 0;
        fw_verify = 1;
    }

    
    SOC_PBMP_ITER(pbmp, port) {
        
        rc = soc_fe3200_port_first_link_in_fsrd_get(unit, port , &is_first_link, 1);
        SOCDNX_IF_ERR_EXIT(rc);

        if (is_first_link == 1)
        {
            rc = soc_fe3200_port_fsrd_block_enable_set(unit, port, 1);
            SOCDNX_IF_ERR_EXIT(rc);
        }

        if (!is_init_sequence)
        {
            rc = soc_fe3200_port_dynamic_soc_init(unit, port, is_first_link);
            SOCDNX_IF_ERR_EXIT(rc);
        }

        rc = soc_fe3200_port_config_get(unit, port, is_init_sequence, &port_config);
        SOCDNX_IF_ERR_EXIT(rc);

        stage = broadcast_load ? dcmn_port_init_until_fw_load : dcmn_port_init_full;
        rc= soc_dcmn_fabric_port_probe(unit, port, stage, fw_verify, &port_config);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    if(!SOC_WARM_BOOT(unit) && broadcast_load)
    {
        rc = soc_dcmn_fabric_broadcast_firmware_loader(unit, tscf_ucode_len, tscf_ucode);
        SOCDNX_IF_ERR_EXIT(rc);
    }
        
    SOC_PBMP_ITER(pbmp, port) {

        if(broadcast_load)
        {
            rc = soc_fe3200_port_config_get(unit, port, is_init_sequence, &port_config);
            SOCDNX_IF_ERR_EXIT(rc);

            rc= soc_dcmn_fabric_port_probe(unit, port, dcmn_port_init_resume_after_fw_load, fw_verify, &port_config);
            SOCDNX_IF_ERR_EXIT(rc);
        }

        SOC_PBMP_PORT_ADD(*okay_pbmp, port);

        
        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi));
        SOCDNX_IF_ERR_EXIT(rc);

        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port));            
        SOCDNX_IF_ERR_EXIT(rc);

        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all));            
        SOCDNX_IF_ERR_EXIT(rc);

        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi_disabled));            
        SOCDNX_IF_ERR_EXIT(rc);

        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port_disabled));            
        SOCDNX_IF_ERR_EXIT(rc);

        rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all_disabled));            
        SOCDNX_IF_ERR_EXIT(rc); 
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_fe3200_port_fsrd_block_enable_set(int unit, soc_port_t port, int enable)
{
    int fsrd_block, fmac_block, fmac_block_idx;
    uint32 field_val;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    
    fsrd_block = port/SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd);

    fmac_block = fsrd_block * SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd);

    
    if (!SOC_WARM_BOOT(unit))
    {
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_POWER_DOWNr(unit, &reg_val64));

        field_val = enable? 0: 1;

        switch (fsrd_block)
        {
            case 0:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_4f, field_val);
                break;
            case 1:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_5f, field_val);
                break;
            case 2:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_6f, field_val);
                break;
            case 3:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_7f, field_val);
                break;
            case 4:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_8f, field_val);
                break;
            case 5:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_9f, field_val);
                break;
            case 6:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_10f, field_val);
                break;
            case 7:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_11f, field_val);
                break;
            case 8:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_12f, field_val);
                break;
            case 9:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_13f, field_val);
                break;
            case 10:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_14f, field_val);
                break;
            case 11:
                soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_15f, field_val);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid fsrd block")));
                break;
        }

        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_POWER_DOWNr(unit, reg_val64));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_fe3200_port_update_valid_block_database(unit, SOC_BLK_FSRD, fsrd_block, enable));

    for (fmac_block_idx = fmac_block; fmac_block_idx < fmac_block +  SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd) ; fmac_block_idx++)
    {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_port_update_valid_block_database(unit, SOC_BLK_FMAC, fmac_block_idx, enable));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link, int enable)
{
    int fsrd_block, i;
    int counter_mask_fsrd_bmp = 0;
    pbmp_t mask_fsrd_bmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(mask_fsrd_bmp);

    fsrd_block = port/SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd);

    for (i=fsrd_block*SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd) ; i<fsrd_block*SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd) + SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd); i++) 
    {
        SOC_PBMP_PORT_ADD(mask_fsrd_bmp, i);
    }

    SOC_PBMP_AND(mask_fsrd_bmp, PBMP_SFI_ALL(unit)); 

    SOC_PBMP_COUNT(mask_fsrd_bmp, counter_mask_fsrd_bmp);

    

    if (counter_mask_fsrd_bmp==0) 
    {
        *is_first_link = 1;
    }
    else
    {
        *is_first_link = 0;
    }

    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
soc_fe3200_dynamic_port_restore(int unit)
{
    int rc;
    int port, valid;
    SOCDNX_INIT_FUNC_DEFS;

    for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links) ; port++) {

        SOCDNX_IF_ERR_EXIT(portmod_port_is_valid(unit, port, &valid));

        if (valid)
        {
            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all));
            SOCDNX_IF_ERR_EXIT(rc);

            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

        }
        else
        {
            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all_disabled));
            SOCDNX_IF_ERR_EXIT(rc);

            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all));
            SOCDNX_IF_ERR_EXIT(rc);

        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_port_init(int unit)
{
    int rc;
    int quads_in_fsrd;
    portmod_pm_instances_t pm_types_and_instances[] = {{portmodDispatchTypeDnx_fabric, SOC_FE3200_NOF_INSTANCES_MAC}};
    SOCDNX_INIT_FUNC_DEFS;
    
    if(portmod_user_access_data[unit] != NULL){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("unit user data memory already allocated")));
    }

    quads_in_fsrd = SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd);

    
    rc = portmod_create(unit, 0, SOC_FE3200_NOF_LINKS, SOC_FE3200_NOF_LINKS, 1 , pm_types_and_instances);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_dcmn_fabric_pms_add(unit, SOC_FE3200_NOF_INSTANCES_MAC, 0, FALSE, quads_in_fsrd, soc_fe3200_core_address_get, &portmod_user_access_data[unit]);
    SOCDNX_IF_ERR_EXIT(rc);


    if (SOC_WARM_BOOT(unit))
    {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_dynamic_port_restore(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_deinit(int unit)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(portmod_user_access_data[unit] != NULL){
        sal_free(portmod_user_access_data[unit]);
        portmod_user_access_data[unit] = NULL;
    }
    rc = portmod_destroy(unit);
    SOCDNX_IF_ERR_EXIT(rc);
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_detach(int unit, soc_port_t port)
{

    int rc, is_first_link;
    SOCDNX_INIT_FUNC_DEFS;


    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi));    
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port));
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all));    
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi_disabled));    
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port_disabled));    
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all_disabled));    
    SOCDNX_IF_ERR_EXIT(rc);
    
    rc = portmod_port_remove(unit, port);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_fe3200_port_first_link_in_fsrd_get(unit, port , &is_first_link, 0);

    if (is_first_link == 1) 
    {
        rc = soc_fe3200_port_fsrd_block_enable_set(unit, port, 0);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_speed_max(int unit, soc_port_t port, int *speed)
{
    SOCDNX_INIT_FUNC_DEFS;
    *speed=SOC_FE3200_PORT_SPEED_MAX;

    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_phy_enable_set(int unit, soc_port_t port, int enable)
{
    int portmod_enable_flags=0;
    SOCDNX_INIT_FUNC_DEFS;

    PORTMOD_PORT_ENABLE_MAC_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_PHY_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_TX_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_RX_SET(portmod_enable_flags);

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_set(unit, port, portmod_enable_flags, enable));
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_phy_enable_get(int unit, soc_port_t port, int *enable)
{
    int portmod_enable_flags=0;
    SOCDNX_INIT_FUNC_DEFS;

    PORTMOD_PORT_ENABLE_MAC_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_PHY_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_TX_SET(portmod_enable_flags);
    PORTMOD_PORT_ENABLE_RX_SET(portmod_enable_flags);

    SOCDNX_IF_ERR_EXIT(portmod_port_enable_get(unit, port, portmod_enable_flags, enable));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_speed_get(int unit, soc_port_t port, int *speed)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
    *speed = config.speed;
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_fe3200_port_speed_set(int unit, soc_port_t port, int speed)
{
    portmod_port_interface_config_t config;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config, 
                                          PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
    config.speed = speed;
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config, 
                                          PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t
soc_fe3200_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1, i;
    portmod_access_get_params_t params;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));

    for (i=0 ; i<phys_returned ; i++) {
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[i], 0 , ref_clk, &phy_config));
        PHYMOD_INTF_MODES_FIBER_CLR(&phy_config);
        PHYMOD_INTF_MODES_COPPER_CLR(&phy_config);
        switch (intf) {
        case SOC_PORT_IF_SR:
            PHYMOD_INTF_MODES_FIBER_SET(&phy_config);
            break;
        case SOC_PORT_IF_CR:
            PHYMOD_INTF_MODES_COPPER_SET(&phy_config);
            break;
        default:
            break;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(&phys[i], 0 ,&phy_config));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1;
    portmod_access_get_params_t params;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DCMN_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SOCDNX_IF_ERR_EXIT(soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[0], 0 , ref_clk, &phy_config));

    if (PHYMOD_INTF_MODES_FIBER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_SR;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_CR;
    } else {
        *intf = SOC_PORT_IF_KR;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_burst_control_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback)
{
    int fmac_block, fmac_inner_link;
    uint32 reg32_val;
    soc_dfe_fabric_link_device_mode_t link_mode;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC)));

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg32_val));
    if (loopback == soc_dcmn_loopback_mode_mac_outer || loopback == soc_dcmn_loopback_mode_mac_async_fifo)
    {
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_MAC_LOOPBACK);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_MAC_LOOPBACK);
    } else {
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, 0, &link_mode)));
        
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        switch (link_mode)
        {
            case soc_dfe_fabric_link_device_mode_multi_stage_fe1:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE1);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE1);
                break;
            case soc_dfe_fabric_link_device_mode_multi_stage_fe3:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3);
                break;
            case soc_dfe_fabric_link_device_mode_fe2:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE2);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE2);
                break;
            case soc_dfe_fabric_link_device_mode_repeater:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid link mode")));
                break;
        }
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_port_serdes_power_disable(int unit , soc_pbmp_t disable_pbmp)
{
    int port, is_first_link = 0;
    SOCDNX_INIT_FUNC_DEFS;

    BCM_PBMP_ITER(disable_pbmp, port)
    {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_port_first_link_in_fsrd_get(unit, port , &is_first_link, 0));

        if (is_first_link) {
            SOCDNX_IF_ERR_EXIT(soc_fe3200_port_fsrd_block_enable_set(unit, port, 0));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_link_status_get(int unit , soc_port_t port, int *link_up)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, link_up));
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_extract_cig_from_llfc_enable_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Extract Congrstion Ind from LLFC cells contorl is supported only by KR_FEC")));
    }
    if(value){
        properties |= PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    } else{
        properties &= ~PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    }
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_port_extract_cig_from_llfc_enable_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Extract Congrstion Ind from LLFC cells contorl is supported only by KR_FEC")));
    }
    *value = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_update_valid_block_database(int unit, int block_type, int block_number, int enable)
{
    soc_info_t *si;
    int blk;

    SOCDNX_INIT_FUNC_DEFS;

    si = &SOC_INFO(unit);

    switch (block_type)
    {
        case SOC_BLK_FSRD:
            if(block_number < SOC_MAX_NUM_FSRD_BLKS) {
                blk = si->fsrd_block[block_number];
                si->block_valid[blk] = enable;
            }
            else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_drv_update_block_database: too much FSRD blocks")));
            }
            break;

        case SOC_BLK_FMAC:
            if(block_number < SOC_MAX_NUM_FMAC_BLKS) {
                blk = si->fmac_block[block_number];
                si->block_valid[blk] = enable;
            }
            else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_drv_update_block_database: too much FMAC blocks")));
            }
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("soc_dfe_drv_update_block_database: block not supported/recognized")));
            break;

    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_port_pump_enable_set(int unit, soc_port_t port, int enable)
{
    int blk, inner_link;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &blk, &inner_link, SOC_BLK_FMAC)));

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, blk, inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, enable ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, blk, inner_link, &reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_dynamic_soc_init(int unit, soc_port_t port, int is_first_link)
{
    uint32 reg32_val, field[1];
    int fmac_block, fmac_inner_link, blk, fmac_first_block, fsrd_block;
    soc_dfe_fabric_link_device_mode_t link_mode;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC)));
    fsrd_block = port/SOC_DFE_DEFS_GET(unit, nof_links_in_fsrd);
    fmac_first_block = fsrd_block * SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd);
    

    
    if (is_first_link)
    {
        for (blk = fmac_first_block; blk < fmac_first_block +  SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd) ; blk++)
        {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, blk, &reg32_val));
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_FILL_RATEf, SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_LINK_UP_THf, 0x20);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_LINK_DN_THf, 0x10);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, SIG_DET_BKT_RST_ENAf, 0x1);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, ALIGN_LCK_BKT_RST_ENAf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, blk, reg32_val));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, 0, &link_mode)));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
    switch (link_mode)
    {
        case soc_dfe_fabric_link_device_mode_multi_stage_fe1:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE1);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE1);
            break;
        case soc_dfe_fabric_link_device_mode_multi_stage_fe3:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3);
            break;
        case soc_dfe_fabric_link_device_mode_fe2:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE2);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE2);
            break;
        case soc_dfe_fabric_link_device_mode_repeater:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid link mode")));
            break;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg32_val));

    
    if(SOC_DFE_IS_FE13(unit)) {
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, 0, &link_mode)));
        if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
        {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_block, &reg32_val));
            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_0f);
            SHR_BITSET(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_0f, *field);
            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_1f);
            SHR_BITCLR(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_1f, *field);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_block, reg32_val));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, fmac_block, &reg32_val));
    *field = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg32_val, LNK_LVL_FC_RX_ENf);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg32_val, LNK_LVL_FC_RX_ENf, *field);
    *field = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg32_val, LNK_LVL_FC_RX_ENf);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg32_val, LNK_LVL_FC_RX_ENf, *field);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, fmac_block, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_block, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg32_val ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
    soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg32_val ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_block, fmac_inner_link, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_block, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_block, fmac_inner_link, &reg32_val));


    
    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_REG_0102r(unit, fmac_block, &reg32_val));
    *field = soc_reg_field_get(unit, FMAC_REG_0102r, reg32_val, FIELD_0_3f);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_REG_0102r, &reg32_val, FIELD_0_3f, *field);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_REG_0102r(unit, fmac_block, reg32_val));


    
    if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) || SOC_DFE_IS_FE2(unit)) 
    {
        if (is_first_link)
        {
            for (blk = fmac_first_block; blk < fmac_first_block +  SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd) ; blk++)
            {
                if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_26 != -1 && blk == 26)
                {
                    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, 8, 2, &reg32_val)); 
                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg32_val, SRD_QUAD_N_LCREF_ENf, 0);
                    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, 8, 2, reg32_val));       
                }
                if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_35 != -1 && blk == 35)
                {
                    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, 11, 2, &reg32_val)); 
                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg32_val, SRD_QUAD_N_LCREF_ENf, 0);
                    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, 11, 2, reg32_val));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_port_quad_disabled(int unit, int quad, int *disabled)
{
    int fsrd_id;

    SOCDNX_INIT_FUNC_DEFS;

    if (dcmn_device_block_for_feature(unit,DCMN_FABRIC_18_QUADS_FEATURE)) {
        if ((quad>=6 && quad<=8)||(quad>=15 && quad<=17)||(quad>=21 && quad<=26)||(quad>=30 && quad<=35)) {
            *disabled = 1;
        } else {
            *disabled = 0;
        }
    } else if (dcmn_device_block_for_feature(unit,DCMN_FABRIC_24_QUADS_FEATURE)) {
        
        fsrd_id = quad / SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd);
        if ((fsrd_id % 3 == 0) ||
            ((fsrd_id - 1) % 3 == 0))
        {
            *disabled = 0;
        } else {
            *disabled = 1;
        }
    } else if (dcmn_device_block_for_feature(unit,DCMN_FABRIC_12_QUADS_FEATURE)) {
        
        fsrd_id = quad / SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd);
        if (fsrd_id % 3 == 0)
        {
            *disabled = 0;
        } else {
            *disabled = 1;
        }

    } else {
        *disabled = 0;
    }

    SOCDNX_FUNC_RETURN; 
}

#endif 

#undef _ERR_MSG_MODULE_NAME
