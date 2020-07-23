/*! \file l3_intf.h
 *
 * BCM L3 intf module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_L3_INTF_H
#define BCMI_LTSW_MBCM_L3_INTF_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_intf_int.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/types.h>

/*!
 * \brief Get the L3 interface table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db L3 interface table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*l3_intf_tbl_db_get_f)(int unit,
                                    bcmint_ltsw_l3_intf_tbl_db_t *tbl_db);

/*!
 * \brief Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_init_f)(int unit);

/*!
 * \brief De-initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_deinit_f)(int unit);

/*!
 * \brief Get ingress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_id_range_get_f)(int unit, int *min, int *max);

/*!
 * \brief Get ingress class id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum class id.
 * \param [out] max Maximum class id.
 * \param [out] width Number of bits of class id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_class_id_range_get_f)(int unit, int *min, int *max,
                                           int *width);

/*!
 * \brief Create an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_create_f)(int unit,
                                    bcm_l3_ingress_t *intf_info,
                                    bcm_if_t *intf_id);

/*!
 * \brief Destroy an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_destroy_f)(int unit, bcm_if_t intf_id);

/*!
 * \brief Get an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 * \param [OUT] ing_intf Ingress interface information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_get_f)(int unit, bcm_if_t intf_id,
                                 bcm_l3_ingress_t *ing_intf);

/*!
 * \brief Find an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ing_intf Ingress interface information.
 * \param [OUT] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_find_f)(int unit, bcm_l3_ingress_t *ing_intf,
                                  bcm_if_t *intf_id);

/*!
 * \brief Traverse all valid ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] trav_fn Callback function.
 * \param [In] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_traverse_f)(int unit,
                                      bcm_l3_ingress_traverse_cb trav_fn,
                                      void *user_data);

/*!
 * \brief Update rp_id profile of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] set Set or unset the rp_id to rp_id_profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_rp_op_f)(int unit, int intf_id, int rp_id, int set);

/*!
 * \brief Get rp_id of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [out] rp_bmp Bitmap of rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_rp_get_f)(int unit, int intf_id, uint32_t *rp_bmp);

/*!
 * \brief Attach flexctr action to the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_flexctr_attach_f)(
        int unit,
        bcm_if_t intf_id,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_flexctr_detach_f)(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_ing_intf_flexctr_info_get_f)(
        int unit,
        bcm_if_t intf_id,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get egress interface id for CPU.
 *
 * \param [in] unit Unit number.
 * \param [out] cpu_intf_id CPU interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_l2cpu_intf_id_get_f)(int unit, int *cpu_intf_id);

/*!
 * \brief Get egress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [in] type Egress interface type.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_id_range_get_f)(int unit,
                                          bcmi_ltsw_l3_egr_intf_type_t type,
                                          int *min, int *max);

/*!
 * \brief Get egress interface LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress interface id.
 * \param [out] index LT index.
 * \param [out] type Egress interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_to_index_f)(int unit, bcm_if_t intf_id, int *index,
                                      bcmi_ltsw_l3_egr_intf_type_t *type);

/*!
 * \brief Get egress interface id from LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] index LT index.
 * \param [in] type Egress interface type.
 * \param [out] intf_id Egress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_index_to_intf_f)(int unit, int index,
                                      bcmi_ltsw_l3_egr_intf_type_t type,
                                      bcm_if_t *intf_id);

/*!
 * \brief Create an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_create_f)(int unit,
                                    bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Get an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_get_f)(int unit,
                                 bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Find an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE  errors.
 * \retval !SHR_E_NONE .
 */
typedef int (*l3_egr_intf_find_f)(int unit,
                                  bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE        No errors.
 * \retval !SHR_E_NONE       Failure.
 */
typedef int (*l3_egr_intf_delete_f)(int unit,
                                    bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete all L3 egress interfaces.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_delete_all_f)(int unit);

/*!
 * \brief Attach flexctr action to the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_flexctr_attach_f)(
        int unit,
        bcm_if_t intf_id,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_flexctr_detach_f)(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an engress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_egr_intf_flexctr_info_get_f)(
        int unit,
        bcm_if_t intf_id,
        bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Internal function to set L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] tnl_type Tunnel type.
 * \param [in] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_tnl_init_set_f)(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx);

/*!
 * \brief Internal function to get L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [out] tnl_type Tunnel type.
 * \param [out] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_tnl_init_get_f)(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx);

/*!
 * \brief Internal function to reset L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_tnl_init_reset_f)(int unit, int intf_if);

/*!
 * \brief Update the vfi information for a given vid.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VFI ID.
 * \param [in] vfi_info VFI information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_vfi_update_f)(int unit, int vid,
                                    bcmi_ltsw_l3_intf_vfi_t *vfi_info);

/*!
 * \brief Recover all MPLS information from L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] user_data User data of callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int(*l3_intf_mpls_recover_all_f)(int unit,
                                         bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                         void *user_data);

/*!
 * \brief Internal function to set L3 egress interface adaptation key field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] key_type Key type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*l3_intf_adapt_lookup_key_set_f)(
    int unit,
    int intf_id,
    int key_type);

/*!
 * \brief L3 function vector driver.
 */
typedef struct mbcm_ltsw_l3_intf_drv_s {
    /*! Get L3 interface table databse. */
    l3_intf_tbl_db_get_f l3_intf_tbl_db_get;

    /*! Initialize L3 intf module. */
    l3_intf_init_f l3_intf_init;

    /*! De-Initialize L3 intf Module. */
    l3_intf_deinit_f l3_intf_deinit;

    /*! Get ingress interface id range. */
    l3_ing_intf_id_range_get_f l3_ing_intf_id_range_get;

    /*! Get ingress class id range. */
    l3_ing_class_id_range_get_f l3_ing_class_id_range_get;

    /*! Create an L3 ingress interface. */
    l3_ing_intf_create_f l3_ing_intf_create;

    /*! Destroy an L3 ingress interface. */
    l3_ing_intf_destroy_f l3_ing_intf_destroy;

    /*! Get an L3 ingress interface. */
    l3_ing_intf_get_f l3_ing_intf_get;

    /*! Find an L3 ingress interface. */
    l3_ing_intf_find_f l3_ing_intf_find;

    /*! Traverse L3 ingress interface. */
    l3_ing_intf_traverse_f l3_ing_intf_traverse;

    /*! Add or delete a rp_id to rp_id_profile of a ingress interface. */
    l3_ing_intf_rp_op_f l3_ing_intf_rp_op;

    /*! Get rp_id of a ingress interface. */
    l3_ing_intf_rp_get_f l3_ing_intf_rp_get;

    /*! Attach flexctr action to the given L3 ingress interface. */
    l3_ing_intf_flexctr_attach_f l3_ing_intf_flexctr_attach;

    /*! Detach flexctr action of the given L3 ingress interface. */
    l3_ing_intf_flexctr_detach_f l3_ing_intf_flexctr_detach;

    /*! Get flex counter information of the given L3 ingress interface. */
    l3_ing_intf_flexctr_info_get_f l3_ing_intf_flexctr_info_get;

    /*! Get egress interface id for CPU. */
    l3_egr_l2cpu_intf_id_get_f l3_egr_l2cpu_intf_id_get;

    /*! Get egress interface id range. */
    l3_egr_intf_id_range_get_f l3_egr_intf_id_range_get;

    /*! Get LT index from egress interface id. */
    l3_egr_intf_to_index_f l3_egr_intf_to_index;

    /*! Get egress interface id from LT index. */
    l3_egr_index_to_intf_f l3_egr_index_to_intf;

    /*! Create an L3 egress interface. */
    l3_egr_intf_create_f l3_egr_intf_create;

    /*! Get an L3 egress interface. */
    l3_egr_intf_get_f l3_egr_intf_get;

    /*! Find an L3 egress interface. */
    l3_egr_intf_find_f l3_egr_intf_find;

    /*! Delete an L3 egress interface. */
    l3_egr_intf_delete_f l3_egr_intf_delete;

    /*! Delete all L3 egress interfaces. */
    l3_egr_intf_delete_all_f l3_egr_intf_delete_all;

    /*! Attach flexctr action to the given L3 egress interface. */
    l3_egr_intf_flexctr_attach_f l3_egr_intf_flexctr_attach;

    /*! Detach flexctr action of the given L3 egress interface. */
    l3_egr_intf_flexctr_detach_f l3_egr_intf_flexctr_detach;

    /*! Get flex counter information of the given L3 egress interface. */
    l3_egr_intf_flexctr_info_get_f l3_egr_intf_flexctr_info_get;

    /*! Set tunnel initiator index. */
    l3_intf_tnl_init_set_f l3_intf_tnl_init_set;

    /*! Get tunnel initiator index. */
    l3_intf_tnl_init_get_f l3_intf_tnl_init_get;

    /*! Reset tunnel initiator index. */
    l3_intf_tnl_init_reset_f l3_intf_tnl_init_reset;

    /*! Update the VFI information for a given vid. */
    l3_intf_vfi_update_f l3_intf_vfi_update;

    /*! Recover all MPLS information from L3 interface. */
    l3_intf_mpls_recover_all_f l3_intf_mpls_recover_all;

    /*! Set adaptation key. */
    l3_intf_adapt_lookup_key_set_f l3_intf_adapt_lookup_key_set;

} mbcm_ltsw_l3_intf_drv_t;

/*!
 * \brief Set the L3 driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Vlan driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_intf_drv_set(int unit, mbcm_ltsw_l3_intf_drv_t *drv);

/*!
 * \brief Get the L3 interface table database.
 *
 * \param [in] unit Unit number.
 * \param [out] tbl_db L3 interface table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
mbcm_ltsw_l3_intf_tbl_db_get(int unit,
                             bcmint_ltsw_l3_intf_tbl_db_t *tbl_db);

/*!
 * \brief Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_init(int unit);

/*!
 * \brief De-Initialize L3 intf module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_deinit(int unit);

/*!
 * \brief Get ingress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max);

/*!
 * \brief Get ingress class id range.
 *
 * \param [in] unit Unit number.
 * \param [out] min Minimum class id.
 * \param [out] max Maximum class id.
 * \param [out] width Number of bits of class id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width);

/*!
 * \brief Create an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_create(int unit, bcm_l3_ingress_t *intf_info,
                             bcm_if_t *intf_id);

/*!
 * \brief Destroy an L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_destroy(int unit, bcm_if_t intf_id);

/*!
 * \brief Get an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id.
 * \param [OUT] ing_intf Ingress interface information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_get(int unit, bcm_if_t intf_id,
                          bcm_l3_ingress_t *ing_intf);

/*!
 * \brief Find an ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ing_intf Ingress interface information.
 * \param [OUT] intf_id L3 ingress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_find(int unit, bcm_l3_ingress_t *ing_intf,
                           bcm_if_t *intf_id);

/*!
 * \brief Traverse all valid ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [In] trav_fn Callback function.
 * \param [In] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_traverse(int unit,
                               bcm_l3_ingress_traverse_cb trav_fn,
                               void *user_data);

/*!
 * \brief Update rp_id profile of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] set Set or unset the rp_id to rp_id_profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_rp_op(int unit, int intf_id, int rp_id, int set);

/*!
 * \brief Get rp_id of a given ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [out] rp_bmp Bitmap of rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32_t *rp_bmp);

/*!
 * \brief Attach flexctr action to the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_flexctr_attach(int unit,
                                     bcm_if_t intf_id,
                                     bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_flexctr_detach(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_ing_intf_flexctr_info_get(int unit,
                                       bcm_if_t intf_id,
                                       bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get egress interface id for CPU.
 *
 * \param [in] unit Unit number.
 * \param [out] cpu_intf_id CPU interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_l2cpu_intf_id_get(int unit, int *cpu_intf_id);

/*!
 * \brief Get egress interface id range.
 *
 * \param [in] unit Unit number.
 * \param [in] type Egress interface type.
 * \param [out] min Minimum interface id of the given type.
 * \param [out] max Maximum interface id of the given type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_id_range_get(int unit,
                                   bcmi_ltsw_l3_egr_intf_type_t type,
                                   int *min, int *max);

/*!
 * \brief Get egress interface LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress interface id.
 * \param [out] index LT index.
 * \param [out] type Egress interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
                               bcmi_ltsw_l3_egr_intf_type_t *type);

/*!
 * \brief Get egress interface id from LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] index LT index.
 * \param [in] type Egress interface type.
 * \param [out] intf_id Egress interface id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_index_to_intf(int unit, int index,
                               bcmi_ltsw_l3_egr_intf_type_t type,
                               bcm_if_t *intf_id);

/*!
 * \brief Create an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_create(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Get an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_get(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Find an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_find(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete an L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_info Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_delete(int unit, bcmint_ltsw_l3_egr_intf_t *intf_info);

/*!
 * \brief Delete all L3 egress interfaces.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_delete_all(int unit);

/*!
 * \brief Attach flexctr action to the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [in] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_flexctr_attach(int unit,
                                     bcm_if_t intf_id,
                                     bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach flexctr action of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_flexctr_detach(int unit, bcm_if_t intf_id);

/*!
 * \brief Get flex counter information of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [out] info Flex counter information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_egr_intf_flexctr_info_get(int unit,
                                       bcm_if_t intf_id,
                                       bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Internal function to set L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] tnl_type Tunnel type.
 * \param [in] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx);

/*!
 * \brief Internal function to get L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [out] tnl_type Tunnel type.
 * \param [out] tnl_idx Tunnel initiator index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx);


/*!
 * \brief Internal function to reset L3 egress interface tunnel initiator field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_tnl_init_reset(int unit, int intf_id);

/*!
 * \brief Update the VFI information for a given vid.
 *
 * \param [in] unit Unit number.
 * \param [in] vid VFI ID.
 * \param [in] vfi_info VFI information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_vfi_update(int unit, int vid,
                             bcmi_ltsw_l3_intf_vfi_t *vfi_info);

/*!
 * \brief Recover all MPLS information from L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] user_data User data of callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_mpls_recover_all(int unit,
                                   bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                   void *user_data);

/*!
 * \brief Internal function to set L3 egress interface adaptation key field.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface index.
 * \param [in] key_type Key type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type);


#endif /* BCMI_LTSW_MBCM_L3_INTF_H */

