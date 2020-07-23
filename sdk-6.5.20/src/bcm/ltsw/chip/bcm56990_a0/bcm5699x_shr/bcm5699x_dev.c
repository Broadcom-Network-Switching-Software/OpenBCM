/*! \file bcm5699x_dev.c
 *
 * BCM5699x DEV subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/common/family.h>

#include <bcm_int/ltsw/mbcm/dev.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Parse port bitmap information.
 *
 * \param [in] unit Unit Number.
 * \param [in] pbmp Port bitmap .
 * \param [out] port_num Number of ports.
 * \param [out] port_max Maximum port.
 *
 * \retval None
 */
static void
dev_port_bmp_parse(int unit, bcm_pbmp_t *pbmp, int *port_num, int *port_max)
{
    int port = 0;
    int num_port = 0;
    int max_port = 0;

    BCM_PBMP_ITER(*pbmp, port) {
        num_port++;
        if (port > max_port) {
            max_port = port;
        }
    }

    *port_num = num_port;
    *port_max = max_port;

    return;
}

/*!
 * \brief Get the information of physical ports.
 *
 * \param [in] unit Unit Number.
 * \param [out] port_num Number of physical ports.
 * \param [out] port_max Maximum physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dev_phys_port_get(int unit, int *port_num, int *port_max)
{
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_phys_pbmp(unit, &pbmp));

    dev_port_bmp_parse(unit, &pbmp, port_num, port_max);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the information of logical ports.
 *
 * \param [in] unit Unit Number.
 * \param [out] port_num Number of logical ports.
 * \param [out] port_max Maximum logical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dev_logic_port_get(int unit, int *port_num, int *port_max)
{
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_logic_pbmp(unit, &pbmp));

    dev_port_bmp_parse(unit, &pbmp, port_num, port_max);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse pipe bitmap information.
 *
 * \param [in] unit Unit Number.
 * \param [in] pipe_bmp Pipe bitmap.
 * \param [out] port_num Number of pipes.
 * \param [out] port_max Maximum pipe.
 *
 * \retval None
 */
static void
dev_pipe_bmp_parse(int unit, uint32_t pipe_bmp, int *pipe_num, int *pipe_max)
{
    int i = 0;
    int num_pipe = 0;
    int max_pipe = 0;

    while (pipe_bmp) {
        if (pipe_bmp & 1) {
            num_pipe++;
            max_pipe = i;
        }
        pipe_bmp >>= 1;
        i++;
    }

    *pipe_num = num_pipe;
    *pipe_max = max_pipe;

    return;
}

/*!
 * \brief Get the information of pipes.
 *
 * \param [in] unit Unit Number.
 * \param [out] pipe_num Number of pipes.
 * \param [out] pipe_max Maximum pipe.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dev_pipe_get(int unit, int *pipe_num, int *pipe_max)
{
    uint32_t pipes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pipe_bmp(unit, &pipes));

    dev_pipe_bmp_parse(unit, pipes, pipe_num, pipe_max);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the information of PP pipes.
 *
 * \param [in] unit Unit Number.
 * \param [out] pipe_num Number of PP pipes.
 * \param [out] pipe_max Maximum PP pipe.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dev_pp_pipe_get(int unit, int *pipe_num, int *pipe_max)
{
    uint32_t pipes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pp_pipe_bmp(unit, &pipes));

    dev_pipe_bmp_parse(unit, pipes, pipe_num, pipe_max);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get physical device level controls.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  core_frequency  Core clock frequency.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
ltsw_dev_config_get(int unit, int *core_frequency)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *sym_val = NULL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_CONFIGs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, CORE_CLK_FREQs,
                                      &sym_val));

    if (sal_strcasecmp(sym_val, CLK_1325MHZs) == 0) {
        /* Clock frequency 1325Mhz. */
        *core_frequency = 1325;
    } else if (sal_strcasecmp(sym_val, CLK_1025MHZs) == 0) {
        /* Clock frequency 1025Mhz. */
        *core_frequency = 1025;
    } else if (sal_strcasecmp(sym_val, CLK_950MHZs) == 0) {
        /* Clock frequency 950Mhz. */
        *core_frequency = 950;
    } else {
        /* Clock frequency 1325Mhz. */
        *core_frequency = 1325;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_dev_info_init(int unit, bcmint_dev_info_t *dev_info)
{
    int core_frequency = 0;

    SHR_FUNC_ENTER(unit);

    dev_info->cmic_port = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (dev_phys_port_get(unit,
                           &(dev_info->phys_port_num),
                           &(dev_info->phys_port_max)));

    SHR_IF_ERR_VERBOSE_EXIT
        (dev_logic_port_get(unit,
                            &(dev_info->logic_port_num),
                            &(dev_info->logic_port_max)));

    SHR_IF_ERR_VERBOSE_EXIT
        (dev_pipe_get(unit,
                      &(dev_info->pipe_num),
                      &(dev_info->pipe_max)));

    SHR_IF_ERR_VERBOSE_EXIT
        (dev_pp_pipe_get(unit,
                         &(dev_info->pp_pipe_num),
                         &(dev_info->pp_pipe_max)));

    dev_info->port_addr_max = 511;
    dev_info->modid_max = 0;
    dev_info->modid_count = 1;
    dev_info->modport_max = 8;
    dev_info->modport_max_first = 7;
    dev_info->family = BCM_FAMILY_TRIDENT;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_dev_config_get(unit, &core_frequency));
    dev_info->core_frequency = core_frequency;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dev sub driver functions for bcm5699x.
 */
static mbcm_ltsw_dev_drv_t bcm5699x_dev_sub_drv = {
    .dev_info_init = bcm5699x_ltsw_dev_info_init,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_dev_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_dev_drv_set(unit, &bcm5699x_dev_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
