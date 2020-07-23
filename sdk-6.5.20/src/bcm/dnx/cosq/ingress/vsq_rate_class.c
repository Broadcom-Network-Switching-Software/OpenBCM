/** \file vsq_rate_class.c
 *
 * VSQ rate class functionality
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

#include <shared/shrextend/shrextend_error.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cosq/cosq_dbal_utils.h>
#include <soc/dnx/dbal/dbal.h>
#include "rate_class_common.h"
#include "ingress_congestion_dbal.h"
#include "ingress_congestion_alloc.h"
#include "ingress_congestion_convert.h"

static const dbal_tables_e dnx_vsq_rate_class_mapping_table_id[] = {
    /** VSQ A */
    DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING,
    /** VSQ B */
    DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING,
    /** VSQ C */
    DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING,
    /** VSQ D */
    DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING,
    /** VSQ E */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING,
    /** VSQ F */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING
};

static const dbal_fields_e dnx_vsq_dbal_fields_id[] = {
    /** VSQ A */
    DBAL_FIELD_VSQ_A_ID,
    /** VSQ B */
    DBAL_FIELD_VSQ_B_ID,
    /** VSQ C */
    DBAL_FIELD_VSQ_C_ID,
    /** VSQ D */
    DBAL_FIELD_VSQ_D_ID,
    /** VSQ E */
    DBAL_FIELD_VSQ_E_ID,
    /** VSQ F */
    DBAL_FIELD_VSQ_F_ID
};

static const dbal_tables_e dnx_vsq_rate_class_wred_table_id[] = {
    /** VSQ A */
    DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP,
    /** VSQ B */
    DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP,
    /** VSQ C */
    DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP,
    /** VSQ D */
    DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP,
    /** VSQ E */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP,
    /** VSQ F */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP
};

static const dbal_tables_e dnx_vsq_rate_class_wred_avrg_table_id[] = {
    /** VSQ A */
    DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS,
    /** VSQ B */
    DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS,
    /** VSQ C */
    DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS,
    /** VSQ D */
    DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS,
    /** VSQ E */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS,
    /** VSQ F */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS
};

static const dbal_tables_e dnx_vsq_rate_class_tail_drop_table_id[][4] = {
    /** Words */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS},

    /** SRAM buffers */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS},

    /** SRAM PDs */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS}
};

static const dbal_tables_e dnx_vsq_rate_class_fc_table_id[][5] = {
    /** Words */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS,
        /** VSQ E */
     DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS},

    /** SRAM Buffers */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS,
        /** VSQ E */
     DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS},

    /** SRAM PDs */
    {
        /** VSQ A */
     DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS,
        /** VSQ B */
     DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS,
        /** VSQ C */
     DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS,
        /** VSQ D */
     DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS,
        /** VSQ E */
     DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS}
};

static const dbal_tables_e dnx_vsq_f_rate_class_fc_table_id[] = {
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS,
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS,
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS
};

static const dbal_tables_e dnx_vsq_e_rate_class_resource_alloc_table_id[] = {
    /** Words */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS,
    /** SRAM Buffers */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS,
};

static const dbal_tables_e dnx_vsq_f_rate_class_resource_alloc_table_id[] = {
    /** Words */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS,
    /** SRAM Buffers */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS
};

int
dnx_vsq_rate_class_dest_default_data_get(
    int unit,
    dnx_ingress_congestion_queue_vsq_rate_class_info_t * dest_vsq_rate_class_info)
{

    int dp;
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dest_vsq_rate_class_info, 0x0, sizeof(dnx_ingress_congestion_queue_vsq_rate_class_info_t));

    for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            /** set taildrop to maximal available resource */
            dest_vsq_rate_class_info->taildrop.max_size_th[rsrc_type][dp] =
                dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
        }
        /** set FC to maximal available resource */
        dest_vsq_rate_class_info->fc[rsrc_type].set = dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
        dest_vsq_rate_class_info->fc[rsrc_type].clear =
            dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
    }

    SHR_FUNC_EXIT;
}

int
dnx_vsq_rate_class_src_port_default_data_get(
    int unit,
    dnx_ingress_congestion_src_port_vsq_rate_class_info_t * src_port_vsq_rate_class_info)
{

    int dp, pool_id;
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(src_port_vsq_rate_class_info, 0x0, sizeof(dnx_ingress_congestion_src_port_vsq_rate_class_info_t));

    for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
    {
        for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
        {
            for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
            {
                src_port_vsq_rate_class_info->pool[pool_id].guaranteed_size[rsrc_type][dp] = 0;
                /** set taildrop to maximal available resource */
                src_port_vsq_rate_class_info->pool[pool_id].shared_size[rsrc_type][dp] =
                    dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
            }
            src_port_vsq_rate_class_info->pool[pool_id].headroom_size[rsrc_type] = 0;

            /** set FC to maximal available resource */
            src_port_vsq_rate_class_info->pool[pool_id].fc[rsrc_type].set =
                dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
            src_port_vsq_rate_class_info->pool[pool_id].fc[rsrc_type].clear =
                dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
        }
    }

    SHR_FUNC_EXIT;

}

int
dnx_vsq_rate_class_pg_default_data_get(
    int unit,
    dnx_ingress_congestion_pg_vsq_rate_class_info_t * pg_vsq_rate_class_info)
{

    int dp;
    dnx_ingress_congestion_resource_type_e rsrc_type;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(pg_vsq_rate_class_info, 0x0, sizeof(dnx_ingress_congestion_pg_vsq_rate_class_info_t));

    for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            pg_vsq_rate_class_info->guaranteed_size[rsrc_type][dp] = 0;
            pg_vsq_rate_class_info->shared_size[rsrc_type][dp].max_threshold =
                dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
            pg_vsq_rate_class_info->shared_size[rsrc_type][dp].min_threshold =
                dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
            pg_vsq_rate_class_info->shared_size[rsrc_type][dp].alpha = 0;
        }
        pg_vsq_rate_class_info->headroom_size[rsrc_type].max_headroom = 0;
        pg_vsq_rate_class_info->headroom_size[rsrc_type].max_headroom_nominal = 0;
        pg_vsq_rate_class_info->headroom_size[rsrc_type].max_headroom_extension = 0;

        pg_vsq_rate_class_info->fadt_fc[rsrc_type].set.max_threshold =
            dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
        pg_vsq_rate_class_info->fadt_fc[rsrc_type].set.min_threshold =
            dnx_data_ingr_congestion.info.resource_get(unit, rsrc_type)->max;
        pg_vsq_rate_class_info->fadt_fc[rsrc_type].set.alpha = 0;

    }

    SHR_FUNC_EXIT;

}

/**
 * \brief - 
 * get default rate class data for provided VSQ type
 */
int
dnx_vsq_rate_class_default_data_get(
    int unit,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_vsq_rate_class_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(info, 0x0, sizeof(dnx_ingress_congestion_vsq_rate_class_info_t));
    info->vsq_type = vsq_type;

    switch (vsq_type)
    {
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT:
            /** VSQ E */
            SHR_IF_ERR_EXIT(dnx_vsq_rate_class_src_port_default_data_get(unit, &info->data.vsq_e_rate_class_info));
            break;
        case DNX_INGRESS_CONGESTION_VSQ_GROUP_PG:
            /** VSQ F */
            SHR_IF_ERR_EXIT(dnx_vsq_rate_class_pg_default_data_get(unit, &info->data.vsq_f_rate_class_info));
            break;
        default:
            /** VSQ A-D */
            SHR_IF_ERR_EXIT(dnx_vsq_rate_class_dest_default_data_get(unit, &info->data.vsq_a_d_rate_class_info));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get a pointer to the VSQ rate class wred data
 */
dnx_ingress_congestion_vsq_rate_class_wred_info_t *
dnx_vsq_rate_class_wred_ptr_get(
    int unit,
    dnx_ingress_congestion_vsq_group_e vsq_group,
    int pool_id,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls)
{

    if (vsq_group < DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        return &(data_rate_cls->data.vsq_a_d_rate_class_info.wred);
    }
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        return &(data_rate_cls->data.vsq_e_rate_class_info.pool[pool_id].wred);
    }
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        return &(data_rate_cls->data.vsq_f_rate_class_info.wred);
    }
    return (void *) NULL;
}

/**
 * \brief -
 * Get a pointer to the VSQ rate class exp weight wred data
 */
uint32 *
dnx_vsq_rate_class_exp_weight_wred_ptr_get(
    int unit,
    const dnx_ingress_congestion_vsq_group_e vsq_group,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_cls)
{

    if (vsq_group < DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        return &(data_rate_cls->data.vsq_a_d_rate_class_info.wred_exp_wq);
    }
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
    {
        return &(data_rate_cls->data.vsq_e_rate_class_info.wred_exp_wq);
    }
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        return &(data_rate_cls->data.vsq_f_rate_class_info.wred_exp_wq);
    }
    return (void *) NULL;
}

/**
 * \brief -
 * Set vsq to rate class mapping
 */
int
dnx_vsq_rate_class_mapping_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_id,
    int rt_cls)
{
    dbal_tables_e table_id;
    int core_exists;

    int nof_keys;
    dnx_cosq_dbal_field_t key[2];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    table_id = dnx_vsq_rate_class_mapping_table_id[vsq_type];

    nof_keys = 1;
    /** key -- rate class */
    key[0].id = dnx_vsq_dbal_fields_id[vsq_type];
    key[0].value = vsq_id;

    /** results */
    result.id = DBAL_FIELD_RATE_CLASS;
    result.value = rt_cls;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &core_exists));
    if (core_exists)
    {
        key[1].id = DBAL_FIELD_CORE_ID;
        key[1].value = core;
        nof_keys++;
    }

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_set(unit, table_id, nof_keys, key, &result));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get vsq to rate class mapping
 */
int
dnx_vsq_rate_class_mapping_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_id,
    int *rt_cls)
{
    dbal_tables_e table_id;
    int core_exists;

    int nof_keys;
    dnx_cosq_dbal_field_t key[2];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(vsq_type, DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, _SHR_E_INTERNAL,
                   "vsq_type %d is not expected\n", vsq_type);

    table_id = dnx_vsq_rate_class_mapping_table_id[vsq_type];

    nof_keys = 1;
    /** key -- rate class */
    key[0].id = dnx_vsq_dbal_fields_id[vsq_type];
    key[0].value = vsq_id;

    /** results */
    result.id = DBAL_FIELD_RATE_CLASS;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &core_exists));
    if (core_exists)
    {
        key[1].id = DBAL_FIELD_CORE_ID;
        key[1].value = core;
        nof_keys++;
    }

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_get(unit, table_id, nof_keys, key, &result));
    *rt_cls = result.value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * retrieve VSQ WRED 
 */
int
dnx_vsq_rate_class_wred_get(
    int unit,
    bcm_core_t core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int rate_class,
    int pool,
    int dp,
    dnx_ingress_congestion_wred_info_t * wred_data,
    uint32 *gain)
{
    dbal_tables_e table_id, avrg_table_id;
    dnx_ingress_congestion_dbal_wred_key_t dbal_key;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_rate_class_wred_table_id[vsq_type];
    avrg_table_id = dnx_vsq_rate_class_wred_avrg_table_id[vsq_type];

    dbal_key.core_id = core;
    dbal_key.rate_class = rate_class;
    dbal_key.pool_id = pool;
    dbal_key.dp = dp;

    SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_get(unit, table_id, &dbal_key, wred_data));

    /** get average weight - not per DP */
    SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_avrg_get(unit, table_id, avrg_table_id, &dbal_key, gain));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Configure VSQ rate class tail drop threshold
 */
int
dnx_vsq_rate_class_tail_drop_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_resource_type_e resource,
    int rt_cls,
    int dp,
    uint32 max_size)
{
    dbal_tables_e table_id;
    int exists;

    int nof_keys = 2;
    dnx_cosq_dbal_field_t key[3];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_rate_class_tail_drop_table_id[resource][vsq_type];

    /** key -- rate class */
    key[0].id = DBAL_FIELD_RATE_CLASS;
    key[0].value = rt_cls;

    /** key -- rate class */
    key[1].id = DBAL_FIELD_DP;
    key[1].value = dp;

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        key[2].id = DBAL_FIELD_CORE_ID;
        key[2].value = core;
        nof_keys++;
    }

    /** results */
    result.id = DBAL_FIELD_MAX_SIZE;
    result.value = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, max_size);

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_set(unit, table_id, nof_keys, key, &result));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Obtain VSQ rate class tail drop threshold
 */
int
dnx_vsq_rate_class_tail_drop_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    dnx_ingress_congestion_resource_type_e resource,
    int rt_cls,
    int dp,
    uint32 *max_size)
{
    int exists;

    int nof_keys = 2;
    dnx_cosq_dbal_field_t key[3];
    dnx_cosq_dbal_field_t result;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_rate_class_tail_drop_table_id[resource][vsq_type];

    /** key -- rate class */
    key[0].id = DBAL_FIELD_RATE_CLASS;
    key[0].value = rt_cls;

    /** key -- rate class */
    key[1].id = DBAL_FIELD_DP;
    key[1].value = dp;

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        key[2].id = DBAL_FIELD_CORE_ID;
        key[2].value = core;
        nof_keys++;
    }

    /** results */
    result.id = DBAL_FIELD_MAX_SIZE;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_get(unit, table_id, nof_keys, key, &result));

    *max_size = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, result.value);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets hysteresis FC parameters
 */
int
dnx_vsq_rate_class_hyst_fc_set(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int pool,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_hyst_threshold_t * fc)
{
    dbal_tables_e table_id;
    dnx_cosq_hyst_threshold_t hw_fc;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_rate_class_fc_table_id[resource][vsq_type];

    hw_fc.set = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fc->set);
    hw_fc.clear = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fc->clear);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_hyst_fc_hw_set(unit, core, table_id, pool, rt_cls, &hw_fc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets hysteresis FC parameters
 */
int
dnx_vsq_rate_class_hyst_fc_get(
    int unit,
    int core,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int pool,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_hyst_threshold_t * fc)
{
    dbal_tables_e table_id;
    dnx_cosq_hyst_threshold_t hw_fc;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_rate_class_fc_table_id[resource][vsq_type];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_hyst_fc_hw_get(unit, core, table_id, pool, rt_cls, &hw_fc));

    fc->set = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fc.set);
    fc->clear = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fc.clear);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets FADT FC parameters (for VSQ-F)
 */
int
dnx_vsq_rate_class_fadt_fc_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc)
{
    dbal_tables_e table_id;
    dnx_cosq_fadt_hyst_threshold_t hw_fadt_fc;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_f_rate_class_fc_table_id[resource];

    hw_fadt_fc.set.max_threshold = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                                            fadt_fc->set.max_threshold);
    hw_fadt_fc.set.min_threshold = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                                            fadt_fc->set.min_threshold);
    hw_fadt_fc.set.alpha = fadt_fc->set.alpha;

    hw_fadt_fc.clear_offset = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                                       fadt_fc->clear_offset);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_set(unit, core, table_id, rt_cls, &hw_fadt_fc));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets FADT FC parameters (for VSQ-F)
 */
int
dnx_vsq_rate_class_fadt_fc_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc)
{
    dbal_tables_e table_id;
    dnx_cosq_fadt_hyst_threshold_t hw_fadt_fc;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_f_rate_class_fc_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_get(unit, core, table_id, rt_cls, &hw_fadt_fc));

    fadt_fc->set.max_threshold = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                                          hw_fadt_fc.set.max_threshold);
    fadt_fc->set.min_threshold = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                                          hw_fadt_fc.set.min_threshold);
    fadt_fc->set.alpha = hw_fadt_fc.set.alpha;

    fadt_fc->clear_offset = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                                     hw_fadt_fc.clear_offset);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ-E resource allocation
 */
int
dnx_vsq_e_rate_class_resource_allocation_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t resource_alloc_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_e_rate_class_resource_alloc_table_id[resource];

    resource_alloc_hw_params.guaranteed =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, resource_alloc_params->guaranteed);
    resource_alloc_hw_params.shared_pool =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, resource_alloc_params->shared_pool);
    resource_alloc_hw_params.headroom =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, resource_alloc_params->headroom);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_set(unit, core, table_id, rt_cls, pool, dp,
                                                                            &resource_alloc_hw_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get VSQ-E resource allocation
 */
int
dnx_vsq_e_rate_class_resource_allocation_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t resource_alloc_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_e_rate_class_resource_alloc_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_get(unit, core, table_id, rt_cls, pool, dp,
                                                                            &resource_alloc_hw_params));

    resource_alloc_params->guaranteed =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, resource_alloc_hw_params.guaranteed);
    resource_alloc_params->shared_pool =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, resource_alloc_hw_params.shared_pool);
    resource_alloc_params->headroom =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, resource_alloc_hw_params.headroom);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ-F resource allocation
 */
int
dnx_vsq_f_rate_class_resource_allocation_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t resource_alloc_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_f_rate_class_resource_alloc_table_id[resource];

    resource_alloc_hw_params.guaranteed =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, resource_alloc_params->guaranteed);
    resource_alloc_hw_params.shared_pool_fadt.max_threshold =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                 resource_alloc_params->shared_pool_fadt.max_threshold);
    resource_alloc_hw_params.shared_pool_fadt.min_threshold =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                 resource_alloc_params->shared_pool_fadt.min_threshold);
    resource_alloc_hw_params.shared_pool_fadt.alpha = resource_alloc_params->shared_pool_fadt.alpha;

    resource_alloc_hw_params.nominal_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                 resource_alloc_params->nominal_headroom);

    if (resource != DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)
    {
        resource_alloc_hw_params.max_headroom =
            DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                     resource_alloc_params->max_headroom);
        resource_alloc_hw_params.headroom_extension =
            DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource,
                                                                     resource_alloc_params->headroom_extension);

    }

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_set(unit, core, table_id, rt_cls, dp,
                                                                            &resource_alloc_hw_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get VSQ-F resource allocation
 */
int
dnx_vsq_f_rate_class_resource_allocation_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t resource_alloc_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_vsq_f_rate_class_resource_alloc_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_get(unit, core, table_id, rt_cls, dp,
                                                                            &resource_alloc_hw_params));

    resource_alloc_params->guaranteed =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, resource_alloc_hw_params.guaranteed);
    resource_alloc_params->shared_pool_fadt.max_threshold =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                 resource_alloc_hw_params.
                                                                 shared_pool_fadt.max_threshold);
    resource_alloc_params->shared_pool_fadt.min_threshold =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                 resource_alloc_hw_params.
                                                                 shared_pool_fadt.min_threshold);
    resource_alloc_params->shared_pool_fadt.alpha = resource_alloc_hw_params.shared_pool_fadt.alpha;

    resource_alloc_params->nominal_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                 resource_alloc_hw_params.nominal_headroom);
    resource_alloc_params->max_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, resource_alloc_hw_params.max_headroom);
    resource_alloc_params->headroom_extension =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource,
                                                                 resource_alloc_hw_params.headroom_extension);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Commit in HW VSQ rate class information 
 */
int
dnx_vsq_rate_class_hw_set(
    int unit,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_vsq_group_e vsq_category_index,
    dnx_ingress_congestion_vsq_rate_class_info_t * rate_class_info)
{
    int dp_index;

    int pool_id;
    dnx_ingress_congestion_vsq_rate_class_wred_info_t *wred;
    dnx_ingress_congestion_vsq_rate_class_taildrop_info_t *taildrop;
    uint32 *wred_exp_weight_ptr;
    dnx_ingress_congestion_resource_type_e rsrc_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;

    dnx_ingress_congestion_dbal_wred_key_t wred_key;
    int wred_any_enable = FALSE;
    dnx_ingress_congestion_wred_avrg_params_t avrg_params;
    dbal_tables_e wred_table_id;

    SHR_FUNC_INIT_VARS(unit);

    wred_table_id = dnx_vsq_rate_class_wred_table_id[vsq_category_index];

    for (pool_id = 0; pool_id < ((vsq_category_index == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT) ?
                                 dnx_data_ingr_congestion.vsq.pool_nof_get(unit) : 1); pool_id++)
    {
        wred_key.pool_id = pool_id;
        wred_key.rate_class = rate_class;
        wred_key.core_id = core_id;

        wred = dnx_vsq_rate_class_wred_ptr_get(unit, vsq_category_index, pool_id, rate_class_info);

        for (dp_index = 0; dp_index < DNX_COSQ_NOF_DP; dp_index++)
        {
            /*
             * -- WRED --
             */
            wred_key.dp = dp_index;

            SHR_IF_ERR_EXIT(dnx_rate_class_common_wred_set(unit, wred_table_id, &wred_key,
                                                           &wred->wred_params[dp_index]));
        }

        /*
         * -- Tail Drop --
         */
        if (vsq_category_index < DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            /*
             * We separate tail drop of destination-based VSQs from source-based VSQs.
             * This part is for the destination-based VSQs tail-drop 
             */
            taildrop = &rate_class_info->data.vsq_a_d_rate_class_info.taildrop;

            for (dp_index = 0; dp_index < DNX_COSQ_NOF_DP; dp_index++)
            {
                for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
                {
                    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_tail_drop_set(unit, core_id, vsq_category_index,
                                                                     rsrc_type, rate_class, dp_index,
                                                                     taildrop->max_size_th[rsrc_type][dp_index]));
                }

            }
        }
        else
        {
            /*
             * We separate tail drop of destination-based VSQs from source-based VSQs.
             * This part relates to the source-based VSQs. 
             */
            dnx_ingress_congestion_src_port_vsq_rate_class_info_t *src_port =
                &rate_class_info->data.vsq_e_rate_class_info;
            dnx_ingress_congestion_vsq_e_resource_alloc_params_t vsq_e_resource_alloc_params;
            dnx_ingress_congestion_pg_vsq_rate_class_info_t *pg = &rate_class_info->data.vsq_f_rate_class_info;
            dnx_ingress_congestion_vsq_f_resource_alloc_params_t vsq_f_resource_alloc_params;

            if (vsq_category_index == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
            {
                for (dp_index = 0; dp_index < DNX_COSQ_NOF_DP; dp_index++)
                {
                    for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
                    {
                        vsq_e_resource_alloc_params.guaranteed =
                            src_port->pool[pool_id].guaranteed_size[rsrc_type][dp_index];
                        vsq_e_resource_alloc_params.shared_pool =
                            src_port->pool[pool_id].shared_size[rsrc_type][dp_index];
                        vsq_e_resource_alloc_params.headroom = src_port->pool[pool_id].headroom_size[rsrc_type];
                        SHR_IF_ERR_EXIT(dnx_vsq_e_rate_class_resource_allocation_set(unit, core_id, rsrc_type,
                                                                                     rate_class, pool_id, dp_index,
                                                                                     &vsq_e_resource_alloc_params));
                    }
                }
            }

            if (vsq_category_index == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
            {
                for (dp_index = 0; dp_index < DNX_COSQ_NOF_DP; dp_index++)
                {
                    for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
                    {
                        vsq_f_resource_alloc_params.guaranteed = pg->guaranteed_size[rsrc_type][dp_index];

                        vsq_f_resource_alloc_params.shared_pool_fadt.max_threshold =
                            pg->shared_size[rsrc_type][dp_index].max_threshold;
                        vsq_f_resource_alloc_params.shared_pool_fadt.min_threshold =
                            pg->shared_size[rsrc_type][dp_index].min_threshold;
                        vsq_f_resource_alloc_params.shared_pool_fadt.alpha = pg->shared_size[rsrc_type][dp_index].alpha;

                        vsq_f_resource_alloc_params.max_headroom = pg->headroom_size[rsrc_type].max_headroom;
                        vsq_f_resource_alloc_params.nominal_headroom =
                            pg->headroom_size[rsrc_type].max_headroom_nominal;
                        vsq_f_resource_alloc_params.headroom_extension =
                            pg->headroom_size[rsrc_type].max_headroom_extension;
                        SHR_IF_ERR_EXIT(dnx_vsq_f_rate_class_resource_allocation_set(unit, core_id, rsrc_type,
                                                                                     rate_class, dp_index,
                                                                                     &vsq_f_resource_alloc_params));
                    }
                }
            }
        }

        /*
         * -- FC --
         */
        for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; ++rsrc_type)
        {
            if (vsq_category_index == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
            {
                /** VSQ-F */
                SHR_IF_ERR_EXIT(dnx_vsq_rate_class_fadt_fc_set(unit, core_id, rsrc_type, rate_class,
                                                               &rate_class_info->data.
                                                               vsq_f_rate_class_info.fadt_fc[rsrc_type]));

            }
            else
            {
                /** VSQ A-E */
                dnx_cosq_hyst_threshold_t *hyst_fc =
                    ((vsq_category_index == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT) ?
                     &rate_class_info->data.vsq_e_rate_class_info.pool[pool_id].fc[rsrc_type] :
                     &rate_class_info->data.vsq_a_d_rate_class_info.fc[rsrc_type]);

                SHR_IF_ERR_EXIT(dnx_vsq_rate_class_hyst_fc_set(unit, core_id, vsq_category_index, pool_id, rsrc_type,
                                                               rate_class, hyst_fc));
            }
        }

    }

    /*
     * Exponential weight (gain)
     */
    wred_exp_weight_ptr = dnx_vsq_rate_class_exp_weight_wred_ptr_get(unit, vsq_category_index, rate_class_info);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_enable_hw_get(unit, wred_table_id, &wred_key, &wred_any_enable));

    /** WRED exp wq parameter is same for all dps */
    avrg_params.avrg_en = wred_any_enable;
    avrg_params.avrg_weight = *wred_exp_weight_ptr;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_weight_hw_set(unit,
                                                                   dnx_vsq_rate_class_wred_avrg_table_id
                                                                   [vsq_category_index], core_id, wred_key.rate_class,
                                                                   &avrg_params));
exit:
    SHR_FUNC_EXIT;
}

/* 
 * VSQ rate class exchange set commit includes allocation management and HW.
 */
int
dnx_vsq_rate_class_exchange_and_set(
    int unit,
    int core_id,
    dnx_ingress_congestion_vsq_group_e vsq_type,
    int vsq_index,
    dnx_ingress_congestion_vsq_rate_class_info_t * data_rate_class)
{
    dnx_ingress_congestion_vsq_rate_class_info_t cleared_data_rate;
    int new_rate_class, is_allocated, old_rate_class, is_last;

    SHR_FUNC_INIT_VARS(unit);

    /** Update information in SW */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_alloc_vsq_rate_class_exchange(unit, core_id, vsq_type, vsq_index,
                                                                         data_rate_class, &old_rate_class,
                                                                         &is_last, &new_rate_class, &is_allocated));

    /** Commit Changes in HW according to given thresholds */
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_hw_set(unit, core_id, new_rate_class, vsq_type, data_rate_class));
    }

    /** Set new mapping */
    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_set(unit, core_id, vsq_type, vsq_index, new_rate_class));

    /** last profile data */
    if (is_last && new_rate_class != old_rate_class)
    {
        /** clear profile data */
        sal_memset(&cleared_data_rate, 0x0, sizeof(dnx_ingress_congestion_vsq_rate_class_info_t));
        SHR_IF_ERR_EXIT(dnx_vsq_rate_class_hw_set(unit, core_id, old_rate_class, vsq_type, &cleared_data_rate));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * unset rate class of the VSQ by setting it to default rate class
 */
int
dnx_vsq_rate_class_destroy(
    int unit,
    int core_id,
    int vsq_index,
    dnx_ingress_congestion_vsq_group_e vsq_type)
{
    dnx_ingress_congestion_vsq_rate_class_info_t vsq_rate_class_info;

    SHR_FUNC_INIT_VARS(unit);

    /** reset all rate class thresholds to their default values --> point to default profile */
    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_default_data_get(unit, vsq_type, &vsq_rate_class_info));

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_exchange_and_set(unit, core_id, vsq_type, vsq_index, &vsq_rate_class_info));

exit:
    SHR_FUNC_EXIT;
}
