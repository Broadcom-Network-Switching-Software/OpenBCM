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
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_generate_container_bitmap(int unit,
                              bcmfp_pdd_cfg_t *pdd_cfg_info,
                              uint32_t *container_bmp)
{
    uint16_t idx = 0;
    uint16_t id = 0;
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_action_cfg_t **action_cfg;
    bcmfp_action_t action_id = 0;

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
    uint8_t fill_bits =0;
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

                    /* Fill bits needed to make section offset byte boundary*/
                    fill_bits = (actual_policy_width %
                                (pdd_cfg_info->pdd_hw_info->section_align_bits));
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
                         */
                        section_info[part].section_offset[sec_id] =
                                ((pdd_cfg_info->pdd_hw_info->raw_policy_width
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
 * \brief Calcualte the least possible group mode to accomodate the
 *  actions added in PDD profile LT. Also update the part container
 *  bitmap and section offsets to the PDD oper info corresponding LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [out] Number of parts needed to accomodate all PDD LT entry actions
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
int
bcmfp_pdd_oper_num_parts_get(int unit,
                             bcmfp_pdd_cfg_t *pdd_cfg_info,
                             uint8_t *num_parts)
{
    uint32_t all_container_bitmap[BCMFP_MAX_PDD_WORDS] = {0};
    uint32_t flags = 0;
    bcmfp_pdd_oper_t *pdd_info = NULL;
    bcmfp_pdd_oper_t *pdd_temp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_generate_container_bitmap(unit,
                                             pdd_cfg_info,
                                             all_container_bitmap));

    flags = BCMFP_COMPUTE_ONLY_PDD_PARTS_REQUIRED;

    /*
     * Considering sections s0, s1, s2 and s3 from LSB to MSB
     * in policy raw data
     */
    if (pdd_cfg_info->pdd_hw_info->read_pdd_bitmap_from_lsb) {

        if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_bitmap_lsb_to_data_lsb(unit,
                                                         flags,
                                                         pdd_cfg_info,
                                                         all_container_bitmap,
                                                         &pdd_info,
                                                         num_parts));
        }
    } else {

        if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_lsb(unit,
                                                         flags,
                                                         pdd_cfg_info,
                                                         all_container_bitmap,
                                                         &pdd_info,
                                                         num_parts));
        } else {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_bitmap_msb_to_data_msb(unit,
                                                         flags,
                                                         pdd_cfg_info,
                                                         all_container_bitmap,
                                                         &pdd_info,
                                                         num_parts));
        }
    }

exit:
    while (pdd_info != NULL) {
        pdd_temp = pdd_info;
        pdd_info = pdd_info->next;
        SHR_FREE(pdd_temp);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calcualte the final bus offsets for the actions added
 *  in PDD LT entry. Also update the action offsets to the
 *  PDD oper info corresponding to PDD LT entry.
 *
 * \param [in] unit Logical device id
 * \param [in] PDD Config Info
 * \param [in] num PDD parts.
 * \param [out] PDD oper info.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_pdd_calc_action_offsets(int unit,
                              bcmfp_pdd_cfg_t *pdd_cfg_info,
                              bcmfp_pdd_oper_t **pdd_operational_info,
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
    bcmfp_action_t act_id;
    bcmfp_action_oper_info_t *pdd_act_oper_info = NULL;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    bool section_write_from_lsb;
    bool section_bitmap_read_from_lsb;
    uint8_t section_offset_gran;

    SHR_FUNC_ENTER(unit);

    action_cfg = pdd_cfg_info->action_cfg_db->action_cfg;
    sec_hw_info = pdd_cfg_info->pdd_hw_info->section_hw_info;
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

                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmfp_pdd_get_section_id_from_cont_id(unit,
                                                                   sec_hw_info,
                                                                   cont_id,
                                                                   &sec_id));
                        if (pdd_cfg_info->pdd_hw_info->pdd_data_mixed_msb_lsb) {
                            section_write_from_lsb =
                                sec_hw_info->section_is_lsb[sec_id];
                            section_bitmap_read_from_lsb =
                                sec_hw_info->section_is_lsb[sec_id];
                            section_offset_gran =
                                sec_hw_info->section_align_bits[sec_id];
                        }
                        if (TRUE == section_write_from_lsb) {
                            cont_offset +=
                                (section_info[part].section_offset[sec_id]) * section_offset_gran;
                        } else {
                            cont_offset +=
                                (pdd_cfg_info->pdd_hw_info->raw_policy_width
                                - (pdd_cfg_info->raw_data_offset)
                                - ((section_info[part].section_offset[sec_id]) *
                                   section_offset_gran)
                                - (pdd_cfg_info->action_cfg_db->container_size[cont_id]));
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

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_generate_container_bitmap(unit,
                                             pdd_cfg_info,
                                             all_container_bitmap));

    /*
     * Considering sections s0, s1, s2 and s3 from LSB to MSB
     * in policy raw data
     */
    if (pdd_cfg_info->pdd_hw_info->read_pdd_bitmap_from_lsb) {

        if (pdd_cfg_info->pdd_hw_info->write_pdd_data_from_lsb) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_gen_sec_info_bitmap_lsb_to_data_lsb(unit,
                                                         flags,
                                                         pdd_cfg_info,
                                                         all_container_bitmap,
                                                         pdd_oper_info,
                                                         num_parts));
        }
    } else {

        if (pdd_cfg_info->pdd_hw_info->pdd_data_mixed_msb_lsb) {

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
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_pdd_calc_action_offsets(unit,
                                      pdd_cfg_info,
                                      pdd_oper_info,
                                      num_parts));
exit:
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
