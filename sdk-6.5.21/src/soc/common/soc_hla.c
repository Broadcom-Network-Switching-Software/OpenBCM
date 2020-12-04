/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/util.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_HLA_SUPPORT)
#include <soc/mcm/cmicx.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/mcm/memregs.h>
#include <soc/intr.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/iproc.h>
#include <soc/cmicx.h>
#include <shared/hla/hla.h>

#define SOC_HLA_POLL_WAIT_TIMEOUT 5000000 /* 5sec */
#define SOC_HLA_POLL_WAIT_TIMEOUT_QT 20000000 /* 20 sec */
#define SOC_HLA_POLL_MIN 100

#define HLA_REG_FIELD_GET(reg, val, field) \
              (val >> reg##_##field##_SHIFT) & ((1 << reg##_##field##_WIDTH) -1)
/*
 * Function:
 *     soc_hla_ccf_load
 * Purpose:
 *     Copies CCF (License file) to HLA SRAM
 * Parameters:
 *     unit Unit number
 *     data Firmware array.
 *     len Length in bytes.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_hla_ccf_load(int unit, const uint8 *data, int len)
{
    uint32 addr,data32, *data_p, status, error, error2, state;
    int i;
    int wlen = (len + 3) & 0xFFFFFFFC;
    scha_hdr_t scha;
    soc_timeout_t to;
    uint32 max_allowed_len;
    uint32 timeout = (SAL_BOOT_QUICKTURN) ? SOC_HLA_POLL_WAIT_TIMEOUT_QT : SOC_HLA_POLL_WAIT_TIMEOUT;
    int rv = SOC_E_NONE;

    /* Check CCF file size */
    if((sizeof(scha_hdr_t) + len) > HLA_AUTH_DATA_SIZE_MAX) {
        max_allowed_len = HLA_AUTH_DATA_SIZE_MAX - sizeof(scha_hdr_t);
        LOG_CLI((BSL_META_U(unit, "CCF blob size %d bytes, exceeds max allowed %u"),
                    len, max_allowed_len));
        return SOC_E_PARAM;
    }

    sal_memset(&scha, 0, sizeof(scha_hdr_t));
    scha.id = SCHA_IDENTIFIER;
    scha.len = len;

    /* Copy scha to HLA SRAM */
    wlen = sizeof(scha_hdr_t) / sizeof(uint32);
    addr = HLA_LIC_OFFSET;  /* load address */
    data_p = (uint32 *)&scha;
    for (i = 0; i < wlen; i++) {
        soc_iproc_setreg(unit, addr + i*sizeof(uint32), *data_p);
        data_p++;
    }

    /* Load CCF data to SRAM */
    addr = HLA_LIC_OFFSET  + sizeof(scha_hdr_t); /* load address */
    for (i = 0; i < len; i += 4) {
        data32 = (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
#if defined(BE_HOST)
        _shr_swap32(data32);
#endif
        soc_iproc_setreg(unit, addr + i, data32);
    }

    /* Set the trigger */
    soc_iproc_getreg(unit, HLA_USR_CTRL_STATr, &data32);
    data32 |= 1 << HLA_USR_CTRL_STAT_START_SHIFT;
    soc_iproc_setreg(unit, HLA_USR_CTRL_STATr, data32);
    READ_IPROC_WRAP_HLA_INTR_CONTROLr(unit, &data32);
    data32 |= 0x1;
    WRITE_IPROC_WRAP_HLA_INTR_CONTROLr(unit, data32);

    sal_msleep(10);

    
    /* Wait for loading done, polling or interrupt */
    soc_timeout_init(&to, timeout, SOC_HLA_POLL_MIN);
    do {
        soc_iproc_getreg(unit, HLA_USR_CTRL_STATr, &status);
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            LOG_CLI((BSL_META_U(unit,
              " HLA Status 0x%x after timeout in %d polls\n"), status, to.polls));
            break;
        }

    } while((status & (1 << HLA_USR_CTRL_STAT_DONE_SHIFT)) == 0);

    if(rv == SOC_E_NONE) {
        error = (status >> HLA_USR_CTRL_STAT_ERROR_SHIFT) & ((1 << HLA_USR_CTRL_STAT_ERROR_WIDTH) - 1);
        if(error) {
            soc_iproc_getreg(unit, HLA_SEC_ERRr, &error2);
            soc_iproc_getreg(unit, HLA_STATE_PREVr, &state);
            LOG_CLI((BSL_META_U(unit,
              " HLA License loading failed, \n\tStatus: 0x%08x\n\tError:0x%04x\n\tSecondary error: 0x%08x\n\tState: 0x%08x\n"),
                                                status, error, error2, state));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "HLA License loaded OK\n")));
        }
    }

    return rv;
}

int
soc_hla_status(int unit, hla_status_t *hla_status)
{
    int rv = SOC_E_NONE;
    uint32 rval;

    soc_iproc_getreg(unit, HLA_FW_VERSIONr, &hla_status->fw_ver);

    soc_iproc_getreg(unit, soc_reg_addr(unit, HLA_PUBLIC_STATUSr, REG_PORT_ANY, 0), &rval);
    hla_status->reset = soc_reg_field_get(unit, HLA_PUBLIC_STATUSr, rval, SYSRESETREQf);

    soc_iproc_getreg(unit, HLA_USR_CTRL_STATr, &rval);
    hla_status->busy = HLA_REG_FIELD_GET(HLA_USR_CTRL_STAT, rval, BUSY);
    hla_status->error = HLA_REG_FIELD_GET(HLA_USR_CTRL_STAT, rval, ERROR);

    soc_iproc_getreg(unit, HLA_SEC_ERRr, &hla_status->error2);

    soc_iproc_getreg(unit, HLA_STATE_CURr, &rval);
    hla_status->cur_run = HLA_REG_FIELD_GET(HLA_STATE_CUR, rval, ITERATION);
    hla_status->cur_state = HLA_REG_FIELD_GET(HLA_STATE_CUR, rval, STATE);

    soc_iproc_getreg(unit, HLA_STATE_PREVr, &rval);
    hla_status->prev_state = HLA_REG_FIELD_GET(HLA_STATE_CUR, rval, STATE);

    soc_iproc_getreg(unit, HLA_DBG_CHIP_ID_L_OTPr, &hla_status->chipid_l);
    soc_iproc_getreg(unit, HLA_DBG_CHIP_ID_H_OTPr, &hla_status->chipid_h);
    return rv;
}

#endif /* BCM_ESW_SUPPORT && BCM_HLA_SUPPORT */
