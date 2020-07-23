/*! \file bcma_testcase.h
 *
 * Broadcom Test Case.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_H
#define BCMA_TESTCASE_H

#include <sal/sal_types.h>

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Test procedures of test case.
 */
typedef struct bcma_test_proc_s {

    /*! Description of test procedure */
    const char *desc;

    /*!
     * \brief Callback function of procedure.
     *
     * This function is used to execute this procedure.
     *
     * \param [in] u Device Number.
     * \param [in] bp Buffer pointer for saving the case-specific parameters.
     * \param [in] option Test Database Option.
     *
     * \retval SHR_E_NONE Execute this procedure successfully.
     * \retval SHR_E_PARAM Invalid parameters.
     */
    int (*cb)(int u, void *bp, uint32_t option);

} bcma_test_proc_t;

/*!
 * \brief Test related operating routines.
 */
typedef struct bcma_test_op_s {

    /*!
     * \brief For checking whether test case is selected on this unit or not.
     *
     * This function is for checking whether test case is selected on this unit.
     *
     * \param [in] u Device Number.
     *
     * \return true if test case is selected on this device, otherwise false.
     */
    int (*select)(int u);

    /*!
     * \brief parse arguments from command line inputs.
     *
     * This function is for parsing arguments from command line inputs.
     *
     * \param [in] u Device Number.
     * \param [in] cli CLI object.
     * \param [in] a CLI arguments.
     * \param [in] flags Test Engine Flags.
     * \param [out] bp Buffer pointer for case-specific parameters.
     *
     * \retval SHR_E_NONE Parse the arguments successfully.
     * \retval SHR_E_MEMORY Memory allocated fail.
     * \retval SHR_E_PARAM Invalid parameters.
     */
    int (*parser)(int u, bcma_cli_t *cli, bcma_cli_args_t *a,
                  uint32_t flags, void **bp);

    /*!
     * \brief Print help information for specific test case.
     *
     * This function is for printing help information for specific test case.
     *
     * \param [in] u Device Number.
     * \param [in] bp Buffer pointer for case-specific parameters.
 */
    void (*help)(int u, void *bp);

    /*!
     * \brief Recycle the resource allocated during parsing.
     *
     * This function is for recycling the allocated memory resource.
     *
     * \param [in] u Device Number.
     * \param [in] bp Buffer pointer for case-specific parameters.
 */
    void (*recycle)(int u, void *bp);

    /*! Procedure array needed for this test case */
    bcma_test_proc_t *procs;

    /*! Procedure count */
    int proc_count;

} bcma_test_op_t;

/*!
 * \brief Get operation routines function.
 *
 * The function is for getting case-specific operation
 * routines for a test case.
 *
 * \retval operation routines pointer.
 */
typedef bcma_test_op_t *(*bcma_test_op_get_f)(void);

/*!
 * \brief Structure for test case.
 */
typedef struct bcma_test_case_s {

    /*! Test Group Name */
    const char *group;

    /*! Test identifier */
    int id;

    /*! Test # name */
    const char *name;

    /*! Description */
    const char *desc;

    /*! Test flags */
    uint32_t flags;
    /*! Force to execute rc before running test case */
#define BCMA_TEST_F_RC               (0x1 << 0)
    /*! Test case is destructive */
#define BCMA_TEST_F_DESTRUCTIVE      (0x1 << 1)
    /*! Test case needs to be executed while detached */
#define BCMA_TEST_F_DETACHED         (0x1 << 2)

    /*! Default iterations */
    int loops;

    /*! Default arguments */
    const char *dflt_args;

    /*! Get operation routines function. */
    bcma_test_op_get_f op_get;

    /*! Operation routines */
    bcma_test_op_t *op;

    /*! Test feature condition */
    int feature;

} bcma_test_case_t;

#endif /* BCMA_TESTCASE_H */
