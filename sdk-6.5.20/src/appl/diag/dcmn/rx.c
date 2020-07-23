/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx.c
 * Purpose:     rx activation code.
 */

/* 
 * Includes
 */
#include <shared/bsl.h>
#include <bcm_int/dispatch.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#endif /* BCM_PETRA_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
/* 
 * Globals
 */

/* Keeps track of the number of packets received */
int _packet_counter[BCM_MAX_NUM_UNITS]; 


/* 
 * RX CallBacks
 */
bcm_rx_t appl_dcmn_rx_cos_callback_0(int unit, bcm_pkt_t* pkt, void* cookie)
{
    /* Increment and return */
    _packet_counter[unit]++;
     LOG_VERBOSE(BSL_LS_BCM_RX,
               (BSL_META_U(unit,
                           "+++++RX 0 +++++++++++++++\n++++++++++++++++\n")));
    return BCM_RX_HANDLED;

}
bcm_rx_t appl_dcmn_rx_cos_callback_1(int unit, bcm_pkt_t* pkt, void* cookie)
{
    /* Increment and return */
    _packet_counter[unit]++; 
     LOG_VERBOSE(BSL_LS_BCM_RX,
               (BSL_META_U(unit,
                           "+++++RX 1 +++++++++++++++\n++++++++++++++++\n")));
    return BCM_RX_HANDLED; 

}
bcm_rx_t appl_dcmn_rx_cos_callback_2(int unit, bcm_pkt_t* pkt, void* cookie)
{
    LOG_VERBOSE(BSL_LS_BCM_RX,
              (BSL_META_U(unit,
                          "+++RX 2 +++++++++++++++++++++++\n")));
    return BCM_RX_HANDLED; 

}

bcm_rx_t appl_dcmn_rx_cos_callback_3(int unit, bcm_pkt_t* pkt, void* cookie)
{
     LOG_VERBOSE(BSL_LS_BCM_RX,
               (BSL_META_U(unit,
                           "++++++++++++RX 3 ++++++++++++++++++++++++\n")));
    return BCM_RX_HANDLED; 

}

bcm_rx_t appl_dcmn_rx_cos_callback_4(int unit, bcm_pkt_t* pkt, void* cookie)
{
     LOG_VERBOSE(BSL_LS_BCM_RX,
               (BSL_META_U(unit,
                           "+++++RX 4 +++++++++++++++\n++++++++++++++++\n")));
    return BCM_RX_HANDLED; 
}

bcm_rx_t appl_dcmn_rx_cos_callback_5(int unit, bcm_pkt_t* pkt, void* cookie)
{
    LOG_VERBOSE(BSL_LS_BCM_RX,
              (BSL_META_U(unit,
                          "+++RX 5 +++++++++++++++++++++++\n")));
    return BCM_RX_HANDLED; 
}

bcm_rx_t appl_dcmn_rx_cos_callback_6(int unit, bcm_pkt_t* pkt, void* cookie)
{
    LOG_VERBOSE(BSL_LS_BCM_RX,
              (BSL_META_U(unit,
                          "++++++++++++RX 6 ++++++++++++++++++++++++\n")));
    return BCM_RX_HANDLED; 
}

bcm_rx_t appl_dcmn_rx_cos_callback_other(int unit, bcm_pkt_t* pkt, void* cookie)
{
    LOG_VERBOSE(BSL_LS_BCM_RX,
              (BSL_META_U(unit,
                          "++++++++++++RX Other ++++++++++++++++++++++++\n")));
    return BCM_RX_HANDLED; 
}


/* 
 * Interrupt level packet handler
 *
 * This function receives packets at interrupt context. 
 * As an example, it increments a counter for each packet, then 
 * returns that packet to the RX API for further processing.
 */
bcm_rx_t appl_dcmn_rx_intr_callback(int unit, bcm_pkt_t* pkt, void* cookie)
{
    /* 
     * We won't examine the packet contents in this handler, although we could. 
     * Instead, we will just increment a counter and return it for 
     * further processing. 
     *
     * Each RX handler should return one of the following values to the RX API:
     *
     *  BCM_RX_NOT_HANDLED
     *      This handler did not handle this packet, and it should 
     *          continue handling by other handlers.
     *  BCM_RX_HANDLED
     *      This handler handled the packet completely, and the 
     *          packet buffer may be reused by the RX API. The packet is 
     *          totally done with, and will not go to other handlers. 
     *      The packet buffer will be rescheduled for RX. 
     * BCM_RX_HANDLED_OWNED
     *      This handler handled the packet, but it retains the 
     *          packet buffer for further processing. 
     *      The packet will not be sent to other handlers, and 
     *          the RX API will no longer track the packet pointer. The 
     *          application now "owns" the packet buffer (perhaps for 
     *          further processing), and it is the application's 
     *          responsibility to free the packet buffer when it is 
     *          done with it. 
     *      NOTE: this applies ONLY to the packet DATA pointers in the 
     *          bcm_pkt_t structure, NOT the bcm_pkt_t itself structure. 
     *      You may not own the structure pointer which is passed to you, 
     *          and that structure will be immediately reused for other packets. 
 */

    /* Increment and return */
    _packet_counter[unit]++; 
    if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityDebug, unit)) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "***RX intr callback **\n")));
    }
    return BCM_RX_NOT_HANDLED; 
}

/*
 * Non-interrupt level packet handler. 
 *
 * This function will be executed within the context of the RX Thread 
 * instead of interrupt context. 
 */
bcm_rx_t appl_dcmn_rx_nonintr_callback(int unit, bcm_pkt_t* pkt, void* cookie)
{    
    SOC_PPC_FRWRD_MACT_LEARN_MSG
        learn_msg;
    SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO
        learn_events;
    uint32
        /* data_u32=0,*/
        sand_rv;


    /*
     * Dump the packet contents, then return that we have handled and 
     * we are done with the packet. 
     *
     * The bcm_pkt_t structure is defined in orion/include/bcm/pkt.h
     * The RX API supports only one packet buffer per packet. 
     * There is no scatter/gather RX support. 
     */
    
    /* 
     * Get the current packet count, as incremented by the interrupt 
     * level handler. This is silly, but it illustrates the usage 
     * and demonstrates both handlers working together. 
     *
     * Need to be interrupt safe while reading this value. Disable 
     * interrupts using the SAL
     */
      if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit)) {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "***RX non-intr callback **\n")));
      }
          
      if(pkt->src_port == (int16)ARAD_OLP_PORT_ID) {
        
            learn_msg.max_nof_events = 8;
            learn_msg.msg_len = pkt->tot_len;
            learn_msg.msg_buffer = sal_alloc(sizeof(uint8)* pkt->tot_len, "learn_msg.msg_buffer");
            if(learn_msg.msg_buffer) {
                sal_memcpy(learn_msg.msg_buffer,pkt->_pkt_data.data, pkt->tot_len);
        } else {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Can not allocate memory for learn_msg.msg_buffer\n")));
                return -1;       
            }
        
            learn_events.events = sal_alloc(sizeof(SOC_PPC_FRWRD_MACT_EVENT_INFO)*learn_msg.max_nof_events,"learn_events.events");
            if(!learn_events.events) {         
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "Can not allocate memory for learn_events.events\n")));
                return -1;
            }

            unit = (unit);
            sand_rv = soc_ppd_frwrd_mact_learn_msg_parse(unit,&learn_msg,&learn_events);
            if(SOC_SAND_FAILURE(sand_rv)) {
                LOG_ERROR(BSL_LS_BCM_RX,
                          (BSL_META_U(unit,
                                      "soc_ppd_frwrd_mact_learn_msg_parse failed\n")));
                return -1;
            }
        
            SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(&learn_events);
        
            if(learn_events.events) {
                sal_free(learn_events.events);
            }
            if(learn_msg.msg_buffer) {
                sal_free(learn_msg.msg_buffer);
            }
            
            /* We are done with this packet. Return it and the buffer to the RX API */
            return BCM_RX_HANDLED; 
     }

     return BCM_RX_NOT_HANDLED; 
}

/*
 * Initialize our RX Infrastructure. 
 */
int appl_dcmn_rx_activate(int unit, int packet_rx_cosq)
{
    /* RX Configuration Structure */
    bcm_rx_cfg_t cfg; 

    /* Initialize the BCM RX API */
    if (bcm_rx_active(unit)) {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "arad_rx_activate - rx is already activated\n")));
        return -1;
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
     /* 4; */  /*1;      */
    
    
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
    

   /*
     * Initialize the counter for the number of packets received.
     */
    _packet_counter[unit] = 0;
    
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

    if (packet_rx_cosq) {
        cfg.chan_cfg[1].flags = 0; 
        cfg.chan_cfg[1].cos_bmp = 0xf0f000f;
        cfg.chan_cfg[1].chains = 2;
        cfg.chan_cfg[2].flags = 0;
        cfg.chan_cfg[2].cos_bmp = 0xf000f0f0;
        cfg.chan_cfg[2].chains = 2;
        cfg.chan_cfg[3].flags = 0;
        cfg.chan_cfg[3].cos_bmp = 0xf00f00;
        cfg.chan_cfg[3].chains = 2;

        {
            int rv;
            if ((rv =  bcm_rx_queue_register(unit, "RX CMD1",  1 , appl_dcmn_rx_cos_callback_1, 100, NULL,  0x1)) < 0) {
                return rv;
            }
            if ((rv =  bcm_rx_queue_register(unit, "RX CMD2",  2 , appl_dcmn_rx_cos_callback_2, 101, NULL,  0x2)) < 0) {
                return rv;
            }
            if ((rv =  bcm_rx_queue_register(unit, "RX CMD3",  3 , appl_dcmn_rx_cos_callback_3, 102, NULL,  0x4)) < 0) {
                return rv;
            }
            if ((rv =  bcm_rx_queue_register(unit, "RX CMD4",  4 , appl_dcmn_rx_cos_callback_4, 103, NULL,  0x8)) < 0) {
                return rv;
            }

            if ((rv =  bcm_rx_queue_register(unit, "RX CMD5", 5, appl_dcmn_rx_cos_callback_5, 104, NULL, 0x10)) < 0) {
                return rv;
            }
            if ((rv =  bcm_rx_queue_register(unit, "RX CMD6", 6, appl_dcmn_rx_cos_callback_6, 105, NULL, 0x20)) < 0) {
                return rv;
            }

        }
        {
             int __rv__;                                                            
             if ((__rv__ = (bcm_rx_start(unit, &cfg))) < 0) {                                             
                 LOG_ERROR(BSL_LS_BCM_RX,
                           (BSL_META_U(unit,
                                       "bcm_rx_start fail: %s \n"), bcm_errmsg(__rv__)));      
                 return -1;
             }                                                                      
          } 
        if (soc_feature(unit, soc_feature_cmicm) && 
            soc_feature(unit, soc_feature_cos_rx_dma)) {
            /* FIXME: adding if(0 != NUM_CPU_ARM_COSQ(unit, 1)) is a temporary fix till the issue will be fixed in src/bcm/common/rx.c . 
               the issue is when cosq allociton is as folloiwng, 
               the arm1 and arm2 bits cosq bits from 32 to 64 in CMIC_CMCx_CHy_COS_CTRL_RX_z registers are set:
               cpu  - 64 cosq
               arm1 - 0  cosq
               arm2 - 0  cosq
             */
            if(0 != NUM_CPU_ARM_COSQ(unit, 1)) {
                /* enable all cosq on the first Rx channel of ARM Core 0 */
                bcm_rx_queue_channel_set(unit, -1, BCM_RX_CHANNELS + 1);
            }
            /* FIXME: adding if(0 != NUM_CPU_ARM_COSQ(unit, 2)) is a temporary fix till the issue will be fixed in src/bcm/common/rx.c . 
               the issue is when cosq allociton is as folloiwng, 
               the arm1 and arm2 bits cosq bits from 32 to 64 in CMIC_CMCx_CHy_COS_CTRL_RX_z registers are set:
               cpu  - 64 cosq
               arm1 - 0  cosq
               arm2 - 0  cosq
             */
             if(0 != NUM_CPU_ARM_COSQ(unit, 2)) {
                /* enable all cosq on the first Rx channel of ARM Core 1 */
                bcm_rx_queue_channel_set(unit, -1, BCM_RX_CHANNELS * 2 + 1);
            }
        }             

        bcm_rx_queue_channel_set(unit, 0, 1);
        bcm_rx_queue_channel_set(unit, 1, 1);
        bcm_rx_queue_channel_set(unit, 2, 1);
        bcm_rx_queue_channel_set(unit, 3, 1);
        bcm_rx_queue_channel_set(unit, 4, 2);
        bcm_rx_queue_channel_set(unit, 5, 2);
        bcm_rx_queue_channel_set(unit, 6, 2);
        bcm_rx_queue_channel_set(unit, 7, 2);
        bcm_rx_queue_channel_set(unit, 8, 3);
        bcm_rx_queue_channel_set(unit, 9, 3);
        bcm_rx_queue_channel_set(unit, 10, 3);
        bcm_rx_queue_channel_set(unit, 11, 3);
        bcm_rx_queue_channel_set(unit, 12, 2);
        bcm_rx_queue_channel_set(unit, 13, 2);
        bcm_rx_queue_channel_set(unit, 14, 2);
        bcm_rx_queue_channel_set(unit, 15, 2);
        bcm_rx_queue_channel_set(unit, 16, 1);
        bcm_rx_queue_channel_set(unit, 17, 1);
        bcm_rx_queue_channel_set(unit, 18, 1);
        bcm_rx_queue_channel_set(unit, 19, 1);
        bcm_rx_queue_channel_set(unit, 20, 3);
        bcm_rx_queue_channel_set(unit, 21, 3);
        bcm_rx_queue_channel_set(unit, 22, 3);
        bcm_rx_queue_channel_set(unit, 23, 3);
        bcm_rx_queue_channel_set(unit, 24, 1);
        bcm_rx_queue_channel_set(unit, 25, 1);
        bcm_rx_queue_channel_set(unit, 26, 1);
        bcm_rx_queue_channel_set(unit, 27, 1);
        bcm_rx_queue_channel_set(unit, 28, 2);
        bcm_rx_queue_channel_set(unit, 29, 2);
        bcm_rx_queue_channel_set(unit, 30, 2);
        bcm_rx_queue_channel_set(unit, 31, 2);
        bcm_rx_queue_channel_set(unit, 32, 3);
        bcm_rx_queue_channel_set(unit, 33, 3);
        bcm_rx_queue_channel_set(unit, 34, 3);
        bcm_rx_queue_channel_set(unit, 35, 3);
        bcm_rx_queue_channel_set(unit, 36, 2);
        bcm_rx_queue_channel_set(unit, 37, 2);
        bcm_rx_queue_channel_set(unit, 38, 2);
        bcm_rx_queue_channel_set(unit, 39, 2);
        bcm_rx_queue_channel_set(unit, 40, 1);
        bcm_rx_queue_channel_set(unit, 41, 1);
        bcm_rx_queue_channel_set(unit, 42, 1);
        bcm_rx_queue_channel_set(unit, 43, 1);
        bcm_rx_queue_channel_set(unit, 44, 2);
        bcm_rx_queue_channel_set(unit, 45, 2);
    } else {
        /* for no cos*/
        cfg.chan_cfg[1].flags = 0;
        cfg.chan_cfg[1].cos_bmp = 0xffffffff;
        cfg.chan_cfg[1].chains = 4;



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


       {
          int __rv__;                                                            
          if ((__rv__ = (bcm_rx_queue_register(unit, "EXAMPLE RX INTR CALLBACK",  BCM_RX_COS_ALL , appl_dcmn_rx_intr_callback, 1, NULL,   BCM_RCO_F_ALL_COS | BCM_RCO_F_INTR)     )) < 0) {                                             
                    LOG_ERROR(BSL_LS_BCM_RX,
                              (BSL_META_U(unit,
                                          "bcm_rx_register fail: %s \n"), bcm_errmsg(__rv__)));   
                    return -1;
          }                                                                      
        /* 
         * Our RX Configuration is complete. 
         * Our handlers are registered. 
         * Lets start up packet reception. 
         */
         if ((__rv__ = (bcm_rx_queue_register(unit, "EXAMPLE RX NONINTR CALLBACK", BCM_RX_COS_ALL , appl_dcmn_rx_nonintr_callback, 100, NULL,  BCM_RCO_F_ALL_COS)    )) < 0) {  
                           LOG_ERROR(BSL_LS_BCM_RX,
                                     (BSL_META_U(unit,
                                                 "bcm_rx_register rx_nonintr_callback fail: %s \n"), bcm_errmsg(__rv__)));                                             
              return -1;
         }                                                                      

         bcm_rx_cos_max_len_set(unit, BCM_RX_COS_ALL, 30);
         
          if ((__rv__ = (bcm_rx_start(unit, &cfg))) < 0) {                                             
              LOG_ERROR(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "bcm_rx_start fail: %s \n"), bcm_errmsg(__rv__)));      
              return -1;
          } 
       }
         
         if (soc_feature(unit, soc_feature_cmicm) && 
            soc_feature(unit, soc_feature_cos_rx_dma)) {
            /* FIXME: adding if(0 != NUM_CPU_ARM_COSQ(unit, 1)) is a temporary fix till the issue will be fixed in src/bcm/common/rx.c . 
               the issue is when cosq allociton is as folloiwng, 
               the arm1 and arm2 bits cosq bits from 32 to 64 in CMIC_CMCx_CHy_COS_CTRL_RX_z registers are set:
               cpu  - 64 cosq
               arm1 - 0  cosq
               arm2 - 0  cosq
            */
             if(0 != NUM_CPU_ARM_COSQ(unit, 1)) {
                 /* enable all cosq on the first Rx channel of ARM Core 0 */
                 bcm_rx_queue_channel_set(unit, -1, BCM_RX_CHANNELS + 1);
             }
             /* FIXME: adding if(0 != NUM_CPU_ARM_COSQ(unit, 2)) is a temporary fix till the issue will be fixed in src/bcm/common/rx.c . 
                the issue is when cosq allociton is as folloiwng, 
                the arm1 and arm2 bits cosq bits from 32 to 64 in CMIC_CMCx_CHy_COS_CTRL_RX_z registers are set:
                cpu  - 64 cosq
                arm1 - 0  cosq
                arm2 - 0  cosq
             */
             if(0 != NUM_CPU_ARM_COSQ(unit, 2)) {
                 /* enable all cosq on the first Rx channel of ARM Core 1 */
                 bcm_rx_queue_channel_set(unit, -1, BCM_RX_CHANNELS * 2 + 1);
            }
         }
         bcm_rx_cos_max_len_set(unit, BCM_RX_COS_ALL, 30);
        /*
         * Things are running at this point. Packets should be flowing, and 
         * handlers should be handling. 
         */
    }
    return 0; 
}





/*
 * Stop the RX API
 *
 * This function will stop packet reception. 
 */
 int 
_rx_stop(int unit)
{
    int rv;
  
    /* Unregister our RX handlers */
    rv = bcm_rx_queue_unregister( unit,  BCM_RX_COS_ALL, appl_dcmn_rx_intr_callback,1);
    if (rv < 0)  {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "bcm_rx_unregister rx_intr_callback fail: %s \n"), bcm_errmsg(rv)));       
    } 

    if ((rv = bcm_rx_queue_unregister( unit, BCM_RX_COS_ALL, appl_dcmn_rx_nonintr_callback,100)) < 0) /* callback priority used on registration */ {
        LOG_ERROR(BSL_LS_BCM_RX,
                  (BSL_META_U(unit,
                              "bcm_rx_unregister rx_nonintr_callback fail: %s \n"),  bcm_errmsg(rv)));    
    }
    /*
    * Stop the BCM RX API 
    */
    if (bcm_rx_active(unit)) {         
        if ((rv = bcm_rx_stop( unit, NULL)) < 0) /* callback priority used on registration */ {
            LOG_ERROR(BSL_LS_BCM_RX,
                      (BSL_META_U(unit,
                                  "bcm_rx_stop  fail: %s \n"),  bcm_errmsg(rv)));     
        }
    } 
    /* Done */
    return 0; 
}

#endif
