
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT

#include <soc/sand/shrextend/shrextend_debug.h>

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>
#include <soc/dnxc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <sal/appl/config.h>
#include <bcm/init.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/dnxc/dnxc_access.h>
#endif
#include <soc/mcm/driver.h>
#include <soc/sand/sand_aux_access.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif
#include <shared/cmicfw/cmicx_link.h>
#include <soc/cmicx_miim.h>
#include <soc/cmicx.h>
#include <soc/dnxc/dnxc_mem.h>
#include <unistd.h>
#include <signal.h>

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/access_intr.h>
#include <soc/access/hotswap.h>
#endif

#ifdef BCM_DNX_SUPPORT
#define USE_SOC_DNXC_CMICX_RESET_USING_IDM
#endif
#ifdef USE_SOC_DNXC_CMICX_RESET_USING_IDM

static shr_error_e
soc_dnxc_cmicx_reset_using_idm(
    int unit)
{
    uint32 reg32_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_CMICX_M0_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M0_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 1);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M0_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_M1_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M1_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 1);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M1_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_M2_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M2_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 1);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M2_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_S0_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_S0_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 1);
    SHR_IF_ERR_EXIT(WRITE_CMICX_S0_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    sal_usleep(1000);
    SHR_IF_ERR_EXIT(READ_CMICX_M0_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M0_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 0);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M0_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_M1_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M1_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 0);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M1_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_M2_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_M2_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 0);
    SHR_IF_ERR_EXIT(WRITE_CMICX_M2_IDM_IDM_RESET_CONTROLr(unit, reg32_val));
    SHR_IF_ERR_EXIT(READ_CMICX_S0_IDM_IDM_RESET_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMICX_S0_IDM_IDM_RESET_CONTROLr, &reg32_val, RESETf, 0);
    SHR_IF_ERR_EXIT(WRITE_CMICX_S0_IDM_IDM_RESET_CONTROLr(unit, reg32_val));

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
soc_dnxc_cmicx_device_hard_reset(
    int unit,
    int reset_action)
{
#ifdef BCM_DNX_SUPPORT
    int hard_reset_mode = 0x0;
#endif
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
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        hard_reset_mode = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
    }
#endif
    if ((reset_action == SOC_DNXC_RESET_ACTION_IN_RESET) || (reset_action == SOC_DNXC_RESET_ACTION_INOUT_RESET))
    {
#ifdef BCM_DNX_SUPPORT

        if (hard_reset_mode == 2)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_cmicx_reset_using_idm(unit));
        }
        else
#endif
        {
            {
                SHR_IF_ERR_EXIT(READ_DMU_CRU_RESETr(unit, &reg32_val));
                soc_reg_field_set(unit, DMU_CRU_RESETr, &reg32_val, SW_RESET_Nf, 0);
                SHR_IF_ERR_EXIT(WRITE_DMU_CRU_RESETr(unit, reg32_val));
            }
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
                {
                    if (soc_reg_field_get(unit, DMU_CRU_RESETr, reg32_val, SW_RESET_Nf) == 1)
                    {
                        break;
                    }
                }
                if (soc_timeout_check(&to))
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_INIT, "Error: DMU_CRU_RESET field not asserted correctly.\n%s%s%s", EMPTY,
                                      EMPTY, EMPTY);
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_eci_reset(
    int unit)
{
    soc_reg_t reg;
    uint64 default_value;

    SHR_FUNC_INIT_VARS(unit);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg) ||
            !SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_ECI) ||
            (SOC_REG_INFO(unit, reg).flags &
             (SOC_REG_FLAG_RO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL)))
        {
            continue;
        }

        switch (reg)
        {

            case ECI_INDIRECT_COMMAND_WR_DATAr:
            case ECI_INDIRECT_COMMANDr:

            case ECI_INDIRECT_COMMAND_ADDRESSr:

            case ECI_TAP_CONTROLr:
            case ECI_TAP_CPU_INTERFACE_COMMANDr:
            case ECI_TAP_CPU_INTERFACE_DATA_INr:

            case ECI_VERSION_REGISTERr:
                continue;
        }

        if (SOC_REG_IS_ABOVE_64(unit, reg))
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, SOC_REG_ABOVE_64_INFO(unit, reg).reset));
        }
        else
        {
            SOC_REG_RST_VAL_GET(unit, reg, default_value);
            SHR_IF_ERR_EXIT(soc_reg_set(unit, reg, REG_PORT_ANY, 0, default_value));
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
    void *driver)
{
    soc_info_t *si;
    int i, count = 0;
    soc_port_t port;
    char pfmt[SOC_PBMP_FMT_LEN];
    uint16 dev_id;
    uint8 rev_id;
    int blk, bindex;
    char instance_string[3], block_name[14];
    soc_driver_t *d = (soc_driver_t *) driver;
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
            for (blk = SOC_BLOCK_INSTANCES(unit, block_type).first_blk_instance;
                 blk <
                 (SOC_BLOCK_INSTANCES(unit, block_type).nof_block_instance +
                  SOC_BLOCK_INSTANCES(unit, block_type).first_blk_instance); blk++)
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
exit:
    SHR_FUNC_EXIT;
}

#define PERSISTANT_VALUE_MARKING_USED 0xaface8c0
#define PERSISTANT_VALUE_MARKING_MASK 0xfffffff0

#define PERSISTANT_VALUE_NEEDS_SYSTEM_RESET 1

#define PERSISTANT_VALUE_IN_CPU2TAP 2

#define PERSISTANT_VALUE_CPU2TAP_ERROR 4

#define RTS_TEST_VALUE 0xe8f

int
soc_dnxc_does_need_system_reset(
    int unit)
{
    uint32 reg32_val = 0;
    soc_reg_t r = ICFG_GEN_PURPOSE_REG15r;
    int r_index = 0;

    if (!SOC_REG_IS_VALID(unit, r))
    {
        r = ICFG_GEN_PURPOSE_REGr;
        r_index = 15;
    }
    soc_bar_read_reg32(unit, r, r_index, &reg32_val);

    return (reg32_val & PERSISTANT_VALUE_MARKING_MASK) == PERSISTANT_VALUE_MARKING_USED &&
        ((reg32_val &
          (SOC_IS_JERICHO2(unit)
           ? (PERSISTANT_VALUE_NEEDS_SYSTEM_RESET | PERSISTANT_VALUE_IN_CPU2TAP | PERSISTANT_VALUE_CPU2TAP_ERROR) :
           PERSISTANT_VALUE_NEEDS_SYSTEM_RESET)) != 0);
}

void
soc_dnxc_warn_if_device_is_used_by_diffrent_sdk(
    int unit)
{
    uint32 reg32_val = 0;
    uint32 pid;

    soc_reg_t r = ICFG_GEN_PURPOSE_REG14r;
    int r_index = 0;

    if (!SOC_REG_IS_VALID(unit, r))
    {
        r = ICFG_GEN_PURPOSE_REGr;
        r_index = 14;
    }
    soc_bar_read_reg32(unit, r, r_index, &reg32_val);

    pid = sal_getpid();

    if (reg32_val == -1)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U
                   (unit, "Error: The device seems to be disconnected from the processor running the SDK.\n")));
    }
    else
    {

        if (reg32_val != 0 && reg32_val != pid && !sal_kill(reg32_val, 0))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit,
                        "Process %u may be an SDK process already controlling the device, which will prevent this process from controlling the device.\n Current process id is %u\n"),
                       reg32_val, pid));
            print_process_info(reg32_val);
        }
    }

    soc_bar_write_reg32(unit, r, r_index, pid);
}

static int
soc_dnxc_is_persistent_register_set(
    int unit)
{
    uint32 reg32_val = 0;
    soc_reg_t r = ICFG_GEN_PURPOSE_REG15r;
    int r_index = 0;

    if (!SOC_REG_IS_VALID(unit, r))
    {
        r = ICFG_GEN_PURPOSE_REGr;
        r_index = 15;
    }
    soc_bar_read_reg32(unit, r, r_index, &reg32_val);

    return (reg32_val & PERSISTANT_VALUE_MARKING_MASK) == PERSISTANT_VALUE_MARKING_USED;
}

static void
soc_dnxc_set_persistent_register_flag(
    int unit,
    uint32 flag_mask,
    uint32 flag_to_set)
{
    uint32 orig_reg_val = 0, new_reg_val;
    soc_reg_t r = ICFG_GEN_PURPOSE_REG15r;
    int r_index = 0;

    if (!SOC_REG_IS_VALID(unit, r))
    {
        r = ICFG_GEN_PURPOSE_REGr;
        r_index = 15;
    }
    soc_bar_read_reg32(unit, r, r_index, &orig_reg_val);

    new_reg_val = ((orig_reg_val & PERSISTANT_VALUE_MARKING_MASK) == PERSISTANT_VALUE_MARKING_USED) ?
        orig_reg_val : PERSISTANT_VALUE_MARKING_USED;

    new_reg_val &= flag_mask;
    new_reg_val |= flag_to_set;

    if (new_reg_val != orig_reg_val)
    {
        soc_bar_write_reg32(unit, r, r_index, new_reg_val);
    }
}

void
soc_dnxc_set_need_system_reset(
    int unit,
    int needs_system_reset)
{
    soc_dnxc_set_persistent_register_flag(unit, ~PERSISTANT_VALUE_NEEDS_SYSTEM_RESET,
                                          needs_system_reset ? PERSISTANT_VALUE_NEEDS_SYSTEM_RESET : 0);
}

void
soc_dnxc_set_in_cpu2tap(
    int unit,
    int in_cpu2tap)
{
    soc_dnxc_set_persistent_register_flag(unit, ~PERSISTANT_VALUE_IN_CPU2TAP,
                                          in_cpu2tap ? PERSISTANT_VALUE_IN_CPU2TAP : 0);
}

void
soc_dnxc_set_cpu2tap_had_error(
    int unit,
    int in_cpu2tap)
{
    soc_dnxc_set_persistent_register_flag(unit, ~PERSISTANT_VALUE_CPU2TAP_ERROR,
                                          in_cpu2tap ? PERSISTANT_VALUE_CPU2TAP_ERROR : 0);
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
    uint32 reg32_val = RTS_TEST_VALUE, straps_val = -2, straps_val2 = -3;

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
        soc_iproc_setreg(unit, soc_reg_addr(unit, rts_reg, REG_PORT_ANY, 0), RTS_TEST_VALUE);
        READ_ICFG_PCIE_0_STRAPSr(unit, &straps_val);

        soc_dnxc_set_need_system_reset(unit, 1);

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
            else if (reg32_val == RTS_TEST_VALUE)
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit, "System reset callback seemed to succeed, but in fact it did not reset RTS\n")));
                results = SOC_E_FAIL;
            }
            else if (soc_dnxc_is_persistent_register_set(unit))
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U
                           (unit,
                            "System reset callback seemed to succeed, but the persistent register was not reset\n")));
                results = SOC_E_FAIL;
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

static int
soc_dnxc_mem_mutex_init(
    int unit)
{
    soc_mem_t mem;

    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {

            SOC_MEM_STATE_MEMBER_SET(unit, mem, lock, sal_mutex_create(SOC_MEM_NAME(unit, mem)));
            SHR_NULL_CHECK(SOC_MEM_STATE(unit, mem)->lock, _SHR_E_MEMORY, "failed to allocate memState lock");

            sal_memset(SOC_MEM_STATE(unit, mem)->cache, 0, sizeof(SOC_MEM_STATE(unit, mem)->cache));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
soc_dnxc_mem_mutex_deinit(
    int unit)
{
    soc_mem_t mem;

    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_STATE(unit, mem) && SOC_MEM_STATE(unit, mem)->lock != NULL)
        {
            sal_mutex_destroy(SOC_MEM_STATE(unit, mem)->lock);
            SOC_MEM_STATE(unit, mem)->lock = NULL;
        }
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxc_mutexes_init(
    int unit)
{
    int cmc;
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate SchanWB\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate soc_control lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->counterMutex = sal_mutex_create("Counter")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate counter Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate MIIM lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->miimIntr = sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate MIIM interrup Sem\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if ((soc->schanMutex = sal_mutex_create("SCHAN")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate Schan Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if ((soc->schanIntr[cmc] = sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate Schan interrupt Sem\n%s%s%s", EMPTY, EMPTY,
                                     EMPTY);
        }
    }

#ifdef BCM_CMICM_SUPPORT
    soc->fschanMutex = NULL;
    if ((soc->fschanMutex = sal_mutex_create("FSCHAN")) == NULL)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to allocate fSchan Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
#endif

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit))
#endif
    {

        SHR_IF_ERR_EXIT_WITH_LOG(soc_dnxc_mem_mutex_init(unit),
                                 "Failed to allocate mutex Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_mutexes_deinit(
    int unit)
{
    soc_control_t *soc;
    int cmc;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_IF_ERR_EXIT(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_CONT_WITH_LOG(soc_dnxc_mem_mutex_deinit(unit), "Failed to deinit mutex Lock\n%s%s%s", EMPTY, EMPTY,
                             EMPTY);

#ifdef BCM_CMICM_SUPPORT
    if (soc->fschanMutex != NULL)
    {
        sal_mutex_destroy(soc->fschanMutex);
        soc->fschanMutex = NULL;
    }
#endif

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++)
    {
        if (soc->schanIntr[cmc])
        {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }
    if (soc->schanMutex != NULL)
    {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }
    if (soc->miimIntr != NULL)
    {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    if (soc->miimMutex != NULL)
    {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex != NULL)
    {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->socControlMutex != NULL)
    {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (soc->schan_wb_mutex != NULL)
    {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_schan_config_and_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_sand_access_conf_get(unit);

    if (!SOC_IS_NEW_ACCESS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(soc_schan_init(unit));
    }
#ifdef BCM_ACCESS_SUPPORT
    else
    {
        SHR_IF_ERR_EXIT(access_device_init(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_schan_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_IS_NEW_ACCESS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(soc_schan_deinit(unit));
    }
#ifdef BCM_ACCESS_SUPPORT
    else
    {
        SHR_IF_ERR_EXIT(access_device_deinit(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_ACCESS_SUPPORT

int
soc_dnxc_access_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_feature(unit, soc_feature_access))
    {
        uint32 use_new_access = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                            "use_new_access", soc_is(unit, J2P_DEVICE) ? 0 : 1);
        if (use_new_access != 0)
        {
            SHR_IF_ERR_EXIT(access_init(unit));
            if (use_new_access < 10)
            {
                ACCESS_RUNTIME_INFO(unit)->flags |= ACCESS_RUNTIME_INFO_FLAGS_ALOW_MCM;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

static int
soc_dnxc_configure_soc_control(
    int unit)
{

    uint16 device_id;
    uint8 revision_id;
    soc_control_t *soc;
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

    SHR_IF_ERR_EXIT(soc_cm_get_id(unit, &device_id, &revision_id));
    SHR_IF_ERR_EXIT_WITH_LOG(soc_dnxc_chip_driver_find(unit, device_id, revision_id, (void *) &(soc->chip_driver)),
                             "Couldn't find driver for unit %d (device 0x%04x, revision 0x%02x)\n",
                             unit, device_id, revision_id);

    SHR_IF_ERR_EXIT(soc_dnxc_chip_type_set(unit, device_id));

    si = &SOC_INFO(unit);

    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    if ((CMDEV(unit).dev.info->rev_id & SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT) != 0)
    {
        uint32 rev_id = CMDEV(unit).dev.info->rev_id & SOC_CM_REVID_MASK;
        uint8 last_revid_digit = (rev_id - 1) & 0xf;
        sal_snprintf(SOC_CONTROL(unit)->device_name, sizeof(SOC_CONTROL(unit)->device_name) - 1, "BCM8%.4X_%c%c",
                     (unsigned) CMDEV(unit).dev.dev_id, 'A' + (rev_id >> 4),
                     (last_revid_digit < 10 ? '0' + last_revid_digit : ('A' - 10) + last_revid_digit));
    }
    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) | SOC_F_ATTACHED);

    soc_feature_init(unit);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "use_new_access", 1) >= 10)
    {
        SOC_DRIVER(unit) = NULL;
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_info_config_init(
    int unit)
{
    soc_info_t *si;
    int mem, blk;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnxc_access_init(unit));
    }
#endif
    si = &SOC_INFO(unit);
    si->num_ucs = (soc_feature(unit, soc_feature_cmicx_v4)) ? 4 : 2;
    si->num_time_interface = 1;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_J2X(unit))
        si->num_time_interface = 2;
#endif

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit) || ACCESS_MAY_USE_BY_MCM(unit))
    {
#endif
        SHR_IF_ERR_EXIT(soc_sand_info_config_blocks(unit, 0));

        for (mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            si->mem_block_any[mem] = -1;
            if (SOC_MEM_IS_VALID(unit, mem))
            {
                SOC_MEM_BLOCK_ITER(unit, mem, blk)
                {
                    si->mem_block_any[mem] = blk;
                    break;
                }
            }
        }
#ifdef BCM_ACCESS_SUPPORT
    }
#endif

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit) || ACCESS_MAY_USE_BY_MCM(unit))
    {
#endif
        if (SOC_IS_DNX(unit))
        {

            SOC_USE_GPORT_SET(unit, TRUE);
        }
#ifdef BCM_ACCESS_SUPPORT
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxf_control_init(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_info_config_deinit(
    int unit)
{
    int mem;
#ifdef BCM_ACCESS_SUPPORT
    int has_old_access = 1;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        unsigned sub_unit_index, nof_sub_units = soc_cm_get_nof_sub_units(unit);
        int sub_unit;
        has_old_access = !ACCESS_IS_INITIALIZED(unit) || ACCESS_MAY_USE_BY_MCM(unit);
        SHR_IF_ERR_CONT(access_deinit(unit));

        for (sub_unit_index = 0; sub_unit_index < nof_sub_units; ++sub_unit_index)
        {
            sub_unit = CM_MAIN2SUB_DEV(unit, sub_unit_index);
            if (ACCESS_IS_INITIALIZED(unit))
            {
                SHR_IF_ERR_CONT(access_deinit(sub_unit));
            }
        }
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SHR_IF_ERR_CONT(soc_dnxf_control_deinit(unit));
    }
#endif
    soc_memstate_free(unit);

    sal_memset(SOC_CONTROL(unit), 0, sizeof(soc_control_t));

    SHR_IF_ERR_CONT(soc_dnxc_configure_soc_control(unit));
#ifdef BCM_ACCESS_SUPPORT
    if (has_old_access)
    {
#endif
        SHR_IF_ERR_CONT(soc_memstate_alloc(unit));

        if (!CM_IS_SUB_DEVICE(unit))
        {

            for (mem = 0; mem < NUM_SOC_MEM; mem++)
            {
                if (!SOC_MEM_IS_VALID(unit, mem))
                {
                    continue;
                }
                SOP_MEM_STATE_MAX_INDEX_SET(unit, mem, SOC_MEM_INFO(unit, mem).index_max);
            }
        }
#ifdef BCM_ACCESS_SUPPORT
    }
#endif

    if (SOC_UNIT_NUM_VALID(unit))
    {
        SOC_DETACH(unit, 0);
    }

    SHR_FUNC_EXIT;
}

int
soc_dnxc_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id,
    void **found_driver)
{
    uint16 driver_dev_id;
    uint16 driver_rev_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id));
    {
        if (SOC_IS_JERICHO2_TYPE(driver_dev_id))
        {
#if defined(BCM_88690_A0)

            if (driver_rev_id == JERICHO2_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88690_a0;
            }
            else
#endif
#if defined(BCM_88690_B0)

            if ((driver_rev_id == JERICHO2_B0_REV_ID) || (driver_rev_id == JERICHO2_B1_REV_ID))
            {
                *found_driver = &soc_driver_bcm88690_b0;
            }
            else
#endif
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else if (SOC_IS_J2C_TYPE(driver_dev_id))
        {
#if defined(BCM_88800_A0)

            if ((driver_rev_id == J2C_A0_REV_ID) || (driver_rev_id == J2C_A1_REV_ID))
            {
                *found_driver = &soc_driver_bcm88800_a0;
            }
            else
#endif
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else if (SOC_IS_J2P_TYPE(driver_dev_id))
        {
#if defined(BCM_88850_A0)

            *found_driver = &soc_driver_bcm88850_a0;

#endif
        }
        else if (SOC_IS_J2X_TYPE(driver_dev_id))
        {
#if defined(BCM_88830_A0)

            if (driver_rev_id == J2X_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88830_a0;
            }
            else
#endif
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }

        else if (SOC_IS_Q2A_TYPE(driver_dev_id))
        {
#if defined(BCM_88480_A0)

            if (driver_rev_id == Q2A_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_a0;
            }
            else
#endif
#if defined(BCM_88480_B0)

            if (driver_rev_id >= Q2A_B0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_b0;
            }
            else
#endif
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }

#ifdef BCM_DNXF_SUPPORT

        else if (SOC_IS_RAMON_TYPE(driver_dev_id))
        {
#if defined(BCM_88790_A0)

            *found_driver = &soc_driver_bcm88790_a0;

#endif
        }

#endif

        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "a suitable driver was not found for specified "
                                     "device_id %u and revision_id %u\n%s", driver_dev_id, driver_rev_id, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_chip_type_set(
    int unit,
    uint16 dev_id)
{
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);
    {

        if (SOC_IS_JERICHO2_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO2;
            SOC_CHIP_STRING(unit) = "jericho2";
        }
        else if (SOC_IS_J2C_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_J2C;
            SOC_CHIP_STRING(unit) = "jericho2c";
        }
        else if (SOC_IS_J2P_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_J2P;
            SOC_CHIP_STRING(unit) = "jericho2p";
        }
        else if (SOC_IS_J2X_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_J2X;
            SOC_CHIP_STRING(unit) = "jericho2x";
        }
        else if (SOC_IS_Q2A_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_Q2A;
            SOC_CHIP_STRING(unit) = "q2a";
        }
#ifdef BCM_DNXF1_SUPPORT
        else if (SOC_IS_RAMON_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_RAMON;
        }
#endif
        else
        {
            si->chip_type = 0;
            SOC_CHIP_STRING(unit) = "???";
            LOG_VERBOSE_EX(BSL_LOG_MODULE, "soc_dnxc_chip_type_set: driver device %04x unexpected\n%s%s%s",
                           dev_id, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "soc_dnxc_chip_type_set: cannot find a match for driver device\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_CMICX_SUPPORT

static shr_error_e
soc_dnxc_linkscan_fw_init(
    int unit)
{
    static int default_fw_loaded[SOC_MAX_NUM_DEVICES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        soc_cmicx_linkscan_pause(unit);
        soc_cmicx_led_enable(unit, 0);

        if (!soc_feature(unit, soc_feature_cmicx_gen2))
        {
            soc_iproc_m0ssq_reset(unit);
        }

        if (default_fw_loaded[unit] == 0)
        {
            SOC_IF_ERROR_RETURN(soc_cmicx_led_linkscan_default_fw_load(unit));
            default_fw_loaded[unit] = 1;
        }
    }

    SHR_FUNC_EXIT;
}
#endif

static shr_error_e soc_dnxc_detach_no_soc_ndev_attached(
    int unit);

shr_error_e
soc_dnxc_attach(
    int unit)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    int mem, rv;

    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT(soc, sizeof(soc_control_t), "soc_control", "%s%s%s", EMPTY, EMPTY, EMPTY);
        SOC_CONTROL(unit) = soc;
    }
    else
    {
        sal_memset(soc, 0, sizeof(soc_control_t));
    }

#ifdef BCM_ACCESS_SUPPORT

    if (ACCESS_RUNTIME_INFO(unit) != NULL)
    {
        SHR_EXIT_WITH_LOG(SOC_E_INTERNAL, "new access is unexpectedly initialized%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
#endif

    SHR_IF_ERR_EXIT(soc_dnxc_configure_soc_control(unit));

#ifdef BCM_ACCESS_SUPPORT
    SHR_IF_ERR_EXIT(soc_dnxc_access_init(unit));
#endif

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit) || ACCESS_MAY_USE_BY_MCM(unit))
    {
#endif
        soc_dnxc_warn_if_device_is_used_by_diffrent_sdk(unit);
#ifdef BCM_ACCESS_SUPPORT
    }
#endif

    SHR_IF_ERR_EXIT_WITH_LOG(soc_cmic_intr_init(unit), "failed to Initialize CMIC interrupt framework\n%s%s%s", EMPTY,
                             EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(soc_cmic_intr_all_disable(unit));

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {

        SHR_IF_ERR_EXIT(access_handle_hotswap_and_dma_abort(unit));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(soc_cmicx_handle_hotswap_and_dma_abort(unit));
    }

    soc_led_driver_init(unit);

    soc_dnxc_info_config_init(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (!ACCESS_IS_INITIALIZED(unit) || ACCESS_MAY_USE_BY_MCM(unit))
    {
#endif
        if (!CM_IS_SUB_DEVICE(unit))
        {
            SHR_ALLOC_SET_ZERO_ERR_EXIT(SOC_PERSIST(unit),
                                        sizeof(soc_persist_t), "soc_persist", "%s%s%s",
                                        "failed to allocate soc_persist", EMPTY, EMPTY);
            sop = SOC_PERSIST(unit);
            sop->version = 1;

            rv = soc_memstate_alloc(unit);
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "soc_attach: unit %d fails to allocate mem space" " for memstate.\n"),
                           unit));
                return rv;
            }

            for (mem = 0; mem < NUM_SOC_MEM; mem++)
            {
                if (SOC_MEM_IS_VALID(unit, mem))
                {
                    SOP_MEM_STATE_MAX_INDEX_SET(unit, mem, SOC_MEM_INFO(unit, mem).index_max);
                }
            }
        }
        else
        {
            rv = soc_memstate_alloc(unit);
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "soc_attach: unit %d fails to allocate mem space" " for memstate.\n"),
                           unit));
                return rv;
            }
        }
#ifdef BCM_ACCESS_SUPPORT
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        SHR_IF_ERR_EXIT(soc_dnxf_lock_init(unit));

        SHR_IF_ERR_EXIT(mbcm_dnxf_init(unit));
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    SHR_IF_ERR_EXIT(soc_dnxc_linkscan_fw_init(unit));
#endif

exit:
    if (SHR_FUNC_ERR())
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "soc_dnx_attach: unit %d failed\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
        soc_dnxc_detach_no_soc_ndev_attached(unit);
    }
    else
    {
        ++soc_ndev_attached;
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_detach_no_soc_ndev_attached(
    int unit)
{
    int spl;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_NUM_VALID(unit))
    {
        SHR_IF_ERR_EXIT(_SHR_E_UNIT);
    }

    if (SOC_CONTROL(unit) == NULL)
    {
        SHR_EXIT();
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {

        SHR_IF_ERR_EXIT(access_deinit(unit));
    }
#endif
    soc_memstate_free(unit);

    SHR_FREE(SOC_PERSIST(unit));

    spl = sal_splhi();

    SHR_FREE(SOC_CONTROL(unit));
    SOC_CONTROL(unit) = NULL;

    sal_spl(spl);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_detach_no_soc_ndev_attached(unit));

    if (soc_ndev_attached)
    {
        --soc_ndev_attached;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "failed to update soc_ndev_attached");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_endian_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_CMCS_NUM(unit) = CMIC_CMC_NUM_MAX;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {

        if (!SOC_IS_NEW_ACCESS_INITIALIZED(unit))
        {
            SHR_IF_ERR_EXIT(soc_sbusdma_reg_init(unit));
        }
    }
#endif

    soc_endian_config(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_only_enough_for_access_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnxc_init_is_init_done_get(unit) == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "Access only mode can't be set on an initialized system");
    }

    sal_config_set("custom_feature_access_only", "1");
    sal_config_set("custom_feature_multithread_en*", NULL);
    sal_config_set("custom_feature_kbp_multithread_en*", NULL);
    SHR_IF_ERR_EXIT(bcm_init(unit));
    sal_config_set("custom_feature_access_only", NULL);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_hard_reset(
    int unit)
{
    int disable_hard_reset = 0x0;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_EXIT();
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SHR_EXIT();
    }
#endif
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
    }
#endif

    if (disable_hard_reset != 1)
    {
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {
            SHR_IF_ERR_EXIT(access_dnxc_device_hard_reset(unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, 4));
        }

        if (SOC_IS_JERICHO2(unit))
        {
            SHR_IF_ERR_EXIT(READ_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, &reg_val));
            soc_reg_field_set(unit, AXIIC_EXT_S0_FN_MOD_BM_ISSr, &reg_val, WRITE_ISS_OVERRIDEf, 1);
            SHR_IF_ERR_EXIT(WRITE_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, reg_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
