/*! \file xflow_macsec.c
 *
 * xflow_macsec Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw_dispatch.h>
#include <shr/shr_debug.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>

#if defined(INCLUDE_XFLOW_MACSEC)
#include <bcm/xflow_macsec.h>
#include <ltsw/xflow_macsec_common.h>
#include <bcm_int/ltsw/xflow_macsec_int.h>
#endif

#if defined(INCLUDE_XFLOW_MACSEC)


/*
 * Global variable to hold xflow_macsec info.
 */
ltsw_xflow_macsec_cfg_t ltsw_xflow_macsec_info[BCM_MAX_NUM_UNITS] = {{0}};

/*!
 * \brief Clean up xflow_macsec module software database.
 *
 * \param [in]  unit          Unit number.
 */
    static void
xflow_macsec_info_cleanup(int unit)
{
    ltsw_xflow_macsec_cfg_t *xflow_macsec_cfg = XFLOW_MACSEC_INFO(unit);

    if (xflow_macsec_cfg->mutex) {
        sal_mutex_destroy(xflow_macsec_cfg->mutex);
        xflow_macsec_cfg->mutex = NULL;
    }

    return;
}

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
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    if (XFLOW_MACSEC_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    XFLOW_MACSEC_LOCK(unit);
    locked = true;

    XFLOW_MACSEC_INFO(unit)->inited = false;

    XFLOW_MACSEC_UNLOCK(unit);
    locked = false;

    xflow_macsec_info_cleanup(unit);

exit:
    if (locked) {
        XFLOW_MACSEC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init Xflow_macsec cfg info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
xflow_macsec_cfg_info_init(int unit)
{
    uint32_t alloc_size = 0, req_size = 0;
    int warm = bcmi_warmboot_get(unit);
    ltsw_xflow_macsec_cfg_t *xflow_macsec_cfg = XFLOW_MACSEC_INFO(unit);
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    uint32_t tbl_size = 0, num_blocks = 0;
    bool detached = false;  /* xflow_macsec is detached */

    SHR_FUNC_ENTER(unit);
    if (xflow_macsec_cfg->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_xflow_macsec_detach(unit));
    }
    detached = true;

    if (xflow_macsec_cfg->mutex == NULL) {
        xflow_macsec_cfg->mutex = sal_mutex_create("xflow_macsec_info_mutex");
        SHR_NULL_CHECK(xflow_macsec_cfg->mutex, SHR_E_MEMORY);
    }

    /* Allocate HA memory for xflow_macsec SC encrypt bitmap. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, SEC_BLOCKs, &num_blocks));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, SEC_ENCRYPT_MACSEC_SC_POLICYs, &tbl_size));
    xflow_macsec_cfg->num_blocks = num_blocks;
    xflow_macsec_cfg->num_sc_encrypt = (tbl_size / num_blocks);

    ha_instance_size = sizeof(uint8_t);
    ha_array_size = tbl_size;
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    xflow_macsec_cfg->sc_encrypt_bmp = bcmi_ltsw_ha_mem_alloc(unit,
            BCMI_HA_COMP_ID_XFLOW_MACSEC,
            BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_ENCRYPT_BITMAP,
            "bcmXflowmacsecScEncryptBitmap",
            &alloc_size);
    SHR_NULL_CHECK(xflow_macsec_cfg->sc_encrypt_bmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(xflow_macsec_cfg->sc_encrypt_bmp, 0, alloc_size);
    }


    /* Allocate HA memory for xflow_macsec SC decrypt bitmap. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, SEC_DECRYPT_MACSEC_SC_POLICYs, &tbl_size));
    xflow_macsec_cfg->num_sc_decrypt = (tbl_size / num_blocks);

    ha_instance_size = sizeof(uint8_t);
    ha_array_size = tbl_size;
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    xflow_macsec_cfg->sc_decrypt_bmp = bcmi_ltsw_ha_mem_alloc(unit,
            BCMI_HA_COMP_ID_XFLOW_MACSEC,
            BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_DECRYPT_BITMAP,
            "bcmXflowmacsecScDecryptBitmap",
            &alloc_size);
    SHR_NULL_CHECK(xflow_macsec_cfg->sc_decrypt_bmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(xflow_macsec_cfg->sc_decrypt_bmp, 0, alloc_size);
    }


    /* Allocate HA memory for xflow_macsec policy bitmap. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, SEC_DECRYPT_SUBPORT_POLICYs, &tbl_size));

    xflow_macsec_cfg->num_policies = (tbl_size / num_blocks);

    ha_instance_size = sizeof(uint8_t);
    ha_array_size = tbl_size;
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    xflow_macsec_cfg->policy_bmp = bcmi_ltsw_ha_mem_alloc(unit,
            BCMI_HA_COMP_ID_XFLOW_MACSEC,
            BCMINT_XFLOW_MACSEC_SUB_COMP_ID_POLICY_BITMAP,
            "bcmXflowmacsecPolicyBitmap",
            &alloc_size);
    SHR_NULL_CHECK(xflow_macsec_cfg->policy_bmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(xflow_macsec_cfg->policy_bmp, 0, alloc_size);
    }

    /* Allocate HA memory for sc_tcam_param. */
    tbl_size = xflow_macsec_cfg->num_sc_decrypt;

    ha_instance_size = sizeof(ltsw_xflow_macsec_sc_tcam_param_t);
    ha_array_size = tbl_size;
    req_size = ha_instance_size * ha_array_size;
    alloc_size = req_size;
    xflow_macsec_cfg->sc_tcam_param = bcmi_ltsw_ha_mem_alloc(unit,
            BCMI_HA_COMP_ID_XFLOW_MACSEC,
            BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_TCAM_MAP,
            "bcmXflowmacsecScTcamMap",
            &alloc_size);
    SHR_NULL_CHECK(xflow_macsec_cfg->sc_tcam_param , SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(xflow_macsec_cfg->sc_tcam_param, 0, alloc_size);
    }

exit:
    if (SHR_FUNC_ERR() && detached) {
        xflow_macsec_info_cleanup(unit);
        if (!warm) {
            if (xflow_macsec_cfg->sc_encrypt_bmp) {
                (void)bcmi_ltsw_ha_mem_free(unit, xflow_macsec_cfg->sc_encrypt_bmp);
                xflow_macsec_cfg->sc_encrypt_bmp = NULL;
            }
            if (xflow_macsec_cfg->policy_bmp) {
                (void)bcmi_ltsw_ha_mem_free(unit, xflow_macsec_cfg->policy_bmp);
                xflow_macsec_cfg->policy_bmp = NULL;
            }
        }
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_xflow_macsec_init(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Init Xflow_macsec config info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (xflow_macsec_cfg_info_init(unit));


    SHR_IF_ERR_EXIT
        (bcmi_ltsw_xflow_macsec_init(unit));

    XFLOW_MACSEC_INFO(unit)->inited = TRUE;

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
    return BCM_E_UNAVAIL;
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
    return BCM_E_UNAVAIL;
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
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_xflow_macsec_decrypt_flow_set(
    int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int priority)
{
    return BCM_E_UNAVAIL;
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

#endif
