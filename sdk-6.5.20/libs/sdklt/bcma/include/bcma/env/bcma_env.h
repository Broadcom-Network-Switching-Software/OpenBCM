/*! \file bcma_env.h
 *
 * CLI Environment
 *
 * Interface for managing environment variables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_ENV_H
#define BCMA_ENV_H

#include <sal/sal_types.h>

/*!
 * Return value from \ref bcma_env_var_traverse call-back function
 * to stop traversing.
 */
#define BCMA_ENV_VAR_TRAVERSE_STOP   1

/*!
 * Return value from \ref bcma_env_var_traverse call-back function
 * to delete variable.
 */
#define BCMA_ENV_VAR_TRAVERSE_DELETE 2

/*! Opaque environment handle. */
typedef void *bcma_env_handle_t;

/*!
 * \brief Allocate and initialize environment object.
 *
 * The env module supports hash table to speed up searches for variables.
 * The hash table size is (1 << 'hash_order'). No hashing is applied
 * if 'hash_order' is 0. Users can choose the hash order arbitrarily
 * depending on their applications. The larger the hash table size is,
 * the faster the search result might be retrieved. On the other hand,
 * the larger the hash table size is, the more memory spaces would be
 * consumed for hash table allocation.
 *
 * \param [in] hash_order Order of hash table size
 *
 * \return Opaque handle to environment object or NULL if error.
 */
extern bcma_env_handle_t
bcma_env_create(uint32_t hash_order);

/*!
 * \brief Free previously allocated environment object.
 *
 * This function will also free all resources associated with the
 * environment object.
 *
 * \param [in] eh Environment handle.
 *
 * \retval 0 No errors.
 * \retval -1 Not dynamically allocated.
 */
extern int
bcma_env_destroy(bcma_env_handle_t eh);

/*!
 * \brief Initialize environment object.
 *
 * \param [in] eh Environment handle.
 * \param [in] hash_order Order of hash table size.
 *
 * \retval 0 No errors.
 * \retval -1 Bad environment or memory allocation error.
 */
extern int
bcma_env_init(bcma_env_handle_t eh, uint32_t hash_order);

/*!
 * \brief Free resources of environment object.
 *
 * \param [in] eh Environment handle.
 *
 * \retval 0 No errors.
 * \retval -1 Bad environment.
 */
extern int
bcma_env_cleanup(bcma_env_handle_t eh);

/*!
 * \brief Set an environment variable.
 *
 * Variables set in the local scope are hidden when the scope is
 * pushed, and they will be deleted when the scope is popped. Global
 * variables are visible in all scopes.
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] value Value of variable.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 Variable was set successfully.
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_set(bcma_env_handle_t eh, const char *name, const char *value,
                 int local);

/*!
 * \brief Set decimal environment variable.
 *
 * Convenience function to store integer as a decimal string. For
 * example, the value 12 will be stored as the string "12".
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] value Value of variable (as raw integer).
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 Variable was set successfully.
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_integer_set(bcma_env_handle_t eh, const char *name, int value,
                         int local);

/*!
 * \brief Set hexadecimal environment variable.
 *
 * Convenience function to store integer as a hexadecimal string. For
 * example, the value 12 will be stored as the string "0xc".
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] value Value of variable (as raw integer).
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 Variable was set successfully.
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_hex_set(bcma_env_handle_t eh, const char *name, int value,
                     int local);

/*!
 * \brief Unset an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] local Check against variables in local scope (TRUE/FALSE).
 * \param [in] global Check against global variables (TRUE/FALSE).
 *
 * \retval 0 Variable was unset successfully.
 * \retval -1 Bad environment or variable not found..
 */
extern int
bcma_env_var_unset(bcma_env_handle_t eh, const char *name,
                   int local, int global);

/*!
 * \brief Get value of an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 *
 * \return Name of variable or NULL if the variable is not found.
 */
extern const char *
bcma_env_var_get(bcma_env_handle_t eh, const char *name);

/*!
 * \brief Check if an environment variable exists.
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] local Check against variables in local scope (TRUE/FALSE).
 * \param [in] global Check against global variables (TRUE/FALSE).
 *
 * \retval TRUE Variable exists.
 * \retval FALSE Variable does not exist.
 */
extern int
bcma_env_var_exists(bcma_env_handle_t eh, const char *name,
                    int local, int global);

/*!
 * \brief Traverse all scope variables.
 *
 * The function traverses all variables within the specified \c scope.
 * Value 0 of \c scope means to traverse global variables.
 * Value 1 of \c scope means to traverse current local variables.
 * Next local scope would be indicated by \c scope + 1 starting from 1.
 *
 * The function would call call-back function \c cb for all variables.
 * The traverse function would pass variable name, value, tag pointer
 * to call-back function \c cb.
 *
 * The call-back function \c cb can control the traverse function by return
 * value. If \ref BCMA_ENV_VAR_TRAVERSE_STOP is returned from \c cb,
 * \ref bcma_env_var_traverse will be stopped immediately.
 * If \ref BCMA_ENV_VAR_TRAVERSE_DELETE is returned from \c cb,
 * current traversing variable will be deleted then.
 * Negative value should be returned from call-back function \c cb on error
 * and this would also stop traversing immediately. Otherwise 0 should be
 * returned from call-back function \c cb.
 *
 * \param [in] eh Environment handle.
 * \param [in] scope Scope value to indicate the scope of variables to traverse.
 * \param [in] cb Traverse call-back function.
 * \param [in] cookie Information to be passed to call-back function.
 *
 * \retval Last return value from call-back function \c cb.
 */
extern int
bcma_env_var_traverse(bcma_env_handle_t eh, int scope,
                      int (*cb)(void *cookie, const char *name,
                                const char *value, uint32_t *tag),
                      void *cookie);

/*!
 * \brief Set an environment variable tag.
 *
 * A tag value \c tag can be set to environment variable \c name to classify
 * the variables in the specified scope. Default variable tag value is 0
 * if tag value is not set through this function.
 *
 * \param [in] eh Environment handle.
 * \param [in] name Name of variable.
 * \param [in] tag Tag of variable.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 Variable tag was set successfully.
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_tag_set(bcma_env_handle_t eh, const char *name, uint32_t tag,
                     int local);

/*!
 * \brief Get tag of an environment variable.
 *
 * Variables in the local scope will be searched first, and if not
 * found here, the list of global variables will be searched.
 *
 * Variable tag will be updated in content of pointer \c tag.
 *
 * \param [in] eh Environment handle
 * \param [in] name Name of variable
 * \param [out] tag Pointer of tag
 *
 * \retval 0 Variable tag was get successfully
 * \retval -1 Bad environment or variable not found.
 */
extern int
bcma_env_var_tag_get(bcma_env_handle_t eh, const char *name, uint32_t *tag);

/*!
 * \brief Push scope for local environment variables.
 *
 * Global environment variables are unaffected by a change of scope.
 * Current local variables are hidden when the scope is pushed.
 *
 * \param [in] eh Environment handle.
 *
 * \retval 0 No errors.
 * \retval -1 Bad environment or memory allocation error.
 */
extern int
bcma_env_scope_push(bcma_env_handle_t eh);

/*!
 * \brief Pop scope for local environment variables.
 *
 * Global environment variables are unaffected by a change of scope.
 * Current local variables are deleted when the scope is popped.
 *
 * \param [in] eh Environment handle.
 *
 * \retval 0 No errors.
 * \retval -1 Bad environment or no more scopes.
 */
extern int
bcma_env_scope_pop(bcma_env_handle_t eh);

/*!
 * \brief Number of pushed scopes.
 *
 * \param [in] eh Environment handle.
 *
 * \retval Number of pushed scopes.
 * \retval -1 Bad environment.
 */
extern int
bcma_env_num_scopes_get(bcma_env_handle_t eh);

#endif /* BCMA_ENV_H */
