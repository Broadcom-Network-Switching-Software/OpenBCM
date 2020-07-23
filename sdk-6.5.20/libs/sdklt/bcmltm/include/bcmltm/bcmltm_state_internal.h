/*! \file bcmltm_state_internal.h
 *
 * Logical Table Manager State Internal Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_STATE_INTERNAL_H
#define BCMLTM_STATE_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_pb_local.h>

#include <bcmdrd_config.h>

#include <bcmltd/bcmltd_handler.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_ha_internal.h>


/*
 * LTM state for a single LT
 *
 * Defined within the HA yaml file.
 */

/*!
 * \brief Define format to retrieve total state of a LT
 *
 * This is the function prototype for fetching the total working state
 * of a particular Logical Table.  The state is managed within the high
 * level LTM submodule controlling metadata.  Some state-related
 * operations and tables require access to the state data of a table.
 * This callback permits access to the LT state structure.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state_p Pointer to the pointer for state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
typedef int (*bcmltm_state_lt_get_f)(int unit,
                                     uint32_t ltid,
                                     bcmltm_lt_state_t **lt_state_p);

/*!
 * \brief Define format to retrieve LT info values.
 *
 * This is the function prototype for fetching the information values of
 * a Logical Table.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [in] field_id The ID number for this TABLE_* field.
 * \param [in] field_idx The array index for this TABLE_* field.
 * \param [out] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
typedef int (*bcmltm_info_lt_get_f)(int unit,
                                    uint32_t table_id,
                                    uint32_t field_id,
                                    uint32_t field_idx,
                                    uint64_t *data_value);

/*!
 * \brief Define format to update LT valid bitmap
 *
 * This is the function prototype for updating the valid bitmap of
 * an Index w/Allocation LT.  No other Logical Table requires
 * such a bitmap.  This operation requires a number of complex
 * operations, and so must be performed at the top layer
 * of LTM.  It must be invoked from within the EE node that
 * manages TABLE_FIELD_CONTROL, so this callback is provided.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
typedef int (*bcmltm_state_lt_valid_bitmap_update_f)(int unit,
                                                     uint32_t ltid);

/*!
 * \brief Define format to retrieve an entry limit handler.
 *
 * This is the function prototype for fetching the entry limit
 * table handler of a Logical Table.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] tel Pointer to the entry limit table handler
 *                  structure for this LT.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmltm_entry_limit_get_f)(int unit,
                                        uint32_t table_id,
            const bcmltd_table_entry_limit_handler_t **tel);

/*!
 * \brief Define format to retrieve an entry usage handler.
 *
 * This is the function prototype for fetching the entry usage
 * table handler of a Logical Table.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] tel Pointer to the entry usage table handler
 *                  structure for this LT.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmltm_entry_usage_get_f)(int unit,
                                        uint32_t table_id,
            const bcmltd_table_entry_usage_handler_t **teu);



/*!
 * \brief LTM state management information per-unit.
 *
 * The fixed parameters for the LTM state tracking system of a single unit.
 */
typedef struct bcmltm_state_mgmt_s {
    /*!
     * Maximum size of a single LT bcmltm_lt_state_t structure.
     * This is included in calculating the size of the
     * ROLLBACK_STATE HA block.
     */
    uint32_t maximum_state_size;

    /*!
     * The maximum number of LT state clones in a transaction.
     */
    uint32_t lt_trans_max;

    /*!
     * The current number of LT state clones in a transaction.
     */
    uint32_t lt_clone_count;

    /*!
     * Maximum size of a single LT's combined state data.
     * This is included in calculating the size of the
     * ROLLBACK_STATE_DATA HA block.
     */
    uint32_t maximum_state_data_size;

    /*!
     * An array of HA pointers to the space reserved to contain
     * rollback copies of bcmltm_lt_state_t structures during
     * a transaction.
     */
    bcmltm_ha_ptr_t *lt_state_rollback_chunk_hap;

    /*!
     * An array of HA pointers to the space reserved to contain
     * rollback copies of a single LT's combined state data during
     * a transaction.
     */
    bcmltm_ha_ptr_t *lt_state_data_rollback_chunk_hap;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT state for a given Logical Table.
     */
    bcmltm_state_lt_get_f lt_state_get_cb;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT info for a given Logical Table.
     */
    bcmltm_info_lt_get_f lt_info_get_cb;

    /*!
     * The registered callback from a higher level submodule to update
     * the valid bitmap of an Index w/Allocation Logical Table.
     */
    bcmltm_state_lt_valid_bitmap_update_f lt_valid_bitmap_update_cb;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LTA entry limit structure for a given Logical Table.
     */
    bcmltm_entry_limit_get_f entry_limit_get_cb;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LTA entry usage structure for a given Logical Table.
     */
    bcmltm_entry_usage_get_f entry_usage_get_cb;
} bcmltm_state_mgmt_t;

/*!
 * \brief Retrieve the LTM state management structure for a unit.
 *
 * The LTM state management structure contains a several critical
 * elements required for proper transaction operation.  The context for
 * these elements exist at higher layers of the LTM submodule tree,
 * so this function permits those higher layers to initialize the
 * information in the structure for use within the state management
 * domain.
 *
 * \param [in] unit Logical device id.
 *
 * \retval None
 */
extern bcmltm_state_mgmt_t *
bcmltm_state_mgmt_get(int unit);


/*!
 * \brief Register a callback for retrieving LT state.
 *
 * Some LT node functions require access to the LT state of a LT
 * other than the LTID specified in the entry structure.  Any LT
 * with a key including LTID will need this capability.  To preserve
 * the layering hierarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs working state is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state_get_cb Callback function to retrieve LT state.
 *
 * \retval None
 */
extern void
bcmltm_state_lt_get_register(int unit,
                             bcmltm_state_lt_get_f lt_state_get_cb);

/*!
 * \brief Retrieve total state of a LT.
 *
 * Fetch the total working state of a particular Logical Table.
 * This function provides an interface to the registered state
 * retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_state_p Pointer to the state for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int
 bcmltm_state_lt_get(int unit,
                     uint32_t ltid,
                     bcmltm_lt_state_t **lt_state_p);

/*!
 * \brief Register a callback for retrieving LT info values.
 *
 * Some LT node functions require access to the LT metadata of a LT
 * other than the LTID specified in the entry structure.  Any LT
 * with a key including LTID will need this capability.  To preserve
 * the layering hierarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs info values is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_info_get_cb Callback function to retrieve LT info.
 *
 * \retval None
 */
extern void
bcmltm_info_lt_get_register(int unit,
                            bcmltm_info_lt_get_f lt_info_get_cb);

/*!
 * \brief Retrieve info values for a LT.
 *
 * Fetch the specific info value of a particular Logical Table.
 * This function provides an interface to the registered info value
 * retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] table_id Target Logical Table ID.
 * \param [in] field_id The ID number for this TABLE_INFO field.
 * \param [in] field_idx The array index for this TABLE_INFO field.
 * \param [out] data_value The uint64_t pointer to return the field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int
 bcmltm_info_lt_get(int unit,
                    uint32_t table_id,
                    uint32_t field_id,
                    uint32_t field_idx,
                    uint64_t *data_value);

/*!
 * \brief Register a callback for updating LT valid bitmap.
 *
 * Creating the valid bitmap state for an Index w/Allocation
 * Logical Table requires many steps from the top layer of the LTM.
 * To preserve the layering hierarchy of the LTM submodules, a
 * higher-level function to provide a specific LTs info values
 * is registered for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_valid_bitmap_update_cb Callback function to update
 *                                       valid bitmap.
 *
 * \retval None
 */
extern void
bcmltm_state_lt_valid_bitmap_update_register(int unit,
                                    bcmltm_state_lt_valid_bitmap_update_f
                                    lt_valid_bitmap_update_cb);

/*!
 * \brief Update the valid bitmap of a Logical Table.
 *
 * Recalculate the valid bitmap of an Index w/Allocation Logical Table.
 * Using the currently configured index key field bounds, iterate
 * through every valid key field combination to create the valid
 * bitmap of tracking indexes.
 * This function provides an interface to the registered info value
 * retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] table_id Target Logical Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
extern int
bcmltm_state_lt_valid_bitmap_update(int unit,
                                    uint32_t table_id);

/*!
 * \brief Register a callback for retrieving LTA entry limit structure.
 *
 * Some LT node functions require access to the LTA entry limit
 * handler of a LT other than the LTID specified in the entry structure.
 * TABLE_CONTROL and TABLE_INFO need this ability.  To preserve
 * the layering hierarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs table handlers is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id
 * \param [in] entry_limit_get_cb Callback function to retrieve entry
 *                                limit LTA handler.
 *
 * \retval None
 */
extern void
bcmltm_entry_limit_get_register(int unit,
                                bcmltm_entry_limit_get_f entry_limit_get_cb);

/*!
 * \brief Register a callback for retrieving LTA entry usage structure.
 *
 * Some LT node functions require access to the LTA entry usage
 * handler of a LT other than the LTID specified in the entry structure.
 * TABLE_CONTROL and TABLE_INFO need this ability.  To preserve
 * the layering hierarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs table handlers is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id
 * \param [in] entry_usage_get_cb Callback function to retrieve entry
 *                                usage LTA handler.
 *
 * \retval None
 */
extern void
bcmltm_entry_usage_get_register(int unit,
                                bcmltm_entry_usage_get_f entry_usage_get_cb);

/*!
 * \brief Retrieve LTA entry limit structure.
 *
 * Fetch the LTA entry limit handler of a particular Logical Table.
 * This function provides an interface to the registered LTA structure
 * retrieval callback which may be used by other LTM submodules.
 *
 * If no LTA entry limit handler exists for this LT, the function
 * provides a NULL pointer without error.
 *
 * \param [in] unit Logical device id.
 * \param [in] table_id Target Logical Table ID.
 * \param [out] table_entry_limit Pointer to the entry limit handler
 *                                for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 */
 extern int
 bcmltm_entry_limit_get(int unit,
                        uint32_t table_id,
                        const bcmltd_table_entry_limit_handler_t
                        **table_entry_limit);

/*!
 * \brief Retrieve LTA entry usage structure.
 *
 * Fetch the LTA entry usage handler of a particular Logical Table.
 * This function provides an interface to the registered LTA structure
 * retrieval callback which may be used by other LTM submodules.
 *
 * If no LTA entry usage handler exists for this LT, the function
 * provides a NULL pointer without error.
 *
 * \param [in] unit Logical device id.
 * \param [in] table_id Target Logical Table ID.
 * \param [out] table_entry_usage Pointer to the entry usage handler
 *                                for this LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 */
 extern int
 bcmltm_entry_usage_get(int unit,
                        uint32_t table_id,
                        const bcmltd_table_entry_usage_handler_t
                        **table_entry_usage);

/*!
 * \brief Retrieve specific state of a LT
 *
 * Search a Logical Table's LT state records for the particular
 * category of state data.
 * Return a pointer to the data if it exists, an error if not.
 * Internally this will traverse the HA pointers, but the input
 * and output are both live pointers.  This is safe because this
 * function is used only within the LT node functions.  The HA pointer
 * to LT state or rollback state are resolved in the encompassing
 * tree wrappers.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] state_type Category of state data.
 * \param [out] state_data Pointer to state data of selected type
 *                         for the Logical Table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int
bcmltm_state_data_get(int unit,
                      bcmltm_lt_state_t *lt_state,
                      bcmltm_state_type_t state_type,
                      void **state_data);

/*!
 * \brief Retrieve size of specific state of a LT
 *
 * Search a Logical Table's LT state records for the particular
 * category of state data.
 * Return a pointer to the data size if it exists, an error if not.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] state_type Category of state data.
 * \param [out] state_data_size Pointer to state data size of selected type
 *                              for the Logical Table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int
bcmltm_state_data_size_get(int unit,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_state_type_t state_type,
                           uint32_t *state_data_size);

/*!
 * \brief Duplicate the state of a LT
 *
 * Copy the source LT state to the destination LT state.
 * The destination should already be allocated to be of sufficient
 * size to hold all of the chained state.  This calculation is
 * performed by bcmltm_state_size.  Further, the linkages between the
 * structures must already be populated, and match in size.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state_src_hap LT state source.
 * \param [in,out] lt_state_dst_hap LT state destination.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int
bcmltm_state_copy(int unit,
                  bcmltm_ha_ptr_t lt_state_src_hap,
                  bcmltm_ha_ptr_t lt_state_dst_hap);

/*!
 * \brief Duplicate the state of a LT
 *
 * Create a copy the source LT state.
 * The destination will be allocated and returned in the provided
 * destination pointer.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_idx LT record number within this transaction.
 * \param [in] lt_state_src_hap LT state source.
 * \param [out] lt_state_dst_hap_p Pointer to the LT state destination.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No state of this type found for this LT.
 */
extern int
bcmltm_state_clone(int unit,
                   uint8_t lt_idx,
                   bcmltm_ha_ptr_t lt_state_src_hap,
                   bcmltm_ha_ptr_t *lt_state_dst_hap_p);

/*!
 * \brief Initialize handle for PT status IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_pt_info_init(int unit);

/*!
 * \brief Cleanup handle for PT status IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_pt_info_cleanup(int unit);

/*!
 * \brief Initialize handle for PT status IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_dop_info_init(int unit);

/*!
 * \brief Cleanup handle for PT status IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_table_op_dop_info_cleanup(int unit);

/*!
 * \brief Initialize handle for DEVICE_OP_PT_INFO IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op_pt_info_init(int unit);

/*!
 * \brief Cleanup handle for DEVICE_OP_PT_INFO IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op_pt_info_cleanup(int unit);

/*!
 * \brief Initialize handle for DEVICE_OP_DSH_INFO IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op_dsh_info_init(int unit);

/*!
 * \brief Cleanup handle for DEVICE_OP_DSH_INFO IMM operations.
 *
 * \param [in] unit Logical device ID.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_device_op_dsh_info_cleanup(int unit);

/*!
 * \brief Print to CLI the state of a single LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \return Nothing
 */
extern void
bcmltm_state_lt_dump(int unit,
                     bcmltm_lt_state_t *lt_state,
                     shr_pb_t *pb);

/*!
 * \brief Initialize PT tainted state.
 *
 * \param [in] unit Logical device ID.
 * \param [in] warm Indicates if this is a warm or cold boot.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_dev_ptstat_init(int unit, bool warm);

/*!
 * \brief Update device state for a successful tainted PT operation.
 *
 * If the PT operation taints the table content, this function would
 * update the device state.
 *
 * \param [in] unit Logical device ID.
 * \param [in] ptid Physical Table ID.
 * \param [in] pt_op PT passthrough operation type.
 * \param [in] op_failure The operation is failed or not.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int
bcmltm_dev_ptstat_update(int unit, uint32_t ptid,
                         bcmlt_pt_opcode_t pt_op, bool op_failure);

#endif /* BCMLTM_STATE_INTERNAL_H */
