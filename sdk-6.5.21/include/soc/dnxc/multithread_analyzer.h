
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_MULTITHREAD_ANALYZER_H

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

    MTA_RESOURCE_NOF
} mta_resource_e;

typedef enum
{
    MTA_ELEMENT_UNKNOWN = 0,
    MTA_ELEMENT_API,
    MTA_ELEMENT_MUTEX,
    MTA_ELEMENT_CINT,
    MTA_ELEMENT_NOF
} mta_element_e;

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

int dnxc_multithread_analyzer_application_init_is_done(
    int unit,
    uint32 is_done);

int dnxc_multithread_analyzer_start(
    int unit,
    uint32 flags);

int dnxc_multithread_analyzer_generate_summary_files(
    int unit,
    uint32 flags,
    char *file_name);

int dnxc_multithread_analyzer_declare_api_in_play(
    int unit,
    const char *api_name,
    uint32 flags,
    uint8 in_play);

int dnxc_multithread_analyzer_log_resource_use(
    int unit,
    mta_resource_e resource_type,
    uint32 resource_id,
    uint8 is_write);

int dnxc_multithread_analyzer_mark_dbal_region(
    int unit,
    int is_start);

int dnxc_multithread_analyzer_dynamic_sub_res_get(
    int unit,
    mta_dynamic_res_id_type_e resource,
    char *res_name);

#endif
#endif
