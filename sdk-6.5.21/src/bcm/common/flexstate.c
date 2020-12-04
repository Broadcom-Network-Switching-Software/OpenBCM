/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flexstate.h>
#include <sal/core/libc.h>

/*!
 * \breif Flex state quantization data structure initialization function.
 *
 * This function is used to initialize a quantization data structure.
 *
 * \param [in/out] quantization Flex state quantization data structure.
 */
void
bcm_flexstate_quantization_t_init(bcm_flexstate_quantization_t *quant_cfg)
{
    if (quant_cfg != NULL) {
        sal_memset(quant_cfg, 0, sizeof(bcm_flexstate_quantization_t));
    }
}

/*!
 * \brief Flex state action data structure initialization function.
 *
 * This function is used to initialize a packet/byte action data structure.
 *
 * \param [in/out] action Flex state action data structure.
 */
void
bcm_flexstate_action_t_init(bcm_flexstate_action_t *act_cfg)
{
    if (act_cfg != NULL) {
        sal_memset(act_cfg, 0, sizeof(bcm_flexstate_action_t));
    }
}

/*!
 * \brief Flex state group action data structure initialization function.
 *
 * This function is used to initialize group action data structure.
 *
 * \param [in/out] group_action Flex state group action data structure.
 */
void
bcm_flexstate_group_action_t_init(bcm_flexstate_group_action_t *group_action)
{
    if (group_action != NULL) {
        sal_memset(group_action, 0, sizeof(bcm_flexstate_group_action_t));
    }
}
