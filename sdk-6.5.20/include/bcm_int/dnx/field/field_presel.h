/**
 *  \file bcm_int/dnx/field/field_presel.h 
 *
 *  This file exposes DNX level functions for Field Preselection,
 *  and their input structure types.
 *
 *  
 *  Internal DNX RX APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef DNX_FIELD_PRESEL_H_INCLUDED

#define DNX_FIELD_PRESEL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * { Include files 
 */

/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm_int/dnx/field/field.h>
/** soc */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
/** shared */
#include <shared/bsl.h>
/*
 * } Include files
 */

/*
 * { Types 
 */

/** 
*   This structure holds the info to identify a preselector.
*   It has unique ID per stage.
*/
typedef struct
{
    /*
     * Hold the Preselector ID
     */
    dnx_field_presel_t presel_id;
    /*
     * Hold the Stage of the preselector
     */
    dnx_field_stage_e stage;

} dnx_field_presel_entry_id_t;

/** 
*   This structure holds the data per preselector qualifier.
*/
typedef struct
{
    /*
     * Hold for each set qualifier the DBAL field to be called
     */
    dbal_fields_e qual_type_dbal_field;
    /*
     * Hold the value provided for each set qualifier
     */
    uint32 qual_value;
    /*
     * Hold the mask provided for each set qualifier
     */
    uint32 qual_mask;

} dnx_field_presel_qual_data_t;
/** 
*   This structure holds the data to be written in preselection.
*/
typedef struct
{
    /*
     * Indicate if the program valid will be set at presel creation
     */
    uint8 entry_valid;
    /*
     * Hold the context ID to be associated by this preselector
     */
    dnx_field_context_t context_id;
    /*
     * Hold the number of set qualifiers
     */
    uint32 nof_qualifiers;
    /*
     * Hold for each set qualifier its data
     */
    dnx_field_presel_qual_data_t qual_data[DNX_FIELD_CS_QUAL_NOF_MAX];

} dnx_field_presel_entry_data_t;

/*
 * } Types
 */
/**
* \brief
*  Install a functional preselector with access_id (preselector with given ID): Program
*  action and qualifiers data
* \param [in] unit       - Device ID
* \param [in] flags      - preselection flags
* \param [in] entry_id_p   - Identifier of the preselector: 
*                          User-provided preselector ID and stage
* \param [in] entry_data_p - Data to be written in hardware: 
*                          PMF Program ID and valid, qualifiers
*                          data
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_set(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p);

/**
* \brief
*  Get the HW data associated with access_id (preselector with given ID):
*  Program action and qualifiers data
* \param [in] unit       - Device ID
* \param [in] flags      - preselection flags
* \param [in] entry_id_p   - Identifier of the preselector: 
*                          User-provided preselector ID and stage
* \param [out] entry_data_p - Data contained in hardware: 
*                          PMF Program ID and valid, qualifiers
*                          data
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_get(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p);

/**
* \brief
*  Returns first free preselector ID for the given stage.
* \param [in] unit       - Device ID
* \param [in] field_stage   -
*  Stage for which free presel ID have to be taken.
* \param [out] presel_id_p -
*  First free preselector ID for the given stage.
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_free_id_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_presel_t * presel_id_p);

/**
* \brief
*  Clear the dnx_field_group_info_t, set it to preferred init values
* \param [in] unit          - Device ID
* \param [in] entry_id_p    - Pointer to input structure of
*                        dnx_field_presel_entry_id_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_entry_id_t_init(
    int unit,
    dnx_field_presel_entry_id_t * entry_id_p);
/**
* \brief
*  Clear the dnx_field_group_info_t, set it to preferred init values
* \param [in] unit          - Device ID
* \param [in] entry_data_p  - Pointer to input structure of
*                           dnx_field_presel_entry_data_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_entry_data_t_init(
    int unit,
    dnx_field_presel_entry_data_t * entry_data_p);

/**
* \brief Convert BCM preselection ID to DNX format
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags
* \param [in] bcm_entry_id_p - preselection entry identifier in
*        BCM format
* \param [out] dnx_entry_id_p - preselection entry identifier in
*        DNX format
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_id_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * bcm_entry_id_p,
    dnx_field_presel_entry_id_t * dnx_entry_id_p);

/**
* \brief Convert BCM preselection data to DNX format
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags
* \param [in] stage        - Stage
* \param [in] bcm_entry_data_p - preselection entry data in BCM
*        format
* \param [out] dnx_entry_data_p - preselection entry data in DNX
*        format
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_data_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    bcm_field_presel_entry_data_t * bcm_entry_data_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p);

/**
* \brief Convert DNX preselection data to BCM format
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags
* \param [in] dnx_entry_id_p - preselection entry identifier in
*        DNX format
* \param [out] dnx_entry_data_p - preselection entry data in DNX
*        format
* \param [in] bcm_entry_data_p - preselection entry data in BCM
*        format
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_data_dnx_to_bcm_convert(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * dnx_entry_id_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p,
    bcm_field_presel_entry_data_t * bcm_entry_data_p);

/**
* \brief
*  run init for Presel module, such as default context presel entry.
* \param [in] unit       - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_init(
    int unit);

/**
* \brief
*  run deinit for Presel module.
* \param [in] unit       - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_presel_deinit(
    int unit);

#endif/*_FIELD_API_INCLUDED_*/
