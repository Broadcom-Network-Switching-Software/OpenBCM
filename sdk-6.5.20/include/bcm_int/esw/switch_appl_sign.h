/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains internal definitions for
 * switch application signature module to the BCM library.
 */

#ifndef _BCM_INT_SWITCH_APPL_SIGN_H_
#define _BCM_INT_SWITCH_APPL_SIGN_H_

#include <bcm/types.h>
#include <bcm/switch.h>

/* Invalid Application signature Id */
#define BCMI_SWITCH_APPL_SIGNATURE_INVALID_ID           0

/* Parser Stage Identifer */
#define BCMI_SWITCH_PARSER1_STAGE2              0x1
#define BCMI_SWITCH_PARSER1_STAGE3              0x2
#define BCMI_SWITCH_PARSER1_STAGE4              0x3
#define BCMI_SWITCH_PARSER2_STAGE2              0x4
#define BCMI_SWITCH_PARSER2_STAGE3              0x5
#define BCMI_SWITCH_PARSER2_STAGE4              0x6

/* Structure for  Parser stage entry */
typedef struct bcmi_switch_appl_sign_parser_stage_entry_s {
    int stage_id;                       /* Parser Stage */
    int key_size;                       /* Key size in words */
    uint32 *hw_key_buf;                 /* Entry Key buffer */
    uint32 *hw_mask_buf;                /* Entry Mask buffer */
    int data_size;                      /* Data size un words */
    uint32 *hw_data_buf;                /* Entry Data buffer */
} bcmi_switch_appl_sign_parser_stage_entry_t;

/* Application sign entry flags */
#define BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER        (1 << 0)
#define BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP          (1 << 1)
#define BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE     (1 << 2)
#define BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED         (1 << 3)

/* Structure for Application signature entry */
typedef struct bcmi_switch_appl_sign_entry_s {
    uint32 flags;                       /* Flags */
    uint32 eid;                         /* Entry Id */
    uint32 prio;                        /* Priority */
    int hw_index;                       /* H/W index */
    uint32 context_id;                  /* Internal context Id */
    int parts;                          /* Number of stage entries */
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry; /* Array of
                                           stage entries */
} bcmi_switch_appl_sign_entry_t;

/* Application signature control */
typedef struct bcmi_switch_appl_sign_control_s {
    bcmi_switch_appl_sign_entry_t **entry_list;     /* Application sign entry list
                                           for outer payload */
    uint8 *context_id_ref;              /* Internal context Id reference
                                           for outer payload */
    bcm_switch_appl_signature_t next_alloc_eid; /* Allocated entry id
                                           sequecence for outer payload */
    bcmi_switch_appl_sign_entry_t **tnl_entry_list; /* Application sign entry list
                                           for inner payload */
    uint8 *tnl_context_id_ref;          /* Internal context Id reference
                                           for inner payload */
    bcm_switch_appl_signature_t tnl_next_alloc_eid; /* Allocated entry Id
                                            sequence for inner payload */
} bcmi_switch_appl_sign_control_t;

/* Structure for Derived/fixed field values */
typedef struct bcmi_switch_appl_sign_entry_flex_field_s {
    uint8 type;             /* 0x0 - Fixed */
                            /* 0x1 - Cancun derived */
                            /* 0x2 - Key field */
                            /* 0x4 - Mask field */
    soc_field_t field;      /* Field */
    char *obj_str;          /* String if cancun */
    uint8 arg;              /* Field string if cancun */
    uint32 value;           /* Value if fixed */
} bcmi_switch_appl_sign_entry_flex_field_t;

/* First application entry id */
#define BCMI_SWITCH_APPL_SIGN_ID_BASE                1

/* Last application entry Id */
#define BCMI_SWITCH_APPL_SIGN_ID_MAX                 63

#define BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES            63


/* Initilize Application signature control */
extern int bcmi_esw_switch_appl_signature_init(int unit);

/* Cleanup Application signature control */
extern int bcmi_esw_switch_appl_signature_detach(int unit);

/* Lock */
extern int bcmi_switch_appl_sign_lock(int unit);

/* Unlock */
extern int bcmi_switch_appl_sign_unlock(int unit);

#define BCMI_SWITCH_APPL_SIGN_LOCK(_u_)                 \
    BCM_IF_ERROR_RETURN(bcmi_switch_appl_sign_lock((_u_)))

#define BCMI_SWITCH_APPL_SIGN_UNLOCK(_u_)                 \
    BCM_IF_ERROR_RETURN(bcmi_switch_appl_sign_unlock((_u_)))

/* Create Application Signature */
extern int bcmi_esw_switch_appl_signature_create(int unit,
        uint32 options,
        bcm_switch_appl_signature_info_t *appl_signature_info,
        bcm_switch_appl_signature_t *appl_signature);

/* Destroy Application Signature */
extern int bcmi_esw_switch_appl_signature_destroy(int unit,
        uint32 options,
        bcm_switch_appl_signature_t appl_signature);

/* Get Application Signature Info */
extern int bcmi_esw_switch_appl_signature_get(int unit,
        uint32 options,
        bcm_switch_appl_signature_t appl_signature,
        bcm_switch_appl_signature_info_t *appl_signature_info);

/* Get all Application Signatures */
extern int bcmi_esw_switch_appl_signature_get_all(int unit,
        uint32 options,
        int size,
        bcm_switch_appl_signature_t *appl_signature_arr,
        int *actual_count);

/* Set Priority to Application siganture */
extern int bcmi_esw_switch_appl_signature_prio_set(int unit,
        uint32 options,
        bcm_switch_appl_signature_t appl_signature,
        uint32 priority);

/* Show Application signature */
extern int bcmi_esw_switch_appl_signature_show(int unit,
        uint32 options,
        bcm_switch_appl_signature_t appl_signature);

#if defined(BCM_WARM_BOOT_SUPPORT)
extern int bcmi_esw_switch_appl_signature_reinit(int unit);

extern int bcmi_esw_switch_appl_signature_scache_sync(int unit);

extern int bcmi_esw_switch_appl_signature_wb_alloc(int unit);
#endif

#endif /* _BCM_INT_SWITCH_APPL_SIGN_H_ */
