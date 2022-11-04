/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * 
 * $Copyright: (c) 2011 Broadcom Corp.
 * All Rights Reserved.$
 *
 *File: cint_sand_rxtx.c
 * Purpose: An example of receiving packets and sending packets
 *
 * 1. Example of receiving packets
 * 2. Example of sending packets out from a specified port
 * 3. Example of mapping all CPU ports on same DMA channel
 * 4. Example of configuring the map between CPU port and DMA channel
 *
 * 1. Example of receiving packets
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
     cint ../../../../src/examples/dnx/rx/cint_dnx_rxtx.c
     cint
     pkt_start(0);
     exit;
 * Run traffic:
     tx 1 pbm=cpu200 data=0x001100000022000007000100810000640800450000350000000080002cc47fffff038fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Expected:
 *    packet is dumped out by registered callback.
 *    in order to remove callbacks:
     cint
     pkt_stop(0);
     exit;
 * Test Scenario - end
 *
 * 2. Example of sending packets out from a specified port
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
     cint ../../../../src/examples/dnx/rx/cint_dnx_rxtx.c
     cint
     uint8 pkt_data[256] = {0x00,0x0C,0x00,0x02,0x00,0x01,
                            0x00,0x0C,0x00,0x02,0x00,0x00,
                            0x81,0x00,0x00,0x01,0x88,0x47,
                            0x00,0xD0,0x50,0x40,
                            0x00,0xD8,0x01,0x40,
                            0x00,0x11,0x00,0x00,0x01,0x12,
                            0x00,0x0C,0x00,0x02,0x00,0x00,
                            0x81,0x00,0x00,0x05,0x88,0xF7,
                            0x00,0x02,0x00,0x2C,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
                            0xFE,0x00,0x00,0x66,0x00,0x01,0x22,0x68,
                            0x00,0xFA,0x00,0x00,0x00,0x00,0x00,0x9A,
                            0x06,0x2F,0x7C,0x00};
 
     pkt_send(0, 13, pkt_data, 88);
     exit;
 * Expected:
 *    1588oETHoMPLSoMPLSoETH is sent out from port 13
 * Test Scenario - end
 *
 * 3. Example of mapping all CPU ports on same DMA channel
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
     cint ../../../../src/examples/dnx/rx/cint_dnx_rxtx.c
     cint
     pkt_start_multiple_ports_to_same_channel(0, 1);
     exit;
 * Expected:
 *    CPU RX works
 * Test Scenario - end
 *
 * 4. Example of configuring the map between CPU port and DMA channel
 *
 * Set up sequence:
 * Test Scenario - start
     cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
     cint ../../../../src/examples/dnx/rx/cint_dnx_rxtx.c
     cint
     pkt_map_cpu_ports_to_dma_channels(0);
     exit;
 * Expected:
 *    CPU RX works
 * Test Scenario - end
 */

int module_header_size = 16;
int ptch_type2_size = 2;
int itmh_base_jr1_size = 4;
int itmh_base_jr2_size = 5;
uint32 receive_packets_count = 0;

/** Example of origianl packet 1588oETHoMPLSoMPLSoETH */
uint8 rxtx_pkt_data[256] = {0x00,0x0C,0x00,0x02,0x00,0x01,
                           0x00,0x0C,0x00,0x02,0x00,0x00,
                           0x81,0x00,0x00,0x01,0x88,0x47,
                           0x00,0xD0,0x50,0x40,
                           0x00,0xD8,0x01,0x40,
                           0x00,0x11,0x00,0x00,0x01,0x12,
                           0x00,0x0C,0x00,0x02,0x00,0x00,
                           0x81,0x00,0x00,0x05,0x88,0xF7,
                           0x00,0x02,0x00,0x2C,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
                           0xFE,0x00,0x00,0x66,0x00,0x01,0x22,0x68,
                           0x00,0xFA,0x00,0x00,0x00,0x00,0x00,0x9A,
                           0x06,0x2F,0x7C,0x00};

int rx_dma_channel_index_start = 1;
int rx_dma_channel_index_end = 7;
int rx_dma_channel_available = 7;

/*
 * Receive callbacks for received packets. In this example, it only dumps packets and leaves packets to other callbacks
 * BCM_RX_NOT_HANDLED - The packet is not handled, SDK continues to processing
 * BCM_RX_HANDLED -  The packet is handled, will not be offered to any further callbacks, SDK continues to free packet
 * BCM_RX_HANDLED_OWNED - The packet is handled and the caller has taken control of the packet data, the caller takes responsibility to free the packet.
 */
bcm_rx_t
pkt_dump_cb(int unit, bcm_pkt_t *pkt, void *cookie)
{
    uint8 *data;
    int offset = 0;
    int len = pkt->pkt_len;
    int rv = BCM_E_NONE;

    while (len--)
    {
        if ((offset & 15) == 0)
        {
            printf("\n%04x", offset);
        }

        rv = bcm_pkt_byte_index(pkt, offset, NULL, NULL, &data);
        if (rv != BCM_E_NONE)
        {
            /** End of packets data */
            if (rv == BCM_E_NOT_FOUND)
            {
                rv = BCM_E_NONE;
            }
            break;
        }
        else
        {
            if (data)
            {
                printf(" %02x", *data);
                offset++;
            }
        }
    }
    printf("\n");
    receive_packets_count++;

    return BCM_RX_NOT_HANDLED;
}

/*
 * Register to receive callbacks for received packets.
 * Normally RX is already active after loading bcm.user, or RX will be started with default configuration
 */
int
pkt_start(
    int unit)
{
    void *pkt_user_data = NULL;

    if (!bcm_rx_active(unit))
    {
        /** Initialize RX as default */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_cfg_init(unit), "");
        /** Start RX with default configraiton */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_start(unit, NULL), "");
    }

    receive_packets_count = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_register(unit, "CINT-RX", pkt_dump_cb, BCM_RX_PRIO_MAX, pkt_user_data, BCM_RCO_F_ALL_COS), "");

    return BCM_E_NONE;
}


/*
 * Unregister to receive callbacks for received packets.
 */
int
pkt_stop(
    int unit)
{
    receive_packets_count = 0;
    return bcm_rx_unregister(unit, pkt_dump_cb, BCM_RX_PRIO_MAX);
}


/*
 * Example of sending Packets out from a specified port.
 * Input variables
 * - port: specify the port from which the packet will be sent out
 * - data: original packet data buffer without any headers. when data is NULL, using default rxtx_pkt_data
 * - len:  length of original packet data
 * Note
 * 1. On JR2 and above devices, Module Header should be appended to the start of packet data explicitly.
 * The size of the Module Header is 16 bytes, only the first byte is used to deliver CPU channel.
 */


int
pkt_send(
    int unit,
    int port,
    uint8 *data,
    int len)
{

    bcm_pkt_t *pkt =NULL;
    int size = 0;
    int dest_byte = 0;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint8 module_header[16] ={0};
    uint8 ptch_header[2] ={0};
    uint8 itmh_base_jr1[4] ={0};
    uint8 itmh_base_jr2[5] ={0};
    bcm_gport_t sysport_gport;
    bcm_gport_t dest_port;

    /** Append 16 bytes Module header */
    size = module_header_size + ptch_type2_size + itmh_base_jr2_size + len;

    BCM_IF_ERROR_RETURN_MSG(bcm_pkt_alloc(unit, size, 0, &pkt), "");

    /** Module Header will be filled in bcm_tx(), just reserve space */
    bcm_pkt_memcpy(pkt, dest_byte, &module_header, sizeof(module_header));
    dest_byte += module_header_size;

    /** First bit of  PTCH is 0 to indicate that next header is ITMH*/
    bcm_pkt_memcpy(pkt, dest_byte, &ptch_header, sizeof(ptch_header));
    dest_byte += ptch_type2_size;

    /** Convert port to system gport */
    BCM_IF_ERROR_RETURN_MSG(port_to_system_port(unit, port, &sysport_gport), "");
    dest_port = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);

    /** Fill ITMH Base.Destination, */
    /*
     * Destination size 21 bits 29:9
     * 01100(5) + System_Port_Agg(16)
     */
    itmh_base_jr2[1] = 0x18 | ((dest_port >> 15) & 0x1);
    itmh_base_jr2[2] = (dest_port >> 7) & 0xff;
    itmh_base_jr2[3] = (dest_port & 0x7f) << 1;
    bcm_pkt_memcpy(pkt, dest_byte, itmh_base_jr2, itmh_base_jr2_size);
    dest_byte += itmh_base_jr2_size;

    /** Copy original packet data */
    if (data)
    {
        bcm_pkt_memcpy(pkt, dest_byte, data, len);
    }
    else
    {
        bcm_pkt_memcpy(pkt, dest_byte, rxtx_pkt_data, len);
    }

    pkt->blk_count = 1;

    pkt->_pkt_data.len = size;

    /** Flag BCM_TX_CRC_APPEND indicates CRC bytes are appended by switch */
    pkt->flags |= BCM_TX_CRC_APPEND;

    /** Flag BCM_PKT_F_HGHDR indicates Module Header exists */
    pkt->flags |= BCM_PKT_F_HGHDR;

    /** Get CPU channel of CPU port 0 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, 0, &flags, &interface_info, &mapping_info), "");

    /** Set CPU channel value */
    pkt->_dpp_hdr[0] = mapping_info.channel;

    /** Sent packet out */
    BCM_IF_ERROR_RETURN_MSG(bcm_tx(unit, pkt, NULL), "");

    /** Free routine work with bcm_pkt_alloc */
    BCM_IF_ERROR_RETURN_MSG(bcm_pkt_free(unit, pkt), "");

    return BCM_E_NONE;
}


/*
 * Example of RX reference
 *  - mapped all CPU ports to specified DMA channel
 */
int
pkt_start_multiple_ports_to_same_channel(
    int unit,
    int channel_index)
{
    bcm_rx_cfg_t cfg;
    bcm_port_config_t port_config;
    int nof_cpu_ports;

    if (channel_index < rx_dma_channel_index_start || channel_index > rx_dma_channel_index_end)
    {
        printf("Error, invalid channel_index\n");
        return BCM_E_PARAM;
    }

    /*
     * Stop RX if RX is already active
     */
    if (bcm_rx_active(unit))
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_stop(unit, NULL), "");
    }

    /*
     * Get port configuration
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");
    /*
     * Get number of CPU ports
     */
    BCM_PBMP_COUNT(port_config.cpu, nof_cpu_ports);


    /*
     * Initialize Dune RX Configuration Structure
     */
    bcm_rx_cfg_t_init(&cfg);
    cfg.pkt_size = (16 * 1024);
    cfg.global_pps = 0;
    cfg.max_burst = 0;
    cfg.flags = 0;
    cfg.pkts_per_chain = 32;

    /*
     * Map all CPU ports to DMA channel 1
     */
    cfg.chan_cfg[channel_index].cos_bmp = 0xffffffff;
    cfg.chan_cfg[channel_index].chains = 8;
    /*
     * When the number of CPU ports is greater than available DMA channel number, there should be multiple CPU ports are mapped to same channel
     * Flag BCM_RX_F_TAKE_PORT_FROM_PARSER should be set to get received port from FTMH.PP_DSP
     * When the number of CPU ports is not more than available DMA channel number, it will be a 1-1 map between CPU ports and DMA channels
     * Flag BCM_RX_F_TAKE_PORT_FROM_PARSER is not requried
     */
    if (nof_cpu_ports > rx_dma_channel_available)
    {
        cfg.chan_cfg[channel_index].flags = BCM_RX_F_TAKE_PORT_FROM_PARSER;
    }

    /*
     * When Input parameter queue_id = -1
     * 1. All DMA cos is set to the specified DMA channel
     * 2. All CPU ports will be mapped the specified DMA channel
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_queue_channel_set(unit, -1, channel_index), "");

    /*
     * Start DMA channel channel_index
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_start(unit, &cfg), "");

    return BCM_E_NONE;
}

/*
 * Example of mapping CPU ports to DMA channel
 *  - travser all CPU ports, set each CPU port to one of the available DMA channel as below:
 *    1st + rx_dma_channel_available*N CPU port -> Channel rx_dma_channel_index_start
 *    2nd + rx_dma_channel_available*N CPU port -> Channel 2
 *    3rd + rx_dma_channel_available*N CPU port -> Channel 3
 *    ...
 *    nth + rx_dma_channel_available*N CPU port -> Channel rx_dma_channel_index_end
 *  - please note this example is only applied to JR2 and above device
 */
int
pkt_map_cpu_ports_to_dma_channels(
    int unit)
{
    bcm_rx_cfg_t cfg;
    uint32 cosq;
    bcm_port_config_t port_config;
    int nof_cpu_ports;
    bcm_gport_t logical_port;
    int channel_index;

    /*
     * Stop RX if RX is already active
     */
    if (bcm_rx_active(unit))
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_stop(unit, NULL), "");
    }

    /*
     * Get port configuration
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");
    /*
     * Get number of CPU ports
     */
    BCM_PBMP_COUNT(port_config.cpu, nof_cpu_ports);

    /*
     * Initialize Dune RX Configuration Structure
     */
    bcm_rx_cfg_t_init(&cfg);
    cfg.pkt_size = (16 * 1024);
    cfg.global_pps = 0;
    cfg.max_burst = 0;
    cfg.flags = 0;
    cfg.pkts_per_chain = 32;

    /*
     * Start all available RX channels
     */
    for (channel_index = rx_dma_channel_index_start; channel_index <= rx_dma_channel_index_end; channel_index++)
    {
        cfg.chan_cfg[channel_index].chains = 2;
        cfg.chan_cfg[channel_index].rate_pps = 0;

        /*
         * When the number of CPU ports is greater than available DMA channel number, there should be multiple CPU ports are mapped to same channel
         * Flag BCM_RX_F_TAKE_PORT_FROM_PARSER should be set to get received port from FTMH.PP_DSP
         * When the number of CPU ports is not more than available DMA channel number, it will be a 1-1 map between CPU ports and DMA channels
         * Flag BCM_RX_F_TAKE_PORT_FROM_PARSER is not requried
         */
        if (nof_cpu_ports > rx_dma_channel_available)
        {
            cfg.chan_cfg[channel_index].flags = BCM_RX_F_TAKE_PORT_FROM_PARSER;
        }
    }

    /*
     * Start all available DMA channels
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_start(unit, &cfg), "");

    /*
     *  - travser all CPU ports, set each CPU port to one of the available DMA channel as below:
     *    1st + rx_dma_channel_available*N CPU port -> Channel rx_dma_channel_index_start
     *    2nd + rx_dma_channel_available*N CPU port -> Channel 2
     *    3rd + rx_dma_channel_available*N CPU port -> Channel 3
     *    ...
     *    nth + rx_dma_channel_available*N CPU port -> Channel rx_dma_channel_index_end
     */
    channel_index = rx_dma_channel_index_start;
    BCM_PBMP_ITER(port_config.cpu, logical_port)
    {
        int channel;
        int nof_priorities;
        uint32 dummy_flags;
        bcm_port_interface_info_t interface_info;
        bcm_port_mapping_info_t mapping_info;

        if (channel_index > rx_dma_channel_index_end)
        {
            channel_index = rx_dma_channel_index_start;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, logical_port, &dummy_flags, &interface_info, &mapping_info), "");
        nof_priorities = mapping_info.num_priorities;
        channel = mapping_info.channel;

        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            /*
             * The DMA channel support CoS(classes-of-service), it's 64-bit bitmap,  we can specify CoS allowed for a particular channel via API bcm_rx_queue_channel_set
             * There is a 1-1 map between EGQ Qpair and DMA CoS
             * DMA Cos(= channel + cosq) is 1-1 mapped to (base_q_pair + cosq), please refer to DABL table EGQ_QPAIR_ATTRIBUTES
             * The traffic to a specified logical port will be recevied by a certain DMA channel like:
             * logical port -> EGQ Qpair -> DMA CoS -> DMA channel
             */
            BCM_IF_ERROR_RETURN_MSG(bcm_rx_queue_channel_set(unit, (channel + cosq), channel_index), "");
        }
        channel_index++;
    }

    return BCM_E_NONE;
}

/*
 * Example of executing RX callback function per VLAN.PCP
 */
bcm_rx_t pkt_rx_callback_vlan_pcp_1(int unit, bcm_pkt_t* pkt, void* cookie)
{
    receive_packets_count ++;
    printf("RX callback pkt_rx_callback_vlan_pcp_1 is triggered\n");
    return BCM_RX_HANDLED;
}

int
pkt_rx_callback_register(
    int unit)
{
    /* VLAN.PCP = 1 */
    int vlan_pcp = 1;
    /* Highest priority */
    int priority = 255;
    uint32 flags = 0;

    /* Clear counter */
    receive_packets_count = 0;
    /* Register the RX callback function, it will be called when the recived VLAN.PCP is specified value */
    bcm_rx_queue_register(unit, "RX CALLBACK1",  vlan_pcp, pkt_rx_callback_vlan_pcp_1, priority, NULL,  flags);
}

int
pkt_rx_callback_unregister(
    int unit)
{
    /* VLAN.PCP = 1 */
    int vlan_pcp = 1;
    /* Highest priority */
    int priority = 255;

    bcm_rx_queue_unregister(unit , vlan_pcp, pkt_rx_callback_vlan_pcp_1, priority);

    /* Clear counter */
    receive_packets_count = 0;
}
