/*! \file bcmlt.h
 *
 * BCM Logical Table API.
 */
/*
 * Apache License
 * Version 2.0, January 2004
 * http://www.apache.org/licenses/
 * TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
 * 1. Definitions.
 * "License" shall mean the terms and conditions for use, reproduction, and 
 * distribution as defined by Sections 1 through 9 of this document.
 * "Licensor" shall mean the copyright owner or entity authorized by the 
 * copyright owner that is granting the License.
 * "Legal Entity" shall mean the union of the acting entity and all other 
 * entities that control, are controlled by, or are under common control with 
 * that entity. For the purposes of this definition, "control" means (i) the 
 * power, direct or indirect, to cause the direction or management of such 
 * entity, whether by contract or otherwise, or (ii) ownership of fifty 
 * percent (50%) or more of the outstanding shares, or (iii) beneficial 
 * ownership of such entity.
 * "You" (or "Your") shall mean an individual or Legal Entity exercising 
 * permissions granted by this License.
 * "Source" form shall mean the preferred form for making modifications, 
 * including but not limited to software source code, documentation source, 
 * and configuration files.
 * "Object" form shall mean any form resulting from mechanical transformation 
 * or translation of a Source form, including but not limited to compiled 
 * object code, generated documentation, and conversions to other media types.
 * "Work" shall mean the work of authorship, whether in Source or Object form, 
 * made available under the License, as indicated by a copyright notice that 
 * is included in or attached to the work (an example is provided in the 
 * Appendix below).
 * "Derivative Works" shall mean any work, whether in Source or Object form, 
 * that is based on (or derived from) the Work and for which the editorial 
 * revisions, annotations, elaborations, or other modifications represent, as 
 * a whole, an original work of authorship. For the purposes of this License, 
 * Derivative Works shall not include works that remain separable from, or 
 * merely link (or bind by name) to the interfaces of, the Work and Derivative 
 * Works thereof.
 * "Contribution" shall mean any work of authorship, including the original 
 * version of the Work and any modifications or additions to that Work or 
 * Derivative Works thereof, that is intentionally submitted to Licensor for 
 * inclusion in the Work by the copyright owner or by an individual or Legal 
 * Entity authorized to submit on behalf of the copyright owner. For the 
 * purposes of this definition, "submitted" means any form of electronic, 
 * verbal, or written communication sent to the Licensor or its 
 * representatives, including but not limited to communication on electronic 
 * mailing lists, source code control systems, and issue tracking systems that 
 * are managed by, or on behalf of, the Licensor for the purpose of discussing 
 * and improving the Work, but excluding communication that is conspicuously 
 * marked or otherwise designated in writing by the copyright owner as "Not a 
 * Contribution."
 * "Contributor" shall mean Licensor and any individual or Legal Entity on 
 * behalf of whom a Contribution has been received by Licensor and 
 * subsequently incorporated within the Work.
 * 2. Grant of Copyright License. Subject to the terms and conditions of this 
 * License, each Contributor hereby grants to You a perpetual, worldwide, 
 * non-exclusive, no-charge, royalty-free, irrevocable copyright license to 
 * reproduce, prepare Derivative Works of, publicly display, publicly perform, 
 * sublicense, and distribute the Work and such Derivative Works in Source or 
 * Object form.
 * 3. Grant of Patent License. Subject to the terms and conditions of this 
 * License, each Contributor hereby grants to You a perpetual, worldwide, 
 * non-exclusive, no-charge, royalty-free, irrevocable (except as stated in 
 * this section) patent license to make, have made, use, offer to sell, sell, 
 * import, and otherwise transfer the Work, where such license applies only to 
 * those patent claims licensable by such Contributor that are necessarily 
 * infringed by their Contribution(s) alone or by combination of their 
 * Contribution(s) with the Work to which such Contribution(s) was submitted. 
 * If You institute patent litigation against any entity (including a 
 * cross-claim or counterclaim in a lawsuit) alleging that the Work or a 
 * Contribution incorporated within the Work constitutes direct or 
 * contributory patent infringement, then any patent licenses granted to You 
 * under this License for that Work shall terminate as of the date such 
 * litigation is filed.
 * 4. Redistribution. You may reproduce and distribute copies of the Work or 
 * Derivative Works thereof in any medium, with or without modifications, and 
 * in Source or Object form, provided that You meet the following conditions:
 * You must give any other recipients of the Work or Derivative Works a copy 
 * of this License; and
 * You must cause any modified files to carry prominent notices stating that 
 * You changed the files; and
 * You must retain, in the Source form of any Derivative Works that You 
 * distribute, all copyright, patent, trademark, and attribution notices from 
 * the Source form of the Work, excluding those notices that do not pertain to 
 * any part of the Derivative Works; and
 * If the Work includes a "NOTICE" text file as part of its distribution, then 
 * any Derivative Works that You distribute must include a readable copy of 
 * the attribution notices contained within such NOTICE file, excluding those 
 * notices that do not pertain to any part of the Derivative Works, in at 
 * least one of the following places: within a NOTICE text file distributed as 
 * part of the Derivative Works; within the Source form or documentation, if 
 * provided along with the Derivative Works; or, within a display generated by 
 * the Derivative Works, if and wherever such third-party notices normally 
 * appear. The contents of the NOTICE file are for informational purposes only 
 * and do not modify the License. You may add Your own attribution notices 
 * within Derivative Works that You distribute, alongside or as an addendum to 
 * the NOTICE text from the Work, provided that such additional attribution 
 * notices cannot be construed as modifying the License. 
 * 
 * You may add Your own copyright statement to Your modifications and may 
 * provide additional or different license terms and conditions for use, 
 * reproduction, or distribution of Your modifications, or for any such 
 * Derivative Works as a whole, provided Your use, reproduction, and 
 * distribution of the Work otherwise complies with the conditions stated in 
 * this License.
 * 5. Submission of Contributions. Unless You explicitly state otherwise, any 
 * Contribution intentionally submitted for inclusion in the Work by You to 
 * the Licensor shall be under the terms and conditions of this License, 
 * without any additional terms or conditions. Notwithstanding the above, 
 * nothing herein shall supersede or modify the terms of any separate license 
 * agreement you may have executed with Licensor regarding such Contributions.
 * 6. Trademarks. This License does not grant permission to use the trade 
 * names, trademarks, service marks, or product names of the Licensor, except 
 * as required for reasonable and customary use in describing the origin of 
 * the Work and reproducing the content of the NOTICE file.
 * 7. Disclaimer of Warranty. Unless required by applicable law or agreed to 
 * in writing, Licensor provides the Work (and each Contributor provides its 
 * Contributions) on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 * KIND, either express or implied, including, without limitation, any 
 * warranties or conditions of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or 
 * FITNESS FOR A PARTICULAR PURPOSE. You are solely responsible for 
 * determining the appropriateness of using or redistributing the Work and 
 * assume any risks associated with Your exercise of permissions under this 
 * License.
 * 8. Limitation of Liability. In no event and under no legal theory, whether 
 * in tort (including negligence), contract, or otherwise, unless required by 
 * applicable law (such as deliberate and grossly negligent acts) or agreed to 
 * in writing, shall any Contributor be liable to You for damages, including 
 * any direct, indirect, special, incidental, or consequential damages of any 
 * character arising as a result of this License or out of the use or 
 * inability to use the Work (including but not limited to damages for loss of 
 * goodwill, work stoppage, computer failure or malfunction, or any and all 
 * other commercial damages or losses), even if such Contributor has been 
 * advised of the possibility of such damages.
 * 9. Accepting Warranty or Additional Liability. While redistributing the 
 * Work or Derivative Works thereof, You may choose to offer, and charge a fee 
 * for, acceptance of support, warranty, indemnity, or other liability 
 * obligations and/or rights consistent with this License. However, in 
 * accepting such obligations, You may act only on Your own behalf and on Your 
 * sole responsibility, not on behalf of any other Contributor, and only if 
 * You agree to indemnify, defend, and hold each Contributor harmless for any 
 * liability incurred by, or claims asserted against, such Contributor by 
 * reason of your accepting any such warranty or additional liability.
 * END OF TERMS AND CONDITIONS
 * APPENDIX: HOW TO APPLY THE APACHE LICENSE TO YOUR WORK
 * To apply the Apache License to your work, attach the following boilerplate 
 * notice, with the fields enclosed by brackets "[]" replaced with your own 
 * identifying information. (Don't include the brackets!) The text should be 
 * enclosed in the appropriate comment syntax for the file format. We also 
 * recommend that a file or class name and description of purpose be included 
 * on the same "printed page" as the copyright notice for easier 
 * identification within third-party archives.
 * Copyright [yyyy] [name of copyright owner]
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not 
 * use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
 * License for the specific language governing permissions and limitations 
 * under the License.
 */

#ifndef BCMLT_H
#define BCMLT_H

#include <sal/sal_types.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_lt_types.h>

/*! \brief Overview
 *
 * The SDK uses logical tables (LT) to enable the application to configure
 * the underlying device. Each LT is associated with a unique name.
 * A LT is composed of one or more columns and rows.
 * BCMLT is the API to manage the various LT by manipulating the field values
 * of its rows.
 * At a high level, the API interacts with four types of objects:
 * 1. Fields - these are the logical table columns. There are two types of
 *    fields: data and key. A key field (if it exists) must be used for every
 *    operation.
 * 2. Entries - these are the logical table rows. An entry is a collection of
 *    fields that form a row in a logical table.
 * 3. Transactions - these are a collection of entries that can be bundled
 *    together for a single operation. A transaction might contain entries from
 *    multiple logical tables.
 * 4. Tables - these are the logical tables.
 *
 * A simpler mode of LT operation involves the following steps:
 * a. Create an entry.
 * b. Add the fields to the entry.
 * c. Commit the entry.
 * Nothing will be changed in the state of the system until the commit operation
 * is performed. The operations prior to the commit are merely involved in
 * creating the entry container and filling it with fields.
 * The commit operation pushes the content of the entry container onto the
 * LT in the context of the commit operation code (insert, delete, update
 * or lookup).
 * Once the commit function returns successfully, the logical table has been
 * updated or searched.
 *
 * The API allows the commit operation to be performed synchronously or
 * asynchronously. Synchronous operations block until the operation has
 * completed. Asynchronous operations return immediately and will invoke an
 * application supplied callback function as the operation progresses.
 * Asynchronous operations can perform better since they allow the application
 * to submit multiple requests without waiting for their completion. With that
 * the application can achieve a higher degree of parallel processing where one
 * thread processes the transaction and queues the generated hardware
 * instructions, while the other thread (WAL receive thread) reads the hardware
 * instructions from its queue and uses them to program the h/w.
 *
 * Transactions are more sophisticated. There are two types of
 * transactions: batch and atomic.
 * A batch transaction is simply a bundle of entries that executes sequentially.
 * A failure of one entry has no impact on the activity of other entries.
 * An atomic transaction is an all or nothing type of transaction. It means that
 * all of the entry operations are successful or none of them are applied. A failed
 * atomic transaction cleans up and leaves the system state exactly as it was
 * prior to the transaction operation.
 * Similar to entries, transactions can be committed synchronously or
 * asynchronously.
 *
 * For tables, the API provides a few distinct types of operations:
 * 1. Information - that is the ability to query the structure of the table,
 * its fields, and any restrictions associated with each field.
 * The information also contains field attributes as well as the physical
 * tables that are mapped by the LT.
 * 2. Event - the application may request to be notified when a particular
 * table changes.
 * 3. Traverse - the application may traverse the entire content of a table.
 */

/*!
 * \brief Attach a device.
 *
 * This function creates a new instance of the system manager that will
 * initialize the device. The device is fully functioning once this function
 * returns successfully.
 *
 * \param [in] unit Is the unit number of the device.
 * \param [in] warm Indicates if the operation is for cold or warm attachment.
 * In cold attach the SDK resets the device to bring all its tables into
 * default state. The SDK also sets all its resource managers into the same
 * default states.
 * In warm attach, the SDK will not reset the device keeping its operation
 * uninterrupted. The SDK will synchronize it resource managers (through HA
 * memory) to match the content of the device.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_EXISTS Device is already attached.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_INIT BCMLT module was not initialized yet.
 * \retval SHR_E_MEMORY Insufficient resources.
 * \retval SHR_E_FAIL One or more components failed to initialize this unit.
 */
extern int bcmlt_device_attach(int unit, bool warm);

/*!
 * \brief Detach a device.
 *
 * This function stops and then shuts down the device by calling the system
 * manager functions with the proper instance. All of the tables (LT or PT)
 * associated with this device will not be available once the function
 * returns successfully.
 * The function will also free all the resources associated with the unit,
 * including all the transactions and entries that were allocated for this
 * unit.
 *
 * \param [in] unit Is the unit number of the device.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number or unit not attached.
 * \retval SHR_E_FAIL One or more components failed to shutdown this unit.
 */
extern int bcmlt_device_detach(int unit);

/*!
 * \brief Check if a device is attached.
 *
 * This function returns true is the unit, specified as input, is attached.
 *
 * \param [in] unit Is the unit number of the device.
 *
 * \retval True if the unit is attached and false otherwise.
 */
extern bool bcmlt_device_attached(int unit);


/*!
 * \brief Clears all inactive entries/transactions.
 *
 * This function free the resources of all the transactions and entries,
 * associated with a particular unit, that are currently not active. Active
 * transaction/entry are transactions/entries that had been committed and the
 * commit operation had not been completed.
 * Note that transactions/entries that already completed their commit processing
 * are also considered inactive and therefore will be freed.
 *
 * The purpose of this API is to enable recovery of a remote application that
 * lost connection with the SDK. Upon return, the remote application will issue
 * the API clean operation to avoid memory leaks of stale transactions/entries.
 *
 * The application must be careful not to alter an entry/transaction on one
 * thread while calling this API from another thread as there is no protection
 * between the two threads.
 *
 * \param [in] unit is the unit number of the device.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int bcmlt_api_clear(int unit);

/********************************************************************/
/*             F I E L D    F U N C T I O N A L I T Y               */
/********************************************************************/
/*!
 * \brief basic field information
 *
 * This data structure used to identify the field, its type and value.
 */
typedef struct bcmlt_field_data_info_s {
    bool       symbol; /*!< If true sym_val is valid, otherwise val is valid. */
    bool       array;  /*!< Indicating if the field is part of an array.      */
    uint32_t   idx;    /*!< If array indicating the index of the element.     */
    uint64_t   val;    /*!< data is the actual data.                          */
    const char *sym_val; /*!< name is the field name.                         */
} bcmlt_field_data_info_t;


/*! \brief Field attribute data structure.
 *
 * This information data structure provides the callers with the details of
 * a field's attributes.
 */
typedef struct bcmlt_field_def_s {
    const char              *name;      /*!< The field name.                */
    bcmlt_field_acc_t       access;     /*!< Access type of the field.      */
    uint64_t                min;        /*!< Minimum value allowed.         */
    uint64_t                max;        /*!< Maximum value allowed.         */
    uint64_t                def;        /*!< Default value for the field.   */
    const char              *sym_def;   /*!< Default symbol value.          */
    bcmlt_field_data_tag_t  dtag;       /*!< Data tag to identify type.     */
    uint32_t                width;      /*!< Number of bits in the field.   */
    uint32_t                depth;      /*!< Number of elements in the array. */
    /*! Number of scalar elements needed for field. */
    uint32_t                elements;
    bool                    key;        /*!< True if the field part of the key. */
    bool                    symbol;     /*!< True if the field is a symbol. */
} bcmlt_field_def_t;

/*!
 * \brief Add/Update a field to an entry.
 *
 * This function is used to add a field to an entry. The field can be up to
 * 64 bits in size. Use field array for larger fields (see
 * \ref bcmlt_entry_field_array_add). If the field
 * already exists within the entry its value will be updated to the
 * provided value.
 *
 * \param [in] entry_hdl Handle to the entry for adding the field.
 * \param [in] field_name The name of the field.
 * \param [in] data 64-bit value of the field to be added.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 * \retval SHR_E_NOT_FOUND The field name is not valid within the logical table.
 */
extern int bcmlt_entry_field_add(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 uint64_t data);

/*!
 * \brief Add/Update a field value to an entry using the field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_add()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for adding the field.
 * \param [in] fid The field ID. This can be obtained by using the include file
 * $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a variant this
 * ID contains the variant name and therefore the code will have to change
 * for each variant.
 * \param [in] data 64-bit value of the field to be added.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 */
extern int bcmlt_entry_field_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                       uint32_t fid,
                                       uint64_t data);

/*!
 * \brief Add/Update a symbol field to an entry.
 *
 * Symbol field values are represented as a string taken from
 * a well defined set of strings. The notion of symbols provides an
 * enumerated value for a field instead of direct integer value.
 * This function is used to add a symbol field to an entry. If the field
 * already exists within the entry its value will be updated to the
 * provided value.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol field.
 * \param [in] field_name The name of the field.
 * \param [in] data The string value of the field to be added.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name or invalid enum string.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 */
extern int bcmlt_entry_field_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                        const char *field_name,
                                        const char *data);

/*!
 * \brief Add/update a symbol field to an entry using the field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_symbol_add()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol field.
 * \param [in] fid The ID of the field. This can be obtained by using the include
 * file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a variant this
 * ID contains the variant name and therefore the code will have to change
 * for each variant.
 * \param [in] data The string value of the field to be added/modified.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name or invalid enum string.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 */
extern int bcmlt_entry_field_symbol_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                              uint32_t fid,
                                              const char *data);

/*!
 * \brief Unset a field from a table entry.
 *
 * This function is used to unset a field from an entry in a table. The field
 * would have been previously being set by calling \ref bcmlt_entry_field_add()
 * or \ref bcmlt_entry_field_symbol_add() followed by entry commit. This function
 * will set the field value on the LT to the field default value. This
 * operation might reduce the width of the entry, in particular for fields
 * that were defined using PDD.
 * It is important to note that this function will not clear the field from
 * the entry container. To do that the user should use the function
 * \ref bcmlt_entry_field_remove() which is the exact opposite of the functions
 * \ref bcmlt_entry_field_add() and \ref bcmlt_entry_field_symbol_add().
 *
 * \param [in] entry_hdl Handle to the entry for unsetting the field.
 * \param [in] field_name The name of the field to unset.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources
 * for this action.
 */
extern int bcmlt_entry_field_unset(bcmlt_entry_handle_t entry_hdl,
                                   const char *field_name);

/*!
 * \brief Get the field value from an entry.
 *
 * This function is used to get a field value from an entry. The field can be
 * up to 64 bits in size. Use field array for larger fields (see
 * \ref bcmlt_entry_field_array_get)
 *
 * \param [in] entry_hdl Handle to the entry for getting the field.
 * \param [in] field_name The name of the field.
 * \param [out] data Up to 64-bit value of the field to get.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 * \retval SHR_E_PARAM Invalid field name or wrong entry handle or data is NULL.
 */
extern int bcmlt_entry_field_get(bcmlt_entry_handle_t entry_hdl,
                                 const char *field_name,
                                 uint64_t *data);

/*!
 * \brief Get the field value from an entry using the field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_get()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for getting the field.
 * \param [in] fid Field ID of the field to obtain its data value.
 * This can be obtained by using the
 * include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a
 * variant this ID contains the variant name and therefore the code will have
 * to change for each variant.
 * \param [out] data Up to 64-bit value of the field to get.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 * \retval SHR_E_PARAM Invalid field ID or wrong entry handle or data is NULL.
 */
extern int bcmlt_entry_field_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                       uint32_t fid,
                                       uint64_t *data);

/*!
 * \brief Get a field symbol from an entry.
 *
 * This function is used to get the value of a symbol field from an entry.
 * The field is of type symbol and its value is a string. The string is one
 * in a well defined set of strings.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol field.
 * \param [in] field_name The name of the field to get.
 * \param [out] data The field value will be written into this parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 * \retval SHR_E_PARAM Invalid entry handle or field name or data is NULL.
 */
extern int bcmlt_entry_field_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                        const char *field_name,
                                        const char **data);

/*!
 * \brief Get a field symbol from an entry using field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_symbol_get()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol field.
 * \param [in] fid The ID of the field to get. This can be obtained by using the
 * include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a
 * variant this ID contains the variant name and therefore the code will have
 * to change for each variant.
 * \param [out] data The field value will be written into this parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 * \retval SHR_E_PARAM Invalid entry handle or field name or data is NULL.
 */
extern int bcmlt_entry_field_symbol_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                              uint32_t fid,
                                              const char **data);

/*!
 * \brief Add/Update a field array to an entry.
 *
 * This function is used to add a field array to an entry. Each element of
 * the field array can be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to insert two
 * elements at index 3 & 4 the application will set \c start_idx = 3 and
 * \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes.
 * If a particular entry of the array is already occupied, this function will
 * update the value with the new value provided by the \c data array.
 *
 * \param [in] entry_hdl Handle to the entry for adding the array fields.
 * \param [in] field_name The name of the field array.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of 64 bits value element. These values will be
 * set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in commit state or
 * invalid field name or data = NULL or attempt to add elements outside the
 * array boundary.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 * \retval SHR_E_NOT_FOUND The field name is not valid within the logical table.
 */
extern int bcmlt_entry_field_array_add(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       uint64_t *data,
                                       uint32_t num_of_elem);

/*!
 * \brief Add/Update a field array to an entry using field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_array_add()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for adding the array fields.
 * \param [in] fid The ID of the field array. This can be obtained by using the
 * include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a
 * variant this ID contains the variant name and therefore the code will have
 * to change for each variant.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of 64 bits value element. These values will be
 * set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in commit state or
 * invalid field name or data = NULL or attempt to add elements outside the
 * array boundary.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 * \retval SHR_E_NOT_FOUND The field name is not valid within the logical table.
 */
extern int bcmlt_entry_field_array_add_by_id(bcmlt_entry_handle_t entry_hdl,
                                             uint32_t fid,
                                             uint32_t start_idx,
                                             uint64_t *data,
                                             uint32_t num_of_elem);

/*!
 * \brief Add/Update a field symbols array to an entry.
 *
 * This is a similar function to \ref bcmlt_entry_field_array_add() except
 * that the array elements are symbols and not scalars.
 * This function is used to add a field symbols array to an entry. Each element
 * of the field array is a symbol represented by a string. This function can
 * be called multiple times with different \c start_idx values to construct a
 * complete array of elements. For example, if the application wants to insert
 * two elements at index 3 & 4 the application will set \c start_idx = 3 and
 * \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes.
 * If a particular entry of the array is already occupied, this function will
 * update the value with the new value provided by the \c data array.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol fields.
 * \param [in] field_name The name of the symbol array field.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of symbols (strings). These symbols will
 * be set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in commit state or
 * invalid field name or data = NULL or attempt to add elements outside the
 * array boundary or invalid symbol name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 * \retval SHR_E_NOT_FOUND The field name is not valid within the logical table.
 */
extern int bcmlt_entry_field_array_symbol_add(bcmlt_entry_handle_t entry_hdl,
                                              const char *field_name,
                                              uint32_t start_idx,
                                              const char **data,
                                              uint32_t num_of_elem);

/*!
 * \brief Add/Update a field symbols array to an entry using field ID.
 *
 * This is a similar function to \ref bcmlt_entry_field_array_symbol_add()
 * with the exception of using field ID instead of field name as input.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for adding the symbol fields.
 * \param [in] fid The ID of the symbol array field. This can be obtained by
 * using the include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note
 * that for a variant this ID contains the variant name and therefore the code
 * will have to change for each variant.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] data Is an array of symbols (strings). These symbols will
 * be set into the field.
 * \param [in] num_of_elem Indicates the number of array elements pointed by
 * \c data.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in commit state or
 * invalid field name or data = NULL or attempt to add elements outside the
 * array boundary or invalid symbol name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources for this
 * action.
 * \retval SHR_E_NOT_FOUND The field name is not valid within the logical table.
 */
extern int bcmlt_entry_field_array_sym_add_by_id(
        bcmlt_entry_handle_t entry_hdl,
        uint32_t fid,
        uint32_t start_idx,
        const char **data,
        uint32_t num_of_elem);

/*!
 * \brief Unset a field array from a table entry.
 *
 * This function is used to unset a field array from an entry in a table. The
 * field would have been previously set by calling
 * \ref bcmlt_entry_field_array_add() or
 * \ref bcmlt_entry_field_array_symbol_add() followed by entry commit.
 * This function will set the field array values to the field default
 * value. This operation might reduce the width of the entry, in particular for
 * fields that were defined using PDD.
 * It is important to note that this function will not clear the field from
 * the entry container. To do that the user should use the function
 * \ref bcmlt_entry_field_array_remove() which is the exact opposite of the
 * functions \ref bcmlt_entry_field_array_add() and
 * \ref bcmlt_entry_field_array_symbol_add().
 *
 * \param [in] entry_hdl Handle to the entry for unsetting the field.
 * \param [in] field_name The name of the field to unset.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] num_of_elem Indicates the maximum number of array elements to
 * unset.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry or entry in the wrong state or
 * invalid field name.
 * \retval SHR_E_MEMORY Insufficient memory to allocate resources
 * for this action.
 */
extern int bcmlt_entry_field_array_unset(bcmlt_entry_handle_t entry_hdl,
                                         const char *field_name,
                                         uint32_t start_idx,
                                         uint32_t num_of_elem);

/*!
 * \brief Remove a field array from the entry container.
 *
 * Fields are identified using the field name.
 *
 * If the field array exists in the entry container, it will be removed from the
 * entry. Note that this operation will not impact LT values or hardware
 * values. The field array is only deleted from the entry container and not from
 * the LT. To remove a field from a particular entry in the LT use
 * \ref bcmlt_entry_field_array_unset().
 *
 * \param [in] entry_hdl Handle to the entry for removing the field.
 * \param [in] field_name The name of the field to remove from the entry.
 * \param [in] start_idx Is the target array index where elements should start
 * being applied.
 * \param [in] num_of_elem Indicates the maximum number of array elements to
 * remove from the entry. All the array element from index \c start_idx to
 * \c start_idx + \c num_of_elem will be removed from the entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_array_remove(bcmlt_entry_handle_t entry_hdl,
                                          const char *field_name,
                                          uint32_t start_idx,
                                          uint32_t num_of_elem);

/*!
 * \brief Get a field array from an entry.
 *
 * This function is used to get a field array from an entry. Each element of
 * the field array may be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to get the values
 * of two elements in index 3 & 4 the application will set \c start_idx = 3
 * and \c num_of_elem = 2. Note that the array index starts from index 0.
 * The array might have holes which are filled with default values.
 * The data array will not be filled beyond the highest index that is present in
 * the entry, even if the number of elements parameter (\c num_of_elem) is
 * sufficiently large. In such a case the return element count (\c r_elem_cnt)
 * output parameter will be smaller than the number of elements.
 * For example consider an array has elements at index 1 and 4 and the caller
 * set start_idx = 0 and num_of_elem = 6. Since holes will be filled by default
 * values the function will sets the values of the elements from index 0 to 4
 * and sets r_elem_cnt = 5.
 *
 * \param [in] entry_hdl Handle to the entry for getting the array fields.
 * \param [in] field_name The name of the array field.
 * \param [in] start_idx is the target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of u64 elements. The field array
 * values will be written into the \c data array.
 * \param [in] num_of_elem Indicates the maximum number of array elements that
 * the parameter \c data can have.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the \c data array.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name or
 * data = NULL or attempt to get elements outside the array boundary.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_array_get(bcmlt_entry_handle_t entry_hdl,
                                       const char *field_name,
                                       uint32_t start_idx,
                                       uint64_t *data,
                                       uint32_t num_of_elem,
                                       uint32_t *r_elem_cnt);

/*!
 * \brief Get a field array from an entry using field ID.
 *
 * This function is similar to the function \ref bcmlt_entry_field_array_get()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for getting the array fields.
 * \param [in] fid The ID of the array field. This can be obtained by using the
 * include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note that for a
 * variant this ID contains the variant name and therefore the code will have
 * to change for each variant.
 * \param [in] start_idx is the target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of u64 elements. The field array
 * values will be written into the \c data array.
 * \param [in] num_of_elem Indicates the maximum number of array elements that
 * the parameter \c data can have.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the \c data array.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name or
 * data = NULL or attempt to get elements outside the array boundary.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_array_get_by_id(bcmlt_entry_handle_t entry_hdl,
                                             uint32_t fid,
                                             uint32_t start_idx,
                                             uint64_t *data,
                                             uint32_t num_of_elem,
                                             uint32_t *r_elem_cnt);

/*!
 * \brief Get a symbol field array from an entry.
 *
 * This function is similar to the function
 * \ref bcmlt_entry_field_array_get() except that the array is array of
 * symbols and not scalars. Each
 * element of the field array is a symbol represented by a string. This
 * function can be called multiple times with different \c start_idx values
 * to construct a complete array of elements. For example, if the application
 * wants to get the values of two elements in index 3 & 4 the application
 * will set \c start_idx = 3 and \c num_of_elem = 2. Note that the array index
 * starts from index 0.
 * The array might have holes.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol array
 * fields.
 * \param [in] field_name The name of the symbol array field.
 * \param [in] start_idx The target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of symbol elements. The field
 * array values will be written into the \c data array. Note that the data
 * content can not be modified by the application (hence it is constant).
 * \param [in] num_of_elem Indicates the maximum number of array elements
 * that can fit into the array \c data.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the data array.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name or
 * data = NULL or attempt to add elements outside the array boundary.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_array_symbol_get(bcmlt_entry_handle_t entry_hdl,
                                              const char *field_name,
                                              uint32_t start_idx,
                                              const char **data,
                                              uint32_t num_of_elem,
                                              uint32_t *r_elem_cnt);

/*!
 * \brief Get a symbol field array from an entry using field ID.
 *
 * This function is similar to the function
 * \ref bcmlt_entry_field_array_symbol_get()
 * except that it uses field ID instead of the field name as input parameter.
 * Using the field ID provides slightly better performance for the function as
 * the conversion from field name to ID is done by the caller.
 *
 * \param [in] entry_hdl Handle to the entry for getting the symbol array
 * fields.
 * \param [in] fid The ID of the symbol array field. This can be obtained by
 * using the include file $sdk/bcmltd/include/bcmltd/chip/bcmltd_id.h. Note
 * that for a variant this ID contains the variant name and therefore the code
 * will have to change for each variant.
 * \param [in] start_idx The target array index where elements should start
 * being applied.
 * \param [out] data Pointer to an empty array of symbol elements. The field
 * array values will be written into the \c data array. Note that the data
 * content can not be modified by the application (hence it is constant).
 * \param [in] num_of_elem Indicates the maximum number of array elements
 * that can fit into the array \c data.
 * \param [out] r_elem_cnt Read element count is the actual number of array
 * elements that were written into the data array.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name or
 * data = NULL or attempt to add elements outside the array boundary.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_array_sym_get_by_id(
        bcmlt_entry_handle_t entry_hdl,
        uint32_t fid,
        uint32_t start_idx,
        const char **data,
        uint32_t num_of_elem,
        uint32_t *r_elem_cnt);

/*!
 * \brief Remove a field from the entry container.
 *
 * Fields are identified using the field name.
 *
 * If the field exists in the entry container, it will be removed from the
 * entry. Note that this operation will not impact LT values or hardware
 * values. The field is only deleted from the entry container and not from
 * the LT. To remove a field from a particular entry in the LT use
 * \ref bcmlt_entry_field_unset().
 *
 * \param [in] entry_hdl Handle to the entry for removing the field.
 * \param [in] field_name The name of the field to remove from the entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 */
extern int bcmlt_entry_field_remove(bcmlt_entry_handle_t entry_hdl,
                                    const char *field_name);

/*!
 * \brief Retrieves all valid symbol names for a field.
 *
 * This function retrieves all the valid symbol names for a given field. To
 * determine the number of symbols the application can call this function
 * twice. First is with \c sym_names equal to NULL to only retrieve the number
 * of symbol names available for this field. The number will be placed in
 * the output parameter \c actual_size. For the second call, the application
 * should allocate a \c sym_names array that is large enough to hold the
 * pointers for all the names. The application should make the second call
 * with the allocated array and specify the number of available elements in
 * the array.
 *
 * \param [in] entry_hdl Handle of the entry for querying the symbol field
 * information.
 * \param [in] field_name The name of the field.
 * \param [in] sym_names_size Contains the number of names that can be written
 * into the array \c sym_names.
 * \param [out] sym_names This array to place the symbol names pointer into it.
 * \param [out] actual_size This output specifies what is the actual number of
 * symbol names available for this field.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or invalid field name or the
 * provided symbol names array size was too small.
 * \retval SHR_E_NOT_FOUND The requested field was not found in the entry.
 * \retval SHR_E_MEMORY Insufficient memory to perform this action.
 */
extern int bcmlt_entry_field_symbol_info_get(bcmlt_entry_handle_t entry_hdl,
                                             const char *field_name,
                                             uint32_t sym_names_size,
                                             const char *sym_names[],
                                             uint32_t *actual_size);

/*!
 * \brief Invalid search handle value.
 *
 * Use this constant on the first traverse of fields within an entry using the
 * function \ref bcmlt_entry_field_traverse().
 */
#define BCMLT_INVALID_FIELD_SEARCH_HDL  0xFFFFFFFF

/*!
 * \brief Traverse all the fields in an entry.
 *
 * This function traverses all the fields in an entry container. To start the
 * search the caller must set the value of the search_hdl to
 * BCMLT_INVALID_FIELD_SEARCH_HDL. After the
 * first field is retrieved, the caller should continue to use the same value of
 * \c search_hdl as it was returned before. The function uses the value of
 * \c search_hdl for its internal context.
 * The application can continue to call this function and retrieve all the
 * fields that are currently available to the entry. After the last field was
 * traversed the function returns the error SHR_E_NOT_FOUND.
 *
 * \param [in] entry_hdl Handle of the entry to perform the traverse operation.
 * \param [out] field_name Is the next traversed field name.
 * \param [out] data Is the next traversed field data and some field attributes.
 * \param [out] search_hdl Is the traverse function context and should not be
 * changed after the first call to the field traverse function. The value of
 * this field must be set to BCMLT_INVALID_FIELD_SEARCH_HDL before calling the
 * field traverse function for the first time.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND No more fields in the entry.
 * \retval SHR_E_PARAM One or more of the input parameters is invalid.
 */
extern int bcmlt_entry_field_traverse(bcmlt_entry_handle_t entry_hdl,
                                      const char **field_name,
                                      bcmlt_field_data_info_t *data,
                                      uint32_t *search_hdl);

/*!
 * \brief Returns the number of fields in an entry container.
 *
 * This function returns the number of fields that are available within the
 * entry container. Note that each element in an array will be counted.
 *
 *
 * \param [in] entry_hdl Handle of the entry to find the field count.
 * \param [out] field_count Is the number of fields available in the entry
 * container.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or field count = NULL.
 */
extern int bcmlt_entry_field_count(bcmlt_entry_handle_t entry_hdl,
                                   uint32_t *field_count);

/********************************************************************/
/*              E N T R Y   F U N C T I O N A L I T Y               */
/********************************************************************/

/*!
 * \brief Definition of invalid handle.
 */
#define BCMLT_INVALID_HDL   0

/*!
 * \brief Get entry information.
 *
 * Retrieve the information for a given entry. Field values can be retrieved
 * via other APIs (such as \ref bcmlt_entry_field_get()).
 * Typical use case of this function is to check the result of a previous
 * asynchronous commit operation (for entry or transaction). Also this function
 * can be used to query individual entry status after a batch transaction
 * commit operation was partially successful (returned SHR_E_PARTIAL).
 *
 * \param [in] entry_hdl Is the handle of the entry.
 * \param [out] entry_info Is a pointer to the entry information info. The
 * entry status will be copied into this parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry info is NULL.
 */
extern int bcmlt_entry_info_get(bcmlt_entry_handle_t entry_hdl,
                                bcmlt_entry_info_t  *entry_info);

/*!
 * \brief Allocate an entry.
 *
 * This function allocates a new table entry container for the device and table
 * type specified by the caller. The entry handle is returned in the argument.
 * Any subsequent references to the entry should be via the entry handle.
 *
 * Entry is a container of fields, fields need to be added explicitly to build
 * an entry for operation.
 *
 * The handle is only valid if the return code is successful.
 *
 * \param [in] unit The device number for which the entry is associated.
 * \param [in] table_name Unique table name(string) identifying a LT or PT.
 * \param [out] entry_hdl Pointer to return value of the allocated entry
 * handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INIT BCMLT was not initialized.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM Missing table name or entry handle = NULL.
 * \retval SHR_E_NOT_FOUND Table was not found.
 * \retval SHR_E_RESOURCE Failed to allocate new entry due to entry limit
 * restrictions (configurable parameter).
 */
extern int bcmlt_entry_allocate(int unit, const char *table_name,
                                bcmlt_entry_handle_t *entry_hdl);

/*!
 * \brief Allocate entry using entry ID.
 *
 * This function allocates a new entry container for the device and table
 * type specified by the caller. The entry handle is returned in the argument.
 * Any subsequent references to the entry should be via the entry handle.
 * This API used for high performance operation by skipping the conversion
 * of string to table ID. The caller should also specify the maximal number
 * of fields that will be used by this entry. These fields will be allocated
 * during this function. Note that for maximum performance for lookup operation
 * the number of fields should be sufficiently large to contain all the fields
 * from the entry.
 * Asynchronous operations are not allowed with this type of entry.
 *
 * Entry is a container of fields, fields need to be added explicitly to build
 * an entry for operation. For this entry fields might be added using one of
 * the field add functions bcmlt_entry_field_xxx_add() or using the field ID
 * directly with \ref bcmlt_entry_field_add_by_id().
 *
 * The handle is only valid if the return code is successful.
 *
 * \param [in] unit The device number for which the entry is associated.
 * \param [in] tid Is the table ID associated with the table name.
 * \param [in] field_cnt Is the maximal number of fields that will be used
 * throughout the lifecycle of this entry.
 * \param [out] entry_hdl Pointer to return value of the allocated entry
 * handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INIT BCMLT was not initialized.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM Missing table name or entry handle = NULL.
 * \retval SHR_E_NOT_FOUND Table was not found.
 * \retval SHR_E_RESOURCE Failed to allocate new entry due to entry limit
 * restrictions (configurable parameter).
 * \retval SHR_E_MEMORY Failed to allocate memory for the fields.
 */
extern int bcmlt_entry_allocate_by_id(int unit,
                                      uint32_t tid,
                                      uint32_t field_cnt,
                                      bcmlt_entry_handle_t *entry_hdl);

/*!
 * \brief Free allocated entry.
 *
 * This function frees an allocated entry. All allocated entries must be freed
 * once the operation associated with the entry had been concluded. Failing
 * to free allocated entries will result in memory leaks.
 *
 * After freeing an entry the application should not assume the content of
 * the entry is valid. The entry can be reused by a different thread upon
 * allocation using \ref bcmlt_entry_allocate().
 *
 * An entry involved in a synchronous commit cannot be freed while it is still
 * active (i.e. in commit state). Entry involved in asynchronous commit can be
 * freed anytime after asynchronous commit, but once freed, the application will
 * not receive any more notifications for this entry since the associated entry
 * object is freed.
 *
 * It is therefore recommended that an asynchronous entry be freed only during
 * or after receipt of the last expected notification. By doing so the
 * application can be sure to receive all the expected notifications.
 *
 * Also, entries belonging to a transaction cannot be individually freed.
 * Instead they will be freed only once the transaction is freed.
 *
 * \param [in] entry_hdl Handle to the entry to be freed.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in wrong state. During
 * synchronous commit operation entry can't be freed.
 * \retval SHR_E_NOT_FOUND Invalid entry handle.
 * \retval SHR_E_BUSY Entry is part of a transaction.
 */
extern int bcmlt_entry_free(bcmlt_entry_handle_t entry_hdl);


/*!
 * \brief Clear entry content.
 *
 * This function clears the entry from its content. The entry content will be
 * restored into the same state of the entry after calling
 * \ref bcmlt_entry_allocate().
 * All the resources that were allocated during a calls to
 * \ref bcmlt_entry_field_add() will be freed.
 * The entry must be in idle state for it to be cleared. Also the entry can't
 * be part of a transaction.
 *
 * \param [in] entry_hdl The entry handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in wrong state.
 * \retval SHR_E_BUSY Entry is part of a transaction.
 */
extern int bcmlt_entry_clear(bcmlt_entry_handle_t entry_hdl);

/*!
 * \brief Set an entry attributes.
 *
 * This function sets entry attributes. Multiple attributes can be set within
 * a single call. This function can be called multiple times to set different
 * attributes.
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [in] attrib A logical OR of all the attributes to set.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_attrib_set(bcmlt_entry_handle_t entry_hdl,
                                  uint32_t attrib);

/*!
 * \brief Clear an entry attributes.
 *
 * This function clears the entry attributes. Multiple attributes can be
 * cleared within a single call or this function can be called multiple times
 * to clear different attributes. Note that this function may clear any
 * attribute previously set via the \ref bcmlt_entry_attrib_set() function.
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [in] attrib A logical OR of all the attributes to clear.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_attrib_clear(bcmlt_entry_handle_t entry_hdl,
                                    uint32_t attrib);

/*!
 * \brief Get entry attributes.
 *
 * This function retrieves the entry attributes. Multiple attributes can be
 * obtained within a single call, if multiple were set.
 * Attributes can not be cleared directly. To clear an attribute the
 * application can use the function \ref bcmlt_entry_clear().
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [out] attrib A logical or of all the entry attributes.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or entry in wrong state.
 */
extern int bcmlt_entry_attrib_get(bcmlt_entry_handle_t entry_hdl,
                                  uint32_t *attrib);

/*!
 * \brief Unified operation code data type.
 */
typedef struct bcmlt_unified_opcode_s {
    bool pt;               /*!< Indicate which field in \c opcode is relevant.*/
    union {
        bcmlt_opcode_t lt;    /*!< opcode for logical table entries.          */
        bcmlt_pt_opcode_t pt; /*!< opcode for physical table entries.         */
    } opcode;              /*!< The opcode as a union of opcode types.        */
} bcmlt_unified_opcode_t;

/*!
 * \brief Retrieve the operation code of an entry.
 *
 * This function retrieve the operation code associated with the entry. This
 * functionality is mostly relevant in the case of capture replay operation (see
 * \ref bcmlt_capture_replay() and \ref replay_action_f()). However, it can also
 * been used on entries that were committed.
 *
 * \param [in] entry_hdl The handle associated with the entry.
 * \param [out] uni_opcode The operation code associated with this entry. The
 * caller should provide a pointer to the unified opcode output.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid entry handle or NULL output parameter.
 */
extern int bcmlt_entry_oper_get(bcmlt_entry_handle_t entry_hdl,
                                bcmlt_unified_opcode_t *uni_opcode);

/*!
 * \brief LT Entry asynchronous commit.
 *
 * This function dispatches a single entry operation for processing.
 * This method is to be used when clients prefer individual operation
 * flexibility and control. The operation is asynchronous, meaning that
 * the function will return immediately. The application can provide a callback
 * function to monitor the progress of the operation along with the stages of
 * the processing for which it would like to be notified.
 * Options for notification are: to get notified after the entry was
 * committed into PTCache, after the hardware was modified, or both. The caller
 * may also decide to receive no notification.
 *
 * The caller context may be part of the callback if the caller provide its
 * context as an opaque pointer. This same pointer will be provided as one of
 * the callback arguments.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/alloc...
 * \param [in] user_data Opaque client context.
 * \param [in] notif_opt Notification option (after commit, h/w, both or none)
 * \param [in] notif_fn Override Callback function for status update. Can be
 * NULL if no notification needed.
 * \param [in] priority Indicates normal or high priority operation. If the
 * application commits multiple entries back to back the system will queue
 * these requests up until it has finished all previous processing. Using
 * the high priority flag will place the entry at the top of the queue.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the entry
 * operation. Note that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the operation was successfully posted into the transaction manager for
 * execution. Once completed, the application should test the entry status to
 * determine if the operation was successful or not.
 * \retval SHR_E_PARAM Invalid entry handle or entry is for physical table or
 * opcode, priority is invalid or the combination of notification option and
 * notification function is invalid.
 * \retval SHR_E_NOT_FOUND Traverse operation (not the first one) has no key
 * field.
 * \retval SHR_E_MEMORY Insufficient resources to implement this operation.
 */
extern int bcmlt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                                    bcmlt_opcode_t opcode,
                                    void *user_data,
                                    bcmlt_notif_option_t notif_opt,
                                    bcmlt_entry_cb notif_fn,
                                    bcmlt_priority_level_t priority);

/*!
 * \brief LT Synchronous entry commit.
 *
 * This function is a synchronous version of the function
 * \ref bcmlt_entry_commit_async(). It will block the caller until the entry
 * had been processed or until an error was discovered.
 *
 * While the performance of this function is potentially slower, this is the
 * simplest form of committing an entry. If the calling thread has
 * many entries to process, the asynch model would provide better
 * performance. However, if it is not the case then this is the preferred method
 * due to its simplicity. Note that a few parallel threads that are using this
 * synchronous API will perform similarly to the asynch form.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/alloc...
 * \param [in] priority Not used.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the entry
 * operation.
 *
 * \code the following code shows how to use this function to traverses all
 * the entries in the VLAN table and prints out the STG ID.
 * Note that the operation should be verified by examining the entry status.
 *
 * bcmlt_entry_handle_t e_hdl;
 * uint64_t vlan_id;
 * uint64_t stg_id;
 * bcmlt_entry_info_t e_info;
 * int rv;
 *
 * rv = bcmlt_entry_allocate(0, "VLAN", &e_hdl);
 * if (rv != SHR_E_NONE) {
 *    return rv;
 * }
 *
 * while ((rv = bcmlt_entry_commit(e_hdl,
 *                                 BCMLT_OPCODE_TRAVERSE,
 *                                 BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
 *    if (bcmlt_entry_field_get(e_hdl, "VLAN_ID", &vlan_id) != SHR_E_NONE) {
 *       break;
 *    }
 *    if (bcmlt_entry_field_get(e_hdl, "VLAN_STG_ID", &stg_id) != SHR_E_NONE) {
 *       break;
 *    }
 *    printf("vlan ID: %"PRId64", stg ID: %"PRId64"\n":, vlan_id, stg_id);
 * }
 *
 * bcmlt_entry_free(e_hdl);   This will stop the search
 * \endcode
 */
extern int bcmlt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                              bcmlt_opcode_t opcode,
                              bcmlt_priority_level_t priority);


/*!
 * \brief PT Entry asynchronous commit
 *
 * This function is the PT version of \ref bcmlt_entry_commit_async(). It
 * differs from the LT version by the opcode and notification function parameters
 * but behaves similarly. The function is a pass though meaning that the only true
 * notification is h/w complete as the entry passes though the PTCache stage.
 *
 * Note that after using this function it is possible that the application can
 * not continue its normal operation unless the operations are reversed.
 * That is since the device state and the PTCache state might be different.
 *
 * This is an asynchronous API meaning that the function returns before the
 * processing occurs. The caller, if desired, should specify the notification
 * option and callback function to be notified about the progress and success/
 * failure of the operation.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/modify...
 * \param [in] user_data Opaque client context.
 * \param [in] notif_opt Notification option (after commit, h/w, both or none)
 * \param [in] notif_fn Override Callback function for status update.
 * \param [in] priority Indicates normal or high priority operation. If the
 * application commits multiple entries back to back, the system will queue
 * these requests up until it has finished all previous processing. Using
 * the high priority flag will place the entry at the top of the queue.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the entry
 * operation. Note, that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the operation was successfully posted into the transaction manager for
 * execution. Once completed, the application should test the entry status to
 * determine if the operation was successful or not.
 * \retval SHR_E_PARAM Invalid entry handle or entry is for logical table or
 * opcode, priority is invalid or the combination of notification option and
 * notification function is invalid.
 * \retval SHR_E_MEMORY Insufficient resources to implement this operation.
 */
extern int bcmlt_pt_entry_commit_async(bcmlt_entry_handle_t entry_hdl,
                                       bcmlt_pt_opcode_t opcode,
                                       void *user_data,
                                       bcmlt_notif_option_t notif_opt,
                                       bcmlt_entry_pt_cb notif_fn,
                                       bcmlt_priority_level_t priority);

/*!
 * \brief PT Synchronous entry commit.
 *
 * This function is the PT version of \ref bcmlt_entry_commit(). It
 * only differs in the opcode parameter but behaves similarly otherwise.
 * Typical use of this function is for debug purposes as this function bypasses
 * (pass though) the PTCache and access the h/w directly. The caller must have
 * good knowledge of the underlining device to use this function.
 *
 * Note that after using this function it is possible that the application can
 * not continue its normal operation unless the operations are reversed.
 * That is since the device state and the PTCache state might be different.
 *
 * This function will block until the operation had completed.
 *
 * \param [in] entry_hdl Handle to the entry.
 * \param [in] opcode Type of operations set/clear/modify...
 * \param [in] priority Unused parameter.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the entry
 * operation.
 */
extern int bcmlt_pt_entry_commit(bcmlt_entry_handle_t entry_hdl,
                                 bcmlt_pt_opcode_t opcode,
                                 bcmlt_priority_level_t priority);


/********************************************************************/
/*              T A B L E   F U N C T I O N A L I T Y               */
/********************************************************************/
/*!
 * This group of functions provide table based functionality. These are
 * functions that can operate on an entire table or tables.
 */
/*!
 * \brief Table search criteria.
 *
 * The following flags can be used to control the output of the table search
 * functions.
 * Multiple flags can be used together. For example to find all the PT and LT
 * tables the user should provide
 * BCMLT_TABLE_SEARCH_PT | BCMLT_TABLE_SEARCH_LT as input
 */
#define BCMLT_TABLE_SEARCH_PT            0x1  /*!< Search PT tables          */
#define BCMLT_TABLE_SEARCH_LT            0x2  /*!< Search logical tables     */
#define BCMLT_TABLE_SEARCH_INTERACTIVE   0x4  /*!< Search interactive tables */

/*!
 * \brief Start table enumeration.
 *
 * This function starts the retrieval of all available logical or physical
 * table names on a particular device. The function returns the name of the
 * first table in the list of tables. The caller can set the proper search
 * criteria by setting the \c flags parameter.
 *
 * To retrieve the rest of the tables, one by one, use the function
 * \ref bcmlt_table_names_get_next().
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] flags Control the search results by selecting the desired table
 * types to be enumerated.
 * \param [out] name The name of the table that was retrieved first.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM The name parameter was NULL.
 * \retval SHR_E_NOT_FOUND No table was found.
 */
extern int bcmlt_table_names_get_first(int unit,
                                       uint32_t flags,
                                       char **name);

/*!
 * \brief Continue table enumeration.
 *
 * This function continues the retrieval of all available logical or physical
 * table names on a particular device. This function should be called after a
 * call to \ref bcmlt_table_names_get_first() to retrieve the first table. The
 * caller can control the type of tables to retrieve via the \c flags parameter.
 *
 * This function returns the name of the next table.
 * Use this function again to retrieve more names.
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] flags Control the search results by selecting the desired table
 * types to be enumerated.
 * \param [out] name The name of the table that was retrieved.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_PARAM The name parameter was NULL.
 * \retval SHR_E_NOT_FOUND No more tables were found.
 */
extern int bcmlt_table_names_get_next(int unit,
                                      uint32_t flags,
                                      char **name);


/*!
 * \brief Retrieve table field definitions.
 *
 * This function retrieves all of the field information associated with
 * a particular table on a particular device
 *
 * \param [in] unit Device number for the table of interest.
 * \param [in] table_name Table name for subscribing to updates.
 * \param [in] fields_array_size Specifies the number of elements
 *              available in the array field_defs_array.
 * \param [out] field_defs_array Pointer to user provided buffer where the
 *              field definitions will be copied too. The application must
 *              set the num_fields to indicate the number
 *              of fields that can be placed within entry_hdl buffer.
 * \param [out] require_array_size Used by the function to place the required
 *              array size in term of elements
 *              (byte size = sizeof(bcmlt_field_def_t) * require_array_size)
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid table name or table was not found or the output
 * buffer is too small.
 *
 * \code the following code shows how to use this function. It calls the
 * function \c bcmlt_table_field_defs_get() twice. Once to retrieve the number
 * of fields in the table, and a second time to retrieve the field definition.
 * Assume that the unit and t_name (table name) provided to this example
 *
 * uint32_t number_of_elements;
 * bcmlt_field_def_t *buffer;
 * bcmlt_field_def_t *field_def;
 * uint32_t actual_number;
 * int rv;
 * uint32_t idx;
 * rv = bcmlt_table_field_defs_get(unit, t_name, 0, &number_of_elements);
 * if (rv != SHR_E_NONE) {
 *    return rv;
 * }
 * buffer = malloc(number_of_elements * sizeof(bcmlt_field_def_t));
 * if (!buffer) {
 *    return SHR_E_MEMORY;
 * }
 * rv = bcmlt_table_field_defs_get(unit,
 *                                 t_name,
 *                                 number_of_elements,
 *                                 buffer,
 *                                 &actual_number);
 * if ((rv != SHR_E_NONE) || (actual_number != number_of_elements)) {
 *    return SHR_E_INTERNAL;
 * }
 * for (idx = 0, field_def = buffer;
        idx < number_of_elements;
        idx++, field_def++) {
 * }
 * free (buffer);
 * return SHR_E_NONE;
 * \endcode
 */
extern int bcmlt_table_field_defs_get(int unit,
                                      const char *table_name,
                                      uint32_t    fields_array_size,
                                      bcmlt_field_def_t *field_defs_array,
                                      uint32_t *require_array_size);

/*!
 * \brief Retrieve field description of logical table.
 *
 * Retrieve the description for given field in the logical table.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name.
 * \param [in] fld_name The field name.
 * \param [out] fld_desc Field description.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM NULL table name.
 * \retval SHR_E_NOT_FOUND The table/field is not found for the device.
 * \retval SHR_E_INIT Sanity check failure.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int bcmlt_field_desc_get(int unit,
                                const char *tbl_name,
                                const char *fld_name,
                                const char **fld_desc);


/*
 * Table Notification APIs
 */

/*!
 * \brief Subscribe for table updates.
 *
 * This function registers a callback to get notified on any changes of a
 * particular table within a unit.
 * The application may provide a pointer to an application context that can be
 * used during the callback event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] table_name The table name for subscribing to updates.
 * \param [in] callback Notification handler to be invoked.
 * \param [in] user_data Application opaque context for notifications.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM invalid table name or table is physical table.
 * \retval SHR_E_NOT_FOUND Table was not found.
 */
extern int bcmlt_table_subscribe(int unit,
                                 const char *table_name,
                                 bcmlt_table_sub_cb callback,
                                 void *user_data);

/*!
 * \brief Unsubscribe for table updates.
 *
 * This function unregisters a callback to get notified on any table updates.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] table_name The table name for unsubscribing from any updates.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM invalid table name or table is physical table.
 * \retval SHR_E_NOT_FOUND Table was not found.
 */
extern int bcmlt_table_unsubscribe(int unit, const char *table_name);

/*!
 * \brief Retrieve the maximum number of physical table names.
 *
 * Retrieve the maximum number of physical table names for the LT.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name(string).
 * \param [out] pt_max_count Maximum number of physical tables.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Null table.
 * \retval SHR_E_INIT Sanity check failure.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Table not found.
 */
extern int
bcmlt_table_pt_name_max_num_get(int unit,
                                const char *tbl_name,
                                uint32_t *pt_max_count);

/*!
 * \brief Retrieve physical table names associated with a given LT.
 *
 * Retrieve the physical table information for the LT
 * which have direct physical table mapping.
 * For LTs with CTH, PT count is set to 0.
 * Upon return, the number of physical tables mapped to the LT
 * would be available in \c pt_counter_ptr.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name(string).
 * \param [in] pt_count Array size of pt_names.
 * \param [out] pt_names Pointer to the list of PT names.
 * \param [out] actual_pt_count Actual count of physical tables.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Null table name.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Table not found.
 */
extern int bcmlt_table_pt_name_get(int unit,
                                   const char *tbl_name,
                                   uint32_t pt_count,
                                   const char **pt_names,
                                   uint32_t *actual_pt_count);

/*!
 * \brief Retrieve ltid and lfid by table name and field name.
 *
 * Retrieve the logical table ID and logical field ID by
 * table name and field name.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name(string).
 * \param [in] fld_name The field name(string).
 * \param [out] ltid Logical Table ID.
 * \param [out] lfid Logical Field ID.
 *
 * \retval SHR_E_NONE  Success.
 * \retval SHR_E_PARAM Invalid table name field name or unit number.
 * \retval SHR_E_NOT_FOUND Table or field within the table was not found.
 */
extern int bcmlt_table_field_id_get_by_name(int unit,
                                            const char *tbl_name,
                                            const char *fld_name,
                                            uint32_t *ltid,
                                            uint32_t *lfid);

/*!
 * \brief Retrieve logical table description.
 *
 * Retrieve the description for given logical table.
 *
 * \param [in] unit The device number for which the entry is started.
 * \param [in] tbl_name The table name.
 * \param [out] tbl_desc Logical table description.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM NULL table name.
 * \retval SHR_E_NOT_FOUND Table is not found for device.
 * \retval SHR_E_INIT Sanity check failure.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int bcmlt_table_desc_get(int unit,
                                const char *tbl_name,
                                const char **tbl_desc);

/********************************************************************/
/*        T R A N S A C T I O N S   F U N C T I O N A L I T Y       */
/********************************************************************/
/*
 * Transaction is a collection of entries that can be bundled together.
 * The execution of the entries Will be done sequentially according to the order
 * they were inserted into the transaction.
 * There are two types of transactions: Atomic and Batch
 * A batch transaction simply stages and commits every entry in the transaction
 * according to the entry order. If an entry operation fails, the transaction
 * simply jumps to the next entry.
 * An atomic transaction is all or nothing, meaning that either all of the
 * entries are staged correctly or the system is placed in the state that
 * proceeded the call to committing the transaction. Unlike a batch transaction
 * that stages and commits each entry separately, an atomic transaction only
 * stages each entry separately but commits the entire transaction in a single
 * step.
 *
 * The operational flow of a transaction is an extension to the entry operation.
 * A typical transaction operation sequence looks as follows:
 * a. Allocate the transaction.
 * b. Allocate an entry.
 * c. Set the entry fields.
 * d. Add the entry into the transaction. The desired entry operation is
 *    specified here.
 * e. If more operations are required go back to step (b).
 * f. Commit the transaction (synchronous or asynchronous).
 * g. Check the transaction state and individual entries (if needed).
 * h. Free the transaction.
 */
/*!
 * \brief Transaction allocation.
 *
 * This function allocates a new transaction resources for a type of
 * transaction specified by the caller. The transaction handle is returned in
 * the argument.
 *
 * The handle is only valid on successful call return values.
 * The transaction is a container that may contain multiple entries.
 * Note that to prevent memory leaks the caller should free the transaction
 * once it completed its operations using the function
 * \ref bcmlt_transaction_free().
 *
 * \param [in] type The transaction type.
 * \param [out] trans_hdl Pointer to the variable to hold the transaction
 *                      handle. A valid pointer to a transaction handle
 *                      storage location is required. The contents of
 *                      entry_hdl location pointed at will be updated with
 *                      the valid transaction handle on success.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_INIT BCMLT had not been initialized.
 * \retval SHR_E_PARAM Invalid type parameter.
 * \retval SHR_E_RESOURCE Insufficient memory or the number
 * of allocated transactions exceeded the specified limit (configuration).
 */
extern int bcmlt_transaction_allocate(bcmlt_trans_type_t type,
                                      bcmlt_transaction_hdl_t *trans_hdl);


/*!
 * \brief Free the transaction.
 *
 * This function frees a previously allocated transaction. Failure to free
 * allocated transactions leads to memory leaks.
 * Synchronous transactions (i.e. transactions involved in synchronous commit)
 * can't be freed during the commit operation (even not by other thread).
 * This restriction is not applied for asynchronous commit. However, the caller
 * must understand that freeing a transaction will stop all the notifications
 * for the commit status. Therefore it is recommended that the user will not
 * free the transaction until it receives the last notification.
 *
 * \param [in] trans_hdl The handle to the transaction to be released.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid transaction handle.
 */
extern int bcmlt_transaction_free(bcmlt_transaction_hdl_t trans_hdl);


/*!
 * \brief Add an entry to a transaction.
 *
 * This function inserts a single entry operation into a transaction. The
 * transaction needs to be committed to take effect. Upon transaction commit,
 * the entries will be staged in the same order as they were added to the
 * transaction.
 * The transaction state should be idle for this function to succeed.
 * If the transaction is in commit state the operation will fail.
 *
 * The current implementation only supports entries belonging to the same
 * unit.
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] opcode The LT operations code (i.e. insert/lookup/update...).
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The transaction or entry handles are invalid.
 * May also occur if the entry is a PT entry or if the entry already belongs to
 * a transaction (same or different). This error also occurs if the op code is
 * invalid or when adding an
 * entry for an interactive table into an atomic type transaction.
 * \retval SHR_E_UNAVAIL Attempting to mix entries from different units
 * within one transaction. The transaction is automatically associated with
 * the unit of the first entry.
 */
extern int bcmlt_transaction_entry_add(bcmlt_transaction_hdl_t trans_hdl,
                                       bcmlt_opcode_t opcode,
                                       bcmlt_entry_handle_t entry_hdl);

/*!
 * \brief Add a PT entry to a transaction.
 *
 * This function is similar to the function \ref bcmlt_transaction_entry_add()
 * except that it applied for pass through entries.
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] opcode The PT operation code (i.e. set/modify/get...).
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The transaction or entry handles are invalid.
 * May also occur if the entry is a LT entry or the transaction is atomic.
 * This error also occurs if the op code is invalid.
 * \retval SHR_E_UNAVAIL Attempting to mix entries from
 * different units within the transaction. The transaction is
 * automatically associated with the unit of the first entry.
 */
extern int bcmlt_transaction_pt_entry_add(bcmlt_transaction_hdl_t  trans_hdl,
                                          bcmlt_pt_opcode_t    opcode,
                                          bcmlt_entry_handle_t  entry_hdl);

/*!
 * \brief Remove an entry from a transaction.
 *
 * This function removes a single entry operation from the transaction. This can
 * be done only on transactions in an idle state (either before committing or
 * after the transaction is committed). This allows client code to modify
 * the transaction with complete flexibility until the logic is ready to commit
 * the transaction.
 * This function is the reverse of the functions
 * \ref bcmlt_transaction_entry_add and \ref bcmlt_transaction_pt_entry_add().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] entry_hdl The entry handle that was obtained by calling
 * \ref bcmlt_entry_allocate().
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The transaction or entry handles are invalid.
 * May also occur if the entry was not found.
 */
extern int bcmlt_transaction_entry_delete(bcmlt_transaction_hdl_t  trans_hdl,
                                          bcmlt_entry_handle_t  entry_hdl);

/*!
 * \brief Get transaction information.
 *
 * Retrieve the transaction information for the given transaction.
 * Individual entries can be retrieved via the function
 * \ref bcmlt_transaction_entry_num_get().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [out] trans_info The transaction information. See
 * bcmlt_transaction_info_t for more information.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM The transaction or entry handles are invalid or the
 * output parameter trans_info is NULL.
 */
extern int bcmlt_transaction_info_get(bcmlt_transaction_hdl_t trans_hdl,
                                      bcmlt_transaction_info_t  *trans_info);


/*!
 * \brief Transaction asynchronous commit.
 *
 * This function commits a transaction for asynchronous processing.
 * On successful return from this call, clients are assured the transaction
 * is queued for processing. Since this function is asynchronous, the API
 * enables the application to trace the progress of this operation by receiving
 * a callback for the specified event (see bcmlt_trans_cb()).
 *
 * User_data can be used as context for application callback events.
 *
 * This function fails if one or more entries were allocated using the
 * function \ref bcmlt_entry_allocate_by_id().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] notif_opt Notification option for transaction.
 * \param [in] user_data  Caller context to be provided with the event callback
 * function.
 * \param [in] notif_fn  Processing progress callback function.
 * This function will be called on transaction status updates for events
 * requested with the \c notif_opt parameter.
 * \param [in] priority Indicates normal or high priority operation. High
 * priority operations are queued in front of all normal priority transactions.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the transaction.
 * Note, that this function might return before the operation has
 * been executed. Therefore, the return value of this function only indicates
 * that the transaction was successfully posted into the transaction manager
 * for execution. Once completed, the application should test the transaction
 * status to determine if the operation was successful or not (see
 * \ref bcmlt_transaction_info_get()). If the transaction status is partially
 * successful the application should check the individual entries status
 * (for batch transactions) to determine which one failed.
 * \retval SHR_E_PARAM Invalid transaction handle or empty transaction or
 * invalid priority. Also invalid combination of notification option and
 * function.
 * \retval SHR_E_MEMORY Insufficient resources to perform this operation.
 * \retval SHR_E_DISABLED One or more entries was allocated using
 * bcmlt_entry_allocate_by_id().
 */
extern int bcmlt_transaction_commit_async(bcmlt_transaction_hdl_t trans_hdl,
                                          bcmlt_notif_option_t notif_opt,
                                          void *user_data,
                                          bcmlt_trans_cb notif_fn,
                                          bcmlt_priority_level_t priority);

/*!
 * \brief Transaction synchronous commit.
 *
 * This function commits a transaction. Since this is a synchronous
 * operation the function will block until all entries in the transaction are
 * committed into the hardware.
 *
 * This function handles all types of transactions.
 *
 * Upon return, the application can query the success of the transaction
 * operation as well as the status of each entry. For more information see
 * \ref bcmlt_transaction_info_get() and \ref bcmlt_transaction_entry_num_get().
 *
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * \ref bcmlt_transaction_allocate().
 * \param [in] priority Unused.
 *
 * \retval SHR_E_NONE Success, otherwise failure in committing the transaction.
 * \retval SHR_E_PARTIAL when some of the entries were successful and some were
 * unsuccessful. When this error code is returned the application should use
 * \ref bcmlt_transaction_info_get() to obtain the number of entries with in
 * the transaction. Than the for every entry the application should call the
 * function \ref bcmlt_entry_info_get() to obtain its status.
 * \retval SHR_E_FAIL For atomic transaction failure or if all the entries
 * within a batch transaction had failed.
 * \retval SHR_E_PARAM Invalid transaction handle or empty transaction or
 * invalid priority.
 * \retval SHR_E_MEMORY Insufficient resources to perform this operation.
 */
extern int bcmlt_transaction_commit(bcmlt_transaction_hdl_t trans_hdl,
                                    bcmlt_priority_level_t priority);


/*!
 * \brief Get the n'th entry of a transaction.
 *
 * This function enables the caller to control each specific entry in a
 * transaction. By obtaining the entry handle the caller can verify the entry
 * operational status as well as to obtain fields values for lookup operations.
 *
 * \param [in] trans_hdl Handle to the transaction.
 * \param [in] entry_num Nth entry to retrieve (starting from 0).
 * The entry number is corresponding to the order that the entry was inserted
 * using the function \ref bcmlt_transaction_entry_add(). Therefore the j'th
 * entry that was inserted into the entry will be retrieved with
 * entry_num = j-1.
 * \param [out] entry_info Pointer to memory containing nth entry information.
 *
 * \retval SHR_E_NONE Success, otherwise failure in retrieving the n'th entry of
 * the transaction.
 * \retval SHR_E_PARAM Invalid transaction handle or entry_info is NULL or the
 * entry number doesn't exist.
 */
extern int bcmlt_transaction_entry_num_get(bcmlt_transaction_hdl_t trans_hdl,
                                           uint32_t entry_num,
                                           bcmlt_entry_info_t *entry_info);


/********************************************************************/
/* O P E R A T I O N    C A P T U R E    F U N C T I O N A L I T Y  */
/********************************************************************/
/*!
 * \brief Replay functions (debug).
 *
 * This set of functions can be used to enable operation capture and replay.
 * The main purpose of these functions is to be able to reproduce specific state
 * of the system for debug purposes.
 * The capture function uses the application IO functions to save each
 * commit operation in non-volatile memory.
 * The replay function will replay the captured operations in the exact order
 * (and possibly with the exact timing).
 */
/*!
 * \brief BCMLT operations capture callback structure.
 *
 * This data structure should be passed to the capture start function. The
 * purpose of this structure is to enable the capture function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular the ability to save the captured data into
 * non-volatile memory where it can be read back at a later time.
 */
typedef struct bcmlt_capture_cb_s {
    /*! File descriptor of a file with write permission */
    void *fd;
    /*!
      * Write function to write a number of bytes (nbyte) from the buffer into
      * the file using the file descriptor fd. The function returns the number
      * of bytes that were actually written.
      */
    uint32_t (*write)(void *fd, void *buffer, uint32_t nbyte);
} bcmlt_capture_cb_t;

/*!
 * \brief BCMLT operations replay callback structure.
 *
 * This data structure should be passed to the replay function. The
 * purpose of this structure is to enable the replay function to perform
 * IO operations that are outside the scope of the SDK. These operations
 * includes in particular reading the captured data.
 */
typedef struct bcmlt_replay_cb_s {
    /*! File descriptor of a file to replay with read permission */
    void *fd;
    /*!
      * Read function to read number of bytes (nbyte) from the file into
      * the buffer. The function returns the number of bytes that were
      * actually read.
      */
    uint32_t (*read)(void *fd, void *buffer, uint32_t nbyte);
} bcmlt_replay_cb_t;

/*!
 * \brief Control the operations that will be captured and replayed.
 *
 * This function can be called before or during capture or play back. It sets
 * a filter to prevent certain operations from being captured or played back.
 * The purpose of this feature is to less important operations that
 * otherwise may increase the size of the capture file.
 * It is only possible to filter operations on logical tables. All operations
 * on physical tables will be captured and replayed.
 *
 * \param [in] unit The unit number of the device for which to apply the
 * filters.
 * \param [in] filters Array of opcodes that should be filtered. A NULL
 * value indicates no filters.
 * \param [in] num_of_opcodes The number of elements in the \c filters array.
 * A value of zero will clear all the filters for this unit
 * (similar to opcodes = NULL).
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid opcode to filter.
 */
extern int bcmlt_capture_filter_set(int unit,
                                    bcmlt_opcode_t *filters,
                                    uint32_t num_of_opcodes);

/*!
 * \brief Start operation capturing.
 *
 * This function starts the capture of all the entry and transaction
 * operations for a given unit. The capture of the activity will continue
 * until the application calls \ref bcmlt_capture_stop(). After
 * that the application can play back the entire captured sequence by
 * calling \ref bcmlt_capture_replay().
 * The application should provide the file descriptor and a write function to
 * be used to store the trace information. If the file descriptor represents
 * a file the application should make sure to truncate older file content.
 * Note that the content of the file will be binary data.
 *
 * \param [in] unit The unit number of the device for which to start
 * capturing.
 * \param [in] app_cb A data structure that provides the IO
 * instrumentation required by the capture functionality. This functionality
 * is implemented by the application.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_BUSY Currently capturing or playing back the activity for
 * this unit.
 */
extern int bcmlt_capture_start(int unit, bcmlt_capture_cb_t *app_cb);

/*!
 * \brief Stop operations capture.
 *
 * This function should stop operations capturing on the specified unit.
 * The operations capture should start by calling
 * \ref bcmlt_capture_start().
 * Note that calling \ref bcmlt_capture_start() again after calling this
 * function will erase the entire captured history for this device.
 *
 * \param [in] unit The unit number of the device for which to stop
 * capturing
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int bcmlt_capture_stop(int unit);

/*!
 * \brief BCMLT Generic object handle.
 *
 * This data structure contains the handle of entry or transaction objects.
 * The entry field indicates of the relevant portion of the \c hdl field is
 * an entry or trans.
 */
typedef struct bcmlt_object_hdl_t {
    bool entry;          /*!< Selector indicating if the hdl is entry or not. */
    union {
        bcmlt_entry_handle_t entry;  /*!< An entry handle.                    */
        bcmlt_transaction_hdl_t trans; /*!< A transaction handle.             */
    } hdl;               /*!< Object handle can be entry or transaction.      */
    sal_usecs_t ts;      /*!< Time stamp where the entry was recorded.        */
} bcmlt_object_hdl_t;

/*!
 * \brief Pre-Commit notification for replay object.
 *
 * This callback function can be passed to the function
 * \ref bcmlt_capture_replay to receive the handle of the object that is
 * going to be replayed. The object can be a transaction or an entry.
 * From the callback the user can further query the object and to obtain all
 * the details associated with the objects. The following functions should
 * be useful for this effort:
 * \ref bcmlt_entry_field_traverse() to extract the fields and their data from
 * the entry.
 * \ref bcmlt_entry_info_get() to obtain entry and table information.
 * \ref bcmlt_transaction_info_get() to obtain the transaction type and number
 * of entries in the transaction.
 * \ref bcmlt_transaction_entry_num_get() to obtain the handle for a particular
 * entry in a transaction.
 * \ref bcmlt_entry_oper_get() to obtain the operation associated with the
 * entry.
 */
typedef void (replay_action_f)(int unit, bcmlt_object_hdl_t *object);

/*!
 * \brief Play back previously captured operations.
 *
 * This function will play back the exact same sequence of operations that
 * was previously captured using \ref bcmlt_capture_start() and
 * \ref bcmlt_capture_stop() functions. The playback maintains the
 * timing of the original operation (and will therefore takes the same time
 * to execute), unless the parameter \c timing is set to false.
 *
 * \param [in] timing Timing set to true to maintain similar timing as the
 * original operations (max delay between operations is 1 second).
 * Otherwise, the execution will be as soon as possible.
 * \param [in] cb Callback function to be called by the playback engine for
 * every entry or transaction that is about to execute. This function may use
 * other APIs to retrieve and display the content of the entry/transaction in
 * a human readable format. See \ref replay_action_f for more information.
 * \param [in] view_only Control if the actions will be install on the device
 * or if they will be retrieved for information only (see \c cb parameter).
 * \param [in] app_cb The app_cb is a data structure that provides the IO
 * instrumentation required by the capture replay function. This functionality
 * is implemented by the application.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid app_cb parameter.
 * \retval SHR_E_MEMORY Insufficient resources to perform this operation.
 */
extern int bcmlt_capture_replay(bool timing,
                                replay_action_f *cb,
                                bool view_only,
                                bcmlt_replay_cb_t *app_cb);

/********************************************************************/
/*          S T A T I S T I C    F U N C T I O N A L I T Y          */
/********************************************************************/
/*!
 * \brief Entities (entries & transactions) usage statistics.
 */
typedef struct {
    uint32_t in_use;        /*!< Number of elements in use.          */
    uint32_t avail;         /*!< Number of available elements.       */
    uint32_t high_wtr_mark; /*!< High water-mark of elements in use. */
}bcmlt_entity_stats_t;

/*!
 * \brief Retrieves the entry usage statistics.
 *
 * This function retrieves the entry usage statistics. This function is
 * useful to determine if the entry resources are properly tuned. Allocating
 * too many entries resources will waste precious memory resources while
 * too little resources may prevent parallel operations or large transactions
 * that contain multiple entries.
 *
 * \param [out] stats The retrieved statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Input param is invalid.
 * \retval SHR_E_INIT The API did not initialize yet.
 */
extern int bcmlt_stat_entry(bcmlt_entity_stats_t *stats);

/*!
 * \brief Retrieves the transaction usage statistics.
 *
 * This function retrieves the transaction usage statistics. This function is
 * useful to determine if the transaction resources are properly tuned.
 * Allocating too many transaction resources will waste precious memory
 * resources while too little resources may prevent parallel transaction
 * operations.
 *
 * \param [out] stats The retrieved statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Input param is invalid.
 * \retval SHR_E_INIT The API did not initialize yet.
 */
extern int bcmlt_stat_transaction(bcmlt_entity_stats_t *stats);

#endif /* BCMLT_H */
