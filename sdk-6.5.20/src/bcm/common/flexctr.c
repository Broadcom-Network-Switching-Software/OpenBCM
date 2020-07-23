/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flexctr.h>
#include <sal/core/libc.h>

/*!
 * \breif Flex counter quantization data structure initialization function.
 *
 * This function is used to initialize a quantization data structure.
 *
 * \param [in/out] quantization Flex counter quantization data structure.
 */
void
bcm_flexctr_quantization_t_init(bcm_flexctr_quantization_t *quant_cfg)
{
    if (quant_cfg != NULL) {
        sal_memset(quant_cfg, 0, sizeof(bcm_flexctr_quantization_t));
    }
}

/*!
 * \brief Flex counter action data structure initialization function.
 *
 * This function is used to initialize a packet/byte action data structure.
 *
 * \param [in/out] action Flex counter action data structure.
 */
void
bcm_flexctr_action_t_init(bcm_flexctr_action_t *act_cfg)
{
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL, *value_b_op = NULL;

    if (act_cfg != NULL) {
        sal_memset(act_cfg, 0, sizeof(bcm_flexctr_action_t));

        act_cfg->mode = bcmFlexctrCounterModeNormal;
        act_cfg->index_num = 1;

        /* Counter index is always zero. */
        index_op = &act_cfg->index_operation;
        index_op->object[0] = bcmFlexctrObjectConstZero;
        index_op->mask_size[0] = 16;
        index_op->shift[0] = 0;
        index_op->object[1] = bcmFlexctrObjectConstZero;
        index_op->mask_size[1] = 16;
        index_op->shift[1] = 0;

        /* Increase counter per packet. */
        value_a_op = &act_cfg->operation_a;
        value_a_op->select = bcmFlexctrValueSelectCounterValue;
        value_a_op->object[0] = bcmFlexctrObjectConstOne;
        value_a_op->mask_size[0] = 16;
        value_a_op->shift[0] = 0;
        value_a_op->object[1] = bcmFlexctrObjectConstZero;
        value_a_op->mask_size[1] = 16;
        value_a_op->shift[1] = 0;
        value_a_op->type = bcmFlexctrValueOperationTypeInc;

        /* Increase counter per packet bytes. */
        value_b_op = &act_cfg->operation_b;
        value_b_op->select = bcmFlexctrValueSelectPacketLength;
        value_b_op->type = bcmFlexctrValueOperationTypeInc;
    }
}

/*!
 * \brief Flex counter group action data structure initialization function.
 *
 * This function is used to initialize group action data structure.
 *
 * \param [in/out] group_action Flex counter group action data structure.
 */
void
bcm_flexctr_group_action_t_init(bcm_flexctr_group_action_t *group_action)
{
    if (group_action != NULL) {
        sal_memset(group_action, 0, sizeof(bcm_flexctr_group_action_t));
    }
}

/*!
 * \brief Flex counter packet attribute selector data structure initialization
 * function.
 *
 * This function is used to initialize a packet attribute selector
 * data structure.
 *
 * \param [in/out] attr_selector Flex counter packet attribute selector
 *                               data structure.
 */
void
bcm_flexctr_packet_attribute_selector_t_init(
    bcm_flexctr_packet_attribute_selector_t *attr_selector)
{
    if (attr_selector != NULL) {
        sal_memset(attr_selector, 0,
                   sizeof(bcm_flexctr_packet_attribute_selector_t));
    }
}

/*!
 * \brief Flex counter packet attribute profile data structure initialization
 * function.
 *
 * This function is used to initialize a packet attribute profile
 * data structure.
 *
 * \param [in/out] attr_profile Flex counter packet attribute profile
 *                              data structure.
 */
void
bcm_flexctr_packet_attribute_profile_t_init(
    bcm_flexctr_packet_attribute_profile_t *attr_profile)
{
    if (attr_profile != NULL) {
        sal_memset(attr_profile, 0,
                   sizeof(bcm_flexctr_packet_attribute_profile_t));
    }
}

/*!
 * \brief Flex counter packet attribute map entry data structure initialization
 * function.
 *
 * This function is used to initialize a packet attribute map entry
 * data structure.
 *
 * \param [in/out] attr_map_entry Flex counter packet attribute map entry
 *                                data structure.
 */
void
bcm_flexctr_packet_attribute_map_entry_t_init(
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entry)
{
    if (attr_map_entry != NULL) {
        sal_memset(attr_map_entry, 0,
                   sizeof(bcm_flexctr_packet_attribute_map_entry_t));
    }
}

/*!
 * \brief Flex counter packet attribute map data structure initialization
 * function.
 *
 * This function is used to initialize a packet attribute map
 * data structure.
 *
 * \param [in/out] attr_map Flex counter packet attribute map
 *                          data structure.
 */
void
bcm_flexctr_packet_attribute_map_t_init(
    bcm_flexctr_packet_attribute_map_t *attr_map)
{
    if (attr_map != NULL) {
        sal_memset(attr_map, 0,
                   sizeof(bcm_flexctr_packet_attribute_map_t));
    }
}
