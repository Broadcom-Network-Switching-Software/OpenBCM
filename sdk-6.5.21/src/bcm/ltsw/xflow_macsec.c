/*! \file xflow_macsec.c
 *
 * xflow_macsec Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcm_int/ltsw_dispatch.h>
#include <shr/shr_debug.h>
#include <bcm_int/ltsw/issu.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>

#if defined(INCLUDE_XFLOW_MACSEC)
#include <ltsw/xflow_macsec_common.h>
#include <bcm_int/ltsw/xflow_macsec_int.h>
#endif

#if defined(INCLUDE_XFLOW_MACSEC)

#define BSL_LOG_MODULE BSL_LS_BCM_XFLOW_MACSEC


/*
 * Global variable to hold xflow_macsec info.
 */
ltsw_xflow_macsec_cfg_t ltsw_xflow_macsec_info[BCM_MAX_NUM_UNITS] = {{0}};

/*!
 * \brief Detach xflow_macsec module.
 *
 * \param [in] unit           Unit number
 *
 * \retval SHR_E_NONE         No errors.
 */
    int
bcm_ltsw_xflow_macsec_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_detach(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_init(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_secure_chan_info_t *chan_info,
    int priority,
    bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_create(unit, flags,
                                            instance_id, chan_info,
                                            priority, chan_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_secure_chan_info_t *chan_info,
    int priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_set(unit, flags,
                                            chan_id, chan_info,
                                            priority));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_get(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_secure_chan_info_t *chan_info,
    int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_get(unit, chan_id,
                                            chan_info,
                                            priority));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_destroy(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_destroy(unit, chan_id));
exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_xflow_macsec_secure_chan_enable_get(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_enable_get(unit,
                                                       chan_id,
                                                       enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_enable_set(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_enable_set(unit,
                                                       chan_id,
                                                       enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_chan_info_traverse(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_chan_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_chan_info_traverse
                (unit, flags, instance_id, callback, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_port_rsvd_secure_chan_get(int unit, uint32 flags,
                                                bcm_gport_t gport,
                                                bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    SHR_FUNC_ENTER(unit);

    bcm_port_t      local_port = BCM_PORT_INVALID;
    int             is_local = FALSE, id;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;

    if (BCM_GPORT_IS_SET(gport)) {
        /* GPORT Validations */
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_resolve(
                    unit, gport, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Error: Unable to get local_port information for "
                    " gport 0x%x.\n"), gport));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT(
                bcmi_ltsw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Error: Mod_id %d is not local to the device.\n"),
                    modid));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_port_rsvd_secure_chan_get(unit,
                                                 flags,
                                                 local_port,
                                                 chan_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_vlan_tpid_array_get(
    int unit,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_vlan_tpid_t *vlan_tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_vlan_tpid_array_get(unit,
                                                    instance_id,
                                                    vlan_tpid));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_vlan_tpid_array_set(
    int unit,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_vlan_tpid_t *vlan_tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_vlan_tpid_array_set(unit,
                                                    instance_id,
                                                    vlan_tpid));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_vlan_tpid_array_index_get(
    int unit,
    bcm_xflow_macsec_instance_id_t instance_id,
    uint32 vlan_tpid,
    uint8 *tpid_index_sel)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_vlan_tpid_array_index_get(unit,
                                                          instance_id,
                                                          vlan_tpid,
                                                          tpid_index_sel));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_sectag_etype_set(
    int unit,
    int flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    uint32 sectag_etype,
    bcm_xflow_macsec_sectag_ethertype_t *sectag_etype_sel)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_sectag_etype_set(unit,
                                                 flags,
                                                 instance_id,
                                                 sectag_etype,
                                                 (xflow_macsec_sectag_ethertype_t *)sectag_etype_sel));
exit:
    SHR_FUNC_EXIT();


}

int
bcm_ltsw_xflow_macsec_sectag_etype_get(
    int unit,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_sectag_ethertype_t sectag_etype_sel,
    uint32 *sectag_etype)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_sectag_etype_get(unit,
                                                 instance_id,
                                                 sectag_etype_sel,
                                                 sectag_etype));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_control_set(int unit, uint32 flags,
                    bcm_xflow_macsec_instance_id_t instance_id,
                    bcm_xflow_macsec_control_t type, uint64 value)
{
     SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_control_set(unit,
                                            flags,
                                            instance_id,
                                            type,
                                            value));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_control_get(int unit, uint32 flags,
                    bcm_xflow_macsec_instance_id_t instance_id,
                    bcm_xflow_macsec_control_t type, uint64 *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_control_get(unit,
                                            flags,
                                            instance_id,
                                            type,
                                            value));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_mac_addr_control_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_mac_addr_control_t control_type,
    bcm_xflow_macsec_mac_addr_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_mac_addr_control_set(unit,
                                                     flags,
                                                     instance_id,
                                                     control_type,
                                                     control_info));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_mac_addr_control_get(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_mac_addr_control_t control_type,
    bcm_xflow_macsec_mac_addr_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_mac_addr_control_get(unit,
                                                     flags,
                                                     instance_id,
                                                     control_type,
                                                     control_info));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_port_control_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_xflow_macsec_port_control_t control_type,
    bcm_xflow_macsec_port_info_t *port_info)
{
    SHR_FUNC_ENTER(unit);

    bcm_port_t      local_port = BCM_PORT_INVALID;
    int             is_local = FALSE, id;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;

    SHR_NULL_CHECK(port_info, SHR_E_PARAM);
    if (BCM_GPORT_IS_SET(gport)) {
        /* GPORT Validations */
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_resolve(
                    unit, gport, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT(
                bcmi_ltsw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_port_control_set(unit,
                                                 flags,
                                                 local_port,
                                                 control_type,
                                                 port_info->value));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_port_control_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_xflow_macsec_port_control_t control_type,
    bcm_xflow_macsec_port_info_t *port_info)
{
    SHR_FUNC_ENTER(unit);

    bcm_port_t      local_port = BCM_PORT_INVALID;
    int             is_local = FALSE, id;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;

    SHR_NULL_CHECK(port_info, SHR_E_PARAM);
    if (BCM_GPORT_IS_SET(gport)) {
        /* GPORT Validations */
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_resolve(
                    unit, gport, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT(
                bcmi_ltsw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_port_control_get(unit,
                                                 flags,
                                                 local_port,
                                                 control_type,
                                                 &port_info->value));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_flow_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int priority,
    bcm_xflow_macsec_flow_id_t *flow_id)
{
    SHR_FUNC_ENTER(unit);
    flags |= BCM_XFLOW_MACSEC_DECRYPT;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_flow_create(unit,
                                            flags,
                                            instance_id,
                                            flow_info,
                                            priority,
                                            flow_id));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_flow_default_policy_get(
    int unit,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_policy_id_t *policy_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_decrypt_flow_default_policy_get(unit,
                                                     instance_id,
                                                     policy_id));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_flow_destroy(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_flow_destroy(unit,
                                             flow_id));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_flow_enable_get(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    int *enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_xflow_macsec_decrypt_flow_enable_set(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_xflow_macsec_decrypt_flow_get(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_flow_get(unit,
                                         flow_id,
                                         flow_info,
                                         priority));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_flow_set(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_flow_set(unit,
                                         flow_id,
                                         flow_info,
                                         priority));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_policy_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info,
    bcm_xflow_macsec_policy_id_t *policy_id)
{
    SHR_FUNC_ENTER(unit);
    flags |= BCM_XFLOW_MACSEC_DECRYPT;
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_policy_create (unit,
                                                          flags,
                                                          instance_id,
                                                          policy_info,
                                                          policy_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_decrypt_policy_destroy(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_policy_destroy (unit,
                                                           policy_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_decrypt_policy_get(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_policy_get (unit,
                                                       policy_id,
                                                       policy_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_decrypt_policy_set(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_policy_set (unit,
                                                       policy_id,
                                                       policy_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_instance_pbmp_map_get(
    int unit,
    int instance_max,
    bcm_xflow_macsec_instance_pbmp_t *instance_pbmp_map,
    int *instance_count)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_instance_pbmp_map_get (unit,
                                                                  instance_max,
                                                                  instance_pbmp_map,
                                                                  instance_count));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_port_map_info_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_xflow_macsec_port_map_info_t *port_map_info)
{
    bcm_port_t      local_port = BCM_PORT_INVALID;
    int             is_local = FALSE, id;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(gport)) {
        /* GPORT Validations */
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_resolve(
                    unit, gport, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT(
                bcmi_ltsw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        SHR_IF_ERR_EXIT(
                bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    }
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_port_map_info_get (unit,
                                                              flags,
                                                              local_port,
                                                              port_map_info));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_stat_get(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    bcm_xflow_macsec_stat_type_t  stat_type,
    uint64 *value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_stat_get (unit,
                                                     flags,
                                                     id,
                                                     stat_type,
                                                     value));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_stat_multi_get(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    uint32 num_stats,
    bcm_xflow_macsec_stat_type_t  *stat_type_array,
    uint64 *value_array)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_stat_multi_get (unit,
                flags,
                id,
                num_stats,
                (xflow_macsec_stat_type_t  *)stat_type_array,
                value_array));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_stat_multi_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    uint32 num_stats,
    bcm_xflow_macsec_stat_type_t  *stat_type_array,
    uint64 *value_array)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_stat_multi_set (unit,
                flags,
                id,
                num_stats,
                (xflow_macsec_stat_type_t  *) stat_type_array,
                value_array));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_stat_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    bcm_xflow_macsec_stat_type_t  stat_type,
    uint64 value)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_stat_set (unit,
                                                     flags,
                                                     id,
                                                     stat_type,
                                                     value));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_subport_id_get(
    int unit,
    bcm_xflow_macsec_id_t id,
    bcm_xflow_macsec_subport_id_t *macsec_subport_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmi_ltsw_xflow_macsec_subport_id_get (unit,
                                                           id,
                                                           macsec_subport_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_assoc_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_secure_assoc_info_t *assoc_info,
    bcm_xflow_macsec_secure_assoc_id_t *assoc_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_assoc_create(unit, flags,
                                               chan_id, assoc_info,
                                               assoc_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_assoc_destroy(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_assoc_destroy(unit, assoc_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_assoc_get(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id,
    bcm_xflow_macsec_secure_assoc_info_t *assoc_info,
    bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_assoc_get(unit, assoc_id,
                                            assoc_info, chan_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_assoc_set(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id,
    bcm_xflow_macsec_secure_assoc_info_t *assoc_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_assoc_set(unit, assoc_id,
                                            assoc_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_secure_assoc_traverse(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_secure_assoc_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_secure_assoc_traverse(unit, chan_id,
                                            callback, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_event_register(
    int unit,
    bcm_xflow_macsec_event_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_event_register(unit, (xflow_macsec_event_cb)cb,
                                               user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_event_deregister(
    int unit,
    bcm_xflow_macsec_event_cb cb)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_event_deregister(unit, (xflow_macsec_event_cb)cb));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_handle_info_get(int unit, bcm_xflow_macsec_id_t id,
                                      bcm_xflow_macsec_handle_info_t *handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_handle_info_get(unit, id, handle));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_chan_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_ipsec_secure_chan_info_t *chan_info,
    bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_chan_create(unit, flags,
                                                         instance_id, chan_info,
                                                         chan_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_chan_destroy(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_chan_destroy(unit, chan_id));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_ipsec_secure_chan_get(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_ipsec_secure_chan_info_t *chan_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_chan_get(unit,
                                                      chan_id, chan_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_chan_info_traverse(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_instance_id_t instance_id,
    bcm_xflow_macsec_ipsec_chan_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_chan_info_traverse
                (unit, flags, instance_id, callback, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_chan_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_ipsec_secure_chan_info_t *chan_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_chan_set(unit, flags,
                                                      chan_id, chan_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_assoc_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_ipsec_secure_assoc_info_t *assoc_info,
    bcm_xflow_macsec_secure_assoc_id_t *assoc_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_assoc_create(unit, flags,
                                               chan_id, assoc_info,
                                               assoc_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_assoc_destroy(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_assoc_destroy
                                            (unit, assoc_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_assoc_get(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id,
    bcm_xflow_macsec_ipsec_secure_assoc_info_t *assoc_info,
    bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_assoc_get(unit, assoc_id,
                                            assoc_info, chan_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_assoc_set(
    int unit,
    bcm_xflow_macsec_secure_assoc_id_t assoc_id,
    bcm_xflow_macsec_ipsec_secure_assoc_info_t *assoc_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_assoc_set(unit, assoc_id,
                                            assoc_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_ipsec_secure_assoc_traverse(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_ipsec_secure_assoc_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_ipsec_secure_assoc_traverse(unit,
                                      chan_id, callback, user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_get(
        int unit,
        bcm_xflow_macsec_instance_id_t instance_id,
        int index,
        bcm_xflow_macsec_svtag_cpu_flex_map_info_t *svtag_cpu_flex_map_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_get(unit,
                                      instance_id, index, svtag_cpu_flex_map_info));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_set(
        int unit,
        bcm_xflow_macsec_instance_id_t instance_id,
        int index,
        bcm_xflow_macsec_svtag_cpu_flex_map_info_t *svtag_cpu_flex_map_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_set(unit,
                                      instance_id, index, svtag_cpu_flex_map_info));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_enable_get(
        int unit,
        bcm_xflow_macsec_instance_id_t instance_id,
        int index,
        int *enable)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_enable_get(unit,
                                      instance_id, index, enable));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_enable_set(
        int unit,
        bcm_xflow_macsec_instance_id_t instance_id,
        int index,
        int enable)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_decrypt_svtag_cpu_flex_map_enable_set(unit,
                                      instance_id, index, enable));
exit:
    SHR_FUNC_EXIT();

}
#endif
