/*! \file bcm56780_a0_dna_2_4_13_dev_misc.c
 *
 * Broadcom BCM56780_a0 DNA_2_4_13 dev misc configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/dev_misc_int.h>
#include <bcm_int/ltsw/mbcm/dev_misc.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/******************************************************************************
 * Private functions
 */

/* Egress forwarding type. */
typedef enum egr_fwd_type_e {
    /* L2 unicast forwarding. */
    fwdTypeL2uc = 0,

    /* L2 multicast forwarding. */
    fwdTypeL2mc = 1,

    /* L3 unicast forwarding. */
    fwdTypeL3uc = 2,

    /* IPMC forwarding. */
    fwdTypeIpmc = 3,

    /* Copy to CPU. */
    fwdTypeCopy2Cpu = 4,

    /* Other types of forwarding. */
    fwdTypeOther = 8
} egr_fwd_type_t;

/* Egress mirror forwarding type. */
typedef enum egr_mirror_fwd_type_e {
    /* Mirror forwarding. */
    fwdTypeMirror = 0,

    /* XMirror forwarding. */
    fwdTypeXMirror = 1,
} egr_mirror_fwd_type_t;

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent0_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL2uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1f
    },
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent1_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL2uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent2_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xd
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent3_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xd
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent4_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xd
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent5_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xd
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent6_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent7_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent8_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent9_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeIpmc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent10_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeIpmc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent11_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL2uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1f
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent12_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent13_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL2uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent14_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent15_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent16_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent17_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeIpmc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent18_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent19_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeIpmc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent20_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeCopy2Cpu
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = REMAPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent21_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent22_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent23_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent24_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent25_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent26_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent27_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent28_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent29_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent30_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent31_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent32_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent33_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x5
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent34_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x5
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent35_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeMirror
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x5
    },
    {
        .name         = REMAP_CTRLs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent36_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x6
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent37_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x6
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_pkt_fwd_type_table_ent38_flds[] = {
    {
        .name         = MIRROR_ON_DROPs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = COPY_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = PKT_TO_CPUs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = INGRESS_MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MIRRORs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = L3_MC_L3ONLYs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3_MCs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = SWITCHs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MULTICASTs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = L3UC_ROUTEDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = FWD_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = fwdTypeL3uc
    },
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x6
    }
};


static bcmint_ltsw_dev_misc_entry_t
tm_pkt_fwd_type_table_entries[] = {
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent0_flds),
        .field_data  = tm_pkt_fwd_type_table_ent0_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent1_flds),
        .field_data  = tm_pkt_fwd_type_table_ent1_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent2_flds),
        .field_data  = tm_pkt_fwd_type_table_ent2_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent3_flds),
        .field_data  = tm_pkt_fwd_type_table_ent3_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent4_flds),
        .field_data  = tm_pkt_fwd_type_table_ent4_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent5_flds),
        .field_data  = tm_pkt_fwd_type_table_ent5_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent6_flds),
        .field_data  = tm_pkt_fwd_type_table_ent6_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent7_flds),
        .field_data  = tm_pkt_fwd_type_table_ent7_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent8_flds),
        .field_data  = tm_pkt_fwd_type_table_ent8_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent9_flds),
        .field_data  = tm_pkt_fwd_type_table_ent9_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent10_flds),
        .field_data  = tm_pkt_fwd_type_table_ent10_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent11_flds),
        .field_data  = tm_pkt_fwd_type_table_ent11_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent12_flds),
        .field_data  = tm_pkt_fwd_type_table_ent12_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent13_flds),
        .field_data  = tm_pkt_fwd_type_table_ent13_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent14_flds),
        .field_data  = tm_pkt_fwd_type_table_ent14_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent15_flds),
        .field_data  = tm_pkt_fwd_type_table_ent15_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent16_flds),
        .field_data  = tm_pkt_fwd_type_table_ent16_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent17_flds),
        .field_data  = tm_pkt_fwd_type_table_ent17_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent18_flds),
        .field_data  = tm_pkt_fwd_type_table_ent18_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent19_flds),
        .field_data  = tm_pkt_fwd_type_table_ent19_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent20_flds),
        .field_data  = tm_pkt_fwd_type_table_ent20_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent21_flds),
        .field_data  = tm_pkt_fwd_type_table_ent21_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent22_flds),
        .field_data  = tm_pkt_fwd_type_table_ent22_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent23_flds),
        .field_data  = tm_pkt_fwd_type_table_ent23_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent24_flds),
        .field_data  = tm_pkt_fwd_type_table_ent24_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent25_flds),
        .field_data  = tm_pkt_fwd_type_table_ent25_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent26_flds),
        .field_data  = tm_pkt_fwd_type_table_ent26_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent27_flds),
        .field_data  = tm_pkt_fwd_type_table_ent27_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent28_flds),
        .field_data  = tm_pkt_fwd_type_table_ent28_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent29_flds),
        .field_data  = tm_pkt_fwd_type_table_ent29_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent30_flds),
        .field_data  = tm_pkt_fwd_type_table_ent30_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent31_flds),
        .field_data  = tm_pkt_fwd_type_table_ent31_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent32_flds),
        .field_data  = tm_pkt_fwd_type_table_ent32_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent33_flds),
        .field_data  = tm_pkt_fwd_type_table_ent33_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent34_flds),
        .field_data  = tm_pkt_fwd_type_table_ent34_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent35_flds),
        .field_data  = tm_pkt_fwd_type_table_ent35_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent36_flds),
        .field_data  = tm_pkt_fwd_type_table_ent36_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent37_flds),
        .field_data  = tm_pkt_fwd_type_table_ent37_flds
    },
    {
        .field_count = COUNTOF(tm_pkt_fwd_type_table_ent38_flds),
        .field_data  = tm_pkt_fwd_type_table_ent38_flds
    }
};

/* Egress object tables selection 0. */
#define SEL0_EGR_L3_NEXT_HOP_1_SEL (1 << 0)
#define SEL0_EGR_L3_OIF_1_SEL      (1 << 1)
#define SEL0_EGR_L3_NEXT_HOP_2_SEL (1 << 2)
#define SEL0_EGR_DVP_SEL           (1 << 3)

/* Egress object tables selection 1. */
#define SEL1_TBL_SEL_RESERVED  (1 << 0)
#define SEL1_EGR_L3_OIF_2_SEL  (1 << 1)
#define SEL1_EGR_L3_TUNNEL_SEL (1 << 2)
#define SEL1_MPLS_VPN_SEL      (1 << 3)

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent0_flds[] = {
    {
        .name  = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags = 0,
        .index = 0,
        .value.scalar = 0x1f
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL |
                        SEL0_EGR_L3_NEXT_HOP_1_SEL | SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL1_MPLS_VPN_SEL
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent1_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL1_TBL_SEL_RESERVED
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent2_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_L3_NEXT_HOP_1_SEL | SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent3_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL |
                        SEL0_EGR_L3_OIF_1_SEL | SEL0_EGR_L3_NEXT_HOP_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent4_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xd
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL |
                        SEL0_EGR_L3_OIF_1_SEL | SEL0_EGR_L3_NEXT_HOP_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent5_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xe
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_3s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_0s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_4s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_1s
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL |
                        SEL0_EGR_L3_OIF_1_SEL | SEL0_EGR_L3_NEXT_HOP_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL1_EGR_L3_TUNNEL_SEL | SEL1_EGR_L3_OIF_2_SEL
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent6_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_2s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_NEXT_HOP_INDEXs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_L3_NEXT_HOP_2_SEL | SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent7_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xb
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_3s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_0s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_4s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_1s
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent8_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xc
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_2s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_2s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_3s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_0s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_4s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_1s
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL | SEL0_EGR_L3_NEXT_HOP_2_SEL |
                        SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent9_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent10_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent11_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x5
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent12_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x6
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_L3_NEXT_HOP_2_SEL | SEL0_EGR_L3_OIF_1_SEL |
                        SEL0_EGR_L3_NEXT_HOP_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent13_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xa
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_2s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_1s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_3s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_0s
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_L3_NEXT_HOP_2_SEL | SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
};

static bcmint_ltsw_dev_misc_field_t
tm_egr_object_update_profile_ent14_flds[] = {
    {
        .name         = TM_EGR_OBJECT_UPDATE_PROFILE_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_0s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_L2_OIFs
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_2s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_2s
    },
    {
        .name         = EGR_OBJ_INDEX_SEL_4s,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SEL_REMAP_OBJECT_INDEX_0s
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = EGR_OBJ_TABLE_SEL_0s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = SEL0_EGR_DVP_SEL |
                        SEL0_EGR_L3_OIF_1_SEL
    },
    {
        .name         = TRUST_EGR_OBJ_TABLE_SEL_1s,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_entry_t
tm_egr_object_update_profile_entries[] = {
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent0_flds),
        .field_data  = tm_egr_object_update_profile_ent0_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent1_flds),
        .field_data  = tm_egr_object_update_profile_ent1_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent2_flds),
        .field_data  = tm_egr_object_update_profile_ent2_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent3_flds),
        .field_data  = tm_egr_object_update_profile_ent3_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent4_flds),
        .field_data  = tm_egr_object_update_profile_ent4_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent5_flds),
        .field_data  = tm_egr_object_update_profile_ent5_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent6_flds),
        .field_data  = tm_egr_object_update_profile_ent6_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent7_flds),
        .field_data  = tm_egr_object_update_profile_ent7_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent8_flds),
        .field_data  = tm_egr_object_update_profile_ent8_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent9_flds),
        .field_data  = tm_egr_object_update_profile_ent9_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent10_flds),
        .field_data  = tm_egr_object_update_profile_ent10_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent11_flds),
        .field_data  = tm_egr_object_update_profile_ent11_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent12_flds),
        .field_data  = tm_egr_object_update_profile_ent12_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent13_flds),
        .field_data  = tm_egr_object_update_profile_ent13_flds
    },
    {
        .field_count = COUNTOF(tm_egr_object_update_profile_ent14_flds),
        .field_data  = tm_egr_object_update_profile_ent14_flds
    },
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent0_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = NO_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent1_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_OIF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent2_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent3_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = DVP_MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent4_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = TTL_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent5_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L3_SAME_INTF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent6_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_SAME_PORT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent7_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SPLIT_HORIZON_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent8_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_DISABLE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent9_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_BLOCK_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent10_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EFP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent11_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_METER_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent12_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_MTU_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent13_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_VXLT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent14_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_LARGEs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent15_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_SMALLs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent16_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = QOS_REMARKING_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent17_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SVP_EQUAL_DVP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent18_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = INVALID_1588_PKTs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent19_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = FLEX_EDITOR_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent20_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EP_RECIRC_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_copytocpu_ent21_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = COPY_TO_CPUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = IFA_MD_DELETE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent0_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = NO_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent1_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_OIF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent2_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent3_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = DVP_MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent4_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = TTL_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent5_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L3_SAME_INTF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent6_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_SAME_PORT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent7_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SPLIT_HORIZON_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent8_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_DISABLE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent9_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_BLOCK_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent10_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EFP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent11_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_METER_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent12_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_MTU_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent13_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_VXLT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent14_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_LARGEs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent15_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_SMALLs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent16_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = QOS_REMARKING_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent17_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SVP_EQUAL_DVP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent18_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = INVALID_1588_PKTs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent19_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = FLEX_EDITOR_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent20_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EP_RECIRC_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_mirror_ent21_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MIRRORs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = IFA_MD_DELETE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent0_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = NO_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent1_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_OIF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent2_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent3_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = DVP_MEMBERSHIP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent4_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = TTL_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent5_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L3_SAME_INTF_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent6_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = L2_SAME_PORT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent7_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SPLIT_HORIZON_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent8_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_DISABLE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent9_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = STG_BLOCK_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent10_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EFP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent11_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_METER_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent12_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_MTU_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent13_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_VXLT_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent14_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_LARGEs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent15_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EGR_CELL_TOO_SMALLs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent16_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = QOS_REMARKING_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent17_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = SVP_EQUAL_DVP_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent18_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = INVALID_1588_PKTs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent19_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = FLEX_EDITOR_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent20_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = EP_RECIRC_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_field_t
mon_egr_drop_mask_profile_bpdu_ent21_flds[] = {
    {
        .name         = FLOWs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = BPDUs
    },
    {
        .name         = EVENT_IDs,
        .flags        = BCMINT_LTSW_DEV_MISC_FLD_SYM,
        .index        = 0,
        .value.str    = IFA_MD_DELETE_DROPs
    },
    {
        .name         = DROP_MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    }
};

static bcmint_ltsw_dev_misc_entry_t
mon_egr_drop_mask_profile_entries[] = {
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent0_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent0_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent1_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent1_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent2_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent2_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent3_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent3_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent4_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent4_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent5_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent5_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent6_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent6_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent7_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent7_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent8_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent8_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent9_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent9_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent10_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent10_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent11_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent11_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent12_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent12_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent13_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent13_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent14_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent14_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent15_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent15_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent16_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent16_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent17_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent17_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent18_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent18_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent19_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent19_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent20_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent20_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_copytocpu_ent21_flds),
        .field_data  = mon_egr_drop_mask_profile_copytocpu_ent21_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent0_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent0_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent1_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent1_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent2_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent2_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent3_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent3_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent4_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent4_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent5_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent5_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent6_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent6_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent7_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent7_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent8_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent8_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent9_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent9_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent10_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent10_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent11_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent11_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent12_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent12_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent13_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent13_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent14_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent14_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent15_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent15_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent16_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent16_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent17_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent17_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent18_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent18_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent19_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent19_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent20_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent20_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_mirror_ent21_flds),
        .field_data  = mon_egr_drop_mask_profile_mirror_ent21_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent0_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent0_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent1_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent1_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent2_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent2_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent3_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent3_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent4_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent4_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent5_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent5_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent6_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent6_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent7_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent7_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent8_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent8_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent9_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent9_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent10_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent10_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent11_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent11_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent12_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent12_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent13_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent13_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent14_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent14_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent15_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent15_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent16_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent16_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent17_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent17_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent18_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent18_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent19_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent19_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent20_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent20_flds
    },
    {
        .field_count = COUNTOF(mon_egr_drop_mask_profile_bpdu_ent21_flds),
        .field_data  = mon_egr_drop_mask_profile_bpdu_ent21_flds
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_1_dst_mac_ent0_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_1_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_1_dst_mac_ent1_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_1_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10000000000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10000000000
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_1_dst_mac_ent2_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_1_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF800000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1005E000000
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_1_dst_mac_ent3_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_1_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFF00000000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x333300000000
    }
};

static bcmint_ltsw_dev_misc_entry_t
l2_header_validation_1_dst_mac_entries[] = {
    {
        .field_count = COUNTOF(l2_header_validation_1_dst_mac_ent0_flds),
        .field_data  = l2_header_validation_1_dst_mac_ent0_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_1_dst_mac_ent1_flds),
        .field_data  = l2_header_validation_1_dst_mac_ent1_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_1_dst_mac_ent2_flds),
        .field_data  = l2_header_validation_1_dst_mac_ent2_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_1_dst_mac_ent3_flds),
        .field_data  = l2_header_validation_1_dst_mac_ent3_flds
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_1_src_mac_ent0_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_1_SRC_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_entry_t
l2_header_validation_1_src_mac_entries[] = {
    {
        .field_count = COUNTOF(l2_header_validation_1_src_mac_ent0_flds),
        .field_data  = l2_header_validation_1_src_mac_ent0_flds
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_2_dst_mac_ent0_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_2_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_2_dst_mac_ent1_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_2_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10000000000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10000000000
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_2_dst_mac_ent2_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_2_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x2
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF800000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x1005E000000
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_2_dst_mac_ent3_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_2_DST_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x3
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x4
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFF00000000
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x333300000000
    }
};

static bcmint_ltsw_dev_misc_entry_t
l2_header_validation_2_dst_mac_entries[] = {
    {
        .field_count = COUNTOF(l2_header_validation_2_dst_mac_ent0_flds),
        .field_data  = l2_header_validation_2_dst_mac_ent0_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_2_dst_mac_ent1_flds),
        .field_data  = l2_header_validation_2_dst_mac_ent1_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_2_dst_mac_ent2_flds),
        .field_data  = l2_header_validation_2_dst_mac_ent2_flds
    },
    {
        .field_count = COUNTOF(l2_header_validation_2_dst_mac_ent3_flds),
        .field_data  = l2_header_validation_2_dst_mac_ent3_flds
    }
};

static bcmint_ltsw_dev_misc_field_t
l2_header_validation_2_src_mac_ent0_flds[] = {
    {
        .name         = L2_HEADER_VALIDATION_2_SRC_MAC_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x8
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFF
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    }
};

static bcmint_ltsw_dev_misc_entry_t
l2_header_validation_2_src_mac_entries[] = {
    {
        .field_count = COUNTOF(l2_header_validation_2_src_mac_ent0_flds),
        .field_data  = l2_header_validation_2_src_mac_ent0_flds
    }
};

/* IPv4 DIP is BC: 255.255.255.255/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10
    }
};

/* IPv4 DIP is MC: 224.0.0.0/4. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xF0000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x20
    }
};

/* IPv4 DIP error: 0.0.0.0/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv4 DIP error: 127.0.0.0/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent3_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 3
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7F000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv4 DIP is a Reserved MC: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent4_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 4
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv4 DIP is a Reserved MC: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv4_ent5_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 5
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_1_dst_ipv4_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent0_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent1_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent2_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent2_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent3_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent3_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent4_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent4_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv4_ent5_flds),
        .field_data  = l3_header_validation_1_dst_ipv4_ent5_flds
    }
};

/* IPv4 SIP error: 0.0.0.0/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv4_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv4 SIP error: 127.0.0.0/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv4_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7F000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv4 SIP error: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv4_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_1_src_ipv4_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv4_ent0_flds),
        .field_data  = l3_header_validation_1_src_ipv4_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv4_ent1_flds),
        .field_data  = l3_header_validation_1_src_ipv4_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv4_ent2_flds),
        .field_data  = l3_header_validation_1_src_ipv4_ent2_flds
    }
};

/* IPv4 DIP is BC: 255.255.255.255/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10
    }
};

/* IPv4 DIP is MC: 224.0.0.0/4. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xF0000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x20
    }
};

/* IPv4 DIP error: 0.0.0.0/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv4 DIP error: 127.0.0.0/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent3_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 3
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7F000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv4 DIP is a Reserved MC: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent4_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 4
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv4 DIP is a Reserved MC: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv4_ent5_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 5
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_2_dst_ipv4_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent0_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent1_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent2_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent2_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent3_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent3_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent4_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent4_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv4_ent5_flds),
        .field_data  = l3_header_validation_2_dst_ipv4_ent5_flds
    }
};

/* IPv4 SIP error: 0.0.0.0/255. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv4_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv4 SIP error: 127.0.0.0/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv4_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x7F000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv4 SIP error: 224.0.0/24. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv4_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV4_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xE0000000
    },
    {
        .name         = MASKs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFF00
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_2_src_ipv4_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_2_src_ipv4_ent0_flds),
        .field_data  = l3_header_validation_2_src_ipv4_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_src_ipv4_ent1_flds),
        .field_data  = l3_header_validation_2_src_ipv4_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_src_ipv4_ent2_flds),
        .field_data  = l3_header_validation_2_src_ipv4_ent2_flds
    }
};

/* IPv6 DIP is BC: 0xFFFFFFFF.../128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10
    }
};

/* IPv6 DIP is MC: 0xFF::/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x20
    }
};

/* IPv6 DIP error: 0::1/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv6 DIP error: 0xFE80::/10. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent3_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 3
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFE80000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFC0000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0X::0/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent4_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 4
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFF0FFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0X::0/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent5_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 5
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFF0FFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF00::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent6_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 6
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF00::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent7_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 7
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF03::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent8_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 8
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF03000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF03::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent9_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 9
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF03000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0F::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent10_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 10
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0F::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_dst_ipv6_ent11_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 11
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_1_dst_ipv6_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent0_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent1_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent2_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent2_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent3_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent3_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent4_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent4_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent5_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent5_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent6_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent6_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent7_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent7_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent8_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent8_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent9_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent9_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent10_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent10_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_dst_ipv6_ent11_flds),
        .field_data  = l3_header_validation_1_dst_ipv6_ent11_flds
    }
};

/* IPv6 SIP error: 0::1/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv6_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv6 SIP error: 0xFE80::/10. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv6_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFE80000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFC0000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv6 SIP error: 0xFF00::/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_1_src_ipv6_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_1_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_1_src_ipv6_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv6_ent0_flds),
        .field_data  = l3_header_validation_1_src_ipv6_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv6_ent1_flds),
        .field_data  = l3_header_validation_1_src_ipv6_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv6_ent2_flds),
        .field_data  = l3_header_validation_1_src_ipv6_ent2_flds
    }
};

/* IPv6 DIP is BC: 0xFFFFFFFF.../128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x10
    }
};

/* IPv6 DIP is MC: 0xFF::/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x20
    }
};

/* IPv6 DIP error: 0::1/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv6 DIP error: 0xFE80::/10. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent3_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 3
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFE80000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFC0000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x80
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0X::0/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent4_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 4
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFF0FFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0X::0/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent5_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 5
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFF0FFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF00::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent6_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 6
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF00::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent7_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 7
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF03::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent8_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 8
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF03000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF03::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent9_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 9
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF03000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0F::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent10_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 10
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x40
    }
};

/* IPv6 DIP is a Reserved MC: 0xFF0F::/16. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_dst_ipv6_ent11_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_DST_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 11
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF0F000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x800
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_2_dst_ipv6_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent0_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent1_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent2_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent2_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent3_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent3_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent4_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent4_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent5_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent5_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent6_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent6_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent7_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent7_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent8_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent8_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent9_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent9_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent10_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent10_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_dst_ipv6_ent11_flds),
        .field_data  = l3_header_validation_2_dst_ipv6_ent11_flds
    }
};

/* IPv6 SIP error: 0::1/128. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv6_ent0_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFFFFFFFFFFFFFFF
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv6 SIP error: 0xFE80::/10. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv6_ent1_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 1
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFE80000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFFC0000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

/* IPv6 SIP error: 0xFF00::/8. */
static bcmint_ltsw_dev_misc_field_t
l3_header_validation_2_src_ipv6_ent2_flds[] = {
    {
        .name         = L3_HEADER_VALIDATION_2_SRC_IPV6_IDs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 2
    },
    {
        .name         = VALUE_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0
    },
    {
        .name         = VALUE_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = MASK_LOWERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x0000000000000000
    },
    {
        .name         = MASK_UPPERs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0xFF00000000000000
    },
    {
        .name         = ADDRESS_TYPEs,
        .flags        = 0,
        .index        = 0,
        .value.scalar = 0x100
    }
};

static bcmint_ltsw_dev_misc_entry_t
l3_header_validation_2_src_ipv6_entries[] = {
    {
        .field_count = COUNTOF(l3_header_validation_2_src_ipv6_ent0_flds),
        .field_data  = l3_header_validation_2_src_ipv6_ent0_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_2_src_ipv6_ent1_flds),
        .field_data  = l3_header_validation_2_src_ipv6_ent1_flds
    },
    {
        .field_count = COUNTOF(l3_header_validation_1_src_ipv6_ent2_flds),
        .field_data  = l3_header_validation_2_src_ipv6_ent2_flds
    }
};

static bcmint_ltsw_dev_misc_lt_t
bcm56780_a0_dna_2_4_13_lt[] = {
    {
        .name        = TM_PKT_FWD_TYPE_TABLEs,
        .entry_count = COUNTOF(tm_pkt_fwd_type_table_entries),
        .entry_data  = tm_pkt_fwd_type_table_entries
    },
    {
        .name        = TM_EGR_OBJECT_UPDATE_PROFILEs,
        .entry_count = COUNTOF(tm_egr_object_update_profile_entries),
        .entry_data  = tm_egr_object_update_profile_entries
    },
    {
        .name        = MON_EGR_DROP_MASK_PROFILEs,
        .entry_count = COUNTOF(mon_egr_drop_mask_profile_entries),
        .entry_data  = mon_egr_drop_mask_profile_entries
    },
    {
        .name        = L2_HEADER_VALIDATION_1_DST_MACs,
        .entry_count = COUNTOF(l2_header_validation_1_dst_mac_entries),
        .entry_data  = l2_header_validation_1_dst_mac_entries
    },
    {
        .name        = L2_HEADER_VALIDATION_1_SRC_MACs,
        .entry_count = COUNTOF(l2_header_validation_1_src_mac_entries),
        .entry_data  = l2_header_validation_1_src_mac_entries
    },
    {
        .name        = L2_HEADER_VALIDATION_2_DST_MACs,
        .entry_count = COUNTOF(l2_header_validation_2_dst_mac_entries),
        .entry_data  = l2_header_validation_2_dst_mac_entries
    },
    {
        .name        = L2_HEADER_VALIDATION_2_SRC_MACs,
        .entry_count = COUNTOF(l2_header_validation_2_src_mac_entries),
        .entry_data  = l2_header_validation_2_src_mac_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_1_DST_IPV4s,
        .entry_count = COUNTOF(l3_header_validation_1_dst_ipv4_entries),
        .entry_data  = l3_header_validation_1_dst_ipv4_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_1_SRC_IPV4s,
        .entry_count = COUNTOF(l3_header_validation_1_src_ipv4_entries),
        .entry_data  = l3_header_validation_1_src_ipv4_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_2_DST_IPV4s,
        .entry_count = COUNTOF(l3_header_validation_2_dst_ipv4_entries),
        .entry_data  = l3_header_validation_2_dst_ipv4_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_2_SRC_IPV4s,
        .entry_count = COUNTOF(l3_header_validation_2_src_ipv4_entries),
        .entry_data  = l3_header_validation_2_src_ipv4_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_1_DST_IPV6s,
        .entry_count = COUNTOF(l3_header_validation_1_dst_ipv6_entries),
        .entry_data  = l3_header_validation_1_dst_ipv6_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_1_SRC_IPV6s,
        .entry_count = COUNTOF(l3_header_validation_1_src_ipv6_entries),
        .entry_data  = l3_header_validation_1_src_ipv6_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_2_DST_IPV6s,
        .entry_count = COUNTOF(l3_header_validation_2_dst_ipv6_entries),
        .entry_data  = l3_header_validation_2_dst_ipv6_entries
    },
    {
        .name        = L3_HEADER_VALIDATION_2_SRC_IPV6s,
        .entry_count = COUNTOF(l3_header_validation_2_src_ipv6_entries),
        .entry_data  = l3_header_validation_2_src_ipv6_entries
    }
};

static bcmint_ltsw_dev_misc_data_t
bcm56780_a0_dna_2_4_13_dev_misc_db = {
    .lt_count = COUNTOF(bcm56780_a0_dna_2_4_13_lt),
    .lt_data  = bcm56780_a0_dna_2_4_13_lt
};
/******************************************************************************
 * Private functions
 */
 /*!
 * \brief Get the database of dev_misc.
 *
 * \param [in]  unit      Unit Number.
 * \param [out] db       dev_misc database.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
bcm56780_a0_dna_2_4_13_dev_misc_db_get(
    int unit,
    bcmint_ltsw_dev_misc_data_t  *db)
{
    *db = bcm56780_a0_dna_2_4_13_dev_misc_db;
    return SHR_E_NONE;
}

/*!
 * \brief dev_misc driver function variable for bcm56780_a0 DNA_2_4_13 device.
 */
static mbcm_ltsw_dev_misc_drv_t bcm56780_a0_dna_2_4_13_ltsw_dev_misc_drv = {
    .dev_misc_db_get = bcm56780_a0_dna_2_4_13_dev_misc_db_get
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_dna_2_4_13_ltsw_dev_misc_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_dev_misc_drv_set(unit, &bcm56780_a0_dna_2_4_13_ltsw_dev_misc_drv));

exit:
    SHR_FUNC_EXIT();
}
