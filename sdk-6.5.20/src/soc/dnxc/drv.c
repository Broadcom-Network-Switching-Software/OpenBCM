

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT



#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/sand/sand_aux_access.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>

#endif 
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif 
#include <soc/cmicx_miim.h>
#include <soc/dnxc/dnxc_mem.h>



shr_error_e
soc_dnxc_cmicx_device_hard_reset(
    int unit,
    int reset_action)
{
    soc_timeout_t to;
    uint32 reg32_val;
    uint32 usleep = 1000;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_REG_IS_VALID(unit, DMU_CRU_RESETr))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Called for wrong device type\n");
    }
    if (SOC_IS_RELOADING(unit))
    {
        SHR_EXIT();
    }

    if ((reset_action == SOC_DNXC_RESET_ACTION_IN_RESET) || (reset_action == SOC_DNXC_RESET_ACTION_INOUT_RESET))
    {
        SHR_IF_ERR_EXIT(READ_DMU_CRU_RESETr(unit, &reg32_val)); 
        soc_reg_field_set(unit, DMU_CRU_RESETr, &reg32_val, SW_RESET_Nf, 0);    
        SHR_IF_ERR_EXIT(WRITE_DMU_CRU_RESETr(unit, reg32_val)); 

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            SHR_EXIT();
        }
#endif
        if (soc_sand_is_emulation_system(unit) != 0)
        {
            usleep *= 1000;
        }

        sal_usleep(usleep);     
        
        soc_timeout_init(&to, 100000, 100);
        for (;;)
        {
            SHR_IF_ERR_EXIT(READ_DMU_CRU_RESETr(unit, &reg32_val));     
            if (soc_reg_field_get(unit, DMU_CRU_RESETr, reg32_val, SW_RESET_Nf) == 1)
            {
                break;
            }
            if (soc_timeout_check(&to))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_INIT, "Error: DMU_CRU_RESET field not asserted correctly.\n%s%s%s", EMPTY,
                                  EMPTY, EMPTY);
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxc_data_mdio_config_get(
    int unit,
    int *int_divisor,
    int *ext_divisor,
    int *int_delay,
    int *ext_delay)
{
    *int_divisor = 0;
    *ext_divisor = 0;
    *int_delay = 0;
    *ext_delay = 0;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        *int_divisor = dnx_data_access.mdio.int_divisor_get(unit);
        *ext_divisor = dnx_data_access.mdio.ext_divisor_get(unit);
        *int_delay = dnx_data_access.mdio.int_div_out_delay_get(unit);
        *ext_delay = dnx_data_access.mdio.ext_div_out_delay_get(unit);
    }
#endif 
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        *int_divisor = dnxf_data_device.access.mdio_int_divisor_get(unit);
        *ext_divisor = dnxf_data_device.access.mdio_ext_divisor_get(unit);
        *int_delay = dnxf_data_device.access.mdio_int_div_out_delay_get(unit);
        *ext_delay = dnxf_data_device.access.mdio_ext_div_out_delay_get(unit);
    }
#endif
}
shr_error_e
soc_dnxc_mdio_config_set(
    int unit)
{
    int int_divisor = 0, ext_divisor = 0, int_delay = 0, ext_delay = 0;
    SHR_FUNC_INIT_VARS(unit);
    

    soc_dnxc_data_mdio_config_get(unit, &int_divisor, &ext_divisor, &int_delay, &ext_delay);

    SHR_IF_ERR_EXIT(soc_cmicx_miim_init(unit));
    
    SHR_IF_ERR_EXIT(soc_cmicx_miim_divider_set_all(unit, int_divisor, ext_divisor, int_delay));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_avs_value_get(
    int unit,
    uint32 *avs_val)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    *avs_val = 0;

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, ECI_BOND_STATUSr, REG_PORT_ANY, 0, &reg_val));
    *avs_val = soc_reg_field_get(unit, ECI_BOND_STATUSr, reg_val, AVS_STATUSf);

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_nof_block_instances(
    int unit,
    soc_block_types_t block_types,
    int *nof_block_instances)
{
    int block_index;
    uint32 nof_instances = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_block_instances, _SHR_E_PARAM, "nof_block_instances");
    SHR_NULL_CHECK(block_types, _SHR_E_PARAM, "block_types");

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    SOC_BLOCK_ITER_ALL(unit, block_index, *block_types)
    {
        ++nof_instances;
    }

    *nof_block_instances = nof_instances;

exit:
    SHR_FUNC_EXIT;
}

void
soc_dnxc_chip_dump(
    int unit,
    soc_driver_t * d)
{
    soc_info_t *si;
    int i, count = 0;
    soc_port_t port;
    char pfmt[SOC_PBMP_FMT_LEN];
    uint16 dev_id;
    uint8 rev_id;
    int blk, bindex;
    char instance_string[3], block_name[14];

    if (d == NULL)
    {
        LOG_CLI((BSL_META_U(unit, "unit %d: no driver attached\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit, "driver %s (%s)\n"), SOC_CHIP_NAME(d->type), d->chip_string));
    LOG_CLI((BSL_META_U(unit, "\tregsfile\t\t%s\n"), d->origin));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci identifier\t\tvendor 0x%04x device 0x%04x rev 0x%02x\n"),
             d->pci_vendor, d->pci_device, d->pci_revision));
    LOG_CLI((BSL_META_U(unit, "\tclasses of service\t%d\n"), d->num_cos));
    LOG_CLI((BSL_META_U(unit,
                        "\tmaximums\t\tblock %d ports %d mem_bytes %d\n"),
             SOC_MAX_NUM_BLKS, SOC_MAX_NUM_PORTS, SOC_MAX_MEM_BYTES));

    if (unit < 0)
    {
        return;
    }
    si = &SOC_INFO(unit);
    for (blk = 0; d->block_info[blk].type >= 0; blk++)
    {
        sal_snprintf(instance_string, sizeof(instance_string), "%d", d->block_info[blk].number);
        if (d->block_info[blk].type == SOC_BLK_PORT_GROUP4 || d->block_info[blk].type == SOC_BLK_PORT_GROUP5)
        {
            sal_strncpy(instance_string, d->block_info[blk].number ? "_y" : "_x", 2);
            instance_string[2] = '\0';
        }
        sal_snprintf(block_name, sizeof(block_name), "%s%s",
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit), instance_string);
        LOG_CLI((BSL_META_U(unit,
                            "\tblk %d\t\t%-14s schan %d cmic %d\n"),
                 blk, block_name, d->block_info[blk].schan, d->block_info[blk].cmic));
    }
    for (port = 0;; port++)
    {
        blk = d->port_info[port].blk;
        bindex = d->port_info[port].bindex;
        if (blk < 0 && bindex < 0)
        {       
            break;
        }
        if (blk < 0)
        {       
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "\tport %d\t\t%s\tblk %d %s%d.%d\n"),
                 soc_feature(unit, soc_feature_logical_port_num) ?
                 si->port_p2l_mapping[port] : port,
                 soc_block_port_name_lookup_ext(d->block_info[blk].type, unit),
                 blk, soc_block_name_lookup_ext(d->block_info[blk].type, unit), d->block_info[blk].number, bindex));
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    LOG_CLI((BSL_META_U(unit, "unit %d:\n"), unit));
    LOG_CLI((BSL_META_U(unit, "\tpci\t\t\tdevice %04x rev %02x\n"), dev_id, rev_id));
    LOG_CLI((BSL_META_U(unit,
                        "\tdriver\t\t\ttype %d (%s) group %d (%s)\n"),
             si->driver_type, SOC_CHIP_NAME(si->driver_type),
             si->driver_group, soc_chip_group_names[si->driver_group]));
    LOG_CLI((BSL_META_U(unit, "\tchip\t\t\t\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\tGE ports\t%d\t%s (%d:%d)\n"),
             si->ge.num, SOC_PBMP_FMT(si->ge.bitmap, pfmt), si->ge.min, si->ge.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tXE ports\t%d\t%s (%d:%d)\n"),
             si->xe.num, SOC_PBMP_FMT(si->xe.bitmap, pfmt), si->xe.min, si->xe.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tHG ports\t%d\t%s (%d:%d)\n"),
             si->hg.num, SOC_PBMP_FMT(si->hg.bitmap, pfmt), si->hg.min, si->hg.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tST ports\t%d\t%s (%d:%d)\n"),
             si->st.num, SOC_PBMP_FMT(si->st.bitmap, pfmt), si->st.min, si->st.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tETHER ports\t%d\t%s (%d:%d)\n"),
             si->ether.num, SOC_PBMP_FMT(si->ether.bitmap, pfmt), si->ether.min, si->ether.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tPORT ports\t%d\t%s (%d:%d)\n"),
             si->port.num, SOC_PBMP_FMT(si->port.bitmap, pfmt), si->port.min, si->port.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tALL ports\t%d\t%s (%d:%d)\n"),
             si->all.num, SOC_PBMP_FMT(si->all.bitmap, pfmt), si->all.min, si->all.max));
    LOG_CLI((BSL_META_U(unit, "\tIPIC port\t%d\tblock %d\n"), si->ipic_port, si->ipic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tCMIC port\t%d\t%s block %d\n"), si->cmic_port,
             SOC_PBMP_FMT(si->cmic_bitmap, pfmt), si->cmic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tother blocks\t\tARL %d MMU %d MCU %d\n"), si->arl_block, si->mmu_block, si->mcu_block));
    LOG_CLI((BSL_META_U(unit,
                        "\t            \t\tIPIPE %d IPIPE_HI %d EPIPE %d EPIPE_HI %d BSAFE %d ESM %d\n"),
             si->ipipe_block, si->ipipe_hi_block, si->epipe_block, si->epipe_hi_block, si->bsafe_block, si->esm_block));

    for (i = 0; i < COUNTOF(si->has_block); i++)
    {
        if (si->has_block[i])
        {
            count++;
        }
    }
    LOG_CLI((BSL_META_U(unit, "\thas blocks\t%d\t"), count));
    for (i = 0; i < COUNTOF(si->has_block); i++)
    {
        if (si->has_block[i])
        {
            LOG_CLI((BSL_META_U(unit, "%s "), soc_block_name_lookup_ext(si->has_block[i], unit)));
            if ((i) && !(i % 6))
            {
                LOG_CLI((BSL_META_U(unit, "\n\t\t\t\t")));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));
    LOG_CLI((BSL_META_U(unit, "\tport names\t\t")));
    for (port = 0; port < si->port_num; port++)
    {
        if (port > 0 && (port % 5) == 0)
        {
            LOG_CLI((BSL_META_U(unit, "\n\t\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit, "%d=%s\t"), port, si->port_name[port]));
    }
    LOG_CLI((BSL_META_U(unit, "\n")));
    i = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
    {
        if (SOC_PBMP_IS_NULL(si->block_bitmap[blk]))
        {
            continue;
        }
        if (++i == 1)
        {
            LOG_CLI((BSL_META_U(unit, "\tblock bitmap\t")));
        }
        else
        {
            LOG_CLI((BSL_META_U(unit, "\n\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-2d  %-14s %s (%d ports)"),
                 blk, si->block_name[blk], SOC_PBMP_FMT(si->block_bitmap[blk], pfmt), si->block_valid[blk]));
    }
    if (i > 0)
    {
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    {
        soc_feature_t f;

        LOG_CLI((BSL_META_U(unit, "\tfeatures\t")));
        i = 0;
        for (f = 0; f < soc_feature_count; f++)
        {
            if (soc_feature(unit, f))
            {
                if (++i > 3)
                {
                    LOG_CLI((BSL_META_U(unit, "\n\t\t\t")));
                    i = 1;
                }
                LOG_CLI((BSL_META_U(unit, "%s "), soc_feature_name[f]));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }
}


shr_error_e
soc_dnxc_verify_device_init(
    int unit)
{
    uint32 val = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (((SOC_IS_DNX(unit) && !SOC_IS_JERICHO2_A0(unit)) || SOC_IS_RAMON_B0(unit)) &&
#ifdef PLISIM
        !SAL_BOOT_PLISIM &&     
#endif
        soc_sand_is_emulation_system(unit) == 0)
    {
        SHR_IF_ERR_EXIT(READ_ECI_RESERVED_30r(unit, &val));     
        if (val == 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Device init failure\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_block_instance_validity_override(
    int unit)
{
    int index = 0;
    int table_size;
#ifdef BCM_DNXF_SUPPORT
    const dnxf_data_device_blocks_override_t *dnxf_override_info = NULL;
#endif
#ifdef BCM_DNX_SUPPORT
    const dnx_data_access_blocks_override_t *dnx_override_info = NULL;
#endif
    const dnxc_data_table_info_t *table_info;
    soc_block_t block_type;
    soc_block_t blk;
    unsigned block_instance_number = 0;
    char *block_type_name = NULL;
    int value = 0;
    soc_info_t *si = &SOC_INFO(unit);
    SHR_FUNC_INIT_VARS(unit);
    {
    
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            table_info = dnx_data_access.blocks.override_info_get(unit);
        }
        else
#endif
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            table_info = dnxf_data_device.blocks.override_info_get(unit);
        }
        else
#endif
        {
            SHR_ERR_EXIT(_SHR_E_NONE, "Called for an incompatible device. \n");
        }
    }
    table_size = table_info->key_size[0];

    
    for (index = 0; index < table_size; index++)
    {
        
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            dnxf_override_info = dnxf_data_device.blocks.override_get(unit, index);
            block_type_name = dnxf_override_info->block_type;
            block_instance_number = dnxf_override_info->block_instance;
            value = dnxf_override_info->value;
        }
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            dnx_override_info = dnx_data_access.blocks.override_get(unit, index);
            block_type_name = dnx_override_info->block_type;
            block_instance_number = dnx_override_info->block_instance;
            value = dnx_override_info->value;
        }
#endif
        if (block_type_name == NULL)
        {
            LOG_VERBOSE_EX(BSL_LOG_MODULE, "block not found %s\n%s%s%s", block_type_name, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            
            block_type = soc_block_name_match(unit, block_type_name);
            if (block_type == SOC_BLK_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "block not found %s\n", block_type_name);
            }
            for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
            {
                if (SOC_BLOCK_TYPE(unit, blk) == block_type)
                {
                    if (SOC_BLOCK_NUMBER(unit, blk) == block_instance_number)
                    {
                        si->block_valid[blk] = value ? 1 : 0;   
                        LOG_VERBOSE(BSL_LOG_MODULE,
                                    (BSL_META_U
                                     (unit,
                                      "block type\"%s\" indentified as:%d - instance %d identified a soc block instance number %d - validity was overridden to %u\n"),
                                     block_type_name, block_type, block_instance_number, blk, value));
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}


#define ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET 0xe8f


int
soc_dnxc_does_need_system_reset(
    int unit)
{
    uint32 reg32_val = 0;

    soc_iproc_getreg(unit, soc_reg_addr(unit, PAXB_0_FUNC0_IMAP1_6r, REG_PORT_ANY, 0), &reg32_val);
    return (soc_reg_field_get(unit, PAXB_0_FUNC0_IMAP1_6r, reg32_val, ADDRf) == ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET)
        && (soc_reg_field_get(unit, PAXB_0_FUNC0_IMAP1_6r, reg32_val, VALIDf) == 0);
}


void
soc_dnxc_set_need_system_reset(
    int unit,
    int needs_system_reset)
{
    uint32 reg32_val = 0;

    
    if (needs_system_reset != 0)
    {
        soc_reg_field_set(unit, PAXB_0_FUNC0_IMAP1_6r, &reg32_val, ADDRf, ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET);
    }
    soc_iproc_setreg(unit, soc_reg_addr(unit, PAXB_0_FUNC0_IMAP1_6r, REG_PORT_ANY, 0), reg32_val);
}


int
soc_dnxc_does_support_system_reset(
    int unit)
{
    return SOC_CONTROL(unit)->system_reset_cb &&
        SOC_CONTROL(unit)->system_reset_cb(unit, SOC_SYSTEM_RESET_FLAG_IS_SUPPORTED) == SOC_E_NONE;
}


int
soc_dnxc_perform_system_reset(
    int unit)
{
    int results = SOC_E_NONE;
    soc_reg_t rts_reg = RTS_ICFG_PCIE64B_ACCESS_MSB_63TO32r;
    uint32 reg32_val = ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET, straps_val = -2, straps_val2 = -3;

    if (SOC_CONTROL(unit)->system_reset_cb &&
        SOC_CONTROL(unit)->system_reset_cb(unit, SOC_SYSTEM_RESET_FLAG_IS_SUPPORTED) == SOC_E_NONE)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Performing a system reset of the device\n")));

        
        if (!SOC_REG_IS_VALID(unit, rts_reg)
            && !SOC_REG_IS_VALID(unit, (rts_reg = RTS1_ICFG_PCIE64B_ACCESS_MSB_63TO32r)))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Device not currently supported\n")));
            return SOC_E_UNAVAIL;
        }
        soc_iproc_setreg(unit, soc_reg_addr(unit, rts_reg, REG_PORT_ANY, 0), ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET);
        READ_ICFG_PCIE_0_STRAPSr(unit, &straps_val); 

        results = SOC_CONTROL(unit)->system_reset_cb(unit, 0); 
        if (results != SOC_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Device system reset failed.\n")));
        }
        else
        {   
            READ_ICFG_PCIE_0_STRAPSr(unit, &straps_val2); 
            soc_iproc_getreg(unit, soc_reg_addr(unit, rts_reg, REG_PORT_ANY, 0), &reg32_val);
            if (straps_val != straps_val2)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Device BARs are not working properly after the system reset\n")));
                results = SOC_E_FAIL;
            }
            else if (reg32_val == ADDR_VALUE_MARKING_NEEDS_SYSTEM_RESET)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit, "System reset callback seemed to succeed, but in fact it did not reset RTS\n")));
                results = SOC_E_FAIL;
            }
            else if (!soc_dnxc_does_need_system_reset(unit))
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit, "System reset callback seemed to succeed, but PAXB was unexpectedly changed\n")));
                results = SOC_E_FAIL;
            }

            if (results == SOC_E_NONE)
            { 
                soc_dnxc_set_need_system_reset(unit, 0);
                
            }
        }

    }
    else
    {
        results = SOC_E_NO_HANDLER;
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "A callback supporting system reset was not registered for the device.\n")));
    }

    return results;
}


int
soc_dnxc_perform_system_reset_if_needed(
    int unit)
{
    int results = SOC_E_NONE;

    if (soc_dnxc_does_need_system_reset(unit))
    {
        if (dnx_drv_soc_property_get(unit, spn_PERFORM_SYSTEM_RESET_WHEN_NEEDED, 0) == 0)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Device needs a system reset or powering off, stopping the startup\n")));
            return SOC_E_FAIL;
        }

        results = soc_dnxc_perform_system_reset(unit);

        if (results != SOC_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit,
                        "Could not perform the system reset, that was required by the device probably due to a previous MBIST run\nCannot continue running\n")));
        }
    }
    return results;
}
