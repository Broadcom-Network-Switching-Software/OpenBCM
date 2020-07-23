

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
#include <soc/dnx/access.h>
#include <soc/drv.h>
#include <soc/led.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/drv.h>
#include <soc/mcm/driver.h>
#include <soc/linkctrl.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include "include/soc/dnx/adapter/adapter_reg_access.h"
#include <soc/dnxc/dnxc_cmic.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/ipoll.h>
#include <soc/intr_cmicx.h>
#include <soc/dcb.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <bcm_int/dnx/dram/dram.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif
extern const dnx_init_step_t dnx_init_deinit_seq[];

dnx_startup_test_function_f dnx_startup_test_functions[SOC_MAX_NUM_DEVICES] = { 0 };



shr_error_e
soc_dnx_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id,
    soc_driver_t ** found_driver)
{
    uint16 driver_dev_id;
    uint16 driver_rev_id;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id));
    {
        if (SOC_IS_JERICHO2_TYPE(driver_dev_id))
        {
            if (driver_rev_id == JERICHO2_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88690_a0;
            }
            else if ((driver_rev_id == JERICHO2_B0_REV_ID) || (driver_rev_id == JERICHO2_B1_REV_ID))
            {
                *found_driver = &soc_driver_bcm88690_b0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
#if !defined(NO_BCM_88800_A0)

        else if (SOC_IS_J2C_TYPE(driver_dev_id))
        {
            if ((driver_rev_id == J2C_A0_REV_ID) || (driver_rev_id == J2C_A1_REV_ID))
            {
                *found_driver = &soc_driver_bcm88800_a0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }

#endif
        else if (SOC_IS_J2P_TYPE(driver_dev_id))
        {
            if (driver_rev_id == J2P_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88850_a0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else if (SOC_IS_Q2A_TYPE(driver_dev_id))
        {
            if (driver_rev_id == Q2A_A0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_a0;
            }
            else if (driver_rev_id >= Q2A_B0_REV_ID)
            {
                *found_driver = &soc_driver_bcm88480_b0;
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "a suitable driver was not found for specified revision_id %u\n%s%s",
                                         driver_rev_id, EMPTY, EMPTY);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "a suitable driver was not found for specified "
                                     "device_id %u and revision_id %u\n%s", driver_dev_id, driver_rev_id, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_chip_type_set(
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
        else if (SOC_IS_Q2A_TYPE(dev_id))
        {
            si->chip_type = SOC_INFO_CHIP_TYPE_Q2A;
            SOC_CHIP_STRING(unit) = "q2a";
        }
        else
        {
            si->chip_type = 0;
            SOC_CHIP_STRING(unit) = "???";
            LOG_VERBOSE_EX(BSL_LOG_MODULE, "soc_dnx_chip_type_set: driver device %04x unexpected\n%s%s%s",
                           dev_id, EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "soc_dnx_chip_type_set: cannot find a match for driver device\n%s%s%s", EMPTY,
                                     EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_restore(
    int unit)
{
    int rv;
    uint16 device_id;
    uint8 revision_id;
    soc_control_t *soc;
    soc_info_t *si;
    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);

    sal_memset(soc, 0, sizeof(soc_control_t));

    
    rv = soc_cm_get_id(unit, &device_id, &revision_id);
    SHR_IF_ERR_EXIT(rv);
    rv = soc_dnx_chip_driver_find(unit, device_id, revision_id, &(soc->chip_driver));
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Couldn't find driver for unit %d (device 0x%04x, revision 0x%02x)\n",
                             unit, device_id, revision_id);

    rv = soc_dnx_chip_type_set(unit, device_id);
    SHR_IF_ERR_EXIT(rv);

    
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
    soc->soc_flags |= SOC_F_ATTACHED;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_attach(
    int unit)
{
    soc_control_t *soc;
    soc_persist_t *sop;
    int mem;
#ifdef BCM_CMICX_SUPPORT
    static int default_fw_loaded[SOC_MAX_NUM_DEVICES] = { 0 };
#endif

    SHR_FUNC_INIT_VARS(unit);

    
    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        soc = sal_alloc(sizeof(soc_control_t), "soc_control");
        if (soc == NULL)
        {
            SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
        }
        SOC_CONTROL(unit) = soc;
    }

    SHR_IF_ERR_EXIT(soc_dnx_restore(unit));

    
    soc_feature_init(unit);

    
    SHR_IF_ERR_EXIT(soc_cmicx_handle_hotswap_and_dma_abort(unit));

    
    soc_led_driver_init(unit);

    
    SHR_IF_ERR_EXIT_WITH_LOG(soc_cmic_intr_init(unit), "failed to Initialize CMIC interrupt framework\n%s%s%s", EMPTY,
                             EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(soc_cmic_intr_all_disable(unit));

    soc_dnx_info_config(unit);

    SOC_PERSIST(unit) = sal_alloc(sizeof(soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate soc_persist");
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof(soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (SOC_MEM_IS_VALID(unit, mem))
        {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
            
            if ((soc->memState[mem].lock = sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate memState lock");
            }

            
            sal_memset(soc->memState[mem].cache, 0, sizeof(soc->memState[mem].cache));
        }
        else
        {
            sop->memState[mem].index_max = -1;
        }
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        soc_cmicx_linkscan_pause(unit);
        soc_cmicx_led_enable(unit, 0);
        soc_iproc_m0ssq_reset(unit);

        if (default_fw_loaded[unit] == 0)
#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
            if (!SAL_BOOT_I2C_ACCESS)   
#endif

            {
                SOC_IF_ERROR_RETURN(soc_cmicx_led_linkscan_default_fw_load(unit));
                default_fw_loaded[unit] = 1;
            }
    }
#endif

exit:
    if (SHR_FUNC_ERR())
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "soc_dnx_attach: unit %d failed\n%s%s%s", unit, EMPTY, EMPTY, EMPTY);
        soc_dnx_detach(unit);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_detach(
    int unit)
{
    soc_control_t *soc;
    int spl;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_NUM_VALID(unit))
    {
        SHR_IF_ERR_EXIT(_SHR_E_UNIT);
    }

    if (SOC_PERSIST(unit) != NULL)
    {
        sal_free(SOC_PERSIST(unit));
    }
    SOC_PERSIST(unit) = NULL;

    soc = SOC_CONTROL(unit);
    if (soc == NULL)
    {
        SHR_EXIT();
    }
    
    spl = sal_splhi();
    sal_free(soc);
    SOC_CONTROL(unit) = NULL;
    sal_spl(spl);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_ledup_init(
    int unit)
{
    uint32 reg_val;
    int freq;
    uint32 clk_half_period, refresh_period;
    uint32 last_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    freq = dnx_data_device.general.core_clock_khz_get(unit);
    last_port = dnx_data_nif.global.last_port_led_scan_get(
    unit);
    
    refresh_period = (freq * 33);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &reg_val, REFRESH_CYCLE_PERIODf, refresh_period);

    SHR_IF_ERR_EXIT(WRITE_U0_LED_REFRESH_CTRLr(unit, reg_val));
    
    freq = 1000 * 1000000;
    clk_half_period = (freq + 39 * 1000000) / (20000000 * 2);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &reg_val, LEDCLK_HALF_PERIODf, clk_half_period);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_CLK_DIV_CTRLr(unit, reg_val));
    
    SHR_IF_ERR_EXIT(READ_U0_LED_ACCU_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &reg_val, LAST_PORTf, last_port);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_ACCU_CTRLr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_iproc_config(
    int unit)
{
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));

    SHR_IF_ERR_EXIT(READ_PAXB_0_FUNC0_IMAP1_7r(unit, &reg_val));
    
    soc_reg_field_set(unit, PAXB_0_FUNC0_IMAP1_7r, &reg_val, ADDRf, 0xe80);
    
    soc_reg_field_set(unit, PAXB_0_FUNC0_IMAP1_7r, &reg_val, VALIDf, 0x1);
    SHR_IF_ERR_EXIT(WRITE_PAXB_0_FUNC0_IMAP1_7r(unit, 0xe8000001));

    SHR_IF_ERR_EXIT(soc_dnx_ledup_init(unit));
    SHR_IF_ERR_EXIT(dnxc_block_instance_validity_override(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_ring_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x34422227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x55566333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000066));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x22222000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000655));
    }
    else if (SOC_IS_J2C(unit))
    {
        
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x30222227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x00440333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x33322000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x23333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x02222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000222));
    }
    else if (SOC_IS_Q2A(unit))
    {
        
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x33000007));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x44443333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00555444));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x20000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00022222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x20022000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000000));
    }
    else if (SOC_IS_J2P(unit))
    {
        
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x00000017));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x22220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000022));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00440000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x32220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x00000033));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x22200000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x23233332));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x23200003));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00400323));
    }
    SHR_IF_ERR_EXIT(soc_dnxc_cmic_sbus_timeout_set(unit, dnx_data_device.general.core_clock_khz_get(unit)  ,
                                                   SOC_CONTROL(unit)->schanTimeout));
    
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH0_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH1_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH2_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH3_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH4_ERRr(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_schan_config_and_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    
    soc_sand_access_conf_get(unit);
    
    SHR_IF_ERR_EXIT(soc_schan_init(unit));
exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_dnx_soft_reset(
    int unit)
{
    uint32 reg_val;
    int disable_hard_reset = 0x0;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    
    reg_val = 0;
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, MBU_RESETf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, TIME_SYNC_RESETf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IPROC_TS_RESETf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));

    
    if ((disable_hard_reset != 0) && (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit)))
    {
        
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, 7));
        SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 1);
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
        SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));

        
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, 1));

        
        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 0);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 1);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));

        
        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 0);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    }

    sal_usleep(5000);
    
    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 1));
    }
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr) &&
        (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "disable_gated_top_level_sampling", 1) == 0))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 0));
    }
    reg_val = 0;
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    sal_usleep(40000);
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);
    
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);

    
    SHR_IF_ERR_EXIT(READ_ECI_PIR_CONFIGURATIONSr(unit, &reg_val));
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 0);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 1);
    sal_usleep(200);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));
    if (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit))
    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_ENABLEf, 1);
    }
    else
    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_ENABLEf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_RESET_Nf, 1);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_ENABLEf, 1);
    }
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INIT_VALUEf, 0);
    soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
    soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_ENABLEf, 1);
    soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_ENABLEf, 1);
    sal_usleep(200);
    SHR_IF_ERR_EXIT(WRITE_ECI_PIR_CONFIGURATIONSr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_pll_configuration(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_sbus_polled_interrupts(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_unmask_mem_writes(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 i;

    CONST STATIC soc_reg_t indirect_wr_mask_reg[] = {
        DDHA_INDIRECT_WR_MASKr,
        DDHB_INDIRECT_WR_MASKr,
        DHC_INDIRECT_WR_MASKr,
        ERPP_INDIRECT_WR_MASKr,
        IPPA_INDIRECT_WR_MASKr,
        IPPB_INDIRECT_WR_MASKr,
        IPPC_INDIRECT_WR_MASKr,
        IPPD_INDIRECT_WR_MASKr,
        IPPE_INDIRECT_WR_MASKr,
        IPPF_INDIRECT_WR_MASKr,
        KAPS_INDIRECT_WR_MASKr,
        MDB_INDIRECT_WR_MASKr,
        SCH_INDIRECT_WR_MASKr,
        TCAM_INDIRECT_WR_MASKr
    };

    CONST STATIC soc_reg_t indirect_force_bubble_reg[] = {
        CDUM_INDIRECT_FORCE_BUBBLEr,
        CDU_INDIRECT_FORCE_BUBBLEr,
        CFC_INDIRECT_FORCE_BUBBLEr,
        CGM_INDIRECT_FORCE_BUBBLEr,
        CLU_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED    
        DCC_INDIRECT_FORCE_BUBBLEr,
#endif
        CRPS_INDIRECT_FORCE_BUBBLEr,
        DDHA_INDIRECT_FORCE_BUBBLEr,
        DDHB_INDIRECT_FORCE_BUBBLEr,
        DHC_INDIRECT_FORCE_BUBBLEr,
        DPC_INDIRECT_FORCE_BUBBLEr,
        DQM_INDIRECT_FORCE_BUBBLEr,
        ECGM_INDIRECT_FORCE_BUBBLEr,
        ECI_INDIRECT_FORCE_BUBBLEr,
        EDB_INDIRECT_FORCE_BUBBLEr,
        EPNI_INDIRECT_FORCE_BUBBLEr,
        EPS_INDIRECT_FORCE_BUBBLEr,
        ERPP_INDIRECT_FORCE_BUBBLEr,
        ETPPA_INDIRECT_FORCE_BUBBLEr,
        ETPPB_INDIRECT_FORCE_BUBBLEr,
        ETPPC_INDIRECT_FORCE_BUBBLEr,
        EVNT_INDIRECT_FORCE_BUBBLEr,
        FCR_INDIRECT_FORCE_BUBBLEr,
        FCT_INDIRECT_FORCE_BUBBLEr,
        FDR_INDIRECT_FORCE_BUBBLEr,
        FDT_INDIRECT_FORCE_BUBBLEr,
        FSRD_INDIRECT_FORCE_BUBBLEr,
        FQP_INDIRECT_FORCE_BUBBLEr,
        HBMC_INDIRECT_FORCE_BUBBLEr,
        ILE_INDIRECT_FORCE_BUBBLEr,
        ILU_INDIRECT_FORCE_BUBBLEr,
        IPPA_INDIRECT_FORCE_BUBBLEr,
        IPPB_INDIRECT_FORCE_BUBBLEr,
        IPPC_INDIRECT_FORCE_BUBBLEr,
        IPPD_INDIRECT_FORCE_BUBBLEr,
        IPPE_INDIRECT_FORCE_BUBBLEr,
        IPPF_INDIRECT_FORCE_BUBBLEr,
        IPS_INDIRECT_FORCE_BUBBLEr,
        IPT_INDIRECT_FORCE_BUBBLEr,
        IQM_INDIRECT_FORCE_BUBBLEr,
        IRE_INDIRECT_FORCE_BUBBLEr,
        ITPPD_INDIRECT_FORCE_BUBBLEr,
        ITPP_INDIRECT_FORCE_BUBBLEr,
        KAPS_INDIRECT_FORCE_BUBBLEr,
        MACT_INDIRECT_FORCE_BUBBLEr,
        MCP_INDIRECT_FORCE_BUBBLEr,
        MDB_INDIRECT_FORCE_BUBBLEr,
        MRPS_INDIRECT_FORCE_BUBBLEr,
        MTM_INDIRECT_FORCE_BUBBLEr,
        OAMP_INDIRECT_FORCE_BUBBLEr,
        OCB_INDIRECT_FORCE_BUBBLEr,
        OLP_INDIRECT_FORCE_BUBBLEr,
        PEM_INDIRECT_FORCE_BUBBLEr,
        PQP_INDIRECT_FORCE_BUBBLEr,
        RQP_INDIRECT_FORCE_BUBBLEr,
        RTP_INDIRECT_FORCE_BUBBLEr,
        SCH_INDIRECT_FORCE_BUBBLEr,
        SPB_INDIRECT_FORCE_BUBBLEr,
        SQM_INDIRECT_FORCE_BUBBLEr,
        TCAM_INDIRECT_FORCE_BUBBLEr,
        
        ESB_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED    
        FASIC_INDIRECT_FORCE_BUBBLEr,
        FLEXEWP_INDIRECT_FORCE_BUBBLEr,
        FSAR_INDIRECT_FORCE_BUBBLEr,
        FSCL_INDIRECT_FORCE_BUBBLEr,
#endif
        FEU_INDIRECT_FORCE_BUBBLEr
    };

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    
    for (i = 0; i < sizeof(indirect_wr_mask_reg) / sizeof(indirect_wr_mask_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_wr_mask_reg[i]))
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set_all_instances(unit, indirect_wr_mask_reg[i], 0, reg_above_64_val));
        }
    }
    
    for (i = 0; i < sizeof(indirect_force_bubble_reg) / sizeof(indirect_force_bubble_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_force_bubble_reg[i]))
        {
            SHR_IF_ERR_EXIT(soc_reg32_set_all_instances(unit, indirect_force_bubble_reg[i], 0, 0x020019));
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_soft_init_sch(
    int unit)
{

    uint32 error_check_en;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 mem_data;
    uint32 all_ones[SOC_MAX_MEM_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(entry));
    sal_memset(all_ones, 0xff, sizeof(all_ones));

    
    error_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_err_check);
    SOC_FEATURE_CLEAR(unit, soc_feature_schan_err_check);
    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        mem_data = 0;
    }
    else
    {
        mem_data = 1;
    }
    SHR_IF_ERR_EXIT(soc_mem_write(unit, SCH_SCHEDULER_INITm, MEM_BLOCK_ALL, 0, &mem_data));

    if (soc_sand_is_emulation_system(unit))
    {
        sal_sleep(10);
    }
    else
    {
        sal_usleep(1000);
    }

    
    if (!(SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
    {
        
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, TOKEN_COUNTf, all_ones);
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, SLOW_STATUSf, all_ones);
        soc_mem_field_width_fit_set(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, entry, FULLf, all_ones);

        SHR_IF_ERR_EXIT(sand_fill_table_with_entry_skip_emul_err(unit, SCH_TOKEN_MEMORY_CONTROLLER_TMCm, MEM_BLOCK_ALL,
                                                                 entry));
    }
    
    if (error_check_en)
    {
        SOC_FEATURE_SET(unit, soc_feature_schan_err_check);
    }

    
    SHR_IF_ERR_EXIT(dnx_sch_reserve_se_update(unit));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_dnx_soft_init(
    int unit,
    int without_fabric,
    int without_ile)
{
    uint32 reg_val, field_val;
    uint16 dev_id;
    uint8 rev_id;

    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t field_above_64_val = { 0 };
    soc_reg_above_64_val_t epni_field_above_64_val = { 0 };
    int array_index, array_index_min = 0, array_index_max = 0;
    soc_reg_above_64_val_t zero_field_val = { 0 };
    dnx_hbmc_mrs_values_t mrs_values;
    uint32 reset_cdu_eth[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    bcm_pbmp_t cdu_eth_pbmp[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    int saved_rx_enable[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint64 saved_mac_ctrl[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint32 rx_fifo_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 tx_port_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 fc_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 aligner_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 instru_reset_store[DNX_DATA_MAX_NIF_ETH_CDU_NOF] = { 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_save(unit, &mrs_values));

    

    array_index_min = SOC_REG_FIRST_ARRAY_INDEX(unit, SCH_DVS_NIF_CONFIGr);
    array_index_max = SOC_REG_NUMELS(unit, SCH_DVS_NIF_CONFIGr) + array_index_min;

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 0xffffffff;
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 1;
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    
    if (SOC_IS_Q2A_A0(unit))
    {
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0x0));
        }
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }
    
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCHf, 0);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHAf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPSf, 0);
        if (SOC_IS_Q2A(unit))
        {
            
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FLEXEWPf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSCLf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSARf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FASICf, 0);
        }
    }
    else if (SOC_IS_JERICHO2_ONLY(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_7f, 0);
    }

    
    if ((without_fabric != 0) && (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2C(unit) || SOC_IS_J2P(unit)))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCRf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDAf, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_0f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_1f, 0);
        }
        else
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDRf, 0);
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTf, 0);
        if (SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_0f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_1f, 0);
        }
        else
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTLf, 0);
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCTf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MESH_TOPOLOGYf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_RTPf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_7f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_8f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_9f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_10f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_11f, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_12f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_13f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_14f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_15f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_16f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_17f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_18f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_19f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_20f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_21f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_22f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_23f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_24f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_25f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_26f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_27f, 0);
            if (SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_28f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_29f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_30f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_31f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_32f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_33f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_34f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_35f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_36f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_37f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_38f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_39f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_40f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_41f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_42f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_43f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_44f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_45f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_46f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_47f,
                                             0);
            }
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_5f, 0);
        if (SOC_IS_JERICHO2_ONLY(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_6f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_7f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_8f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_9f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_10f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_11f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_12f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_13f, 0);
            if (SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_14f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_15f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_16f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_17f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_18f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_19f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_20f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_21f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_22f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_23f,
                                             0);
            }
        }
    }
    
    if (without_ile != 0)
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, 0);
        if (SOC_IS_J2C(unit) || SOC_IS_J2P(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, 0);

            if (SOC_IS_J2C(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, 0);
            }
        }
        
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
        {
            bcm_pbmp_t elk_ports_bitmap;
            bcm_port_t port = 0, active_port;
            dnx_algo_port_info_s active_port_info;
            int pm_idx, phy_id, is_active = 0;
            int nof_pm_cdu;
            bcm_pbmp_t nif_phys;
            bcm_pbmp_t pm_pmbp, pm_pmbp_temp;
            soc_field_t cdu_reset_field[] = { BLOCKS_SOFT_INIT_CDU_0f, BLOCKS_SOFT_INIT_CDU_1f, BLOCKS_SOFT_INIT_CDU_2f,
                BLOCKS_SOFT_INIT_CDU_3f
            };
            
            nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
            for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
            {
                BCM_PBMP_CLEAR(cdu_eth_pbmp[pm_idx]);
            }
            BCM_PBMP_CLEAR(elk_ports_bitmap);
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &elk_ports_bitmap));
            BCM_PBMP_ITER(elk_ports_bitmap, port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
                for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
                {
                    
                    pm_pmbp = dnx_data_nif.eth.pm_properties_get(unit, pm_idx)->phys;
                    BCM_PBMP_ASSIGN(pm_pmbp_temp, pm_pmbp);
                    BCM_PBMP_AND(pm_pmbp_temp, nif_phys);
                    if (BCM_PBMP_NOT_NULL(pm_pmbp_temp))
                    {
                        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                     cdu_reset_field[pm_idx], 0);
                        
                        BCM_PBMP_REMOVE(pm_pmbp, pm_pmbp_temp);
                        BCM_PBMP_ITER(pm_pmbp, phy_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
                            if (is_active)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                                (unit, phy_id, FALSE, 0, &active_port));
                                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, active_port, &active_port_info));
                                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, active_port_info, TRUE, TRUE))
                                {
                                    reset_cdu_eth[pm_idx] = 1;
                                    BCM_PBMP_PORT_ADD(cdu_eth_pbmp[pm_idx], active_port);
                                }
                            }
                        }
                    }
                }
            }   
        }
    }

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(2000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
        
        sal_memset(saved_rx_enable, 0x0, sizeof(saved_rx_enable));
        sal_memset(saved_mac_ctrl, 0x0, sizeof(saved_mac_ctrl));

        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, &rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, &tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_FC_RESETr, cdu_idx, 0, &fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, 0x1));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, &aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, &instru_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, 0x0));
            
            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeIn,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_REG_IS_VALID(unit, EPNI_INIT_EPNI_NIF_TXIr))
    {
        
        int field_size = 0;
        field_size = soc_reg_field_length(unit, EPNI_INIT_EPNI_NIF_TXIr, INIT_EPNI_NIF_TXIf);

        SOC_REG_ABOVE_64_CREATE_MASK(epni_field_above_64_val, field_size, 0);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));

        sal_usleep(1000);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SOC_REG_ABOVE_64_CLEAR(epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        sal_usleep(5000);
    }

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        bcm_core_t core;
        int egq_if, nof_egq_if;

        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_REG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        nof_egq_if = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
        
        core = DBAL_CORE_ALL;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (egq_if = 0; egq_if < nof_egq_if; egq_if++)
        {
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_FQP_TXI_BIT, INST_SINGLE, (uint32) 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_FQP_TXI_BIT, INST_SINGLE, (uint32) 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        SOC_REG_ABOVE_64_CREATE_MASK(zero_field_val, 0, 0);
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FQPf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_EPNIf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ESBf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    }
    sal_usleep(1000);
    if (SOC_REG_IS_VALID(unit, ESB_ESB_INITr))
    { 
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 0));
        sal_usleep(5000);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 1));
        sal_usleep(5000);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_NMGf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }

    

    if (without_ile != 0)
    {
        int block, array_iter;
        soc_reg_t reg;
        uint32 nof_block = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

        reg = ILE_ILKN_TX_PORT_FORCE_DATA_PCK_FLUSHr;
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
    }

    if (SOC_IS_Q2A(unit))
    {
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }
    else if (SOC_IS_J2C(unit))
    {
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_6f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_7f, zero_field_val); 
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }

    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(5000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
        
        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, instru_reset_store[cdu_idx]));
            
            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeOut,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_IS_Q2A_A0(unit))
    {
        
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0xff));
        }

        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }
    if (SOC_REG_IS_VALID(unit, ESB_ESB_CALENDER_SETTINGr))
    {
        
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
    }
    
    {
        SHR_IF_ERR_EXIT(soc_dnx_soft_init_sch(unit));
    }

    if (!SAL_BOOT_PLISIM)
    { 
        uint32 prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat = 0, init_not_done = 1, i;

        for (i = 2; i && init_not_done;)
        {
            SHR_IF_ERR_EXIT(READ_RQP_RQP_BLOCK_INIT_STATUSr(unit, 0, &prp_init));
            SHR_IF_ERR_EXIT(READ_PQP_EGQ_BLOCK_INIT_STATUSr(unit, 0, &eqm_init));       
            SHR_IF_ERR_EXIT(READ_FQP_FQP_BLOCK_INIT_STATUSr(unit, 0, &user_counter_init));
            SHR_IF_ERR_EXIT(READ_EPNI_EPNI_INIT_STATUSr(unit, 0, &aligner_init));
            if (SOC_REG_IS_VALID(unit, ESB_FPC_0_STATr))
            {
                SHR_IF_ERR_EXIT(READ_ESB_FPC_0_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat = soc_reg_field_get(unit, ESB_FPC_0_STATr, init_not_done, FPC_0_INIT_STATf);
                SHR_IF_ERR_EXIT(READ_ESB_FPC_1_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_1_STATr, init_not_done, FPC_1_INIT_STATf) << 1;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_2_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_2_STATr, init_not_done, FPC_2_INIT_STATf) << 2;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_3_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_3_STATr, init_not_done, FPC_3_INIT_STATf) << 3;
            }
            
            eqm_init = eqm_init & 0x7;

            init_not_done = prp_init | eqm_init | user_counter_init | aligner_init | fpc_init_stat;
            if (init_not_done != 0 && --i != 0)
            {
                sal_usleep(5000);
                
                LOG_ERROR(BSL_LS_SOC_INIT, ("Warning: waiting for EGQ blocks to be ready\n"));
            }

        }
        if (init_not_done != 0)
        { 
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U
                       (unit,
                        "EGQ blocks did not become ready  prp_init=%u eqm_init=%u user_counter_init=%u aligner_init=%u fpc_init_stat=0x%x\n"),
                       prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat));
        }
    }

    
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    
    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_EXIT(READ_ECI_VERSION_REGISTERr(unit, reg_above_64_val));
    field_val = soc_reg_above_64_field32_get(unit, ECI_VERSION_REGISTERr, reg_above_64_val, CHIP_TYPEf);
#ifdef JER2B0_BRINGUP_DONE
    if ((field_val & 0xffff) != dev_id)
#else
    if (((field_val ^ dev_id) & 0xfff0) != 0)
#endif
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "Chip version is wrong: unit %d expected 0x%x and found 0x%x\n%s", unit,
                     dev_id, field_val, EMPTY);
    }
    SHR_IF_ERR_EXIT(soc_dnxc_verify_device_init(unit));
    SHR_IF_ERR_EXIT(READ_SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, &reg_val));
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, IRE_STATIC_CONFIGURATIONr, 0, 0, &reg_val));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(unit));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_restore(unit, &mrs_values));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_perform_bist_tests(
    int unit)
{
    dnx_startup_test_function_f test_function = dnx_startup_test_functions[unit];
    utilex_seq_step_t *utilex_list = NULL;
    uint32 mbist_soc_prop_enable = dnx_data_device.general.bist_enable_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {

        
        if (SOC_IS_Q2A(unit)
            && (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cpu2tap_enable", 1) != 0))
        {
            SHR_IF_ERR_EXIT(soc_q2a_cpu2tap_init_mems(unit));
        }
        
        if (test_function != NULL)
        {
            test_function(unit);
        }
        
        if (((any_bist_performed[unit / 32] & (((uint32) 1) << (unit % 32))) ||
             mbist_soc_prop_enable) && !SAL_BOOT_PLISIM)
        {
            int step_found, i, mbist_arg = (mbist_soc_prop_enable != 1);

            int deinit_steps[] = { DNX_INIT_STEP_DMA, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT, DNX_INIT_STEP_PLL };
            int init_steps[] =
                { DNX_INIT_STEP_IPROC, DNX_INIT_STEP_HARD_RESET, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT,
                DNX_INIT_STEP_SOFT_RESET,
                DNX_INIT_STEP_DMA, DNX_INIT_STEP_PLL,
                DNX_INIT_STEP_SBUS_INTERRUPTS
            };
            do
            {
                if (SOC_IS_J2P(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2p(unit, mbist_arg));
                }
                else if (SOC_IS_J2C(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2c(unit, mbist_arg));
                }
                else if (SOC_IS_Q2A(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_q2a(unit, mbist_arg));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_jr2(unit, mbist_arg));
                }
            }
            while (mbist_soc_prop_enable == 9999);   

            
            test_was_run_and_suceeded[unit / 32] |= ((uint32) 1) << (unit % 32);

            SHR_IF_ERR_EXIT(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &utilex_list));

            
            for (i = 0; i < sizeof(deinit_steps) / sizeof(deinit_steps[0]); i++)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, deinit_steps[i], 0, &step_found));
                if (step_found != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                }
            }

            
            for (i = 0; i < sizeof(init_steps) / sizeof(init_steps[0]); i++)
            {
                SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, init_steps[i], 1, &step_found));
                if (step_found != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                }
            }

        }

        
        if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
        {
            int hbm_id = dnx_data_dram.general_info.max_nof_drams_get(unit);
            for (--hbm_id; hbm_id >= 0; --hbm_id)
            {
                if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap & (1 << hbm_id)))
                {
                    SHR_IF_ERR_EXIT(soc_set_hbm_wrst_n_to_0(unit, hbm_id));
                }
            }
        }

    }
exit:
    if (utilex_list != NULL)
    {
        dnx_init_step_list_destory(unit, utilex_list);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_endian_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SOC_CMCS_NUM(unit) = 2;
    SHR_IF_ERR_EXIT(soc_sbusdma_reg_init(unit));
    soc_endian_config(unit);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_polling_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_hard_reset(
    int unit)
{
    int disable_hard_reset = 0x0;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);

    if (disable_hard_reset == 0)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, 4));

        if (SOC_IS_JERICHO2_ONLY(unit))
        {       
            SHR_IF_ERR_EXIT(READ_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, &reg_val));
            soc_reg_field_set(unit, AXIIC_EXT_S0_FN_MOD_BM_ISSr, &reg_val, WRITE_ISS_OVERRIDEf, 1);
            SHR_IF_ERR_EXIT(WRITE_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, reg_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_init(
    int unit)
{
    int cmc_index = 0;
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    

    
    SOC_CMCS_NUM(unit) = dnx_data_dev_init.cmc.cmc_num_get(unit);
    
    NUM_CPU_COSQ(unit) = dnx_data_dev_init.cmc.num_cpu_cosq_get(unit);
    
    SOC_PCI_CMC(unit) = dnx_data_dev_init.cmc.cmc_pci_get(unit);
    
    SOC_PCI_CMCS_NUM(unit) = dnx_data_dev_init.cmc.pci_cmc_num_get(unit);
    
    SOC_ARM_CMC(unit, 0) = dnx_data_dev_init.cmc.cmc_uc0_get(unit);
    
    SOC_ARM_CMC(unit, 1) = dnx_data_dev_init.cmc.cmc_uc1_get(unit);
    
    NUM_CPU_ARM_COSQ(unit, 0) = dnx_data_dev_init.cmc.num_queues_pci_get(unit);
    
    NUM_CPU_ARM_COSQ(unit, 1) =
        dnx_data_dev_init.cmc.num_queues_uc0_get(unit) + dnx_data_dev_init.cmc.num_queues_uc1_get(unit);
    if ((NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1)) > NUM_CPU_COSQ(unit))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM,
                                 "Number of cosq reserved for CMC 0/1 exceed. num_queues_pci %u num_queues_uc0 %d\n%s",
                                 NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1), EMPTY);
    }

    
    for (cmc_index = 0; cmc_index < SOC_CMCS_NUM(unit); cmc_index++)
    {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc_index), 0, NUM_CPU_COSQ(unit));
    }

    
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 0), 0, NUM_CPU_ARM_COSQ(unit, 0));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 1), NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1));

    
    soc_dcb_unit_init(unit);

    if (SOC_WARM_BOOT(unit))
    {
        
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, TRUE));
    }

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;
    
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, dnx_data_intr.general.mem_clear_chunk_size_get(unit));
#endif

    

    if (soc_feature(unit, soc_feature_sbusdma))
    {
        
        SHR_IF_ERR_EXIT(soc_sbusdma_lock_init(unit));   
        if (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)) || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)))  
        {
            
            SHR_IF_ERR_EXIT(soc_sbusdma_init(unit, dnx_data_intr.general.sbus_dma_interval_get(unit),
                                             dnx_data_intr.general.sbus_dma_intr_enable_get(unit)));
            SHR_IF_ERR_EXIT(sand_init_fill_table(unit));
        }
    }
    
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_init(unit));

    
    SHR_IF_ERR_EXIT(soc_cmicx_uc_msg_init(unit));

    
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_RESERVED_22r, reg_above_64_val, FIELD_30_33f, 0);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (soc_feature(unit, soc_feature_sbusdma))
    {
        if (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))
            || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm)))
        {
            SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));
        }
        (void) soc_sbusdma_lock_deinit(unit);
    }

    
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_deinit(unit));

    SHR_IF_ERR_EXIT(soc_dma_detach(unit));

    SHR_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));

    
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mutexes_init(
    int unit)
{
    int cmc;
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    SOC_PCI_CMCS_NUM(unit) = dnx_data_dev_init.cmc.pci_cmc_num_get(unit);

    
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
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_access_mem_mutex_init(unit), "Failed to allocate mutex Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);    

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

    
    SHR_IF_ERR_EXIT(dnx_access_mem_mutex_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mutexes_deinit(
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

    
    SHR_IF_ERR_EXIT(dnx_access_mem_mutex_deinit(unit));

    
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
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_access_mem_mutex_deinit(unit), "Failed to allocate mutex Lock\n%s%s%s", EMPTY, EMPTY, EMPTY);  
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mark_not_inited(
    int unit)
{
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    soc->soc_flags &= ~SOC_F_INITED;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_info_config(
    int unit)
{
    soc_info_t *si;
    int mem, blk;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);
    si->num_ucs = 2;
    si->num_time_interface = 1;

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

    
    SOC_USE_GPORT_SET(unit, TRUE);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_dnx_feature_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    soc_feature_init(unit);

    SHR_FUNC_EXIT;
}

int
soc_dnx_info_config_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_CONT(soc_dnx_restore(unit));

    
    SHR_IF_ERR_CONT(soc_dnx_feature_init(unit));

    
    SOC_DETACH(unit, 0);

    SHR_FUNC_EXIT;
}

int
soc_dnx_rcpu_port_init(
    int unit,
    int port_i)
{
    uint32 rval = 0;

    SHR_FUNC_INIT_VARS(unit);
    {
        if (port_i < 32)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_0r(unit, &rval));
            rval |= 0x1 << port_i;
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_0r(unit, rval));
        }
        else if (port_i < 64)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_1r(unit, &rval));
            rval |= 0x1 << (port_i - 32);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_1r(unit, rval));
        }
        else if (port_i < 96)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_2r(unit, &rval));
            rval |= 0x1 << (port_i - 64);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_2r(unit, rval));
        }
        else if (port_i < 128)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_3r(unit, &rval));
            rval |= 0x1 << (port_i - 96);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_3r(unit, rval));
        }
        else if (port_i < 160)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_4r(unit, &rval));
            rval |= 0x1 << (port_i - 128);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_4r(unit, rval));
        }
        else if (port_i < 192)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_5r(unit, &rval));
            rval |= 0x1 << (port_i - 160);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_5r(unit, rval));
        }
        else if (port_i < 224)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_6r(unit, &rval));
            rval |= 0x1 << (port_i - 192);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_6r(unit, rval));
        }
        else if (port_i < 256)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_7r(unit, &rval));
            rval |= 0x1 << (port_i - 224);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_7r(unit, rval));
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU port range is 0 - 255\n")));
            SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_rcpu_init(
    int unit)
{
    int port_i = 0;
    const dnx_data_access_rcpu_rx_t *rcpu_rx;

    SHR_FUNC_INIT_VARS(unit);

    rcpu_rx = dnx_data_access.rcpu.rx_get(unit);

    SOC_PBMP_ITER(rcpu_rx->pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(soc_dnx_rcpu_port_init(unit, port_i));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_block_enable_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (SOC_IS_Q2A(unit))
    {
        
#ifndef PLISIM
        if (!(SAL_BOOT_PLISIM))
        {
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fasic_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).flexewp_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fsar_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fscl_block] = 0;
        }
#endif
    }
    
    SHR_IF_ERR_EXIT(dnx_dram_blocks_enable_set(unit));

exit:
    SHR_FUNC_EXIT;
}


int
dnx_tbl_is_dynamic(
    int unit,
    soc_mem_t mem)
{
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    return soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem);
}
