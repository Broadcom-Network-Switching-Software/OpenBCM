/*! \file bcmmgmt_sysm.h
 *
 * Component definitions for system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMMGMT_SYSM_H
#define BCMMGMT_SYSM_H

#include <sal/sal_types.h>

/*!
 * \brief System Manager component IDs for SDK.
 *
 * Component owner should add his/her component ID here
 */
typedef enum bcmmgmt_sysm_comp_id_e {
    /*! \cond Define enums */
#define BCMMGMT_COMPLIST_ENTRY(_name, _id) \
    BCMMGMT_##_name##_COMP_ID = _id,
#include <bcmmgmt/bcmmgmt_complist.h>
    BCMMGMT_MAX_COMP_ID
    /*! \endcond */
} bcmmgmt_sysm_comp_id_t;

/*!
 * \brief Component start callback function signature.
 *
 * This function signature is used by each component to register itself
 * with the different system manager categories.
 * This function will be the first function of the component being called.
 * Its only purpose is to register the system manager callback functions
 * for this component. No other activity should be performed.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcmmgmt_comp_start_f)(void);

/*!
 * \brief System Manager component list.
 *
 * This structure contains a list of component start functions, which
 * are used to register each component with the System Manager.
 */
typedef struct bcmmgmt_comp_list_s {

    /*! Array of component start functions. */
    const bcmmgmt_comp_start_f *comp_start;

    /*! Number of entries in start function array. */
    const uint32_t num_comp;

} bcmmgmt_comp_list_t;

#endif /* BCMMGMT_SYSM_H */
