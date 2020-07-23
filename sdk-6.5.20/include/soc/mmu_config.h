/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef _SOC_MMU_CONFIG_H_
#define _SOC_MMU_CONFIG_H_

/*
 * general syntax: buf.object[id].attribute[.port]="string"
 * Some examples of "cell value" string type:
 *   1000b (b for byte)
 *   1000k (k for kilobyte)
 *   1000m (m for megabyte)
 *   1000c (c for cell)
 *
 * objects     attributes        string type and trident register for reference
 * ========================================================================
 * device
 *            headroom           cell value
 *
 * pool#                         # is pool id
 *            size               cell value or %
 *                               This size will affect both ingress and egress
 *                               pool size, it is recommended to use percentage
 *                               since the total pool size calculate for
 *                               ingress and egress are different.
 *            yellow_size        cell value or %
 *                               If the specified value is percentage, it is
 *                               the percentage of the value specified by the
 *                               size attribute
 *                               Use the same value for size, yellow_size, and
 *                               red_size or use 100% for yellow_size and
 *                               red_size to disable color aware.
 *            red_size           cell value or %
 *                               (see notes for yellow_size)
 *
 * port
 *            guarantee          cell value
 *            pool_limit         cell value
 *            pool_resume        cell value
 *            pkt_size           cell value
 *
 * prigroup#                     # is priority group
 *            guarantee          cell value
 *                               specify 0 to disable
 *            headroom           cell value
 *            device_headroom_enable boolean
 *            port_guarantee_enable boolean
 *            port_max_enable    boolean
 *            pool_scale         floating point number value between 1/64 and 8
 *                               Dynamic method will be enabled if this
 *                               attribute is specified.
 *            pool_limit         cell value
 *            pool_resume        cell value
 *            pool_floor         cell value
 *            flow_control_enable boolean
 *
 * queue#                        # is queue id
 * equeue#                       # is extended queue id
 * mqueue#                       # is multicast queue id
 *            guarantee          cell value
 *            pool_scale         floating point number value between 1/64 and 8
 *                               Dynamic method will be enabled if this
 *                               attribute is specified.
 *            pool_limit         cell value
 *                               Specify 0 to indicate disabling queue limit
 *                               and allow to use shared space.
 *            pool_resume        cell value
 *            yellow_limit       cell value for static or % for dynamic
 *                               Use the same value for pool_limit,
 *                               yellow_limit and red_limit to disable color
 *                               aware.
 *                               Use percentage (such as 12.5%, ... 100%) to
 *                               indicate dynamic color limit
 *            red_limit          cell value for static or % for dynamic
 *                               (see notes for yellow_limit)
 *            yellow_resume      cell value
 *            red_resume         cell value
 *
 * ===== mapping =====
 * general syntax: buf.map.from_object.to_object[.port]="string"
 * string is a command separated value list, use 0 if the value is missing
 * for example:
 *     "a,b,c" has same effect as "a,b,c,0,0,0"
 *     "a,,c" has same effect as "a,0,c"
 *
 * buf.map.pri.prigroup[.port]="a,b,c,..."
 *   a is priority group id for priority 0
 *   b is priority group id for priority 1
 *   ...
 *
 * buf.map.prigroup.pool[.port]="a,b,c,..."
 *   a is service pool id for priority group 0
 *   b is service pool id for priority group 1
 *   ...
 *
 * buf.map.queue.pool[.port]="a,b,c,..."
 * buf.map.mqueue.pool[.port]="a,b,c,..."
 *   a is service pool id for queue 0
 *   b is service pool id for queue 1
 *   ...
 */

#define _MMU_CFG_BUF_DYNAMIC_FLAG        (0x80000000)
#define _MMU_CFG_BUF_PERCENT_FLAG        (0x40000000)
/*
*                               Flag indicates use below encoding as the percentage of color limit to
*                               green threshold in dynamic buffer mode:
*                               0: 100% of green threshold
*                               1: 12.5% of green threshold
*                               2: 25% of green threshold
*                               3: 37.5% of green threshold
*                               4: 50% of green threshold
*                               5: 67.5% of green threshold
*                               6: 75% of green threshold
*                               7: 87.5% of green threshold
*/
#define _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1        (0x20000000)

#ifdef BCM_KATANA2_SUPPORT
#define SOC_MMU_CFG_SERVICE_POOL_MAX    4
#define SOC_MMU_CFG_PORT_MAX            170
#define SOC_MMU_CFG_PRI_GROUP_MAX       8
#define SOC_MMU_CFG_INT_PRI_MAX         16
#define SOC_MMU_CFG_QGROUP_MAX          256
#define SOC_MMU_CFG_EQUEUE_MAX          4096 
#define SOC_MMU_CFG_RQE_QUEUE_MAX       12
#define SOC_MMU_CFG_XPE_MAX             4
#else
#define SOC_MMU_CFG_SERVICE_POOL_MAX    4
#if defined(BCM_TOMAHAWK3_SUPPORT)
#define SOC_MMU_CFG_PORT_MAX            160
#else
#define SOC_MMU_CFG_PORT_MAX            136
#endif
#define SOC_MMU_CFG_PRI_GROUP_MAX       8
#define SOC_MMU_CFG_INT_PRI_MAX         16
#define SOC_MMU_CFG_QGROUP_MAX          256
#define SOC_MMU_CFG_EQUEUE_MAX          512
#define SOC_MMU_CFG_RQE_QUEUE_MAX       11
#define SOC_MMU_CFG_XPE_MAX             4
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#define SOC_MMU_CFG_ITM_MAX             2
#define SOC_MMU_CFG_NUM_PROFILES_MAX    8
#define SOC_MMU_CFG_COS_MAX             12
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
typedef struct _soc_mmu_cfg_buf_mapping_profile_s
{
     int inpri_to_prigroup_uc[SOC_MMU_CFG_INT_PRI_MAX];
     int inpri_to_prigroup_mc[SOC_MMU_CFG_INT_PRI_MAX];
     int prigroup_to_servicepool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int prigroup_to_headroompool[SOC_MMU_CFG_PRI_GROUP_MAX];
     int pfcpri_to_prigroup[SOC_MMU_CFG_PRI_GROUP_MAX];
     int valid;
} _soc_mmu_cfg_buf_mapping_profile_t;
#endif 

typedef struct _soc_mmu_cfg_buf_prigroup_s {
    int pool_idx; /* from mapping variable */
    int guarantee;
    int headroom;
    int user_delay;
    int switch_delay;
    int pkt_size;
    int device_headroom_enable;
    int port_guarantee_enable;
    int port_max_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int pool_floor;
    int flow_control_enable;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int lossless;
#endif
} _soc_mmu_cfg_buf_prigroup_t;

typedef struct _soc_mmu_cfg_buf_qgroup_s
{
    int guarantee;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int guarantee_mc;
#endif
    int discard_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int yellow_limit;
    int red_limit;
    int yellow_resume;
    int red_resume;
} _soc_mmu_cfg_buf_qgroup_t;

typedef struct _soc_mmu_cfg_buf_queue_s
{
    int numq;
    int pool_idx; /* from mapping variable */
    int guarantee;
    int discard_enable;
    int pool_scale;
    int pool_limit;
    int pool_resume;
    int color_discard_enable;
    int yellow_limit;
    int red_limit;
    int yellow_resume;
    int red_resume;
    int qgroup_id;
    int qgroup_min_enable;
    int mcq_entry_guarantee;
    int mcq_entry_reserved;
    int mcqe_limit;
    int pkt_guarantee;
    int pkt_pool_scale;
} _soc_mmu_cfg_buf_queue_t;

typedef struct _soc_mmu_cfg_buf_pool_s {
    int size;
    int yellow_size;
    int red_size;
    int total; /* calculated value (not from config variable) */
    int port_guarantee; /* calculated value (not from config variable) */
    int prigroup_guarantee; /* calculated value (not from config variable) */
    int prigroup_headroom; /* calculated value (not from config variable) */
    int queue_guarantee; /* calculated value (not from config variable) */
    int queue_group_guarantee;/* calculated value (not from config variable) */
    int rqe_guarantee; /* calculated value (not from config variable) */
    int asf_reserved; /* calculated value (not from config variable) */
    int mcq_entry_reserved;
    int total_mcq_entry;
    int total_rqe_entry;
} _soc_mmu_cfg_buf_pool_t;

typedef struct _soc_mmu_cfg_buf_port_pool_s {
    int guarantee;
    int pool_limit;
    int pool_resume;
} _soc_mmu_cfg_buf_port_pool_t;

typedef struct _soc_mmu_cfg_buf_port_s
{
    int guarantee;
    int pool_limit;
    int pool_resume;
    int pkt_size;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int pri_to_prigroup_profile_idx;
    int prigroup_profile_idx;
#endif
    _soc_mmu_cfg_buf_prigroup_t prigroups[SOC_MMU_CFG_PRI_GROUP_MAX];
    _soc_mmu_cfg_buf_queue_t *queues;
    int pri_to_prigroup[SOC_MMU_CFG_INT_PRI_MAX]; /* from mapping variable */
    _soc_mmu_cfg_buf_port_pool_t pools[SOC_MMU_CFG_SERVICE_POOL_MAX];
} _soc_mmu_cfg_buf_port_t;

typedef struct _soc_mmu_cfg_buf_mcengine_queue_s {
    int pool_idx;
    int guarantee;
    int pool_scale;
    int pool_limit;
    int discard_enable;
    int yellow_limit;
    int red_limit;
    int ext_guarantee;
    int pkt_guarantee;
    int ext_pool_scale;
    int pkt_pool_scale;
} _soc_mmu_cfg_buf_mcengine_queue_t;

typedef struct soc_th_mmu_rsvd_buffer_s {
    int ing_rsvd_cells;
    int egr_rsvd_cells;
    int asf_rsvd_cells;
    int mcq_entry_rsvd;
} _soc_mmu_rsvd_buffer_t;

typedef struct _soc_mmu_cfg_buf_s
{
    int headroom;
    _soc_mmu_rsvd_buffer_t rsvd_buffers[4];
    _soc_mmu_cfg_buf_pool_t pools[SOC_MMU_CFG_SERVICE_POOL_MAX];
#if defined(BCM_TOMAHAWK3_SUPPORT)
    _soc_mmu_cfg_buf_pool_t pools_itm[SOC_MMU_CFG_ITM_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
#endif
    _soc_mmu_cfg_buf_pool_t pools_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    _soc_mmu_cfg_buf_port_t ports[SOC_MMU_CFG_PORT_MAX];
    _soc_mmu_cfg_buf_qgroup_t qgroups[SOC_MMU_CFG_QGROUP_MAX];
    _soc_mmu_cfg_buf_mcengine_queue_t rqe_queues[SOC_MMU_CFG_RQE_QUEUE_MAX];
    _soc_mmu_cfg_buf_queue_t  equeues[SOC_MMU_CFG_EQUEUE_MAX];
#if defined(BCM_TOMAHAWK3_SUPPORT)
    _soc_mmu_cfg_buf_mapping_profile_t mapprofiles[SOC_MMU_CFG_NUM_PROFILES_MAX];
#endif
    /* equeues must be the last member of this strucutre */
} _soc_mmu_cfg_buf_t;

#define SOC_MMU_CFG_INIT(cfg)   \
            sal_memset(cfg, 0, sizeof(_soc_mmu_cfg_buf_t))

/*
 * flags to specifiy various capablities of the MMU for the device.
 */

/* supports port min guarentee */
#define SOC_MMU_CFG_F_PORT_MIN              0x01

/* supports per port per pool min guarentee */
#define SOC_MMU_CFG_F_PORT_POOL_MIN         0x02
#define SOC_MMU_CFG_F_RQE                   0x04
#define SOC_MMU_CFG_F_EGR_MCQ_ENTRY         0x08

/* Timeout timer for threshold dynamic check: Check current use count againist new threshold*/
#define SOC_MMU_THRESHOLD_DYNAMIC_CHECK_TIMEOUT         250000

typedef struct _soc_mmu_device_info_s {
    uint32  flags;
    uint32  max_pkt_byte;
    uint32  mmu_hdr_byte;
    uint32  jumbo_pkt_size;
    uint32  default_mtu_size;
    uint32  mmu_total_cell;
    uint32  mmu_cell_size;
    uint32  num_pg;
    uint32  num_service_pool;
    uint32  total_mcq_entry;
    uint32  total_rqe_queue_entry;
    uint32  rqe_queue_num;
} _soc_mmu_device_info_t;

typedef enum soc_mmu_pkt_buf_size_e {
    _MMU_PKT_BUF_SIZE_9MB = 0,
    _MMU_PKT_BUF_SIZE_12MB,
    _MMU_PKT_BUF_SIZE_16MB,
    _MMU_PKT_BUF_SIZE_MAX_COUNT
} soc_mmu_pkt_buf_size_t;

typedef enum soc_mmu_params_e {
    _MMU_MAX_PACKET_BYTES = 0,
    _MMU_PHYSICAL_CELLS,
    _MMU_TOTAL_CELLS,
    _MMU_RSVD_CELLS_CFAP,
    _MMU_MAX_PARAMS
} soc_mmu_params_t;

extern uint32 soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_MAX_COUNT][_MMU_MAX_PARAMS];

typedef enum soc_mmu_type_e {
    _MMU_DEFAULT_THRES_TYPE = 0,
    _MMU_QENTRY_BASED,
    _MMU_EXTMEM_BASED,
    _MMU_MAX_THRESHOLD_TYPE
} soc_mmu_type_t;

#define _MMU_CFG_MMU_BYTES_TO_CELLS(byte,cellhdr) (((byte) + (cellhdr) - 1) / (cellhdr))

#define _SOC_MMU_CFG_DEV_POOL_NUM(dcfg) ((dcfg)->num_service_pool)
#define _SOC_MMU_CFG_DEV_PG_NUM(dcfg) ((dcfg)->num_pg)
#define _SOC_MMU_CFG_MMU_TOTAL_CELLS(dcfg)  ((dcfg)->mmu_total_cell)

#if 0
#define _SOC_MMU_CFG_DEV_EXT_NUM_COSQ(dcfg, port) (dcfg)->port_ext_num_cosq[port]
#define _SOC_MMU_CFG_DEV_MC_NUM_COSQ(dcfg, port) (dcfg)->port_mc_num_cosq[port]
#define _SOC_MMU_CFG_DEV_UC_NUM_COSQ(dcfg, port) (dcfg)->port_uc_num_cosq[port]
#endif

void
_soc_mmu_cfg_property_get_cells(int unit, soc_port_t port, const char *obj,
        int index, const char *attr, int allow_dynamic,
        int *setting, int byte_per_cell,
        soc_mmu_type_t threshold_type);

void
_soc_mmu_cfg_buf_read(int unit, _soc_mmu_cfg_buf_t *buf,
                                 _soc_mmu_device_info_t *devcfg);

int
_soc_mmu_cfg_buf_calculate(int unit, _soc_mmu_cfg_buf_t *buf,
                           _soc_mmu_device_info_t *devcfg);

int
_soc_mmu_cfg_buf_check(int unit, _soc_mmu_cfg_buf_t *buf, 
                        _soc_mmu_device_info_t *devcfg);

_soc_mmu_cfg_buf_t* soc_mmu_cfg_alloc(int unit);
void soc_mmu_cfg_free(int unit, _soc_mmu_cfg_buf_t *cfg);

extern int soc_mmu_pool_map_cfg_read(int unit, int *pool_map);
#endif /* _SOC_MMU_CONFIG_H_ */

