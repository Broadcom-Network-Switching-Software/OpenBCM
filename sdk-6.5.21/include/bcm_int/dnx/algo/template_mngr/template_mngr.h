/** \file template_mngr.h
 *
 * Internal DNX template manager APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNX_ALGO_TEMPLATE_MNGR_INCLUDED
/*
 * {
 */
#define DNX_ALGO_TEMPLATE_MNGR_INCLUDED

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/**
 * \brief
 * Flags for dnx_algo_template_create_data_t struct
 *
 * \see
 * dnx_algo_template_create
 */
/**
 * This template uses an advanced algorithm.
 */
#define DNX_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM SAL_BIT(0)
/**
 * This template uses a default profile to which all entries are allocated on creation.
 * This profile will then never be freed, unless the flag
 * \ref DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE is also set at creation.
 */
#define DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE    SAL_BIT(1)
/**
 *  Set this flag to allow freeing the default profile. Default profile will never be freed if this flag is not set.
 */
#define DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE    SAL_BIT(2)

/**
 * \brief
 * Flags for dnx_algo_template_allocate and dnx_algo_template_exchange
 *
 * \see
 * dnx_algo_template_allocate
 * dnx_algo_template_exchange
 */
/**
 * Allocate with a given ID.
 */
#define DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID                          SAL_BIT(0)
/**
 * On exchange operation (see below), if the old template does not exist then an error message will not be returned.
 * \see dnx_algo_template_exchange
 */
#define DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE    SAL_BIT(1)
/**
 *  Run exchange operation to see if it works but don't change sw state.
 *  \see dnx_algo_template_exchange
 */
#define DNX_ALGO_TEMPLATE_EXCHANGE_TEST                             SAL_BIT(2)
/*
 * On exchange operation, ignore input data.
 *   This is used for setting the profile with the WITH_ID flag, without reading the data first.
 *   Therefore, it must be used WITH_ID.
 * \see dnx_algo_template_exchange
 */
#define DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA                      SAL_BIT(3)

/**
 * This define will be return from dnx_algo_template_get_next if we can't provide the next profile.
 * \see dnx_algo_template_get_next.
 */
#define DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE (-1)

/**
 * Maximum length of a template's name
 */
#define DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH 100

/**
 * \brief Template creation information
 *
 * This structure contains the information required for creating a new template.
 *
 *  \see
 * dnx_algo_template_create
 */
typedef struct
{
    /*
     *
     * DNX_ALGO_TEMPLATE_CREATE_* flags
     */
    uint32 flags;
    /*
     *
     * First profile id of the template.
     */
    int first_profile;
    /*
     *
     * How many profiles are in the template.
     */
    int nof_profiles;
    /*
     *
     * Maximum number of pointers to each profile.
     */
    int max_references;
    /*
     *
     * Default profile for the template. To be used if flag
     * DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set.
     */
    int default_profile;
    /*
     *
     * Size of the template's data.
     */
    int data_size;
    /*
     *
     * If flag DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set, put here the data that it will contain.
     */
    void *default_data;
    /*
     * Enum of the relevant advanced algorithm. If we did not have any we should put DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_template_advanced_algorithms_e advanced_algorithm;
    /*
     * Template name.
     */
    char name[DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH];
} dnx_algo_template_create_data_t;

#endif /* DNX_ALGO_TEMPLATE_MNGR_INCLUDED */
