/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BCM_INT_XGS4_H__
#define __BCM_INT_XGS4_H__

#include <bcm/udf.h>
#include <bcm/field.h>
#include <bcm/types.h>

#include <soc/field.h>
#include <soc/mcm/memregs.h>

#include<sal/core/sync.h>

#define BCMI_XGS4_UDF_ID_MIN 0x1
#define BCMI_XGS4_UDF_ID_MAX 0xfffe

#define BCMI_XGS4_UDF_PKT_FORMAT_ID_MIN 0x1
#define BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX 0xfffe

#define BCMI_XGS4_UDF_MAX_BYTE_SELECTION 128

#define BCMI_XGS4_UDF_INVALID_PIPE_NUM -1
/* Offset entry flags */
#define BCMI_XGS4_UDF_OFFSET_ENTRY_GROUP    0x1
#define BCMI_XGS4_UDF_OFFSET_ENTRY_MEMBER   0x2
#define BCMI_XGS4_UDF_OFFSET_ENTRY_SHARED   0x4
#define BCMI_XGS4_UDF_OFFSET_ENTRY_HALF     0x8

/* Offset Support flags */
#define BCMI_XGS4_UDF_CTRL_MODULE_INITIALIZED (1 << 1)
#define BCMI_XGS4_UDF_CTRL_OFFSET_ADJUST_IP4  (1 << 2)
#define BCMI_XGS4_UDF_CTRL_OFFSET_ADJUST_GRE  (1 << 3)

#define BCMI_XGS4_UDF_CTRL_TCAM_HIGIG         (1 << 4)
#define BCMI_XGS4_UDF_CTRL_TCAM_VNTAG         (1 << 5)
#define BCMI_XGS4_UDF_CTRL_TCAM_ETAG          (1 << 6)
#define BCMI_XGS4_UDF_CTRL_TCAM_ICNM          (1 << 7)
#define BCMI_XGS4_UDF_CTRL_TCAM_CNTAG         (1 << 8)
#define BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG   (1 << 9)
#define BCMI_XGS4_UDF_CTRL_FLEXHASH           (1 << 10)
#define BCMI_XGS4_UDF_CTRL_POLICER_GROUP      (1 << 11)
#define BCMI_XGS4_UDF_CTRL_UDFHASH            (1 << 12)
#define BCMI_XGS4_UDF_CTRL_RANGE_CHECK        (1 << 13)
#define BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID     (1 << 14)
#define BCMI_XGS4_UDF_CONDITIONAL_CHECK       (1 << 15)
#define BCMI_XGS4_UDF_CTRL_FLEX_COUNTER       (1 << 16)
#define BCMI_XGS4_UDF_CTRL_L4_DST_PORT        (1 << 17)
#define BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE    (1 << 18)
#define BCMI_XGS4_UDF_CTRL_INT_PKT            (1 << 19)
#define BCMI_XGS4_UDF_CTRL_SRC_PORT           (1 << 20)
#define BCMI_XGS4_UDF_CTRL_LB_PKT_TYPE        (1 << 21)


/* TD2 - Offset Base */
#define BCMI_XGS4_UDF_OFFSET_BASE_START_OF_MH        (0)
#define BCMI_XGS4_UDF_OFFSET_BASE_START_OF_L2        (1)
#define BCMI_XGS4_UDF_OFFSET_BASE_START_OF_OUTER_L3  (2)
#define BCMI_XGS4_UDF_OFFSET_BASE_START_OF_INNER_L3  (3)
#define BCMI_XGS4_UDF_OFFSET_BASE_START_OF_L4        (4)

/* Offset entry flags */
#define BCMI_XGS4_UDF_OFFSET_INSTALLED  (1 << 0)
#define BCMI_XGS4_UDF_OFFSET_IFP        (1 << 1)
#define BCMI_XGS4_UDF_OFFSET_VFP        (1 << 2)
#define BCMI_XGS4_UDF_OFFSET_FLEXHASH   (1 << 3)
#define BCMI_XGS4_UDF_OFFSET_POLICER    (1 << 4)
#define BCMI_XGS4_UDF_OFFSET_UDFHASH    (1 << 5)
#define BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG (1 << 6)
#define BCMI_XGS4_UDF_OFFSET_RANGE_CHECK    (1 << 7)

/* TD2 Maximums */
#define MAX_UDF_TCAM_ENTRIES 512
#define MAX_UDF_OFFSET_CHUNKS 16
#define MAX_UDF_ID_LIST (MAX_UDF_OFFSET_CHUNKS*SOC_MAX_NUM_PIPES)

#define UDF_CTRL(u) udf_control[u]
#define UDF_CTRL_FLAGS(u) udf_control[u]->flags

#define BCMI_XGS4_UDF_ID_RUNNING(u) \
    (udf_control[u]->udf_id_running += \
    (udf_control[u]->udf_id_running > BCMI_XGS4_UDF_ID_MAX ? 0 : 1))
#define BCMI_XGS4_UDF_PKT_FORMAT_ID_RUNNING(u) \
    (udf_control[u]->udf_pkt_format_id_running += \
    (udf_control[u]->udf_pkt_format_id_running > BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX ? 0 : 1))

#define BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(u)     udf_control[u]->gran
#define BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(u)  udf_control[u]->noffsets
#define BCMI_XGS4_UDF_CTRL_MAX_UDF_ENTRIES(u) udf_control[u]->nentries

#define BCMI_XGS4_UDF_OFFSET_INFO_SHARED 0x1

#define UDF_LOCK(u) sal_mutex_take((UDF_CTRL(u)->udf_mutex), sal_mutex_FOREVER);
#define UDF_UNLOCK(u) sal_mutex_give(UDF_CTRL(u)->udf_mutex);

/* Offset Chunk Granularity */
#define UDF_OFFSET_GRAN2 0x2
#define UDF_OFFSET_GRAN4 0x4

/* No. of Offset Chunks */
#define UDF_OFFSET_CHUNKS16 0x10
#define UDF_OFFSET_CHUNKS8  0x8

/* No. of bytes parsed in frame */
#define UDF_PACKET_PARSE_LENGTH 0x80

/* Flags of udf used by module */
#define UDF_USED_BY_NONE          0
#define UDF_USED_BY_UDF_MODULE    1
#define UDF_USED_BY_FIELD_MODULE  2

#if defined (BCM_TOMAHAWK3_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT)
#define MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS (0x4)
#endif

/* User input UDF info structure and hardware offset map */
typedef struct bcmi_xgs4_udf_offset_info_s {
    bcm_udf_id_t    udf_id;
    bcm_udf_layer_t layer;
    uint16          start;
    uint16          width;

    uint8 flags;
    uint8 grp_id;         /* First chunk when group of chunks in use     */
    uint32 hw_bmap;       /* HW bitmap : 1 bit for each chunk            */
    uint32 byte_bmap;     /* Byte granular bitmap: 2 bits for each chunk
                           * when gran is 2 and 4 bits for each chunk when
                           * gran is 4
                           */
    uint8 byte_offset;    /* Data offset inside the word                 */
    int num_pkt_formats;  /* Number of packet formats associated         */

    bcm_pbmp_t associated_ports; /* Used to store bitmap provided */
    uint32 associated_pipe; /* used only in pipe local mode */

    struct bcmi_xgs4_udf_offset_info_s *next;
    struct bcmi_xgs4_udf_offset_info_s *prev;
} bcmi_xgs4_udf_offset_info_t;


typedef struct bcmi_xgs4_udf_tcam_info_s {
    bcm_udf_pkt_format_id_t pkt_format_id;    /* Packet format ID          */
    int priority;                             /* priority of the entry     */
    uint32 offset_bmap;                       /* offset chunk bitmap       */

    int hw_idx;                               /* TCAM index                */
    fp_udf_tcam_entry_t hw_buf;               /* UDF tcam buffer           */
    int num_udfs;                             /* Number of UDFs associated */

    int class_id;                             /* class id used by IFP */
    int associated_pipe; /* used only in pipe local mode */

    /* Offset chunk bitmap per pipe */
    uint32 offset_bmap_per_pipe[SOC_MAX_NUM_PIPES];

    /* TCAM index per pipe */
    int hw_idx_per_pipe[SOC_MAX_NUM_PIPES];

    /* Number of UDFs associated per pipe */
    int num_udfs_per_pipe[SOC_MAX_NUM_PIPES];

    /* Associated pipes */
    int associated_pipes[SOC_MAX_NUM_PIPES];

    /* List of udfs associated */
    uint16 udf_id_list[MAX_UDF_ID_LIST];

    /* List of udfs associated - used in delete_all/get_all */
    struct bcmi_xgs4_udf_offset_info_s *offset_info_list[MAX_UDF_OFFSET_CHUNKS];

    struct bcmi_xgs4_udf_tcam_info_s *next;
    struct bcmi_xgs4_udf_tcam_info_s *prev;
} bcmi_xgs4_udf_tcam_info_t;


typedef struct bcmi_xgs4_udf_tcam_entry_s {
    int valid;                              /* Entry Valid/Installed */
    bcmi_xgs4_udf_tcam_info_t   *tcam_info; /* Pointer to tcam info  */
} bcmi_xgs4_udf_tcam_entry_t;

typedef struct bcmi_xgs4_udf_offset_entry_s {
    uint8 flags;            /* BCMI_XGS4_UDF_OFFSET_ENTRY_XXX                 */
    uint8 grp_id;           /* first chunk id in an offset group              */
    uint8 num_udfs;         /* Number of (shared) udfs using the offset chunk */
    uint8 num_pkt_formats;  /* Num of pkt formats currently using             */
    int   associated_pipe;  /* used only in PipeLocal Mode */
} bcmi_xgs4_udf_offset_entry_t;

/*
 * Typedef:
 *     _field_udf_bmp_t
 * Purpose:
 *     Udf chunk bit map for tracking allocated chunks in UDF 1 and 2
 */
typedef struct bcmi_xgs4_udf_bmp_s {
    SHR_BITDCL  *w;
} bcmi_xgs4_udf_bmp_t;

#define _BCMI_UDF_BMP_FREE(bmp)         sal_free((bmp).w)
#define _BCMI_UDF_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _BCMI_UDF_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _BCMI_UDF_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))


typedef struct bcmi_xgs4_udf_ctrl_s {
    soc_mem_t offset_mem;  /* offset memory */
    soc_mem_t tcam_mem;    /* tcam memory   */
    sal_mutex_t udf_mutex; /* UDF lock      */

    uint32 flags;          /* misc. info    */

    uint8 gran;           /* UDF chunk granularity (in bytes) */
    uint8 noffsets;       /* Number of offsets per entry      */
    uint16 nentries;      /* Number of entries in UDF table   */

    uint16 max_udfs;      /* Restrict max udfs (for WB)       */

    uint16 num_udfs;        /* Number of UDFs existing in the system     */
    uint16 num_pkt_formats; /* Number of packet formats installed so far */

    uint16 max_parse_bytes; /* Restrict max byte selection */

    /* UDF & Packet format ID running trackers */
    uint16 udf_id_running;
    uint16 udf_pkt_format_id_running;

    uint32 hw_bmap;   /* HW bitmap : 1 bit for each chunk */
    uint32 byte_bmap; /* Byte granular bitmap: 2 bits for each chunk
                       * when gran is 2 and 4 bits for each chunk when
                       * gran is 4
                       */
    uint32 hw_bmap_per_pipe[SOC_MAX_NUM_PIPES];  /* HW bitmap in pipeLocalMode */
    uint32 byte_bmap_per_pipe[SOC_MAX_NUM_PIPES];/* Byte bitmap in pipeLocalMode */

    uint8 udf_hash_1;       /* udf hash chunk 1 */
    uint8 udf_hash_2;       /* udf hash chunk 2 */

    uint32 udf_used_by_module; /* udf used by module:
                                        0 - used by none module
                                        1 - used by udf module
                                        2 - used by field module */

    /* Pointers to tcam entry arrays used in global mode*/
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_array;

    /* Pointers to tcam entry arrays used in pipe local mode*/
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_array_per_pipe[SOC_MAX_NUM_PIPES];

    /* Pointers to tcam/offset entry arrays */
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array;

    /* Pointers to tcam/offset entry arrays used in pipe local mode*/
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array_per_pipe[SOC_MAX_NUM_PIPES];

    /* Pointers to tcam/offset nodes */
    bcmi_xgs4_udf_offset_info_t *offset_info_head;
    bcmi_xgs4_udf_tcam_info_t *tcam_info_head;

    /* number of udf qualfiers _bcmFieldQualifyData0~15 */
    bcmi_xgs4_udf_bmp_t udf_field_bmp[SOC_MAX_NUM_PIPES];
    int max_chunks;

} bcmi_xgs4_udf_ctrl_t;

/* UDF module initialized state */
extern int _bcm_xgs_udf_init[BCM_MAX_NUM_UNITS];

extern bcmi_xgs4_udf_ctrl_t *udf_control[BCM_MAX_NUM_UNITS];

#define UDF_INIT_CHECK(unit) \
    do {                                        \
        if (udf_control[unit] == NULL) {        \
            return BCM_E_INIT;                  \
        }                                       \
    } while(0)


/* Extern function declarations */
extern int bcmi_xgs4_udf_init(int unit);
extern int bcmi_xgs4_udf_detach(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_udf_scache_sync(int unit);
extern int bcmi_xgs4_udf_wb_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int bcmi_xgs4_udf_tcam_node_get(int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcmi_xgs4_udf_tcam_info_t **cur);

extern int bcmi_xgs4_udf_offset_node_get(int unit,
    bcm_udf_id_t udf_id,
    bcmi_xgs4_udf_offset_info_t **cur);

extern int bcm_xgs4_udf_detach(int unit);

extern int bcm_xgs4_udf_init(int unit);

extern int bcmi_xgs4_udf_create(int unit, bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info, bcm_udf_id_t *udf_id);

extern int bcmi_xgs4_udf_get(int unit,
    bcm_udf_id_t udf_id, bcm_udf_t *udf_info);

extern int bcmi_xgs4_udf_destroy(int unit, bcm_udf_id_t udf_id);

extern int bcmi_xgs4_udf_get_all(int unit, int max,
    bcm_udf_id_t *udf_list, int *actual);

extern int bcmi_xgs4_udf_pkt_format_create(int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format_info,
    bcm_udf_pkt_format_id_t *pkt_format_id);

extern int bcmi_xgs4_udf_pkt_format_destroy(int unit,
    bcm_udf_pkt_format_id_t pkt_format_id);

extern int bcmi_xgs4_udf_pkt_format_info_get(int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format_info);

extern int bcmi_xgs4_udf_pkt_format_add(int unit,
    bcm_udf_id_t udf_id, bcm_udf_pkt_format_id_t pkt_format_id);

extern int bcmi_xgs4_udf_pkt_format_delete(int unit,
    bcm_udf_id_t udf_id, bcm_udf_pkt_format_id_t pkt_format_id);

extern int bcmi_xgs4_udf_pkt_format_delete_all(int unit, bcm_udf_id_t udf_id);

extern int bcmi_xgs4_udf_pkt_format_get(int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max, bcm_udf_id_t *udf_id_list, int *actual);

extern int bcmi_xgs4_udf_pkt_format_get_all(int unit,
    bcm_udf_id_t udf_id, int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list, int *actual);

extern int bcmi_xgs4_udf_hash_config_add(int unit,
    uint32 options,
    bcm_udf_hash_config_t *config);

extern int bcmi_xgs4_udf_hash_config_delete(int unit,
    bcm_udf_hash_config_t *config);

extern int bcmi_xgs4_udf_hash_config_delete_all(int unit);

extern int bcmi_xgs4_udf_hash_config_get(int unit,
    bcm_udf_hash_config_t *config);

extern int bcmi_xgs4_udf_hash_config_get_all(int unit,
    int max,
    bcm_udf_hash_config_t *udf_hash_config_list,
    int *actual);

#if defined (BCM_TOMAHAWK_SUPPORT)
extern int bcmi_xgs4_udf_oper_mode_set(int unit,
    bcm_udf_oper_mode_t mode);

extern int bcmi_xgs4_udf_oper_mode_get(int unit,
    bcm_udf_oper_mode_t *mode);
#endif

#endif /*BCM_XGS4_H*/
