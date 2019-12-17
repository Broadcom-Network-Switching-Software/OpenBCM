/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.14 routines
 */

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6514.h>

#ifdef INCLUDE_TCB
/*
 * Function:
 *      _bcm_compat6514in_cosq_tcb_config_t
 * Purpose:
 *      Convert the bcm_cosq_tcb_config_t datatype from <=6.5.14 to
 *      SDK 6.5.14+
 * Parameters:
 *      from        - (IN) The <=6.5.14 version of the datatype
 *      to          - (OUT) The SDK 6.5.14+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514in_cosq_tcb_config_t(
    bcm_compat6514_cosq_tcb_config_t *from,
    bcm_cosq_tcb_config_t *to)
{
    bcm_cosq_tcb_config_t_init(to);
    to->scope_type = from->scope_type;
    to->gport = from->gport;
    to->cosq = from->cosq;
    to->trigger_reason = from->trigger_reason;
    to->pre_freeze_capture_num = from->pre_freeze_capture_num;
    to->pre_freeze_capture_time = from->pre_freeze_capture_time;
    to->post_sample_probability = from->post_sample_probability;
    to->pre_sample_probability = from->pre_sample_probability;
}

/*
 * Function:
 *      _bcm_compat6514out_cosq_tcb_config_t
 * Purpose:
 *      Convert the bcm_cosq_tcb_config_t datatype from 6.5.14+ to
 *      <=SDK 6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.14+ version of the datatype
 *      to          - (OUT) The <=6.5.14 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514out_cosq_tcb_config_t(
    bcm_cosq_tcb_config_t *from,
    bcm_compat6514_cosq_tcb_config_t *to)
{
    to->scope_type = from->scope_type;
    to->gport = from->gport;
    to->cosq = from->cosq;
    to->trigger_reason = from->trigger_reason;
    to->pre_freeze_capture_num = from->pre_freeze_capture_num;
    to->pre_freeze_capture_time = from->pre_freeze_capture_time;
    to->post_sample_probability = from->post_sample_probability;
    to->pre_sample_probability = from->pre_sample_probability;
}

/*
 * Function:
 *      bcm_compat6514_cosq_tcb_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tcb_config_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_compat6514_cosq_tcb_config_get(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6514_cosq_tcb_config_t *config)
{
    int rv;
    bcm_cosq_tcb_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_cosq_tcb_config_t*)
            sal_alloc(sizeof(bcm_cosq_tcb_config_t), "New cosq tcb config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_cosq_tcb_config_t(config, new_config);
    }

    rv = bcm_cosq_tcb_config_get(unit, buffer_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6514out_cosq_tcb_config_t(new_config, config);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_cosq_tcb_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tcb_config_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_compat6514_cosq_tcb_config_set(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6514_cosq_tcb_config_t *config)
{
    int rv;
    bcm_cosq_tcb_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_cosq_tcb_config_t*)
            sal_alloc(sizeof(bcm_cosq_tcb_config_t), "New cosq tcb config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_cosq_tcb_config_t(config, new_config);
    }

    rv = bcm_cosq_tcb_config_set(unit, buffer_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6514out_cosq_tcb_config_t(new_config, config);
        sal_free(new_config);
    }

    return rv;
}
#endif
#endif /* BCM_RPC_SUPPORT */
