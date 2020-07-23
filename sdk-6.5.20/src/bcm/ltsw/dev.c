/*! \file dev.c
 *
 * Device information of SDKLT-based devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/cm.h>
#include <bcm/types.h>
#include <bcm_int/common/family.h>

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcm_int/ltsw/dev_int.h>
#include <bcm_int/ltsw/mbcm/dev.h>
#include <bcm_int/ltsw/property.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

typedef struct ltsw_dev_s {
    /*! BCMDRD device handle. */
    int dunit;

    /*! Vendor ID. */
    uint16_t vendor_id;

    /*! Device ID. */
    uint16_t dev_id;

    /*! Device revision. */
    uint16_t rev_id;

    /*! Bit map of valid physical ports in the device. */
    bcm_pbmp_t valid_phys_ports;

    /*! Bit map of valid logic ports in the device. */
    bcm_pbmp_t valid_logic_ports;

    /*! Bit map of valid pipes in the device. */
    uint32_t valid_pipes;

    /*! Bit map of valid pp pipes in the device. */
    uint32_t valid_pp_pipes;

    /*! Error handling of user callback. */
    int cb_err_abort;

    /*! Device info. */
    bcmint_dev_info_t dev_info;

} ltsw_dev_t;

/*! Device info of SDKLT-based devices. */
static ltsw_dev_t ltsw_dev[BCM_MAX_NUM_UNITS] = {{0}};

/*! Mapping from BCMDRD unit to BCM unit. */
static int ltsw_dunit2unit[BCMDRD_CONFIG_MAX_UNITS] = {-1};

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the device information for specified unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_dev_init(int unit)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];
    bcmdrd_dev_id_t id;
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_chip_info_t *cinfo;
    int dunit, p;
    int blktype;

    SHR_FUNC_ENTER(unit);

    /* Clean up the device information structure. */
    sal_memset(dev, 0, sizeof(ltsw_dev_t));
    dev->dunit = -1;

    SHR_IF_ERR_EXIT
        (soc_cm_dev_num_get(unit, &dunit));
    dev->dunit = dunit;
    ltsw_dunit2unit[dunit] = unit;

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_id_get(dunit, &id));
    dev->vendor_id = id.vendor_id;
    dev->dev_id = id.device_id;
    dev->rev_id = id.revision;

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_logic_pbmp(dunit, &pbmp));
    BCMDRD_PBMP_ITER(pbmp, p) {
        if (p >= BCM_PBMP_PORT_MAX) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        BCM_PBMP_PORT_ADD(dev->valid_logic_ports, p);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_phys_pbmp(dunit, &pbmp));
    BCMDRD_PBMP_ITER(pbmp, p) {
        if (p >= BCM_PBMP_PORT_MAX) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        BCM_PBMP_PORT_ADD(dev->valid_phys_ports, p);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(dunit, &dev->valid_pipes));

    cinfo = bcmdrd_dev_chip_info_get(unit);
    blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "ipipe");

    if (blktype >= 0) {
        SHR_IF_ERR_EXIT
            (bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype,
                                            &dev->valid_pp_pipes));
    }

    dev->cb_err_abort =
        bcmi_ltsw_property_get(unit, BCMI_CPN_CB_ABORT_ON_ERR, 0);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_info_init(int unit)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_dev_info_init(unit, &dev->dev_info));

    SHR_IF_ERR_EXIT
        (bcm_chip_family_set(unit, dev->dev_info.family));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_dunit(int unit)
{
    return (ltsw_dev[unit].dunit);
}

int
bcmi_ltsw_dev_unit(int dunit)
{
    if (dunit < 0 || dunit >= BCMDRD_CONFIG_MAX_UNITS) {
        return -1;
    }

    return ltsw_dunit2unit[dunit];
}

int
bcmi_ltsw_dev_exists(int unit)
{
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS || ltsw_dev[unit].dunit < 0 ||
        !bcmdrd_dev_exists(ltsw_dev[unit].dunit)) {
        return false;
    }
    return true;
}

int
bcmi_ltsw_dev_vendor_id(int unit)
{
    return ltsw_dev[unit].vendor_id;
}

int
bcmi_ltsw_dev_device_id(int unit)
{
    return ltsw_dev[unit].dev_id;
}

int
bcmi_ltsw_dev_revision_id(int unit)
{
    return ltsw_dev[unit].rev_id;
}

int
bcmi_ltsw_dev_cb_err_abort(int unit)
{
    return (ltsw_dev[unit].cb_err_abort);
}

int
bcmi_ltsw_dev_logic_port_num(int unit)
{
    return (ltsw_dev[unit].dev_info.logic_port_num);
}

int
bcmi_ltsw_dev_logic_port_max(int unit)
{
    return (ltsw_dev[unit].dev_info.logic_port_max);
}

int
bcmi_ltsw_dev_phys_port_num(int unit)
{
    return (ltsw_dev[unit].dev_info.phys_port_num);
}

int
bcmi_ltsw_dev_phys_port_max(int unit)
{
    return (ltsw_dev[unit].dev_info.phys_port_max);
}

int
bcmi_ltsw_dev_max_pipe_num(int unit)
{
    return (ltsw_dev[unit].dev_info.pipe_num);
}

int
bcmi_ltsw_dev_pipe_max(int unit)
{
    return (ltsw_dev[unit].dev_info.pipe_max);
}

int
bcmi_ltsw_dev_pipe_bmp(int unit, uint32_t *pipe_bmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_bmp, SHR_E_PARAM);

    *pipe_bmp = dev->valid_pipes;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_max_pp_pipe_num(int unit)
{
    return (ltsw_dev[unit].dev_info.pp_pipe_num);
}

int
bcmi_ltsw_dev_max_packet_buffer_num(int unit)
{
    return (ltsw_dev[unit].dev_info.packet_buffer_num);
}

int
bcmi_ltsw_dev_pp_pipe_max(int unit)
{
    return (ltsw_dev[unit].dev_info.pp_pipe_max);
}

int
bcmi_ltsw_dev_pp_pipe_bmp(int unit, uint32_t *pipe_bmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pipe_bmp, SHR_E_PARAM);

    *pipe_bmp = dev->valid_pp_pipes;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_max_modid(int unit)
{
    return (ltsw_dev[unit].dev_info.modid_max);
}

int
bcmi_ltsw_dev_modid_count(int unit)
{
    return (ltsw_dev[unit].dev_info.modid_count);
}

int
bcmi_ltsw_dev_modport_max_first(int unit)
{
    return (ltsw_dev[unit].dev_info.modport_max_first);
}

int
bcmi_ltsw_dev_max_port_addr(int unit)
{
    return (ltsw_dev[unit].dev_info.port_addr_max);
}

int
bcmi_ltsw_dev_max_modport(int unit)
{
    return (ltsw_dev[unit].dev_info.modport_max);
}

int
bcmi_ltsw_dev_cmic_port(int unit)
{
    return (ltsw_dev[unit].dev_info.cmic_port);
}

int
bcmi_ltsw_dev_core_clk_freq(int unit)
{
    return (ltsw_dev[unit].dev_info.core_frequency);
}

int
bcmi_ltsw_dev_logic_port_pipe(int unit, int port)
{
    return bcmdrd_dev_logic_port_pipe(ltsw_dev[unit].dunit, port);
}

int
bcmi_ltsw_dev_phys_port_pipe(int unit, int port)
{
    return bcmdrd_dev_phys_port_pipe(ltsw_dev[unit].dunit, port);
}

int
bcmi_ltsw_dev_logic_port_pp_pipe(int unit, int port)
{
    int pipe;

    (void)mbcm_ltsw_dev_logic_port_pp_pipe(unit, port, &pipe);

    return pipe;
}

int
bcmi_ltsw_dev_logic_port_pp_pipe_port(int unit, int port)
{
    int pp_port;

    (void)mbcm_ltsw_dev_logic_port_pp_pipe_port(unit, port, &pp_port);

    return pp_port;
}

int
bcmi_ltsw_dev_pp_pipe_port_logic_port(int unit, int pipe, int pp_port)
{
    int port;

    (void)mbcm_ltsw_dev_pp_pipe_port_logic_port(unit, pipe, pp_port, &port);

    return port;
}

int
bcmi_ltsw_dev_phys_pbmp(int unit, bcm_pbmp_t *pbmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    sal_memcpy(pbmp, &dev->valid_phys_ports, sizeof(bcm_pbmp_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_pipe_phys_pbmp(int unit, int pipe, bcm_pbmp_t *pbmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];
    bcm_port_t p;
    int pipe_no;

    SHR_FUNC_ENTER(unit);

    if (pipe < 0 || pipe > dev->dev_info.pipe_max) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(dev->valid_pipes & (1 << pipe))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_ITER(dev->valid_phys_ports, p) {
        pipe_no = bcmdrd_dev_phys_port_pipe(dev->dunit, p);
        if (pipe_no == pipe) {
            BCM_PBMP_PORT_ADD(*pbmp, p);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_logic_pbmp(int unit, bcm_pbmp_t *pbmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    sal_memcpy(pbmp, &dev->valid_logic_ports, sizeof(bcm_pbmp_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_pipe_logic_pbmp(int unit, int pipe, bcm_pbmp_t *pbmp)
{
    ltsw_dev_t *dev = &ltsw_dev[unit];
    bcm_port_t p;
    int pipe_no;

    SHR_FUNC_ENTER(unit);

    if (pipe < 0 || pipe > dev->dev_info.pipe_max) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(dev->valid_pipes & (1 << pipe))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_ITER(dev->valid_logic_ports, p) {
        pipe_no = bcmdrd_dev_logic_port_pipe(dev->dunit, p);
        if (pipe_no == pipe) {
            BCM_PBMP_PORT_ADD(*pbmp, p);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_dev_pp_pipe_logic_pbmp(int unit, int pipe, bcm_pbmp_t *pbmp)
{
    return mbcm_ltsw_dev_pp_pipe_logic_pbmp(unit, pipe, pbmp);
}

