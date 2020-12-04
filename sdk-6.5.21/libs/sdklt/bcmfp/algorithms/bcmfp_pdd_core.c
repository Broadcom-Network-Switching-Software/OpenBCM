/*! \file bcmfp_pdd_core.c
 *
 * PDD profile Create and Delete APIs.
 *
 * Core PDD create and Delete APIs common to all stages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_action_cfg_internal.h>
#include <bcmfp/bcmfp_pdd.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief Loop through the actions in PDD config info
 *  and construct the bitmap of containers.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] Container bitmap of PDD entry LT actions.
 * \param [out] Container linked list based on entry LT actions.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_generate_container_bitmap(int unit,
                              bcmfp_pdd_cfg_t *pdd_cfg_info,
                              uint32_t *container_bmp,
                              bcmfp_pdd_mux_cont_info_t **cont_list)
{
    uint16_t idx = 0;
    uint16_t id = 0;
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_action_cfg_t **action_cfg;
    bcmfp_action_t action_id = 0;
    bcmfp_pdd_mux_cont_info_t *mux_cont_ptr;

    SHR_FUNC_ENTER(unit);

    action_cfg = pdd_cfg_info->action_cfg_db->action_cfg;

    for (id = 0; id < pdd_cfg_info->num_actions; id++) {

        /*
         * Get the action offset's associated
         * with this action.
         */
        action_id = pdd_cfg_info->pdd_action[id];
        if (action_cfg[action_id] != NULL &&
            !(action_cfg[action_id]->flags & BCMFP_ACTION_NOT_VALID)) {
            a_offset = action_cfg[action_id]->offset_cfg;
        } else {
            /* Action is not mapped to physical containers
             * return un avail error in this case.
            */
            if (action_cfg[action_id] != NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "BCMFP: Action is not"
                    " mapped to physical container.Action: %s\n"),
                    action_cfg[action_id]->action_name));
            }
            continue;
        }

        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (a_offset.width[idx] > 0 ) {

                /*
                 * Set the corresponding bit
                 * in container bitmap.
                 */
                SHR_BITSET(container_bmp,
                           a_offset.phy_cont_id[idx]);

                /* Full mux need number of 8bit required from the container. */
                if (pdd_cfg_info->pdd_hw_info->pdd_data_full_mux_format &&
                    cont_list != NULL) {
                    mux_cont_ptr = *cont_list;
                    while (mux_cont_ptr) {
                        if (mux_cont_ptr->cont_id ==
                            a_offset.phy_cont_id[idx]) {
                            SHR_BITSET_RANGE(&(mux_cont_ptr->bitmap),
                                             a_offset.offset[idx],
                                             a_offset.width[idx]);
                            break;
                        }
                        mux_cont_ptr = mux_cont_ptr->next;
                    }
                    if (mux_cont_ptr == NULL) {
                        BCMFP_ALLOC(mux_cont_ptr,
                                    sizeof(bcmfp_pdd_mux_cont_info_t),
                                    "bcmfpPddMuxCont");
                        SHR_BITSET_RANGE(&(mux_cont_ptr->bitmap),
                                         a_offset.offset[idx],
                                         a_offset.width[idx]);
                        mux_cont_ptr->cont_id = a_offset.phy_cont_id[idx];
                        mux_cont_ptr->next = *cont_list;
                        *cont_list = mux_cont_ptr;
                    }
                }

            }
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the section id from the container id from
 *  section info.
 *
 * \param [in] unit Logical device id
 * \param [in] HW Section Info
 * \param [in] Action Container ID.
 * \param [out] PDD section ID.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_get_section_id_from_cont_id(int unit,
                                      bcmfp_pdd_hw_section_info_t *section_info,
                                      uint16_t cont_id,
                                      uint8_t *section_id)
{
    uint8_t id = 0;
    uint16_t sec_start_bit = 0;
    uint16_t sec_end_bit = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    for (id = 0; id < section_info->num_pdd_sections; id++) {

        sec_start_bit = section_info->section_start_bit[id];
        sec_end_bit = section_info->section_end_bit[id];

        if (cont_id >= sec_start_bit &&
            cont_id <= sec_end_bit) {

            *section_id = section_info->section_id[id];
            found = TRUE;
            break;
        }
    }
    if (FALSE == found) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief get the container offset given the part container bitmap
 *  and section id.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate read PDD from LSB or MSB
 * \param [in] Action config DB
 * \param [in] HW Section Info
 * \param [in] Container Identifier
 * \param [in] PDD Section ID
 * \param [in] part Container bitmap of PDD entry LT actions.
 * \param [out] container offset within section
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static
int bcmfp_pdd_get_cont_offset_within_section(int unit,
                                      bool read_from_lsb,
                                      bcmfp_action_cfg_db_t *action_cfg_db,
                                      bcmfp_pdd_hw_section_info_t *section_info,
                                      uint16_t cont_id,
                                      uint8_t sec_id,
                                      uint32_t *cont_bitmap,
                                      uint16_t *cont_offset)
{
    uint16_t id = 0;

    SHR_FUNC_ENTER(unit);

    /* Calculate the container offset in the current section */
    if (TRUE == read_from_lsb) {
        for (id = section_info->section_start_bit[sec_id];
             id < cont_id; id++) {
            if (!(SHR_BITGET(cont_bitmap,id))) {
                continue;
            } else {
                (*cont_offset) +=
                      action_cfg_db->container_size[id];
            }
        }
    } else {
        for (id = section_info->section_end_bit[sec_id];
             id > cont_id; id--) {
            if (!(SHR_BITGET(cont_bitmap,id))) {
                continue;
            } else {
                (*cont_offset) -=
                      action_cfg_db->container_size[id];
            }
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To build part container bitmap and section offsets for
 *  the actions added to the PDD LT entry. Section offsets are
 *  build from lsb to msb so that the action data is placed
 *  correctly in the H/w PDD policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate generate oper info
 * \param [in] PDD config info
 * \param [in] Container bitmap of PDD entry LT actions.
 * \param [out] pdd operational info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_gen_sec_info_bitmap_lsb_to_data_lsb(int unit,
                                            uint32_t flags,
                                            bcmfp_pdd_cfg_t *pdd_cfg_info,
                                            uint32_t *all_container_bitmap,
                                            bcmfp_pdd_oper_t **pdd_operational_info,
                                            uint8_t *num_parts)
{
    int16_t idx = 0;
    uint8_t part = 0;
    int8_t prev_sec_id = -1;
    uint8_t cont_size = 0;
    uint8_t sec_id = 0;
    uint8_t part_prev = 0;
    uint8_t fill_bits =0;
    uint32_t actual_policy_width = 0;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    uint32_t cont_bitmap[BCMFP_ENTRY_PARTS_MAX][BCMFP_MAX_PDD_WORDS];
    uint32_t data_width[BCMFP_ENTRY_PARTS_MAX] = { 0 };
    bcmfp_pdd_section_info_t section_info[BCMFP_ENTRY_PARTS_MAX];
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_oper_t *prev_pdd_oper_info = NULL;
    bool valid_pdd_data = FALSE;

    SHR_FUNC_ENTER(unit);

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        sal_memset(cont_bitmap[part], 0,
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
        sal_memset(&section_info[part], 0, sizeof(bcmfp_pdd_section_info_t));
    }

    part = 0;
    sec_hw_info = (pdd_cfg_info->pdd_hw_info->section_hw_info);
    *num_parts = 0;

    for (idx = 0; idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {

        if (SHR_BITGET(all_container_bitmap, idx)) {

            valid_pdd_data = TRUE;

            cont_size = pdd_cfg_info->action_cfg_db->container_size[idx];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));
            part_prev =part;

            do {
                fill_bits = 0;

                if (prev_sec_id == -1) {

                    /*
                     * This is the first section. set the
                     * section offset to 0 for this section id.
                     */
                    section_info[part].num_sections =
                                     sec_hw_info->num_pdd_sections;
                    prev_sec_id = sec_id;

                    section_info[part].section_id[sec_id] = sec_id;

                    actual_policy_width = pdd_cfg_info->raw_data_offset;

                } else if (prev_sec_id != sec_id) {

                    /* Fill bits needed to make section offset byte boundary */
                    fill_bits = (actual_policy_width %
                                (pdd_cfg_info->pdd_hw_info->section_align_bits));
                } else {
                    fill_bits = 0;
                }

                if ((actual_policy_width + cont_size + fill_bits)
                    <= pdd_cfg_info->pdd_hw_info->raw_policy_width) {

                    /* Container can be accomadated in this part */
                    SHR_BITSET(cont_bitmap[part], idx);

                    actual_policy_width = actual_policy_width +
                                          cont_size +
                                          fill_bits;
                    if (prev_sec_id != sec_id) {
                        /*
                         * Section offset is section width
                         * calculated in bytes. i.e., for 24 bits of
                         * section width, section offset is 3.
                         */
                        section_info[part].section_offset[sec_id] =
                                          (actual_policy_width/8);
                        prev_sec_id = sec_id;
                    }
                    data_width[part] = actual_policy_width;
                } else {

                    /*
                     * Current part cannot accomodate the
                     * current container. Move to the next part.
                     */
                    part_prev = part;
                    part++;
                    if (part >= pdd_cfg_info->pdd_hw_info->num_max_pdd_parts) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } while (part_prev != part);
        }
    }

    if (valid_pdd_data == TRUE) {
        *num_parts = part + 1;
    }


    if (!(flags & BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED)) {

        for (part = 0; part < *num_parts; part++) {

            BCMFP_ALLOC(pdd_oper_info,
                    sizeof(bcmfp_pdd_oper_t),
                    "bcmfpPddOperPartInfo");

            sal_memcpy(pdd_oper_info->container_bitmap, cont_bitmap[part],
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));

            sal_memcpy(&(pdd_oper_info->section_info), &(section_info[part]),
                            sizeof(bcmfp_pdd_section_info_t));

            pdd_oper_info->data_width = data_width[part];
            pdd_oper_info->next = NULL;
            /*
             * The condition part == 0 is with in for loop
             * and its value is incremented and else part is
             * executed when 2nd, 3rd iterations of for loop.
             */
            /* coverity[dead_error_condition: FALSE] */
            if (part == 0) {
                prev_pdd_oper_info = pdd_oper_info;
            } else {
                /* coverity[dead_error_line: FALSE] */
                prev_pdd_oper_info->next = pdd_oper_info;
            }
        }
        *pdd_operational_info = prev_pdd_oper_info;
    }

 exit:
    /*
     * Caller function is responsible for
     * releasing memory used for pdd_operational_info
     * by calling bcmfp_pdd_oper_info_free function after it's
     * use of the structure.
     */
    /* coverity[leaked_storage] */
    /* coverity[resource_leak : FALSE] */
    SHR_FUNC_EXIT();
}

/*!
 * \brief To build part container bitmap and section offsets for
 *  the actions added to the PDD LT entry. Section offsets are
 *  build from msb to lsb so that the action data is placed
 *  correctly in the H/w PDD policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate generate oper info
 * \param [in] PDD config info
 * \param [in] Container bitmap of PDD entry LT actions.
 * \param [out] pdd operational info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_lsb(int unit,
                                            uint32_t flags,
                                            bcmfp_pdd_cfg_t *pdd_cfg_info,
                                            uint32_t *all_container_bitmap,
                                            bcmfp_pdd_oper_t **pdd_operational_info,
                                            uint8_t *num_parts)
{
    int16_t idx = 0;
    uint8_t part = 0;
    int8_t prev_sec_id = -1;
    uint8_t cont_size = 0;
    uint8_t sec_id = 0;
    uint8_t part_prev = 0;
    uint8_t fill_bits =0;
    uint32_t actual_policy_width = 0;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    bcmfp_pdd_section_info_t section_info[BCMFP_ENTRY_PARTS_MAX];
    uint32_t cont_bitmap[BCMFP_ENTRY_PARTS_MAX][BCMFP_MAX_PDD_WORDS];
    uint32_t data_width[BCMFP_ENTRY_PARTS_MAX] = { 0 };
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_oper_t *prev_pdd_oper_info = NULL;
    bool valid_pdd_data = FALSE;

    SHR_FUNC_ENTER(unit);

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        sal_memset(cont_bitmap[part], 0,
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
        sal_memset(&section_info[part], 0, sizeof(bcmfp_pdd_section_info_t));
    }

    part = 0;
    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;
    *num_parts = 0;

    for (idx = (pdd_cfg_info->pdd_hw_info->pdd_bitmap_size - 1);
         idx >= 0; idx--) {

        if (SHR_BITGET(all_container_bitmap, idx)) {

            valid_pdd_data = TRUE;
            cont_size = pdd_cfg_info->action_cfg_db->container_size[idx];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));
            part_prev =part;
            do {
                fill_bits = 0;
                if (prev_sec_id == -1) {

                    /* this is the first section. set the
                     * section offset to 0 for this section id.
                     */
                    section_info[part].num_sections =
                                      sec_hw_info->num_pdd_sections;
                    prev_sec_id = sec_id;

                    section_info[part].section_id[sec_id] = sec_id;

                    actual_policy_width = pdd_cfg_info->raw_data_offset;

                } else if (prev_sec_id != sec_id) {

                    /* Fill bits needed to make section offset byte boundary*/
                    fill_bits = (actual_policy_width %
                                (pdd_cfg_info->pdd_hw_info->section_align_bits));
                } else {
                    fill_bits = 0;
                }

                if ((actual_policy_width + cont_size + fill_bits)
                    <= pdd_cfg_info->pdd_hw_info->raw_policy_width) {

                    /* Container can be accomadated in this part */
                    SHR_BITSET(cont_bitmap[part], idx);

                    actual_policy_width = actual_policy_width +
                                          cont_size +
                                          fill_bits;
                    if (prev_sec_id != sec_id) {

                        /*
                         * Section offset is section width
                         * calculated in bytes. i.e., for 24 bits of
                         * section width, section offset is 3.
                         */
                        section_info[part].section_offset[sec_id] =
                                          (actual_policy_width/8);
                        prev_sec_id = sec_id;
                    }
                    data_width[part] = actual_policy_width;
                } else {

                    /*
                     * Current part cannot accomodate the current
                     * container. Move to next part
                     */

                    part_prev = part;
                    part++;

                    if (part >= pdd_cfg_info->pdd_hw_info->num_max_pdd_parts) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } while (part_prev != part);
        }
    }

    if (valid_pdd_data == TRUE) {
        *num_parts = part + 1;
    }

    if (!(flags & BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED)) {

        for (part = 0; part < *num_parts; part++) {

            BCMFP_ALLOC(pdd_oper_info,
                    sizeof(bcmfp_pdd_oper_t),
                    "bcmfpPddOperPartInfo");

            sal_memcpy(pdd_oper_info->container_bitmap, cont_bitmap[part],
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));

            sal_memcpy(&(pdd_oper_info->section_info), &(section_info[part]),
                            sizeof(bcmfp_pdd_section_info_t));

            pdd_oper_info->data_width = data_width[part];
            pdd_oper_info->next = NULL;
            /*
             * The condition part == 0 is with in for loop
             * and its value is incremented and else part is
             * executed when 2nd, 3rd iterations of for loop.
             */
            /* coverity[dead_error_condition: FALSE] */
            if (part == 0) {
                prev_pdd_oper_info = pdd_oper_info;
            } else {
                /* coverity[dead_error_line: FALSE] */
                prev_pdd_oper_info->next = pdd_oper_info;
            }
        }
        *pdd_operational_info = prev_pdd_oper_info;
    }

 exit:
     /*
     * Caller function is responsible for
     * releasing memory used for pdd_operational_info
     * by calling bcmfp_pdd_oper_info_free function after it's
     * use of the structure.
     */
    /* coverity[leaked_storage] */
    /* coverity[resource_leak : FALSE] */
    SHR_FUNC_EXIT();
}


/*!
 * \brief To build part container bitmap and section offsets for
 *  the actions added to the PDD LT entry. Section offsets are
 *  build from msb to lsb so that the action data is placed
 *  correctly in the H/w PDD policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate generate oper info
 * \param [in] PDD config info
 * \param [in] Container bitmap of PDD entry LT actions.
 * \param [out] pdd operational info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_msb(int unit,
                                        uint32_t flags,
                                        bcmfp_pdd_cfg_t *pdd_cfg_info,
                                        uint32_t *all_container_bitmap,
                                        bcmfp_pdd_oper_t **pdd_operational_info,
                                        uint8_t *num_parts)
{
    int16_t idx = 0;
    uint8_t part = 0;
    int8_t prev_sec_id = -1;
    uint8_t cont_size = 0;
    uint8_t sec_id = 0;
    uint8_t part_prev = 0;
    uint8_t fill_bits = 0;
    uint32_t prev_sec_bits = 0;
    int32_t actual_policy_width = 0;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    bcmfp_pdd_section_info_t section_info[BCMFP_ENTRY_PARTS_MAX];
    uint32_t cont_bitmap[BCMFP_ENTRY_PARTS_MAX][BCMFP_MAX_PDD_WORDS];
    uint32_t data_width[BCMFP_ENTRY_PARTS_MAX] = { 0 };
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_oper_t *prev_pdd_oper_info = NULL;
    bool valid_pdd_data = FALSE;

    SHR_FUNC_ENTER(unit);

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        sal_memset(cont_bitmap[part], 0,
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
        sal_memset(&section_info[part], 0, sizeof(bcmfp_pdd_section_info_t));
    }

    part = 0;
    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;

    *num_parts = 0;

    for (idx = (pdd_cfg_info->pdd_hw_info->pdd_bitmap_size - 1);
         idx >= 0; idx--) {

        if (SHR_BITGET(all_container_bitmap, idx)) {

            valid_pdd_data = TRUE;
            cont_size = pdd_cfg_info->action_cfg_db->container_size[idx];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));
            part_prev =part;
            do {
                fill_bits = 0;
                if (prev_sec_id == -1) {

                    /*
                     * This is the first section. set the
                     * section offset to 0 for this section id.
                     */
                    section_info[part].num_sections =
                                      sec_hw_info->num_pdd_sections;
                    prev_sec_id = sec_id;

                    section_info[part].section_id[sec_id] = sec_id;

                    section_info[part].section_offset[sec_id] = 0;

                    actual_policy_width =
                                pdd_cfg_info->pdd_hw_info->raw_policy_width
                                - pdd_cfg_info->raw_data_offset;

                } else if (prev_sec_id != sec_id) {

                    /* compute the bits occupied by the prev sections */
                    prev_sec_bits = pdd_cfg_info->pdd_hw_info->raw_policy_width
                                  - pdd_cfg_info->raw_data_offset
                                  - actual_policy_width;
                    /*
                     * compute the fill bits needed to make the curr sec offset
                     * w.r.t the first section offset
                     * a multiple of 8
                     */
                    /* Fill bits to be added to the prev_section
                     * to make section offset w.r.t prev sec byte boundary*/
                    if ((prev_sec_bits % 8) != 0) {
                       fill_bits = pdd_cfg_info->pdd_hw_info->section_align_bits
                                 - (prev_sec_bits %
                                 pdd_cfg_info->pdd_hw_info->section_align_bits);
                    } else {
                        fill_bits = 0;
                    }
                } else {
                    fill_bits = 0;
                }

                if ((actual_policy_width - cont_size - fill_bits)
                    >= 0) {

                    /* Container can be accomadated in this part */
                    SHR_BITSET(cont_bitmap[part], idx);

                    actual_policy_width = actual_policy_width -
                                          fill_bits;
                    if (prev_sec_id != sec_id) {

                        /*
                         * Section offset is section width
                         * calculated in bytes. i.e., for 24 bits of
                         * section width, section offset is 3.
                         * offset would be (bits occupied by the
                         * prev section  (prev_sec_bits) + fill_bits)/8
                         */
                        section_info[part].section_offset[sec_id] =
                                ((pdd_cfg_info->pdd_hw_info->raw_policy_width
                                  - pdd_cfg_info->raw_data_offset
                                         - actual_policy_width)/8);
                        prev_sec_id = sec_id;
                    }
                    actual_policy_width = actual_policy_width -
                                          cont_size;
                    if (prev_sec_id == -1) {
                        data_width[part] = pdd_cfg_info->raw_data_offset +
                                           cont_size + fill_bits;
                    } else {
                        data_width[part] += cont_size + fill_bits;
                    }
                } else {

                    /*
                     * Current part cannot accomodate the current
                     * container. Move to next part
                     */

                    part_prev = part;
                    part++;

                    if (part >= pdd_cfg_info->pdd_hw_info->num_max_pdd_parts) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } while (part_prev != part);
        }
    }

    if (valid_pdd_data == TRUE) {
        *num_parts = part + 1;
    }

    if (!(flags & BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED)) {

        for (part = 0; part < *num_parts; part++) {

            BCMFP_ALLOC(pdd_oper_info,
                    sizeof(bcmfp_pdd_oper_t),
                    "bcmfpPddOperPartInfo");

            sal_memcpy(pdd_oper_info->container_bitmap, cont_bitmap[part],
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));

            sal_memcpy(&(pdd_oper_info->section_info), &(section_info[part]),
                            sizeof(bcmfp_pdd_section_info_t));

            pdd_oper_info->data_width = data_width[part];
            pdd_oper_info->next = NULL;
            /*
             * The condition part == 0 is with in for loop
             * and its value is incremented and else part is
             * executed when 2nd, 3rd iterations of for loop.
             */
            /* coverity[dead_error_condition: FALSE] */
            if (part == 0) {
                prev_pdd_oper_info = pdd_oper_info;
            } else {
                /* coverity[dead_error_line: FALSE] */
                prev_pdd_oper_info->next = pdd_oper_info;
            }
        }
        *pdd_operational_info = prev_pdd_oper_info;
    }

 exit:
     /*
     * Caller function is responsible for
     * releasing memory used for pdd_operational_info
     * by calling bcmfp_pdd_oper_info_free function after it's
     * use of the structure.
     */
    /* coverity[leaked_storage] */
    /* coverity[resource_leak : FALSE] */
    SHR_FUNC_EXIT();
}

/*!
 * \brief To build part container bitmap and section offsets for
 *  the actions added to the PDD LT entry. Section offsets are
 *  build from mixed msb and lsb so that the action data is placed
 *  correctly in the H/w PDD policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate generate oper info
 * \param [in] PDD config info
 * \param [in] Container bitmap of PDD entry LT actions.
 * \param [out] pdd operational info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_gen_sec_info_bitmap_mixed_msb_lsb_data(int unit,
                                        uint32_t flags,
                                        bcmfp_pdd_cfg_t *pdd_cfg_info,
                                        uint32_t *all_container_bitmap,
                                        bcmfp_pdd_oper_t **pdd_operational_info,
                                        uint8_t *num_parts)
{
    int16_t idx = 0;
    uint8_t part = 0;
    int8_t prev_sec_id = -1;
    uint8_t cont_size = 0;
    uint8_t sec_id = 0;
    uint8_t part_prev = 0;
    uint8_t fill_bits =0;
    int32_t rem_policy_width = 0;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    bcmfp_pdd_section_info_t section_info[BCMFP_ENTRY_PARTS_MAX];
    uint32_t cont_bitmap[BCMFP_ENTRY_PARTS_MAX][BCMFP_MAX_PDD_WORDS];
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_oper_t *prev_pdd_oper_info = NULL;
    bool valid_pdd_data = FALSE;
    int32_t msb_data_offset = 0;
    int32_t lsb_data_offset = 0;

    SHR_FUNC_ENTER(unit);

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        sal_memset(cont_bitmap[part], 0,
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
        sal_memset(&section_info[part], 0, sizeof(bcmfp_pdd_section_info_t));
    }

    part = 0;
    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;

    *num_parts = 0;

    rem_policy_width =
                pdd_cfg_info->pdd_hw_info->raw_policy_width
                - pdd_cfg_info->raw_data_offset;

    msb_data_offset = pdd_cfg_info->pdd_hw_info->raw_policy_width;

    if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {
        lsb_data_offset = pdd_cfg_info->raw_data_offset;
    } else {
        msb_data_offset -= pdd_cfg_info->raw_data_offset;
    }

    /* Process MSB containers */
    for (idx = (pdd_cfg_info->pdd_hw_info->pdd_bitmap_size - 1);
         idx >= 0; idx--) {

        if (SHR_BITGET(all_container_bitmap, idx)) {
            cont_size = pdd_cfg_info->action_cfg_db->container_size[idx];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));

            /* Skip LSB aligned section conatiners */
            if (sec_hw_info->section_is_lsb[sec_id]) {
                continue;
            }

            valid_pdd_data = TRUE;
            part_prev = part;

            do {
                fill_bits = 0;

                if (prev_sec_id == -1) {
                    /*
                     * This is the first section. set the
                     * section offset to 0 for this section id.
                     */
                    section_info[part].num_sections =
                                      sec_hw_info->num_pdd_sections;
                    prev_sec_id = sec_id;

                    section_info[part].section_id[sec_id] = sec_id;

                    section_info[part].section_offset[sec_id] = 0;

                } else if (prev_sec_id != sec_id) {
                    /* Fill bits needed to make section offset gran bit boundary */
                    fill_bits = (msb_data_offset %
                                 sec_hw_info->section_align_bits[sec_id]);
                } else {
                    fill_bits = 0;
                }

                if ((rem_policy_width - cont_size - fill_bits) >= 0) {

                    /* Container can be accomadated in this part */
                    SHR_BITSET(cont_bitmap[part], idx);

                    rem_policy_width -= fill_bits;
                    msb_data_offset -= fill_bits;

                    if (prev_sec_id != sec_id) {
                        /*
                         * Section offset is section width
                         * calculated in offset gran bits. i.e., for 24 bits of
                         * section width, section offset is 3 if offset gran is 8.
                         */
                        section_info[part].section_offset[sec_id] =
                                ((pdd_cfg_info->pdd_hw_info->raw_policy_width
                                         - msb_data_offset) /
                                 sec_hw_info->section_align_bits[sec_id]);
                        prev_sec_id = sec_id;
                    }
                    rem_policy_width -= cont_size;
                    msb_data_offset -= cont_size;
                } else {

                    /*
                     * Current part cannot accomodate the current
                     * container. Move to next part
                     */

                    part_prev = part;
                    part++;

                    if (part >= pdd_cfg_info->pdd_hw_info->num_max_pdd_parts) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } while (part_prev != part);
        }
    }

    /* Process LSB containers */
    prev_sec_id = -1;
    for (idx = 0;
         idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {

        if (SHR_BITGET(all_container_bitmap, idx)) {
            cont_size = pdd_cfg_info->action_cfg_db->container_size[idx];

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));

            /* Skip non-LSB containers */
            if (sec_hw_info->section_is_lsb[sec_id] != 1) {
                continue;
            }

            valid_pdd_data = TRUE;
            part_prev = part;

            do {
                fill_bits = 0;

                if (prev_sec_id == -1) {
                    /*
                     * This is the first section. set the
                     * section offset to 0 for this section id.
                     */
                    section_info[part].num_sections =
                                      sec_hw_info->num_pdd_sections;
                    prev_sec_id = sec_id;

                    section_info[part].section_id[sec_id] = sec_id;

                    section_info[part].section_offset[sec_id] = 0;

                } else if (prev_sec_id != sec_id) {
                    /* Fill bits needed to make section offset gran bit boundary */
                    fill_bits = (lsb_data_offset %
                                  sec_hw_info->section_align_bits[sec_id]);
                    if (fill_bits > 0) {
                        fill_bits = sec_hw_info->section_align_bits[sec_id] - fill_bits;
                    }
                } else {
                    fill_bits = 0;
                }

                if ((rem_policy_width - cont_size - fill_bits) >= 0) {

                    /* Container can be accomadated in this part */
                    SHR_BITSET(cont_bitmap[part], idx);

                    rem_policy_width -= fill_bits;
                    lsb_data_offset += fill_bits;

                    if (prev_sec_id != sec_id) {
                        /*
                         * Section offset is section width
                         * calculated in offset gran bits. i.e., for 24 bits of
                         * section width, section offset is 3 if offset gran is 8.
                         */
                        section_info[part].section_offset[sec_id] =
                                         (lsb_data_offset /
                                          sec_hw_info->section_align_bits[sec_id]);
                        prev_sec_id = sec_id;
                    }
                    rem_policy_width -= cont_size;
                    lsb_data_offset += cont_size;
                } else {
                    /*
                     * Current part cannot accomodate the current
                     * container. Move to next part
                     */

                    part_prev = part;
                    part++;

                    if (part >= pdd_cfg_info->pdd_hw_info->num_max_pdd_parts) {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
            } while (part_prev != part);
        }
    }

    if (valid_pdd_data == TRUE) {
        *num_parts = part + 1;
    }

    if (!(flags & BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED)) {

        for (part = 0; part < *num_parts; part++) {

            BCMFP_ALLOC(pdd_oper_info,
                    sizeof(bcmfp_pdd_oper_t),
                    "bcmfpPddOperPartInfo");

            sal_memcpy(pdd_oper_info->container_bitmap, cont_bitmap[part],
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));

            sal_memcpy(&(pdd_oper_info->section_info), &(section_info[part]),
                            sizeof(bcmfp_pdd_section_info_t));

            pdd_oper_info->next = NULL;
            /*
             * The condition part == 0 is with in for loop
             * and its value is incremented and else part is
             * executed when 2nd, 3rd iterations of for loop.
             */
            /* coverity[dead_error_condition: FALSE] */
            if (part == 0) {
                prev_pdd_oper_info = pdd_oper_info;
            } else {
                /* coverity[dead_error_line: FALSE] */
                prev_pdd_oper_info->next = pdd_oper_info;
            }
        }
        *pdd_operational_info = prev_pdd_oper_info;
    }

 exit:
     /*
     * Caller function is responsible for
     * releasing memory used for pdd_operational_info
     * by calling bcmfp_pdd_oper_info_free function after it's
     * use of the structure.
     */
    /* coverity[leaked_storage] */
    /* coverity[resource_leak : FALSE] */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Finds container id in the linked list.
 *
 * \param [in] unit Logical device id
 * \param [in] cont_list contatiner linked list
 * \param [in] idx contatiner id
 * \param [out] ptr Container id pointer.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_mux_cont_ptr_get(int unit,
                       bcmfp_pdd_mux_cont_info_t *cont_list,
                       int16_t idx,
                       bcmfp_pdd_mux_cont_info_t **ptr)
{
    bcmfp_pdd_mux_cont_info_t *mux_cont_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cont_list, SHR_E_INTERNAL);
    SHR_NULL_CHECK(ptr, SHR_E_INTERNAL);

    mux_cont_ptr = cont_list;
    while (mux_cont_ptr) {
        if (mux_cont_ptr->cont_id == idx) {
            break;
        }
        mux_cont_ptr = mux_cont_ptr->next;
    }
    if (mux_cont_ptr == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    *ptr = mux_cont_ptr;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocates a free mux for container.
 *
 * \param [in] unit Logical device id
 * \param [in] mux_min Minimum number of mux.
 * \param [in] mux_max Maximum number of mux.
 * \param [in] num_mux Number of mux to be allocated.
 * \param [in] is_mux_msb Mux allocation is from MSB.
 * \param [inout] mux_bitmap Mux allocation bitmap.
 * \param [out] alloc_mux_id Allocated mux id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_mux_alloc(int unit,
                    int8_t mux_min,
                    int8_t mux_max,
                    uint8_t num_mux,
                    bool is_mux_msb,
                    uint32_t *mux_bitmap,
                    int8_t *alloc_mux_id)
{
    int8_t mux_id;

    SHR_FUNC_ENTER(unit);

    if (mux_max > BCMFP_MAX_PDD_MUX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (is_mux_msb) {
        for (mux_id = mux_max - 1; mux_id >= mux_min; mux_id--) {
            if (SHR_BITGET(mux_bitmap, mux_id) == 0) {
                if ((num_mux == 2) &&
                    !(((mux_id - 1) >= mux_min) &&
                      (SHR_BITGET(mux_bitmap, (mux_id - 1)) == 0))) {
                    continue;
                }
                SHR_BITSET(mux_bitmap, mux_id);
                if (num_mux == 2) {
                    SHR_BITSET(mux_bitmap, (mux_id - 1));
                }
                break;
            }
        }

        if (mux_id < mux_min) {
            /* Free Mux not available */
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    } else {
        for (mux_id = mux_min; mux_id < mux_max; mux_id++) {
            if (SHR_BITGET(mux_bitmap, mux_id) == 0) {
                if ((num_mux == 2) &&
                    !(((mux_id + 1) < mux_max) &&
                      (SHR_BITGET(mux_bitmap, (mux_id + 1)) == 0))) {
                    continue;
                }
                SHR_BITSET(mux_bitmap, mux_id);
                if (num_mux == 2) {
                    SHR_BITSET(mux_bitmap, (mux_id + 1));
                }
                break;
            }
        }

        if (mux_id == mux_max) {
            /* Free Mux not available */
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }
    *alloc_mux_id = mux_id;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assigns mux select and container offset values.
 *
 * \param [in] unit Logical device id
 * \param [in] mux_id Mux id
 * \param [in] mux_max Maximum number of mux.
 * \param [in] num_mux Number of mux to be allocated.
 * \param [in] mux_size  Width of mux.
 * \param [in] mux_start offset for mux section start.
 * \param [in] raw_policy_width Width of policy data.
 * \param [in] is_mux_msb Mux allocation is from MSB.
 * \param [in] sec_start_bit Start bit for the section.
 * \param [inout] mux_sel Mux Select array.
 * \param [inout] mux_cont_ptr Container pointer.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_mux_sel_cont_offset_update(int unit,
                                     int8_t mux_id,
                                     int8_t mux_max,
                                     uint8_t num_mux,
                                     uint8_t mux_size,
                                     uint8_t mux_start,
                                     uint16_t raw_policy_width,
                                     bool is_mux_msb,
                                     uint8_t sec_start_bit,
                                     uint8_t mul,
                                     int16_t  *mux_sel,
                                     bcmfp_pdd_mux_cont_info_t *mux_cont_ptr)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mux_sel, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mux_cont_ptr, SHR_E_INTERNAL);

    mux_sel[mux_id] = (mux_cont_ptr->cont_id - sec_start_bit) * mul;

    if (is_mux_msb) {
        mux_cont_ptr->offset = ((mux_max - mux_id) * mux_size) + mux_start;
        mux_cont_ptr->offset = raw_policy_width - mux_cont_ptr->offset;

        if (num_mux == 2) {
            mux_sel[mux_id - 1] = mux_sel[mux_id];
            mux_sel[mux_id]++;
            mux_cont_ptr->offset -= mux_size;
            mux_cont_ptr->two_8bit_mux = 1;
        } else {
            if ((mul == 2) && (mux_cont_ptr->bitmap & 0xFF00)) {
                mux_sel[mux_id]++;
                mux_cont_ptr->offset_adjust = mux_size;
            }
        }
    } else {
        mux_cont_ptr->offset = (mux_id * mux_size) + mux_start;

        if (num_mux == 2) {
            mux_sel[mux_id + 1] = mux_sel[mux_id];
            mux_sel[mux_id + 1]++;
            mux_cont_ptr->two_8bit_mux = 1;
        } else {
            if ((mul == 2) && (mux_cont_ptr->bitmap & 0xFF00)) {
                mux_sel[mux_id]++;
                mux_cont_ptr->offset_adjust = mux_size;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate if any MUX are left based on allocation bitmap.
 *
 * \param [in] unit Logical device id
 * \param [in] mux_min Minimum number of mux.
 * \param [in] mux_max Maximum number of mux.
 * \param [in] mux_count  Number of Mux used.
 * \param [in] is_mux_msb Mux allocation is from MSB.
 * \param [inout] mux_bitmap Mux allocation bitmap.
 * \param [out] hole_count Number of holes in the bitmap.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_mux_hole_count_get(int unit,
                             int8_t mux_min,
                             int8_t mux_max,
                             uint8_t mux_count,
                             bool is_mux_msb,
                             uint32_t *mux_bitmap,
                             uint8_t *hole_count)
{
    bool count_en = FALSE;
    int8_t mux_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mux_bitmap, SHR_E_INTERNAL);
    SHR_NULL_CHECK(hole_count, SHR_E_INTERNAL);

    *hole_count = 0;
    if (mux_count == 0) {
        SHR_EXIT();
    }

    if (mux_max > BCMFP_MAX_PDD_MUX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (is_mux_msb) {
        mux_id = mux_min;
    } else {
        mux_id = mux_max - 1;
    }

    for (;(is_mux_msb ? (mux_id < mux_max) : (mux_id >= mux_min));) {
        if ((SHR_BITGET(mux_bitmap, mux_id) == 0) && count_en) {
            *hole_count = *hole_count + 1;
        } else if ((count_en == FALSE) &&
                   SHR_BITGET(mux_bitmap, mux_id)) {
            count_en = TRUE;
        }
        if (is_mux_msb) {
            mux_id++;
        } else {
            mux_id--;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reserves destion mux based on the source bitmap.
 * Required when both the sections are overlapping.
 *
 * \param [in] unit Logical device id
 * \param [in] is_msb Mux allocation is from MSB.
 * \param [in] src_mux_width Width of source mux.
 * \param [in] src_mux_start Source mux start offset.
 * \param [in] src_mux_max Maximum number of source mux.
 * \param [in] src_mux_bitmap Source Mux allocation bitmap.
 * \param [in] dst_mux_width Width of destination mux.
 * \param [in] dst_mux_start Destination mux start offset.
 * \param [in] dst_mux_max Maximum number of destination mux.
 * \param [inout] dst_mux_bitmap Destination Mux allocation bitmap.
 * \param [inout] dst_mux_count Used destination Mux count.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_dst_mux_reserve(int unit,
                          bool is_msb,
                          uint8_t src_mux_width,
                          uint8_t src_mux_start,
                          int8_t src_mux_max,
                          uint32_t *src_mux_bitmap,
                          uint8_t dst_mux_width,
                          uint8_t dst_mux_start,
                          int8_t dst_mux_max,
                          uint32_t *dst_mux_bitmap,
                          uint8_t *dst_mux_count)
{
    uint8_t overlap_byte_start;
    uint8_t dst_mux, bidx;
    int8_t idx;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src_mux_bitmap, SHR_E_INTERNAL);
    SHR_NULL_CHECK(dst_mux_bitmap, SHR_E_INTERNAL);
    SHR_NULL_CHECK(dst_mux_count, SHR_E_INTERNAL);

    if (src_mux_max > BCMFP_MAX_PDD_MUX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (dst_mux_max > BCMFP_MAX_PDD_MUX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (is_msb) {
        for (idx = src_mux_max - 1; idx >= 0; idx--) {
            if (SHR_BITGET(src_mux_bitmap, idx)) {
                overlap_byte_start = (src_mux_max - 1 - idx) * src_mux_width;
                overlap_byte_start += src_mux_start;
                overlap_byte_start -= dst_mux_start;
                for (bidx = 0; bidx < src_mux_width; bidx++) {
                    dst_mux = (overlap_byte_start + bidx) / dst_mux_width;
                    if (dst_mux < dst_mux_max) {
                        dst_mux = (dst_mux_max - 1 - dst_mux);
                        if (!SHR_BITGET(dst_mux_bitmap, dst_mux)) {
                            SHR_BITSET(dst_mux_bitmap, dst_mux);
                            (*dst_mux_count)++;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
    } else {
        for (idx = 0; idx < src_mux_max; idx++) {
            if (SHR_BITGET(src_mux_bitmap, idx)) {
                overlap_byte_start = idx * src_mux_width;
                overlap_byte_start += src_mux_start;
                overlap_byte_start -= dst_mux_start;
                for (bidx = 0; bidx < src_mux_width; bidx++) {
                    dst_mux = (overlap_byte_start + bidx) / dst_mux_width;
                    if (dst_mux < dst_mux_max) {
                        if (!SHR_BITGET(dst_mux_bitmap, dst_mux)) {
                            SHR_BITSET(dst_mux_bitmap, dst_mux);
                            (*dst_mux_count)++;
                        }
                    } else {
                        break;
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To build part container bitmap and mux sel and offsets for
 *  the actions added to the PDD LT entry. Mux sel and offsets are
 *  build for full mux mode so that the action data is placed
 *  correctly in the H/w PDD policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] flag to indicate generate oper info
 * \param [in] PDD config info
 * \param [in] Container bitmap of PDD entry LT actions.
 * \param [in] Container linked list.
 * \param [out] pdd operational info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_gen_sec_info_full_mux(int unit,
                                uint32_t flags,
                                bcmfp_pdd_cfg_t *pdd_cfg_info,
                                uint32_t *all_container_bitmap,
                                bcmfp_pdd_mux_cont_info_t *cont_list,
                                bcmfp_pdd_oper_t **pdd_operational_info,
                                uint8_t *num_parts)
{
    int rv;
    int16_t idx = 0;
    uint8_t sec_id = 0;
    int8_t mux_id, iter;
    uint16_t total_bytes;
    int16_t unused_sel[2];
    uint8_t cont_size = 0;
    uint8_t hole_count = 0;
    bool valid_pdd_data = FALSE;
    int32_t rem_policy_width = 0;
    bcmfp_pdd_mux_info_t mux_info;
    bcmfp_pdd_section_info_t section_info;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_mux_cont_info_t *mux_cont_ptr;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    uint32_t cont_bitmap[BCMFP_MAX_PDD_WORDS];
    uint8_t mux4_size, mux8_size, mux16_size;
    uint8_t mux4_count, mux8_count, mux16_count;
    uint8_t mux4_start, mux8_start, mux16_start;
    SHR_BITDCLNAME(mux4_bitmap, BCMFP_MAX_PDD_MUX);
    SHR_BITDCLNAME(mux8_bitmap, BCMFP_MAX_PDD_MUX);
    SHR_BITDCLNAME(mux16_bitmap, BCMFP_MAX_PDD_MUX);

    SHR_FUNC_ENTER(unit);

    sal_memset(cont_bitmap, 0,
               BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
    sal_memset(&section_info, 0, sizeof(bcmfp_pdd_section_info_t));
    sal_memset(&mux_info, 0, sizeof(bcmfp_pdd_mux_info_t));
    for (idx = 0 ; idx < BCMFP_MAX_PDD_MUX ; idx++) {
        mux_info.s0_mux4_sel[idx] = BCMFP_EXT_SELCODE_DONT_CARE;
        mux_info.s1_mux8_sel[idx] = BCMFP_EXT_SELCODE_DONT_CARE;
        mux_info.s1_mux16_sel[idx] = BCMFP_EXT_SELCODE_DONT_CARE;
    }
    unused_sel[0] = unused_sel[1] = BCMFP_EXT_SELCODE_DONT_CARE;

    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;

    *num_parts = 0;

    SHR_BITCLR_RANGE(mux4_bitmap, 0, BCMFP_MAX_PDD_MUX);
    SHR_BITCLR_RANGE(mux8_bitmap, 0, BCMFP_MAX_PDD_MUX);
    SHR_BITCLR_RANGE(mux16_bitmap, 0, BCMFP_MAX_PDD_MUX);
    mux4_count = mux8_count = mux16_count = 0;
    mux4_size = mux8_size = mux16_size = 0;
    mux4_start = mux8_start = mux16_start = 0;

    rem_policy_width =
                pdd_cfg_info->pdd_hw_info->raw_policy_width
                - pdd_cfg_info->raw_data_offset;

    /*
     * Per entry SBR/PDD is requried.
     * Start of 4bit section in full Mux has per entry SBR/PDD id's.
     * This will reserve 4Bit Mux's based on the raw_data_offset.
     */
    if ((pdd_cfg_info->raw_data_offset) &&
        (sec_hw_info->is_mux4_msb == sec_hw_info->is_fixed_sbr_pdd_msb)) {
        mux4_count = pdd_cfg_info->raw_data_offset / 4;
        mux_info.s0_mux4_align = pdd_cfg_info->raw_data_offset % 4;
        mux4_start = mux_info.s0_mux4_align;
        if (mux4_count) {
            if (sec_hw_info->is_mux4_msb) {
                SHR_BITSET_RANGE(mux4_bitmap, \
                                 (sec_hw_info->num_mux4 - mux4_count), \
                                 mux4_count);
            } else {
                SHR_BITSET_RANGE(mux4_bitmap, 0, mux4_count);
            }
        }
    }

    /*
     * AUX is required and 4bit & 8bit sections are overlaied.
     * AUX will be in 0th or 1st byte of 8bit section.
     * reserve 4Bit Mux which point to data bits used by 8bit Mux for AUX.
     */
    if ((sec_hw_info->is_mux4_msb == sec_hw_info->is_mux8_msb) &&
        SHR_BITGET(all_container_bitmap, sec_hw_info->aux_cont_id)) {
        if (sec_hw_info->is_mux4_msb) {
            SHR_BITSET_RANGE(mux4_bitmap, \
                             (sec_hw_info->num_mux4 - \
                             ((sec_hw_info->aux_byte_offset + 1) * 2)), 2);
        } else {
            SHR_BITSET_RANGE(mux4_bitmap, \
                             (sec_hw_info->aux_byte_offset * 2), 2);
        }
        mux4_count += 2;
    }
    mux4_size = mux4_count * 4;

    section_info.num_sections = sec_hw_info->num_pdd_sections;

    /* Process 4Bit Mux section. */
    for (idx = 0;
         idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {

        /* Skip Aux container for 4bit Mux */
        if (idx == sec_hw_info->aux_cont_id) {
            continue;
        }

        if (SHR_BITGET(all_container_bitmap, idx)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));
            /* Skip non-4Bit Sections */
            if (sec_id != sec_hw_info->mux4_sec_id) {
                continue;
            }

            if ((rem_policy_width - 4 - mux4_size) >= 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_mux_cont_ptr_get(unit, cont_list, idx,
                                            &mux_cont_ptr));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_mux_alloc(unit, 0, sec_hw_info->num_mux4, 1,
                                         sec_hw_info->is_mux4_msb,
                                         mux4_bitmap, &mux_id));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_mux_sel_cont_offset_update(
                            unit,
                            mux_id,
                            sec_hw_info->num_mux4,
                            1, 4,
                            mux4_start,
                            pdd_cfg_info->pdd_hw_info->raw_policy_width,
                            sec_hw_info->is_mux4_msb,
                            sec_hw_info->section_start_bit[sec_id],
                            sec_hw_info->mux4_mul,
                            mux_info.s0_mux4_sel,
                            mux_cont_ptr));

                SHR_BITSET(cont_bitmap, idx);
                valid_pdd_data = TRUE;
                mux4_size += 4;
                mux4_count++;
            } else {
                /* Currently only one part is supported. */
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    /* Check if there are any holes left in between 4Bit Mux section. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_mux_hole_count_get(unit, 0, sec_hw_info->num_mux4,
                                      mux4_count,
                                      sec_hw_info->is_mux4_msb,
                                      mux4_bitmap, &hole_count));
    mux4_size += (hole_count * 4);
    rem_policy_width -= mux4_size;

    if (rem_policy_width < 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (sec_hw_info->is_mux4_msb == sec_hw_info->is_mux8_msb) {
        /*
         * To simplify 4bit section and 8bit section align is set to same.
         * Worst case it might result 4bit data in 8bit section to be reserved.
         */
        mux8_start = mux_info.s1_mux8_align = mux_info.s0_mux4_align;
        /* When policy data is overlaied reset the rem_policy_width to max */
        rem_policy_width =
                    pdd_cfg_info->pdd_hw_info->raw_policy_width
                    - mux8_start;
    } else {
        mux_info.s1_mux8_align = 0;
        mux8_start = 0;
    }

    /* AUX would need one 8bit Mux to be reserved */
    if (SHR_BITGET(all_container_bitmap, sec_hw_info->aux_cont_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_mux_cont_ptr_get(unit, cont_list,
                                    sec_hw_info->aux_cont_id,
                                    &mux_cont_ptr));
        if (sec_hw_info->is_mux8_msb) {
            mux_cont_ptr->offset = (((sec_hw_info->aux_byte_offset + 1) * 8)
                                    + mux8_start);
            mux_cont_ptr->offset =
                pdd_cfg_info->pdd_hw_info->raw_policy_width -
                mux_cont_ptr->offset;
        } else {
            mux_cont_ptr->offset = sec_hw_info->aux_byte_offset * 8;
        }
        SHR_BITSET(cont_bitmap, sec_hw_info->aux_cont_id);
        if (sec_hw_info->is_mux8_msb) {
            SHR_BITSET(mux8_bitmap,
                       (sec_hw_info->num_mux8 - \
                        sec_hw_info->aux_byte_offset - 1));
        } else {
            SHR_BITSET(mux8_bitmap, sec_hw_info->aux_byte_offset);
        }
        mux8_count++;
    }

    /*
     * Reserve 8bit Mux's which point to data bits
     * already allocated to 4Bit Mux.
     */
    if (sec_hw_info->is_mux4_msb == sec_hw_info->is_mux8_msb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_dst_mux_reserve(unit, sec_hw_info->is_mux8_msb,
                                       4, mux4_start, sec_hw_info->num_mux4,
                                       mux4_bitmap,
                                       8, mux8_start, sec_hw_info->num_mux8,
                                       mux8_bitmap, &mux8_count));
    }
    mux8_size = mux8_count * 8;

    if ((rem_policy_width - mux8_size) < 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Process 8Bit Mux in 2 pass.
     * PASS 1 for 8bit selection only
     * PASS 2 for 16bit selection only.
     * PASS 2 requires atleast 2 Mux's to be free.
     */
    for (iter = 0; iter < 2; iter++) {
        for (idx = 0; idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {

            /* Skip container is already allocated */
            if (SHR_BITGET(cont_bitmap, idx)) {
                continue;
            }

            /* 2nd iteration is for 16bit seclection. Need atleast 2 mux's. */
            if ((iter == 1) &&
                (mux8_count >= (sec_hw_info->num_mux8 - 1))) {
                break;
            }

            if (SHR_BITGET(all_container_bitmap, idx)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                           sec_hw_info,
                                                           idx,
                                                           &sec_id));
                /* Skip non-8Bit Sections */
                if (sec_id != sec_hw_info->mux8_sec_id) {
                    continue;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_mux_cont_ptr_get(unit, cont_list, idx,
                                            &mux_cont_ptr));

                /* Requires 16bit container. Skip in pass 1. */
                if ((iter == 0) && (mux_cont_ptr->bitmap & 0xFF00) &&
                    (mux_cont_ptr->bitmap & 0xFF)) {
                    continue;
                }

                /* Requires 16bit container. */
                if ((mux_cont_ptr->bitmap & 0xFF00) &&
                    (mux_cont_ptr->bitmap & 0xFF)) {
                    cont_size = 16;
                } else {
                    cont_size = 8;
                }

                if ((rem_policy_width - cont_size - mux8_size) >= 0) {
                    /* Allocate required Mux's. */
                    rv = bcmfp_pdd_mux_alloc(unit, 0, sec_hw_info->num_mux8,
                                             ((cont_size == 8) ? 1 : 2),
                                             sec_hw_info->is_mux8_msb,
                                             mux8_bitmap, &mux_id);

                    /*
                     * Failure in 8bit Mux is acceptable.
                     * It can be accommodated in 16bit Mux.
                     */
                    if (SHR_FAILURE(rv)) {
                        continue;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_pdd_mux_sel_cont_offset_update(
                                unit,
                                mux_id,
                                sec_hw_info->num_mux8,
                                ((cont_size == 8) ? 1 : 2), 8,
                                mux8_start,
                                pdd_cfg_info->pdd_hw_info->raw_policy_width,
                                sec_hw_info->is_mux8_msb,
                                sec_hw_info->section_start_bit[sec_id],
                                sec_hw_info->mux8_mul,
                                mux_info.s1_mux8_sel,
                                mux_cont_ptr));

                    SHR_BITSET(cont_bitmap, idx);
                    valid_pdd_data = TRUE;
                    mux8_size += cont_size;
                    if (cont_size == 16) {
                        mux8_count += 2;
                    } else {
                        mux8_count++;
                    }
                } else {
                    /* Currently only one part is supported. */
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }

    /* Check if there are any holes left in between 8Bit Mux section. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_mux_hole_count_get(unit, 0, sec_hw_info->num_mux8,
                                      mux8_count,
                                      sec_hw_info->is_mux8_msb,
                                      mux8_bitmap, &hole_count));
    mux8_count += hole_count;
    mux8_size += (hole_count * 8);
    rem_policy_width -= mux8_size;

    if (sec_hw_info->is_mux8_msb == sec_hw_info->is_mux16_msb) {
        total_bytes = sec_hw_info->num_mux8 - mux8_count;
        mux16_start = mux8_size + mux8_start;
        /*
         * Only 1byte align can be done.
         * Remaining unused 8bit will be holes.
         */
        if (total_bytes & 1) {
            mux_info.s1_mux16_align = 1;
            total_bytes--;
        }
        mux16_start += (total_bytes * 8);
        rem_policy_width -= (total_bytes * 8);
    } else {
        /* 8bit and 16bit are aligned at the same side */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Reserve 16bit Mux's which point to data bits
     * already allocated to 4Bit Mux.
     */
    if (sec_hw_info->is_mux4_msb == sec_hw_info->is_mux8_msb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_dst_mux_reserve(unit, sec_hw_info->is_mux8_msb,
                                       4, mux4_start, sec_hw_info->num_mux4,
                                       mux4_bitmap,
                                       16, mux16_start, sec_hw_info->num_mux16,
                                       mux16_bitmap, &mux16_count));
    }
    mux16_size = mux16_count * 16;

    if ((rem_policy_width - mux16_size) < 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Process 16Bit Mux. */
    for (idx = 0;
         idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {

        /* Skip container is already allocated */
        if (SHR_BITGET(cont_bitmap, idx)) {
            continue;
        }

        if (SHR_BITGET(all_container_bitmap, idx)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                       sec_hw_info,
                                                       idx,
                                                       &sec_id));
            /* Skip non-16Bit Sections */
            if (sec_id != sec_hw_info->mux16_sec_id) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_mux_cont_ptr_get(unit, cont_list, idx, &mux_cont_ptr));

            if ((rem_policy_width - 16 - mux16_size) >= 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_mux_alloc(unit, 0, sec_hw_info->num_mux16, 1,
                                         sec_hw_info->is_mux16_msb,
                                         mux16_bitmap, &mux_id));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_mux_sel_cont_offset_update(
                            unit,
                            mux_id,
                            sec_hw_info->num_mux16,
                            1, 16,
                            mux16_start,
                            pdd_cfg_info->pdd_hw_info->raw_policy_width,
                            sec_hw_info->is_mux16_msb,
                            sec_hw_info->section_start_bit[sec_id],
                            sec_hw_info->mux16_mul,
                            mux_info.s1_mux16_sel,
                            mux_cont_ptr));

                SHR_BITSET(cont_bitmap, idx);
                valid_pdd_data = TRUE;
                mux16_size += 16;
                mux16_count++;
            } else {
                /* Currently only one part is supported. */
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    /* Check if there are any holes left in between 16Bit Mux section. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_mux_hole_count_get(unit, 0, sec_hw_info->num_mux16,
                                      mux16_count,
                                      sec_hw_info->is_mux16_msb,
                                      mux16_bitmap, &hole_count));
    mux16_size += (hole_count * 16);
    rem_policy_width -= mux16_size;

    if (rem_policy_width < 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Verify all containers are allocated. */
    for (idx = 0; idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {
        if (SHR_BITGET(all_container_bitmap, idx) &&
            !SHR_BITGET(cont_bitmap, idx)) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    /* Update reserved and unused MUX's to point to unused container. */
    for (idx = 0;
         idx < pdd_cfg_info->pdd_hw_info->pdd_bitmap_size; idx++) {
        if (SHR_BITGET(all_container_bitmap, idx) == 0) {
            if ((idx >=
                 sec_hw_info->section_start_bit[sec_hw_info->mux4_sec_id]) &&
                (idx <=
                 sec_hw_info->section_end_bit[sec_hw_info->mux4_sec_id])) {
                if (unused_sel[0] == BCMFP_EXT_SELCODE_DONT_CARE) {
                    unused_sel[0] = idx -
                      sec_hw_info->section_start_bit[sec_hw_info->mux4_sec_id];
                }
            } else {
                if (unused_sel[1] == BCMFP_EXT_SELCODE_DONT_CARE) {
                    unused_sel[1] = idx -
                      sec_hw_info->section_start_bit[sec_hw_info->mux8_sec_id];
                }
            }
        }
        if (unused_sel[0] != BCMFP_EXT_SELCODE_DONT_CARE &&
            unused_sel[1] != BCMFP_EXT_SELCODE_DONT_CARE) {
            break;
        }
    }

    for (mux_id = 0; mux_id < sec_hw_info->num_mux4; mux_id++) {
        if (mux_info.s0_mux4_sel[mux_id] == BCMFP_EXT_SELCODE_DONT_CARE) {
            mux_info.s0_mux4_sel[mux_id] = unused_sel[0];
            if (unused_sel[0] == BCMFP_EXT_SELCODE_DONT_CARE) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    for (mux_id = 0; mux_id < sec_hw_info->num_mux8; mux_id++) {
        if (mux_info.s1_mux8_sel[mux_id] == BCMFP_EXT_SELCODE_DONT_CARE) {
            mux_info.s1_mux8_sel[mux_id] = unused_sel[1] * 2;
            if (unused_sel[1] == BCMFP_EXT_SELCODE_DONT_CARE) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    for (mux_id = 0; mux_id < sec_hw_info->num_mux16; mux_id++) {
        if (mux_info.s1_mux16_sel[mux_id] == BCMFP_EXT_SELCODE_DONT_CARE) {
            mux_info.s1_mux16_sel[mux_id] = unused_sel[1];
            if (unused_sel[1] == BCMFP_EXT_SELCODE_DONT_CARE) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }

    if (valid_pdd_data == TRUE) {
        *num_parts = 1;
    }

    if (!(flags & BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED)) {

        BCMFP_ALLOC(pdd_oper_info,
                sizeof(bcmfp_pdd_oper_t),
                "bcmfpPddOperPartInfo");

        sal_memcpy(pdd_oper_info->container_bitmap, cont_bitmap,
                        BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));

        sal_memcpy(&(pdd_oper_info->section_info), &(section_info),
                        sizeof(bcmfp_pdd_section_info_t));

        sal_memcpy(&(pdd_oper_info->mux_info), &(mux_info),
                        sizeof(bcmfp_pdd_mux_info_t));

        pdd_oper_info->next = NULL;
        *pdd_operational_info = pdd_oper_info;
    }

exit:
     /*
     * Caller function is responsible for
     * releasing memory used for pdd_operational_info
     * by calling bcmfp_pdd_oper_info_free function after it's
     * use of the structure.
     */
    /* coverity[leaked_storage] */
    /* coverity[resource_leak : FALSE] */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calcualte the final bus offsets for the actions added
 *  in PDD LT entry. Also update the action offsets to the
 *  PDD oper info corresponding to PDD LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] PDD oper info.
 * \param [in] Container linked list based on entry LT actions.
 * \param [in] num PDD parts.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_calc_action_offsets(int unit,
                              bcmfp_pdd_cfg_t *pdd_cfg_info,
                              bcmfp_pdd_oper_t **pdd_operational_info,
                              bcmfp_pdd_mux_cont_info_t *cont_list,
                              uint8_t *num_parts)
{
    uint16_t idx = 0;
    uint16_t id = 0;
    uint8_t part = 0;
    uint8_t sec_id = 0;
    bcmfp_pdd_section_info_t section_info[BCMFP_ENTRY_PARTS_MAX];
    uint32_t cont_bitmap[BCMFP_ENTRY_PARTS_MAX][BCMFP_MAX_PDD_WORDS];
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_action_oper_info_t act_oper_info[BCMFP_ENTRY_PARTS_MAX];
    uint16_t cont_id = 0;
    uint16_t cont_offset = 0;
    uint8_t num_valid_offset = 0;
    bcmfp_action_cfg_t **action_cfg;
    bcmfp_pdd_hw_section_info_t *sec_hw_info;
    bcmfp_pdd_hw_info_t *pdd_hw_info;
    bcmfp_action_cfg_db_t  *action_cfg_db;
    bcmfp_action_t act_id;
    bcmfp_action_oper_info_t *pdd_act_oper_info = NULL;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bool section_write_from_lsb;
    bool section_bitmap_read_from_lsb;
    uint8_t section_offset_gran;
    uint8_t section_offset;
    bcmfp_pdd_mux_cont_info_t *mux_cont_ptr;

    SHR_FUNC_ENTER(unit);

    action_cfg = pdd_cfg_info->action_cfg_db->action_cfg;
    action_cfg_db = pdd_cfg_info->action_cfg_db;
    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;
    pdd_hw_info = pdd_cfg_info->pdd_hw_info;
    pdd_oper_info = *pdd_operational_info;

    for (part = 0; part < *num_parts; part++) {
        sal_memcpy(&(cont_bitmap[part]), &(pdd_oper_info->container_bitmap),
                            BCMFP_MAX_PDD_WORDS * sizeof(uint32_t));
        sal_memcpy(&(section_info[part]), &(pdd_oper_info->section_info),
                            sizeof(bcmfp_pdd_section_info_t));
        pdd_oper_info->action_oper_info = NULL;
        pdd_oper_info = pdd_oper_info->next;

    }

    section_write_from_lsb =
        pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb;

    section_bitmap_read_from_lsb =
        pdd_cfg_info->pdd_hw_info->read_pdd_bitmap_from_lsb;

    section_offset_gran = 8;

    for (id = 0; id < pdd_cfg_info->num_actions; id++) {

        sal_memset(&act_oper_info, 0,
                   BCMFP_ENTRY_PARTS_MAX * sizeof(bcmfp_action_oper_info_t));

        /* Get the action */
        act_id = pdd_cfg_info->pdd_action[id];
        if (action_cfg[act_id] == NULL) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        if (action_cfg[act_id]->flags & BCMFP_ACTION_NOT_VALID) {
           /* Action is not mapped to physical containers
            * return un avail error in this case.
            */
            LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit, "BCMFP: Action is not"
                " mapped to physical container.Action: %s\n"),
                action_cfg[act_id]->action_name));
            continue;
        }

        /*
         * Get the action offset's associated
         * with this action.
         */
        a_offset = action_cfg[act_id]->offset_cfg;

        num_valid_offset = 0;
        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (a_offset.width[idx] > 0 ) {

                cont_id = a_offset.phy_cont_id[idx];

                /*
                 * Get the part number in which the current container
                 * is present.
                 */
                for (part = 0; part < *num_parts; part++) {
                    if (!(SHR_BITGET(cont_bitmap[part], cont_id))) {
                        continue;
                    } else {
                        cont_offset = 0;

                        if (pdd_hw_info->pdd_data_full_mux_format) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmfp_mux_cont_ptr_get(unit, cont_list,
                                                        cont_id,
                                                        &mux_cont_ptr));
                            cont_offset = (mux_cont_ptr->offset
                                           - mux_cont_ptr->offset_adjust);
                        } else {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                                sec_hw_info,
                                                                cont_id,
                                                                &sec_id));
                            if (pdd_hw_info->pdd_data_mixed_msb_lsb) {
                                section_write_from_lsb =
                                    sec_hw_info->section_is_lsb[sec_id];
                                section_bitmap_read_from_lsb =
                                    sec_hw_info->section_is_lsb[sec_id];
                                section_offset_gran =
                                    sec_hw_info->section_align_bits[sec_id];
                            }
                            section_offset =
                                section_info[part].section_offset[sec_id];
                            if (TRUE == section_write_from_lsb) {
                                cont_offset +=
                                    section_offset * section_offset_gran;
                            } else {
                                cont_offset +=
                                    (pdd_hw_info->raw_policy_width
                                    - (pdd_cfg_info->raw_data_offset)
                                    - (section_offset * section_offset_gran)
                                    - (action_cfg_db->container_size[cont_id]));
                            }
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmfp_pdd_get_cont_offset_within_section(unit,
                                      section_bitmap_read_from_lsb,
                                      pdd_cfg_info->action_cfg_db,
                                      sec_hw_info,
                                      cont_id,
                                      sec_id,
                                      cont_bitmap[part],
                                      &cont_offset));
                        }


                        act_oper_info[part].action = act_id;

                        /*
                         * To indicate number of valid offsets
                         * for the given part
                         */
                        act_oper_info[part].num_offsets++;

                        /*
                         * Update the container info in corresponding
                         * part's action offset structure
                         */
                        act_oper_info[part].offset[num_valid_offset] =
                                    cont_offset + a_offset.offset[idx];
                        act_oper_info[part].width[num_valid_offset] =
                                    a_offset.width[idx];

                        /* Total number of valid offsets across parts */
                        num_valid_offset++;
                    }
                }
            }
        }
        pdd_oper_info = *pdd_operational_info;

        /* Update the pdd oper info with action offsets */
        for (part = 0; part < *num_parts; part++) {

            if (act_oper_info[part].num_offsets > 0) {
                BCMFP_ALLOC(pdd_act_oper_info,
                    sizeof(bcmfp_action_oper_info_t),
                    "bcmfpActionOperInfo");

                /*
                 * Update total number of valid offsets instead of
                 * part offsets in num_offsets.
                 */

                act_oper_info[part].num_offsets = num_valid_offset;

                sal_memcpy(pdd_act_oper_info, &(act_oper_info[part]),
                                sizeof(bcmfp_action_oper_info_t));

                /* Update the action offsets in pdd oper info */
                pdd_act_oper_info->next = pdd_oper_info->action_oper_info;
                pdd_oper_info->action_oper_info = pdd_act_oper_info;
                pdd_act_oper_info = NULL;

            }
            pdd_oper_info = pdd_oper_info->next;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calcualte Container bitmap, Section Offsets,
 *  and the final bus offsets for the actions added
 *  in PDD LT entry. Also update the action offsets to the
 *  PDD oper info corresponding to PDD LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] PDD oper info
 * \param [out] Number of PDD parts required.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 *
 * NOTE: Memory for pdd_oper_info is allocated in the
 * below function. Caller function is responsible for
 * freeing the memory after their use of it. Below fucntion
 * needs to be called for freeing the memory used for pdd_oper_info.
 * Function: bcmfp_pdd_oper_info_free
 */
int
bcmfp_pdd_cfg_process(int unit,
                      bcmfp_pdd_cfg_t *pdd_cfg_info,
                      bcmfp_pdd_oper_t **pdd_oper_info,
                      uint8_t *num_parts)
{
    uint32_t all_container_bitmap[BCMFP_MAX_PDD_WORDS] = {0};
    uint32_t flags = 0;
    bcmfp_pdd_mux_cont_info_t *cont_info = NULL;
    bcmfp_pdd_mux_cont_info_t *next = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_generate_container_bitmap(unit,
                                             pdd_cfg_info,
                                             all_container_bitmap,
                                             &cont_info));

    if (pdd_cfg_info->pdd_hw_info->pdd_data_full_mux_format) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_full_mux(unit,
                                                 flags,
                                                 pdd_cfg_info,
                                                 all_container_bitmap,
                                                 cont_info,
                                                 pdd_oper_info,
                                                 num_parts));
    }
    /*
     * Considering sections s0, s1, s2 and s3 from LSB to MSB
     * in policy raw data
     */
    else if (pdd_cfg_info->pdd_hw_info->read_pdd_bitmap_from_lsb) {

        if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_bitmap_lsb_to_data_lsb(unit,
                                                         flags,
                                                         pdd_cfg_info,
                                                         all_container_bitmap,
                                                         pdd_oper_info,
                                                         num_parts));
        }
    } else if (pdd_cfg_info->pdd_hw_info->pdd_data_mixed_msb_lsb) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_gen_sec_info_bitmap_mixed_msb_lsb_data(unit,
                                                     flags,
                                                     pdd_cfg_info,
                                                     all_container_bitmap,
                                                     pdd_oper_info,
                                                     num_parts));

    } else if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_lsb(unit,
                                                     flags,
                                                     pdd_cfg_info,
                                                     all_container_bitmap,
                                                     pdd_oper_info,
                                                     num_parts));
    } else {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_msb(unit,
                                                     flags,
                                                     pdd_cfg_info,
                                                     all_container_bitmap,
                                                     pdd_oper_info,
                                                     num_parts));
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_pdd_calc_action_offsets(unit,
                                      pdd_cfg_info,
                                      pdd_oper_info,
                                      cont_info,
                                      num_parts));
exit:
    while (cont_info) {
        next = cont_info->next;
        SHR_FREE(cont_info);
        cont_info = next;
    }
    SHR_FUNC_EXIT();

}

int bcmfp_pdd_oper_info_free(int unit,
                             uint8_t num_parts,
                             bcmfp_pdd_oper_t *pdd_operational_info)
{
    uint8_t part =0;
    bcmfp_action_oper_info_t *act_info = NULL;
    bcmfp_action_oper_info_t *act_prev_info = NULL;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bcmfp_pdd_oper_t *temp_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    pdd_oper_info = pdd_operational_info;

    if (pdd_operational_info == NULL) {
        SHR_EXIT();
    }
   /*
    *    COVERITY
    *
    *    In case the act_info is defined, the value of
    *    act_info is copied to act_prev_info, which is being freed
    *    and new value is assigned to act_info.
    *    so it's a false alarm that
    *    the value is being overwritten before use
    */
    do {
        /*   coverity[value_overwrite : FALSE]    */
        act_info = pdd_oper_info->action_oper_info;
        while (NULL != act_info) {
            act_prev_info = act_info;
            act_info = act_info->next;
            SHR_FREE(act_prev_info);
        }
        temp_oper_info = pdd_oper_info;
        pdd_oper_info = pdd_oper_info->next;

        SHR_FREE(temp_oper_info);
        part++;
    } while (NULL != pdd_oper_info);

    if (part == num_parts) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
    SHR_FUNC_EXIT()
}
