/** \file appl_ref_rx_init.c
 * 
 *
 * CPU RX procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_RX

/*
 * Includes
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bitop.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/common/rx.h>
#include <bcm/pkt.h>
#include <bcm/knet.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_rx_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

/*
 * DMA channel 1~7 are used for CPU RX
 */
#define RX_DMA_CHANNEL_INDEX_START      1
#define RX_DMA_CHANNEL_INDEX_END        7
#define RX_DMA_CHANNEL_AVAILABLE        7

shr_error_e
appl_dnx_rx_init(
    int unit)
{
    bcm_rx_cfg_t cfg;
    uint32 cosq;
    int nof_cpu_ports;
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    int channel_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    if (bcm_rx_active(unit))
    {
        SHR_EXIT();
    }

#ifdef INCLUDE_KNET
    /*
     * Init KNET
     */
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(bcm_knet_init(unit));
    }
#endif

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_COUNT(port_config.cpu, nof_cpu_ports);
    if (nof_cpu_ports == 0)
    {
        /*
         * don't start CPU rx if no cpu ports
         */
        SHR_EXIT();
    }

    /*
     * Initialize Dune RX Configuration Structure
     */
    bcm_rx_cfg_t_init(&cfg);

    cfg.pkt_size = RX_PKT_SIZE_DFLT;
    /*
     * Disable RX rate limitation
     */
    cfg.global_pps = 0;
    cfg.max_burst = 0;
    cfg.flags = 0;
    cfg.pkts_per_chain = RX_PPC_MAX;

    if (nof_cpu_ports <= RX_DMA_CHANNEL_AVAILABLE)
    {
        /*
         * Map CPU ports to a dedicated DMA channel
         */
        channel_index = RX_DMA_CHANNEL_INDEX_START;
        BCM_PBMP_ITER(port_config.cpu, logical_port)
        {
            int channel;
            int nof_priorities;
            uint32 dummy_flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;

            if (channel_index > RX_DMA_CHANNEL_INDEX_END)
            {
                break;
            }

            /*
             * Get number of priorities per port
             */
            SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &dummy_flags, &interface_info, &mapping_info));
            nof_priorities = mapping_info.num_priorities;
            channel = mapping_info.channel;

            cfg.chan_cfg[channel_index].chains = 2;
            cfg.chan_cfg[channel_index].rate_pps = 0;
            cfg.chan_cfg[channel_index].flags = 0;

            /*
             * Only set RX DMA COS reserved for PCI Host
             */
            if (channel >= dnx_data_dev_init.cmc.num_queues_pci_get(unit))
            {
                continue;
            }

            for (cosq = 0; cosq < nof_priorities; cosq++)
            {
                /*
                 * 1-1 map between CPU Port and DMA channel via DMA cos control
                 */
                SHR_IF_ERR_EXIT(bcm_rx_queue_channel_set(unit, (channel + cosq), channel_index));
            }
            channel_index++;
        }
    }
    else
    {
        /*
         * Map all CPU ports to DMA channel 1
         */
        cfg.chan_cfg[RX_DMA_CHANNEL_INDEX_START].flags = BCM_RX_F_TAKE_PORT_FROM_PARSER;
        cfg.chan_cfg[RX_DMA_CHANNEL_INDEX_START].cos_bmp = 0xffffffff;
        cfg.chan_cfg[RX_DMA_CHANNEL_INDEX_START].chains = 8;

        SHR_IF_ERR_EXIT(bcm_rx_queue_channel_set(unit, -1, RX_DMA_CHANNEL_INDEX_START));
    }

    SHR_IF_ERR_EXIT(bcm_rx_start(unit, &cfg));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /*
     * Stop RX
     */
    SHR_IF_ERR_EXIT(bcm_rx_stop(unit, NULL));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
