/** \file field_kbp.c
 * 
 *
 * Field KBP configurations for DNX
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#if defined(INCLUDE_KBP)

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_kbp.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_kbp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>

/*
 * }Include files
 */

/*
 * Defines.
 * {
 */
/**
 * The minimum number of ACL keys for to be reserved in KBP for each FWD context.
 */
#define DNX_FIELD_KBP_ACL_KEYS_MIN (2)
/*
 * }Defines
 */

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_key_qual_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_attach_info_p)
{
    uint32 qual_size;
    int external_total_payload_size = dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_attach_info_p, _SHR_E_PARAM, "qual_attach_info_p");

    if (qual_attach_info_p->input_type != DNX_FIELD_INPUT_TYPE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be reached for input type external TCAM. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage));
    }

    /*
     * For external TCAM input type, verify stage. Also verify we only read from the range of external TCAM payloads.
     */

    if (field_stage != DNX_FIELD_STAGE_IPMF1 && field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from KBP only supported in iPMF1 and iPMF2. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, DNX_QUAL_STAGE(qual_type)));
    }

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, NULL, NULL, &qual_size));
    if (qual_attach_info_p->offset < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) cannot be smaller then 0. "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_attach_info_p->offset, qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }
    if (qual_attach_info_p->offset + qual_size > external_total_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) plus qualifier size (%d) (together %d) cannot exceed (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_attach_info_p->offset, qual_size,
                     qual_attach_info_p->offset + qual_size, external_total_payload_size,
                     qual_attach_info_p->input_type, qual_attach_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unused by a FWD context for KBP using "is_acl" (which context selects the KBP).
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_is_acl_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    uint8 is_acl;
    int key_id;
    uint32 extern_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);
    int kbp_key = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    /*
     * Go over all of the keys in FWD stage that can be used by the KBP.
     */
    for (key_id = 0; extern_key_bmp != 0; (key_id++), (extern_key_bmp >>= 1))
    {
        if ((extern_key_bmp & 1) == 0)
        {
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, key_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, &is_acl));

        if (is_acl == TRUE)
        {
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
        kbp_key++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unsused by a FWD context for KBP using KBR.
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context for KBP.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_kbr_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    int key_id;
    uint32 extern_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);
    uint32 kbp_ffc_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_BASE_IFWD2_NOF_FFC)] = { 0 };
    int ffc_bmp_ndx;
    int ffc_in_bitmap;
    int kbp_key = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    /*
     * Go over all of the keys in FWD stage that can be used by the KBP.
     */
    for (key_id = 0; extern_key_bmp != 0; (key_id++), (extern_key_bmp >>= 1))
    {
        if ((extern_key_bmp & 1) == 0)
        {
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, key_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, kbp_ffc_bmp));
        ffc_in_bitmap = FALSE;
        for (ffc_bmp_ndx = 0; ffc_bmp_ndx < (sizeof(kbp_ffc_bmp) / sizeof(kbp_ffc_bmp[0])); ffc_bmp_ndx++)
        {
            if (kbp_ffc_bmp[ffc_bmp_ndx] != 0)
            {
                ffc_in_bitmap = TRUE;
                break;
            }
        }
        if (ffc_in_bitmap == FALSE)
        {
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
        kbp_key++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unused by a FWD context for KBP using aligner mapping.
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context for KBP.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_aligner_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key;
    uint32 aligner_key_size;
    int nof_extern_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    for (kbp_key = 0; kbp_key < nof_extern_keys; kbp_key++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &aligner_key_size));

        if (aligner_key_size == 0)
        {
            /*
             * Key is in use.
             */
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_kbp_key_aligner_mapping_mem_hw_set(
    int unit,
    int context_id,
    dbal_enum_value_field_kbp_kbr_idx_e key_id,
    uint32 key_size)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, key_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE, key_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function converts the FWD2 (external stage) KBR form values 4-5, 16-17 to
 *  KBP KBR values 0-3.
 *
 * \param [in] unit          - Device Id
 * \param [in] key_id        - Key Id
 * \param [out] external_kbr_id_p - Converted kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_field_kbp_key_fwd2_to_acl_key_id_convert(
    int unit,
    dbal_enum_value_field_field_key_e key_id,
    dbal_enum_value_field_kbp_kbr_idx_e * external_kbr_id_p)
{
    int external_key_id;
    int fwd_key_id;
    uint32 extern_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    *external_kbr_id_p = DNX_FIELD_KEY_ID_INVALID;

    for (external_key_id = 0, fwd_key_id = 0; extern_key_bmp != 0; (fwd_key_id++), (extern_key_bmp >>= 1))
    {
        if ((extern_key_bmp & 1) == 0)
        {
            continue;
        }

        if (fwd_key_id == key_id)
        {
            *external_kbr_id_p = external_key_id;
            break;
        }

        external_key_id++;
    }

    if (*external_kbr_id_p == DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The key_id (%d) supplied is not a valid KBP ACL key.\n", key_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function converts the KBP KBR values 0-3 to FWD2 (external stage)
 *  KBR form values 4-5, 16-17.
 *
 * \param [in] unit          - Device Id
 * \param [in] external_kbr_id        - KBP KBR Id
 * \param [out] key_id_p - Converted FWD2 kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_field_kbp_key_acl_to_fwd2_key_id_convert(
    int unit,
    dbal_enum_value_field_kbp_kbr_idx_e external_kbr_id,
    dbal_enum_value_field_field_key_e * key_id_p)
{
    int external_key_id;
    int fwd_key_id;
    uint32 extern_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    *key_id_p = DNX_FIELD_KEY_ID_INVALID;

    for (external_key_id = 0, fwd_key_id = 0; extern_key_bmp != 0; (fwd_key_id++), (extern_key_bmp >>= 1))
    {
        if ((extern_key_bmp & 1) == 0)
        {
            continue;
        }

        if (external_key_id == external_kbr_id)
        {
            *key_id_p = fwd_key_id;
            break;
        }

        external_key_id++;
    }

    if (*key_id_p == DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The key_id (%d) supplied is not valid KBP ACL key.\n", external_key_id);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_kbp_key_aligner_mapping_mem_set(
    int unit,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 key_size)
{
    dbal_enum_value_field_kbp_kbr_idx_e external_kbr_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_kbp_key_fwd2_to_acl_key_id_convert(unit, key_id, &external_kbr_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "aligner_mapping_mem_set context %d, key_id %d, kbp_kbr_id %d key_size %d\r\n",
                 context_id, key_id, external_kbr_id, key_size);

    SHR_IF_ERR_EXIT(dnx_field_kbp_key_aligner_mapping_mem_hw_set
                    (unit, context_id, external_kbr_id, BITS2BYTES(key_size)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Obtains the external keys available for ACL in an ACL context based on a FWD context.
 * \param [in] unit        - Device Id
 * \param [in] fwd_context - The FWD context upon which the ACL context is based upon
 * \param [in] key_ids     - The list of available keys.
 *                           An array with DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX elements.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_master_key_id_get(
    int unit,
    uint32 fwd_context,
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX])
{
    uint32 acl_keys_bmp;
    int key_id;
    uint32 extern_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);
    int max_acl_keys = dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit);
    int key_index;
    int key_index_2;
    int key_is_acl;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_ids, _SHR_E_PARAM, "key_ids");

    SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_is_acl_get(unit, fwd_context, &acl_keys_bmp));

    key_index = 0;
    for (key_id = 0; extern_key_bmp != 0; (key_id++), (extern_key_bmp >>= 1))
    {
        /** If the key is not used for external, do not use it. */
        if ((extern_key_bmp & 1) == 0)
        {
            continue;
        }
        key_is_acl = acl_keys_bmp & 1;
        acl_keys_bmp >>= 1;
        /** If the key is used for FWD, do not use it. */
        if (key_is_acl == 0)
        {
            continue;
        }

        /*
         * We begin allocating from the last key, so if we exceed the maximum number of keys, we delete the first one.
         */

        if (key_index >= max_acl_keys)
        {
            for (key_index_2 = 0; key_index_2 < (max_acl_keys - 1); key_index_2++)
            {
                key_ids[key_index_2] = key_ids[key_index_2 + 1];
            }
            key_ids[key_index_2] = key_id;
        }
        else
        {
            key_ids[key_index] = key_id;
        }

        key_index++;
    }

    /** Fill the rest if the array with an invalid key.*/
    for (; key_index < DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX; key_index++)
    {
        key_ids[key_index] = DNX_FIELD_KEY_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_key_acl_compare_qualifiers_for_sharing(
    int unit,
    int nof_quals_1,
    dnx_field_qual_t dnx_qual_1[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_1[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int nof_quals_2,
    dnx_field_qual_t dnx_qual_2[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_2[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int *fully_shareable_p,
    int *partially_shareable_p,
    int *partial_sharing_lsb_offset_p)
{
    int qual_ndx;
    dnx_field_key_attached_qual_info_t qual_info_1[DNX_FIELD_KEY_MAX_SUB_QUAL];
    dnx_field_key_attached_qual_info_t qual_info_2[DNX_FIELD_KEY_MAX_SUB_QUAL];
    int qual_nof_mapping_1;
    int qual_nof_mapping_2;
    int qual_part_index;
    int qual_part_index_1;
    int qual_part_index_2;

    SHR_FUNC_INIT_VARS(unit);

    (*fully_shareable_p) = FALSE;
    (*partially_shareable_p) = FALSE;
    (*partial_sharing_lsb_offset_p) = 0;

    /*
     * At this point we only compare the qualifier arrays one qualifier at the time, and do not combine qualifiers.
     */
    if ((nof_quals_1 == nof_quals_2) && (nof_quals_1 > 0))
    {
        int shareable = TRUE;
        for (qual_ndx = 0; shareable && (qual_ndx < nof_quals_1); qual_ndx++)
        {
            shareable = FALSE;
            /*
             * Qualifiers must have the same input type.
             */
            if (qual_attach_info_1[qual_ndx].input_type == qual_attach_info_2[qual_ndx].input_type)
            {
                /*
                 * At the moment, only certain input types are supported.
                 */
                if (qual_attach_info_1[qual_ndx].input_type == DNX_FIELD_INPUT_TYPE_LAYER_FWD ||
                    qual_attach_info_1[qual_ndx].input_type == DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE ||
                    qual_attach_info_1[qual_ndx].input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
                    qual_attach_info_1[qual_ndx].input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE ||
                    qual_attach_info_1[qual_ndx].input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
                {
                    /*
                     * Qualifier must have the same size, and the same FFC field offset.
                     */
                    int dummy_offset_on_key = 0;
                    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                                    (unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_CONTEXT_ID_INVALID,
                                     dnx_qual_1[qual_ndx], dummy_offset_on_key, &qual_attach_info_1[qual_ndx],
                                     &qual_nof_mapping_1, qual_info_1));
                    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                                    (unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_CONTEXT_ID_INVALID,
                                     dnx_qual_2[qual_ndx], dummy_offset_on_key, &qual_attach_info_2[qual_ndx],
                                     &qual_nof_mapping_2, qual_info_2));
                    /** Sanity check. */
                    if ((qual_nof_mapping_1 < 1) || (qual_nof_mapping_2 < 1))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal number of parst (%d and %d).\n",
                                     qual_nof_mapping_1, qual_nof_mapping_2);
                    }
                    /*
                     * Size, FFC field offset and, for non metadata input types, index, should match
                     * for full shareability.
                     */
                    if (qual_nof_mapping_1 == qual_nof_mapping_2)
                    {
                        for (qual_part_index = 0; qual_part_index < qual_nof_mapping_1; qual_part_index++)
                        {
                            if ((qual_info_1[qual_part_index].size != qual_info_2[qual_part_index].size) ||
                                (qual_info_1[qual_part_index].offset != qual_info_2[qual_part_index].offset) ||
                                (qual_info_1[qual_part_index].index != qual_info_2[qual_part_index].index))
                            {
                                break;
                            }
                        }
                        if (qual_part_index == qual_nof_mapping_1)
                        {
                            shareable = TRUE;
                        }
                    }

                    /*
                     * Attempt to pefrorm partial sharing, where dnx_qual_2 is a part of dnx_qual_1.
                     * At this point we only support partial sharing for single qualifiers.
                     */
                    (*partial_sharing_lsb_offset_p) = 0;
                    (*partially_shareable_p) = FALSE;
                    if ((shareable == FALSE) && (nof_quals_1 == 1) && (nof_quals_2 == 1))
                    {
                        int offset_from_start = 0;
                        int current_part_is_partial;
                        int current_part_is_full;
                        /** find where the offset of the start of dnx_qual_2 is on dnx_qual_1*/
                        for (qual_part_index_1 = 0; qual_part_index_1 < qual_nof_mapping_1; qual_part_index_1++)
                        {
                            if ((qual_info_1[qual_part_index_1].offset <= qual_info_2[0].offset) &&
                                (qual_info_1[qual_part_index_1].offset + qual_info_1[qual_part_index_1].size >
                                 qual_info_2[0].offset))
                            {
                                offset_from_start += qual_info_2[0].offset - qual_info_1[qual_part_index_1].offset;
                                break;
                            }
                            offset_from_start += qual_info_1[qual_part_index_1].size;
                        }
                        /** If starting point was found.*/
                        if (qual_part_index_1 < qual_nof_mapping_1)
                        {
                            current_part_is_partial = FALSE;
                            current_part_is_full = FALSE;
                            /** Check if the first part fit inside.*/
                            if (qual_info_1[0].index == qual_info_2[0].index)
                            {
                                if ((qual_info_2[0].offset - qual_info_1[qual_part_index_1].offset) +
                                    qual_info_2[0].size <= qual_info_1[qual_part_index_1].size)
                                {
                                    current_part_is_partial = TRUE;
                                }
                                if ((qual_info_2[0].offset - qual_info_1[qual_part_index_1].offset) +
                                    qual_info_2[0].size == qual_info_1[qual_part_index_1].size)
                                {
                                    current_part_is_full = TRUE;
                                }
                            }
                            /*
                             * Check the rest of the parts. Each part must be fully hasreable to continue to the next. 
                             */
                            for (qual_part_index_1++, qual_part_index_2 = 1;
                                 (qual_part_index_1 < qual_nof_mapping_1) && (qual_part_index_2 < qual_nof_mapping_2) &&
                                 current_part_is_full; qual_part_index_1++, qual_part_index_2++)
                            {
                                current_part_is_partial = FALSE;
                                current_part_is_full = FALSE;
                                if ((qual_info_2[qual_part_index_2].offset != qual_info_1[qual_part_index_1].offset) ||
                                    (qual_info_2[qual_part_index_2].index != qual_info_1[qual_part_index_1].index))
                                {
                                    break;
                                }
                                if (qual_info_2[qual_part_index_2].size <= qual_info_1[qual_part_index_1].size)
                                {
                                    current_part_is_partial = TRUE;
                                }
                                if (qual_info_2[qual_part_index_2].size == qual_info_1[qual_part_index_1].size)
                                {
                                    current_part_is_full = TRUE;
                                }
                            }
                            if ((qual_part_index_2 == qual_nof_mapping_2) && current_part_is_partial)
                            {
                                (*partial_sharing_lsb_offset_p) = offset_from_start;
                                (*partially_shareable_p) = TRUE;
                            }
                        }
                    }

                    /** Sanity check.*/
                    if ((shareable || (*partially_shareable_p)) &&
                        ((qual_info_1[0].input_type != qual_info_2[0].input_type) ||
                         (qual_info_1[0].ffc_type != qual_info_2[0].ffc_type)))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch for shareable qualifiers, input type is %d, "
                                     "but after parsing qualifiers 0x%x and 0x%x, we get input type s %d and %d, "
                                     "and FFC types %d and %d.\n",
                                     qual_attach_info_1[qual_ndx].input_type,
                                     dnx_qual_1[qual_ndx], dnx_qual_2[qual_ndx],
                                     qual_info_1[0].input_type, qual_info_2[0].input_type,
                                     qual_info_1[0].ffc_type, qual_info_2[0].ffc_type);
                    }
                }
            }
        }
        (*fully_shareable_p) = shareable;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function provides the key and location on key of a certain segement on KBP master key.
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] fwd_context      - The FWD context
 * \param [in] acl_context_id   - The ACL context
 * \param [in] seg_idx_on_master_key - The segemnt insex on the master key (from MSB to LSB)
 * \param [out] key_kbp_id_p    - The key on which the segment resides
 * \param [out] lsb_on_key_p    - The offset of the LSB of the segment (from the LSB of the key).
 * \param [out] size_on_key_p   - The part of the segment in bits on the same key as the LSB of the segment.
 *                                Can be smaller the segment size if the segment straddles multiple keys.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_segment_position(
    int unit,
    uint32 opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p)
{
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key_index;
    uint32 seg_size_bits;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint32 fwd_key_size_aligner_in_bytes;
    uint32 acl_key_size_aligner_in_bytes;
    uint32 key_size_aligner[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_KEYS_TOTAL];
    uint32 entry_handle_id;
    uint32 max_single_kbp_key_size;
    int seg_index;
    uint32 nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);
    uint32 segment_lsb_on_master_key_from_msb;
    uint32 segment_lsb_on_key_from_msb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_kbp_id_p, _SHR_E_PARAM, "key_kbp_id_p");
    SHR_NULL_CHECK(lsb_on_key_p, _SHR_E_PARAM, "lsb_on_key_p");
    SHR_NULL_CHECK(size_on_key_p, _SHR_E_PARAM, "size_on_key_p");

    max_single_kbp_key_size = dnx_data_field.external_tcam.max_single_key_size_get(unit);

    /*
     * Get the segments used by the opcode.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    /** Chcek that the segment index exists.*/
    if (seg_idx_on_master_key >= nof_segments || seg_idx_on_master_key < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Opcode %d has %d segments, requested segment %d.\n",
                     opcode_id, nof_segments, seg_idx_on_master_key);
    }
    if (ms_key_segments[seg_idx_on_master_key].is_overlay_field)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find position of overlay segment. segment %d in opcode %d.\n",
                     seg_idx_on_master_key, opcode_id);
    }
    seg_size_bits = BYTES2BITS(ms_key_segments[seg_idx_on_master_key].nof_bytes);
    /** Check that the segment size isn't zero.*/
    if (seg_size_bits <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Opcode %d segment %d has %d bytes %d bits, cannot be zero.\n",
                     opcode_id, seg_idx_on_master_key, ms_key_segments[seg_idx_on_master_key].nof_bytes, seg_size_bits);
    }

    /** Get the key size in the aligner for each key. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));
    for (kbp_key_index = 0; kbp_key_index < nof_kbp_keys; kbp_key_index++)
    {
        /** Get the part of the key used by FWD, according to fwd_context.*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key_index);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &fwd_key_size_aligner_in_bytes));

        /** Get the part of the key used by ACL, according to acl_context.*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE, &acl_key_size_aligner_in_bytes));

        /** Sum of both ACL and FWD key sizes to retrieve the total size used from current KBR. */
        key_size_aligner[kbp_key_index] = BYTES2BITS(fwd_key_size_aligner_in_bytes + acl_key_size_aligner_in_bytes);

        /** Sanity check*/
        if (key_size_aligner[kbp_key_index] > max_single_kbp_key_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Opcode %d key %d has fwd aligner %d bytes and acl aligner %d bytes, together %d bits, "
                         "more than maximum %d.\n",
                         opcode_id, kbp_key_index, fwd_key_size_aligner_in_bytes, acl_key_size_aligner_in_bytes,
                         key_size_aligner[kbp_key_index], max_single_kbp_key_size);
        }
    }

    /*
     * Iterate over master key segments to find the location on master key.
     * Note that the segment are placed from MSB to LSB.
     */
    segment_lsb_on_master_key_from_msb = 0;
    for (seg_index = 0; seg_index <= seg_idx_on_master_key; seg_index++)
    {
        if (ms_key_segments[seg_index].is_overlay_field == FALSE)
        {
            segment_lsb_on_master_key_from_msb += BYTES2BITS(ms_key_segments[seg_index].nof_bytes);
        }
    }

    /*
     * Iterate over the keys to find the position on key.
     */
    segment_lsb_on_key_from_msb = segment_lsb_on_master_key_from_msb;
    for (kbp_key_index = 0;
         (kbp_key_index < nof_kbp_keys) && (segment_lsb_on_key_from_msb > key_size_aligner[kbp_key_index]);
         kbp_key_index++)
    {
        segment_lsb_on_key_from_msb -= key_size_aligner[kbp_key_index];
    }

    /** Sanity check*/
    if (kbp_key_index >= nof_kbp_keys)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Opcode %d segment %d has LSB on master key from LSB %d bits, not fitting into aligner.\n",
                     opcode_id, seg_idx_on_master_key, segment_lsb_on_master_key_from_msb);
    }

    /*
     * Set the key.
     */
    (*key_kbp_id_p) = kbp_key_index;

    /*
     * Check if the segment straddles multiple keys.
     */
    if (segment_lsb_on_key_from_msb >= seg_size_bits)
    {
        (*size_on_key_p) = seg_size_bits;
    }
    else
    {
        (*size_on_key_p) = segment_lsb_on_key_from_msb;
    }

    /*
     * Find the offset from the lsb.
     */
    (*lsb_on_key_p) = max_single_kbp_key_size - segment_lsb_on_key_from_msb;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function provides the key and location on key of a certain FWD segement on KBP master key,
 *  and verifies it is the same for all fwd contexts
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] seg_idx_on_master_key - The segemnt insex on the master key (from MSB to LSB)
 * \param [out] key_kbp_id_p    - The key on which the segment resides
 * \param [out] lsb_on_key_p    - The offset of the LSB of the segment (from the LSB of the key).
 * \param [out] size_on_key_p   - The part of the segment in bits on the same key as the LSB of the segment.
 *                                Can be smaller the segment size if the segment straddles multiple keys.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_fwd_segment_unified_position(
    int unit,
    uint32 opcode_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p)
{
    int fwd_ctx_index;
    dbal_enum_value_field_kbp_kbr_idx_e key_kbp_id_first = DNX_FIELD_KEY_ID_INVALID;
    int lsb_on_key_first = -1;
    int size_on_key_first = -1;
    uint8 nof_fwd_seg;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_kbp_id_p, _SHR_E_PARAM, "key_kbp_id_p");
    SHR_NULL_CHECK(lsb_on_key_p, _SHR_E_PARAM, "lsb_on_key_p");
    SHR_NULL_CHECK(size_on_key_p, _SHR_E_PARAM, "size_on_key_p");

    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_seg));
    if (seg_idx_on_master_key >= nof_fwd_seg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode %d has %d fwd segments, requested segment %d.\n",
                     opcode_id, nof_fwd_seg, seg_idx_on_master_key);
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts(unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
    if (nof_fwd_contexts <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode %d has %d fwd contexts.\n", opcode_id, nof_fwd_contexts);
    }
    if (nof_fwd_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode %d has %d fwd contexts, maximum is %d.\n",
                     opcode_id, nof_fwd_contexts, DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM);
    }
    for (fwd_ctx_index = 0; fwd_ctx_index < nof_fwd_contexts; fwd_ctx_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_segment_position(unit, opcode_id,
                                                           fwd_contexts[fwd_ctx_index],
                                                           acl_contexts[fwd_ctx_index],
                                                           seg_idx_on_master_key, key_kbp_id_p, lsb_on_key_p,
                                                           size_on_key_p));
        if (fwd_ctx_index == 0)
        {
            key_kbp_id_first = *key_kbp_id_p;
            lsb_on_key_first = *lsb_on_key_p;
            size_on_key_first = *size_on_key_p;
        }
        else
        {
            if ((key_kbp_id_first != *key_kbp_id_p) || (lsb_on_key_first != *lsb_on_key_p) ||
                (size_on_key_first != *size_on_key_p))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode %d has different positions depending "
                             "on the fwd_context. First differing fwd_context is %d (index %d). "
                             "key_kbp_id (%d and %d) lsb_on_key (%d and %d) lsb_on_key (%d and %d.\n",
                             opcode_id, fwd_contexts[fwd_ctx_index], fwd_ctx_index,
                             key_kbp_id_first, *key_kbp_id_p, lsb_on_key_first, *lsb_on_key_p,
                             size_on_key_first, *size_on_key_p);
            }
        }
    }

    /** Sanity check*/
    if (key_kbp_id_first == DNX_FIELD_KEY_ID_INVALID || lsb_on_key_first == -1 || size_on_key_first == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode %d value never set. "
                     "key_kbp_id_first %d lsb_on_key_first %d size_on_key_first %d.\n",
                     opcode_id, key_kbp_id_first, lsb_on_key_first, size_on_key_first);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_key_master_offset_to_key(
    int unit,
    uint32 opcode_id,
    int offset_on_master_from_msb,
    int size_on_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_last_p,
    uint32 *keys_bmp_p,
    int *offset_on_last_key_p)
{
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key_index;
    uint32 fwd_key_size_aligner_in_bytes;
    uint32 acl_key_size_aligner_in_bytes;
    uint32 fwd_key_size_aligner_in_bytes_prev = -1;
    uint32 acl_key_size_aligner_in_bytes_prev = -1;
    uint32 key_size_aligner[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_KEYS_TOTAL];
    uint32 entry_handle_id;
    uint32 max_single_kbp_key_size;
    uint32 nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);
    uint32 lsb_on_key_from_msb;
    uint32 msb_on_key_from_msb;
    int context_index;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(keys_bmp_p, _SHR_E_PARAM, "keys_bmp_p");
    SHR_NULL_CHECK(offset_on_last_key_p, _SHR_E_PARAM, "offset_on_last_key_p");

    if (size_on_key <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Function supports only positive sie, size given %d.\n", size_on_key);
    }

    max_single_kbp_key_size = dnx_data_field.external_tcam.max_single_key_size_get(unit);

    /*
     * Get the FWD and ACL contexts.
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts(unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
    /** Sanity check.*/
    if (nof_fwd_contexts <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode %d had %d FWD contexts.\n", opcode_id, nof_fwd_contexts);
    }

    /** Get the key size in the aligner for each key. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));
    for (kbp_key_index = 0; kbp_key_index < nof_kbp_keys; kbp_key_index++)
    {
        /*
         * Verify that the result if the same for all contexts.
         */
        for (context_index = 0; context_index < nof_fwd_contexts; context_index++)
        {
            /** Get the part of the key used by FWD, according to fwd_context.*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_contexts[context_index]);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key_index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE,
                             &fwd_key_size_aligner_in_bytes));

            /** Get the part of the key used by ACL, according to acl_context.*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_contexts[context_index]);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE,
                             &acl_key_size_aligner_in_bytes));

            /** Sum of both ACL and FWD key sizes to retrieve the total size used from current KBR. */
            key_size_aligner[kbp_key_index] = BYTES2BITS(fwd_key_size_aligner_in_bytes + acl_key_size_aligner_in_bytes);

            /** Sanity check*/
            if (key_size_aligner[kbp_key_index] > max_single_kbp_key_size)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Opcode %d key %d has fwd aligner %d bytes and acl aligner %d bytes, together %d bits, "
                             "more than maximum %d.\n",
                             opcode_id, kbp_key_index, fwd_key_size_aligner_in_bytes, acl_key_size_aligner_in_bytes,
                             key_size_aligner[kbp_key_index], max_single_kbp_key_size);
            }
            /*
             * Verify that the result if the same for all contexts.
             */
            if (context_index > 0)
            {
                if (fwd_key_size_aligner_in_bytes_prev != fwd_key_size_aligner_in_bytes)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "FWD aligner size not identical for all FWD contexts. Opcode %d. "
                                 "Aligner size for fwd context %d is %d. "
                                 "Aligner size for fwd context %d is %d.\n",
                                 opcode_id, fwd_contexts[context_index - 1], fwd_key_size_aligner_in_bytes_prev,
                                 fwd_contexts[context_index], fwd_key_size_aligner_in_bytes);
                }
                if (acl_key_size_aligner_in_bytes_prev != acl_key_size_aligner_in_bytes)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "ACL aligner size not identical for all ACL contexts. Opcode %d. "
                                 "Aligner size for acl context %d is %d. "
                                 "Aligner size for acl context %d is %d.\n",
                                 opcode_id, acl_contexts[context_index - 1], acl_key_size_aligner_in_bytes_prev,
                                 acl_contexts[context_index], acl_key_size_aligner_in_bytes);
                }
            }
            fwd_key_size_aligner_in_bytes_prev = fwd_key_size_aligner_in_bytes;
            acl_key_size_aligner_in_bytes_prev = acl_key_size_aligner_in_bytes;
        }
    }

    /*
     * Iterate over the keys to find the position on key.
     */
    msb_on_key_from_msb = offset_on_master_from_msb + 1;
    for (kbp_key_index = 0;
         (kbp_key_index < nof_kbp_keys) && (msb_on_key_from_msb > key_size_aligner[kbp_key_index]); kbp_key_index++)
    {
        msb_on_key_from_msb -= key_size_aligner[kbp_key_index];
    }

    /** Sanity check*/
    if (kbp_key_index >= nof_kbp_keys)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d offset %d, not fitting into aligner.\n",
                     opcode_id, offset_on_master_from_msb);
    }

    /*
     * Set the first key to bitmap
     */
    (*keys_bmp_p) = SAL_BIT(kbp_key_index);

    /*
     * Set the rest of the keys.
     */
    lsb_on_key_from_msb = msb_on_key_from_msb + size_on_key - 1;
    for (; (kbp_key_index < nof_kbp_keys) && (lsb_on_key_from_msb > key_size_aligner[kbp_key_index]); kbp_key_index++)
    {
        if (key_size_aligner[kbp_key_index] > 0)
        {
            (*keys_bmp_p) |= SAL_BIT(kbp_key_index);
            lsb_on_key_from_msb -= key_size_aligner[kbp_key_index];
        }
    }

    /** Sanity check*/
    if (kbp_key_index >= nof_kbp_keys)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d offset %d, size %d, not fitting into aligner.\n",
                     opcode_id, offset_on_master_from_msb, size_on_key);
    }

    /*
     * Set the last key to bitmap
     */
    (*keys_bmp_p) = SAL_BIT(kbp_key_index);

    (*key_kbp_id_last_p) = kbp_key_index;

    /*
     * Find the offset from the lsb.
     */
    (*offset_on_last_key_p) = max_single_kbp_key_size - lsb_on_key_from_msb;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find an FFC_ID available for a list of contexts.
 * \param [in] unit                     - Device Id
 * \param [in] field_stage              - Stage. At the moment only external stage is supported for this function.
 * \param [in] contexts_to_share_ffc    - Array of contexts.
 * \param [in] nof_context_to_share_ffc - number of contexts in contexts_to_share_ffc
 * \param [out] ffc_id_p                - FFC ID available in all contexts.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_find_common_available_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t contexts_to_share_ffc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS],
    unsigned int nof_context_to_share_ffc,
    int *ffc_id_p)
{
    int context_idx;
    int nof_ffcs_in_stage = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc;
    int ffc_id;
    uint8 is_alloc;
    int ffc_found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_id_p, _SHR_E_PARAM, "ffc_id_p");

    if (field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "function only supports external \"%s\", requested stage \"%s\".\n",
                     dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL), dnx_field_stage_text(unit, field_stage));
    }

    if (nof_context_to_share_ffc > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_context_to_share_ffc %d larger than maximum %d.\n",
                     nof_context_to_share_ffc, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);
    }
    if (nof_context_to_share_ffc <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_context_to_share_ffc %d must be at least one.\n", nof_context_to_share_ffc);
    }

    ffc_found = FALSE;
    for (ffc_id = 0; (ffc_id < nof_ffcs_in_stage) && (ffc_found == FALSE); ffc_id++)
    {
        ffc_found = TRUE;
        for (context_idx = 0; context_idx < nof_context_to_share_ffc; context_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_is_allocated
                            (unit, contexts_to_share_ffc[context_idx], ffc_id, &is_alloc));
            if (is_alloc)
            {
                ffc_found = FALSE;
                break;
            }
        }
    }

    if (ffc_found)
    {
        (*ffc_id_p) = ffc_id - 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find FFC ID shared by all contexts.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function configures an FFC for the filling a zero padding segment.
 *
 * \param [in] unit           - Device Id
 * \param [in] opcode_id      - Opcode with the zero padding segment
 * \param [in] segment_id     - The zero padding segemnt index.
 * \param [in] segment_info_p - One of the field group and qualifier index that use the segment
 *                              (fill the zero padding segment) in the opcode. Useed to determine how the qualifier
 *                              looks. If the opcode isn't using zero padding, we take the segment info of another
 *                              opcode that does.
 * \param [in] is_zero_padding - Indicates if we perform an actual zero padding or just fill the FFC info so it
 *                               wouldn't write outsize the zero padding, as the KBR is controlled by the FWD context.
 * \param [in] opcode_id_that_uses_zero_padding - If the segment isn't zero padding for the opcode, we take another
 *                                                opcode where it is zero padded for the qualifier info.
 * \param [in] ffc_id         - The ffc_id to allocate (must be shared by all opcodes and all contexts.)
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_kbp_fwd_zero_padding_fill(
    int unit,
    uint32 opcode_id,
    int segment_id,
    dnx_field_kbp_segment_info_t * segment_info_p,
    int is_zero_padding,
    uint32 opcode_id_that_uses_zero_padding,
    int ffc_id)
{
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
    int nof_sub_quals;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dbal_enum_value_field_kbp_kbr_idx_e key_kbp_id = DNX_FIELD_KEY_ID_INVALID;
    int lsb_on_key = -1;
    int seg_size_on_key;
    dbal_enum_value_field_field_key_e fwd_key_id;
    dnx_field_key_template_t key_template;
    dnx_field_qual_t qual_type;
    int context_index;
    dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 opcode_id_for_qual_info;
    uint8 ctx_is_dynamic;
    uint8 opcode_is_dynamic;
    int dummy_offset_on_key = 0;

    int unpacked_qual_ind = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (is_zero_padding)
    {
        opcode_id_for_qual_info = opcode_id;
    }
    else
    {
        opcode_id_for_qual_info = opcode_id_that_uses_zero_padding;
    }

    if (segment_info_p->fg_id == DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment_id %d isn't a filled zero padding segment.\n",
                     opcode_id_for_qual_info, segment_id);
    }
    if (segment_info_p->qual_idx[unpacked_qual_ind] >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG ||
        (unsigned int) segment_info_p->qual_idx[unpacked_qual_ind] >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "segment_info_p->qual_idx %d exceeds maximum %d.\n",
                     segment_info_p->qual_idx[unpacked_qual_ind], DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, segment_info_p->fg_id, &key_template));

    /*
     * Qualifier type is pointed by the qual_in_key_template_idx, extracted from the fg_info
     */
    qual_type = key_template.key_qual_map[segment_info_p->qual_idx[unpacked_qual_ind]].qual_type;

    /*
     * Qualifier attach_info is pointed by the qual_in_key_template_idx, extracted from the fg_info
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get
                    (unit, segment_info_p->fg_id, opcode_id_for_qual_info, qual_attach_info_arr));

    /*
     * Get the qual_info based on the qual_type+attach_info
     */
    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_CONTEXT_ID_INVALID, qual_type, dummy_offset_on_key,
                     &qual_attach_info_arr[segment_info_p->qual_idx[unpacked_qual_ind]],
                     &nof_sub_quals, dnx_qual_info));
    if (nof_sub_quals != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Attempting to zero pad with qualifier with multiple mappings (%d).\n",
                     nof_sub_quals);
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts(unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
    for (context_index = 0; context_index < nof_fwd_contexts; context_index++)
    {
        /** Sanity check.*/
        SHR_IF_ERR_EXIT(kbp_mngr_context_acl_is_dynamic(unit, acl_contexts[context_index], &ctx_is_dynamic));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &opcode_is_dynamic));
        if ((!ctx_is_dynamic) != (!opcode_is_dynamic))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Opcode %d acl context %d mismatch between ctx_is_dynamic %d and opcode_is_dynamic %d.\n",
                         opcode_id, acl_contexts[context_index], ctx_is_dynamic, opcode_is_dynamic);
        }
        if ((opcode_is_dynamic == FALSE) && (fwd_contexts[context_index] != acl_contexts[context_index]))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Opcode %d is predefined, but fwd context %d doesn't equal acl context %d.\n",
                         opcode_id, fwd_contexts[context_index], acl_contexts[context_index]);
        }
        if (opcode_is_dynamic && (context_index > 0) &&
            (acl_contexts[context_index] != acl_contexts[context_index - 1]))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Opcode %d is usr defined, it has more than one ACL contexts (%d and %d).\n",
                         opcode_id, acl_contexts[context_index], acl_contexts[context_index - 1]);
        }
        /** User defined contexts have only one ACL context.*/
        if (opcode_is_dynamic && (context_index > 0))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_segment_position
                        (unit, opcode_id, fwd_contexts[context_index], acl_contexts[context_index], segment_id,
                         &key_kbp_id, &lsb_on_key, &seg_size_on_key));
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_to_fwd2_key_id_convert(unit, key_kbp_id, &fwd_key_id));

        dnx_qual_info[0].offset_on_key = lsb_on_key;

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set
                        (unit, DNX_FIELD_STAGE_EXTERNAL, acl_contexts[context_index], &(dnx_qual_info[0]),
                         fwd_key_id, TRUE, ffc_id));

        /** Sanity check*/
        if (key_kbp_id == DNX_FIELD_KEY_ID_INVALID || lsb_on_key == -1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode %d segment %d value never set. key_kbp_id %d lsb_on_key %d.\n",
                         opcode_id, segment_id, key_kbp_id, lsb_on_key);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_key_kbp_set(
    int unit,
    uint32 opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_kbp_opcode_info_t * kbp_opcode_info_p)
{
    int seg_idx;
    dnx_field_key_template_t key_template;
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX];
    uint32 location_in_key;
    uint32 key_idx;
    uint32 key_idx_3;
    uint32 key_size[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX];
    uint32 key_size_inverted[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_NOF_ACL_KEYS_MASTER_MAX];
    uint32 qual_lsb_aligning_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 qual_lsb_unaligned[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 qual_key_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    int ffc_needed[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
    uint32 qual_in_key_template_idx_arr[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY][DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
    dnx_field_group_t fg_id_arr[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 aligned_location_on_key;
    uint8 nof_fwd_seg;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    int packed_qual_segment;

    SHR_FUNC_INIT_VARS(unit);

    for (seg_idx = 0; seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; seg_idx++)
    {
        fg_id_arr[seg_idx] = DNX_FIELD_GROUP_INVALID;
        for (packed_qual_segment = 0; packed_qual_segment < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_segment++)
        {
            qual_in_key_template_idx_arr[seg_idx][packed_qual_segment] = DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID;
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_seg));

    /*
     * Get the master key from KBP to find out which segments are overlay segments.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));

    /** Sanity check*/
    if ((unsigned int) nof_fwd_seg > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_fwd_segments is %d, must be between %d and %d. Opcode ID %d\n",
                     nof_fwd_seg, opcode_id, 0, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1);
    }

    /*
     * Get the key id and the free offset inside that key for the KBP use
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_master_key_id_get(unit, fwd_context, key_ids));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Processing key FFC for opcode %d.%s%s%s\n", opcode_id, EMPTY, EMPTY, EMPTY);

    location_in_key = 0;

    key_idx = 0;

    /*
     * We calculate the location of each qualifier on key. Note that the segments run from MSB to lsb so we iterate
     * over the KBP segments from high to low.
     */
    for (seg_idx = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx >= nof_fwd_seg; seg_idx--)
    {
        dnx_field_qual_t qual_type_last_packed;
        uint32 qual_size_single_packed;
        uint32 qual_size_within_segment;
        uint32 qual_in_key_template_idx[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
        int nof_quals_per_segment;
        dnx_field_group_t fg_id;
        uint32 aligned_offset_in_key;
        int is_packed_segment;
        int is_support_packing;

        /*
         * Initialize qual_in_key_template_idx
         */
        for (packed_qual_segment = 0; packed_qual_segment < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_segment++)
        {
            qual_in_key_template_idx[packed_qual_segment] = DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID;
        }

        /*
         * Initializing qual_type_last_packed to silence the compiler
         */
        qual_type_last_packed = DNX_FIELD_QUAL_TYPE_INVALID;

        if (kbp_opcode_info_p->master_key_info.segment_info[seg_idx].fg_id == DNX_FIELD_GROUP_INVALID)
        {
            /*
             * Sanity check.
             */
            if (seg_idx < nof_segments)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has invalid fg_id in SW state but is above "
                             "number of FWD segments %d and below the number of segments in opcode, %d.\n",
                             opcode_id, seg_idx, nof_fwd_seg, nof_segments);
            }
            continue;
        }
        /*
         * Preparation stage begins.
         */

        /*
         * Extract the fg_id from the given segment info
         */
        fg_id = kbp_opcode_info_p->master_key_info.segment_info[seg_idx].fg_id;

        /*
         * Sanity check
         */
        if (seg_idx >= nof_segments)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has fg_id %d in SW state but is above "
                         "number of segments in opcode, %d.\n", opcode_id, seg_idx, fg_id, nof_segments);
        }

        /*
         * Overlay segments are not on the key.
         */
        /*
         * Get which qualifiers are overlay segments and should not be added to master key.
         */
        if (ms_key_segments[seg_idx].is_overlay_field)
        {
            continue;
        }

        /*
         * Get the key template for the field group using a given segment
         */
        SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));

        qual_size_within_segment = 0;
        for (packed_qual_segment = 0; packed_qual_segment < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_segment++)
        {

            /*
             * Extract the qualifier index from the given segment info. This is the index of the qualifier_type AND of the
             * attach_info for this qualifier inside the fg_info.
             */
            qual_in_key_template_idx[packed_qual_segment] =
                kbp_opcode_info_p->master_key_info.segment_info[seg_idx].qual_idx[packed_qual_segment];

            if (qual_in_key_template_idx[packed_qual_segment] == DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID)
            {
                break;
            }

            /*
             * Qualifier type is pointed by the qual_in_key_template_idx, extracted from the fg_info
             */
            qual_type_last_packed = key_template.key_qual_map[qual_in_key_template_idx[packed_qual_segment]].qual_type;
            /*
             * Get the qual size.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type_last_packed,
                             NULL, NULL, &qual_size_single_packed));
            qual_size_within_segment += qual_size_single_packed;

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Processed segment %d qualifier number %d \"%s\" size %d .\n",
                         seg_idx, packed_qual_segment,
                         dnx_field_dnx_qual_text(unit, qual_type_last_packed), qual_size_single_packed);
        }

        nof_quals_per_segment = packed_qual_segment;

        if (nof_quals_per_segment == 1)
        {
            is_packed_segment = FALSE;
        }
        else if (nof_quals_per_segment > 1)
        {
            is_packed_segment = TRUE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has fg_id %d but no qualifiers. "
                         "packed_qual_segment %d.\n", opcode_id, seg_idx, fg_id, nof_quals_per_segment);
        }

        SHR_IF_ERR_EXIT(dnx_field_group_external_tcam_support_packing(unit, fg_id, &is_support_packing));
        if (is_packed_segment && (is_support_packing == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "opcode_id %d segment %d has fg_id %d that does not support packing, but it has %d qualifiers.\n",
                         opcode_id, seg_idx, fg_id, nof_quals_per_segment);
        }

        /*
         * Sanity check
         */
        if (BITS2BYTES(qual_size_within_segment) != ms_key_segments[seg_idx].nof_bytes)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has fg_id %d in SW state and size "
                         "%d in bits (%d bytes), but in KBP the segment has size %d bytes. "
                         "Segment has %d qualfiers, the last is \"%s\".\n",
                         opcode_id, seg_idx, fg_id, qual_size_within_segment, BITS2BYTES(qual_size_within_segment),
                         ms_key_segments[seg_idx].nof_bytes, nof_quals_per_segment,
                         dnx_field_dnx_qual_text(unit, qual_type_last_packed));
        }

        /*
         * (Byte) Aligned offset in key is the offset we add to the beginning of the ffc content in order to align the
         * segments in the master key.
         * In case the content is not byte aligned we will add the
         * (BYTE - mod(qual_size)) to the beginning of the content (location_in_key)
         */
        aligned_offset_in_key =
            ((qual_size_within_segment % SAL_UINT8_NOF_BITS) ?
             (SAL_UINT8_NOF_BITS - (qual_size_within_segment % SAL_UINT8_NOF_BITS)) : 0);
        /*
         * Add mod8 to the location. (segment location should be byte aligned).
         * For this matter, we align the qualifier to LSB.
         * Thus, we add the modulo8 to the location_in_key, and the size of the qualifier
         * will be aligned inside the key.
         */
        location_in_key += aligned_offset_in_key;

        /*
         * Verify that the size of the (aligned) qualifier fits into the first key. If the size of the qualifier
         * doesn't fit, start using the next key.
         */
        if (location_in_key + qual_size_within_segment > dnx_data_field.external_tcam.max_single_key_size_get(unit))
        {
            if (key_idx + 1 >= dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "KBP key for ACL lookups in opcode_id %d context %d does not fit into "
                             "maximum %d keys for KBP ACL.\n",
                             opcode_id, acl_context_id, dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit));
            }
            if ((key_idx + 1 >= sizeof(key_ids) / sizeof(key_ids[0])) ||
                key_ids[key_idx + 1] == DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "KBP key for ACL lookups in opcode_id %d context %d does not fit into "
                             "%d keys available for ACL lookups.\n",
                             opcode_id, acl_context_id, dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit));
            }

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Moving to next Key %d because segment %d doesn't fit. "
                         "Current offset in key %d. Segment has %d qualifiers.\n",
                         key_ids[key_idx + 1], seg_idx, location_in_key, nof_quals_per_segment);

            key_size_inverted[key_idx] = location_in_key - aligned_offset_in_key;

            /*
             * Switch to the next key
             */
            location_in_key = aligned_offset_in_key;
            key_idx++;
        }

        /*
         * Updated the location of the FFC to be configured.
         */
        qual_lsb_aligning_size[seg_idx] = aligned_offset_in_key;
        qual_lsb_unaligned[seg_idx] = location_in_key;
        qual_key_index[seg_idx] = key_idx;
        fg_id_arr[seg_idx] = fg_id;
        ffc_needed[seg_idx] = TRUE;
        for (packed_qual_segment = 0; packed_qual_segment < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_segment++)
        {
            qual_in_key_template_idx_arr[seg_idx][packed_qual_segment] = qual_in_key_template_idx[packed_qual_segment];
        }

        /*
         * location_in_key is used as a pointer inside the key. we add the size of each qualifier in the segment to the
         * location.
         */
        location_in_key += qual_size_within_segment;
    }

    key_size_inverted[key_idx] = location_in_key;

    /*
     * Reverse the order of the keys as the first key is filled from the MSB by DBAL.
     */
    {
        int seg_idx_2;
        int key_idx_2;

        /*
         * Swap the keys for each qualifier.
         */
        if (key_idx > 0)
        {
            for (seg_idx_2 = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx_2 >= 0; seg_idx_2--)
            {
                qual_key_index[seg_idx_2] = key_idx - qual_key_index[seg_idx_2];
            }
        }
        /*
         * Swap the key size for each key.
         */
        for (key_idx_2 = 0; key_idx_2 <= key_idx; key_idx_2++)
        {
            key_size[key_idx_2] = key_size_inverted[key_idx - key_idx_2];
        }
    }
    /*
     * Update the master key length for the given key.
     */
    for (key_idx_3 = 0; key_idx_3 <= key_idx; key_idx_3++)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_aligner_mapping_mem_set
                        (unit, acl_context_id, key_ids[key_idx_3], key_size[key_idx_3]));
    }

    /*
     * Configure the FFCs
     */
    for (seg_idx = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx >= 0; seg_idx--)
    {
        uint32 qual_size_already_configure;
        if (ffc_needed[seg_idx] == FALSE)
        {
            continue;
        }

        qual_size_already_configure = 0;

        /*
         * Configure for each qualifier in the segemnt an FFC.
         */
        for (packed_qual_segment = 0; packed_qual_segment < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_segment++)
        {
            dnx_field_qual_t qual_type;
            dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL];
            int nof_sub_quals;
            dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
            int qual_in_key_template_index;
            int size_total = 0;
            int sub_qual_ndx;
            int dummy_offset_on_key = 0;

            if (kbp_opcode_info_p->master_key_info.segment_info[seg_idx].fg_id == DNX_FIELD_GROUP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has invalid fg_id in SW state "
                             "yet it requires an FFC to be set.\n", opcode_id, seg_idx);
            }

            qual_in_key_template_index = qual_in_key_template_idx_arr[seg_idx][packed_qual_segment];

            if (qual_in_key_template_index == DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID)
            {
                continue;
            }

            if (fg_id_arr[seg_idx] == DNX_FIELD_GROUP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d has fg_id_arr not set.\n", opcode_id, seg_idx);
            }

            /*
             * Get the key template for the field group using a given segment
             */
            SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id_arr[seg_idx], &key_template));

            /*
             * Qualifier type is pointed by the qual_in_key_template_idx, extracted from the fg_info
             */
            qual_type = key_template.key_qual_map[qual_in_key_template_index].qual_type;

            /*
             * Qualifier attach_info is pointed by the qual_in_key_template_idx, extracted from the fg_info
             */
            SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get
                            (unit, fg_id_arr[seg_idx], opcode_id, qual_attach_info_arr));

            /*
             * Get the qual_info based on the qual_type+attach_info
             */
            SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                            (unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_CONTEXT_ID_INVALID, qual_type,
                             dummy_offset_on_key, &qual_attach_info_arr[qual_in_key_template_index], &nof_sub_quals,
                             dnx_qual_info));
            if (nof_sub_quals <= 0 || nof_sub_quals > DNX_FIELD_KEY_MAX_SUB_QUAL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_sub_quals %d, must be 1-%d.\n",
                             nof_sub_quals, DNX_FIELD_KEY_MAX_SUB_QUAL);
            }

            /*
             * Configure the HW for the given qualifier (allocate the required FFCs,
             * construct the instruction for them...).
             */
            aligned_location_on_key =
                dnx_data_field.external_tcam.max_single_key_size_get(unit) - key_size[qual_key_index[seg_idx]] +
                qual_lsb_unaligned[seg_idx] - qual_lsb_aligning_size[seg_idx] + qual_size_already_configure;
            dnx_qual_info[0].offset_on_key = aligned_location_on_key;
            for (sub_qual_ndx = 0; sub_qual_ndx < nof_sub_quals; sub_qual_ndx++)
            {
                if (sub_qual_ndx > 0)
                {
                    dnx_qual_info[sub_qual_ndx].offset_on_key = dnx_qual_info[sub_qual_ndx - 1].offset_on_key +
                        dnx_qual_info[sub_qual_ndx - 1].size;
                }
                SHR_IF_ERR_EXIT(dnx_field_key_ffc_set
                                (unit, DNX_FIELD_STAGE_EXTERNAL, acl_context_id, &(dnx_qual_info[sub_qual_ndx]),
                                 key_ids[qual_key_index[seg_idx]], FALSE, DNX_FIELD_FFC_ID_INVALID));
                size_total += dnx_qual_info[sub_qual_ndx].size;

                LOG_DEBUG_EX(BSL_LOG_MODULE, "field_stage %s, context_id %d fg_id %d, qual index in key template %d\n",
                             dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL), acl_context_id, fg_id_arr[seg_idx],
                             qual_in_key_template_index);
                LOG_DEBUG_EX(BSL_LOG_MODULE, " qual in key %s, seg_idx %d, offset in key %d key_id %d\n",
                             dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                             aligned_location_on_key, key_ids[qual_key_index[seg_idx]]);
                LOG_DEBUG_EX(BSL_LOG_MODULE, "qual attach info: offset 0x%X, size %d, input_type %d, "
                             "qual number in segment %d.\n",
                             dnx_qual_info[sub_qual_ndx].offset, dnx_qual_info[sub_qual_ndx].size,
                             dnx_qual_info[sub_qual_ndx].input_type, packed_qual_segment);
            }

            /** Sanity check*/
            if (size_total != key_template.key_qual_map[qual_in_key_template_index].size)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s mapped to different size (%d and %d).\n",
                             dnx_field_dnx_qual_text(unit, qual_type), size_total,
                             key_template.key_qual_map[qual_in_key_template_index].size);
            }

            /*
             * Update the size of the packed qualifiers already used.
             */
            qual_size_already_configure += size_total;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_key_ffc_info_get(
    int unit,
    dnx_field_group_t fg_id,
    int opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    dnx_field_key_template_t key_template;
    dnx_field_key_attached_qual_info_t dummy_dnx_qual_info;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_index;
    uint32 starting_key_offset;
    uint32 max_acl_fwd_key_size;
    uint32 seg_size_bits;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    int qual_iter;
    uint32 fwd_key_size_value_in_bytes, acl_key_size_value_in_bytes;
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info;
    uint32 entry_handle_id;
    uint32 max_single_kbp_key_size;
    uint32 ending_key_offset;
    dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_kbp_opcode_info_t kbp_opcode_info;
    uint8 nof_fwd_segments;
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&dummy_dnx_qual_info, 0, sizeof(dummy_dnx_qual_info));
    dnx_field_key_qualifier_ffc_info_init(group_ffc_info_p->key.qualifier_ffc_info,
                                          DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);

    max_acl_fwd_key_size = 0;
    starting_key_offset = ending_key_offset = 0;

    fwd_key_size_value_in_bytes = 0;
    acl_key_size_value_in_bytes = 0;

    max_single_kbp_key_size = dnx_data_field.external_tcam.max_single_key_size_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, fg_id, opcode_id, qual_attach_info_arr));
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.get(unit, opcode_id, &kbp_opcode_info));
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_segment_indices(unit, fg_id, opcode_id,
                                                        seg_index_on_master_by_qual, qual_index_on_segment,
                                                        qual_offset_on_segment));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

    /** Iterate over all qualifiers for the given field group. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         key_template.key_qual_map[qual_iter].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        int is_segment_found = FALSE;
        int seg_index = 0;
        uint32 seg_on_master_key;
        uint32 qual_offset_on_segment_iter;
        /** Retrieve the index of the qual on master key. */
        seg_on_master_key = seg_index_on_master_by_qual[qual_iter];
        /** Saityn check*/
        if ((seg_on_master_key == DNX_KBP_INVALID_SEGMENT_ID) || (seg_on_master_key >= nof_segments))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode id %d, field group %d, qualifier index %d, the segment "
                         "index on master key is %d. Number of segments on master key %d.\n",
                         opcode_id, fg_id, qual_iter, seg_on_master_key, nof_segments);
        }
        qual_offset_on_segment_iter = qual_offset_on_segment[qual_iter];

        /*
         * Do not get the FFCs for segments that use the overlay feature.
         */
        if (ms_key_segments[seg_on_master_key].is_overlay_field)
        {
            group_ffc_info_p->key.qualifier_ffc_info[qual_iter].qual_type =
                key_template.key_qual_map[qual_iter].qual_type;
            continue;
        }

        /** Iterate over all KBP KBRs. */
        for (kbp_kbr_index = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0; kbp_kbr_index < nof_kbp_keys; kbp_kbr_index++)
        {
            /** Specifying for which context_profile, the DBAL table info should be taken. */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

            /** Setting the key ID, for which we want to retrieve an information. */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_kbr_index);

            /** Perform the DBAL read. */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            /** Get the FWD key size for the current key. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE,
                             &fwd_key_size_value_in_bytes));

            /** Specifying for which context_profile, the DBAL table info should be taken. */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context_id);

            /** Perform the DBAL read. */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

            /** Get the ACL key size for the current key. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE,
                             &acl_key_size_value_in_bytes));

            /** Sanity check*/
            if ((fwd_key_size_value_in_bytes != 0) && (acl_key_size_value_in_bytes != 0))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode id %d, fwd context %d acl context %d key %d,"
                             "the FWD key size is %d while the the ACL key size is %d. "
                             "Can't both be non zero for the same key.\n",
                             opcode_id, fwd_context, acl_context_id, kbp_kbr_index,
                             fwd_key_size_value_in_bytes, acl_key_size_value_in_bytes);
            }

            /** Sum of both ACL and FWD key sizes to retrieve the total size used from current KBR. */
            max_acl_fwd_key_size = BYTES2BITS(fwd_key_size_value_in_bytes + acl_key_size_value_in_bytes);
            /** Because the segments are MSB aligned , we store the ending offset to be used as ending statement later. */
            ending_key_offset = max_single_kbp_key_size - max_acl_fwd_key_size;
            /** Set the starting offset to be equal to max KBP KBR size (160b). */
            starting_key_offset = max_single_kbp_key_size;

            /** Iterate over all master key segments. */
            for (; seg_index < nof_segments && starting_key_offset > ending_key_offset; seg_index++)
            {
                /*
                 * Do not count segments that use the overlay feature for key size.
                 */
                if (ms_key_segments[seg_index].is_overlay_field)
                {
                    continue;
                }
                seg_size_bits = BYTES2BITS(ms_key_segments[seg_index].nof_bytes);
                /**
                 * Extract the current segment size from the starting key offset in order to calculate the
                 * offset of the current segmnet on the key. For example:
                 *      Master key = { <SRC_MAC (32b)> <DST_MAC (32b)>}
                 *      We are using KBR 0, where the offsets of both segmest will be as follow:
                 *
                 *      DST_MAC_starting_key_offset = starting_key_offset (160) - 32 = 128
                 *      SRC_MAC_starting_key_offset = DST_MAC_starting_key_offset (128) - 32 = 96
                 */
                starting_key_offset -= seg_size_bits;

                /** Compare the current segment name with the returned index of the current field qualifier on Master Key. */
                if (seg_index == seg_on_master_key)
                {
                    is_segment_found = TRUE;
                    break;
                }
            }

            /** If is_segment_found is TRUE, we will return FFC info about current segment. */
            if (is_segment_found)
            {
                uint32 output_ffc_index = 0;
                dbal_enum_value_field_field_key_e fwd_kbr_id;
                uint32 context_for_ffc;

                /** Sanity check*/
                if ((seg_on_master_key < nof_fwd_segments) &&
                    ((fwd_key_size_value_in_bytes == 0) || (acl_key_size_value_in_bytes != 0)))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode id %d, fwd context %d acl context %d key %d,"
                                 "segment %d is FWD, but the FWD key size is %d while the the ACL key size is %d.\n",
                                 opcode_id, fwd_context, acl_context_id, kbp_kbr_index, seg_on_master_key,
                                 fwd_key_size_value_in_bytes, acl_key_size_value_in_bytes);
                }

                dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);

                /** Create dummy qual info. */
                dummy_dnx_qual_info.qual_type = key_template.key_qual_map[qual_iter].qual_type;
                dummy_dnx_qual_info.size = key_template.key_qual_map[qual_iter].size;
                dummy_dnx_qual_info.offset_on_key = starting_key_offset + qual_offset_on_segment_iter;

                SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_to_fwd2_key_id_convert(unit, kbp_kbr_index, &fwd_kbr_id));

                if (seg_on_master_key < nof_fwd_segments)
                {
                    if (kbp_opcode_info.master_key_info.segment_info[seg_on_master_key].fg_id ==
                        DNX_FIELD_GROUP_INVALID)
                    {
                        /** Qualifier is sharing with FWD.*/
                        context_for_ffc = fwd_context;
                    }
                    else
                    {
                        /** Qualifier is filling zero padding.*/
                        context_for_ffc = acl_context_id;
                    }
                }
                else
                {
                    /** Qualifier is not shared with FWD or filling zero padding.*/
                    context_for_ffc = acl_context_id;
                }

                SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal
                                (unit, DNX_FIELD_STAGE_EXTERNAL, context_for_ffc, fwd_kbr_id,
                                 &dummy_dnx_qual_info, &qualifier_ffc_info, &output_ffc_index));

                sal_memcpy(&group_ffc_info_p->key.qualifier_ffc_info[qual_iter],
                           &qualifier_ffc_info, sizeof(group_ffc_info_p->key.qualifier_ffc_info[qual_iter]));

                break;
            }
        }
        if (is_segment_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "segment %d on master key, for qualifier number %d, for field group %d "
                         "on opcode %d not found.\n", seg_on_master_key, qual_iter, fg_id, opcode_id);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function returns the hw is_acl value for a given quad.
 *
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] quad_id     - Quad id we would like to check
 * \param [out] quad_is_acl_p    - TRUE if the quad belongs to ACL context
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_is_quad_acl_hw_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 quad_id,
    uint32 *quad_is_acl_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_TABLE_KLEAP_FWD12_FFC_QUAD_IS_ACL;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IDX, quad_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the quad_is_acl */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IS_ACL, INST_SINGLE, quad_is_acl_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  This function gets the stage for the given ffc group.
 *
 * \param [in] unit        - Device Id
 * \param [in] group_id    - Group Id
 * \param [in] stage_p     - Value to return
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_field_kbp_key_ifwd2_resource_mapping_ffc_group_stage_hw_get(
    int unit,
    uint32 group_id,
    uint32 *stage_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the quad_is_acl */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, group_id, stage_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Determines if an FFC is accessible to ACL in iFWD2. Note that the FFC may be inaccassible to the ACL due to
 *  preallocation to limit the number of FFCs used by ACL, this function will not consider that.
 * \param [in] unit        - Device Id
 * \param [in] fwd_context - The FWD context
 * \param [in] ffc_id      - FFC ID
 * \param [in] accessible_to_acl_p - Whether the FFC is controlled by the ACL context.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_kbp_key_ifwd2_acl_ffc_update_for_new_context
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_acl_ffc_is_acl(
    int unit,
    int fwd_context,
    int ffc_id,
    int *accessible_to_acl_p)
{
    uint32 quad_id;
    uint32 group_id;
    uint32 fwd_stage;
    uint32 quad_is_acl;
    uint8 is_dynamic;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Map the ffc_id to it's quad
     */
    quad_id = dnx_data_field.external_tcam.ffc_to_quad_and_group_map_get(unit, ffc_id)->quad_id;
    /*
     * Check if the quad is_acl - belongs to acl context
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_is_quad_acl_hw_get(unit, fwd_context, quad_id, &quad_is_acl));
    /*
     * Map the ffc_id to it's group
     */
    group_id = dnx_data_field.external_tcam.ffc_to_quad_and_group_map_get(unit, ffc_id)->group_id;
    /*
     * Check if the group is in the FWD2
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_resource_mapping_ffc_group_stage_hw_get(unit, group_id, &fwd_stage));
    if (fwd_stage != DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A &&
        fwd_stage != DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal value for fwd_stage (%d) for context %d ffc_group_id %d .\n",
                     fwd_stage, fwd_context, group_id);
    }
    /*
     * Sanity check.
     * If the FFC is used by fwd2 and not used by ACL, then we have FWD in that context.
     * verify that the context is not within the dynamic range
     */
    SHR_IF_ERR_EXIT(kbp_mngr_context_acl_is_dynamic(unit, fwd_context, &is_dynamic));
    if (fwd_stage == DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B && quad_is_acl == FALSE && is_dynamic)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d has FFC ID %d in quad_id %d ffc_group_id %d, "
                     "used by FWD2 but not by ACL, but the context is within the dynamic context range.\n",
                     fwd_context, ffc_id, quad_id, group_id);
    }

    /*
     * Some of the ffcs are used in FWD1 (stageA) and cannot be used in the acl (which is in FWD2).
     * We will mark those ffcs as unavailable for the ACL.
     * Also, some of the FFCs are used by FWD in iFWD2 and not ACL (controlled by fwd_context and not acl_context)
     */
    if ((!quad_is_acl) || (fwd_stage == DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_A /* FWD1 */ ))
    {
        (*accessible_to_acl_p) = FALSE;
    }
    else
    {
        (*accessible_to_acl_p) = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function manages the available for ACL context ffc by
 *  masking out the ffc's that are used by FWD context.
 *  The function goes over all ifwd2 contexts and checks which ffc's were marked as used by
 *  FWD context. Those ffc's are set as 'used' in the ffc resource manager, thus will never
 *  be allocated for the ACL context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_acl_ffc_init(
    int unit)
{
    int ffc_id;
    int ffc_is_acl;
    dnx_field_context_t context_id;
    uint32 nof_ifwd2_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    uint32 nof_ifwd2_ffc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;
    uint32 ctx_used_by_opcode[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    uint32 opcode_id;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    int fwd_ctx_index;
    uint32 nof_ffcs_used_by_acl;
    uint32 nof_ffcs_allocated_to_acl = 0;
    int current_ctx_used_by_opcode;
    uint8 is_dynamic;
    uint8 is_standard_image;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    /*
     * Initialize arrays.
     */
    for (context_id = 0; context_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; context_id++)
    {
        ctx_used_by_opcode[context_id] = DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID;
    }
    /*
     * Find all contexts used by FWD.
     */
    DNX_FIELD_KBP_OPCODE_ITERATOR(opcode_id)
    {
        DNX_FIELD_KBP_OPCODE_NOT_IN_USE_CONTINUE(unit, opcode_id);
        SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts
                        (unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
        if (nof_fwd_contexts == 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "opcode %d has 0 fwd contexts.\n", opcode_id);
        }
        if (nof_fwd_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "opcode %d has %d fwd contexts, maximum number of fwd contexts is %d.\n",
                         opcode_id, nof_fwd_contexts, DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM);
        }
        if (nof_fwd_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "opcode %d has %d fwd contexts, maximum number of "
                         "fwd contexts per opcode is %d.\n",
                         opcode_id, nof_fwd_contexts,
                         DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
        }
        for (fwd_ctx_index = 0; fwd_ctx_index < nof_fwd_contexts; fwd_ctx_index++)
        {
            if (fwd_contexts[fwd_ctx_index] >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS ||
                fwd_contexts[fwd_ctx_index] >= nof_ifwd2_contexts)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d is out of bounds.\n", fwd_contexts[fwd_ctx_index]);
            }
            if (ctx_used_by_opcode[fwd_contexts[fwd_ctx_index]] != DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d is used by both opcode_id %d and %d.\n",
                             fwd_contexts[fwd_ctx_index], ctx_used_by_opcode[fwd_contexts[fwd_ctx_index]], opcode_id);
            }

            ctx_used_by_opcode[fwd_contexts[fwd_ctx_index]] = opcode_id;
        }
    }

    /*
     * Technically not all of the contexts are FWD context, some are ACL contexts.
     * In fact Pemla marks all contexts, so we can rely on it and run the
     * following loop for all dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts contexts.
     */
    for (context_id = 0; context_id < nof_ifwd2_contexts; context_id++)
    {
        nof_ffcs_used_by_acl = 0;
        if (ctx_used_by_opcode[context_id] == DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID)
        {
            nof_ffcs_allocated_to_acl = 0;
            current_ctx_used_by_opcode = FALSE;
        }
        else
        {
            if (is_standard_image)
            {
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_nof_acl_ffcs
                                (unit, ctx_used_by_opcode[context_id], &nof_ffcs_allocated_to_acl));
            }
            current_ctx_used_by_opcode = TRUE;
        }
        SHR_IF_ERR_EXIT(kbp_mngr_context_acl_is_dynamic(unit, context_id, &is_dynamic));
        if (is_dynamic && current_ctx_used_by_opcode)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d of predefined opcode %d is in the dynamic range.\n",
                         context_id, ctx_used_by_opcode[context_id]);
        }
        for (ffc_id = 0; ffc_id < nof_ifwd2_ffc; ffc_id++)
        {
            /*
             * Check if the FFC is controlled by the ACL context.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_acl_ffc_is_acl(unit, context_id, ffc_id, &ffc_is_acl));
            /*
             * In case this ffc DOES NOT belongs to acl context - mark this ffc as used (by FWD context) in
             * the resource manager.
             * Also, if the number of FFCs exceed the number used by the opcode
             */
            if ((ffc_is_acl == FALSE) ||
                (current_ctx_used_by_opcode &&
                 (is_standard_image && (nof_ffcs_used_by_acl >= nof_ffcs_allocated_to_acl))))
            {
                /*
                 * This ffc is used by Pemla (FWD context) or by FWD1 stage  - should not be presented as available in the ACL context
                 */
                SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate
                                (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, context_id, &ffc_id));
                if (ffc_is_acl)
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "context_id %d ffc_id %d set to busy (number of FFCs per opcode reached)%s%s\n",
                                 context_id, ffc_id, EMPTY, EMPTY);
                }
                else
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "context_id %d ffc_id %d set to busy (used by FWD)%s%s\n",
                                 context_id, ffc_id, EMPTY, EMPTY);
                }
            }
            else
            {
                nof_ffcs_used_by_acl++;
            }
        }
        if (is_standard_image)
        {
            if (current_ctx_used_by_opcode && (nof_ffcs_used_by_acl != nof_ffcs_allocated_to_acl))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d of opcode %d has only %d FFC available for ACL, "
                             "required %d.\n",
                             context_id, ctx_used_by_opcode[context_id], nof_ffcs_used_by_acl,
                             nof_ffcs_allocated_to_acl);
            }
            /*
             * Verify that dynamic contexts have enough FFCs for any OPCODE ID.
             */
            if (is_dynamic && (nof_ffcs_used_by_acl < 16))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fwd_context %d has only %d FFC available for ACL, "
                             "but the dynamic range requires %d.\n", context_id, nof_ffcs_used_by_acl, 16);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function manages the available FFCs for a newly created ACL context by
 *  masking out the ffc's that are used by original FWD context.
 *
 * \param [in] unit           - Device Id
 * \param [in] base_opcode_id - The predefined opcode on which the new context is based.
 * \param [in] fwd_context_id - The FWD context that comes before the ACL context.
 * \param [in] acl_context_id - The newly created ACL cntext
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_kbp_key_ifwd2_acl_ffc_init
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    uint32 base_opcode_id,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id)
{
    int ffc_id;
    int ffc_is_acl;
    uint32 nof_ifwd2_ffc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;
    uint32 nof_ffcs_used_by_acl = 0;
    uint32 nof_ffcs_allocated_to_acl = 0;
    uint8 ffc_is_alloc;
    uint8 is_standard_image;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));

    if (is_standard_image)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_nof_acl_ffcs(unit, base_opcode_id, &nof_ffcs_allocated_to_acl));
    }

    for (ffc_id = 0; ffc_id < nof_ifwd2_ffc; ffc_id++)
    {
        /*
         * Check if the FFC ID was already allocated at init for the ACL context.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_is_allocated(unit, acl_context_id, ffc_id, &ffc_is_alloc));
        if (ffc_is_alloc == FALSE)
        {
            /*
             * Check if the FFC is controlled by the ACL context, from the perspective of the fwd_context.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_acl_ffc_is_acl(unit, fwd_context_id, ffc_id, &ffc_is_acl));
            /*
             * If the FFC does not belong to the ACL context from the FWD context's perspective, or if the number
             * of FFCs exceed the number used by the opcode, mark the FFC as used.
             */
            if ((ffc_is_acl == FALSE) || (is_standard_image && (nof_ffcs_used_by_acl >= nof_ffcs_allocated_to_acl)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate
                                (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, acl_context_id, &ffc_id));
                if (ffc_is_acl)
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "context_id %d ffc_id %d set to busy (number of FFCs per opcode reached)%s%s\n",
                                 acl_context_id, ffc_id, EMPTY, EMPTY);
                }
                else
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "context_id %d ffc_id %d set to busy (used by FWD)%s%s\n",
                                 acl_context_id, ffc_id, EMPTY, EMPTY);
                }
            }
            else
            {
                nof_ffcs_used_by_acl++;
            }
        }
    }

    if (is_standard_image && (nof_ffcs_used_by_acl != nof_ffcs_allocated_to_acl))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dynamic acl_context %d, based of opcode %d, has only %d FFC available for ACL, "
                     "required %d. Issue should have been detected at init.\n",
                     acl_context_id, base_opcode_id, nof_ffcs_used_by_acl, nof_ffcs_allocated_to_acl);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function finds what keys are being used by FWD by checking both the KBR mapping of FFCs
 *  and the aligner mapping.
 *  It verifies that there is no mismatsch between the different tables.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_used_keys_on_init_verify(
    int unit)
{
    uint32 fwd_context;
    uint32 unused_bmp_by_is_acl;
    uint32 unused_bmp_by_kbr;
    uint32 unused_bmp_by_aligner;

    SHR_FUNC_INIT_VARS(unit);

    /** Sanity check.*/
    if (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts != DNX_KBP_NOF_FWD_CONTEXTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD context %d and ACL FWD context %d mismatch.\n",
                     DNX_KBP_NOF_FWD_CONTEXTS,
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts);
    }

    for (fwd_context = 0; fwd_context < DNX_KBP_NOF_FWD_CONTEXTS; fwd_context++)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_is_acl_get
                        (unit, fwd_context, &unused_bmp_by_is_acl));
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_kbr_get(unit, fwd_context, &unused_bmp_by_kbr));
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_fwd_context_unused_keys_aligner_get
                        (unit, fwd_context, &unused_bmp_by_aligner));
        if ((unused_bmp_by_is_acl & (~unused_bmp_by_kbr)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d mismatch between KBR context allocation (keys %x) "
                         "and KBR values (keys %x).\n", fwd_context, unused_bmp_by_is_acl, unused_bmp_by_kbr);
        }
        if ((unused_bmp_by_is_acl & (~unused_bmp_by_aligner)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d mismatch between KBR context allocation (keys %x) "
                         "and aligner (keys %x).\n", fwd_context, unused_bmp_by_is_acl, unused_bmp_by_kbr);
        }
        if (utilex_nof_on_bits_in_long(unused_bmp_by_is_acl) < DNX_FIELD_KBP_ACL_KEYS_MIN)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d There are %d keys available for ACL, at least %d are "
                         "needed.\n",
                         fwd_context, utilex_nof_on_bits_in_long(unused_bmp_by_is_acl), DNX_FIELD_KBP_ACL_KEYS_MIN);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies the minimum of available for ACL hw
 *  resources such as ffc, per each context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_ffc_verify(
    int unit)
{
    dnx_field_context_t context_id;
    int nof_ffc;
    uint32 min_nof_acl_ffc = dnx_data_field.external_tcam.min_acl_nof_ffc_get(unit);
    uint32 nof_ifwd2_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;

    /** Variables are defined as arrays to fix coverity issues */
    uint32 mpls_context_encoded_val[1];
    uint8 is_image_standard_1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_image_standard_1));

    if (is_image_standard_1)
    {
        /** Encode DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD. */
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD,
                         mpls_context_encoded_val));
    }

    for (context_id = 0; context_id < nof_ifwd2_contexts; context_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_num_of_elements_get(unit, context_id, &nof_ffc));

        if (nof_ffc < min_nof_acl_ffc)
        {
            LOG_ERROR_EX(BSL_LOG_MODULE,
                         "context_id %d has only %d ffc for ACL purposes. Expected number is at least %d %s\n",
                         context_id, nof_ffc, min_nof_acl_ffc, EMPTY);
            return _SHR_E_RESOURCE;
        }

        if (is_image_standard_1)
        {
            /** MPLS context requires double amount of FFCs for ACLs */
            if ((context_id == *mpls_context_encoded_val) && (nof_ffc < (2 * min_nof_acl_ffc)))
            {
                LOG_ERROR_EX(BSL_LOG_MODULE,
                             "MPLS context has only %d FFCs for ACL purposes. Expected number is at least %d %s %s\n",
                             nof_ffc, min_nof_acl_ffc * 2, EMPTY, EMPTY);
                return _SHR_E_RESOURCE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the stage (iFWD1 or iFWD2) for the given FWD key.
 *
 * \param [in] unit        - Device Id
 * \param [in] key_id      - Key Id
 * \param [in] stage_p     - Value to get, which stage (iFWD1 or iFWD2) uses the key.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_resource_mapping_key_stage_hw_get(
    int unit,
    uint32 key_id,
    dbal_enum_value_field_kleap_stage_selector_e * stage_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the stage for the given key */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, key_id, stage_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that the aligner mapping for KBP ACL lookups is all zeros.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_kbp_acl_aligner_mapping_on_init_verify(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key = 0;
    dnx_field_context_t acl_context;
    uint32 nof_acl_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    uint32 aligner_key_size;
    int nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

    for (acl_context = 0; acl_context < nof_acl_contexts; acl_context++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context);
        for (kbp_key = 0; kbp_key < nof_kbp_keys; kbp_key++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE, &aligner_key_size));

            if (aligner_key_size != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ACL iFWD2 context %d has size %d in KBP ACL key aligner mapping of key %d.\n",
                             acl_context, aligner_key_size, kbp_key);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that all keys used by KBP are assigned to iFWD2.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_kbp_stage_keys_verify(
    int unit)
{
    dbal_enum_value_field_kleap_stage_selector_e key_stage;
    uint32 kbp_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);
    int key_id;
    SHR_FUNC_INIT_VARS(unit);

    for (key_id = 0; kbp_key_bmp != 0; (key_id++), (kbp_key_bmp >>= 1))
    {
        if ((kbp_key_bmp & 1) == 0)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_resource_mapping_key_stage_hw_get(unit, key_id, &key_stage));

        if (key_stage != DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FWD key %d on is not assigned to FWD stage B.\n", key_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that the qualifier for the result size fits the payload result size.
 * \param [in] unit        - Device Id
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_payload_qualifier_verify(
    int unit)
{
    dnx_field_qual_t dnx_qual_external_result_part_0;
    dnx_field_qual_t dnx_qual_external_result_part_1;
    uint32 qual_size_part_0;
    uint32 qual_size_part_1;
    int external_total_payload_size;

    SHR_FUNC_INIT_VARS(unit);

    dnx_qual_external_result_part_0 =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0);
    dnx_qual_external_result_part_1 =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, DNX_FIELD_STAGE_IPMF1, dnx_qual_external_result_part_0,
                                                NULL, NULL, &qual_size_part_0));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, DNX_FIELD_STAGE_IPMF1, dnx_qual_external_result_part_1,
                                                NULL, NULL, &qual_size_part_1));
    external_total_payload_size = qual_size_part_0 + qual_size_part_1;

    if (external_total_payload_size != dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "size of external TCAM payload buffer in DNX DATA is %d, on PBUS %d.\r\n",
                     dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit), external_total_payload_size);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that ACL Key resources were not used by other applications (such as Pemla).
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_key_ifwd2_acl_key_verify(
    int unit)
{
    int nof_kbp_keys = dnx_data_field.external_tcam.nof_keys_total_get(unit);
    uint32 kbp_key_bmp = dnx_data_field.external_tcam.key_bmp_get(unit);
    uint32 kbp_key_bmp_changing;
    int kbr_general_id;
    int kbr_kbp_id;
    int nof_ones_bmp;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit) > nof_kbp_keys)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Maximum number of ACL keys in KBP %d is bigger than the maximum number "
                     "of KBP keys in DBAL %d.\n",
                     dnx_data_field.external_tcam.nof_acl_keys_master_max_get(unit), nof_kbp_keys);
    }

    if (((uint32) DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) != ((uint8) DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Maximum number of qualifiers per field group %d does not fit into uint8. "
                     "dnx_field_kbp_segment_info_t qual_idx in SW state is uint8.\n",
                     DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    }

    nof_ones_bmp = utilex_bitstream_get_nof_on_bits(&kbp_key_bmp, 1);
    if (nof_ones_bmp != nof_kbp_keys)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The KBP key bitmap 0x%x has %d ones, expected %d.\n",
                     kbp_key_bmp, nof_ones_bmp, nof_kbp_keys);
    }

    kbp_key_bmp_changing = kbp_key_bmp;
    kbr_general_id = 0;
    kbr_kbp_id = 0;
    while (kbp_key_bmp_changing)
    {
        if ((1 & kbp_key_bmp_changing) != 0)
        {
            int expected_kbr_general_id;
            switch (kbr_kbp_id)
            {
                case 0:
                {
                    expected_kbr_general_id = DBAL_DEFINE_FWD12_KBR_ID_KBP_0;
                    break;
                }
                case 1:
                {
                    expected_kbr_general_id = DBAL_DEFINE_FWD12_KBR_ID_KBP_1;
                    break;
                }
                case 2:
                {
                    expected_kbr_general_id = DBAL_DEFINE_FWD12_KBR_ID_KBP_2;
                    break;
                }
                case 3:
                {
                    expected_kbr_general_id = DBAL_DEFINE_FWD12_KBR_ID_KBP_3;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Unknown kbr_kbp_id %d.\n", kbr_kbp_id);
                    break;
                }
            }
            if (expected_kbr_general_id != kbr_general_id)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "expected KBR %d instead of %d for KBP key %d.\n",
                             expected_kbr_general_id, kbr_general_id, kbr_kbp_id);
            }
            kbr_kbp_id++;
        }
        kbp_key_bmp_changing >>= 1;
        kbr_general_id++;
    }

    {
        uint32 db_width;
        /*
         * Verify maximum KBP DB key size.
         */
        SHR_IF_ERR_EXIT(dnx_kbp_valid_key_width(unit, DNX_KBP_MAX_KEY_LENGTH_IN_BITS, &db_width));
        if (DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS != db_width)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS (%d) Must be the same as the DB size (%d) "
                         "of DNX_KBP_MAX_KEY_LENGTH_IN_BITS (%d).\n",
                         DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS, db_width, DNX_KBP_MAX_KEY_LENGTH_IN_BITS);
        }
    }

    if (dnx_kbp_device_enabled(unit))
    {
        /*
         * Verifies minimum number of FFCs available.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_ffc_verify(unit));
        /*
         * Verify that the keys being used by FWD for KBP match between different HW tables.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_used_keys_on_init_verify(unit));
        /*
         * Verify that the KBP ACL aligner is empty.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_kbp_acl_aligner_mapping_on_init_verify(unit));
        /*
         * Verify that all of the keys used by KBP are mapped to FWD stage B (iFWD2).
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_kbp_stage_keys_verify(unit));
        /*
         * Verify that the iPMF1 qualifier of the result payload matched in size to the result payload.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_payload_qualifier_verify(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get list of opcodes in a group.
* \param [in] unit                    - Device ID
* \param [in] opcode_id               - The KBP opcode ID to find the group for.
* \param [out] nof_opcodes_in_group_p - Number of opcodes in the group (and in opcodes_in_group)
* \param [out] opcodes_in_group        - The opcodes in the group.
* \param [out] opcode_predefined_p     - The predefined opcode of the group.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_opcode_group(
    int unit,
    uint32 opcode_id,
    int *nof_opcodes_in_group_p,
    uint32 opcodes_in_group[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF + 1],
    uint32 *opcode_predefined_p)
{
    uint8 is_user_defined;
    bcm_field_AppType_t user_app_types[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF];
    bcm_field_AppType_t apptype_pre;
    int apptype_index;
    int nof_user_defiend_apptypes;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &is_user_defined));
    if (is_user_defined)
    {
        bcm_field_AppType_t apptype_user;
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype_user));
        SHR_IF_ERR_EXIT(dnx_field_context_user_apptype_to_predef_apptype(unit, apptype_user, &apptype_pre));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype_pre, opcode_predefined_p));
    }
    else
    {
        (*opcode_predefined_p) = opcode_id;
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, (*opcode_predefined_p), &apptype_pre));
    }
    SHR_IF_ERR_EXIT(dnx_field_context_apptype_to_child_apptypes
                    (unit, apptype_pre, &nof_user_defiend_apptypes, user_app_types));
    for (apptype_index = 0; apptype_index < nof_user_defiend_apptypes; apptype_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                        (unit, user_app_types[apptype_index], &(opcodes_in_group[apptype_index])));
    }
    opcodes_in_group[apptype_index] = (*opcode_predefined_p);
    (*nof_opcodes_in_group_p) = nof_user_defiend_apptypes + 1;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the struct dnx_field_kbp_segment_info_t, used in the SW state of hte master key for the KBP
*  (external TCAM field groups in external stage).
* \param [in] unit            - Device Id
* \param [in] fg_segment_info_p  - Pointer to struct to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_segment_info_t_init(
    int unit,
    dnx_field_kbp_segment_info_t * fg_segment_info_p)
{
    int packed_qual_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_segment_info_p, _SHR_E_PARAM, "fg_segment_info_p");

    sal_memset(fg_segment_info_p, 0x0, sizeof(*fg_segment_info_p));

    fg_segment_info_p->fg_id = DNX_FIELD_GROUP_INVALID;

    for (packed_qual_idx = 0; packed_qual_idx < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_idx++)
    {
        fg_segment_info_p->qual_idx[packed_qual_idx] = DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Updates the SW state when splitting a zero padding segment.
*  The old segment remains first, but the field sw state information is given to the newer split segment.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP opcode ID
* \param [in] seg_id          - The segment to split.
* \param [in] opcode_predefined - If opcode_id is predefined, the opcode_id,
*                                 if user defined the predefined opcode it is based upon.
* \param [in] nof_opcodes_in_group - Number of elements in opcodes_in_group
* \param [in] opcodes_in_group - predefined opcode plus all user defined opcodes based upon it.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_acl_split_zero_pad_update_sw_state(
    int unit,
    uint32 opcode_id,
    int seg_id,
    uint32 opcode_predefined,
    int nof_opcodes_in_group,
    uint32 opcodes_in_group[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF + 1])
{
    uint8 is_valid;
    uint8 nof_fwd_segments;
    dnx_field_group_t fg_id;
    int seg_index;
    dnx_field_kbp_segment_info_t segment_info;
    int opcode_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sanity checks.
     */
    if (seg_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "seg_id %d is negative.\n", seg_id);
    }
    if (seg_id >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "seg_id %d is above maximum %d.\n",
                     seg_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 2);
    }
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.is_valid.get(unit, opcode_id, &is_valid));
    if (is_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d is not marked as used by any field group.\n", opcode_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                    nof_fwd_segments.get(unit, opcode_predefined, &nof_fwd_segments));
    if (seg_id >= nof_fwd_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d splitting zero padding segment %d, but only %d FWD segments.\n",
                     opcode_predefined, seg_id, nof_fwd_segments);
    }
    if (nof_fwd_segments >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d nof_fwd_segments %d is filling all of the segments on "
                     "master key %d.\n", opcode_predefined, nof_fwd_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
    }
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.segment_info.
                    fg_id.get(unit, opcode_predefined, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1, &fg_id));
    if (fg_id != DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d splitting zero padding segment %d, "
                     "but last segment is being used.\n", opcode_predefined, nof_fwd_segments);
    }
    {
        int found_opcode = FALSE;
        int found_predef_opcode = FALSE;
        for (opcode_index = 0; opcode_index < nof_opcodes_in_group; opcode_index++)
        {
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.segment_info.
                            fg_id.get(unit, opcodes_in_group[opcode_index], DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1,
                                      &fg_id));
            if (fg_id != DNX_FIELD_GROUP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d child of %d (from %d) splitting zero padding segment %d, "
                             "but last segment is being used.\n",
                             opcodes_in_group[opcode_index], opcode_predefined, opcode_id, nof_fwd_segments);
            }
            if (opcodes_in_group[opcode_index] == opcode_id)
            {
                found_opcode = TRUE;
            }
            if (opcodes_in_group[opcode_index] == opcode_predefined)
            {
                found_predef_opcode = TRUE;
            }
        }
        if (found_opcode == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d is not in opcodes_in_group.\n", opcode_id);
        }
        if (found_predef_opcode == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_predefined %d is not in opcodes_in_group.\n", opcode_predefined);
        }
    }

    /*
     * Move the segments, for the predefined apptype and all user defined apptypes based on it.
     */
    for (opcode_index = 0; opcode_index < nof_opcodes_in_group; opcode_index++)
    {
        for (seg_index = DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1; seg_index > seg_id; seg_index--)
        {
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.get(unit, opcodes_in_group[opcode_index], seg_index - 1, &segment_info));
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.set(unit, opcodes_in_group[opcode_index], seg_index, &segment_info));
        }
    }
    /*
     * Delete the info for the new zero padding segment.
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_segment_info_t_init(unit, &segment_info));
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                    segment_info.set(unit, opcode_predefined, seg_id, &segment_info));

    /*
     * Increase the number of FWD segments.
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                    nof_fwd_segments.set(unit, opcode_predefined, nof_fwd_segments + 1));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given FWD segment is an unused zero_padding segment.
* \param [in] unit               - Device ID
* \param [in] ms_key_segment_p   - The FWD segment
* \param [out] is_zero_padding_p - Whether the segment is zero padding.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_acl_segment_is_zero_padding(
    int unit,
    kbp_mngr_key_segment_t * ms_key_segment_p,
    int *is_zero_padding_p)
{
    char zero_padding_string[] = "ZERO_PADDING";
    int zero_padding_string_size = sal_strnlen(zero_padding_string, sizeof(zero_padding_string));

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ms_key_segment_p, _SHR_E_PARAM, "ms_key_segment_p");

    /** Sanity check. */
    if (zero_padding_string_size >= sizeof(ms_key_segment_p->name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Zero padding string (\"%s\" size %d) is too big for segment name (%d).\r\n",
                     zero_padding_string, zero_padding_string_size, (int) sizeof(ms_key_segment_p->name));
    }
    if (zero_padding_string_size <= 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Zero padding string size %d too small.\r\n", zero_padding_string_size);
    }

    if (sal_memcmp(ms_key_segment_p->name, zero_padding_string, zero_padding_string_size))
    {
        (*is_zero_padding_p) = FALSE;
    }
    else
    {
        (*is_zero_padding_p) = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key in the ACL part of the master key.
*  The verification will be done over all acl segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP opcode ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] force_same_qual - Only share the same qualifier type.
* \param [in] nof_packed_quals - The number of qualifiers in the segment.
* \param [in] qual_type       - qualifier type added by user.
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [in] qual_info       - attach info as was passed by user
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
*                                    Holds segment_idx if found match
*                                    Holds DNX_KBP_INVALID_SEGMENT_ID if not found
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_acl_segment_exist(
    int unit,
    uint32 opcode_id,
    dnx_field_group_t fg_id,
    int force_same_qual,
    int nof_packed_quals,
    dnx_field_qual_t qual_type[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_info[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    dnx_field_kbp_opcode_info_t kbp_kbp_info_info;
    dnx_field_key_template_t key_template;
    dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 seg_idx;
    uint32 qual_in_key_template_idx;
    int packed_qual_ndx;
    int nof_packed_quals_in_segment;
    dnx_field_qual_t contender_qual_type[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
    dnx_field_qual_attach_info_t contender_attach_info[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
    uint8 is_contender_qual_ranged;
    uint8 is_qual_ranged;
    dbal_fields_e contender_qual_field_id;
    dbal_fields_e qual_field_id;
    dbal_tables_e contender_dbal_table_id;
    dbal_tables_e dbal_table_id;
    dnx_field_group_t contender_fg_id;
    int fully_shareable;
    int partially_shareable;
    int partial_sharing_offset_lsb;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;

    SHR_FUNC_INIT_VARS(unit);

    /** Sanity check. */
    if (nof_packed_quals <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_packed_quals is %d, must be at least 1.\n", nof_packed_quals);
    }

    *found_segment_idx_p = DNX_KBP_INVALID_SEGMENT_ID;
    *msb_offset_bytes_overlay_sharing_p = 0;

    /*
     * Get the kbp information for this opcode. kbp info includes the qualifier type
     and the qualifier attach info hints for all segments in the master key.
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.get(unit, opcode_id, &kbp_kbp_info_info));

    /*
     * Get the master key information from KBP, to find which segemnts are overlay segments.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));

    /*
     * Ranged and non-ranged qualifiers cannot be shared at the moment, except with overlay.
     */
    if (is_overlay == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, DNX_FIELD_STAGE_EXTERNAL, qual_type[0], &qual_field_id));
        SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_field_id, &is_qual_ranged));

        if (is_qual_ranged && nof_packed_quals > 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d we have a packed ranged qualifer.\r\n", opcode_id);
        }
    }

    for (seg_idx = 0; seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; seg_idx++)
    {
        /*
         * The same segment cannot be shared twice, except with overlay.
         */
        if ((is_overlay == FALSE) && (already_shared_segments[seg_idx]))
        {
            continue;
        }
        /*
         * Check that the segment isn't an overlay segment already.
         */
        if (ms_key_segments[seg_idx].is_overlay_field)
        {
            continue;
        }
        /*
         * Get the fg_info for a given segment
         */
        contender_fg_id = kbp_kbp_info_info.master_key_info.segment_info[seg_idx].fg_id;
        if (contender_fg_id == DNX_FIELD_GROUP_INVALID)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, contender_fg_id, &key_template));

        SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, contender_fg_id, opcode_id,
                                                                          qual_attach_info_arr));

        /*
         * Extract the info for each packed qualifer (or for one qualifer if there.
         */
        for (packed_qual_ndx = 0; packed_qual_ndx < DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT; packed_qual_ndx++)
        {
            /*
             * Extract the qualifier index from the given segment info. This is the index of the qualifier_type AND
             * of the attach_info for this qualifier inside the fg_info.
             */
            qual_in_key_template_idx =
                kbp_kbp_info_info.master_key_info.segment_info[seg_idx].qual_idx[packed_qual_ndx];

            if (qual_in_key_template_idx == DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID)
            {
                break;
            }

            contender_qual_type[packed_qual_ndx] = key_template.key_qual_map[qual_in_key_template_idx].qual_type;

            /*
             * Qualifier attach_info is pointed by the qual_in_key_template_idx, extracted from the fg_info
             */
            sal_memcpy(&contender_attach_info[packed_qual_ndx], &qual_attach_info_arr[qual_in_key_template_idx],
                       sizeof(contender_attach_info[0]));

        }
        nof_packed_quals_in_segment = packed_qual_ndx;
        if (nof_packed_quals_in_segment <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d  segment %d has field group %d, "
                         "but no qualifers associated with it.\r\n", opcode_id, seg_idx, fg_id);
        }

        /*
         * If we force the qualifers to be the same, verify that they are the same.
         */
        if (force_same_qual)
        {
            int all_quals_the_same = TRUE;
            if (nof_packed_quals != nof_packed_quals_in_segment)
            {
                continue;
            }
            for (packed_qual_ndx = 0; packed_qual_ndx < nof_packed_quals_in_segment; packed_qual_ndx++)
            {
                if (qual_type[packed_qual_ndx] != contender_qual_type[packed_qual_ndx])
                {
                    all_quals_the_same = FALSE;
                    break;
                }
            }
            if (all_quals_the_same == FALSE)
            {
                continue;
            }
        }

        /*
         * Ranged and non-ranged qualifiers cannot be shared at the moment, unless using overlay sharing.
         * ranged qualifiers are not packed.
         */
        if (is_overlay == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, contender_fg_id, &contender_dbal_table_id));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                            (unit, DNX_FIELD_STAGE_EXTERNAL, contender_qual_type[0], &contender_qual_field_id));
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get
                            (unit, contender_dbal_table_id, contender_qual_field_id, &is_contender_qual_ranged));
            if (is_contender_qual_ranged && nof_packed_quals_in_segment > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d we have a packed ranged qualifer.\r\n", opcode_id);
            }
            if ((is_contender_qual_ranged && (is_qual_ranged == FALSE)) ||
                ((is_contender_qual_ranged == FALSE) && is_qual_ranged))
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_compare_qualifiers_for_sharing
                        (unit, nof_packed_quals_in_segment, contender_qual_type, contender_attach_info,
                         nof_packed_quals, qual_type, qual_info,
                         &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
        if (fully_shareable)
        {
            /*
             * Full sharing.
             */
            *found_segment_idx_p = seg_idx;
            if (is_overlay)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found ACL match with fg_id %d opcod_id %d qual %s seg_idx %d, using overlay sharing.\n",
                             contender_fg_id, opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx);
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found ACL match with fg_id %d opcod_id %d qual %s seg_idx %d\n",
                             contender_fg_id, opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx);
            }
            if (nof_packed_quals > 1)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "The ACL match with fg_id %d opcod_id %d seg_idx %d is a packed segment "
                             "with %d qualifiers.\n", contender_fg_id, opcode_id, seg_idx, nof_packed_quals);
            }

            break;
        }
        else if (partially_shareable && is_overlay &&
                 (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb) &&
                 (nof_packed_quals == 1))
        {
            uint32 containing_qual_size;
            uint32 contained_qual_size;
            /*
             * Try partial sharing, if we use overlay sharing and the offset from LSB is in full bytes.
             * Do not do partial sharing for packed qualifiers at this point.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type[0], NULL, NULL, &contained_qual_size));
            *msb_offset_bytes_overlay_sharing_p =
                ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                BITS2BYTES(partial_sharing_offset_lsb);
            *found_segment_idx_p = seg_idx;
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Found ACL match opcod_id %d qual %s seg_idx %d, "
                         "using overlay sharing. Partial sharing with offset from MSB %d\n",
                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx,
                         *msb_offset_bytes_overlay_sharing_p);
            /** Sanity check. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, DNX_FIELD_STAGE_EXTERNAL, contender_qual_type[0],
                             NULL, NULL, &containing_qual_size));
            if (ms_key_segments[seg_idx].nof_bytes != BITS2BYTES(containing_qual_size))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment \"%.*s\" is %d bytes, while qualifier %s is %d bytes.\r\n",
                             DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                             ms_key_segments[seg_idx].nof_bytes, dnx_field_dnx_qual_text(unit, contender_qual_type[0]),
                             BITS2BYTES(containing_qual_size));
            }
            /** Sanity check. */
            if ((*msb_offset_bytes_overlay_sharing_p) < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                             *msb_offset_bytes_overlay_sharing_p);
            }

            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in zero padding in any of the opcodes
*  in the group based on the predefine qualifier.
* \param [in] unit            - Device ID
* \param [in] opcode_id_sharing - The KBP opcode ID to do the sharing
* \param [in] force_same_qual - Only share the same qualifier type.
* \param [in] nof_packed_quals - The number of qualifiers in the segment.
* \param [in] qual_type       - qualifier type added by user.
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [in] qual_info       - attach info as was passed by user
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
*                                    Holds segment_idx if found match
*                                    Holds DNX_KBP_INVALID_SEGMENT_ID if not found
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_acl_segment_exist_zero_padding(
    int unit,
    uint32 opcode_id_sharing,
    int force_same_qual,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    dnx_field_kbp_opcode_info_t kbp_kbp_info_info;
    dnx_field_key_template_t key_template;
    dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 seg_idx;
    uint32 qual_in_key_template_idx;
    dnx_field_qual_t contender_qual_type;
    dnx_field_qual_attach_info_t contender_attach_info;
    dnx_field_group_t contender_fg_id;
    int fully_shareable;
    int partially_shareable;
    int partial_sharing_offset_lsb;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    uint32 opcodes_in_group[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF + 1] = { 0 };
    int nof_opcodes_in_group;
    uint32 opcode_predefined;
    int opcode_index;
    uint8 nof_fwd_segments;
    SHR_FUNC_INIT_VARS(unit);

    *found_segment_idx_p = DNX_KBP_INVALID_SEGMENT_ID;
    *msb_offset_bytes_overlay_sharing_p = 0;

    SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_group
                    (unit, opcode_id_sharing, &nof_opcodes_in_group, opcodes_in_group, &opcode_predefined));
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id_sharing, &nof_fwd_segments));

    for (opcode_index = 0;
         (opcode_index < nof_opcodes_in_group) && (*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID); opcode_index++)
    {
        uint32 opcode_id_shared = opcodes_in_group[opcode_index];
        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.get(unit, opcode_id_shared, &kbp_kbp_info_info));
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id_shared, &nof_segments, ms_key_segments));
        for (seg_idx = 0; (seg_idx < nof_fwd_segments) && (*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID);
             seg_idx++)
        {
            /** Sanity check.*/
            if (ms_key_segments[seg_idx].is_overlay_field)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "In opcode %d segment %d is within %d FWD segments, but is overlay.\r\n",
                             opcode_id_shared, seg_idx, nof_fwd_segments);
            }
            contender_fg_id = kbp_kbp_info_info.master_key_info.segment_info[seg_idx].fg_id;
            if (contender_fg_id == DNX_FIELD_GROUP_INVALID)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, contender_fg_id, &key_template));
            SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, contender_fg_id, opcode_id_shared,
                                                                              qual_attach_info_arr));
            /** Sanity check.*/
            if (kbp_kbp_info_info.master_key_info.segment_info[seg_idx].qual_idx[0] ==
                DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "In opcode %d segment %d is zero padding of fg_id %d, "
                             "but it has no qualifier.\r\n", opcode_id_shared, seg_idx, contender_fg_id);
            }
            /** Sanity check.*/
            if (kbp_kbp_info_info.master_key_info.segment_info[seg_idx].qual_idx[1] !=
                DNX_FIELD_KBP_PACKED_QUAL_INDEX_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "In opcode %d segment %d is zero padding of fg_id %d, but it is packed.\r\n",
                             opcode_id_shared, seg_idx, contender_fg_id);
            }
            qual_in_key_template_idx = kbp_kbp_info_info.master_key_info.segment_info[seg_idx].qual_idx[0];

            contender_qual_type = key_template.key_qual_map[qual_in_key_template_idx].qual_type;
            sal_memcpy(&contender_attach_info, &qual_attach_info_arr[qual_in_key_template_idx],
                       sizeof(contender_attach_info));
            if (force_same_qual && (qual_type != contender_qual_type))
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_compare_qualifiers_for_sharing
                            (unit, 1, &contender_qual_type, &contender_attach_info,
                             1, &qual_type, qual_info_p,
                             &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
            if (fully_shareable)
            {
                /*
                 * Full sharing.
                 */
                *found_segment_idx_p = seg_idx;
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found sharing zero padding opcode %d segment %d to opcode %d qual %s.\n",
                             opcode_id_shared, seg_idx, opcode_id_sharing, dnx_field_dnx_qual_text(unit, qual_type));
            }
            else if (partially_shareable &&
                     (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb))
            {
                uint32 containing_qual_size;
                uint32 contained_qual_size;
                /*
                 * Try partial sharing, if we use overlay sharing and the offset from LSB is in full bytes.
                 * Do not do partial sharing for packed qualifiers at this point.
                 */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, NULL, NULL, &contained_qual_size));
                *msb_offset_bytes_overlay_sharing_p =
                    ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                    BITS2BYTES(partial_sharing_offset_lsb);
                *found_segment_idx_p = seg_idx;
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found ACL match opcod_id %d/%d qual %s, "
                             "using overlay sharing. Partial sharing with offset from MSB %d. Zero padding.\n",
                             opcode_id_shared, opcode_id_shared, dnx_field_dnx_qual_text(unit, qual_type),
                             *msb_offset_bytes_overlay_sharing_p);
                /** Sanity check. */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                (unit, DNX_FIELD_STAGE_EXTERNAL, contender_qual_type,
                                 NULL, NULL, &containing_qual_size));
                if (ms_key_segments[seg_idx].nof_bytes != BITS2BYTES(containing_qual_size))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment \"%.*s\" is %d bytes, while qualifier %s is %d bytes.\r\n",
                                 DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                 ms_key_segments[seg_idx].nof_bytes, dnx_field_dnx_qual_text(unit, contender_qual_type),
                                 BITS2BYTES(containing_qual_size));
                }
                /** Sanity check. */
                if ((*msb_offset_bytes_overlay_sharing_p) < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                                 *msb_offset_bytes_overlay_sharing_p);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key in the FWD part of the master key.
*  The verification will be done over all fwd segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP OPCODE ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] qual_type       - qualifier type added by user
* \param [in] qual_info_p     - attach info as was passed by user
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
*                                    Holds segment_idx if found match
*                                    Holds DNX_KBP_INVALID_SEGMENT_ID if not found
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_fwd_segment_exist(
    int unit,
    uint32 opcode_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 seg_idx;
    uint32 qual_size;
    uint32 nof_segments;
    uint8 nof_fwd_segments;
    dnx_field_qual_t associated_dnx_qual;
    dnx_field_qual_class_e qual_class;
    dnx_field_qual_attach_info_t associated_attach_info;
    int fully_shareable;
    int partially_shareable;
    int partial_sharing_offset_lsb;
    uint8 is_qual_ranged;
    dbal_fields_e qual_field_id;
    dbal_tables_e dbal_table_id;
    int partial_only_qualifier;
    /** Initializations to appease the compiler. */
    int partial_only_offset_from_lsb = 0;
    int partial_only_size_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    *found_segment_idx_p = DNX_KBP_INVALID_SEGMENT_ID;
    *msb_offset_bytes_overlay_sharing_p = 0;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));
    if (nof_fwd_segments > nof_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d nof_fwd_segments %d is larger than nof_segments %d.\r\n",
                     opcode_id, nof_fwd_segments, nof_segments);
    }
    if (nof_fwd_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has %d fwd segments, maximum is %d.\r\n",
                     opcode_id, nof_fwd_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
    }

    /*
     * Ranged and non-ranged qualifiers cannot be shared at the moment, except using overlay.
     * Even with overlay, we can share non-ranged with ranges, but not vice versa.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, &qual_field_id));
    SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_field_id, &is_qual_ranged));

    if (is_qual_ranged == FALSE)
    {
        for (seg_idx = 0; (seg_idx < nof_fwd_segments) && (*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID);
             seg_idx++)
        {
            if ((is_overlay == FALSE) && (already_shared_segments[seg_idx]))
            {
                continue;
            }
            partial_only_qualifier = FALSE;
            if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV4_SIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV4_DIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV4_DST);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV6_SIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV6_SIP);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV6_DIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV6_DIP);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "VRF", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VRF_VALUE);
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF_15_00", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
                partial_only_qualifier = TRUE;
                partial_only_offset_from_lsb = 0;
                partial_only_size_bits = 16;
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF_21_16", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
                partial_only_qualifier = TRUE;
                partial_only_offset_from_lsb = 16;
                partial_only_size_bits = 6;
            }
            else
            {
                associated_dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
            }

            if (associated_dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID)
            {
                /** Sanity check: Verify the size of the segment.*/
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                (unit, DNX_FIELD_STAGE_EXTERNAL, associated_dnx_qual, NULL, NULL, &qual_size));
                if (partial_only_qualifier)
                {
                    if (qual_size < partial_only_size_bits)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is supposed to be a part of a qualfier with size %d bits, "
                                     "but the full qualifier size is %d bits.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     partial_only_size_bits, qual_size);
                    }
                    if (qual_size == partial_only_size_bits)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is supposed to be a part of a qualfier with size %d bits, "
                                     "but the full qualifier size is the same as the part.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     partial_only_size_bits);
                    }
                    if (BITS2BYTES(partial_only_size_bits) != ms_key_segments[seg_idx].nof_bytes)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment \"%.*s\" is %d bytes, while it should be %d bytes.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     ms_key_segments[seg_idx].nof_bytes, BITS2BYTES(qual_size));
                    }
                }
                else
                {
                    if (BITS2BYTES(qual_size) != ms_key_segments[seg_idx].nof_bytes)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is %d bytes, while qualifier %s is %d bytes.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     ms_key_segments[seg_idx].nof_bytes,
                                     dnx_field_dnx_qual_text(unit, associated_dnx_qual), BITS2BYTES(qual_size));
                    }
                }
                /*
                 * Create the attach info of the associated qualifier.
                 */
                SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &associated_attach_info));
                qual_class = DNX_QUAL_CLASS(associated_dnx_qual);
                switch (qual_class)
                {
                    case DNX_FIELD_QUAL_CLASS_HEADER:
                    {
                        associated_attach_info.input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
                        associated_attach_info.input_arg = 0;
                        associated_attach_info.offset = 0;
                        break;
                    }
                    case DNX_FIELD_QUAL_CLASS_META:
                    {
                        associated_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
                        break;
                    }
                        /*
                         * coverity explanation: The possible qual_class values are HEADER and META, we can't have any other types.
                         * This can be seen from the assigned values for associated_dnx_qual. It is possible that the class of above qualifiers
                         * can be change in the future or by mistake someone added wrong class, that's why this default case was added.
                         */
                         /* coverity[dead_error_begin:FALSE]  */
                    default:
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown qual class for sharing %d. DNX qual 0x%x.\n",
                                     qual_class, qual_type);
                        break;
                    }
                }

                /*
                 * Verify that the qualifiers take the same info.
                 */
                SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_compare_qualifiers_for_sharing
                                (unit, 1, &associated_dnx_qual, &associated_attach_info, 1, &qual_type, qual_info_p,
                                 &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
                if (partial_only_qualifier == FALSE)
                {
                    if (fully_shareable)
                    {
                        /*
                         * full_sharing
                         */
                        *found_segment_idx_p = seg_idx;
                        if (is_overlay)
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                         "using overlay sharing.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                        else
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                    }
                    else if (partially_shareable && is_overlay &&
                             (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb))
                    {
                        uint32 contained_qual_size;
                        /*
                         * Try partial sharing, if we use overlay sharing and the offset from LSB is in full bytes.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                        (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, NULL, NULL, &contained_qual_size));
                        *msb_offset_bytes_overlay_sharing_p =
                            ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                            BITS2BYTES(partial_sharing_offset_lsb);
                        *found_segment_idx_p = seg_idx;
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                     "using overlay sharing. Partial sharing with offset from MSB %d\n",
                                     opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                                     *msb_offset_bytes_overlay_sharing_p);
                        /** Sanity check. */
                        if ((*msb_offset_bytes_overlay_sharing_p) < 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                                         *msb_offset_bytes_overlay_sharing_p);
                        }
                    }
                }
                else if (partially_shareable && (partial_sharing_offset_lsb >= partial_only_offset_from_lsb))
                {
                    /*
                     * Only a part of the quailfier is present on the FWD segment.
                     */
                    uint32 contained_qual_size;
                    int offset_diff;
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                    (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, NULL, NULL, &contained_qual_size));
                    offset_diff = partial_sharing_offset_lsb - partial_only_offset_from_lsb;
                    /** Try full sharing*/
                    if ((offset_diff == 0) && (contained_qual_size == partial_only_size_bits))
                    {
                        *found_segment_idx_p = seg_idx;
                        if (is_overlay)
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                         "using overlay sharing.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                        else
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                    }
                    /** Try partial sharing*/
                    else if ((BYTES2BITS(BITS2BYTES(offset_diff)) == offset_diff) &&
                             (partial_only_offset_from_lsb + partial_only_size_bits >=
                              partial_sharing_offset_lsb + contained_qual_size))
                    {
                        *msb_offset_bytes_overlay_sharing_p =
                            ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                            BITS2BYTES(offset_diff);
                        *found_segment_idx_p = seg_idx;
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                     "using overlay sharing. Partial sharing with offset from MSB %d\n",
                                     opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                                     *msb_offset_bytes_overlay_sharing_p);
                        /** Sanity check. */
                        if ((*msb_offset_bytes_overlay_sharing_p) < 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                                         *msb_offset_bytes_overlay_sharing_p);
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key.
*  The verification will be done over all acl and fwd segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP OPCODE ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] force_same_qual - Only share the same qualifier type.
* \param [in] nof_packed_quals - The number of qualifers in qual_type and qual_info.
* \param [in] qual_type       - qualifier type added by user.
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [in] qual_info       - attach info as was passed by user
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT elements.
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_group_segment_exist(
    int unit,
    uint32 opcode_id,
    dnx_field_group_t fg_id,
    int force_same_qual,
    int nof_packed_quals,
    dnx_field_qual_t qual_type[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_info[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    SHR_FUNC_INIT_VARS(unit);

    (*found_segment_idx_p) = DNX_KBP_INVALID_SEGMENT_ID;

    /*
     * Check if the qualifier (future segment) exists in zero padding.
     */
    if ((nof_packed_quals == 1) && is_overlay)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_acl_segment_exist_zero_padding
                        (unit, opcode_id, force_same_qual, qual_type[0], &qual_info[0],
                         found_segment_idx_p, msb_offset_bytes_overlay_sharing_p));
    }
    /*
     * Check if the qualifier (future segment) exists in the ACL part of the master key .
     */
    if (*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_acl_segment_exist
                        (unit, opcode_id, fg_id, force_same_qual, nof_packed_quals, qual_type, qual_info, is_overlay,
                         already_shared_segments, found_segment_idx_p, msb_offset_bytes_overlay_sharing_p));
    }
    /*
     * If not found in the ACL part, Check if the qualifier (future segment) exists in the FWD part of the master key.
     * We do not share packed qualifiers with FWD segments.
     */
    if ((*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID) && (force_same_qual == FALSE) && nof_packed_quals == 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_fwd_segment_exist
                        (unit, opcode_id, fg_id, qual_type[0], &qual_info[0], is_overlay,
                         already_shared_segments, found_segment_idx_p, msb_offset_bytes_overlay_sharing_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the master key info in SW state, specifically the number of segments used by FWD.
 * \param[in] unit       - Device ID
 * \return
 *   shr_error_e - Error Type
 */
static shr_error_e
dnx_field_kbp_group_master_key_init(
    int unit)
{
    uint32 opcode_id;
    uint32 nof_segments;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_KBP_OPCODE_ITERATOR(opcode_id)
    {
        DNX_FIELD_KBP_OPCODE_NOT_IN_USE_CONTINUE(unit, opcode_id);
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
        if (((int) nof_segments) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has negative number of segments %d.\r\n", opcode_id, nof_segments);
        }
        if (nof_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has %d segments, more than the maximum %d.\r\n",
                         opcode_id, nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
        }
        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                        nof_fwd_segments.set(unit, opcode_id, nof_segments));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_group_opcode_attach(
    int unit,
    dnx_field_group_t fg_id,
    uint32 opcode_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    int qual_idx;
    int nof_quals;
    uint32 lookup_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
    uint8 key_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
    int lookup_seg_idx_without_packing;
    uint32 absolute_key_segs_idx;
    dnx_field_key_template_t key_template;
    kbp_opcode_lookup_info_t lookup_info;
    dnx_field_kbp_segment_info_t fg_segment_info;
    kbp_mngr_key_segment_t ms_key_segments_existing[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    kbp_mngr_key_segment_t ms_key_segments_new[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    unsigned int payload_size;
    dbal_tables_e dbal_table_id;
    dnx_field_stage_e field_stage;
    uint32 in_ms_key_utilizes_zero_padding_seg_idx[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    uint32 found_in_ms_key_seg_idx_overlay[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    uint32 master_key_nof_bytes;
    int segment_position_on_master_key_bytes[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 same_field_group_overlay_sharing_is_on_master[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 same_field_group_overlay_sharing_is_not_on_master[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 same_field_group_overlay_sharing_the_shared_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    int overlay_sharing_extra_msb_offset_for_partial[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 absolute_segment_used_by_lookup_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    uint8 qual_packed_lookup_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint8 qual_packed_lookup_segment_last[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    int zero_padding_by_overlay_bytes_diff[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 qual_sizes_bits[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 qual_sizes_bytes[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    dbal_fields_e qual_dbal_field[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 nof_segments_existing;
    int unpacked_qual_ind = 0;
    int qual_packing_supported;
    uint32 opcodes_in_group[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF + 1] = { 0 };
    int nof_opcodes_in_group;
    uint32 opcode_predefined;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    /*
     * Note that we do not have keys updated for KBP field groups.
     */

    /*
     * 1.  Each aligned qualifier will represent a segment
     * 2.  Check if we can re-use(share) the quals. (The attach-info pointers
     *     are stored with the master key)
     * 3.  Result buffer id and offset in the buffer is supplied by user.
     * 4.  KBP: Add new lookup with the List of segments to the Opcode
     *     a.  kbp_mngr_opcode_result_add()
     * 5.  KBP: Update the KBP device with the new Master Key
     *     a. kbp_mngr_opcode_master_key_segments_add()
     */

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));

    for (key_seg_idx = 0; key_seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; key_seg_idx++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &ms_key_segments_existing[key_seg_idx]));
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &ms_key_segments_new[key_seg_idx]));
    }

    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    /*
     * Update basic lookup info
     */
    lookup_info.dbal_table_id = dbal_table_id;
    lookup_info.result_index = attach_info_p->payload_id;
    lookup_info.result_size = payload_size;

    /*
     * Convert the payload offset from offset from LSB to offset from MSB
     */
    if (attach_info_p->payload_offset >= dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Payload offset %d exceeds payload buffer range 0-%d\r\n",
                     attach_info_p->payload_offset,
                     dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit) - 1);
    }
    if ((attach_info_p->payload_offset % SAL_UINT8_NOF_BITS) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For external TCAM, payload_offset (%d) must be byte aligned.\n",
                     attach_info_p->payload_offset);
    }
    if (attach_info_p->payload_offset + lookup_info.result_size >
        dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Payload offset %d plus payload size %d exceeds payload buffer size %d\r\n",
                     attach_info_p->payload_offset, lookup_info.result_size,
                     dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit));
    }
    lookup_info.result_offset =
        dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit) - attach_info_p->payload_offset -
        lookup_info.result_size;
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fg_id %d opcode_id %d payload size %d%s\n", fg_id, opcode_id, lookup_info.result_size,
                 EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fg_id %d opcode_id %d offset from lsb %d, from msb %d\n", fg_id, opcode_id,
                 attach_info_p->payload_offset, lookup_info.result_offset);
    /** Sanity check*/
    if (((unsigned int) lookup_info.result_offset >= dnx_data_field.external_tcam.max_payload_size_per_opcode_get(unit))
        || (lookup_info.result_offset % 8) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MSB payload offset calculations reached illogical value %d.\r\n",
                     lookup_info.result_offset);
    }

    /*
     * Get the List of already configured segments in the master key.
     * absolute_key_segs_idx will receive the number of segments already configured in the key.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments_existing, ms_key_segments_existing));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fg_id %d opcode_id %d absolute_key_segs_idx %d%s\n", fg_id, opcode_id,
                 nof_segments_existing, EMPTY);

    /*
     * Find the position in bytes of each segment we add to the master key.
     */
    for (absolute_key_segs_idx = 0; absolute_key_segs_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY;
         absolute_key_segs_idx++)
    {
        /*
         * Initialize to invalid negative value for sanity check.
         */
        segment_position_on_master_key_bytes[absolute_key_segs_idx] = -1;
    }
    master_key_nof_bytes = 0;
    for (absolute_key_segs_idx = 0; absolute_key_segs_idx < nof_segments_existing; absolute_key_segs_idx++)
    {
        /*
         * Do not count overlay segments.
         */
        if (ms_key_segments_existing[absolute_key_segs_idx].is_overlay_field)
        {
            continue;
        }
        segment_position_on_master_key_bytes[absolute_key_segs_idx] = master_key_nof_bytes;
        master_key_nof_bytes += ms_key_segments_existing[absolute_key_segs_idx].nof_bytes;
    }

    /*
     * Count the number of qualifiers in the field group.
     * We use it to invert the order of the qualifiers (write them to KBP manager from MSB to LSB) so that they match
     * their order in DBAL.
     */
    for (nof_quals = 0; (nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[nof_quals].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); nof_quals++);

    if (nof_quals > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Too many segments per field group for fg_id %d. Maximum is %d.\r\n",
                     fg_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP);
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.is_valid.set(unit, opcode_id, TRUE));

    /*
     * Initialize arrays.
     */
    for (lookup_seg_idx = 0; lookup_seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; lookup_seg_idx++)
    {
        found_in_ms_key_seg_idx_overlay[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        absolute_segment_used_by_lookup_segment[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        absolute_segment_used_by_lookup_segment[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }

    /*
     * Find qual sizes and dbal fields.
     */
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type,
                         NULL, NULL, &qual_sizes_bits[qual_idx]));
        qual_sizes_bytes[qual_idx] = BITS2BYTES(qual_sizes_bits[qual_idx]);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, DNX_FIELD_STAGE_EXTERNAL, key_template.key_qual_map[qual_idx].qual_type,
                         &qual_dbal_field[qual_idx]));
    }

    /*
     * Mark the packed qualifiers.
     * All DBAL fields other than the last (last qualifer, first DBAL field) in a packed bunch will be marked as packed.
     * We verify that the field group uses the packing flag as a sanity check.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_external_tcam_support_packing(unit, fg_id, &qual_packing_supported));
    {
        int prev_qual_marked_as_packed = FALSE;
        uint8 is_qual_packed;
        for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
        {
            SHR_IF_ERR_EXIT(dbal_table_field_is_packed_get(unit, dbal_table_id, qual_dbal_field[qual_idx],
                                                           &is_qual_packed));
            if (is_qual_packed && (qual_packing_supported == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d opcode_id %d qualfier %d dbal field %d is marked "
                             "as packed, but the field group was not created with qual packing flag.\r\n",
                             fg_id, opcode_id, qual_idx, qual_dbal_field[qual_idx]);
            }
            if (is_qual_packed || prev_qual_marked_as_packed)
            {
                qual_packed_lookup_segment[lookup_seg_idx] = TRUE;
            }
            if ((is_qual_packed == FALSE) && prev_qual_marked_as_packed)
            {
                /*
                 * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
                 */
                 /* coverity[dead_error_line:FALSE]  */
                qual_packed_lookup_segment_last[lookup_seg_idx] = TRUE;
            }
            prev_qual_marked_as_packed = is_qual_packed;
        }
        if (prev_qual_marked_as_packed)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Field group %d opcode_id %d last qualifer %d (0x%x) is marked as packed.\r\n",
                         fg_id, opcode_id, qual_idx, key_template.key_qual_map[nof_quals].qual_type);
        }
    }

    /*
     * Sanity check: verify that no packed qualifer is ranged.
     */
    for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
    {
        uint8 is_qual_ranged;
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_dbal_field[qual_idx],
                                                           &is_qual_ranged));
            if (is_qual_ranged)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Field group %d opcode_id %d qualifier %d (0x%x) is both packed and ranged.\r\n",
                             fg_id, opcode_id, qual_idx, key_template.key_qual_map[nof_quals].qual_type);
            }
        }
    }

    /*
     * Find what segemnts can be shared using overlay with already existing segments.
     */
    for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
    {
        uint32 segment_found;
        int qual_idx_2;
        int lookup_seg_idx_2;
        int nof_packed_quals;
        dnx_field_qual_t qual_types[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
        dnx_field_qual_attach_info_t qual_infos[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT];
        int packed_index;
        int partial_sharing_extra_offset;

        qual_types[0] = key_template.key_qual_map[qual_idx].qual_type;
        sal_memcpy(&qual_infos[0], &attach_info_p->qual_info[qual_idx], sizeof(qual_infos[0]));
        nof_packed_quals = 1;

        /*
         * Get all packed segments, if we have any.
         */
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;; qual_idx_2++, lookup_seg_idx_2--)
            {
                packed_index = qual_idx_2 - qual_idx;
                if (qual_idx_2 >= nof_quals || qual_packed_lookup_segment[lookup_seg_idx_2] == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In opcode_id %d reached beyond a bunch of packed qualifers without "
                                 "reaching the last of them.\r\n", opcode_id);
                }
                if (packed_index >= DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In opcode_id %d there is a bunch of packed qualifers beyond maximum %d.\r\n",
                                 opcode_id, DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT);
                }
                qual_types[packed_index] = key_template.key_qual_map[qual_idx_2].qual_type;
                sal_memcpy(&qual_infos[packed_index], &attach_info_p->qual_info[qual_idx_2], sizeof(qual_infos[0]));
                if (qual_packed_lookup_segment_last[lookup_seg_idx_2])
                {
                    nof_packed_quals = packed_index + 1;
                    break;
                }
            }
        }
        if (nof_packed_quals <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d, reached zero sized packed qualifers.\r\n", opcode_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_kbp_group_segment_exist(unit,
                                                          opcode_id, fg_id, FALSE,
                                                          nof_packed_quals, qual_types, qual_infos, TRUE,
                                                          NULL, &segment_found, &partial_sharing_extra_offset));
        if (segment_found != DNX_KBP_INVALID_SEGMENT_ID)
        {
            /** Sanity check.*/
            if (segment_found >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shareable segment %d found in master key, maximum is %x. "
                             "Field group %d opcode_id %d share run overlay.\r\n",
                             segment_found, (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1), fg_id, opcode_id);
            }
            for (packed_index = 0; packed_index < nof_packed_quals; packed_index++)
            {
                found_in_ms_key_seg_idx_overlay[lookup_seg_idx - packed_index] = segment_found;
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx - packed_index] =
                    partial_sharing_extra_offset;
            }
        }
        /*
         * Jump over the packed segments.
         */
        qual_idx += nof_packed_quals - 1;
        lookup_seg_idx -= nof_packed_quals - 1;
    }

    /*
     * Find what segemnts can be shared using overlay with the newly added segments.
     * That is relevant for sharing within the same field group and for sharing with ranged qualifiers.
     */
    for (qual_idx = nof_quals - 1, lookup_seg_idx = 0; qual_idx >= 0; qual_idx--, lookup_seg_idx++)
    {
        int fully_shareable;
        int partially_shareable;
        int partial_sharing_offset_lsb;
        int qual_idx_2;
        int lookup_seg_idx_2;

        if (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
            same_field_group_overlay_sharing_is_on_master[lookup_seg_idx])
        {
            continue;
        }

        /*
         * We do not support same field group overlay sharing for packed qualifiers,
         * as that is only useful for ranged quals.
         */
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            continue;
        }
        for (qual_idx_2 = 0, lookup_seg_idx_2 = nof_quals - 1; qual_idx_2 < nof_quals; qual_idx_2++, lookup_seg_idx_2--)
        {
            if (qual_idx_2 == qual_idx)
            {
                continue;
            }
            /*
             * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
             */
             /* coverity[dead_error_line:FALSE]  */
            if (found_in_ms_key_seg_idx_overlay[lookup_seg_idx_2] != DNX_KBP_INVALID_SEGMENT_ID)
            {
                continue;
            }
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx_2])
            {
                continue;
            }
            /*
             * We do not support same field group overlay sharing for packed qualifiers,
             * as that is only useful for ranged quals.
             */
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_kbp_key_acl_compare_qualifiers_for_sharing
                            (unit, 1, &key_template.key_qual_map[qual_idx_2].qual_type,
                             &(attach_info_p->qual_info[qual_idx_2]), 1, &key_template.key_qual_map[qual_idx].qual_type,
                             &(attach_info_p->qual_info[qual_idx]),
                             &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
            if (fully_shareable)
            {
                same_field_group_overlay_sharing_is_on_master[lookup_seg_idx_2] = TRUE;
                same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx] = TRUE;
                same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx] = lookup_seg_idx_2;
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found same field group match with opcode_id %d quals %s and %s, "
                             "using overlay sharing. field group Id %d.\n",
                             opcode_id, dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx_2].qual_type), fg_id);
                break;
            }
            else if (partially_shareable &&
                     (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb))
            {
                /*
                 * Try partial sharing.
                 */
                same_field_group_overlay_sharing_is_on_master[lookup_seg_idx_2] = TRUE;
                same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx] = TRUE;
                same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx] = lookup_seg_idx_2;
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx] =
                    qual_sizes_bytes[qual_idx_2] - qual_sizes_bytes[qual_idx] - BITS2BYTES(partial_sharing_offset_lsb);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found same field group match with opcode_id %d quals %s and %s, "
                             "using overlay sharing. Partial sharing with offset from MSB %d\n", opcode_id,
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx_2].qual_type),
                             overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx]);
                break;
            }
        }
    }

    /*
     * Find zero padding.
     */
    {
        int zero_padding_segment_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
        int nof_zero_padding_segments;
        int smallest_zero_padding_size;
        int smallest_zero_padding_idx;
        int biggest_qualifier_size;
        int biggest_qualifier_idx;
        int biggest_qualifier_lookup_seg_idx;
        int is_zero_padding;
        uint8 is_one_ffc_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
        uint8 nof_fwd_segments;
        dbal_enum_value_field_kbp_kbr_idx_e seg_key;
        int seg_lsb_on_key;
        int seg_size_on_one_key;

        /** Find the number of segments used by FWD.*/
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));

        nof_zero_padding_segments = 0;
        for (key_seg_idx = 0; key_seg_idx < nof_fwd_segments; key_seg_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.get(unit, opcode_id, key_seg_idx, &fg_segment_info));
            if (fg_segment_info.fg_id == DNX_FIELD_GROUP_INVALID)
            {
                /*
                 * We have a segment used by FWD and not being filled already.
                 */
                SHR_IF_ERR_EXIT(dnx_field_kbp_group_acl_segment_is_zero_padding
                                (unit, &ms_key_segments_existing[key_seg_idx], &is_zero_padding));
                if (is_zero_padding)
                {
                    zero_padding_segment_size[key_seg_idx] =
                        BYTES2BITS(ms_key_segments_existing[key_seg_idx].nof_bytes);
                    /** Sanity check.*/
                    if (zero_padding_segment_size[key_seg_idx] <= 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d, we have segemnt %d that is zero padding with "
                                     "size 0 (%d bytes).\r\n",
                                     opcode_id, key_seg_idx, ms_key_segments_existing[key_seg_idx].nof_bytes);
                    }
                    /*
                     * If the zero padded segment straddles multiple keys, only count the LSB.
                     * For now we do not support filling straddling segments at all.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_kbp_key_fwd_segment_unified_position
                                    (unit, opcode_id, key_seg_idx, &seg_key, &seg_lsb_on_key, &seg_size_on_one_key));
                    if (zero_padding_segment_size[key_seg_idx] > seg_size_on_one_key)
                    {
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "In opcode_id %d, segment %d is a zero padding segment that straddles multiple "
                                     "keys, %d bits in LSB key out of %d bits in segment in total.\n",
                                     opcode_id, key_seg_idx, seg_size_on_one_key,
                                     zero_padding_segment_size[key_seg_idx]);
                        zero_padding_segment_size[key_seg_idx] = 0;
                    }
                    else
                    {
                        nof_zero_padding_segments++;
                    }
                }
            }
        }
        /*
         * Find what qualifers require only one FFC.
         */
        for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_qual_info_uses_up_to_one_ffc
                            (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type,
                             &attach_info_p->qual_info[qual_idx], &is_one_ffc_qual[qual_idx]));
        }
        /*
         * Match segments by size.
         */
        while (nof_zero_padding_segments > 0)
        {
            /** Find the smallest zero padding */
            smallest_zero_padding_size = 0;
            smallest_zero_padding_idx = DNX_KBP_INVALID_SEGMENT_ID;
            for (key_seg_idx = 0; key_seg_idx < nof_fwd_segments; key_seg_idx++)
            {
                if ((zero_padding_segment_size[key_seg_idx] > 0) &&
                    ((zero_padding_segment_size[key_seg_idx] < smallest_zero_padding_size) ||
                     (smallest_zero_padding_idx == DNX_KBP_INVALID_SEGMENT_ID)))
                {
                    smallest_zero_padding_size = zero_padding_segment_size[key_seg_idx];
                    smallest_zero_padding_idx = key_seg_idx;
                }
            }
            /** Sanity check.*/
            if (smallest_zero_padding_size == 0 || smallest_zero_padding_idx == DNX_KBP_INVALID_SEGMENT_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not find zero padding, for zero padding %d/%d.\r\n",
                             key_seg_idx, nof_fwd_segments);
            }
            /*
             * Find the biggest fitting non shared one FFC qualifier.
             */
            biggest_qualifier_size = 0;
            biggest_qualifier_idx = DNX_KBP_INVALID_SEGMENT_ID;
            biggest_qualifier_lookup_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
            for (lookup_seg_idx = 0, qual_idx = nof_quals - 1; qual_idx >= 0; qual_idx--, lookup_seg_idx++)
            {
                int is_shared = (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                                 same_field_group_overlay_sharing_is_on_master[lookup_seg_idx] ||
                                 same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);
                int is_packed = qual_packed_lookup_segment[lookup_seg_idx];
                if (is_shared == FALSE && is_packed == FALSE &&
                    in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] == DNX_KBP_INVALID_SEGMENT_ID &&
                    zero_padding_segment_size[smallest_zero_padding_idx] >= qual_sizes_bits[qual_idx] &&
                    is_one_ffc_qual[qual_idx] &&
                    ((qual_sizes_bits[qual_idx] > biggest_qualifier_size) ||
                     (biggest_qualifier_idx == DNX_KBP_INVALID_SEGMENT_ID)))
                {
                    biggest_qualifier_size = qual_sizes_bits[qual_idx];
                    biggest_qualifier_idx = qual_idx;
                    biggest_qualifier_lookup_seg_idx = lookup_seg_idx;
                }
            }
            /** Check if a suitable qualifier was found.*/
            if (biggest_qualifier_size == 0 || biggest_qualifier_idx == DNX_KBP_INVALID_SEGMENT_ID)
            {
                /** Remove the smallest zero padding from the list.*/
                zero_padding_segment_size[smallest_zero_padding_idx] = 0;
                nof_zero_padding_segments--;
                continue;
            }
            else
            {
                /** Reduce the size of the smallest zero padding, to remove the bytes taken by the filling.*/
                zero_padding_by_overlay_bytes_diff[biggest_qualifier_lookup_seg_idx] =
                    BITS2BYTES(zero_padding_segment_size[smallest_zero_padding_idx]) -
                    BITS2BYTES(biggest_qualifier_size);
                zero_padding_segment_size[smallest_zero_padding_idx] -= BYTES2BITS(BITS2BYTES(biggest_qualifier_size));
                if (zero_padding_segment_size[smallest_zero_padding_idx] == 0)
                {
                    nof_zero_padding_segments--;
                }
                /** Sanity check*/
                if (zero_padding_segment_size[smallest_zero_padding_idx] < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Zero padding segment filled with a larger segment.\r\n");
                }
            }
            /** Mark the padding.*/
            in_ms_key_utilizes_zero_padding_seg_idx[biggest_qualifier_lookup_seg_idx] = smallest_zero_padding_idx;
            LOG_DEBUG_EX(BSL_LOG_MODULE, "In fg_id %d opcode_id %d, sharing qualifier %d with zero padding %d \n",
                         fg_id, opcode_id, qual_idx, smallest_zero_padding_idx);
        }
    }

    /*
     * 1. Update the list of Segments for the Master Key out of the FG qualifiers.
     * 2. Build the list of segments for the lookup update.
     * In case the segment for a given qualifier exists in the master key we will only add its
     * seg_id into the list of the segments for the lookup.
     * We will not add it into the ms_key_segments as it already exists in there.
     */
    absolute_key_segs_idx = nof_segments_existing;
    for (key_seg_idx = 0, lookup_seg_idx = 0, lookup_seg_idx_without_packing = 0, qual_idx = nof_quals - 1;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        int is_shared_overlay;
        int is_zero_padding_filling;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        int packed_qual_idx;
        /** Sanity check */
        if (lookup_seg_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "lookup_seg_idx %d exceeds maximum %d, even though number of qualifer "
                         "was already verified.\r\n", lookup_seg_idx, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP - 1);
        }
        is_shared_overlay = (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                             same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);
        is_zero_padding_filling =
            (in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID);
        /** Sanity check.*/
        if (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID &&
            same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is overlay shared both within field "
                         "group and without.\r\n", qual_idx, fg_id, opcode_id);
        }

        nof_quals_packing = 0;
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            /** Sanity check.*/
            if (qual_packed_lookup_segment_last[lookup_seg_idx] == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is not last in packing bunch "
                             "but was not skipped.\r\n", qual_idx, fg_id, opcode_id);
            }
            nof_quals_packing++;
            for (qual_idx_2 = qual_idx - 1, lookup_seg_idx_2 = lookup_seg_idx + 1;
                 qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
            {
                /*
                 * Count the number of packed qualifiers until we reach a non packed qualifer or one that
                 * begins a new packed bunch.
                 */
                /*
                 * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
                 */
                 /* coverity[dead_error_line:FALSE]  */
                if (qual_packed_lookup_segment[lookup_seg_idx_2] &&
                    (qual_packed_lookup_segment_last[lookup_seg_idx_2] == FALSE))
                {
                    nof_quals_packing++;
                }
                else
                {
                    break;
                }
            }
        }
        /** Sanity check.*/
        if (nof_quals_packing == 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is a packed qualifer "
                         "only with itself.\r\n", qual_idx, fg_id, opcode_id);
        }
        if (nof_quals_packing > DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d Has a packing group of size %d, maximum is %d.\r\n",
                         opcode_id, nof_quals_packing, DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT);
        }

        /*
         * Check if the segment exists. If not - add it to the Master Key.
         * the search for shared segments is done on FWD and ACL segments.
         */
        else if (is_shared_overlay && is_zero_padding_filling)
        {
            /** Sanity check*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is both shared (with segment %d) and "
                         "filling zero padding with segment %d) \r\n", qual_idx, fg_id, opcode_id,
                         found_in_ms_key_seg_idx_overlay[lookup_seg_idx],
                         in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx]);
        }
        else if ((is_zero_padding_filling) && (nof_quals_packing > 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is packed but still zero "
                         "padding filling.\r\n", qual_idx, fg_id, opcode_id);
        }
        else if (is_zero_padding_filling)
        {
            /*
             * An existing zero padding segment will be used.
             * The zero padding segment will be changed later.
             * The zero padding will be done using overlasy sharing.
             */
            lookup_info.nof_segments++;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = DNX_KBP_INVALID_SEGMENT_ID;
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Filling zero padding qual %s fg_id %d opcode_id %d key_seg_idx %d overlay \n",
                         ms_key_segments_existing[lookup_info.key_segment_index[lookup_seg_idx_without_packing]].name,
                         fg_id, opcode_id, found_in_ms_key_seg_idx_overlay[lookup_seg_idx]);
        }
        else if (is_shared_overlay)
        {
            /*
             * Segment already exists and will be shared - Update the Lookup Info
             */
            lookup_info.nof_segments++;
            /*
             * The master segment index will be updated later.
             */
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = DNX_KBP_INVALID_SEGMENT_ID;
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Re-use qual %s fg_id %d opcode_id %d using same field group overlay.%s\n",
                             ms_key_segments_existing[lookup_info.
                                                      key_segment_index[lookup_seg_idx_without_packing]].name, fg_id,
                             opcode_id, EMPTY);
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Re-use qual %s fg_id %d opcode_id %d key_seg_idx %d using overlay.\n",
                             ms_key_segments_existing[lookup_info.
                                                      key_segment_index[lookup_seg_idx_without_packing]].name, fg_id,
                             opcode_id, found_in_ms_key_seg_idx_overlay[lookup_seg_idx]);
            }

            /*
             * The overlay sharing segment will be added in the following loop.
             */
        }
        else
        {
            int segment_size_bits_total;
            /*
             * There is no such segment in the already defined Master Key, create a new Segment.
             */
            if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                             opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            }
            if (nof_quals_packing > 1)
            {
                sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                             sizeof(ms_key_segments_new[key_seg_idx].name),
                             "%.*s:%d:P%d",
                             BCM_FIELD_MAX_SHORT_NAME_LEN,
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type), fg_id,
                             nof_quals_packing);
            }
            else
            {
                sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                             sizeof(ms_key_segments_new[key_seg_idx].name),
                             "%.*s:%d",
                             BCM_FIELD_MAX_SHORT_NAME_LEN,
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type), fg_id);
            }

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Add qual %s fg_id %d to opcode_id %d key_seg_idx %d \n",
                         ms_key_segments_new[key_seg_idx].name, fg_id, opcode_id, key_seg_idx);

            segment_size_bits_total = qual_sizes_bits[qual_idx];
            for (packed_qual_idx = 1; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                segment_size_bits_total += qual_sizes_bits[qual_idx - packed_qual_idx];
            }

            /** size has to be byte align need to validate it   */
            ms_key_segments_new[key_seg_idx].nof_bytes = BITS2BYTES(segment_size_bits_total);

            /*
             * Save the information required for future hw configuration of this segment (fg_segment_info)
             * */
            SHR_IF_ERR_EXIT(dnx_field_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[0] = qual_idx;
            for (packed_qual_idx = 0; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                fg_segment_info.qual_idx[packed_qual_idx] = qual_idx - nof_quals_packing + packed_qual_idx + 1;
            }
            /*
             * absolute_key_segs_idx represents the absolute index in the master key, considering all lookups for this
             * opcode. thus, we save the information on index absolute_key_segs_idx.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.set(unit, opcode_id, absolute_key_segs_idx, &fg_segment_info));

            /*
             * Update positon on master key.
             */
            segment_position_on_master_key_bytes[absolute_key_segs_idx] = master_key_nof_bytes;
            master_key_nof_bytes += BITS2BYTES(segment_size_bits_total);
            absolute_segment_used_by_lookup_segment[lookup_seg_idx] = absolute_key_segs_idx;

            /*
             * Update the Lookup Info
             * */
            lookup_info.nof_segments++;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = absolute_key_segs_idx;

            /*
             * Increment only if new segment was created
             */
            key_seg_idx++;
            absolute_key_segs_idx++;
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id %d to opcode_id %d nof added segments %d absolute_key_segs_idx %d.\n",
                 fg_id, opcode_id, key_seg_idx, absolute_key_segs_idx);

    /*
     * Add the overlay segments.
     */
    for (lookup_seg_idx = 0, qual_idx = nof_quals - 1, lookup_seg_idx_without_packing = 0;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        int is_shared_overlay;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        int packed_qual_idx;
        /** Sanity check */
        if (lookup_seg_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "lookup_seg_idx %d exceeds maximum %d, even though number of qualifer "
                         "was already verified.\r\n", lookup_seg_idx, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP - 1);
        }
        is_shared_overlay = (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                             same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);

        nof_quals_packing = 0;
        for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;
             qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
        {
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                if ((lookup_seg_idx_2 > lookup_seg_idx) && qual_packed_lookup_segment_last[lookup_seg_idx_2])
                {
                    /** If we are not at the first packed qual and we reached a new set of packed quals, stop.*/
                    break;
                }
                nof_quals_packing++;
            }
            else
            {
                break;
            }
        }

        /*
         * If overlay shared, add that info to master key.
         */
        if (is_shared_overlay)
        {
            uint32 segment_shared_with;
            /*
             * Sanity check.
             */
            if ((lookup_info.nof_segments < lookup_seg_idx_without_packing) ||
                (lookup_info.key_segment_index[lookup_seg_idx_without_packing] !=
                 ((typeof(lookup_info.key_segment_index[0])) DNX_KBP_INVALID_SEGMENT_ID)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error updating lookup for overlay sharing.\r\n");
            }
            /*
             * Sanity check.
             */
            if (overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx] < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error negative offset for overlay partial sharing.\r\n");
            }

            /*
             * There is no such segment in the already defined Master Key, create a new overlay segment.
             */
            if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                             opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            }

            sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                         sizeof(ms_key_segments_new[key_seg_idx].name),
                         "%.*s:%d",
                         BCM_FIELD_MAX_SHORT_NAME_LEN,
                         dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type), fg_id);

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Add qual %s fg_id %d to opcode_id %d key_seg_idx %d \n",
                         ms_key_segments_new[key_seg_idx].name, fg_id, opcode_id, key_seg_idx);

            /** size has to be byte align need to validate it   */
            ms_key_segments_new[key_seg_idx].nof_bytes = qual_sizes_bytes[qual_idx];

            ms_key_segments_new[key_seg_idx].is_overlay_field = TRUE;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = absolute_key_segs_idx;

            /*
             * Find the segment to share with.
             */
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
            {
                segment_shared_with = absolute_segment_used_by_lookup_segment
                    [same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx]];
            }
            else
            {
                segment_shared_with = found_in_ms_key_seg_idx_overlay[lookup_seg_idx];
            }
            /*
             * Sanity check
             */
            if (segment_shared_with == (typeof(segment_shared_with)) DNX_KBP_INVALID_SEGMENT_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error peforming overlay sharing.\r\n");
            }
            /*
             * Sanity check
             */
            if (segment_position_on_master_key_bytes[segment_shared_with] < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error peforming overlay sharing, "
                             "sharing with a segment with no position.\r\n");
            }

            /*
             * Set the position to share with.
             */
            ms_key_segments_new[key_seg_idx].overlay_offset_bytes =
                segment_position_on_master_key_bytes[segment_shared_with] +
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx];
            /*
             * Save the information required for future hw configuration of this segment (fg_segment_info)
             * */
            SHR_IF_ERR_EXIT(dnx_field_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[0] = qual_idx;
            for (packed_qual_idx = 0; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                fg_segment_info.qual_idx[packed_qual_idx] = qual_idx - nof_quals_packing + packed_qual_idx + 1;
            }
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.set(unit, opcode_id, absolute_key_segs_idx, &fg_segment_info));
            /*
             * Increment only if new segment was created
             */
            key_seg_idx++;
            absolute_key_segs_idx++;
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    /*
     * Preparation for zero padding.
     */
    {
        /*
         * Get the list of opcodes.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_group
                        (unit, opcode_id, &nof_opcodes_in_group, opcodes_in_group, &opcode_predefined));
        /*
         * Sanity check. Verify that the FWD segments are the same.
         */
        {
            kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
            uint32 nof_segments;
            kbp_mngr_key_segment_t master_key_segments_iter[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
            uint32 nof_segments_iter;
            int fwd_seg_index;
            int opcode_index;
            uint8 nof_fwd_segments;
            uint8 nof_fwd_segments_iter;
            int opcode_found = FALSE;
            /** perform sal_memset to ensure the bits between the fields of the structure are zeroed for sal_cmp.*/
            sal_memset(&master_key_segments, 0x0,
                       sizeof(master_key_segments[0]) * DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            sal_memset(&master_key_segments_iter, 0x0,
                       sizeof(master_key_segments_iter[0]) * DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            for (opcode_index = 0; opcode_index < nof_opcodes_in_group; opcode_index++)
            {
                if (opcodes_in_group[opcode_index] == opcode_id)
                {
                    opcode_found = TRUE;
                }
                if (opcode_index == 0)
                {
                    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments
                                    (unit, opcodes_in_group[opcode_index], &nof_fwd_segments));
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                                    (unit, opcodes_in_group[opcode_index], &nof_segments, master_key_segments));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments
                                    (unit, opcodes_in_group[opcode_index], &nof_fwd_segments_iter));
                    if (nof_fwd_segments != nof_fwd_segments_iter)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_fwd_segments has different values for different "
                                     "opcodes within the group of %d. Values %d and %d.\r\n",
                                     opcode_id, nof_fwd_segments, nof_fwd_segments_iter);
                    }
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                                    (unit, opcodes_in_group[opcode_index], &nof_segments_iter,
                                     master_key_segments_iter));
                    for (fwd_seg_index = 0; fwd_seg_index < nof_fwd_segments; fwd_seg_index++)
                    {
                        if (sal_memcmp(&(master_key_segments_iter[fwd_seg_index]),
                                       &(master_key_segments[fwd_seg_index]), sizeof(master_key_segments[0])))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FWD segments are different for different "
                                         "opcodes within the group of %d. Segment %d.\r\n", opcode_id, fwd_seg_index);
                        }
                    }
                }
            }
            if (opcode_found == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d is is not a part of its own group.\n", opcode_id);
            }
        }
    }

    /*
     * Handle zero padding.
     */
    for (lookup_seg_idx = 0, qual_idx = nof_quals - 1, lookup_seg_idx_without_packing = 0;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        uint32 zero_padding_segment_filled;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        char new_segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES];
        int opcode_index;

        /** Sanity check */
        if (zero_padding_by_overlay_bytes_diff[lookup_seg_idx] < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative byte size difference (%d) between qualifier and zero padding.\r\n",
                         zero_padding_by_overlay_bytes_diff[lookup_seg_idx]);
        }

        zero_padding_segment_filled = in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx];

        /** Sanity check. */
        if ((zero_padding_by_overlay_bytes_diff[lookup_seg_idx] > 0) &&
            (zero_padding_segment_filled == DNX_KBP_INVALID_SEGMENT_ID))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment marked partial sharing of zero "
                         "padding filling, but is not zero padding filling.\r\n");
        }

        nof_quals_packing = 0;
        for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;
             qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
        {
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                if ((lookup_seg_idx_2 > lookup_seg_idx) && qual_packed_lookup_segment_last[lookup_seg_idx_2])
                {
                    /** If we are not at the first packed qual and we reached a new set of packed quals, stop.*/
                    break;
                }
                nof_quals_packing++;
            }
            else
            {
                break;
            }
        }

        if (zero_padding_segment_filled != DNX_KBP_INVALID_SEGMENT_ID)
        {
            if (nof_quals_packing > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment marked for zero padding filling, but is a packed segment.\r\n");
            }

            /*
             * Create the new segment.
             */
            sal_snprintf(new_segment_name, sizeof(new_segment_name),
                         "%.*s:%d-ZP", BCM_FIELD_MAX_SHORT_NAME_LEN,
                         dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type), fg_id);
            sal_snprintf(ms_key_segments_new[key_seg_idx].name, sizeof(ms_key_segments_new[key_seg_idx].name),
                         "%.*s:%d", BCM_FIELD_MAX_SHORT_NAME_LEN,
                         dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type), fg_id);

            /** Check for place for overlay segment. */
            if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                             opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            }
            /*
             * Change the name of the zero padding segment.
             * If the new qualifier does not cover the full segment, split the zero padding segment into two.
             */
            if (zero_padding_by_overlay_bytes_diff[lookup_seg_idx] == 0)
            {
                kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
                uint32 nof_segments;
                /** Just change the name.*/
                for (opcode_index = 0; opcode_index < nof_opcodes_in_group; opcode_index++)
                {
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get
                                    (unit, opcodes_in_group[opcode_index], &nof_segments, master_key_segments));
                    sal_strncpy_s(master_key_segments[zero_padding_segment_filled].name, new_segment_name,
                                  sizeof(master_key_segments[0].name));
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_set
                                    (unit, opcodes_in_group[opcode_index], nof_segments, master_key_segments));
                    LOG_DEBUG_EX(BSL_LOG_MODULE, "Changed zero padding segment %d to \"%s\" fg_id %d opcode_id %d.\n",
                                 zero_padding_segment_filled, new_segment_name, fg_id, opcodes_in_group[opcode_index]);
                }
            }
            else
            {
                kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
                uint32 nof_segments;
                kbp_mngr_key_segment_t padding_segment;
                kbp_mngr_key_segment_t new_segment;
                int lookup_seg_index2;
                /** Split into two.*/
                if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                                 opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
                }
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, master_key_segments));
                sal_memcpy(&padding_segment, &master_key_segments[zero_padding_segment_filled],
                           sizeof(padding_segment));
                sal_memcpy(&new_segment, &master_key_segments[zero_padding_segment_filled], sizeof(padding_segment));

                sal_strncpy_s(new_segment.name, new_segment_name, sizeof(master_key_segments[0].name));
                padding_segment.nof_bytes -= zero_padding_by_overlay_bytes_diff[lookup_seg_idx];
                new_segment.nof_bytes = qual_sizes_bytes[qual_idx];

                /** Sanity check.*/
                if ((padding_segment.nof_bytes + new_segment.nof_bytes) !=
                    master_key_segments[zero_padding_segment_filled].nof_bytes)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Splitting %d bytes into %d and %d.\r\n",
                                 master_key_segments[zero_padding_segment_filled].nof_bytes,
                                 padding_segment.nof_bytes, new_segment.nof_bytes);
                }

                /** Insert the new segments.*/
                for (opcode_index = 0; opcode_index < nof_opcodes_in_group; opcode_index++)
                {
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segment_remove
                                    (unit, opcodes_in_group[opcode_index], zero_padding_segment_filled));
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcodes_in_group[opcode_index], zero_padding_segment_filled, 1,
                                     &new_segment));
                    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                                    (unit, opcodes_in_group[opcode_index], zero_padding_segment_filled, 1,
                                     &padding_segment));
                }

                /*
                 * Update the arrays with the new values.
                 */
                for (lookup_seg_index2 = 0; lookup_seg_index2 < lookup_info.nof_segments; lookup_seg_index2++)
                {
                    if (lookup_info.key_segment_index[lookup_seg_index2] > zero_padding_segment_filled)
                    {
                        lookup_info.key_segment_index[lookup_seg_index2]++;
                    }
                }
                for (lookup_seg_index2 = 0; lookup_seg_index2 < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP;
                     lookup_seg_index2++)
                {
                    if (in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID &&
                        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] > zero_padding_segment_filled)
                    {
                        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx]++;
                    }
                }
                /*
                 * Update the zero paddign segment filled.
                 * The new segment is at the LSB of the zero padding (higher index)
                 */
                zero_padding_segment_filled++;
                /*
                 * Rewrite positions of segments.
                 */
                for (lookup_seg_index2 = zero_padding_segment_filled + 1;
                     lookup_seg_index2 < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; lookup_seg_index2++)
                {
                    segment_position_on_master_key_bytes[lookup_seg_index2] =
                        segment_position_on_master_key_bytes[lookup_seg_index2 - 1];
                }
                segment_position_on_master_key_bytes[zero_padding_segment_filled] =
                    segment_position_on_master_key_bytes[zero_padding_segment_filled - 1] + padding_segment.nof_bytes;

                /*
                 * Update the SW state.
                 */
                SHR_IF_ERR_EXIT(dnx_field_kbp_group_acl_split_zero_pad_update_sw_state
                                (unit, opcode_id, zero_padding_segment_filled,
                                 opcode_predefined, nof_opcodes_in_group, opcodes_in_group));

                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Split zero padding segment %d to \"%s\", remaining %d bytes. opcode_id %d.\n",
                             zero_padding_segment_filled, new_segment_name,
                             zero_padding_by_overlay_bytes_diff[lookup_seg_idx], opcode_id);
                /*
                 * Update the number of segments due to the split.
                 */
                absolute_key_segs_idx++;

            }
            /*
             * Mark the zero padding use in the SW state.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[unpacked_qual_ind] = qual_idx;
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.set(unit, opcode_id, zero_padding_segment_filled, &fg_segment_info));
            /*
             * Use overlay sharing.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[unpacked_qual_ind] = qual_idx;
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                            segment_info.set(unit, opcode_id, absolute_key_segs_idx, &fg_segment_info));
            ms_key_segments_new[key_seg_idx].nof_bytes = qual_sizes_bytes[qual_idx];
            ms_key_segments_new[key_seg_idx].is_overlay_field = TRUE;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = absolute_key_segs_idx;
            ms_key_segments_new[key_seg_idx].overlay_offset_bytes =
                segment_position_on_master_key_bytes[zero_padding_segment_filled];
            /*
             * Increment only if new segment was created
             */
            key_seg_idx++;
            absolute_key_segs_idx++;
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id %d to opcode_id %d nof added segments %d absolute_key_segs_idx %d including overlay.\n",
                 fg_id, opcode_id, key_seg_idx, absolute_key_segs_idx);

    /*
     * Update the Master Key with the newly constructed segments.
     * Corner-case: Even if no new segments were added for this fg,
     * we call this function as it just sets the sw-state on this stage.
     * Hw-configuration of the kbp will be done after the 'sync' call (see dnx_field_kbp_key_kbp_set()).
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, key_seg_idx, ms_key_segments_new));
    /*
     * Add the new Lookup to the KBP
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_kbp.h
 */
shr_error_e
dnx_field_kbp_group_segment_indices(
    int unit,
    dnx_field_group_t fg_id,
    uint32 opcode_id,
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    kbp_opcode_lookup_info_t lookup_info;
    dnx_field_attach_context_info_t context_info;
    int qual_idx;
    int seg_idx;
    int nof_quals;
    dnx_field_key_template_t key_template;
    uint8 qual_is_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_is_last_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_is_packed_with[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_within_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int nof_quals_packed;
    int nof_segments_packed;
    int first_qual_packing;
    int qual_packing_size;
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_qual;
    uint8 is_qual_packed;
    uint8 is_qual_packed_prev = FALSE;
    dnx_field_stage_e field_stage;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&lookup_info, 0x0, sizeof(lookup_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, opcode_id, &context_info));

    lookup_info.result_index = context_info.payload_id;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info));

    /*
     * Sanity check: count the number of qualifiers on the field group and verify that they are identical to the
     * number of segments on the KBP DB.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[qual_idx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++);
    nof_quals = qual_idx;
    if (nof_quals < lookup_info.nof_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d, opcode_id %d, has less qualifers (%d) than segments (%d).\n",
                     fg_id, opcode_id, nof_quals, lookup_info.nof_segments);
    }
    if (lookup_info.nof_segments > DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG ||
        lookup_info.nof_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of segments on database is %d, but maximum segments on database is %d, "
                     "and maximum qualifiers on field group %d. Field group %d, opcode_id %d.\n",
                     lookup_info.nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP,
                     DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG, fg_id, opcode_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Find packed quals
     */
    nof_quals_packed = 0;
    nof_segments_packed = 0;
    first_qual_packing = 0;
    qual_packing_size = 0;
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, DNX_FIELD_STAGE_EXTERNAL, key_template.key_qual_map[qual_idx].qual_type,
                         &dbal_field_qual));
        SHR_IF_ERR_EXIT(dbal_table_field_is_packed_get(unit, dbal_table_id, dbal_field_qual, &is_qual_packed));
        if (is_qual_packed_prev == FALSE)
        {
            first_qual_packing = qual_idx;
            qual_packing_size = 0;
        }
        if (is_qual_packed || is_qual_packed_prev)
        {
            qual_is_packed[qual_idx] = TRUE;
            qual_is_packed_with[qual_idx] = first_qual_packing;
            qual_offset_within_segment[qual_idx] = qual_packing_size;
            nof_quals_packed++;
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type, NULL, NULL, &qual_size));
            qual_packing_size += qual_size;
        }
        if (is_qual_packed == FALSE && is_qual_packed_prev)
        {
            qual_is_last_packed[qual_idx] = TRUE;
            nof_segments_packed++;
        }
        is_qual_packed_prev = is_qual_packed;
    }

    if ((nof_quals - nof_quals_packed) != (lookup_info.nof_segments - nof_segments_packed))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d, opcode_id %d, has less qualifers (%d,%d) than segments (%d,%d).\n",
                     fg_id, opcode_id, nof_quals, nof_quals_packed, lookup_info.nof_segments, nof_segments_packed);
    }

    /*
     * Swap the order of the indices, as the order of the qualifiers and he segments are inverted.
     */
    for (qual_idx = 0, seg_idx = lookup_info.nof_segments - 1; qual_idx < nof_quals; qual_idx++)
    {
        seg_index_on_master_by_qual[qual_idx] = lookup_info.key_segment_index[seg_idx];
        if (qual_is_packed[qual_idx])
        {
            qual_index_on_segment[qual_idx] = qual_idx - qual_is_packed_with[qual_idx];
            qual_offset_on_segment[qual_idx] = qual_offset_within_segment[qual_idx];
        }
        else
        {
            qual_index_on_segment[qual_idx] = 0;
            qual_offset_on_segment[qual_idx] = 0;
        }
        /** Progress seg_idx when we end packed qualifer bunch, or non packed qualifer.*/
        if (qual_is_packed[qual_idx] == FALSE || qual_is_last_packed[qual_idx])
        {
            seg_idx--;
        }
    }
    for (; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        seg_index_on_master_by_qual[qual_idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds FFCs for the filling of zero padding segments.
 *
 * \param [in] unit          - Device Id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_kbp_group_fwd_zero_padding_fill(
    int unit)
{
    uint32 predef_opcode;
    uint32 opcode_user;
    dnx_field_kbp_opcode_info_t kbp_opcode_info;
    bcm_field_AppType_t predef_apptype;
    int nof_user_defiend_apptypes;
    bcm_field_AppType_t user_app_types[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF];
    uint32 user_opcodes[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF];
    int opcode_idx;
    int nof_user_defiend_opcodes_used;
    int seg_idx;
    uint8 segment_is_zero_filled[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF +
                                 1][DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    dnx_field_context_t contexts_to_share_ffc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int nof_context_to_share_ffc;
    int context_idx;
    uint8 is_valid;
    int shared_ffc_id;
    dnx_field_kbp_segment_info_t segment_info;
    uint8 nof_fwd_segments;
    int segemnt_is_zero_padded;
    int predefined_opcode_used;
    uint32 opcode_that_uses_zero_padding = DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_KBP_OPCODE_ITERATOR(predef_opcode)
    {
        DNX_FIELD_KBP_OPCODE_NOT_IN_USE_CONTINUE(unit, predef_opcode);
        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.is_valid.get(unit, predef_opcode, &is_valid));
        /** Check if opcode is used by ACL.*/
        if (is_valid == FALSE)
        {
            predefined_opcode_used = FALSE;
        }
        else
        {
            predefined_opcode_used = TRUE;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, predef_opcode, &predef_apptype));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_to_child_apptypes
                        (unit, predef_apptype, &nof_user_defiend_apptypes, user_app_types));
        /** Sanity check.*/
        if (nof_user_defiend_apptypes > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_user_defiend_apptypes %d, is bigget than maximum %d.\n",
                         nof_user_defiend_apptypes, DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF);
        }
        nof_user_defiend_opcodes_used = 0;
        for (opcode_idx = 0; opcode_idx < nof_user_defiend_apptypes; opcode_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, user_app_types[opcode_idx], &opcode_user));
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.is_valid.get(unit, opcode_user, &is_valid));
            if (is_valid)
            {
                user_opcodes[nof_user_defiend_opcodes_used] = opcode_user;
                nof_user_defiend_opcodes_used++;
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "User defined apptype %d opcode %d was created but no field groups attached to it."
                             "%s%s\n", user_app_types[opcode_idx], opcode_user, EMPTY, EMPTY);
            }
        }
        /*
         * We now have all of the user defined opcodes being used based on the predefined opcode.
         * Go over each qualifier to see if it fills a zero padded segment.
         */
        for (opcode_idx = 0; opcode_idx < DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_APPTYPE_USER_NOF + 1; opcode_idx++)
        {
            for (seg_idx = 0; seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; seg_idx++)
            {
                segment_is_zero_filled[opcode_idx][seg_idx] = FALSE;
            }
        }
        SHR_IF_ERR_EXIT(dnx_field_kbp_group_opcode_nof_fwd_segments(unit, predef_opcode, &nof_fwd_segments));
        for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
        {
            uint32 opcode_id_curr;
            if (opcode_idx == 0)
            {
                opcode_id_curr = predef_opcode;
                if (predefined_opcode_used == FALSE)
                {
                    continue;
                }
            }
            else
            {
                opcode_id_curr = user_opcodes[opcode_idx - 1];
            }
            SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.get(unit, opcode_id_curr, &kbp_opcode_info));
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id_curr, &nof_segments, ms_key_segments));
            for (seg_idx = 0; seg_idx < nof_fwd_segments; seg_idx++)
            {
                if (kbp_opcode_info.master_key_info.segment_info[seg_idx].fg_id != DNX_FIELD_GROUP_INVALID)
                {
                    segment_is_zero_filled[opcode_idx][seg_idx] = TRUE;
                    if (ms_key_segments[seg_idx].is_overlay_field)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d is FWD segment (nof_fwd_segemntd %d) "
                                     "but it uses overlay sharing (offset %d bytes on master key).\n",
                                     opcode_id_curr, seg_idx, nof_fwd_segments,
                                     ms_key_segments[seg_idx].overlay_offset_bytes);
                    }
                }
            }
        }
        /*
         * Now for each segment find an FFC shared by all opcodes, and for each opcode, allocate the FFCs.
         */
        for (seg_idx = 0; seg_idx < nof_fwd_segments; seg_idx++)
        {
            /*
             * Find an FFC used by all contexts that use the zero padding.
             */
            segemnt_is_zero_padded = FALSE;
            nof_context_to_share_ffc = 0;
            for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
            {
                uint32 opcode_id_curr;
                int nof_fwd_contexts;
                dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

                if (opcode_idx == 0)
                {
                    opcode_id_curr = predef_opcode;
                }
                else
                {
                    opcode_id_curr = user_opcodes[opcode_idx - 1];
                }

                SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts
                                (unit, opcode_id_curr, &nof_fwd_contexts, fwd_contexts, acl_contexts));
                for (context_idx = 0; context_idx < nof_fwd_contexts; context_idx++)
                {
                    if (nof_context_to_share_ffc >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Number of contexts that need to share an FFC exceeds maximum %d.\n",
                                     DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);
                    }
                    if ((nof_context_to_share_ffc > 0) && (context_idx > 0) &&
                        (acl_contexts[context_idx] == contexts_to_share_ffc[nof_context_to_share_ffc - 1]))
                    {
                        continue;
                    }
                    contexts_to_share_ffc[nof_context_to_share_ffc] = acl_contexts[context_idx];
                    nof_context_to_share_ffc++;
                    if (segment_is_zero_filled[opcode_idx][seg_idx])
                    {
                        segemnt_is_zero_padded = TRUE;
                        opcode_that_uses_zero_padding = opcode_id_curr;
                    }
                }
            }
            /*
             * Sanity check: verify that no context appears twice.
             */
            for (context_idx = 0; context_idx < nof_context_to_share_ffc; context_idx++)
            {
                int context_idx_2;
                for (context_idx_2 = 0; context_idx_2 < context_idx; context_idx_2++)
                {
                    if (contexts_to_share_ffc[context_idx] == contexts_to_share_ffc[context_idx_2])
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Predefined opcode %d segment %d, context %d appears twice.\n",
                                     predef_opcode, seg_idx, contexts_to_share_ffc[context_idx]);
                    }
                }
            }

            /*
             * Allocate a shared FFC and configure it for each context, whether is uses zero padding or not.
             */
            if (segemnt_is_zero_padded)
            {
                /** Initialization to appease the compiler. */
                shared_ffc_id = -1;

                rv = dnx_field_kbp_key_find_common_available_ffc(unit, DNX_FIELD_STAGE_EXTERNAL,
                                                                 contexts_to_share_ffc, nof_context_to_share_ffc,
                                                                 &shared_ffc_id);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "Predefined opcode %d segment %d, cannot find FFC shared by all contexts to "
                                 "fill zero padding.\n", predef_opcode, seg_idx);
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv);
                }
                /** Sanity check.*/
                if (shared_ffc_id == -1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "shared_ffc_id found but not assigned.\n");
                }
                /*
                 * Allocate FFCs for each opcode.
                 */
                for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
                {
                    uint32 opcode_id_curr;

                    if (opcode_idx == 0)
                    {
                        opcode_id_curr = predef_opcode;
                    }
                    else
                    {
                        opcode_id_curr = user_opcodes[opcode_idx - 1];
                    }
                    if (segment_is_zero_filled[opcode_idx][seg_idx])
                    {
                        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                                        segment_info.get(unit, opcode_id_curr, seg_idx, &segment_info));
                        SHR_IF_ERR_EXIT(dnx_field_kbp_key_kbp_fwd_zero_padding_fill
                                        (unit, opcode_id_curr, seg_idx, &segment_info, TRUE, opcode_id_curr,
                                         shared_ffc_id));
                    }
                    else
                    {
                        /*
                         * If the opcode doesn't need zero padding, configure the FFC by another opcode that does
                         * use zero padding.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                                        segment_info.get(unit, opcode_that_uses_zero_padding, seg_idx, &segment_info));
                        SHR_IF_ERR_EXIT(dnx_field_kbp_key_kbp_fwd_zero_padding_fill
                                        (unit, opcode_id_curr, seg_idx, &segment_info, FALSE,
                                         opcode_that_uses_zero_padding, shared_ffc_id));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_group_set(
    int unit)
{
    uint32 opcode_id;
    uint8 is_valid;
    dnx_field_kbp_opcode_info_t kbp_opcode_info;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    uint8 context_used[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM] = { 0 };
    unsigned int fwd_context_idx;
    uint8 acl_context_is_user_defined;
    int device_locked;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_kbp_is_device_locked(unit, &device_locked));
    if (device_locked)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Device lock cannot happen more than once.\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_group_fwd_zero_padding_fill(unit));

    for (opcode_id = 0; opcode_id < DNX_KBP_MAX_NOF_OPCODES; opcode_id++)
    {
        uint8 context_used_for_opcode[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_ACL_CONTEXT_NUM] = { 0 };
        /*
         * Check if opcode is used by ACL.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.is_valid.get(unit, opcode_id, &is_valid));
        if (is_valid == FALSE)
        {
            continue;
        }

        /*
         * Get the kbp information for this opcode. kbp info includes the qualifier type
         * and the qualifier attach info for all segments in the master key.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.get(unit, opcode_id, &kbp_opcode_info));
        /*
         * Get Attached ACL (field iFWD2) contexts to this opcode.
         */
        SHR_IF_ERR_EXIT(dnx_field_kbp_opcode_to_contexts
                        (unit, opcode_id, &nof_fwd_contexts, fwd_contexts, acl_contexts));
        for (fwd_context_idx = 0; fwd_context_idx < nof_fwd_contexts; fwd_context_idx++)
        {
            if (context_used_for_opcode[acl_contexts[fwd_context_idx]])
            {
                SHR_IF_ERR_EXIT(kbp_mngr_context_acl_is_dynamic
                                (unit, acl_contexts[fwd_context_idx], &acl_context_is_user_defined));
                if (acl_context_is_user_defined)
                {
                    continue;
                }
                else
                {
                    /** Sanity check*/
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "ACL context %d is being used by opcode_id %d more than once, "
                                 "even though it is a user define context.\n", acl_contexts[fwd_context_idx],
                                 opcode_id);
                }
            }
            if (context_used[acl_contexts[fwd_context_idx]])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ACL context %d is being used by more than one opcode_id, including %d.\n",
                             acl_contexts[fwd_context_idx], opcode_id);
            }
            /*
             * Install the master key (opcode) for given context.
             */
            SHR_IF_ERR_EXIT(dnx_field_kbp_key_kbp_set(unit, opcode_id,
                                                      fwd_contexts[fwd_context_idx], acl_contexts[fwd_context_idx],
                                                      &kbp_opcode_info));
            context_used[acl_contexts[fwd_context_idx]] = TRUE;
            context_used_for_opcode[acl_contexts[fwd_context_idx]] = TRUE;

            LOG_DEBUG_EX(BSL_LOG_MODULE, "Set opcode_id %d on context %d.%s%s\n", opcode_id,
                         acl_contexts[fwd_context_idx], EMPTY, EMPTY);
        }
    }

    /*
     * Sync (Lock) the External Tcam (kbp) device
     */
    SHR_IF_ERR_EXIT(kbp_mngr_sync(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_opcode_to_contexts(
    int unit,
    uint32 opcode_id,
    int *nof_contexts_p,
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE],
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE])
{
    uint8 fwd_nof_contexts;
    kbp_mngr_fwd_acl_context_mapping_t
        fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    unsigned int fwd_context_idx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_contexts_p, _SHR_E_PARAM, "nof_contexts_p");
    SHR_NULL_CHECK(fwd_contexts, _SHR_E_PARAM, "fwd_contexts");
    SHR_NULL_CHECK(acl_contexts, _SHR_E_PARAM, "acl_contexts");

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
    if (fwd_nof_contexts > DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FWD contexts for pcoode %d is %d, maximum is %d.\n",
                     opcode_id, fwd_nof_contexts, DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE);
    }
    for (fwd_context_idx = 0; fwd_context_idx < fwd_nof_contexts; fwd_context_idx++)
    {
        fwd_contexts[fwd_context_idx] = fwd_acl_ctx_mapping[fwd_context_idx].fwd_context;
        if (fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, FWD context %d, the number of ACL contexts is %d."
                         "Only one ACL context per FWD context is supported.\n",
                         opcode_id, fwd_acl_ctx_mapping[fwd_context_idx].fwd_context,
                         fwd_acl_ctx_mapping[fwd_context_idx].nof_acl_contexts);
        }
        acl_contexts[fwd_context_idx] = fwd_acl_ctx_mapping[fwd_context_idx].acl_contexts[0];
    }
    (*nof_contexts_p) = fwd_nof_contexts;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify parameters of dnx_field_context_apptype_create()
*
* \param [in] unit            - Device ID
* \param [in] flags           - Flags for context creation (WITH_ID)
* \param [in] apptype_info_p  - The information for creating the apptype
* \param [in] apptype_p       - The apptype
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_kbp_context_apptype_create_verify(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p)
{
    uint32 base_opcode_id;
    uint32 opcode_id;
    uint8 is_alloc;
    int device_locked;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(apptype_info_p, _SHR_E_PARAM, "apptype_info_p");
    SHR_NULL_CHECK(apptype_p, _SHR_E_PARAM, "apptype_p");

    /*
     * Verify that the device has a KBP
     */
    if (dnx_kbp_device_enabled(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring user defined Apptypes (contexts in the external stage) requires a "
                     "KBP device.\n");
    }
    /*
     * Verify that a device lock hasn't happened yet.
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_is_device_locked(unit, &device_locked));
    if (device_locked)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configuring user defined Apptypes (contexts in the external stage) cannot "
                     "happen after device lock.\n");
    }

    /*
     * Verify that the source predefined Apptype is legal.
     */
    if (apptype_info_p->base_static_apptype < 0 || apptype_info_p->base_static_apptype >= bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When creating a context for for stage external, we are actually creating an apptype. "
                     "The apptype must be based on a predefined apptype with value no more than %d. "
                     "Value for base Apptype is (in cascaded from) %d.\r\n",
                     bcmFieldAppTypeCount - 1, apptype_info_p->base_static_apptype);
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                             (unit, apptype_info_p->base_static_apptype, &base_opcode_id),
                             "When creating a context for for stage external, we are actually creating an apptype. "
                             "The apptype must be based on a predefined apptype."
                             "Value for base Apptype is (in cascaded from) %d.%s%s\r\n",
                             apptype_info_p->base_static_apptype, EMPTY, EMPTY);

    if ((flags & (~DNX_FIELD_CONTEXT_FLAG_WITH_ID)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags (0x%x) includes unknown flags.\r\n", flags);
    }

    /*
     * Verify the Apptype value if given WITH_ID, to provide a clearer error message than resource manager.
     */
    if (flags & DNX_FIELD_CONTEXT_FLAG_WITH_ID)
    {
        if (*apptype_p < dnx_data_field.external_tcam.apptype_user_1st_get(unit) ||
            *apptype_p >=
            dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
            dnx_data_field.external_tcam.apptype_user_nof_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When creating a context for for stage external, we are actually creating an apptype. "
                         "User created apptypes must be within the range %d to %d. Requested Apptype %d with a "
                         "WITH_ID flag.\r\n",
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit),
                         dnx_data_field.external_tcam.apptype_user_1st_get(unit) +
                         dnx_data_field.external_tcam.apptype_user_nof_get(unit) - 1, *apptype_p);
        }
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, *apptype_p, &is_alloc));
        if (is_alloc)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When creating a context for for stage external, we are actually creating an apptype. "
                         "Requested Apptype %d with a WITH_ID flag, but the apptype already exists.\r\n", *apptype_p);
        }
    }

    /** Avoid usage of predefined Apptypes names for user defined Apptypes. */
    DNX_FIELD_KBP_OPCODE_ITERATOR(opcode_id)
    {
        bcm_field_AppType_t app_type_internal;
        SHR_IF_ERR_EXIT(dnx_field_map_static_opcode_to_apptype_get(unit, opcode_id, &app_type_internal));
        if (sal_strncmp
            (apptype_info_p->name, dnx_field_bcm_apptype_text(unit, app_type_internal),
             sizeof(apptype_info_p->name)) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Requested Apptype name %s is equal to one of the predefined Apptype names %s."
                         "They must be different! \r\n", apptype_info_p->name,
                         dnx_field_bcm_apptype_text(unit, app_type_internal));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_context_apptype_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p)
{
    int alloc_flags = 0;
    uint32 opcode_id;
    uint32 base_static_opcode_id;
    int apptype_index;
    uint8 acl_context;
    uint8 opcode_uint8;
    int cs_profile_id;
    uint8 fwd_nof_contexts;
    kbp_mngr_fwd_acl_context_mapping_t
        fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    uint8 base_fwd_context;
    dnx_field_apptype_flags_e apptype_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & DNX_FIELD_APPTYPE_FLAG_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        apptype_flags |= DNX_FIELD_APPTYPE_FLAG_WITH_ID;
    }

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_kbp_context_apptype_create_verify(unit, flags, apptype_info_p, apptype_p));

    /** Allocate new Apptye*/
    SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_allocate(unit, alloc_flags, apptype_p));

    apptype_index = (*apptype_p) - dnx_data_field.external_tcam.apptype_user_1st_get(unit);

    /** Get source static opcode.*/
    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx
                    (unit, apptype_info_p->base_static_apptype, &base_static_opcode_id));

    /** Allocate Opcode and ACL context.*/
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_create
                    (unit, base_static_opcode_id, apptype_info_p->name, &acl_context, &opcode_uint8));
    opcode_id = opcode_uint8;

    /** Allocate CS profile ID.*/
    SHR_IF_ERR_EXIT(dnx_algo_field_external_tcam_opcode_cs_profile_id_allocate(unit, 0, &cs_profile_id));
    /** Set the CS profile ID.*/
    SHR_IF_ERR_EXIT(dnx_field_context_apptype_acl_cs_profile_set(unit, acl_context, cs_profile_id));

    /*
     * Mark the FFCs used.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
    base_fwd_context = fwd_acl_ctx_mapping[0].fwd_context;
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_acl_ffc_update_for_new_context
                    (unit, base_static_opcode_id, base_fwd_context, acl_context));

    /**
     * If the there is no given name to the context, we shouldn't set it to SW state.
     * To avoid sw state journal issues.
     */
    if (sal_strncmp(apptype_info_p->name, "", sizeof(apptype_info_p->name)))
    {
        /** Set the name of the apptype to the SW state */
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.name.stringncpy(unit, apptype_index, apptype_info_p->name));
    }

    /** Set the rest of the information to SW state.*/
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.flags.set(unit, apptype_index, apptype_flags));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.
                    base_apptype.set(unit, apptype_index, apptype_info_p->base_static_apptype));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.acl_context.set(unit, apptype_index, acl_context));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.opcode_id.set(unit, apptype_index, opcode_id));
    SHR_IF_ERR_EXIT(dnx_field_apptype_sw.user_def_info.profile_id.set(unit, apptype_index, cs_profile_id));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_is_device_locked(
    int unit,
    int *device_locked_p)
{
    int is_device_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(device_locked_p, _SHR_E_PARAM, "device_locked_p");

    SHR_IF_ERR_EXIT(KBP_ACCESS.is_device_locked.get(unit, &is_device_locked));
    if (is_device_locked)
    {
        (*device_locked_p) = TRUE;
    }
    else
    {
        (*device_locked_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_is_kbp_device_available(
    int unit,
    int *kbp_device_exist_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(kbp_device_exist_p, _SHR_E_PARAM, "kbp_device_exist_p");

    (*kbp_device_exist_p) = dnx_kbp_device_enabled(unit);

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_contexts_to_opcode_get(
    int unit,
    uint32 fwd_context,
    uint32 acl_context,
    uint32 *opcode_id_p)
{
    uint8 opcode_id_uint8;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_get(unit, fwd_context, acl_context, &opcode_id_uint8, NULL));
    *opcode_id_p = opcode_id_uint8;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   This API creates KBP dynamic table.
*   \param [in]  unit          - Relevant unit.
*   \param [in]  lpm_instead_of_tcam - If TRUE, create LPM instead of TCAM.
*   \param [in]  counters_enable - enable counters for ACL db.
*   \param [in]  dbal_table_id - the related table ID.
*   \param [in]  min_capacity_valid - True when min_capacity has a meaningful info, otherwise default values are set.
*   \param [in]  min_capacity - Set the minimum capacity reserved in EXTERNAL TCAM. Used if capacity_valid is true.
*   \param [in]  algorithmic_mode_valid - Whether to set the property ALGORITHMIC for algorithmic mode
*   \param [in]  algorithmic_mode - Set the algorithmic mode (algorithmic property) to this value. Used if algorithmic_mode_valid is true.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
static shr_error_e
dnx_field_kbp_db_create(
    int unit,
    uint8 lpm_instead_of_tcam,
    uint8 counters_enable,
    dbal_tables_e dbal_table_id,
    uint8 min_capacity_valid,
    uint32 min_capacity,
    uint8 algorithmic_mode_valid,
    uint32 algorithmic_mode)
{
    uint32 initial_capacity;
    int algo_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (min_capacity_valid)
    {
        if (min_capacity == DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Value %d is blocked for min_capacity.\r\n",
                         DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION);
        }
        initial_capacity = min_capacity;
    }
    else
    {
        initial_capacity = DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION;
    }
    if (algorithmic_mode_valid)
    {
        if (algorithmic_mode == DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Value %d is blocked for min_capacity.\r\n",
                         DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION);
        }
        algo_mode = algorithmic_mode;
    }
    else
    {
        algo_mode = DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_db_create
                    (unit, lpm_instead_of_tcam, counters_enable, dbal_table_id, initial_capacity, algo_mode));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_db_properties_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *counters_enable_p,
    uint8 *min_capacity_valid_p,
    uint32 *min_capacity_p,
    uint8 *algorithmic_mode_valid_p,
    uint32 *algorithmic_mode_p,
    uint8 *lpm_instead_of_tcam_p)
{
    dbal_tables_e dbal_table_id;
    uint32 initial_capacity;
    int algo_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(kbp_mngr_db_properties_get
                    (unit, dbal_table_id, counters_enable_p, &initial_capacity, &algo_mode, lpm_instead_of_tcam_p));
    if (initial_capacity == DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION)
    {
        (*min_capacity_valid_p) = FALSE;
    }
    else
    {
        (*min_capacity_valid_p) = TRUE;
        (*min_capacity_p) = initial_capacity;
    }
    if (algo_mode == DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION)
    {
        (*algorithmic_mode_valid_p) = FALSE;
    }
    else
    {
        (*algorithmic_mode_valid_p) = TRUE;
        (*algorithmic_mode_p) = algo_mode;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_db_is_lpm_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *lpm_instead_of_tcam_p)
{
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(kbp_mngr_db_properties_get(unit, dbal_table_id, NULL, NULL, NULL, lpm_instead_of_tcam_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_group_cache(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_group_cache_mode_e mode)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    switch (mode)
    {
        case DNX_FIELD_GROUP_CACHE_MODE_START:
        {
            /** Set cache mode to Start */
            SHR_IF_ERR_EXIT(dnx_kbp_cache_db_enable(unit, dbal_table_id));
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                            cache_mode.set(unit, fg_id, DNX_FIELD_GROUP_CACHE_MODE_START));
            break;
        }
        case DNX_FIELD_GROUP_CACHE_MODE_INSTALL:
        {
            /** Set cache mode to Clear and set the cached entries to 0 */
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                            cache_mode.set(unit, fg_id, DNX_FIELD_GROUP_CACHE_MODE_CLEAR));
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.set(unit, fg_id, 0));
            /** Commit cached entries */
            SHR_IF_ERR_EXIT(dnx_kbp_cache_db_commit(unit, dbal_table_id));
            break;
        }
        case DNX_FIELD_GROUP_CACHE_MODE_CLEAR:
        {
            int nof_entries_to_clear;
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                            nof_valid_entries.get(unit, fg_id, &nof_entries_to_clear));
            /** Set cache mode to Clear and set the cached entries to 0 */
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                            cache_mode.set(unit, fg_id, DNX_FIELD_GROUP_CACHE_MODE_CLEAR));
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.set(unit, fg_id, 0));
            /** Clear cached entries */
            SHR_IF_ERR_EXIT(dnx_kbp_cache_db_clear(unit, dbal_table_id, (uint32) nof_entries_to_clear));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cache mode: %d not supported\r\n", mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_payload_size_verify(
    int unit,
    int payload_size_bits)
{
    uint32 legal_payload_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, payload_size_bits, &legal_payload_size));
    /** Verify that the payload size is byte aligned.*/
    if (((payload_size_bits) % SAL_UINT8_NOF_BITS) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal payload size %d, must be byte aligned. Pad to %d.\r\n",
                     payload_size_bits, ((((payload_size_bits) / (SAL_UINT8_NOF_BITS)) + 1)) * SAL_UINT8_NOF_BITS);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h */
shr_error_e
dnx_field_kbp_context_ifwd2_kbp_acl_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Perform keys (ffc/quad) related initializations
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_acl_ffc_init(unit));
    /*
     * Perform verifications for keys/ffc/quad for proper ACL setup
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_key_ifwd2_acl_key_verify(unit));
    /*
     * Initialize the SW state for master key
     */
    SHR_IF_ERR_EXIT(dnx_field_kbp_group_master_key_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_kbp.h
 */
shr_error_e
dnx_field_kbp_group_opcode_nof_fwd_segments(
    int unit,
    uint32 opcode_id,
    uint8 *nof_fwd_segments_p)
{
    uint32 opcode_predefined;
    bcm_field_AppType_t apptype_user;
    bcm_field_AppType_t apptype_pre;
    uint8 is_user_defined;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &is_user_defined));
    if (is_user_defined)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype_user));
        SHR_IF_ERR_EXIT(dnx_field_context_user_apptype_to_predef_apptype(unit, apptype_user, &apptype_pre));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype_pre, &opcode_predefined));
    }
    else
    {
        opcode_predefined = opcode_id;
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_sw.opcode_info.master_key_info.
                    nof_fwd_segments.get(unit, opcode_predefined, nof_fwd_segments_p));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_group_external_tcam_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_fg_info_for_sw_t * fg_info_p,
    dnx_field_group_external_tcam_info_t * external_tcam_info_p,
    unsigned int actions_size_in_bits)
{
    uint8 counters_enable;
    uint8 lpm_instead_of_tcam;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Call KBP Manager to create the ACL database (cased on the dbal) in the KBP device
     */
    if (fg_info_p->flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS)
    {
        counters_enable = TRUE;
    }
    else
    {
        counters_enable = FALSE;
    }

    if (external_tcam_info_p->flags & BCM_FIELD_GROUP_EXTERNAL_FLAG_LPM)
    {
        lpm_instead_of_tcam = TRUE;
        if (external_tcam_info_p->algorithmic_mode_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set LPM and algorithmic mode together.\r\n");
        }
    }
    else
    {
        lpm_instead_of_tcam = FALSE;
    }

    SHR_IF_ERR_EXIT(dnx_field_kbp_db_create(unit, lpm_instead_of_tcam, counters_enable,
                                            fg_info_p->dbal_table_id,
                                            external_tcam_info_p->min_capacity_valid,
                                            external_tcam_info_p->min_capacity,
                                            external_tcam_info_p->algorithmic_mode_valid,
                                            external_tcam_info_p->algorithmic_mode));

exit:
    SHR_FUNC_EXIT;
}

/** See field_kbp.h*/
shr_error_e
dnx_field_kbp_tcam_external_swstate_init(
    int unit)
{
    int kbp_device_exists;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_is_external_tcam_available(unit, &kbp_device_exists));
    if (kbp_device_exists)
    {
        int nof_opcodes_to_alloc =
            MIN(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts,
                DNX_KBP_MAX_NOF_OPCODES);

        SHR_IF_ERR_EXIT(dnx_field_kbp_sw.init(unit));

        dnx_field_kbp_sw.opcode_info.alloc(unit, nof_opcodes_to_alloc);
    }

exit:
    SHR_FUNC_EXIT;
}

#else /* defined(INCLUDE_KBP) */
#include <soc/sand/shrextend/shrextend_debug.h>
#endif
