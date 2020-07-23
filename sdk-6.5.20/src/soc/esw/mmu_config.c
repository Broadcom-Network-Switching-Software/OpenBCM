/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMAC driver
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/ll.h>
#include <soc/debug.h>
#include <soc/mmu_config.h>

 /* 16 MB Packet Buffer Size */
uint32 soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_MAX_COUNT][_MMU_MAX_PARAMS] = {
    /* _TD2_MMU_PKT_BUF_SIZE_9MB */
    {9416,    /* _TD2_MMU_MAX_PACKET_BYTES */
        53248,    /* _TD2_MMU_PHYSICAL_CELLS : 13 banks * 4096 cells */
        45372,    /* _TD2_MMU_TOTAL_CELLS (9Mbytes) */
        490},     /* _TD2_MMU_RSVD_CELLS_CFAP */

    /* _TD2_MMU_PKT_BUF_SIZE_12MB */
    {9416,    /* _TD2_MMU_MAX_PACKET_BYTES */
        65536,    /* _TD2_MMU_PHYSICAL_CELLS : 16 banks * 4096 cells */
        60495,    /* _TD2_MMU_TOTAL_CELLS 60494.76 cells (12Mbytes) */
        595},     /* _TD2_MMU_RSVD_CELLS_CFAP */

    /* _TD2_MMU_PKT_BUF_SIZE_16MB */
    {9416,     /* _TD2_MMU_MAX_PACKET_BYTES */
        86016,    /* _TD2_MMU_PHYSICAL_CELLS : 21 banks * 4096 bytes*/
        80660,    /* _TD2_MMU_TOTAL_CELLS : 16MB */
        735}      /* _TD2_MMU_RSVD_CELLS_CFAP */
};



#if defined(BCM_TRIUMPH3_SUPPORT)

char soc_mmu_theshold_type[_MMU_MAX_THRESHOLD_TYPE][SOC_PROPERTY_NAME_MAX] =
{
    "",
    spn_PKT,
    spn_EXTMEM
};


STATIC void
_soc_mmu_cfg_property_get(int unit, soc_port_t port, const char *obj,
        int index, const char *attr, int *setting,
        soc_mmu_type_t threshold_type)
{
    char suffix;
    char attr1[SOC_PROPERTY_NAME_MAX];

    if (_MMU_DEFAULT_THRES_TYPE != threshold_type) {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s.%s",
                soc_mmu_theshold_type[threshold_type], attr);
    } else {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s",
                attr);
    }


    if (port < 0) {
        *setting = soc_property_obj_attr_get
            (unit, spn_BUF, obj, index, attr1, 0, &suffix, *setting);
    } else {
        *setting = soc_property_port_obj_attr_get
            (unit, port, spn_BUF, obj, index, attr1, 0, &suffix, *setting);
    }
}

void
_soc_mmu_cfg_property_get_cells(int unit, soc_port_t port, const char *obj,
        int index, const char *attr, int allow_dynamic,
        int *setting, int byte_per_cell,
        soc_mmu_type_t threshold_type)
{
    int val;
    char suffix;

    char attr1[SOC_PROPERTY_NAME_MAX];

    if (_MMU_DEFAULT_THRES_TYPE != threshold_type) {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s.%s",
                soc_mmu_theshold_type[threshold_type], attr);
    } else {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s",
                attr);
    }

    if (*setting & _MMU_CFG_BUF_DYNAMIC_FLAG) {
        suffix =  '%';
        val = *setting & ~_MMU_CFG_BUF_DYNAMIC_FLAG;
        if (*setting & _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1) {
            /* 0:1000 (100%), 1:125 (12.5%), ... 7:875 (87.5%) */
            val = (val == 0) ? 1000 : val * 125;
        } else {
            /* 0:125 (12.5%), ... 7:1000 (100%) */
            val = (val + 1) * 125;
        }
    } else {
        /* scale up by 10 during calculation */
        suffix = '\0';
        val = *setting * 10;
    }

    if (port < 0) {
        val = soc_property_obj_attr_get
            (unit, spn_BUF, obj, index, attr1, 1, &suffix, val);
    } else {
        val = soc_property_port_obj_attr_get
            (unit, port, spn_BUF, obj, index, attr1, 1, &suffix, val);
    }

    if (val < 0) { /* treat negative number as zero */
        val = 0;
    }

    if (allow_dynamic && suffix == '%') {
        if (*setting & _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1) {
            /* 0:1000 (100%), 1:125 (12.5%), ... 7:875 (87.5%) */
            val = (val > 875) ? 0 : ((val == 0) ? 1 : (val + 125 -1) / 125);
            val |= _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
        } else {
            /* 0:125 (12.5%), ... 7:1000 (100%) */
            val = val > 1000 ? 7 : (val - 1) / 125;
        }
        val |= _MMU_CFG_BUF_DYNAMIC_FLAG;
    } else {
        /* scale down by 10 after calculation */
        val /= 10;
        switch (suffix) {
        case 'B': /* byte */
        case 'b':
            val = _MMU_CFG_MMU_BYTES_TO_CELLS(val, byte_per_cell);
            break;
        case 'K': /* kilobyte */
        case 'k':
            val = _MMU_CFG_MMU_BYTES_TO_CELLS(val * 1024, byte_per_cell);
            break;
        case 'M': /* megabyte */
        case 'm':
            val = _MMU_CFG_MMU_BYTES_TO_CELLS(val * 1048576, byte_per_cell);
            break;
        default:
            break;
        }
    }

    *setting = val;
}

STATIC void
_soc_mmu_cfg_property_get_percentage_x100(int unit, soc_port_t port,
                                          const char *obj, int index,
                                          const char *attr, int *setting)
{
    int val;
    char suffix;

    if (*setting & _MMU_CFG_BUF_PERCENT_FLAG) {
        suffix =  '%';
        val = *setting & ~_MMU_CFG_BUF_PERCENT_FLAG;
    } else {
        suffix =  '\0';
        val = *setting;
    }

    if (port < 0) {
        val = soc_property_obj_attr_get
            (unit, spn_BUF, obj, index, attr, 2, &suffix, val);
    } else {
        val = soc_property_port_obj_attr_get
            (unit, port, spn_BUF, obj, index, attr, 2, &suffix, val);
    }

    if (suffix == '%') {
        if (val < 0) { /* treat negative number as zero */
            val = 0;
        } else if (val > 10000) { /* treat number larger than 100% as 100% */
            val = 10000;
        }
        *setting = val | _MMU_CFG_BUF_PERCENT_FLAG;
    } else {
        *setting = val;
    }
}

STATIC void
_soc_mmu_cfg_property_get_scale(int unit, soc_port_t port,
        const char *obj, int index,
        const char *attr, int *setting ,
        soc_mmu_type_t threshold_type)
{
    int val, alpha, index_max = 9;
    char suffix;
    char attr1[SOC_PROPERTY_NAME_MAX];

    if (_MMU_DEFAULT_THRES_TYPE != threshold_type) {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s.%s",
                soc_mmu_theshold_type[threshold_type], attr);
    } else {
        sal_snprintf(attr1, SOC_PROPERTY_NAME_MAX, "%s",
                attr);
    }


    alpha = 156250;  /* 1/64 = 0.015625, scale up by 10000000 (2**7) */
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit) ||
        SOC_IS_TRIDENT3X(unit)) {
        alpha /= 2; /* start from 1/128. */
        index_max = 10;
    }
    val = *setting < 0 ? -1 : alpha << *setting;

    if (port < 0) {
        val = soc_property_obj_attr_get
            (unit, spn_BUF, obj, index, attr1, 7, &suffix, val);
    } else {
        val = soc_property_port_obj_attr_get
            (unit, port, spn_BUF, obj, index, attr1, 7, &suffix, val);
    }

    if (val < 0) { /* ignore negative number */
        *setting = -1;
    } else {
        for (*setting = 0; *setting < index_max; (*setting)++) {
            if (val <= (alpha << *setting)) {
                break;
            }
        }
    }
}

void
_soc_mmu_cfg_buf_read(int unit, _soc_mmu_cfg_buf_t *buf,
                                 _soc_mmu_device_info_t *devcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    _soc_mmu_cfg_buf_mapping_profile_t *buf_mapprofile;
#endif
#if defined(BCM_KATANA2_SUPPORT)
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
#endif
    int port, idx, count;
    _soc_mmu_cfg_buf_qgroup_t *queue_group;
    char name[80];
    int values[64];

    si = &SOC_INFO(unit);

    _soc_mmu_cfg_property_get_cells
        (unit, -1, spn_DEVICE, -1, spn_HEADROOM, FALSE, &buf->headroom,
         devcfg->mmu_cell_size, _MMU_DEFAULT_THRES_TYPE);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        buf_pool = &buf->pools[idx];

        _soc_mmu_cfg_property_get_percentage_x100
            (unit, -1, spn_POOL, idx, spn_SIZE, &buf_pool->size);

        _soc_mmu_cfg_property_get_percentage_x100
            (unit, -1, spn_POOL, idx, spn_YELLOW_SIZE, &buf_pool->yellow_size);

        _soc_mmu_cfg_property_get_percentage_x100
            (unit, -1, spn_POOL, idx, spn_RED_SIZE, &buf_pool->red_size);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    for (idx = 0; idx < SOC_MMU_CFG_NUM_PROFILES_MAX; idx++) {
        buf_mapprofile = &buf->mapprofiles[idx];

        _soc_mmu_cfg_property_get
            (unit, -1, spn_MAPPROFILE, idx, spn_PROFILE_VALID,
             &buf_mapprofile->valid,  _MMU_DEFAULT_THRES_TYPE);


        /* internal priority to priority group mapping for UC*/
        sal_sprintf(name, "%s.%s%d.%s",
                    spn_BUF, spn_MAPPROFILE, idx, spn_INPUT_PRI_TO_PRIORITY_GROUP_UC);
        (void)soc_property_get_csv(unit, name, 16,
                                        buf_mapprofile->inpri_to_prigroup_uc);

        /* internal priority to priority group mapping for MC*/
        sal_sprintf(name, "%s.%s%d.%s",
                    spn_BUF, spn_MAPPROFILE, idx, spn_INPUT_PRI_TO_PRIORITY_GROUP_MC);
        (void)soc_property_get_csv(unit, name, 16,
                                        buf_mapprofile->inpri_to_prigroup_mc);

        /* priority group to service pool mapping */
        sal_sprintf(name, "%s.%s%d.%s",
                    spn_BUF, spn_MAPPROFILE, idx, spn_PRIORITY_GROUP_TO_SERVICE_POOL);
        (void)soc_property_get_csv(unit, name, 8,
                                        buf_mapprofile->prigroup_to_servicepool);

        /* priority group to headroom pool mapping */
        sal_sprintf(name, "%s.%s%d.%s",
                    spn_BUF, spn_MAPPROFILE, idx, spn_PRIORITY_GROUP_TO_HEADROOM_POOL);
        (void)soc_property_get_csv(unit, name, 8,
                                        buf_mapprofile->prigroup_to_headroompool);

        /* pfc class to priority group mapping */
        sal_sprintf(name, "%s.%s%d.%s",
                    spn_BUF, spn_MAPPROFILE, idx, spn_PFCCLASS_TO_PRIORITY_GROUP);
        (void)soc_property_get_csv(unit, name, 8,
                                        buf_mapprofile->pfcpri_to_prigroup);
    }
#endif

    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        queue_group = &buf->qgroups[idx];

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_GUARANTEE, FALSE,
             &queue_group->guarantee, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

#if defined(BCM_TOMAHAWK3_SUPPORT)
        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_GUARANTEE_MC, FALSE,
             &queue_group->guarantee_mc, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);
#endif

        _soc_mmu_cfg_property_get
            (unit, -1, spn_QGROUP, idx, spn_DISCARD_ENABLE,
             &queue_group->discard_enable,  _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_scale
            (unit, -1, spn_QGROUP, idx, spn_POOL_SCALE,
             &queue_group->pool_scale,  _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_POOL_LIMIT, FALSE,
             &queue_group->pool_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_POOL_RESUME, FALSE,
             &queue_group->pool_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_YELLOW_LIMIT, TRUE,
             &queue_group->yellow_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_RED_LIMIT, TRUE,
             &queue_group->red_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_YELLOW_RESUME, FALSE,
             &queue_group->yellow_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_QGROUP, idx, spn_RED_RESUME, FALSE,
             &queue_group->red_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

#if defined(BCM_TOMAHAWK3_SUPPORT)
        _soc_mmu_cfg_property_get
            (unit, -1, spn_PORT, port, spn_INPUT_PRI_TO_PRIORITY_GROUP_PROFILE_IDX,
             &buf_port->pri_to_prigroup_profile_idx,  _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get
            (unit, -1, spn_PORT, port, spn_PRIORITY_GROUP_PROFILE_IDX,
             &buf_port->prigroup_profile_idx,  _MMU_DEFAULT_THRES_TYPE);
#endif

        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            buf_port_pool = &buf_port->pools[idx];
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_INGPORTPOOL, idx, spn_GUARANTEE, FALSE,
                 &buf_port_pool->guarantee, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_INGPORTPOOL, idx, spn_POOL_LIMIT, FALSE,
                 &buf_port_pool->pool_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_INGPORTPOOL, idx, spn_POOL_RESUME, FALSE,
                 &buf_port_pool->pool_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }

        /* priority group */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_GUARANTEE, FALSE,
                 &buf_prigroup->guarantee, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_HEADROOM, FALSE,
                 &buf_prigroup->headroom, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

#if defined(BCM_TOMAHAWK3_SUPPORT)
            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_PRIORITY_GROUP_LOSSLESS,
                 &buf_prigroup->lossless,
                 _MMU_DEFAULT_THRES_TYPE);
#endif

            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_USER_DELAY,
                 &buf_prigroup->user_delay,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_SWITCH_DELAY,
                 &buf_prigroup->switch_delay,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_PKT_SIZE,
                 &buf_prigroup->pkt_size, 1 );

            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_DEVICE_HEADROOM_ENABLE,
                 &buf_prigroup->device_headroom_enable,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_scale
                (unit, port, spn_PRIGROUP, idx, spn_POOL_SCALE,
                 &buf_prigroup->pool_scale ,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_POOL_LIMIT, FALSE,
                 &buf_prigroup->pool_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_POOL_RESUME, FALSE,
                 &buf_prigroup->pool_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_POOL_FLOOR, FALSE,
                 &buf_prigroup->pool_floor, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_PRIGROUP, idx, spn_FLOW_CONTROL_ENABLE,
                 &buf_prigroup->flow_control_enable,
                 _MMU_DEFAULT_THRES_TYPE);
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            /* In Tomahawk3, the per-port queues are organized as unicast
               queues followed by multicast queues */
            if (SOC_IS_TOMAHAWK3(unit)) {
                buf_queue = &buf_port->queues[si->port_num_uc_cosq[port] + idx];
            } else {
                buf_queue = &buf_port->queues[idx];
            }
#else
            buf_queue = &buf_port->queues[idx];
#endif

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_GUARANTEE, FALSE,
                 &buf_queue->guarantee, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_MQUEUE, idx, spn_DISCARD_ENABLE,
                 &buf_queue->discard_enable,  _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_scale
                (unit, port, spn_MQUEUE, idx, spn_POOL_SCALE,
                 &buf_queue->pool_scale,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_POOL_LIMIT, FALSE,
                 &buf_queue->pool_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_POOL_RESUME, FALSE,
                 &buf_queue->pool_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_MQUEUE, idx, spn_COLOR_DISCARD_ENABLE,
                 &buf_queue->color_discard_enable,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_RED_LIMIT, TRUE,
                 &buf_queue->red_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

             _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_YELLOW_LIMIT, TRUE,
                 &buf_queue->yellow_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);


            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_RED_RESUME, TRUE,
                 &buf_queue->red_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_YELLOW_RESUME, TRUE,
                 &buf_queue->yellow_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);


            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_POOL, FALSE,
                 &buf_queue->pool_idx, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            /* In Tomahawk3, the per-port queues are organized as unicast
               queues followed by multicast queues */
            if (SOC_IS_TOMAHAWK3(unit)) {
                buf_queue = &buf_port->queues[idx];
            } else {
                buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            }
#else
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
#endif

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_GUARANTEE, FALSE,
                 &buf_queue->guarantee, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            if (SOC_IS_KATANA2(unit)) {
                _soc_mmu_cfg_property_get_cells
                    (unit, port, spn_QUEUE, idx, spn_GUARANTEE, FALSE,
                     &buf_queue->pkt_guarantee, devcfg->mmu_cell_size,
                     _MMU_QENTRY_BASED);
            }


            _soc_mmu_cfg_property_get
                (unit, port, spn_QUEUE, idx, spn_DISCARD_ENABLE,
                 &buf_queue->discard_enable,  _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_POOL, FALSE,
                 &buf_queue->pool_idx, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_scale
                (unit, port, spn_QUEUE, idx, spn_POOL_SCALE,
                 &buf_queue->pool_scale,
                 _MMU_DEFAULT_THRES_TYPE);

            if (SOC_IS_KATANA2(unit)) {
                _soc_mmu_cfg_property_get_scale
                    (unit, port, spn_QUEUE, idx, spn_POOL_SCALE,
                     &buf_queue->pkt_pool_scale,
                  _MMU_QENTRY_BASED);
            }



            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_POOL_LIMIT, FALSE,
                 &buf_queue->pool_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_POOL_RESUME, FALSE,
                 &buf_queue->pool_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_QUEUE, idx, spn_COLOR_DISCARD_ENABLE,
                 &buf_queue->color_discard_enable,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_YELLOW_LIMIT, TRUE,
                 &buf_queue->yellow_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_RED_LIMIT, TRUE,
                 &buf_queue->red_limit, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_YELLOW_RESUME, FALSE,
                 &buf_queue->yellow_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_RED_RESUME, FALSE,
                 &buf_queue->red_resume, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get
                (unit, port, spn_QUEUE, idx, spn_QGROUP_ID,
                 &buf_queue->qgroup_id,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_QGROUP_GUARANTEE_ENABLE, FALSE,
                 &buf_queue->qgroup_min_enable, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }

        /* internal priority to priority group mapping */
        sal_sprintf(name, "%s.%s.%s.%s",
                    spn_BUF, spn_MAP, spn_PRI, spn_PRIGROUP);
        (void)soc_property_port_get_csv(unit, port, name, 16,
                                        buf_port->pri_to_prigroup);

        /* priority group to pool mapping */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            values[idx] = buf_port->prigroups[idx].pool_idx;
        }

        sal_sprintf(name, "%s.%s.%s.%s",
                    spn_BUF, spn_MAP, spn_PRIGROUP, spn_POOL);
        count = soc_property_port_get_csv(unit, port, name,
                                _SOC_MMU_CFG_DEV_PG_NUM(devcfg), values);
        for (idx = 0; idx < count; idx++) {
            buf_port->prigroups[idx].pool_idx = values[idx];
        }

        /* regular unicast queue to pool mapping */
        count = si->port_num_uc_cosq[port];
        if (count > 0) {
            buf_queue = SOC_IS_TOMAHAWK3(unit) ?
                            &buf_port->queues[0] :
                            &buf_port->queues[si->port_num_cosq[port]];
            for (idx = 0; idx < count; idx++) {
                values[idx] = buf_queue[idx].pool_idx;
            }
            sal_sprintf(name, "%s.%s.%s.%s",
                        spn_BUF, spn_MAP, spn_QUEUE, spn_POOL);
            (void)soc_property_port_get_csv(unit, port, name, count,
                                            values);
            for (idx = 0; idx < count; idx++) {
                buf_queue[idx].pool_idx = values[idx];
            }
        }

        /* multicast queue to pool mapping */
        count = si->port_num_cosq[port];
        if (count > 0) {
            buf_queue = SOC_IS_TOMAHAWK3(unit) ?
                            &buf_port->queues[si->port_num_cosq[port]] :
                            &buf_port->queues[0];
            for (idx = 0; idx < count; idx++) {
                values[idx] = buf_queue[idx].pool_idx;
            }
            sal_sprintf(name, "%s.%s.%s.%s",
                        spn_BUF, spn_MAP, spn_MQUEUE, spn_POOL);
            count = soc_property_port_get_csv(unit, port, name, count, values);
            for (idx = 0; idx < count; idx++) {
                buf_queue[idx].pool_idx = values[idx];
            }
        }
    }

    /* virtual queue profiles */
    for (idx = 0; idx < SOC_MMU_CFG_EQUEUE_MAX; idx++) {
        buf_queue = &buf->equeues[idx];

       _soc_mmu_cfg_property_get
            (unit, -1, spn_EQUEUE, idx, spn_NUMQ, &buf_queue->numq,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_GUARANTEE, FALSE,
             &buf_queue->guarantee, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get
            (unit, -1, spn_EQUEUE, idx, spn_DISCARD_ENABLE,
             &buf_queue->discard_enable,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_POOL, FALSE,
             &buf_queue->pool_idx, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_scale
            (unit, -1, spn_EQUEUE, idx, spn_POOL_SCALE,
             &buf_queue->pool_scale,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_POOL_LIMIT, FALSE,
             &buf_queue->pool_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_POOL_RESUME, FALSE,
             &buf_queue->pool_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get
            (unit, -1, spn_EQUEUE, idx, spn_COLOR_DISCARD_ENABLE,
             &buf_queue->color_discard_enable,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_YELLOW_LIMIT, TRUE,
             &buf_queue->yellow_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_RED_LIMIT, TRUE,
             &buf_queue->red_limit, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_YELLOW_RESUME, FALSE,
             &buf_queue->yellow_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_RED_RESUME, FALSE,
             &buf_queue->red_resume, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get
            (unit, -1, spn_EQUEUE, idx, spn_QGROUP_ID,
             &buf_queue->qgroup_id,
             _MMU_DEFAULT_THRES_TYPE);

        _soc_mmu_cfg_property_get_cells
            (unit, -1, spn_EQUEUE, idx, spn_QGROUP_GUARANTEE_ENABLE, FALSE,
             &buf_queue->qgroup_min_enable, devcfg->mmu_cell_size,
             _MMU_DEFAULT_THRES_TYPE);
    }

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        /* RQE queue settings */
        for (idx = 0; idx < SOC_MMU_CFG_RQE_QUEUE_MAX; idx++) {
            buf_rqe_queue = &buf->rqe_queues[idx];

            _soc_mmu_cfg_property_get_cells
                (unit, -1, spn_RQEQUEUE, idx, spn_GUARANTEE, FALSE,
                 &buf_rqe_queue->guarantee, devcfg->mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_scale
                (unit, -1, spn_RQEQUEUE, idx, spn_POOL_SCALE,
                 &buf_rqe_queue->pool_scale,
                 _MMU_DEFAULT_THRES_TYPE);

            _soc_mmu_cfg_property_get_cells
                (unit, -1, spn_RQEQUEUE, idx, spn_GUARANTEE, FALSE,
                 &buf_rqe_queue->ext_guarantee, devcfg->mmu_cell_size,
                 _MMU_EXTMEM_BASED);

            _soc_mmu_cfg_property_get_scale
                (unit, -1, spn_RQEQUEUE, idx, spn_POOL_SCALE,
                 &buf_rqe_queue->ext_pool_scale,
                 _MMU_EXTMEM_BASED);
            _soc_mmu_cfg_property_get_cells
                (unit, -1, spn_RQEQUEUE, idx, spn_GUARANTEE, FALSE,
                 &buf_rqe_queue->pkt_guarantee, devcfg->mmu_cell_size,
                 _MMU_QENTRY_BASED);

            _soc_mmu_cfg_property_get_scale
                (unit, -1, spn_RQEQUEUE, idx, spn_POOL_SCALE,
                 &buf_rqe_queue->pkt_pool_scale,
                 _MMU_QENTRY_BASED);


        }
    }
#endif
}

STATIC int
_soc_mmu_cfg_cross_point_buf_calculate(int unit, _soc_mmu_cfg_buf_t *buf,
                           _soc_mmu_device_info_t *devcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_qgroup_t *queue_group;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int port, idx;
    int pool, qgidx, total_usable_buffer;
    uint8 *qgpid;
    int xpe_map, xpe, pipe;
    int headroom_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int pg_min_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int port_min_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int queue_min_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int mcq_entry_reserved_xpe[SOC_MMU_CFG_XPE_MAX][SOC_MMU_CFG_SERVICE_POOL_MAX];
    int qgpid_size = SOC_MMU_CFG_QGROUP_MAX;

    si = &SOC_INFO(unit);

    for (idx = 0; idx < SOC_MMU_CFG_SERVICE_POOL_MAX; idx++) {
        for (xpe = 0; xpe < si->num_xpe; xpe ++) {
            queue_min_xpe[xpe][idx] = 0;
            pg_min_xpe[xpe][idx] = 0;
            port_min_xpe[xpe][idx] = 0;
            headroom_xpe[xpe][idx] = 0;
            mcq_entry_reserved_xpe[xpe][idx] = 0;
        }
    }

    total_usable_buffer = _SOC_MMU_CFG_MMU_TOTAL_CELLS(devcfg);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        for (xpe = 0; xpe < si->num_xpe; xpe ++) {
            buf_pool = &buf->pools_xpe[xpe][idx];
            if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                continue;
            }

            if (buf_pool->size & _MMU_CFG_BUF_PERCENT_FLAG) {
                buf_pool->total = ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                    total_usable_buffer) / 10000;
                if (devcfg->flags & SOC_MMU_CFG_F_EGR_MCQ_ENTRY) {
                    buf_pool->total_mcq_entry =
                        ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                                 devcfg->total_mcq_entry) / 10000;
                }
            }
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        pipe = si->port_pipe[port];

        xpe_map = ((pipe >= 0) && (pipe < si->num_pipe)) ? si->ipipe_xpe_map[pipe] : 0x0;
        /* pg headroom and pg min */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                if (xpe_map & (1 << xpe)) {
                    headroom_xpe[xpe][buf_prigroup->pool_idx] += buf_prigroup->headroom;
                    pg_min_xpe[xpe][buf_prigroup->pool_idx] += buf_prigroup->guarantee;
                }
            }
        }

        xpe_map = ((pipe >= 0) && (pipe < si->num_pipe)) ? si->epipe_xpe_map[pipe] : 0x0;
        /* port min */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            buf_port_pool = &buf_port->pools[idx];
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                if (xpe_map & (1 << xpe)) {
                    port_min_xpe[xpe][idx] += buf_port_pool->guarantee;
                }
            }
        }

        /* queue min */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                if (xpe_map & (1 << xpe)) {
                    if ((buf_queue->qgroup_id == -1) ||
                        (buf_queue->qgroup_min_enable == 0)) {
                        queue_min_xpe[xpe][buf_queue->pool_idx] += buf_queue->guarantee;
                    }

                    mcq_entry_reserved_xpe[xpe][buf_queue->pool_idx] +=
                        (buf_queue->mcq_entry_guarantee + buf_queue->mcq_entry_reserved);
                }
            }
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                if (xpe_map & (1 << xpe)) {
                    if ((buf_queue->qgroup_id == -1) ||
                        (buf_queue->qgroup_min_enable == 0)) {
                        queue_min_xpe[xpe][buf_queue->pool_idx] += buf_queue->guarantee;
                    }
                }
            }
        }
    }

    if (devcfg->flags & SOC_MMU_CFG_F_RQE) {
        for (idx = 0; idx < 11; idx++) {
            buf_rqe_queue = &buf->rqe_queues[idx];
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                queue_min_xpe[xpe][buf_rqe_queue->pool_idx] += buf_rqe_queue->guarantee;
            }
        }
    }

    if (SOC_IS_TD2P_TT2P(unit)) {
        qgpid_size = SOC_MMU_CFG_QGROUP_MAX * si->num_pipe;
    }
    qgpid = sal_alloc(sizeof(uint8) * qgpid_size, "queue2grp");
    if (!qgpid) {
        return SOC_E_MEMORY;
    }

    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        qgpid[idx] = 0;
    }

    /* Check all the queues belonging to queue group use same service pool. */
    pool = -1;

    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        xpe_map = ((pipe >= 0) && (pipe < si->num_pipe)) ? si->epipe_xpe_map[pipe] : 0x0;
        buf_port = &buf->ports[port];

        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];

            if (buf_queue->qgroup_id == -1) {
                continue;
            }

            qgidx = buf_queue->qgroup_id;
            /* deduct qgroup guarantee if not done so far */
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                    if (xpe_map & (1 << xpe)) {
                        queue_min_xpe[xpe][buf_queue->pool_idx] += queue_group->guarantee;
                    }
                }
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];

            if (buf_queue->qgroup_id == -1) {
                continue;
            }
            qgidx = buf_queue->qgroup_id;

            /* On TD2/TD2+ QGrp is per pipe, port in different pipe may have same QGrp id.
             * Thus add pipe offset here.
             */
            if (SOC_IS_TD2P_TT2P(unit)) {
                qgidx = qgidx + (SOC_MMU_CFG_QGROUP_MAX * pipe);
            }
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                    if (xpe_map & (1 << xpe)) {
                        queue_min_xpe[xpe][buf_queue->pool_idx] += queue_group->guarantee;
                    }
                }
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }
    }

    for (idx = 0; idx < SOC_MMU_CFG_EQUEUE_MAX; idx++) {
        buf_queue = &buf->equeues[idx];
        if (buf_queue->numq <= 0) {
            continue;
        }

        pool = buf_queue->pool_idx;
        if (buf_queue->qgroup_id == -1) {
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                queue_min_xpe[xpe][pool] += buf_queue->guarantee * buf_queue->numq;
            }
        } else {
            qgidx = buf_queue->qgroup_id;
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                    queue_min_xpe[xpe][buf_queue->pool_idx] += queue_group->guarantee;
                }
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }
    }

    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        if (qgpid[idx] & (qgpid[idx] - 1)) {
            LOG_CLI((BSL_META_U(unit,
                                "Queue belonging to same group use different Pools !!")));
            sal_free(qgpid);
            return SOC_E_PARAM;
        }
    }

    sal_free(qgpid);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        for (xpe = 0; xpe < si->num_xpe; xpe ++) {
            buf_pool = &buf->pools_xpe[xpe][idx];
            buf_pool->queue_guarantee = queue_min_xpe[xpe][idx];
            buf_pool->prigroup_headroom = headroom_xpe[xpe][idx];
            buf_pool->prigroup_guarantee = pg_min_xpe[xpe][idx];
            buf_pool->port_guarantee = port_min_xpe[xpe][idx];
            buf_pool->mcq_entry_reserved = mcq_entry_reserved_xpe[xpe][idx];
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "MMU buffer usage:\n")));
        LOG_CLI((BSL_META_U(unit,
                            "  Global headroom: %d\n"), buf->headroom));
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            for (xpe = 0; xpe < si->num_xpe; xpe ++) {
                buf_pool = &buf->pools_xpe[xpe][idx];
                if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "  Xpe %d Pool %d total prigroup guarantee: %d\n"),
                         xpe, idx, buf_pool->prigroup_guarantee));
                LOG_CLI((BSL_META_U(unit,
                                    "  Xpe %d Pool %d total prigroup headroom: %d\n"),
                         xpe, idx, buf_pool->prigroup_headroom));
                LOG_CLI((BSL_META_U(unit,
                                    "  Xpe %d Pool %d total queue guarantee: %d\n"),
                         xpe, idx, buf_pool->queue_guarantee));
                LOG_CLI((BSL_META_U(unit,
                                    "  Xpe %d Pool %d total mcq entry reserved: %d\n"),
                         xpe, idx, buf_pool->mcq_entry_reserved));
            }
        }
    }

    return SOC_E_NONE;
}

int
_soc_mmu_cfg_buf_calculate(int unit, _soc_mmu_cfg_buf_t *buf,
                           _soc_mmu_device_info_t *devcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_qgroup_t *queue_group;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int port, idx, headroom[SOC_MMU_CFG_SERVICE_POOL_MAX];
    int min_guarentees[SOC_MMU_CFG_SERVICE_POOL_MAX];
    int queue_min[SOC_MMU_CFG_SERVICE_POOL_MAX];
    int pool, qgidx, total_usable_buffer;
    uint8 *qgpid;
    int mcq_entry_reserved[SOC_MMU_CFG_SERVICE_POOL_MAX];

    for (idx = 0; idx < SOC_MMU_CFG_SERVICE_POOL_MAX; idx++) {
        queue_min[idx] = 0;
        min_guarentees[idx] = 0;
        headroom[idx] = 0;
        mcq_entry_reserved[idx] = 0;
    }

    si = &SOC_INFO(unit);

    total_usable_buffer = _SOC_MMU_CFG_MMU_TOTAL_CELLS(devcfg);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        if (buf_pool->size & _MMU_CFG_BUF_PERCENT_FLAG) {
            buf_pool->total = ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                total_usable_buffer) / 10000;
            if (devcfg->flags & SOC_MMU_CFG_F_EGR_MCQ_ENTRY) {
                buf_pool->total_mcq_entry =
                    ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                                             devcfg->total_mcq_entry) / 10000;
            }
        } else { 
            buf_pool->total = buf_pool->size ;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            if (buf_prigroup->user_delay != -1 &&
                buf_prigroup->switch_delay != -1) {
                /*
                 * number of max leftever cells =
                 *   port speed (megabits per sec) * 10**6 (megabits to bit) *
                 *   delay (nsec) / 10**9 (nsecs to second) /
                 *   8 (bits per byte) /
                 *   (IPG (12 bytes) + preamble (8 bytes) +
                 *    worse case packet size (145 bytes)) *
                 *   worse case packet size (2 cells)
                 * heandroom =
                 *   mtu (cells) + number of leftover cells
                 */
                buf_prigroup->headroom = buf_prigroup->pkt_size +
                    si->port_speed_max[port] *
                    (buf_prigroup->user_delay + buf_prigroup->switch_delay) *
                    2 / (8 * (12 + 8 + 145) * 1000);
            }
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            headroom[buf_prigroup->pool_idx] += buf_prigroup->headroom;
            min_guarentees[buf_prigroup->pool_idx] += buf_prigroup->guarantee;
        }

        /* port mins */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            buf_port_pool = &buf_port->pools[idx];
            min_guarentees[idx] += buf_port_pool->guarantee;
        }

        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            if ((buf_queue->qgroup_id == -1) ||
                (buf_queue->qgroup_min_enable == 0)) {
                queue_min[buf_queue->pool_idx] += buf_queue->guarantee;
            }

            mcq_entry_reserved[buf_queue->pool_idx] +=
              (buf_queue->mcq_entry_guarantee + buf_queue->mcq_entry_reserved);
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            if ((buf_queue->qgroup_id == -1) ||
                (buf_queue->qgroup_min_enable == 0)) {
                queue_min[buf_queue->pool_idx] += buf_queue->guarantee;
            }
        }
    }

    if (devcfg->flags & SOC_MMU_CFG_F_RQE) {
        for (idx = 0; idx < 11; idx++) {
            buf_rqe_queue = &buf->rqe_queues[idx];
            queue_min[buf_rqe_queue->pool_idx] += buf_rqe_queue->guarantee;
        }
    }

    qgpid = sal_alloc(sizeof(uint8)*SOC_MMU_CFG_QGROUP_MAX, "queue2grp");
    if (!qgpid) {
        return SOC_E_MEMORY;
    }


    /* Check all the queues belonging to queue group use same service pool. */
    pool = -1;

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        /* Give each port, a chance to add,
         * it's qgroup reservation to the total */
        for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
            qgpid[idx] = 0;
        }
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];

            if (buf_queue->qgroup_id == -1) {
                continue;
            }

            qgidx = buf_queue->qgroup_id;
            /* deduct qgroup guarantee if not done so far */
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                queue_min[buf_queue->pool_idx] += queue_group->guarantee;
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }

        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];

            if (buf_queue->qgroup_id == -1) {
                continue;
            }
            qgidx = buf_queue->qgroup_id;
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                queue_min[buf_queue->pool_idx] += queue_group->guarantee;
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }
    }

    for (idx = 0; idx < SOC_MMU_CFG_EQUEUE_MAX; idx++) {
        buf_queue = &buf->equeues[idx];
        if (buf_queue->numq <= 0) {
            continue;
        }

        pool = buf_queue->pool_idx;
        if (buf_queue->qgroup_id == -1) {
            queue_min[pool] += buf_queue->guarantee * buf_queue->numq;
        } else {
            qgidx = buf_queue->qgroup_id;
            if (qgpid[qgidx] == 0) {
                queue_group = &buf->qgroups[qgidx];
                queue_min[buf_queue->pool_idx] += queue_group->guarantee;
            }
            qgpid[qgidx] |= 1 << buf_queue->pool_idx;
        }
    }

    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        if (qgpid[idx] & (qgpid[idx] - 1)) {
            LOG_CLI((BSL_META_U(unit,
                                "Queue belonging to same group use different Pools !!")));
            sal_free(qgpid);
            return SOC_E_PARAM;
        }
    }

    sal_free(qgpid);

    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        buf_pool = &buf->pools[idx];

        buf_pool->queue_guarantee = queue_min[idx];
        buf_pool->prigroup_headroom = headroom[idx];
        buf_pool->prigroup_guarantee = min_guarentees[idx];
        buf_pool->mcq_entry_reserved = mcq_entry_reserved[idx];
#if 0
        buf_pool->total -= (buf_pool->queue_guarantee +
                            buf_pool->prigroup_headroom +
                            buf_pool->prigroup_guarantee);
#endif
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "MMU buffer usage:\n")));
        LOG_CLI((BSL_META_U(unit,
                            "  Global headroom: %d\n"), buf->headroom));
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
            buf_pool = &buf->pools[idx];
            if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                continue;
            }
            LOG_CLI((BSL_META_U(unit,
                                "  Pool %d total prigroup guarantee: %d\n"),
                     idx, buf_pool->prigroup_guarantee));
            LOG_CLI((BSL_META_U(unit,
                                "  Pool %d total prigroup headroom: %d\n"),
                     idx, buf_pool->prigroup_headroom));
            LOG_CLI((BSL_META_U(unit,
                                "  Pool %d total queue guarantee: %d\n"),
                     idx, buf_pool->queue_guarantee));
            LOG_CLI((BSL_META_U(unit,
                                "  Pool %d total mcq entry reserved: %d\n"),
                     idx, buf_pool->mcq_entry_reserved));
        }
    }

    SOC_IF_ERROR_RETURN(
        _soc_mmu_cfg_cross_point_buf_calculate(unit, buf, devcfg));

    return SOC_E_NONE;
}

int
_soc_mmu_cfg_buf_check(int unit, _soc_mmu_cfg_buf_t *buf,
                        _soc_mmu_device_info_t *devcfg)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    int yellow_cells, red_cells;
    int port, dft_pool, idx;
    uint32 pool_map;

    SOC_IF_ERROR_RETURN(_soc_mmu_cfg_buf_calculate(unit, buf, devcfg));

    dft_pool =0;
    pool_map = 0;
    for (idx = 0; idx < _SOC_MMU_CFG_DEV_POOL_NUM(devcfg); idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        if (pool_map == 0) {
            dft_pool = idx;
        }
        pool_map |= 1 << idx;

        if (buf_pool->total <= 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Pool %d has no shared space after "
                                "deducting guaranteed !!"), idx));
            return SOC_E_FAIL;
        }

        if (buf_pool->yellow_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            yellow_cells = (buf_pool->yellow_size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                buf_pool->total / 10000;
        } else {
            yellow_cells = buf_pool->yellow_size;
        }
        if (buf_pool->red_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            red_cells = (buf_pool->red_size & ~_MMU_CFG_BUF_PERCENT_FLAG) *
                buf_pool->total / 10000;
        } else {
            red_cells = buf_pool->red_size;
        }

        if (yellow_cells > red_cells) {
            LOG_CLI((BSL_META_U(unit,
                                "MMU config pool %d: Yellow cells offset is higher "
                                "than red cells\n"), idx));
        }
        if (red_cells > buf_pool->total) {
            LOG_CLI((BSL_META_U(unit,
                                "MMU config pool %d: Red cells offset is higher "
                                "than pool shared cells\n"), idx));
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            if (buf_port->pri_to_prigroup[idx] < 0 ||
                buf_port->pri_to_prigroup[idx] >= _SOC_MMU_CFG_DEV_PG_NUM(devcfg)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d: Invalid prigroup value (%d) "
                                    "for internal priority %d\n"),
                         port, buf_port->pri_to_prigroup[idx], idx));
                buf_port->pri_to_prigroup[idx] = 0; /* use prigroup 0 */
            }
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _SOC_MMU_CFG_DEV_PG_NUM(devcfg); idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            if (buf_prigroup->pool_idx < 0 ||
                buf_prigroup->pool_idx >= _SOC_MMU_CFG_DEV_POOL_NUM(devcfg)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d prigroup %d: "
                                    "Invalid pool value (%d)\n"),
                         port, idx, buf_prigroup->pool_idx));
                /* use first non-empty pool */
                buf_prigroup->pool_idx = dft_pool;
            } else if (!(pool_map & (1 << buf_prigroup->pool_idx))) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d prigroup %d: "
                                    "Pool %d is empty\n"),
                         port, idx, buf_prigroup->pool_idx));
            }
        }

#if 0
        /* queue to pool mapping */
        count = (IS_LB_PORT(unit, port) ? 5 : si->port_num_cosq[port]) +
                                               si->port_num_uc_cosq[port] +
                                               si->port_num_ext_cosq[port];
        for (idx = 0; idx < count; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            queue_idx = idx;
            if (queue_idx < si->port_num_cosq[port]) {
                sal_sprintf(queue_name, "mqueue %d", queue_idx);
            } else {
                queue_idx -= si->port_num_cosq[port];
                if (queue_idx < si->port_num_uc_cosq[port]) {
                    sal_sprintf(queue_name, "queue %d", queue_idx);
                } else {
                    queue_idx -= si->port_num_uc_cosq[port];
                    sal_sprintf(queue_name, "equeue %d", queue_idx);
                }
            }

            if (buf_queue->pool_idx < 0 ||
                buf_queue->pool_idx >= _SOC_MMU_CFG_DEV_POOL_NUM(devcfg)) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d %s: "
                         "Invalid pool value (%d)\n"),
                         port, queue_name, buf_queue->pool_idx));
                buf_queue->pool_idx = dft_pool; /* use first non-empty pool */
            } else if (!(pool_map & (1 << buf_queue->pool_idx))) {
                LOG_CLI((BSL_META_U(unit,
                                    "MMU config port %d %s: Pool %d is empty\n"),
                         port, queue_name, buf_queue->pool_idx));
            }
        }
#endif
    }

    return SOC_E_NONE;
}

_soc_mmu_cfg_buf_t* soc_mmu_cfg_alloc(int unit)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    int num_cosq, alloc_size=0;
    soc_port_t port;
    si = &SOC_INFO(unit);

    alloc_size = sizeof(_soc_mmu_cfg_buf_t);
    PBMP_ALL_ITER(unit, port) {
        num_cosq = si->port_num_cosq[port] + si->port_num_uc_cosq[port];
        if (SOC_IS_MONTEREY(unit) && !IS_CPU_PORT (unit, port) ) { 
            num_cosq = 20;
        }
        alloc_size += (sizeof(_soc_mmu_cfg_buf_queue_t) * num_cosq);
    }

    buf = sal_alloc(alloc_size, "MMU config buffer");
    if (buf == NULL) {
        return NULL;
    }

    sal_memset(buf, 0, alloc_size);
    buf_queue = (_soc_mmu_cfg_buf_queue_t *)&buf[1];
    PBMP_ALL_ITER(unit, port) {
        num_cosq = si->port_num_cosq[port] + si->port_num_uc_cosq[port];
        buf->ports[port].queues = buf_queue;
        buf_queue += num_cosq;
    }
    return buf;
}

void
soc_mmu_cfg_free(int unit, _soc_mmu_cfg_buf_t *cfg)
{
    sal_free(cfg);
}

int
soc_mmu_pool_map_cfg_read(int unit, int *pool_map)
{
    char name[80];
    int  egr_idx, ing_idx;
    int pool_map_arr[SOC_MMU_CFG_SERVICE_POOL_MAX];
    int count;

    for (egr_idx = 0; egr_idx < SOC_MMU_CFG_SERVICE_POOL_MAX; egr_idx ++) {
        for (ing_idx = 0; ing_idx < SOC_MMU_CFG_SERVICE_POOL_MAX; ing_idx ++) {
            pool_map_arr[ing_idx] = -1;
        }

        /* Egress Service Pool ID to Ingress Service Pool mapping*/
        sal_sprintf(name, "%s.%s.%s%d.%s",
                    spn_BUF, spn_MAP, spn_EGRESS_POOL, egr_idx, spn_INGRESS_POOL);
        count = soc_property_get_csv(unit, name, SOC_MMU_CFG_SERVICE_POOL_MAX,
                                     pool_map_arr);

        /* Default mapping: Egress pool ID same as ingress pool ID */
        if (count == 0) {
            pool_map_arr[0] = egr_idx;
        }

        for (ing_idx = 0; ing_idx < SOC_MMU_CFG_SERVICE_POOL_MAX; ing_idx ++) {
            if ((pool_map_arr[ing_idx] < -1) ||
                (pool_map_arr[ing_idx] >= SOC_MMU_CFG_SERVICE_POOL_MAX)) {
                return SOC_E_PARAM;
            }
            if (pool_map_arr[ing_idx] != -1) {
                pool_map[egr_idx] |= 1 << pool_map_arr[ing_idx];
            }
        }
    }

    return SOC_E_NONE;
}

#endif /* defined(BCM_TRIUMPH3_SUPPORT) */
