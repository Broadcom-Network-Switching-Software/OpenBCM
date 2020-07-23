/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 


/* 
   The following are typical guidelines to compute DRAM bound thresholds.
   Application/platform specific tuning might be required.
*/

/*
 Typical guidelines for computing DRAM bounds threshold values for QAX device:

-   Global Parameters
  o   DRAM-Bound-Buffer-Factor = 3  
  o   DRAM-Recovery-Failure-Buffer-Factor = 0.5
  o   Min-Free-Buffer = 1000*256                   -- in bytes: 1000 SRAM buffers of 256 bytes
  o   Min-Free-PD =  1000                          -- in PDs:   1000 PDs
  o   DRAM-Recovery-Multiplier =  4
  o   Min-Packet-Size = 64                         -- in bytes
  o   Bundle-Size = 4*1024                         -- in bytes: DRAM bundle size (4K)

-  Port Specific Parameters
  o    Q-ON-Threshold = 38 * 1024 * PortSpeed / 100G_Port_Speed  -- in bytes: transition from Slow to Normal queue state (see src/appl/diag/dcmn/credit_request_thresholds.c for details)
  o    if (Single FAP device)
         Q-ON-Threshold = Q-ON-Threshold / (PortSpeed > 10G ? 5 : 1)
 
-  DRAM bound FADT Bytes
  o   Min-Threshold  = Bundle-Size
  o   Max-Threshold  = Max(DRAM-Bound-Buffer-Factor * Q-ON-Threshold, 2 * Min-Threshold)
  o   Min-Free =  Min-Free-Buffer
  o   Max-Free = 16 * 1024 * 0.25 * 256
  o   Alpha - Approximate a line to Max threshold -- Log(Max-Threhold/Max-Free), bounded in range -7:7

-  DRAM bound FADT PD
  o   Min-Threshold  = Bundle-Size / Min-Packet-Size  
  o   Max-Threshold  = Max(DRAM-Bound-Buffer-Factor * Q-ON-Threshold / Min-Packet-Size,  2 * Min-Threshold)
  o   Min-Free =  Min-Free-PD
  o   Max Free = 32 * 1024 * 0.25
  o   Alpha - Approximate a line to Max threshold -- Log(Max-Threshold/Max-Free), bounded in range -7:7 

 
- Recovery Failure  FADT  Bytes
  o   Min-Threshold  = Bundle-Size
  o   Max-Threshold  = Max(DRAM-Bound-Buffer-Factor * DRAM-Recovery-Failure-Factor * Q-ON-Threshold,  2 * Min-Threshold)
  o   Min-Free =  Min( Min-Free-Buffer * 2 , Max-Free)
  o   Max Free = 16 * 1024 * 0.5 * 256
  o   Alpha - Approximate a line to Max threshold -- Log(Max-Threshold/Macx-Free), bounded in range -7:7 
 
-   Recovery Failure  FADT PDs 
  o   Min-Threshold  = Bundle-Size / Min-Packet-Size 
  o   Max-Threshold  = Max(DRAM-Bound-Buffer-Factor * DRAM-Recovery-Failure-Factor * Q-ON-Threshold/ Min-Packet-Size, 2 * Min-Threshold) 
  o   Min-Free =  Min( Min-Free-PD * 2 , Max-Free)
  o   Max Free = 32 * 1024 * 0.5
  o   Alpha - Approximate a line to Max threshold -- Log(Max-Threshold/Macx-Free), bounded in range -7:7
 
-  Recovery-Threshold =  Min(Bundle-Size * DRAM-Recovery-Multiplier, DRAM-Bound-FADT-Max-Threshold * DRAM-Recovery-Failure-Buffer-Factor)


Function configure_dram_bound_thresholds configures pre-computed dram bound thresholds for single FAP QAX device for specific port speeds.

For DNX devices - see tuning function dnx_tune_ingress_congestion_default_voq_rate_class_set

*/


enum port_speed_t {
    PORT_SPEED_1G = 0,
    PORT_SPEED_10G, 
    PORT_SPEED_25G, 
    PORT_SPEED_40G,
    PORT_SPEED_100G, 
    PORT_SPEED_COUNT
};

int configure_dram_bound_thresholds(int unit, bcm_gport_t voq_gport, bcm_cos_queue_t cosq, port_speed_t speed)
{

    uint32 flags = 0;
    int result=0;
    bcm_cosq_fadt_info_t thresh_info;
    bcm_cosq_threshold_t recovery_threshold;
    int num_of_speeds = PORT_SPEED_COUNT;

    bcm_cosq_fadt_threshold_t dram_bound_fadt_threshold_bytes_per_port[num_of_speeds] = {
        { /* PORT_SPEED_1G */
            4096,   /* thresh_min */
            8192,   /* thresh_max */
            -7,    /* alpha */
            256000, /* resource_range_min */
            1048576 /* resource_range_max */
        },
       { /* PORT_SPEED_10G */
            4096,   /* thresh_min */
            11674,  /* thresh_max */
            -6,     /* alpha */
            256000, /* resource_range_min */
            1048576 /* resource_range_max */
        },
        { /* PORT_SPEED_25G */
            4096,   /* thresh_min */
            8192,  /* thresh_max */
            -7,     /* alpha */
            256000, /* resource_range_min */
            1048576 /* resource_range_max */
        },
        { /* PORT_SPEED_40G */
            4096,   /* thresh_min */
            9339,  /* thresh_max */
            -7,     /* alpha */
            256000, /* resource_range_min */
            1048576 /* resource_range_max */
        },
        { /* PORT_SPEED_100G */
            4096,   /* thresh_min */
            23347, /* thresh_max */
            -5,     /* alpha */
            256000, /* resource_range_min */
            1048576 /* resource_range_max */
        }
    };

    bcm_cosq_fadt_threshold_t dram_bound_fadt_threshold_pds_per_port[num_of_speeds] = {
        { /* PORT_SPEED_1G */
            64,   /* thresh_min */
            128,   /* thresh_max */
            -6,   /* alpha */
            1000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_10G */
            64,   /* thresh_min */
            182,  /* thresh_max */
            -5,   /* alpha */
            1000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_25G */
            64,   /* thresh_min */
            128,  /* thresh_max */
            -6,   /* alpha */
            1000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_40G */
            64,   /* thresh_min */
            146,  /* thresh_max */
            -6,   /* alpha */
            1000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_100G */
            64,   /* thresh_min */
            365, /* thresh_max */
            -4,   /* alpha */
            1000, /* resource_range_min */
            8192  /* resource_range_max */
        }
    };
            
    bcm_cosq_fadt_threshold_t dram_bound_recovery_failure_fadt_threshold_bytes_per_port[num_of_speeds] = {
        { /* PORT_SPEED_1G */
            4096,    /* thresh_min */
            8192,    /* thresh_max */
            -7,      /* alpha */
            512000, /* resource_range_min */
            2097152 /* resource_range_max */
        },
       { /* PORT_SPEED_10G */
            4096,    /* thresh_min */
            8192,    /* thresh_max */
            -7,      /* alpha */
        },
       { /* PORT_SPEED_25G */
            4096,    /* thresh_min */
            8192,    /* thresh_max */
            -7,      /* alpha */
            512000, /* resource_range_min */
            2097152 /* resource_range_max */
        },
       { /* PORT_SPEED_40G */
            4096,    /* thresh_min */
            8192,    /* thresh_max */
            -7,      /* alpha */
        },
       { /* PORT_SPEED_100G */
            4096,    /* thresh_min */
            11674,   /* thresh_max */
            -7,      /* alpha */
        }
    };
    bcm_cosq_fadt_threshold_t dram_bound_recovery_failure_fadt_threshold_pds_per_port[num_of_speeds] = {
        { /* PORT_SPEED_1G */
            64,   /* thresh_min */
            128,  /* thresh_max */
            -7,   /* alpha */
            4000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_10G */
            64,   /* thresh_min */
            128,  /* thresh_max */
            -7,   /* alpha */
            4000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_25G */
            64,   /* thresh_min */
            128,  /* thresh_max */
            -7,   /* alpha */
            4000, /* resource_range_min */
            8192  /* resource_range_max */
        },
        { /* PORT_SPEED_40G */
            64,   /* thresh_min */
            128,  /* thresh_max */
            -7,   /* alpha */
            4000, /* resource_range_min */
            8192  /* resource_range_max */
        },

        { /* PORT_SPEED_100G */
            64,   /* thresh_min */
            182,  /* thresh_max */
            -6,   /* alpha */
            4000, /* resource_range_min */
            8192  /* resource_range_max */
        }
    };
 

    uint32 recovery_threshold_per_port[num_of_speeds] = {
        584,   /* PORT_SPEED_1G */
        5837,  /* PORT_SPEED_10G */
        14592 ,/* PORT_SPEED_25G */
        16384, /* PORT_SPEED_40G */
        16384  /* PORT_SPEED_100G */
    };

    thresh_info.gport = voq_gport;
    thresh_info.cosq = cosq;

    /* DRAM bound FADT Ocb Bytes */
    thresh_info.thresh_type = bcmCosqFadtDramBound;
    thresh_info.resource_type = bcmResourceOcbBytes;
    bcm_cosq_gport_fadt_threshold_set(unit, flags, &thresh_info, &dram_bound_fadt_threshold_bytes_per_port[speed]);

    /* DRAM bound FADT Ocb PD */
    thresh_info.thresh_type = bcmCosqFadtDramBound;
    thresh_info.resource_type = bcmResourceOcbPacketDescriptors;
    result = bcm_cosq_gport_fadt_threshold_set(unit, flags, &thresh_info, &dram_bound_fadt_threshold_pds_per_port[speed]);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_gport_fadt_threshold_set failed \n"); 
        return result;
    }

    /* Recovery Failure  FADT  Ocb Bytes */
    thresh_info.thresh_type = bcmCosqFadtDramBoundRecoveryFailure;
    thresh_info.resource_type = bcmResourceOcbBytes;
    bcm_cosq_gport_fadt_threshold_set(unit, flags, &thresh_info, &dram_bound_recovery_failure_fadt_threshold_bytes_per_port[speed]);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_gport_fadt_threshold_set failed \n"); 
        return result;
    }

    /* Recovery Failure  FADT Ocb PDs */
    thresh_info.thresh_type = bcmCosqFadtDramBoundRecoveryFailure;
    thresh_info.resource_type = bcmResourceOcbPacketDescriptors;
    bcm_cosq_gport_fadt_threshold_set(unit, flags, &thresh_info, &dram_bound_recovery_failure_fadt_threshold_pds_per_port[speed]);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_gport_fadt_threshold_set failed \n"); 
        return result;
    }

    /* DRAM bound recovery */
    recovery_threshold.type = bcmCosqThresholdBytes; 
    recovery_threshold.flags = BCM_COSQ_THRESHOLD_INGRESS|BCM_COSQ_THRESHOLD_QSIZE_RECOVERY; 
    recovery_threshold.value = recovery_threshold_per_port[speed];

    bcm_cosq_gport_threshold_set(unit, voq_gport, cosq, recovery_threshold);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_gport_threshold_set failed \n"); 
        return result;
    }

    return result;
}


