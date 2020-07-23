/*! \file bcm_int/dnx/kbp/kbp_mngr.h
 *
 * Internal DNX KBP APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _KBP_MNGR_H_INCLUDED__
/*
 * {
 */
#define _KBP_MNGR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/** Internal KBP manager indications for distinguishing between master key and DB key */
typedef enum
{
    DNX_KBP_DB_TABLE_INDICATION,
    DNX_KBP_OPCODE_TABLE_INDICATION,
    DNX_KBP_NOF_SW_TABLE_INDICATION
} dnx_kbp_sw_table_indication_e;

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define KBP_ACCESS kbp_sw_state

/** Convenience macro for blocking dynamic APIs for ACL configurations after the device has been locked */
#define DNX_KBP_BLOCK_API_IF_DEVICE_IS_LOCKED() \
    do { \
        dbal_enum_value_field_kbp_device_status_e _kbp_mngr_status_; \
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &_kbp_mngr_status_)); \
        if (_kbp_mngr_status_ == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED) { \
            SHR_ERR_EXIT(_SHR_E_CONFIG, "API is not allowed to run when KBP device is locked\n"); \
        } \
    } while(0)

#if defined(INCLUDE_KBP)

/** KBP is configured in dual port mode by default in SDK */
#define DNX_KBP_MAX_NOF_OPCODES                 KBP_HW_MAX_LTR_DUAL_PORT

#define DNX_KBP_MAX_NOF_LOOKUPS                 KBP_HW_MAX_SEARCH_DB
#define DNX_KBP_MAX_NOF_RESULTS                 KBP_INSTRUCTION_MAX_RESULTS

#define MAX_NUMBER_OF_LOOKUPS_WHEN_XL_DB_EXISTS 6

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP     KBP_MAX_NUM_OF_SEGMENTS_PER_KEY

/**
 * MAX_KEY_LENGTH limited to 360 from the available 640 /NLMDEV_MAX_KEY_LEN_IN_BITS/ due to bandwidth requirements.
 * However, we allow up to 480 bits to accomodate user scenarios where the larger keys are not expected to run at 
 * full bandwidth.
 */
#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS_LOSSLESS 360
#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS          480

#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS      NLMDEV_MAX_AD_LEN_IN_BITS

#define DNX_KBP_MAX_NOF_SMTS                    NLM_MAX_NUM_SMT

#define DNX_KBP_MIN_ACL_PRIORITY                KBP_HW_MINIMUM_PRIORITY

#else

/*
 * Below defines are mapped to KBPSDK defines.
 * For compatibility with non-KBP compilations they are redefined to 0 or 1.
 */
#define DNX_KBP_MAX_NOF_OPCODES                 1 /** Used as array initializer */

#define DNX_KBP_MAX_NOF_LOOKUPS                 1 /** Used as array initializer */
#define DNX_KBP_MAX_NOF_RESULTS                 1

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP     1 /** Used as array initializer */

#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS_LOSSLESS 0
#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS          0
#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS      0

#define DNX_KBP_MAX_NOF_SMTS                    0

#define DNX_KBP_MIN_ACL_PRIORITY                0

#endif

#define DNX_KBP_OPCODE_MIN                      1 /** Min opcode Value (HW) */
#define DNX_KBP_OPCODE_MAX                      200 /** MAx opcode Value (HW) */
#define DNX_KBP_MAX_NOF_DBS                     256 /** No suitable define was found amongst the KBPSDK defines */
#define DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES     16 /** No suitable define was found amongst the KBPSDK defines */
#define DNX_KBP_HIT_INDICATION_SIZE_IN_BITS     8 /** the size of the hitbit in the result buffer from the KBP */

#define DNX_KBP_MAX_KEY_LENGTH_IN_BYTES         BITS2BYTES(DNX_KBP_MAX_KEY_LENGTH_IN_BITS)
#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BYTES     BITS2BYTES(DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS)

#define DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES  64 /** Arbitrary size that should fit almost all field names */
#define DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS  BYTES2WORDS(DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES)

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY 22 /** Arbitrary size */

/*
 * In SMT mode the opcode pool is divided between the two threads.
 * Offset is used to configure same context to different thread.
 */
#define DNX_KBP_SMT_INSTRUCTION_OFFSET          DNX_KBP_MAX_NOF_OPCODES


#define DNX_KBP_RESULT_SIZE_FWD                 64
#define DNX_KBP_RESULT_SIZE_RPF                 32

/** this assumes that there are 32 contexts that are reserved to FWD and 32 reserved for ACL. the first ACL ID context is
 *  32. this will allow support ISSU (we keeping some context for FWD and some for ACLs */
#define DNX_KBP_FWD_CONTEXT_LAST                (32)
/*
 *  This assumes that there are 32 opcodes that are reserved to FWD (static) and 32 reserved for ACL (dynamic).
 *  the first dynamic opcode is 32. this will allow support ISSU (we keeping some opcodes for FWD and some for ACLs)
 */
#define DNX_KBP_MAX_STATIC_OPCODES_NOF          (32)

/** 171 taken from "kbp_field_types_definitions.xml" */
#define DNX_KBP_KEY_SEGMENT_SIZE                (BITS2WORDS(179))

/** Bitmap for swstate caching indication */
#define DNX_KBP_CACHING_BMP_FWD                 (1 << 0)
#define DNX_KBP_CACHING_BMP_ACL                 (1 << 1)
#define DNX_KBP_CACHING_BMP_ALLOWED             (1 << 2)

/** Values indicating to use the default values for  */
#define DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION       (0)
#define DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION   (DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT)

#define DNX_KBP_OVERLAY_FIELD_INVALID 80
#define DNX_KBP_KEY_IS_OVERLAY_FIELD(_overlay_field_offset_) \
    (((_overlay_field_offset_) == DNX_KBP_OVERLAY_FIELD_INVALID) ? FALSE : TRUE)

/** The maximum allowed data length per result */
#define DNX_KBP_MAX_RESULT_DATA_LEN             128

/*
 * }
 */
/*
 * INVALID DEFINEs
 * {
 */
#define DNX_KBP_INVALID_LOOKUP_ID                 -1
#define DNX_KBP_INVALID_RESULT_ID                 -1
#define DNX_KBP_INVALID_OPCODE_ID                 -1
#define DNX_KBP_INVALID_SEGMENT_ID                -1
#define DNX_KBP_INVALID_SEGMENT_SIZE              -1
#define DNX_KBP_INVALID_DB_ID                     -1
/*
 * }
 */

/*
 * MACROs
 * {
 */
/**
 * Check if KBP status means that device lock has occured.
 */
#define DNX_KBP_STATUS_IS_LOCKED(_status) \
    ((_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED) || \
     (_status == DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED))

#define DNX_KBP_FIELD_OMMITTED_FROM_OPTIMIZED_RESULT(_field_id_) \
    (((_field_id_ == DBAL_FIELD_KBP_ALIGNMENT_PADDING_1) || (_field_id_ == DBAL_FIELD_KBP_ALIGNMENT_PADDING_2) || (_field_id_ == DBAL_FIELD_RESULT_TYPE)) ? 1 : 0)

#define DNX_KBP_OPTIMIZED_RESULT_TYPE_HW_VALUE 0

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

#if defined(INCLUDE_KBP)
typedef struct
{
    struct kbp_allocator *dalloc_p[DNX_KBP_MAX_NOF_INST];
    void *xpt_p[DNX_KBP_MAX_NOF_INST];
    struct kbp_device *device_p[DNX_KBP_MAX_NOF_INST];
    struct kbp_device *smt_p[DNX_KBP_MAX_NOF_INST][DNX_KBP_MAX_NOF_SMTS];
    uint32 flags;
    enum kbp_device_type device_type;
    struct kbp_device_config device_config[DNX_KBP_MAX_NOF_INST];
    dnx_kbp_user_data_t user_data[DNX_KBP_MAX_NOF_INST];
} generic_kbp_app_data_t;

#else

typedef struct
{
    void *dummy_p;
} generic_kbp_app_data_t;

#endif

/** Used in FP to provide to KBP manager information on new ACL lookup */
typedef struct
{
    /** the result index in the KBP result (payload) buffer */
    uint8 result_index;

    /** offset from the start of the KBP result buffer, indicating the result location of the lookup */
    uint32 result_offset;

    /** the DBAL table ID related to the lookup */
    dbal_tables_e dbal_table_id;

    /** the number of relevant segments */
    uint32 nof_segments;

    /** the relevant key segments from the master key */
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];

    /*
     * The result size in the result buffer (note, can be smaller than the result size in the DB).
     * 0 means to take the size from the DBAL table (thus it will be the same as in the DB).
     */
    uint32 result_size;

} kbp_opcode_lookup_info_t;

typedef struct
{
    dbal_tables_e table_id;
    uint8 lookup_id;
    char lookup_type_str[DBAL_MAX_STRING_LENGTH];
    dbal_printable_entry_t entry_print_info;
} kbp_printable_entry_t;

/*
 * Utility structure that represent one segment in the master key or the DB key.
 * This structure is used to retrieve/update master key and DB key.
 * The max number of master key segments is DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY.
 * The max number of DB key segments is DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP.
 *
 * Overlay indication and overlay offset are supposed to be used only for master key.
 * An overlay field is a key segment that takes its value from the master key, based on its offset and size.
 * The overlay fields are used as keys for DB lookups.
 * They allow for the reduction of the master key and DB keys.
 */
typedef struct
{
    /** Indication whether the key field is a master key or an overlay field */
    uint8 is_overlay_field;

    /** Offset of the overlay field location in relation to the MSB of the master key */
    uint32 overlay_offset_bytes;

    /** Size in bytes; KBP segments has to be byte aligned; In case of DNX_KBP_INVALID_SEGMENT_SIZE the segment is not valid */
    uint32 nof_bytes;

    /** Name of the key segment */
    char name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES];
} kbp_mngr_key_segment_t;

/*
 * struct that represents a FWD context and all of the ACL contexts that can be derived from it.
 */
typedef struct
{
    uint8 fwd_context;
    uint8 nof_acl_contexts;
    uint8 acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM];
} kbp_mngr_fwd_acl_context_mapping_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* \brief
*   KBP Manager Init API, called from Init sequence. Performs KBP device initialization, and set all static applications
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_init(
    int unit);

/**
* \brief
*   KBP Manager De-Init API, called from DeInit sequence. Performs KBP device de-initialization
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_deinit(
    int unit);

/**
* \brief
*   For a given opcode ID provides all FWD context and their respective ACL contexts that use the opcode ID.
*   \param [in] unit - Relevant unit.
*   \param [in] opcode_id - The opcode ID.
*   \param [out] fwd_nof_contexts - The number of fwd contexts the use the opcode_id. The numbe rod elements in fwd_acl_mapping.
*   \param [out] fwd_acl_ctx_mapping - An array of FWD contexts and the ACL contexts that derive from them.
*                                      Contains DNX_DATA_MAX_FIELD_KBP_MAX_CONTEXT_NUM_FOR_ONE_APPTYPE elements.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_to_contexts_get(
    int unit,
    uint8 opcode_id,
    uint8 *fwd_nof_contexts,
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE]);

/**
* \brief
*   this API returns the KBP opcode and opcode anme from the FWD and RPF contexts.
*   \param [in] unit - Relevant unit.
*   \param [in] fwd_context - context of the required info
*   \param [in] acl_context - context of the required info
*   \param [out] opcode_id - the relevant opcode ID
*   \param [out] opcode_name - string that represnts the opcde should be at least char DBAL_MAX_STRING_LENGTH
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_context_to_opcode_get(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id,
    char *opcode_name);

/**
 * \brief
 *   Returns printable entry for visibility packet diagnostic
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Flags for result parsing.
 *   \param [in] fwd_context - Forwarding context (only from IFWD2 stage).
 *   \param [in] acl_context - ACL context.
 *   \param [in] key_sig_value - The key signal for parsing the lookup key.
 *   \param [in] key_sig_size - number of fields in the key.
 *   \param [in] res_sig_value - The result signal for parsing the lookup results.
 *   \param [in] res_sig_size - number of fields in the results.
 *   \param [in] nof_print_info - number of returned printable entry infos
 *   \param [out] entry_print_info - The returned printable entry info, built by the key and result signals.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e kbp_mngr_opcode_printable_entry_get(
    int unit,
    uint32 flags,
    uint8 fwd_context,
    uint8 acl_context,
    uint32 *key_sig_value,
    int key_sig_size,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * entry_print_info);

/**
* \brief
*   this API takes the KBP SW information update the DB according to the valid opcodes and configures the the KBP HW,
*   once this API is called the device is ready to use.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_sync(
    int unit);

/**
* \brief
*   During warmboot this API performs KBP device restore, takes the KBP swstate information for KBP DB and instruction handles,
*   performs refresh on the handles and locks the device.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_wb_init(
    int unit);

/**
* \brief
*   this API updates the KBP HW with the previously applied configurations during warmboot.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_wb_sync(
    int unit);

/**
* \brief
*   This API creates KBP dynamic table. Used for ACLs that are created after init. It takes the DBAL KBP SW
*   information and transform it to the KBP info. Updates the number of DBs in KBP.
*   Returns error if KBP device is locked.
*   \param [in]  unit          - Relevant unit.
*   \param [in]  counters_enable - enable counters for ACL db.
*   \param [in]  dbal_table_id - the related table ID.
*   \param [in]  initial_capacity - initial capacity of the DB, this parameter affect on how much the DB can grow. if
*          user doens't have any special requirements set to DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION and kbp will
*          use default value:
*          DNX_KBP_INITIAL_DB_SIZE_CREATION_TCAM.
*   \param [in]  algo_mode - the DB algorithmic mode, the value is used to set the DB property KBP_PROP_ALGORITHMIC
*          optional values are 0 - non algo, 1 - power ctrl (partitions of the TCAM), 2 - power ctrl advance, 3 - net
*          ACL (improve capacity and power) to use default value set (DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION).
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_db_create(
    int unit,
    uint8 counters_enable,
    dbal_tables_e dbal_table_id,
    uint32 initial_capacity,
    int algo_mode);

/**
* \brief
*   This API retrieves the properties provided to a dynamic KBP table created by kbp_mngr_db_create.
*   \param [in]  unit          - Relevant unit.
*   \param [in]  dbal_table_id - the related table ID.
*   \param [out]  counters_enable - enable counters for ACL db.
*   \param [out]  initial_capacity - initial capacity of the DB, this parameter affect on how much the DB can grow. if
*          user doens't have any special requirements set to DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION and kbp will
*          use default value:
*          DNX_KBP_INITIAL_DB_SIZE_CREATION_TCAM.
*   \param [out]  algo_mode - the DB algorithmic mode, the value is used to set the DB property KBP_PROP_ALGORITHMIC
*          optional values are 0 - non algo, 1 - power ctrl (partitions of the TCAM), 2 - power ctrl advance, 3 - net
*          ACL (improve capacity and power) to use default value set (DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION).
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_db_properties_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 *counters_enable,
    uint32 *initial_capacity,
    int *algo_mode);

/**
 * \brief - updating the KBP status in the SW table.
 * \param [in] unit - Relevant unit.
 * \param [in] kbp_mngr_status - example: DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_status_update(
    int unit,
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status);

/**
 * \brief - Get the KBP status from the SW table.
 * \param [in]  unit - Relevant unit.
 * \param [out] kbp_mngr_status - The returned status. Can be one of these: DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_status_get(
    int unit,
    dbal_enum_value_field_kbp_device_status_e * kbp_mngr_status);

/**
 * \brief - Get from the SW table if KBP IPv4 public is enabled.
 * \param [in]  unit - Relevant unit.
 * \param [out] enabled - The returned indication, TRUE or FALSE.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_ipv4_public_enabled_get(
    int unit,
    uint8 *enabled);

/**
 * \brief - Get from the SW table if KBP IPv6 public is enabled.
 * \param [in]  unit - Relevant unit.
 * \param [out] enabled - The returned indication, TRUE or FALSE.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_ipv6_public_enabled_get(
    int unit,
    uint8 *enabled);

/**
 * \brief - Get the total result size of a given opcode.
 * \param [in]  unit - Relevant unit.
 * \param [in]  opcode_id - Relevant opcode ID.
 * \param [out] result_size - The returned total opcode result size.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_opcode_total_result_size_get(
    int unit,
    uint8 opcode_id,
    uint32 *result_size);

/**
* \brief
*   Set the master key of an opcode using the provided segments.
*   This will replace all current master key segments.
*   The first 'nof_segments' will be set according to the provided segments.
*   All other segments will be set to invalid '0' values.
*   Overlay fields can be set by sending them via the segments array.
*   \param [in] unit          - Relevant unit.
*   \param [in] opcode_id     - ID of related opcode.
*   \param [in] nof_segments  - The number of segments to set.
*   \param [in] key_segments  - Array of segments to be set to the opcode master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_set(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   This API retrieves the master key structure for specific KBP opcode.
*   Used by FP for updating the KBP opcode master key when attaching KBP group to context.
*   Overlay fields are retrieved in the segments array.
*   \param [in] unit              - Relevant unit.
*   \param [in] opcode_id         - ID of related opcode.
*   \param [out] nof_key_segments - The number of retrieved segments.
*   \param [out] key_segments     - Array of retrieved segments info each segment is used. the location of the array is
*          the location in the master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_get(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   This API used to add segments to the master key for specific KBP opcode. This API will add only the new key
*   segments. all the segments in the master key has to have unique names. The new segments will be added at the end of
*   the master key if new_segments_placement is DNX_KBP_INVALID_SEGMENT_ID, otherwise the will be added in the middle
*   and all lookups using the following segments will be updated.
*   Returns error if KBP device is locked.
*   \param [in] unit              - Relevant unit.
*   \param [in] opcode_id         - ID of related opcode.
*   \param [in] new_segments_index  - The place on the master key the new segments will be placed.
*                                     If the value is DNX_KBP_INVALID_SEGMENT_ID, the segments will be appended
*                                     at the end of the master key.
*   \param [in] nof_new_segments  - The number of new segments.
*   \param [in] key_segments      - Array of segments to be added to the opcode master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_segments_add(
    int unit,
    uint32 opcode_id,
    uint32 new_segments_index,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   Removes a segment from a master key. Lookups will be updated (unless a lookup uses the segment to be removed).
*   Returns error if KBP device is locked.
*   \param [in] unit              - Relevant unit.
*   \param [in] opcode_id         - ID of related opcode.
*   \param [in] segment_index  - The segment to be removed.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_segment_remove(
    int unit,
    uint32 opcode_id,
    uint32 segment_index);

/**
* \brief
*   This API adds a lookup to an existing opcode. The lookup ID has to be available (unused) in the opcode.
*   Used by FP when attaching KBP group to a context.
*   Returns error if KBP device is locked.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID to update.
*   \param [in] lookup_info  - The lookup info to add.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_lookup_add(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
* \brief
*   This API retrieves a lookup from an existing opcode.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID to get.
*   \param [out] lookup_info - The lookup to get; result_index need to be set.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_lookup_get(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
* \brief
*   This API clears a lookup from an existing opcode.
*   Returns error if KBP device is locked.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID.
*   \param [in] result_id    - The related result to clear.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_result_clear(
    int unit,
    uint8 opcode_id,
    uint8 result_id);

/**
* \brief
*   This API clears all configurations done for an opcode.
*   Returns error if KBP device is locked.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_clear(
    int unit,
    uint8 opcode_id);

/**
* \brief
*   Return the number of entries in the DB and the estimated capacity.
*   \param [in] unit - Relevant unit.
*   \param [in] db_id - the DB id.
*   \param [out] nof_entries - the returned number of entries.
*   \param [out] estimated_capacity - the returned estimated capacity.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_db_capacity_get(
    int unit,
    uint8 db_id,
    uint32 *nof_entries,
    uint32 *estimated_capacity);

/**
* \brief
*   This API create a new opcode accroding to the source opcode. all the source opcode info (lookups and master key) is
*   duplicated (since it is a cascaded opcode, meaning that the ACL is done on top of all other FWD lookups). NOTES:
*   this API support only apptypes that connected to one FWC Ctx.
*   Returns error if KBP device is locked.
*   \param [in] unit         - Relevant unit.
*   \param [in] source_opcode_id    - the opcode ID to generate from it the new opcode. (cascaded from).
*   \param [in] opcode_name    - The new opcode name.
*   \param [out] acl_ctx    - an available fwd2 context that will be use for this opcode.
*   \param [out] new_opcode_id    - the new opcode ID.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_create(
    int unit,
    uint8 source_opcode_id,
    char *opcode_name,
    uint8 *acl_ctx,
    uint8 *new_opcode_id);

/**
* \brief
*   This API returns an array with result indexes, which are using given acl_qual_name.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID.
*   \param [in] acl_qual_name    - The name of the ACL qualifier for which info
*    will be returned.
*   \param [out] result_ids    - Array with result indexes. Note the initial value is
*   DNX_KBP_INVALID_RESULT_ID, which is indicating the end of the array.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_segment_used_result_indexes_get(
    int unit,
    uint8 opcode_id,
    char *acl_qual_name,
    uint32 result_ids[DNX_KBP_MAX_NOF_RESULTS]);

/**
* \brief
*   This function returns the number of FFCs allocated for ACL usage at init for a specific predefined opcode.
*   Used to reserve FFCs for future use in the FWD.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The predefined (static) opcode ID which uses the FFCs
*   \param [out] nof_ffcs    - The amount of FFCs that we can allocate for ACL for the OPCODE
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_nof_acl_ffcs(
    int unit,
    uint8 opcode_id,
    uint32 *nof_ffcs);

/**
* \brief
*   This function returns whether a certain acl context is static (created for predefined opcode) or
*   dynamic (created when creating a new opcode)
*   \param [in] unit         - Relevant unit.
*   \param [in] acl_ctx      - The ACL context
*   \param [out] is_dynamic  - Whether the ACL context is dynamic or static.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_context_acl_is_dynamic(
    int unit,
    uint8 acl_ctx,
    uint8 *is_dynamic);

/**
* \brief
*   This function returns whether a certain opcode is static (predefined opcode) or
*   dynamic (created when creating a new opcode, user defined opcode)
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The opcode Id
*   \param [out] is_dynamic  - Whether the ACL context is dynamic or static.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_is_dynamic(
    int unit,
    uint8 opcode_id,
    uint8 *is_dynamic);

/**
* \brief
*   Translates FWD and ACL contexts to KBP opcode ID.
*   \param [in] unit - Relevant unit.
*   \param [in] fwd_context - Forwarding context (only from IFWD2 stage).
*   \param [in] acl_context - ACL context.
*   \param [out] opcode_id - The returned opcode ID if not value is mapped will return -1 and error _SHR_E_NOT_FOUND.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e if context not mapped returns _SHR_E_NOT_FOUND
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_context_to_opcode_translate(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id);

/**
* \brief
*   this API inits the kbp_mngr_key_segment_t structure to default (invalid) values
*   \param [in] unit      - Relevant unit.
*   \param [in] segment_p - This procedure loads the pointed memory by that structure, containing invalid values.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_key_segment_t_init(
    int unit,
    kbp_mngr_key_segment_t * segment_p);

/**
* \brief
*   this API inits the kbp_opcode_lookup_info_t structure to default (invalid) values
*   \param [in] unit          - Relevant unit.
*   \param [in] lookup_info_p - The pointer to structure to initialize.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_opcode_lookup_info_t_init(
    int unit,
    kbp_opcode_lookup_info_t * lookup_info_p);

/**
 * \brief
 *   Sets the KBP tables capacities according to the KBP DBAL table ID and capacity value
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_table_id - The KBP DBAL table ID.
 * \param [in] is_opt - Optimized result indication.
 * \param [in] capacity_value - The capacity to be set.
  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e kbp_mngr_table_capacity_set(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 is_opt,
    int capacity_value);

/**
 * \brief
 *   Gets the KBP tables capacity according to the DBAL table ID
 * \param [in] unit - Relevant unit.
 * \param [in] dbal_table_id - The KBP DBAL table ID.
 * \param [in] is_opt - Optimized result indication.
 * \param [out] capacity_value - The returned capacity.
  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e kbp_mngr_table_capacity_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint8 is_opt,
    int *capacity_value);

/**
 * \brief - Indicates if SMT is enabled for the KBP device or not.
 * \param [in] unit - Relevant unit.
  \return
 *   TRUE when SMT is enabled, FALSE otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 kbp_mngr_smt_enabled(
    int unit);

/**
 * \brief
 *   Indicates if split or non-split KBP RPF tables are used.
 *   Non-split RPF is used on single core devices. FWD and RPF tables are single physical table and entries are shared.
 *   Split RPF is available for devices with two cores.
 *   FWD and RPF tables are physically divided and entries need to be managed separately.
 * \param [in] unit - Relevant unit.
  \return
 *   TRUE when KBP RPF is split, FALSE otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 kbp_mngr_split_rpf_in_use(
    int unit);

/**
 * \brief
 *   Indicates if KBP IPv4 FWD is used.
 * \param [in] unit - Relevant unit.
  \return
 *   TRUE when KBP IPv4 FWD is used, FALSE otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 kbp_mngr_ipv4_in_use(
    int unit);

/**
 * \brief
 *   Indicates if KBP IPv6 FWD is used.
 * \param [in] unit - Relevant unit.
  \return
 *   TRUE when KBP IPv6 FWD is used, FALSE otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 kbp_mngr_ipv6_in_use(
    int unit);

/**
 * \brief
 *   init to KBP device according to the KBP mode.
 *   \param [in] unit - Relevant unit.
 * \return
 *   \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_kbp_device_init(
    int unit);

/**
 * \brief
 *  deinit to KBP device according to the KBP mode.
 *   \param [in] unit - Relevant unit.
 * \return
 *   \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_kbp_device_deinit(
    int unit);

/*
 * }
 */

shr_error_e kbp_mngr_db_key_segment_set(
    int unit,
    uint32 db_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES]);

shr_error_e kbp_mngr_db_key_segment_get(
    int unit,
    uint32 db_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES]);

shr_error_e kbp_mngr_master_key_segment_set(
    int unit,
    uint32 opcode_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES],
    uint32 *overlay_offset);

shr_error_e kbp_mngr_master_key_segment_get(
    int unit,
    uint32 opcode_id,
    int index,
    uint32 *segment_type,
    uint32 *segment_size,
    char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES],
    uint32 *overlay_offset);

#endif/*_KBP_MNGR_H_INCLUDED__*/
