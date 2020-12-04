/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

/*
 * Initialize our RX Infrastructure. 
 */
int 
arad_rx_activate_no_cos(uint32 unit)
{
    int rv;
    /* RX Configuration Structure */
    bcm_rx_cfg_t cfg; 

    /* Initialize the BCM RX API */
    if (bcm_rx_active(unit)) {
        print("Stopping active RX.\n");
        rv = bcm_rx_stop(unit, NULL);
        if (rv != BCM_E_NONE) {
            printf("Unable to stop RX: %d\n", rv);
            return rv;
        }
    }

    /*
     * Setup our RX Configuration. 
     * The following values are global settings for all of the RX API. 
     */
    bcm_rx_cfg_t_init(&cfg); 

    /* 
     * Maximum packet buffer size. Packets beyond this size will require 
     * scatter gather, and be delivered in multiple data buffers which 
     * the application must the reconstruct. 
     */
    cfg.pkt_size = 16*1024; 			

    /*
     * Number of packet buffers per DMA descriptor chain. This determines 
     * how many packets the hardware can DMA into before the buffers have 
     * to be refilled by software. 
     */
    cfg.pkts_per_chain = 15 ;

    /*
     * This sets the global rate of packets sent to the CPU by all RX channels. 
     * A value of 0 disables global rate limiting. 
     */
    cfg.global_pps = 0;

    /*
     * Maximum burst size to the CPU, in packets. Must be less than or equal 
     * to cfg.pkts_per_chain. 
     */
    cfg.max_burst = 15;  

    /*
     * Packets are received from the hardware in interrupt context. 
     * The BCM RX API uses a separate thread to handle operations which 
     * originate in interrupt context, but cannot be executed there. 
     * These operations are scheduled with the RX thread as a deferred 
     * procedure call from interrupt context. 
     *
     * In addition to its own internal administration, the RX Thread is 
     * used by the RX API to deliver packets to the application in 
     * non-interrupt context for processing, if the application so 
     * designates in its handler registration. (See RX Registration below). 
     *
     * Packet buffers allocated for reception by the RX API are done through 
     * user registered allocation routines.  The RX API provides default 
     * allocation routines which use the BCM Configuration Manager's 
     * DMA shared memory vectors. Setting these function vectors to NULL
     * indicate usage of the default allocation routines.
     */
    cfg.rx_alloc = NULL;
    cfg.rx_free = NULL;

    /* 
     * These flags modify the behavior of packet reception. There are 
     * currently two flags:
     *
     *  BCM_RX_F_IGNORE_HGHDR
     *      The RX API normally strips the HiGig header from 
     *          packets received on XGS fabric chips, and sets the 
     *          corresponding abstractions in the packet structure 
     *          based on the higig information. If you specify this flags, 
     *          the higig header will not be stripped from the packet, 
     *          and instead will precede the packet data proper (DA.SA...)
     *  
     *  BCM_RX_F_IGNORE_SL_TAG
     *      The RX API normally strips the SL Stack tag on Strata, 
     *          Strata II, and XGS modules configured for SL stacking. 
     *          This flag causes the SL Stack tag to remain in the packet. 
     *
     *
     * This example does not use either of these flags. 
     */
    cfg.flags = 0;              
    
    /*
     * RX Channel Configuration. 
     * 
     * The Strata and StrataII family of chips support one RX Channel per chip. 
     * The XGS Family of chips supports 4 RX Channels per chip, which may 
     * be prioritized by COS. 
     *
     * You may only specify one channel in this structure if operating 
     * on Strata/Strata II devices. 
     * 
     * This example assumes an XGS device, and will configure 2 RX chains, 
     * one for COS 0-3, one for COS 4-7. 
     *
     * 
     * Configure RX Channel 1   (Assumes channel 0 has been reserved for TX)
     */

    /*
     * This specifies the number of DV packet chains which will 
     * be preallocated for use with this channel. 
     *
     * Each chain is basically a queue of packets of length cfg.pkts_per_chain 
     * which is setup with the DMA engine. 
     * When one chain is full, the driver will queue up 
     * another chain (subject to rate limiting). 
     */
    cfg.chan_cfg[1].chains = 4; /* 4; */  /*1;      */
    
    /*
     * Flags:
     *  
     * The following per-channel flags are supported:
     *  
     *  BCM_RX_F_CRC_STRIP
     *      If specified, packets from this channel will have their 
     *          CRC stripped before they are received. 
     *  BCM_RX_F_VTAG_STRIP
     *      If specified, packets from this channel will have 
     *          their VLAN tag stripped before they are received. 
     *  BCM_RX_F_RATE_STALL
     *      Currently unimplemented. 
     *
     *
     * This example does no use any of these per-channel flags.        
     */     
    cfg.chan_cfg[1].flags = 0;
    
    /* 
     * COS Bitmap
     *
     * This value specifies which packet COS will be assigned to this 
     * RX Channel. 
     * COS mapping to RX channels must be unique. 
     *
     * In this example, we want all packets to be assigned to this RX channel. 
     * Thus, our COS bitmap will be 0xFF, for all 8 COS values. 
     */
    cfg.chan_cfg[1].cos_bmp = 0xFffffffF;

    /*
     * Now that our RX configuration is complete, lets register our 
     * packet handlers before starting up packet reception. 
     *
     * The RX API supports two types of packet handlers -- one which 
     * receives packets at interrupt context, and one which receives 
     * packets in the RX Thread (non-interrupt) context. 
     *
     * This example will register a handler of both types. 
     *
     * The interrupt-level packet handler in this example does nothing 
     * more than increment a packet counter. 
     * This packet handler will return the packet back to the RX API 
     * as unhandled, so it can be processed in non-interrupt context. 
     *
     * The non-interrupt level packet handler in this example will 
     * just dump the packet info and contents. 
     *
     * See the implementation notes for these functions below. 
     */
    /* 
     * Register the interrupt-level callback. 
     */
                                  
    if ((rv = bcm_rx_queue_register(unit, "EXAMPLE RX INTR CALLBACK", BCM_RX_COS_ALL, appl_dcmn_rx_intr_callback, 1, NULL, BCM_RCO_F_ALL_COS | BCM_RCO_F_INTR)) < 0) {                                         
        printf("bcm_rx_register fail: %s \n", rv);
        return rv;
    }                                                               

    /* 
     * Our RX Configuration is complete. 
     * Our handlers are registered. 
     * Lets start up packet reception. 
     */
         
    if ((rv = bcm_rx_queue_register(unit, "EXAMPLE RX NONINTR CALLBACK", BCM_RX_COS_ALL, appl_dcmn_rx_nonintr_callback, 100, NULL,  BCM_RCO_F_ALL_COS)) < 0) {
        printf("bcm_rx_register appl_dcmn_rx_nonintr_callback fail: %s \n", rv);                                     
        return rv;
    }                                                                  

    bcm_rx_cos_max_len_set(unit, BCM_RX_COS_ALL, 30);
            
    if ((rv = (bcm_rx_start(unit, &cfg))) < 0) {                                        
        printf("bcm_rx_start fail: %s \n", rv);     
        return rv;
    }                                                                          

    bcm_rx_cos_max_len_set(unit, BCM_RX_COS_ALL, 30);
    /*
     * Things are running at this point. Packets should be flowing, and 
     * handlers should be handling. 
     */
    return 0; 
}

/*
 * Stop the RX API
 *
 * This function will stop packet reception. 
 */
int _rx_nocos_stop(int unit)
{
    int rv;
  
    /* Unregister our RX handlers */
    rv = bcm_rx_queue_unregister( unit, BCM_RX_COS_ALL, appl_dcmn_rx_intr_callback,1);
    if (rv < 0)  {
        printf("bcm_rx_unregister appl_dcmn_rx_intr_callback fail\n");       
    } 

    if ((rv = bcm_rx_queue_unregister( unit, BCM_RX_COS_ALL, appl_dcmn_rx_nonintr_callback,100)) < 0) /* callback priority used on registration */ {
        printf("bcm_rx_unregister appl_dcmn_rx_nonintr_callback fail\n");    
    }
    /*
    * Stop the BCM RX API 
    */
    if (bcm_rx_active(unit)) {         
        if ((rv = bcm_rx_stop( unit, NULL)) < 0) /* callback priority used on registration */ {
            printf("bcm_rx_stop  fail\n");     
        }
       
    } else {
        printf(("Rx is not active... \n"));
    }
    /* Done */
    return 0; 
}

/*
print "RX activate no COS...\n";
print arad_rx_activate_no_cos(0);
print _rx_nocos_stop(0);
*/