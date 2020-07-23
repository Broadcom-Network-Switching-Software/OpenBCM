/*! \file bcma_bcmpkt_parse_data.c
 *
 * Parse data drivers for bcmpkt command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_pb_format.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_hg3.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_extension_0_t.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include "./cmd/bcma_bcmpktcmd_internal.h"
#include <bcmlrd/bcmlrd_conf.h>

/* Module header Op-Codes */
#define HIGIG_OP_CPU        0x00 /* CPU Frame */
#define HIGIG_OP_UC         0x01 /* Unicast Frame */
#define HIGIG_OP_BC         0x02 /* Broadcast or DLF frame */
#define HIGIG_OP_MC         0x03 /* Multicast Frame */
#define HIGIG_OP_IPMC       0x04 /* IP Multicast Frame */

static int
parse_bitmap_str(const char *str, SHR_BITDCL *bits, int size)
{
    if (!sal_strcasecmp(str, "none")) {
        SHR_BITCLR_RANGE(bits, 0, size);
        return 0;
    }

    return bcma_cli_parse_bit_list(str, size, bits);
}

static int
bmp_parse_arg(const char *arg, void *data, void **option)
{
    SHR_BITDCL *bits;
    int size;

    if (data == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    bits = (SHR_BITDCL *)data;
    size = **(int **)option;
    if (parse_bitmap_str(arg, bits, size) != 0) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("Input error!\n")));
        return -1;
    }

    return 0;
}

static char *
bmp_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    SHR_BITDCL *bits;
    int size, len;
    const char *str;
    shr_pb_t *pb;

    if (data == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return NULL;
    }

    bits = (SHR_BITDCL *)data;
    size = *(int *)option;

    pb = shr_pb_create();
    str = shr_pb_format_bit_list(pb, bits, size);
    len = sal_strlen(str);
    if (len < bufsz) {
        sal_memcpy(buf, str, len + 1);
        str = buf;
    } else {
        str = "<overrun>";
    }
    shr_pb_destroy(pb);

    return (char *)str;
}

static bcma_cli_parse_data_t parse_data_bmp = {
    "bmp",
    bmp_parse_arg,
    bmp_format_arg,
    NULL
};

static int
txpmd_parse_arg(const char *arg, void *data, void **option)
{
    bcma_txpmd_t *txpmd = (bcma_txpmd_t *)data;
    uint32_t val;
    int fid;

    if (txpmd == NULL || txpmd->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    fid = **(int **) option;

    if (bcmpkt_txpmd_field_get(txpmd->dev_type, txpmd->data,
                               BCMPKT_TXPMD_START, &val)) {
        return -1;
    }
    if (val == 0) {
        bcma_bcmpkt_txpmd_init(txpmd->dev_type, txpmd->data);
    }

    val = sal_strtoul(arg, NULL, 0);

    return bcmpkt_txpmd_field_set(txpmd->dev_type, txpmd->data, fid, val);
}

static char *
txpmd_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_txpmd_t *txpmd = (bcma_txpmd_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (txpmd == NULL || txpmd->data == NULL || option == NULL || buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;
    if (!bcmpkt_txpmd_field_get(txpmd->dev_type, txpmd->data, fid, &val)) {
        len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_txpmd = {
    "txpmd",
    txpmd_parse_arg,
    txpmd_format_arg,
    NULL
};

static int
lbhdr_parse_arg(const char *arg, void *data, void **option)
{
    bcma_lbhdr_t *lbhdr = (bcma_lbhdr_t *)data;
    uint32_t val;
    int fid;

    if (lbhdr == NULL || lbhdr->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }
    fid = **(int **) option;

    if (bcmpkt_lbhdr_field_get(lbhdr->dev_type, lbhdr->data,
                               BCMPKT_LBHDR_START, &val)) {
        return -1;
    }
    if (val == 0) {
        bcma_bcmpkt_lbhdr_init(lbhdr->dev_type, lbhdr->data);
    }

    val = sal_strtoul(arg, NULL, 0);

    return bcmpkt_lbhdr_field_set(lbhdr->dev_type, lbhdr->data, fid, val);
}

static char *
lbhdr_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_lbhdr_t *lbhdr = (bcma_lbhdr_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (lbhdr == NULL || lbhdr->data == NULL || option == NULL || buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;
    if (!bcmpkt_lbhdr_field_get(lbhdr->dev_type, lbhdr->data, fid, &val)) {
        len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_lbhdr = {
    "lbhdr",
    lbhdr_parse_arg,
    lbhdr_format_arg,
    NULL
};

static int
generic_loopback_parse_arg(const char *arg, void *data, void **option)
{
    bcma_generic_loopback_t *generic_loopback = (bcma_generic_loopback_t *)data;
    uint32_t val;
    int fid;

    if (generic_loopback == NULL || generic_loopback->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    if (bcmpkt_generic_loopback_t_field_id_get("START_BYTE", &fid) ||
        bcmpkt_generic_loopback_t_field_get(generic_loopback->variant, generic_loopback->data,
                             fid, &val)) {
        return -1;
    }
    if (val == 0) {
        bcma_bcmpkt_generic_loopback_init(generic_loopback->variant, generic_loopback->data);
    }

    fid = **(int **) option;
    val = sal_strtoul(arg, NULL, 0);

    return bcmpkt_generic_loopback_t_field_set(generic_loopback->variant, generic_loopback->data, fid, val);
}

static char *
generic_loopback_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_generic_loopback_t *generic_loopback = (bcma_generic_loopback_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (generic_loopback == NULL || generic_loopback->data == NULL || option == NULL || buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;
    if (!bcmpkt_generic_loopback_t_field_get(generic_loopback->variant, generic_loopback->data, fid, &val)) {
        len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_generic_loopback = {
    "generic_loopback",
    generic_loopback_parse_arg,
    generic_loopback_format_arg,
    NULL
};

static void
higig_init(HIGIG_t *mh)
{
    sal_memset(mh, 0, sizeof(*mh));

    HIGIG_STARTf_SET(*mh, BCMPKT_HIGIG_SOF);
    HIGIG_OPCODEf_SET(*mh, HIGIG_OP_UC);
}

static void
higig2_init(HIGIG2_t *mh)
{
    sal_memset(mh, 0, sizeof(*mh));

    HIGIG2_STARTf_SET(*mh, BCMPKT_HIGIG2_SOF);
    HIGIG2_PPD_TYPEf_SET(*mh, 2);
    HIGIG2_PPD2_OPCODEf_SET(*mh, HIGIG_OP_UC);
}

static int
hg_sym_field_get(uint32_t sflags, uint32_t size, uint16_t minbit,
                 uint16_t maxbit, uint32_t *sbuf, uint32_t *fbuf)
{
    if (sflags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        bcmdrd_field_be_get(sbuf, size, minbit, maxbit, fbuf);
    } else {
        bcmdrd_field_get(sbuf, minbit, maxbit, fbuf);
    }
    return 0;
}

static int
hg_sym_field_set(uint32_t sflags, uint32_t size, uint16_t minbit,
                 uint16_t maxbit, uint32_t *sbuf, uint32_t *fbuf)
{
    if (sflags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        bcmdrd_field_be_set(sbuf, size, minbit, maxbit, fbuf);
    } else {
        bcmdrd_field_set(sbuf, minbit, maxbit, fbuf);
    }
    return 0;
}

/* Parse data driver for type "higig". */
static int
higig_parse_arg(const char *arg, void *data, void **option)
{
    HIGIG_t *mh = (HIGIG_t *)data;
    higig_symbl_info_t *info;
    uint32_t val = 0;

    if (mh == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    info = *(higig_symbl_info_t **)option;

    val = HIGIG_STARTf_GET(*mh);

    if (val != BCMPKT_HIGIG_SOF ) {
        if (val != 0) {
            LOG_WARN(BSL_LS_APPL_PACKET,
                     (BSL_META("Switch module header (start=0x%x) to HIGIG\n"),
                       val));
        }
        higig_init(mh);
    }

    val = sal_strtoul(arg, NULL, 0);
    /*
     * No Higig field size bigger than 32 bits.
     */
    /* coverity[address_of] */
    hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, &val);

    return 0;
}

static char *
higig_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int len;
    HIGIG_t *mh = (HIGIG_t *)data;
    higig_symbl_info_t *info;
    uint32_t val = 0;

    if (mh == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return "<null pointer>";
    }

    info = (higig_symbl_info_t *)option;
    hg_sym_field_get(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, &val);

    sal_memset(buf, 0, bufsz);
    len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_higig = {
    "higig",
    higig_parse_arg,
    higig_format_arg,
    NULL
};

static int
higig2_parse_arg(const char *arg, void *data, void **option)
{
    HIGIG2_t *mh = (HIGIG2_t *)data;
    higig_symbl_info_t *info;
    uint32_t val[2]={0,};

    if (mh == NULL || option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    info = *(higig_symbl_info_t **)option;

    val[0] = HIGIG2_STARTf_GET(*mh);

    if (val[0] != BCMPKT_HIGIG2_SOF ) {
        if (val[0] != 0) {
            LOG_WARN(BSL_LS_APPL_PACKET,
                     (BSL_META("Switch module header (start=0x%x) to HIGIG2\n"),
                      val[0]));
        }
        higig2_init(mh);
    }

    if (!sal_strcasecmp(info->name, "ppd_data")) {
        int len=0;
        char str1[9];
        char str2[9];
        char *strtmp;
        sal_memset(str1, 0, 9);
        sal_memset(str2, 0, 9);
        if (arg[0] == 'x' || arg[0] == 'X') {
            arg++;
        }
        else if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
            arg += 2;
        }
        len = sal_strlen(arg);
        if (len > 16) {
            return -1;
        }

        if (len <= 8) {
            val[0] = sal_strtoul(arg, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
        }
        else {
            sal_memcpy(str1, arg + len - 8, 8);
            val[0] = sal_strtoul(str1, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
            sal_memcpy(str2, arg, len - 8);
            val[1] = sal_strtoul(str2, &strtmp, 16);
            if (strtmp != NULL && sal_strlen(strtmp) > 0) {
                return -1;
            }
        }
        hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                         info->maxbit, (uint32_t *)mh, val);
    }
    else {
        val[0] = sal_strtoul(arg, NULL, 0);
        hg_sym_field_set(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                         info->maxbit, (uint32_t *)mh, val);
    }

    return 0;
}

static char *
higig2_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int len = 0;
    HIGIG2_t *mh = (HIGIG2_t *)data;
    higig_symbl_info_t *info;
    uint32_t val[2]={0,};

    if (mh == NULL || option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return "<null pointer>";
    }

    info = (higig_symbl_info_t *)option;
    hg_sym_field_get(info->sflags, (sizeof(*mh) + 3)/4, info->minbit,
                     info->maxbit, (uint32_t *)mh, val);
    sal_memset(buf, 0, bufsz);
    len = sal_snprintf(buf, bufsz, "0x");
    if (!sal_strcasecmp(info->name, "HG2ppd_data")) {
        len += sal_snprintf(buf + len, bufsz, "%08x ", val[1]);
        len += sal_snprintf(buf + len, bufsz, "%08x", val[0]);
    }
    else {
        len += sal_snprintf(buf + len, bufsz, "0x%x(%d)", val[0], val[0]);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_higig2 = {
    "higig2",
    higig2_parse_arg,
    higig2_format_arg,
    NULL
};

static int
higig3_parse_arg(const char *arg, void *data, void **option)
{
    bcma_hg3_t *higig3 = (bcma_hg3_t *)data;
    uint32_t val;
    int fid;

    if (higig3 == NULL || higig3->data == NULL ||
        option == NULL || *option == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return -1;
    }

    if (bcmpkt_hg3_base_t_field_id_get("RESERVED_ETYPE", &fid) ||
        bcmpkt_hg3_base_t_field_get(higig3->variant,
                                    higig3->data,
                                    fid,
                                    &val)) {
        return -1;
    }

    if (val != BCMPKT_HG3_ETHER_TYPE) {
        if (val != 0) {
            LOG_ERROR(BSL_LS_APPL_PACKET,
                     (BSL_META("Incorrect ethertype 0x%X, expected"
                               " 0x%X ethertype\n"),
                      val, BCMPKT_HG3_ETHER_TYPE));
        }
        bcma_bcmpkt_hg3hdr_init(higig3->variant, higig3->data);
    }

    val = sal_strtoul(arg, NULL, 0);

    /* Call base header field set or ext header field set depending on fid's
     * value
     */
    fid = **(int **) option;
    if (fid < BCMPKT_HG3_BASE_T_FID_COUNT) {
        return bcmpkt_hg3_base_t_field_set(higig3->variant,
                                           higig3->data, fid, val);
    } else {
        return bcmpkt_hg3_extension_0_t_field_set(higig3->variant,
                                          higig3->data +
                                          BCMPKT_HG3_BASE_HEADER_SIZE_WORDS,
                                          fid - BCMPKT_HG3_EXT0_FID_OFFSET,
                                          val);
    }
}

static char *
higig3_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    bcma_hg3_t *higig3 = (bcma_hg3_t *)data;
    int len = 0;
    uint32_t val;
    int fid;

    if (higig3 == NULL || higig3->data == NULL || option == NULL ||
        buf == NULL) {
        LOG_ERROR(BSL_LS_APPL_PACKET, (BSL_META("NULL pointer!\n")));
        return buf;
    }
    sal_memset(buf, 0, bufsz);
    fid = *(int *)option;

    /* Call base header field get or extension header field get based on fid's
     * value
     */
    if (fid < BCMPKT_HG3_BASE_T_FID_COUNT) {
        if (!bcmpkt_hg3_base_t_field_get(higig3->variant,
                                         higig3->data, fid, &val)) {
            len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
        }
    } else {
        if (!bcmpkt_hg3_extension_0_t_field_get(higig3->variant,
                                         higig3->data, fid -
                                         BCMPKT_HG3_BASE_T_FID_COUNT, &val)) {
            len = sal_snprintf(buf, bufsz, "0x%x(%d)", val, val);
        }
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_higig3 = {
    "higig3",
    higig3_parse_arg,
    higig3_format_arg,
    NULL
};

int
bcma_bcmpkt_parse_data_add(void)
{
    bcma_cli_parse_data_add(&parse_data_txpmd);
    bcma_cli_parse_data_add(&parse_data_lbhdr);
    bcma_cli_parse_data_add(&parse_data_generic_loopback);
    bcma_cli_parse_data_add(&parse_data_higig);
    bcma_cli_parse_data_add(&parse_data_higig2);
    bcma_cli_parse_data_add(&parse_data_higig3);
    bcma_cli_parse_data_add(&parse_data_bmp);

    return BCMA_CLI_CMD_OK;
}

