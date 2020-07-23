/*! \file issu_internal.h
 *
 * Internal definitions shared between ISSU modules.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ISSU_INTERNAL_H
#define ISSU_INTERNAL_H
#include <bcmissu/issu_types.h>
#include <sal/sal_mutex.h>

/*!
 * \brief Definition of no unit
 */
#define BCMISSU_NO_UNIT (-1)

/*!
 * \brief Control block for blocks that manages the components structure info.
 */
typedef struct issu_comp_s_ctrl_blk_s {
    /*! Every block starts with signature */
    uint32_t signature;
    /*! Number of elements already filled in this block. */
    uint32_t num_of_elements;
    /*! The number of elements this block may contain. */
    uint32_t block_size;
    /*! Indicate if last block. */
    bool last;
} bcmissu_comp_s_ctrl_blk_t;

/*!
 * \brief Maximal string length for software signature.
 */
#define BCMISSU_SW_SIGNATURE_MAX_LEN    64

/*!
 * \brief General control block structure.
 *
 * This data structure will be stored in a single HA block to maintain general
 * elements of s/w. For example maintain the s/w signature value.
 */
typedef struct bcmissu_general_ctrl_s {
    /*! Software signature string */
    char software_signature[BCMISSU_SW_SIGNATURE_MAX_LEN];
} bcmissu_general_ctrl_t;

/*!
 * \brief Component structure information structure.
 *
 * This structure use to trace the location of the components data structure.
 * This information is only used in case of ISSU.
 */
typedef struct {
    bcmissu_struct_id_t struct_id;  /*!< Unique structure ID. */
    shr_ha_mod_id comp_id;    /*!< The block component ID containing the structure. */
    shr_ha_sub_id sub_id;     /*!< The bock sub ID containing the structure */
    uint16_t instances; /*!< Number of instances of the structure, >1: arrays */
    uint32_t offset;    /*!< The location of the structure within the block. */
    uint32_t data_size; /*!< The size of the structure. */
    /*!
     * The maximal data size of this structure from the start to current
     * version. Upon the first pass of resizing each instance of this
     * structure will be copied into a size matches this parameter.
     */
    uint32_t max_data_size;
    /*!
     * At the end of the process we expect that the structures will occupy
     * exactly the size of bytes as their current size. If the structure had
     * grown it is not a problem. However, if the structure shrunk then there
     * will be spaces between multiple instances of this structure (when it
     * is reported with multiple instances). For this case we maintain the
     * data size of the current version. After the upgrade we reduce the space
     * between the structure elements within an array of structures.
     */
    uint32_t last_ver_data_size;
} bcmissu_comp_s_info_t;

/*!
 * \brief Structure to keep the total increment size for HA block
 *
 * This structure is used to accumulate all the size increments that are a
 * result of a data structure size change. When a data structure size changes
 * (due to changes in definitions) the HA block containing the structure should
 * increase its size by the same amount. Often, there are several data
 * structures within a single HA block that can change their size. The purpose
 * of this data structure is to maintain the total size change required so the
 * reallocation of the HA block need to be done only once.
 */
typedef struct bcmissu_ha_blk_inc_s {
    shr_ha_mod_id comp_id;     /*!< The module ID associated with the HA block */
    shr_ha_sub_id sub_id;      /*!< The sub-module ID associated with the HA block */
    uint32_t orig_size;  /*!< The original block size */
    uint8_t *blk;        /*!< The block pointer */
    uint32_t inc;        /*!< The total size increment for this block */
    struct bcmissu_ha_blk_inc_s *next; /*!< To form a linked list */
} bcmissu_ha_blk_inc_t;

/*!
 * \brief DLL API functions definition.
 *
 * The following function type are used to define the shared library API
 * functions.
 * The shared library API only provides a pointer to a given
 * variable/function (based on the string provided). These provided pointers
 * must be typed cast to the proper function prototype so the call to the
 * DLL functions will be properly done. For this reason we are defining these
 * function prototypes.
 */

/*!
 * \brief ISSU shared library type - set the ISSU version parameters.
 *
 * This function marked the start and destination s/w versions
 *
 * \param [in] from_ver The software version to start from.
 * \param [in] to_ver The destination software version.
 * \param [in] native Indicates if the upgrade should be perfromed within the
 * system manager context or a native context.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
typedef int (*bcmissu_version_udt_f)(const char *from_ver,
                                     const char *to_ver,
                                     bool native);

/*!
 * \brief ISSU shared library type - resize function.
 *
 * This function resizes the HA blocks for structures that changed their size.
 *
 * \param [in] unit The unit to perform the resizing operation.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
typedef int (*bcmissu_data_resize_f)(int unit);

/*!
 * \brief ISSU shared library type - data upgrade function.
 *
 * This function updates the data structures of structures that were changed
 * between the two given versions.
 *
 * \param [in] unit The unit to perform the data upgrade operation.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
typedef int (*bcmissu_data_upgrade_f)(int unit);

/*!
 * \brief ISSU shared library type - old LTID to new.
 *
 * This function converts LTID from the start version to the current version
 * LTID. Look at \ref bcmissu_version_udt_f to see the two versions.
 *
 * \param [in] unit The unit to perform the LTID conversion.
 * \param [in] old_ltid LTID representing a LT in the start version.
 * \param [out] current_ltid LTID of the current version representing the same
 * LT that the \c old_ltid represnted in the start version.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
typedef int (*bcmissu_ltid_to_current_f)(int unit,
                                         uint32_t old_ltid,
                                         uint32_t *current_ltid);

/*!
 * \brief ISSU shared library type - old field ID to new.
 *
 * This function converts field ID from the start version to the current
 * version.
 *
 * \param [in] unit The unit to perform the field ID conversion.
 * \param [in] old_ltid LTID representing a LT in the start version.
 * \param [in] old_flid Field ID representing a field of the LT in the start
 * version.
 * \param [out] current_flid Field ID of the same field in the current version.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
typedef int (*bcmissu_fid_to_current_f)(int unit,
                                        uint32_t old_ltid,
                                        uint32_t old_flid,
                                        uint32_t *current_flid);

/*!
 * \brief ISSU shared library type - check if enum definition had changed.
 *
 * This function check if an enum type definition had changed from start
 * version.
 *
 * \param [in] unit The unit associated with the enum type.
 * \param [in] enum_type_name The enum type name to check.
 *
 * \return true if enum definition had changed and false otherwise.
 */
typedef bool (*bcmissu_dll_enum_changed_f)(int unit,
                                           const char *enum_type_name);

/*!
 * \brief ISSU shared library type - Convert old enum value to current value.
 *
 * This function converts the enum value of a given enum type to its
 * current value.
 *
 * It is recommended that the caller will first call \ref
 * bcmissu_dll_enum_has_changed() before calling this function for
 * multiple instances of the same enum definition.
 *
 * \param [in] unit The unit associated with this enum type.
 * \param [in] enum_type_name The enum type name.
 * \param [in] old_val The older value of the enum symbol.
 * \param [out] current_val The current value of the enum symbol.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND The original enum record was not found.
 * \return SHR_E_PARAM Invalid input parameter.
 */
typedef int (*bcmissu_dll_enum_val_to_current_f)(
        int unit,
        const char *enum_type_name,
        uint32_t old_val,
        uint32_t *current_val);

/*!
 * \brief Check if an LT description had changed.
 *
 * This function checks if the LT definition had changed. Change includes
 * different fields or change in field ID. It doesn't include changes
 * to a field type.
 *
 * \param [in] unit The unit containing the LT.
 * \param [in] ltid The original LTID.
 *
 * \return TRUE if LT definitions had changed, FALSE otherwise.
 */
typedef bool (*bcmissu_dll_lt_has_changed_f)(int unit, uint32_t ltid);

/*!
 * \brief Set the ISSU version parameters.
 *
 * This function marked the start and destination s/w versions. It also
 * records if the upgrade is done in a native or system manager (SDKLT) context.
 * For SDKLT the upgrade also contains LTID migration and specific component
 * migration callbacks.
 *
 * \param [in] from_ver The software version to start from.
 * \param [in] to_ver The destination software version.
 * \param [in] native Indicates if the upgrade should be perfromed within the
 * system manager context or a native context.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_version_update(const char *from_ver,
                                  const char *to_ver,
                                  bool native);

/*!
 * \brief Resize function.
 *
 * This function resizes the HA blocks for structures that changed their size.
 *
 * \param [in] unit The unit to perform the resizing operation.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_data_resize(int unit);

/*!
 * \brief Data upgrade function.
 *
 * This function updates the data structures of structures that were changed
 * between the two given versions.
 *
 * \param [in] unit The unit to perform the data upgrade operation.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_data_upgrade(int unit);

/*!
 * \brief Data upgrade function.
 *
 * This function updates the data structures of structures that were changed
 * between the two given versions.
 *
 * \param [in] unit The unit to perform the data upgrade operation.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_comp_upgrade(int unit);

/*!
 * \brief Old LTID to new.
 *
 * This function converts LTID from the start version to the current version
 * LTID. Look at \ref bcmissu_version_update to see the two versions.  The
 * function is part of the ISSU DLL API.
 *
 * \param [in] unit The unit to perform the LTID conversion.
 * \param [in] old_ltid LTID representing a LT in the start version.
 * \param [out] current_ltid LTID of the current version representing the same
 * LT that the \c old_ltid represnted in the start version.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_dll_ltid_to_current(int unit,
                                       uint32_t old_ltid,
                                       uint32_t *current_ltid);

/*!
 * \brief Old field ID to new.
 *
 * This function converts field ID from the start version to the current
 * version. The function is part of the ISSU DLL API.
 *
 * \param [in] unit The unit to perform the field ID conversion.
 * \param [in] old_ltid LTID representing a LT in the start version.
 * \param [in] old_flid Field ID representing a field of the LT in the start
 * version.
 * \param [out] current_flid Field ID of the same field in the current version.
 *
 * \return SHR_E_NONE success. Error code otherwise.
 */
extern int bcmissu_dll_fid_to_current(int unit,
                                      uint32_t old_ltid,
                                      uint32_t old_flid,
                                      uint32_t *current_flid);

/*!
 * \brief Check if an LT definition had change.
 *
 * This function checks if the LT definition had changed. Change includes
 * different fields or change in field ID. It doesn't include changes
 * to a field type.
 *
 * \param [in] unit The unit containing the LT.
 * \param [in] ltid The original LTID.
 *
 * \return TRUE if LT definitions had changed, FALSE otherwise.
 */
extern bool bcmissu_dll_lt_has_changed(int unit, uint32_t ltid);

/*!
 * \brief Check if enum definition had changed.
 *
 * This function check if an enum type definition had changed from the start
 * to current version. The function is part of the ISSU DLL API.
 *
 * \param [in] unit The unit associated with the enum type.
 * \param [in] enum_type_name The enum type name to check.
 *
 * \return true if enum definition had changed and false otherwise.
 */
extern bool bcmissu_dll_enum_has_changed(int unit, const char *enum_type_name);

/*!
 * \brief Convert old enum value to current value.
 *
 * This function converts the enum value of a given enum type to its
 * current value. The function is part of the ISSU DLL API.
 *
 * It is recommended that the caller will first call \ref
 * bcmissu_dll_enum_has_changed() before calling this function for
 * multiple instances of the same enum definition.
 *
 * \param [in] unit The unit associated with this enum type.
 * \param [in] enum_type_name The enum type name.
 * \param [in] old_val The older value of the enum symbol.
 * \param [out] current_val The current value of the enum symbol.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND The original enum record was not found.
 * \return SHR_E_PARAM Invalid input parameter.
 */
extern int bcmissu_dll_enum_val_to_current(int unit,
                                           const char *enum_type_name,
                                           uint32_t old_val,
                                           uint32_t *current_val);


/*!
 * \brief Initializes the structure control DB.
 *
 * This function initializes the structure control DB by allocating some
 * resources that will be used in future DB operations.
 *
 * \param [in] unit The unit associated with the structure control DB.
 * \param [in] comp_id Is the component ID assigned to the ISSU component for
 * this specific unit.
 * \param [in] warm Indicating warm or cold start.
 *
 * \return The calculated size of the structure.
 */
extern int bcmissu_struct_init(int unit, shr_ha_mod_id comp_id, bool warm);

/*!
 * \brief Shut down the structure control DB.
 *
 * This function shuts down the structure control DB by freeing some
 * resources that were used in the DB operations.
 *
 * \param [in] unit The unit associated with the structure control DB.
 * \param [in] graceful Indicating graceful or abrupt shutdown.
 *
 * \return The calculated size of the structure.
 */
extern int bcmissu_struct_shutdown(int unit, bool graceful);

/*!
 * \brief Free the structure control DB array.
 *
 * This function free the memory of the structure control array. The array
 * is dynamically allocated to adjust to the number of active units.
 * This function is called when the generic system (not per unit) is being shut
 * down.
 *
 * \return None.
 */
extern void bcmissu_struct_cleanup(void);

/*!
 * \brief Find the first structure control entry associated with a structure ID.
 *
 * This function searches the structure control records for an entry that
 * is associated with a given structure ID. The function might also return
 * a hint to future searches of more entries that associated with the structure
 * ID.
 *
 * \param [in] unit Search the database associated with this unit.
 * \param [in] id The structure ID to search for.
 * \param [out] hint_struct Hint to the search engine indicating the control
 * block to start the next search.
 *
 * \return The component structure information associated with the structure ID.
 */
extern bcmissu_comp_s_info_t *bcmissu_find_first_s_info(
                                      int unit,
                                      bcmissu_struct_id_t id,
                                      void **hint_struct);

/*!
 * \brief Find the next structure control entry associated with a structure ID.
 *
 * This function continue the search started by \ref bcmissu_find_first_s_info()
 * and finds the next structure control for a given structure ID. Note that
 * structure ID is associated with a structure defintion. However, the same
 * structure might be used in multiple HA blocks, hence multiple structure
 * controls will be required to follow all the instances of a particular
 * data structure.
 *
 * \param [in] id The structure ID to search for.
 * \param [in] hint_info Hint to the search engine to indicate what was the last
 * entry that was found.
 * \param [in,out] hint_struct Hint to the search engine indicating the control
 * block to start the search from.
 *
 * \retval The component structure information associated with the structure ID.
 */
extern bcmissu_comp_s_info_t *bcmissu_find_next_s_info(
                                   bcmissu_struct_id_t id,
                                   bcmissu_comp_s_info_t *hint_info,
                                   void **hint_struct);


/*!
 * \brief Find the structure control entry based on offset.
 *
 * This function find the structure control entry by searching all the entries
 * that are using the component and sub IDs (identifies the HA block). The
 * offset is being used to identify the specific structure ID that covers the
 * offset within the HA block.
 * This function is being used when calculating the new offset of the HA
 * pointer, as the structure location might have changed.
 *
 * \param [in] unit The using to search in.
 * \param [in] comp_id The component ID owns the HA block.
 * \param [in] sub_id The component sub ID identify the HA block.
 * \param [out] offset The offset within the HA block. The function is searches
 * for the structure ID that is located inside the HA block and covers the
 * offset area.
 *
 * \retval The structure control entry.
 */
extern bcmissu_comp_s_info_t *bcmissu_find_struct_using_offset(
                                            int unit,
                                            shr_ha_mod_id comp_id,
                                            shr_ha_sub_id sub_id,
                                            uint32_t offset);
/*!
 * \brief Calculate the field offset and width within a given structure version.
 *
 * This function searches for the field within a given structure definition.
 * Once found, it sets the field width and returns the field offset.
 *
 * \param [in] s The structure definition.
 * \param [in] fid The field ID.
 * \param [out] width The desired field width.
 *
 * \retval The field offset within the given data structure.
 */
extern uint32_t bcmissu_fld_offset(const issu_struct_db_t *s,
                                   bcmissu_field_id_t fid,
                                   uint32_t *width);


/*!
 * \brief Retrieve a given field value.
 *
 * This function converts the value pointed by \c src into a field value.
 *
 * \param [in] src Pointing to the memory where the value should be retrieved
 * from.
 * \param [in] fld_len The size of the memory containing the field value.
 *
 * \retval The retrieved field value.
 */
extern uint64_t bcmissu_obtain_fld_val(uint8_t *src, uint32_t fld_len);

/*!
 * \brief Find the oldest structure definition.
 *
 * This function search for the oldest definition of a structure that matches
 * the structure ID. It start searching from the oldest version (version prior
 * to the upgrade) and continue up to the current version.
 *
 * \param [in] id The structure ID to search for.
 *
 * \retval A pointer to the structure definition on success.
 * \retval NULL on failure.
 */
extern const issu_struct_t *bcmissu_find_oldest_struct(bcmissu_struct_id_t id);

/*!
 * \brief Record each HA pointer into individual record.
 *
 * This function searches for HA pointer fields in all the data structures.
 * Once found, the function creates a record for each instances of the
 * structure. The record contains the control block of the structure instance
 * as well as the control block of the structure the pointer point to and the
 * field ID within this structure.
 *
 * \param [in] unit The unit to update the HA pointers offset.
 * \param [in] from_ver Starting version.
 * \param [in] to_ver Last version.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INTERNAL For internal errors.
 */
extern int bcmissu_ha_ptr_record_all(int unit,
                                     const char *from_ver,
                                     const char *to_ver);

/*!
 * \brief Update the values of all HA pointers.
 *
 * This function go through the list of previously recorded HA pointers and
 * update the offset values of these pointers to match with the new structure
 * location and structure type.
 *
 * \param [in] unit The unit to update the HA pointers offset.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INTERNAL For internal errors.
 */
extern int bcmissu_update_ha_ptr(int unit);

/*!
 * \brief Update the values of all LTIDs
 *
 * This function go through all the data structure to find if it has LTID as
 * a field. If it does, it updates the LTID in all the instances of this data
 * structure to the new LTID.
 *
 * \param [in] unit The unit to update the HA pointers offset.
 * \param [in] cur_ver_db The current version structure DB.
 * \param [in] start_ver A string of the starting version to upgrade from.
 * \param [in] current_ver A string of the target version for the upgrade.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INTERNAL For internal errors.
 */
extern int bcmissu_update_ltids(int unit,
                                const issu_struct_db_t *cur_ver_db,
                                const char *start_ver,
                                const char *current_ver);

/*!
 * \brief Update the values of all ENUMs
 *
 * This function goes through all the data structure to find if it has enum as
 * a field. If it does, it updates the enum value in all the instances of this
 * data structure to the new value.
 *
 * \param [in] unit The unit to update the HA pointers offset.
 * \param [in] cur_ver_db The current version structure DB.
 * \param [in] current_ver_idx The index of the current version.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INTERNAL For internal errors.
 */
extern int bcmissu_update_enums(int unit,
                                const issu_struct_db_t *cur_ver_db,
                                uint32_t current_ver_idx);

/*!
 * \brief Extends the size of a specific structure within an HA memory block.
 *
 * This function expends the space occupied by a given structure. The expansion
 * occurs by expending the HA block size and shifting the structures, following
 * the given structure, by the expansion amount.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] ha_blk_info The HA block information.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
extern int bcmissu_ha_block_extend(int unit,
                                   bcmissu_ha_blk_inc_t *ha_blk_info);


/*!
 * \brief Validates that the unit was initialized.
 *
 * \param [in] unit The unit to validate.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM on failure.
 */
extern int bcmissu_validate_unit(int unit);

/*!
 * \brief Update the size of a particular structure.
 *
 * This function calculates if the size of a specific data structure had
 * been changed. The function only runs of structures that were changed (but
 * their size might stayed the same or shrunk).
 * The function examine the size of the structure within the context of the
 * HA block it resides as the size of a structures containing unbounded arrays
 * is dependent on the content of the structure (the field identifies the
 * array size).
 * The function is looking for all the instances of the given structure. The
 * structure ID is given by the input parameter \c from.
 * Once a structure size being updated the function also update the size
 * adjustment required for the HA block containing the structure instance.
 *
 * \param [in] unit The unit associated with the size upgrade.
 * \param [in] from Contains information about the structure to update. In
 * particular the field from->id is the identifier of the structure.
 * \param [in] ver_idx Indicates the starting version index. The update should
 * start from this version on.
 * \param [out] blk_list Is a link list containing all the HA block IDs that
 * their size should be modified as a result of a structure size increase.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_MEMORY Insufficient memory resources or HA block was not found.
 */
extern int bcmissu_size_update(int unit,
                               const issu_struct_db_t *from,
                               uint32_t ver_idx,
                               bcmissu_ha_blk_inc_t **blk_list);

/*!
 * \brief Update the structure data from old to new definition.
 *
 * This function migrates the data of a structure from old to newer definition.
 * The function will update all the instances of this data structure. The
 * upgrade will be done for only one version.
 *
 * \param [in] unit The unit associated with the upgrade.
 * \param [in] from Contains information about the structure to update. In
 * particular the field from->id is the identifier of the structure.
 * \param [in] ver_idx Is the index of the 'from' version.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_NOT_FOUND The structure is already at its highest version.
 */
extern int bcmissu_struct_upgrade(int unit,
                                  const issu_struct_db_t *from,
                                  uint32_t ver_idx);

/*!
 * \brief Compress structure array if structure had size reduction.
 *
 * This function compresses an array of structures that had size reduction.
 * This enables the components to access these structures as true array of
 * structures.
 * If a structure within an array was reduced in size the ISSU resize process
 * still maintains the maximal size of the structure for each element if the
 * array. That is required since the data upgrade procedure goes through all
 * the intermediate versions and adjust the structure content for each version.
 * If the final version of the structure is smaller in size then all the fields
 * of the structure will be at the start of the space, leaving the end of the
 * slot for the structure empty. From the component perspective these
 * structures should behave like an array so no gaps allowed.
 * This function will only operates on array structures that their final size
 * is smaller then their slot size.
 *
 * \param [in] unit The unit associated with the upgrade.
 * \param [in] from Contains information about the structure to update. In
 * particular the field from->id is the identifier of the structure.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_MEMORY Failed to allocate the HA block.
 */
extern int bcmissu_struct_array_comp(int unit, const issu_struct_db_t *from);

/*!
 * \brief Get the global unit synchronization object.
 *
 * \return Global unit synchronization mutex.
 */
extern sal_mutex_t bcmissu_unit_sync_obj_get(void);

/*!
 *\brief Get the instances of the field (i.e. array size).
 *
 * This function examines the field attributes. If the field is of type array
 * it further checks if it is a fixed array or variable size array. For variable
 * size array it reads the content of the field that indicates the array size.
 * The function returns the array size (if array) or 1 for scalar fields.
 *
 * \param [in] fld Is the field to examine.
 * \param [in] s_db Is the structure DB. It might be used to find the location
 * and width of the variable size field. This is not the general version
 * structure DB array but rather a pointer to the structure to query about its
 * field instance count.
 * \param [in] s_loc Is the location of the original structure.
 * \param [in] orig_cnt When checking the destination field the location of the
 * original structure is not relevant anymore (and therefore = NULL). In this
 * case the value from the original structure is provided (and being used as
 * the count). This field is only relevant for variable size arrays.
 *
 * \return The array size or 1 for scalar fields.
 */
extern uint64_t bcmissu_get_fld_ins(const issu_field_t *fld,
                                    const issu_struct_db_t *s_db,
                                    uint8_t *s_loc,
                                    uint64_t orig_cnt);

/*!
 *\brief Invoke component patch callback.
 *
 * This function receives a vector of h/w patch callback functions. It then
 * invokes every callback function of the vector.
 *
 * \param [in] unit The unit associated with the upgrade.
 * \param [in] patches The patches vector.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_apply_patches(int unit,
                                 const bcmissu_comp_patch_hdl_t *patches);

#endif /* ISSU_INTERNAL_H */
