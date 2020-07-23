/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/* cint */


/* extern int _bcm_common_rx_cos_max_len_set(int unit, int cos, int max_q_len); */


int cos_limit[8] = { 2, 4, 6, 8, 10, 12, 14, 16}; /* Max packets to schedule due to rate limiting.  */ 
int cos_called[8] = {0};

/*
 * Initialize our RX Infrastructure. 
 */
int 
arad_rx_activate_cos(uint32 unit)
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
     * COS Bitmap
     *
     * This value specifies which packet COS will be assigned to this 
     * RX Channel. 
     * COS mapping to RX channels must be unique. 
     *
     * In this example, we want all packets to be assigned to this RX channel. 
     * Thus, our COS bitmap will be 0xFF, for all 8 COS values. 
     */

    cfg.chan_cfg[1].flags = 0;
    cfg.chan_cfg[1].cos_bmp = 0xf0f000f;
    cfg.chan_cfg[1].chains = 2;
    cfg.chan_cfg[2].flags = 0;
    cfg.chan_cfg[2].cos_bmp = 0xf000f0f0;
    cfg.chan_cfg[2].chains = 2;
    cfg.chan_cfg[3].flags = 0;
    cfg.chan_cfg[3].cos_bmp = 0xf00f00;
    cfg.chan_cfg[3].chains = 2;

    if ((rv = (bcm_rx_start(unit, &cfg))) < 0) {
        printf("bcm_rx_start fail: %d\n", rv);
        return rv;
    }                                                

    printf("Register call backs for different COS\n");
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD0",  0 , appl_dcmn_rx_cos_callback_0, 100, NULL,  0x1)) < 0) {
        printf("Error in bcm_rx_queue_register 0\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD1",  1 , appl_dcmn_rx_cos_callback_1, 101, NULL,  0x2)) < 0) {
        printf("Error in bcm_rx_queue_register 1\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD2",  2 , appl_dcmn_rx_cos_callback_2, 102, NULL,  0x4)) < 0) {
        printf("Error in bcm_rx_queue_register 2\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD3",  3 , appl_dcmn_rx_cos_callback_3, 103, NULL,  0x8)) < 0) {
        printf("Error in bcm_rx_queue_register 3\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD4",  4 , appl_dcmn_rx_cos_callback_4, 104, NULL,  0x10)) < 0) {
        printf("Error in bcm_rx_queue_register 4\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD5", 5, appl_dcmn_rx_cos_callback_5, 105, NULL, 0x20)) < 0) {
        printf("Error in bcm_rx_queue_register 5\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD6", 6, appl_dcmn_rx_cos_callback_6, 106, NULL, 0x40)) < 0) {
        printf("Error in bcm_rx_queue_register 6\n");
        return rv;
    }
    if ((rv =  bcm_rx_queue_register(unit, "RX CMD_OTHER", 7, appl_dcmn_rx_cos_callback_other, 107, NULL, 0x80)) < 0) {
        printf("Error in bcm_rx_queue_register 6\n");
        return rv;
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
 
    return 0; 
}

/*
 * Stop the RX API
 *
 * This function will stop packet reception. 
 */
 int 
_rx_stop_cos(int unit)
{
    int rv;

    /* Unregister our RX handlers */
    rv = bcm_rx_queue_unregister( unit, 0, appl_dcmn_rx_cos_callback_0,100);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 0\n");
    }
    rv = bcm_rx_queue_unregister( unit, 1, appl_dcmn_rx_cos_callback_1,101);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 1\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 2, appl_dcmn_rx_cos_callback_2,102);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 2\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 3, appl_dcmn_rx_cos_callback_3,103);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 3\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 4, appl_dcmn_rx_cos_callback_4,104);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 4\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 5, appl_dcmn_rx_cos_callback_5,105);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 5\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 6, appl_dcmn_rx_cos_callback_6,106);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 6\n");       
    } 
    rv = bcm_rx_queue_unregister( unit, 7, appl_dcmn_rx_cos_callback_other,107);
    if (rv < 0)  {
        printf("bcm_rx_unregister rx_intr_callback fail 7\n");       
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

int
arad_rx_scheduler(int unit, int *sched_unit, 
                  bcm_cos_queue_t *sched_cosq, int *sched_count) 
{
    int rv;    
    int pkt_in_queue; /* Number of packets waiting.   */      
    static bcm_cos_queue_t cosq_idx; /* Queue iterator.  */
    static int unit_next = unit;/* Bcm device iterator.         */
    
    for (cosq_idx = 7; cosq_idx >= 0; cosq_idx--) {
        for(;unit_next != -1;) {
            /* Get number of packets awaiting processing. */
            rv = bcm_rx_queue_packet_count_get(unit_next, cosq_idx, &pkt_in_queue);
            if (rv < 0)  {
                printf("bcm_rx_queue_packet_count_get fail\n");       
            }
            /* Queue rate limiting enforcement. */
            if (pkt_in_queue) {
                *sched_unit = unit_next;
                *sched_cosq = cosq_idx;
                cos_called[cosq_idx]++;

                if (cos_limit[cosq_idx] < pkt_in_queue) {
                    *sched_count = cos_limit[cosq_idx];
                }  else {
                    *sched_count = pkt_in_queue;
                }
                return (BCM_E_NONE);
            }
            rv = bcm_rx_unit_next_get(unit_next, &unit_next);
            if (rv < 0)  {
                printf("bcm_rx_unit_next_get fail\n");       
            }
        }
        unit_next = unit;
    }

    /* Prep for the next scheduling cycle. */
    cosq_idx = -1;   
    unit_next = -1;   
    /* Done indicator. */
    *sched_count = 0;

    return (BCM_E_NONE);
}

int
arad_rx_scheduler_start(int unit) 
{
    int rv;
    int cosq_idx;
    
    for (cosq_idx = 0; cosq_idx < 8; cosq_idx++) {
        cos_called[cosq_idx] = 0;
    }
    rv = bcm_rx_sched_register(unit, arad_rx_scheduler);
    if (rv < 0)  {
        printf("bcm_rx_sched_register fail\n");       
    }

    return rv;
}

int
arad_rx_scheduler_stop(int unit) 
{
    int rv;
    int cosq_idx;
    
    rv = bcm_rx_sched_unregister(unit);
    if (rv < 0)  {
        printf("bcm_rx_sched_unregister fail\n");       
    }

    printf("arad_rx_scheduler result:\n");
    for (cosq_idx = 0; cosq_idx < 8; cosq_idx++) {
        printf("cos-%d: %d\n", cosq_idx, cos_called[cosq_idx]);
    }

    return rv;
}

/*
print "RX activate with COS...\n";
print arad_rx_activate_cos(0);
print arad_rx_scheduler_start(0);
*/

