/*! \file bcma_tecsup_header.c
 *
 * CLI 'techsupport' welcome message.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_date.h>

#include <shr/shr_pb.h>
#include <shr/shr_crc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_clicmd_version.h>

#include <bcma/tecsup/bcma_tecsup_drv.h>
#include <bcma/tecsup/bcma_tecsup_header.h>


#define BSL_LOG_MODULE BSL_LS_APPL_TECSUP


/*******************************************************************************
 * Local defines
 */

static const char *tecsup_septal_line =
"*****************************************************************************";

#define TECSUP_HPT_WORDS 6
#define TECSUP_HPT_SIZE 64

typedef struct tecsup_htp_s {
    char data[TECSUP_HPT_SIZE];
} tecsup_htp_t;

/*! Device specific attach function type. */
typedef int (*bcma_tecsup_dev_data_get_f)(int unit, uint32_t sel,
                                          bcma_tecsup_dev_data_t *data);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_bcma_tecsup_dev_data_get },
static struct {
    bcma_tecsup_dev_data_get_f dev_data_get;
} dev_list[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the high precision timer source.
 */
static uint32_t
tecsup_hpt_src_get(int unit, uint32_t clk, uint32_t sel)
{
    bcmdrd_dev_type_t dev_type;
    bcma_tecsup_dev_data_t data;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return 0;
    }

    sal_memset(&data, 0, sizeof(data));
    if (SHR_FAILURE(dev_list[dev_type].dev_data_get(unit, sel, &data))) {
        return 0;
    }

    return data.src;
}

/*!
 * \brief Calibrate the high precision timer.
 */
static uint32_t
tecsup_hpt_cal(int unit, uint32_t clk, uint32_t *meta)
{
    uint32_t idx, t, adj = 0;

    t = meta[1];
    for (idx = 0; idx < TECSUP_HPT_WORDS; idx++) {
        adj = adj ^ meta[idx];
        if (idx > 0 && idx < TECSUP_HPT_WORDS - 1) {
            meta[idx] = meta[idx + 1];
        }
    }

    adj ^= clk ^ t;
    if (adj != t) {
        sal_memset(meta, 0, sizeof(*meta) * TECSUP_HPT_WORDS);
        sal_memcpy(meta, &clk, sizeof(clk));
    }

    return adj;
}

/*!
 * \brief Get the high precision timer data.
 */
static int
tecsup_hpt_get(int unit, uint32_t clk, tecsup_htp_t *hpt)
{
    int rv, idx = 0, bdx = 0;
    uint32_t data[TECSUP_HPT_WORDS] = { 0x12611000, 0x2, 0x20, 0x10, 0xe, 0x8 };
    uint32_t src, raw[TECSUP_HPT_WORDS];
    uint8_t rawb[(TECSUP_HPT_WORDS - 1) * 4];

    sal_memset(hpt, 0, sizeof(*hpt));
    sal_memset(raw, 0, sizeof(raw));

    src = tecsup_hpt_src_get(unit, clk, data[0]);
    raw[idx++] = clk;
    for (; idx < COUNTOF(raw) && src > 0; idx++) {
        rv = BCMDRD_DEV_READ32(unit, src + ((idx - 1) << data[1]), &raw[idx]);
        if (SHR_FAILURE(rv)) {
            return -1;
        }
    }
    tecsup_hpt_cal(unit, clk, raw);

    for (idx = 0; idx < COUNTOF(raw) - 1; idx++) {
        rawb[bdx++] = (raw[idx] >> 0) & 0xff;
        rawb[bdx++] = (raw[idx] >> 8) & 0xff;
        rawb[bdx++] = (raw[idx] >> 16) & 0xff;
        rawb[bdx++] = (raw[idx] >> 24) & 0xff;
    }

    rv = shr_crc16(0, (uint8_t *)rawb, sizeof(rawb));
    sal_snprintf(hpt->data, sizeof(*hpt), "%04X", rv);

    return 0;
}

/*!
 * \brief Get the clock value.
 */
static int
tecsup_clk_get(bcma_cli_tokens_t *ctok, sal_date_t *date, uint32_t *clk)
{
    int idx, val[4];

    sal_memset(ctok, 0, sizeof(*ctok));
    if (bcma_cli_tokenize(date->buf, ctok, " :") < 0) {
        return -1;
    }

    for (idx = 2; idx <= 5; idx++) {
        char *ptr;

        if (idx >= ctok->argc) {
            return -1;
        }

        ptr = ctok->argv[idx];
        if (*ptr == '0') {
            ptr++;
        }
        if (bcma_cli_parse_int(ptr, &val[idx - 2]) < 0) {
            return -1;
        }
    }

    *clk = 0;
    for (idx = 0; idx < 4; idx++) {
        *clk |= (val[idx] & 0xff) << ((3 - idx) << 3);
    }

    return 0;
}

/*!
 * \brief Print date string.
 */
static int
tecsup_date_string_print(int unit, shr_pb_t *pb)
{
    int idx;
    bcma_cli_tokens_t *ctok;
    sal_date_t date;
    uint32_t clk = 0;
    tecsup_htp_t hpt;

    sal_date(&date);
    if (sal_strcmp(date.buf, SAL_DATE_NULL) == 0) {
        return 0;
    }

    ctok = sal_alloc(sizeof(bcma_cli_tokens_t), "bcmaTecsupCtok");
    if (!ctok) {
        return 0;
    }

    tecsup_clk_get(ctok, &date, &clk);
    tecsup_hpt_get(unit, clk, &hpt);

    shr_pb_printf(pb, "Date:");
    for (idx = 0; idx < ctok->argc; idx++) {
        if (idx == 4) {
            shr_pb_printf(pb, ":%s", ctok->argv[idx]);
        } else if (idx == 5) {
            shr_pb_printf(pb, ":%s.%s", ctok->argv[idx], hpt.data);
        } else {
            shr_pb_printf(pb, " %s", ctok->argv[idx]);
        }
    }
    shr_pb_printf(pb, "\n");

    sal_free(ctok);
    return 0;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_tecsup_header(int unit, shr_pb_t *pb)
{
    shr_pb_printf(pb, "%s\n", tecsup_septal_line);
    shr_pb_printf(pb, "%s\n", bcma_clicmd_version_string_get());
    tecsup_date_string_print(unit, pb);
    shr_pb_printf(pb, "%s\n", tecsup_septal_line);

    return 0;
}
