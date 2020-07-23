/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packetized Statistic
 * Purpose: API to set Packetized Stats registers.
 */

#include <sal/core/libc.h>

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(INCLUDE_PSTATS) && defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk2.h>
#include <soc/pstats.h>

#include <bcm/error.h>
#include <bcm/pstats.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/tomahawk2.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/pstats.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define PSTATS_SESSION_MIN          1
#define PSTATS_SESSION_MAX          256
/* Max 4 XPEs and 4 PIPEs */
#define TH2_MAX_XPEPIPE             16
#define TH2_MAX_POOL_COUNT          4
#define TH2_MAX_UCQ_COUNT           1344

/* Max Elements Count = UCQ + ING POOL + EGR POOL */
#define TH2_MAX_ELEMENT_COUNT       TH2_MAX_UCQ_COUNT + TH2_MAX_POOL_COUNT * 2

#define TH2_PORT_NUM_COS(unit, port)                            \
    ((IS_CPU_PORT(unit, port)) ? NUM_CPU_COSQ(unit) :           \
     (IS_LB_PORT(unit, port)) ? 10 : NUM_COS(unit))

/*
 * Macro to get buffer index
 * Per XPE or PIPE instance (ING POOL & EGR POOL)
 * buffer_id = xpe or pipe
 *  ----------------------
 * | buffer | xpe or pipe |
 * |   0    |     0       |
 * |   1    |     1       |
 * |   2    |     2       |
 * |   3    |     3       |
 *  ----------------------
 * Per XPE-PIPE instance (UCQ)
 * buffer_id = xpe * NUM_PIPE(unit) + pipe
 *  --------------------
 * | buffer | xpe | pipe |
 * |   0    |  0  |  0   |
 * |   1    |  0  |  1   |
 * |   2    |  0  |  2   |
 * |   3    |  0  |  3   |
 * |   4    |  1  |  0   |
 * |   5    |  1  |  1   |
 * |   6    |  1  |  2   |
 * |   7    |  1  |  3   |
 * |   8    |  2  |  0   |
 * |   9    |  2  |  1   |
 * |   10   |  2  |  2   |
 * |   11   |  2  |  3   |
 * |   12   |  3  |  0   |
 * |   13   |  3  |  1   |
 * |   14   |  3  |  2   |
 * |   15   |  3  |  3   |
 *  ---------------------
 */
#define TH2_XPE_PIPE_INDEX(unit, xpe, pipe)                     \
    (((xpe) == -1) ? (pipe) :                                   \
     ((pipe) == -1) ? (xpe) : (xpe) * NUM_PIPE(unit) + (pipe))

#define TH2_XPE_VALID(unit, xpe, valid)                        \
        do {                                                    \
            if (soc_property_get(unit, "num_pipe", 4) == 2) {   \
                if (xpe == 1 || xpe == 3) {                     \
                    valid = FALSE;                              \
                }                                               \
            }                                                   \
        } while (0);

typedef struct pstats_session_s {
    bcm_pstats_session_id_t         session_id;

    /* session buffer */
    bcm_pstats_timestamp_t          timestamp;
    bcm_pstats_data_t               *data_array;

    /* session element info */
    int                             element_array_count;
    bcm_pstats_session_element_t    element_array[TH2_MAX_ELEMENT_COUNT];

    uint32                          enable;

    /* prev session */
    struct pstats_session_s         *prev_session;
    /* next session */
    struct pstats_session_s         *next_session;
} pstats_session_t;

typedef struct pstats_buffer_s {
    soc_mem_t                       mem;
    soc_mem_t                       clear_mem;
    int                             min_idx;
    int                             max_idx;
    int                             num_entries;
    uint8                           *buf;
} pstats_buffer_t;

typedef struct pstats_control_s {
    /* full buffer */
#ifdef BCM_PSTATS_MEASURE
    pstats_buffer_t                 local_time;
    pstats_buffer_t                 local_time_end;
#endif
    pstats_buffer_t                 utc_time;
    pstats_buffer_t                 ucq_buffer[TH2_MAX_XPEPIPE];
    pstats_buffer_t                 ing_pool_buffer[TH2_MAX_POOL_COUNT];
    pstats_buffer_t                 egr_pool_buffer[TH2_MAX_POOL_COUNT];

    bcm_pstats_session_id_t         next_session_id;
    uint32                          session_count;
#ifdef BCM_PSTATS_MEASURE
    uint64                          time_cost;
#endif
    int                             stat_mode;
    int                             hw_cor_in_max_use_count;
    pstats_session_t                *first_session;
} pstats_control_t;

static soc_field_t                  ucq_field[4] = {UCQ_COUNT0f, UCQ_COUNT1f,
                                                    UCQ_COUNT2f, UCQ_COUNT3f};
static soc_field_t                  isp_field[4] = {SP0f, SP1f, SP2f, SP3f};
static soc_field_t                  esp_field[4] = {COUNT_0f, COUNT_1f,
                                                    COUNT_2f, COUNT_3f};

static pstats_control_t             pstats_control[BCM_MAX_NUM_UNITS];
#define PSTATS_CTRL(u)              pstats_control[u]

STATIC int
bcm_th2_pstats_session_create(int unit, int options, int array_count,
                              bcm_pstats_session_element_t  *element_array,
                              bcm_pstats_session_id_t *session_id);
STATIC int
bcm_th2_pstats_session_destroy(int unit, bcm_pstats_session_id_t session_id);
STATIC int
bcm_th2_pstats_session_get(int unit, bcm_pstats_session_id_t session_id,
                           int array_max,
                           bcm_pstats_session_element_t *element_array,
                           int *array_count);
STATIC int bcm_th2_pstats_data_sync(int unit);
STATIC int
bcm_th2_pstats_session_data_get(int unit, bcm_pstats_session_id_t session_id,
                                int sync,
                                bcm_pstats_timestamp_t *timestamp,
                                int array_max,
                                bcm_pstats_data_t *data,
                                int *array_count);
STATIC int
bcm_th2_pstats_session_data_clear(int unit, bcm_pstats_session_id_t session_id);
STATIC int
bcm_th2_pstats_session_traverse(int unit, bcm_pstats_session_traverse_cb cb,
                                void *user_data);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     bcm_th2_pstats_sw_dump
 * Purpose:
 *     Displays PSTATS information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
STATIC void
bcm_th2_pstats_sw_dump(int unit)
{
    pstats_session_t *cur_session;

    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));
    LOG_CLI((BSL_META_U(unit, "Pstats mode: %s\n"),
                        PSTATS_CTRL(unit).stat_mode ?
                        "Max-use-count" : "Instantaneous"));
    LOG_CLI((BSL_META_U(unit, "Total pstats session count: %d\n"),
                        PSTATS_CTRL(unit).session_count));
    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        LOG_CLI((BSL_META_U(unit,
                            "Pstats session id: %d element count: %d\n"),
                            cur_session->session_id,
                            cur_session->element_array_count));
        cur_session = cur_session->next_session;
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

STATIC void
_bcm_th2_pstats_session_cleanup(int unit, pstats_session_t *session)
{
    session->enable = FALSE;
    if (NULL != session->data_array) {
        sal_free(session->data_array);
    }

    sal_memset(session, 0, sizeof(pstats_session_t));

    return;
}

STATIC pstats_session_t*
_bcm_th2_pstats_session_get(int unit, bcm_pstats_session_id_t session_id)
{
    int                 found = 0;
    pstats_session_t   *cur_session;

    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        if (cur_session->session_id == session_id) {
            found = 1;
            break;
        }
        cur_session = cur_session->next_session;
    }

    if (found) {
        return cur_session;
    }

    return NULL;
}

/* Set a minimum available session id */
STATIC void
_bcm_th2_pstats_session_next_id_set(int unit)
{
    pstats_session_t            *cur_session;
    bcm_pstats_session_id_t     id = PSTATS_SESSION_MIN;

    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        if (id < cur_session->session_id) {
            break;
        } else if (id == cur_session->session_id) {
            /* this id already existed */
            id++;
        }
        cur_session = cur_session->next_session;
    }

    PSTATS_CTRL(unit).next_session_id = id;

    LOG_INFO(BSL_LS_BCM_PSTATS, (BSL_META_U(unit, "next %d\n"), id));
}

STATIC int
_bcm_th2_pstats_session_delete(int unit, bcm_pstats_session_id_t session_id)
{
    pstats_session_t            *cur_session;

    cur_session = _bcm_th2_pstats_session_get(unit, session_id);
    if (NULL == cur_session) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "pstats session %d not found\n"), session_id));
        return BCM_E_NOT_FOUND;
    }
    /* update session link */
    if ((NULL == cur_session->prev_session) &&
        (NULL == cur_session->next_session)) {
        /* only one session */
        PSTATS_CTRL(unit).first_session = NULL;
    } else if (NULL == cur_session->prev_session) {
        /* first session */
        PSTATS_CTRL(unit).first_session = cur_session->next_session;
        cur_session->next_session->prev_session = NULL;
    } else if (NULL == cur_session->next_session) {
        /* last session */
        cur_session->prev_session->next_session = NULL;
    } else {
        cur_session->prev_session->next_session = cur_session->next_session;
        cur_session->next_session->prev_session = cur_session->prev_session;
    }
    _bcm_th2_pstats_session_cleanup(unit, cur_session);
    if (NULL != cur_session) {
        sal_free(cur_session);
    }
    PSTATS_CTRL(unit).session_count--;

    _bcm_th2_pstats_session_next_id_set(unit);

    return BCM_E_NONE;
}

STATIC void
_bcm_th2_pstats_session_insert(int unit, pstats_session_t *session)
{
    pstats_session_t            *cur_session;
    bcm_pstats_session_id_t     id = session->session_id;

    cur_session = PSTATS_CTRL(unit).first_session;
    if (NULL == cur_session) {
        /* first session */
        PSTATS_CTRL(unit).first_session = session;
        session->prev_session = NULL;
        session->next_session = NULL;
    }
    while (NULL != cur_session) {
        if (id > cur_session->session_id) {
            /* last session */
            if (NULL == cur_session->next_session) {
                cur_session->next_session = session;
                session->prev_session = cur_session;
                session->next_session = NULL;
                break;
            } else {
                cur_session = cur_session->next_session;
            }
        } else {
            /* first session */
            if (NULL == cur_session->prev_session) {
                PSTATS_CTRL(unit).first_session = session;
                session->prev_session = NULL;
                session->next_session = cur_session;
                cur_session->prev_session = session;
                break;
            } else {
                session->prev_session = cur_session->prev_session;
                session->next_session = cur_session;
                cur_session->prev_session->next_session = session;
                cur_session->prev_session = session;
                break;
            }
            break;
        }
    }
    PSTATS_CTRL(unit).session_count++;

    session->enable = TRUE;

    return;
}

STATIC void
_bcm_th2_pstats_buffer_deinit(int unit, pstats_buffer_t *pbuf)
{
    if (NULL == pbuf) {
        return;
    }

    if (NULL != pbuf->buf) {
        sal_free(pbuf->buf);
    }
    sal_memset(pbuf, 0, sizeof(pstats_buffer_t));

    return;
}

STATIC int
_bcm_th2_pstats_buffer_init(int unit, soc_mem_t mem, soc_mem_t clear_mem,
                            pstats_buffer_t *pbuf)
{
    int alloc_size;

    if (NULL == pbuf) {
        return BCM_E_PARAM;
    }

    if (NULL != pbuf->buf) {
        return BCM_E_PARAM;
    }

    pbuf->mem = mem;
    pbuf->clear_mem = clear_mem;
    pbuf->min_idx = soc_mem_index_min(unit, mem);
    pbuf->max_idx = soc_mem_index_max(unit, mem);
    pbuf->num_entries = soc_mem_index_max(unit, mem) + 1;
    alloc_size = sizeof(uint32) * soc_mem_entry_words(unit, mem);
    alloc_size *= pbuf->num_entries;

    pbuf->buf = sal_alloc(alloc_size, "pstats buffer");
    if (pbuf->buf == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(pbuf->buf, 0, alloc_size);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_pstats_deinit
 * Purpose:
 *      Deinitialize PSTATS driver functions
 *      for Tomahawk2 (BCM56970).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
void
bcm_th2_pstats_deinit(int unit)
{
    pstats_session_t *cur_session, *next_session;
    int i;

    soc_pstats_deinit(unit);

#ifdef BCM_PSTATS_MEASURE
    _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).local_time);
    _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).local_time_end);
#endif
    _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).utc_time);
    for (i = 0; i < TH2_MAX_XPEPIPE; i++) {
        _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).ucq_buffer[i]);
    }
    for (i = 0; i < TH2_MAX_POOL_COUNT; i++) {
        _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).ing_pool_buffer[i]);
        _bcm_th2_pstats_buffer_deinit(unit, &PSTATS_CTRL(unit).egr_pool_buffer[i]);
    }

    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        next_session = cur_session->next_session;
        _bcm_th2_pstats_session_cleanup(unit, cur_session);
        if (NULL != cur_session) {
            sal_free(cur_session);
        }
        PSTATS_CTRL(unit).session_count--;
        cur_session = next_session;
    }

    if (PSTATS_CTRL(unit).session_count != 0) {
        /* debug info */
    }
    sal_memset(&PSTATS_CTRL(unit), 0, sizeof(pstats_control_t));

    return;
}


/*
 * Function:
 *      bcm_th2_pstats_init
 * Purpose:
 *      Initialize PSTATS driver functions
 *      for Tomahawk2 (BCM56970).
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_pstats_init(int unit)
{
    _bcm_pstats_unit_driver_t   *pstats_driver;
    pstats_buffer_t             *pbuf;
    int                         rv = BCM_E_NONE;
    soc_mem_t                   mem, clear_mem, base_mem;
    int                         index, pipe, xpe;
    uint32                      flag;

    if (2 != soc_property_get(unit, spn_BUFFER_STATS_COLLECT_TYPE, 1)) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "current mode is not pstats, no need init\n")));
        return BCM_E_NONE;
    }

    pstats_driver = _BCM_PSTATS_UNIT_DRIVER(unit);
    if (pstats_driver == NULL) {
        return BCM_E_MEMORY;
    }

    /* Initializing Driver functions for PSTATS */
    pstats_driver->session_create = bcm_th2_pstats_session_create;
    pstats_driver->session_destroy = bcm_th2_pstats_session_destroy;
    pstats_driver->session_get = bcm_th2_pstats_session_get;
    pstats_driver->data_sync = bcm_th2_pstats_data_sync;
    pstats_driver->session_data_get = bcm_th2_pstats_session_data_get;
    pstats_driver->session_data_clear = bcm_th2_pstats_session_data_clear;
    pstats_driver->session_traverse = bcm_th2_pstats_session_traverse;
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    pstats_driver->pstats_sw_dump = bcm_th2_pstats_sw_dump;
#endif

    /* alloc buffer for full pstats table */
#ifdef BCM_PSTATS_MEASURE
    /* local time */
    mem = MMU_INTFO_TIMESTAMPm;
    pbuf = &PSTATS_CTRL(unit).local_time;
    rv = _bcm_th2_pstats_buffer_init(unit, mem, mem, pbuf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    mem = MMU_INTFO_TIMESTAMPm;
    pbuf = &PSTATS_CTRL(unit).local_time_end;
    rv = _bcm_th2_pstats_buffer_init(unit, mem, mem, pbuf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
#endif

    /* utc time */
    mem = MMU_INTFO_UTC_TIMESTAMPm;
    pbuf = &PSTATS_CTRL(unit).utc_time;
    rv = _bcm_th2_pstats_buffer_init(unit, mem, mem, pbuf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* ucq buffer */
    base_mem = MMU_THDU_UCQ_STATS_TABLEm;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
            clear_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, MMU_THDU_UCQ_STATSm,
                                                    xpe, pipe);
            if (mem == INVALIDm) {
                continue;
            }

            index = TH2_XPE_PIPE_INDEX(unit, xpe, pipe);
            if (index >= TH2_MAX_XPEPIPE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            pbuf = &PSTATS_CTRL(unit).ucq_buffer[index];

            rv = _bcm_th2_pstats_buffer_init(unit, mem, clear_mem, pbuf);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } /* ucq buffer */

    /* ing pool buffer */
    base_mem = THDI_PKT_STAT_SP_SHARED_COUNTm;
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[xpe];
        if (mem == INVALIDm) {
            continue;
        }

        index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
        if (index >= TH2_MAX_POOL_COUNT) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        pbuf = &PSTATS_CTRL(unit).ing_pool_buffer[index];

        rv = _bcm_th2_pstats_buffer_init(unit, mem, mem, pbuf);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } /* ing pool buffer */

    /* egr pool buffer */
    base_mem = MMU_THDM_DB_POOL_MCUC_PKSTATm;
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[xpe];
        if (mem == INVALIDm) {
            continue;
        }

        index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
        if (index >= TH2_MAX_POOL_COUNT) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        pbuf = &PSTATS_CTRL(unit).egr_pool_buffer[index];

        rv = _bcm_th2_pstats_buffer_init(unit, mem, mem, pbuf);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } /* egr pool buffer */

    PSTATS_CTRL(unit).next_session_id = PSTATS_SESSION_MIN;

    rv = soc_pstats_init(unit);
    if (BCM_FAILURE(rv)) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "soc_pstats_init failed rv = %d\n"), rv));
        goto cleanup;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        int config;

        /* get pstats config from HW */
        rv = soc_tomahawk2_mmu_pstats_mode_get(unit, &flag);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* HW config is different with config property */
        if (!(flag & _TH2_MMU_PSTATS_ENABLE) ||
            !(flag & _TH2_MMU_PSTATS_PKT_MOD)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "WARMBOOT ERROR.\n"
                                  "Current buffer stats mode is not pstats.\n")));
            rv = BCM_E_INIT;
            goto cleanup;
        }

        if (flag & _TH2_MMU_PSTATS_HWM_MOD) {
            PSTATS_CTRL(unit).stat_mode = 1;
            if (flag & _TH2_MMU_PSTATS_RESET_ON_READ) {
                PSTATS_CTRL(unit).hw_cor_in_max_use_count = 1;
            } else {
                PSTATS_CTRL(unit).hw_cor_in_max_use_count = 0;
            }
        } else {
            PSTATS_CTRL(unit).stat_mode = 0;
            PSTATS_CTRL(unit).hw_cor_in_max_use_count = 0;
        }

        config = soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0);
        if (config != PSTATS_CTRL(unit).stat_mode) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "WARMBOOT WARNING.\n"
                                 "Config mode %d. HW mode %d.\n"),
                                 config, PSTATS_CTRL(unit).stat_mode));
        }

        flag |= _TH2_MMU_PSTATS_SYNC | _TH2_MMU_PSTATS_MOR_EN;

        /* Reset pstats mode in warmboot */
        rv = soc_tomahawk2_mmu_pstats_mode_set(unit, flag);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } else
#endif
    {
        PSTATS_CTRL(unit).stat_mode =
                soc_property_get(unit, spn_BUFFER_STATS_COLLECT_MODE, 0);
        if ((PSTATS_CTRL(unit).stat_mode == 1) &&
            soc_property_get(unit, "oob_pstats_hw_cor_en", 1)) {
            PSTATS_CTRL(unit).hw_cor_in_max_use_count = 1;
        }

        /* call soc function */
        flag = _TH2_MMU_PSTATS_ENABLE |
               _TH2_MMU_PSTATS_PKT_MOD |
               _TH2_MMU_PSTATS_SYNC |
               _TH2_MMU_PSTATS_MOR_EN;


        if (PSTATS_CTRL(unit).stat_mode) {
            flag |= _TH2_MMU_PSTATS_HWM_MOD;
            if (PSTATS_CTRL(unit).hw_cor_in_max_use_count == 1) {
                flag |= _TH2_MMU_PSTATS_RESET_ON_READ;
            }
        }

        rv = soc_tomahawk2_mmu_pstats_mode_set(unit, flag);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    return BCM_E_NONE;

cleanup:
    soc_pstats_deinit(unit);
    (void )bcm_th2_pstats_deinit(unit);

    return rv;
}

STATIC int
_bcm_th2_pstats_session_element_check(int unit,
                                       bcm_pstats_session_element_t element)
{
    bcm_gport_t                 gport;
    bcm_port_t                  local_port;
    bcm_cos_queue_t             cosq;

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_element_check unit %d\n"), unit));

    if (element.type == bcmPStatsSessionUnicastQueue) {
        gport = element.gport;
        cosq = element.cosq;
        if (BCM_GPORT_IS_SET(gport)) {
            if (BCM_GPORT_IS_LOCAL(gport)) {
                local_port = BCM_GPORT_LOCAL_GET(gport);
            } else if (BCM_GPORT_IS_MODPORT(gport)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, gport, &local_port));
            } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                return BCM_E_NONE;
            } else {
                LOG_INFO(BSL_LS_BCM_PSTATS,
                         (BSL_META_U(unit,
                                     "pstats_session_element_check: "
                                     "gport 0x%x is illegal!\n"), gport));
                return BCM_E_PARAM;
            }
        } else {
            local_port = gport;
        }

        if (!SOC_PORT_VALID(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
            LOG_INFO(BSL_LS_BCM_PSTATS,
                     (BSL_META_U(unit,
                                 "pstats_session_element_check: "
                                 "gport 0x%x is illegal!\n"), gport));
            return BCM_E_PORT;
        }
        /* cosq check when gport is a port gport */
        if ((cosq < 0) || (cosq >= TH2_PORT_NUM_COS(unit, local_port))) {
            LOG_INFO(BSL_LS_BCM_PSTATS,
                     (BSL_META_U(unit,
                                 "pstats_session_element_check: "
                                 "cosq %d is illegal!\n"), cosq));
            return BCM_E_PARAM;
        }
    } else if ((element.type == bcmPStatsSessionIngPool) ||
               (element.type == bcmPStatsSessionEgrPool)) {
        if ((element.pool < 0) || (element.pool >= TH2_MAX_POOL_COUNT)) {
            LOG_INFO(BSL_LS_BCM_PSTATS,
                     (BSL_META_U(unit,
                                 "pstats_session_element_check: "
                                 "pool_id %d is illegal!\n"), element.pool));
            return BCM_E_PARAM;
        }
    } else {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit,
                             "pstats_session_element_check: "
                             "unknow element type %d\n"), element.type));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th2_pstats_session_create
 * Purpose:
 *      Create a PKT STAT session instance for SDK management.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      options - (IN) BCM_PSTATS_REPLACE: replace existing.
 *                     BCM_PSTATS_WITH_ID: session_id argument is given.
 *      array_count - (IN) Number of elements in array.
 *      element_array - (IN) PSTATS session element array.
 *      session_id - (INOUT) id of a PSTATS session created via this API.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_create(int unit, int options, int array_count,
                              bcm_pstats_session_element_t  *element_array,
                              bcm_pstats_session_id_t *session_id)
{
    int                         rv, i, alloc_size;
    bcm_pstats_session_id_t     id = 0;
    pstats_session_t            *cur_session = NULL;

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_create unit %d options %d "
                               "array_count %d\n"), unit, options, array_count));

    /* element check */
    if ((array_count < 0) ||
        (array_count > TH2_MAX_ELEMENT_COUNT)) {
        LOG_ERROR(BSL_LS_BCM_PSTATS,
                  (BSL_META_U(unit, "array_count %d is illegal!\n"), array_count));
        return BCM_E_PARAM;
    }
    if (NULL == element_array) {
        LOG_ERROR(BSL_LS_BCM_PSTATS,
                  (BSL_META_U(unit, "element_array is null\n")));
        return BCM_E_PARAM;
    }

    for (i = 0; i < array_count; i++) {
        BCM_IF_ERROR_RETURN(
                _bcm_th2_pstats_session_element_check(unit, element_array[i]));
    }

    if (options & BCM_PSTATS_OPTIONS_REPLACE) {
        /* delete original session first */
        id = *session_id;
        BCM_IF_ERROR_RETURN(_bcm_th2_pstats_session_delete(unit, id));
    } else if (options & BCM_PSTATS_OPTIONS_WITH_ID) {
        id = *session_id;
        if (id < PSTATS_SESSION_MIN) {
            LOG_ERROR(BSL_LS_BCM_PSTATS,
                      (BSL_META_U(unit, "input session_id %d is illegal!\n"), id));
            return BCM_E_PARAM;
        }
        cur_session = _bcm_th2_pstats_session_get(unit, id);
        if (NULL != cur_session) {
            LOG_INFO(BSL_LS_BCM_PSTATS,
                     (BSL_META_U(unit, "pstats_session_create: "
                                       "session_id %d is existed\n"), id));
            return BCM_E_EXISTS;
        }
    } else {
        id = PSTATS_CTRL(unit).next_session_id;
    }

    /* SDK has a limitation of maxium session count */
    if (PSTATS_CTRL(unit).session_count >= PSTATS_SESSION_MAX) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "pstats_session_create: "
                                   "session count is full\n")));
        return BCM_E_FULL;
    }

    cur_session =
        (pstats_session_t*) sal_alloc(sizeof(pstats_session_t),
                                      "pstats session");
    if (NULL == cur_session) {
        return BCM_E_MEMORY;
    }
    sal_memset(cur_session, 0, sizeof(pstats_session_t));

    /* record session info */
    cur_session->session_id = id;
    cur_session->element_array_count = array_count;
    sal_memcpy(cur_session->element_array,
               element_array,
               array_count * sizeof(bcm_pstats_session_element_t));

    alloc_size = array_count * sizeof(bcm_pstats_data_t);
    cur_session->data_array = sal_alloc(alloc_size, "data array");
    if (NULL == cur_session->data_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(cur_session->data_array, 0, alloc_size);

    _bcm_th2_pstats_session_insert(unit, cur_session);
    _bcm_th2_pstats_session_next_id_set(unit);

    *session_id = id;
    return BCM_E_NONE;
cleanup:
    _bcm_th2_pstats_session_cleanup(unit, cur_session);
    if (NULL != cur_session) {
        sal_free(cur_session);
    }
    return rv;
}

/*
 * Function:
 *      bcm_th2_pstats_session_destroy
 * Purpose:
 *      Destroy an existing PSTATS session.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pkt==stats_session_create
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_destroy(int unit,
                               bcm_pstats_session_id_t session_id)
{
    return _bcm_th2_pstats_session_delete(unit, session_id);
}

/*
 * Function:
 *      bcm_th2_pstats_session_get
 * Purpose:
 *      Get an existing PSTATS session info.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 *      array_max - (IN) Maximum number of elements in array.
 *      element_array - (OUT) element array of a PSTATS session.
 *      array_count - (OUT) Number of elements in array.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_get(int unit,
                           bcm_pstats_session_id_t session_id,
                           int array_max,
                           bcm_pstats_session_element_t *element_array,
                           int *array_count)
{
    pstats_session_t *cur_session = NULL;
    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_get unit %d session_id %d\n"),
                               unit, session_id));

    if (NULL == element_array) {
        LOG_ERROR(BSL_LS_BCM_PSTATS,
                  (BSL_META_U(unit, "element_array is null\n")));
        return BCM_E_PARAM;
    }

    cur_session = _bcm_th2_pstats_session_get(unit, session_id);
    if (NULL == cur_session) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "pstats session %d not found\n"), session_id));
        return BCM_E_NOT_FOUND;
    }

    if (array_max >= cur_session->element_array_count) {
        *array_count = cur_session->element_array_count;
    } else if (array_max >= 0){
        *array_count = array_max;
    } else {
        return BCM_E_PARAM;
    }

    sal_memcpy(element_array,
               cur_session->element_array,
               *array_count * sizeof(bcm_pstats_session_element_t));

    return BCM_E_NONE;
}

STATIC int
_bcm_th2_pstats_session_data_update(int unit,
                                    bcm_pstats_session_element_t element,
                                    bcm_pstats_data_t *data)
{
    bcm_gport_t                 gport;
    bcm_port_t                  local_port;
    bcm_cos_queue_t             cosq;
    bcm_service_pool_id_t       pool;
    int                         index, pen_idx = 0;
    int                         mode, i, xpe, pipe, startq, offset;
    uint8                       *total_buf;
    uint32                      cur_val = 0, *pre_p;
    void                        *pentry;
    soc_info_t                  *si = &SOC_INFO(unit);
    soc_mem_t                   mem;
    soc_field_t                 field;

    mode = PSTATS_CTRL(unit).stat_mode;

    if (element.type == bcmPStatsSessionUnicastQueue) {
        /* update ucq use counts
         * ucq_buf:
         * 0-3   |xpeA-q0|xpeB-q0|
         */
        /* each XPE has two pipes */
        gport = element.gport;
        cosq = element.cosq;
        BCM_IF_ERROR_RETURN(
            _bcm_th_cosq_index_resolve(unit, gport, cosq,
                                       _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &startq, NULL));
        pipe = si->port_pipe[local_port];
        for (xpe = 0, i = 0; xpe < NUM_XPE(unit); xpe++) {

            if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                continue;
            }
            index = TH2_XPE_PIPE_INDEX(unit, xpe, pipe);
            if (index >= TH2_MAX_XPEPIPE) {
                return BCM_E_INTERNAL;
            }
            total_buf = PSTATS_CTRL(unit).ucq_buffer[index].buf;
            mem = PSTATS_CTRL(unit).ucq_buffer[index].mem;
            pen_idx = startq / 4;
            offset = startq % 4;
            pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                  total_buf, pen_idx);
            cur_val = soc_mem_field32_get(unit, mem, pentry, ucq_field[offset]);

            pre_p = &data->use_counts_array[i];
            if (mode) {
                if (cur_val > *pre_p) {
                    *pre_p = cur_val;
                }
            } else {
                *pre_p = cur_val;
            }
            i++;
        }
        data->array_count = i;
    } else {
        /* update pool use counts
         * pool_buf:
         * 0-3   |xpe0-p0|xpe1-p0|xpe2-p0|xpe3-p0|
         */
        pool = element.pool;
        for (xpe = 0, i = 0; xpe < NUM_XPE(unit); xpe++) {

            index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
            if (index >= TH2_MAX_POOL_COUNT) {
                return BCM_E_INTERNAL;
            }
            if (element.type == bcmPStatsSessionIngPool) {
                total_buf = PSTATS_CTRL(unit).ing_pool_buffer[xpe].buf;
                mem = PSTATS_CTRL(unit).ing_pool_buffer[pool].mem;
                field = isp_field[pool];
            } else if (element.type == bcmPStatsSessionEgrPool) {
                total_buf = PSTATS_CTRL(unit).egr_pool_buffer[xpe].buf;
                mem = PSTATS_CTRL(unit).egr_pool_buffer[pool].mem;
                field = esp_field[pool];
            } else {
                return BCM_E_INTERNAL;
            }

            pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                  total_buf, pen_idx);
            cur_val = soc_mem_field32_get(unit, mem, pentry, field);
            pre_p = &data->use_counts_array[i];
            if (mode) {
                if (cur_val > *pre_p) {
                    *pre_p = cur_val;
                }
            } else {
                *pre_p = cur_val;
            }
            i++;
        }
        data->array_count = i;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th2_pstats_session_update(int unit, pstats_session_t *session)
{
    bcm_pstats_session_element_t    element;
    bcm_pstats_data_t               *data;
    pstats_session_t                *cur_session = session;
    uint8                           *total_buf;
    int                             i;
    void                            *pentry;
    soc_mem_t                       mem;

    /*
     * update timestamp
     * if MMU_INTFO_TOD_TIMESTAMP is updated by ARM core,
     * MMU_INTFO_UTC_TIMESTAMP is a utc time.
     * otherwise it is a local time.
     */
    total_buf = PSTATS_CTRL(unit).utc_time.buf;
    mem = PSTATS_CTRL(unit).utc_time.mem;
    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                          total_buf, 0);
    cur_session->timestamp.nanoseconds =
            soc_mem_field32_get(unit, mem, pentry, UTC_TIMESTAMP_NSECf);
    soc_mem_field64_get(unit, mem, pentry, UTC_TIMESTAMP_SECf,
                        &cur_session->timestamp.seconds);

#ifdef BCM_PSTATS_MEASURE
    if (soc_property_get(unit, "pstats_timestamp_info", 0) == 1) {
        COMPILER_64_TO_32_LO(cur_session->timestamp.nanoseconds,
                             PSTATS_CTRL(unit).time_cost);
    }
#endif

    /* update element data */
    for (i = 0; i < cur_session->element_array_count; i++) {
        element = cur_session->element_array[i];
        data = &cur_session->data_array[i];
        BCM_IF_ERROR_RETURN(
            _bcm_th2_pstats_session_data_update(unit, element, data));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_pstats_hw_sync
 * Purpose:
 *      Start a pstats sync operation.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_pstats_hw_sync(int unit)
{
    pstats_buffer_t             *pbuf;
    int                         index, pipe, xpe;
    soc_info_t                  *si = &SOC_INFO(unit);
#ifdef BCM_PSTATS_MEASURE
    void                        *pentry;
    uint64                      start_ts, end_ts;
#endif

    SOC_IF_ERROR_RETURN(soc_pstats_sync(unit));

#ifdef BCM_PSTATS_MEASURE
    /* sync local time */
    pbuf = &PSTATS_CTRL(unit).local_time;
    SOC_IF_ERROR_RETURN(
        soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 0));
    pentry = soc_mem_table_idx_to_pointer(unit, pbuf->mem, void*,
                                          pbuf->buf, 0);
    soc_mem_field64_get(unit, pbuf->mem, pentry, TIMESTAMPf, &start_ts);

    pbuf = &PSTATS_CTRL(unit).local_time_end;
    SOC_IF_ERROR_RETURN(
        soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 1));
    pentry = soc_mem_table_idx_to_pointer(unit, pbuf->mem, void*,
                                          pbuf->buf, 0);
    soc_mem_field64_get(unit, pbuf->mem, pentry, TIMESTAMPf, &end_ts);

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "start 0x%x%8x end 0x%x%8x\n"),
             COMPILER_64_HI(start_ts), COMPILER_64_LO(start_ts),
             COMPILER_64_HI(end_ts), COMPILER_64_LO(end_ts)));

    /* handle rollover */
    if (COMPILER_64_LT(end_ts, start_ts)) {
        uint64 wrap_amt;
        int width;

        width = soc_mem_field_length(unit, MMU_INTFO_TIMESTAMPm, TIMESTAMPf);
        COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
        COMPILER_64_ADD_64(end_ts, wrap_amt);
    }
    COMPILER_64_DELTA(PSTATS_CTRL(unit).time_cost, start_ts, end_ts);
#endif

    /* sync utc time */
    pbuf = &PSTATS_CTRL(unit).utc_time;
    SOC_IF_ERROR_RETURN(
        soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 0));

    /* sync uc queue */
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {

            if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                continue;
            }
            index = TH2_XPE_PIPE_INDEX(unit, xpe, pipe);
            if (index >= TH2_MAX_XPEPIPE) {
                return BCM_E_INTERNAL;
            }
            pbuf = &PSTATS_CTRL(unit).ucq_buffer[index];
            SOC_IF_ERROR_RETURN(
                soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 0));
        }
    }

    /* sync ing pool */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {

        index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
        if (index >= TH2_MAX_POOL_COUNT) {
            return BCM_E_INTERNAL;
        }
        pbuf = &PSTATS_CTRL(unit).ing_pool_buffer[index];
        SOC_IF_ERROR_RETURN(
            soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 0));
    }

    /* sync egr pool */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {

        index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
        if (index >= TH2_MAX_POOL_COUNT) {
            return BCM_E_INTERNAL;
        }
        pbuf = &PSTATS_CTRL(unit).egr_pool_buffer[index];
        SOC_IF_ERROR_RETURN(
            soc_pstats_mem_get(unit, pbuf->mem, pbuf->buf, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th2_pstats_session_data_sync
 * Purpose:
 *      Start a pstats sync operation for a special session
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_pstats_session_data_sync(int unit, pstats_session_t *session)
{
    BCM_IF_ERROR_RETURN(_bcm_th2_pstats_hw_sync(unit));
    BCM_IF_ERROR_RETURN(_bcm_th2_pstats_session_update(unit, session));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_pstats_data_sync
 * Purpose:
 *      Start a pstats sync operation.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_data_sync(int unit)
{
    pstats_session_t    *cur_session = NULL;
    sal_usecs_t         start_time = sal_time_usecs();

    BCM_IF_ERROR_RETURN(_bcm_th2_pstats_hw_sync(unit));
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit, "Time taken for pstats dma: %d usec\n"),
                          SAL_USECS_SUB(sal_time_usecs(), start_time)));
    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        if (cur_session->enable != TRUE) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
                _bcm_th2_pstats_session_update(unit, cur_session));
        cur_session = cur_session->next_session;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_pstats_session_data_get
 * Purpose:
 *      Get an existing PSTATS session data.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create
 *      sync - (IN) Need a sync operation
 *      timestamp - (OUT) time stamp info.
 *      array_max - (IN) Maximum number of elements in array.
 *      data - (OUT) array of use count data.
 *      array_count - (OUT) Number of elements in array.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_data_get(int unit, bcm_pstats_session_id_t session_id,
                                int sync,
                                bcm_pstats_timestamp_t *timestamp,
                                int array_max,
                                bcm_pstats_data_t *data_array,
                                int *array_count)
{
    pstats_session_t        *cur_session = NULL;
    int                     mode;

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_data_get unit %d session_id %d "
                               "sync %d\n"), unit, session_id, sync));

    if (NULL == data_array) {
        LOG_ERROR(BSL_LS_BCM_PSTATS,
                  (BSL_META_U(unit, "data_array is null\n")));
        return BCM_E_PARAM;
    }

    mode = PSTATS_CTRL(unit).stat_mode;
    cur_session = _bcm_th2_pstats_session_get(unit, session_id);
    if (NULL == cur_session) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "pstats_session_data_get: "
                                   "session_id %d is not found\n"), session_id));
        return BCM_E_NOT_FOUND;
    }

    if (array_max >= cur_session->element_array_count) {
        *array_count = cur_session->element_array_count;
    } else if (array_max >= 0){
        *array_count = array_max;
    } else {
        LOG_ERROR(BSL_LS_BCM_PSTATS,
                  (BSL_META_U(unit, "array_max %d is illegal\n"), array_max));
        return BCM_E_PARAM;
    }

    /* if PKT STAT mode is HWM, need to sync up all sessions */
    if (TRUE == sync) {
        if (mode) {
            BCM_IF_ERROR_RETURN(bcm_th2_pstats_data_sync(unit));
        } else {
            BCM_IF_ERROR_RETURN(
                    _bcm_th2_pstats_session_data_sync(unit, cur_session));
        }
    }

    sal_memcpy(data_array, cur_session->data_array,
               *array_count * sizeof(bcm_pstats_data_t));

    sal_memcpy(timestamp, &cur_session->timestamp, sizeof(bcm_pstats_timestamp_t));

    return BCM_E_NONE;
}

STATIC int
_bcm_th2_pstats_session_data_hw_clear(int unit, bcm_pstats_session_element_t element)
{
    bcm_gport_t                 gport;
    bcm_port_t                  local_port;
    bcm_cos_queue_t             cosq;
    bcm_service_pool_id_t       pool;
    int                         index, xpe, pipe, startq;
    soc_info_t                  *si = &SOC_INFO(unit);
    soc_mem_t                   mem;
    soc_field_t                 field;
    uint32                      entry[SOC_MAX_MEM_WORDS];

    if (element.type == bcmPStatsSessionUnicastQueue) {
        /* update ucq use counts
         * ucq_buf:
         * 0-3   |xpeA-q0|xpeB-q0|
         */
        /* each XPE has two pipes */
        gport = element.gport;
        cosq = element.cosq;
        BCM_IF_ERROR_RETURN(
            _bcm_th_cosq_index_resolve(unit, gport, cosq,
                                       _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                       &local_port, &startq, NULL));
        pipe = si->port_pipe[local_port];
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {

            if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                continue;
            }
            index = TH2_XPE_PIPE_INDEX(unit, xpe, pipe);
            if (index >= TH2_MAX_XPEPIPE) {
                return BCM_E_INTERNAL;
            }
            mem = PSTATS_CTRL(unit).ucq_buffer[index].clear_mem;
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
            soc_mem_field32_set(unit, mem, entry, UCQ_COUNTf, 0);
            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        /* update pool use counts
         * pool_buf:
         * 0-3   |xpe0-p0|xpe1-p0|xpe2-p0|xpe3-p0|
         */
        pool = element.pool;
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {

            index = TH2_XPE_PIPE_INDEX(unit, xpe, -1);
            if (index >= TH2_MAX_POOL_COUNT) {
                return BCM_E_INTERNAL;
            }
            if (element.type == bcmPStatsSessionIngPool) {
                mem = PSTATS_CTRL(unit).ing_pool_buffer[xpe].clear_mem;
                field = isp_field[pool];
            } else if (element.type == bcmPStatsSessionEgrPool) {
                mem = PSTATS_CTRL(unit).egr_pool_buffer[xpe].clear_mem;
                field = esp_field[pool];
            } else {
                return BCM_E_INTERNAL;
            }
            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0, entry));
            soc_mem_field32_set(unit, mem, entry, field, 0);
            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_pstats_session_data_clear
 * Purpose:
 *      Clear an existing PSTATS session hw use counts and sw use counts buffer.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      session_id - (IN) id of a PSTATS session created via the
 *                        bcm_pstats_session_create.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_data_clear(int unit, bcm_pstats_session_id_t session_id)
{
    bcm_pstats_session_element_t    element;
    pstats_session_t                *cur_session = NULL;
    int                             i, need_hw_clear = 1;

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_data_clear unit %d session_id %d\n"),
                               unit, session_id));

    cur_session = _bcm_th2_pstats_session_get(unit, session_id);
    if (NULL == cur_session) {
        LOG_INFO(BSL_LS_BCM_PSTATS,
                 (BSL_META_U(unit, "pstats_session_data_clear: "
                                   "session_id %d is not found\n"), session_id));
        return BCM_E_NOT_FOUND;
    }

    /* clear HW use count buffer */
    if ((PSTATS_CTRL(unit).stat_mode == 1) &&
        (PSTATS_CTRL(unit).hw_cor_in_max_use_count == 1)) {
        /* HW clear on read is enabled, no need do hw clear */
        need_hw_clear = 0;
    }

    if (need_hw_clear == 1) {
        for (i = 0; i < cur_session->element_array_count; i++) {
            element = cur_session->element_array[i];
            BCM_IF_ERROR_RETURN(
                _bcm_th2_pstats_session_data_hw_clear(unit, element));
        }
    }

    /* clear SW use count buffer */
    sal_memset(cur_session->data_array, 0,
               cur_session->element_array_count * sizeof(bcm_pstats_data_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th2_pstats_session_traverse
 * Purpose:
 *      Traverse through the pstats sessions and run callback at each session.
 * Parameters:
 *      unit - (IN) StrataSwitch unit number.
 *      cb - (IN) bcm_pstats_session_traverse_cb.
 *      user_data - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th2_pstats_session_traverse(int unit, bcm_pstats_session_traverse_cb cb,
                                void *user_data)
{
    bcm_pstats_session_element_t    element_array[TH2_MAX_ELEMENT_COUNT];
    bcm_pstats_session_id_t         session_id;
    pstats_session_t                *cur_session = NULL;
    int                             array_count;
    int                             rv = BCM_E_NONE;

    LOG_INFO(BSL_LS_BCM_PSTATS,
             (BSL_META_U(unit, "pstats_session_travers unit %d\n"), unit));

    cur_session = PSTATS_CTRL(unit).first_session;
    while (NULL != cur_session) {
        session_id = cur_session->session_id;
        array_count = cur_session->element_array_count;
        sal_memcpy(element_array, cur_session->element_array,
                   array_count * sizeof(bcm_pstats_session_element_t));

        cur_session = cur_session->next_session;

        rv = cb(unit, session_id, array_count, element_array, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        /*
         * If CB_ABORT_ON_ERR is not defined don't return error
         * and continue.
         */
        rv = BCM_E_NONE;
#endif
    }

    return rv;
}
#else  /* INCLUDE_PSTATS && BCM_TOMAHAWK2_SUPPORT */
typedef int bcm_esw_tomahawk2_pstats_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PSTATS && BCM_TOMAHAWK2_SUPPORT */

