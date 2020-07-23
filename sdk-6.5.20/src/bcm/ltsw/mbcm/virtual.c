/*! \file virtual.c
 *
 * BCM virtual module multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/virtual.h>

#include <shr/shr_debug.h>


/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON


static mbcm_ltsw_virtual_drv_t mbcm_ltsw_virtual_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_virtual_drv_set(int unit, mbcm_ltsw_virtual_drv_t *drv)
{
    mbcm_ltsw_virtual_drv_t *local = &mbcm_ltsw_virtual_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

/*!
 * \brief Get available hardware index for VPN and VP.
 *
 * This function is used to init related hardware tables.
 *
 * \param [in]   unit          Unit Number.
 * \param [out]  vfi_min       Minimum hardware index for VPN.
 * \param [out]  vfi_max       Maximum hardware index for VPN.
 * \param [out]  vpn_min       Minimum hardware index for VP.
 * \param [out]  vpn_max       Maximum hardware index for VP.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
mbcm_virtual_index_get(int unit, int *vfi_min, int *vfi_max, int *vp_min,
                       int *vp_max, int *l2_iif_min, int *l2_iif_max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_index_get(unit, vfi_min, vfi_max,
                                                       vp_min, vp_max,
                                                       l2_iif_min, l2_iif_max));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize VFI hardware tables.
 *
 * This function is used to init related hardware tables.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vfi                  VFI index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
mbcm_virtual_vfi_init(int unit, int vfi)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_vfi_init(unit, vfi));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cleanup VFI hardware tables.
 *
 * This function is used to cleanup related hardware tables.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vfi                  VFI index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
mbcm_virtual_vfi_cleanup(int unit, int vfi)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_vfi_cleanup(unit, vfi));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_learn_get(int unit, bcm_port_t port, uint32 *flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_learn_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_learn_set(int unit, bcm_port_t port, uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_learn_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_vlan_action_set(unit, port,
                                                                  action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_vlan_action_get(unit, port,
                                                                  action));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_vlan_action_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_vlan_action_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_class_get(int unit, bcm_port_t port, bcm_port_class_t pclass,
                            uint32_t *pclass_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_class_get(unit, port, pclass,
                                                            pclass_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_class_set(int unit, bcm_port_t port, bcm_port_class_t pclass,
                            uint32_t pclass_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_class_set(unit, port, pclass,
                                                            pclass_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_port_vlan_ctrl_get(unit, port,
                                                                type,
                                                                arg));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int arg)
 {
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_vlan_ctrl_set(unit, port,
                                                                 type,
                                                                 arg));
 exit:
     SHR_FUNC_EXIT();
 }

int
mbcm_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port, int *enable,
                                  int *mirror_idx)
{
 SHR_FUNC_ENTER(unit);

 SHR_IF_ERR_EXIT
     (mbcm_ltsw_virtual_drv[unit].virtual_port_mirror_ctrl_get(unit, port,
                                                               enable,
                                                               mirror_idx));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port, int enable,
                                  int *mirror_idx)
{
 SHR_FUNC_ENTER(unit);

 SHR_IF_ERR_EXIT
     (mbcm_ltsw_virtual_drv[unit].virtual_port_mirror_ctrl_set(unit, port,
                                                               enable,
                                                               mirror_idx));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_dscp_map_mode_set(unit, port,
                                                                     mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_dscp_map_mode_get(unit, port,
                                                                     mode));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_ptr)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_vlan_map_get(unit, port,
                                                                    map_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_ptr)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_vlan_map_set(unit, port,
                                                                    map_ptr));
exit:
    SHR_FUNC_EXIT();
}


int
mbcm_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_ptr)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_dscp_map_get(unit, port,
                                                                    map_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_ptr)
{
     SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
         (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_dscp_map_set(unit, port,
                                                                    map_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                          int *map_ptr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
     (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_vlan_remark_map_get(unit,
                                                                       port,
                                                                       map_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                          int map_ptr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_port_phb_vlan_remark_map_set(unit,
                                                                         port,
                                                                         map_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_nw_group_num_get(int unit, int *num)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_nw_group_num_get(unit, num));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_virtual_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_init(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
mbcm_virtual_port_control_get(int unit, bcm_port_t port,
                              bcm_port_control_t type, int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_port_control_get(unit, port, type,
                                                             value));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
int
mbcm_virtual_port_control_set(int unit, bcm_port_t port,
                              bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_port_control_set(unit, port, type,
                                                             value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
mbcm_virtual_port_stat_attach(int unit, bcm_port_t port,
                              uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_port_stat_attach(unit, port,
                                                             stat_counter_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to given port with a given stat counter id.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
mbcm_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                      uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_port_stat_detach_with_id
           (unit, port, stat_counter_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [in] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
mbcm_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_network_group_split_config_set
           (unit, source_network_group_id, config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Gets the split horizon network group configuration for
 * a given pair of source and destination network group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source_network_group_id Source network group id.
 * \param [out] config Configuration information.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
mbcm_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
       (mbcm_ltsw_virtual_drv[unit].virtual_network_group_split_config_get
           (unit, source_network_group_id, config));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get available SW index for tunnel.
 *
 * \param [in]   unit  Unit Number.
 * \param [out]  min   Minimum SW index for tunnel.
 * \param [out]  vmax  Maximum SW index for tunnel.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
mbcm_virtual_tnl_index_get(int unit, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_virtual_drv[unit].virtual_tnl_index_get(unit, min, max));

exit:
    SHR_FUNC_EXIT();
}
