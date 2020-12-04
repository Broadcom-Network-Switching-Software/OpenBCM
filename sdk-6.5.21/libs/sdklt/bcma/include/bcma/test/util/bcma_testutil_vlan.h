/*! \file bcma_testutil_vlan.h
 *
 * VLAN operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_VLAN_H
#define BCMA_TESTUTIL_VLAN_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Create a VLAN entity.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] ing_egrmbr_pid The profile ID for egress member ports in ingress pipeline.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_create(int unit, int vid, int ing_egrmbr_pid);

/*!
 * \brief Destroy a VLAN entity.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_destroy(int unit, int vid);

/*!
 * \brief Check if a VLAN entity exist.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_check(int unit, int vid);

/*!
 * \brief Add ports into VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] ubmp The untagged port bitmap.
 * \param [in] src_port_block Whether to disable forwarding to source port
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_port_add(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                            const bcmdrd_pbmp_t ubmp, bool src_port_block);

/*!
 * \brief Update ports of VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] ubmp The untagged port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_port_update(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                               const bcmdrd_pbmp_t ubmp);

/*!
 * \brief Remove ports from VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_port_remove(int unit, int vid, const bcmdrd_pbmp_t pbmp);

/*!
 * \brief Set port-based VLAN Id.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port ID.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_port_vlan_set(int unit, int port, int vid);

/*! STP state */
typedef enum bcma_testutil_stp_state_e {
    BCMA_TESTUTIL_STP_DISALBE = 0,
    BCMA_TESTUTIL_STP_BLOCK,
    BCMA_TESTUTIL_STP_LEARN,
    BCMA_TESTUTIL_STP_FORWARD,
    BCMA_TESTUTIL_STP_COUNT
} bcma_testutil_stp_state_t;

/*!
 * \brief Set port stp state.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] stp_state The stp state.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_stp_set(int unit, int vid, const bcmdrd_pbmp_t pbmp,
                           bcma_testutil_stp_state_t stp_state);

/*! tag action */
typedef enum bcma_testutil_vlan_tag_action_e {
    BCMA_TESTUTIL_VLAN_TAG_ACTION_DO_NOT_MODIFY = 0,
    BCMA_TESTUTIL_VLAN_TAG_ACTION_ADD,
    BCMA_TESTUTIL_VLAN_TAG_ACTION_COUNT
} bcma_testutil_vlan_tag_action_t;

/*!
 * \brief Add action for vlan untag packet
 *
 * \param [in] unit Unit number.
 * \param [in] tag_action The tag action.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_untag_action_add(int unit,
                                    bcma_testutil_vlan_tag_action_t tag_action);

/*!
 * \brief Remove action for vlan untag packet
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_vlan_untag_action_remove(int unit);


/*!
 * \brief Create a VLAN entity.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] ing_egrmbr_pid The profile ID for egress member ports in ingress pipeline.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_create_f)(int unit, int vid,
                                           int ing_egrmbr_pid);

/*!
 * \brief Check a VLAN entity.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_check_f)(int unit, int vid);


/*!
 * \brief Destroy a VLAN entity.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_destroy_f)(int unit, int vid);

/*!
 * \brief Add ports into VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] ubmp The untagged port bitmap.
 * \param [in] src_port_block Whether to disable forwarding to source port
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_port_add_f)(int unit, int vid,
                                             const bcmdrd_pbmp_t pbmp,
                                             const bcmdrd_pbmp_t ubmp,
                                             bool src_port_block);

/*!
 * \brief Update ports of VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] ubmp The untagged port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_port_update_f)(int unit, int vid,
                                                const bcmdrd_pbmp_t pbmp,
                                                const bcmdrd_pbmp_t ubmp);

/*!
 * \brief Remove ports from VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_port_remove_f)(int unit, int vid,
                                                const bcmdrd_pbmp_t pbmp);

/*!
 * \brief Set port-based VLAN Id.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port ID.
 * \param [in] vid The Vlan ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_port_vlan_set_f)(int unit, int port, int vid);

/*!
 * \brief Set port stp state.
 *
 * \param [in] unit Unit number.
 * \param [in] vid The Vlan ID.
 * \param [in] pbmp The port bitmap.
 * \param [in] stp_state The stp state.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_stp_set_f)(int unit, int vid,
                                            const bcmdrd_pbmp_t pbmp,
                                            bcma_testutil_stp_state_t stp_state);

/*!
 * \brief Add action for vlan untag packet
 *
 * \param [in] unit Unit number.
 * \param [in] tag_action The tag action.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_untag_action_add_f)(int unit,
                                                     bcma_testutil_vlan_tag_action_t tag_action);

/*!
 * \brief Remove action for vlan untag packet
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_vlan_untag_action_remove_f)(int unit);

/*! VLAN operation functions. */
typedef struct bcma_testutil_vlan_op_s {
    /*! VLAN create function. */
    bcma_testutil_vlan_check_f check;

    /*! VLAN create function. */
    bcma_testutil_vlan_create_f create;

    /*! VLAN destroy function. */
    bcma_testutil_vlan_destroy_f destroy;

    /*! VLAN add port function. */
    bcma_testutil_vlan_port_add_f port_add;

    /*! VLAN update port function. */
    bcma_testutil_vlan_port_update_f port_update;

    /*! VLAN remove port function. */
    bcma_testutil_vlan_port_remove_f port_remove;

    /*! Port-based VLAN set function. */
    bcma_testutil_vlan_port_vlan_set_f port_vlan_set;

    /*! VLAN stp set function. */
    bcma_testutil_vlan_stp_set_f stp_set;

    /*! VLAN untag action add function. */
    bcma_testutil_vlan_untag_action_add_f untag_action_add;

    /*! VLAN untag action remove function. */
    bcma_testutil_vlan_untag_action_remove_f untag_action_remove;
} bcma_testutil_vlan_op_t;

/*! XGS device VLAN operation functions. */
extern bcma_testutil_vlan_op_t *bcma_testutil_xgs_vlan_op_get(int unit);

/*! XGS device VFI operation functions. */
extern bcma_testutil_vlan_op_t *bcma_testutil_xgs_vfi_op_get(int unit);

/*! XFS device VLAN operation functions. */
extern bcma_testutil_vlan_op_t *bcma_testutil_xfs_vlan_op_get(int unit);

#endif /* BCMA_TESTUTIL_VLAN_H */
