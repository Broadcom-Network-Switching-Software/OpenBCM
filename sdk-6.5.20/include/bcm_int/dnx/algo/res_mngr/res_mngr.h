/** \file res_mngr.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SW_STATE_RES_MNGR_H__
#define __SW_STATE_RES_MNGR_H__


#include <include/bcm_int/dnx/algo/res_mngr/res_mngr_types.h>

/*
 * DEFINES
 */
/*
 * Resource name max lenght.
 */
#define DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH 100

/**
 * This resource uses an advanced algorithm.
 */
#define DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM  SAL_BIT(0)
/**
 * This define will be return from dnx_algo_res_get_next if we can't provide the next element.
 * \see dnx_algo_res_get_next
 */
#define DNX_ALGO_RES_ILLEGAL_ELEMENT (-1)
/**
 * Flags for dnx_algo_res_allocate
 *
 * \see
 * dnx_algo_res_allocate
 */
/**
 * Allocate with a given ID.
 */
#define DNX_ALGO_RES_ALLOCATE_WITH_ID       SAL_BIT(0)
/**
 * Use this flag to verify before allocation if the allocation 
 * will succeed. when usd the manager will return Error code as 
 * if it allocated the resource but will not perform any 
 * allocation in prctice. 
 */
#define DNX_ALGO_RES_ALLOCATE_SIMULATION    SAL_BIT(1)
/**
 * This range of flags is reserved for advanced algorithms. Each
 * advanced algorithm can add his flags in this range. 
 */
#define DNX_ALGO_RES_ADVANCED_ALGOIRTHM_FLAGS  UTILEX_BITS_MASK(25,31)

/*
 * STRUCTs
 */

/**
 * \brief Resource creation information
 *
 * This structure contains the information required for creating a new resource.
 */
typedef struct
{
    /** 
     *  DNX_ALGO_RES_INIT_* flags
     */
    uint32 flags;
    /**
     *  First element of the resource
     */
    int first_element;
    /**
     * How many elements are in the resource.
     */
    int nof_elements;
    /**
     * Enum of the relevant advanced algorithm. If we did not have any we should put SW_STATE_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_resource_advanced_algorithms_e advanced_algorithm;
    /**
     * Resource name
     */
    char name[DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH];

} dnx_algo_res_create_data_t;

/**
 * \brief Resource information
 *
 * This structure contains the information that will be displayed with the swstate dump command.
 */
typedef struct
{
    /*
     *This structure contains the information required for creating a new resource.
     */
    dnx_algo_res_create_data_t create_data;
    /*
     * *
     * *How many elements are used in the resource.
     */
    int nof_used_elements;
    /*
     * Enum of the relevant advanced algorithm. If we did not have any we should put DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_resource_advanced_algorithms_e advanced_algorithm;
    /*
     * algo_instance_id.
     */
    int algo_instance_id;
} dnx_algo_res_dump_data_t;

#endif /* __SW_STATE_RES_MNGR_H__ */
