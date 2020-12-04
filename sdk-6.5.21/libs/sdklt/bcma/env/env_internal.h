/*! \file env_internal.h
 *
 * CLI Environment
 *
 * Environment definitions intended for internal use only.
 *
 * Application code should normally not need to include this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ENV_INTERNAL_H
#define ENV_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_internal.h>
#include <sal/sal_spinlock.h>

/*! Signature for sanity checks. */
#define ENV_SIGNATURE   SAL_SIG_DEF('E', 'N', 'V', '>')

/*! Check that this is a valid environment object. */
#define BAD_ENV(_env)   ((_env)->env_sig != ENV_SIGNATURE)

/*! Check that this environment object was dynamically allocated. */
#define BAD_OBJ(_env)   ((_env)->dyn_sig != ENV_SIGNATURE)

/*!
 * \brief Environment variable.
 *
 * Simple double-linked list of variables consisting of a name and
 * value pair, as well as tag information.
 */
typedef struct env_var_s {

    /*! Next variable in linked list. */
    struct env_var_s *next;

    /*! Previous variable in linked list. */
    struct env_var_s *prev;

    /*! Variable name. */
    char *name;

    /*! Variable value. */
    char *value;

    /*! Variable tag mainly for classification (application-specific). */
    uint32_t tag;

} env_var_t;

/*!
 * \brief Environment variable list.
 *
 * Hash table is introduced to speed up the search function
 * in variable list.
 */
typedef struct env_var_list_s {

    /*! Hash table for variable list. */
    env_var_t *htab;

    /*! Hash table size order. */
    uint32_t hash_order;

} env_var_list_t;

/*!
 * \brief Environment scope.
 *
 * The environment support multiple local variable scopes.  Local
 * variables are only visible within the scope where they are defined,
 * whereas global variables are visible everywhere.
 *
 * Typically a new local variable scope is created whenever a new
 * shell script is loaded, as this allows each script to use the same
 * local variable names without side-effects, when one script launches
 * another script.
 */
typedef struct env_var_scope_s {

    /*! Next scope in list. */
    struct env_var_scope_s *next;

    /*! Local variable list for this scope. */
    env_var_list_t local;

} env_var_scope_t;

/*!
 * \brief Environment object.
 *
 * All data for the environment is contained within this object.
 *
 * The signatures in the structure are used for sanity checks.
 */
typedef struct env_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int env_sig;

    /*! Signature for dynamically allocated objects. */
    unsigned int dyn_sig;

    /*! Root of local variable scopes. */
    env_var_scope_t *var_scope_list;

    /*! Lock for list operations. */
    sal_spinlock_t lock;

    /*! Global variable list. */
    env_var_list_t var_global_list;

    /*! Hash table size order. */
    uint32_t hash_order;

} env_t;

/*!
 * \brief Initialize variable list.
 *
 * For internal use only.
 *
 * \param[in] vl Variable list for initialization.
 * \param[in] hash_order Hash table size order for initialization.
 *
 * \retval 0 No errors.
 * \retval -1 resource allocation error.
 */
extern int
bcma_env_var_list_init(env_var_list_t *vl, uint32_t hash_order);

/*!
 * \brief Free resources of variable list.
 *
 * For internal use only.
 *
 * \param[in] vl Variable list to free.
 *
 * \retval 0 No errors.
 * \retval -1 No resource to be freed in variable list.
 */
extern int
bcma_env_var_list_cleanup(env_var_list_t *vl);

#endif /* ENV_INTERNAL_H */
