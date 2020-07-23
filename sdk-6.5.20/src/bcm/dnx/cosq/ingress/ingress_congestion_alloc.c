/** \file ingress_congestion_alloc.c
 * 
 *
 * Allocation for ingress congestion module
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include "vsq_rate_class.h"

#include <bcm_int/dnx/cosq/cosq.h>

/* ---------------------------------------------------
 * VSQ Rate Class Template Manager
 *
 *----------------------------------------------------
 */

/*
 * Template Manager for:
 *  Category VSQ Rate class,
 *  Traffic class VSQ Rate class,
 *  Connection class VSQ Rate class,
 *  Statistics VSQ Rate class,
 *  Source Port VSQ Rate class,
 *  PG VSQ Rate class
 */

int
dnx_ingress_congestion_alloc_vsq_rate_class_template_mngr_init(
    int unit)
{
    dnx_ingress_congestion_vsq_group_e vsq_type;
    dnx_algo_template_create_data_t create_info;
    uint32 core_id;

    dnx_ingress_congestion_vsq_rate_class_info_t vsq_rate_class_info;

    static const char *template_name[] = {
        "TEMPLATE_VSQ_RATE_CLS_CT",
        "TEMPLATE_VSQ_RATE_CLS_CTTC",
        "TEMPLATE_VSQ_RATE_CLS_CTCC",
        "TEMPLATE_VSQ_RATE_CLS_PP",
        "TEMPLATE_VSQ_RATE_CLS_SRC_PORT",
        "TEMPLATE_VSQ_RATE_CLS_PG"
    };

    SHR_FUNC_INIT_VARS(unit);

    for (vsq_type = 0; vsq_type < DNX_INGRESS_CONGESTION_VSQ_GROUP_NOF; vsq_type++)
    {

        sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));

        create_info.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
        create_info.first_profile = 0;
        create_info.nof_profiles = (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY ?
                                    dnx_data_ingr_congestion.vsq.vsq_a_rate_class_nof_get(unit) :
                                    dnx_data_ingr_congestion.vsq.vsq_rate_class_nof_get(unit));

        create_info.max_references = dnx_data_ingr_congestion.vsq.info_get(unit, vsq_type)->nof;

        create_info.default_profile = 0;
        sal_strncpy(create_info.name, template_name[vsq_type], DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_default_data_get(unit, vsq_type, &vsq_rate_class_info));

        if (vsq_type < DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            create_info.default_data = &vsq_rate_class_info.data.vsq_a_d_rate_class_info;
            create_info.data_size = sizeof(dnx_ingress_congestion_queue_vsq_rate_class_info_t);
            if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
            {
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_d.alloc(unit));

                for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
                {
                    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_d.create(unit,
                                                                                                   core_id,
                                                                                                   &create_info, NULL));
                }

            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_a_c.create(unit, vsq_type,
                                                                                                 &create_info, NULL));
            }
        }
        else if (vsq_type == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            create_info.default_data = &vsq_rate_class_info.data.vsq_e_rate_class_info;
            create_info.data_size = sizeof(dnx_ingress_congestion_src_port_vsq_rate_class_info_t);

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_source_port.alloc(unit));

            for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_source_port.create(unit,
                                                                                                         core_id,
                                                                                                         &create_info,
                                                                                                         NULL));
            }
        }
        else
        {
            create_info.default_data = &vsq_rate_class_info.data.vsq_f_rate_class_info;
            create_info.data_size = sizeof(dnx_ingress_congestion_pg_vsq_rate_class_info_t);

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_pg.alloc(unit));

            for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_pg.create(unit,
                                                                                                core_id,
                                                                                                &create_info, NULL));
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ingress_congestion_alloc_vsq_rate_class_data_get(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_group,
    int vsq_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls)
{
    int rate_cls, ref_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_group, vsq_id, &rate_cls));

    data_rate_cls->vsq_type = vsq_group;

    switch (vsq_group)
    {
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG:
            /** VSQ D */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.
                            vsq_rate_cls_d.profile_data_get(unit, core_id, rate_cls, &ref_count,
                                                            &data_rate_cls->data.vsq_a_d_rate_class_info));
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT:
            /** VSQ E */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.
                            vsq_rate_cls_source_port.profile_data_get(unit, core_id, rate_cls, &ref_count,
                                                                      &data_rate_cls->data.vsq_e_rate_class_info));
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_PG:
            /** VSQ F */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.
                            vsq_rate_cls_pg.profile_data_get(unit, core_id, rate_cls, &ref_count,
                                                             &data_rate_cls->data.vsq_f_rate_class_info));
            break;
        default:
            /** VSQ A-C */
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.
                            vsq_rate_cls_a_c.profile_data_get(unit, vsq_group, rate_cls, &ref_count,
                                                              &data_rate_cls->data.vsq_a_d_rate_class_info));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_ingress_congestion_alloc_vsq_rate_class_exchange(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_group,
    int vsq_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls,
    int *old_rate_cls,
    int *is_last,
    int *new_rate_cls,
    int *is_allocated)
{
    int tmp_old_rate_cls;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_alloc = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core_id, vsq_group, vsq_id, &tmp_old_rate_cls));

    switch (vsq_group)
    {
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.
                            vsq_rate_cls_d.exchange(unit, core_id, 0, &data_rate_cls->data.vsq_a_d_rate_class_info,
                                                    tmp_old_rate_cls, NULL, new_rate_cls, &tmp_is_alloc, &tmp_is_last));
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_source_port.exchange(unit, core_id, 0,
                                                                                                       &data_rate_cls->data.vsq_e_rate_class_info,
                                                                                                       tmp_old_rate_cls,
                                                                                                       NULL,
                                                                                                       new_rate_cls,
                                                                                                       &tmp_is_alloc,
                                                                                                       &tmp_is_last));
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_PG:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_pg.exchange(unit, core_id, 0,
                                                                                              &data_rate_cls->
                                                                                              data.vsq_f_rate_class_info,
                                                                                              tmp_old_rate_cls, NULL,
                                                                                              new_rate_cls,
                                                                                              &tmp_is_alloc,
                                                                                              &tmp_is_last));
            break;
        default:
            SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_rate_class.vsq_rate_cls_a_c.exchange(unit, vsq_group,
                                                                                               0,
                                                                                               &data_rate_cls->
                                                                                               data.vsq_a_d_rate_class_info,
                                                                                               tmp_old_rate_cls, NULL,
                                                                                               new_rate_cls,
                                                                                               &tmp_is_alloc,
                                                                                               &tmp_is_last));

            break;
    }

    if (old_rate_cls != NULL)
    {
        *old_rate_cls = tmp_old_rate_cls;
    }
    if (is_last != NULL)
    {
        *is_last = tmp_is_last ? 1 : 0;
    }
    if (is_allocated != NULL)
    {
        *is_allocated = tmp_is_alloc ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

#define DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_WRED_PRINT(wred)                                                             \
    do                                                                                                     \
    {                                                                                                      \
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "WRED", NULL, NULL);    \
        {                                                                                                  \
            int dp;                                                                                        \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                                 \
            for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)                                                                     \
            {                                                                                              \
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "dp", dp, NULL, NULL);   \
                                                                                                           \
                {                                                                                          \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                         \
                                                                                                           \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "enable",            \
                                                        (wred)->wred_params[dp].wred_en, NULL, NULL);                       \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "min avrg threshold", \
                                                        (wred)->wred_params[dp].min_avrg_th, NULL, NULL);                  \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max avrg threshold", \
                                                        (wred)->wred_params[dp].max_avrg_th, NULL, NULL);                  \
                                                                                                               \
                                                                                                               \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "max probability",       \
                                                        (wred)->wred_params[dp].max_probability, NULL, NULL);              \
                                                                                                               \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                               \
                                                                                                               \
                }                                                                                              \
                                                                                                               \
            }                                                                                                  \
                                                                                                               \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                                       \
        }                                                                                                      \
    }                                                                                                          \
    while (0)

/**
 * print simple FC of rate class
 * used for vsq A-E
 */
#define DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_FC_PRINT(data)                                                                        \
    do                                                                                                              \
    {                                                                                                               \
        int resource;                                                                                               \
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "FC", NULL, NULL);               \
        {                                                                                                           \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                                          \
            for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)                                                            \
            {                                                                                                       \
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL); \
                {                                                                                                   \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                                  \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "set  ",                   \
                                                        (data)->fc[resource].set, NULL, NULL);                      \
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "clear",                   \
                                                        (data)->fc[resource].clear, NULL, NULL);                    \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                                    \
                }                                                                                                   \
                                                                                                                    \
            }                                                                                                       \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                                            \
        }                                                                                                           \
    }                                                                                                               \
    while (0)

/**
 * print Guaranteed size of VSQ rate class
 * used for vsq E and F
 */
#define DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_GUARANTEED_PRINT(data)                                                                \
    do                                                                                                              \
    {                                                                                                               \
        int resource;                                                                                               \
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Guaranteed", NULL, NULL);       \
        {                                                                                                           \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                                          \
            for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)                                                            \
            {                                                                                                       \
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL); \
                {                                                                                                   \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();                                                  \
                    SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "guaranteed size",          \
                                                       (data)->guaranteed_size[resource], "index is dp", NULL, DNX_COSQ_NOF_DP);  \
                    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                                    \
                }                                                                                                   \
                                                                                                                    \
            }                                                                                                       \
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();                                                            \
                                                                                                                    \
        }                                                                                                           \
    }                                                                                                               \
    while (0)

void
dnx_ingress_congestion_alloc_vsq_a_d_rate_cls_print_cb(
    int unit,
    const void *data)
{
    dnx_ingress_congestion_queue_vsq_rate_class_info_t *data_rate_cls;
    int resource;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    data_rate_cls = (dnx_ingress_congestion_queue_vsq_rate_class_info_t *) data;

    /** WRED */
    DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_WRED_PRINT(&data_rate_cls->wred);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "exp weight",
                                   data_rate_cls->wred_exp_wq, NULL, NULL);

    /** Tail drop */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Tail drop", NULL, NULL);
    {
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL);
             /* coverity[overrun-buffer-val:FALSE]  */
            SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max size",
                                          data_rate_cls->taildrop.max_size_th[resource], "index is dp", NULL,
                                          DNX_COSQ_NOF_DP);

        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();

    }

    /** FC */
    DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_FC_PRINT(data_rate_cls);

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

void
dnx_ingress_congestion_alloc_vsq_e_rate_cls_print_cb(
    int unit,
    const void *data)
{

    dnx_ingress_congestion_src_port_vsq_rate_class_info_t *data_rate_cls;
    int resource;
    int pool;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    data_rate_cls = (dnx_ingress_congestion_src_port_vsq_rate_class_info_t *) data;

    for (pool = 0; pool < 2; pool++)
    {

        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "pool", pool, NULL, NULL);
        {
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();

            /** WRED */
            DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_WRED_PRINT(&data_rate_cls->pool[pool].wred);

            /** FC */
            DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_FC_PRINT(&data_rate_cls->pool[pool]);

            /** Guaranteed */
             /* coverity[overrun-buffer-val:FALSE]  */
            DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_GUARANTEED_PRINT(&data_rate_cls->pool[pool]);

            /** Shared */
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Shared", NULL, NULL);
            {
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
                {
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL);
                    {
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                         /* coverity[overrun-buffer-val:FALSE]  */
                        SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "shared size",
                                                      data_rate_cls->pool[pool].shared_size[resource],
                                                      "index is dp", NULL, DNX_COSQ_NOF_DP);
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                    }

                }
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();

            }

            /** Headroom */
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Headroom", NULL, NULL);
            {
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                 /* coverity[overrun-buffer-val:FALSE]  */
                SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "headroom size",
                                              data_rate_cls->pool[pool].headroom_size, "index is resource", NULL,
                                              DNX_INGRESS_CONGESTION_RESOURCE_NOF);
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
            }
        }
    }

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "exp weight",
                                   data_rate_cls->wred_exp_wq, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

void
dnx_ingress_congestion_alloc_vsq_f_rate_cls_print_cb(
    int unit,
    const void *data)
{
    dnx_ingress_congestion_pg_vsq_rate_class_info_t *data_rate_cls;
    int resource, dp;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    data_rate_cls = (dnx_ingress_congestion_pg_vsq_rate_class_info_t *) data;

    /** WRED */
    DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_WRED_PRINT(&data_rate_cls->wred);

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "exp weight",
                                   data_rate_cls->wred_exp_wq, NULL, NULL);

    /** FADT FC */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "FC", NULL, NULL);
    {
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL);
            {
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
                {
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "DP", dp, NULL, NULL);
                    {
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Set", NULL, NULL);
                        {
                            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max  ",
                                                           data_rate_cls->fadt_fc[resource].set.max_threshold,
                                                           NULL, NULL);
                            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "min  ",
                                                           data_rate_cls->fadt_fc[resource].set.min_threshold,
                                                           NULL, NULL);
                            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "alpha",
                                                           data_rate_cls->shared_size[resource][dp].alpha, NULL, NULL);
                            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                        }
                        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "clear offset",
                                                       data_rate_cls->fadt_fc[resource].clear_offset, NULL, NULL);
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                    }
                }
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
            }
        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
    }

    /** Guaranteed */
     /* coverity[overrun-buffer-val:FALSE]  */
    DNX_INGRESS_CONGESTION_ALLOC_VSQ_RATE_CLS_GUARANTEED_PRINT(data_rate_cls);

    /** Shared */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Shared", NULL, NULL);
    {
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL);
            {
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
                {
                    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "DP", dp, NULL, NULL);
                    {
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
                        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "min  ",
                                                       data_rate_cls->shared_size[resource][dp].min_threshold,
                                                       NULL, NULL);
                        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max  ",
                                                       data_rate_cls->shared_size[resource][dp].max_threshold,
                                                       NULL, NULL);
                        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "alpha",
                                                       data_rate_cls->shared_size[resource][dp].alpha, NULL, NULL);
                        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
                    }
                }
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
            }
        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
    }

    /** Headroom */
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "Headroom", NULL, NULL);
    {
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();

        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Resource", resource, NULL, NULL);
            {
                SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();

                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max headroom          ",
                                               data_rate_cls->headroom_size[resource].max_headroom, NULL, NULL);
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max headroom nominal  ",
                                               data_rate_cls->headroom_size[resource].max_headroom_nominal, NULL, NULL);
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max headroom extension",
                                               data_rate_cls->headroom_size[resource].max_headroom_extension, NULL,
                                               NULL);

                SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
            }
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
        }
    }

    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/* COSQ vsq rate class - End */

/* ---------------------------------------------------
 * VSQ-F PG TC mapping profile Template Manager
 *
 *----------------------------------------------------
 */

int
dnx_ingress_congestion_alloc_vsq_pg_tc_mapping_profile_create(
    int unit)
{
    dnx_algo_template_create_data_t create_info;

    /** All TCs are mapped to pg 0, by default*/
    dnx_ingress_congestion_vsq_tc_pg_mapping_t tc_pg_mapping = { {0} };

    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create template manager instance
     */
    sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));
    create_info.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    create_info.first_profile = 0;
    create_info.nof_profiles = dnx_data_ingr_congestion.vsq.tc_pg_profile_nof_get(unit);
    create_info.max_references = dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)->nof;
    create_info.default_profile = 0;
    create_info.data_size = sizeof(dnx_ingress_congestion_vsq_tc_pg_mapping_t);
    create_info.default_data = &tc_pg_mapping;
    sal_strncpy(create_info.name, "TC-PG mapping", DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_pg_tc_mapping.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.vsq_pg_tc_mapping.create(unit, core_id, &create_info, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_ingress_congestion_alloc_vsq_pg_tc_mapping_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_ingress_congestion_vsq_tc_pg_mapping_t *tc_pg_mapping = (dnx_ingress_congestion_vsq_tc_pg_mapping_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "TC -> PG mapping", NULL, NULL);

    SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
     /* coverity[overrun-buffer-val:FALSE]  */
    SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "PG", tc_pg_mapping->pg, NULL, NULL, DNX_COSQ_NOF_TC);
    SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();

    SW_STATE_PRETTY_PRINT_FINISH();

}
