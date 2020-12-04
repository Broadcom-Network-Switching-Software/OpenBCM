/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ISM-HASH configuration
 */

#include <sal/types.h>
#include <soc/util.h>
#include <soc/ism_hash.h>
#include <soc/drv.h>

#ifdef BCM_ISM_SUPPORT

/* 
 * Ordered set of banks based upon base entry in memory set.
 * Populated dynamically.
 */
soc_hash_bank_t _soc_ism_shb[SOC_MAX_NUM_DEVICES][_SOC_ISM_MAX_BANKS];

/* Memory sets. Partially auto-generated, partially populated dynamically */
soc_hash_mem_set_t _soc_ism_shms[] = {
    { SOC_MEM_SET_VLAN_XLATE, &_soc_ism_shm[0], 2, NULL, 0,
      &_soc_ism_shk[0], 20, 0, 53 },
    { SOC_MEM_SET_L2_ENTRY, &_soc_ism_shm[2], 2, NULL, 0,
      &_soc_ism_shk[20], 11, 0, 68 },
    { SOC_MEM_SET_L3_ENTRY, &_soc_ism_shm[4], 3, NULL, 0,
      &_soc_ism_shk[31], 9, 0, 279 },
    { SOC_MEM_SET_EP_VLAN_XLATE, &_soc_ism_shm[7], 1, NULL, 0,
      &_soc_ism_shk[40], 4, 0, 43 },
    { SOC_MEM_SET_MPLS, &_soc_ism_shm[8], 2, NULL, 0,
      &_soc_ism_shk[44], 8, 0, 89 }
};

/* Logical memories. Auto-generated */
soc_hash_mem_t _soc_ism_shm[] = {
    { VLAN_XLATEm, &_soc_ism_shmv[0], 3, &_soc_ism_shms[0] },
    { VLAN_XLATE_EXTDm, &_soc_ism_shmv[3], 2, &_soc_ism_shms[0] },
    { L2_ENTRY_1m, &_soc_ism_shmv[5], 6, &_soc_ism_shms[1] },
    { L2_ENTRY_2m, &_soc_ism_shmv[11], 1, &_soc_ism_shms[1] },
    { L3_ENTRY_1m, &_soc_ism_shmv[12], 4, &_soc_ism_shms[2] },
    { L3_ENTRY_2m, &_soc_ism_shmv[16], 3, &_soc_ism_shms[2] },
    { L3_ENTRY_4m, &_soc_ism_shmv[19], 2, &_soc_ism_shms[2] },
    { EGR_VLAN_XLATEm, &_soc_ism_shmv[21], 2, &_soc_ism_shms[3] },
    { MPLS_ENTRYm, &_soc_ism_shmv[23], 1, &_soc_ism_shms[4] },
    { MPLS_ENTRY_EXTDm, &_soc_ism_shmv[24], 4, &_soc_ism_shms[4] },
};

/* Various views. Auto-generated */
soc_hash_mem_view_t _soc_ism_shmv[] = {
    { SOC_MEM_VIEW_VLAN_XLATE_1_XLATE, { XLATE__KEYf, -1 }, 45,
      XLATE__HASH_LSBf, &_soc_ism_shm[0] },
    { SOC_MEM_VIEW_VLAN_XLATE_1_VLAN_MAC, { VLAN_MAC__KEYf, -1 }, 53,
      VLAN_MAC__HASH_LSBf, &_soc_ism_shm[0] },
    { SOC_MEM_VIEW_VLAN_XLATE_1_VIF, { VIF__KEYf, -1 }, 45, VIF__HASH_LSBf, 
      &_soc_ism_shm[0] },
    { SOC_MEM_VIEW_VLAN_XLATE_2_XLATE, { XLATE__KEY_0f, -1 }, 45,
      XLATE__HASH_LSBf, &_soc_ism_shm[1] },
    { SOC_MEM_VIEW_VLAN_XLATE_2_MAC_IP_BIND, { MAC_IP_BIND__KEY_0f, -1 }, 37,
      MAC_IP_BIND__HASH_LSBf, &_soc_ism_shm[1] },
    { SOC_MEM_VIEW_L2_ENTRY_1_L2, { L2__KEYf, -1}, 66, L2__HASH_LSBf,
      &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_1_VLAN, { VLAN__KEYf, -1 }, 28, VLAN__HASH_LSBf,
      &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_1_VIF, { VIF__KEYf, -1 }, 31, VIF__HASH_LSBf,
      &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG,
      { TRILL_NONUC_NETWORK_LONG__KEYf, -1 }, 68, 
        TRILL_NONUC_NETWORK_LONG__HASH_LSBf, &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT, 
      { TRILL_NONUC_NETWORK_SHORT__KEYf, -1 }, 20,
        TRILL_NONUC_NETWORK_SHORT__HASH_LSBf, &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_1_BFD, { BFD__KEYf, -1 }, 37, BFD__HASH_LSBf,
      &_soc_ism_shm[2] },
    { SOC_MEM_VIEW_L2_ENTRY_2_L2, { L2__KEY_0f, -1 }, 66, L2__HASH_LSBf,
      &_soc_ism_shm[3] },
    { SOC_MEM_VIEW_L3_ENTRY_1_IPV4UC, { IPV4UC__KEYf, -1 }, 49,
      IPV4UC__HASH_LSBf, &_soc_ism_shm[4] },
    { SOC_MEM_VIEW_L3_ENTRY_1_LMEP, { LMEP__KEYf, -1 }, 33, LMEP__HASH_LSBf,
      &_soc_ism_shm[4] },
    { SOC_MEM_VIEW_L3_ENTRY_1_RMEP, { RMEP__KEYf, -1 }, 52, RMEP__HASH_LSBf,
      &_soc_ism_shm[4] },
    { SOC_MEM_VIEW_L3_ENTRY_1_TRILL, { TRILL__KEYf, -1 }, 24, TRILL__HASH_LSBf,
      &_soc_ism_shm[4] },
    { SOC_MEM_VIEW_L3_ENTRY_2_IPV4UC, { IPV4UC__KEY_0f, -1, }, 49,
      IPV4UC__HASH_LSBf, &_soc_ism_shm[5] },
    { SOC_MEM_VIEW_L3_ENTRY_2_IPV4MC, { IPV4MC__KEY_0f, -1 }, 95,
      IPV4MC__HASH_LSBf, &_soc_ism_shm[5] },
    { SOC_MEM_VIEW_L3_ENTRY_2_IPV6UC, { IPV6UC__KEY_0f, IPV6UC__KEY_1f, -1 },
      145 /*81+64*/, IPV6UC__HASH_LSBf, &_soc_ism_shm[5] },
    { SOC_MEM_VIEW_L3_ENTRY_4_IPV6UC, { IPV6UC__KEY_0f, IPV6UC__KEY_1f, -1 },
      145 /*81+64*/, IPV6UC__HASH_LSBf, &_soc_ism_shm[6] },
    { SOC_MEM_VIEW_L3_ENTRY_4_IPV6MC, { IPV6MC__KEY_0f, IPV6MC__KEY_1f,
      IPV6MC__KEY_2f, IPV6MC__KEY_3f, -1 }, 279 /*95+64+96+24*/,
      IPV6MC__HASH_LSBf, &_soc_ism_shm[6] },
    { SOC_MEM_VIEW_EP_VLAN_XLATE_1_XLATE, { XLATE__KEYf, -1 }, 43,
      XLATE__HASH_LSBf, &_soc_ism_shm[7] },
    { SOC_MEM_VIEW_EP_VLAN_XLATE_1_MIM_ISID, { MIM_ISID__KEYf, -1 }, 33,
      MIM_ISID__HASH_LSBf, &_soc_ism_shm[7] },
    { SOC_MEM_VIEW_MPLS_ENTRY_1_MPLS, { MPLS__KEYf, -1 }, 41, MPLS__HASH_LSBf,
      &_soc_ism_shm[8] },
    { SOC_MEM_VIEW_MPLS_ENTRY_2_MPLS, { MPLS__KEY_0f, -1 }, 41, MPLS__HASH_LSBf,
      &_soc_ism_shm[9] },
    { SOC_MEM_VIEW_MPLS_ENTRY_2_MIM_NVP, { MIM_NVP__KEY_0f, -1 }, 65,
      MIM_NVP__HASH_LSBf, &_soc_ism_shm[9] },
    { SOC_MEM_VIEW_MPLS_ENTRY_2_MIM_ISID, { MIM_ISID__KEY_0f, -1 }, 89,
      MIM_ISID__HASH_LSBf, &_soc_ism_shm[9] },
    { SOC_MEM_VIEW_MPLS_ENTRY_2_TRILL, { TRILL__KEY_0f, -1 }, 21,
      TRILL__HASH_LSBf, &_soc_ism_shm[9] }
};

/* Different key types. Auto-generated */
soc_hash_key_t _soc_ism_shk[] = {
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID_OVID, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID_OVID, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OVID, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OVID, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OTAG, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OTAG, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_ITAG, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_ITAG, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_PRI_CFI, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_PRI_CFI, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_IVID_OVID_SVP, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_IVID_OVID_SVP, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_XLATE_OVID_SVP, &_soc_ism_shmv[0] },
    { SOC_MEM_KEY_VLAN_XLATE_2_XLATE_OVID_SVP, &_soc_ism_shmv[3] },
    { SOC_MEM_KEY_VLAN_XLATE_1_VLAN_MAC_VLAN_MAC, &_soc_ism_shmv[1] },
    { SOC_MEM_KEY_VLAN_XLATE_1_VIF_VIF, &_soc_ism_shmv[2] },
    { SOC_MEM_KEY_VLAN_XLATE_1_VIF_VIF_VLAN, &_soc_ism_shmv[2] },
    { SOC_MEM_KEY_VLAN_XLATE_2_MAC_IP_BIND_HPAE, &_soc_ism_shmv[3] },    
    { SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE, &_soc_ism_shmv[5] },
    { SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE, &_soc_ism_shmv[11] },
    { SOC_MEM_KEY_L2_ENTRY_1_L2_VFI, &_soc_ism_shmv[5] },
    { SOC_MEM_KEY_L2_ENTRY_2_L2_VFI, &_soc_ism_shmv[11] },
    { SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT, &_soc_ism_shmv[6] },
    { SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT, &_soc_ism_shmv[6] },
    { SOC_MEM_KEY_L2_ENTRY_1_VIF_VIF, &_soc_ism_shmv[7] },
    { SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS, &_soc_ism_shmv[11] },
    { SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG,
      &_soc_ism_shmv[8] },
    {SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT,
      &_soc_ism_shmv[9] },
    { SOC_MEM_KEY_L2_ENTRY_1_BFD_BFD, &_soc_ism_shmv[10] },
    { SOC_MEM_KEY_L3_ENTRY_1_IPV4UC_IPV4_UNICAST, &_soc_ism_shmv[12] },
    { SOC_MEM_KEY_L3_ENTRY_2_IPV4UC_IPV4_UNICAST, &_soc_ism_shmv[16] },
    { SOC_MEM_KEY_L3_ENTRY_2_IPV4MC_IPV4_MULTICAST, &_soc_ism_shmv[17] },
    { SOC_MEM_KEY_L3_ENTRY_4_IPV6UC_IPV6_UNICAST, &_soc_ism_shmv[19] },
    { SOC_MEM_KEY_L3_ENTRY_2_IPV6UC_IPV6_UNICAST, &_soc_ism_shmv[18] },
    { SOC_MEM_KEY_L3_ENTRY_4_IPV6MC_IPV6_MULTICAST, &_soc_ism_shmv[20] },
    { SOC_MEM_KEY_L3_ENTRY_1_LMEP_LMEP, &_soc_ism_shmv[13] },
    { SOC_MEM_KEY_L3_ENTRY_1_RMEP_RMEP, &_soc_ism_shmv[14] },
    { SOC_MEM_KEY_L3_ENTRY_1_TRILL_TRILL, &_soc_ism_shmv[15] },
    { SOC_MEM_KEY_EP_VLAN_XLATE_1_XLATE_VLAN_XLATE, &_soc_ism_shmv[21] },
    { SOC_MEM_KEY_EP_VLAN_XLATE_1_XLATE_VLAN_XLATE_DVP, &_soc_ism_shmv[21] },
    { SOC_MEM_KEY_EP_VLAN_XLATE_1_MIM_ISID_MIM_ISID, &_soc_ism_shmv[22] },
    { SOC_MEM_KEY_EP_VLAN_XLATE_1_MIM_ISID_MIM_ISID_DVP, &_soc_ism_shmv[22] },
    { SOC_MEM_KEY_MPLS_ENTRY_1_MPLS_MPLS_FIRST_PASS, &_soc_ism_shmv[23] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_MPLS_MPLS_FIRST_PASS, &_soc_ism_shmv[24] },
    { SOC_MEM_KEY_MPLS_ENTRY_1_MPLS_MPLS_SECOND_PASS, &_soc_ism_shmv[23] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_MPLS_MPLS_SECOND_PASS, &_soc_ism_shmv[24] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_MIM_NVP_MIM_NVP, &_soc_ism_shmv[25] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_MIM_ISID_MIM_ISID, &_soc_ism_shmv[26] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_MIM_ISID_MIM_ISID_SVP, &_soc_ism_shmv[26] },
    { SOC_MEM_KEY_MPLS_ENTRY_2_TRILL_TRILL, &_soc_ism_shmv[27] }
};

#endif

