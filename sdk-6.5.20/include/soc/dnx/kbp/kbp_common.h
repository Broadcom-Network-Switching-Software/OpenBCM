
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_KBP_COMMON_H

#define DNX_KBP_COMMON_H

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal_structures.h>

extern const int g_kbp_compiled;

#define KBP_COMPILATION_CHECK                                                                                   \
    if (!g_kbp_compiled)                                                                                        \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot perform KBP action, KBP lib not compiled");                       \
    }

#define KBP_COMMON_ACCESS kbp_common_sw_state

#define DNX_KBP_CACHE_COMMIT_FWD_CHECK(_db_type_, _dbal_table_id_) \
((_db_type_ == DBAL_TABLE_TYPE_EM) || (_db_type_ == DBAL_TABLE_TYPE_LPM) || \
 (_dbal_table_id_ == DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD) || \
 (_dbal_table_id_ == DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD))

#if defined(INCLUDE_KBP)

#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>
#include <soc/kbp/alg_kbp/include/arch.h>
#include <soc/kbp/alg_kbp/include/hw_limits.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_access.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_common_access.h>

#define KBP_TO_DNX_RESULT(result) ((result < 0) ? _SHR_E_INTERNAL : dnx_kbp_error_translation(result))

#define DNX_KBP_IS_TABLE_USED_FOR_FWD(_db_type_) \
((_db_type_ == DBAL_TABLE_TYPE_EM) || (_db_type_ == DBAL_TABLE_TYPE_LPM) || (_db_type_ == DBAL_TABLE_TYPE_TCAM))

#define DNX_KBP_IS_TABLE_USED_FOR_ACL(_db_type_) \
((_db_type_ == DBAL_TABLE_TYPE_TCAM_BY_ID))

#ifndef KBP_WARMBOOT_TYPE_DEFINED
#define KBP_WARMBOOT_TYPE_DEFINED

#define KBP_FWD_TCAM_ACCESS kbp_fwd_tcam_access_mapper

#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS        320
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BYTES       BITS2BYTES(DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_WORDS       BITS2WORDS(DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS    DNX_KBP_RESULT_SIZE_FWD
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BYTES   BITS2BYTES(DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_WORDS   BITS2WORDS(DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS)

#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS        480
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES       BITS2BYTES(DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_WORDS       BITS2WORDS(DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS    DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES   BITS2BYTES(DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS   BITS2WORDS(DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS)

#define DNX_KBP_MAX_NOF_RANGES                  KBP_HW_MAX_RANGE_COMPARES
#define DNX_KBP_MAX_RANGE_SIZE_IN_BITS          16

#define DNX_KBP_ACCESS_BMP_INDICATION_CACHING_ENABLED       (1 << 0)
#define DNX_KBP_ACCESS_BMP_INDICATION_DEFAULT_ENTRY         (1 << 1)

typedef struct kbp_warmboot_s
{
    FILE *kbp_file_fp;
    kbp_device_issu_read_fn kbp_file_read;
    kbp_device_issu_write_fn kbp_file_write;
} kbp_warmboot_t;
#endif

typedef enum
{

    DNX_KBP_DEV_TYPE_NONE = 0,

    DNX_KBP_DEV_TYPE_BCM52311 = 1,

    DNX_KBP_DEV_TYPE_BCM52321 = 2,

    DNX_KBP_NOF_DEV_TYPE = 2
} dnx_kbp_device_type_e;

#define DNX_KBP_TRY(A)                                                                                              \
    do                                                                                                              \
    {                                                                                                               \
        kbp_status __tmp_status = A;                                                                                \
        if (__tmp_status != KBP_OK)                                                                                 \
        {                                                                                                           \
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_kbp_error_translation(__tmp_status),                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kbp_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

#define DNX_KBP_TRY_PRINT(A, _format_, ...)                                                                         \
    do                                                                                                              \
    {                                                                                                               \
        kbp_status __tmp_status = A;                                                                                \
        if (__tmp_status != KBP_OK)                                                                                 \
        {                                                                                                           \
            LOG_CLI((BSL_META(_format_), ##__VA_ARGS__));                                                             \
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_kbp_error_translation(__tmp_status),                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kbp_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

shr_error_e dnx_kbp_error_translation(
    kbp_status kbp_err);

uint32 dnx_kbp_calculate_prefix_length(
    uint32 *payload_mask,
    uint32 max_key_size_in_bits);

shr_error_e dnx_kbp_prefix_len_to_mask(
    int unit,
    int prefix_length,
    uint32 max_key_size_in_bits,
    dbal_physical_entry_t * entry);

shr_error_e dnx_kbp_buffer_dbal_to_kbp(
    int unit,
    uint32 nof_bits,
    uint32 *data_in,
    uint8 *data_out);

shr_error_e dnx_kbp_buffer_kbp_to_dbal(
    int unit,
    uint32 nof_bits,
    uint8 *data_in,
    uint32 *data_out);

shr_error_e dnx_kbp_entry_lpm_get(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    uint8 *kaps_hitbit,
    kbp_db_handles_t * op2_kbp_handles,
    uint32 *op2_payload_size);

shr_error_e dnx_kbp_entry_lpm_add(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    dbal_indirect_commit_mode_e * indirrect_commit_mode,
    uint8 kaps_hitbit,
    uint32 op2_access_bmp_indication,
    kbp_db_handles_t * op2_kbp_handles);

shr_error_e dnx_kbp_entry_lpm_delete(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p kaps_hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 prefix_length,
    uint32 op2_access_bmp_indication,
    kbp_db_handles_t * op2_kbp_handles,
    struct kbp_entry *op2_db_entry_in);

int dnx_kbp_file_read_func(
    void *handle,
    uint8 *buffer,
    uint32 size,
    uint32 offset);

int dnx_kbp_file_write_func(
    void *handle,
    uint8 *buffer,
    uint32 size,
    uint32 offset);

#else

#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS        1
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BYTES       1
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_WORDS       1
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS    1
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BYTES   1
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_WORDS   1

#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS        1
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES       1
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_WORDS       1
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS    1
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES   1
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS   1

#define DNX_KBP_MAX_NOF_RANGES                  0
#define DNX_KBP_MAX_RANGE_SIZE_IN_BITS          0

#endif

#define DUMMY_PROTOTYPE_ERR_MSG(__func_name__)                                                      \
        SHR_FUNC_INIT_VARS(unit);                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot perform %s, KBP lib is not compiled", __func_name__); \
        exit:                                                                                       \
        SHR_FUNC_EXIT;

uint8 dnx_kbp_device_enabled(
    int unit);

uint8 dnx_kbp_interface_enabled(
    int unit);

shr_error_e dnx_kbp_valid_key_width(
    int unit,
    uint32 width,
    uint32 *valid_width);

shr_error_e dnx_kbp_valid_result_width(
    int unit,
    uint32 width,
    uint32 *valid_width);

shr_error_e dnx_kbp_caching_enabled_get(
    int unit,
    int is_acl,
    int *enabled);

shr_error_e dnx_kbp_caching_enabled_set(
    int unit,
    int is_acl,
    int enable);

#endif
