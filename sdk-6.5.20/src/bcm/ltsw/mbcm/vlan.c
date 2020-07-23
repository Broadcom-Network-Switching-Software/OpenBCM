/*! \file vlan.c
 *
 * BCM VLAN module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/vlan.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

static mbcm_ltsw_vlan_drv_t mbcm_ltsw_vlan_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_vlan_drv_set(int unit, mbcm_ltsw_vlan_drv_t *drv)
{
    mbcm_ltsw_vlan_drv_t *local = &mbcm_ltsw_vlan_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_vlan_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_otpid_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_otpid_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_otpid_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_otpid_add(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_otpid_add(unit, index, tpid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_payload_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_payload_otpid_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_payload_otpid_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_payload_otpid_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_payload_otpid_add(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_payload_otpid_add(unit, index, tpid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_itpid_set(int unit, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_itpid_set(unit, tpid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_itpid_get(int unit, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_itpid_get(unit, tpid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_add(int unit,
                        bcm_vlan_t vid,
                        bcm_pbmp_t ing_pbmp,
                        bcm_pbmp_t egr_pbmp,
                        bcm_pbmp_t egr_ubmp,
                        int flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_add(unit, vid,
                                                ing_pbmp, egr_pbmp,
                                                egr_ubmp, flags));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_get(int unit,
                        bcm_vlan_t vid,
                        bcm_pbmp_t *ing_pbmp,
                        bcm_pbmp_t *egr_pbmp,
                        bcm_pbmp_t *egr_ubmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_get(unit, vid, ing_pbmp,
                                                egr_pbmp, egr_ubmp));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_remove(int unit,
                           bcm_vlan_t vid,
                           bcm_pbmp_t ing_pbmp,
                           bcm_pbmp_t egr_pbmp,
                           bcm_pbmp_t egr_ubmp)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_remove(unit, vid, ing_pbmp,
                                                   egr_pbmp, egr_ubmp));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_check_set(int unit, bcm_port_t port,
                              int disable, uint16_t flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_check_set(unit, port,
                                                      disable, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_control_vlan_set(int unit, bcm_vlan_t vid, uint32_t valid_fields,
                                bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_vlan_set(unit, vid,
                                                        valid_fields,
                                                        control));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_vlan_control_vlan_get(int unit, bcm_vlan_t vid, uint32_t valid_fields,
                                bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_vlan_get(unit, vid,
                                                        valid_fields,
                                                        control));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_control_port_set(int unit,
                                bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_port_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_control_port_get(int unit,
                                bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_port_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_add(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_add(unit, key_type,
                                                     port, outer_vlan,
                                                     inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_update(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_update(unit, key_type,
                                                        port, outer_vlan,
                                                        inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_get(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_get(unit, key_type,
                                                     port, outer_vlan,
                                                     inner_vlan, cfg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_delete(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_delete(unit, key_type,
                                                        port, outer_vlan,
                                                        inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_traverse(int unit,
                                  bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                                  void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_action_add(int unit,
                                    bcm_vlan_translate_key_t key_type,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_action_add(unit, key_type,
                                                            port, outer_vlan,
                                                            inner_vlan,
                                                            action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_action_get(int unit,
                                    bcm_vlan_translate_key_t key_type,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_action_get(unit, key_type,
                                                            port, outer_vlan,
                                                            inner_vlan,
                                                            action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_action_delete(int unit,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_action_delete(unit, key_type,
                                                               port, outer_vlan,
                                                               inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_action_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_action_traverse(unit, cb,
                                                                 user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_action_add(int unit,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_action_add(unit, port,
                                                            outer_vlan,
                                                            inner_vlan,
                                                            action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_action_get(int unit,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_action_get(unit, port,
                                                            outer_vlan,
                                                            inner_vlan,
                                                            action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_action_delete(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_action_delete(unit, port,
                                                               outer_vlan,
                                                               inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_egress_action_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_action_traverse(unit, cb,
                                                                 user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_force_port_set(int unit, int vlan, int port_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_force_port_set(unit, vlan, port_type));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_force_port_get(unit, vlan, port_type));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_action_set(int unit,
                               bcm_port_t port,
                               bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_action_set(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_action_get(int unit,
                               bcm_port_t port,
                               bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_action_get(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_port_action_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_port_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_egr_vlan_port_action_set(int unit,
                                   bcm_port_t port,
                                   bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].egr_vlan_port_action_set(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_egr_vlan_port_action_get(int unit,
                                   bcm_port_t port,
                                   bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].egr_vlan_port_action_get(unit, port, action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_egr_vlan_port_action_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].egr_vlan_port_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_create(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_create(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_destroy(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_stat_attach(int unit,
                           bcm_vlan_t vid,
                           bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!mbcm_ltsw_vlan_drv[unit].vlan_stat_attach) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_stat_attach(unit, vid, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_stat_detach(int unit,
                           bcm_vlan_t vid,
                           bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!mbcm_ltsw_vlan_drv[unit].vlan_stat_detach) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_stat_detach(unit, vid, info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_flexctr_attach(int unit,
                                        bcm_vlan_translate_key_t key_type,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_flexctr_attach(
             unit, key_type, port, outer_vlan, inner_vlan, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_flexctr_detach(int unit,
                                        bcm_vlan_translate_key_t key_type,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_flexctr_detach(
             unit, key_type, port, outer_vlan, inner_vlan, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_xlate_flexctr_get(int unit,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_gport_t port,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan,
                                     int array_size,
                                     bcm_vlan_flexctr_config_t *config_array,
                                     int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_xlate_flexctr_get(
             unit, key_type, port, outer_vlan, inner_vlan,
             array_size, config_array, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_flexctr_attach(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_flexctr_attach(
             unit, port, outer_vlan, inner_vlan, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_flexctr_detach(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan,
                                        bcm_vlan_t inner_vlan,
                                        bcm_vlan_flexctr_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_flexctr_detach(
             unit, port, outer_vlan, inner_vlan, config));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_xlate_flexctr_get(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan,
                                     int array_size,
                                     bcm_vlan_flexctr_config_t *config_array,
                                     int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_xlate_flexctr_get(
             unit, port, outer_vlan, inner_vlan,
             array_size, config_array, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_stat_get(int unit,
                        bcm_vlan_t vid,
                        bcmi_ltsw_flexctr_counter_info_t *info,
                        uint32_t *num_info)
{
    SHR_FUNC_ENTER(unit);

    if (!mbcm_ltsw_vlan_drv[unit].vlan_stat_get) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_stat_get(unit, vid, info, num_info));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan,
                         bcm_gport_t port, int flags)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_gport_add(unit, vlan, port, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan,
                            bcm_gport_t port, int flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_gport_delete(unit, vlan,
                                                    port, flags));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_gport_delete_all(unit, vlan));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
                         bcm_gport_t port, int* flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_gport_get(unit, vlan, port, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                             bcm_gport_t *gport_array, int *flags_array,
                             int* array_size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_gport_get_all(unit, vlan, array_max,
                                                     gport_array, flags_array,
                                                     array_size));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_get(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_control_set(unit, type, arg));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_l2_station_update_all(int unit,
                                     bcmi_ltsw_vlan_l2_station_update_cb cb,
                                     void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_l2_station_update_all(unit, cb,
                                                             user_data));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_l3_intf_update_all(int unit,
                                  bcmi_ltsw_vlan_l3_intf_update_cb cb,
                                  void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_l3_intf_update_all(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_translate_action_range_add(int unit,
                                          bcm_gport_t port,
                                          bcm_vlan_t outer_vlan_low,
                                          bcm_vlan_t outer_vlan_high,
                                          bcm_vlan_t inner_vlan_low,
                                          bcm_vlan_t inner_vlan_high,
                                          bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_translate_action_range_add(
                                     unit, port,
                                     outer_vlan_low, outer_vlan_high,
                                     inner_vlan_low, inner_vlan_high, action));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_translate_action_range_delete(int unit,
                                             bcm_gport_t port,
                                             bcm_vlan_t outer_vlan_low,
                                             bcm_vlan_t outer_vlan_high,
                                             bcm_vlan_t inner_vlan_low,
                                             bcm_vlan_t inner_vlan_high)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_translate_action_range_delete(
                                          unit, port,
                                          outer_vlan_low, outer_vlan_high,
                                          inner_vlan_low, inner_vlan_high));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_translate_action_range_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_translate_action_range_delete_all(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_translate_action_range_traverse(
                                                         unit, cb, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_block_set(unit, vid, block));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_block_get(unit, vid, block));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_profile_delete(int unit,
                              bcmi_ltsw_profile_hdl_t profile_hdl,
                              int index)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_profile_delete(unit,
                                                      profile_hdl, index));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                            int num_joining, bcm_gport_t *joining_arr,
                            int num_leaving, bcm_gport_t *leaving_arr)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_range_update(unit, gport,
                                                    num_joining, joining_arr,
                                                    num_leaving, leaving_arr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_lt_db_get(int unit,
                         bcmint_vlan_lt_db_t *lt_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_lt_db_get(unit, lt_info));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_def_flood_port_get(unit, pbmp));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_stg_set(unit, vid, ingress, stg));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_stg_get(unit, vid, ingress, stg));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_check_info_notify(unit, vid, add));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_mcast_flood_set(int unit,
                               bcm_vlan_t vid,
                               bcm_vlan_mcast_flood_t mode)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_mcast_flood_set(unit, vid, mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_mcast_flood_get(int unit,
                               bcm_vlan_t vid,
                               bcm_vlan_mcast_flood_t *mode)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_mcast_flood_get(unit, vid, mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_cross_connect_add(int unit,
                                 bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                 bcm_gport_t port_1, bcm_gport_t port_2)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_cross_connect_add(unit,
                                                     outer_vlan, inner_vlan,
                                                     port_1, port_2));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_cross_connect_delete(int unit,
                                    bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_cross_connect_delete(unit,
                                                     outer_vlan, inner_vlan));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_cross_connect_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_cross_connect_delete_all(unit));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_cross_connect_traverse(int unit,
                                      bcm_vlan_cross_connect_traverse_cb cb,
                                      void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_cross_connect_traverse(unit, cb,
                                                              user_data));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_qos_set(int unit, bcm_vlan_t vid, int id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_qos_set(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_ing_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_ing_qos_get(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_qos_set(int unit, bcm_vlan_t vid, int id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_qos_set(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_egr_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_egr_qos_get(unit, vid, id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_internal_control_set(int unit,
                                    bcmi_ltsw_vlan_control_t type,
                                    int value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_internal_control_set(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_internal_control_get(int unit,
                                    bcmi_ltsw_vlan_control_t type,
                                    int *value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_internal_control_get(unit, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                 int pipe, bcmi_ltsw_vlan_table_opcode_t op)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_xlate_pipe_update(unit, pg,
                                                         ingress, pipe, op));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_xlate_table_share(int unit,
                                 vlan_xlate_table_id_t lt_id, int *share)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_xlate_table_share(unit, lt_id, share));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_info_dump(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_info_dump(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_mshp_default_profile_get(int unit,
                                        bcmi_ltsw_profile_hdl_t type,
                                        int *profile_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_mshp_default_profile_get(unit, type,
                                                                profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_vlan_traverse(int unit,
                        bcmi_ltsw_vlan_traverse_cb cb, void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_vlan_drv[unit].vlan_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}
