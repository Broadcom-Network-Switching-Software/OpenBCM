/*! \file cfgdb_internal.h
 *
 * CLI Configuration data manager
 *
 * Configuration data manager definitions intended for internal use only.
 *
 * Application code should normally not need to include this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CFGDB_INT_H
#define BCMA_CFGDB_INT_H

#include <sal/sal_internal.h>

#include <bcma/env/bcma_env.h>

/*! Signature for sanity checks. */
#define CFGDB_SIGNATURE   SAL_SIG_DEF('C', 'F', 'G', '>')

/*! Check that this is a valid configuration data manager object. */
#define BAD_CFGDB(_cfgdb)   ((_cfgdb)->cfgdb_sig != CFGDB_SIGNATURE)

/*!
 * \brief Configuration data manager object.
 *
 * All data for the configuration data manager is containe within this object.
 *
 * The signatures in the structure are used for sanity checks.
 */
typedef struct cfgdb_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int cfgdb_sig;

    /*! Environment handle */
    bcma_env_handle_t env;

    /*! File name to load variables from */
    char *fname;

    /*! Temporary file name to write variables to */
    char *tmp_fname;

} cfgdb_t;

/*!
 * \brief Cookie for cfgdb_var_traverse call-back fucntion.
 *
 * Cookie for cfgdb_var_traverse call-back function var_tag_flag_clear
 * and var_tag_flag_set.
 */
struct trvs_tag_flag_op {

    /*! Variable name */
    const char *name;

    /*! Flags value to be clear/set in variable tag */
    int flags;

    /*! Flag to indicate if the flags value has been updated */
    int done;
};

/*!
 * \brief Get Configuration data manager instance.
 *
 * \retval Pointer to configuration data manager instance.
 */
extern cfgdb_t *
bcma_cfgdb_get(void);

/*!
 * \brief Wildcard string match.
 *
 * For internal use only.
 *
 * Function for comparing wildcard string 'wc_str' with wildcard character '*'
 * and target string 'str'.
 * Return value is TRUE if string 'str' matches the wildcard string 'wc_str'.
 * Otherwise return value is FALSE.
 * For example:
 * If wildcard string is "*mem*", it would match "memory" or "socmem".
 * If wildcard string is "mem*", it would only match "memory".
 * If wild card string is "*mem", it would only match "socmem".
 *
 * \param[in] str Target string to be compared
 * \param[in] wc_str String with wildcard character '*' to compare
 *
 * \retval TRUE Strings are matching
 * \retval FALSE Strings don't match
 */
extern int
bcma_cfgdb_wildcard_match(const char *str, const char *wc_str);

/*!
 * \brief Call-back function of bcma_cfgdb_var_traverse.
 *
 * For internal use only.
 *
 * Call-back function of cfgdb_var_traverse to clear flags in variable tag.
 * Pointer of struct trvs_tag_flag_op is the cookie of the call-back function.
 * If name in cookie is NULL, all variables flags will be cleared by mask off
 * flags in cookie. Otherwise only flag of variable name matches the name in
 * cookie will be update.
 *
 * \param[in] cookie Cookie for traverse function
 * \param[in] name Variable name
 * \param[in] value Variable value
 * \param[in] tag Variable tag pointer
 *
 * \retval 0 Continue traversing
 * \retval BCMA_CFGDB_VAR_TRAVERSE_STOP Stop traversing
 */
extern int
bcma_cfgdb_var_tag_flags_clear(void *cookie, const char *name,
                               const char *value, uint32_t *tag);

/*!
 * \brief Call-back function of bcma_cfgdb_var_traverse.
 *
 * For internal use only.
 *
 * Call-back function of cfgdb_var_traverse to set flags in variable tag.
 * Pointer of struct trvs_tag_flag_op is the cookie of the call-back function.
 * If name in cookie is NULL, all variables flags will be set by ORed with
 * flags in cookie. Otherwise only flag of variable name matches the name in
 * cookie will be set.
 *
 * \param[in] cookie Cookie for traverse function
 * \param[in] name Variable name
 * \param[in] value Variable value
 * \param[in] tag Variable tag pointer
 *
 * \retval 0 Continue traversing
 * \retval BCMA_CFGDB_VAR_TRAVERSE_STOP Stop traversing
 */
extern int
bcma_cfgdb_var_tag_flags_set(void *cookie, const char *name, const char *value,
                             uint32_t *tag);

#endif /* BCMA_CFGDB_INT_H */
