/** \file dnx_data_max_fc.h
 * 
 * MODULE DATA MAX - 
 * Maintain max value (of all devices) per define
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MAX_FC_H_
/*{*/
#define _DNX_DATA_MAX_FC_H_
/*
 * INCLUDE FILES:
 * {
 */
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MAX DEFINES:
 * {
 */
/**
 * \brief
 * Number of Global Resources
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_GLB_RSC (14)

/**
 * \brief
 * Number of Generic bitmaps
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_GEN_BMPS (64)

/**
 * \brief
 * First ID of Generic bitmaps that can be used by users.
 */
#define DNX_DATA_MAX_FC_GENERAL_GEN_BMP_FIRST_ID (1)

/**
 * \brief
 * Number of AF NIF FC infications
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_AF_NIF_INDICATIONS (144)

/**
 * \brief
 * Number of Link-Level FC (from the NIF-RX-MLF) that can be mapped to calendar.
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_CAL_LLFCS (148)

/**
 * \brief
 * Number of entries in the Rx Quad MAC Lane FIFO (QMLF).
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_ENTRIES_QMLF (32767)

/**
 * \brief
 * Number of bits in selector 64 of the NIF PFC statuses to be presented.
 */
#define DNX_DATA_MAX_FC_GENERAL_NOF_BITS_IN_PFC_STATUS_SELECT (4)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one of the PFCs.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_PFC (0)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one of the nof-links NIF's link-level FCs.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_NIF_LLFC (1)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one port (2 priorities mode).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_PORT_2_PRIO (2)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one port (4 priorities mode).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_PORT_4_PRIO (3)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one port (8 priorities mode).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_PORT_8_PRIO (4)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is one of the the generic PFC bitmaps.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_PFC_GENERIC_BMP (7)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's destination is currently empty.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_RX_DEST_EMPTY (6)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of STE VSQs (Groups A/B/C).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_VSQ_A_B_C (0)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of STE STAT VSQs (Groups D).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_VSQ_D (1)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of STE LLFC VSQs (Groups E).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_LLFC_VSQ (2)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of STE PFC VSQs (Groups F).
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_PFC_VSQ (3)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of the Global Resource FCs.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_GLB_RSC (4)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is one of the Link-Level FC from the NIF-RX-MLF.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_NIF_LLFC (6)

/**
 * \brief
 * Index to write in HW to indicate that the calendar entry's source is currently empty.
 */
#define DNX_DATA_MAX_FC_CALENDAR_COMMON_TX_SRC_EMPTY (7)

/**
 * \brief
 * Number of PP COE calendar instances
 */
#define DNX_DATA_MAX_FC_COE_NOF_COE_CAL_INSTANCES (2)

/**
 * \brief
 * Number of supported channel IDs in COE flow control
 */
#define DNX_DATA_MAX_FC_COE_NOF_SUPPORTED_CHAN_IDS (4096)

/**
 * \brief
 * Number of COE_VID_MASK width
 */
#define DNX_DATA_MAX_FC_COE_NOF_COE_VID_MASK_WIDTH (10)

/**
 * \brief
 * Number of COE VID right shift width
 */
#define DNX_DATA_MAX_FC_COE_NOF_COE_VID_RIGHT_SHIFT_WIDTH (4)

/**
 * \brief
 * Number of COE_PTC_MASK width
 */
#define DNX_DATA_MAX_FC_COE_NOF_COE_PTC_MASK_WIDTH (10)

/**
 * \brief
 * Number of COE PTC left shift width
 */
#define DNX_DATA_MAX_FC_COE_NOF_COE_PTC_LEFT_SHIFT_WIDTH (4)

/**
 * \brief
 * Max value for COE ticks. N-1 where N number of clocks for pause counters decrement.
 */
#define DNX_DATA_MAX_FC_COE_MAX_COE_TICKS (1048575)

/**
 * \brief
 * Indication to support of Flow_Control on COE port
 */
#define DNX_DATA_MAX_FC_COE_FC_SUPPORT  (1)

/**
 * \brief
 * Number of Out of Band interfaces.
 */
#define DNX_DATA_MAX_FC_OOB_NOF_OOB_IDS (2)

/**
 * \brief
 * Number of Inband ILKN calendar instances
 */
#define DNX_DATA_MAX_FC_INBAND_NOF_INBAND_INTLKN_CAL_INSTANCES (16)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_FC_H_*/
/* *INDENT-ON* */
