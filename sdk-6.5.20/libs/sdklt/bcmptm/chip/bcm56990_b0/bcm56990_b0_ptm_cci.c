/*! \file bcm56990_b0_ptm_cci.c
 *
 * This file contains the misc utils for bcm56990_b0's CCI
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/chip/bcm56990_b0_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/generated/bcmptm_cci_ha.h>
#include "./bcm56990_b0_ptm_cci.h"

#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI

#define MEM_FIFO_DMA_CHANNEL_0 0x0

#define CTR_EVICT_ENTRY_NUM    4096

#define NORMAL_MODE_WIDTH      36
#define WIDE_MODE_WIDTH        72
#define SLIM_MODE_WIDTH        24

#define MIB_ROW_PER_PORT       16
#define PORT_PER_TABLE         4
#define MIB_FRMT_WIDTH         40
#define MIB_MEMORY_ROW_FIELDS  8
#define MIB_MEMORY_ROW_12      12


typedef struct sym_format_map_s {
    bcmdrd_sid_t sid;
    bcmdrd_sid_t frmt_sid;
    int          row_per_port;
    int          port_per_table;
    size_t       frmt_width;
} sym_format_map_t;

typedef struct sym_map_s {
    bcmdrd_sid_t sid;
    bcmdrd_sid_t map_sid;
    int port_start;
    int port_end;
    int row;
    int field;
} sym_map_t;

typedef struct ctr_evict_s {
    bcmdrd_sid_t sid;
    bcmdrd_sid_t ctrl_sid;
} ctr_evict_t;

static sym_format_map_t sym_format[] =
{
    {CDMIB_MEMm, MIB_MEMORY_ROW0, MIB_ROW_PER_PORT, PORT_PER_TABLE, MIB_FRMT_WIDTH},
};

/* Field format of CDMIB_MEMm ROW 12 */
static size_t mib_field_width_format[] =
{
  40, 40, 40, 40, 48, 48, 48, 40,
};

static sym_map_t xlmib_sym_map[] =
{
    {XLMIB_R1023r, CDMIB_MEMm, 1, 256, 0, 4},
    {XLMIB_R127r, CDMIB_MEMm, 1, 256, 0, 1},
    {XLMIB_R1518r, CDMIB_MEMm, 1, 256, 0, 5},
    {XLMIB_R16383r, CDMIB_MEMm, 1, 256, 1, 2},
    {XLMIB_R2047r, CDMIB_MEMm, 1, 256, 0, 7},
    {XLMIB_R255r, CDMIB_MEMm, 1, 256, 0, 2},
    {XLMIB_R4095r, CDMIB_MEMm, 1, 256, 1, 0},
    {XLMIB_R511r, CDMIB_MEMm, 1, 256, 0, 3},
    {XLMIB_R64r, CDMIB_MEMm, 1, 256, 0, 0},
    {XLMIB_R9216r, CDMIB_MEMm, 1, 256, 1, 1},
    {XLMIB_RALNr, XLMIB_RALNr, 0, 0, 0, 0},
    {XLMIB_RBCAr, CDMIB_MEMm, 1, 256, 1, 3},
    {XLMIB_RBYTr, CDMIB_MEMm, 1, 256, 12, 4},
    {XLMIB_RDVLNr, CDMIB_MEMm, 1, 256, 3, 7},
    {XLMIB_RERPKTr, CDMIB_MEMm, 1, 256, 3, 1},
    {XLMIB_RFCRr, XLMIB_RFCRr, 0, 0, 0, 0},
    {XLMIB_RFCSr, CDMIB_MEMm, 1, 256, 3, 0},
    {XLMIB_RFLRr, CDMIB_MEMm, 1, 256, 3, 2},
    {XLMIB_RFRGr, CDMIB_MEMm, 1, 256, 6, 5},
    {XLMIB_RJBRr, CDMIB_MEMm, 1, 256, 3, 3},
    {XLMIB_RMCAr, CDMIB_MEMm, 1, 256, 2, 4},
    {XLMIB_RMCRCr, XLMIB_RMCRCr, 0, 0, 0, 0},
    {XLMIB_RMGVr, CDMIB_MEMm, 1, 256, 0, 6},
    {XLMIB_RMTUEr, CDMIB_MEMm, 1, 256, 3, 4},
    {XLMIB_ROVRr, CDMIB_MEMm, 1, 256, 3, 5},
    {XLMIB_RPFC0r, CDMIB_MEMm, 1, 256, 4, 4},
    {XLMIB_RPFC1r, CDMIB_MEMm, 1, 256, 4, 6},
    {XLMIB_RPFC2r, CDMIB_MEMm, 1, 256, 5, 0},
    {XLMIB_RPFC3r, CDMIB_MEMm, 1, 256, 5, 2},
    {XLMIB_RPFC4r, CDMIB_MEMm, 1, 256, 5, 4},
    {XLMIB_RPFC5r, CDMIB_MEMm, 1, 256, 5, 6},
    {XLMIB_RPFC6r, CDMIB_MEMm, 1, 256, 6, 0},
    {XLMIB_RPFC7r, CDMIB_MEMm, 1, 256, 6, 2},
    {XLMIB_RPFCOFF0r, CDMIB_MEMm, 1, 256, 4, 5},
    {XLMIB_RPFCOFF1r, CDMIB_MEMm, 1, 256, 4, 7},
    {XLMIB_RPFCOFF2r, CDMIB_MEMm, 1, 256, 5, 1},
    {XLMIB_RPFCOFF3r,CDMIB_MEMm, 1, 256, 5, 3},
    {XLMIB_RPFCOFF4r, CDMIB_MEMm, 1, 256, 5, 5},
    {XLMIB_RPFCOFF5r, CDMIB_MEMm, 1, 256, 5, 7},
    {XLMIB_RPFCOFF6r, CDMIB_MEMm, 1, 256, 6, 1},
    {XLMIB_RPFCOFF7r, CDMIB_MEMm, 1, 256, 6, 3},
    {XLMIB_RPKTr, CDMIB_MEMm, 1, 256, 2, 0},
    {XLMIB_RPOKr, CDMIB_MEMm, 1, 256, 2, 1},
    {XLMIB_RPRMr, CDMIB_MEMm, 1, 256, 4, 3},
    {XLMIB_RPROG0r, CDMIB_MEMm, 1, 256, 1, 4},
    {XLMIB_RPROG1r, CDMIB_MEMm, 1, 256, 1, 5},
    {XLMIB_RPROG2r, CDMIB_MEMm, 1, 256, 1, 6},
    {XLMIB_RPROG3r, CDMIB_MEMm, 1, 256, 1, 7},
    {XLMIB_RRBYTr, CDMIB_MEMm, 1, 256, 12, 5},
    {XLMIB_RRPKTr, CDMIB_MEMm, 1, 256, 6, 6},
    {XLMIB_RSCHCRCr, XLMIB_RSCHCRCr, 0, 0, 0, 0},
    {XLMIB_RTRFUr, XLMIB_RTRFUr, 1, 256, 0, 0},
    {XLMIB_RUCAr, CDMIB_MEMm, 1, 256, 2, 2},
    {XLMIB_RUNDr, CDMIB_MEMm, 1, 256, 6, 4},
    {XLMIB_RVLNr, CDMIB_MEMm, 1, 256, 3, 6},
    {XLMIB_RXCFr, CDMIB_MEMm, 1, 256, 2, 7},
    {XLMIB_RXPFr, CDMIB_MEMm, 1, 256, 2, 5},
    {XLMIB_RXPPr, CDMIB_MEMm, 1, 256, 2, 6},
    {XLMIB_RXUDAr, CDMIB_MEMm, 1, 256, 4, 1},
    {XLMIB_RXUOr, CDMIB_MEMm, 1, 256, 4, 0},
    {XLMIB_RXWSAr, CDMIB_MEMm, 1, 256, 4, 2},
    {XLMIB_RX_EEE_LPI_DURATION_COUNTERr, XLMIB_RX_EEE_LPI_DURATION_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_EEE_LPI_EVENT_COUNTERr, XLMIB_RX_EEE_LPI_EVENT_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_HCFC_COUNTERr, XLMIB_RX_HCFC_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_HCFC_CRC_COUNTERr, XLMIB_RX_HCFC_CRC_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_LLFC_CRC_COUNTERr, XLMIB_RX_LLFC_CRC_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_LLFC_LOG_COUNTERr, XLMIB_RX_LLFC_LOG_COUNTERr, 0, 0, 0, 0},
    {XLMIB_RX_LLFC_PHY_COUNTERr, XLMIB_RX_LLFC_PHY_COUNTERr, 0, 0, 0, 0},
    {XLMIB_T1023r, CDMIB_MEMm, 1, 256, 7, 4},
    {XLMIB_T127r, CDMIB_MEMm, 1, 256, 7, 1},
    {XLMIB_T1518r, CDMIB_MEMm, 1, 256, 7, 5},
    {XLMIB_T16383r, CDMIB_MEMm, 1, 256, 8, 2},
    {XLMIB_T2047r, CDMIB_MEMm, 1, 256, 7, 7},
    {XLMIB_T255r, CDMIB_MEMm, 1, 256, 7, 2},
    {XLMIB_T4095r, CDMIB_MEMm, 1, 256, 8, 0},
    {XLMIB_T511r, CDMIB_MEMm, 1, 256, 7, 3},
    {XLMIB_T64r, CDMIB_MEMm, 1, 256, 7, 0},
    {XLMIB_T9216r, CDMIB_MEMm, 1, 256, 8, 1},
    {XLMIB_TBCAr, CDMIB_MEMm, 1, 256, 8, 3},
    {XLMIB_TBYTr, CDMIB_MEMm, 1, 256, 12, 6},
    {XLMIB_TDFRr, XLMIB_TDFRr, 1, 256, 0, 0},
    {XLMIB_TDVLNr, CDMIB_MEMm, 1, 256, 12, 3},
    {XLMIB_TEDFr, XLMIB_TEDFr, 1, 256, 0, 0},
    {XLMIB_TERRr, CDMIB_MEMm, 1, 256, 11, 5},
    {XLMIB_TFCSr, CDMIB_MEMm, 1, 256, 11, 4},
    {XLMIB_TFRGr, CDMIB_MEMm, 1, 256, 12, 1},
    {XLMIB_TJBRr, CDMIB_MEMm, 1, 256, 11, 7},
    {XLMIB_TLCLr, XLMIB_TLCLr, 0, 0, 0, 0},
    {XLMIB_TMCAr, CDMIB_MEMm, 1, 256, 11, 0},
    {XLMIB_TMCLr, XLMIB_TMCLr, 0, 0, 0, 0},
    {XLMIB_TMGVr, CDMIB_MEMm, 1, 256, 7, 6},
    {XLMIB_TNCLr, XLMIB_TNCLr, 0, 0, 0, 0},
    {XLMIB_TOVRr, CDMIB_MEMm, 1, 256, 11, 6},
    {XLMIB_TPFC0r, CDMIB_MEMm, 1, 256, 8, 4},
    {XLMIB_TPFC1r, CDMIB_MEMm, 1, 256, 8, 6},
    {XLMIB_TPFC2r, CDMIB_MEMm, 1, 256, 9, 0},
    {XLMIB_TPFC3r, CDMIB_MEMm, 1, 256, 9, 2},
    {XLMIB_TPFC4r, CDMIB_MEMm, 1, 256, 9, 4},
    {XLMIB_TPFC5r, CDMIB_MEMm, 1, 256, 9, 6},
    {XLMIB_TPFC6r, CDMIB_MEMm, 1, 256, 10, 0},
    {XLMIB_TPFC7r, CDMIB_MEMm, 1, 256, 10, 2},
    {XLMIB_TPFCOFF0r, CDMIB_MEMm, 1, 256, 8, 5},
    {XLMIB_TPFCOFF1r, CDMIB_MEMm, 1, 256, 8, 7},
    {XLMIB_TPFCOFF2r, CDMIB_MEMm, 1, 256, 9, 1},
    {XLMIB_TPFCOFF3r, CDMIB_MEMm, 1, 256, 9, 3},
    {XLMIB_TPFCOFF4r, CDMIB_MEMm, 1, 256, 9, 5},
    {XLMIB_TPFCOFF5r, CDMIB_MEMm, 1, 256, 9, 7},
    {XLMIB_TPFCOFF6r, CDMIB_MEMm, 1, 256, 10, 1},
    {XLMIB_TPFCOFF7r, CDMIB_MEMm, 1, 256, 10, 3},
    {XLMIB_TPKTr, CDMIB_MEMm, 1, 256, 10, 4},
    {XLMIB_TPOKr, CDMIB_MEMm, 1, 256, 10, 5},
    {XLMIB_TRPKTr, CDMIB_MEMm, 1, 256, 12, 0},
    {XLMIB_TSCLr, XLMIB_TSCLr, 0, 0, 0, 0},
    {XLMIB_TSPARE0r, XLMIB_TSPARE0r, 0, 0, 0, 0},
    {XLMIB_TSPARE1r, XLMIB_TSPARE1r, 0, 0, 0, 0},
    {XLMIB_TSPARE2r, XLMIB_TSPARE2r, 0, 0, 0, 0},
    {XLMIB_TSPARE3r, XLMIB_TSPARE3r, 0, 0, 0, 0},
    {XLMIB_TUCAr, CDMIB_MEMm, 1, 256, 10, 6},
    {XLMIB_TUFLr, CDMIB_MEMm, 1, 256, 10, 7},
    {XLMIB_TVLNr, CDMIB_MEMm, 1, 256, 12, 2},
    {XLMIB_TXCFr, CDMIB_MEMm, 1, 256, 11, 3},
    {XLMIB_TXCLr, XLMIB_TXCLr, 0, 0, 0, 0},
    {XLMIB_TXPFr, CDMIB_MEMm, 1, 256, 11, 1},
    {XLMIB_TXPPr, CDMIB_MEMm, 1, 256, 11, 2},
    {XLMIB_TX_EEE_LPI_DURATION_COUNTERr, XLMIB_TX_EEE_LPI_DURATION_COUNTERr, 0, 0, 0, 0},
    {XLMIB_TX_EEE_LPI_EVENT_COUNTERr, XLMIB_TX_EEE_LPI_EVENT_COUNTERr, 0, 0, 0, 0},
    {XLMIB_TX_HCFC_COUNTERr, XLMIB_TX_HCFC_COUNTERr, 0, 0, 0, 0},
    {XLMIB_TX_LLFC_LOG_COUNTERr, XLMIB_TX_LLFC_LOG_COUNTERr, 0, 0, 0, 0},
    {XLMIB_XTHOLr, XLMIB_XTHOLr, 0, 0, 0, 0},
};

static const ctr_evict_t ctr_eviction[] = {
    {FLEX_CTR_ING_COUNTER_TABLE_0_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_0_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_0_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_0_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_1_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_1_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_1_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_1_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_2_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_2_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_2_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_2_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_3_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_3_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_3_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_3_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_4_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_4_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_4_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_4_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_5_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_5_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_5_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_5_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_6_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_6_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_6_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_6_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_7_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_7_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_7_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_7_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_8_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_8_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_8_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_8_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_9_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_9_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_9_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_9_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_10_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_10_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_10_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_10_INST1r},
    {FLEX_CTR_ING_COUNTER_TABLE_11_INST0m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_11_INST0r},
    {FLEX_CTR_ING_COUNTER_TABLE_11_INST1m, FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_11_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_0_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_0_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_0_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_0_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_1_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_1_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_1_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_1_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_2_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_2_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_2_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_2_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_3_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_3_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_3_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_3_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_4_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_4_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_4_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_4_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_5_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_5_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_5_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_5_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_6_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_6_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_6_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_6_INST1r},
    {FLEX_CTR_EGR_COUNTER_TABLE_7_INST0m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_7_INST0r},
    {FLEX_CTR_EGR_COUNTER_TABLE_7_INST1m, FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_7_INST1r}
};

static int
get_control_sid(int unit, bcmdrd_sid_t sid, bcmdrd_sid_t *ctrl_sid)
{
    int size = COUNTOF(ctr_eviction);
    int rv = SHR_E_PARAM;
    int i;

    for (i = 0; i < size; i++) {
        if (ctr_eviction[i].sid == sid) {
            *ctrl_sid = ctr_eviction[i].ctrl_sid;
            rv = SHR_E_NONE;
            break;
        }
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctrtype_get(int unit,
                               bcmdrd_sid_t sid)
{
    int rv = SHR_E_NONE;
    int blktype;
    bool port;

    rv = bcmdrd_pt_blktype_get(unit, sid, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    blktype = rv;
    port = bcmdrd_pt_is_port_reg(unit, sid);
    if (port) {
        if ((blktype == BLKTYPE_XLPORT) ||
             (blktype == BLKTYPE_EPIPE) ||
             (blktype == BLKTYPE_IPIPE)) {
            return CCI_CTR_TYPE_PORT;
        } else if ((blktype == BLKTYPE_MMU_ITM) ||
            (blktype == BLKTYPE_MMU_EB) ||
            (blktype == BLKTYPE_MMU_GLB)) {
            return CCI_CTR_TYPE_TM;
        } else {
            return -1;
        }
    } else if (blktype == BLKTYPE_CDPORT) {
        return CCI_CTR_TYPE_PORT;
    } else if (blktype == BLKTYPE_EPIPE) {
        return CCI_CTR_TYPE_EPIPE;
    } else if (blktype == BLKTYPE_IPIPE) {
       return CCI_CTR_TYPE_IPIPE;
    } else if ((blktype == BLKTYPE_MMU_ITM) ||
             (blktype == BLKTYPE_MMU_EB) ||
             (blktype == BLKTYPE_MMU_GLB) ||
             (blktype == BLKTYPE_MGMT_OBM) ||
             (blktype == BLKTYPE_PORT_IF)) {
        return CCI_CTR_TYPE_TM;
    } else if (blktype == BLKTYPE_FLEX_CTR) {
        return CCI_CTR_TYPE_EVICT;
    }

    return SHR_E_PARAM;

}

bool
bcm56990_b0_pt_cci_ctr_is_bypass(int unit, bcmdrd_sid_t sid)
{
    int blktype;
    bool rv = false;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    if ((blktype == BLKTYPE_EPIPE) || (blktype == BLKTYPE_IPIPE)) {
        if (bcmdrd_dev_bypass_mode_get(unit) == BYPASS_MODE_EPIP) {
            rv = true;
        }
    }

    return rv;
}

int
bcm56990_b0_pt_cci_frmt_index_from_port(int unit, bcmdrd_sid_t sid, int port,
                                        int *index_min, int *index_max)
{
    int i;
    int size = COUNTOF(sym_format);
    int rv = SHR_E_UNAVAIL;

    if (!index_min || !index_max) {
        return SHR_E_PARAM;
    }
    /* Port 1 is aligned to index 0 so check and subtract 1 to align for 0 index */
    if (port > 0) {
       port = port - 1;
    } else {
        return SHR_E_PARAM;
    }

    for (i = 0; i < size; i++) {
        if (sid == sym_format[i].sid) {
            *index_min = port % sym_format[i].port_per_table;
            *index_min *= sym_format[i].row_per_port;
            *index_max = *index_min + sym_format[i].row_per_port - 1;
            rv = SHR_E_NONE;
            break;
        }
    }
    return rv;
}

int
bcm56990_b0_pt_cci_ctr_sym_map(int unit,
                               bcmptm_cci_ctr_info_t *ctr_info)
{
    size_t size = COUNTOF(xlmib_sym_map);
    uint32_t id;
    int index_min = 0, index_max;
    int rv = SHR_E_NONE;
    bcmdrd_pbmp_t pbmp;

    ctr_info->txfm_fld = -1;
    /* check if Symbol is mapped */
    do {
        if ((ctr_info->sid < xlmib_sym_map[0].sid) ||
                (ctr_info->sid > xlmib_sym_map[size - 1].sid)) {
            break;
        }

        /* For MGMT ports which belong to XLPORT, no need to do SYM map */
        bcmdrd_dev_blktype_pbmp(unit, BLKTYPE_XLPORT, &pbmp);
        if (BCMDRD_PBMP_MEMBER(pbmp, ctr_info->dyn_info.tbl_inst)) {
            break;
        }

        id = ctr_info->sid - xlmib_sym_map[0].sid;
        /* check if matches with the list */
        if (ctr_info->sid != xlmib_sym_map[id].sid) {
            rv = SHR_E_FAIL;
            break;
        }
        /* Find if mapped to itself */
        if (xlmib_sym_map[id].sid == xlmib_sym_map[id].map_sid) {
            break;
        }
        /* check the port is in the range */
        if ((ctr_info->dyn_info.tbl_inst < xlmib_sym_map[id].port_start) ||
            (ctr_info->dyn_info.tbl_inst > xlmib_sym_map[id].port_end)) {
            break;
        }

        ctr_info->sid = xlmib_sym_map[id].map_sid;
        ctr_info->txfm_fld = xlmib_sym_map[id].field;
        /* base index should be shifted to number of rows */
        rv = bcm56990_b0_pt_cci_frmt_index_from_port(unit, ctr_info->sid,
                                                     ctr_info->dyn_info.tbl_inst,
                                                     &index_min, &index_max);
        ctr_info->dyn_info.index = index_min + xlmib_sym_map[id].row;
        /* based on the number of ports in one tbl_inst */
        ctr_info->dyn_info.tbl_inst =
                bcmdrd_pt_tbl_inst_from_port(unit, ctr_info->sid,
                                             ctr_info->dyn_info.tbl_inst);
    } while (0);

    return rv;
}

int
bcm56990_b0_pt_cci_frmt_sym_get(int unit, int index, int field,
                                bcmdrd_sid_t *sid, size_t *width)
{
    int rv = SHR_E_NONE;
    int i;
    int size = COUNTOF(sym_format);

    if ((index >= 0) && (*sid == CDMIB_MEMm)) {
        int row;
        /* Modified format field width */
        if (field >= MIB_MEMORY_ROW_FIELDS) {
            return SHR_E_PARAM;
        }
        row = index % MIB_ROW_PER_PORT;
        if (row == MIB_MEMORY_ROW_12) {
            *width =  mib_field_width_format[field];
        }
    } else {
        /* Generic format info */
        for (i = 0; i < size; i++) {
            if (sym_format[i].sid == *sid) {
                *sid = sym_format[i].frmt_sid;
                *width = sym_format[i].frmt_width;
                break;
            }
        }
    }

    return rv;
}

bool
bcm56990_b0_pt_cci_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int index)
{
    bool rv;

    rv = bcmdrd_pt_index_valid(unit, sid, tbl_inst, index);

    return rv;

}

uint32_t
bcm56990_b0_pt_cci_reg_addr_gap_get(int unit, bcmdrd_sid_t sid)
{
    int blktype;
    uint32_t gap = 10; /* sbus v6*/

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    if (blktype == BLKTYPE_XLPORT) {
        /* sbus v4 */
        gap = 8;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "sid[%s] add_gap %d\n"),
               bcmdrd_pt_sid_to_name(unit, sid), gap));
    return gap;
}

int
bcm56990_b0_pt_cci_ctr_evict_control_get(int unit, bcmdrd_sid_t sid,
                                    bcmdrd_sid_t *ctrl_sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(get_control_sid(unit, sid, ctrl_sid));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56990_b0_pt_cci_ctr_evict_enable_field_get(int unit,
                                              bcmdrd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    *fid = COUNTER_POOL_ENABLEf;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pt_cci_ctr_evict_clr_on_read_field_get(int unit,
                                                   bcmdrd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    *fid = CLR_ON_READf;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pt_cci_ctr_evict_default_enable_list_get(int unit,
                                                     size_t *size,
                                                     bcmdrd_sid_t *sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sid, SHR_E_PARAM);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    *size = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pt_cci_fifodma_chan_get(int unit)
{
    return MEM_FIFO_DMA_CHANNEL_0;
}

int
bcm56990_b0_pt_cci_ctr_evict_fifo_sym_get(int unit, bcmdrd_sid_t *sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sid, SHR_E_PARAM);
    *sid = CENTRAL_CTR_EVICTION_FIFOm;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pt_cci_ctr_evict_fifo_entry_get(int unit,
                                            uint32_t *evict,
                                            uint32_t *pool,
                                            bcmptm_cci_ctr_info_t *info)
{
    bcmbd_pt_dyn_info_t *dyn;
    uint32_t val[2] = {0};
    CENTRAL_CTR_EVICTION_FIFOm_t ctr_evict;
    int id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(evict, SHR_E_PARAM);
    SHR_NULL_CHECK(pool, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    dyn = &info->dyn_info;
    SHR_NULL_CHECK(dyn, SHR_E_PARAM);

    sal_memcpy(&ctr_evict, evict, sizeof(CENTRAL_CTR_EVICTION_FIFOm_t));
    val[0] = CENTRAL_CTR_EVICTION_FIFOm_POOL_NUMf_GET(ctr_evict);
    if ((val[0] < 1) || (val[0] > COUNTOF(ctr_eviction))) {
            SHR_IF_ERR_EXIT(SHR_E_BADID);
    }
    /* Thread info from  PIPE_NUMf[0], pipe info from PIPE_NUMf[2:1] */
    val[1] = CENTRAL_CTR_EVICTION_FIFOm_PIPE_NUMf_GET(ctr_evict);
    id = ((val[0] - 1) * 2) + (val[1] & 0x1);
    info->sid = ctr_eviction[id].sid;
    dyn->tbl_inst = val[1] >> 1;
    dyn->index = CENTRAL_CTR_EVICTION_FIFOm_INDEXf_GET(ctr_evict);
    CENTRAL_CTR_EVICTION_FIFOm_COUNT_Bf_GET(ctr_evict, val);
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, info->sid, pool, COUNTER_Bf, val));

    val[0] = val[1] = 0;
    CENTRAL_CTR_EVICTION_FIFOm_COUNT_Af_GET(ctr_evict, val);
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, info->sid, pool, COUNTER_Af, val));

exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcm56990_b0_pt_cci_ctr_evict_fifo_buf_size_get(int unit)
{
    return CTR_EVICT_ENTRY_NUM;
}

int
bcm56990_b0_pt_cci_ctr_evict_field_num_get(int unit,
                                           bcmptm_cci_ctr_mode_t ctr_mode,
                                           size_t *num)
{
    int rv = SHR_E_NONE;

    switch (ctr_mode) {
    case NORMAL_MODE:
        *num = 2;
    break;
    case WIDE_MODE:
        *num = 2;
    break;
    case SLIM_MODE:
        *num = 3;
    break;
    default:
        rv = SHR_E_PARAM;
    break;
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctr_evict_field_width_get(int unit,
                                             bcmptm_cci_ctr_mode_t ctr_mode,
                                             size_t *width)
{
    int rv = SHR_E_NONE;

    switch (ctr_mode) {
    case NORMAL_MODE:
        *width = NORMAL_MODE_WIDTH;
    break;
    case WIDE_MODE:
        *width = WIDE_MODE_WIDTH;
    break;
    case SLIM_MODE:
        *width = SLIM_MODE_WIDTH;
    break;
    default:
        rv = SHR_E_PARAM;
    break;
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctr_evict_normal_val_get(int unit,
                                            size_t fld_width,
                                            uint32_t *entry,
                                            size_t size_e,
                                            uint64_t *ctr,
                                            size_t size_ctr)
{
    int rv = SHR_E_NONE;
    size_t i;

    for (i = 0; i < size_ctr; i++) {
        /* Get the HW counter Value  for field */
        bcmptm_cci_buff32_field_copy(
            (uint32_t *)&ctr[i], 0, entry, i * fld_width, fld_width);
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctr_evict_wide_val_get(int unit,
                                          size_t fld_width,
                                          uint32_t *entry,
                                          size_t size_e,
                                          uint64_t *ctr,
                                          size_t size_ctr)
{
    int rv = SHR_E_NONE;
    uint32_t c[2] = {0};

    bcmptm_cci_buff32_field_copy((uint32_t *)&c[0], 0, entry, 0, 32);
    bcmptm_cci_buff32_field_copy((uint32_t *)&c[1], 0, entry, 32, 32);
    ctr[0] = ((uint64_t)c[1] << 32) | c[0];
    if (fld_width - 64 > 0) {
        c[0] = 0;
        bcmptm_cci_buff32_field_copy((uint32_t *)&c[0], 0,
            entry, 64, fld_width - 64);
        ctr[1] =  c[0];
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctr_evict_slim_val_get(int unit,
                                          size_t fld_width,
                                          uint32_t *entry,
                                          size_t size_e,
                                          uint32_t *ctr,
                                          size_t size_ctr)
{
    int rv = SHR_E_NONE;
    size_t i;

    for (i = 0; i < size_ctr; i++) {
        /* Get the HW counter Value  for field */
         bcmptm_cci_buff32_field_copy(&ctr[i], 0, entry,
            i *  fld_width, fld_width);
    }

    return rv;
}

int
bcm56990_b0_pt_cci_ctr_evict_fifo_enable_sym_get(int unit,
                                                 bcmdrd_sid_t *sid,
                                                 bcmdrd_fid_t *fid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sid, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    *sid = CENTRAL_CTR_EVICTION_CONTROLr;
    *fid = FIFO_ENABLEf;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pt_cci_ctr_evict_update_mode_num_get(int unit,
                                                 size_t *num)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(num, SHR_E_PARAM);
    *num = UPDATE_MODE_MAX;

exit:
    SHR_FUNC_EXIT();
}

