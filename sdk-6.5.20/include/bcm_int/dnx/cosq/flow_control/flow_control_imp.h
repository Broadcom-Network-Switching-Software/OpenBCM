/** \file bcm_int/dnx/cosq/flow_control/flow_control_imp.h
 * 
 *
 * Flow control callback functions for DNX. \n
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * $Copyright:.$
 */

#ifndef _SRC_DNX_FC_CB_H_INCLUDED_
/** { */
#define _SRC_DNX_FC_CB_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

/*
 * Defines
 * {
 */

 /**
 * \brief - Handle for template manager
 */
#define RESOURCE_FC_GENERIC_BMP "RESOURCE_FC_GENERIC_BMP"

 /**
 * \brief - Default profile for template manager
 */
#define GENERIC_BMP_DEFAULT_PROFILE_ID (dnx_data_fc.general.nof_gen_bmps_get(unit))

 /**
 * \brief - Types for the OOB calendar
 */
#define DNX_OOB_TYPE_SPI         1
#define DNX_OOB_TYPE_ILKN        2

 /*
  * }
  */

/*
 * Typedefs
 * {
 */

/**
 * \brief - Data type for the PFC Generic Bitmap template manager
 */
typedef struct pfc_generic_bmp
{
    uint32 qpair_bits[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS /
                                                             (sizeof(uint32) * 8)];
} dnx_pfc_generic_bmp_t;
  /**
 * \brief - Enumerator for selecting the type of PFC FC that the FcIndex refers to in table FC_REC_PFC_MAP
 */
typedef enum
{
/** Index to 1 of 512 EGQ PFC */
    DNX_REC_PFC_MAP_SEL_PFC = 0,
/** Index to 1 of 16 generifc PFC bitmaps. */
    DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP = 1,
/** Number of PFC mapping types that can be selected */
    DNX_REC_PFC_MAP_SELECT_TYPES
} dnx_rec_pfc_map_sel_t;

  /**
 * \brief - Enumerator for the type of FC that the FcIndex refers to in the tables for Calendars(OOB, COE, Inband ILKN)
 */
typedef enum
{
/** The calendar carries one of the 1024 PFCs. */
    DNX_RX_CAL_DEST_PFC = 0,
/** The calendar carries one of the nof-links NIF's link-level FCs. */
    DNX_RX_CAL_DEST_NIF = 1,
/** The calendar carries one port (2 priorities mode). */
    DNX_RX_CAL_DEST_PORT_2_PRIO = 2,
/** The calendar carries one port (4 priorities mode). */
    DNX_RX_CAL_DEST_PORT_4_PRIO = 3,
/** The calendar carries one port (8 priorities mode). */
    DNX_RX_CAL_DEST_PORT_8_PRIO = 4,
/** The calendar carries one of the the 32 generic PFC bitmaps. */
    DNX_RX_CAL_DEST_PFC_GENERIC_BMP = 5,
/** Constant value. */
    DNX_RX_CAL_DEST_CONST = 6,
/** Number of destinations */
    DNX_RX_CAL_DESTINATIONS
} DNX_RX_CAL_dest_t;

 /**
 * \brief - Enumerator for the type of FC that the FcIndex refers to in table Calendars(OOB, COE, Inband ILKN)
 */
typedef enum
{
/** The index refers to one of 200 STE VSQs (Groups A/B/C) */
    DNX_TX_CAL_SRC_VSQ_A_B_C = 0,
/** The index refers to one of 512 STE STAT VSQs (Groups D) */
    DNX_TX_CAL_SRC_VSQ_D = 1,
/** The index refers to one of 296 STE LLFC VSQs (Groups E) */
    DNX_TX_CAL_SRC_LLFC_VSQ = 2,
/** The index refers to one of 384 STE PFC VSQs (Groups F) */
    DNX_TX_CAL_SRC_PFC_VSQ = 3,
/** The index refers to one of the 14 global resource FCs */
    DNX_TX_CAL_SRC_GLB_RSC = 4,
/** The index refers to one of the 512 HCFC FCs - NOT USED */
    DNX_TX_CAL_SRC_HCFC = 5,
/** The index refers to one of the 96 Link-Level FC (from the NIF-RX-MLF) */
    DNX_TX_CAL_SRC_NIF = 6,
/** Constant value */
    DNX_TX_CAL_SRC_CONST = 7,
/** Number of sources */
    DNX_OOB_TX_CAL_SOURCES
} dnx_oob_tx_cal_src_t;

 /**
 * \brief - Enumerator for the index to one of the generated FC depending on Global resources in table Calendars(OOB, COE, Inband ILKN)
 */
typedef enum
{
/** FC is due to DRAM High priority */
    DNX_GLB_RCS_IDX_DRAM_HIGH = 0,
/** FC is due to SRAM High priority */
    DNX_GLB_RCS_IDX_SRAM_HIGH = 1,
/** FC is due to DRAM High priority Pool 0 */
    DNX_GLB_RCS_IDX_DRAM_POOL_0_HIGH = 2,
/** FC is due to DRAM High priority Pool 1 */
    DNX_GLB_RCS_IDX_DRAM_POOL_1_HIGH = 3,
/** FC is due to SRAM High priority Pool 0 */
    DNX_GLB_RCS_IDX_SRAM_POOL_0_HIGH = 4,
/** FC is due to SRAM High priority Pool 1 */
    DNX_GLB_RCS_IDX_SRAM_POOL_1_HIGH = 5,
/** FC is due to HDRM (no High/Low priority supported) */
    DNX_GLB_RCS_IDX_HDRM = 6,
/** FC is due to DRAM Low priority */
    DNX_GLB_RCS_IDX_DRAM_LOW = 7,
/** FC is due to SRAM Low priority) */
    DNX_GLB_RCS_IDX_SRAM_LOW = 8,
/** FC is due to DRAM Low priority Pool 0 */
    DNX_GLB_RCS_IDX_DRAM_POOL_0_LOW = 9,
/** FC is due to DRAM Low priority Pool 1 */
    DNX_GLB_RCS_IDX_DRAM_POOL_1_LOW = 10,
/** FC is due to SRAM Low priority Pool 0 */
    DNX_GLB_RCS_IDX_SRAM_POOL_0_LOW = 11,
/** FC is due to SRAM Low priority Pool 1 */
    DNX_GLB_RCS_IDX_SRAM_POOL_1_LOW = 12,
/** For HDRM no High/Low priority supported but in HW index is allocated */
    DNX_GLB_RCS_IDX_HDRM_DUMMY = 13,
/** Number of Global recources */
    DNX_GLB_RCS_COUNT
} dnx_oob_glb_rcs_idx_t;

/*
 * }
 */

/*
 * Functions
 * {
 */

/**
 * \brief - This is a function to map reception of flow control from LLFC source to Interface target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_LLFC_STOP_TX_FROM_CFC
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_llfc_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map reception of flow control from PFC source to Interface target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_LLFC_STOP_TX_FROM_CFC
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_pfc_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map reception of flow control from PFC source to Port/Port+COSQ/COSQ
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * Flow Control related HW. See dbal tables: \n
 *     DBAL_TABLE_FC_REC_PFC_MAP, DBAL_TABLE_FC_GENERIC_BITMAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_pfc_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map reception of flow control from OOB calendar source to Interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * Flow Control related HW. See dbal tables: \n
 *     DBAL_TABLE_FC_REC_PFC_MAP, DBAL_TABLE_FC_REC_OOB_CAL_MAP, DBAL_TABLE_FC_REC_COE_CAL_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_cal_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map reception of flow control from OOB calendar source to Port/Port+COSQ/COSQ
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * Flow Control related HW. See dbal tables: \n
 *     DBAL_TABLE_FC_REC_PFC_MAP, DBAL_TABLE_FC_REC_OOB_CAL_MAP, DBAL_TABLE_FC_REC_COE_CAL_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_cal_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Global resources source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_GLB_RCS_LLFC_EN
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Global resources source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_GLB_RCS_PFC_EN
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Global resources source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, DBAL_TABLE_FC_GEN_OOB_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from LLFC VSQ source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_llfc_vsq_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from LLFC VSQ source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow controlt
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_OOB_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_llfc_vsq_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from PFC VSQ source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_pfc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from PFC VSQ source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_OOB_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_pfc_vsq_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Category 2 & Traffic class source to PFC VSQ target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_CAT2_PFC_EN
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_c2_tc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from VSQs A-D source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targete of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_OOB_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_vsq_a_d_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from VSQs D source to PFC/LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targete of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_VSQD_PFC_LLFC_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_vsq_d_pfc_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Almost full NIF source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_LLFC_RX_FIFOS_VECTOR_MASK
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Almost full NIF source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal tables: \n
 *     DBAL_TABLE_FC_PFC_FROM_LLFC_PRIO_OF_RMC, DBAL_TABLE_FC_PFC_PRIO_OF_RMC
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This is a function to map generation of flow control from Almost full NIF source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_GEN_OOB_MAP
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for reception of flow control from LLFC source to Interface target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_llfc_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for reception of flow control from PFC source to Interface target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_pfc_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for reception of flow control from PFC source to Port/Port+COSQ
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_pfc_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for reception of flow control from OOB calendar source to Interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_cal_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for reception of flow control from OOB calendar source to Port/Port+COSQ
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_cal_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Global resources source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit i
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Global resources source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Global resources source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from LLFC VSQ source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_llfc_vsq_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from LLFC VSQ source,
 * Cat2+TC source, VSQ A-D source, Almost full NIF source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_to_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from PFC VSQ source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_pfc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from VSQ D source to PFC/LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_vsq_d_pfc_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from PFC VSQ source to OOB calendar target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_pfc_vsq_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Category 2 & Traffic class source to PFC VSQ target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_c2_tc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Almost full NIF source to PFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - This function to verify the parameters for flow control from Almost full NIF source to LLFC target
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the targeted reaction point
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief -
 * To get reception reaction points from PFC or LLFC source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_llfc_pfc_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get reception reaction points from OOB calendar source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_rec_cal_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from Global Resource source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_glb_res_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from LLFC VSQ source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_llfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from PFC VSQ source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_pfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from A,B,C,D VSQs source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_vsq_a_d_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from Category 2 Traffic Class VSQ source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_c2_tc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * To get generation targets from Almost full NIF source.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object
 *   \param [out] target_count - updated size(valid entries) of target array
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_gen_nif_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

/**
 * \brief -
 * Function to set the Ethernet port thresholds used for Flow control generation
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport - gport indicating the eth port to which the threshold is related
 *   \param [in] threshold - structure that folds information for the global recourse and the threshold we want to set.
 *   \param [in] is_set - indicates if the function was called from the SET or the GET API
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * Flow Control related HW. See dbal table: \n
 *     DBAL_TABLE_FC_RX_ETH_PORT_THRESHOLD
 * \see
 *   * None
 */
shr_error_e dnx_cosq_gport_eth_port_threshold_set_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set);

/**
 * \brief -
 * Function to set/get the Global recourses thresholds used for Flow control generation
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in,out] threshold - structure that folds information for the global recourse and the threshold we want to set.
 *   \param [in] is_set - indicates if the function was called from the SET or the GET API
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * Flow Control related HW. See dbal tables: \n
 *     DBAL_TABLE_FC_GLB_RCS_TOTAL_THS, DBAL_TABLE_FC_GLB_RCS_POOL_THS, DBAL_TABLE_FC_GLB_RCS_HDRM_THS
 * \see
 *   * None
 */
shr_error_e dnx_cosq_vsq_gl_fc_thresh_set_get(
    int unit,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set);

/**
 * \brief - Verify the imput parameters for dnx_cosq_gport_eth_port_threshold_set/get
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] gport - GPORT ID for a queue group (unicast/multicast egress queue group)
 *   \param [in] threshold - structure that folds information for the global recourse and the threshold we want to set.
 *   \param [in] is_set - indicates if the function was called from the SET or the GET API
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_gport_eth_port_threshold_set_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set);

/**
 * \brief - Verify the imput parameters for dnx_cosq_vsq_gl_fc_thresh_set
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] threshold - structure that folds information for the global recourse and the threshold we want to set.
 *   \param [in] is_set - indicates if the function was called from the SET or the GET API
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_vsq_gl_fc_thresh_set_get_verify(
    int unit,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set);

/**
 * \brief -  Function to set CFC Global Enabler for device-level flow-control.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] enable - CFC enable value to be set
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_cfc_global_enable_set(
    int unit,
    uint8 enable);

/**
 * \brief -  Function to get CFC Global Enabler for device-level flow-control.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [out] enable - CFC enable value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_cfc_global_enable_get(
    int unit,
    uint8 *enable);

/**
 * \brief -  Function to set the Priority Flow Control refresh time for a specified port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the refresh time
 *   \param [in] value - PFC refresh time value to be set
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_pfc_refresh_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief -  Function to get the Priority Flow Control refresh time for a specified port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to get the refresh time
 *   \param [out] value - PFC refresh time value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_pfc_refresh_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief -  Function to set the inband mode (PFC, LLFC) for a specified port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the inband mode
 *   \param [in] fc_direction - indicates if the path is reception or generation
 *   \param [in] llfc_enabled - indicates weather we want to enable/disable the LLFC
 *   \param [in] pfc_enabled - indicates weather  we want to enable/disable the PFC
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_inband_mode_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 llfc_enabled,
    uint32 pfc_enabled);

/**
 * \brief -  Function to determine which inband mode (PFC, LLFC) is currently enabled.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to get the inband mode
 *   \param [in] fc_direction - indicates if the path is reception or generation
 *   \param [out] llfc_enabled - indicates weather the LLFC is enabled
 *   \param [out] pfc_enabled - indicates weather the PFC is enabled
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_inband_mode_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 *llfc_enabled,
    uint32 *pfc_enabled);

/**
 * \brief -  Function to set the source MAC address transmitted in MAC
 *           control pause frames for a specified port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [in] mac_sa - MAC address to transmit as the source address in MAC control pause frames
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_port_mac_sa_set(
    int unit,
    int port,
    bcm_mac_t mac_sa);

/**
 * \brief -  Function to get the source MAC address transmitted in MAC
 *           control pause frames for a specified port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set the MAC address
 *   \param [out] mac_sa - MAC address
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_port_mac_sa_get(
    int unit,
    int port,
    bcm_mac_t mac_sa);

/**
 * \brief -  Function to get the Priority Flow Control status for a specified port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to get the PFC status
 *   \param [out] pfc_bitmap - PFC bitmap. If a priority is under FC the corresponding bit will be set.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_pfc_status_get(
    int unit,
    bcm_port_t port,
    int *pfc_bitmap);

/**
 * \brief -  Function set the thresholds for Flow control generation from global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] index_info - additional information derived from the index
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in] threshold - threshold value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_glb_rsc_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief -  Function set the thresholds for Flow control generation from global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] index_info - additional information derived from the index
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [out] threshold - threshold value
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fc_glb_rsc_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief -
 *   Print the PFC Generic Bitmap template content.
 * \param [in] unit - relevant unit
 * \param [in] data - template data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_fc_generic_bmp_profile_template_print_cb(
    int unit,
    const void *data);

shr_error_e dnx_fc_init(
    int unit);

#endif /** _SRC_DNX_FC_CB_H_INCLUDED_ */
