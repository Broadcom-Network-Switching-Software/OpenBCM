/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_OAM_H__
#define __BCM_OAM_H__

#include <bcm/mpls.h>

/* Miscellaneous OAM defines */
#define BCM_OAM_GROUP_NAME_LENGTH           48         /* Group name length in
                                                          bytes */
#define BCM_OAM_GROUP_INVALID               -1         /* Invalid OAM group ID */
#define BCM_OAM_ENDPOINT_INVALID            -2         /* Invalid OAM endpoint
                                                          ID. */
#define BCM_OAM_ENDPOINT_DEFAULT_INGRESS0   -3         /* OAM default endpoint,
                                                          ingress. */
#define BCM_OAM_ENDPOINT_DEFAULT_INGRESS1   -4         /* OAM default endpoint,
                                                          ingress. */
#define BCM_OAM_ENDPOINT_DEFAULT_INGRESS2   -5         /* OAM default endpoint,
                                                          ingress. */
#define BCM_OAM_ENDPOINT_DEFAULT_INGRESS3   -6         /* OAM default endpoint,
                                                          ingress. */
#define BCM_OAM_ENDPOINT_DEFAULT_EGRESS0    -11        /* OAM default endpoint,
                                                          egress. */
#define BCM_OAM_ENDPOINT_DEFAULT_EGRESS1    -12        /* OAM default endpoint,
                                                          egress. */
#define BCM_OAM_ENDPOINT_DEFAULT_EGRESS2    -13        /* OAM default endpoint,
                                                          egress. */
#define BCM_OAM_ENDPOINT_DEFAULT_EGRESS3    -14        /* OAM default endpoint,
                                                          egress. */
#define BCM_OAM_INTPRI_MAX                  16         /* Internal priority max
                                                          value */
#define BCM_OAM_CARRIER_CODE_MAX            6          /* ITU-T Carrier code
                                                          length */
#define BCM_OAM_LM_COUNTER_MAX              3          /* Maximum number of Loss
                                                          Measurement counters
                                                          that can be touched by
                                                          an LM packet Tx
                                                          (Sample 1, Increment2) */
#define BCM_OAM_MAX_NUM_TLVS_FOR_LBM        2          /* The max TLVS numbers
                                                          of LBM */
#define BCM_OAM_MAX_NUM_SLINDING_WINDOWS    256        /* The max TLVS numbers
                                                          of LBM */
#define BCM_OAM_MAX_NUM_TLVS_FOR_DM         1          /* The max TLVS numbers
                                                          of DM */

/* OAM group flags */
#define BCM_OAM_GROUP_REPLACE               0x01       /* Replace an existing
                                                          OAM group */
#define BCM_OAM_GROUP_WITH_ID               0x02       /* Use the specified
                                                          group ID */
#define BCM_OAM_GROUP_REMOTE_DEFECT_TX      0x04       /* Set the remote defect
                                                          indicator in outgoing
                                                          CCMs for this group */
#define BCM_OAM_GROUP_COPY_ERRORS_TO_CPU    0x08       /* Copy mismatched MAID
                                                          packets to CPU */
#define BCM_OAM_GROUP_COPY_TO_CPU           0x10       /* Copy the received CCM
                                                          packet to the CPU */
#define BCM_OAM_GROUP_GET_FAULTS_ONLY       0x20       /* oam_group_get API gets
                                                          only fault status of
                                                          the group */
#define BCM_OAM_GROUP_TYPE_BHH              0x40       /*  Indicating that this
                                                          group belongs to BHH
                                                          endpoint */
#define BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE 0x80       /* fully flexible 48B
                                                          MAID format */
#define BCM_OAM_GROUP_RX_NAME               0x100      /* This group has two
                                                          MAIDs, one for packets
                                                          transmitted and
                                                          another for packets
                                                          received. The MAID for
                                                          packets transmitted is
                                                          specified in the
                                                          'name' field, and the
                                                          MAID for packets
                                                          received is specified
                                                          in the 'rx_name' field */
#define BCM_OAM_GROUP_FLEXIBLE_MAID_20_BYTE 0x200      /* Only 20 first bytes of
                                                          MAID name may be set.
                                                          One extra MEP-DB entry
                                                          will be allocated. */
#define BCM_OAM_GROUP_FLEXIBLE_MAID_40_BYTE 0x400      /* Only 40 first bytes of
                                                          MAID name may be set.
                                                          Two extra MEP-DB entry
                                                          will be allocated. */

/* OAM group fault flags */
#define BCM_OAM_GROUP_FAULT_REMOTE      0x01       /* A remote defect indicator
                                                      is set in this group */
#define BCM_OAM_GROUP_FAULT_CCM_TIMEOUT 0x02       /* A CCM timeout has occurred
                                                      in this group */
#define BCM_OAM_GROUP_FAULT_CCM_ERROR   0x04       /* A CCM error has occurred
                                                      in this group */
#define BCM_OAM_GROUP_FAULT_CCM_XCON    0x08       /* A CCM xcon has occurred in
                                                      this group */
#define BCM_OAM_GROUP_FAULT_CCM_MAC     0x10       /* A CCM Mac defect has
                                                      occurred in this group */
#define BCM_OAM_GROUP_RDI_TX            BCM_OAM_GROUP_REMOTE_DEFECT_TX /* Set the remote defect
                                                      indicator in outgoing CCMs
                                                      for this group. */

/* OAM endpoint flags and flags2 */
#define BCM_OAM_ENDPOINT_REPLACE            0x0001     /* Replace an existing
                                                          OAM endpoint */
#define BCM_OAM_ENDPOINT_WITH_ID            0x0002     /* Use the specified
                                                          endpoint ID */
#define BCM_OAM_ENDPOINT_REMOTE             0x0004     /* Specifies that this is
                                                          a remote endpoint */
#define BCM_OAM_ENDPOINT_UP_FACING          0x0008     /* Specifies that this
                                                          endpoint faces the
                                                          switching logic */
#define BCM_OAM_ENDPOINT_CCM_RX             0x0010     /* Enables CCM reception.
                                                           This is only valid
                                                          for local endpoints. */
#define BCM_OAM_ENDPOINT_LOOPBACK           0x0020     /* Enables loopback
                                                          response.  This is
                                                          only valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_DELAY_MEASUREMENT  0x0040     /* Enables reception and
                                                          timestamping of delay
                                                          measurement messages. 
                                                          This is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_LINKTRACE          0x0080     /* Enables reception of
                                                          linktrace messages. 
                                                          This is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_PORT_STATE_TX      0x0100     /* Enables the
                                                          transmission of port
                                                          state in outgoing
                                                          CCMs.  This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_INTERFACE_STATE_TX 0x0200     /* Enables the
                                                          transmission of
                                                          interface state in
                                                          outgoing CCMs.  This
                                                          is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_LOSS_MEASUREMENT   0x0400     /* Enables the processing
                                                          of loss measurement
                                                          messages.  This is
                                                          only valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_INTERMEDIATE       0x0800     /* Configures this as a
                                                          MIP. */
#define BCM_OAM_ENDPOINT_PBB_TE             0x1000     /* Specifies that this is
                                                          a PBB-TE endpoint. */
#define BCM_OAM_ENDPOINT_CCM_COPYTOCPU      0x2000     /* Copy received CCM to
                                                          CPU. This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_CCM_DROP           0x4000     /* Drop received CCM.
                                                          This is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_DM_COPYTOCPU       0x8000     /* Copy received delay
                                                          measurement messages
                                                          to CPU. This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_DM_DROP            0x10000    /* Drop received delay
                                                          measurement messages.
                                                          This is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_LB_COPYTOCPU       0x20000    /* Copy received loopback
                                                          messages to CPU. This
                                                          is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_LB_DROP            0x40000    /* Drop received loopback
                                                          messages. This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_LT_COPYTOCPU       0x80000    /* Copy received loopback
                                                          messages to CPU. This
                                                          is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_LT_DROP            0x100000   /* Drop received loopback
                                                          messages. This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_USE_QOS_MAP        0x200000   /* Use QoS map profile
                                                          specified on endpoint */
#define BCM_OAM_ENDPOINT_MATCH_INNER_VLAN   0x400000   /* Selection of up MIP on
                                                          CEP (C-comp) rather
                                                          than on PEP (S-comp).
                                                          For doubly tagged
                                                          MEPs, count based on
                                                          C-comp rather than
                                                          S-comp */
#define BCM_OAM_ENDPOINT_REMOTE_DEFECT_TX   0x800000   /* Set the remote defect
                                                          indicator in outgoing
                                                          CCMs for this
                                                          endpoint. */
#define BCM_OAM_ENDPOINT_CCM_COPYFIRSTTOCPU 0x1000000  /* Copy only the first
                                                          (next) received CCM to
                                                          CPU. This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_PORT_STATE_UPDATE  0x2000000  /* Update port state. 
                                                          This is only valid for
                                                          local endpoints. */
#define BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE 0x4000000  /* Update interface
                                                          state.  This is only
                                                          valid for local
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_PRI_TAG            0x8000000  /* Messages generated
                                                          from this endpoint
                                                          should include a Pri
                                                          Tag. */
#define BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE 0x10000000 /* Do not generate
                                                          events. */
#define BCM_OAM_ENDPOINT_REMOTE_DEFECT_AUTO_UPDATE 0x20000000 /* Auto update defect
                                                          state on timeout
                                                          event. */
#define BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN 0x40000000 /* Selection of MEP based
                                                          on S and C VLAN */
#define BCM_OAM_ENDPOINT_LMEP_PKT_FWD       0x80000000 /* Local endpoint CCMs
                                                          must be forwarded to a
                                                          remote unit for
                                                          processing. */
#define BCM_OAM_ENDPOINT_RDI_TX             BCM_OAM_ENDPOINT_REMOTE_DEFECT_TX /* Set the remote defect
                                                          indicator in outgoing
                                                          CCMs for this
                                                          endpoint. */
#define BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE    BCM_OAM_ENDPOINT_REMOTE_DEFECT_AUTO_UPDATE /* Auto update defect
                                                          event. */
#define BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE 0x1        /* RDI bit on outgoing
                                                          packets may be taken
                                                          from RDI indication on
                                                          received packets. */
#define BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE 0x2        /* RDI bit on outgoing
                                                          packets may be taken
                                                          from LOC indication of
                                                          peer endpoint. */
#define BCM_OAM_ENDPOINT2_RDI_ON_LOC        0x4        /* MEP RDI update LOC
                                                          enable - update local
                                                          MEP RDI in case of
                                                          LOC. */
#define BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR 0x8        /* MEP RDI update LOC
                                                          clean enable - update
                                                          local MEP RDI in case
                                                          of LOC clear (time-in)
                                                          event detected. */
#define BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI     0x10       /* MEP RDI update RX
                                                          enable - Update local
                                                          MEP RDI by copying the
                                                          RDI indication from
                                                          the last valid CCM
                                                          packet received. */
#define BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE 0x20       /* Disable updating RMEP
                                                          DB. */
#define BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE 0x40       /* Auto update defect
                                                          state on timeout event
                                                          for RX packets */
#define BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE 0x80       /* Do not generate events
                                                          for RX packets. */
#define BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET 0x100      /* Configure only HW
                                                          accelerator
                                                          properties. */
#define BCM_OAM_ENDPOINT2_REDIRECT_TO_CPU   0x200      /* Used to send BHH
                                                          packets to CPU instead
                                                          of BTE */
#define BCM_OAM_ENDPOINT2_GET_FAULTS_ONLY   0x200      /* oam_endpoint_get API
                                                          gets only fault status
                                                          of endpoint */
#define BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY 0x800      /* Endpoint with only an
                                                          ingress entry.
                                                          Applicable only for
                                                          endpoints of type
                                                          bcmOAMEndpointTypeBHHMPLS,
                                                          bcmOAMEndpointTypeBHHPWE */
#define BCM_OAM_ENDPOINT2_UPDATE_COUNTER_ACTION 0x1000     /* Use along with
                                                          BCM_OAM_ENDPOINT_REPLACE
                                                          to update just the LM
                                                          parameters including
                                                          counters
                                                          incremented/sampled by
                                                          Tx CCM PDUs */
#define BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY  0x2000     /* Endpoint with only an
                                                          egress entry.
                                                          Applicable only for
                                                          endpoints of type
                                                          bcmOAMEndpointTypeBHHMPLS,
                                                          bcmOAMEndpointTypeBHHPWE */
#define BCM_OAM_ENDPOINT_FLAGS2_MATCH_SINGLE_TAGGED_OUTER_VLAN 0x4000     /* Selection of MEP
                                                          look-up based on
                                                          Single outer tagged
                                                          packets only */
#define BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE BCM_OAM_ENDPOINT2_RDI_FROM_RX_DISABLE /* RDI bit on outgoing
                                                          packets may be taken
                                                          from RDI indication on
                                                          received packets. */
#define BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE BCM_OAM_ENDPOINT2_RDI_FROM_LOC_DISABLE /* RDI bit on outgoing
                                                          packets may be taken
                                                          from LOC indication of
                                                          peer endpoint. */
#define BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC  BCM_OAM_ENDPOINT2_RDI_ON_LOC /* MEP RDI update LOC
                                                          enable - update local
                                                          MEP RDI in case of
                                                          LOC. */
#define BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR /* MEP RDI update LOC
                                                          clean enable - update
                                                          local MEP RDI in case
                                                          of LOC clear (time-in)
                                                          event detected. */
#define BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI /* MEP RDI update RX
                                                          enable - Update local
                                                          MEP RDI by copying the
                                                          RDI indication from
                                                          the last valid CCM
                                                          packet received. */
#define BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE BCM_OAM_ENDPOINT2_REMOTE_UPDATE_STATE_DISABLE /* Disable updating RMEP
                                                          DB. */
#define BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE BCM_OAM_ENDPOINT2_RX_REMOTE_DEFECT_AUTO_UPDATE /* Auto update defect
                                                          state on timeout event
                                                          for RX packets */
#define BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE BCM_OAM_ENDPOINT2_RX_REMOTE_EVENT_DISABLE /* Do not generate events
                                                          for RX packets. */
#define BCM_OAM_ENDPOINT_FLAGS2_HW_ACCELERATION_SET BCM_OAM_ENDPOINT2_HW_ACCELERATION_SET /* Configure only HW
                                                          accelerator
                                                          properties. */
#define BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU BCM_OAM_ENDPOINT2_REDIRECT_TO_CPU /* Used to send BHH
                                                          packets to CPU instead
                                                          of BTE */
#define BCM_OAM_ENDPOINT_FLAGS2_GET_FAULTS_ONLY BCM_OAM_ENDPOINT2_GET_FAULTS_ONLY /* oam_endpoint_get API
                                                          gets only fault status
                                                          of endpoint */
#define BCM_OAM_ENDPOINT_FLAGS2_MPLS_INGRESS_ONLY BCM_OAM_ENDPOINT2_MPLS_INGRESS_ONLY /* Endpoint with only an
                                                          ingress entry.
                                                          Applicable only for
                                                          endpoints of type
                                                          bcmOAMEndpointTypeBHHMPLS,
                                                          bcmOAMEndpointTypeBHHPWE */
#define BCM_OAM_ENDPOINT_FLAGS2_UPDATE_COUNTER_ACTION BCM_OAM_ENDPOINT2_UPDATE_COUNTER_ACTION /* Use along with
                                                          BCM_OAM_ENDPOINT_REPLACE
                                                          to update just the LM
                                                          counters updated by Tx
                                                          CCM packets */
#define BCM_OAM_ENDPOINT_FLAGS2_MPLS_EGRESS_ONLY BCM_OAM_ENDPOINT2_MPLS_EGRESS_ONLY /* Endpoint with only an
                                                          egress entry.
                                                          Applicable only for
                                                          endpoints of type
                                                          bcmOAMEndpointTypeBHHMPLS,
                                                          bcmOAMEndpointTypeBHHPWE */
#define BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ 0x8000     /* double outlif for OAM
                                                          over PWE injection */
#define BCM_OAM_ENDPOINT_FLAGS2_VLAN_VP_UP_MEP_IN_HW 0x10000    /*  Endpoint is Vlan
                                                          based VP UP MEP in HW
                                                          engine and use
                                                          tx_gport to honor the
                                                          vlan translation for
                                                          UP MEP TXed packed
                                                          from HW CCM engine */
#define BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN 0x20000    /* Create egress
                                                          injection down
                                                          endpoint */
#define BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS 0x40000    /* Enable OAMP Tx Opcode
                                                          statistics */
#define BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS 0x80000    /* Enable OAMP Rx Opcode
                                                          statistics */
#define BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS 0x100000   /* Enable OAMP Tx/Rx Per
                                                          Opcode statistics,
                                                          count each opcode in
                                                          separate counter */
#define BCM_OAM_ENDPOINT_FLAGS2_PUNT_NEXT_GOOD_PACKET 0x200000   /* Write-only flag.
                                                          Specifies if the next
                                                          received good packet
                                                          should be punted to
                                                          the CPU */
#define BCM_OAM_ENDPOINT_FLAGS2_SIGNAL      0x400000   /* Indication support of
                                                          Signal Degradation for
                                                          Y1731oPWE(CCM) */
#define BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM 0x800000   /* Inferred Loss
                                                          Measurement endpoint */

/* OAM loss measurment flags */
#define BCM_OAM_LM_PCP          0x0001     /* Use different counters for
                                              different vlan tag packet priority
                                              codes */

/* OAM lif profile flags */
#define BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0 0x0001     /* Configure
                                                          ingress/egress LIF
                                                          Profile of
                                                          BCM_OAM_ENDPOINT_DEFAULT_INGRESS0
                                                          / EGRESS0 */
#define BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1 0x0002     /* Configure
                                                          ingress/egress LIF
                                                          Profile of
                                                          BCM_OAM_ENDPOINT_DEFAULT_INGRESS1
                                                          / EGRESS1 */
#define BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_2 0x0004     /* Configure
                                                          ingress/egress LIF
                                                          Profile of
                                                          BCM_OAM_ENDPOINT_DEFAULT_INGRESS2
                                                          / EGRESS2 */
#define BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_3 0x0008     /* Configure
                                                          ingress/egress LIF
                                                          Profile of
                                                          BCM_OAM_ENDPOINT_DEFAULT_INGRESS3
                                                          / EGRESS3 */

/* OAM opcode flags */
#define BCM_OAM_OPCODE_CCM_IN_HW            0x0001     /* Process CCMs in
                                                          Hardware */
#define BCM_OAM_OPCODE_CCM_COPY_TO_CPU      0x0002     /* Copy CCMs to CPU */
#define BCM_OAM_OPCODE_CCM_DROP             0x0004     /* Drop CCMs */
#define BCM_OAM_OPCODE_LBM_IN_HW            0x0008     /* Process Loopback
                                                          Messages in Hardware */
#define BCM_OAM_OPCODE_LBM_UC_COPY_TO_CPU   0x0010     /* Copy Unicast Loopback
                                                          Messages to CPU */
#define BCM_OAM_OPCODE_LBM_UC_DROP          0x0020     /* Drop Unicast Loopback
                                                          Messages */
#define BCM_OAM_OPCODE_LBM_MC_COPY_TO_CPU   0x0040     /* Copy Multicast
                                                          Loopback Messages to
                                                          CPU */
#define BCM_OAM_OPCODE_LBM_MC_DROP          0x0080     /* Drop Multicast
                                                          Loopback Messages */
#define BCM_OAM_OPCODE_LBR_COPY_TO_CPU      0x0100     /* Copy Loopback Reply to
                                                          CPU */
#define BCM_OAM_OPCODE_LBR_DROP             0x0200     /* Drop Loopback Reply */
#define BCM_OAM_OPCODE_LTM_COPY_TO_CPU      0x0400     /* Copy Linktrace
                                                          Messages to CPU */
#define BCM_OAM_OPCODE_LTM_DROP             0x0800     /* Drop Linktrace
                                                          Messages */
#define BCM_OAM_OPCODE_LTR_COPY_TO_CPU      0x1000     /* Copy Linktrace Reply
                                                          to CPU */
#define BCM_OAM_OPCODE_LTR_DROP             0x2000     /* Drop Linktrace Reply */
#define BCM_OAM_OPCODE_LMEP_PKT_FWD         0x4000     /* Forward CCMs to remote
                                                          device */
#define BCM_OAM_OPCODE_OTHER_COPY_TO_CPU    0x8000     /* Copy other OAM opcode
                                                          to CPU */
#define BCM_OAM_OPCODE_OTHER_DROP           0x10000    /* Drop other OAM opcode
                                                          to CPU */

/* OAM endpoint fault flags */
#define BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT  0x01       /* Indicates a CCM
                                                          timeout.  This is only
                                                          valid for remote
                                                          endpoints. */
#define BCM_OAM_ENDPOINT_FAULT_REMOTE       0x02       /* Indicates that a
                                                          remote defect was
                                                          indicated in the last
                                                          CCM received from this
                                                          endpoint.  This is
                                                          only valid for remote
                                                          endpoints and is
                                                          read-only. */
#define BCM_OAM_ENDPOINT_FAULT_PORT_DOWN    0x04       /* Indicates that the
                                                          port on which the
                                                          endpoint resides is
                                                          down.  This is valid
                                                          for both local and
                                                          remote endpoints but
                                                          is read-only for
                                                          remote endpoints. */
#define BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN 0x08       /* Indicates that the
                                                          interface on which teh
                                                          endpoint resides is
                                                          down.  This is valid
                                                          for both local and
                                                          remote endpoints but
                                                          is read-only for
                                                          remote endpoints. */

/* Constants for CCM periods defined by 802.1ag */
#define BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED 0          
#define BCM_OAM_ENDPOINT_CCM_PERIOD_3MS     3          
#define BCM_OAM_ENDPOINT_CCM_PERIOD_10MS    10         
#define BCM_OAM_ENDPOINT_CCM_PERIOD_100MS   100        
#define BCM_OAM_ENDPOINT_CCM_PERIOD_1S      1000       
#define BCM_OAM_ENDPOINT_CCM_PERIOD_10S     10000      
#define BCM_OAM_ENDPOINT_CCM_PERIOD_1M      60000      
#define BCM_OAM_ENDPOINT_CCM_PERIOD_10M     600000     

/* OAM loss flags */
#define BCM_OAM_LOSS_TX_ENABLE              0x0001     /* Enable Loss Packet
                                                          Transmission */
#define BCM_OAM_LOSS_SINGLE_ENDED           0x0002     /* PDU used for ETH-LM
                                                          information is LMM/R */
#define BCM_OAM_LOSS_FIRST_RX_COPY_TO_CPU   0x0004     /* Copy First received
                                                          Loss Measurement
                                                          Packet to CPU */
#define BCM_OAM_LOSS_ALL_RX_COPY_TO_CPU     0x0008     /* Copy all received Loss
                                                          Measurement Packet to
                                                          CPU */
#define BCM_OAM_LOSS_WITH_ID                0x0010     /* Use the specified loss
                                                          id */
#define BCM_OAM_LOSS_PRI_INTERNAL           0x0020     /* Selects counting based
                                                          on internal COS rather
                                                          than packet priority */
#define BCM_OAM_LOSS_DP_INTERNAL            0x0040     /* Selects counting based
                                                          on internal DP rather
                                                          than packet CFI */
#define BCM_OAM_LOSS_COUNT_POST_TRAFFIC_CONDITIONING 0x0080     /* Specifies that OEM
                                                          loss measuements are
                                                          taken on the wire side
                                                          (pre traffic
                                                          conditioning) or on
                                                          the switching side
                                                          (post traffic
                                                          conditioning) of a
                                                          forwarding function */
#define BCM_OAM_LOSS_COUNT_GREEN_AND_YELLOW 0x0100     /* Specifies whether the
                                                          OEM loss measurements
                                                          includes green only
                                                          frames or green and
                                                          yellow frames */
#define BCM_OAM_LOSS_COUNT_CPU_RX_PKT       0x0200     /* Specifies that RX LM
                                                          counter needs to be
                                                          updated for packet
                                                          received from CPU */
#define BCM_OAM_LOSS_COUNT_CPU_TX_PKT       0x0400     /* Specifies that TX LM
                                                          counter needs to be
                                                          updated for packet
                                                          received from CPU */
#define BCM_OAM_LOSS_STATISTICS_EXTENDED    0x800      /* Specifies that
                                                          extended statistics
                                                          are to be maintained */
#define BCM_OAM_LOSS_UPDATE                 0x1000     /* Update LM settings for
                                                          given endpoint (leaves
                                                          statistics unchanged). */
#define BCM_OAM_LOSS_SLM                    0x2000     /* Transmit SLM/Rs (as
                                                          opposed to LMM/Rs). */
#define BCM_OAM_LOSS_REPORT_MODE            0x4000     /* Enable report mode. */
#define BCM_OAM_COMBINED_LOSS_DELAY         0x8000     /* Enable Combined Loss
                                                          and Delay mode for
                                                          MPLS LM/DM (RFC 6374),
                                                          both loss and delay
                                                          add should be called
                                                          with this flag for the
                                                          same endpoint Id */
#define BCM_OAM_LOSS_UPDATE_NEXT_RECEIVED_SLR 0x10000    /* Set the LM DB to be
                                                          updated by next
                                                          received SLR. Applies
                                                          only to a single SLR. */

/* OAM delay flags */
#define BCM_OAM_DELAY_TX_ENABLE             0x0001     /* Enable Delay Packet
                                                          Transmission */
#define BCM_OAM_DELAY_ONE_WAY               0x0002     /* One Way Mode */
#define BCM_OAM_DELAY_FIRST_RX_COPY_TO_CPU  0x0004     /* Copy First received
                                                          Delay Measurement
                                                          Packet to CPU */
#define BCM_OAM_DELAY_ALL_RX_COPY_TO_CPU    0x0008     /* Copy all received
                                                          Delay Measurement
                                                          Packet to CPU */
#define BCM_OAM_DELAY_WITH_ID               0x0010     /* Use the specified
                                                          delay id */
#define BCM_OAM_DELAY_UPDATE                0x20       /* Update DM settings for
                                                          given endpoint (leaves
                                                          statistics unchanged). */
#define BCM_OAM_DELAY_REPORT_MODE           0x40       /* Enable report mode. */
#define BCM_OAM_COMBINED_LOSS_DELAY_RSVD    0x8000     /* Enable Combined Loss
                                                          and Delay mode for
                                                          MPLS LM/DM (RFC 6374),
                                                          both loss and delay
                                                          add should be called
                                                          with this flag for the
                                                          same endpoint Id */

/* OAM endpoint action flags */
#define BCM_OAM_ENDPOINT_ACTION_DISABLE 0x00000001 /* Reset the endpoint
                                                      action(s) */

/* Counter action values for loss add API */
typedef enum bcm_oam_lm_counter_action_e {
    bcmOamCounterActionNone = 0,        /* No action */
    bcmOamCounterActionIncrement = 1,   /* Increment Counter */
    bcmOamCounterActionSample = 2       /* Sample Counter */
} bcm_oam_lm_counter_action_t;

/* OAM AIS flags */
#define BCM_OAM_AIS_UPDATE          0x0001     /* Update AIS settings for given
                                                  endpoint. */
#define BCM_OAM_AIS_MULTICAST       0x0002     /* Transmit AIS frames with
                                                  multicast DA address. */
#define BCM_OAM_AIS_WITH_AIS_ID     0x0004     /* Use given ais_id */
#define BCM_OAM_AIS_TRANSMIT_START  0x0008     /* Enable AIS transmission for
                                                  given local MEP */
#define BCM_OAM_AIS_TRANSMIT_STOP   0x0010     /* Disable AIS transmission for
                                                  given local MEP */

/* BHH Loopback flags, used to set flags field of bcm_oam_loopback_t */
#define BCM_OAM_BHH_INC_REQUESTING_MEP_TLV  0x00000001 /* Include requesting MEP
                                                          ID TLV in message */
#define BCM_OAM_BHH_SET_TTL                 0x00000002 /* Set TLL field in LBM */
#define BCM_OAM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV 0x00000010 /* Use ingress discovery
                                                          MEP ID TLV in LBM */
#define BCM_OAM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV 0x00000020 /* Use egress discovery
                                                          MEP ID TLV in LBM */
#define BCM_OAM_BHH_LBM_ICC_MEP_TLV         0x00000040 /* Use ICC based MEP ID
                                                          TLV in LBM */
#define BCM_OAM_BHH_LBM_ICC_MIP_TLV         0x00000080 /* Use ICC based MIP ID
                                                          TLV in LBM */
#define BCM_OAM_BHH_LBR_ICC_MEP_TLV         0x00000100 /* Use ICC based MEP ID
                                                          TLV in LBR */
#define BCM_OAM_BHH_LBR_ICC_MIP_TLV         0x00000200 /* Use ICC based MIP ID
                                                          TLV in LBR */

/* OAM loopback flags, used to set flags field in bcm_oam_loopback_t */
#define BCM_OAM_LOOPBACK_UPDATE         0x1        /* Update Loopback settings
                                                      (leaves statistics
                                                      unchanged). */
#define BCM_OAM_LOOPBACK_WITH_CTF_ID    0x2        
#define BCM_OAM_LOOPBACK_WITH_GTF_ID    0x4        
#define BCM_OAM_LOOPBACK_PERIOD_IN_PPS  0x8        /* Default is milliseconds.
                                                      flag may be used for
                                                      packet per second. */
#define BCM_OAM_LOOPBACK_TX_ENABLE      0x400      /* Enable Loopback Packet
                                                      Transmission */

/* OAM TST flags */
#define BCM_OAM_TST_TX_UPDATE   0x1        /* Update Loopback settings (leaves
                                              statistics unchanged). */
#define BCM_OAM_TST_RX_UPDATE   0x2        
#define BCM_OAM_TST_TX_WITH_ID  0x4        
#define BCM_OAM_TST_RX_WITH_ID  0x8        

/* OAM PSC flags */
#define BCM_OAM_PSC_TX_BURST                0x0001     /* Trigger PSC Burst
                                                          Transmission */
#define BCM_OAM_PSC_TX_ENABLE               0x0002     /* Enable PSC slow-rate
                                                          Transmission */
#define BCM_OAM_PSC_REVERTIVE               0x0004     /* Enable Revertive mode */
#define BCM_OAM_PSC_FIRST_RX_COPY_TO_CPU    0x0008     /* Copy First received
                                                          PSC message to CPU */
#define BCM_OAM_PSC_ALL_RX_COPY_TO_CPU      0x0010     /* Copy all received PSC
                                                          message to CPU */

/* OAM PW Status flags */
#define BCM_OAM_PW_STATUS_TX_BURST          0x0001     /* Trigger PW Status
                                                          Burst Transmission */
#define BCM_OAM_PW_STATUS_TX_ENABLE         0x0002     /* Enable PW Status
                                                          Packet Transmission */
#define BCM_OAM_PW_STATUS_FIRST_RX_COPY_TO_CPU 0x0004     /* Copy First received PW
                                                          Status message to CPU */
#define BCM_OAM_PW_STATUS_ALL_RX_COPY_TO_CPU 0x0008     /* Copy all received PW
                                                          Status message to CPU */

/* 
 * Constants for FFD frequency defined by Y.1711, which will reuse ccm
 * period field in endpoint objects
 */
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_10MS 10         
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_20MS 20         
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_50MS 50         
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_100MS 100        
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_200MS 200        
#define BCM_OAM_ENDPOINT_FFD_FREQUENCY_500MS 500        

/* OAM SD/SF flags */
#define BCM_OAM_SD_SF_FLAGS_ALERT_METHOD    0x1        /* if set, issue event on
                                                          signal degradation /
                                                          signal fault set,
                                                          otherwise issue alert
                                                          on state change */
#define BCM_OAM_SD_SF_FLAGS_ALERT_SUPRESS   0x2        /* Suppress alert event
                                                          generation */

/* OAM Y.1711 ALARM flags */
#define BCM_OAM_Y_1711_FLAGS_ALERT_METHOD   0x1        /* if alert_method 1,
                                                          issue event on
                                                          mismerge, mismatch or
                                                          excess indication set,
                                                          otherwise issue event
                                                          on state change */

/* OAM Y.1711 ALARM flags */
#define BCM_OAM_Y_1711_FLAGS_THRESHOLD  0x2        /* Use to configure 1711
                                                      threshold globally */

/* OAM CSF flags */
#define BCM_OAM_CSF_UPDATE      0x0001     /* Update CSF settings for given
                                              endpoint. */

/* CSF Type */
#define BCM_OAM_CSF_LOS         0x0        /* Loss of Signal. */
#define BCM_OAM_CSF_FDI         0x1        /* Forward Defect Indication. */
#define BCM_OAM_CSF_RDI         0x2        /* Reverse Defect Indication. */
#define BCM_OAM_CSF_DCI         0x3        /* Defect Clear Indication. */

/* OAM endpoint types */
typedef enum bcm_oam_endpoint_type_e {
    bcmOAMEndpointTypeEthernet = 0,     /* Ethernet endpoint */
    bcmOAMEndpointTypeMPLSPerformance = 1, /* MPLS performance monitoring endpoint */
    bcmOAMEndpointTypePSC = 2,          /* Protection coordination state
                                           protocol endpoint */
    bcmOAMEndpointTypeBHHMPLS = 3,      /* BHH for MPLS-TP */
    bcmOAMEndpointTypeBHHMPLSVccv = 4,  /* BHH for MPLS-TP Psuedowire Virtual
                                           Circuit Connectivity Verification */
    bcmOAMEndpointTypeBHHPwe = 5,       /* BHH for PWE */
    bcmOAMEndpointTypePwStatus = 6,     /* Protection coordination state
                                           protocol endpoint */
    bcmOAMEndpointTypeMplsLmDmSectionPort = 7, /* RFC 6374 Section loss/delay
                                           measurement based on Port */
    bcmOAMEndpointTypeMplsLmDmSectionInnervlan = 8, /* RFC 6374 Section loss/delay
                                           measurement based on Port and Inner
                                           vlan */
    bcmOAMEndpointTypeMplsLmDmSectionOuterVlan = 9, /* RFC 6374 Section loss/delay
                                           measurement based on Port and Outer
                                           vlan */
    bcmOAMEndpointTypeMplsLmDmSectionOuterPlusInnerVlan = 10, /* RFC 6374 Section loss/delay
                                           measurement based on Port and Outer
                                           vlan and Inner vlan */
    bcmOAMEndpointTypeMplsLmDmPw = 11,  /* RFC 6374 Section loss/delay
                                           measurement based on Pseudowire */
    bcmOAMEndpointTypeMplsLmDmLsp = 12, /* RFC 6374 Section loss/delay
                                           measurement based on LSP */
    bcmOAMEndpointTypeBhhSection = 13,  /* BHH Section based on Port */
    bcmOAMEndpointTypeBhhSectionInnervlan = 14, /* BHH section based on Port and Inner
                                           vlan */
    bcmOAMEndpointTypeBhhSectionOuterVlan = 15, /* BHH section based on Port and Outer
                                           vlan */
    bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan = 16, /* BHH section based on Port and Outer
                                           vlan and Inner vlan */
    bcmOAMEndpointTypeMPLSNetwork = 17, /*  MPLS networks monitoring endpoint
                                           for ITU-T Y.1711 */
    bcmOAMEndpointTypeBHHPweGAL = 18,   /* BHH for PWE over GAL */
    bcmOAMEndpointTypeMplsLmDmSection = 19, /* RFC 6374 Section loss/delay
                                           measurement based on SectionBHH for
                                           PWE over GAL */
    bcmOAMEndpointTypeCount = 20        /* Always the last. Not an usable value. */
} bcm_oam_endpoint_type_t;

/* VCCV Control Channel Type */
typedef enum bcm_oam_vccv_type_e {
    bcmOamBhhVccvChannelAch = 0,    /* Channel ACH */
    bcmOamBhhVccvRouterAlert = 1,   /* Router Alert */
    bcmOamBhhVccvTtl = 2,           /* TTL */
    bcmOamBhhVccvGal13 = 3          /* GAL 13 */
} bcm_oam_vccv_type_t;

/* OAM event types */
typedef enum bcm_oam_event_type_e {
    bcmOAMEventEndpointPortDown = 0, 
    bcmOAMEventEndpointPortUp = 1, 
    bcmOAMEventEndpointInterfaceDown = 2, 
    bcmOAMEventEndpointInterfaceUp = 3, 
    bcmOAMEventEndpointInterfaceTestingToUp = 4, 
    bcmOAMEventEndpointInterfaceUnknownToUp = 5, 
    bcmOAMEventEndpointInterfaceDormantToUp = 6, 
    bcmOAMEventEndpointInterfaceNotPresentToUp = 7, 
    bcmOAMEventEndpointInterfaceLLDownToUp = 8, 
    bcmOAMEventEndpointInterfaceTesting = 9, 
    bcmOAMEventEndpointInterfaceUnkonwn = 10, 
    bcmOAMEventEndpointInterfaceDormant = 11, 
    bcmOAMEventEndpointInterfaceNotPresent = 12, 
    bcmOAMEventEndpointInterfaceLLDown = 13, 
    bcmOAMEventGroupCCMxcon = 14, 
    bcmOAMEventGroupCCMError = 15, 
    bcmOAMEventGroupRemote = 16, 
    bcmOAMEventGroupCCMTimeout = 17, 
    bcmOAMEventBHHLBTimeout = 18, 
    bcmOAMEventBHHLBDiscoveryUpdate = 19, 
    bcmOAMEventBHHCCMTimeout = 20, 
    bcmOAMEventBHHCCMState = 21, 
    bcmOAMEventEndpointCCMTimeout = 22, 
    bcmOAMEventEndpointCCMTimein = 23, 
    bcmOAMEventEndpointCCMTimeoutEarly = 24, 
    bcmOAMEventEndpointRemote = 25, 
    bcmOAMEventEndpointRemoteUp = 26, 
    bcmOAMEventEndpointDmStatistics = 27, 
    bcmOAMEventEndpointLmStatistics = 28, 
    bcmOAMEventBHHCCMRdi = 29, 
    bcmOAMEventBHHCCMUnknownMegLevel = 30, 
    bcmOAMEventBHHCCMUnknownMegId = 31, 
    bcmOAMEventBHHCCMUnknownMepId = 32, 
    bcmOAMEventBHHCCMUnknownPeriod = 33, 
    bcmOAMEventBHHCCMUnknownPriority = 34, 
    bcmOAMEventBHHCCMRdiClear = 35, 
    bcmOAMEventBHHCCMUnknownMegLevelClear = 36, 
    bcmOAMEventBHHCCMUnknownMegIdClear = 37, 
    bcmOAMEventBHHCCMUnknownMepIdClear = 38, 
    bcmOAMEventBHHCCMUnknownPeriodClear = 39, 
    bcmOAMEventBHHCCMUnknownPriorityClear = 40, 
    bcmOAMEventBHHRawData = 41, 
    bcmOAMEventEthLmDmRawData = 42, 
    bcmOAMEventMplsLmDmRawData = 43, 
    bcmOAMEventGroupMACStatus = 44, 
    bcmOAMEventGroupCCMxconClear = 45, 
    bcmOAMEventGroupCCMErrorClear = 46, 
    bcmOAMEventGroupRemoteClear = 47, 
    bcmOAMEventGroupCCMTimeoutClear = 48, 
    bcmOAMEventGroupMACStatusClear = 49, 
    bcmOAMEventBHHCCMTimeoutClear = 50, 
    bcmOAMEventBhhPmCounterRollover = 51, 
    bcmOAMEventEthLmDmPmCounterRollover = 52, 
    bcmOAMEventMplsLmDmPmCounterRollover = 53, 
    bcmOAMEventCsfLos = 54, 
    bcmOAMEventCsfFdi = 55, 
    bcmOAMEventCsfRdi = 56, 
    bcmOAMEventCsfDci = 57, 
    bcmOAMEventEndpointSdSet = 58,      /* Signal degradation set. For Y1731 or
                                           MPLS networks endpoints */
    bcmOAMEventEndpointSdClear = 59,    /* Signal degradation clear. For Y1731
                                           or MPLS networks endpoints */
    bcmOAMEventEndpointSfSet = 60,      /* Signal failure set. For Y1731 or MPLS
                                           networks endpoints */
    bcmOAMEventEndpointSfClear = 61,    /* Signal failure clear. For Y1731 or
                                           MPLS networks endpoints */
    bcmOAMEventEndpointDExcessSet = 62, /* DExcess detect. For MPLS networks
                                           endpoints */
    bcmOAMEventEndpointDMissmatch = 63, /* DMissmatch detect. For MPLS networks
                                           endpoints */
    bcmOAMEventEndpointDMissmerge = 64, /* DMissmerge detect. For MPLS networks
                                           endpoints */
    bcmOAMEventEndpointDAllClear = 65,  /* DAllClear. For MPLS networks
                                           endpoints */
    bcmOAMEventCount = 66 
} bcm_oam_event_type_t;

/* OAM Group Fault Alarm Defects and Priorities */
typedef enum bcm_oam_group_fault_alarm_defect_priority_e {
    bcmOAMGroupFaultAlarmPriorityDefectsAll = 0, /* Generate fault alarm for all defects */
    bcmOAMGroupFaultAlarmPriorityDefectRDICCM = 1, /* Generate fault alarm when CCM
                                           received by this Maintenance Endpoint
                                           from some remote Maintenance Endpoint
                                           contained RDI bit */
    bcmOAMGroupFaultAlarmPriorityDefectMACStatus = 2, /* Generate fault alarm when the last
                                           CCM received by this MEP from some
                                           remote MEP indicated that the
                                           Transmitting Maintenance Endpoint's
                                           associated MAC is reporting an error
                                           status */
    bcmOAMGroupFaultAlarmPriorityDefectRemoteCCM = 3, /* Generate fault alarm when Maintenance
                                           Endpoint is not receiving CCMs from
                                           some other Maintenance Endpoint in
                                           its configured list */
    bcmOAMGroupFaultAlarmPriorityDefectErrorCCM = 4, /* Generate fault alarm when Maintenance
                                           Endpoint is receiving Invalid CCMs */
    bcmOAMGroupFaultAlarmPriorityDefectXconCCM = 5, /* Generate fault alarm when Maintenance
                                           Endpoint is receiving CCMs that could
                                           be from some other MA */
    bcmOAMGroupFaultAlarmPriorityDefectsNone = 6, /* No Defects are reported */
    bcmOAMGroupFaultAlarmPriorityCount = 7 
} bcm_oam_group_fault_alarm_defect_priority_t;

/* OAM Timestamp Formats */
typedef enum bcm_oam_timestamp_format_e {
    bcmOAMTimestampFormatNTP = 0,       /* Network Time Protocol version 4
                                           64-bit timestamp format [RFC5905] */
    bcmOAMTimestampFormatIEEE1588v1 = 1 /* Low-order 64 bits of the IEEE
                                           1588-2008 (1588v2) Precision Time
                                           Protocol timestamp format [IEEE1588] */
} bcm_oam_timestamp_format_t;

/* OAM action types */
typedef enum bcm_oam_action_type_e {
    bcmOAMActionCountEnable = 0,        /* Enable counting. */
    bcmOAMActionMeterEnable = 1,        /* Enable metering. */
    bcmOAMActionDrop = 2,               /* Drop the packet. */
    bcmOAMActionCopyToCpu = 3,          /* Send the packet to CPU. */
    bcmOAMActionFwdAsData = 4,          /* Forward the packet as normal data
                                           packet. */
    bcmOAMActionFwd = 5,                /* Forward the packet to a specified
                                           destination. */
    bcmOAMActionUcDrop = 6,             /* Drop UC OAM packet. */
    bcmOAMActionUcCopyToCpu = 7,        /* Send UC OAM packet to CPU. */
    bcmOAMActionUcFwdAsData = 8,        /* Forward UC OAM packet as normal data
                                           packet. */
    bcmOAMActionUcFwd = 9,              /* Forward UC OAM packet to a specified
                                           destination. */
    bcmOAMActionMcDrop = 10,            /* Drop MC OAM packet. */
    bcmOAMActionMcCopyToCpu = 11,       /* Send MC OAM packet to CPU. */
    bcmOAMActionMcFwdAsData = 12,       /* Forward MC OAM packet as normal data
                                           packet. */
    bcmOAMActionMcFwd = 13,             /* Forward MC OAM packet to a specified
                                           destination. */
    bcmOAMActionLowMdlDrop = 14,        /* Drop low MDL packet. */
    bcmOAMActionLowMdlCopyToCpu = 15,   /* copy low MDL packets to CPU. */
    bcmOAMActionLowMdlFwdAsData = 16,   /* Forward low MDL packets as normal
                                           data packets. */
    bcmOAMActionLowMdlFwd = 17,         /* Forward low MDL packets to a
                                           specified destination. */
    bcmOAMActionMyStationMissCopyToCpu = 18, /* Forward MY_STATION_MISS packets to
                                           CPU. */
    bcmOAMActionMyStationMissDrop = 19, /* DROP MY_STATION_MISS packets. */
    bcmOAMActionMyStationMissFwdAsData = 20, /* Forward MY_STATION_MISS packets as
                                           normal data packets. */
    bcmOAMActionMyStationMissFwd = 21,  /* Forward MY_STATION_MISS packets to a
                                           specified destination. */
    bcmOAMActionProcessInHw = 22,       /* Process in internal OAM/CCM engine. */
    bcmOAMActionLowMdlCcmFwdAsRegularCcm = 23, /* Use same action as that of normal
                                           CCM. */
    bcmOAMActionSLMEnable = 24,         /* Count SLM packets, disable of data
                                           packets counting. */
    bcmOAMActionLowMdlProcessInInternalOamEngine = 25, /* Process Low MDL packets in internal
                                           OAM/CCM engine. */
    bcmOAMActionL2StationMissProcessInInternalOamEngine = 26, /* Process l2 station miss packets in
                                           internal CCM engine. */
    bcmOAMActionL2StationHitProcessInInternalOamEngine = 27, /* Process L2 station hit packets in
                                           internal CCM engine. */
    bcmOAMActionMCProcessInInternalOamEngine = 28, /* Process Multicast OAM packets in
                                           internal OAM engine. */
    bcmOAMActionSampleTimeStamp = 29,   /* Sample the time stamp value to DM
                                           packet. */
    bcmOAMActionSampleLmCounter = 30,   /* Sample the counter value to LM
                                           packet. */
    bcmOAMActionCount = 31              /* Always Last. Not a usable value. */
} bcm_oam_action_type_t;

/* OAM Control types */
typedef enum bcm_oam_control_type_e {
    bcmOamControlFlexibleDropPort = 0,  /* Flexible drop control for Ingress
                                           port-domain OAM */
    bcmOamControlFlexibleDropCVlan = 1, /* Flexible drop control for Ingress
                                           C-VLAN domain OAM */
    bcmOamControlFlexibleDropSVlan = 2, /* Flexible drop control for Ingress
                                           S-VLAN domain OAM */
    bcmOamControlFlexibleDropCPlusSVlan = 3, /* Flexible drop control for Ingress
                                           C+S-VLAN domain OAM */
    bcmOamControlFlexibleDropVP = 4,    /* Flexible drop control for Ingress VP
                                           based(PBB-TE) OAM domain */
    bcmOamControlFlexibleDropEgressPort = 5, /* Flexible drop control for egress
                                           port-domain OAM */
    bcmOamControlFlexibleDropEgressCVlan = 6, /* Flexible drop control for egress
                                           C-VLAN domain OAM */
    bcmOamControlFlexibleDropEgressSVlan = 7, /* Flexible drop control for egress
                                           S-VLAN domain OAM */
    bcmOamControlFlexibleDropEgressCPlusSVlan = 8, /* Flexible drop control for egress
                                           C+S-VLAN domain OAM */
    bcmOamControlFlexibleDropEgressVP = 9, /* Flexible drop control for egress VP
                                           based (PBB-TE) OAM domain */
    bcmOamControlFlexibleDropPasssiveSap = 10, /* Flexible drop control for S-VLAN
                                           domain Passive OAM filtering */
    bcmOamControlFlexibleDropEgressPasssiveSap = 11, /* Flexible drop control for egress
                                           S-VLAN domain Passive OAM filtering */
    bcmOamControlReportMode = 12,       /* Accepts of one of the
                                           bcm_oam_report_mode_type_t as the
                                           arg. */
    bcmOamControlUnknownAchCount = 13,  /* Count unknown ACH or Any VCCV packet
                                           with or without ACH. */
    bcmOamControlNtpToD = 14,           /* Set the NTP Time of Day used for DM. */
    bcmOamControl1588ToD = 15,          /* Set the 1588 Time of Day used for DM. */
    bcmOamControlSrcMacLsbReserve = 16, /* Reserve Src-Mac LSB value to be used
                                           for OAM endpoints (resource shared
                                           with BFD endpoints) */
    bcmOamControlSrcMacLsbUnReserve = 17, /* Unreserve Src-Mac LSB value (resource
                                           shared with BFD endpoints) */
    bcmOamControlLmPcpCounterRangeMax = 18, /* lm_counter_base_ids between Max and
                                           Min will count by priority */
    bcmOamControlLmPcpCounterRangeMin = 19, /* lm_counter_base_ids between Max and
                                           Min will count by priority */
    bcmOamControlOampPuntPacketIntPri = 20, /* Bits [1:0] of given value determine
                                           the color (DP), bits [4:2] determine
                                           the Traffic Class */
    bcmOamControlOampCCMWeight = 21,    /* Given may be between 0 and 6 */
    bcmOamControlOampSATWeight = 22,    /* Given may be between 0 and 6 */
    bcmOamControlOampResponseWeight = 23, /* Given may be between 0 and 6 */
    bcmOamControlOampStatsTxOpcode = 24, /* Enable/Disable counting of specific
                                           opcode in Tx, may be used in
                                           bcm_oam_control_indexed_set/get. The
                                           Index is the OpCode (CCM OpCode also
                                           controls counting BFD PDUs) */
    bcmOamControlOampStatsRxOpcode = 25, /* Enable/Disable counting of specific
                                           opcode in Rx, may be used in
                                           bcm_oam_control_indexed_set/get. The
                                           Index is the OpCode (CCM OpCode also
                                           controls counting BFD PDUs) */
    bcmOamControlOampStatsShift = 26,   /* Used for building counter pointer for
                                           OAMP Tx/Rx statistics */
    bcmOamControlNativeEthernetOverPweClassification = 27, /* If value is set, OAM packets over
                                           native Ethernet over PWE is
                                           recognized as data packet on PWE LIF.
                                           If value is unset, OAM packet is
                                           recognized as OAM packet */
    bcmOamControlEnableNextReceivedSlr = 28, /* When set, LM session should not be
                                           used with self-contained MEP-DB
                                           entries. */
    bcmOamControlMplsLmQueryControlCode = 29, /* Configures the Control Code in
                                           transmitted LM Query PDU. */
    bcmOamControlMplsLmResponderControlCode = 30, /* Configures the Control Code in
                                           transmitted LM Response PDU. */
    bcmOamControlMplsDmQueryControlCode = 31, /* Configures the Control Code in
                                           transmitted DM Query PDU. */
    bcmOamControlMplsDmResponderControlCode = 32, /* Configures the Control Code in
                                           transmitted DM Response PDU. */
    bcmOamControlMplsLmDmOampActionPerQueryControlCode = 33, /* Determine action per Control Code for
                                           incoming Query packets in the OAMP -
                                           set arg to 1 to ignore packets, 2 to
                                           punt, and 0 to process measurement
                                           data. */
    bcmOamControlMplsLmDmOampActionPerResponseControlCode = 34, /* Determine action per Control Code for
                                           incoming Response packets in the OAMP
                                           - set arg to 1 to ignore packets, 2
                                           to punt, and 0 to process measurement
                                           data. */
    bcmOamControlMplsOamMdlIgnore = 35, /* Enable/Disable ingoring MDL when
                                           identifying Y.1731 OAM over MPLS-TP. */
    bcmOAMControlCount = 36             /* Always Last. Not a usable value. */
} bcm_oam_control_type_t;

/* Drop conditions that need to be ignored */
typedef enum bcm_oam_condition_e {
    bcmOamConditionPFCDrop = 0,         /* PFC frames marked for drop */
    bcmOamConditionFlowControlDrop = 1, /* VOQ Flow control packet marked for
                                           drop */
    bcmOamConditionMacsaEqualsMacdaDrop = 2, /* Packet marked for drop by the DoS
                                           prevention logic due to MAC SA = MAC
                                           DA condition set via
                                           bcmSwitchDosAttackMACSAEqualMACDA */
    bcmOamConditionAutoMulticastTunnelControlPacketDrop = 3, /* AMT(Auto Multicast Tunnel) control
                                           packet marked for drop due to AMT
                                           tunnel configuration being set to not
                                           to send AMT control packet to CPU */
    bcmOamConditionUnknownSubportErrorDrop = 4, /* Packet marked for drop for coming
                                           from an unknown subtending port */
    bcmOamConditionSubportPktTagDrop = 5, /* Packet with link layer tag marked for
                                           drop. This happens when
                                           bcmPortControlSubportPktTagDrop
                                           control is set to drop packets with
                                           LLTAG(TR101) */
    bcmOamConditionNoSubportPktTagDrop = 6, /* Packet without link layer tag marked
                                           for drop. This happens when
                                           bcmPortControlNoSubportPktTagDrop
                                           control is set to drop packets with
                                           link layer tag(TR101) */
    bcmOamConditionIngressFilterDrop = 7, /* Packet marked for drop due to port
                                           not being a member of VLAN. This
                                           happens when VLAN filtering is
                                           enabled by passing
                                           BCM_PORT_VLAN_MEMBER_INGRESS flags to
                                           bcm_port_vlan_member_set API */
    bcmOamConditionEgressFilterDrop = 8, /* Packet marked for drop due to port
                                           not being a member of VLAN. This
                                           happens when VLAN filtering is
                                           enabled by passing
                                           BCM_PORT_VLAN_MEMBER_EGRESS flags to
                                           bcm_port_vlan_member_set API */
    bcmOamConditionInvalidTpidDrop = 9, /* Packet marked for drop due to
                                           mismatch between packet VLAN TPID and
                                           configured VLAN TPID value (in VLAN
                                           profile) */
    bcmOamConditionOamMetadataHeaderErrorDrop = 10, /* Packet marked for drop due to OLP
                                           (OAM metadata) header error */
    bcmOamConditionFieldLookupDrop = 11, /* Packet marked for drop due to VFP
                                           action */
    bcmOamConditionSATDrop = 12,        /* Packet marked for drop by SAT */
    bcmOamConditionVlanTranslateIngressMissDrop = 13, /* Packet marked for drop due to vlan
                                           translate miss. This drop is
                                           configured via VLAN control
                                           bcmVlanTranslateIngressMissDrop */
    bcmOamConditionVlanTranslateEgressMissDrop = 14, /* Packet marked for drop due to vlan
                                           translate miss. This drop is
                                           configured via VLAN control
                                           bcmVlanTranslateEgressMissDrop */
    bcmOamConditionMplsLabelLookupMissDrop = 15, /* Packet marked for drop due to MPLS
                                           label lookup miss */
    bcmOamConditionMplsInvalidActionDrop = 16, /* Packet marked for drop due to MPLS
                                           lookup result configured with invalid
                                           action */
    bcmOamConditionMplsInvalidPayloadDrop = 17, /* Packet marked for drop by MPLS
                                           processing due to invalid payload */
    bcmOamConditionMplsTtlErrorDrop = 18, /* Packet marked for drop due to MPLS
                                           TTL check fail */
    bcmOamConditionStgNotInForwardDrop = 19, /* Packet marked for drop due to
                                           Spanning tree not being in
                                           FORWARDING_STATE */
    bcmOamConditionEgressTtlErrorDrop = 20, /* Packet marked for drop due to TTL
                                           error */
    bcmOamConditionEgressStgBlockedDrop = 21, /* Packet marked for drop due to
                                           Spanning tree in blocking state */
    bcmOamConditionEgressStgDisabledDrop = 22, /* Packet marked for drop due to
                                           Spanning tree in disable state */
    bcmOamConditionEgressIPMCSameVlanPruneDrop = 23, /* IPMC packet marked for drop due to
                                           bcmSwitchIpmcSameVlanPruning setting.
                                           When this is set, trying to do L2
                                           switching for an IPMC packet results
                                           in packet drop */
    bcmOamConditionEgressMMUErrorDrop = 24, /* Packet marked for drop due to MMU
                                           error - purge/age */
    bcmOamConditionEgressBadTdipDrop = 25, /* Packet marked for drop due to bad
                                           tunnel destination IP in case of 6to4
                                           tunnels and ISATAP tunnels - possible
                                           reasons are incorrect prefix for IPv6
                                           DIP, IPv4 DIP/SIP is
                                           BC/Zero/loopback/Class-D , IPv4 SIP =
                                           DIP */
    bcmOamConditionEgressUnknownVlanDrop = 26, /* Packet marked for drop due to unknown
                                           VLAN. This happens when
                                           bcmVlanDropUnknown vlan control is
                                           used to set dropping of unknown VLAN
                                           packets */
    bcmOamConditionEgressCompositeErrorDrop = 27, /* Packet marked for drop due to parity
                                           error from memories or ISM lookup
                                           fail */
    bcmOamConditionEgressSplitHorizonDrop = 28, /* Packet marked for drop due to split
                                           horizon check. This is done by
                                           enabling pruning for DVP network
                                           group via
                                           bcm_switch_network_group_config_set
                                           with
                                           BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE */
    bcmOamConditionEgressSVPRemovalDrop = 29, /* Packet marked for drop due to virtual
                                           port pruning - SVP = DVP and VP based
                                           pruning is enabled
                                           (DISABLE_VP_PRUNING is set to 0 in
                                           Source VP table) */
    bcmOamConditionEgressNonUCPruneDrop = 30, /* Non-unicast packet marked for drop
                                           due to drop condition set in egress
                                           next hop via bcm_mpls_port_add API
                                           with BCM_MPLS_PORT_DROP flag */
    bcmOamConditionCount = 31           /* Always Last. Not a usable value. */
} bcm_oam_condition_t;

/* OAM lookup types */
typedef enum bcm_oam_lookup_type_e {
    bcmOamLookupTypePort = 0,           /* Port based Ethernet OAM */
    bcmOamLookupTypeInnerVlan = 1,      /* Inner Vlan based Ethernet OAM */
    bcmOamLookupTypeOuterVlan = 2,      /* Outer Vlan based Ethernet OAM */
    bcmOamLookupTypeOuterPlusInnerVlan = 3, /* Outer+inner vlan based Ethernet OAM */
    bcmOamLookupTypeVirtualPort = 4,    /* VP based Ethernet OAM */
    bcmOamLookupTypeOuterVlanPassive = 5, /* Outer Vlan based passive Ethernet OAM */
    bcmOamLookupTypeMplsSection = 6,    /* MPLS section OAM */
    bcmOamLookupTypeMplsLsp = 7,        /* MPLS LSP OAM */
    bcmOamLookupTypeMplsPw = 8,         /* MPLS Pseudo-wire OAM */
    bcmOamLookupTypeEgressPort = 9,     /* Egress Port based Ethernet OAM */
    bcmOamLookupTypeEgressInnerVlan = 10, /* Egress inner VLAN based Ethernet OAM */
    bcmOamLookupTypeEgressOuterVlan = 11, /* Egress Outer Vlan based Ethernet OAM */
    bcmOamLookupTypeEgressOuterPlusInnerVlan = 12, /* Egress outer+inner VLAN based Eth-oam */
    bcmOamLookupTypeEgressVirtualPort = 13, /* Egress VP based Ethernet OAM */
    bcmOamLookupTypeEgressOuterVlanPassive = 14, /* Egress outer Vlan based passive
                                           Eth-OAM */
    bcmOamLookupTypeEgressMplsSection = 15, /* Egress MPLS section OAM */
    bcmOamLookupTypeEgressMplsLsp = 16, /* Egress MPLS LSP OAM */
    bcmOamLookupTypeEgressMplsPw = 17,  /* Egress MPLS Pseudo-wire OAM */
    bcmOamLookupTypeCount = 18          /* Always Last. Not a usable value. */
} bcm_oam_lookup_type_t;

/* MPLS LM DM (RFC 6374) opcode types */
typedef enum bcm_oam_mpls_lm_dm_opcode_type_e {
    bcmOamMplsLmDmOpcodeTypeLm = 0,     /* RFC 6374 Loss Measurement packet */
    bcmOamMplsLmDmOpcodeTypeDm = 1,     /* RFC6374 Delay Measurement Packet */
    bcmOamMplsLmDmOpcodeTypeIlm = 2,    /* RFC6374 Inferred Loss Measurement
                                           Packet */
    bcmOamMplsLmDmOpcodeTypeLmPlusDm = 3, /* RFC 6374 LM+DM packet */
    bcmOamMplsLmDmOpcodeTypeIlmPlusDm = 4, /* RFC 6374 ILM+DM packet */
    bcmOamMplsLmDmOpcodeTypeMax = 5     /* Max number of opcodes. Not an usable
                                           value. */
} bcm_oam_mpls_lm_dm_opcode_type_t;

/* MPLS LM DM (RFC 6374) counter types */
typedef enum bcm_oam_lm_counter_type_e {
    bcmOamLmCounterTypePkt = 0,     /* Counts packets */
    bcmOamLmCounterTypeByte = 1,    /* Counts bytes */
    bcmOamLmCounterTypeCount = 2    /* Always the last value. Not usable. */
} bcm_oam_lm_counter_type_t;

/* MPLS LM DM (RFC 6374) counter sizes */
typedef enum bcm_oam_lm_counter_size_e {
    bcmOamLmCounterSize32Bit = 0,   /* Counter size is 32 bit */
    bcmOamLmCounterSize64Bit = 1,   /* Counter size is 64 bit */
    bcmOamLmCounterSizeCount = 2    /* Always the last value. Not usable. */
} bcm_oam_lm_counter_size_t;

/* OAM TLV pattern type */
typedef enum bcm_oam_tlv_pattern_type_e {
    bcmOamTlvTypeNone = 0,              /* No TLV */
    bcmOamTlvTypeData = 1,              /* Type = Data (3) */
    bcmOamTlvTypeTestPrbsWithCRC = 2,   /* Type = Test (32), pattern type is
                                           PRBS with CRC */
    bcmOamTlvTypeTestPrbsWithoutCRC = 3, /* Type = Test (32), pattern type is
                                           PRBS without CRC */
    bcmOamTlvTypeTestNullWithCRC = 4,   /* Type = Test (32), pattern type is
                                           NULL with CRC */
    bcmOamTlvTypeTestNullWithoutCRC = 5, /* Type = Test (32), pattern type is
                                           NULL without CRC */
    bcmOamTlvTypeCount = 6 
} bcm_oam_tlv_pattern_type_t;

/* OAM event callback flags */
#define BCM_OAM_EVENT_FLAGS_MULTIPLE    0x01       /* Indicates that more than
                                                      one event of the given
                                                      type has occurred. */

typedef struct bcm_oam_event_types_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(bcmOAMEventCount)]; 
} bcm_oam_event_types_t;

typedef struct bcm_oam_conditions_s {
    SHR_BITDCL cbits[_SHR_BITDCLSIZE(bcmOamConditionCount)]; 
} bcm_oam_conditions_t;

typedef struct bcm_oam_lookup_types_s {
    SHR_BITDCL tbits[_SHR_BITDCLSIZE(bcmOamLookupTypeCount)]; 
} bcm_oam_lookup_types_t;

#define BCM_OAM_EVENT_TYPE_SET(_event_types, _event_type)  SHR_BITSET(((_event_types).w), (_event_type)) 

#define BCM_OAM_EVENT_TYPE_GET(_event_types, _event_type)  SHR_BITGET(((_event_types).w), (_event_type)) 

#define BCM_OAM_EVENT_TYPE_CLEAR(_event_types, _event_type)  SHR_BITCLR(((_event_types).w), (_event_type)) 

#define BCM_OAM_EVENT_TYPE_SET_ALL(_event_types)  SHR_BITSET_RANGE(((_event_types).w), 0, bcmOAMEventCount) 

#define BCM_OAM_EVENT_TYPE_CLEAR_ALL(_event_types)  SHR_BITCLR_RANGE(((_event_types).w), 0, bcmOAMEventCount) 

/* OAM Port Status TLV values */
#define BCM_OAM_PORT_TLV_BLOCKED    0x1        /* Port is blocked */
#define BCM_OAM_PORT_TLV_UP         0x2        /* Port is ready to pass data */

/* OAM Interface Status TLV Values */
#define BCM_OAM_INTERFACE_TLV_UP            0x1        /* Interface is ready to
                                                          pass packets */
#define BCM_OAM_INTERFACE_TLV_DOWN          0x2        /* Interface is down */
#define BCM_OAM_INTERFACE_TLV_TESTING       0x3        /* Interface is in some
                                                          test mode */
#define BCM_OAM_INTERFACE_TLV_UNKNOWN       0x4        /* Interface is in an
                                                          unknown state */
#define BCM_OAM_INTERFACE_TLV_DORMANT       0x5        /* Interface is in a
                                                          pending state */
#define BCM_OAM_INTERFACE_TLV_NOTPRESENT    0x6        /* Interface is down
                                                          because of hardware
                                                          component */
#define BCM_OAM_INTERFACE_TLV_LLDOWN        0x7        /* Interface is down due
                                                          to state of
                                                          lower-layer interfaces */

/* OAM group ID type */
typedef int bcm_oam_group_t;

/* 
 * OAM group object.  This is equivalent to an 802.1ag Maintenance
 * Association (MA).
 */
typedef struct bcm_oam_group_info_s {
    uint32 flags; 
    bcm_oam_group_t id; 
    uint8 name[BCM_OAM_GROUP_NAME_LENGTH]; /* This is equivalent to the Maintenance
                                           Association ID (MAID) in 802.1ag. 
                                           Note that this is not a
                                           null-terminated string but an array
                                           of bytes.  To avoid memory
                                           corruption, do not use string copy to
                                           populate this field. */
    uint8 rx_name[BCM_OAM_GROUP_NAME_LENGTH]; /* Additional MAID used for received OAM
                                           packets. It is used when the flag
                                           BCM_OAM_GROUP_RX_NAME is set.  When
                                           used, the received MAID values are
                                           compared to this MAID instead of the
                                           'name' field. */
    uint32 faults;                      /* Fault flags */
    uint32 persistent_faults;           /* Persistent fault flags */
    uint32 clear_persistent_faults;     /* Persistent fault flags to clear on a
                                           'get' */
    bcm_oam_group_fault_alarm_defect_priority_t lowest_alarm_priority; /* Generate fault alarm for this
                                           maintenance endpoint when defects of
                                           greater than or equal to this
                                           priority value are detected on this
                                           maintenance endpoint */
    int group_name_index;               /* Pointer to first extended data entry */
} bcm_oam_group_info_t;

/* OAM endpoint ID type */
typedef int bcm_oam_endpoint_t;

/*  OAM additional data for mpls network */
typedef struct bcm_oam_mpls_network_info_s {
    uint8 function_type;    /* FFD:0x07 or CV:0x01 */
    bcm_ip6_t lsr_id; 
    uint32 lsp_id; 
} bcm_oam_mpls_network_info_t;

/* OAM action profile encoding according type */
typedef int bcm_oam_profile_t;

/* 
 * Type of default memory that should be allocated for endpoint
 * SelfContained - Full entry in endpoint area, LmDmOffloaded - Full
 * entry in extra data area, ShortEntry - Short entry in endpoint area.
 */
typedef enum bcm_oam_endpoint_memory_type_e {
    bcmOamEndpointMemoryTypeSelfContained = 0, 
    bcmOamEndpointMemoryTypeLmDmOffloadedEntry = 1, 
    bcmOamEndpointMemoryTypeShortEntry = 2 
} bcm_oam_endpoint_memory_type_t;

/* Signal degradation indication. */
typedef enum bcm_oam_endpoint_signal_e {
    bcmOamEndpointSignalOk = 0, 
    bcmOamEndpointSignalError = 1 
} bcm_oam_endpoint_signal_t;

/* 
 * OAM endpoint object.  This is equivalent to an 802.1ag Maintenance
 * Endpoint (MEP).
 */
typedef struct bcm_oam_endpoint_info_s {
    uint32 flags; 
    uint32 flags2; 
    uint32 opcode_flags;                /* OAM opcode flags */
    uint32 lm_flags;                    /* OAM loss measurment flags */
    bcm_oam_endpoint_t id; 
    bcm_oam_endpoint_type_t type; 
    bcm_oam_group_t group;              /* The ID of the group to which this
                                           endpoint belongs */
    uint16 name;                        /* A unique identifier of the endpoint
                                           within the group.  This is equivalent
                                           to the MEPID in 802.1ag. */
    bcm_oam_endpoint_t local_id;        /* Used by remote endpoints only. */
    int level;                          /* This is equivalent to the Maintenance
                                           Domain Level (MDL) in 802.1ag. */
    int ccm_period;                     /* For local endpoints, this is the CCM
                                           transmission period in ms.  For
                                           remote endpoints, this is the period
                                           on which CCM timeout is based. */
    bcm_vlan_t vlan;                    /* The VLAN associated with this
                                           endpoint */
    bcm_vlan_t inner_vlan;              /* Configure with CVID for two-tag
                                           operation or set to 0 for one-tag */
    bcm_gport_t gport;                  /* The gport associated with this
                                           endpoint */
    bcm_gport_t tx_gport;               /* TX gport associated with this
                                           endpoint */
    int trunk_index;                    /* The trunk port index for this
                                           endpoint */
    bcm_if_t intf_id;                   /* Interface Identifier */
    bcm_mpls_label_t mpls_label;        /* The MPLS label associated with
                                           packets received from the Peer
                                           Endpoint */
    bcm_mpls_egress_label_t egress_label; /* The MPLS outgoing label information
                                           for the Local Endpoint */
    bcm_oam_mpls_network_info_t mpls_network_info; /* MPLS networks information */
    bcm_mac_t dst_mac_address;          /* The destination MAC address
                                           associated with this endpoint */
    bcm_mac_t src_mac_address;          /* The source MAC address associated
                                           with this endpoint */
    uint8 pkt_pri;                      /* Egress marking for outgoing CCMs */
    uint8 inner_pkt_pri;                /* Packet priority of inner encpsulated
                                           packet */
    uint16 inner_tpid;                  /* Tpid of inner encpsulated packet */
    uint16 outer_tpid;                  /* Tpid of outer encpsulated packet */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing CCMs. For
                                           DownMEPs, this represents the index
                                           from the base queue. For UpMEPs, this
                                           represents the internal priority
                                           itself. */
    uint8 cpu_qid;                      /* CPU queue for CCMs */
    uint8 pri_map[BCM_OAM_INTPRI_MAX];  /* Priority mapping for LM counter table */
    uint32 faults;                      /* Fault flags */
    uint32 persistent_faults;           /* Persistent fault flags */
    uint32 clear_persistent_faults;     /* Persistent fault flags to clear on a
                                           'get' */
    int ing_map;                        /* Ingress QoS map profile */
    int egr_map;                        /* Egress QoS map profile */
    uint8 ms_pw_ttl;                    /* TTL used for multi-segment pseudowire */
    uint8 port_state;                   /* Endpoint port status */
    uint8 interface_state;              /* Endpoint interface status */
    bcm_oam_vccv_type_t vccv_type;      /* VCCV pseudowire type */
    bcm_vpn_t vpn;                      /* VPN id */
    int lm_counter_base_id;             /* Counter id associated with the mep */
    uint8 lm_counter_if;                /* Counter interface associated with the
                                           mep */
    uint8 loc_clear_threshold;          /* Number of packets required to reset
                                           the Loss-of-Continuity status per end
                                           point */
    bcm_oam_timestamp_format_t timestamp_format; /* DM time stamp format -
                                           NTP/IEEE1588(PTP) */
    uint16 subport_tpid;                /* Tpid of subport Vlan in case of
                                           triple vlan encapsulation */
    bcm_gport_t remote_gport;           /* Local endpoint CCMs are forwarded to
                                           this remote unit for processing. */
    bcm_gport_t mpls_out_gport;         /* Out gport used for TX counting by BHH
                                           endpoints. */
    int sampling_ratio;                 /* 0 - No packets sampled to the CPU.
                                           1-8 - Count of packets (with events)
                                           that need to arrive before one is
                                           sampled to the CPU. */
    uint32 lm_payload_offset;           /* Offset in bytes from TOP of the MPLS
                                           label stack from where the payload
                                           starts for byte count computation. */
    uint32 lm_cos_offset;               /* Offset of the Label from top of the
                                           stack which gives the EXP value for
                                           deriving the COS value - valid values
                                           are 0/1/2. */
    bcm_oam_lm_counter_type_t lm_ctr_type; /* BYTE/PACKET. */
    bcm_oam_lm_counter_size_t lm_ctr_sample_size; /* Valid values are 32/64. */
    uint32 pri_map_id;                  /* OAM priority map id. */
    int lm_ctr_pool_id;                 /* The loss measurement counter pool id
                                           from which the counters should be
                                           allocated. Valid values are 0/1/2. */
    uint32 ccm_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx CCM packets */
    uint32 ccm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx CCM packets */
    uint8 ccm_tx_update_lm_counter_action[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter action (type
                                           bcm_oam_lm_counter_action_t) to be
                                           used by TX CCM packets */
    uint8 ccm_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx CCM packets */
    uint32 session_id;                  /* OAM session id for packet processing
                                           in BTE. In FP based OAM - This will
                                           indicate flex counter base ID */
    uint8 session_num_entries;          /* Number of entries that can result in
                                           same session ID. In FP based OAM -
                                           This will indicate number of flex
                                           counter entries corresponding to same
                                           OAM session */
    uint8 lm_count_profile;             /* LM count profile for this endpoint.
                                           It will be 1 or 0 for selecting one
                                           of the two OAM LM count profiles. */
    uint8 mpls_exp;                     /* EXP on which BHH will be running */
    bcm_oam_endpoint_t action_reference_id; /* Reference endpoint id whose actions
                                           will be used on the new created
                                           endpoint. Default value:
                                           BCM_OAM_ENDPOINT_INVALID. API call
                                           will set the parameter to
                                           BCM_OAM_ENDPOINT_INVALID */
    bcm_oam_profile_t acc_profile_id;   /* Used by accelerated endpoints. */
    bcm_oam_endpoint_memory_type_t endpoint_memory_type; /* Type of default endpoint memory */
    int punt_good_packet_period;        /* OAM good packets sampling period.
                                           Every punt_good_packet_period
                                           milliseconds, a single packet is
                                           punted to the CPU */
    uint16 extra_data_index;            /* Pointer to first extra data entry
                                           used per MEP for additional features */
    bcm_oam_endpoint_signal_t rx_signal; /* Expected Signal Indication */
    bcm_oam_endpoint_signal_t tx_signal; /* Transmitted Signal Indication */
} bcm_oam_endpoint_info_t;

/* OAM loss object. */
typedef struct bcm_oam_loss_s {
    uint32 flags; 
    int loss_id;                        /* Oam Loss ID. */
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;       /* Endpoint ID of Remote MEP. */
    int period;                         /* Frequency of loss tx period in
                                           milliseconds. */
    int loss_threshold;                 /* Loss ratio (expressed in 100th of
                                           percent) which if exceeded will
                                           declare the period degraded with a
                                           Loss Measurement packet exception.  A
                                           value of -1 for Loss Measurement
                                           exception on all received packets. */
    int loss_nearend;                   /* Nearend loss (expressed in 100th of
                                           percent). A value os -1 if not
                                           available. */
    int loss_farend;                    /* Farend loss (expressed in 100th of
                                           percent). A value os -1 if not
                                           available. */
    uint32 loss_nearend_byte;           /* Nearend loss in bytes. */
    uint32 loss_farend_byte;            /* Farend loss in bytes. */
    uint32 loss_nearend_byte_upper;     /* Nearend loss in bytes (Upper 32 bits
                                           in 64 bit byte count mode). */
    uint32 loss_farend_byte_upper;      /* Farend loss in bytes (Upper 32 bits
                                           in 64 bit byte count mode). */
    int loss_nearend_max;               /* Nearend maximal loss. A value os -1
                                           if not available. Reset when read. */
    int loss_nearend_acc;               /* Nearend accumulated loss. A value os
                                           -1 if not available. Reset when read. */
    int loss_farend_max;                /* Farend maximal loss. A value os -1 if
                                           not available. Reset when read. */
    int loss_farend_acc;                /* Farend accumulated loss. A value os
                                           -1 if not available. Reset when read. */
    uint32 tx_nearend;                  /* Last local transmit frame count
                                           recorded at time of LMR */
    uint32 rx_nearend;                  /* Last local receive frame count
                                           recorded at time of LMR */
    uint32 tx_farend;                   /* Last peer transmit frame count
                                           recorded at time of LMR */
    uint32 rx_farend;                   /* Last peer receive frame count
                                           recorded at time of LMR */
    uint32 tx_nearend_byte;             /* Local transmit byte count */
    uint32 rx_nearend_byte;             /* Local receive byte count */
    uint32 tx_farend_byte;              /* Peer transmit byte count */
    uint32 rx_farend_byte;              /* Peer receive byte count */
    uint32 tx_nearend_byte_upper;       /* Local transmit byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 rx_nearend_byte_upper;       /* Local receive byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 tx_farend_byte_upper;        /* Peer transmit byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 rx_farend_byte_upper;        /* Peer receive byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint8 pkt_pri_bitmap;               /* Bitmap of packet priorities which
                                           should be counted for LM.  A value of
                                           zero is the equivalent of all ones. */
    uint8 pkt_dp_bitmap;                /* Bitmap of packet color or DP which
                                           should be counted for LM.  A value of
                                           zero is the equivalent of all ones. */
    uint8 pkt_pri;                      /* Egress marking for outgoing LM
                                           messages */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing LM
                                           messages */
    bcm_gport_t gport;                  /* For CEP only; may return a diverged
                                           LP */
    uint32 rx_oam_packets;              /* Count of OAM packets received by this
                                           endpoint */
    uint32 tx_oam_packets;              /* Count of OAM packets transmitted by
                                           this endpoint */
    bcm_mac_t peer_da_mac_address;      /* MAC DA in LMM injection in case peer
                                           endpoint is not configured in
                                           remote_id */
    uint32 lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* Base index of LM Counter group
                                           (returned during endpoint create) of
                                           the counter to be stamped or
                                           incremented */
    uint32 lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter offset from the base */
    int lm_counter_action[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter action (type
                                           bcm_oam_lm_counter_action_t) */
    uint32 lm_counter_byte_offset[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter byte offset for increment */
    uint32 lm_counter_size;             /* Valid index count in LM Counter array */
    uint32 pm_id;                       /* Performance measurement ID in BTE */
    int measurement_period;             /* Slr measurement period in
                                           milliseconds. */
    uint32 slm_counter_base_id;         /* Must be even. Two counters are used,
                                           given counter for RX and given
                                           counter plus 1 for TX */
    uint32 slm_counter_core_id;         /* Slm counter core ID. */
} bcm_oam_loss_t;

/* OAM TLV object. */
typedef struct bcm_oam_tlv_s {
    bcm_oam_tlv_pattern_type_t tlv_type; 
    int tlv_length;                     /* Length of the value field */
    uint32 four_byte_repeatable_pattern; /* used when Type = Data (3) */
} bcm_oam_tlv_t;

/* OAM delay object. */
typedef struct bcm_oam_delay_s {
    uint32 flags; 
    int delay_id;                       /* Oam Delay ID. */
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;       /* Endpoint ID of Remote MEP. */
    int period;                         /* Frequency of delay tx period in
                                           milliseconds. Zero indicates single
                                           shot, -1 indicates disabling delay
                                           packets transmission */
    bcm_time_spec_t delay_min;          /* Minimal recorded delay to remote
                                           peer. Reset when read. */
    bcm_time_spec_t delay_max;          /* Maximul recorded delay to remote
                                           peer. Reset when read. */
    bcm_time_spec_t delay;              /* Last recorded delay to remote peer. */
    bcm_time_spec_t txf;                /* Time of transmit in forward
                                           direction. */
    bcm_time_spec_t rxf;                /* Time of reception in forward
                                           direction. */
    bcm_time_spec_t txb;                /* Time of transmit in backward
                                           direction. */
    bcm_time_spec_t rxb;                /* Time of reception in backward
                                           direction. */
    uint8 pkt_pri;                      /* Egress marking for outgoing DM
                                           messages */
    uint8 pkt_pri_bitmap;               /* Bitmap of packet priorities which
                                           should be measured for DM. A value of
                                           zero is the equivalent of all ones. */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing DM
                                           messages */
    uint32 rx_oam_packets;              /* Count of OAM packets received by this
                                           endpoint */
    uint32 tx_oam_packets;              /* Count of OAM packets transmitted by
                                           this endpoint */
    bcm_oam_timestamp_format_t timestamp_format; /* OAM timestamp type */
    bcm_mac_t peer_da_mac_address;      /* MAC DA in DMM injection in case peer
                                           endpoint is not configured in
                                           remote_id */
    uint32 pm_id;                       /* Performance measurement ID in BTE */
    uint32 dm_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx DM packets */
    uint32 dm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx DM packets */
    uint32 dm_tx_update_lm_counter_byte_offset[BCM_OAM_LM_COUNTER_MAX]; /* Byte offset in the Tx DM packet from
                                           which to increment the LM byte
                                           counter */
    uint8 dm_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx DM packets */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_DM]; /* TLV contents of DM */
    bcm_time_spec_t delay_min_near_end; /* Minimal recorded delay to local peer.
                                           Reset when read. */
    bcm_time_spec_t delay_max_near_end; /* Maximul recorded delay to local peer.
                                           Reset when read. */
    bcm_time_spec_t delay_near_end;     /* Last recorded delay to local peer. */
} bcm_oam_delay_t;

/* OAM AIS (alarm indication signal) object. */
typedef struct bcm_oam_ais_s {
    uint32 flags; 
    int ais_id;                     /* OAM AIS ID. */
    bcm_oam_endpoint_t id;          /* Endpoint ID of Local MEP. */
    int period;                     /* Frequency of AIS tx period in
                                       milliseconds. */
    int level;                      /* MD level on outgoing frames. */
    bcm_mac_t peer_da_mac_address;  /* MAC DA in AIS injection. */
    uint8 pkt_pri;                  /* Egress marking for outgoing AIS messages. */
    uint8 drop_eligible;            /* Drop Eligibility for AIS. */
    bcm_vlan_t outer_vlan;          /* Outer vlan id for the AIS frame. */
    uint8 outer_pkt_pri;            /* Outer vlan packet prio for the AIS frame. */
    uint16 outer_tpid;              /* TPID of outer vlan for the AIS frame. */
    bcm_vlan_t inner_vlan;          /* Inner vlan id for the AIS frame. */
    uint8 inner_pkt_pri;            /* Inner vlan packet prio for the AIS frame. */
    uint16 inner_tpid;              /* TPID of inner vlan for the AIS frame. */
} bcm_oam_ais_t;

/* OAM TST TX object. */
typedef struct bcm_oam_tst_tx_s {
    bcm_oam_endpoint_t endpoint_id; /* Endpoint ID of Local MEP. */
    int gtf_id; 
    uint32 flags; 
    bcm_mac_t peer_da_mac_address;  /* MAC DA in TST injection. */
    bcm_oam_tlv_t tlv; 
} bcm_oam_tst_tx_t;

/* OAM TST RX object. */
typedef struct bcm_oam_tst_rx_s {
    bcm_oam_endpoint_t endpoint_id; /* Endpoint ID of Local MEP. */
    int ctf_id; 
    uint32 flags; 
    bcm_oam_tlv_t expected_tlv; 
    uint64 rx_count; 
    uint64 invalid_tlv_count;       /* Count of invalid packets received
                                       (incorrect TLV) */
    uint64 out_of_sequence;         /* Count of out of sequence packets */
} bcm_oam_tst_rx_t;

/* OAM Signal Degradation/Signal Fault  object. */
typedef struct bcm_oam_sd_sf_detection_s {
    bcm_oam_endpoint_t mep_id;          /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t rmep_id;         /* Endpoint ID of Remote MEP. */
    uint16 sd_set_threshold;            /* threshold of signal degradation
                                           indication */
    uint16 sf_set_threshold;            /* threshold of signal fault indication */
    uint16 sd_clear_threshold;          /* threshold of signal degradation clear */
    uint16 sf_clear_threshold;          /* threshold of signal fault clear */
    uint8 flags;                        /* flags use to set alert method,
                                           suppress and others */
    uint8 sd_indication;                /* signal degradation indication */
    uint8 sf_indication;                /* signal fault indication */
    uint16 sliding_window_length;       /* size of the sliding window (1-256) */
    uint16 sliding_window_cnt[BCM_OAM_MAX_NUM_SLINDING_WINDOWS]; /* 256 counters of per-period packets
                                           received */
    uint16 sum_cnt;                     /* Sum of the sliding window packets
                                           (read-only) */
} bcm_oam_sd_sf_detection_t;

/* OAM Y.1711 alarm object. */
typedef struct bcm_oam_y_1711_alarm_s {
    bcm_oam_endpoint_t mep_id;      /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t rmep_id;     /* Endpoint ID of Remote MEP. */
    uint8 d_excess_threshold;       /* used for Y.1711 defects */
    uint8 indicator_low_threshold;  /* Y.1711 indicator control low threshold */
    uint8 indicator_high_threshold; /* Y.1711 indicator control high threshold */
    uint8 flags;                    /* flags use to set alert mothod and others */
    uint8 d_excess;                 /* excess detect */
    uint8 d_mismatch;               /* mismatch detect */
    uint8 d_mismerge;               /* mismerge detect */
} bcm_oam_y_1711_alarm_t;

/* OAM loopback discovery record. */
typedef struct bcm_oam_loopback_discovery_record_s {
    uint32 flags; 
    bcm_oam_endpoint_t id;  /* Endpoint ID of discovered MEP/MIP. */
    int ttl;                /* TTL value to used to discover MEP/MIP. */
    uint16 name;            /* Discovered MEP ID */
} bcm_oam_loopback_discovery_record_t;

/* OAM loopback object. */
typedef struct bcm_oam_loopback_s {
    uint32 flags; 
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;       /* Endpoint ID of Remote MEP. */
    int period;                         /* Frequency of loopback tx period in
                                           milliseconds or pps (packets/sec)
                                           depending on flags. Zero indicates
                                           single shot, -1 indicates disabling
                                           loopback packets transmission */
    int ttl;                            /* TTL value to use in loopback message. */
    bcm_oam_loopback_discovery_record_t discovered_me; /* Last ME discovered using loopback
                                           discovery. */
    uint32 rx_count;                    /* Count of received packets since
                                           loopback add or last get. */
    uint32 tx_count;                    /* Count of transmitted packets since
                                           loopback add or last get. */
    uint32 drop_count;                  /* Count of dropped packets since
                                           loopback add or last get. */
    uint32 unexpected_response;         /* Count of unexpected response packets
                                           since loopback add or last get. */
    uint32 out_of_sequence;             /* Count of out of sequence packets. */
    uint32 local_mipid_missmatch;       /* Count of local mipid missmatch
                                           packets since loopback add or last
                                           get. */
    uint32 remote_mipid_missmatch;      /* Count of remote mipid missmatch
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_target_mep_tlv;      /* Count of invalid target mep TLV
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_mep_tlv_subtype;     /* Count of invalid mep TLV subtype
                                           packets since loopback add or last
                                           get. */
    uint32 invalid_tlv_offset;          /* Count of invalid TLV offset packets
                                           since loopback add or last get. */
    bcm_mac_t peer_da_mac_address;      /* MAC DA in loopback injection in case
                                           remote_id is not specified */
    int num_tlvs;                       /* Number of TLV */
    int invalid_tlv_count;              /* Count of invalid packets received
                                           (incorrect TLV) */
    int ctf_id;                         /* Ctf id number */
    int gtf_id;                         /* Gtf id number */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_LBM]; /* TLV contents of LBM */
    uint32 lb_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx LB packets */
    uint32 lb_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx LB packets */
    uint8 lb_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx LB packets */
    bcm_cos_t int_pri;                  /* Optional: priority fields on ITMH
                                           header */
    uint8 pkt_pri;                      /* Optional: priority fields on outer
                                           VLAN (PCP + DEI). -1 will ignore this
                                           field */
    uint8 inner_pkt_pri;                /* Optional: priority fields on inner
                                           VLAN (PCP + DEI) , when applicable.
                                           -1 will ignore this field */
} bcm_oam_loopback_t;

/* OAM PSC object. */
typedef struct bcm_oam_psc_s {
    uint32 flags; 
    bcm_oam_endpoint_t id;          /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;   /* Endpoint ID of Remote MEP. */
    int version;                    /* Version number of message. */
    int request;                    /* Request field. */
    int pt;                         /* Protection type. */
    uint8 fpath;                    /* The path reporting failure. */
    uint8 path;                     /* The currently active path. */
    int burst_rate;                 /* Burst interval in ms (suggested 3 ms). */
    int slow_rate;                  /* Slow or normal interval in ms (suggested
                                       5000 ms). */
    int tlv_length;                 /* Length of optional TLV. */
    uint8 tlv;                      /* Optional TLV(s) of tlv_length. */
} bcm_oam_psc_t;

/* OAM PW Status object. */
typedef struct bcm_oam_pw_status_s {
    uint32 flags; 
    bcm_oam_endpoint_t id;          /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;   /* Endpoint ID of Remote MEP. */
    int burst_rate;                 /* Burst interval in ms. */
    int refresh;                    /* Refresh timer is copied into message and
                                       sets the local transmit interval in
                                       seconds. */
    int ack;                        /* Copied into A field. */
    uint8 tlv_length;               /* Length of optional TLV. */
    uint8 tlv;                      /* Optional TLV(s) of tlv_length. */
} bcm_oam_pw_status_t;

/* OAM CSF (alarm indication signal) object. */
typedef struct bcm_oam_csf_s {
    uint32 flags;                       /* CSF flags. */
    uint32 type;                        /* CSF Type value in PDU. */
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    int period;                         /* CSF Tx period in milliseconds.
                                           Supported values are 1000 and 60000 */
    uint8 pkt_pri;                      /* Egress marking for outgoing CSF
                                           messages. */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing CSF
                                           messages */
    uint32 csf_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* The base index of the LM counter to
                                           be updated by Tx CSF packets */
    uint32 csf_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* Offset to the Base LM counter Id to
                                           be incremented by Tx CSF packets */
    uint8 csf_tx_update_lm_counter_size; /* Number of LM counters to be
                                           incremented by Tx CSF packets */
} bcm_oam_csf_t;

typedef struct bcm_oam_endpoint_action_s {
    uint32 flags; 
    SHR_BITDCL opcode_bmp[_SHR_BITDCLSIZE(256)]; 
    SHR_BITDCL opcode_actions[_SHR_BITDCLSIZE(bcmOAMActionCount)]; 
    bcm_gport_t destination;            /* For remote handling. Can also be drop */
    bcm_gport_t destination2;           /* Another destination when required for
                                           example: MIP requires 2 destination
                                           traps, one for each direction */
} bcm_oam_endpoint_action_t;

#define BCM_OAM_ACTION_SET(_action, _action_type)  SHR_BITSET(((_action).opcode_actions), (_action_type)) 

#define BCM_OAM_ACTION_GET(_action, _action_type)  SHR_BITGET(((_action).opcode_actions), (_action_type)) 

#define BCM_OAM_ACTION_CLEAR(_action, _action_type)  SHR_BITCLR(((_action).opcode_actions), (_action_type)) 

#define BCM_OAM_ACTION_CLEAR_ALL(_action)  SHR_BITCLR_RANGE(((_action).opcode_actions), 0, bcmOAMActionCount) 

#define BCM_OAM_OPCODE_SET(_action, _opcode)  SHR_BITSET(((_action).opcode_bmp), (_opcode)) 

#define BCM_OAM_OPCODE_GET(_action, _opcode)  SHR_BITGET(((_action).opcode_bmp), (_opcode)) 

#define BCM_OAM_OPCODE_CLEAR(_action, _opcode)  SHR_BITCLR(((_action).opcode_bmp), (_opcode)) 

#define BCM_OAM_OPCODE_CLEAR_ALL(_action)  SHR_BITCLR_RANGE(((_action).opcode_bmp), 0, 256) 

#define BCM_OAM_LOOKUP_TYPE_SET(_type, _lookup_type)  SHR_BITSET(((_type).tbits), _lookup_type) 

#define BCM_OAM_LOOKUP_TYPE_GET(_type, _lookup_type)  SHR_BITGET(((_type).tbits), _lookup_type) 

#define BCM_OAM_LOOKUP_TYPE_CLEAR(_type, _lookup_type)  SHR_BITCLR(((_type).tbits), _lookup_type) 

#define BCM_OAM_LOOKUP_TYPE_CLEAR_ALL(_type)  SHR_BITCLR_RANGE(((_type).tbits), 0, bcmOamLookupTypeCount) 

#define BCM_OAM_CONDITION_SET(_condition, _drop)  SHR_BITSET(((_condition).cbits), _drop) 

#define BCM_OAM_CONDITION_GET(_condition, _drop)  SHR_BITGET(((_condition).cbits), _drop) 

#define BCM_OAM_CONDITION_CLEAR(_condition, _drop)  SHR_BITCLR(((_condition).cbits), _drop) 

#define BCM_OAM_CONDITION_CLEAR_ALL(_condition)  SHR_BITCLR_RANGE(((_condition).cbits), 0, bcmOamConditionCount) 

/* Callback function type for OAM group traversal */
typedef int (*bcm_oam_group_traverse_cb)(
    int unit, 
    bcm_oam_group_info_t *group_info, 
    void *user_data);

/* Callback function type for OAM endpoint traversal */
typedef int (*bcm_oam_endpoint_traverse_cb)(
    int unit, 
    bcm_oam_endpoint_info_t *endpoint_info, 
    void *user_data);

/* Callback function type for OAM event handling */
typedef int (*bcm_oam_event_cb)(
    int unit, 
    uint32 flags, 
    bcm_oam_event_type_t event_type, 
    bcm_oam_group_t group, 
    bcm_oam_endpoint_t endpoint, 
    void *user_data);

typedef struct bcm_oam_performance_event_data_s {
    uint32 delta_FCB;       /* TXFcb - RXFcb. Used for performance events of
                               type bcmOAMEventEndpointLMStatistics. */
    uint32 delta_FCf;       /* TXFcf - RXFcb. Used for performance events of
                               type bcmOAMEventEndpointLMStatistics. */
    uint64 last_delay;      /* Used for performance events of type
                               bcmOAMEventEndpointDMStatistics. */
    uint64 last_delay_near; /* Used for performance events of type one way
                               bcmOAMEventEndpointDMStatistics in normal mode. */
} bcm_oam_performance_event_data_t;

/* Initialize a bcm_oam_performance_event_data_t structure. */
extern void bcm_oam_performance_event_data_t_init(
    bcm_oam_performance_event_data_t *performance_event_data_ptr);

/* Callback function type for OAM performance event handling */
typedef int (*bcm_oam_performance_event_cb)(
    int unit, 
    bcm_oam_event_type_t event_type, 
    bcm_oam_group_t group, 
    bcm_oam_endpoint_t endpoint, 
    bcm_oam_performance_event_data_t *event_data, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the BCM OAM subsystem. */
extern int bcm_oam_init(
    int unit);

/* Shut down the BCM OAM subsystem. */
extern int bcm_oam_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_group_info_t structure. */
extern void bcm_oam_group_info_t_init(
    bcm_oam_group_info_t *group_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace an OAM group object. */
extern int bcm_oam_group_create(
    int unit, 
    bcm_oam_group_info_t *group_info);

/* Get an OAM group object. */
extern int bcm_oam_group_get(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info);

/* Destroy an OAM group object. */
extern int bcm_oam_group_destroy(
    int unit, 
    bcm_oam_group_t group);

/* Destroy all OAM group objects. */
extern int bcm_oam_group_destroy_all(
    int unit);

/* Traverse the set of OAM groups. */
extern int bcm_oam_group_traverse(
    int unit, 
    bcm_oam_group_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_endpoint_info_t structure. */
extern void bcm_oam_endpoint_info_t_init(
    bcm_oam_endpoint_info_t *endpoint_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace an OAM endpoint object. */
extern int bcm_oam_endpoint_create(
    int unit, 
    bcm_oam_endpoint_info_t *endpoint_info);

/* Get an OAM endpoint object. */
extern int bcm_oam_endpoint_get(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info);

/* Destroy an OAM endpoint object. */
extern int bcm_oam_endpoint_destroy(
    int unit, 
    bcm_oam_endpoint_t endpoint);

/* Destroy all OAM endpoint objects. */
extern int bcm_oam_endpoint_destroy_all(
    int unit, 
    bcm_oam_group_t group);

/* Traverse the set of OAM endpoints in a group. */
extern int bcm_oam_endpoint_traverse(
    int unit, 
    bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_loss_t structure. */
extern void bcm_oam_loss_t_init(
    bcm_oam_loss_t *loss_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM loss object */
extern int bcm_oam_loss_add(
    int unit, 
    bcm_oam_loss_t *loss_ptr);

/* Get an OAM loss object */
extern int bcm_oam_loss_get(
    int unit, 
    bcm_oam_loss_t *loss_ptr);

/* Delete an OAM loss object */
extern int bcm_oam_loss_delete(
    int unit, 
    bcm_oam_loss_t *loss_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_delay_t structure. */
extern void bcm_oam_delay_t_init(
    bcm_oam_delay_t *delay_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM delay object */
extern int bcm_oam_delay_add(
    int unit, 
    bcm_oam_delay_t *delay_ptr);

/* Get an OAM delay object */
extern int bcm_oam_delay_get(
    int unit, 
    bcm_oam_delay_t *delay_ptr);

/* Delete an OAM delay object */
extern int bcm_oam_delay_delete(
    int unit, 
    bcm_oam_delay_t *delay_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_ais_t structure. */
extern void bcm_oam_ais_t_init(
    bcm_oam_ais_t *ais_ptr);

/* Initialize a bcm_oam_endpoint_action_t structure. */
extern void bcm_oam_endpoint_action_t_init(
    bcm_oam_endpoint_action_t *action_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM ais object */
extern int bcm_oam_ais_add(
    int unit, 
    bcm_oam_ais_t *ais_ptr);

/* Get an OAM ais object */
extern int bcm_oam_ais_get(
    int unit, 
    bcm_oam_ais_t *ais_ptr);

/* Delete an OAM ais object */
extern int bcm_oam_ais_delete(
    int unit, 
    bcm_oam_ais_t *ais_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_tst_rx_t structure. */
extern void bcm_oam_tst_rx_t_init(
    bcm_oam_tst_rx_t *tst_rx_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM TST rx object */
extern int bcm_oam_tst_rx_add(
    int unit, 
    bcm_oam_tst_rx_t *tst_ptr);

/* Get an OAM TST rx object */
extern int bcm_oam_tst_rx_get(
    int unit, 
    bcm_oam_tst_rx_t *tst_ptr);

/* Delete an OAM TST rx object */
extern int bcm_oam_tst_rx_delete(
    int unit, 
    bcm_oam_tst_rx_t *tst_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_tst_tx_t structure. */
extern void bcm_oam_tst_tx_t_init(
    bcm_oam_tst_tx_t *tst_tx_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM TST tx object */
extern int bcm_oam_tst_tx_add(
    int unit, 
    bcm_oam_tst_tx_t *tst_ptr);

/* Get an OAM TST tx object */
extern int bcm_oam_tst_tx_get(
    int unit, 
    bcm_oam_tst_tx_t *tst_ptr);

/* Delete an OAM TST tx object */
extern int bcm_oam_tst_tx_delete(
    int unit, 
    bcm_oam_tst_tx_t *tst_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_sd_sf_detection_t structure. */
extern void bcm_oam_sd_sf_detection_t_init(
    bcm_oam_sd_sf_detection_t *sd_sf_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM SD/SF detection object */
extern int bcm_oam_sd_sf_detection_add(
    int unit, 
    bcm_oam_sd_sf_detection_t *sd_sf_ptr);

/* Get an OAM SD/SF detection object */
extern int bcm_oam_sd_sf_detection_get(
    int unit, 
    bcm_oam_sd_sf_detection_t *sd_sf_ptr);

/* Delete an OAM SD/SF detection object */
extern int bcm_oam_sd_sf_detection_delete(
    int unit, 
    bcm_oam_sd_sf_detection_t *sd_sf_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a Y.1711 Alarm structure */
extern void bcm_oam_y_1711_alarm_t_init(
    bcm_oam_y_1711_alarm_t *alarm_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM y_1711 alarm object */
extern int bcm_oam_y_1711_alarm_add(
    int unit, 
    bcm_oam_y_1711_alarm_t *alarm_ptr);

/* Get an OAM y_1711 alarm object */
extern int bcm_oam_y_1711_alarm_get(
    int unit, 
    bcm_oam_y_1711_alarm_t *alarm_ptr);

/* Delete an OAM Y.1711 alarm object */
extern int bcm_oam_y_1711_alarm_delete(
    int unit, 
    bcm_oam_y_1711_alarm_t *alarm_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an OAM PSC structure */
extern void bcm_oam_psc_t_init(
    bcm_oam_psc_t *psc_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM PSC object */
extern int bcm_oam_psc_add(
    int unit, 
    bcm_oam_psc_t *psc_ptr);

/* Get an OAM PSC object */
extern int bcm_oam_psc_get(
    int unit, 
    bcm_oam_psc_t *psc_ptr);

/* Delete an OAM PSC object */
extern int bcm_oam_psc_delete(
    int unit, 
    bcm_oam_psc_t *psc_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize an OAM PW Status structure */
extern void bcm_oam_pw_status_t_init(
    bcm_oam_pw_status_t *pw_status_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM PW Status object */
extern int bcm_oam_pw_status_add(
    int unit, 
    bcm_oam_pw_status_t *pw_status_ptr);

/* Get an OAM PW Status object */
extern int bcm_oam_pw_status_get(
    int unit, 
    bcm_oam_pw_status_t *pw_status_ptr);

/* Delete an OAM PW Status object */
extern int bcm_oam_pw_status_delete(
    int unit, 
    bcm_oam_pw_status_t *pw_status_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_csf_t structure. */
extern void bcm_oam_csf_t_init(
    bcm_oam_csf_t *csf_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM csf object */
extern int bcm_oam_csf_add(
    int unit, 
    bcm_oam_csf_t *csf_ptr);

/* Get an OAM csf object */
extern int bcm_oam_csf_get(
    int unit, 
    bcm_oam_csf_t *csf_ptr);

/* Delete an OAM csf object */
extern int bcm_oam_csf_delete(
    int unit, 
    bcm_oam_csf_t *csf_ptr);

/* Register a handler for OAM events. */
extern int bcm_oam_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb, 
    void *user_data);

/* Unregister a handler for OAM events. */
extern int bcm_oam_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_event_cb cb);

/* Register a handler for OAM performance events. */
extern int bcm_oam_performance_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_performance_event_cb cb, 
    void *user_data);

/* Unregister a handler for OAM performance events. */
extern int bcm_oam_performance_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_performance_event_cb cb);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_loopback_t structure. */
extern void bcm_oam_loopback_t_init(
    bcm_oam_loopback_t *loopback_ptr);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add an OAM loopback object */
extern int bcm_oam_loopback_add(
    int unit, 
    bcm_oam_loopback_t *loopback_ptr);

/* Get an OAM loopback object */
extern int bcm_oam_loopback_get(
    int unit, 
    bcm_oam_loopback_t *loopback_ptr);

/* Delete an OAM loopback object */
extern int bcm_oam_loopback_delete(
    int unit, 
    bcm_oam_loopback_t *loopback_ptr);

/* Set actions for different OAM opcodes. */
extern int bcm_oam_endpoint_action_set(
    int unit, 
    bcm_oam_endpoint_t id, 
    bcm_oam_endpoint_action_t *action);

/* Configure device-wide OAM control. */
extern int bcm_oam_control_set(
    int unit, 
    bcm_oam_control_type_t type, 
    uint64 arg);

/* Get device-wide OAM control value. */
extern int bcm_oam_control_get(
    int unit, 
    bcm_oam_control_type_t type, 
    uint64 *arg);

/* Set Ethernet Y1731 opcode profile mapping. */
extern int bcm_oam_opcode_map_set(
    int unit, 
    int opcode, 
    int profile);

/* Get Ethernet Y1731 opcode profile mapping. */
extern int bcm_oam_opcode_map_get(
    int unit, 
    int opcode, 
    int *profile);

/* Enable/Disable a drop condition for an OAM lookup type */
extern int bcm_oam_lookup_enable_set(
    int unit, 
    bcm_oam_lookup_type_t type, 
    bcm_oam_condition_t condition, 
    int enable);

/* Enable/Disable multiple drop conditions for OAM lookup types. */
extern int bcm_oam_lookup_enable_multi_set(
    int unit, 
    bcm_oam_lookup_types_t types, 
    bcm_oam_conditions_t conditions, 
    int enable);

/* Get the drop conditions that are ignored for an OAM lookup type */
extern int bcm_oam_lookup_enable_get(
    int unit, 
    bcm_oam_lookup_type_t type, 
    bcm_oam_conditions_t *condition);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * OAM report mode types. To be used as the arg parameter when calling
 * bcm_oam_control_set/get() with the type parameter set to
 * bcmOamControlReportMode.
 */
typedef enum bcm_oam_report_mode_type_e {
    bcmOamReportModeTypeNormal = 0,     /* Default report mode. */
    bcmOamReportModeTypeCompact = 1, 
    bcmOamReportModeTypeRaw = 2,        /* Report mode events will be disabled
                                           when report mode is set to raw. */
    bcmOamReportModeTypeCount = 3 
} bcm_oam_report_mode_type_t;

#define BCM_OAM_MAX_PM_PROFILE_BIN_EDGES    7          /* Maximum number of bin
                                                          edges in each PM
                                                          profile */

/* PM profile creation flags */
#define BCM_OAM_PM_PROFILE_REPLACE  0x0001     /* Replace an existing OAM PM
                                                  profile */
#define BCM_OAM_PM_PROFILE_WITH_ID  0x0002     /* Use the specified PM profile
                                                  ID */

/* OAM PM profile ID type */
typedef int bcm_oam_pm_profile_t;

/* PM profile info structure */
typedef struct bcm_oam_pm_profile_info_s {
    bcm_oam_pm_profile_t id; 
    uint32 flags; 
    uint32 bin_edges[BCM_OAM_MAX_PM_PROFILE_BIN_EDGES]; /* Bin edges */
} bcm_oam_pm_profile_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create or replace an OAM Performance monitoring bin profile. */
extern int bcm_oam_pm_profile_create(
    int unit, 
    bcm_oam_pm_profile_info_t *profile_info);

/* Delete an OAM Performance monitoring bin profile */
extern int bcm_oam_pm_profile_delete(
    int unit, 
    bcm_oam_pm_profile_t profile_id);

/* Delete all the OAM Performance monitoring bin profiles. */
extern int bcm_oam_pm_profile_delete_all(
    int unit);

/* Get an OAM Performance monitoring bin profile */
extern int bcm_oam_pm_profile_get(
    int unit, 
    bcm_oam_pm_profile_info_t *profile_info);

/* Attach a PM profile to an OAM endpoint */
extern int bcm_oam_pm_profile_attach(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    int profile_id);

/* Detach a PM profile from an endpoint */
extern int bcm_oam_pm_profile_detach(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    int profile_id);

/* Get the PM profile attached to an OAM endpoint */
extern int bcm_oam_pm_profile_attach_get(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    int *profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Callback function type for PM profiles traversal */
typedef int (*bcm_oam_pm_profile_traverse_cb)(
    int unit, 
    bcm_oam_pm_profile_info_t *profile_info, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the list of pm profiles using the callback function */
extern int bcm_oam_pm_profile_traverse(
    int unit, 
    bcm_oam_pm_profile_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* PM stats structure */
typedef struct bcm_oam_pm_stats_s {
    uint32 far_loss_min;                /* Minimum Frame loss at far-end */
    uint32 far_tx_min;                  /* Minimum Frame transmitted at far-end */
    uint32 far_loss_max;                /* Maximum Frame loss at far-end */
    uint32 far_tx_max;                  /* Maximum Frame transmitted at far-end */
    uint32 far_loss;                    /* Total far loss at far-end */
    uint32 near_loss_min;               /* Minimum Frame loss at near-end */
    uint32 near_tx_min;                 /* Minimum Frame transmitted at near-end */
    uint32 near_loss_max;               /* Maximum Frame loss at near-end */
    uint32 near_tx_max;                 /* Maximum Frame transmitted at near-end */
    uint32 near_loss;                   /* Total near loss at near-end */
    uint32 lm_tx_count;                 /* Count of LM packets transmitted from
                                           local node during sampling interval */
    uint32 DM_min;                      /* Minimum Frame delay */
    uint32 DM_max;                      /* Maximum Frame delay */
    uint32 DM_avg;                      /* Frame delay */
    uint32 dm_tx_count;                 /* Count of DM packets transmitted from
                                           local node during sampling interval */
    uint8 profile_id;                   /* Profile number associated with this
                                           session to measure the DM frames */
    uint32 bin_counters[BCM_OAM_MAX_PM_PROFILE_BIN_EDGES+1]; /* Bins Count of associated profile */
    uint32 lm_rx_count;                 /* Count of LM packets received in local
                                           node during sampling interval */
    uint32 dm_rx_count;                 /* Count of DM packets received in local
                                           node during sampling interval */
    uint32 far_total_tx_pkt_count;      /* Total Tx data packets at far-end */
    uint32 near_total_tx_pkt_count;     /* Total Tx data packets at near-end */
    uint32 flags;                       /* Flags related to PM processed stats */
} bcm_oam_pm_stats_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get the processed stats for a given endpoint */
extern int bcm_oam_pm_stats_get(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    bcm_oam_pm_stats_t *stats_ptr);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_OAM_PM_RAW_DATA_MAX_SAMPLES 10         /* Maximum number of samples
                                                      in each buffer in Raw
                                                      sampling mode */

/* PM raw counters structure. */
typedef struct bcm_oam_pm_raw_counter_s {
    uint32 tx_fcf;  /* Count of packets transmitted by local node */
    uint32 rx_fcf;  /* Count of packets received by remote node */
    uint32 tx_fcb;  /* Count of packets transmitted by remote node */
    uint32 rx_fcb;  /* Count of packets received by local node */
} bcm_oam_pm_raw_counter_t;

/* PM raw timestamp structure. */
typedef struct bcm_oam_pm_raw_timestamp_s {
    uint32 tx_tsf_secs;     /* Time stamp upper of DM packets transmitted by
                               local node */
    uint32 tx_tsf_nsecs;    /* Time stamp lower of DM packets transmitted by
                               local node */
    uint32 rx_tsf_secs;     /* Time stamp upper of DM packets received by remote
                               node */
    uint32 rx_tsf_nsecs;    /* Time stamp lower of DM packets received by remote
                               node */
    uint32 tx_tsb_secs;     /* Time stamp upper of DM packets transmitted by
                               remote node */
    uint32 tx_tsb_nsecs;    /* Time stamp lower of DM packets transmitted by
                               remote node */
    uint32 rx_tsb_secs;     /* Time stamp upper of DM packets received by local
                               node */
    uint32 rx_tsb_nsecs;    /* Time stamp lower of DM packets received by local
                               node */
} bcm_oam_pm_raw_timestamp_t;

/* PM raw samples structure. */
typedef struct bcm_oam_pm_raw_samples_s {
    int pm_id;                          /* Performance measurement ID in BTE */
    uint32 lm_count;                    /* Number of LM samples collected as
                                           part of this buffer */
    uint32 dm_count;                    /* Number of DM samples collected as
                                           part of this buffer */
    bcm_oam_pm_raw_counter_t raw_counter[BCM_OAM_PM_RAW_DATA_MAX_SAMPLES]; /* LM counters collected as part of this
                                           buffer */
    bcm_oam_pm_raw_timestamp_t raw_timestamp[BCM_OAM_PM_RAW_DATA_MAX_SAMPLES]; /* DM Timestamps collected as part of
                                           this buffer */
} bcm_oam_pm_raw_samples_t;

#define BCM_OAM_MAX_PM_ENDPOINTS    256        /* Maximum number of endpoints
                                                  allowed in Raw sampling mode */

/* PM raw buffer structure. */
typedef struct bcm_oam_pm_raw_buffer_s {
    uint32 seq_num;                     /* Seq No. of the data buffer */
    bcm_oam_pm_raw_samples_t raw_sample[BCM_OAM_MAX_PM_ENDPOINTS]; /* Raw data buffer */
} bcm_oam_pm_raw_buffer_t;

#define BCM_OAM_MAX_PM_BUFFERS  8          /* Maximum number of buffers allowed
                                              in Raw sampling mode */

/* PM raw data structure. */
typedef struct bcm_oam_pm_raw_data_s {
    uint32 pm_endpoint_count;           /* Active PM endpoints configured */
    uint8 read_index;                   /* Application read index */
    uint8 write_index;                  /* BTE write index */
    bcm_oam_pm_raw_buffer_t raw_buffer[BCM_OAM_MAX_PM_BUFFERS]; /* Raw data buffer */
} bcm_oam_pm_raw_data_t;

/* Callback function type for OAM event handling */
typedef int (*bcm_oam_pm_event_cb)(
    int unit, 
    bcm_oam_event_type_t event_type, 
    bcm_oam_pm_raw_data_t *raw_data, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register a callback for handling OAM PM events */
extern int bcm_oam_pm_event_register(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_pm_event_cb cb, 
    void *user_data);

/* Unregister a callback handling OAM PM events */
extern int bcm_oam_pm_event_unregister(
    int unit, 
    bcm_oam_event_types_t event_types, 
    bcm_oam_pm_event_cb cb);

/* Indicate SDK that the application has read buffer upto the read_index. */
extern int bcm_oam_pm_raw_data_read_done(
    int unit, 
    bcm_oam_event_types_t event_types, 
    uint32 read_index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Types of OAM protocols */
typedef enum bcm_oam_protocol_type_e {
    bcmOamProtocolEth = 0,      /* ETH-OAM Protocol */
    bcmOamProtocolBhh = 1,      /* BHH-OAM Protocol */
    bcmOamProtocolMplsLmDm = 2, /* MPLS LM/DM Protocol */
    bcmOamProtocolMax = 3       /* Max No. of Protcols.Not an usable
                                   value.Always at the end */
} bcm_oam_protocol_type_t;

/* Types of OAM protocols */
typedef enum bcm_oam_opcode_e {
    bcmOamOpcodeCCM = 1,        /* Opcode for CCM */
    bcmOamOpcodeLBR = 2,        /* Opcode for LBR */
    bcmOamOpcodeLBM = 3,        /* Opcode for LBM */
    bcmOamOpcodeLTR = 4,        /* Opcode for LTR */
    bcmOamOpcodeLTM = 5,        /* Opcode for LTM */
    bcmOamOpcodeAIS = 33,       /* Opcode for AIS */
    bcmOamOpcodeLCK = 35,       /* Opcode for LCK */
    bcmOamOpcodeTST = 37,       /* Opcode for TST */
    bcmOamOpcodeLinearAPS = 39, /* Opcode for Linear APS */
    bcmOamOpcodeRingAPS = 40,   /* Opcode for Ring APS */
    bcmOamOpcodeMCC = 41,       /* Opcode for MCC */
    bcmOamOpcodeLMR = 42,       /* Opcode for LMR */
    bcmOamOpcodeLMM = 43,       /* Opcode for LMM */
    bcmOamOpcode1DM = 45,       /* Opcode for 1DM */
    bcmOamOpcodeDMR = 46,       /* Opcode for DMR */
    bcmOamOpcodeDMM = 47,       /* Opcode for DMM */
    bcmOamOpcodeEXR = 48,       /* Opcode for EXR */
    bcmOamOpcodeEXM = 49,       /* Opcode for EXM */
    bcmOamOpcodeVSR = 50,       /* Opcode for VSR */
    bcmOamOpcodeVSM = 51,       /* Opcode for VSM */
    bcmOamOpcodeCSF = 52,       /* Opcode for MCC */
    bcmOamOpcode1SL = 53,       /* Opcode for 1SL */
    bcmOamOpcodeSLR = 54,       /* Opcode for SLR */
    bcmOamOpcodeSLM = 55,       /* Opcode for 1DM */
    bcmOamOpcodeMax = 256       /* Max. Opcodes.Not an usable value.Always last */
} bcm_oam_opcode_t;

/* Struture to set the Opcodes bitmap */
typedef struct bcm_oam_opcodes_s {
    SHR_BITDCL opcode_bmp[_SHR_BITDCLSIZE(256)]; /* Opcodes bitmap vector */
} bcm_oam_opcodes_t;

#define BCM_OAM_OPCODES_BITMAP_SET(_opcodes_bitmap, _opcode_)  SHR_BITSET(((_opcodes_bitmap).opcode_bmp), (_opcode_)) 

#define BCM_OAM_OPCODES_BITMAP_GET(_opcodes_bitmap, _opcode_)  SHR_BITGET(((_opcodes_bitmap).opcode_bmp), (_opcode_)) 

#define BCM_OAM_OPCODES_BITMAP_CLEAR(_opcodes_bitmap, _opcode_)  SHR_BITCLR(((_opcodes_bitmap).opcode_bmp), (_opcode_)) 

#define BCM_OAM_OPCODES_BITMAP_SET_ALL(_opcodes_bitmap)  SHR_BITSET_RANGE(((_opcodes_bitmap).opcode_bmp), 0, bcmOamOpcodeMax) 

#define BCM_OAM_OPCODES_BITMAP_CLEAR_ALL(_opcodes_bitmap)  SHR_BITCLR_RANGE(((_opcodes_bitmap).opcode_bmp), 0, bcmOamOpcodeMax) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set the Group for the specified opcodes of the Protocols */
extern int bcm_oam_opcode_group_set(
    int unit, 
    bcm_oam_protocol_type_t protocol, 
    bcm_oam_opcodes_t opcodes, 
    uint8 opcode_group);

/* Get the opcode group for each opcode of each protocol */
extern int bcm_oam_opcode_group_get(
    int unit, 
    bcm_oam_protocol_type_t protocol, 
    bcm_oam_opcode_t opcode, 
    uint8 *opcode_group);

#endif /* BCM_HIDE_DISPATCHABLE */

/* OAM BHH endpoint/group fault flags */
#define BCM_OAM_BHH_FAULT_CCM_TIMEOUT       0x01       /* A BHH CCM time out
                                                          alarm */
#define BCM_OAM_BHH_FAULT_CCM_RDI           0x02       /* A BHH CCM with RDI
                                                          alarm */
#define BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL 0x04       /* A BHH CCM with unknown
                                                          MEG level alarm */
#define BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID 0x08       /* A BHH CCM with unknown
                                                          MEG ID alarm */
#define BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID 0x10       /* A BHH CCM with unknown
                                                          MEP ID alarm */
#define BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD 0x20       /* A BHH CCM with unknown
                                                          period alarm */
#define BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY 0x40       /* A BHH CCM with unknown
                                                          priority alarm */
#define BCM_OAM_ENDPOINT_BHH_FAULT_CSF_LOS  0x80       /* CSF Loss of Signal */
#define BCM_OAM_ENDPOINT_BHH_FAULT_CSF_FDI  0x100      /* CSF Forward Defect
                                                          Indication */
#define BCM_OAM_ENDPOINT_BHH_FAULT_CSF_RDI  0x200      /* CSF Reverse Defect
                                                          Indication */

/* Flags will be sent back as bitmap in processed stats structure */
#define BCM_OAM_PM_STATS_PROCESSED_COUNTER_ROLLOVER 0x0001     /* Flag bit in flags
                                                          field of PM stats
                                                          structure to indicate
                                                          counter roll-over */

#ifndef BCM_HIDE_DISPATCHABLE

/* Attach an egress MPLS gport to a MPLS LSP endpoint */
extern int bcm_oam_endpoint_gport_egress_attach(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    bcm_gport_t gport);

/* Detach an egress MPLS gport from a MPLS LSP endpoint */
extern int bcm_oam_endpoint_gport_egress_detach(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    bcm_gport_t gport);

/* Get the list of MPLS gports attached to a MPLS LSP endpoint */
extern int bcm_oam_endpoint_gport_egress_attach_get(
    int unit, 
    bcm_oam_endpoint_t endpoint, 
    int max_count, 
    bcm_gport_t *gport, 
    int *count);

/* Attach an egress interface to a MPLS LSP endpoint */
extern int bcm_oam_endpoint_egress_intf_egress_attach(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    bcm_if_t egress_intf);

/* Detach an egress interface from a MPLS LSP endpoint */
extern int bcm_oam_endpoint_egress_intf_egress_detach(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    bcm_if_t egress_intf);

/* Get the list of Egress interfaces attached to MPLS LSP endpoint */
extern int bcm_oam_endpoint_egress_intf_egress_attach_get(
    int unit, 
    bcm_oam_endpoint_t endpoint_id, 
    int max_count, 
    bcm_if_t *egress_intf, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Fault status of each endpoint alongwith endpoint id */
typedef struct bcm_oam_endpoint_fault_s {
    bcm_oam_endpoint_t endpoint_id; /* Endpoint id assocaietd with the faults
                                       bitmap */
    uint32 faults;                  /* Faults bitmap for the endpoint */
} bcm_oam_endpoint_fault_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Retrieve all faults for all valid end points up to a maximum of
 * "max_endpoints" for a given protocol
 */
extern int bcm_oam_endpoint_faults_multi_get(
    int unit, 
    uint32 flags, 
    bcm_oam_protocol_type_t endpoint_protocol, 
    uint32 max_endpoints, 
    bcm_oam_endpoint_fault_t *faults, 
    uint32 *endpoint_count);

/* Set the packet header on the protection packets injected by the OAMP */
extern int bcm_oam_protection_packet_header_set(
    int unit, 
    bcm_pkt_blk_t *packet_header);

/* Get the packet header of the protection packets injected by the OAMP */
extern int bcm_oam_protection_packet_header_get(
    int unit, 
    bcm_pkt_blk_t *packet_header);

/* 
 * Create a Hardware profile entry for setting count profile of OAM
 * opcodes
 */
extern int bcm_oam_opcodes_count_profile_create(
    int unit, 
    uint8 *lm_count_profile);

/* Set the counting profile of opcodes for a given Loss count profile id */
extern int bcm_oam_opcodes_count_profile_set(
    int unit, 
    uint8 lm_count_profile, 
    uint8 count_enable, 
    bcm_oam_opcodes_t *opcodes_bitmap);

/* 
 * Get the opcodes for a given counting profile for a given Loss count
 * profile id
 */
extern int bcm_oam_opcodes_count_profile_get(
    int unit, 
    uint8 lm_count_profile, 
    uint8 count_enable, 
    bcm_oam_opcodes_t *opcodes_bitmap);

/* Delete the count profile with the given id */
extern int bcm_oam_opcodes_count_profile_delete(
    int unit, 
    uint8 lm_count_profile);

/* Add ports to trunk mapping to port-trunk database in OAM */
extern int bcm_oam_trunk_ports_add(
    int unit, 
    bcm_gport_t trunk_gport, 
    int max_ports, 
    bcm_gport_t *port_arr);

/* Delete ports to trunk mapping from port-trunk database in OAM */
extern int bcm_oam_trunk_ports_delete(
    int unit, 
    bcm_gport_t trunk_gport, 
    int max_ports, 
    bcm_gport_t *port_arr);

/* Get ports to trunk mapping from port-trunk database in OAM */
extern int bcm_oam_trunk_ports_get(
    int unit, 
    bcm_gport_t trunk_gport, 
    int max_ports, 
    bcm_gport_t *port_arr, 
    int *port_count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* MPLS OAM/BFD channel types */
typedef enum bcm_oam_mpls_tp_channel_type_e {
    bcmOamMplsTpChannelPweBfd = 0,      /* BFD control Channel type */
    bcmOamMplsTpChannelPweonoam = 1,    /* PWE OAM channel type - significant
                                           only in TX direction, use
                                           bcmOamMplsTpChannelY1731 in RX
                                           direction */
    bcmOamMplsTpChannelY1731 = 2,       /* Y1731 OAM channel type */
    bcmOamMplsTpChannelMplsTpBfd = 3,   /* BFD MPLS-TP channel type */
    bcmOamMplsTpChannelMplsDirectLm = 4, /* RFC 6374 Direct LM channel type. */
    bcmOamMplsTpChannelMplsDm = 5,      /* RFC 6374 DM channel type. */
    bcmOamMplsTpChannelTypeCount = 6    /* Always Last. Not a usable value. */
} bcm_oam_mpls_tp_channel_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Set a list of values for MPLS TP OAM/BFD channel types that can be
 * received in a packet
 */
extern int bcm_oam_mpls_tp_channel_type_rx_set(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int num_values, 
    int *list_of_values);

/* 
 * Get the list of values for MPLS TP OAM/BFD channel types that can be
 * received in a packet
 */
extern int bcm_oam_mpls_tp_channel_type_rx_get(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int num_values, 
    int *list_of_values, 
    int *value_count);

/* 
 * Set a value for MPLS TP OAM/BFD channel types that would be
 * transmitted in a packet generated by OAMP
 */
extern int bcm_oam_mpls_tp_channel_type_tx_set(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int value);

/* 
 * Get the value for MPLS TP OAM/BFD channel types that would be
 * transmitted in a packet generated by OAMP
 */
extern int bcm_oam_mpls_tp_channel_type_tx_get(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int *value);

/* 
 * Deletes a list of values for MPLS TP OAM/BFD channel types that can be
 * received in a packet
 */
extern int bcm_oam_mpls_tp_channel_type_rx_delete(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int num_values, 
    int *list_of_values);

/* 
 * Delete a value for MPLS TP OAM/BFD channel types that would be
 * transmitted in a packet generated by OAMP
 */
extern int bcm_oam_mpls_tp_channel_type_tx_delete(
    int unit, 
    bcm_oam_mpls_tp_channel_type_t  channel_type, 
    int value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Used for Invalid OAM action profile indication. */
#define BCM_OAM_PROFILE_INVALID -1         

/* Flags for action profile. */
#define BCM_OAM_PROFILE_WITH_ID             0x1        /* Used by oam action
                                                          profile create profile
                                                          create with id. */
#define BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED  0x2        /* When LM is done via
                                                          CCM PDU. */
#define BCM_OAM_PROFILE_DM_TYPE_NTP         0x4        /* Use NTP format for DM. */
#define BCM_OAM_PROFILE_LM_TYPE_NONE        0x8        /* Disable counter
                                                          stamping on LMM/LMR
                                                          SLM/SLR packets. */
#define BCM_OAM_PROFILE_TYPE_SECTION        0x10       /* Set Profile for
                                                          MPLS-Section. */
#define BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE 0x20       /* Disable Hierarchical
                                                          LM by MD-level. */

/* OAM Action key flags. */
#define BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE 0x1        /* Indicates that
                                                          mpls_lm_dm_opcode
                                                          field is used instead
                                                          of opcode. */

/* bcm_oam_profile_type_t will represent the oam action profile type. */
typedef enum bcm_oam_profile_type_e {
    bcmOAMProfileIngressLIF = 0, 
    bcmOAMProfileEgressLIF = 1, 
    bcmOAMProfileIngressAcceleratedEndpoint = 2, 
    bcmOAMProfileEgressAcceleratedEndpoint = 3, 
    bcmOAMProfileCount = 4 
} bcm_oam_profile_type_t;

/* MAC types for OAM packets. Used for action profile configuration. */
typedef enum bcm_oam_dest_mac_type_e {
    bcmOAMDestMacTypeMcast = 0, 
    bcmOAMDestMacTypeMyCfmMac = 1, 
    bcmOAMDestMacTypeUknownUcast = 2, 
    bcmOAMDestMacTypeCount = 3 
} bcm_oam_dest_mac_type_t;

/* Endpoint type used for action profile configuration */
typedef enum bcm_oam_match_type_e {
    bcmOAMMatchTypeMIP = 0, 
    bcmOAMMatchTypeMEP = 1, 
    bcmOAMMatchTypePassive = 2, 
    bcmOAMMatchTypeLowMDL = 3, 
    bcmOAMMatchTypeCount = 4 
} bcm_oam_match_type_t;

/* OAM action key. */
typedef struct bcm_oam_action_key_s {
    uint32 flags; 
    bcm_oam_opcode_t opcode; 
    bcm_oam_mpls_lm_dm_opcode_type_t mpls_lm_dm_opcode; 
    bcm_oam_dest_mac_type_t dest_mac_type; 
    bcm_oam_match_type_t endpoint_type; 
    uint8 inject; 
} bcm_oam_action_key_t;

/* OAM action result. */
typedef struct bcm_oam_action_result_s {
    uint32 flags; 
    uint8 counter_increment; 
    uint8 meter_enable; 
    bcm_gport_t destination; 
} bcm_oam_action_result_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Function allocate oam profile. */
extern int bcm_oam_profile_create(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_type_t oam_profile_type, 
    bcm_oam_profile_t *profile_id);

/* Function release oam profile. */
extern int bcm_oam_profile_delete(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_t profile_id);

/* Function return oam lif profile according lif. */
extern int bcm_oam_lif_profile_get(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_oam_profile_t *ingress_oam_profile, 
    bcm_oam_profile_t *egress_oam_profile);

/* 
 * Function return oam lif profile according lif. If some end point
 * already exist on given LIF, profile will be updated in OAM-LIF-DB
 * table and stored in sw state.If no endpoint yet defined on given LIF,
 * profile will be stored in sw state.
 */
extern int bcm_oam_lif_profile_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t gport, 
    bcm_oam_profile_t ingress_oam_profile, 
    bcm_oam_profile_t egress_oam_profile);

/* Function return action. */
extern int bcm_oam_profile_action_get(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_t profile_id, 
    bcm_oam_action_key_t *oam_action_key, 
    bcm_oam_action_result_t *oam_action_result);

/* Function return action. */
extern int bcm_oam_profile_action_set(
    int unit, 
    uint32 flags, 
    bcm_oam_profile_t profile_id, 
    bcm_oam_action_key_t *oam_action_key, 
    bcm_oam_action_result_t *oam_action_result);

#endif /* BCM_HIDE_DISPATCHABLE */

/* OAM UP MEP PDU Type */
typedef enum bcm_oam_upmep_pdu_type_e {
    bcmOamUpmepPduTypeCcm = 0,      /* Used to set the CPU queue for UP MEP CCM
                                       packets */
    bcmOamUpmepPduTypeLmDm = 1,     /* Used to set the CPU queue for UP MEP
                                       LM/DM packets */
    bcmOamUpmepPduTypeSlowpath = 2, /* Used to set the CPU queue for UP MEP
                                       slowpath packets */
    bcmOamUpmepPduTypeError = 3,    /* Used to set the CPU queue for UP MEP
                                       Error packets */
    bcmOamUpmepPduTypeCount = 4     /* Must be last.  Not a usable value. */
} bcm_oam_upmep_pdu_type_t;

/* OAM Leap Second Control Modes */
typedef enum bcm_oam_leap_sec_control_mode_e {
    bcmOamLeapUpperSecIncr = 1, /* Increment IEEE1588_TIME_SEC_UPPER counter, Do
                                   not increment IEEE1588_TIME_1_SEC counter */
    bcmOamLeap1SecIncr = 2      /* Do not increment IEEE1588_TIME_SEC_UPPER,
                                   Increment IEEE1588_TIME_1_SEC counter */
} bcm_oam_leap_sec_control_mode_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure device-wide UP MEP PDU type CPU queue */
extern int bcm_oam_upmep_cosq_set(
    int unit, 
    bcm_oam_upmep_pdu_type_t upmep_pdu_type, 
    bcm_cos_queue_t cosq);

/* Query device-wide UP MEP PDU type CPU queue */
extern int bcm_oam_upmep_cosq_get(
    int unit, 
    bcm_oam_upmep_pdu_type_t upmep_pdu_type, 
    bcm_cos_queue_t *cosq);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_oam_action_key_t structure. */
extern void bcm_oam_action_key_t_init(
    bcm_oam_action_key_t *action_key_ptr);

/* Initialize a bcm_oam_action_result_t structure. */
extern void bcm_oam_action_result_t_init(
    bcm_oam_action_result_t *action_result_ptr);

/* OAM reflector encap flags */
#define BCM_OAM_REFELCTOR_ENCAP_WITH_ID 0x1        /* Allocate the specified
                                                      encap id for OAM reflector */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create encap_id for OAM reflector. */
extern int bcm_oam_reflector_encap_create(
    int unit, 
    uint32 flags, 
    bcm_if_t *encap_id);

/* Create / Destroy encap ID for OAM reflector. */
extern int bcm_oam_reflector_encap_destroy(
    int unit, 
    uint32 flags, 
    bcm_if_t encap_id);

/* 
 * Function Get encoded oam profile id and associated flags from raw OAM
 * profile id and OAM profile type.
 */
extern int bcm_oam_profile_id_get_by_type(
    int unit, 
    uint8 profile_id, 
    bcm_oam_profile_type_t oam_profile_type, 
    uint32 *flags, 
    bcm_oam_profile_t *enc_profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_oam_control_key_s {
    bcm_oam_control_type_t type;    /* OAM control type */
    int index;                      /* OAM control index */
} bcm_oam_control_key_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure indexed OAM control. */
extern int bcm_oam_control_indexed_set(
    int unit, 
    bcm_oam_control_key_t key, 
    uint64 arg);

/* Get indexed OAM control value. */
extern int bcm_oam_control_indexed_get(
    int unit, 
    bcm_oam_control_key_t key, 
    uint64 *arg);

#endif /* BCM_HIDE_DISPATCHABLE */

typedef struct bcm_oam_tod_leap_control_s {
    uint8 leap_ctrl_enable;             /* Flag to enable/disable Leap Second
                                           Control */
    bcm_oam_leap_sec_control_mode_t leap_sec_control; /* Leap Second control mode */
} bcm_oam_tod_leap_control_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Configure TOD Leap Second Control */
extern int bcm_oam_tod_leap_control_set(
    int unit, 
    bcm_oam_timestamp_format_t ts_format, 
    bcm_oam_tod_leap_control_t leap_info);

/* Query TOD Leap Second Control */
extern int bcm_oam_tod_leap_control_get(
    int unit, 
    bcm_oam_timestamp_format_t ts_format, 
    bcm_oam_tod_leap_control_t *leap_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_OAM_H__ */
