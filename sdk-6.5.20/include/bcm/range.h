/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_RANGE_H__
#define __BCM_RANGE_H__

#include <bcm/types.h>
#include <shared/switch.h>

/* Range checker Types. */
typedef enum bcm_range_type_e {
    bcmRangeTypeL4SrcPort = 0,      /* Check L4 Source port range. */
    bcmRangeTypeL4DstPort = 1,      /* Check L4 Destination port range. */
    bcmRangeTypeOuterVlan = 2,      /* Check Outer Vlan range. */
    bcmRangeTypePacketLength = 3,   /* Check IP payload length range. */
    bcmRangeTypeUdf = 4,            /* Check Udf range. */
    bcmRangeTypeVfi = 5,            /* Check VFI range. */
    bcmRangeTypeIcmpTypeCode = 6,   /* Check ICMP Type Code range. */
    bcmRangeTypeCount = 7           /* Always last. Not a usable value. */
} bcm_range_type_t;

#define BCM_RANGE_TYPE \
{ \
    "L4SrcPort", \
    "L4DstPort", \
    "OuterVlan", \
    "PacketLength", \
    "Udf", \
    "Vfi", \
    "IcmpTypeCode"  \
}

/* bcm_range_oper_mode_e */
typedef enum bcm_range_oper_mode_e {
    bcmRangeOperModeGlobal = 0,     /* Range Checkers Operate in Global Mode
                                       Value. i.e. for a given packet on any
                                       port from any pipe will have the same
                                       range check Result. */
    bcmRangeOperModePipeLocal = 1,  /* Range Checkers Operate in Per Pipe Mode
                                       Value. i.e. for a given packet on ports
                                       from different pipes can have the
                                       different range check Result. */
    bcmRangeOperModeCount = 2       /* Always last. Not a usable value. */
} bcm_range_oper_mode_t;

#define BCM_RANGE_OPER_MODE \
{ \
    "Global", \
    "PipeLocal", \
    "Count"  \
}

/* Opaque handle to a range. */
typedef uint32 bcm_range_t;

/* 
 * Range configuration structure used to create a range with specific
 * attributes.
 */
typedef struct bcm_range_config_s {
    bcm_range_t rid;        /* Range Check Id */
    bcm_range_type_t rtype; /* Range Check Type(bcmRangeTypeXXX) */
    uint32 min;             /* Lower limit of Range inclusive. */
    uint32 max;             /* Upper limit of Range inclusive. */
    uint8 offset;           /* Offset (in bits) for mask to apply before range
                               check comparison. */
    uint8 width;            /* Width(in bits) from offset specified for mask to
                               apply before range check comparison. */
    bcm_udf_id_t udf_id;    /* UDF id created using bcm_udf_create API. */
    bcm_pbmp_t ports;       /* Port bitmap to decide XGS pipe */
} bcm_range_config_t;

/* 
 * Range Module Flags. These flags can be passed to bcm_range_create API
 * via flags parameter.
 */
#define BCM_RANGE_CREATE_WITH_ID    (1 << 0)   

/* 
 * To initialize all members in bcm_range_config_t structure to default
 * values.
 */
extern void bcm_range_config_t_init(
    bcm_range_config_t *range_cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Range checker */
extern int bcm_range_create(
    int unit, 
    int flags, 
    bcm_range_config_t *range_cfg);

/* Fetches the Range object created in the system */
extern int bcm_range_get(
    int unit, 
    bcm_range_config_t *range_cfg);

/* Destroy the range object */
extern int bcm_range_destroy(
    int unit, 
    bcm_range_t rid);

/* Set Operational mode for Range Module */
extern int bcm_range_oper_mode_set(
    int unit, 
    bcm_range_oper_mode_t oper_mode);

/* Get Operational mode of Range module */
extern int bcm_range_oper_mode_get(
    int unit, 
    bcm_range_oper_mode_t *oper_mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Device Range Id's traverse callback function. Customer Applications
 * can create function of this prototype and pass that function pointer
 * to bcm_range_travers API, so that for every valid range id ,  function
 * pointer will be called with some meta data
 */
typedef int (*bcm_range_traverse_cb)(
    int unit, 
    bcm_range_config_t *range_cfg, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all the valid range id's in the system, calling a specified
 * callback function for each one
 */
extern int bcm_range_traverse(
    int unit, 
    bcm_range_traverse_cb callback, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_RANGE_ID_MAX        ((64 * _SHR_SWITCH_MAX_PIPES) - 1) 

/* Bitmap of Range IDs. */
typedef struct bcm_range_id_bmp_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_RANGE_ID_MAX)]; 
} bcm_range_id_bmp_t;

/* Range group configuration structure. */
typedef struct bcm_range_group_config_s {
    uint16 flags;                   /* To hold any specific flags. */
    bcm_pbmp_t ports;               /* Port bitmap for particular pipe 
                                       instance. */
    bcm_range_id_bmp_t range_bmp;   /* Bitmap to hold Range IDs. */
    uint32 range_group_id;          /* Range Group Identifier */
} bcm_range_group_config_t;

#define BCM_RANGE_ID_BMP_ADD(range_bmp, range_id)  SHR_BITSET(((range_bmp).w), (range_id)) 

#define BCM_RANGE_ID_BMP_REMOVE(range_bmp, range_id)  SHR_BITCLR(((range_bmp).w), (range_id)) 

#define BCM_RANGE_ID_BMP_TEST(range_bmp, range_id)  SHR_BITGET(((range_bmp).w), (range_id)) 

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a Range checker Group */
extern int bcm_range_group_create(
    int unit, 
    bcm_range_group_config_t *range_group_config);

/* Delete a Range checker Group */
extern int bcm_range_group_delete(
    int unit, 
    bcm_range_group_config_t *range_group_config);

/* Update a Range checker Group's configuration. */
extern int bcm_range_group_update(
    int unit, 
    bcm_range_group_config_t *range_group_config);

/* Get/Fetch a Range checker Group */
extern int bcm_range_group_get(
    int unit, 
    bcm_range_group_config_t *range_group_config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Initialize all members in bcm_range_group_config_t structure to
 * default values.
 */
extern void bcm_range_group_config_t_init(
    bcm_range_group_config_t *range_group_config);

#endif /* __BCM_RANGE_H__ */
