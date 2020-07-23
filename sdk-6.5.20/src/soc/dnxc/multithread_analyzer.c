
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mcm/memregs.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/swstate/auto_generated/access/multithread_analyzer_access.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/multithread_analyzer_diagnostic.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef DNXC_MTA_ENABLED


#define MTA_MAX_NOF_THREADS 15
#define MTA_MAX_NOF_ELEMENTS_IN_PLAY 100
#define MTA_MAX_THREAD_NAME_LENGTH 128
#define MTA_MAX_RESOURCE_NAME_LENGTH 128
#define MTA_MAX_API_NAME_LENGTH 128
#define MTA_MAX_CINT_NAME_LENGTH 96
#define MTA_MAX_LOGGED_CINTS 1024
#define MTA_MAX_DYNAMIC_RES_IDS 128
#define MTA_TIMEOUT -1

typedef struct
{
    mta_element_e type;
    const char *name;
} mta_element_in_play_t;


typedef struct
{
    char name[MTA_MAX_THREAD_NAME_LENGTH];
    uint32 nof_htb_insertions;
    uint32 tid;
    uint8 is_main;
    uint32 dbal_region_counter;
    uint8 tmp_disabled;         
    mta_element_in_play_t element_in_play[MTA_MAX_NOF_ELEMENTS_IN_PLAY];
    uint32 mta_nof_apis_in_play;
} mta_thread_info_t;

typedef struct
{
    uint8 mta_is_active;
    uint8 is_application_init_done;
    uint8 log_for_main;
    uint8 log_for_background;
    uint8 log_for_memreg;
    uint8 log_for_aloc_mngr;
    uint8 log_only_aloc_mngr;
    sal_mutex_t lock;
    uint32 nof_registered_threads;
    mta_thread_info_t thread[MTA_MAX_NOF_THREADS];
    char cints[MTA_MAX_LOGGED_CINTS][MTA_MAX_CINT_NAME_LENGTH];
    int nof_cints;
    char *dynamic_resource_ids[MTA_DYN_SUB_RES_NOF][MTA_MAX_DYNAMIC_RES_IDS];
    int nof_dynamic_resources[MTA_DYN_SUB_RES_NOF];
} mta_db_t;

mta_db_t mta_db[SOC_MAX_NUM_DEVICES] = { {0} };



static const char *
dnxc_multithread_analyzer_element_type_string_get(
    mta_element_e type)
{
    switch (type)
    {
        case MTA_ELEMENT_API:
            return "API";
        case MTA_ELEMENT_MUTEX:
            return "Mutex";
        case MTA_ELEMENT_CINT:
            return "CINT";
        default:
            return "Unknown";
    }
}

static void
dnxc_multithread_analyzer_thread_dump(
    int unit,
    mta_thread_info_t thread)
{

    int i;

    cli_out("----\n");
    cli_out("name = %s \n", thread.name);
    cli_out("nof_htb_insertions = %d \n", thread.nof_htb_insertions);
    cli_out("tid = %d \n", thread.tid);
    cli_out("is_main = %d \n", thread.is_main);
    cli_out("dbal_region_counter = %d \n", thread.dbal_region_counter);
    cli_out("tmp_disabled = %d \n", thread.tmp_disabled);
    cli_out("mta_nof_apis_in_play = %d \n", thread.mta_nof_apis_in_play);

    cli_out("elements in play:\n");
    for (i = 0; i < thread.mta_nof_apis_in_play; i++)
    {
        cli_out(" - %s (%s)\n",
                thread.element_in_play[i].name,
                dnxc_multithread_analyzer_element_type_string_get(thread.element_in_play[i].type));
    }
    return;
}

static void
dnxc_multithread_analyzer_mta_db_dump(
    int unit)
{
    dnx_sw_state_dump_filters_t dump_filter;
    int i, j;

    cli_out(" ~~~ dumping mta db ~~~ \n");

    cli_out("mta_is_active = %d \n", mta_db[unit].mta_is_active);
    cli_out("is_application_init_done = %d \n", mta_db[unit].is_application_init_done);
    cli_out("log_for_main = %d \n", mta_db[unit].log_for_main);
    cli_out("log_for_background = %d \n", mta_db[unit].log_for_background);
    cli_out("log_for_memreg = %d \n", mta_db[unit].log_for_memreg);
    cli_out("nof_registered_threads = %d \n", mta_db[unit].nof_registered_threads);
    cli_out("nof_cints = %d \n", mta_db[unit].nof_cints);

    cli_out("\ndynamic resources names:\n");
    for (i = 0; i < MTA_DYN_SUB_RES_NOF; i++)
    {
        for (j = 0; j < mta_db[unit].nof_dynamic_resources[i]; j++)
        {
            cli_out(" - %s(%d) \n", mta_db[unit].dynamic_resource_ids[i][j], i);
        }
    }

    cli_out("\n");

    for (i = 0; i < mta_db[unit].nof_registered_threads; i++)
    {
        dnxc_multithread_analyzer_thread_dump(unit, mta_db[unit].thread[i]);
    }

    cli_out("\nfollowing is the list of resource used as saved in the hash table:\n");
    dump_filter.namefilter = "";
    dump_filter.typefilter = "";
    dump_filter.nocontent = 0;
    multithread_analyzer_htb_dump(unit, dump_filter);

    cli_out(" ~~~~~~~~~~~~~~~~~~~~~~ \n");

    return;
}


static int
dnxc_multithread_analyzer_is_active(
    int unit,
    uint32 flags)
{
    
    if (flags & MTA_FLAG_THREAD_MAIN_FUNC)
    {
        return TRUE;
    }

    if (!mta_db[unit].mta_is_active)
    {
        return FALSE;
    }

    return (mta_db[unit].is_application_init_done);
}


static int
dnxc_multithread_analyzer_is_logging_for_this_thread(
    int unit)
{
    
    if (mta_db[unit].mta_is_active)
    {
        if (sal_thread_main_get() == sal_thread_self())
        {
            if (!mta_db[unit].log_for_main)
            {
                return FALSE;
            }
        }
        else
        {
            if (!mta_db[unit].log_for_background)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

const char *
dnxc_multithread_analyzer_thread_name_get(
    int unit,
    uint8 thread_id)
{
    return mta_db[unit].thread[thread_id].name;
}

const char *
dnxc_multithread_analyzer_resource_type_name_get(
    mta_resource_e resource_type)
{
    switch (resource_type)
    {
        case MTA_RESOURCE_SW_STATE:
            return "SwState";
        case MTA_RESOURCE_DBAL:
            return "Dbal";
        case MTA_RESOURCE_MEM:
            return "Mem";
        case MTA_RESOURCE_REG:
            return "Reg";
        case MTA_RESOURCE_MEM_OUTSIDE_DBAL:
            return "NonDbalMem";
        case MTA_RESOURCE_REG_OUTSIDE_DBAL:
            return "NonDbalReg";
        case MTA_RESOURCE_WB_ENGINE:
            return "WbEngine";
        case MTA_RESOURCE_SOC_CONTROL:
            return "socControl";
        case MTA_RESOURCE_SAT:
            return "Sat";
        case MTA_RESOURCE_LINKSCAN:
            return "Linkscan";
        case MTA_RESOURCE_RES_MGR:
            return "ResManager";
        case MTA_RESOURCE_TMP_MGR:
            return "TemplateManager";
        default:
            return "UnknownResource";
    }
}

const char *
dnxc_multithread_analyzer_mutex_list_get(
    const char **mutex_name)
{
    static char mta_mutex_list_string[128];
    sal_snprintf(mta_mutex_list_string, 128, "{%s%s%s%s%s}",
                 (!mutex_name[0] ? "NONE" : mutex_name[0]),
                 (!mutex_name[1] ? "" : ";"),
                 (!mutex_name[1] ? "" : mutex_name[1]),
                 (!mutex_name[2] ? "" : ";"), (!mutex_name[2] ? "" : mutex_name[2]));
    return mta_mutex_list_string;
}


void
dnxc_multithread_analyzer_cint_name_set(
    char *cint_name,
    const char *cint_string)
{
    char *curr = NULL;
    int i = 0;

    sal_strncpy(cint_name, cint_string, MTA_MAX_CINT_NAME_LENGTH - 1);;

    curr = cint_name;

    
    while (i < MTA_MAX_API_NAME_LENGTH && *curr != 0)
    {
        if (*curr == '(')
        {
            *curr = 0;
            return;
        }
        i++;
        curr++;
    }

    return;
}


const char *
dnxc_multithread_analyzer_resource_id_name_get(
    int unit,
    mta_resource_e resource_type,
    uint32 resource_id)
{
    static char mta_resourse_id_string[128];

    switch (resource_type)
    {
        case MTA_RESOURCE_SW_STATE:
            return dnxc_module_id_e_get_name((dnxc_module_id_e) resource_id);
        case MTA_RESOURCE_MEM:
        case MTA_RESOURCE_MEM_OUTSIDE_DBAL:
            sal_snprintf(mta_resourse_id_string, 128, "%s(%d)", SOC_MEM_NAME(unit, resource_id), resource_id);
            return mta_resourse_id_string;
        case MTA_RESOURCE_REG:
        case MTA_RESOURCE_REG_OUTSIDE_DBAL:
            sal_snprintf(mta_resourse_id_string, 128, "%s(%d)", SOC_REG_NAME(unit, (soc_reg_t) resource_id),
                         resource_id);
            return mta_resourse_id_string;
        case MTA_RESOURCE_WB_ENGINE:
            sal_snprintf(mta_resourse_id_string, 128, "%d", resource_id);
            return mta_resourse_id_string;
#ifdef BCM_DNX_SUPPORT
        case MTA_RESOURCE_DBAL:
            return dbal_logical_table_to_string(unit, (dbal_tables_e) resource_id);
        case MTA_RESOURCE_RES_MGR:
            return mta_db[unit].dynamic_resource_ids[MTA_DYN_SUB_RES_RES_MGR][resource_id];
        case MTA_RESOURCE_TMP_MGR:
            return mta_db[unit].dynamic_resource_ids[MTA_DYN_SUB_RES_TMP_MGR][resource_id];
#endif
        default:
            return "n/a";
    }
}


static int
dnxc_multithread_analyzer_thread_id_get(
    int unit,
    uint32 *result)
{
    int i;
    uint8 mutex_released = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    
    for (i = 0; i < mta_db[unit].nof_registered_threads; i++)
    {
        if (sal_thread_id_get() == mta_db[unit].thread[i].tid)
        {
            *result = i;
            SHR_EXIT();
        }
    }

    
    mutex_released = FALSE;
    
    if (mta_db[unit].mta_is_active)
    {
        sal_mutex_take(mta_db[unit].lock, MTA_TIMEOUT);
    }

    
    for (i = 0; i < mta_db[unit].nof_registered_threads; i++)
    {
        char thread_name[MTA_MAX_THREAD_NAME_LENGTH];
        sal_thread_name(sal_thread_self(), thread_name, MTA_MAX_THREAD_NAME_LENGTH - 1);

        if (sal_strncmp(thread_name, mta_db[unit].thread[i].name, MTA_MAX_API_NAME_LENGTH - 1) == 0)
        {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "MTA: thread id %d for thread %s has changed (used to be %d)\n"),
                      sal_thread_id_get(), thread_name, mta_db[unit].thread[i].tid));

            
            mta_db[unit].thread[i].tid = sal_thread_id_get();
            *result = i;

            mutex_released = TRUE;
            
            if (mta_db[unit].mta_is_active)
            {
                sal_mutex_give(mta_db[unit].lock);
            }
            SHR_EXIT();
        }
    }

    
    assert(mta_db[unit].nof_registered_threads < MTA_MAX_NOF_THREADS);
    *result = mta_db[unit].nof_registered_threads;

    mta_db[unit].thread[*result].tid = sal_thread_id_get();
    mta_db[unit].thread[*result].is_main = (sal_thread_self() == sal_thread_main_get());
    sal_thread_name(sal_thread_self(), mta_db[unit].thread[*result].name, MTA_MAX_THREAD_NAME_LENGTH);

    mta_db[unit].nof_registered_threads++;

    mutex_released = TRUE;
    
    if (mta_db[unit].mta_is_active)
    {
        sal_mutex_give(mta_db[unit].lock);
    }

    SHR_EXIT();
exit:
    assert(mutex_released);
    SHR_FUNC_EXIT;
}


static int
dnxc_multithread_analyzer_organize_thread_info(
    int unit)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < mta_db[unit].nof_registered_threads; i++)
    {
        if (mta_db[unit].thread[i].is_main)
        {
            sal_strncpy(mta_db[unit].thread[i].name, "main", MTA_MAX_THREAD_NAME_LENGTH - 1);
        }
        if (mta_db[unit].thread[i].name[0] == '\0')
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "ERROR: thread %d has no string description\n"), mta_db[unit].thread[i].tid));
            assert(0);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static uint8
dnxc_multithread_analyzer_should_skip_api_declaration(
    int unit,
    const char *api_name,
    uint32 flags,
    uint8 in_play)
{
    
    if (flags & MTA_FLAG_SHR_INIT_FUNC)
    {
        if (sal_strncmp(api_name, "bcm_dnx_", 7) != 0)
        {
            return TRUE;
        }
    }

    
    if (flags & MTA_FLAG_MUTEX)
    {
        if (sal_strncmp(api_name, "mta_db lock", 11) == 0)
        {
            return TRUE;
        }
        else if (sal_strncmp(api_name, "console mutex", 13) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}


int
dnxc_multithread_analyzer_application_init_is_done(
    int unit,
    uint32 is_done)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mta_db[unit].mta_is_active == FALSE)
    {
        SHR_EXIT();
    }

    mta_db[unit].is_application_init_done = is_done;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int
dnxc_multithread_analyzer_start(
    int unit,
    uint32 flags)
{
    sw_state_htbl_init_info_t init_info;
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0x0, sizeof(sw_state_htbl_init_info_t));

    init_info.max_nof_elements = 100000;
    init_info.expected_nof_elements = 10000;
    init_info.print_cb_name = "sw_state_htb_multithread_print";

    mta_db[unit].lock = sal_mutex_create("mta_db lock");
    sal_mutex_take(mta_db[unit].lock, MTA_TIMEOUT);

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
    SHR_IF_ERR_EXIT(multithread_analyzer.init(unit));
    SHR_IF_ERR_EXIT(multithread_analyzer.htb.create(unit, &init_info));
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));

    mta_db[unit].mta_is_active = TRUE;

    mta_db[unit].log_for_background = TRUE;
    mta_db[unit].log_for_main = TRUE;
    mta_db[unit].log_for_memreg = TRUE;
    mta_db[unit].log_for_aloc_mngr = TRUE;
    mta_db[unit].log_only_aloc_mngr = FALSE;

    if (flags & MTA_FLAG_LOG_ONLY_MAIN)
    {
        mta_db[unit].log_for_background = FALSE;
    }

    if (flags & MTA_FLAG_LOG_ONLY_BACKGROUND)
    {
        mta_db[unit].log_for_main = FALSE;
    }

    if (flags & MTA_FLAG_DONT_LOG_MEM_REG)
    {
        mta_db[unit].log_for_memreg = FALSE;
    }

    if (flags & MTA_FLAG_DONT_LOG_ALLOC_MNGR)
    {
        mta_db[unit].log_for_aloc_mngr = FALSE;
    }

    if (flags & MTA_FLAG_LOG_ONLY_ALLOC_MNGR)
    {
        mta_db[unit].log_only_aloc_mngr = TRUE;
    }

    if (flags & MTA_FLAG_LOG_ONLY_ALLOC_MNGR)
    {
        mta_db[unit].log_for_memreg = FALSE;
    }

    
    for (i = 0; i < mta_db[unit].nof_registered_threads; i++)
    {
        mta_db[unit].thread[i].nof_htb_insertions = 0;
    }

    
    mta_db[unit].is_application_init_done = TRUE;

    SHR_EXIT();
exit:
    sal_mutex_give(mta_db[unit].lock);
    SHR_FUNC_EXIT;
}


int
dnxc_multithread_analyzer_generate_summary_files(
    int unit,
    uint32 flags,
    char *file_name)
{
    SW_STATE_HASH_TABLE_ITER iterator;
    thread_access_log_entry_t key;
    uint32 value;
    FILE *output_file = NULL;
    int i, j;
    uint8 sanity = 0;
    SHR_FUNC_INIT_VARS(unit);

    mta_db[unit].mta_is_active = FALSE;

    
    sal_usleep(1000);

    sal_mutex_take(mta_db[unit].lock, MTA_TIMEOUT);

    SW_STATE_HASH_TABLE_ITER_SET_BEGIN(&iterator);

    
    SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_organize_thread_info(unit));

    
    if (!(flags & MTA_FLAG_PRINT_TO_SCREEN))
    {
        output_file = sal_fopen(file_name, "w");
        if (!output_file)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Failed to open file.\n")));
            SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
        }
    }

    
    SHR_IF_ERR_EXIT(multithread_analyzer.htb.get_next(unit, &iterator, &key, &value));

    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&iterator))
    {
        if (flags & MTA_FLAG_PRINT_TO_SCREEN)
        {
            cli_out("%s,%s,%s,%s,%s,%s\n",
                    dnxc_multithread_analyzer_thread_name_get(unit, key.thread_id),
                    dnxc_multithread_analyzer_resource_type_name_get(key.resource_type),
                    dnxc_multithread_analyzer_resource_id_name_get(unit, key.resource_type, key.resource_id),
                    key.is_write ? "write" : "read", key.function_name,
                    dnxc_multithread_analyzer_mutex_list_get(key.mutex_name));
        }
        else
        {
            sal_fprintf(output_file,
                        "%s,%s,%s,%s,%s,%s\n",
                        dnxc_multithread_analyzer_thread_name_get(unit, key.thread_id),
                        dnxc_multithread_analyzer_resource_type_name_get(key.resource_type),
                        dnxc_multithread_analyzer_resource_id_name_get(unit, key.resource_type, key.resource_id),
                        key.is_write ? "write" : "read", key.function_name,
                        dnxc_multithread_analyzer_mutex_list_get(key.mutex_name));
        }

        
        SHR_IF_ERR_EXIT(multithread_analyzer.htb.get_next(unit, &iterator, &key, &value));
    }

    if (!(flags & MTA_FLAG_PRINT_TO_SCREEN))
    {
        sal_fclose(output_file);
    }

    SHR_IF_ERR_EXIT(multithread_analyzer.htb.clear(unit));
    SHR_IF_ERR_EXIT(multithread_analyzer.htb.destroy(unit));
    SHR_IF_ERR_EXIT(multithread_analyzer.deinit(unit));

    for (i = 0; i < MTA_DYN_SUB_RES_NOF; i++)
    {
        for (j = 0; j < MTA_MAX_DYNAMIC_RES_IDS; j++)
        {
            if (mta_db[unit].dynamic_resource_ids[i][j])
            {
                sal_free(mta_db[unit].dynamic_resource_ids[i][j]);
                mta_db[unit].dynamic_resource_ids[i][j] = NULL;
            }
        }
        mta_db[unit].nof_dynamic_resources[i] = 0;
    }

    sal_mutex_give(mta_db[unit].lock);
    sal_mutex_destroy(mta_db[unit].lock);

    

    sanity = 1;

    SHR_EXIT();
exit:
    if (!sanity)
    {
        cli_out("failed in MTA generate summary files and didn't give up mutex or released memory");
        assert(0);
    }
    SHR_FUNC_EXIT;
}


int
dnxc_multithread_analyzer_declare_api_in_play(
    int unit,
    const char *api_name,
    uint32 flags,
    uint8 in_play)
{
    uint32 mta_thread_id;
    mta_thread_info_t *mta_thread;
    int rv = _SHR_E_NONE;

    if (flags & MTA_FLAG_MUTEX)
    {
        
        goto exit;
    }

    
    if (!SOC_UNIT_VALID(unit))
    {
        goto exit;
    }

    if (!(SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)))
    {
        goto exit;
    }

    if (!dnxc_multithread_analyzer_is_active(unit, flags))
    {
        goto exit;
    }

    if (!dnxc_multithread_analyzer_is_logging_for_this_thread(unit))
    {
        goto exit;
    }

    
    if (dnxc_multithread_analyzer_should_skip_api_declaration(unit, api_name, flags, in_play))
    {
        goto exit;
    }

    rv = dnxc_multithread_analyzer_thread_id_get(unit, &mta_thread_id);
    if (rv != _SHR_E_NONE)
    {
        goto exit;
    }

    mta_thread = &mta_db[unit].thread[mta_thread_id];

    
    if (mta_thread->tmp_disabled)
    {
        goto exit;
    }

    if (in_play == TRUE)
    {
        int len = sal_strlen(api_name);
        const char *name_to_log = NULL;

        assert(len < MTA_MAX_API_NAME_LENGTH);

        
        name_to_log = (flags & MTA_FLAG_CINT) ? mta_db[unit].cints[mta_db[unit].nof_cints] : api_name;

        
        if (flags & MTA_FLAG_CINT)
        {
            assert(mta_db[unit].nof_cints < MTA_MAX_LOGGED_CINTS);
            dnxc_multithread_analyzer_cint_name_set(mta_db[unit].cints[mta_db[unit].nof_cints], api_name);
            mta_db[unit].nof_cints++;
        }

        assert(MTA_MAX_NOF_ELEMENTS_IN_PLAY > mta_thread->mta_nof_apis_in_play);

        mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play].name = name_to_log;

        if (flags & MTA_FLAG_MUTEX)
        {
            mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play].type = MTA_ELEMENT_MUTEX;
        }
        else
        {
            mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play].type = MTA_ELEMENT_API;
        }

        mta_thread->mta_nof_apis_in_play++;
    }
    else if (in_play == FALSE)
    {
        int idx;
        uint8 found = 0;
        const char *name_to_log = NULL;

        
        name_to_log = (flags & MTA_FLAG_CINT) ? mta_db[unit].cints[mta_db[unit].nof_cints - 1] : api_name;

        
        for (idx = mta_thread->mta_nof_apis_in_play - 1; idx >= 0; idx--)
        {
            if (name_to_log == mta_thread->element_in_play[idx].name)
            {
                found = 1;
                mta_thread->mta_nof_apis_in_play--;
                mta_thread->element_in_play[idx] = mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play];

                mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play].type = MTA_ELEMENT_UNKNOWN;
                mta_thread->element_in_play[mta_thread->mta_nof_apis_in_play].name = NULL;
            }
        }
        if (found == 0)
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "could not find %s in the API in play list\n"), api_name));
        }
    }
    else
    {
        assert(0);
    }

    goto exit;
exit:
    return rv;
}


int
dnxc_multithread_analyzer_log_resource_use(
    int unit,
    mta_resource_e resource_type,
    uint32 resource_id,
    uint8 is_write)
{
    thread_access_log_entry_t key;
    uint32 value = 0;
    uint32 idx = 0;
    uint32 j = 0;
    uint8 success = 0;
    uint8 nof_mutex_used = 0;
    uint32 mta_thread_id;
    mta_thread_info_t *mta_thread;
    uint8 mutex_released = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnxc_multithread_analyzer_is_active(unit, MTA_NO_FLAGS))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_thread_id_get(unit, &mta_thread_id));

    mta_thread = &mta_db[unit].thread[mta_thread_id];

    if (resource_type == MTA_RESOURCE_MEM || resource_type == MTA_RESOURCE_REG)
    {
        if (mta_thread->dbal_region_counter == 0)
        {
            
            resource_type =
                (resource_type == MTA_RESOURCE_MEM) ? MTA_RESOURCE_MEM_OUTSIDE_DBAL : MTA_RESOURCE_REG_OUTSIDE_DBAL;
        }
        else
        {
            
            if (mta_db[unit].log_for_memreg == FALSE)
            {
                SHR_EXIT();
            }
        }
    }

    if (resource_type == MTA_RESOURCE_RES_MGR || resource_type == MTA_RESOURCE_TMP_MGR)
    {
        if (mta_db[unit].log_for_aloc_mngr == FALSE)
        {
            SHR_EXIT();
        }
    }
    else
    {
        if (mta_db[unit].log_only_aloc_mngr == TRUE)
        {
            SHR_EXIT();
        }
    }

    if (!dnxc_multithread_analyzer_is_logging_for_this_thread(unit))
    {
        SHR_EXIT();
    }

    
    if (mta_thread->tmp_disabled)
    {
        SHR_EXIT();
    }

    sal_memset(&key, 0x0, sizeof(thread_access_log_entry_t));
    key.is_write = is_write;
    key.resource_type = resource_type;
    key.resource_id = resource_id;
    key.thread_id = mta_thread_id;

    mutex_released = FALSE;
    sal_mutex_take(mta_db[unit].lock, MTA_TIMEOUT);

    
    for (idx = 0; idx < mta_thread->mta_nof_apis_in_play; idx++)
    {
        assert(nof_mutex_used < MTA_MAX_NOF_MUTEXES);
        if (mta_thread->element_in_play[idx].type == MTA_ELEMENT_MUTEX)
        {
            int found = 0;
            for (j = 0; j < nof_mutex_used; j++)
            {
                if (key.mutex_name[j] == mta_thread->element_in_play[idx].name)
                {
                    found = 1;
                }
            }
            
            if (!found)
            {
                key.mutex_name[nof_mutex_used] = mta_thread->element_in_play[idx].name;
                nof_mutex_used++;
            }
        }
    }

    for (idx = 0; idx < mta_thread->mta_nof_apis_in_play; idx++)
    {
        if (mta_thread->element_in_play[idx].type == MTA_ELEMENT_MUTEX)
        {
            continue;
        }
        success = 0;
        key.function_name = mta_thread->element_in_play[idx].name;
        mta_thread->tmp_disabled++;
        SHR_IF_ERR_EXIT(multithread_analyzer.htb.insert(unit, &key, &value, &success));
        mta_thread->nof_htb_insertions++;
        mta_thread->tmp_disabled--;
        if (!success)
        {

            dnxc_multithread_analyzer_mta_db_dump(unit);
            cli_out
                ("htb insert did not succeed in MTA thread=%s,resource=%s,sub_resource=%s,is_write=%s,function=%s,mutexes=%s\n",
                 dnxc_multithread_analyzer_thread_name_get(unit, key.thread_id),
                 dnxc_multithread_analyzer_resource_type_name_get(key.resource_type),
                 dnxc_multithread_analyzer_resource_id_name_get(unit, key.resource_type, key.resource_id),
                 key.is_write ? "write" : "read", key.function_name,
                 dnxc_multithread_analyzer_mutex_list_get(key.mutex_name));
            assert(success);
        }
    }

    mutex_released = TRUE;
    sal_mutex_give(mta_db[unit].lock);

    SHR_EXIT();
exit:
    assert(mutex_released);
    SHR_FUNC_EXIT;
}


int
dnxc_multithread_analyzer_mark_dbal_region(
    int unit,
    int is_start)
{

    uint32 mta_thread_id;
    mta_thread_info_t *mta_thread;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnxc_multithread_analyzer_is_active(unit, MTA_NO_FLAGS))
    {
        SHR_EXIT();
    }

    if (!dnxc_multithread_analyzer_is_logging_for_this_thread(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_thread_id_get(unit, &mta_thread_id));
    mta_thread = &mta_db[unit].thread[mta_thread_id];

    if (is_start)
    {
        mta_thread->dbal_region_counter++;
    }
    else
    {
        mta_thread->dbal_region_counter--;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


int
dnxc_multithread_analyzer_dynamic_sub_res_get(
    int unit,
    mta_dynamic_res_id_type_e resource,
    char *res_name)
{
    int i;

    if (!dnxc_multithread_analyzer_is_active(unit, MTA_NO_FLAGS))
    {
        return -1;
    }

    if (res_name[0] == '\0')
    {
        res_name = "unknown resource";
    }

    assert(mta_db[unit].nof_dynamic_resources[resource] < MTA_MAX_DYNAMIC_RES_IDS);

    
    for (i = 0; i < mta_db[unit].nof_dynamic_resources[resource]; i++)
    {

        if (mta_db[unit].dynamic_resource_ids[resource][i] == res_name ||
            !sal_strncmp(res_name, mta_db[unit].dynamic_resource_ids[resource][i], 128))
        {
            return i;
        }
    }

    mta_db[unit].dynamic_resource_ids[resource][mta_db[unit].nof_dynamic_resources[resource]] = res_name;

    mta_db[unit].dynamic_resource_ids[resource][mta_db[unit].nof_dynamic_resources[resource]] =
        sal_alloc(MTA_MAX_RESOURCE_NAME_LENGTH, "mta resource name");
    if (!mta_db[unit].dynamic_resource_ids[resource][mta_db[unit].nof_dynamic_resources[resource]])
    {
        cli_out("could not allocate memory for MTA resource name string");
        assert(0);
    }
    sal_strncpy(mta_db[unit].dynamic_resource_ids[resource][mta_db[unit].nof_dynamic_resources[resource]], res_name,
                MTA_MAX_THREAD_NAME_LENGTH - 1);
    mta_db[unit].nof_dynamic_resources[resource]++;

    return mta_db[unit].nof_dynamic_resources[resource] - 1;
}

#endif 
