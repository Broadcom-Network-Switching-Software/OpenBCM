/** \file kbp_xpt.c
 *
 * Functions for handling Jericho2 KBP xpt layer.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/boot.h>

#if defined(INCLUDE_KBP)
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_xpt.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define   DNXXPT_EMPTY_RTQ     (0)
#define   DNXXPT_MAGIC_NUMBER  (0xe11ad)

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];
/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**
 * \brief
 *  Parse the compare data from raw data.
 * \param [in] cmp_data - compare raw data.
 * \param [out] result - compare result.
 * \return
 *   void
 * \remark
 *   None.
 * \see
 *   None.
 */
void
dnx_kbp_fill_rop_compare_result(
    uint8_t * cmp_data,
    struct kbp_search_result *result)
{
    uint32 index = 0, i, j = 0;
    uint32 ad_size = 0;
    uint8 sts = cmp_data[index++];

    for (i = 0; i < KBP_INSTRUCTION_MAX_RESULTS; i++)
    {
        j = 0;
        if ((sts & (1 << (KBP_INSTRUCTION_MAX_RESULTS - (i + 1)))))
        {
            result->hit_or_miss[i] = KBP_HIT;
        }

        if (result->resp_type[i] == 0)
        {
            result->hit_index[i] = (cmp_data[index++] << 16);
            result->hit_index[i] |= (cmp_data[index++] << 8);
            result->hit_index[i] |= cmp_data[index++];
        }
        else
        {
            ad_size = result->resp_type[i];
            if (ad_size > KBP_INDEX_AND_256B_AD)
            {
                ad_size = ad_size - KBP_INDEX_AND_256B_AD;
            }
            if (ad_size == KBP_INDEX_AND_256B_AD)
            {
                ad_size++;
            }
            while (ad_size)
            {
                result->assoc_data[i][j] = cmp_data[index];
                index++;
                j++;
                ad_size--;
            }
        }
    }
}
/**
 * \brief
 *   Free the request queue.
 * \param [in] unit - The unit number.
 * \param [in] alloc - NLM allocator handler.
 * \param [in] rq - request queue handler.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static shr_error_e
dnx_kbp_xpt_free_reqeust_queue(
    int unit,
    NlmCmAllocator * alloc,
    NlmXptRqtQue * rq)
{
    nlm_u32 rqtcnt;
    NlmDnxXptRqt *rqt, *rqtnext;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc, _SHR_E_NONE, "kbp alloc");
    SHR_NULL_CHECK(rq, _SHR_E_NONE, "request queue");

    rqtcnt = rq->count;

    if (rqtcnt == 0)
    {
        SHR_EXIT();
    }

    rqt = rq->head;

    while (rqt)
    {
        rqtnext = rqt->next;
        NlmCmAllocator__free(alloc, rqt);
        rqt = rqtnext;
    }

    rq->count = DNXXPT_EMPTY_RTQ;
    rq->head = rq->tail = NULL;
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief
 *   This function is used by KBP SDK.
 *   It is used for send cmp instruction via ROP packets.
 * \param [in] handle - Transport date base.
 * \param [in] ltr - The LTR number.
 * \param [in] ctx - context buffer ID.
 * \param [in] key - compare key.
 * \param [in] key_len - key length.
 * \param [in] result - search result.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

kbp_status
dnx_op_search(
    void *handle,
    uint32_t ltr,
    uint32_t ctx,
    const uint8_t * key,
    uint32_t key_len,
    struct kbp_search_result *result)
{
    NlmDnxXpt *dnxxpt;
    dnx_kbp_rop_cbw_cmp_t cbw_cmp_data;
    int res;
    const uint8_t *data_ptr;

    if ((handle == NULL) || (key == NULL) || (result == NULL))
    {
        return NLMRSC_INVALID_PARAM;
    }

    dnxxpt = handle;

    sal_memset(&cbw_cmp_data, 0x0, sizeof(dnx_kbp_rop_cbw_cmp_t));

    data_ptr = key;

    cbw_cmp_data.type = NLM_DNX_CB_INST_CMP1;
    cbw_cmp_data.ltr = ltr;
    cbw_cmp_data.opcode = ltr + 1;
    cbw_cmp_data.cbw_data.data_len = key_len;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("%s(): ctx=%d, opcode=%d, LTR=%d, data_len=%d"), FUNCTION_NAME(), ctx, cbw_cmp_data.opcode,
                 cbw_cmp_data.ltr, key_len));

    /*
     * Construct the CB Data based on datalen 
     */
    DNX_KBP_ROP_REVERSE_DATA(data_ptr, cbw_cmp_data.cbw_data.data, cbw_cmp_data.cbw_data.data_len);

    res = dnx_kbp_rop_cbw_cmp(dnxxpt->unit, dnxxpt->core, &cbw_cmp_data);

    if (res != _SHR_E_NONE)
    {
        return NLMRSC_OPR_FAILURE;
    }

    dnx_kbp_fill_rop_compare_result(cbw_cmp_data.result.data_raw, result);

    return KBP_OK;
}
/**
 * \brief
 *   Get the KBP XPT layer handle. This function is
 *   use to be compatible with legacy KBP devices.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] alloc - kbp allocate function.
 * \param [out] handle - transport layer database.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_xpt_handle_get(
    int unit,
    int core,
    struct kbp_allocator * alloc,
    void *handle)
{
    NlmDnxXpt *dnxxpt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc, _SHR_E_PARAM, "kbp alloc");
    SHR_NULL_CHECK(handle, _SHR_E_PARAM, "kbp xpt handle");

    dnxxpt = alloc->xcalloc(alloc->cookie, 1, sizeof(NlmDnxXpt));
    dnxxpt->unit = unit;
    dnxxpt->core = core;
    dnxxpt->m_max_rqt_count = 0;
    dnxxpt->m_max_rslt_count = 0;
    dnxxpt->xpt = 0;
    dnxxpt->dnxxpt_magic = DNXXPT_MAGIC_NUMBER;

    handle = dnxxpt;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Free legacy KBP XPT handle.
 *
 * \param [in] unit - The unit number.
 * \param [in] self - transport layer database.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_xpt_handle_free(
    int unit,
    void *self)
{
    NlmXpt *xpt;
    NlmDnxXpt *dnxxpt = (NlmDnxXpt *) self;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Nothing needs to do if self is NULL 
     */
    SHR_NULL_CHECK(self, _SHR_E_PARAM, "self");

    xpt = dnxxpt->xpt;

    SHR_IF_ERR_EXIT(dnx_kbp_xpt_free_reqeust_queue(unit, dnxxpt->m_alloc_p, &dnxxpt->m_in_use_requests));
    SHR_IF_ERR_EXIT(dnx_kbp_xpt_free_reqeust_queue(unit, dnxxpt->m_alloc_p, &dnxxpt->m_free_requests));

    if (dnxxpt->m_single_rqt_p)
    {
        NlmCmAllocator__free(dnxxpt->m_alloc_p, dnxxpt->m_single_rqt_p);
    }
    /*
     * free xpt first, as the dnxxpt hold the xpt pointer 
     */
    if (xpt)
    {
        NlmCmAllocator__free(xpt->m_alloc, xpt);
    }

    if (dnxxpt)
    {
        NlmCmAllocator__free(dnxxpt->m_alloc_p, dnxxpt);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Init KBP transport layer - PCIe init.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_xpt_init(
    int unit)
{
    uint32 flags = KBP_DEVICE_DEFAULT;
    int kbp_inst;
    uint32 device_no;

    SHR_FUNC_INIT_VARS(unit);

    if ((!soc_sand_is_emulation_system(unit)) && (!SAL_BOOT_PLISIM))
    {
        /*
         * PCIe init
         */
        if (sw_state_is_warm_boot(unit))
        {
            flags |= KBP_DEVICE_SKIP_INIT;
        }
        if (dnx_data_elk.application.thread_safety_get(unit) && (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2))
        {
            flags |= KBP_DEVICE_THREAD_SAFETY;
        }
        DNX_KBP_INST_ITER(unit, kbp_inst)
        {
            device_no = DNX_KBP_IS_SECOND_KBP_EXIST ? (kbp_inst + unit * DNX_KBP_VALID_KBP_PORTS_NUM) : unit;
            DNX_KBP_TRY(kbp_pcie_init
                        (Kbp_app_data[unit]->device_type, flags, device_no, Kbp_app_data[unit]->dalloc_p[kbp_inst],
                         &Kbp_app_data[unit]->device_config[kbp_inst], NULL, &Kbp_app_data[unit]->xpt_p[kbp_inst]));
        }

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   De-init KBP transport layer - Destory PCIe.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_xpt_deinit(
    int unit)
{
    int kbp_inst;
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        if ((Kbp_app_data[unit] == NULL) || (Kbp_app_data[unit]->dalloc_p[kbp_inst] == NULL)
            || (Kbp_app_data[unit]->xpt_p[kbp_inst] == NULL))
        {
            /*
             * Nothing needs to deinit, exit directly.
             */
            SHR_EXIT();
        }

        /*
         * Destory PCIe
         */
        if (dnx_kbp_interface_enabled(unit) && dnx_kbp_device_enabled(unit) && !soc_sand_is_emulation_system(unit))
        {
            DNX_KBP_TRY(kbp_pcie_destroy(Kbp_app_data[unit]->dalloc_p[kbp_inst], Kbp_app_data[unit]->xpt_p[kbp_inst]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
