/*! \file virtual.h
 *
 * BCM virtual resource mangement APIs and Structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_VIRTUAL_H
#define BCMI_LTSW_MBCM_VIRTUAL_H

#include <bcm_int/ltsw/virtual.h>

#include <sal/sal_types.h>

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
 * \param [out]  l2_iif_min    Minimum hardware index for L2_IIF.
 * \param [out]  l2_iif_max    Maximum hardware index for L2_IIF.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
typedef int (*virtual_index_get_f)(int unit, int *vfi_min, int *vfi_max,
                                   int *vp_min, int *vp_max, int *l2_iif_min,
                                   int *l2_iif_max);

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
typedef int (*virtual_vfi_init_f)(int unit, int vfi);

/*!
 * \brief Initialize VIRTUAL module.
 *
 * This function is used to init VIRTUAL module including H/W and S/W.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
typedef int (*virtual_init_f)(int unit);

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
typedef int (*virtual_vfi_cleanup_f)(int unit, int vfi);

/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_learn_get_f)(int unit, bcm_port_t port,
                                        uint32 *flags);

/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_learn_set_f)(int unit, bcm_port_t port,
                                        uint32 flags);
/*!
 * \brief Set virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 * \param [in] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_vlan_action_set_f)(int unit, bcm_port_t port,
                                              bcm_vlan_action_set_t *action);

/*!
 * \brief Get virtual port's vlan actions.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  port   Gport.
 * \param [out] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_vlan_action_get_f)(int unit, bcm_port_t port,
                                              bcm_vlan_action_set_t *action);

/*!
 * \brief Reset virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_vlan_action_reset_f)(int unit, bcm_port_t port);

/*!
 * \brief Get the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [out] pclass_id class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_class_get_f)(int unit, bcm_port_t port,
                                        bcm_port_class_t pclass,
                                        uint32_t *pclass_id);

/*!
 * \brief Set the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [in] pclass_id Class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_class_set_f)(int unit, bcm_port_t port,
                                        bcm_port_class_t pclass,
                                        uint32_t pclass_id);

/*!
 * \brief Get status of VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [out] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_vlan_ctrl_get_f)(int unit, bcm_port_t port,
                                            bcm_vlan_control_port_t type,
                                            int *arg);

/*!
 * \brief Enable/Disable VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [in] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_vlan_ctrl_set_f)(int unit, bcm_port_t port,
                                            bcm_vlan_control_port_t type,
                                            int arg);

/*!
 * \brief Get status of Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [out]  enable     Bitmap of mirror session status.
 * \param [out]  mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_mirror_ctrl_get_f)(int unit, bcm_port_t port,
                                              int *enable, int *mirror_idx);

/*!
 * \brief Enable/Disable Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [in]   enable     Bitmap of mirror session status.
 * \param [in]   mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_mirror_ctrl_set_f)(int unit, bcm_port_t port,
                                              int enable, int *mirror_idx);

/*!
 * \brief Set DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_dscp_map_mode_set_f)(int unit, bcm_port_t port,
                                                int mode);

/*!
 * \brief Get DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_dscp_map_mode_get_f)(int unit, bcm_port_t port,
                                                int *mode);

/*!
 * \brief Get VLAN based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_phb_vlan_map_get_f)(int unit, bcm_port_t port,
                                               int *map_id);

/*!
 * \brief Set VLAN based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_phb_vlan_map_set_f)(int unit, bcm_port_t port,
                                               int map_id);


/*!
 * \brief Get DSCP based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id DSCP based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_phb_dscp_map_get_f)(int unit, bcm_port_t port,
                                               int *map_id);

/*!
 * \brief Set DSCP based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id DSCP based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_phb_dscp_map_set_f)(int unit, bcm_port_t port,
                                               int map_id);
 /*!
  * \brief Get VLAN based PHB remarking mapping ID.
  *
  * \param [in] unit Unit number.
  * \param [in] port Port number.
  * \param [out] map_id VLAN based PHB remarking mapping  ID.
  *
  * \retval SHR_E_NONE              No errors.
  * \retval !SHR_E_NONE             Failure.
  */
typedef int (*virtual_port_phb_vlan_remark_map_get_f)(int unit,
                                                      bcm_port_t port,
                                                      int *map_id);

/*!
 * \brief Set VLAN based PHB remarking mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB remarking mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_port_phb_vlan_remark_map_set_f)(int unit,
                                                      bcm_port_t port,
                                                      int map_id);

/*!
 * \brief Get the supported network group number.
 *
 * \param [in] unit Unit number.
 * \param [out] num  Number of the supported network group.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
typedef int (*virtual_nw_group_num_get_f)(int unit, int *num);


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
typedef int (*virtual_port_control_get_f)(int unit, bcm_port_t port,
                                          bcm_port_control_t type, int *value);

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
typedef int (*virtual_port_control_set_f)(int unit, bcm_port_t port,
                                          bcm_port_control_t type, int value);



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
typedef int (*virtual_port_stat_attach_f)(int unit, bcm_port_t port,
                                          uint32_t stat_counter_id);

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
typedef int (*virtual_port_stat_detach_with_id_f)(int unit,
                                                  bcm_port_t port,
                                                  uint32_t stat_counter_id);

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
typedef int (*virtual_network_group_split_config_set_f)(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

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
typedef int (*virtual_network_group_split_config_get_f)(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

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
typedef int (*virtual_tnl_index_get_f)(
    int unit,
    int *min,
    int *max);

/*!
 * \brief Virtual function vector driver.
 */
typedef struct mbcm_ltsw_virtual_drv_s {

    /*! Get available hardware index info. */
    virtual_index_get_f      virtual_index_get;

    /*! Initialize VFI hardware tables. */
    virtual_vfi_init_f       virtual_vfi_init;

    /*! Initialize VIRTUAL modudle. */
    virtual_init_f       virtual_init;

    /*!  Cleanup VFI hardware tables. */
    virtual_vfi_cleanup_f    virtual_vfi_cleanup;

    /*! Port Learn control */
    virtual_port_learn_get_f      virtual_port_learn_get;
    virtual_port_learn_set_f      virtual_port_learn_set;

    /*! Port VLAN actions */
    virtual_port_vlan_action_set_f      virtual_port_vlan_action_set;
    virtual_port_vlan_action_get_f      virtual_port_vlan_action_get;
    virtual_port_vlan_action_reset_f    virtual_port_vlan_action_reset;

    /*! Port class ID */
    virtual_port_class_get_f      virtual_port_class_get;
    virtual_port_class_set_f      virtual_port_class_set;

    /*! Port VLAN control */
    virtual_port_vlan_ctrl_get_f      virtual_port_vlan_ctrl_get;
    virtual_port_vlan_ctrl_set_f      virtual_port_vlan_ctrl_set;

    /*! Port Mirror control */
    virtual_port_mirror_ctrl_get_f      virtual_port_mirror_ctrl_get;
    virtual_port_mirror_ctrl_set_f      virtual_port_mirror_ctrl_set;

    /*! Port DSCP mode control */
    virtual_port_dscp_map_mode_set_f      virtual_port_dscp_map_mode_set;
    virtual_port_dscp_map_mode_get_f      virtual_port_dscp_map_mode_get;

    /*! Port VLAN based PHB mapping */
    virtual_port_phb_vlan_map_get_f      virtual_port_phb_vlan_map_get;
    virtual_port_phb_vlan_map_set_f      virtual_port_phb_vlan_map_set;

    /*! Port DSCP based PHB mapping */
    virtual_port_phb_dscp_map_get_f      virtual_port_phb_dscp_map_get;
    virtual_port_phb_dscp_map_set_f      virtual_port_phb_dscp_map_set;

    /*! Port VLAN based PHB remarking mapping */
    virtual_port_phb_vlan_remark_map_get_f virtual_port_phb_vlan_remark_map_get;
    virtual_port_phb_vlan_remark_map_set_f virtual_port_phb_vlan_remark_map_set;

    /*! Get the supported network group number */
    virtual_nw_group_num_get_f  virtual_nw_group_num_get;

    /*! Port control */
    virtual_port_control_get_f virtual_port_control_get;
    virtual_port_control_set_f virtual_port_control_set;

    /*! flex counter */
    virtual_port_stat_attach_f         virtual_port_stat_attach;
    virtual_port_stat_detach_with_id_f virtual_port_stat_detach_with_id;

    /*! Split horizon */
    virtual_network_group_split_config_set_f virtual_network_group_split_config_set;
    virtual_network_group_split_config_get_f virtual_network_group_split_config_get;

    /*! Tunnel index */
    virtual_tnl_index_get_f virtual_tnl_index_get;
} mbcm_ltsw_virtual_drv_t;

/*!
 * \brief Set the flow driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Flow driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_virtual_drv_set(int unit, mbcm_ltsw_virtual_drv_t *drv);

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
 * \param [out]  l2_iif_min    Minimum hardware index for L2_IIF.
 * \param [out]  l2_iif_max    Maximum hardware index for L2_IIF.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
mbcm_virtual_index_get(int unit, int *vfi_min, int *vfi_max, int *vp_min,
                       int *vp_max, int *l2_iif_min, int *l2_iif_max);

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
extern int
mbcm_virtual_vfi_init(int unit, int vfi);

/*!
 * \brief Initialize VIRTUAL module.
 *
 * This function is used to init VIRTUAL module including H/W and S/W.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
extern int
mbcm_virtual_init(int unit);

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
extern int
mbcm_virtual_vfi_cleanup(int unit, int vfi);

/*!
 * \brief Get the ARL hardware learning options for this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_learn_get(int unit, bcm_port_t port, uint32 *flags);

/*!
 * \brief Set the ARL hardware learning options for this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] flags Logical OR of BCM_PORT_LEARN_xxx flags.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_learn_set(int unit, bcm_port_t port, uint32 flags);

/*!
 * \brief Set virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 * \param [in] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_vlan_action_set(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action);

/*!
 * \brief Get virtual port's vlan actions.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  port   Gport.
 * \param [out] action Vlan tag actions.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_vlan_action_get(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action);

/*!
 * \brief Reset virtual port's vlan actions.
 *
 * \param [in] unit   Unit number.
 * \param [in] port   Gport.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_vlan_action_reset(int unit, bcm_port_t port);

/*!
 * \brief Get the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [out] pclass_id class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_class_get(int unit, bcm_port_t port, bcm_port_class_t pclass,
                            uint32_t *pclass_id);

/*!
 * \brief Set the class ID of this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pclass Classification type.
 * \param [in] pclass_id Class ID of the port.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_class_set(int unit, bcm_port_t port, bcm_port_class_t pclass,
                            uint32_t pclass_id);

/*!
 * \brief Get status of VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [out] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_vlan_ctrl_get(int unit, bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int *arg);

/*!
 * \brief Enable/Disable VLAN specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Vlan control type.
 * \param [in] arg Vlaue for given feature.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_vlan_ctrl_set(int unit, bcm_port_t port,
                                bcm_vlan_control_port_t type,
                                int arg);

/*!
 * \brief Get status of Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [out]  enable     Bitmap of mirror session status.
 * \param [out]  mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_mirror_ctrl_get(int unit, bcm_port_t port, int *enable,
                                  int *mirror_idx);

/*!
 * \brief Enable/Disable Mirror specified port feature.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   port       Port number.
 * \param [in]   enable     Bitmap of mirror session status.
 * \param [in]   mirror_idx Index array.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_mirror_ctrl_set(int unit, bcm_port_t port, int enable,
                                  int *mirror_idx);

/*!
 * \brief Set DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode);


/*!
 * \brief Get DSCP mapping mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode DSCP mapping mode.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode);

/*!
 * \brief Get VLAN based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_phb_vlan_map_get(int unit, bcm_port_t port, int *map_id);

/*!
 * \brief Set VLAN based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_phb_vlan_map_set(int unit, bcm_port_t port, int map_id);


/*!
 * \brief Get DSCP based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id DSCP based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_phb_dscp_map_get(int unit, bcm_port_t port, int *map_id);

/*!
 * \brief Set DSCP based PHB mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id DSCP based PHB mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_phb_dscp_map_set(int unit, bcm_port_t port, int map_id);

/*!
* \brief Get VLAN based PHB remarking mapping ID.
*
* \param [in] unit Unit number.
* \param [in] port Port number.
* \param [out] map_id VLAN based PHB remarking mapping  ID.
*
* \retval SHR_E_NONE              No errors.
* \retval !SHR_E_NONE             Failure.
*/
extern int
mbcm_virtual_port_phb_vlan_remark_map_get(int unit, bcm_port_t port,
                                          int *map_id);

/*!
 * \brief Set VLAN based PHB remarking mapping ID.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] map_id VLAN based PHB remarking mapping ID.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_port_phb_vlan_remark_map_set(int unit, bcm_port_t port,
                                          int map_id);
/*!
 * \brief Get the supported network group number.
 *
 * \param [in] unit Unit number.
 * \param [out] num  Number of the supported network group.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
mbcm_virtual_nw_group_num_get(int unit, int *num);

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
extern int
mbcm_virtual_port_control_get(int unit, bcm_port_t port,
                              bcm_port_control_t type, int *value);


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
extern int
mbcm_virtual_port_control_set(int unit, bcm_port_t port,
                              bcm_port_control_t type, int value);

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
extern int
mbcm_virtual_port_stat_attach(int unit, bcm_port_t port,
                              uint32_t stat_counter_id);

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
extern int
mbcm_virtual_port_stat_detach_with_id(int unit, bcm_port_t port,
                                      uint32_t stat_counter_id);

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
extern int
mbcm_virtual_network_group_split_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

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
extern int
mbcm_virtual_network_group_split_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config);

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
extern int
mbcm_virtual_tnl_index_get(int unit, int *min, int *max);

#endif /* BCMI_LTSW_MBCM_VIRTUAL_H */
