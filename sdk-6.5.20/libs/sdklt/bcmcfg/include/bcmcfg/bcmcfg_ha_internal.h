/*! \file bcmcfg_ha_internal.h
 *
 *  \brief BCMCFG HA internal interfaces
 *
 * These APIs allow alternative component scanners to be tested.
 *
 * Not intended for production use.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_HA_INTERNAL_H
#define BCMCFG_HA_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmcfg/bcmcfg_types.h>

/*! HA signature for LT_CONFIG */
#define BCMCFG_HA_SIGN_LT_CONFIG              (0xCFAA0001)

/*! HA signature for Component Resource. */
#define BCMCFG_HA_SIGN_COMP_RESOURCE          (0xCFAA0002)

/*! HA signature for Read data */
#define BCMCFG_HA_SIGN_READ_DATA              (0xCFAA0003)

/*! HA signature for Variant data */
#define BCMCFG_HA_SIGN_VARIANT_CONFIG         (0xCFAA0004)

/*! HA sub-module IDs */
typedef enum {
    /* HA SUBID for config data */
    BCMCFG_HA_SUBID_LT_CONFIG = 0,

    /* HA SUBID for component resource. */
    BCMCFG_HA_SUBID_COMP_RESOURCE = 1,

    /* HA SUBID for reader data. */
    BCMCFG_HA_SUBID_READ_DATA = 2,

    /* HA SUBID for variant data. */
    BCMCFG_HA_SUBID_VARIANT_CONFIG = 3,

} bcmcfg_ha_sub_id_t;

/*! BCMCFG HA memory allocation information. */
typedef struct bcmcfg_ha_alloc_info_s {
    /*! Allocated pointer to HA memory. */
    uint8_t *alloc_ptr;

    /*! Allocated size in HA memory. */
    uint32_t  size;
} bcmcfg_ha_alloc_info_t;

/*!
 * \brief Initialize BCMCFG HA data for variant config data.
 *
 * Initialize BCMCFG HA data for variant config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_variant_config_init(int unit, bool warm);

/*!
 * \brief Save BCMCFG HA data for variant config data.
 *
 * Save BCMCFG HA data for variant config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_variant_config_save(int unit, bool warm);

/*!
 * \brief Restore BCMCFG HA data for variant config data.
 *
 * Restore BCMCFG HA data for variant config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_variant_config_restore(int unit, bool warm);

/*!
 * \brief Initialize BCMCFG HA data for LT config data.
 *
 * Initialize BCMCFG HA data for LT config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_lt_config_init(int unit, bool warm);

/*!
 * \brief Save BCMCFG HA data for LT config data.
 *
 * Save BCMCFG HA data for LT config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_lt_config_save(int unit, bool warm);

/*!
 * \brief Restore BCMCFG HA data for LT config data.
 *
 * Restore BCMCFG HA data for LT config data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_lt_config_restore(int unit, bool warm);

/*!
 * \brief Initialize BCMCFG HA data for component resource.
 *
 * Initialize BCMCFG HA data for component resource.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_comp_resource_init(bool warm);

/*!
 * \brief Save BCMCFG HA data for component resource.
 *
 * Save BCMCFG HA data for component resource.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_comp_resource_save(bool warm);

/*!
 * \brief Restore BCMCFG HA data for component resource.
 *
 * Restore BCMCFG HA data for component resource.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_comp_resource_restore(bool warm);

/*!
 * \brief Allocate block in HA mem during per-unit initialization.
 *
 * For coldboot: Allocated mem will be zeroed out, signature added.
 * For warmboot: Signature word will be verified.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 * \param [in] in_req_size Number of requested bytes.
 * \param [in] submod_name Name of requestor submodule.
 * \param [in] submod_id ID of requestor submodule.
 * \param [in] submod_string ID string of requestor submodule.
 * \param [in] ref_sign Signature.
 *
 * \param [out] alloc_ptr Pointer to allocated mem. NULL implies allocation
 * failure.
 *
 * \retval SHR_E_NONE
 */
extern int
bcmcfg_ha_unit_alloc(int unit, bool warm, uint32_t in_req_size,
                     const char *submod_name, uint8_t submod_id,
                     const char *submod_string,
                     uint32_t ref_sign, uint8_t **alloc_ptr);

/*!
 * \brief Allocate block in HA mem during generic initialization.
 *
 * For coldboot: Allocated mem will be zeroed out, signature added.
 * For warmboot: Signature word will be verified.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 * \param [in] in_req_size Number of requested bytes.
 * \param [in] submod_name Name of requestor submodule.
 * \param [in] submod_id ID of requestor submodule.
 * \param [in] submod_string ID string of requestor submodule.
 * \param [in] ref_sign Signature.
 *
 * \param [out] alloc_ptr Pointer to allocated mem. NULL implies allocation
 * failure.
 *
 * \retval SHR_E_NONE
 */
extern int
bcmcfg_ha_gen_alloc(bool warm, uint32_t in_req_size,
                    const char *submod_name, uint8_t submod_id,
                    const char *submod_string,
                    uint32_t ref_sign, uint8_t **alloc_ptr);

/*!
 * \brief Initialize BCMCFG HA.
 *
 * Initialize BCMCFG HA.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_generic_init(bool warm);

/*!
 * \brief Deinitialize BCMCFG HA.
 *
 * Deinitialize BCMCFG HA.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_generic_deinit(void);

/*!
 * \brief Initialize BCMCFG HA read data.
 *
 * Initialize BCMCFG HA read data.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_read_data_init(bool warm);

/*!
 * \brief Save BCMCFG HA read data.
 *
 * Save BCMCFG HA read data.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_read_data_save(bool warm);

/*!
 * \brief Restore BCMCFG HA read data.
 *
 * Restore BCMCFG HA read data.
 *
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_read_data_restore(bool warm);

/*!
 * \brief Initialize BCMCFG HA data.
 *
 * Initialize BCMCFG HA data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_data_init(int unit, bool warm);

/*!
 * \brief Save BCMCFG HA data.
 *
 * Save BCMCFG HA data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_data_save(int unit, bool warm);

/*!
 * \brief Restore BCMCFG HA data.
 *
 * Restore BCMCFG HA data.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 *
 * \retval SHR_E_NONE   OK
 * \retval !SHR_E_NONE  Error
 */
extern int
bcmcfg_ha_data_restore(int unit, bool warm);

#endif /* BCMCFG_HA_INTERNAL_H */
