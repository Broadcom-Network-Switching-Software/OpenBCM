/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 *  \file bcm_int/dnx/field/field_range.h
 *
 *  This file exposes DNX level functions for Field range,
 *  and their input structure types.
 */

#ifndef DNX_FIELD_RANGE_H_INCLUDED

#define DNX_FIELD_RANGE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * { Include files
 */

/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm_int/dnx/field/field.h>
/** soc */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_range_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
/** shared */
#include <shared/bsl.h>
/**
 * } Include files
 */
/**
 * { DEFINES
 */
 /**
  * The maximum value for L4 range quals
  */
#define DNX_FIELD_RANGE_L4_OPS_MAX_VAL   0xFFFF
/**
 * The maximum for Out Lif range quals
 */
#define DNX_FIELD_RANGE_OUT_LIF_MAX_VAL  0x3FFFFF
/**
 * The maximum for Packet Header range quals
 */
#define DNX_FIELD_RANGE_PKT_HDR_MAX_VAL  0xFF
 /**
  * Used for setting the minimum value for range ID and for the Range!
  * The minimum for all range qualifiers (L4 Ops, Out Lif and Packet Header)
  */
#define DNX_FIELD_RANGE_QUALS_MIN_VAL     0
/**
 * Used in the verify function. Indicates that we are verifying in the get function.
 */
#define DNX_FIELD_RANGE_IS_FOR_SET     1
/**
 * Used in the verify function. Indicates that we are verifying in the get function.
 */
#define DNX_FIELD_RANGE_IS_FOR_GET     0
/**
 * } DEFINES
 */
/**
 * { MACROS
 */
/**
 * Range ID validation check for L4 Range Qualifiers.
 * Range must smaller then 24 and bigger then 0.
 * For L4 Ops the DNX_DATA_MAX_FIELD value is the same for both stages!
 */
#define DNX_FIELD_RANGE_L4_RANGE_ID_VALID(_unit, _stage, _range_id)    \
    ((((int)_range_id) >= 0) && \
     ((_range_id) < dnx_data_field.stage.stage_info_get((_unit), (_stage))->nof_l4_ops_ranges_legacy))
/**
 * Range ID validation check for Out Lif Range Qualifiers for IPMF1.
 * Range must smaller then 3 and bigger then 0
 */
#define DNX_FIELD_RANGE_OUT_LIF_IPMF1_RANGE_ID_VALID(_unit, _range_id)    \
    ((((int)_range_id) >= 0) && \
     ((_range_id) < dnx_data_field.stage.stage_info_get((_unit), DNX_FIELD_STAGE_IPMF1)->nof_out_lif_ranges))
/**
 * Range ID validation check for Out Lif Range Qualifiers for IPMF3.
 * Range must smaller then 6 and bigger then 0
 */
#define DNX_FIELD_RANGE_OUT_LIF_IPMF3_RANGE_ID_VALID(_unit, _range_id)    \
    ((((int)_range_id) >= 0) && \
     ((_range_id) < dnx_data_field.stage.stage_info_get((_unit), DNX_FIELD_STAGE_IPMF3)->nof_out_lif_ranges))
/**
 * Range ID validation check for Packet Header Size Range Qualifiers.
 * Range must smaller then 3 and bigger then 0
 */
#define DNX_FIELD_RANGE_PKT_HDR_RANGE_ID_VALID(_unit, _range_id)    \
    ((((int)_range_id) >= 0) && \
     (_range_id < dnx_data_field.stage.stage_info_get(_unit, DNX_FIELD_STAGE_IPMF1)->nof_pkt_hdr_ranges))
 /**
  * Min-max range validation check for L4 Range Qualifiers.
  * Range must be [0-65355]
  */
#define DNX_FIELD_RANGE_L4_RANGE_VALID(range_min_val, range_max_val)  \
    (((int)range_min_val >= DNX_FIELD_RANGE_QUALS_MIN_VAL) && (range_max_val <= DNX_FIELD_RANGE_L4_OPS_MAX_VAL))
/**
 * Min-max range validation check for Out Lif Range Qualifier.
 * Range must be [0-4194303]
 */
#define DNX_FIELD_RANGE_OUT_LIF_RANGE_VALID(range_min_val, range_max_val) \
    (((int)range_min_val >= DNX_FIELD_RANGE_QUALS_MIN_VAL) && (range_max_val <= DNX_FIELD_RANGE_OUT_LIF_MAX_VAL))
/**
 * Min-max range validation check for Packet Header Size Range Qualifiers.
 * Range must be [0-255]
 */
#define DNX_FIELD_RANGE_PKT_HDR_RANGE_VALID(range_min_val, range_max_val) \
    (((int)range_min_val >= DNX_FIELD_RANGE_QUALS_MIN_VAL) && (range_max_val <= DNX_FIELD_RANGE_PKT_HDR_MAX_VAL))

/**
 * Checks whether the given type belongs to the extended L4 OPs.
 */
#define DNX_FIELD_RANGE_EXTENDED_L4_OPS(_range_type) \
    (((_range_type) == DNX_FIELD_RANGE_TYPE_L4_SRC_PORT) || \
     ((_range_type) == DNX_FIELD_RANGE_TYPE_L4_DST_PORT) || \
     (((_range_type) >= DNX_FIELD_RANGE_TYPE_L4OPS_PKT_HDR_SIZE) && \
      ((_range_type) <= DNX_FIELD_RANGE_TYPE_FFC2_HIGH)))
/**
 * } MACROS
 */
/**
 * { Types
 */

/**
 *   This structure holds the data per range qualifier.
 */
typedef struct
{
    /**
     * The type of the range. For more information look at the 'dnx_field_range_type_e'
     */
    dnx_field_range_type_e range_type;
    /**
     * Minimum value of the range.
     * In case the range type is PacketHeaderSize, min val is in bits.
     */
    uint32 min_val;
    /**
     * Maximum value of the range.
     * In case the range type is PacketHeaderSize, max val is in bits.
     */
    uint32 max_val;
} dnx_field_range_info_t;

/**
 * } Types
 */
 /**
 * \brief
 *  Clear the dnx_field_range_info_t, set it to preferred init values
 * \param [in] unit          - Device ID
 * \param [in] range_info_p    - Pointer to input structure of
 *                        dnx_field_range_info_t that needs to be init
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_range_info_t_init(
    int unit,
    dnx_field_range_info_t * range_info_p);
/**
 * \brief
 *  Install a functional range with range_id and range_info.
 * \param [in] unit       - Device ID
 * \param [in] flags      - range flags
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] range_id   - Identifier of the range: User-provided range ID
 * \param [in] range_info_p - Data to be written in hardware:
 *                The type of the range, the minimum and maximum value
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_range_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    uint32 range_id,
    dnx_field_range_info_t * range_info_p);

/**
 * \brief
 *  Get the HW data associated with access_id (range with given ID):
 *  The type of the range, the minimum and maximum value
 * \param [in] unit       - Device ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] range_id_p   - Identifier of the range: User-provided range ID
 * \param [in,out] range_info_p - The range_info structure.
 *                       as in - range_type. Used to configure all needed tables from where the info is get.
 *                       as out - min and max value.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_range_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 *range_id_p,
    dnx_field_range_info_t * range_info_p);

/**
 * \brief
 *  Get the minimum and maximum values usable by a certain rnage type.
 * \param [in] unit       - Device ID
 * \param [in] range_type - DNX range type
 * \param [out] absolute_min_val_p   - The minimum value the range type can accept.
 * \param [out] absolute_max_val_p - The maximum value the range type can accept.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_range_absolute_min_max_values(
    int unit,
    dnx_field_range_type_e range_type,
    int *absolute_min_val_p,
    int *absolute_max_val_p);

/**
 * \brief  Return name of dnx range type
 * \param [in] range_type - dnx range_type
 * \return
 *   \retval Range type            - On success
 *   \retval "Invalid Range Type"  - If invalid range type provided
 *   \retval "Unnamed Range Type"  - If for valid range name was not assigned
 * \remark
 */
char *dnx_field_range_type_text(
    dnx_field_range_type_e range_type);

/**
* \brief
*  Initialize the ranges to accept all values.
* \param [in] unit       - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_range_init(
    int unit);

/**
* \brief
*  Configures the Extended L4 OPS (range) FFCs. The FFC to config is determined by the given dnx_range_type.
* \param [in] unit                     - Device ID
* \param [in] flags                    - TBD
* \param [in] configurable_range_index - Range FFC index to set
* \param [in] dnx_qual_info            - Qualifier info to set into FFC
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
shr_error_e dnx_field_range_ffc_config_set(
    int unit,
    uint32 flags,
    uint32 configurable_range_index,
    dnx_field_range_type_qual_info_t * dnx_qual_info);

/**
* \brief
*  Retrieves the DNX qualifier info (type, attach_info) for the given Extended L4 OPS (range) FFC.
* \param [in]  unit                     - Device ID
* \param [in]  flags                    - TBD
* \param [in]  configurable_range_index - Range FFC index to get
* \param [out] dnx_qual_info            - Retrieved qualifier info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
shr_error_e dnx_field_range_ffc_config_get(
    int unit,
    uint32 flags,
    uint32 configurable_range_index,
    dnx_field_range_type_qual_info_t * dnx_qual_info);

/**
* \brief
*  Returns the range indexes and the number of maximum range IDs for the given range_type
* \param [in] unit      - Device Id.
* \param [in] dnx_range_type    - The range type
* \param [out] range_indexes - The range indexes of the given
* \param [out] nof_range_ids - Number of range IDs for the given type
* \return
*   shr_error_e - Error Type
* \see
*   * None
*/
shr_error_e dnx_field_range_type_nof_range_ids_extended(
    int unit,
    dnx_field_range_type_e dnx_range_type,
    uint32 range_indexes[],
    int *nof_range_ids);

/**
 * \brief
 *  Returns the number of range IDs available for a specific range type in a specidfic stage.
 * \param [in] unit          - Device ID
 * \param [in] range_type    - Range type
 * \param [in] field_stage   - Stage
 * \param [out] nof_ranges_p - maximum number of ranges for range type and stage
 * \return
 *   shr_error_e - Error Type
 * \remark
 */
shr_error_e dnx_field_range_nof_range_ids(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e field_stage,
    int *nof_ranges_p);

#endif/*_FIELD_API_INCLUDED_*/
