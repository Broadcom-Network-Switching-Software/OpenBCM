/** \file multithread_analyzer.h
 * Supply analyzing utils that can list the resources that are 
 * shared between different threads and the APIs that are using 
 * these resources. 
 *  
 * MTA stands for Multi Thread Analyzer 
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_MULTITHREAD_ANALYZER_H
/* { */
#define _DNXC_MULTITHREAD_ANALYZER_H

#include <sal/types.h>


#if defined(DNXC_MTA_ENABLED)
#define DNXC_MTA(x) x
#else
#define DNXC_MTA(x)
#endif

#ifdef DNXC_MTA_ENABLED

#define MTA_NO_FLAGS                  0
#define MTA_FLAG_DISPATCHER           SAL_BIT(0)
#define MTA_FLAG_SHR_INIT_FUNC        SAL_BIT(1)
#define MTA_FLAG_PRINT_TO_SCREEN      SAL_BIT(2)
#define MTA_FLAG_CINT                 SAL_BIT(3)
#define MTA_FLAG_MUTEX                SAL_BIT(4)
#define MTA_FLAG_LOG_ONLY_MAIN        SAL_BIT(5)
#define MTA_FLAG_LOG_ONLY_BACKGROUND  SAL_BIT(6)
#define MTA_FLAG_DONT_LOG_MEM_REG     SAL_BIT(7)
#define MTA_FLAG_THREAD_MAIN_FUNC     SAL_BIT(8)
#define MTA_FLAG_DONT_LOG_ALLOC_MNGR  SAL_BIT(9)
#define MTA_FLAG_LOG_ONLY_ALLOC_MNGR  SAL_BIT(10)

#define MTA_TCL_FLAG_SET              MTA_FLAG_LOG_ONLY_MAIN | MTA_FLAG_DONT_LOG_MEM_REG | MTA_FLAG_LOG_ONLY_ALLOC_MNGR

/**
 * An enum that represent the resource type of an access entry
 */
typedef enum
{
    MTA_RESOURCE_INVALID = 0,
    MTA_RESOURCE_SW_STATE,
    MTA_RESOURCE_DBAL,
    MTA_RESOURCE_MEM,
    MTA_RESOURCE_REG,
    MTA_RESOURCE_WB_ENGINE,
    MTA_RESOURCE_SOC_CONTROL,
    MTA_RESOURCE_SAT,
    MTA_RESOURCE_LINKSCAN,
    MTA_RESOURCE_REG_OUTSIDE_DBAL,
    MTA_RESOURCE_MEM_OUTSIDE_DBAL,
    MTA_RESOURCE_RES_MGR,
    MTA_RESOURCE_TMP_MGR,
    /**
     * should always be last
     */
    MTA_RESOURCE_NOF
} mta_resource_e;

/**
 * An enum that represent the tipe of element in play
 */
typedef enum
{
    MTA_ELEMENT_UNKNOWN = 0,
    MTA_ELEMENT_API,
    MTA_ELEMENT_MUTEX,
    MTA_ELEMENT_CINT,
    MTA_ELEMENT_NOF
} mta_element_e;

/**
 * An enum that represent dynamic sub resource type
 */
typedef enum
{
    MTA_DYN_SUB_RES_RES_MGR = 0,
    MTA_DYN_SUB_RES_TMP_MGR,
    MTA_DYN_SUB_RES_NOF
} mta_dynamic_res_id_type_e;

const char *dnxc_multithread_analyzer_resource_type_name_get(
    mta_resource_e resource_type);

const char *dnxc_multithread_analyzer_thread_name_get(
    int unit,
    uint8 thread_id);

const char *dnxc_multithread_analyzer_resource_id_name_get(
    int unit,
    mta_resource_e resource_type,
    uint32 resource_id);

const char *dnxc_multithread_analyzer_mutex_list_get(
    const char **mutex_name);

/**
 * \brief - get notified by the application that init is done so 
 *        we won't log access during application init
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] is_done - TRUE/FALSE
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_application_init_is_done(
    int unit,
    uint32 is_done);

/**
 * \brief - init the analyzer and start logging
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] flags - flag ideas: ignore_init_deinit
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks
 *   * init the api_in_play db and its mutex
 *   * init the resource-thread_id-api mapping db (it can be a
 *     hash table, a matrix, an sql db or whatever)
 *   * since the analyzer should stay alive even during an
 *     init_deinit cycle, this function should not be called
 *     during init, it should be called on demand by the
 *     regression session.
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_start(
    int unit,
    uint32 flags);

/**
 * \brief - to be ran after analysys in order to generate a 
 *        summary output file.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] flags - determine the format of the output.
 *   \param [in] file_name - output file's name
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks
 *   * The output will be raw-data txt file that should be later
 *     parsed by a higher level script to extract meanningful
 *     info or diagrams.
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_generate_summary_files(
    int unit,
    uint32 flags,
    char *file_name);

/**
 * \brief - this function is used to declare the API that is 
 *        currently in play, it should be called at the
 *        beginning of an API with in_play=TRUE and at the end
 *        of an API with in_play=FALSE, it is automatically
 *        called on BCM APIs by dispatcher but need to be
 *        manually called for "internal APIs"
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] api_name -  A string representing the api name
 *          (typically __FUNCTIOM__)
 *   \param [in] flags - Flags to declare properties on the log
 *          entry
 *   \param [in] in_play - should be called with TRUE when
 *          entering API and with False when exiting API
 * \par INDIRECT INPUT: 
 *   * thread_id - the api_name is stored together with the
 *     thread id
 *   * api_in_play db and mutex - a global DS to hold the
 *     current list of APIs that are in play in which threads
 * \par DIRECT_OUTPUT:
 *   shr_error_e 
 * \remarks 
 *   * there may be multiple APIs in play for a single thread at
 *     any given time, to account for API-in-API scenario.
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_declare_api_in_play(
    int unit,
    const char *api_name,
    uint32 flags,
    uint8 in_play);

/**
 * \brief - called by resources upon access in order to log the 
 *        access in the analyzer
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] resource_type - e.g. DBAL, SW-State
 *   \param [in] resource_id - a specific identifier for the
 *   resource, every resource_type has its own set of
 *   resource_ids (e.g. DBAL table id)
 *   \param [in] is_write - TRUE for write, FALSE for read.
 * \par INDIRECT INPUT: 
 *   * thread_id
 *   * api_in_play db and mutex
 *   * is_init - it can ignore resource access during
 *     init/deinit.
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks 
 *   * None
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_log_resource_use(
    int unit,
    mta_resource_e resource_type,
    uint32 resource_id,
    uint8 is_write);

/**
 * \brief - called by dbal to inform the multi thread analyzer 
 *        that we are inside a dbal transaction. this
 *        information is used to detect MEM/REG access outside
 *        of the dbal layer
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] is_start - TRUE for start FALSE for end
 * \par INDIRECT INPUT: 
 *   * thread_id
 *   * api_in_play db and mutex
 *   * is_init - it can ignore resource access during
 *     init/deinit.
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remarks 
 *   * None
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_mark_dbal_region(
    int unit,
    int is_start);

/**
 * \brief - called by res and template manager to create a 
 *        resource id from the resource name
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] resource - the type of the resource i.e. res or
 *          template
 *   \param [in] res_name - a pointer to a string with the
 *          resource name
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT:
 *   dynamic id 
 * \remarks 
 *   * None
 * \see
 *   * None
 */
int dnxc_multithread_analyzer_dynamic_sub_res_get(
    int unit,
    mta_dynamic_res_id_type_e resource,
    char *res_name);

#endif /* DNXC_MTA_ENABLED */
#endif /* _DNXC_MULTITHREAD_ANALYZER_H */
