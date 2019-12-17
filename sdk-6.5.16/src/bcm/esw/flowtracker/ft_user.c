/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_user.c
 * Purpose:     Routines managing user inserted flows in FT
 * Requires:
 */


#include <bcm_int/esw/flowtracker/ft_user.h>
#include <bcm_int/esw/flowtracker/ft_group.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#define BCMI_FTK_DMA_CHUNK_SIZE 4096

/* Internal structure to traverse memory */
typedef struct bcmi_ftk_user_entry_traverse_data_s {
    bcm_flowtracker_group_t flow_group_id;
    uint32 flow_key_mode;
    uint32 flow_key_type;
    int handle_idx;
    int num_user_entry_handles;
    int actual_entries;
    bcm_flowtracker_user_entry_handle_t *user_entry_handle_list;
} bcmi_ftk_user_entry_traverse_data_t;


/* Generic memory allocation routine. */
#define FTK_ALLOC(_ptr_, _size_, _descr_)                       \
            do {                                                \
                int _lsize_;                                    \
                _lsize_ = (_size_ );                            \
                if (NULL == (_ptr_)) {                          \
                   (_ptr_) = sal_alloc((_lsize_), (_descr_));   \
                }                                               \
                if((_ptr_) != NULL) {                           \
                    sal_memset((_ptr_), 0, (_lsize_));          \
                }  else {                                       \
                    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,           \
                              (BSL_META("FTK Error: Allocation failure %s\n"), (_descr_))); \
                }                                               \
            } while (0)


/* Generic memory free routine. */
#define FTK_FREE(_ptr_)                                         \
            do {                                                \
                if (NULL != (_ptr_)) {                          \
                   sal_free((_ptr_));                           \
                }                                               \
                (_ptr_) = NULL;                                 \
            } while (0)

#define FTK_CPY(_to_, _from_, _size_)                            \
            do {                                                 \
                sal_memcpy((_to_), (_from_), (_size_));          \
            } while (0)


#define FTK_PRINT_ARRAY(_unit, _string_, _nbytes_, _arr_ptr_)                                            \
            do {                                                                                         \
                int ii;                                                                                  \
                LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%s "), (_string_)));                \
                for (ii = 0; ii < _nbytes_; ii++) {                                                      \
                    if ((ii + 1) != _nbytes_) {                                                          \
                        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%02x "), _arr_ptr_[ii]));   \
                    } else {                                                                             \
                        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%02x\n"), _arr_ptr_[ii]));  \
                    }                                                                                    \
                }                                                                                        \
            } while (0)

#define FTK_PRINT_ARRAY_U32(_unit, _string_, _nwords_, _arr_ptr_)                                        \
            do {                                                                                         \
                int ii;                                                                                  \
                LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%s "), (_string_)));                \
                for (ii = 0; ii < _nwords_; ii++) {                                                      \
                    if ((ii + 1) != _nwords_) {                                                          \
                        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%08x "), _arr_ptr_[ii]));   \
                    } else {                                                                             \
                        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(_unit, "%08x\n"), _arr_ptr_[ii]));  \
                    }                                                                                    \
                }                                                                                        \
            } while (0)

/* FT group Internal methods. */
typedef int (*bcmi_ftk_mem_entry_traverse_cb)(int unit,
                soc_mem_t mem, uint32 index, void *entry,
                void *user_data, int *fin);

int bcmi_ftk_mem_traverse(int unit, soc_mem_t mem,
                bcmi_ftk_mem_entry_traverse_cb cb,
                void *user_data);

/*
 * Function:
 *     bcmi_ft_user_cleanup
 * Purpose:
 *     Clear Flowtracker user entries states.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
void
bcmi_ft_user_cleanup(int unit)
{
    return;
}

/*
 * Function:
 *     bcmi_ft_user_init
 * Purpose:
 *     Initialize Flowtracker user state.
 * Parameters:
 *     unit - (IN) BCM device id
 *
 * Returns:
 *     None.
 */
int
bcmi_ft_user_init(int unit)
{
    /* No structure to initialize */
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_param_check
 * Purpose:
 *     Verify user tracking param info.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     num_user_entry_params - (IN) Number of elements in user_entry_param_list
 *     user_entry_param_list - (IN) User tracking param info.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_param_check(
    int unit,
    bcm_flowtracker_group_t group,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list)
{
    int ix, jx;
    int num_params;
    bcm_flowtracker_tracking_param_info_t *t1, *t2;

    num_params = BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, group);
    for (ix = 0; ix < num_params; ix++) {
        t1 = &(BCMI_FT_GROUP_TRACK_PARAM(unit, group)[ix]);

        /* skip checking for non-key tracking params */
        if (!t1->flags & BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            continue;
        }
        for (jx = 0; jx < num_user_entry_params; jx++) {
            t2 = &(user_entry_param_list[jx]);
            if (t2->param == t1->param) {
                break;
            }
        }

        /* key param is not found in user params */
        if (jx == num_user_entry_params) {
            return BCM_E_PARAM;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_key_info_normalize
 * Purpose:
 *     Normalize user tracking param info pair for bidir flows.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     norm_tracking_params_info - (IN) List of normalization tracking params.
 *     num_user_entry_params - (IN) Number of elements in user_entry_param_list
 *     user_entry_param_list - (IN) User tracking param info.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_key_info_normalize(
    int unit,
    bcm_flowtracker_group_t group,
    bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list)
{
    int ii = 0;
    int srcdst = 0;
    int width = 0, num_keys = 0;
    bcmi_ft_group_alu_info_t *key_ext_info = NULL;
    bcm_flowtracker_tracking_param_info_t *t1 = NULL;
    bcm_flowtracker_tracking_param_info_t *t2 = NULL;
    bcm_flowtracker_tracking_param_type_t temp_param;

    key_ext_info = BCMI_FT_GROUP_EXT_KEY_INFO(unit, group);
    num_keys = BCMI_FT_GROUP_EXT_INFO(unit, group).num_key_info;

    /* Find src and dst tp info in user_param_list */
    for (ii = 0, srcdst = 0;
            (ii < num_user_entry_params) && (srcdst < 2); ii++) {
        if (user_entry_param_list[ii].param ==
                norm_tracking_params_info[0]->param) {
            t1 = &user_entry_param_list[ii];
            srcdst++;
        } else if (user_entry_param_list[ii].param ==
                norm_tracking_params_info[1]->param) {
            t2 = &user_entry_param_list[ii];
            srcdst++;
        }
    }
    if ((t1 == NULL) || (t2 == NULL)) {
        return BCM_E_PARAM;
    }

    /* Find key extractor info */
    for (ii = 0; ii < num_keys; ii++) {
        if (t1->param == key_ext_info[ii].element_type1) {
            width = key_ext_info[ii].key1.length;
        } else if (t2->param == key_ext_info[ii].element_type1) {
            width = key_ext_info[ii].key1.length;
        }

        if (width != 0) {
            break;
        }
    }

    /* Swap, if required  */
    for (ii = 0; ii < width; ii++) {
        if (t1->tracking_data[ii] == t2->tracking_data[ii]) {
            continue;
        }

        if (t1->tracking_data[ii] < t2->tracking_data[ii]) {
            temp_param = t2->param;
            t2->param = t1->param;
            t1->param = temp_param;
            break;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_key_normalize
 * Purpose:
 *     Normalize user tracking param info for bidir flows.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     num_user_entry_params - (IN) Number of elements in user_entry_param_list
 *     user_entry_param_list - (IN) User tracking param info.
 *
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     When both tracking param in the pair pare present,
 *     then we can normalize it otherwise skip.
 */
STATIC int
bcmi_ftk_user_entry_key_normalize(
    int unit,
    bcm_flowtracker_group_t group,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    bcm_flowtracker_tracking_param_info_t
        *norm_tracking_params_info[BCMI_FT_GROUP_NORM_TRACKING_PARAMS] = {NULL};

    rv = bcmi_ft_group_norm_tracking_params_info_get(unit,
            group, &norm_tracking_params_info[0]);
    BCM_IF_ERROR_RETURN(rv);

    /* Normalize if both params are present */
    for (idx = 0; idx < BCMI_FT_GROUP_NORM_TRACKING_PARAMS; idx += 2) {
        if ((norm_tracking_params_info[idx] != NULL) &&
                (norm_tracking_params_info[idx + 1] != NULL)) {
            rv = bcmi_ftk_user_entry_key_info_normalize(unit,
                    group, &norm_tracking_params_info[idx],
                    num_user_entry_params, user_entry_param_list);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_data_fill
 * Purpose:
 *     Fill mem entry for session data.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     ftd_mem - (IN) Session Data memory.
 *     ftd_entry - (OUT) Session data entry buffer.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_data_fill(
    int unit,
    bcm_flowtracker_group_t group,
    soc_mem_t ftd_mem,
    uint32 *ftd_entry)
{
    int ii;
    int rv;
    int profile_idx;
    int offset, length;
    uint16 init16_data = 0;
    uint32 init32_data = 0;
    int ftd_width, ftd_words;
    uint8 *ft_data = NULL, *ft_tmp;
    uint32 *ftd_fld = NULL;
    uint8 ftd_val[6], *ftd_val_tmp;
    bcmi_ft_group_template_list_t *ftg_template = NULL;

    soc_format_t fmt;
    bsc_dg_group_table_entry_t dg_entry;
    uint32 profile_data[2], profile_field[2];

    /* First read the group entry. */
    rv = soc_mem_read(unit, BSC_DG_GROUP_TABLEm,
                      MEM_BLOCK_ANY, group, &dg_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Allocate memory for ft_data */
    ftd_width = BITS2BYTES(soc_mem_field_length(unit, ftd_mem, OPAQUE_DATAf));
    ftd_words = BYTES2WORDS(ftd_width);

    FTK_ALLOC(ftd_fld, ftd_width, "ftd field");
    FTK_ALLOC(ft_data, ftd_width, "ft data");


    ftg_template = ((BCMI_FT_GROUP(unit, group))->template_head);
    for (; (ftg_template != NULL); ftg_template = ftg_template->next) {

        offset = (ftg_template->info.cont_offset);
        length = (ftg_template->info.cont_width);
        if (ftg_template->info.data_shift != 0) {
            length -= ftg_template->info.data_shift;
        }

        ftd_val_tmp = ftd_val;

        switch(ftg_template->info.param_type) {
            case bcmiFtGroupParamTypeMeter:

                fmt = BSC_TL_DG_TO_DT_METER_CONTROL_BUSfmt;

                profile_idx = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                                                  ((uint32 *)&dg_entry),
                                                  GROUP_METER_PROFILE_IDXf);
                rv = soc_mem_read(unit, BSC_DG_GROUP_METER_PROFILEm,
                                  MEM_BLOCK_ANY, profile_idx, profile_data);
                BCMI_CLEANUP_IF_ERROR(rv);

                soc_mem_field_get(unit, BSC_DG_GROUP_METER_PROFILEm,
                                  profile_data, DATAf, profile_field);

                init32_data = soc_format_field32_get(unit, fmt,
                                                   profile_field, REFRESHCOUNTf);

                _SHR_PACK_U16(ftd_val_tmp, init16_data);
                _SHR_PACK_U16(ftd_val_tmp, (init32_data >> 16));
                _SHR_PACK_U16(ftd_val_tmp, (init32_data & 0xFFFF));

                FTK_CPY(&ft_data[BITS2BYTES(offset)], ftd_val, BITS2BYTES(length));
                break;

            case bcmiFtGroupParamTypeCollectorCopy:

                fmt = BSC_TL_DG_TO_DT_COPY_TO_COLLECTOR_CONTROL_BUSfmt;

                profile_idx = soc_mem_field32_get(unit, BSC_DG_GROUP_TABLEm,
                                                  ((uint32 *)&dg_entry),
                                                  COPY_TO_COLLECTOR_PROFILE_IDXf);
                rv = soc_mem_read(unit, BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
                                  MEM_BLOCK_ANY, profile_idx, profile_data);
                BCMI_CLEANUP_IF_ERROR(rv);

                soc_mem_field_get(unit, BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILEm,
                                  profile_data, DATAf, profile_field);


                if (soc_format_field32_get(unit, fmt, profile_field,
                                           INITIAL_COPY_COUNT_ENABLEf)) {

                    init16_data = soc_format_field32_get(unit, fmt,
                                                       profile_field, MAX_COUNTf);

                    soc_mem_field32_set(unit, ftd_mem, ftd_entry,
                                        INITIAL_COPY_COLLECTOR_PHASEf, 1);
                }

                _SHR_PACK_U16(ftd_val_tmp, init16_data);
                FTK_CPY(&ft_data[BITS2BYTES(offset)], ftd_val, BITS2BYTES(length));

                break;

            default:
                break;
        }


        if (ftg_template->info.flags & BCMI_FT_ALU_LOAD_NEXT_ATTACH) {
            /* This is surely at least second last. so we can blindly go next.*/
            ftg_template = ftg_template->next;
        }
    }

    ft_tmp = ft_data;

    for (ii = 0; ii < (ftd_words); ii++) {
        _SHR_UNPACK_U32(ft_tmp, ftd_fld[(ftd_words - ii - 1)]);
    }
    FTK_PRINT_ARRAY(unit, "FTD_OPAQUE_FIELD: ", (ftd_words), ftd_fld);


    FTK_PRINT_ARRAY_U32(unit, "FTD_OPAQUE_FIELD: ", (ftd_words), ftd_fld);


    /* Set OPAQUE_DATA in the session table. */
    soc_mem_field_set(unit, ftd_mem, ftd_entry, OPAQUE_DATAf, ftd_fld);
    FTK_PRINT_ARRAY_U32(unit, "FTD_ENTRY: ", soc_mem_entry_words(unit, ftd_mem), ftd_entry);

cleanup:
    FTK_FREE(ftd_fld);
    FTK_FREE(ft_data);

    return rv;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_key_fill
 * Purpose:
 *     Fill mem entry for session key
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     num_user_entry_params - (IN) Numer of elements in user_entry_param_list.
 *     user_entry_param_list - (IN) User entry tracking param data info.
 *     key_type - (IN) Session Key Type.
 *     key_mode - (IN) Session Key Mode.
 *     ftk_width - (IN) Width of Session key entry.
 *     ft_key - (OUT) Pointer to Session Key entry buffer.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_key_fill(
    int unit,
    bcm_flowtracker_group_t group,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    uint32 key_type,
    uint32 key_mode,
    int ftk_width,
    uint8 *ft_key)
{
    int ix, jx;
    uint8 *ele_data;
    bcmi_ft_alu_key_t *key_alu;
    int key_offset, keytype_offset;
    int start, length, num_key_els;
    bcmi_ft_group_alu_info_t *key_ext_info;
    bcm_flowtracker_tracking_param_info_t *t1 = NULL;

    key_ext_info = BCMI_FT_GROUP_EXT_KEY_INFO(unit, group);
    num_key_els = BCMI_FT_GROUP_EXT_INFO(unit, group).num_key_info;


    /* Ensure num_params is same as num key elements */
    if (num_user_entry_params != num_key_els) {
        return BCM_E_PARAM;
    }

    /* 1st Byte: KeyType + Mode */
    ft_key[(ftk_width - 1)] = (((key_type & 0x7F) << 1) | (key_mode & 0x1));
    keytype_offset = 8;

    ftk_width = BYTES2BITS(ftk_width);

    for (ix = 0; ix < num_key_els; ix++) {
        key_alu = &(key_ext_info->key1);
        start = key_alu->location;
        length = key_alu->length;

        for (jx = 0; jx < num_user_entry_params; jx++) {
            t1 = &(user_entry_param_list[jx]);

            if (key_ext_info->element_type1 == t1->param) {
                break;
            }
        }


        /* tracking param not found */
        if (jx == num_user_entry_params) {
            return BCM_E_PARAM;
        }
        key_offset = (ftk_width - (keytype_offset + start + length));

        ele_data = &(t1->tracking_data[0]);
        FTK_CPY(&ft_key[BITS2BYTES(key_offset)], ele_data, BITS2BYTES(length));

        LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "element type: %d start = %d len = %d\n"), t1->param, start, length));
        FTK_PRINT_ARRAY(unit, "Element:", BITS2BYTES(length), t1->tracking_data);

        key_ext_info++;
    }



    FTK_PRINT_ARRAY(unit, "Full Key:", (ftk_width/8), ft_key);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_group_sw_entries_check
 * Purpose:
 *     Verify if user entries allowed on given flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_group_sw_entries_check(int unit, bcm_flowtracker_group_t group)
{
    BCM_IF_ERROR_RETURN(bcmi_ft_group_is_invalid(unit, group));

    if (!BCMI_FT_GROUP_IS_VALIDATED(unit,group)) {
        return BCM_E_CONFIG;
    }

    /* Make sure that the flow group is a SW-only group. */
    if (!BCMI_FT_GROUP(unit, group)->flags &
             BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_group_learn_counter_update
 * Purpose:
 *     Update learn counter for given flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     group - (IN) Flowtracker Group.
 *     incr - (IN) Change.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_group_learn_counter_update(
    int unit,
    bcm_flowtracker_group_t id,
    int incr)
{
    uint64 val;

    COMPILER_64_ZERO(val);

    /* Read Learn Counter */
    BCM_IF_ERROR_RETURN(soc_counter_get(unit, REG_PORT_ANY,
         SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT, id, &val));

    /* Update Learn counter */
    if (incr == 1) {
        COMPILER_64_ADD_32(val, 1);
    } else {
        COMPILER_64_SUB_32(val, 1);
    }

    /* Write Learn counter */
    BCM_IF_ERROR_RETURN(soc_counter_set(unit, REG_PORT_ANY,
       SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_LEARNT_CNT, id, val));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ftk_user_entry_add
 * Purpose:
 *      Add a user flow entry basis user input key elements. API
 *      expects that all tracking parametrs of type = 'KEY' in the
 *      group are specified.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      options - (IN) Options for creting the flowtracker user entry.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN) List of user entry params to add flow entry.
 *      entry_handle - (OUT) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ftk_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int ii;
    int memsize;
    int ft_index, ag_index;
    uint32 key_type, key_mode;
    int dnt_entry_created = 0;
    int ftk_single_bits = 0, ftk_single_bytes = 0;
    int ftk_single_words = 0, ftk_width = 0;
    uint8 *ft_key = NULL, *ft_key_tmp;
    uint32 *ftk_fld = NULL;
    uint32 *ftd_entry = NULL;
    uint32 *ftk_entry = NULL;
    uint32 *ft_key_buf = NULL;
    bsc_ag_age_table_entry_t ag_entry;
    do_not_ft_entry_entry_t *dnt_entry = NULL;
    bcm_flowtracker_group_t hw_group_id;
    bcmi_ft_group_ftfp_data_t ftfp_data;
    soc_mem_t dnt_mem, ftk_mem, ftd_mem, ag_mem;
    bcmi_ft_group_key_data_mode_t k_mode, d_mode;

    int rv = BCM_E_NONE;

    if (num_user_entry_params < 0) {
        return BCM_E_PARAM;
    }

    if (entry_handle == NULL) {
        return BCM_E_PARAM;
    }
    dnt_mem = DO_NOT_FT_ENTRYm;

    /* Make sure group is valid */
    BCM_IF_ERROR_RETURN(bcmi_ftk_group_sw_entries_check(unit, flow_group_id));

    /* The list of user info elements in the entry_add API
     * match with the tracking params of TYPE=?KEY? in the
     * flow group. if there is a mismatch, return error.
     */
     rv = bcmi_ftk_user_entry_param_check(unit, flow_group_id,
                                          num_user_entry_params, user_entry_param_list);
     BCM_IF_ERROR_RETURN(rv);


     rv = bcmi_ft_group_extraction_mode_get(unit, 0, flow_group_id, &k_mode);
     BCM_IF_ERROR_RETURN(rv);

     rv = bcmi_ft_group_extraction_mode_get(unit, 1, flow_group_id, &d_mode);
     BCM_IF_ERROR_RETURN(rv);


    ftk_single_bits = soc_mem_field_length(unit, dnt_mem, KEY_0f);
    ftk_single_bytes = BITS2BYTES(ftk_single_bits);
    ftk_single_words = BYTES2WORDS(ftk_single_bytes);

    /* Allocate memory for ft_key */
    ftk_width = (ftk_single_bytes * 2);
    FTK_ALLOC(ft_key, ftk_width, "ft key");
    FTK_ALLOC(ftk_fld, ftk_width, "ft field");
    FTK_ALLOC(ft_key_buf, ftk_width, "ft key buffer");

    rv = bcmi_ft_group_ftfp_data_get(unit, flow_group_id, &ftfp_data);
    BCMI_CLEANUP_IF_ERROR(rv);

    key_type = ftfp_data.session_key_type;
    key_mode = ftfp_data.session_key_mode;

    /* Normalize key user param fields */
    if (ftfp_data.direction == bcmFlowtrackerFlowDirectionBidirectional) {
        rv = bcmi_ftk_user_entry_key_normalize(unit, flow_group_id,
                                           num_user_entry_params,
                                           user_entry_param_list);
        BCMI_CLEANUP_IF_ERROR(rv);
    }

    /* Prepare key table entry using key extraction info
     * from the group data structures.
     */
    rv = bcmi_ftk_user_entry_key_fill(unit, flow_group_id,
                                      num_user_entry_params,
                                      user_entry_param_list,
                                      key_type, key_mode,
                                      ftk_width, ft_key);
    BCMI_CLEANUP_IF_ERROR(rv);



    /* Install the key entry in the one-entry ?DO_NOT_FT_ENTRY?
     * table and set actions as DO_NOT_LEARN = DO_NOT_FT = 1.
     */
    memsize = soc_mem_entry_bytes(unit, dnt_mem);
    FTK_ALLOC(dnt_entry, memsize, "DONOTFT Mem Entry");

    if (k_mode == bcmiFtGroupModeSingle) {
        ftk_mem = FT_KEY_SINGLEm;
    } else {
        ftk_mem = FT_KEY_DOUBLEm;
    }
    ag_mem = BSC_AG_AGE_TABLEm;

    memsize = soc_mem_entry_bytes(unit, ftk_mem);
    FTK_ALLOC(ftk_entry, memsize, "FT Key Mem Entry");

    ft_key_tmp = ft_key;

    for (ii = 0; ii < BYTES2WORDS(ftk_width); ii++) {
        _SHR_UNPACK_U32(ft_key_tmp, ftk_fld[(BYTES2WORDS(ftk_width) - ii - 1)]);
    }

    if (k_mode == bcmiFtGroupModeSingle) {

        /* Single Wide Key */

        SHR_BITCOPY_RANGE(ft_key_buf, 0, ftk_fld, 0, ftk_single_bits);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD0 : ", ftk_single_words, ft_key_buf);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_0f, ft_key_buf);
        soc_mem_field_set(unit, ftk_mem, (uint32 *)ftk_entry, KEYf, ft_key_buf);

    } else {

        /* Double Wide Key */

        SHR_BITCOPY_RANGE(ft_key_buf, 0, ftk_fld, 0, ftk_single_bits);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD0 : ", ftk_single_words, ft_key_buf);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_0f, ft_key_buf);
        soc_mem_field_set(unit, ftk_mem, (uint32 *)ftk_entry, KEY_0f, ft_key_buf);

        SHR_BITCOPY_RANGE(ft_key_buf, 0, ftk_fld, ftk_single_bits, ftk_single_bits);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD1 : ", ftk_single_words, ft_key_buf);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_1f, ft_key_buf);
        soc_mem_field_set(unit, ftk_mem, (uint32 *)ftk_entry, KEY_1f, ft_key_buf);
    }

    if (k_mode == bcmiFtGroupModeDouble) {

        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_1f, 2);
        soc_mem_field32_set(unit, ftk_mem, (uint32 *)ftk_entry, BASE_VALID_1f, 2);

        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_0f, 1);
        soc_mem_field32_set(unit, ftk_mem, (uint32 *)ftk_entry, BASE_VALID_0f, 1);
    } else {

        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_0f, 1);
        soc_mem_field32_set(unit, ftk_mem, (uint32 *)ftk_entry, BASE_VALIDf, 1);
    }


    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_TYPEf, key_mode);
    soc_mem_field32_set(unit, ftk_mem, (uint32 *)ftk_entry, KEY_TYPEf, key_mode);

    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, DO_NOT_LEARNf, 1);
    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, DO_NOT_FTf, 1);

    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, VALIDf, 1);

    rv = soc_mem_write(unit, dnt_mem, MEM_BLOCK_ALL, 0, dnt_entry);
    BCMI_CLEANUP_IF_ERROR(rv);
    dnt_entry_created = 1;

    rv = soc_mem_search(unit, ftk_mem, MEM_BLOCK_ALL, &ft_index,
            ftk_entry, ftk_entry, 0);
    if (BCM_SUCCESS(rv)) {
        ag_index = (d_mode == bcmiFtGroupModeDouble) ?
            (ft_index * 2) : (ft_index);
        sal_memset(&ag_entry, 0, sizeof (bsc_ag_age_table_entry_t));
        rv = soc_mem_read(unit, ag_mem, MEM_BLOCK_ANY, ag_index, &ag_entry);
        BCMI_CLEANUP_IF_ERROR(rv);

        hw_group_id = soc_mem_field32_get(unit, ag_mem, &ag_entry, GROUP_IDf);
        LOG_DEBUG(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "Key parameters for"
                        " user entry exists for group=%d. \n"), hw_group_id));
        rv = BCM_E_EXISTS;
        BCMI_CLEANUP_IF_ERROR(rv);
    }

    /* Insert the entry into FT_KEY_SINGLE or FT_KEY_DOUBLE
     * based on the group?s ft_key_mode.
     */
    rv = soc_mem_generic_insert(unit, ftk_mem, MEM_BLOCK_ALL, 0, ftk_entry, NULL, &ft_index);

    /* If the insert fails, clean up the single entry and
     * return the error code to user.
     */
    BCMI_CLEANUP_IF_ERROR(rv);
    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "FT insert successful: ft index = %d\n"), ft_index));


    /* If insert succeeds, use the index from the insert
     * operation to populate the BSC_DT_FLEX_SESSION_DATA_SINGLE
     * or BSC_DT_FLEX_SESSION_DATA_DOUBLE based in the group?s
     * ft_data_mode.
     * The actual offset for each ALU engine can be derived
     * from the PDD bitmap (alternately, this info is already
     * available in the group?s template info).
     */
    if (d_mode == bcmiFtGroupModeSingle) {
        ftd_mem = BSC_DT_FLEX_SESSION_DATA_SINGLEm;
        ag_index = ft_index;
    } else {
        ftd_mem = BSC_DT_FLEX_SESSION_DATA_DOUBLEm;
        ag_index = ft_index * 2;
    }

    memsize = soc_mem_entry_bytes(unit, ftd_mem);
    FTK_ALLOC(ftd_entry, memsize, "FT DATA Mem Entry");

    /* Pre-load data table entry for non-zero elements.
     */
    rv = bcmi_ftk_user_entry_data_fill(unit, flow_group_id,
                                       ftd_mem, ftd_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* set VALID=1 in the SESSION_TABLE */
    soc_mem_field32_set(unit, ftd_mem, ftd_entry, VALIDf, 1);

    /* Write DATA table entry. */
    rv = soc_mem_write(unit, ftd_mem, MEM_BLOCK_ALL, ft_index, ftd_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    FTK_PRINT_ARRAY_U32(unit, "FTD_ENTRY: ", BYTES2WORDS(memsize), ftd_entry);

    /* If insert succeeds, use the index from the insert operation
     * to populate GROUP_ID in BSC_AG_AGE_TABLE and mark ENTRY_VALID=1.
     */
    sal_memset(&ag_entry, 0, sizeof (bsc_ag_age_table_entry_t));
    soc_mem_field32_set(unit, ag_mem, &ag_entry, GROUP_IDf, flow_group_id);
    soc_mem_field32_set(unit, ag_mem, &ag_entry, ENTRY_VALIDf, 1);
    rv = soc_mem_write(unit, ag_mem, MEM_BLOCK_ALL, ag_index, &ag_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Delete the one-entry DO_NOT_FT_ENTRY to allow
     * flowtracking for the user installed entry.
     */
    soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_LEARNf, 0);
    soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_FTf, 0);

    soc_mem_field32_set(unit, dnt_mem, dnt_entry, VALIDf, 0);
    rv = soc_mem_write(unit, dnt_mem, MEM_BLOCK_ALL, 0, dnt_entry);
    BCMI_CLEANUP_IF_ERROR(rv);
    dnt_entry_created = 0;

    /* Populate output entry_handle */
    entry_handle->flow_key_mode = k_mode;
    entry_handle->flow_key_type = key_type;
    entry_handle->flow_index = ft_index;
    entry_handle->flow_group_id = flow_group_id;

    /* Update Learn counter */
    bcmi_ftk_group_learn_counter_update(unit, flow_group_id, 1);

cleanup:
    if (dnt_entry_created == 1) {
        soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_LEARNf, 0);
        soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_FTf, 0);

        soc_mem_field32_set(unit, dnt_mem, dnt_entry, VALIDf, 0);
        (void)soc_mem_write(unit, dnt_mem, MEM_BLOCK_ALL, 0, dnt_entry);
    }

    FTK_FREE(dnt_entry);
    FTK_FREE(ft_key);
    FTK_FREE(ftk_fld);
    FTK_FREE(ft_key_buf);
    FTK_FREE(ftk_entry);
    FTK_FREE(ftd_entry);

    return rv;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_handle_validate
 * Purpose:
 *     Validate given entry handle.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     entry_hdl - (IN) Entry handle.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_handle_validate(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_hdl)
{
    int rv;
    int ag_index;
    soc_mem_t mem;
    bsc_ag_age_table_entry_t entry;
    bcmi_ft_group_key_data_mode_t k_mode;

    if (entry_hdl == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ftk_group_sw_entries_check(unit, entry_hdl->flow_group_id);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_group_extraction_mode_get(unit,
            1, entry_hdl->flow_group_id, &k_mode);
    BCM_IF_ERROR_RETURN(rv);

    if (k_mode == bcmiFtGroupModeDouble) {
        ag_index = entry_hdl->flow_index * 2;
    } else {
        ag_index = entry_hdl->flow_index;
    }

    mem = BSC_AG_AGE_TABLEm;
    sal_memset(&entry, 0, sizeof (bsc_ag_age_table_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ag_index, &entry);
    BCM_IF_ERROR_RETURN(rv);

    if(FALSE == soc_mem_field32_get(unit, mem, &entry, ENTRY_VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    if(entry_hdl->flow_group_id !=
            soc_mem_field32_get(unit, mem, &entry, GROUP_IDf)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_ftk_user_entry_get
 * Purpose:
 *      Fetch user entry info given the entry handle.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry_handle - (IN) Flowtracker user entry handle.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN/OUT) List of user entry params added in the flow entry.
 *      actual_user_entry_params - (OUT) Actual number of params in the user entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ftk_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params)
{
    int ii;
    uint8 *ele_data;
    bcmi_ft_alu_key_t *key_alu;
    int start, length, num_key_els;
    int actual_params = 0;
    int key_offset, keytype_offset;

    int memsize;
    int ft_index;
    int ftk_single_bits = 0;
    int ftk_single_bytes, ftk_width;
    int ftk_single_words;
    uint8 *ft_key = NULL, *ft_key_tmp;
    uint32 *ftk_fld = NULL;
    uint32 *ftk_entry = NULL;
    uint32 *ft_key_buf = NULL;
    soc_mem_t ftk_mem;
    bcmi_ft_group_key_data_mode_t k_mode;
    bcmi_ft_group_alu_info_t *key_ext_info;
    bcm_flowtracker_tracking_param_info_t *track_param;
    bcm_flowtracker_group_t flow_group_id;

    int rv = BCM_E_NONE;

    if (entry_handle == NULL) {
        return BCM_E_PARAM;
    }
    if (actual_user_entry_params == NULL) {
        return BCM_E_PARAM;
    }
    if ((num_user_entry_params > 0) && (user_entry_param_list == NULL)) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ftk_user_entry_handle_validate(unit, entry_handle);
    BCM_IF_ERROR_RETURN(rv);


    ft_index = entry_handle->flow_index;
    k_mode =  entry_handle->flow_key_mode;
    flow_group_id = entry_handle->flow_group_id;

    /* Make sure group is valid */
    BCM_IF_ERROR_RETURN(bcmi_ftk_group_sw_entries_check(unit, flow_group_id));

    if (k_mode == bcmiFtGroupModeSingle) {
        ftk_mem = FT_KEY_SINGLEm;
    } else {
        ftk_mem = FT_KEY_DOUBLEm;
    }

    memsize = soc_mem_entry_bytes(unit, ftk_mem);
    FTK_ALLOC(ftk_entry, memsize, "FT Key Mem Entry");

    rv = soc_mem_read(unit, ftk_mem, MEM_BLOCK_ANY, ft_index, ftk_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    ftk_single_bits = soc_mem_field_length(unit, DO_NOT_FT_ENTRYm, KEY_0f);
    ftk_single_bytes = BITS2BYTES(ftk_single_bits);
    ftk_single_words = BYTES2WORDS(ftk_single_bytes);

    /* Allocate memory for ft_key */
    ftk_width = (ftk_single_bytes * 2);
    FTK_ALLOC(ft_key, ftk_width, "ft key");
    FTK_ALLOC(ftk_fld, ftk_width, "ft field");
    FTK_ALLOC(ft_key_buf, ftk_width, "ft key buffer");

    ft_key_tmp = ft_key;

    if (k_mode == bcmiFtGroupModeSingle) {
        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEYf, ft_key_buf);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD0: ", ftk_single_words, ft_key_buf);
        SHR_BITCOPY_RANGE(ftk_fld, 0, ft_key_buf, 0, ftk_single_bits);
    } else {

        /* High part of the Key */
        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEY_1f, ft_key_buf);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD1: ", ftk_single_words, ft_key_buf);
        SHR_BITCOPY_RANGE(ftk_fld, ftk_single_bits, ft_key_buf, 0, ftk_single_bits);

        /* Low part of the Key */
        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEY_0f, ft_key_buf);
        FTK_PRINT_ARRAY_U32(unit, "FTK_FIELD0: ", ftk_single_words, ft_key_buf);
        SHR_BITCOPY_RANGE(ftk_fld, 0, ft_key_buf, 0, ftk_single_bits);
    }
    for (ii = 0; ii < BYTES2WORDS(ftk_width); ii++) {
        _SHR_PACK_U32(ft_key_tmp, ftk_fld[(BYTES2WORDS(ftk_width) - ii - 1)]);
    }
    FTK_PRINT_ARRAY(unit, "Full Key: ", ftk_width, ft_key);

    key_ext_info = BCMI_FT_GROUP_EXT_KEY_INFO(unit, flow_group_id);
    num_key_els = BCMI_FT_GROUP_EXT_INFO(unit, flow_group_id).num_key_info;

    keytype_offset = 8;
    ftk_width = BYTES2BITS(ftk_width);

    for (ii = 0; ii < num_key_els; ii++) {
        key_alu = &(key_ext_info->key1);
        start = key_alu->location;
        length = key_alu->length;

        if (actual_params < num_user_entry_params) {
            track_param = &(user_entry_param_list[actual_params]);
            ele_data = &(track_param->tracking_data[0]);

            key_offset = (ftk_width - (keytype_offset + start + length));
            track_param->param = key_ext_info->element_type1;
            FTK_CPY(ele_data, &ft_key[BITS2BYTES(key_offset)], BITS2BYTES(length));

            FTK_PRINT_ARRAY(unit, "Element:", BITS2BYTES(length), track_param->tracking_data);
        }


        actual_params++;
        key_ext_info++;
    }

    *actual_user_entry_params = actual_params;

cleanup:

    FTK_FREE(ft_key);
    FTK_FREE(ftk_fld);
    FTK_FREE(ftk_entry);
    FTK_FREE(ft_key_buf);

    return rv;
}

/*
 * Function:
 *     bcmi_ftk_user_entry_handle_from_table
 * Purpose:
 *     Create user entry handle if given index in
 *     ag age table is valid.
 * Parameters:
 *     unit - (IN) BCM device id.
 *     mem  - (IN) Memory.
 *     index - (IN) Current index.
 *     entry - (IN) Entry buffer
 *     user_data - (IN/OUT) User data
 *     finish - (OUT) Indicate that no need to traverse more
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_ftk_user_entry_handle_from_table(int unit, soc_mem_t mem,
                    uint32 index, void *entry, void *user_data,
                    int *finish)
{
    int handle_idx;
    int num_user_entry_handles;
    bcm_flowtracker_group_t flow_group_id;
    bcmi_ftk_user_entry_traverse_data_t *data = NULL;
    bcm_flowtracker_user_entry_handle_t *entry_handle = NULL;

    if (!soc_mem_field32_get(unit, mem, entry, ENTRY_VALIDf)) {
        return BCM_E_NONE;
    }

    data = (bcmi_ftk_user_entry_traverse_data_t *) user_data;

    flow_group_id = soc_mem_field32_get(unit, mem, entry, GROUP_IDf);
    if (flow_group_id != data->flow_group_id) {
        return BCM_E_NONE;
    }

    handle_idx = data->handle_idx;
    num_user_entry_handles = data->num_user_entry_handles;

    if (handle_idx < num_user_entry_handles) {
        entry_handle = &(data->user_entry_handle_list[handle_idx]);

        entry_handle->flow_key_mode = data->flow_key_mode;
        entry_handle->flow_key_type = data->flow_key_type;
        entry_handle->flow_group_id = data->flow_group_id;

        if (data->flow_key_mode == bcmiFtGroupModeDouble) {
            entry_handle->flow_index = index / 2;
        } else {
            entry_handle->flow_index = index;
        }
        handle_idx++;
    }

    data->handle_idx = handle_idx;
    data->actual_entries++;

    if((num_user_entry_handles != 0) &&
        (data->actual_entries == num_user_entry_handles)) {
        *finish = TRUE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ftk_mem_traverse
 * Purpose:
 *     Traverse function for calling callback per
 *     entry in given table..
 * Parameters:
 *     unit - (IN) BCM device id.
 *     mem  - (IN) Memory.
 *     cb   - (IN) Callback function.
 *     user_data - (IN/OUT) User data
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ftk_mem_traverse(int unit, soc_mem_t mem,
            bcmi_ftk_mem_entry_traverse_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    int fin = FALSE;
    int8 *buffer = NULL;
    void *entry = NULL;
    uint32 idx = 0, e_idx = 0;
    int mem_min = 0, mem_max = 0;
    uint32 chunk_size = 0, mem_entry_size = 0;

    mem_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);

    chunk_size = ((mem_max - mem_min + 1) > BCMI_FTK_DMA_CHUNK_SIZE) ?
                BCMI_FTK_DMA_CHUNK_SIZE : (mem_max - mem_min + 1);
    mem_entry_size = SOC_MEM_WORDS(unit, mem) * sizeof(uint32);

    buffer = soc_cm_salloc(unit, mem_entry_size * chunk_size, "mem buffer");
    if (buffer == NULL)  {
        return BCM_E_MEMORY;
    }
    for (idx = mem_min; ((idx <= mem_max) && (!fin)); idx += chunk_size) {
        chunk_size = ((mem_max - idx + 1) > BCMI_FTK_DMA_CHUNK_SIZE) ?
                    BCMI_FTK_DMA_CHUNK_SIZE : (mem_max - idx + 1);

        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                    idx, idx + chunk_size - 1, buffer)) != SOC_E_NONE) {
            break;
        }
        for (e_idx = 0; ((e_idx < chunk_size) && (!fin)); e_idx++) {
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                            buffer, e_idx);
            rv = (*cb)(unit, mem, idx + e_idx, entry, user_data, &fin);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_ftk_user_entry_get_all
 * Purpose:
 *      Fetch all user entries added in a against a given flow group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      num_user_entry_handles - (IN) Number of export elements intended in the tempate.
 *      user_entry_handle_list - (IN/OUT) List of user entry handles corresponding to the user entries added.
 *      actual_user_entry_handles - (OUT) Actual number of user entry handles corresponding to the entries added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ftk_user_entry_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_user_entry_handles,
    bcm_flowtracker_user_entry_handle_t *user_entry_handle_list,
    int *actual_user_entry_handles)
{
    int rv = BCM_E_NONE;
    bcmi_ftk_user_entry_traverse_data_t data;
    bcmi_ft_group_ftfp_data_t ftfp_data;

    BCM_IF_ERROR_RETURN(
        bcmi_ftk_group_sw_entries_check(unit, flow_group_id));

    rv = bcmi_ft_group_ftfp_data_get(unit, flow_group_id, &ftfp_data);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&data, 0, sizeof(bcmi_ftk_user_entry_traverse_data_t));

    data.flow_group_id = flow_group_id;
    data.flow_key_mode = ftfp_data.session_key_mode;
    data.flow_key_type = ftfp_data.session_key_type;
    data.num_user_entry_handles = num_user_entry_handles;
    data.handle_idx = 0;
    data.actual_entries = 0;
    data.user_entry_handle_list = user_entry_handle_list;

    rv = bcmi_ftk_mem_traverse(unit, BSC_AG_AGE_TABLEm,
            bcmi_ftk_user_entry_handle_from_table, (void *) &data);
    BCM_IF_ERROR_RETURN(rv);

    *actual_user_entry_handles = data.actual_entries;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_ftk_user_entry_delete
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      entry_handle - (IN) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ftk_user_entry_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int rv = BCM_E_NONE;
    int memsize;
    int ft_index, ag_index;
    uint32 key_type;
    int dnt_entry_created = 0;
    int ftk_single_bytes;
    uint32 *ftk_fld = NULL;
    uint32 *ftk_entry = NULL;
    soc_mem_t dnt_mem, ftk_mem, ftd_mem, ag_mem;
    do_not_ft_entry_entry_t *dnt_entry = NULL;
    bcmi_ft_group_key_data_mode_t k_mode, d_mode;

    if (entry_handle == NULL) {
        return BCM_E_PARAM;
    }

    /* Make sure group is valid */
    rv = bcmi_ftk_group_sw_entries_check(unit, flow_group_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Validate entry handle */
    rv = bcmi_ftk_user_entry_handle_validate(unit, entry_handle);
    BCM_IF_ERROR_RETURN(rv);

    dnt_mem = DO_NOT_FT_ENTRYm;
    ag_mem = BSC_AG_AGE_TABLEm;

    ft_index = entry_handle->flow_index;
    d_mode = k_mode =  entry_handle->flow_key_mode;
    key_type = entry_handle->flow_key_type;

    if (k_mode == bcmiFtGroupModeSingle) {
        ftk_mem = FT_KEY_SINGLEm;
    } else {
        ftk_mem = FT_KEY_DOUBLEm;
    }

    /* Install the key entry in the one-entry ?DO_NOT_FT_ENTRY?
     * table and set actions as DO_NOT_LEARN = DO_NOT_FT = 1.
     */
    ftk_single_bytes = soc_mem_field_length(unit, dnt_mem, KEY_0f);
    ftk_single_bytes = BITS2BYTES(ftk_single_bytes);

    FTK_ALLOC(ftk_fld, ftk_single_bytes, "ft field");

    memsize = soc_mem_entry_bytes(unit, dnt_mem);
    FTK_ALLOC(dnt_entry, memsize, "DONOTFT Mem Entry");


    memsize = soc_mem_entry_bytes(unit, ftk_mem);
    FTK_ALLOC(ftk_entry, memsize, "FT Key Mem Entry");

    rv = soc_mem_read(unit, ftk_mem, MEM_BLOCK_ANY, ft_index, ftk_entry);
    BCMI_CLEANUP_IF_ERROR(rv);

    if (k_mode == bcmiFtGroupModeSingle) {

        /* Single Wide Key */
        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEYf, ftk_fld);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_0f, ftk_fld);

        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_0f, 1);
    } else {

        /* Double Wide Key */
        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEY_0f, ftk_fld);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_0f, ftk_fld);

        soc_mem_field_get(unit, ftk_mem, (uint32 *)ftk_entry, KEY_1f, ftk_fld);
        soc_mem_field_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_1f, ftk_fld);

        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_1f, 1);
        soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, BASE_VALID_0f, 0);
    }


    key_type = soc_mem_field32_get(unit, ftk_mem, (uint32 *)ftk_entry, KEY_TYPEf);
    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, KEY_TYPEf, key_type);

    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, DO_NOT_LEARNf, 1);
    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, DO_NOT_FTf, 1);

    soc_mem_field32_set(unit, dnt_mem, (uint32 *)dnt_entry, VALIDf, 1);

    rv = soc_mem_write(unit, dnt_mem, MEM_BLOCK_ALL, 0, dnt_entry);
    BCMI_CLEANUP_IF_ERROR(rv);
    dnt_entry_created = 1;


    /* Delete flow from key table */
    rv = soc_mem_delete(unit, ftk_mem, MEM_BLOCK_ALL, ftk_entry);
    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit, "FT key delete: rv = %d. \n"), rv));
    BCMI_CLEANUP_IF_ERROR(rv);


    if (d_mode == bcmiFtGroupModeSingle) {
        ftd_mem = BSC_DT_FLEX_SESSION_DATA_SINGLEm;
        ag_index = ft_index;
    } else {
        ftd_mem = BSC_DT_FLEX_SESSION_DATA_DOUBLEm;
        ag_index = ft_index * 2;
    }


    /* Set VALID=0 in DATA table. */
    rv = soc_mem_field32_modify(unit, ftd_mem, ft_index, VALIDf, 0);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Set VALID=0 in AGE table. */
    rv = soc_mem_field32_modify(unit, ag_mem, ag_index, ENTRY_VALIDf, 0);
    BCMI_CLEANUP_IF_ERROR(rv);

    /* Update Learn counter */
    bcmi_ftk_group_learn_counter_update(unit, flow_group_id, -1);

cleanup:
    if (dnt_entry_created == 1) {

        /* Delete the one-entry DO_NOT_FT_ENTRY to allow
         * flowtracking for the user installed entry.
         */
        soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_LEARNf, 0);
        soc_mem_field32_set(unit, dnt_mem, dnt_entry, DO_NOT_FTf, 0);

        soc_mem_field32_set(unit, dnt_mem, dnt_entry, VALIDf, 0);
        (void) soc_mem_write(unit, dnt_mem, MEM_BLOCK_ALL, 0, dnt_entry);
    }

    FTK_FREE(dnt_entry);
    FTK_FREE(ftk_fld);
    FTK_FREE(ftk_entry);

    return rv;

}

/*
 * Function:
 *      bcmi_ftk_user_entry_delete_all
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_ftk_user_entry_delete_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id)
{
    int ix;
    int rv;
    int num_hdl;
    bcm_flowtracker_user_entry_handle_t *hdl_list = NULL;

    rv = bcmi_ftk_user_entry_get_all(
            unit,
            flow_group_id,
            0,
            NULL,
            &num_hdl);
    BCM_IF_ERROR_RETURN(rv);

    if (num_hdl == 0) {
        return BCM_E_NONE;
    }

    FTK_ALLOC(hdl_list,
        (num_hdl * sizeof(bcm_flowtracker_user_entry_handle_t)),
        "FTK Entry handles");

    rv = bcmi_ftk_user_entry_get_all(
            unit,
            flow_group_id,
            num_hdl,
            hdl_list,
            &num_hdl);
    BCMI_CLEANUP_IF_ERROR(rv);

    for (ix = 0; ix < num_hdl; ix++) {
        rv = bcmi_ftk_user_entry_delete(unit, flow_group_id, &(hdl_list[ix]));
        BCMI_CLEANUP_IF_ERROR(rv);
    }

cleanup:
    FTK_FREE(hdl_list);

    return rv;

}

#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_group_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */

