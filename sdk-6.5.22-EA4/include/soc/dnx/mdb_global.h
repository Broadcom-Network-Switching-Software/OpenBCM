
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef MDB_GLOBAL_H_INCLUDED

#define MDB_GLOBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

typedef enum
{

    MDB_MACRO_A,

    MDB_MACRO_B,

    MDB_EEDB_BANK,

    MDB_NOF_MACRO_TYPES
} mdb_macro_types_e;

typedef enum
{
    MDB_EEDB_MEM_TYPE_DATA_CLUSTER,
    MDB_EEDB_MEM_TYPE_DATA_BANK,
    MDB_EEDB_MEM_TYPE_LL,

    MDB_NOF_EEDB_MEM_TYPES
} mdb_eedb_mem_type_e;

typedef enum
{
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_ISEM_1,
    MDB_PHYSICAL_TABLE_ISEM_2,
    MDB_PHYSICAL_TABLE_ISEM_3,
#endif
    MDB_PHYSICAL_TABLE_INLIF_1,
    MDB_PHYSICAL_TABLE_INLIF_2,
    MDB_PHYSICAL_TABLE_INLIF_3,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_IVSI,
#endif
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_LEM,
#endif
    MDB_PHYSICAL_TABLE_ADS_1,
    MDB_PHYSICAL_TABLE_ADS_2,
    MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1,
    MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_IOEM_0,
    MDB_PHYSICAL_TABLE_IOEM_1,
    MDB_PHYSICAL_TABLE_EOEM_0,
    MDB_PHYSICAL_TABLE_EOEM_1,
#endif
    MDB_PHYSICAL_TABLE_MAP,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_FEC_1,
    MDB_PHYSICAL_TABLE_FEC_2,
    MDB_PHYSICAL_TABLE_FEC_3,
#endif
    MDB_PHYSICAL_TABLE_MC_ID,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_GLEM_0,
#endif
    MDB_PHYSICAL_TABLE_GLEM_1,
    MDB_PHYSICAL_TABLE_EEDB_1_LL,
    MDB_PHYSICAL_TABLE_EEDB_1_DATA,
    MDB_PHYSICAL_TABLE_EEDB_2_LL,
    MDB_PHYSICAL_TABLE_EEDB_2_DATA,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EEDB_1_2_DATA,
#endif
    MDB_PHYSICAL_TABLE_EEDB_3_LL,
    MDB_PHYSICAL_TABLE_EEDB_3_DATA,
    MDB_PHYSICAL_TABLE_EEDB_4_LL,
    MDB_PHYSICAL_TABLE_EEDB_4_DATA,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EEDB_3_4_DATA,
#endif
    MDB_PHYSICAL_TABLE_EEDB_5_LL,
    MDB_PHYSICAL_TABLE_EEDB_5_DATA,
    MDB_PHYSICAL_TABLE_EEDB_6_LL,
    MDB_PHYSICAL_TABLE_EEDB_6_DATA,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EEDB_5_6_DATA,
#endif
    MDB_PHYSICAL_TABLE_EEDB_7_LL,
    MDB_PHYSICAL_TABLE_EEDB_7_DATA,
    MDB_PHYSICAL_TABLE_EEDB_8_LL,
    MDB_PHYSICAL_TABLE_EEDB_8_DATA,
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EEDB_7_8_DATA,
#endif
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_ESEM,
#endif
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EVSI,
#endif
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_EXEM_1,
    MDB_PHYSICAL_TABLE_EXEM_2,
    MDB_PHYSICAL_TABLE_EXEM_3,
    MDB_PHYSICAL_TABLE_EXEM_4,
#endif
#if defined(BCM_DNX2_SUPPORT)
    MDB_PHYSICAL_TABLE_RMEP,
#endif

    MDB_NOF_PHYSICAL_TABLES,
} mdb_physical_table_e;

typedef enum
{
    MDB_DIRECT_TYPE_VSI,
    MDB_DIRECT_TYPE_KAPS,
    MDB_DIRECT_TYPE_INLIF,
    MDB_DIRECT_TYPE_MAP,
    MDB_DIRECT_TYPE_FEC,

    MDB_NOF_DIRECT_TYPES
} mdb_direct_type_e;

typedef enum
{
    MDB_EM_TYPE_ISEM,
    MDB_EM_TYPE_LEM,
    MDB_EM_TYPE_OEM,
    MDB_EM_TYPE_PPMC,
    MDB_EM_TYPE_GLEM,
    MDB_EM_TYPE_ESEM,
    MDB_EM_TYPE_EXEM,
    MDB_EM_TYPE_RMEP_EM,

    MDB_NOF_EM_TYPES
} mdb_em_type_e;

#define MDB_LPM_DBS_IN_PAIR (2)

#endif
