/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ces.h
 */
#ifndef CES_H_
#define CES_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#endif
                   

#ifdef BCM_UKERNEL

/*

    from nd_hw.h 
*/                   
typedef enum
{
    AG_ND_TDM_PROTOCOL_E1 = 0x00,
    AG_ND_TDM_PROTOCOL_T1 = 0x01

} AgNdTdmProto;

typedef enum
{
    AG_ND_CCLK_SELECT_REF_CLK_1         = 0x00,
    AG_ND_CCLK_SELECT_REF_CLK_2         = 0x01,
    AG_ND_CCLK_SELECT_EXTERNAL_CLK      = 0x02

} AgNdCClkSelect;

                   
typedef enum
{
    AG_ND_REF_CLK_SELECT_RCLK           = 0x00,
    AG_ND_REF_CLK_SELECT_REF_INPUT_1    = 0x01,
    AG_ND_REF_CLK_SELECT_REF_INPUT_2    = 0x02,
    AG_ND_REF_CLK_SELECT_NOMAD_BRG      = 0x03,
    AG_ND_REF_CLK_SELECT_PORT_BRG       = 0x04,
    AG_ND_REF_CLK_SELECT_PTP            = 0x07

} AgNdRefClkSelect;

/* */
/* */
/* channel/circuit id stuff */
/* */
typedef uint8   AgNdCircuit; 
typedef uint8   AgNdPort; 
typedef uint8   AgNdSlot;
typedef uint8   AgNdChannel;
typedef uint8   AgNdPtpIdx;

#endif

/***************************************************************************************/



/*********************************/
/*
 * CRM messages
 */
#define _CES_INIT_RETRIES 2
#define _CES_NUM_CMICM    2
#define _CES_UC_MSG_TIMEOUT_USECS 1000000

#define BCM_CES_CRM_FLAG_NONE 0x00 /* */
#define BCM_CES_CRM_FLAG_INIT 0x01 /* The host has just (re)started */
#define BCM_CES_CRM_FLAG_WB   0x02 /* Warm boot */

/*
 *   Enumerations for values to set rcovery type
 */
typedef enum t_AgClockSource
{
	CLOCK_SOURCE_ADAPTIVE = 4,
	CLOCK_SOURCE_DIFFERENTIAL = 6,
	CLOCK_SOURCE_DIRECT = 8,
	CLOCK_SOURCE_PTP = 9
} AgClockSource;


/*
 *   Enumerations for values returend by get status
 */
typedef enum AgRcrStates_E
{
    RCR_STATE_FREE_RUNNING,          /* 0 */
    RCR_STATE_ACQUISITION,            /* 1 */
    RCR_STATE_NORMAL,                /* 2 */
    RCR_STATE_HOLDOVER,              /* 3 */
    RCR_STATE_QUALIFICATION_PERIOD,  /* 4 */
    RCR_STATE_FAST_ACQUISITION        /* 5 */
} AgRcrStates;

/*
 * CRM locate message
 * MOS_MSG_SUBCLASS_CES_CRM_INIT
 */
typedef struct {
    uint8 flags;
} bcm_ces_crm_init_msg_t;

/*
 * Comon configuration message
 * MOS_MSG_SUBCLASS_CES_CRM_CONFIG
 */
typedef struct {
    uint8   flags;
    uint8   e_cclk_select;         /* AgNdCClkSelect: ref1/ref2/external*/
    uint8   e_ref_clk_1_select;    /* AgNdRefClkSelect */
    uint8   e_ref_clk_2_select;    /* AgNdRefClkSelect */
    uint32  n_ref_clk_1_port;
    uint32  n_ref_clk_2_port;
    uint8   e_protocol;             /* AgNdTdmProto: T1/E1*/
} bcm_ces_crm_cclk_config_msg_t;

/* 
 * RClock configuration message
 * MOS_MSG_SUBCLASS_CES_CRM_RCLOCK_CONFIG 
 */
typedef struct {
    uint8           flags;                  /* Used exclusively for system status */
    uint8           b_enable;       
    uint8           e_protocol;             /* AgNdTdmProto: T1/E1*/
    uint8           b_structured;           /* Boolean 0 = unstructured, 1 = structured */
    uint8           output_brg;             /* ,  0=port's brg  1=system brg 1  2=system brg 2 */
    uint8           rclock;                 /*Recovery algorithm instance  0-n (n=3)*/
    uint8           port;                   /* Brg of this port will be set, if flags selected port's BRG   */
    uint8           recovery_type;          /* 00 = adaptive / differential = 01 */
    uint16          tdm_clocks_per_packet;
    uint8           service;                /*Any service  AgNdChannel */
    uint8           rcr_flags;              /*bit 0 (low bit) = 1 use ho_q_count / bit 1 = 1 use ho_x_threshold*/
    uint32          ho_q_count;
    uint32          ho_a_threshold;
    uint32          ho_b_threshold;
    uint32          ho_c_threshold;

} bcm_ces_crm_rclock_config_msg_t;

/*
 * Status message
 * MOS_MSG_SUBCLASS_CES_CRM_STATUS_REPLY        
 */
#define CES_RCR_FRACTION_DECIMAL_POINTS_PRECISION 10000000
typedef struct {
     uint8         rclock;        /*Recovery algorithm instance  0-n (n=3)*/
     uint8         clock_state;   /*AgRcrStates  accuistion, normal ,,*/
     uint32        seconds_locked;
     uint32        seconds_active;
     uint32        calculated_frequency_w;  /*Whole part of frequence*/
     uint32        calculated_frequency_f;  /*Fraction part of freq*CES_RCR_FRACTION_DECIMAL_POINTS_PRECISION */
} bcm_ces_crm_status_msg_t;
/*
 * CES control message
 */
typedef struct ces_msg_ctrl_s {
    union {
        bcm_ces_crm_init_msg_t   init;
        bcm_ces_crm_cclk_config_msg_t config ;
        bcm_ces_crm_rclock_config_msg_t rclock;
        bcm_ces_crm_status_msg_t status;
    } u;
}ces_msg_ctrl_t;
#define CES_CONTROL_MESSAGE_MAX_SIZE ( ((sizeof(ces_msg_ctrl_t)+3)/4)*4  )

/*
 * CES Error codes
 */
typedef enum uc_ces_error_e {
    UC_CES_E_NONE = 0,
    UC_CES_E_INTERNAL,
    UC_CES_E_MEMORY,
    UC_CES_E_PARAM,
    UC_CES_E_RESOURCE,
    UC_CES_E_EXISTS,
    UC_CES_E_NOT_FOUND,
    UC_CES_E_INIT
} _uc_fd_error_t;
#endif /* CES_H_ */
