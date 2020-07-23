/*! \file bcm56780_a0_ina_2_4_13_l2.c
 *
 * BCM56780_A0 INA_2_4_13 L2 driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l2.h>
#include <bcm_int/ltsw/l2.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L2

/*!
 * \brief Value for invalid id.
 */
#define INVALID_ID 0xffffffff

/*!
 * \brief Element count for L2 tables.
 */
/* Element count for L2_HOST_TABLE. */
#define L2_ELEMENT_CNT_L2_HOST_TABLE 16

/* Element count for L2_HOST_NARROW_TABLE. */
#define L2_ELEMENT_CNT_L2_HOST_NARROW_TABLE 9

/* Element count for LEARN_CACHE_DATA. */
#define L2_ELEMENT_CNT_LEARN_CACHE_DATA 11

/*!
 * \brief Id map for L2 host table.
 */
static const
bcmint_l2_id_map_t l2_host_table_map[] = {
    {
        .idx = BCMINT_L2_HOST_TBL,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt
    },
    {
        .idx = BCMINT_L2_HOST_FLD_MAC_ADDR,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_MAC_ADDRf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_VFI,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_VFIf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_STRENGTH_PROFILE_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_STRENGTH_PROFILE_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_DESTINATION,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_DESTINATIONf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_DESTINATION_TYPE,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_DESTINATION_TYPEf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_CLASS_ID,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_CLASS_IDf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_DST_PVLAN_PORT_TYPE,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_DST_PVLAN_PORT_TYPEf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_DST_VP_GROUP,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_DST_VP_GROUPf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_COMMUNITY_PVLAN_ID,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_COMMUNITY_PVLAN_IDf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_NHOP_2_OR_ECMP_GROUP_INDEX_1,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_NHOP_2_OR_ECMP_GROUP_INDEX_1f
    },
    {
        .idx = BCMINT_L2_HOST_FLD_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_FLEX_CTR_ACTION,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_FLEX_CTR_ACTIONf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_MISC_CTRL_0,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_MISC_CTRL_0f
    },
    {
        .idx = BCMINT_L2_HOST_FLD_MISC_CTRL_1,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_MISC_CTRL_1f
    },
    {
        .idx = BCMINT_L2_HOST_FLD_DST_ENTRY_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_DST_ENTRY_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_SRC_ENTRY_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_TABLEt_SRC_ENTRY_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_FLD_ELEMENT_CNT,
        .val = L2_ELEMENT_CNT_L2_HOST_TABLE
    }
};

/*!
 * \brief Id map for L2 host narrow table.
 */
static const
bcmint_l2_id_map_t l2_host_narrow_table_map[] = {
    {
        .idx = BCMINT_L2_HOST_NARROW_TBL,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_MAC_ADDR,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_MAC_ADDRf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_VFI,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_VFIf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_STRENGTH_PROFILE_INDEX,
        .val = INVALID_ID
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_DESTINATION,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_DESTINATIONf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_DESTINATION_TYPE,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_DESTINATION_TYPEf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_MISC_CTRL_0,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_MISC_CTRL_0f
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_MISC_CTRL_1,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_MISC_CTRL_1f
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_FLEX_CTR_ACTION,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_FLEX_CTR_ACTIONf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_DST_ENTRY_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_DST_ENTRY_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_SRC_ENTRY_INDEX,
        .val = BCM56780_A0_INA_2_4_13_L2_HOST_NARROW_TABLEt_SRC_ENTRY_INDEXf
    },
    {
        .idx = BCMINT_L2_HOST_NARROW_FLD_ELEMENT_CNT,
        .val = L2_ELEMENT_CNT_L2_HOST_NARROW_TABLE
    }
};

/*!
 * \brief Id map for L2 LEARN_CACHE_DATA table.
 */
static const
bcmint_l2_id_map_t learn_cache_data_table_map[] = {
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_TBL,
        .val = LEARN_CACHE_DATAt
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD0,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD0f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD1,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD1f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD2,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD2f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD3,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD3f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD4,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD4f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD5,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD5f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD6,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD6f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_CACHE_FIELD7,
        .val = LEARN_CACHE_DATAt_CACHE_FIELD7f
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_LEARN_CACHE_DATA_INFO_ID,
        .val = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_INFO_IDf
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_PIPE,
        .val = LEARN_CACHE_DATAt_PIPEf
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_LEARN_CACHE_DATA_ID,
        .val = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf
    },
    {
        .idx = BCMINT_L2_LEARN_CACHE_DATA_FLD_ELEMENT_CNT,
        .val = L2_ELEMENT_CNT_LEARN_CACHE_DATA
    }
};

/*!
 * \brief String map for L2 destination type.
 */
static const
bcmint_l2_str_map_t l2_dest_type_map[] = {
    {
        .idx = BCMINT_L2_DEST_TYPE_NO_OP,
        .str = NO_OPs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_L2_OIF,
        .str = L2_OIFs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_RSVD,
        .str = RESERVEDs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_VP,
        .str = VPs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_ECMP,
        .str = ECMPs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_NHOP,
        .str = NHOPs,
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_L2MC_GROUP,
        .str = L2MC_GROUPs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_L3MC_GROUP,
        .str = L3MC_GROUPs
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_RSVD_1,
        .str = RESERVED_1s
    },
    {
        .idx = BCMINT_L2_DEST_TYPE_ECMP_MEMBER,
        .str = ECMP_MEMBERs
    }
};

/*!
 * \brief String map for L2 destination private vlan port type.
 */
static const
bcmint_l2_str_map_t l2_dst_pvlan_port_type_map[] = {
    {
        .idx = BCMINT_L2_DST_PVLAN_PORT_TYPE_PROMISCUOUS,
        .str = PROMISCUOUSs
    },
    {
        .idx = BCMINT_L2_DST_PVLAN_PORT_TYPE_COMMUNITY,
        .str = COMMUNITYs
    },
    {
        .idx = BCMINT_L2_DST_PVLAN_PORT_TYPE_ISOLATED,
        .str = ISOLATEDs
    },
};

/*!
 * \brief L2 id map database.
 */
static const
bcmint_l2_id_map_db_t id_map_db[] = {
    {
        .hdl = BCMINT_L2_MAP_IH_HOST_TABLE,
        .info = l2_host_table_map,
        .num = 18
    },
    {
        .hdl = BCMINT_L2_MAP_IH_HOST_NARROW_TABLE,
        .info = l2_host_narrow_table_map,
        .num = 12
    },
    {
        .hdl = BCMINT_L2_MAP_IH_LEARN_CACHE_DATA_TABLE,
        .info = learn_cache_data_table_map,
        .num = 11
    }
};

/*!
 * \brief L2 string map database.
 */
static const
bcmint_l2_str_map_db_t str_map_db[] = {
    {
        .hdl = BCMINT_L2_MAP_SH_DEST_TYPE,
        .info = l2_dest_type_map,
        .num = 10
    },
    {
        .hdl = BCMINT_L2_MAP_SH_DST_PVLAN_PORT_TYPE,
        .info = l2_dst_pvlan_port_type_map,
        .num = 3
    }
};

/*!
 * \brief L2 map database.
 */
static const
bcmint_l2_map_db_t l2_map_db[] = {
    {
        .id_db = id_map_db,
        .id_num = 3,
        .str_db = str_map_db,
        .str_num = 2
    }
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize L2 map database.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] map_db L2 map database pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_l2_map_db_init(int unit,
                                          const bcmint_l2_map_db_t **map_db)
{
    *map_db = l2_map_db;

    return SHR_E_NONE;
}

/*!
 * \brief L2 driver function variable for bcm56780_a0 INA_2_4_13 device.
 */
static mbcm_ltsw_l2_drv_t bcm56780_a0_ina_2_4_13_ltsw_l2_drv = {
    .l2_map_db_init = bcm56780_a0_ina_2_4_13_ltsw_l2_map_db_init
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_l2_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l2_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_l2_drv));

exit:
    SHR_FUNC_EXIT();
}
