/*! \file bcma_bcmpkt_test_internal.h
 *
 * Declaration of the internal structures, enumerations, and functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKT_TEST_INTERNAL_H
#define BCMA_BCMPKT_TEST_INTERNAL_H

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int bcma_bcmpkt_test_##_bd##_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \brief IFP entry priority for creating datapath. */
#define PRIORITY_HIGH              0xFFFF
/*! \brief IFP group ingress port bitmap for creating datapath. */
#define IFP_GROUP_INPORT_BITMAP    0x3
/*! \brief IFP group mode for creating datapath. */
#define IFP_GROUP_MODE_AUTO        1
/*! \brief IFP group priority for creating datapath. */
#define IFP_GROUP_PRI              1
/*! \brief IFP rule ingress port mask for creating datapath. */
#define IFP_RULE_INPORT_MASK       0x3

/*! \brief Last byte of Destination MAC address of test packet. */
#define DA_BYTE_5   0x11
/*! \brief Last byte of Source MAC address of test packet. */
#define SA_BYTE_5   0x22

/*!
 * \brief IFP qualifier types for creating datapath.
 */
typedef enum fp_qual_type_e{
    /*! For qualify INPORT. */
    FP_QUAL_INPORT = 0,

    /*! Must be the last. */
    FP_QUAL_COUNT
} fp_qual_type_t;

/*!
 * \brief IFP policy types for creating datapath.
 */
typedef enum fp_policy_type_e{
    /*! For drop policy. */
    FP_POLICY_DROP = 0,

    /*! For copy-to-cpu and redirect-to-port policy. */
    FP_POLICY_COPYTOCPU,

    /*! Must be the last. */
    FP_POLICY_COUNT
} fp_policy_type_t;

/*!
 * \brief Create datapath of packet test.
 *
 * The datapath for pkttest command includes baseline device configuration for
 * forwarding the test flow (if required) and creation of IFP entry and
 * policies.
 * For pkttest rx case, the IFP entry attaches the policy of copy-to-cpu and
 * redirect-to-port (to a front port in loopback mode) at the beginning of
 * each test loop for examing the CPU received packets and generating the
 * continuous test flow. At the end of each test loop, the IFP entry detach
 * the original policy and attach the drop policy to avoid flooding.
 * For pkttest tx case, the IFP entry attaches the drop policy at the beginning
 * of the test since the purpose is only for measuring the CPU transmission.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port that is in loopback mode for the test.
 * \param [in] redirect_port Redirect-to port of redirect policy.
 * \param [in] ifp_cfg IFP information of the datapath.
 *
 * \retval SHR_E_NONE Datapath creation is succeeded, error code otherwise.
 */
typedef int
(*bcma_bcmpkt_test_dpath_create_f)(int unit, int port, int redirect_port,
                                   bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Destroy datapath of packet test.
 *
 * Recover configurations created during pkttest command execution.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port that is in loopback mode for the test.
 * \param [in] ifp_cfg IFP information of the datapath.
 *
 * \retval SHR_E_NONE Datapath destroy is succeeded, error code otherwise.
 */
typedef int
(*bcma_bcmpkt_test_dpath_destroy_f)(int unit, int port,
                                    bcma_bcmpkt_test_fp_cfg_t *ifp_cfg);

/*!
 * \brief Update policy of a created IFP entry.
 *
 * Attach the policy that is needed for certain test procedure to the IFP entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry_id The IFP entry to be updated.
 * \param [in] policy_id The new policy.
 *
 * \retval SHR_E_NONE IFP policy update is succeeded, error code otherwise.
 */
typedef int
(*bcma_bcmpkt_test_lt_policy_update_f)(int unit, uint64_t entry_id,
                                       uint64_t policy_id);

/*!
 * \brief Chip-specific driver.
 *
 * These function pointers will be attached to the specific implementation
 * when pkttest tx or rx command executed.
 */
typedef struct bcma_bcmpkt_test_drv_s {
    /*! Create datapath of packet test. */
    bcma_bcmpkt_test_dpath_create_f dpath_create;

    /*! Destroy datapath of packet test. */
    bcma_bcmpkt_test_dpath_destroy_f dpath_destroy;

    /*! Update policy of a created IFP entry. */
    bcma_bcmpkt_test_lt_policy_update_f lt_policy_update;
} bcma_bcmpkt_test_drv_t;

/*!
 * \brief Set the chip specific drivers to common driver pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] drv The chip spefic drivers.
 *
 * \retval SHR_E_NONE Attaching driver is succeeded, error code otherwise.
 */
extern int
bcma_bcmpkt_test_drv_set(int unit, bcma_bcmpkt_test_drv_t *drv);

#endif /* BCMA_BCMPKT_TEST_INTERNAL_H */

