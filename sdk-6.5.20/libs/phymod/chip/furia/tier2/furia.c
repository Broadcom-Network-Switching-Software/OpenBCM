/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_acc.h>

#include <phymod/chip/furia.h>

#include "../tier1/furia_regs_structs.h"
#include "../tier1/furia_reg_access.h"
#include "../tier1/furia_address_defines.h"
#include "../tier1/furia_micro_seq.h"
#include "../tier1/furia_types.h"
#include "../tier1/furia_cfg_seq.h"
#include "../tier1/furia_pkg_cfg.h"

#define FURIA_ID0       0xae02   
#define FURIA_ID1       0x5230  
#define FURIA_IF_SIDE_SHIFT (31)
#define FURIA_NOF_LANES_IN_CORE (4)
#define FURIA_PHY_ALL_LANES (0xf)
#define FURIA_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access,\
                       sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane = FURIA_PHY_ALL_LANES; \
    }while(0)

/* uController's firmware */
extern unsigned char furia_ucode_Furia[];
extern unsigned short furia_ucode_Furia_len;

int _furia_tx_rx_power_set(const phymod_access_t *pa,
                           phymod_power_t tx_rx_power,
                           uint8_t tx_rx);
/**   Core identify 
 *    This function reads device ID2 & ID3 registers and confirm whether furia
 *    device presents or not. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_core_identify(const phymod_core_access_t* core, uint32_t core_id,
                        uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_t id2;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_t id3; 
    uint32_t chip_id = 0;
    chip_id =  _furia_get_chip_id(pm_acc);

    *is_identified = 0;

    if(core_id == 0){
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_0_Adr,
                               &id2.data);  
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_1_Adr,
                               &id3.data);  
    } else {
        id2.data = (uint16_t) ((core_id >> 16) & 0xffff);
        id3.data = (uint16_t) core_id & 0xffff;
    }

    if (((id2.data) == FURIA_ID0 &&
        (id3.data) == FURIA_ID1) && (FURIA_IS_SIMPLEX(chip_id) || FURIA_IS_DUPLEX(chip_id))) {
        if (chip_id == FURIA_ID_82212) {
            /* PHY IDs match */
            *is_identified = 0;
        } else {
            /* PHY IDs match */
            *is_identified = 1;
        }
    }
        
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;    
}
  
/**   Core firmware download 
 *    This function downloads the firmware using either internal or external
 *    method as specified by the user. Firmware loader function supplied by user
 *    will be used for external download method. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param load_method        Load method will be specified by user 
 *    @param fw_loader          Loader function specified by user and will be
 *                              used for external download method
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC
int _furia_core_firmware_load(const phymod_core_access_t* core,
                              phymod_firmware_load_method_t load_method,
                              phymod_firmware_loader_f fw_loader)
{
    phymod_access_t pa;
    FUR_PAD_CTRL_EXT_UC_RSTB_CONTROL_t ext_uc_rstb_ctrl;
    FUR_MICRO_BOOT_BOOT_POR_t boot_por; 
    int ret_val = 0; 
    uint32_t chip_id = 0;
    uint32_t data = 0;
    phymod_tx_t tx_params;
    const phymod_access_t *pm_acc = &core->access;
    PHYMOD_MEMSET(&ext_uc_rstb_ctrl, 0 , sizeof(FUR_PAD_CTRL_EXT_UC_RSTB_CONTROL_t));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(FUR_MICRO_BOOT_BOOT_POR_t));
    PHYMOD_MEMSET(&tx_params, 0 , sizeof(phymod_tx_t));
    switch(load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DEBUG_VERBOSE((" Starting Firmware download through MDIO,  it takes few seconds...\n"));
            /* In case of FW Download is only for second die i.e for odd address of the die
             * To get the micro out of reset on even die(previous die); for MDIO only
             * Applicable for only duel die chips;
             */
            chip_id =  _furia_get_chip_id(pm_acc);
            if (FURIA_IS_DUAL_DIE_DUPLEX(chip_id)) { 
                if ((pm_acc->addr & 0x1) == 0x1) {
                    PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
                    /* modify the mdio address to be even */
                    pa.addr &= ~(0x1);
                    /* Set the 11th and 9th bit of even die to get the micro out of reset */
                    PHYMOD_IF_ERR_RETURN
                        (READ_FURIA_PMA_PMD_REG(&pa,\
                                                FUR_PAD_CTRL_ext_uc_rstb_control_Adr,\
                                                &ext_uc_rstb_ctrl.data));
                    ext_uc_rstb_ctrl.fields.out_frcval = 1;
                    ext_uc_rstb_ctrl.fields.ibof       = 1;
                    PHYMOD_IF_ERR_RETURN
                        (WRITE_FURIA_PMA_PMD_REG(&pa,\
                                                 FUR_PAD_CTRL_ext_uc_rstb_control_Adr,\
                                                 ext_uc_rstb_ctrl.data));
                }
            }
            ret_val = furia_download_prog_eeprom(pm_acc,
                                                 furia_ucode_Furia,
                                                 furia_ucode_Furia_len,
                                                 0);
            if ((ret_val != PHYMOD_E_NONE) && 
                (ret_val != FURIA_FW_ALREADY_DOWNLOADED) && 
                (ret_val != furia_ucode_Furia_len)) {
                PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_CONFIG,
                    (_PHYMOD_MSG("firmware download failed")));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Firmware download through MDIO success \n"));
            }
                  
        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;        
        case phymodFirmwareLoadMethodNone:
            /* Expected to download firmware from Flashed EEPROM */
            /* Firmware download works only when serboot pin is set on the chip */
            PHYMOD_IF_ERR_RETURN
                (READ_FURIA_PMA_PMD_REG(pm_acc,\
                                        FUR_MICRO_BOOT_boot_por_Adr,\
                                        &boot_por.data));
            if (boot_por.fields.serboot) {
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pm_acc,\
                                            FUR_MICRO_BOOT_boot_por_Adr,\
                                            &boot_por.data));
                /* check download_done flags again */
                if (boot_por.fields.mst_dwld_done != 1 || boot_por.fields.slv_dwld_done != 1) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                }
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pm_acc,\
                                            FUR_GEN_CNTRLS_firmware_version_Adr,\
                                            &data));
                if (data == 0x1) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                }
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pm_acc,\
                                            FUR_GEN_CNTRLS_mst_running_chksum_Adr,\
                                            &data));
                if (0x600D != data) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("Master Dload chksum Fail")));
                }
                PHYMOD_IF_ERR_RETURN
                    (READ_FURIA_PMA_PMD_REG(pm_acc,\
                                            FUR_GEN_CNTRLS_slv_running_chksum_Adr,\
                                            &data));
                if (0x600D != data) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("Slave Dload chksum Fail")));
                }
                /* Set Tx driver current, FIR main/pre/post default values 
                 *  Default values are 
                 *  driver current = 0xf
                 *  TX FIR main = 0x3c
                 *  TX FIR pre = 0x0
                 *  TX FIR post = 0x0
                 */ 

                PHYMOD_IF_ERR_RETURN
                    (furia_tx_get(pm_acc, &tx_params)); 
                tx_params.amp = 0xf;
                tx_params.pre = 0x0;
                tx_params.post = 0x0;
                tx_params.main = 0x3c;
                PHYMOD_IF_ERR_RETURN
                    (furia_tx_set(pm_acc, &tx_params));  

            } 
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            PHYMOD_DIAG_OUT(("Firmware will be downloaded first, and flashed on to EEPROM \n"));
            PHYMOD_DIAG_OUT(("This process will take few minutes.....\n"));
            ret_val = furia_download_prog_eeprom(pm_acc,
                                                 furia_ucode_Furia,
                                                 furia_ucode_Furia_len,
                                                 1);
            if ((ret_val != PHYMOD_E_NONE) &&
                (ret_val != FURIA_FW_ALREADY_DOWNLOADED) &&
                (ret_val != furia_ucode_Furia_len)) {
                PHYMOD_RETURN_WITH_ERR
                    (PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("Flasing Firmware to EEPROM faliled failed")));
            } else {
                PHYMOD_DIAG_OUT(("Firmware is flashed to EEPROM successfully\n"));
            }
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }
    return PHYMOD_E_NONE;
}

/**   Set Interface config 
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation). This is requried to put the furia into specific mode specified
 *    by the user.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use 
 *    @param config             Interface config structure where user specifies 
 *                              interface, speed and the frequency of operation.
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_interface_config_set(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   const phymod_phy_inf_config_t* config)
{
    return furia_set_config_mode(&phy->access,
                       config);
    
}

/**   Get Interface config 
 *    This function retrieves interface configuration(interface, speed and 
 *    frequency of operation) from the device. 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use 
 *    @param config             Interface config structure where  
 *                              interface, speed and the frequency of operation
 *                              will be populated from device .
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_interface_config_get(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   phymod_ref_clk_t ref_clock,
                                   phymod_phy_inf_config_t* config)
{
    return furia_get_config_mode(&phy->access,
                       &config->interface_type,
                       &config->data_rate,
                       &config->ref_clock,
                       &config->interface_modes, config, (uint16_t)(*(&phy->access.lane_mask)));
}

/**   Core Init  
 *    This function initializes the furia core and 
 *    download the firmware and setting the PHY into 
 *    default mode. 
 *
 *    @param phy                Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        Core status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_core_init(const phymod_core_access_t* core,
                    const phymod_core_init_config_t* init_config,
                    const phymod_core_status_t* core_status)
{
    uint32_t chip_id = 0;
    int num_lanes = 0;
    int rv = -1;
    int lane_index = 0;
    uint32_t phy_id_idx = 0;
    uint32_t exist_phy = 0;
    uint32_t is_write_disabled=0;
    phymod_phy_inf_config_t interface;
    phymod_access_t l_access;
    FUR_GEN_CNTRLS_micro_sync_7_t gen_ctrls_intf_type;
    FUR_GEN_CNTRLS_micro_sync_6_t gen_ctrls6_intf_type;
    FURIA_DEVICE_AUX_MODE_T aux_mode;
    FURIA_DEVICE_AUX_MODE_T *an_aux_mode = (FURIA_DEVICE_AUX_MODE_T*)init_config->interface.device_aux_modes;
    uint32_t *tx_source_array=NULL;
    phymod_phy_access_t l_phy_access;
    PHYMOD_MEMSET(&gen_ctrls_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_7_t));
    PHYMOD_MEMSET(&gen_ctrls6_intf_type, 0 , sizeof(FUR_GEN_CNTRLS_micro_sync_6_t));

    PHYMOD_MEMSET(&aux_mode, 0 , sizeof(FURIA_DEVICE_AUX_MODE_T));
    PHYMOD_MEMSET(&aux_mode, 0 , sizeof(FURIA_DEVICE_AUX_MODE_T));
    PHYMOD_MEMCPY(&l_access, &core->access, sizeof(phymod_access_t));
    PHYMOD_MEMSET(&l_phy_access, 0 , sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&(l_phy_access.access), &l_access, sizeof(phymod_access_t));
    /* Write is disabled in warm boot, get the status of of write disable flag */
    PHYMOD_IF_ERR_RETURN(
              phymod_is_write_disabled(&core->access, &is_write_disabled));

    aux_mode.avdd_txdrv = init_config->tx_input_voltage;
    aux_mode.ull_dp = init_config->op_datapath;

    interface.data_rate = init_config->interface.data_rate;
    interface.ref_clock = init_config->interface.ref_clock;
    interface.interface_type = init_config->interface.interface_type; 
    interface.pll_divider_req = init_config->interface.pll_divider_req;
    interface.interface_modes = init_config->interface.interface_modes;
    num_lanes = FURIA_IS_SIMPLEX(chip_id) ? 8 : 4;
    _furia_get_phy_id_idx(core->access.addr, &phy_id_idx, &exist_phy);
    chip_id =  _furia_get_chip_id(&core->access);
    if(!is_write_disabled){
        PHYMOD_IF_ERR_RETURN
            (_furia_core_firmware_load(core,
                                       init_config->firmware_load_method,
                                       init_config->firmware_loader));
        /* enable/disable ULL Datapath based on static configuration specified by the user */
        PHYMOD_IF_ERR_RETURN
                (_furia_cfg_fw_ull_dp(core, aux_mode.ull_dp));
        /* Select AVDD TXDRV voltage based on static configuration specified by the user */
        PHYMOD_IF_ERR_RETURN
            (_furia_core_avddtxdrv_config_set(core, aux_mode.avdd_txdrv));
        if(an_aux_mode){
            PHYMOD_IF_ERR_RETURN
                (_furia_cfg_an_master_lane(core, an_aux_mode->an_master_lane));
        }
        PHYMOD_IF_ERR_RETURN (
           _furia_core_cfg_pfifo_4_max_ieee_input_skew(&core->access,
               init_config->tx_fifo_sync_offset, init_config->rx_fifo_sync_offset));
        PHYMOD_IF_ERR_RETURN
            (_furia_pcs_monitor_enable_set(core, 0));
       
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(&core->access,\
                                    FUR_GEN_CNTRLS_micro_sync_7_Adr,\
                                    &gen_ctrls_intf_type.data));
		PHYMOD_IF_ERR_RETURN
			(READ_FURIA_PMA_PMD_REG(&core->access,\
                                FUR_GEN_CNTRLS_micro_sync_6_Adr,\
                                &gen_ctrls6_intf_type.data));


        if (FURIA_IS_DUPLEX(chip_id)) {
            PHYMOD_IF_ERR_RETURN
                (furia_fec_enable_set(&core->access, 0));
            /*clear sync7 register in hard reboot*/
            gen_ctrls_intf_type.fields.intf_type_ln0 = 0;
            gen_ctrls_intf_type.fields.intf_type_ln1 = 0;
            gen_ctrls_intf_type.fields.intf_type_ln2 = 0;
            gen_ctrls_intf_type.fields.intf_type_ln3 = 0;
            PHYMOD_IF_ERR_RETURN
                (WRITE_FURIA_PMA_PMD_REG(&core->access,\
                                         FUR_GEN_CNTRLS_micro_sync_7_Adr,\
                                         gen_ctrls_intf_type.data));
        }

        if (!exist_phy) {
            if (FURIA_IS_SIMPLEX(chip_id)) {
               if (chip_id == FURIA_ID_82212) {
                   if ((core->access.addr & 1) == 1) {
                       glb_phy_list[phy_id_idx].tx_lanes = 0xba5432;
                   } else {
                       glb_phy_list[phy_id_idx].tx_lanes = 0x987610;
                   }
               }
           }
        }
        if (FURIA_IS_SIMPLEX(chip_id)) {
            interface.data_rate = 25000;
            interface.interface_type = phymodInterfaceKR;
            l_access.lane_mask = 0xFF;
            l_access.flags &= (~(1 << 31));
            rv = furia_set_config_mode(&l_access, &interface);
            if(!rv) {
                for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                    gen_ctrls_intf_type.fields.intf_type_ln0 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln1 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln2 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln3 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln4 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln5 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln6 = 3;
                    gen_ctrls_intf_type.fields.intf_type_ln7 = 3;
                }
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(&core->access,\
                                             FUR_GEN_CNTRLS_micro_sync_7_Adr,\
                                             gen_ctrls_intf_type.data));
            } else {
                for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                    gen_ctrls_intf_type.fields.intf_type_ln0 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln1 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln2 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln3 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln4 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln5 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln6 = 0;
                    gen_ctrls_intf_type.fields.intf_type_ln7 = 0;
                }
                PHYMOD_IF_ERR_RETURN
                    (WRITE_FURIA_PMA_PMD_REG(&core->access,
                                             FUR_GEN_CNTRLS_micro_sync_7_Adr,
                                             gen_ctrls_intf_type.data));
            }
        }
		gen_ctrls6_intf_type.fields.ref_clk = 1;
	}
    if (!exist_phy) {
        if (FURIA_IS_SIMPLEX(chip_id)) {
            if(is_write_disabled){/* warmboot enabled */
                tx_source_array = (uint32_t *)PHYMOD_MALLOC( num_lanes * sizeof(uint32_t),"tx_source_array" );
                if(tx_source_array == NULL){
                   return PHYMOD_E_MEMORY;
                }
                rv = furia_phy_lane_cross_switch_map_get(&l_phy_access,tx_source_array);
                if (rv != PHYMOD_E_NONE){
                    PHYMOD_FREE(tx_source_array);
                    return rv;
                }
                for (lane_index = 0; lane_index < 8; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (tx_source_array[lane_index] << (4 * lane_index));
                }
                PHYMOD_FREE(tx_source_array);
            }else{ /* cold boot  */
                for (lane_index = 0; lane_index < 8; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (lane_index << (4 * lane_index));
                }
                if (chip_id == FURIA_ID_82212) {
                    if ((core->access.addr & 1) == 1) {
                        glb_phy_list[phy_id_idx].tx_lanes = 0xba5432;
                    } else {
                        glb_phy_list[phy_id_idx].tx_lanes = 0x987610;
                    }
                }
            }
        } else {
            if(is_write_disabled){ /* warmboot enabled */
                tx_source_array = (uint32_t *)PHYMOD_MALLOC( num_lanes * sizeof(uint32_t),"tx_source_array" );
                if(tx_source_array == NULL){
                   return PHYMOD_E_MEMORY;
                }
                rv = furia_phy_lane_cross_switch_map_get(&l_phy_access,tx_source_array);
                if (rv != PHYMOD_E_NONE){
                    PHYMOD_FREE(tx_source_array);
                    return rv;
                }
                for (lane_index = 0; lane_index < 4; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (tx_source_array[lane_index] << (4 * lane_index));
                }
                l_phy_access.access.flags = SIDE_B << FURIA_IF_SIDE_SHIFT;/*get array for the system side*/
                rv = furia_phy_lane_cross_switch_map_get(&l_phy_access,tx_source_array);
                if (rv != PHYMOD_E_NONE){
                    PHYMOD_FREE(tx_source_array);
                    return rv;
                }
                for (lane_index = 0; lane_index < 4; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (tx_source_array[lane_index] << (4 * (lane_index + 4)));
                }
                PHYMOD_FREE(tx_source_array);
            }else{ /* cold boot  */
                for (lane_index = 0; lane_index < 4; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (lane_index << (4 * lane_index));
                }
                for (lane_index = 0; lane_index < 4; lane_index ++) {
                    glb_phy_list[phy_id_idx].tx_lanes |= (lane_index << (4 * (lane_index + 4)));
                }
            }
        }
    }
    return PHYMOD_E_NONE;
}

/**   PHY init
 *    This function initializes PHY 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param init_config        User specified phy init configuration
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_init(const phymod_phy_access_t* phy,
                   const phymod_phy_init_config_t* init_config)
{
/*    int pll_restart = 0;
    const phymod_access_t *pm_acc = &phy->access;
*/

    PHYMOD_IF_ERR_RETURN
        (_furia_core_rptr_rtmr_mode_set(&phy->access, init_config->op_mode));

    PHYMOD_IF_ERR_RETURN
        (_furia_core_cfg_hcd_link_status_criteria(&phy->access, init_config->an_link_qualifier));

    PHYMOD_IF_ERR_RETURN
        (_furia_core_cfg_polarity_set(&phy->access, init_config->polarity.rx_polarity, init_config->polarity.tx_polarity));

    PHYMOD_IF_ERR_RETURN
        (_furia_core_cfg_tx_set(&phy->access, init_config->tx));
    return PHYMOD_E_NONE;
}

/**   Get PHY link status 
 *    This function retrieves PHY link status from device 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Link status retrieved from the device
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_link_status_get(const phymod_phy_access_t* phy,
                              uint32_t* link_status)
{
    return furia_link_status(&phy->access, link_status);
}

/**   PHY register read 
 *    This function reads the PHY register 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param reg_addr           PHY Register Address to Read
 *    @param val                PHY Register content of the specified 
 *                              register Address to Read
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr,
                       uint32_t *val)
{
    /*check if this is a slice reg read*/
    int rv = PHYMOD_E_NONE, rv1 = PHYMOD_E_NONE;
    if (phy->access.devad & FURIA_BIT(SLICE_REG_SET_BIT)) {
        rv1 = furia_slice_reg_set(&phy->access);
    }
    
    rv = furia_reg_read(&phy->access,\
                        FURIA_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                        val);
    if (phy->access.devad & FURIA_BIT(SLICE_REG_SET_BIT)) {
        if (rv1 == 0) {
            rv1 = furia_slice_reg_unset(&phy->access);
        }

   }
    return rv;
}


/**   PHY register write
 *    This function is used to write content to PHY register 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param reg_addr           PHY Register Address to Write 
 *    @param val                Content/Value to Write to the PHY Register 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr,
                        uint32_t val)
{
    /*check if this is a slice reg read*/
    int rv = PHYMOD_E_NONE, rv1 = PHYMOD_E_NONE;
    if (phy->access.devad & FURIA_BIT(SLICE_REG_SET_BIT)) {
        rv1 = furia_slice_reg_set(&phy->access);
    }
    rv = furia_reg_write(&phy->access,\
                         FURIA_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                         val);
    if (phy->access.devad & FURIA_BIT(SLICE_REG_SET_BIT)) {
        if (rv1 == 0) {
            rv1 = furia_slice_reg_unset(&phy->access);
        }

   }
    return rv;

}

/**   PHY core info get 
 *    This function is used to write content to PHY register 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param info               Pointer to core info structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t chip_id;
    chip_id =  _furia_get_chip_id(&phy->access);
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionFuriaA2;
    PHYMOD_STRNCPY(info->name, "FuriaA2", PHYMOD_STRLEN("FuriaA2")+1);
    info->phy_id0 = FURIA_ID0;
    info->phy_id1 = FURIA_ID1;
    return PHYMOD_E_NONE;
}
/**  PHY polarity set  
 *   This function is used to set the lane polarity 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return furia_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return furia_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  PHY Rx PMD lock 
 *   PHY Rx PMD lock This function is used to get the rx PMD lock status 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_seq_done        Pointer to rx sequence    
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){
    return furia_pmd_lock_get(&phy->access, rx_seq_done);
}

/**  PHY power set 
 *   PHY power set This function is used to set the power
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param power              Pointer to phymod phy power structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_power_set(const phymod_phy_access_t* phy,
                        const phymod_phy_power_t* power)
{
    /* Set Tx power */
    _furia_tx_rx_power_set(&phy->access, power->tx, 0);
    /* Set Rx Power */
    _furia_tx_rx_power_set(&phy->access, power->rx, 1);
    return PHYMOD_E_NONE;
}

int _furia_tx_rx_power_set(const phymod_access_t *pa,
                           phymod_power_t tx_rx_power,
                           uint8_t tx_rx)
{
    switch(tx_rx_power) {
        case phymodPowerOff:
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_power_set(pa, tx_rx, 0));
        break;
        case phymodPowerOn:
            PHYMOD_IF_ERR_RETURN
                (furia_tx_rx_power_set(pa, tx_rx, 1));
        break;
        case phymodPowerOffOn:
        break;
        case phymodPowerNoChange:
        break; 
        default:
        break; 
    }
    return PHYMOD_E_NONE;
}

/**  PHY power get 
 *   PHY power get This function is used to get the power 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param power              Pointer to phymod phy power structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    phymod_power_t power_tx = 0, power_rx = 0;
    PHYMOD_IF_ERR_RETURN    
        (furia_tx_rx_power_get(&phy->access, &power_tx, &power_rx));
    power->tx = power_tx;
    power->rx = power_rx; 
    return PHYMOD_E_NONE;
}

/**  PHY CL73 ability Set 
 *   This function is used to set the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param an_ability         Pointer to the ability of cl73 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    furia_an_ability_t value;
    PHYMOD_MEMSET(&value, 0, sizeof(value));

    value.cl73_adv.an_fec = an_ability->an_fec;
    value.cl73_adv.an_cl72 = an_ability->an_cl72;
    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return PHYMOD_E_PARAM;
    }

    /* Configure the user specified master lane */
    PHYMOD_IF_ERR_RETURN
        (_furia_cfg_an_master_lane((phymod_core_access_t*)phy, an_ability->an_master_lane));
    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = FURIA_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = FURIA_ASYM_PAUSE;
    } else {
        value.cl73_adv.an_pause = FURIA_NO_PAUSE;
    }

    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = FURIA_CL73_1000BASE_KX;
    } else if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = FURIA_CL73_10GBASE_KR;
    } else if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = FURIA_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = FURIA_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_CR10_GET(an_ability->an_cap)) { 
        return PHYMOD_E_UNAVAIL;
    } else if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) { 
        value.cl73_adv.an_base_speed = FURIA_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) { 
        value.cl73_adv.an_base_speed = FURIA_CL73_100GBASE_KR4;
    } else if (an_ability->an_cap == 0) {
        return PHYMOD_E_NONE;
    } else {
        return PHYMOD_E_PARAM;
    } 

    PHYMOD_IF_ERR_RETURN
        (_furia_autoneg_ability_set(&phy->access, &value));
    return PHYMOD_E_NONE;
    
}

/**  PHY CL73 ability get 
 *   This function is used to get the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param an_ability         Pointer to the ability of cl73 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    furia_an_ability_t value;

   PHYMOD_IF_ERR_RETURN(   
    _furia_autoneg_ability_get (&phy->access, &value));
   
    an_ability_get_type->an_fec = value.cl73_adv.an_fec;
    an_ability_get_type->an_master_lane = value.an_master_lane;
    switch (value.cl73_adv.an_pause) {
        case FURIA_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case FURIA_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (value.cl73_adv.an_base_speed) {
        case FURIA_CL73_100GBASE_CR10:
            PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_10GBASE_KR:
            PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_1000BASE_KX:
            PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case FURIA_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
        break;
        default:
            break;
    }
    return PHYMOD_E_NONE;
}

/**  PHY enable/Disable CL73  
 *   This function is used to enable/disable CL73 of a lane
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param an                 Pointer to the AN control  
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_furia_autoneg_set(&phy->access, an));
}

/**  Get CL73 completion state 
 *   This function is used get CL73 completion state of a lane
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param an                 Pointer to the AN control
 *    @param an_done            Pointer to the AN completion state
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_furia_autoneg_get(&phy->access, an, an_done));
}

/**  PHY CL73 remote ability get 
 *   This function is used to get the CL73 remote ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param an_ability         Pointer to the ability of cl73 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get)
{
    return (_furia_autoneg_remote_ability_get(&phy->access, an_ability_get));
}

/**  Get CL73 status 
 *   This function is used get CL73 resolved status
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param status             Pointer to AN resolved status
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}

/**  Set Core reset 
 *   This function is used to reset the core
 *
 *    @param core                Pointer to phymod core access structure
 *    @param reset_mode          Type of reset
 *        0 - Hard reset 
 *        1 - Soft reset
 *    @param direction           Reset direction
 *        0 - In
 *        1 - Out
 *        2 - In Out (toggle)
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_reset_set(const phymod_core_access_t* core,
                         phymod_reset_mode_t reset_mode,
                         phymod_reset_direction_t direction)
{
    return furia_reset_set(&core->access, reset_mode, direction); 
}
/**  Get Core reset 
 *   This function is used to get the status of core reset
 *
 *    @param core                Pointer to phymod core access structure
 *    @param reset_mode          Type of reset
 *        0 - Hard reset 
 *        1 - Soft reset
 *    @param direction           Reset direction
 *        0 - In
 *        1 - Out
 *        2 - In Out (toggle)
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_reset_get(const phymod_core_access_t* core,
                         phymod_reset_mode_t reset_mode,
                         phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;
}

/**  Tx set 
 *   This function is used to set transmitter analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return furia_tx_set(&phy->access, tx);
}

/**  Tx get 
 *   This function is used to get transmitter analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    return furia_tx_get(&phy->access, tx);
}

/**  Rx set 
 *   This function is used to set receiver analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return furia_rx_set(&phy->access, rx);
}

/**  Rx get 
 *   This function is used to get receiver analog parameters 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return furia_rx_get(&phy->access, rx);
}

/**  PHY Rx adaptation resume 
 *   This function is used to perform PHY reset 
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    return furia_rx_adaptation_resume(&phy->access);
}
/**  Set PHY reset 
 *   This function is used to perform PHY reset 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phy reset structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_reset_set(const phymod_phy_access_t* phy,
                        const phymod_phy_reset_t* reset)
{
    return _furia_phy_reset_set(&phy->access, reset);
}

/**  Get PHY reset 
 *   This function is used to get status of PHY reset 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phy reset structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_reset_get(const phymod_phy_access_t* phy,
                        phymod_phy_reset_t* reset)
{
    return _furia_phy_reset_get(&phy->access, reset);
}

/**  Tx lane control set 
 *   This function is used to set the lane control for example
 *   resetting the traffic 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                                  phymod_phy_tx_lane_control_t tx_control)
{
     return furia_tx_lane_control_set(&phy->access, tx_control);
}

/**  Tx lane control get 
 *   This function is used to get the lane control status for 
 *   a particular lane control
 *    
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                                  phymod_phy_tx_lane_control_t* tx_control)
{
    return furia_tx_lane_control_get(&phy->access, tx_control);;
}


/**  Rx lane control set 
 *   This function is used to set the lane control for example
 *   resetting the traffic 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                                  phymod_phy_rx_lane_control_t rx_control)
{
    return furia_rx_lane_control_set(&phy->access, rx_control);
}


/**  Rx lane control get 
 *   This function is used to get the lane control status for 
 *   a particular lane control
 *    
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                                  phymod_phy_rx_lane_control_t* rx_control)
{
    return furia_rx_lane_control_get(&phy->access, rx_control);
}
/**  Set loopback  
 *   This function is used to set a particular loopback mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_loopback_set(const phymod_phy_access_t* phy,
                           phymod_loopback_mode_t loopback,
                           uint32_t enable)
{
    uint32_t ena_dis = 0; 
    PHYMOD_IF_ERR_RETURN(furia_loopback_get(&phy->access, loopback, &ena_dis));
    if (ena_dis || enable) {
        return furia_loopback_set(&phy->access, loopback, enable);
    }
    return PHYMOD_E_NONE; 
}

/**  Get loopback  
 *   This function is used to get the status of a particular loopback mode
 *   whether or not enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_loopback_get(const phymod_phy_access_t* phy,
                           phymod_loopback_mode_t loopback,
                           uint32_t* enable)
{
    return furia_loopback_get(&phy->access, loopback, enable);
}

/**  Set Force Tx training
 *   This function is used to enable/disable forced Tx training 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable / disable force Tx training
 *        1 - enable
 *        0 - disable 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return furia_force_tx_training_set(&phy->access, cl72_en);
}


/**  Get force Tx training 
 *   This function is used to get the status whether Tx training is enabled or disabled 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable 
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return furia_force_tx_training_get(&phy->access, cl72_en);
}

/**  Get force Tx training status 
 *   This function is used to get the status  of Tx training 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *    @param training_failure  training failure status
 *        1 - failure detected
 *        0 - no failure detected 
 *    @trained                 Receiver training status
 *        1 - receiver trained 
 *        0 - receiver not trained
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return furia_force_tx_training_status_get(&phy->access, status);
}

/**  Get core firmware info 
 *   This function is used to get core firmware version and CRC 
 *    
 *
 *    @param core                Pointer to phymod core access structure
 *    @param fw_info             Firmware information such as version and CRC
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    return furia_firmware_info_get(&core->access, fw_info);
}

int furia_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    return furia_firmware_core_config_set(&phy->access, fw_config);
}


int furia_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    return furia_firmware_core_config_get(&phy->access, fw_config);
}


int furia_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    return furia_firmware_lane_config_set(&phy->access, fw_config);
}


int furia_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    return furia_firmware_lane_config_get(&phy->access, fw_config);
}
/**  Restart PLL Sequecer 
 *   This function is used to get restart the PLL sequencer 
 *    
 *
 *    @param core                Pointer to phymod core access structure
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return furia_pll_sequencer_restart(&core->access, operation);
}
/**  FEC enable set 
 *   This function is used to enable the FEC 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return furia_fec_enable_set(&phy->access, enable);
}
/**  FEC enable get 
 *   This function is used to check whether or not FEC is enabled 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return furia_fec_enable_get(&phy->access, enable);
}
/**  PHY status dump 
 *   This function is used to display status dump of a core and for given lane 
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_status_dump(const phymod_phy_access_t* phy)
{
    return _furia_phy_status_dump(&phy->access);
}
/**  Interrupt status get 
 *   This function is used to check whether or not particualr interrupt is pending
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param status             Pending status mask
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_intr_status_get(const phymod_phy_access_t *phy, uint32_t* intr_status)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;

    for (intr_index = 0; intr_index < FURIA_MAX_INTRS_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN(
            furia_ext_intr_status_get(phy,(0x1 << intr_index), &value));
        if (value) {
            *intr_status |= (0x1 << intr_index);
        }
    } 
    return PHYMOD_E_NONE; 
}
/**  Interrupt enable set 
 *   This function is used to enable or disable particular interrupt
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_intr_enable_set(const phymod_phy_access_t *phy, uint32_t enable)
{
    uint32_t intr_index = 0;
    for (intr_index = 0; intr_index < FURIA_MAX_INTRS_SUPPORT; intr_index ++) {
        if (enable & (0x1 << intr_index)) {
            PHYMOD_IF_ERR_RETURN(furia_ext_intr_enable_set(phy, (0x1 << intr_index), 1));
        } else {
            PHYMOD_IF_ERR_RETURN(furia_ext_intr_enable_set(phy, (0x1 << intr_index), 0));
        }
    }
    return PHYMOD_E_NONE; 
} 
/**  Interrupt enable get 
 *   This function is used to check whether or not particular interrupt is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_intr_enable_get(const phymod_phy_access_t *phy, uint32_t* enable)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;

    *enable = 0;
    for (intr_index = 0; intr_index < FURIA_MAX_INTRS_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN(
            furia_ext_intr_enable_get(phy, (0x1 << intr_index), &value));
        if (value) {
            *enable |= (0x1 << intr_index);
        }
    }
    return PHYMOD_E_NONE;
} 
/**  Interrupt status clear 
 *   This function is used for clearing interrupt status
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param intr_type          Interrupt type
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_clear)
{
    uint32_t intr_index = 0;

    for (intr_index = 0; intr_index < FURIA_MAX_INTRS_SUPPORT; intr_index ++) {
        if (intr_clear & (0x1 << intr_index)) {
            PHYMOD_IF_ERR_RETURN(
                furia_ext_intr_status_clear(phy, (0x1 << intr_index)));
        }
    }
    return PHYMOD_E_NONE;
}

int furia_phy_lane_cross_switch_map_set(const phymod_phy_access_t* phy, const uint32_t* tx_array)
{
    return furia_lane_cross_switch_map_set(&phy->access, tx_array);
}

int furia_phy_lane_cross_switch_map_get(const phymod_phy_access_t* phy, uint32_t* tx_array)
{
    return furia_lane_cross_switch_map_get(&phy->access, tx_array);
}
/*Read data from I2C device attached to PHY*/
int furia_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
        return furia_module_read(&phy->access, addr, offset, size, data);
}

/*Write data to I2C device attached to PHY*/
int furia_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
    return furia_module_write(&phy->access, addr, offset, size, data); 
}

/*Set/Get the output/input value of a PHY GPIO pin*/
int furia_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    return furia_gpio_pin_value_set(&phy->access, pin_no, value);
}    
int furia_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    return furia_gpio_pin_value_get(&phy->access, pin_no, value);
}

/*Set/Get the configuration of a PHY GPIO pin*/
int furia_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    return furia_gpio_config_set(&phy->access, pin_no, gpio_mode);
}
int furia_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    return furia_gpio_config_get(&phy->access, pin_no, gpio_mode);
}

int  furia_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    switch (media) {
    case phymodMediaTypeChipToChip:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeShort:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeMid:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    case phymodMediaTypeLong:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    default:
      tx->pre   = 0xc;
      tx->main  = 0x66;
      tx->post  = 0x0;
      tx->post2 = 0x0;
      tx->post3 = 0x0;
      tx->amp   = 0xc;
      break;
    }

    return PHYMOD_E_NONE;
}


/** Short channel mode set
 *   This function is used set the enable or diable the short channel mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int furia_phy_short_chn_mode_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{        
    return _furia_phy_short_channel_mode_set(&phy->access, enable);
}
/** Short channel mode get
 *   This function is used get the enable or diable the short channel mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        0 - enable
 *        1 - disable
 *    @param status             enable or disable
 *        0 - enable
 *        1 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int furia_phy_short_chn_mode_enable_get(const phymod_phy_access_t* phy, uint32_t* enable, uint32_t* status)
{        
    return _furia_phy_short_channel_mode_get(&phy->access, enable, status);
}

int furia_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}

