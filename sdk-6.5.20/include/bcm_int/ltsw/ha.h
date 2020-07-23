/*! \file ha.h
 *
 * ltsw HA definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_INT_LTSW_HA_H
#define BCM_INT_LTSW_HA_H

#include <sal/sal_types.h>
#include <shr/shr_ha.h>

/*!
 * The HA memory blocks are distinguished by component-ID and sub-component-ID,
 * which then are used to retrieve the same memory content that was held prior
 * to the boot during warm boot procedure.
 */

/*! Component ID of each module in BCM API Emulator (BAE). */
typedef enum {
    BCMI_HA_COMP_ID_L2 = 0,
    BCMI_HA_COMP_ID_TRUNK = 1,
    BCMI_HA_COMP_ID_FLEXCTR = 2,
    BCMI_HA_COMP_ID_ISSU = 3,
    BCMI_HA_COMP_ID_L3_INTF = 4,
    BCMI_HA_COMP_ID_MIRROR = 5,
    BCMI_HA_COMP_ID_MULTICAST = 6,
    BCMI_HA_COMP_ID_STG = 7,
    BCMI_HA_COMP_ID_FLOW = 8,
    BCMI_HA_COMP_ID_VLAN = 9,
    BCMI_HA_COMP_ID_DLB = 10,
    BCMI_HA_COMP_ID_SWITCH = 11,
    BCMI_HA_COMP_ID_ECN = 12,
    BCMI_HA_COMP_ID_STACK = 13,
    BCMI_HA_COMP_ID_COSQ = 14,
    BCMI_HA_COMP_ID_PROPERTY = 15,
    BCMI_HA_COMP_ID_QOS = 16,
    BCMI_HA_COMP_ID_FLOWTRACKER = 17,
    BCMI_HA_COMP_ID_PORT = 18,
    BCMI_HA_COMP_ID_L3_EGR = 19,
    BCMI_HA_COMP_ID_FLEXDIGEST = 20,
    BCMI_HA_COMP_ID_COLLECTOR = 21,
    BCMI_HA_COMP_ID_L3_FIB = 22,
    BCMI_HA_COMP_ID_TELEMETRY = 23,
    BCMI_HA_COMP_ID_L3 = 24,
    BCMI_HA_COMP_ID_VIRTUAL = 25,
    BCMI_HA_COMP_ID_PKTIO = 26,
    BCMI_HA_COMP_ID_FIELD = 27,
    BCMI_HA_COMP_ID_POLICER = 28,
    BCMI_HA_COMP_ID_FLEXSTATE = 29,
    BCMI_HA_COMP_ID_LINK = 30,
    BCMI_HA_COMP_ID_BFD = 31,
    BCMI_HA_COMP_ID_IPMC = 32,
    BCMI_HA_COMP_ID_MPLS = 33,
    BCMI_HA_COMP_ID_UDF = 34,
    BCMI_HA_COMP_ID_LB_HASH = 35,
    BCMI_HA_COMP_ID_SEQ_NUM = 36,
    BCMI_HA_COMP_ID_IFA = 37,
    BCMI_HA_COMP_ID_INT = 38,
    BCMI_HA_COMP_ID_XFLOW_MACSEC = 39,
    BCMI_HA_COMP_ID_MAX /* Support up to 255 */
} bcmi_ha_comp_id_t;

#define BCMI_HA_COMP_ID_STRINGS { \
    "L2", \
    "TRUNK", \
    "FLEXCTR", \
    "ISSU", \
    "L3_INTF", \
    "MIRROR", \
    "MULTICAST", \
    "STG", \
    "FLOW", \
    "VLAN", \
    "DLB", \
    "SWITCH", \
    "ECN", \
    "STACK", \
    "COSQ", \
    "PROPERTY", \
    "QOS", \
    "FLOWTRACKER", \
    "PORT", \
    "L3_EGR", \
    "FLEXDIGEST", \
    "COLLECTOR", \
    "L3_FIB", \
    "TELEMETRY", \
    "L3", \
    "VIRTUAL", \
    "PKTIO", \
    "FIELD", \
    "POLICER", \
    "FLEXSTATE", \
    "LINK", \
    "BFD", \
    "IPMC", \
    "MPLS", \
    "UDF", \
    "LB_HASH", \
    "SEQNUM", \
    "IFA", \
    "INT", \
    "XFLOW_MACSEC", \
    "UNKNOWN" \
}

/*!
 * \brief Allocate HA memory block.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] comp_id Pre-allocated component ID.
 * \param [in] sub_id Internal ID managed by the component.
 * \param [in] blk_id Given string ID that is associated with the block. This
 * ID makes it easier to determine the owner of an HA block.
 * \param [out] length This parameter used as input and output. For input it
 * specifies the desired memory size. For output it is the actual memory block
 * that was allocated. The output value has meaning during warm boot where the
 * requested size might be different from the actual size of the block that
 * was allocated in previous run.
 *
 * \return If success the function returns pointer to the desired memory.
 * Otherwise it returns NULL.
 */
extern void *
bcmi_ltsw_ha_mem_alloc(int unit,
                       shr_ha_mod_id comp_id,
                       shr_ha_sub_id sub_id,
                       const char *blk_id,
                       uint32_t *length);

/*!
 * \brief Free previously allocated HA memory block.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] mem The current block to to free
 *
 * \return SHR_E_NONE success, otherwise failure.
 */
extern int
bcmi_ltsw_ha_mem_free(int unit, void *mem);

/*!
 * \brief Get component name from component ID.
 *
 * \param [in] name Component name.
 *
 * \return Component ID or BCMMGMT_MAX_COMP_ID on error.
 */
extern uint32_t
bcmi_ltsw_ha_comp_id_from_name(const char *name);

/*!
 * \brief Get component ID from component name.
 *
 * \param [in] comp_id Component ID.
 *
 * \return Pointer to component name or NULL on error.
 */
extern const char *
bcmi_ltsw_ha_comp_name_from_id(uint32_t comp_id);

#endif /* BCM_INT_LTSW_HA_H */
