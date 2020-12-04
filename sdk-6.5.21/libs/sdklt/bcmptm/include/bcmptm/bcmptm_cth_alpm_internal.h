/*! \file bcmptm_cth_alpm_internal.h
 *
 * APIs, defines for PTM to interface with ALPM Custom Handler
 *
 * This file contains APIs, defines for CTH-ALPM interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CTH_ALPM_INTERNAL_H
#define BCMPTM_CTH_ALPM_INTERNAL_H


/*******************************************************************************
  Includes
 */
#include <bcmlrd/bcmlrd_types.h>
#include <bcmptm/bcmptm_cth_alpm_be_internal.h>


/*******************************************************************************
 * Defines
 */

#define REGS_ENTRY_WORDS_MAX    20

/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief ALPM register entry
 */
typedef uint32_t alpm_control_entry_t[REGS_ENTRY_WORDS_MAX];

/*!
 * \brief CTH_ALPM device specific info.
 */
typedef struct cth_alpm_device_spec_s {
    int tcam_depth;             /*!< TCAM depth */
    int tcam_blocks;            /*!< TCAM blocks */
    int l1_blocks;              /*!< Number of L1 blocks supported */

    bcmlrd_sid_t l1_sid[ALPM_L1_BANKS_MAX];     /*!< ALPM level 1 symbol ID */
    bcmlrd_sid_t l2_sid[ALPM_LN_BANKS_MAX];     /*!< ALPM level 2 symbol ID */
    bcmlrd_sid_t l3_sid[ALPM_LN_BANKS_MAX];     /*!< ALPM level 3 symbol ID */
    bcmptm_cth_alpm_control_t default_control;  /*!< Default ALPM controls */
} cth_alpm_device_spec_t;


/*!
 * \brief CTH_ALPM device specific info.
 */
typedef struct cth_alpm_device_info_s {
    cth_alpm_device_spec_t spec;/*!< ALPM device specs */
    int feature_urpf;           /*!< uRPF feature supported */
    int feature_pkms;           /*!< Number of packing modes supported */
    uint8_t l1_tcams_shared;    /*!< L1 TCAM resources are shared between
                                 * RM_TCAM and RM_ALPM */
} cth_alpm_device_info_t;

/*!
 * \brief ALPM register info
 */
typedef struct cth_alpm_control_pt_s {
    uint8_t count;                  /*!< Number of registers */
    bcmdrd_sid_t *sid;              /*!< Symbol ids of registers */
    alpm_control_entry_t *entry;    /*!< Entry words of registers */
} cth_alpm_control_pt_t;

/*! Function type for device info initialization */
typedef int (*cth_alpm_device_info_init_f) (int u, int m,
                                            cth_alpm_device_info_t *device);

/*! Function type for ALPM_CONTROL table sed. */
typedef int (*cth_alpm_device_control_validate_f)
                                        (int u, int m, uint8_t stage,
                                         bcmptm_cth_alpm_control_t *control);


/*! Function type for ALPM_CONTROL table sed. */
typedef int (*cth_alpm_device_control_encode_f)
                                        (int u, int m,
                                         bcmptm_cth_alpm_control_t *control,
                                         cth_alpm_control_pt_t *control_pt);

/*!
 * \brief CTH_ALPM device specific routines
 */
typedef struct cth_alpm_driver_s {
    /*! Callback for device info initialization */
    cth_alpm_device_info_init_f device_info_init;

    /*! Callback for table encode */
    cth_alpm_device_control_encode_f control_encode;

    /*! Callback for control info validation */
    cth_alpm_device_control_validate_f control_validate;
} bcmptm_cth_alpm_driver_t;

/*******************************************************************************
  Function prototypes
 */

/*!
 * \brief Stop the custom handler for ALPM.
 * \param [in] u Logical device id
 *
 * \retval SHR_E_NONE if success.
 */
extern int
bcmptm_cth_alpm_stop(int u);

/*!
 * \brief Initialize the custom handler for ALPM.
 * \param [in] u Logical device id
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_alpm_init(int u, bool warm);

/*!
 * \brief De-initialize the custom handler for ALPM.
 * \param [in] u Logical device id
 * \param [in] warm Warm cleanup
 *
 * \retval SHR_E_NONE if success.
 */
extern int
bcmptm_cth_alpm_cleanup(int u, bool warm);
/*!
 * \brief Register device specfic driver for CTH_ALPM
 *
 * \param [in] u Device u.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_cth_alpm_driver_register(int u, bcmptm_cth_alpm_driver_t *drv);
#endif /* BCMPTM_CTH_ALPM_INTERNAL_H */

