/*! \file bcmecmp_rh_util.h
 *
 * Resilient hashing re-balance utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_RH_UTIL_H
#define BCMECMP_RH_UTIL_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>

/*! Shift operation of resilient hashing random seed. */
#define RH_RAND_SEED_SHIFT 16

/*! Define RH algorithm operation code. */
typedef enum {
    RH_INIT,
    RH_ADD,
    RH_DEL
} rh_op_e;

/*! Define return parameter struture. */
typedef struct rh_memb_info_s {
    /*! rh entry infomation */
    void *entry;

    /*! rh algorithm operation encode */
    rh_op_e opc;

    /*! rh member index */
    uint32_t mindex;

    /*! rh entry index */
    uint32_t eindex;

    /*! rh user data */
    uint32_t arg;

} rh_memb_info_t;

/*! Define callback prototype of rh member handle routine. */
typedef int (* bcmecmp_rh_memb_fn)(int unit, void *data);

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes NUM_PATHS group members among RH_SIZE member table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] eindex entry index.
 * \param [in] entry Pointer to logical table data structure.
 * \param [in] cb Resilient hashing group member callback handler.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmecmp_rh_elem_member_init(int unit,
                            uint32_t eindex,
                            void *entry,
                            bcmecmp_rh_memb_fn cb);

/*!
 * \brief Add new member to an existing RH element.
 *
 * This function adds a new member to an existing RH element and re-balances
 * the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] new_mindex New RH group member array index.
 * \param [in] entry Pointer to logical table data structure.
 * \param [in] cb Resilient hashing group member callback handler.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmecmp_rh_elem_member_add(int unit,
                           uint32_t new_mindex,
                           void *entry,
                           bcmecmp_rh_memb_fn cb);

/*!
 * \brief Delete a member from an existing RH element.
 *
 * This function deletes a member from an existing RH element and
 * re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] del_mindex New RH group member array index.
 * \param [in] entry Pointer to logical table data structure.
 * \param [in] cb Resilient hashing group member callback handler.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
extern int
bcmecmp_rh_elem_member_delete(int unit,
                              uint32_t del_mindex,
                              void *entry,
                              bcmecmp_rh_memb_fn cb);

#endif /* BCMECMP_RH_UTIL_H */
