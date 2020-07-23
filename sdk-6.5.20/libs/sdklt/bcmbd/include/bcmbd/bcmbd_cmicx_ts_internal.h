/*! \file bcmbd_cmicx_ts_internal.h
 *
 * TimeSync driver internal functions for CMICx register access.
 *
 * The register offset header files must be provided from the
 * device-specific TimeSync drivers that operate on the CMICx
 * TimeSync hardware.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_TS_INTERNAL_H
#define BCMBD_CMICX_TS_INTERNAL_H

/*******************************************************************************
 * Private functions
 */

static inline int
bcmbd_cmicx_ts_timestamp_enable_get(int unit, int ts_inst, bool *enable)
{
    NS_TIMESYNC_TS_COUNTER_ENABLEr_t ctr_en;

    if (READ_NS_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, &ctr_en) < 0) {
        return -1;
    }
    *enable = (NS_TIMESYNC_TS_COUNTER_ENABLEr_ENABLEf_GET(ctr_en) != 0);

    return 0;
}

static inline int
bcmbd_cmicx_ts_timestamp_enable_set(int unit, int ts_inst, bool enable)
{
    int ioerr = 0;
    NS_TIMESYNC_TS_FREQ_CTRL_FRAC_UPPERr_t upr;
    NS_TIMESYNC_TS_FREQ_CTRL_FRAC_LOWERr_t lwr;
    NS_TIMESYNC_TS_COUNTER_ENABLEr_t ctr_en;

    if (enable) {
        ioerr += READ_NS_TIMESYNC_TS_FREQ_CTRL_FRAC_UPPERr(unit, ts_inst, &upr);
        /* 500 Mhz NCO - 2.000 ns */
        NS_TIMESYNC_TS_FREQ_CTRL_FRAC_UPPERr_UINCf_SET(upr, 0x2000);
        ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_FRAC_UPPERr(unit, ts_inst, upr);

        ioerr += READ_NS_TIMESYNC_TS_FREQ_CTRL_FRAC_LOWERr(unit, ts_inst, &lwr);
        NS_TIMESYNC_TS_FREQ_CTRL_FRAC_LOWERr_LINCf_SET(lwr, 0);
        ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_FRAC_LOWERr(unit, ts_inst, lwr);
    }

    /* Enable/Disable timestamp generation */
    ioerr += READ_NS_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, &ctr_en);
    NS_TIMESYNC_TS_COUNTER_ENABLEr_ENABLEf_SET(ctr_en, enable);
    ioerr += WRITE_NS_TIMESYNC_TS_COUNTER_ENABLEr(unit, ts_inst, ctr_en);

    return ioerr;
}

static inline int
bcmbd_cmicx_ts_timestamp_get(int unit, int ts_inst, uint64_t *timestamp)
{
    int ioerr = 0;
    uint64_t timestamp_upr, timestamp_lwr;
    NS_TIMESYNC_TS_TIMESTAMP_LOWER_STATUSr_t lwr;
    NS_TIMESYNC_TS_TIMESTAMP_UPPER_STATUSr_t upr;

    /* Do an immediate capture */
    ioerr += READ_NS_TIMESYNC_TS_TIMESTAMP_UPPER_STATUSr(unit, ts_inst, &upr);
    ioerr += READ_NS_TIMESYNC_TS_TIMESTAMP_LOWER_STATUSr(unit, ts_inst, &lwr);

    timestamp_lwr = NS_TIMESYNC_TS_TIMESTAMP_LOWER_STATUSr_LTSf_GET(lwr);
    timestamp_upr = NS_TIMESYNC_TS_TIMESTAMP_UPPER_STATUSr_UTSf_GET(upr);
    /* HW supports 48-bit nanoseconds + 4-bit fractions of nanoseconds. */
    *timestamp = (timestamp_upr << 32) | (timestamp_lwr);

    return ioerr;
}

static inline int
bcmbd_cmicx_ts_timestamp_set(int unit, int ts_inst, uint64_t timestamp)
{
    int ioerr = 0;
    uint32_t acc2, acc1, acc0;
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_2r_t ts_acc2;
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_1r_t ts_acc1;
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_0r_t ts_acc0;

    /*
     * Set 52-bit timestamp (48-bit nanoseconds + 4-bit fractions) to
     * 92-bit accumulator (48-bit nanoseconds + 44-bit fractions) to adjust
     * the timestamp counter value.
     * acc2: [27:0] nanoseconds
     * acc1: [31:12] nanoseconds, [11:0] fractions of nanoseconds
     * acc0: [31:0] fractions of nanoseconds
     */
    acc2 = (timestamp >> 24) & 0xFFFFFFF;
    acc1 = (timestamp & 0xFFFFFF) << 8;
    acc0 = 0;
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_2r_SET(ts_acc2, acc2);
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_1r_SET(ts_acc1, acc1);
    NS_TIMESYNC_TS_INIT_ACCUMULATOR_0r_SET(ts_acc0, acc0);
    ioerr += WRITE_NS_TIMESYNC_TS_INIT_ACCUMULATOR_2r(unit, ts_inst, ts_acc2);
    ioerr += WRITE_NS_TIMESYNC_TS_INIT_ACCUMULATOR_1r(unit, ts_inst, ts_acc1);
    ioerr += WRITE_NS_TIMESYNC_TS_INIT_ACCUMULATOR_0r(unit, ts_inst, ts_acc0);

    return ioerr;
}

static inline int
bcmbd_cmicx_ts_timestamp_offset_get(int unit, int ts_inst, uint64_t *ts_offset)
{
    int ioerr = 0;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr_t ofs_u;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr_t ofs_l;

    ioerr += READ_NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr(unit, ts_inst, &ofs_u);
    ioerr += READ_NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr(unit, ts_inst, &ofs_l);
    *ts_offset = NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr_UPHASEf_GET(ofs_u);
    *ts_offset = (*ts_offset << 32) |
                 NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr_LPHASEf_GET(ofs_l);

    return ioerr;
}

static inline int
bcmbd_cmicx_ts_timestamp_offset_set(int unit, int ts_inst,
                                    uint64_t ts_offset, uint64_t update_at_ns)
{
    int ioerr = 0;
    uint32_t val;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr_t ofs_u;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr_t ofs_l;
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_UPPERr_t upd_u;
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_LOWERr_t upd_l;

    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr_CLR(ofs_u);
    val = ts_offset >> 32;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr_UPHASEf_SET(ofs_u, val);
    val = ts_offset & 0xFFFFFFFF;
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr_CLR(ofs_l);
    NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr_LPHASEf_SET(ofs_l, val);
    ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_UPPERr(unit, ts_inst, ofs_u);
    ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_OFFSET_LOWERr(unit, ts_inst, ofs_l);

    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_LOWERr_CLR(upd_l);
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_UPPERr_CLR(upd_u);
    val = update_at_ns & 0xFFFFFFFF;
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_LOWERr_MATCH_LPHASEf_SET(upd_l, val);
    val = update_at_ns >> 32;
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_UPPERr_MATCH_UPHASEf_SET(upd_u, val);
    NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_UPPERr_ENABLEf_SET(upd_u, 0x1);

    ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_LOWERr(unit, ts_inst, upd_l);
    ioerr += WRITE_NS_TIMESYNC_TS_FREQ_CTRL_UPDATE_UPPERr(unit, ts_inst, upd_u);

    return ioerr;
}

#endif /* BCMBD_CMICX_TS_INTERNAL_H */
