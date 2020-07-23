/*! \file bcmltd_lt_types.h
 *
 * Logical Table type definitions for public APIs.
 *
 * These types retain their bcmlt_ prefixes as they are ultimately
 * exposed by the BCMLT API.
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

#ifndef BCMLTD_LT_TYPES_H
#define BCMLTD_LT_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_error.h>

/*!
 * \brief Entry attributes definitions.
 *
 * It is possible to assign some attributes to an entry using the function
 * bcmlt_entry_attrib_set(). Setting an attribute will impact the
 * operations performed on the entry as defined in the attributes below.
 */

/*!
 * \brief Attribute to obtain the entry values directly from hardware.
 *
 * Set this flag, using bcmlt_entry_attrib_set(), to obtain
 * the entry values directly from hardware and not from cache.
 */
#define BCMLT_ENT_ATTR_GET_FROM_HW  0x1

/*!
 * \brief Attribute to filter out fields with default values.
 *
 * Set this flag, using bcmlt_entry_attrib_set(), to receive
 * only the values of the fields with non-default values.
 * This flag applies to lookup or traverse (BCMLT_OPCODE_LOOKUP and
 * BCMLT_OPCODE_TRAVERSE) operations.
 */
#define BCMLT_ENT_ATTR_FILTER_DEFAULTS  0x2

/*!
 * \brief Attribute to search for maximum prefix length match for ALPM lookup
 *
 * Set this flag, using bcmlt_entry_attrib_set(), to indicate a special
 * lookup. This flag applies to lookup (BCMLT_OPCODE_LOOKUP) operations only.
 * The 'special lookup' match conditions are as follows:
 * For each entry in the route tables where entry prefix length <= input prefix
 * length, compare input subnet & entry mask with entry subnet & entry mask.
 * If they are equal then pickup the longest prefix from the matched entries.
 * This flag is a temporary solution to perform selective lookup on a particular
 * logical table. It is expected that this will be replaced by a different, more
 * generic API that will enable similar functionality within multiple LTs.
 */
#define BCMLT_ENT_ATTR_LPM_LOOKUP   0x4

/*!
 * \brief Attribute to generate traverse snapshot.
 *
 * Set this flag, using bcmlt_entry_attrib_set(), to indicate that the current
 * entries of a LT should be saved in a newly allocated memory. This operation
 * creates a snapshot of the current entries that are present within the LT.
 * The application may use this flag to guarantee that the traverse results
 * will not yield duplicate entries (for non-index tables).
 * Without a snapshot the SDK sequentially moves along the corresponding
 * physical table starting from index 0 until the end of the table. Therefore
 * it is possible for the application to miss an entries or to traverse the
 * same entry multiple times in case that the entries were relocated into
 * different physical location.
 * On the other hand taking a snapshot is an expensive operation as the SDK
 * needs to allocate memory to store the keys of all the entries that are
 * currently present in the LT. Furthermore, the SDK has to parse the entire
 * table and write the key value of each entry within the allocated snapshot
 * memory.
 * Note that the snapshot will remain until being free.
 */
#define BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT    0x8

/*!
 * \brief Free the traverse snapshot.
 *
 * Set this flag, using bcmlt_entry_attrib_set(), to indicate that the current
 * traverse snapshot is no longer required. This flag is only applicable if
 * a traverse operation was taken with the flag \ref
 * BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT being set. In this case using this flag is
 * the only way to free the traverse snapshot.
 */
#define BCMLT_ENT_ATTR_TRAVERSE_DONE   0x10

/*!
 * \brief Priority levels for entry and transaction operations.
 */
typedef enum bcmlt_priority_level_e {
    BCMLT_PRIORITY_NORMAL,   /*!< Normal priority */
    BCMLT_PRIORITY_HIGH      /*!< High priority   */
} bcmlt_priority_level_t;


/*!
 * \brief Field access modes.
 */
typedef enum bcmltd_field_acc_e {
    BCMLT_FIELD_ACCESS_READ_WRITE = 0,  /*!< Allow read and write access. */
    BCMLT_FIELD_ACCESS_READ_ONLY        /*!< Read only access mode.       */
} bcmlt_field_acc_t;

/*!
 * \brief Logical Table Entry update notification.
 *
 * For Transactions and single Entry Async operations use
 * notification option definitions.
 */
typedef enum bcmlt_notif_option_e {
    BCMLT_NOTIF_OPTION_NO_NOTIF = 0, /*!< Don't notify.                     */
    BCMLT_NOTIF_OPTION_COMMIT,       /*!< Notify after operation committed. */
    BCMLT_NOTIF_OPTION_HW,           /*!< Notify after hardware updated.    */
    BCMLT_NOTIF_OPTION_ALL,          /*!< Notify for all updates.           */
    BCMLT_NOTIF_OPTION_NUM
} bcmlt_notif_option_t;

/*!
 *  * \brief Logical Table entry handle.
 */
typedef uint32_t bcmlt_entry_handle_t;


/*!
 * \brief Information about the entry.
 *
 * This data structure gives one shot access to all the relevant external
 * information associated with the entry.
 *
 * Typical use case is checking a commit operation status for an entry. It
 * can also be used for asynchronous notification callback.
 */
typedef struct bcmlt_entry_info_s {
    int                   unit;        /*!< Device number for the entry     */
    const char            *table_name; /*!< Table name for the entry        */
    bcmlt_entry_handle_t  entry_hdl;   /*!< Entry handle                    */
    bcmlt_notif_option_t  notif_opt;   /*!< Notification option             */
    shr_error_t           status;      /*!< Operation status of the entry   */
} bcmlt_entry_info_t;


/*!
 * \brief Logical Table Entry operations.
 *
 * The operation codes are determined by the TRM northbound APIs into
 * the LTM.  They are then recorded as part of the LTM entry structure.
 *
 * The operation codes are specified in the entry building API. The entry
 * is assembled using a set of fields that identify the entry and it's contents.
 * In some operations like alloc, on success the fields used as index will
 * contain the allocated value for the entry.
 */
typedef enum bcmlt_opcode_e {
    BCMLT_OPCODE_NOP = 0,
    BCMLT_OPCODE_INSERT = 1, /*!< Set the entry contents. May allocate.    */
    BCMLT_OPCODE_LOOKUP = 2, /*!< Retrieve the contents of an entry.       */
    BCMLT_OPCODE_DELETE = 3, /*!< Unallocate and clear the an entry.       */
    BCMLT_OPCODE_UPDATE = 4, /*!< Modify the specified fields of an entry. */
    BCMLT_OPCODE_TRAVERSE = 5, /*!< Fetch a sequential table entry.        */
    BCMLT_OPCODE_NUM = 6
} bcmlt_opcode_t;

/*!
 * \brief Logical Table Entry operations for PassThru.
 *
 * The operation codes are determined by the TRM northbound APIs into
 * the LTM.  They are then recorded as part of the LTM entry structure.
 * This encoding is used for PassThru directly to the physical resources.
 *
 * The operation codes are specified in the entry building API. The entry
 * is assembled using a set of fields that identify the entry and it's contents.
 * In some operations like alloc, on success the fields used as index will
 * contain the allocated value for the entry.
 */
typedef enum bcmlt_pt_opcode_e {
    BCMLT_PT_OPCODE_NOP = 0,
    BCMLT_PT_OPCODE_FIFO_POP = 1,  /*!< Pop top entry off a HW FIFO.       */
    BCMLT_PT_OPCODE_FIFO_PUSH = 2, /*!< Push a new entry onto a HW FIFO.   */
    BCMLT_PT_OPCODE_SET = 3,       /*!< Specify the entry contents.        */
    BCMLT_PT_OPCODE_MODIFY = 4,    /*!< Modify some fields of an entry.    */
    BCMLT_PT_OPCODE_GET = 5,       /*!< Retrieve the contents of an entry
                                        via PT index. */
    BCMLT_PT_OPCODE_CLEAR = 6,     /*!< Restore entry to default values.   */
    BCMLT_PT_OPCODE_INSERT = 7,    /*!< Insert an entry via key.           */
    BCMLT_PT_OPCODE_DELETE = 8,    /*!< Delete an entry via key.           */
    BCMLT_PT_OPCODE_LOOKUP = 9,    /*!< Search for an entry via key.       */
    BCMLT_PT_OPCODE_NUM = 10
} bcmlt_pt_opcode_t;


/*!
 * \brief Field data tag type.
 *
 * This value is used to interpret a field data type.
 */
typedef enum bcmlt_field_data_tag_e {
    BCMLT_FIELD_DATA_TAG_UNDEFINED = 0,
    BCMLT_FIELD_DATA_TAG_BOOL,
    BCMLT_FIELD_DATA_TAG_U8,
    BCMLT_FIELD_DATA_TAG_U16,
    BCMLT_FIELD_DATA_TAG_U32,
    BCMLT_FIELD_DATA_TAG_U64,
    BCMLT_FIELD_DATA_TAG_RAW,
    BCMLT_FIELD_DATA_TAG_NUM
} bcmlt_field_data_tag_t;

/*!
 * \brief Define asynchronous entry operation callback for a LT.
 *
 * This type definition describes the asynchronous operation callback
 * function for asynchronous entry operations. The callback function is
 * called in the context of the transaction manager. It is recommended that
 * this function performs the minimum operations required by the application
 * since other application notifications will be blocked behind this call.
 *
 * This particular function callback is defined for LT operations only. A
 * different type is also defined for PT.
 * The callback function is passed as a parameter to the function
 * bcmlt_entry_commit_async().
 *
 * \param [in] event Indicates the reason the callback being called. This should
 * match the requested event during the call to bcmlt_entry_commit_async(),
 * unless there was an error where the BCMLT_NOTIF_OPTION_HW will be returned.
 * \param [in] opcode The operation opcode associated with the entry.
 * \param [in] entry_info A pointer to the entry info.
 * \param [in] user_data An opaque pointer provided by the application
 * when calling bcmlt_entry_commit_async().
 *
 * \return void None.
 */
typedef void (*bcmlt_entry_cb)(bcmlt_notif_option_t event,
                               bcmlt_opcode_t opcode,
                               bcmlt_entry_info_t *entry_info,
                               void *user_data);

/*!
 * \brief Define asynchronous entry operation callback for a PT.
 *
 * This data type describes the asynchronous callback function signature for
 * asynchronous commit operation done on pass through (PT) tables. The
 * application should provide the callback function to the function
 * bcmlt_pt_entry_commit_async(). This function is called in the
 * context of the transaction manager and therefore should return fast as it
 * blocks other application notifications.
 *
 * \param [in] event Indicates the reason the callback being called. The
 * application can choose the type of events it wants to receive during
 * the call to bcmlt_pt_entry_commit_async().
 * \param [in] opcode The operation opcode as it was established during the
 * call to bcmlt_pt_entry_commit_async().
 * \param [in] entry_info A pointer to the entry information.
 * \param [in] user_data An opaque pointer provided by the application
 * when calling bcmlt_pt_entry_commit_async().
 *
 * \return void None.
 */
typedef void (*bcmlt_entry_pt_cb)(bcmlt_notif_option_t event,
                                  bcmlt_pt_opcode_t opcode,
                                  bcmlt_entry_info_t *entry_info,
                                  void *user_data);


/*!
 * \brief Transaction types.
 */
typedef enum bcmlt_trans_type_e {
    /*! All or none operation. */
    BCMLT_TRANS_TYPE_ATOMIC = 0,
    /*! Batch mode, some operations might fail, others succeed. */
    BCMLT_TRANS_TYPE_BATCH,
    BCMLT_TRANS_TYPE_NUM
} bcmlt_trans_type_t;

/*!
 * \brief Transaction handle.
 *
 * Every function associated with a transaction requires a
 * transaction handle.
 */
typedef uint32_t bcmlt_transaction_hdl_t;

/*!
 * \brief Information about the transaction.
 *
 * This data structure contains the information associated with a transaction.
 *
 * This data structure can be used to determine the status of a committed
 * transaction or to traverse the entries of the transaction for their state.
 */
typedef struct bcmlt_transaction_info_s {
    bcmlt_transaction_hdl_t trans_hdl;  /*!< Transaction handle              */
    bcmlt_trans_type_t      type;       /*!< The type of the transaction     */
    bcmlt_notif_option_t    notif_opt;  /*!< Notification option             */
    shr_error_t             status;     /*!< Operation status of the entry   */
    uint32_t                num_entries; /*!< number of entries in transaction*/
} bcmlt_transaction_info_t;

/*!
 * \brief Define a transaction progress callback function.
 *
 * This function defines the notification function signature for
 * transaction processing progress. This function will be called in the context
 * of the notification thread. The notification function is only relevant for
 * asynchronous operations. The caller can define the events that trigger this
 * function when committing the transaction.
 *
 * \param [in] event Indicate the reason the callback being called.
 * \param [in] trans_hdl The transaction handle that was obtained by calling
 * bcmlt_transaction_allocate().
 * \param [in] user_data An opaque pointer provided by the caller to
 * bcmlt_transaction_commit_async().
 *
 * \return None
 */
typedef void (*bcmlt_trans_cb)(bcmlt_notif_option_t event,
                               bcmlt_transaction_info_t *trans_info,
                               void *user_data);

/*!
 * \brief Table notification information.
 *
 * This data structure is an input to an application event function. The
 * application event function is called when a specific logical table
 * is changed.
 * The application can register a callback function for any table using the
 * function bcmlt_table_subscribe().
 *
 * The information provides the table name and the unit along
 * with handle to the entry that was changed. The application can use the
 * bcmlt_entry_field_xxx_get family of functions to obtain the field values
 * of the entry.
 */
typedef struct bcmlt_table_notif_info_s {
    int                   unit;        /*!< Device number for the entry     */
    const char            *table_name; /*!< Table name for the entry        */
    bcmlt_entry_handle_t  entry_hdl;   /*!< Entry handle                    */
} bcmlt_table_notif_info_t;

/*!
 * \brief Notification callback definition for table.
 *
 * The signature of the callback function that processes notifications for
 * subscribed table updates.
 *
 * The client can pass a user_data context which will be passed back as input
 * parameter to the callback notification function.
 *
 * \param [in] table_notify_info The table notify information.
 * \param [in] user_data The application context provided at the time of
 * table subscription (see bcmlt_table_subscribe()).
 *
 * \return none
 */
typedef void (*bcmlt_table_sub_cb)(bcmlt_table_notif_info_t *table_notify_info,
                                   void *user_data);


#endif /* BCMLTD_LT_TYPES_H */
