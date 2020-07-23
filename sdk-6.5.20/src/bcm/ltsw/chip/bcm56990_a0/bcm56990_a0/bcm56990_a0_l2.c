/*! \file bcm56990_a0_l2.c
 *
 * BCM56990_A0 L2 subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/xgs/l2_int.h>
#include <bcm_int/ltsw/l2_int.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */
#define MAX_DEPTH_DST_BLOCK_MASK    272

static const
bcmint_l2_sym_map_t dest_type_fld[] = {
    {
        "L2_MC_GRP", 0, BCM_L2_MCAST, '|', L2_FDB_VLANt_L2_MC_GRP_IDf
    },
    {
        "PORT", 1, ~(BCM_L2_TRUNK_MEMBER | BCM_L2_MCAST), '&', L2_FDB_VLANt_MODPORTf
    },
    {
        "TRUNK", 2, BCM_L2_TRUNK_MEMBER, '|', L2_FDB_VLANt_TRUNK_IDf
    },
    /* Last one */
    { NULL, INVALID_FLD_VAL, 0,  'n', INVALID_LT_FID }
};

static const
bcmint_l2_sym_map_t l2_assign_int_pri_fld[] = {
    {
        NULL, 1, BCM_L2_SETPRI, '|', L2_FDB_VLANt_INT_PRIf
    },
    {
        NULL, 0, ~BCM_L2_SETPRI, '&', L2_FDB_VLANt_INT_PRIf
    },
     /* Last one */
    { NULL, INVALID_FLD_VAL, 0, 'n', INVALID_LT_FID }
};

static const
bcmint_l2_fld_map_t l2_host_tbl_entry[] = {
    /* L2_FDB_VLANt_VLAN_IDf */
    {
        L2_ADDR_MEMB(vid), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLANt_MAC_ADDRf */
    {
        L2_ADDR_MEMB(mac), "bcm_mac_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLANt_DEST_TYPEf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        dest_type_fld, INVALID_FLG_VAL, xgs_ltsw_l2_dest_type_trans, 0,
        SELECT_FLAG_DATA | SELECT_FLAG_L2_HOST_TAB_REPL_MODPORT
    },
    /* L2_FDB_VLANt_TRUNK_IDf */
    {
        L2_ADDR_MEMB(tgid), "bcm_trunk_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_tgid_trans, 0, 0
    },
    /* L2_FDB_VLANt_MODPORTf */
    {
        L2_ADDR_MEMB(port), "int",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_dest_port_trans, 0, 0
    },
    /* L2_FDB_VLANt_L2_MC_GRP_IDf */
    {
        L2_ADDR_MEMB(l2mc_group), "bcm_multicast_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_mc_grp_trans, 0, 0
    },
    /* L2_FDB_VLANt_CLASS_IDf */
    {
        INVALID_MEMBER_OFFSET, NULL,
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_class_id_trans, 0,
        SELECT_FLAG_DATA | SELECT_FLAG_L2_HOST_TAB_CLASS_ID
    },
    /* L2_FDB_VLANt_ASSIGN_INT_PRIf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        l2_assign_int_pri_fld, INVALID_FLG_VAL, NULL, 0,
        SELECT_FLAG_DATA
    },
    /* L2_FDB_VLANt_INT_PRIf */
    {
        L2_ADDR_MEMB(cos_dst), "bcm_cos_t",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
    /* L2_FDB_VLANt_DST_DROPf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_DISCARD_DST, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLANt_COPY_TO_CPUf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_COPY_TO_CPU, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLANt_SRC_DROPf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_DISCARD_SRC, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLANt_STATICf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATIC, NULL, 0,
        SELECT_FLAG_DATA | SELECT_FLAG_L2_HOST_TAB_AGE
    },
    /* L2_FDB_VLANt_HIT_SRC_MACf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_SRC_HIT | BCM_L2_HIT, NULL, 0,
        SELECT_FLAG_DATA | SELECT_FLAG_L2_HOST_TAB_AGE | SELECT_FLAG_L2_HOST_TAB_REPL_HIT
    },
    /* L2_FDB_VLANt_HIT_DST_MACf */
    {
        L2_ADDR_MEMB(flags), "uint32_t",
        NULL, BCM_L2_DES_HIT | BCM_L2_HIT, NULL, 0,
        SELECT_FLAG_DATA | SELECT_FLAG_L2_HOST_TAB_AGE | SELECT_FLAG_L2_HOST_TAB_REPL_HIT
    }
};

static const
bcmint_l2_id_map_t l2_host_tab_map[] = {
    {
        .idx = 0,
        .val = L2_FDB_VLANt_VLAN_IDf,
        .map = &l2_host_tbl_entry[0]
    },
    {
        .idx = 1,
        .val = L2_FDB_VLANt_MAC_ADDRf,
        .map = &l2_host_tbl_entry[1]
    },
    {
        .idx = 2,
        .val = L2_FDB_VLANt_DEST_TYPEf,
        .map = &l2_host_tbl_entry[2]
    },
    {
        .idx = 3,
        .val = L2_FDB_VLANt_TRUNK_IDf,
        .map = &l2_host_tbl_entry[3]
    },
    {
        .idx = 4,
        .val = L2_FDB_VLANt_MODPORTf,
        .map = &l2_host_tbl_entry[4]
    },
    {
        .idx = 5,
        .val = L2_FDB_VLANt_L2_MC_GRP_IDf,
        .map = &l2_host_tbl_entry[5]
    },
    {
        .idx = 6,
        .val = L2_FDB_VLANt_CLASS_IDf,
        .map = &l2_host_tbl_entry[6]
    },
    {
        .idx = 7,
        .val = L2_FDB_VLANt_ASSIGN_INT_PRIf,
        .map = &l2_host_tbl_entry[7]
    },
    {
        .idx = 8,
        .val = L2_FDB_VLANt_INT_PRIf,
        .map = &l2_host_tbl_entry[8]
    },
    {
        .idx = 9,
        .val = L2_FDB_VLANt_DST_DROPf,
        .map = &l2_host_tbl_entry[9]
    },
    {
        .idx = 10,
        .val = L2_FDB_VLANt_COPY_TO_CPUf,
        .map = &l2_host_tbl_entry[10]
    },
    {
        .idx = 11,
        .val = L2_FDB_VLANt_SRC_DROPf,
        .map = &l2_host_tbl_entry[11]
    },
    {
        .idx = 12,
        .val = L2_FDB_VLANt_STATICf,
        .map = &l2_host_tbl_entry[12]
    },
    {
        .idx = 13,
        .val = L2_FDB_VLANt_HIT_SRC_MACf,
        .map = &l2_host_tbl_entry[13]
    },
    {
        .idx = 14,
        .val = L2_FDB_VLANt_HIT_DST_MACf,
        .map = &l2_host_tbl_entry[14]
    }
};

static const
bcmint_l2_fld_map_t l2_my_station_entry[] = {
    /* L2_MY_STATIONt_VLAN_IDf */
    {
        L2_STATION_MEMB(vlan), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATIONt_VLAN_ID_MASKf */
    {
        L2_STATION_MEMB(vlan_mask), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATIONt_MAC_ADDRf */
    {
        L2_STATION_MEMB(dst_mac), "bcm_mac_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATIONt_MAC_ADDR_MASKf */
    {
        L2_STATION_MEMB(dst_mac_mask), "bcm_mac_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATION_TRUNKt_TRUNK_IDf */
    {
        L2_STATION_MEMB(src_port), "bcm_port_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_tgid_trans, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATION_TRUNKt_TRUNK_ID_MASKf */
    {
        L2_STATION_MEMB(src_port_mask), "bcm_port_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_tgid_trans, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATION_MODPORTt_MODPORTf */
    {
        L2_STATION_MEMB(src_port), "bcm_port_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_dest_port_trans, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATION_MODPORTt_MODPORT_MASKf */
    {
        L2_STATION_MEMB(src_port_mask), "bcm_port_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_dest_port_trans, 0, SELECT_FLAG_KEY
    },
    /* L2_MY_STATIONt_ENTRY_PRIORITYf */
    {
        L2_STATION_MEMB(priority), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_IPV6_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_IPV6, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_IPV6_MC_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_IPV6_MCAST, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_IPV4_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_IPV4, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_IPV4_MC_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_IPV4_MCAST, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_ARP_RARP_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_ARP_RARP, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_MPLS_TERMINATIONf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_MPLS, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_DROPf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_DISCARD, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_MY_STATIONt_COPY_TO_CPUf */
    {
        L2_STATION_MEMB(flags), "uint32_t",
        NULL, BCM_L2_STATION_COPY_TO_CPU, NULL, 0, SELECT_FLAG_DATA
    }
};

static const
bcmint_l2_id_map_t l2_my_station_map[] = {
    {
        .idx = 0,
        .val = L2_MY_STATIONt_VLAN_IDf,
        .map = &l2_my_station_entry[0]
    },
    {
        .idx = 1,
        .val = L2_MY_STATIONt_VLAN_ID_MASKf,
        .map = &l2_my_station_entry[1]
    },
    {
        .idx = 2,
        .val = L2_MY_STATIONt_MAC_ADDRf,
        .map = &l2_my_station_entry[2]
    },
    {
        .idx = 3,
        .val = L2_MY_STATIONt_MAC_ADDR_MASKf,
        .map = &l2_my_station_entry[3]
    },
    {
        .idx = 4,
        .val = L2_MY_STATIONt_ENTRY_PRIORITYf,
        .map = &l2_my_station_entry[8]
    },
    {
        .idx = 5,
        .val = L2_MY_STATIONt_IPV6_TERMINATIONf,
        .map = &l2_my_station_entry[9]
    },
    {
        .idx = 6,
        .val = L2_MY_STATIONt_IPV6_MC_TERMINATIONf,
        .map = &l2_my_station_entry[10]
    },
    {
        .idx = 7,
        .val = L2_MY_STATIONt_IPV4_TERMINATIONf,
        .map = &l2_my_station_entry[11]
    },
    {
        .idx = 8,
        .val = L2_MY_STATIONt_IPV4_MC_TERMINATIONf,
        .map = &l2_my_station_entry[12]
    },
    {
        .idx = 9,
        .val = L2_MY_STATIONt_ARP_RARP_TERMINATIONf,
        .map = &l2_my_station_entry[13]
    },
    {
        .idx = 10,
        .val = L2_MY_STATIONt_MPLS_TERMINATIONf,
        .map = &l2_my_station_entry[14]
    },
    {
        .idx = 11,
        .val = L2_MY_STATIONt_DROPf,
        .map = &l2_my_station_entry[15]
    },
    {
        .idx = 12,
        .val = L2_MY_STATIONt_COPY_TO_CPUf,
        .map = &l2_my_station_entry[16]
    }
};

static const
bcmint_l2_id_map_t l2_my_station_modport_map[] = {
    {
        .idx = 0,
        .val = L2_MY_STATION_MODPORTt_VLAN_IDf,
        .map = &l2_my_station_entry[0]
    },
    {
        .idx = 1,
        .val = L2_MY_STATION_MODPORTt_VLAN_ID_MASKf,
        .map = &l2_my_station_entry[1]
    },
    {
        .idx = 2,
        .val = L2_MY_STATION_MODPORTt_MAC_ADDRf,
        .map = &l2_my_station_entry[2]
    },
    {
        .idx = 3,
        .val = L2_MY_STATION_MODPORTt_MAC_ADDR_MASKf,
        .map = &l2_my_station_entry[3]
    },
    {
        .idx = 4,
        .val = L2_MY_STATION_MODPORTt_MODPORTf,
        .map = &l2_my_station_entry[6]
    },
    {
        .idx = 5,
        .val = L2_MY_STATION_MODPORTt_MODPORT_MASKf,
        .map = &l2_my_station_entry[7]
    },
    {
        .idx = 6,
        .val = L2_MY_STATION_MODPORTt_ENTRY_PRIORITYf,
        .map = &l2_my_station_entry[8]
    },
    {
        .idx = 7,
        .val = L2_MY_STATION_MODPORTt_IPV6_TERMINATIONf,
        .map = &l2_my_station_entry[9]
    },
    {
        .idx = 8,
        .val = L2_MY_STATION_MODPORTt_IPV6_MC_TERMINATIONf,
        .map = &l2_my_station_entry[10]
    },
    {
        .idx = 9,
        .val = L2_MY_STATION_MODPORTt_IPV4_TERMINATIONf,
        .map = &l2_my_station_entry[11]
    },
    {
        .idx = 10,
        .val = L2_MY_STATION_MODPORTt_IPV4_MC_TERMINATIONf,
        .map = &l2_my_station_entry[12]
    },
    {
        .idx = 11,
        .val = L2_MY_STATION_MODPORTt_ARP_RARP_TERMINATIONf,
        .map = &l2_my_station_entry[13]
    },
    {
        .idx = 12,
        .val = L2_MY_STATION_MODPORTt_MPLS_TERMINATIONf,
        .map = &l2_my_station_entry[14]
    },
    {
        .idx = 13,
        .val = L2_MY_STATION_MODPORTt_DROPf,
        .map = &l2_my_station_entry[15]
    },
    {
        .idx = 14,
        .val = L2_MY_STATION_MODPORTt_COPY_TO_CPUf,
        .map = &l2_my_station_entry[16]
    }
};

static const
bcmint_l2_id_map_t l2_my_station_trunk_map[] = {
    {
        .idx = 0,
        .val = L2_MY_STATION_TRUNKt_VLAN_IDf,
        .map = &l2_my_station_entry[0]
    },
    {
        .idx = 1,
        .val = L2_MY_STATION_TRUNKt_VLAN_ID_MASKf,
        .map = &l2_my_station_entry[1]
    },
    {
        .idx = 2,
        .val = L2_MY_STATION_TRUNKt_MAC_ADDRf,
        .map = &l2_my_station_entry[2]
    },
    {
        .idx = 3,
        .val = L2_MY_STATION_TRUNKt_MAC_ADDR_MASKf,
        .map = &l2_my_station_entry[3]
    },
    {
        .idx = 4,
        .val = L2_MY_STATION_TRUNKt_TRUNK_IDf,
        .map = &l2_my_station_entry[4]
    },
    {
        .idx = 5,
        .val = L2_MY_STATION_TRUNKt_TRUNK_ID_MASKf,
        .map = &l2_my_station_entry[5]
    },
    {
        .idx = 6,
        .val = L2_MY_STATION_TRUNKt_ENTRY_PRIORITYf,
        .map = &l2_my_station_entry[8]
    },
    {
        .idx = 7,
        .val = L2_MY_STATION_TRUNKt_IPV6_TERMINATIONf,
        .map = &l2_my_station_entry[9]
    },
    {
        .idx = 8,
        .val = L2_MY_STATION_TRUNKt_IPV6_MC_TERMINATIONf,
        .map = &l2_my_station_entry[10]
    },
    {
        .idx = 9,
        .val = L2_MY_STATION_TRUNKt_IPV4_TERMINATIONf,
        .map = &l2_my_station_entry[11]
    },
    {
        .idx = 10,
        .val = L2_MY_STATION_TRUNKt_IPV4_MC_TERMINATIONf,
        .map = &l2_my_station_entry[12]
    },
    {
        .idx = 11,
        .val = L2_MY_STATION_TRUNKt_ARP_RARP_TERMINATIONf,
        .map = &l2_my_station_entry[13]
    },
    {
        .idx = 12,
        .val = L2_MY_STATION_TRUNKt_MPLS_TERMINATIONf,
        .map = &l2_my_station_entry[14]
    },
    {
        .idx = 13,
        .val = L2_MY_STATION_TRUNKt_DROPf,
        .map = &l2_my_station_entry[15]
    },
    {
        .idx = 14,
        .val = L2_MY_STATION_TRUNKt_COPY_TO_CPUf,
        .map = &l2_my_station_entry[16]
    }
};

static const
bcmint_l2_sym_map_t assign_int_pri[] = {
    {
        NULL, 1, BCM_L2_CACHE_SETPRI, '|', L2_FDB_VLAN_STATICt_INT_PRIf
    },
    {
        NULL, 0, ~BCM_L2_CACHE_SETPRI, '&', INVALID_LT_FID
    },
    /* Last one */
    { NULL, INVALID_FLD_VAL, 0, 'n', INVALID_LT_FID }
};


static const
bcmint_l2_sym_map_t dest_truck_port[] = {
    {
        NULL, 1, BCM_L2_CACHE_TRUNK, '|', L2_FDB_VLAN_STATICt_TRUNK_IDf
    },
    {
        NULL, 0, ~BCM_L2_CACHE_TRUNK, '&', L2_FDB_VLAN_STATICt_MODPORTf
    },
    /* Last one */
    { NULL, INVALID_FLD_VAL, 0, 'n', INVALID_LT_FID }
};

static const
bcmint_l2_fld_map_t l2_cache_entry[] = {
    /* L2_FDB_VLAN_STATICt_VLAN_IDf */
    {
        L2_CACHE_MEMB(vlan), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLAN_STATICt_VLAN_ID_MASKf */
    {
        L2_CACHE_MEMB(vlan_mask), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLAN_STATICt_MAC_ADDRf */
    {
        L2_CACHE_MEMB(mac), "bcm_mac_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLAN_STATICt_MAC_ADDR_MASKf */
    {
        L2_CACHE_MEMB(mac_mask), "bcm_mac_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_FDB_VLAN_STATICt_IS_TRUNKf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        dest_truck_port, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_TRUNK_IDf */
    {
        L2_CACHE_MEMB(dest_trunk), "bcm_trunk_t",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
    /* L2_FDB_VLAN_STATICt_MODPORTf */
    {
        L2_CACHE_MEMB(dest_port), "bcm_port_t",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
    /* L2_FDB_VLAN_STATICt_SKIP_LEARNINGf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        NULL, BCM_L2_CACHE_LEARN_DISABLE, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_BPDUf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        NULL, BCM_L2_CACHE_BPDU, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_L2_PROTOCOL_PKTf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        NULL, BCM_L2_CACHE_PROTO_PKT, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_CLASS_IDf */
    {
        L2_CACHE_MEMB(lookup_class), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_ASSIGN_INT_PRIf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        assign_int_pri, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_INT_PRIf */
    {
        L2_CACHE_MEMB(prio), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
    /* L2_FDB_VLAN_STATICt_DROPf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        NULL, BCM_L2_CACHE_DISCARD, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_FDB_VLAN_STATICt_COPY_TO_CPUf */
    {
        L2_CACHE_MEMB(flags), "uint32_t",
        NULL, BCM_L2_CACHE_CPU, NULL, 0, SELECT_FLAG_DATA
    },
};

static const
bcmint_l2_id_map_t l2_cache_map[] = {
    {
        .idx = 0,
        .val = L2_FDB_VLAN_STATICt_VLAN_IDf,
        .map = &l2_cache_entry[0]
    },
    {
        .idx = 1,
        .val = L2_FDB_VLAN_STATICt_VLAN_ID_MASKf,
        .map = &l2_cache_entry[1]
    },
    {
        .idx = 2,
        .val = L2_FDB_VLAN_STATICt_MAC_ADDRf,
        .map = &l2_cache_entry[2]
    },
    {
        .idx = 3,
        .val = L2_FDB_VLAN_STATICt_MAC_ADDR_MASKf,
        .map = &l2_cache_entry[3]
    },
    {
        .idx = 4,
        .val = L2_FDB_VLAN_STATICt_IS_TRUNKf,
        .map = &l2_cache_entry[4]
    },
    {
        .idx = 5,
        .val = L2_FDB_VLAN_STATICt_TRUNK_IDf,
        .map = &l2_cache_entry[5]
    },
    {
        .idx = 6,
        .val = L2_FDB_VLAN_STATICt_MODPORTf,
        .map = &l2_cache_entry[6]
    },
    {
        .idx = 7,
        .val = L2_FDB_VLAN_STATICt_SKIP_LEARNINGf,
        .map = &l2_cache_entry[7]
    },
    {
        .idx = 8,
        .val = L2_FDB_VLAN_STATICt_BPDUf,
        .map = &l2_cache_entry[8]
    },
    {
        .idx = 9,
        .val = L2_FDB_VLAN_STATICt_L2_PROTOCOL_PKTf,
        .map = &l2_cache_entry[9]
    },
    {
        .idx = 10,
        .val = L2_FDB_VLAN_STATICt_CLASS_IDf,
        .map = &l2_cache_entry[10]
    },
    {
        .idx = 11,
        .val = L2_FDB_VLAN_STATICt_ASSIGN_INT_PRIf,
        .map = &l2_cache_entry[11]
    },
    {
        .idx = 12,
        .val = L2_FDB_VLAN_STATICt_INT_PRIf,
        .map = &l2_cache_entry[12]
    },
    {
        .idx = 13,
        .val = L2_FDB_VLAN_STATICt_DROPf,
        .map = &l2_cache_entry[13]
    },
    {
        .idx = 14,
        .val = L2_FDB_VLAN_STATICt_COPY_TO_CPUf,
        .map = &l2_cache_entry[14]
    }
};

static const
bcmint_l2_fld_map_t l2_dst_blk_entry[] = {
    /* L2_DST_BLOCKt_L2_DST_BLOCK_IDf */
    {
        L2_DST_BLK(index), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_DST_BLOCKt_DST_BLOCK_MASKf */
    {
        L2_DST_BLK(profile), "bcm_pbmp_t",
        NULL, INVALID_FLG_VAL, xgs_ltsw_l2_dst_blk_profile_trans, MAX_DEPTH_DST_BLOCK_MASK,
        SELECT_FLAG_DATA
    }
};

static const
bcmint_l2_id_map_t l2_dst_blk_map[] = {
    {
        .idx = 0,
        .val = L2_DST_BLOCKt_L2_DST_BLOCK_IDf,
        .map = &l2_dst_blk_entry[0]
    },
    {
        .idx = 1,
        .val = L2_DST_BLOCKt_DST_BLOCK_MASKf,
        .map = &l2_dst_blk_entry[1]
    }
};

static const
bcmint_l2_sym_map_t src_type_sym[] = {
    {
        "PORT", 0, 0, '=', L2_LEARN_DATAt_MODPORTf
    },
    {
        "TRUNK", 1, 1, '=', L2_LEARN_DATAt_TRUNK_IDf
    },
    /* Last one */
    { NULL, INVALID_FLD_VAL, 0, 'n', INVALID_LT_FID }
};

static const
bcmint_l2_fld_map_t l2_learn_entry[] = {
    /* L2_LEARN_DATAt_PIPEf */
    {
        L2_LEARN_NOTIFY(pipe), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_LEARN_DATAt_L2_LEARN_DATA_IDf */
    {
        L2_LEARN_NOTIFY(data_id), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_LEARN_DATAt_VLAN_IDf */
    {
        L2_LEARN_NOTIFY(vid), "bcm_vlan_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_LEARN_DATAt_MAC_ADDRf */
    {
        L2_LEARN_NOTIFY(mac), "uint64_t",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_LEARN_DATAt_SRC_TYPEf */
    {
        L2_LEARN_NOTIFY(src_type), "int",
        src_type_sym, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_LEARN_DATAt_TRUNK_IDf */
    {
        L2_LEARN_NOTIFY(tgid), "bcm_trunk_t",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
    /* L2_LEARN_DATAt_MODPORTf */
    {
        L2_LEARN_NOTIFY(modport), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, 0
    },
};

static const
bcmint_l2_id_map_t l2_learn_map[] = {
    {
        .idx = 0,
        .val = L2_LEARN_DATAt_PIPEf,
        .map = &l2_learn_entry[0]
    },
    {
        .idx = 1,
        .val = L2_LEARN_DATAt_L2_LEARN_DATA_IDf,
        .map = &l2_learn_entry[1]
    },
    {
        .idx = 2,
        .val = L2_LEARN_DATAt_VLAN_IDf,
        .map = &l2_learn_entry[2]
    },
    {
        .idx = 3,
        .val = L2_LEARN_DATAt_MAC_ADDRf,
        .map = &l2_learn_entry[3]
    },
    {
        .idx = 4,
        .val = L2_LEARN_DATAt_SRC_TYPEf,
        .map = &l2_learn_entry[4]
    },
    {
        .idx = 5,
        .val = L2_LEARN_DATAt_TRUNK_IDf,
        .map = &l2_learn_entry[5]
    },
    {
        .idx = 6,
        .val = L2_LEARN_DATAt_MODPORTf,
        .map = &l2_learn_entry[6]
    }
};

static const
bcmint_l2_fld_map_t l2_learn_ctrl_entry[] = {
    /* L2_LEARN_CONTROLt_REPORTf */
    {
        L2_LEARN_CTRL(report), "bool",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    }
};

static const
bcmint_l2_id_map_t l2_learn_ctrl_map[] = {
    {
        .idx = 0,
        .val = L2_LEARN_CONTROLt_REPORTf,
        .map = &l2_learn_ctrl_entry[0]
    }
};

static const
bcmint_l2_fld_map_t l2_opaque_entry[] = {
    /* L2_OPAQUE_TAGt_L2_OPAQUE_TAG_IDf */
    {
        L2_OPAQUE_TAG(opaque_tag_id), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_OPAQUE_TAGt_TAG_SIZEf */
    {
        L2_OPAQUE_TAG(tag_size), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_OPAQUE_TAGt_ETHERTYPEf */
    {
        L2_OPAQUE_TAG(ether_type), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    },
    /* L2_OPAQUE_TAGt_TAG_TYPEf */
    {
        L2_OPAQUE_TAG(tag_type), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_DATA
    }
};

static const
bcmint_l2_id_map_t l2_opaque_map[] = {
    {
        .idx = 0,
        .val = L2_OPAQUE_TAGt_L2_OPAQUE_TAG_IDf,
        .map = &l2_opaque_entry[0]
    },
    {
        .idx = 1,
        .val = L2_OPAQUE_TAGt_TAG_SIZEf,
        .map = &l2_opaque_entry[1]
    },
    {
        .idx = 2,
        .val = L2_OPAQUE_TAGt_ETHERTYPEf,
        .map = &l2_opaque_entry[2]
    },
    {
        .idx = 3,
        .val = L2_OPAQUE_TAGt_TAG_TYPEf,
        .map = &l2_opaque_entry[3]
    }
};

static const
bcmint_l2_fld_map_t l2_learn_override_entry[] = {
    /* L2_LEARN_OVERRIDEt_MAC_LEARN_OVERRIDEf */
    {
        L2_LEARN_OVERRIDE(mac_learn_override), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    },
    /* L2_LEARN_OVERRIDEt_MAC_MOVE_OVERRIDEf */
    {
        L2_LEARN_OVERRIDE(mac_move_override), "int",
        NULL, INVALID_FLG_VAL, NULL, 0, SELECT_FLAG_KEY
    }
};

static const
bcmint_l2_id_map_t l2_learn_override_map[] = {
    {
        .idx = 0,
        .val = L2_LEARN_OVERRIDEt_MAC_LEARN_OVERRIDEf,
        .map = &l2_learn_override_entry[0]
    },
    {
        .idx = 1,
        .val = L2_LEARN_OVERRIDEt_MAC_MOVE_OVERRIDEf,
        .map = &l2_learn_override_entry[1]
    }
};

static const
bcmint_l2_map_info_t bcm56990_l2_lt_tbls_info[XGS_L2_MAX_NUM] = {
    {
        XGS_L2_HOST_TAB_ID, L2_FDB_VLANt,
        L2_FDB_VLANs, L2_FDB_VLANt_FIELD_COUNT,
        l2_host_tab_map, COUNTOF(l2_host_tab_map)
    },
    {
        XGS_L2_MY_STATION_ID, L2_MY_STATIONt,
        L2_MY_STATIONs, L2_MY_STATIONt_FIELD_COUNT,
        l2_my_station_map, COUNTOF(l2_my_station_map)
    },
    {
        XGS_L2_MY_STATION_TRUNK_ID, L2_MY_STATION_TRUNKt,
        L2_MY_STATION_TRUNKs, L2_MY_STATION_TRUNKt_FIELD_COUNT,
        l2_my_station_trunk_map, COUNTOF(l2_my_station_trunk_map)
    },
    {
        XGS_L2_MY_STATION_MODPORT_ID, L2_MY_STATION_MODPORTt,
        L2_MY_STATION_MODPORTs, L2_MY_STATION_MODPORTt_FIELD_COUNT,
        l2_my_station_modport_map, COUNTOF(l2_my_station_modport_map)
    },
    {
        XGS_L2_CACHE_ID, L2_FDB_VLAN_STATICt,
        L2_FDB_VLAN_STATICs, L2_FDB_VLAN_STATICt_FIELD_COUNT,
        l2_cache_map, COUNTOF(l2_cache_map)
    },
    {
        XGS_L2_DST_BLOCK_ID, L2_DST_BLOCKt,
        L2_DST_BLOCKs, L2_DST_BLOCKt_FIELD_COUNT,
        l2_dst_blk_map, COUNTOF(l2_dst_blk_map)
    },
    {
        XGS_L2_LEARN_DATA_ID, L2_LEARN_DATAt,
        L2_LEARN_DATAs, L2_LEARN_DATAt_FIELD_COUNT,
        l2_learn_map, COUNTOF(l2_learn_map)
    },
    {
        XGS_L2_LEARN_CTRL_ID, L2_LEARN_CONTROLt,
        L2_LEARN_CONTROLs, L2_LEARN_CONTROLt_FIELD_COUNT,
        l2_learn_ctrl_map, COUNTOF(l2_learn_ctrl_map)
    },
    {
        XGS_L2_OPAQUE_TAG_ID, L2_OPAQUE_TAGt,
        L2_OPAQUE_TAGs, L2_OPAQUE_TAGt_FIELD_COUNT,
        l2_opaque_map, COUNTOF(l2_opaque_map)
    },
    {
        XGS_L2_LEARN_OVERRIDE_ID, L2_LEARN_OVERRIDEt,
        L2_LEARN_OVERRIDEs, L2_LEARN_OVERRIDEt_FIELD_COUNT,
        l2_learn_override_map, COUNTOF(l2_learn_override_map)
    }
};

/******************************************************************************
 * Private functions
 */


/******************************************************************************
 * Public functions
 */
int
bcm56990_a0_l2_sub_drv_attach(int unit, void *hdl)
{
    return xgs_ltsw_l2_lt_map_register(unit, bcm56990_l2_lt_tbls_info);
}


