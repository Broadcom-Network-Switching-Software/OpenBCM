/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.c
 * Purpose:
 * Requires:
 */
#include <shared/bsl.h>

#ifdef  INCLUDE_AVS
#include <soc/avs.h>
#include <soc/i2c.h>

#define AVS_LEGACY_THRESHOLD   (0)  /* Adopt the original threshold algorithm*/

static soc_avs_control_t *soc_avs_ctrl[SOC_MAX_NUM_DEVICES];
static soc_avs_functions_t *soc_avs_functions[SOC_MAX_NUM_DEVICES] = {NULL};
static soc_avs_vrm_access_t *soc_vrm_access_functions[SOC_MAX_NUM_DEVICES] = {NULL};
static uint32 _soc_last_set_voltage[SOC_MAX_NUM_DEVICES];


static int _cent_freq_thr[] = {
     27368.00,    /* CENT_FREQ_THRESHOLD_0 */
     23800.00,    /* CENT_FREQ_THRESHOLD_1 */
     19704.00,    /* CENT_FREQ_THRESHOLD_2 */
     11384.50,    /* CENT_FREQ_THRESHOLD_3 */
     37474.00,    /* CENT_FREQ_THRESHOLD_4 */
     32060.50,    /* CENT_FREQ_THRESHOLD_5 */
     26603.00,    /* CENT_FREQ_THRESHOLD_6 */
     15597.00,    /* CENT_FREQ_THRESHOLD_7 */
    105960.00,    /* CENT_FREQ_THRESHOLD_8 */
     89260.00,    /* CENT_FREQ_THRESHOLD_9 */
     75010.00,    /* CENT_FREQ_THRESHOLD_10*/
     42939.00,    /* CENT_FREQ_THRESHOLD_11*/
     77610.00,    /* CENT_FREQ_THRESHOLD_12*/
     63710.00,    /* CENT_FREQ_THRESHOLD_13*/
     52685.00,    /* CENT_FREQ_THRESHOLD_14*/
     29877.50,    /* CENT_FREQ_THRESHOLD_15*/
     73335.00,    /* CENT_FREQ_THRESHOLD_16*/
     58880.00,    /* CENT_FREQ_THRESHOLD_17*/
     50385.00,    /* CENT_FREQ_THRESHOLD_18*/
     29011.50,    /* CENT_FREQ_THRESHOLD_19*/
     43638.00,    /* CENT_FREQ_THRESHOLD_20*/
     35133.50,    /* CENT_FREQ_THRESHOLD_21*/
     30099.00,    /* CENT_FREQ_THRESHOLD_22*/
     17083.00,    /* CENT_FREQ_THRESHOLD_23*/
     08787.00,    /* CENT_FREQ_THRESHOLD_24*/
     07574.50,    /* CENT_FREQ_THRESHOLD_25*/
     06485.00,    /* CENT_FREQ_THRESHOLD_26*/
     05713.50,    /* CENT_FREQ_THRESHOLD_27*/
     05986.00,    /* CENT_FREQ_THRESHOLD_28*/
     07330.50,    /* CENT_FREQ_THRESHOLD_29*/
     03933.15,    /* CENT_FREQ_THRESHOLD_30*/
     04304.60,    /* CENT_FREQ_THRESHOLD_31*/
     01610.80,    /* CENT_FREQ_THRESHOLD_32*/
     01273.10,    /* CENT_FREQ_THRESHOLD_33*/
     01610.80,    /* CENT_FREQ_THRESHOLD_34*/
     01273.10,    /* CENT_FREQ_THRESHOLD_35*/
};

static int _rmt_freq_thr[] = {
    23800.00,    /* RMT_FREQ_THRESHOLD_GS */
    11384.50,     /* RMT_FREQ_THRESHOLD_GH */
};


static int _cent_fth_slope[] = {
     72120,     /* CENT_FREQ_THRESHOLD_SLOPE_0 */
     82720,     /* CENT_FREQ_THRESHOLD_SLOPE_1 */
     71370,     /* CENT_FREQ_THRESHOLD_SLOPE_2 */
     66720,     /* CENT_FREQ_THRESHOLD_SLOPE_3 */
     97800,     /* CENT_FREQ_THRESHOLD_SLOPE_4 */
    109680,     /* CENT_FREQ_THRESHOLD_SLOPE_5 */
     94700,     /* CENT_FREQ_THRESHOLD_SLOPE_6 */
     89820,     /* CENT_FREQ_THRESHOLD_SLOPE_7 */
    277250,     /* CENT_FREQ_THRESHOLD_SLOPE_8 */
    301000,     /* CENT_FREQ_THRESHOLD_SLOPE_9 */
    269500,     /* CENT_FREQ_THRESHOLD_SLOPE_10*/
    245600,     /* CENT_FREQ_THRESHOLD_SLOPE_11*/
    214250,     /* CENT_FREQ_THRESHOLD_SLOPE_12*/
    231250,     /* CENT_FREQ_THRESHOLD_SLOPE_13*/
    201250,     /* CENT_FREQ_THRESHOLD_SLOPE_14*/
    180750,     /* CENT_FREQ_THRESHOLD_SLOPE_15*/
    205000,     /* CENT_FREQ_THRESHOLD_SLOPE_16*/
    216000,     /* CENT_FREQ_THRESHOLD_SLOPE_17*/
    189500,     /* CENT_FREQ_THRESHOLD_SLOPE_18*/
    172500,     /* CENT_FREQ_THRESHOLD_SLOPE_19*/
    122980,     /* CENT_FREQ_THRESHOLD_SLOPE_20*/
    128750,     /* CENT_FREQ_THRESHOLD_SLOPE_21*/
    114080,     /* CENT_FREQ_THRESHOLD_SLOPE_22*/
    101950,     /* CENT_FREQ_THRESHOLD_SLOPE_23*/
     28500,     /* CENT_FREQ_THRESHOLD_SLOPE_24*/
     29300,     /* CENT_FREQ_THRESHOLD_SLOPE_25*/
     34480,     /* CENT_FREQ_THRESHOLD_SLOPE_26*/
     32700,     /* CENT_FREQ_THRESHOLD_SLOPE_27*/
     33080,     /* CENT_FREQ_THRESHOLD_SLOPE_28*/
     33650,     /* CENT_FREQ_THRESHOLD_SLOPE_29*/
     32190,     /* CENT_FREQ_THRESHOLD_SLOPE_30*/
     35620,     /* CENT_FREQ_THRESHOLD_SLOPE_31*/
      7660,     /* CENT_FREQ_THRESHOLD_SLOPE_32*/
      5490,     /* CENT_FREQ_THRESHOLD_SLOPE_33*/
      8670,     /* CENT_FREQ_THRESHOLD_SLOPE_34*/
      7290,     /* CENT_FREQ_THRESHOLD_SLOPE_35*/
};

int
soc_avs_inited(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return(0);
    }
    if (SOC_AVS_CONTROL(unit) == NULL) {
        return (0);
    }
    return ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_INITED) != 0);
}

int
soc_avs_track_inited(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return(0);
    }
    if (SOC_AVS_CONTROL(unit) == NULL) {
        return (0);
    }
    return ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_TRACK_INITED) != 0);
}
#ifdef BCM_SBUSDMA_SUPPORT
STATIC sbusdma_desc_handle_t
       _soc_avs_rosc_handles[SOC_MAX_NUM_DEVICES][SOC_AVS_ROSC_TYPE_ALL];

void
_soc_sbusdma_avs_cb(int unit, int status, sbusdma_desc_handle_t handle,
                    void *data)
{
    soc_avs_control_t *avs;
    int i;

    avs = SOC_AVS_CONTROL(unit);

    if (status == SOC_E_NONE) {
        if(PTR_TO_INT(data) == SOC_AVS_ROSC_TYPE_REMOTE){
            avs->rmt_rosc_count_sync = 1;
        }
        if(PTR_TO_INT(data) == SOC_AVS_ROSC_TYPE_CENTRAL){
            avs->cent_rosc_count_sync= 1;
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_AVS,
                  (BSL_META_U(unit,
                              "avs ROSC count SBUSDMA failed: type %d\n"),
                   PTR_TO_INT(data)));
        if (status == SOC_E_TIMEOUT) {
            (void)soc_sbusdma_desc_delete(unit, handle);
            for (i = 0; i < SOC_AVS_ROSC_TYPE_ALL; i++) {
                if (_soc_avs_rosc_handles[unit][i] == handle) {
                    _soc_avs_rosc_handles[unit][i] = 0;
                    break;
                }
            }
        }
    }
}

int
_soc_avs_sbusdma_run(int unit, int type)
{
    int ret=SOC_E_NONE;
    sal_usecs_t dma_timeout = 1000000;
    int  timer_started = 0;
    soc_timeout_t       to;

    do{
        ret = soc_sbusdma_desc_run(unit, _soc_avs_rosc_handles[unit][type]);
        if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
            if (ret == SOC_E_INIT) {
                break;
            }
            /* timeout if sbus dma has been busy for too long. */
            if (!timer_started) {
                soc_timeout_init(&to, 2 * dma_timeout, 0);
                timer_started = 1;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_AVS,
                         (BSL_META_U(unit,
                                     "sbusdma desc run operation timeout\n")));
                break;
            }
            sal_usleep(10);
        }
    } while ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT));

    return ret;
}

int
soc_avs_sbusdma_desc_setup(int unit)
{
    soc_sbusdma_desc_ctrl_t ctrl;
    soc_sbusdma_desc_cfg_t cfg;
    uint32 *buff;
    int alloc_size;
    uint8 cent_acc_type, rmt_acc_type;
    int cent_blkoff, rmt_blkoff;
    soc_reg_t cent_reg, rmt_reg;
    uint32 cent_addr, rmt_addr;
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    /* allocate memory */
    alloc_size = (avs_info->num_centrals + avs_info->num_remotes) *
                sizeof(uint32);
    if ((buff = (uint32 *)soc_cm_salloc(unit, alloc_size,
                                        "Storage for _soc_avs_osc_count dma"))
                                        == NULL) {
        return (SOC_E_MEMORY);
    }
    sal_memset(buff, 0, alloc_size);
    avs->cent_desc_buff = buff;
    avs->rmt_desc_buff = &buff[avs_info->num_centrals];

    cent_reg = avs_info->cen_osc_reg;
    rmt_reg = avs_info->rmt_osc_reg;

    if (!SOC_REG_IS_VALID(unit, cent_reg) ||
        !SOC_REG_IS_VALID(unit, rmt_reg)) {
        return (SOC_E_INTERNAL);
    }

    cent_addr = soc_reg_addr_get(unit, cent_reg, REG_PORT_ANY, 0,
                                 SOC_REG_ADDR_OPTION_NONE,
                                 &cent_blkoff, &cent_acc_type);

    rmt_addr = soc_reg_addr_get(unit, rmt_reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE,
                                &rmt_blkoff, &rmt_acc_type);

    /* central */
    sal_memset(&ctrl, 0, sizeof(soc_sbusdma_desc_ctrl_t));
    sal_memset(&cfg, 0, sizeof(soc_sbusdma_desc_cfg_t));
    ctrl.flags = 0;
    ctrl.cfg_count = 1;
    ctrl.buff = avs->cent_desc_buff;
    ctrl.cb = _soc_sbusdma_avs_cb;
    ctrl.data = INT_TO_PTR(SOC_AVS_ROSC_TYPE_CENTRAL);
    sal_strncpy(ctrl.name, "CROSC COUNTERS", sizeof(ctrl.name)-1);
    cfg.acc_type = cent_acc_type;
    cfg.blk = cent_blkoff;
    cfg.addr = cent_addr;
    cfg.width = SOC_REG_IS_64(unit, cent_reg) ? 2 : 1;
    cfg.count = avs_info->num_centrals;
    cfg.addr_shift = 10;
    SOC_AVS_FREE_IF_ERROR_RETURN
        (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                 &_soc_avs_rosc_handles[unit][SOC_AVS_ROSC_TYPE_CENTRAL]),
                                 buff);

    /* remote */
    sal_memset(&ctrl, 0, sizeof(soc_sbusdma_desc_ctrl_t));
    sal_memset(&cfg, 0, sizeof(soc_sbusdma_desc_cfg_t));
    ctrl.flags = 0;
    ctrl.cfg_count = 1;
    ctrl.buff = avs->rmt_desc_buff;
    ctrl.cb = _soc_sbusdma_avs_cb;
    ctrl.data = INT_TO_PTR(SOC_AVS_ROSC_TYPE_REMOTE);
    sal_strncpy(ctrl.name, "RROSC COUNTERS", sizeof(ctrl.name)-1);
    cfg.acc_type = rmt_acc_type;
    cfg.blk = rmt_blkoff;
    cfg.addr = rmt_addr;
    cfg.width = SOC_REG_IS_64(unit, rmt_reg) ? 2 : 1;
    cfg.count = avs_info->num_remotes;
    cfg.addr_shift = 8;
    SOC_AVS_FREE_IF_ERROR_RETURN
        (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                 &_soc_avs_rosc_handles[unit][SOC_AVS_ROSC_TYPE_REMOTE]),
                                 buff);

    avs->rmt_rosc_count_sync = 0;
    avs->cent_rosc_count_sync = 0;

    return SOC_E_NONE;
}
int
soc_avs_sbusdma_desc_free(int unit)
{
    uint8 i, state = 0;
    int ret, err = 0;
    soc_avs_control_t *avs;

    avs = SOC_AVS_CONTROL(unit);

    if (avs->cent_desc_buff != NULL) {
        soc_cm_sfree(unit, avs->cent_desc_buff);
        avs->cent_desc_buff = NULL;
        avs->rmt_desc_buff = NULL;
    }
    avs->cent_rosc_count_sync = 0;
    avs->rmt_rosc_count_sync = 0;

    for (i = 0; i < SOC_AVS_ROSC_TYPE_ALL; i++) {
        if (_soc_avs_rosc_handles[unit][i]) {
            do {
                (void)soc_sbusdma_desc_get_state(unit,
                                                 _soc_avs_rosc_handles[unit][i],
                                                 &state);
                if (state) {
                    sal_usleep(10);
                }
            } while (state);
            ret = soc_sbusdma_desc_delete(unit,
                                          _soc_avs_rosc_handles[unit][i]);
            if (ret) {
                err++;
            }
            _soc_avs_rosc_handles[unit][i] = 0;
        }
    }
    return err;
}
#endif

/*
 * Function:
 *  _soc_avs_xbmp_check
 * Purpose:
 *  Check if the index of the ROSC in exclude bitmap or not
 * Parameters:
 *  type - (IN)SOC_AVS_ROSC_TYPE_REMOTE or SOC_AVS_ROSC_TYPE_CENTRAL
 *  index - (IN) index of ROSC
 * Returns:
 *  SOC_AVS_BOOL_TRUE  :the index should be exclude in the process
 *  SOC_AVS_BOOL_FALSE :the index should be used in the process
 */
STATIC int
_soc_avs_xbmp_check(int unit, int type, int index)
{
    if (type == SOC_AVS_ROSC_TYPE_REMOTE) {
        return SOC_AVS_OSC_EXCLUDED(index,
            SOC_AVS_XBMP(unit)->rmt_xbmp[index/NUM_BITS_PER_XBMP] |
            SOC_AVS_INFO(unit)->rmt_xbmp[index/NUM_BITS_PER_XBMP]);
    }
    if (type == SOC_AVS_ROSC_TYPE_CENTRAL) {
        return SOC_AVS_OSC_EXCLUDED(index,
            SOC_AVS_XBMP(unit)->cent_xbmp[index/NUM_BITS_PER_XBMP] |
            SOC_AVS_INFO(unit)->cent_xbmp[index/NUM_BITS_PER_XBMP]);
    }
    return SOC_AVS_BOOL_TRUE;

}

int
soc_avs_ioctl(int unit, soc_avs_ioctl_t opcode, void* data, int len)
{

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    if ((SOC_AVS_FUNCTIONS(unit) != NULL) &&
        (SOC_AVS_FUNCTIONS(unit)->ioctl != NULL)) {
        return SOC_AVS_FUNCTIONS(unit)->ioctl(unit, opcode, data, len);
    }
    return (SOC_E_UNAVAIL);

}

/*
 * Function:
 *  _soc_avs_pvt_value_read
 * Purpose:
 *  Read the 10b data value for specified PVT_MON.
 * Parameters:
 *  pvtmon - (IN) specifies one of PVT_TEMPERATURE, PVT_1V_0, PVT_1V_1, etc
 *  data - (OUT) 10b pvtmon.data
 * Returns:
 *  data - (OUT) 10b pvtmon.data
 */
#define _SOC_AVS_PVTMON_MAX_REREADS 10
STATIC int
_soc_avs_pvt_value_read(int unit, soc_avs_pvt_t pvtmon, uint32 *data)
{
    int j;
    uint32 regval;
    uint32 valid_data = 0;
    uint32 done = 0;

    if (data == NULL) {
        return (SOC_E_PARAM);
    }
    *data = 0;
    regval = 0;
    for (j = 0; j < _SOC_AVS_PVTMON_MAX_REREADS; j++) {
        switch (pvtmon) {
        case SOC_AVS_PVT_TEMPERATURE:
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr(unit,
                                                                         &regval));
            break;
        case SOC_AVS_PVT_1V_0:
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUSr(unit,
                                                                  &regval));
            break;
        default:
            *data = 0;
            return (SOC_E_PARAM);
        }
        /* all PVT monitor measurement status register have the same field format */
        valid_data = soc_reg_field_get(unit,
                                       AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr,
                                       regval, VALID_DATAf);
        done = soc_reg_field_get(unit,
                                 AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr,
                                 regval, DONEf);

        /* Wait for valid to be set */
        if (valid_data && done) {
            break;
        }
    }
    *data = soc_reg_field_get(unit,
                              AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr,
                              regval, DATAf);
    if (!valid_data) {
        return (SOC_E_FAIL);
    } else {
        return (SOC_E_NONE);
    }
}

/* This just assumes that the PVTMON sequencer did the measurement. */
/* Get the value from PVTMON for the specified value */
STATIC int
_soc_avs_pvtmon_get(int unit, soc_avs_pvt_t pvtmon, uint32 *data)
{
    int i;
    uint32 code, count, sum;

    /* Disable all but the particular measurement we're interested in */
    count = ~(1 << pvtmon) & 0x7F;
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, count));
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0r(unit, SOC_AVS_ALL_ONES));
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1r(unit, SOC_AVS_ALL_ONES));
    sal_usleep(SOC_AVS_PVT_DISABLE_DELAY);

    sum = 0;
    for (i = 0; i < _SOC_AVS_PVTMON_MAX_REREADS; i ++) {
        SOC_IF_ERROR_RETURN(
            _soc_avs_pvt_value_read(unit, pvtmon, &code));
        sum += code;
        sal_usleep(SOC_AVS_PVT_DISABLE_DELAY);
    }
    code = sum / _SOC_AVS_PVTMON_MAX_REREADS;
    *data = code;
    /* Re-enable all of the other measurements */
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, 0));
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0r(unit, 0));
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1r(unit, 0));

    return SOC_E_NONE;
}
STATIC int
_soc_avs_reset_measurement(int unit, soc_avs_pvt_t pvtmon)
{
    uint32 regval;
    uint32 m_init_pvt_mntr;

    /* set */
    SOC_IF_ERROR_RETURN(
        READ_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, &regval));

    m_init_pvt_mntr = (soc_reg_field_get(unit,
                                         AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
                                         regval, M_INIT_PVT_MNTRf));
    m_init_pvt_mntr |= (0x1 << pvtmon);

    soc_reg_field_set(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
                      &regval, M_INIT_PVT_MNTRf, m_init_pvt_mntr);

    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, regval));

    /* clr */
    SOC_IF_ERROR_RETURN(
        READ_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, &regval));

    m_init_pvt_mntr = (soc_reg_field_get(unit,
                                         AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
                                         regval, M_INIT_PVT_MNTRf));
    m_init_pvt_mntr &= ~(0x1 << pvtmon);

    soc_reg_field_set(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr,
                      &regval, M_INIT_PVT_MNTRf, m_init_pvt_mntr);

    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr(unit, regval));

    return (SOC_E_NONE);
}

/* Use Software Override to do measurements */
/* This just reads the requested register with debounce */
STATIC int
_soc_avs_sw_takeover_measure(int unit, soc_avs_pvt_t pvtmon, uint32 *code)
{
#define _SOC_AVS_RETRY_LOOPS 5
    /* found that we sometimes don't get valid data even after the 26 tries */
#define _SOC_AVS_AVERAGE_LOOPS 26
    /* the more loops the longer the process, but better the average */
#define _SOC_AVS_HW_MNTR_SW_CONTROLS_RESET_VALUE 0
#define _SOC_AVS_MAX_ITERATIONS 100
    /* don't let any loops run forever */

    int i, j, k;
    uint32 regval, busy = 1, count = 0, sum;

    if (code == NULL) {
        return (SOC_E_PARAM);
    }
    *code = 0;

    for (k = 0; k < _SOC_AVS_RETRY_LOOPS; k++) {
        *code = sum = count = 0;

        for (i = 0; i < _SOC_AVS_AVERAGE_LOOPS; i++) {

            /* These steps need to be done one-at-a-time */
            regval = 0;
            soc_reg_field_set(unit, AVS_REG_HW_MNTR_SW_CONTROLSr, &regval,
                              SW_TAKEOVERf, 1);
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, regval));

            soc_reg_field_set(unit, AVS_REG_HW_MNTR_SW_CONTROLSr, &regval,
                              SW_SENSOR_IDXf, pvtmon);
                /* sensors in 'PVT Monitor' group */
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, regval));

            soc_reg_field_set(unit, AVS_REG_HW_MNTR_SW_CONTROLSr, &regval,
                              SW_DO_MEASUREf, 1);
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, regval));

            /* _soc_avs_msec_sleep(500);
             * delay a bit before checking busy status */
            sal_usleep(1000);
                /* delay a bit before checking busy status */

            /* The busy comes on during processing and goes off when done */
            for (j = 0; j < _SOC_AVS_MAX_ITERATIONS; j++) {
                SOC_IF_ERROR_RETURN(
                    READ_AVS_REG_HW_MNTR_SW_MEASUREMENT_UNIT_BUSYr(unit,
                                                                   &regval));
                busy = soc_reg_field_get(unit,
                                         AVS_REG_HW_MNTR_SW_MEASUREMENT_UNIT_BUSYr,
                                         regval, BUSYf);
                if (busy == 0) break; /* from j for loop */
                /* _soc_avs_msec_sleep(1); delay a bit before checking again! */
            }

            /* We sometimes see the busy stuck on for some reason
             * (if its still on, dismiss this data) */
            if (busy == 1) {
                SOC_IF_ERROR_RETURN(
                    WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit,
                        _SOC_AVS_HW_MNTR_SW_CONTROLS_RESET_VALUE));
                continue; /* with next averaging loop */
            }

            if (i < 10) {
                continue; /* skip the first couple of results */
            }

            SOC_IF_ERROR_RETURN(_soc_avs_pvt_value_read(unit, pvtmon, code));
            if (*code == 0) {
                continue;
            }
                /* ignore results from this loop and continue with
                 * next averaging loop */

            sum += *code;
            count++;

            /*
             * LOG_VERBOSE(BSL_LS_SOC_AVS,
             * (BSL_META_U(unit,
             *             "i = %3d, code = %0d, sum = %0d, count = %0d"
             *             "\n"),
             *  i, *code, sum, count));
             */

            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, &regval));
            soc_reg_field_set(unit, AVS_REG_HW_MNTR_SW_CONTROLSr, &regval,
                              SW_DO_MEASUREf, 0);
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, regval));

            /* enable this to just read it once
             * break;
             */

        } /* _SOC_AVS_AVERAGE_LOOPS */

        /* Use an average value to dismiss the variance in the reads (this is
         * called debouncing */
        if (count != 0) {
            *code = sum/count;
            break; /* from _SOC_AVS_RETRY_LOOPS */
        }

        /* If we failed to get valid from this pass, reset for next pass */
        SOC_IF_ERROR_RETURN(_soc_avs_reset_measurement(unit, pvtmon));
    } /* _SOC_AVS_RETRY_LOOPS */


    SOC_IF_ERROR_RETURN(
        READ_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, &regval));
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SW_CONTROLSr, &regval,
                      SW_TAKEOVERf, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_SW_CONTROLSr(unit, regval));

    return (count == 0? SOC_E_FAIL : SOC_E_NONE);
}

int
soc_avs_temperature_get(int unit, int *temperature)
{
    uint32 value;
    soc_avs_info_t  *avs_info;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    if (temperature == NULL) {
        return (SOC_E_PARAM);
    }
    avs_info = SOC_AVS_INFO(unit);
    *temperature = 0;
    if (avs_info->avs_flags & SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER) {
        /* use software override to do measurements */
        SOC_IF_ERROR_RETURN(
            _soc_avs_sw_takeover_measure(unit, SOC_AVS_PVT_TEMPERATURE, &value));
    } else {
        /* PVTMON sequencer did the measurement. */
        SOC_IF_ERROR_RETURN(
            _soc_avs_pvtmon_get(unit, SOC_AVS_PVT_TEMPERATURE, &value));
    }
    SOC_IF_ERROR_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_PVTMON_TEMP_GET, &value, 0));
    *temperature = value;
    return SOC_E_NONE;
}

int
soc_avs_pvtmon_voltage_get(int unit, uint32 *voltage)
{
    uint32 value;
    soc_avs_info_t  *avs_info;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    if (voltage == NULL) {
        return (SOC_E_PARAM);
    }
    avs_info = SOC_AVS_INFO(unit);
    *voltage = 0;
    if (avs_info->avs_flags & SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER) {
        /* use software override to do measurements */
        SOC_IF_ERROR_RETURN(
            _soc_avs_sw_takeover_measure(unit, SOC_AVS_PVT_1V_0, &value));
    } else {
        /* PVTMON sequencer did the measurement. */
        SOC_IF_ERROR_RETURN(
            _soc_avs_pvtmon_get(unit, SOC_AVS_PVT_1V_0, &value));
    }
    SOC_IF_ERROR_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_PVTMON_VOLTAGE_GET, &value, 0));
    *voltage = value;

    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "==== unit = %d, pvt get voltage = %0d (0.1 mV)\n"),
                 unit, *voltage));
    return SOC_E_NONE;
}

STATIC int
_soc_avs_pvt_value_write(int unit, soc_avs_pvt_t pvtmon, uint32 data)
{
    int rval;

    switch (pvtmon) {
    case SOC_AVS_PVT_1V_0:
        rval = 1;
        SOC_IF_ERROR_RETURN(WRITE_AVS_REG_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, rval));
        SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(WRITE_AVS_REG_PVT_MNTR_CONFIG_DAC_CODEr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_AVS_REG_PVT_MNTR_CONFIG_MIN_DAC_CODEr(unit, data));
        SOC_IF_ERROR_RETURN(WRITE_AVS_REG_PVT_MNTR_CONFIG_MAX_DAC_CODEr(unit, data));

        rval = 0x1180;
        /* PVTMON control bits : RMON_sel = 0x6; Mode = 0x4 */
        SOC_IF_ERROR_RETURN(WRITE_AVS_REG_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, rval));
        break;

    default:
        return SOC_E_PARAM;

    }

    return SOC_E_NONE;
}

int
soc_avs_pvtmon_voltage_set(int unit, uint32 voltage)
{
    uint32 value;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    value = voltage;
    SOC_IF_ERROR_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_PVTMON_VOLTAGE_SET, &value, 0));

    SOC_IF_ERROR_RETURN(
        _soc_avs_pvt_value_write(unit, SOC_AVS_PVT_1V_0, value));

    /* Delay for the voltage to get reflected. */
    sal_msleep(SOC_AVS_DAC_DELAY);

    return SOC_E_NONE;
}

#define AVS_V_TO_UV_SCALE   (1000000)
#define AVS_SCALING_FACTOR  (100)
/* voltage :(OUT) voltage value (0.1mV granularity)*/
int
soc_avs_voltage_get(int unit, uint32 *voltage)
{
    if (voltage == NULL) {
        return SOC_E_PARAM;
    }

    if ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_VRM_INITED) == 0) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_AVS_VRM_ACCESS(unit)) {
        if (SOC_AVS_VRM_ACCESS(unit)->voltage_get){
            return SOC_AVS_VRM_ACCESS(unit)->voltage_get(unit,
                                                         voltage);
        }
    }
    if (SOC_AVS_VRM_INFO(unit)) {
#ifdef COMPILER_HAS_DOUBLE
        double volts;
#else
        int volts;
#endif
        int rv, fd;

        if ((SOC_AVS_VRM_INFO(unit)->mux_id != -1) &&
            SOC_AVS_VRM_INFO(unit)->profile.mux_enable_value) {
            fd = SOC_AVS_VRM_INFO(unit)->mux_id;
            rv = soc_i2c_device(unit, fd)->driver->write(unit, fd, 0,
                    &SOC_AVS_VRM_INFO(unit)->profile.mux_enable_value, 1);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        }

        fd = SOC_AVS_VRM_INFO(unit)->vrm_id;
        *voltage = 0;
        rv = soc_i2c_device(unit, fd)->driver->ioctl(unit, fd,
                                                     PMBUS_IOC_READ_VOUT,
                                                     &volts, 0);
        if (SOC_FAILURE(rv)) {
            return rv;
        }

#ifdef COMPILER_HAS_DOUBLE
        /* V -> 0.1mV */
        *voltage = (uint32)((volts * AVS_V_TO_UV_SCALE) / AVS_SCALING_FACTOR);
#else
        /* uV -> 0.1mV */
        *voltage = volts / AVS_SCALING_FACTOR;
#endif
        if (*voltage == 0) {
            /* Some i2c device report non-error on un-supported type */
            return SOC_E_UNAVAIL;
        }
    }
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *  soc_avs_voltage_set
 * Purpose:
 *  Set the voltage to external voltage supplier
 * Parameters
 *  voltage - (IN) voltage corresponding to desired voltage (0.1mV granularity)
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_voltage_set(int unit, uint32 voltage)
{
    int rv = SOC_E_UNAVAIL;

    if ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_VRM_INITED) == 0) {
        return SOC_E_UNAVAIL;
    }
    LOG_VERBOSE(BSL_LS_SOC_AVS,
        (BSL_META_U(unit,
                    "==== unit = %d, request to "
                    "set voltage = %0d (0.1 mV)\n"),
         unit, voltage));

    if (voltage > SOC_AVS_MAX_VOLTAGE) {
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "Fail to set voltage %d "
                             "(> SOC_AVS_MAX_VOLTAGE) try %d \n"),
                  voltage, SOC_AVS_MAX_VOLTAGE));
        voltage = SOC_AVS_MAX_VOLTAGE;
    } else if(voltage < SOC_AVS_MIN_VOLTAGE){
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "Fail to set voltage %d "
                             "(< SOC_AVS_MIN_VOLTAGE) try %d \n"),
                  voltage, SOC_AVS_MIN_VOLTAGE));
        voltage = SOC_AVS_MIN_VOLTAGE;
    }
    if (SOC_AVS_VRM_ACCESS(unit)) {
        if (SOC_AVS_VRM_ACCESS(unit)->voltage_get){
            rv = SOC_AVS_VRM_ACCESS(unit)->voltage_set(unit,
                                                       voltage);
        }
    } else if (SOC_AVS_VRM_INFO(unit)) {
#ifdef COMPILER_HAS_DOUBLE
        double volts;
#else
        int volts;
#endif
        int fd;

        if ((SOC_AVS_VRM_INFO(unit)->mux_id != -1) &&
            SOC_AVS_VRM_INFO(unit)->profile.mux_enable_value) {
            fd = SOC_AVS_VRM_INFO(unit)->mux_id;
            rv = soc_i2c_device(unit, fd)->driver->write(unit, fd, 0,
                    &SOC_AVS_VRM_INFO(unit)->profile.mux_enable_value, 1);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        }

#ifdef COMPILER_HAS_DOUBLE
        /* 0.1mV -> V */
        volts = (double)voltage * AVS_SCALING_FACTOR / AVS_V_TO_UV_SCALE;
#else
        /* 0.1mV -> uV */
        volts = voltage * AVS_SCALING_FACTOR;
#endif
        fd = SOC_AVS_VRM_INFO(unit)->vrm_id;
        rv = soc_i2c_device(unit, fd)->driver->ioctl(unit, fd,
                                                     PMBUS_IOC_SET_VOUT,
                                                     &volts, 0);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "Fail to set voltage %d\n"),
                  voltage));
       return (rv);
    } else {
       /* remember last_set_voltage for avs_track voltage adjusment*/
       _soc_last_set_voltage[unit] = voltage;
       return (SOC_E_NONE);
    }
}

/*
 * Function:
 *  _soc_avs_calc_v3_for_ref_f3
 * Purpose:
 *  Computes voltage v3 corresponding to ref_freq f3.
 * Parameters:
 *  f2 - (IN)Frequency at low voltage
 *  v2 - (IN)Low voltage
 *  m - (IN)Slope
 *  f3 - (IN)Frequency at high voltage
 * Returns:
 *  Voltage v3 (units of 0.1mV)
 */
STATIC uint32
_soc_avs_calc_v3_for_ref_f3(uint32 f2, uint32 v2, uint32 m, uint32 f3)
{
    uint32 v3;
    uint32 vdelta;

    /* find v3 corresponding to f3 */
    if (f2 >= f3) {
        vdelta = ((f2 - f3) * SOC_AVS_S2)/m;
        SOC_AVS_ASSERT(v2 > vdelta);
        v3 = v2 - vdelta;
    } else {
        vdelta = ((f3 - f2) * SOC_AVS_S2)/m;
        v3 = v2 + vdelta;
    }

    return (v3);
}

/* We need the frequency units to be in 10K but multiplying by 10K will cause
 * the numerator to overflow in frequency calculation.
 * So use 1K before division and multiply by 10 after.
 */
#define SOC_AVS_FREQ_DIVIDER 1000U
#define SOC_AVS_FREQ_DIVIDER_ADJUSTMENT 10U
STATIC uint32
_soc_avs_reg_to_freq(int unit, uint32 rosc_count)
{
    uint32 freq;
    int rosc_count_mode;
    int ref_clk_freq; /* in MHz*/
    int max_ref_clk_counter;
    soc_avs_info_t  *avs_info;

    avs_info = SOC_AVS_INFO(unit);
    rosc_count_mode = avs_info->rosc_count_mode;
    ref_clk_freq = avs_info->ref_clk_freq;
    max_ref_clk_counter = (avs_info->measurement_time_control * 256U)
                          + 255U;
    freq = ( ((ref_clk_freq * rosc_count) * SOC_AVS_FREQ_DIVIDER) /
             (max_ref_clk_counter * rosc_count_mode)
           ) * SOC_AVS_FREQ_DIVIDER_ADJUSTMENT;

    return (freq);
}

STATIC uint32
_soc_avs_freq_to_reg(int unit, uint32 freq)
{
    uint32 reg;
    int rosc_count_mode;
    int ref_clk_freq; /* in MHz*/
    int max_ref_clk_counter;
    soc_avs_info_t  *avs_info;

    avs_info = SOC_AVS_INFO(unit);
    rosc_count_mode = avs_info->rosc_count_mode;
    ref_clk_freq = avs_info->ref_clk_freq;
    max_ref_clk_counter = (avs_info->measurement_time_control * 256U)
                          + 255U;
    reg = ((freq / ref_clk_freq) * (max_ref_clk_counter * rosc_count_mode)) /
          (SOC_AVS_FREQ_DIVIDER * SOC_AVS_FREQ_DIVIDER_ADJUSTMENT);
    return reg;
}


/*
 * Function:
 *  _soc_avs_get_cent_osc_ref_freq
 * Purpose:
 *  Get certain central ROSC's reference_frequency value
 * Parameters:
 *  osc_num - (IN)specifies one of the central_osc
 *  ref_freq - (OUT)reference_frequency_threshold
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_get_cent_osc_ref_freq(int unit, int osc_num, uint32 *ref_freq)
{
    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    if (ref_freq == NULL){
        return (SOC_E_PARAM);
    }
    if (osc_num > SOC_AVS_INFO(unit)->num_centrals) {
        return (SOC_E_PARAM);
    }
    *ref_freq = 0; /* default: consider it un-implemented osc */
    *ref_freq = (SOC_AVS_CONTROL(unit)->cen_freq_thr[osc_num]);
    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_avs_get_rmt_osc_ref_freq
 * Purpose:
 *  Get certain Remote ROSC's HVT and SVT reference_frequency value
 * Parameters:
 *  osc_num - (IN)specifies one of the central_osc
 *  ref_freq_h - (OUT)HVT reference_frequency_threshold
 *  ref_freq_s - (OUT)SVT reference_frequency_threshold
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_get_rmt_osc_ref_freq(int unit, int osc_num, uint32
        *ref_freq_h, uint32 *ref_freq_s)
{
    if ((ref_freq_h == NULL) || (ref_freq_s == NULL)) {
        return (SOC_E_PARAM);
    }
    *ref_freq_h = 0;
    *ref_freq_s = 0;
    *ref_freq_h = (SOC_AVS_CONTROL(unit)->rmt_freq_thr[1]);
    *ref_freq_s = (SOC_AVS_CONTROL(unit)->rmt_freq_thr[0]);
    return SOC_E_NONE;
}

/*
 * There is a divide_by_2 for cent_rosc_counter in NTSW chips.
 * so real_count_value = 2*value_that_we_read
 */
#define SOC_AVS_CENT_OSC_COUNT_MULTIPLIER 2

/*
 * Function:
 *  _soc_avs_cent_osc_count_get
 * Purpose:
 *  Get specified central ROSC count
 * Parameters:
 *  start_osc - (IN)specifies first central_osc
 *  num_osc - (IN)specifies number of central_osc
 *  ref_freq_s - (OUT)specifies array to return oscillator counts
 * Returns:
 *  SOC_E_xxx
 */

STATIC uint32
_soc_avs_cent_osc_count_get(int unit, int start_osc, int num_osc, uint32 *count)
{
    int i, failed = 0, n;
    soc_avs_info_t  *avs_info;
#ifdef BCM_SBUSDMA_SUPPORT
    soc_avs_control_t *avs;
    int use_dma = 0;
    soc_timeout_t       to;
    sal_usecs_t dma_timeout = 1000000;
#endif

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    avs_info = SOC_AVS_INFO(unit);
    if (count == NULL) {
        return (SOC_E_PARAM);
    }
    sal_memset(count, 0, num_osc*(sizeof(*count)));

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma) &&
        (avs_info->avs_flags & SOC_AVS_INFO_F_RSOC_COUNT_DMA)){
        avs = SOC_AVS_CONTROL(unit);
        use_dma = 1;
        (void)_soc_avs_sbusdma_run(unit, SOC_AVS_ROSC_TYPE_CENTRAL);

        soc_timeout_init(&to, 2 * dma_timeout, 0);
        for(;;) {
            if(avs->cent_rosc_count_sync){
                avs->cent_rosc_count_sync = 0;
                break;
            }
            if (soc_timeout_check(&to)) {
                use_dma = 0;
                LOG_WARN(BSL_LS_SOC_AVS,
                         (BSL_META_U(unit,
                                     "cent rosc count read operation "
                                     "timeout\n")));
                break;
            }
        }
    }
    if (use_dma) {
        sal_memcpy(count, &SOC_AVS_CONTROL(unit)->cent_desc_buff[start_osc],
                   num_osc * sizeof(uint32));

    }else
#endif
    {
        /* read osc_counts */
        for (i = start_osc; i < start_osc + num_osc; i++) {
            n = i - start_osc;
            if (avs_info->avs_flags &
                    SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
                if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
                    count[n] = 0;
                    continue;
                }
            }
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr(
                    unit, i, &count[n]));
        }
    }
    /* process the counts */
    for (i = start_osc; i < start_osc + num_osc; i++) {
        n = i - start_osc;
        if (avs_info->avs_flags & SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
                count[n] = 0;
                continue;
            }
        }
        if (soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr,
                    count[n], VALIDf) == 0) {
            failed = 1;
            LOG_WARN(BSL_LS_SOC_AVS,
                     (BSL_META_U(unit,
                                 "Fail to get cent osc[%d] %x\n"),
                      i, count[i]));
        }
        count[n] = SOC_AVS_CENT_OSC_COUNT_MULTIPLIER *
                   soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr,
                                     count[n], DATAf);
    }
    if (failed == 1) {
        return (SOC_E_FAIL);
    } else {
        return (SOC_E_NONE);
    }
}
/*
 * Function:
 *  _soc_avs_rmt_osc_count_get
 * Purpose:
 *  Get specified remote ROSC COUNT_S, COUNT_H
 * Parameters:
 *  start_osc - (IN)specifies first central_osc
 *  num_osc - (IN)specifies number of central_osc
 *  count_h - (OUT)specifies array to return COUNT_H values
 *  count_s - (OUT)specifies array to return COUNT_S values
 * Returns:
 *  SOC_E_xxx
 */

STATIC int
_soc_avs_rmt_osc_count_get(int unit, int start_osc, int num_osc,
        uint32 *count_h, uint32 *count_s)
{
    int i, n;
    soc_avs_info_t  *avs_info;
#ifdef BCM_SBUSDMA_SUPPORT
    soc_avs_control_t *avs;
    int use_dma = 0;
    soc_timeout_t       to;
    sal_usecs_t dma_timeout = 1000000;
#endif

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    avs_info = SOC_AVS_INFO(unit);
    if ((count_h == NULL) || (count_s == NULL)) {
        return (SOC_E_PARAM);
    }
    sal_memset(count_h, 0, num_osc*(sizeof(*count_h)));
    sal_memset(count_s, 0, num_osc*(sizeof(*count_s)));

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma) &&
        (avs_info->avs_flags & SOC_AVS_INFO_F_RSOC_COUNT_DMA)){
        avs = SOC_AVS_CONTROL(unit);
        use_dma = 1;
        (void)_soc_avs_sbusdma_run(unit, SOC_AVS_ROSC_TYPE_REMOTE);

        soc_timeout_init(&to, 2 * dma_timeout, 0);
        for(;;) {
            if (avs->rmt_rosc_count_sync){
                avs->rmt_rosc_count_sync = 0;
                break;
            }
            if (soc_timeout_check(&to)) {
                use_dma = 0;
                LOG_WARN(BSL_LS_SOC_AVS,
                         (BSL_META_U(unit,
                                     "rmt rosc count read operation "
                                     "timeout\n")));
                break;
            }
        }
    }
    if (use_dma) {
        sal_memcpy(count_s, &SOC_AVS_CONTROL(unit)->rmt_desc_buff[start_osc],
                   num_osc * sizeof(uint32));
    }else
#endif
    {
        /* read count registers */
        for (i = start_osc; i < start_osc + num_osc; i++) {
            n = i - start_osc;
            if (avs_info->avs_flags &
                SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
                if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
                    count_h[n] = 0;
                    count_s[n] = 0;
                    continue;
                }
            }
            /* use (count_s + i) to hold regval */
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr(unit, i, &count_s[n]));
            sal_msleep(SOC_AVS_REMOTE_DELAY);
        }
    }
    /* extract fields from regval */
    for (i = start_osc; i < start_osc + num_osc; i++) {
        n = i - start_osc;
        if (avs_info->avs_flags & SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
                count_h[n] = 0;
                count_s[n] = 0;
                continue;
            }
        }
        count_h[n] = soc_reg_field_get(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr,
                                       count_s[n], COUNT_Hf);
        count_s[n] = soc_reg_field_get(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_COUNTr,
                                       count_s[n], COUNT_Sf);
    }

    return (SOC_E_NONE);

}

int
soc_avs_osc_count_dump(int unit, int type)
{
    int i, alloc_size;
    uint32 *alloc_ptr = NULL,
           *pcount_cent_osc = NULL,
           *pcount_rmt_osc_h = NULL,
           *pcount_rmt_osc_s = NULL;
    soc_avs_info_t  *avs_info;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs_info = SOC_AVS_INFO(unit);

    /* allocate memory */
    alloc_size = 3 * SOC_AVS_MAX(avs_info->num_centrals, avs_info->num_remotes)
                    *sizeof(uint32);
    if ((alloc_ptr = (uint32 *)sal_alloc(alloc_size,
                    "Storage for _soc_avs_osc_count_dump")) == NULL) {
        return (SOC_E_MEMORY);
    }

    pcount_cent_osc = alloc_ptr;
    pcount_rmt_osc_h = pcount_cent_osc +
                       avs_info->num_centrals;
    pcount_rmt_osc_s = pcount_rmt_osc_h +
                       avs_info->num_remotes;

    sal_memset(alloc_ptr, 0, alloc_size);

    /* read all cent,rmt osc counts  */
    if ((type == SOC_AVS_ROSC_TYPE_CENTRAL) || (type == SOC_AVS_ROSC_TYPE_ALL)) {
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_cent_osc_count_get(unit, avs_info->first_cent,
                                        avs_info->num_centrals,
                                        pcount_cent_osc), alloc_ptr);
    }
    if ((type == SOC_AVS_ROSC_TYPE_REMOTE) || (type == SOC_AVS_ROSC_TYPE_ALL)) {
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_rmt_osc_count_get(unit, avs_info->first_rmt,
                                       avs_info->num_remotes,
                                       pcount_rmt_osc_h, pcount_rmt_osc_s),
                                       alloc_ptr);
    }

    if ((type == SOC_AVS_ROSC_TYPE_CENTRAL) || (type == SOC_AVS_ROSC_TYPE_ALL)) {
        LOG_CLI((BSL_META_U(unit, " index, cent_osc_count\n")));
        for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
            LOG_CLI((BSL_META_U(unit, "%d, %05d \n"), i, pcount_cent_osc[i]));
        }
    }

    if ((type == SOC_AVS_ROSC_TYPE_REMOTE) || (type == SOC_AVS_ROSC_TYPE_ALL)) {
        LOG_CLI((BSL_META_U(unit,
                            " index, count_rmt_osc_h, count_rmt_osc_s\n")));
        for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "%d, %05d, %5d \n"),
                                i, pcount_rmt_osc_h[i],pcount_rmt_osc_s[i]));
        }
    }
    sal_free(alloc_ptr);
    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_avs_reg_field_all_ones
 * Purpose:
 *  Get the all ones value of spefic register's field
 * Parameters:
 *  soc_reg_t - (IN)specifies register
 *  soc_field_t - (IN)specifies field
 * Returns:
 *  return the all ones field value
 */
uint32
_soc_avs_reg_field_all_ones(int unit, soc_reg_t reg, soc_field_t field)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
            "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }
    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;
    } else {
        mask = -1;
    }
    return mask;
}


#define SOC_AVS_ROSC_CONTROL_INIT_VALUE 0x7FFF50CF

/*
 * Function:
 *  _soc_avs_initialize_oscs
 * Purpose:
 *  Initialiaze central, remote osc
 * Parameters:
 *  unit - (IN)specifies unit
 * Returns:
 *  SOC_E_xxx
 */
int
_soc_avs_initialize_oscs(int unit)
{
    soc_avs_info_t  *avs_info;
    uint32 regval, fieldval;
    soc_avs_reg_info_t *osc_cen_init_list,*osc_cen_thr_en_list;
    int i;
    soc_reg_t reg;
    soc_field_t field;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs_info = SOC_AVS_INFO(unit);
    /* AVS_MEASUREMENT_TIME_CONTROLr.LIMITf = AVS_MEASUREMENT_TIME_CONTROL*/
    regval = 0;
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROLr,
                      &regval, LIMITf, avs_info->measurement_time_control);
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_ROSC_MEASUREMENT_TIME_CONTROLr(unit,
                                                             regval));

    /* AVS_REG_HW_MNTR_ROSC_COUNTING_MODEr.MODEf = avs_info->rosc_count_mode*/
    regval = 0;
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_ROSC_COUNTING_MODEr, &regval,
                      MODEf, avs_info->rosc_count_mode);
    SOC_IF_ERROR_RETURN(
        WRITE_AVS_REG_HW_MNTR_ROSC_COUNTING_MODEr(unit, regval));

    SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_OSC_INIT, NULL, 0));

    if (avs_info->osc_cen_init_info) {
        /* always enable all oscillators at start-up */
        osc_cen_init_list = avs_info->osc_cen_init_info;
        for (i = 0; osc_cen_init_list[i].reg_name != -1 ; i++) {
            reg = osc_cen_init_list[i].reg_name;
            field = osc_cen_init_list[i].reg_field;
            /* always enable all oscillators at start-up */
            regval = 0;
            fieldval = _soc_avs_reg_field_all_ones(unit, reg, field);
            soc_reg_field_set(unit, reg,
                &regval, field, fieldval);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit,
                reg, REG_PORT_ANY, 0, regval));
            /* different form BCG, we don't clear this after enable */
        }
    }

    SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_OSC_THRESHOLD, NULL, 0));

    if (avs_info->osc_cen_thr_en_info) {
        osc_cen_thr_en_list = avs_info->osc_cen_thr_en_info;
        /* Set up the Threshold enables */
        for (i = 0; osc_cen_thr_en_list[i].reg_name != -1 ; i++) {
            reg = osc_cen_thr_en_list[i].reg_name;
            field = osc_cen_thr_en_list[i].reg_field;
            regval = 0;
            if (avs_info->avs_flags & SOC_AVS_INFO_F_USE_SOFTWARE_TAKEOVER) {
            /* If using software takeover then DISABLE all the
               threshold enables */
                fieldval = 0;
            } else {
            /* If NOT using software takeover then ENABLE all the
               threshold enables */
                fieldval = _soc_avs_reg_field_all_ones(unit, reg, field);
            }
            soc_reg_field_set(unit, reg,
                              &regval, field, fieldval);
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));
        }
    }

    /* Remote oscillators are on PMB bus on newer parts */
    regval = SOC_AVS_ROSC_CONTROL_INIT_VALUE;
    for (i = 0; i < avs_info->num_remotes; i++) {
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
            continue;
        }
        SOC_IF_ERROR_RETURN(
            WRITE_AVS_REG_PMB_SLAVE_AVS_ROSC_CONTROLr(unit, i, regval));

        sal_msleep(SOC_AVS_REMOTE_DELAY);
    }

    return (SOC_E_NONE);
}
/* to determine v1s, v2s - local_fit */
#define SOC_AVS_VHIGH           SOC_AVS_UINT(10200)
#define SOC_AVS_V1_DIFF         SOC_AVS_UINT(200) /* 20 mV*/
#define SOC_AVS_V1_MINUS_V2     SOC_AVS_UINT(300) /* 30 mV */
/* lowest slow-slow part voltage */
#define SOC_AVS_VLOW_SS         SOC_AVS_INT(9400)
#define SOC_AVS_V2S_INC_FOR_2ND_PASS SOC_AVS_UINT(280); /* 28mV */
#define SOC_AVS_VLOW_DIFF       SOC_AVS_INT(600) /* 60 mV */

#define SOC_AVS_VHI_VLO_MIN_DIFF    SOC_AVS_UINT(100) /* 10 mV */

/*
 * Function:
 *  _soc_avs_predict_vpred
 * Purpose:
 *   Applies (v1, v2), records (f1, f2),
 *   computes v3 for pre-defined ref f3, returns vpred = max(v3).
 * Parameters:
 *  dac_code_low - (IN)dac_code correspoding to v1
 *  dac_code_high - (IN)dac_code correspoding to v2
 *  vpred   - (OUT)The converged voltage
 * Returns:
 *  SOC_E_xxx
 */
int
_soc_avs_predict_vpred(int unit, int pass, uint32 dac_code_low, uint32
        dac_code_high, uint32 *vpred, uint32 *vlow, uint32 *vhigh)
{
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;
    int32 v3_max_cent_osc, v3_max_rmt_osc;
    int i, alloc_size;
    uint32 *alloc_ptr = NULL,
           *pcount_cent_osc_at_vhigh = NULL, *pcount_cent_osc_at_vlow = NULL,
           *pcount_rmt_osc_at_vhigh_h = NULL, *pcount_rmt_osc_at_vlow_h = NULL,
           *pcount_rmt_osc_at_vhigh_s = NULL, *pcount_rmt_osc_at_vlow_s = NULL;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    if (vpred == NULL){
        return SOC_E_PARAM;
    }
    SOC_AVS_ASSERT(dac_code_high > dac_code_low);
    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);
    /* Return Vpred value 0 in case of errors */
    *vpred = 0;

    /* allocate memory */
    alloc_size = (2 * avs_info->num_centrals + 4 * avs_info->num_remotes) *
                 sizeof(uint32);
    if ((alloc_ptr = (uint32 *)sal_alloc(alloc_size,
                    "Storage for _soc_avs_predict_vpred")) == NULL) {
        return (SOC_E_MEMORY);
    }
    pcount_cent_osc_at_vhigh = alloc_ptr;
    pcount_cent_osc_at_vlow = pcount_cent_osc_at_vhigh +
                              avs_info->num_centrals;
    pcount_rmt_osc_at_vhigh_h = pcount_cent_osc_at_vlow +
                                avs_info->num_centrals;
    pcount_rmt_osc_at_vlow_h = pcount_rmt_osc_at_vhigh_h +
                               avs_info->num_remotes;
    pcount_rmt_osc_at_vhigh_s = pcount_rmt_osc_at_vlow_h +
                                avs_info->num_remotes;
    pcount_rmt_osc_at_vlow_s = pcount_rmt_osc_at_vhigh_s +
                               avs_info->num_remotes;
    sal_memset(alloc_ptr, 0, alloc_size);

    if ((pass == 0) || (pass == -1)){
        /* Apply first voltage (high DAC is higher voltage) */
        SOC_AVS_FREE_IF_ERROR_RETURN(
            soc_avs_voltage_set(unit, dac_code_high), alloc_ptr);
        SOC_AVS_FREE_IF_ERROR_RETURN(
            soc_avs_voltage_get(unit, vhigh),alloc_ptr);
        avs->v1r_cache = *vhigh;

        /* read all cent,rmt osc counts (at vhigh) */
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_cent_osc_count_get(unit,avs_info->first_cent,
                                        avs_info->num_centrals,
                                        pcount_cent_osc_at_vhigh), alloc_ptr);
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_rmt_osc_count_get(unit, avs_info->first_rmt,
                                       avs_info->num_remotes,
                                       pcount_rmt_osc_at_vhigh_h,
                                       pcount_rmt_osc_at_vhigh_s), alloc_ptr);
        if (pass == 0) {
            /* save the v1s values to cache */
            sal_memcpy(avs->cent_v1s_osc_cache, pcount_cent_osc_at_vhigh,
                       avs_info->num_centrals * sizeof(uint32));
            sal_memcpy(avs->rmt_v1s_h_osc_cache, pcount_rmt_osc_at_vhigh_h,
                       avs_info->num_remotes * sizeof(uint32));
            sal_memcpy(avs->rmt_v1s_s_osc_cache, pcount_rmt_osc_at_vhigh_s,
                       avs_info->num_remotes * sizeof(uint32));
        }
    } else {
        /* restore the v1s value from cache */
        sal_memcpy(pcount_cent_osc_at_vhigh, avs->cent_v1s_osc_cache,
                   avs_info->num_centrals * sizeof(uint32));
        sal_memcpy(pcount_rmt_osc_at_vhigh_h, avs->rmt_v1s_h_osc_cache,
                   avs_info->num_remotes * sizeof(uint32));
        sal_memcpy(pcount_rmt_osc_at_vhigh_s, avs->rmt_v1s_s_osc_cache,
                   avs_info->num_remotes * sizeof(uint32));
        *vhigh = avs->v1r_cache;
    }
    /* Switch to the 2nd voltage and record the oscillator values
     * (low DAC is lower voltage) */
    SOC_AVS_FREE_IF_ERROR_RETURN(
        soc_avs_voltage_set(unit, dac_code_low),alloc_ptr);
    SOC_AVS_FREE_IF_ERROR_RETURN(
        soc_avs_voltage_get(unit, vlow),alloc_ptr);

   /* average read-write delta */
    avs_info->rw_delta = (((int)dac_code_high - (int)*vhigh) +
                          ((int)dac_code_low - (int)*vlow))
                          / 2;

    if (soc_property_get(unit,"use_rw_delta", 0) == 0){
        avs_info->rw_delta = 0;
    }
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "dac_code_high %d dac_code_low %d "
                            "vhigh %d vlow %d rw_delta %d\n"),
                 dac_code_high, dac_code_low, *vhigh, *vlow,
                 avs_info->rw_delta));
    /* Verify that AVS hardware is enabled on this board!.
     * If the difference in the two voltages is < 10mV then we don't do
     * AVS on this part!. */
    SOC_AVS_ASSERT(*vhigh > *vlow);
    if ((*vhigh - *vlow) < SOC_AVS_VHI_VLO_MIN_DIFF) {
        LOG_INFO(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "Voltage diff was < "
                             "%0d mV -- stopping AVS processing!\n"),
                  SOC_AVS_VHI_VLO_MIN_DIFF/10));
        sal_free(alloc_ptr);
        return (SOC_E_DISABLED);
    }

    /* read all cent, rmt osc counts (at vlow) */
    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_cent_osc_count_get(unit, avs_info->first_cent,
                                    avs_info->num_centrals,
                                    pcount_cent_osc_at_vlow), alloc_ptr);
    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_rmt_osc_count_get(unit, avs_info->first_rmt,
                                   avs_info->num_remotes,
                                   pcount_rmt_osc_at_vlow_h,
                                   pcount_rmt_osc_at_vlow_s), alloc_ptr);

    /* we want largest predicted voltage across all the usable oscillators */
    v3_max_cent_osc = 0;
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "cent_osc, v1, r_count1, f1, v2, r_count2,"
                            " f2, f3, v3, exclude, h/s\n")));
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        uint32 freq_at_vhigh, freq_at_vlow;
        uint32 f3;
        uint32 slope;
        uint32 v3;
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
            LOG_VERBOSE(BSL_LS_SOC_AVS,
                        (BSL_META_U(unit,
                                    "%d, %d, %d, -, %d, %d, -, -, -, 1, -\n"),
                         i, *vhigh, pcount_cent_osc_at_vhigh[i],
                         *vlow, pcount_cent_osc_at_vlow[i]));
            continue;
        }
        if ((pcount_cent_osc_at_vhigh[i] == 0) ||
            (pcount_cent_osc_at_vlow[i] == 0)) {
                LOG_VERBOSE(BSL_LS_SOC_AVS,
                            (BSL_META_U(unit,
                                        "%d, %d, %d, -, "
                                        "%d, %d, -, -, -, 0, -\n"),
                             i, *vhigh, pcount_cent_osc_at_vhigh[i],
                             *vlow, pcount_cent_osc_at_vlow[i]));
            continue;
        }

        freq_at_vhigh = _soc_avs_reg_to_freq(unit,
                                             pcount_cent_osc_at_vhigh[i]);
        freq_at_vlow = _soc_avs_reg_to_freq(unit,
                                            pcount_cent_osc_at_vlow[i]);

        SOC_AVS_ASSERT(freq_at_vhigh > freq_at_vlow);

        /* m = (f1-f2)/(v1-v2)
         * Scaling factor is added to improve accuracy.
         * Must remember slope is now in 10,000th */
        slope = (freq_at_vhigh - freq_at_vlow)*SOC_AVS_S2 / (*vhigh - *vlow);

        /* read pre-defined freq_threshold (f3) */
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_get_cent_osc_ref_freq(unit, i, &f3), alloc_ptr);

        /* find v3 corresponding to f3 */
        v3 = _soc_avs_calc_v3_for_ref_f3(freq_at_vlow, *vlow, slope, f3);

        avs->saved_voltage[i] = v3;
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit,
                                "%d, %d, %d, %d, %d, %d, %d, %d, %d, 0, -\n"),
                     i, *vhigh, pcount_cent_osc_at_vhigh[i], freq_at_vhigh,
                     *vlow, pcount_cent_osc_at_vlow[i], freq_at_vlow, f3, v3));
        /* max */
        v3_max_cent_osc = SOC_AVS_MAX(v3, v3_max_cent_osc);
    } /* for all cent_osc */
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Max voltage across "
                            "Central oscillators = %0d\n"),
                 v3_max_cent_osc));

    v3_max_rmt_osc = 0;
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "rmt_osc, v1, r_count1, f1, v2, r_count2,"
                            " f2, f3, v3, exclude, h/s\n")));
    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        uint32 freq_at_vhigh_s, freq_at_vhigh_h;
        uint32 freq_at_vlow_s, freq_at_vlow_h;
        uint32 f3_s, f3_h;
        uint32 slope;
        uint32 v3_s, v3_h;

        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
            LOG_VERBOSE(BSL_LS_SOC_AVS,
                        (BSL_META_U(unit,
                                    "%d, %d, %d, -, %d, %d, -, -, -, 1, h\n"),
                                    i, *vhigh, pcount_rmt_osc_at_vhigh_h[i],
                                    *vlow, pcount_rmt_osc_at_vlow_h[i]));
            LOG_VERBOSE(BSL_LS_SOC_AVS,
                        (BSL_META_U(unit,
                                    "%d, %d, %d, -, %d, %d, -, -, -, 1, s\n"),
                                    i, *vhigh, pcount_rmt_osc_at_vhigh_s[i],
                                    *vlow, pcount_rmt_osc_at_vlow_s[i]));
            continue;
        }

        freq_at_vhigh_h = _soc_avs_reg_to_freq(unit,
                                               pcount_rmt_osc_at_vhigh_h[i]);
        freq_at_vhigh_s = _soc_avs_reg_to_freq(unit,
                                               pcount_rmt_osc_at_vhigh_s[i]);

        freq_at_vlow_h = _soc_avs_reg_to_freq(unit,
                                              pcount_rmt_osc_at_vlow_h[i]);
        freq_at_vlow_s = _soc_avs_reg_to_freq(unit,
                                              pcount_rmt_osc_at_vlow_s[i]);

        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_get_rmt_osc_ref_freq(unit, i, &f3_h, &f3_s), alloc_ptr);

        SOC_AVS_ASSERT(freq_at_vhigh_h > freq_at_vlow_h);
        slope = (freq_at_vhigh_h - freq_at_vlow_h)*SOC_AVS_S2 / (*vhigh - *vlow);
        v3_h = _soc_avs_calc_v3_for_ref_f3(freq_at_vlow_h, *vlow, slope, f3_h);

        SOC_AVS_ASSERT(freq_at_vhigh_s > freq_at_vlow_s);
        slope = (freq_at_vhigh_s - freq_at_vlow_s)*SOC_AVS_S2 / (*vhigh - *vlow);
        v3_s = _soc_avs_calc_v3_for_ref_f3(freq_at_vlow_s, *vlow, slope, f3_s);


        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit,
                                "%d, %d, %d, %d, %d, %d, %d, %d, %d, 0, h\n"),
                     i, *vhigh, pcount_rmt_osc_at_vhigh_h[i], freq_at_vhigh_h,
                     *vlow, pcount_rmt_osc_at_vlow_h[i],freq_at_vlow_h, f3_h,
                     v3_h));

        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit,
                                "%d, %d, %d, %d, %d, %d, %d, %d, %d, 0, s\n"),
                     i, *vhigh, pcount_rmt_osc_at_vhigh_s[i], freq_at_vhigh_s,
                     *vlow, pcount_rmt_osc_at_vlow_s[i],freq_at_vlow_s, f3_s,
                     v3_s));

        v3_max_rmt_osc = SOC_AVS_MAX(v3_h, v3_max_rmt_osc);
        v3_max_rmt_osc = SOC_AVS_MAX(v3_s, v3_max_rmt_osc);
    }
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "Max voltage across "
                            "Remote oscillators = %0d\n"),
                 v3_max_rmt_osc));
    /* Final Vpred */
    *vpred = SOC_AVS_MAX(v3_max_cent_osc, v3_max_rmt_osc);

    sal_free(alloc_ptr);

    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "v1s=%0d, v2s=%0d, "
                         "v1r=%0d, v2r=%0d, vpred=%0d\n"),
              dac_code_high, dac_code_low, *vhigh, *vlow, *vpred));

    return (SOC_E_NONE);
}
#define DAC_CALIBRATION
/*
 * Function:
 *  _soc_avs_find_final_voltage
 * Purpose:
 *   Runs avs_predict_vpred, adds margins, sets voltage to computed Vavs
 * Parameters:
 *  vpred   - (OUT)Output from _soc_avs_predict_vpred
 *  vavs_sv - (OUT)Output vpred plus margins
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_find_final_voltage(int unit, uint32 *vpred, uint32 *vavs_sv)
{
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;
    int32 new_vmin_avs;
    uint32 ccv;
    int32 v2s, v1s;
    int pass;
    int32 vsum, vavs;
    int32 slope_avs, intercept_avs, vmargin;
    int32 vmargin_high, vmargin_low;
    int32 vlow, vhigh;
#ifdef DAC_CALIBRATION
    int32 slope_dac;
    int32 avs_dac_code;
#endif

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    new_vmin_avs = avs_info->vmin_avs;

    SOC_AVS_LOCK(avs);
    if (SOC_AVS_MARGIN(unit)->vmargin_high){
        vmargin_high = SOC_AVS_MARGIN(unit)->vmargin_high;
    } else {
        vmargin_high = avs_info->vmargin_high;
    }
    if (SOC_AVS_MARGIN(unit)->vmargin_low){
        vmargin_low = SOC_AVS_MARGIN(unit)->vmargin_low;
    } else {
        vmargin_low = avs_info->vmargin_low;
    }
    slope_avs = ((vmargin_high - vmargin_low)*SOC_AVS_S2) /
                ((avs_info->vmax_avs - avs_info->vmin_avs) +
                 (vmargin_low - vmargin_high));
    avs->slope_margin = slope_avs;

    intercept_avs = vmargin_low -
                    ((slope_avs * (avs_info->vmin_avs - vmargin_low)) /
                     SOC_AVS_S2);
    avs->intercept_margin = intercept_avs;

    SOC_AVS_UNLOCK(avs);
    /* Read cur_core_voltage (will depend on OTP) */
    SOC_IF_ERROR_RETURN(soc_avs_voltage_get(unit, &ccv));

    /* Determine v1s, v2s (voltage points for local fit) */
    v1s = ccv + SOC_AVS_V1_DIFF;
    v2s = v1s - SOC_AVS_V1_MINUS_V2;

    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "ccv=%0d, "
                         "v1s=%0d, v2s=%0d\n"),
                         ccv, v1s, v2s));

    /* First pass uses rough values (local fit) and the second uses
     * the more refined (wider range) values (global fit) */
    for (pass = 0; pass < 2; pass++) {
        SOC_IF_ERROR_RETURN(_soc_avs_predict_vpred(unit, pass, v2s,
                    v1s, vpred, (uint32 *)&vlow, (uint32 *)&vhigh));
        if (*vpred == 0) return 0;

        vmargin = (slope_avs * (int32)(*vpred))/SOC_AVS_S2 + intercept_avs;

        vsum = (int32)(*vpred) + vmargin;
        /* Make sure this is NEVER outside the allowable voltage min/max
         * values */
        if (vsum < new_vmin_avs) {
            vavs = new_vmin_avs;
        } else if (vsum > avs_info->vmax_avs) {
            vavs = avs_info->vmax_avs;
        } else {
            vavs = vsum;
        }
        LOG_INFO(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "pass=%0d, "
                             "vpred=%0d, vavs=%0d, vmargin %0d\n"),
                  pass, *vpred, vavs, vmargin));
#ifdef DAC_CALIBRATION
        slope_dac = ((v1s - v2s) * SOC_AVS_S2) / (vhigh - vlow);
        avs_dac_code = ((slope_dac * vavs) - (slope_dac * vlow))/SOC_AVS_S2 + v2s;

        if (vavs < SOC_AVS_VLOW_SS) {
            v2s = avs_dac_code + SOC_AVS_V2S_INC_FOR_2ND_PASS;
        }
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "avs_dac_code calibration "
                                "slope_dac %d vavs %d avs_dac_code %d "
                                "v2s %d \n"),
                     slope_dac, vavs, avs_dac_code, v2s));
#else
        /* On next pass, adjust low value (v2s) to get more precise calculation
         * on second pass */
        /* This turns the process from a local fit to a global fit process */
        /* For some chip boot from 0.9V, in below case, no need to do 2nd pass.
                      orig: if(vavs  > 0.94)  exit;
                      change to: voltage diff if(v2s-vavs) < 0.06 exit; */
        if ((v2s - vavs) > SOC_AVS_VLOW_DIFF) {
            SOC_AVS_ASSERT(vavs > 0);
            v2s = (uint32)vavs + SOC_AVS_V2S_INC_FOR_2ND_PASS;
            SOC_AVS_ASSERT(v1s > v2s);
        } else {
           /* no need to do 2nd pass - because v1s, v2s remain same as
                      1st pass */
            break;
        }
#endif
    }

    SOC_AVS_ASSERT(vavs > 0);
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "set avs_dac_code %d (vavs %d)\n"),
              avs_dac_code, vavs));
    SOC_IF_ERROR_RETURN(soc_avs_voltage_set(unit, (uint32)avs_dac_code));
    if (avs_info->avs_flags & SOC_AVS_INFO_F_USE_LVM_FLAG){
        /* check LVM */
        SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
            soc_avs_ioctl(unit, SOC_AVS_CTRL_LVM_MODE, NULL, 0));
    }

    *vavs_sv = (uint32)*vpred;
    SOC_IF_ERROR_RETURN(soc_avs_voltage_get(unit, (uint32 *)&vlow));
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit, "Find Voltage %d\n"), vlow));
    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_avs_rmt_osc_thr_get
 * Purpose:
 *   Get hi, lo h, s thresholds for specified range of
 *   remote_oscillators.
 * Parameters:
 *  start_osc   - (IN)specifies first remote_osc
 *  num_osc  - (IN)specifies number of remote_osc
 *  lo_thr_h - (OUT)low threshold_h
 *  hi_thr_h - (OUT)high threshold_h
 *  lo_thr_s - (OUT)low threshold_s
 *  hi_thr_s - (OUT)high threshold_s
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_rmt_osc_thr_get(int unit, int start_osc, int num_osc,
        uint32 *lo_thr_h, uint32 *hi_thr_h, uint32 *lo_thr_s, uint32 *hi_thr_s)
{
    int i, cached = 0, n;
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    if ((lo_thr_h == NULL) || (hi_thr_h == NULL) || (lo_thr_s == NULL) ||
            (hi_thr_s == NULL)) {
        return (SOC_E_PARAM);
    }
    sal_memset(lo_thr_h, 0, num_osc*(sizeof(*lo_thr_h)));
    sal_memset(hi_thr_h, 0, num_osc*(sizeof(*hi_thr_h)));
    sal_memset(lo_thr_s, 0, num_osc*(sizeof(*lo_thr_s)));
    sal_memset(hi_thr_s, 0, num_osc*(sizeof(*hi_thr_s)));

    if (avs_info->avs_flags &
            SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE) {
        if ((avs->rmt_h_thr_cache == NULL) ||
            (avs->rmt_s_thr_cache == NULL)) {
            cached = 0;
        } else {
            cached = 1;
        }
    }

    if (cached) {
        sal_memcpy(lo_thr_h, &avs->rmt_h_thr_cache[start_osc],
            num_osc * sizeof(uint32));
        sal_memcpy(lo_thr_s, &avs->rmt_s_thr_cache[start_osc],
            num_osc * sizeof(uint32));
    } else {
        /* read thr_h, thr_s registers */
        for (i = start_osc; i < start_osc + num_osc; i++) {
            n = i - start_osc;
            if (SOC_AVS_INFO(unit)->avs_flags &
                SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
                if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
                    continue;
                }
            }
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr(unit, i,
                &lo_thr_h[n]));
            sal_msleep(SOC_AVS_REMOTE_DELAY);

            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr(unit, i,
                &lo_thr_s[n]));
            sal_msleep(SOC_AVS_REMOTE_DELAY);
        }
    }
    /* extract hi,lo_thr_h from registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
        n = i - start_osc;
        if (SOC_AVS_INFO(unit)->avs_flags &
            SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
                hi_thr_h[n] = 0;
                lo_thr_h[n] = 0;
                hi_thr_s[n] = 0;
                lo_thr_s[n] = 0;
                continue;
            }
        }

        hi_thr_h[n] = soc_reg_field_get(unit,
                AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr, lo_thr_h[n],
                THRESH_HIf);
        lo_thr_h[n] = soc_reg_field_get(unit,
                AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr, lo_thr_h[n],
                THRESH_LOf);
        hi_thr_s[n] = soc_reg_field_get(unit,
                AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr, lo_thr_s[n],
                THRESH_HIf);
        lo_thr_s[n] = soc_reg_field_get(unit,
                AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr, lo_thr_s[n],
                THRESH_LOf);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_avs_cent_osc_thr_get
 * Purpose:
 *   Get hi, lo counter thresholds for specified range of central
 *   oscillators
 * Parameters:
 *  start_osc   - (IN)specifies first remote_osc
 *  num_osc  - (IN)specifies number of remote_osc
 *  lo_thr - (OUT)low threshold
 *  hi_thr - (OUT)high threshold
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_cent_osc_thr_get(int unit, int start_osc, int num_osc,
        uint32 *lo_thr, uint32 *hi_thr)
{
    int i, cached = 0, n = 0;
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    if ((lo_thr == NULL) || (hi_thr == NULL)) {
        return (SOC_E_PARAM);
    }
    sal_memset(lo_thr, 0, num_osc*(sizeof(*lo_thr)));
    sal_memset(hi_thr, 0, num_osc*(sizeof(*hi_thr)));

    if (avs_info->avs_flags &
            SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE) {
        if ((avs->cent_lo_thr_cache == NULL) ||
            (avs->cent_hi_thr_cache == NULL)) {
            cached = 0;
        } else {
            cached = 1;
        }
    }
    if (cached) {
        sal_memcpy(lo_thr, &avs->cent_lo_thr_cache[start_osc],
                   num_osc * sizeof(uint32));
        sal_memcpy(hi_thr, &avs->cent_hi_thr_cache[start_osc],
                   num_osc * sizeof(uint32));
    } else {
        /* read lo_thr, hi_thr reg */
        for (i = start_osc; i < start_osc + num_osc; i++) {
            n = i - start_osc;
            if (avs_info->avs_flags &
                SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
                if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
                    continue;
                }
            }
            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSCr(
                    unit, i, &lo_thr[n]));

            SOC_IF_ERROR_RETURN(
                READ_AVS_REG_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSCr(
                    unit, i, &hi_thr[n]));
        }
    }
    /* Extract threshold field from registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
        n = i - start_osc;
        if (avs_info->avs_flags &
            SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
                lo_thr[n] = 0;
                hi_thr[n] = 0;
                continue;
            }
        }
        lo_thr[n] = soc_reg_field_get(unit,
                AVS_REG_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSCr, lo_thr[n],
                THRESHOLDf);
        hi_thr[n] = soc_reg_field_get(unit,
                AVS_REG_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSCr, hi_thr[n],
                THRESHOLDf);
    }
    return SOC_E_NONE;
}

#define _W_OP_HI    1
#define _W_OP_LO    2
#define _W_OP_ALL   3
/*
 * Function:
 *  _soc_avs_rmt_osc_thr_set
 * Purpose:
 *  Fill hi/lo s_, h_ thresholds into specified range of remote osc.
 *  SAME value is copied into all registers.
 * Parameters:
 *  start_osc   - (IN)specifies first remote_osc
 *  num_osc  - (IN)specifies number of remote_osc
 *  write_op - (IN)specifies write fields of the register
 *  regval_h - (IN)value to be programmed in ROSC_H_THRESHOLD register
 *  regval_s - (IN)value to be programmed in ROSC_S_THRESHOLD register
 * Returns:
 *  SOC_E_xxx
 */

STATIC int
_soc_avs_rmt_osc_thr_set(int unit, int start_osc, int num_osc,
                         int write_op,
                         uint32 regval_h, uint32 regval_s)
{
    int i, cached = 0;
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;
    uint32 thr_h, thr_s;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    if (avs_info->avs_flags &
            SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE) {
        if ((avs->rmt_h_thr_cache == NULL) ||
            (avs->rmt_s_thr_cache == NULL)) {
            cached = 0;
        } else {
            cached = 1;
        }
    }
    /* write (same) regval_h, regval_s to all registers */
    for (i = start_osc; i < start_osc + num_osc; i++) {
        if (SOC_AVS_INFO(unit)->avs_flags &
            SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
                continue;
            }
        }
        SOC_IF_ERROR_RETURN(
            READ_AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr(unit, i,
                                                         &thr_h));
        sal_msleep(SOC_AVS_REMOTE_DELAY);

        SOC_IF_ERROR_RETURN(
            READ_AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr(unit, i,
                                                         &thr_s));
        sal_msleep(SOC_AVS_REMOTE_DELAY);

        if (write_op == _W_OP_HI) {
            soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr,
                              &thr_h, THRESH_HIf, regval_h);
            soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr,
                              &thr_s, THRESH_HIf, regval_s);
        } else if (write_op == _W_OP_LO) {
            soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr,
                              &thr_h, THRESH_LOf, regval_h);
            soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr,
                              &thr_s, THRESH_LOf, regval_s);
        } else {
            thr_h = regval_h;
            thr_s = regval_s;
        }
        SOC_IF_ERROR_RETURN(
            WRITE_AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr(unit, i, thr_h));
        sal_msleep(SOC_AVS_REMOTE_DELAY);
        SOC_IF_ERROR_RETURN(
            WRITE_AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr(unit, i, thr_s));
        sal_msleep(SOC_AVS_REMOTE_DELAY);
        if (cached) {
            avs->rmt_h_thr_cache[i] = thr_h;
            avs->rmt_s_thr_cache[i] = thr_s;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  _soc_avs_cent_osc_thr_set
 * Purpose:
 *  Program hi/lo threshold for specified range of central osc.
 *  Unique values can be specified for each osc.
 * Parameters:
 *  start_osc   - (IN)specifies first remote_osc
 *  num_osc  - (IN)specifies number of remote_osc
 *  sel_hi_thr - (IN)when true means program hi_threshold
 *  regval - (IN)array of values to be written into threshold registers
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_cent_osc_thr_set(int unit, int start_osc, int num_osc,
        soc_avs_bool_t sel_hi_thr, const uint32 *regval)
{
    int i, cached = 0, n;
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);

    if (regval == NULL) {
        return (SOC_E_PARAM);
    }
    if (avs_info->avs_flags &
            SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE) {
        if ((avs->cent_lo_thr_cache == NULL) ||
            (avs->cent_hi_thr_cache == NULL)) {
            cached = 0;
        } else {
            cached = 1;
        }
    }

    for (i = start_osc; i < start_osc + num_osc; i++) {
        n = i - start_osc;
        if (avs_info->avs_flags &
            SOC_AVS_INFO_F_DO_NOT_ACCESS_XOSC) {
            if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
                continue;
            }
        }
        if (sel_hi_thr == SOC_AVS_BOOL_TRUE) {
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSCr(unit, i,
                    regval[n]));
            if (cached) {
                avs->cent_hi_thr_cache[i] = regval[n];
            }
        } else {
            SOC_IF_ERROR_RETURN(
                WRITE_AVS_REG_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSCr(unit, i,
                    regval[n]));
            if (cached) {
                avs->cent_lo_thr_cache[i] = regval[n];
            }
        }
    }
    return SOC_E_NONE;
}

#if AVS_LEGACY_THRESHOLD
/*
 * Function:
 *  _soc_avs_find_new_thr
 * Purpose:
 * Reads osc_counts, determines lowest_osc_count and programs osc_thresholds.
 * Is a helper function for _soc_avs_set_new_thr.
 * Parameters:
 *  sel_hi_thr   - (IN)1 => hi_thr, 0 => lo_thr
 *  write_rmt_osc_thr  - (IN)1 => write rmt_osc_thr
 *  alloc_ptr - (OUT)points to area in mem to store osc_counts temporarily
 *  rmt_osc_regval_h - (OUT)rmt_osc_regval_h
 *  rmt_osc_regval_s - (OUT)rmt_osc_regval_s
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_find_new_thr(int unit, soc_avs_bool_t sel_hi_thr,
                      int write_rmt_osc_thr, uint32 *alloc_ptr,
                      uint32 *rmt_osc_regval_h, uint32 *rmt_osc_regval_s)
{
    soc_avs_info_t  *avs_info;
    uint32 *rmt_osc_count_s = NULL, *rmt_osc_count_h = NULL,
           *cent_osc_count = NULL, *cent_osc_thr = NULL;
    uint32 lowest_count_s = SOC_AVS_ALL_ONES;
    uint32 lowest_count_h = SOC_AVS_ALL_ONES;
    int i;

    avs_info = SOC_AVS_INFO(unit);

    if ((alloc_ptr == NULL) || (rmt_osc_regval_h == NULL) ||
            (rmt_osc_regval_s == NULL)) {
        return (SOC_E_PARAM);
    }

    /* Read osc_count for remote oscillators and find lowest */
    rmt_osc_count_s = alloc_ptr;
    rmt_osc_count_h = rmt_osc_count_s + avs_info->num_remotes;
    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_rmt_osc_count_get(unit,
                                   avs_info->first_rmt, avs_info->num_remotes,
                                   rmt_osc_count_h, rmt_osc_count_s),
                                   alloc_ptr);
    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
            continue;
        }
        if (rmt_osc_count_s[i] != 0) {
            lowest_count_s = SOC_AVS_MIN(lowest_count_s, rmt_osc_count_s[i]);
        }
        if (rmt_osc_count_h[i] != 0) {
            lowest_count_h = SOC_AVS_MIN(lowest_count_h, rmt_osc_count_h[i]);
        }
    }
    /* Read osc_count for central oscillators and find lowest */
    cent_osc_count = alloc_ptr;
    SOC_IF_ERROR_RETURN(
        _soc_avs_cent_osc_count_get(unit,
                                    avs_info->first_cent,
                                    avs_info->num_centrals, cent_osc_count));
    for (i = 0; i < avs_info->num_centrals; i++) {
        /* In 28nm , only oscillators 1&3 are SVT and HVT types */
        /* So we can include only them in finding lowest oscillator count */
        if (i != 1 && i != 3) {
            continue; /*skip others*/
        }
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
            continue;
        }
        if (cent_osc_count[i] == 0) {
            continue;
        }
        if (i == 1) {
            lowest_count_s = SOC_AVS_MIN(lowest_count_s, cent_osc_count[i]);
        }
        if (i == 3) {
            lowest_count_h = SOC_AVS_MIN(lowest_count_h, cent_osc_count[i]);
        }
    }
    /* update thresholds for rmt_osc */
    if (sel_hi_thr == SOC_AVS_BOOL_TRUE) {
        soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr,
                          rmt_osc_regval_h, THRESH_HIf, lowest_count_h);
        soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr,
                          rmt_osc_regval_s, THRESH_HIf, lowest_count_s);
    } else {
        soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr,
                          rmt_osc_regval_h, THRESH_LOf, lowest_count_h);
        soc_reg_field_set(unit, AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr,
                          rmt_osc_regval_s, THRESH_LOf, lowest_count_s);
    }

    cent_osc_thr = cent_osc_count;
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        /* Central #1 is SVT and #3 is HVT */
        if (i == 1) {
            cent_osc_thr[i] = lowest_count_s;
        } else if (i == 3) {
            cent_osc_thr[i] = lowest_count_h;
        } else {
            cent_osc_thr[i] = cent_osc_count[i];
        }
        if (sel_hi_thr == SOC_AVS_BOOL_TRUE) {
            soc_reg_field_set(unit,
                              AVS_REG_ROSC_THRESHOLD_2_THRESHOLD2_CEN_ROSCr,
                              cent_osc_count + i, THRESHOLDf, cent_osc_thr[i]);
        } else {
            soc_reg_field_set(unit,
                              AVS_REG_ROSC_THRESHOLD_1_THRESHOLD1_CEN_ROSCr,
                              cent_osc_count + i, THRESHOLDf, cent_osc_thr[i]);
        }
    }
    /* write thresholds for cent_osc */
    SOC_IF_ERROR_RETURN(
        _soc_avs_cent_osc_thr_set(unit, avs_info->first_cent,
                                  avs_info->num_centrals, sel_hi_thr,
                                  cent_osc_count));

    /* write thresholds for rmt_osc */
    if (write_rmt_osc_thr) {
        SOC_IF_ERROR_RETURN(
            _soc_avs_rmt_osc_thr_set(unit,
                                     avs_info->first_rmt, avs_info->num_remotes,
                                     _W_OP_ALL,
                                     *rmt_osc_regval_h, *rmt_osc_regval_s));
    }

    return SOC_E_NONE;
}
#endif /* AVS_LEGACY_THRESHOLD */

#define SOC_AVS_VINC_FOR_SET_NEW_THR_STEP (16)

STATIC int
_soc_avs_set_new_thresholds(int unit)
{
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;
    int32 vmarginl, vmarginh, slope_margin, intercept_margin;
    uint32 reg, freq;
    uint32 lower1, upper1, lower3, upper3;
    int i;

    avs = SOC_AVS_CONTROL(unit);
    avs_info = SOC_AVS_INFO(unit);
    slope_margin = avs->slope_margin;
    intercept_margin = avs->intercept_margin;
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "slope_margin %d intercept margin %d\n"),
                 slope_margin, intercept_margin));
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "i, saved_voltage, vmarginl, vmarginh,"
                            " cen_freq_thr, freq_l, reg_l, freq_h, reg_h ")));

    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
            continue;
        }
        vmarginl = (slope_margin * avs->saved_voltage[i]) / SOC_AVS_S2 +
                   intercept_margin;
        /* high is 10mV higher than low */
        vmarginh = vmarginl + avs->new_thr_inc;

        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "%d, %d, %d, %d, %d "),
                     i, avs->saved_voltage[i], vmarginl, vmarginh,
                     avs->cen_freq_thr[i]));

        freq = avs->cen_freq_thr[i] +
               (avs->cen_fth_slope[i] * vmarginl) / SOC_AVS_S2;
        reg = _soc_avs_freq_to_reg(unit, freq);
        SOC_IF_ERROR_RETURN(
            _soc_avs_cent_osc_thr_set(unit, i, 1, SOC_AVS_BOOL_FALSE, &reg));

        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "%d, %d, "), freq, reg));

        freq = avs->cen_freq_thr[i] +
               (avs->cen_fth_slope[i] * vmarginh) / SOC_AVS_S2;
        reg = _soc_avs_freq_to_reg(unit, freq);
        SOC_IF_ERROR_RETURN(
            _soc_avs_cent_osc_thr_set(unit, i, 1, SOC_AVS_BOOL_TRUE, &reg));

        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "%d, %d \n"), freq, reg));
    }

    /* All the remotes are same types as centrals 1 & 3 */
    SOC_IF_ERROR_RETURN(
        _soc_avs_cent_osc_thr_get(unit, 1, 1, &lower1, &upper1));
    SOC_IF_ERROR_RETURN(
        _soc_avs_cent_osc_thr_get(unit, 3, 1, &lower3, &upper3));

    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit, "lower1 %d upper1 %d lower3 %d upper3 %d\n"),
                 lower1, upper1, lower3, upper3));

    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
            continue;
        }
        SOC_IF_ERROR_RETURN(
            _soc_avs_rmt_osc_thr_set(unit, i, 1, _W_OP_LO, lower3, lower1));
        SOC_IF_ERROR_RETURN(
            _soc_avs_rmt_osc_thr_set(unit, i, 1, _W_OP_HI, upper3, upper1));
    }
    avs->flags |= SOC_AVS_F_THRESHOLD_SET;
    return SOC_E_NONE;
}

#if AVS_LEGACY_THRESHOLD
/*
 * Function:
 *  _soc_avs_set_new_thr
 * Purpose:
 *  Finds lowest_count for cur_dac_code and sets lo_thr for all osc to
 *  this value. Changes voltage to cur_dac_code+OFFSET, finds new lowest_count
 *  and sets hi_thr for all osc to this value.
 * Parameters:
 *  vavs_sv   - (IN)Voltage at which lo_thr is determined.
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_set_new_thr(int unit, uint32 vavs_sv)
{
    uint32 current_voltage; /* current_dac; */
    uint32 read_voltage;
    soc_avs_bool_t sel_hi_thr, write_rmt_osc_thr;
    uint32 rmt_osc_regval_h, rmt_osc_regval_s;
    int alloc_size;
    uint32 *alloc_ptr = NULL;
    soc_avs_info_t *avs_info = SOC_AVS_INFO(unit);
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);

    if ((SOC_AVS_INFO(unit)->avs_flags & SOC_AVS_INFO_F_SET_THRESHOLDS) == 0) {
        return (SOC_E_NONE);
    }

    /* allocate memory */
    alloc_size = 2 * SOC_AVS_MAX(avs_info->num_centrals, avs_info->num_remotes) *
                 sizeof(uint32);
    if ((alloc_ptr = (uint32 *)sal_alloc(alloc_size,
                    "Storage for _soc_avs_set_new_thr")) == NULL) {
        return (SOC_E_MEMORY);
    }
    /* Set the lo_thr with the smallest count found at current (low) voltage */
    /* Find_and_write lo_thr for cent, find_only lo_thr for rmt */
    rmt_osc_regval_h = 0, rmt_osc_regval_s = 0;

    /* get threshold low based on vavs + rw_delta */
    current_voltage = vavs_sv + avs_info->rw_delta;
    SOC_IF_ERROR_RETURN(soc_avs_voltage_set(unit, (uint32)current_voltage));

    if (LOG_CHECK(BSL_LS_SOC_AVS|BSL_VERBOSE)) {
        SOC_AVS_FREE_IF_ERROR_RETURN(
            soc_avs_voltage_get(unit, &read_voltage), alloc_ptr);
    }

    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_find_new_thr(unit, sel_hi_thr = SOC_AVS_BOOL_FALSE,
                              write_rmt_osc_thr = FALSE, alloc_ptr,
                              &rmt_osc_regval_h, &rmt_osc_regval_s),
                              alloc_ptr);

    /* Now, raise the DAC (raise the voltage) to figure out the hi_thr value */
    /* get threshold high based on vavs + rw_delta + new_thr_inc */
    current_voltage += avs->new_thr_inc;

    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "set voltage %d to set hi threshold\n"),
              current_voltage));

    SOC_AVS_FREE_IF_ERROR_RETURN(
        soc_avs_voltage_set(unit, current_voltage), alloc_ptr);
    if (LOG_CHECK(BSL_LS_SOC_AVS|BSL_VERBOSE)) {
        SOC_AVS_FREE_IF_ERROR_RETURN(
            soc_avs_voltage_get(unit, &read_voltage), alloc_ptr);
    }
    /* Set the hi_thr with the smallest count found at higher voltage */
    /* Find_and_write hi_thr for cent,
     * find hi_thr for rmt, write hi_lo_thr for rmt */
    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_find_new_thr(unit, sel_hi_thr = SOC_AVS_BOOL_TRUE,
                              write_rmt_osc_thr = TRUE, alloc_ptr,
                              &rmt_osc_regval_h, &rmt_osc_regval_s),
                              alloc_ptr);

    sal_free(alloc_ptr);

    avs->flags |= SOC_AVS_F_THRESHOLD_SET;
    return (SOC_E_NONE);
}
#endif

/*
 * Function:
 *  _soc_avs_start_main
 * Purpose:
 *  Runs initialize_oscs, find_final_voltage, set_new_thr.
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_start_main(int unit)
{
    uint32 vpred = 0, vavs_sv = 0;

    SOC_IF_ERROR_RETURN(_soc_avs_initialize_oscs(unit));

    SOC_IF_ERROR_RETURN(_soc_avs_find_final_voltage(unit, &vpred, &vavs_sv));
    if (vpred == 0) {
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit, "get the vpred=0\n")));
        return (SOC_E_DISABLED);
    }

#if AVS_LEGACY_THRESHOLD
    SOC_IF_ERROR_RETURN(_soc_avs_set_new_thr(unit, vavs_sv));
#endif
    SOC_IF_ERROR_RETURN(_soc_avs_set_new_thresholds(unit));

    return (SOC_E_NONE);
}


/* This is the main entry point for AVS processing */
/* This is one-time routine that must be called during init.*/
int
soc_avs_start(int unit)
{
    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    SOC_IF_ERROR_RETURN(_soc_avs_start_main(unit));
    return (SOC_E_NONE);
}

/* System VRM access function registeration */
int
soc_avs_vrm_access_func_register(int unit, soc_avs_vrm_access_t *vrm_access)
{
    soc_avs_control_t *avs;

    soc_vrm_access_functions[unit] = vrm_access;
    if (SOC_AVS_CONTROL(unit) != NULL) {
        avs = SOC_AVS_CONTROL(unit);
        avs->vrm_access = vrm_access;
    }
    return SOC_E_NONE;
}

/*
 * Default vrm info when vrm access functions are not specified.
 * The default vrm should be pmbus compatiable device which driver is
 * implemented in soc i2c device descriptor scheme.
 */
static int
soc_avs_vrm_info_init(int unit, soc_avs_vrm_info_t *vrm_info)
{
    int dev_id = 0;
    int fd, rv;
    i2c_driver_t *i2c_drv;

    if (vrm_info == NULL) {
        return SOC_E_PARAM;
    }

    if (!(vrm_info->profile.vrm_name)) {
        return SOC_E_PARAM;
    }
    vrm_info->mux_id = -1;
    vrm_info->vrm_id = -1;

    /* Access the device to check if it exists or not. */
    if (vrm_info->profile.mux_name[0] != '\0') {
        dev_id = soc_i2c_devopen(unit, vrm_info->profile.mux_name, 0, 0);
        if (dev_id < 0) {
            LOG_ERROR(BSL_LS_SOC_AVS,
                      (BSL_META_U(unit, "Fail to open mux in VRM profile.\n")));
            return SOC_E_FAIL;
        }
        vrm_info->mux_id = dev_id;
    }
    /* Enable mux when both mux device and its enable vlaue are specified */
    if ((vrm_info->mux_id != -1) && (vrm_info->profile.mux_enable_value)) {
        fd = vrm_info->mux_id;
        i2c_drv = soc_i2c_device(unit, fd)->driver;
        rv = i2c_drv->write(unit, vrm_info->mux_id, 0,
                            &vrm_info->profile.mux_enable_value, 1);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_AVS,
                      (BSL_META_U(unit,
                                  "Fail to enable mux in VRM profile.\n")));
            return rv;
        }
    }

    if (vrm_info->profile.vrm_name[0] != '\0') {
        dev_id = soc_i2c_devopen(unit, vrm_info->profile.vrm_name, 0, 0);
        if (dev_id < 0) {
            LOG_ERROR(BSL_LS_SOC_AVS,
                      (BSL_META_U(unit, "Fail to open vrm in VRM profile.\n")));
            return SOC_E_FAIL;
        }
        vrm_info->vrm_id = dev_id;
    }

    return SOC_E_NONE;
}

/* Specify the vrm as the default vrm */
int
soc_avs_vrm_profile_add(int unit, soc_avs_vrm_profile_t *vrm_profile)
{
    soc_avs_vrm_info_t *vrm_info;

    if (SOC_AVS_CONTROL(unit) == NULL) {
        return SOC_E_INIT;
    }
    /* Only allow one vrm profile, new one will replace the old one */
    if (SOC_AVS_VRM_INFO(unit) == NULL) {
        vrm_info = sal_alloc(sizeof(soc_avs_vrm_info_t), "avs vrm");
        if (vrm_info == NULL) {
            return SOC_E_MEMORY;
        }
        SOC_AVS_VRM_INFO(unit) = vrm_info;
    } else {
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit, "VRM profile has been replaced \n")));
    }

    sal_memcpy(&SOC_AVS_VRM_INFO(unit)->profile, vrm_profile,
               sizeof(soc_avs_vrm_profile_t));

    return SOC_E_NONE;
}

/*
 * VRM initialization function
 * If default vrm is not available or compatable with i2c pmbus driver,
 * user has to register its vrm access functions.
 */
int
soc_avs_vrm_init(int unit)
{
    /* Check the custom registered access function */
    if (SOC_AVS_VRM_ACCESS(unit) != NULL) {
        if (SOC_AVS_VRM_ACCESS(unit)->vrm_init != NULL) {
            return SOC_AVS_VRM_ACCESS(unit)->vrm_init(unit);
        }
    }
    /* Try to use the pmbus vrm */
    if (SOC_AVS_VRM_INFO(unit) != NULL) {
        return soc_avs_vrm_info_init(unit, SOC_AVS_VRM_INFO(unit));
    }
    return SOC_E_UNAVAIL;
}

int
soc_avs_vrm_deinit(int unit)
{
    int rv;

    /* Check the custom registered access function */
    if (SOC_AVS_VRM_ACCESS(unit) != NULL) {
        if (SOC_AVS_VRM_ACCESS(unit)->vrm_deinit != NULL) {
            rv = SOC_AVS_VRM_ACCESS(unit)->vrm_deinit(unit);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        }
    }
    /* Try to use the pmbus vrm */
    if (SOC_AVS_VRM_INFO(unit) != NULL) {
        sal_free(SOC_AVS_VRM_INFO(unit));
        SOC_AVS_VRM_INFO(unit) = NULL;
    }
    return SOC_E_NONE;
}

/* Get the device openloop target voltage */
int
soc_avs_openloop_voltage_get(int unit, uint32 *voltage)
{
    if ((SOC_AVS_FUNCTIONS(unit) != NULL) &&
        (SOC_AVS_FUNCTIONS(unit)->openloop_voltage_get != NULL)) {
        return SOC_AVS_FUNCTIONS(unit)->openloop_voltage_get(unit, voltage);
    }
    return SOC_E_UNAVAIL;
}

int
soc_avs_openloop_main(int unit)
{
    int rv;
    uint32 voltage;

    if (SOC_AVS_CONTROL(unit) == NULL) {
        return SOC_E_UNAVAIL;
    }
    if ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_VRM_INITED) == 0) {
        rv = soc_avs_vrm_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_AVS,
                      (BSL_META_U(unit, "Fail to init VRM !\n")));
            return rv;
        }
        SOC_AVS_CONTROL(unit)->flags |= SOC_AVS_F_VRM_INITED;
    }
    rv = soc_avs_openloop_voltage_get(unit, &voltage);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* If the VRM is not initialized, do not try to set the voltage */
    if ((SOC_AVS_CONTROL(unit)->flags & SOC_AVS_F_VRM_INITED) == 0) {
        return SOC_E_NONE;
    }

    rv = soc_avs_voltage_set(unit, voltage);
    if (SOC_FAILURE(rv)) {
        return rv;
    }
    /* need to sleep a while to stabelize the voltage */
    sal_usleep(SOC_AVS_OPENLOOP_DELAY);
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_avs_function_register
 * Purpose:
 *  avs_functions register function
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_function_register(int unit, soc_avs_functions_t *avs_functions)
{
    soc_avs_functions[unit] = avs_functions;

    /* Overwrite existed avs functions */
    if (SOC_AVS_CONTROL(unit) != NULL) {
        SOC_AVS_CONTROL(unit)->avs_functions = soc_avs_functions[unit];
    }
    return SOC_E_NONE;
}

int
soc_avs_unregister(int unit)
{
    soc_avs_functions[unit] = NULL;
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_avs_init
 * Purpose:
 *  software data structures allocation and initialization for avs module.
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_init(int unit)
{
    soc_avs_control_t *avs;
    soc_avs_info_t  *avs_info;
    int rv, i, track_step_size, new_thr_inc;
    int vmin_avs;
    int alloc_size;

    if (!soc_feature(unit, soc_feature_avs) &&
        !soc_feature(unit, soc_feature_avs_openloop)) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_AVS_CONTROL(unit) == NULL) {
        SOC_AVS_CONTROL(unit) =
            sal_alloc(sizeof(soc_avs_control_t), "avs_control");
        if (SOC_AVS_CONTROL(unit) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_AVS_CONTROL(unit), 0, sizeof(soc_avs_control_t));
    }

    avs = SOC_AVS_CONTROL(unit);

    avs->avs_functions = soc_avs_functions[unit];
    avs->vrm_access = soc_vrm_access_functions[unit];
    if (soc_feature(unit, soc_feature_avs_openloop)) {
        return SOC_E_NONE;
    }

    /* avs_debug_xbmp, avs_debug_margin */
    avs->avs_debug_xbmp = sal_alloc(sizeof(soc_avs_debug_xbmp_t), "avs_debug_xbmp");
    if (avs->avs_debug_xbmp == NULL) {
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->avs_debug_xbmp, 0, sizeof(soc_avs_debug_xbmp_t));

    avs->avs_debug_margin =
        sal_alloc(sizeof(soc_avs_debug_margin_t), "avs_debug_margin");
    if (avs->avs_debug_margin == NULL) {
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->avs_debug_margin, 0, sizeof(soc_avs_debug_margin_t));


    /* cen_freq_thr, rmt_freq_thr, cen_fth_slope */
    avs->cen_freq_thr = _cent_freq_thr;
    avs->rmt_freq_thr = _rmt_freq_thr;
    avs->cen_fth_slope = _cent_fth_slope;

    avs->avs_info = sal_alloc(sizeof(soc_avs_info_t), "avs_info");
    if (avs->avs_info == NULL) {
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->avs_info, 0, sizeof(soc_avs_info_t));

    avs_info = avs->avs_info;
    /* general avs_info initialization */
        /* default - disabled */
    avs_info->num_centrals = 0;
    avs_info->first_cent = 0;
    for (i = 0; i < SOC_AVS_NUM_CENT_XBMP; i++) {
        avs_info->cent_xbmp[i] = 0xFFFFFFFF;
    }
    avs_info->num_remotes = 0;
    avs_info->first_rmt = 0;
    for (i = 0; i < SOC_AVS_NUM_RMT_XBMP; i++) {
        avs_info->rmt_xbmp[i] = 0xFFFFFFFF;
    }
    avs_info->vmin_avs = SOC_AVS_INT(SOC_AVS_DEFAULT_VMIN_AVS);
    avs_info->vmax_avs = SOC_AVS_INT(SOC_AVS_DEFAULT_VMAX_AVS);
    avs_info->vmargin_low  = SOC_AVS_INT(SOC_AVS_DEFAULT_VMARGIN_LOW);
    avs_info->vmargin_high = SOC_AVS_INT(SOC_AVS_DEFAULT_VMARGIN_HIGH);
#ifdef BCM_SBUSDMA_SUPPORT
    avs_info->cen_osc_reg = INVALIDr;
    avs_info->rmt_osc_reg = INVALIDr;
#endif
    if (SOC_AVS_FUNCTIONS(unit)->info_init == NULL) {
        LOG_ERROR(BSL_LS_SOC_AVS,
                  (BSL_META_U(unit, "no init functions registerd\n")));
        sal_free(avs_info);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs);
        return SOC_E_INTERNAL;
    }
    /* chip specific avs_info initialization */
    rv = (SOC_AVS_FUNCTIONS(unit)->info_init)(unit, avs_info);
    if (SOC_FAILURE(rv)) {
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return rv;
    }
    /* avs_info checker */
    if ((avs_info->num_centrals == 0) ||
        (avs_info->num_centrals > 128) ||
        (avs_info->num_remotes == 0) ||
        (avs_info->num_remotes > 256)) {
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return (SOC_E_PARAM);
    }
    /* overwrite the avs info from config */
    vmin_avs = soc_property_get(unit, "avs_vmin", avs_info->vmin_avs);
    avs_info->vmin_avs = vmin_avs;

    /* Create multex */
    if ((avs->avsMutex = sal_mutex_create("AVS CONTROL")) == NULL) {
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return (SOC_E_INTERNAL);
    }
    /* init avs track thread */
    avs->avs_track_pid = SAL_THREAD_ERROR;
    avs->avs_track_interval = 0;

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc_feature(unit, soc_feature_sbusdma)) {
        rv = soc_avs_sbusdma_desc_setup(unit);
        if (rv) {
            (void)soc_avs_sbusdma_desc_free(unit);
            avs_info->avs_flags &= ~SOC_AVS_INFO_F_RSOC_COUNT_DMA;
        }
    }
#endif
    alloc_size = avs_info->num_centrals * sizeof(uint32);
    avs->cent_v1s_osc_cache = sal_alloc(alloc_size, "cent v1s ROSC cache");
    if (avs->cent_v1s_osc_cache == NULL) {
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->cent_v1s_osc_cache, 0, alloc_size);
    alloc_size = avs_info->num_remotes * sizeof(uint32);
    avs->rmt_v1s_h_osc_cache = sal_alloc(alloc_size, "remote v1s h ROSC cache");
    if (avs->rmt_v1s_h_osc_cache == NULL) {
        sal_free(avs->cent_v1s_osc_cache);
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->rmt_v1s_h_osc_cache, 0, alloc_size);
    avs->rmt_v1s_s_osc_cache = sal_alloc(alloc_size, "remote v1s s ROSC cache");
    if (avs->rmt_v1s_s_osc_cache == NULL) {
        sal_free(avs->cent_v1s_osc_cache);
        sal_free(avs->rmt_v1s_h_osc_cache);
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->rmt_v1s_s_osc_cache, 0, alloc_size);

    if (avs_info->avs_flags & SOC_AVS_INFO_F_RSOC_THRESHOLD_CACHE) {
        alloc_size = avs_info->num_centrals * sizeof(uint32);
        avs->cent_lo_thr_cache = sal_alloc(alloc_size, "cent ROSC lo thr cache");
        if (avs->cent_lo_thr_cache == NULL) {
            sal_free(avs->cent_v1s_osc_cache);
            sal_free(avs->rmt_v1s_h_osc_cache);
            sal_free(avs->rmt_v1s_s_osc_cache);
            sal_free(avs->avs_debug_margin);
            sal_free(avs->avs_debug_xbmp);
            sal_free(avs_info);
            sal_free(avs);
            return SOC_E_MEMORY;
        }
        avs->cent_hi_thr_cache = sal_alloc(alloc_size, "cent ROSC hi thr cache");
        if (avs->cent_hi_thr_cache == NULL) {
            sal_free(avs->cent_v1s_osc_cache);
            sal_free(avs->rmt_v1s_h_osc_cache);
            sal_free(avs->rmt_v1s_s_osc_cache);
            sal_free(avs->cent_lo_thr_cache);
            sal_free(avs->avs_debug_margin);
            sal_free(avs->avs_debug_xbmp);
            sal_free(avs_info);
            sal_free(avs);
            return SOC_E_MEMORY;
        }

        sal_memset(avs->cent_lo_thr_cache, 0, alloc_size);
        sal_memset(avs->cent_hi_thr_cache, 0, alloc_size);

        alloc_size = avs_info->num_remotes * sizeof(uint32);
        avs->rmt_h_thr_cache = sal_alloc(alloc_size, "rmt ROSC h thr cache");
        if (avs->rmt_h_thr_cache == NULL) {
            sal_free(avs->cent_v1s_osc_cache);
            sal_free(avs->rmt_v1s_h_osc_cache);
            sal_free(avs->rmt_v1s_s_osc_cache);
            sal_free(avs->cent_hi_thr_cache);
            sal_free(avs->cent_lo_thr_cache);
            sal_free(avs->avs_debug_margin);
            sal_free(avs->avs_debug_xbmp);
            sal_free(avs_info);
            sal_free(avs);
            return SOC_E_MEMORY;
        }
        avs->rmt_s_thr_cache = sal_alloc(alloc_size, "rmt ROSC s thr cache");
        if (avs->rmt_s_thr_cache == NULL) {
            sal_free(avs->cent_v1s_osc_cache);
            sal_free(avs->rmt_v1s_h_osc_cache);
            sal_free(avs->rmt_v1s_s_osc_cache);
            sal_free(avs->rmt_h_thr_cache);
            sal_free(avs->cent_hi_thr_cache);
            sal_free(avs->cent_lo_thr_cache);
            sal_free(avs->avs_debug_margin);
            sal_free(avs->avs_debug_xbmp);
            sal_free(avs_info);
            sal_free(avs);
            return SOC_E_MEMORY;
        }
        sal_memset(avs->rmt_h_thr_cache, 0, alloc_size);
        sal_memset(avs->rmt_s_thr_cache, 0, alloc_size);
    }

    /* allocate saved_voltage */
    alloc_size = avs_info->num_centrals * sizeof(uint32);
    avs->saved_voltage = sal_alloc(alloc_size, "cent ROSC saved voltage");
    if (avs->saved_voltage == NULL) {
        sal_free(avs->cent_v1s_osc_cache);
        sal_free(avs->rmt_v1s_h_osc_cache);
        sal_free(avs->rmt_v1s_s_osc_cache);
        sal_free(avs->rmt_s_thr_cache);
        sal_free(avs->rmt_h_thr_cache);
        sal_free(avs->cent_hi_thr_cache);
        sal_free(avs->cent_lo_thr_cache);
        sal_free(avs->avs_debug_margin);
        sal_free(avs->avs_debug_xbmp);
        sal_free(avs_info);
        sal_free(avs);
        return SOC_E_MEMORY;
    }
    sal_memset(avs->saved_voltage, 0, alloc_size);

    avs->flags |= SOC_AVS_F_INITED;

    /* track_inc_unit */
    rv = soc_avs_ioctl(unit, SOC_AVS_CTRL_QUERY_TRACK_STEP_SIZE, &track_step_size, 1);

    if (SOC_FAILURE(rv)) {
        /* default 3 mV*/
        avs->track_step_size = 30;
    } else {
        avs->track_step_size = track_step_size;
    }
    /* overwrite the avs from config */
    avs->track_step_size = soc_property_get(unit, "avs_track_step", avs->track_step_size);

    /* new_thr_inc_unit */
    rv = soc_avs_ioctl(unit, SOC_AVS_CTRL_QUERY_NEW_THR_INC, &new_thr_inc, 1);

    if (SOC_FAILURE(rv)) {
        /* default 10 mV */
        avs->new_thr_inc = 100;
    } else {
        avs->new_thr_inc = new_thr_inc;
    }
    /* overwrite the avs info from config */
    avs->new_thr_inc = soc_property_get(unit, "avs_thr_inc", avs->new_thr_inc);

    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_avs_deinit
 * Purpose:
 *  software data structures, resource de-initialization for avs module.
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_deinit(int unit)
{
    soc_avs_control_t *avs;

    avs = SOC_AVS_CONTROL(unit);
    if (avs) {
        if (SOC_AVS_FUNCTIONS(unit)->info_deinit) {
            SOC_AVS_FUNCTIONS(unit)->info_deinit(unit);
        }
        SOC_AVS_FUNCTIONS(unit) = NULL;
        (void)soc_avs_vrm_deinit(unit);
        if (avs->avsMutex) {
            sal_mutex_destroy(avs->avsMutex);
            avs->avsMutex = NULL;
        }
        if (avs->avs_debug_margin) {
            sal_free(avs->avs_debug_margin);
        }
        if (avs->avs_debug_xbmp) {
            sal_free(avs->avs_debug_xbmp);
        }
        if (avs->avs_info) {
            sal_free(avs->avs_info);
        }
#ifdef BCM_SBUSDMA_SUPPORT
        if (soc_feature(unit, soc_feature_sbusdma)) {
            (void)soc_avs_sbusdma_desc_free(unit);
        }
#endif
        if(avs->rmt_s_thr_cache) {
            sal_free(avs->rmt_s_thr_cache);
        }
        if(avs->rmt_h_thr_cache) {
            sal_free(avs->rmt_h_thr_cache);
        }
        if(avs->cent_hi_thr_cache) {
            sal_free(avs->cent_hi_thr_cache);
        }
        if(avs->cent_lo_thr_cache) {
            sal_free(avs->cent_lo_thr_cache);
        }
        if(avs->cent_v1s_osc_cache) {
            sal_free(avs->cent_v1s_osc_cache);
        }
        if(avs->rmt_v1s_h_osc_cache) {
            sal_free(avs->rmt_v1s_h_osc_cache);
        }
        if(avs->rmt_v1s_s_osc_cache) {
            sal_free(avs->rmt_v1s_s_osc_cache);
        }
        if(avs->saved_voltage) {
            sal_free(avs->saved_voltage);
        }
        sal_free(avs);
    }
    SOC_AVS_CONTROL(unit) = NULL;

    return 0;
}

/*
 * Function:
 *  soc_avs_debug_margin_set
 * Purpose:
 *  Mainly for CLI cmd usage to change the margin during the verification
 *  If the custom margin value is set, algorithm will take this value instead
 *  of chip initialized margin value.
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_debug_margin_set(int unit, soc_avs_debug_margin_t *avs_debug_margin)
{
    soc_avs_control_t *avs;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs = SOC_AVS_CONTROL(unit);

    SOC_AVS_LOCK(avs);
    SOC_AVS_MARGIN(unit)->vmargin_low = avs_debug_margin->vmargin_low;
    SOC_AVS_MARGIN(unit)->vmargin_high = avs_debug_margin->vmargin_high;
    SOC_AVS_UNLOCK(avs);
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_avs_debug_margin_get
 * Purpose:
 *  Mainly for CLI cmd usage to get the customed margin during the verification
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_debug_margin_get(int unit, soc_avs_debug_margin_t *avs_debug_margin)
{
    soc_avs_control_t *avs;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs = SOC_AVS_CONTROL(unit);

    SOC_AVS_LOCK(avs);
    avs_debug_margin->vmargin_low = SOC_AVS_MARGIN(unit)->vmargin_low;
    avs_debug_margin->vmargin_high = SOC_AVS_MARGIN(unit)->vmargin_high;
    SOC_AVS_UNLOCK(avs);
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_avs_xbmp_set
 * Purpose:
 *  Mainly for CLI cmd usage to config the osc exclusive bitmap
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_xbmp_set(int unit, int type, int start_osc, int num_osc, int value)
{
    soc_avs_control_t *avs;
    int i;
    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs = SOC_AVS_CONTROL(unit);

    SOC_AVS_LOCK(avs);
    if (type == SOC_AVS_ROSC_TYPE_CENTRAL) {
        for (i = start_osc; i < start_osc + num_osc; i++) {
            if ((i >= 0) && (i / NUM_BITS_PER_XBMP < SOC_AVS_NUM_CENT_XBMP)) {
                if (value == 0) {
                    SOC_AVS_XBMP(unit)->cent_xbmp[i / NUM_BITS_PER_XBMP] &= ~(0x1 <<
                        (i % NUM_BITS_PER_XBMP));
                } else {
                    SOC_AVS_XBMP(unit)->cent_xbmp[i / NUM_BITS_PER_XBMP] |= 0x1 <<
                    (i % NUM_BITS_PER_XBMP);
                }
            } else {
                SOC_AVS_UNLOCK(avs);
                return (SOC_E_FAIL);
            }
        }
    }
    if (type == SOC_AVS_ROSC_TYPE_REMOTE) {
        for (i = start_osc; i < start_osc + num_osc; i++) {
            if ((i >= 0) && (i / NUM_BITS_PER_XBMP < SOC_AVS_NUM_RMT_XBMP)) {
                if (value == 0) {
                    SOC_AVS_XBMP(unit)->rmt_xbmp[i / NUM_BITS_PER_XBMP] &= ~(0x1 <<
                            (i % NUM_BITS_PER_XBMP));
                } else {
                    SOC_AVS_XBMP(unit)->rmt_xbmp[i / NUM_BITS_PER_XBMP] |=
                        0x1 << (i % NUM_BITS_PER_XBMP);
                }
            } else {
                SOC_AVS_UNLOCK(avs);
                return (SOC_E_FAIL);
            }
        }
    }
    SOC_AVS_UNLOCK(avs);
    return SOC_E_NONE;
}
/*
 * Function:
 *  soc_avs_xbmp_get
 * Purpose:
 *  Mainly for CLI cmd usage to get the currently osc exclusive bitmap
 *  which is the "OR" result of the customized value and chip initialized value
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
int
soc_avs_xbmp_dump(int unit, int type)
{
    int i;
    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    if (type == SOC_AVS_ROSC_TYPE_REMOTE) {
        for (i = 0; i < SOC_AVS_NUM_RMT_XBMP; i++) {
            LOG_CLI((BSL_META_U(unit,
                    "soc_avs_xbmp_dump: rmt_xbmp[%d] = 0x%08x\n"),
                        i, (SOC_AVS_XBMP(unit)->rmt_xbmp[i] |
                        SOC_AVS_INFO(unit)->rmt_xbmp[i])));
        }
    }
    if (type == SOC_AVS_ROSC_TYPE_CENTRAL) {
        for (i = 0; i < SOC_AVS_NUM_CENT_XBMP; i++) {
            LOG_CLI((BSL_META_U(unit,
                    "soc_avs_xbmp_dump: cent_xbmp[%d] = 0x%08x\n"),
                    i, (SOC_AVS_XBMP(unit)->cent_xbmp[i] |
                    SOC_AVS_INFO(unit)->cent_xbmp[i])));
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *  _soc_avs_reset_sequencer
 * Purpose:
 *  Reset the sequencer and all of its calculations.
 * Parameters:
 *
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_reset_sequencer(int unit)
{
    uint32 regval, fieldval;
    soc_reg_t reg;
    soc_field_t field;
    int i;
    soc_avs_reg_info_t *osc_seq_reset_list;
    soc_avs_info_t *avs_info;

    if(soc_avs_track_inited(unit)) {
        return SOC_E_NONE;
    }
    avs_info = SOC_AVS_INFO(unit);

    SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_SEQUENCER_INIT, NULL, 0));


    /* AVS_REG_HW_MNTR_SEQUENCER_INITr.SEQUENCER_INITf */
    regval = 0;
    reg = AVS_REG_HW_MNTR_SEQUENCER_INITr;
    field = SEQUENCER_INITf;
    soc_reg_field_set(unit, reg, &regval, field, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));
    regval = 0;
    soc_reg_field_set(unit, reg, &regval, field, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_PVTMON_INIT, NULL, 0));

    /* AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr.M_INIT_PVT_MNTRf */
    reg = AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr;
    field = M_INIT_PVT_MNTRf;
    fieldval = _soc_avs_reg_field_all_ones(unit, reg, field);
    regval = 0;
    soc_reg_field_set(unit, reg, &regval, field, fieldval);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

   SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_OSC_MNTR_INIT, NULL, 0));

    /* reset cent and rmt OSCs measurement */
    if (avs_info->osc_seq_reset_info) {
        uint32 delay;
        int rosc_count_mode;
        int ref_clk_freq; /* in MHz*/
        uint32 max_ref_clk_counter;

        osc_seq_reset_list = avs_info->osc_seq_reset_info;
        for (i = 0; osc_seq_reset_list[i].reg_name != -1 ; i++) {
            reg = osc_seq_reset_list[i].reg_name;
            field = osc_seq_reset_list[i].reg_field;
            regval = 0;
            fieldval = _soc_avs_reg_field_all_ones(unit, reg, field);
            soc_reg_field_set(unit, reg, &regval, field, fieldval);
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));
            regval = 0;
            fieldval = 0;
            soc_reg_field_set(unit, reg, &regval, field, fieldval);
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));
        }

        /* add delay to let all cent ROSC counts be valid*/
        rosc_count_mode = avs_info->rosc_count_mode;
        ref_clk_freq = avs_info->ref_clk_freq;
        max_ref_clk_counter = (avs_info->measurement_time_control * 256U)
                               + 255U;
        /* delay in us */
        delay = (max_ref_clk_counter * rosc_count_mode * avs_info->num_centrals)
                 / ref_clk_freq;
        /* delay in ms */
        delay /= 1000;
        /* delay with safe margin */
        delay = delay * 2;
        sal_msleep(delay);
    }

   SOC_AVS_IF_ERROR_NOT_UNAVAIL_RETURN(
        soc_avs_ioctl(unit, SOC_AVS_CTRL_HW_MNTR_SW_CONTROL, NULL, 0));

    /* Make sure we're not in take-over mode so sequencer will begin its
     * measurements */
    regval = 0;
    reg = AVS_REG_HW_MNTR_SW_CONTROLSr;
    soc_reg_field_set(unit, reg, &regval, SW_TAKEOVERf, 0);
    soc_reg_field_set(unit, reg, &regval, SW_DO_MEASUREf, 0);
    soc_reg_field_set(unit, reg, &regval, SW_SENSOR_IDXf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, regval));

    sal_msleep(SOC_AVS_SEQUENCER_DELAY);

    SOC_AVS_CONTROL(unit)->flags |= SOC_AVS_F_TRACK_INITED;
    return (SOC_E_NONE);
}


/*
 * Function:
 *  _soc_avs_adjust_voltage
 * Purpose:
 *  If current voltage is within [Vmin_avs,Vmax_avs], last_set_voltage is
 *  adjusted with adj_step. If current voltage is outside the range,
 *  last_set_voltage is adjusted by adj_step in direction to bring back voltage
 *  within the range (by ignoring sign of adj_step).
 * Parameters:
 *  adjustment_step - (IN)Positive, negative value to be added to
 *                      last_set_voltage.
 *  voltage_change_aborted - (OUT)TRUE if we could change voltage as requested.
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_adjust_voltage(int unit, int adjustment_step, soc_avs_bool_t
        *voltage_change_aborted)
{
    uint32 last_set_voltage, new_set_voltage;
    uint32 read_voltage;
    int32 new_vmin_avs;
    soc_avs_info_t *avs_info = SOC_AVS_INFO(unit);

    if (voltage_change_aborted == NULL) {
        return (SOC_E_PARAM);
    }
    *voltage_change_aborted = SOC_AVS_BOOL_FALSE;
    new_vmin_avs = avs_info->vmin_avs;

    last_set_voltage = _soc_last_set_voltage[unit];
    SOC_AVS_ASSERT(last_set_voltage > 0);

    SOC_IF_ERROR_RETURN(soc_avs_voltage_get(unit, &read_voltage));

    /* Make sure we never violate the voltage max and min values */
    /* Note that the adjustment step can be negative to adjust the other way */
    if ((new_vmin_avs <= (int32)read_voltage) &&
        ((int32)read_voltage <= avs_info->vmax_avs)) {
        *voltage_change_aborted = SOC_AVS_BOOL_FALSE;
        if (adjustment_step < 0) {
            SOC_AVS_ASSERT(last_set_voltage > SOC_AVS_ABS(adjustment_step));
        }
        new_set_voltage = last_set_voltage + (uint32)adjustment_step;

        LOG_INFO(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "adjust step %d to voltage %d\n"),
                  adjustment_step, new_set_voltage));

        SOC_IF_ERROR_RETURN(soc_avs_voltage_set(unit, new_set_voltage));

    } else {
        *voltage_change_aborted = SOC_AVS_BOOL_TRUE;

        /* new algorithm says we shouldn't ALLOW the voltage to exceed its
         * limits.
         * So we know we're outside the safe voltage range -- bring it back */

        if ((int32)read_voltage > new_vmin_avs) {
            SOC_AVS_ASSERT(last_set_voltage > SOC_AVS_ABS(adjustment_step));
            new_set_voltage = last_set_voltage - SOC_AVS_ABS(adjustment_step);
            LOG_WARN(BSL_LS_SOC_AVS,
                     (BSL_META_U(unit,
                                 "WARNING, read_voltage = %0d, is out of"
                                 " limits, voltage will be decreased\n"),
                      read_voltage));

        } else {
            new_set_voltage = last_set_voltage + SOC_AVS_ABS(adjustment_step);
            LOG_WARN(BSL_LS_SOC_AVS,
                     (BSL_META_U(unit,
                                 "WARNING, read_voltage = %0d, is out of"
                                 " limits, voltage will be increased\n"),
                      read_voltage));
        }
        SOC_IF_ERROR_RETURN(soc_avs_voltage_set(unit, new_set_voltage));
    }
    return (SOC_E_NONE);
}
#define SOC_AVS_TRACK_VINC_STEP  (2)
    /* 2 step * 3 mV per step  */
#define SOC_AVS_TRACK_VDEC_STEP  (-1)
    /* -1 step * 3 mV per step */

/*
 * Function:
 *  _soc_avs_converge_process
 * Purpose:
 *  If any osc_count < lo_thr then increase the voltage.
 *  Else if all osc_count are > hi_thr, then decrease the voltage.
 * Parameters:
 *  voltage_change_requested - (OUT)0,+1,-1 to indicated no change, inc, dec
 *  voltage_change_aborted - (OUT)TRUE if we could change voltage as requested.
 * Returns:
 *  SOC_E_xxx
 */
STATIC int
_soc_avs_converge_process(int unit, int *voltage_change_requested,
        soc_avs_bool_t *voltage_change_aborted)
{
    int i, alloc_size;
    uint32 *posc_count = NULL, *posc_lo_thr = NULL, *posc_hi_thr = NULL;
    uint32 *posc_count_h = NULL, *posc_count_s = NULL, *posc_lo_thr_h = NULL,
           *posc_hi_thr_h = NULL, *posc_lo_thr_s = NULL, *posc_hi_thr_s = NULL;
    soc_avs_bool_t inc_voltage = SOC_AVS_BOOL_FALSE,
                   found_one_osc_below_hi_thr = SOC_AVS_BOOL_FALSE;
    uint32 *alloc_ptr = NULL;
    int inc_val, dec_val;
    char *s = NULL, *s1 = NULL;

    soc_avs_info_t *avs_info = SOC_AVS_INFO(unit);
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);

    if ((avs->flags & SOC_AVS_F_THRESHOLD_SET) == 0) {
        LOG_WARN(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit,
                             "Threshold values are not set. "
                             "Stop the _soc_avs_converge_process.\n")));
        return (SOC_E_INTERNAL);
    }

    if ((voltage_change_requested == NULL) ||
        (voltage_change_aborted == NULL)) {
        return (SOC_E_PARAM);
    }
    /* init outputs */
    *voltage_change_aborted = SOC_AVS_BOOL_FALSE;
    *voltage_change_requested = 0; /* no change */

    /* allocate memory */
    alloc_size = 6 * SOC_AVS_MAX(avs_info->num_centrals, avs_info->num_remotes)
                * sizeof(uint32);
    if ((alloc_ptr = (uint32 *)sal_alloc(alloc_size,
                    "Storage for _soc_avs_converge_process")) == NULL) {
        return (SOC_E_MEMORY);
    }

    /* read all cent_osc counts, thresholds */
    sal_memset(alloc_ptr, 0, alloc_size);
    posc_count = alloc_ptr;
    posc_hi_thr = posc_count + avs_info->num_centrals;
    posc_lo_thr = posc_hi_thr + avs_info->num_centrals;

    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_cent_osc_count_get(unit, avs_info->first_cent,
                                    avs_info->num_centrals, posc_count),
                                    alloc_ptr);

    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_cent_osc_thr_get(unit, avs_info->first_cent,
                                  avs_info->num_centrals, posc_lo_thr,
                                  posc_hi_thr), alloc_ptr);
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "index, osc_count, [lo_thr, hi_thr], +/-/ \n")));

    /* Process cent_osc counts, thresholds */
    for (i = avs_info->first_cent; i < avs_info->num_centrals; i++) {
        /* skip the items we are excluding */
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_CENTRAL, i)){
            continue;
        }
        if (posc_count[i] == 0) {
            continue;
        }
        /* this should have already been handled by exclude list, but ... */
        if (LOG_CHECK(BSL_LS_SOC_AVS|BSL_VERBOSE)) {
            if (posc_count[i] < posc_lo_thr[i]) {
                s = "-";
            } else if (posc_count[i] > posc_hi_thr[i]) {
                s = "+";
            } else {
                s = " ";
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "%d, %d, [%d, %d], %s\n"),
                     i, posc_count[i], posc_lo_thr[i], posc_hi_thr[i], s));

        if ((posc_lo_thr[i] != 0) &&
            (posc_count[i] < posc_lo_thr[i])) {
            inc_voltage = SOC_AVS_BOOL_TRUE;
        } else if ((posc_hi_thr[i] != 0) &&
                   (posc_count[i] <= posc_hi_thr[i])) {
            found_one_osc_below_hi_thr = SOC_AVS_BOOL_TRUE;
        }
    }

    /* read all rmt_osc counts, thresholds */
    sal_memset(alloc_ptr, 0, alloc_size);
    posc_count_h = alloc_ptr;
    posc_count_s = posc_count_h + avs_info->num_remotes;
    posc_hi_thr_h = posc_count_s + avs_info->num_remotes;
    posc_lo_thr_h = posc_hi_thr_h + avs_info->num_remotes;
    posc_hi_thr_s = posc_lo_thr_h + avs_info->num_remotes;
    posc_lo_thr_s = posc_hi_thr_s + avs_info->num_remotes;

    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_rmt_osc_count_get(unit, avs_info->first_rmt,
                                   avs_info->num_remotes,
                                   posc_count_h, posc_count_s), alloc_ptr);

    SOC_AVS_FREE_IF_ERROR_RETURN(
        _soc_avs_rmt_osc_thr_get(unit, avs_info->first_rmt,
                                 avs_info->num_remotes, posc_lo_thr_h,
                                 posc_hi_thr_h, posc_lo_thr_s, posc_hi_thr_s),
                                 alloc_ptr);
    LOG_VERBOSE(BSL_LS_SOC_AVS,
                (BSL_META_U(unit,
                            "index, osc_count_h , [lo_thr_h, hi_thr_h], "
                            "osc_count_s, [lo_thr_s, hi_thr_s], +/-/ ,+/-/ \n")));

    /* Process remote_osc counts, thresholds */
    for (i = avs_info->first_rmt; i < avs_info->num_remotes; i++) {
        /* skip the items we are excluding */
        if (_soc_avs_xbmp_check(unit, SOC_AVS_ROSC_TYPE_REMOTE, i)){
            continue;
        }

        /*if we didn't get a value then this is an unimplemented oscillator*/
        /* this should have already been handled by exclude list */
        if ((posc_count_h[i] == 0) || (posc_count_s[i] == 0)) {
            continue;
        }
        if (LOG_CHECK(BSL_LS_SOC_AVS|BSL_VERBOSE)) {
            if (posc_count_h[i] < posc_lo_thr_h[i]) {
                s = "-";
            } else if (posc_count_h[i] > posc_hi_thr_h[i]) {
                s = "+";
            } else {
                s = " ";
            }
            if (posc_count_s[i] < posc_lo_thr_s[i]) {
                s1 = "-";
            } else if (posc_count_s[i] > posc_hi_thr_s[i]) {
                s1 = "+";
            } else {
                s1 = " ";
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "%d, %d, [%d, %d], %d, [%d, %d], %s, %s\n"),
                     i, posc_count_h[i], posc_lo_thr_h[i], posc_hi_thr_h[i],
                     posc_count_s[i], posc_lo_thr_s[i], posc_hi_thr_s[i],
                     s, s1));

        if (((posc_lo_thr_s[i] != 0) &&
             (posc_count_s[i] < posc_lo_thr_s[i])) ||
            ((posc_lo_thr_h[i] != 0) &&
             (posc_count_h[i] < posc_lo_thr_h[i]))) {
            inc_voltage = SOC_AVS_BOOL_TRUE;

        } else if (((posc_hi_thr_s[i] != 0) &&
                    (posc_count_s[i] <= posc_hi_thr_s[i])) ||
                   ((posc_hi_thr_h[i] != 0) &&
                    (posc_count_h[i] <= posc_hi_thr_h[i]))) {
            found_one_osc_below_hi_thr = SOC_AVS_BOOL_TRUE;
        }
    }
    if (inc_voltage == SOC_AVS_BOOL_TRUE) {
        /* we raise the voltage by raising the DAC */
        *voltage_change_requested = +1; /* inc */
        inc_val = SOC_AVS_TRACK_VINC_STEP * avs->track_step_size;
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_adjust_voltage(unit, inc_val, voltage_change_aborted),
                                    alloc_ptr);
    } else if (found_one_osc_below_hi_thr == SOC_AVS_BOOL_FALSE) {
        /* we lower the voltage by lowering the DAC */
        *voltage_change_requested = -1; /* dec */
        dec_val = SOC_AVS_TRACK_VDEC_STEP * avs->track_step_size;
        SOC_AVS_FREE_IF_ERROR_RETURN(
            _soc_avs_adjust_voltage(unit, dec_val, voltage_change_aborted),
                                    alloc_ptr);
    }

    sal_free(alloc_ptr);
    return (SOC_E_NONE);
}
/*
 * Function:
 *  soc_avs_track
 * Purpose:
 *  Reset sequencer and invoke _soc_avs_converge_process function.
 *  This function will be called periodically when system is running.
 */
int
soc_avs_track(int unit)
{
    int voltage_change_requested;
    soc_avs_bool_t voltage_change_aborted;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }

    SOC_IF_ERROR_RETURN(_soc_avs_reset_sequencer(unit));

    SOC_IF_ERROR_RETURN(
        _soc_avs_converge_process(unit,
                                  &voltage_change_requested,
                                  &voltage_change_aborted));

    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "voltage_change_requested=%0d, "
                         "voltage_change_aborted=%0d, last_set_voltage=%0d\n"),
              voltage_change_requested, voltage_change_aborted,
              _soc_last_set_voltage[unit]));

    return SOC_E_NONE;
}
void soc_avs_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);
    int rv = SOC_E_NONE;
    int interval;

    while ((interval = avs->avs_track_interval) != 0) {
#ifdef SOC_AVS_DEBUG
        sal_usecs_t     start_time;
#endif
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit, "sleep %d\n"), interval));

        (void)sal_sem_take(avs->avs_track_notify, interval);

        if (avs->avs_track_interval == 0) {       /* Exit signaled */
            break;
        }
#ifdef SOC_AVS_DEBUG
        start_time = sal_time_usecs();
#endif
        rv = soc_avs_track(unit);
#ifdef SOC_AVS_DEBUG
        LOG_VERBOSE(BSL_LS_SOC_AVS,
                    (BSL_META_U(unit,
                                "Time taken for avs track: %d usec\n"),
                     SAL_USECS_SUB(sal_time_usecs(), start_time)));
#endif
        if (SOC_FAILURE(rv)) {
            LOG_INFO(BSL_LS_SOC_AVS,
                     (BSL_META_U(unit, "Fail to do avs_track\n")));
        }

        if (!(avs->avs_track_interval)) {
            break;
        }
    }
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "exiting thread\n")));

    avs->avs_track_pid = SAL_THREAD_ERROR;
    avs->avs_track_interval = 0;
    sal_thread_exit(0);
}

int
soc_avs_track_stop(int unit)
{
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);
    soc_timeout_t       to;
    sal_usecs_t     timeout;
    int rv = SOC_E_NONE;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit, "stop avs track")));
    timeout = (1   * SECOND_USEC);
    /* Stop thread if present */
    if (avs->avs_track_interval != 0) {
        avs->avs_track_interval = 0;
        sal_sem_give(avs->avs_track_notify);
        soc_timeout_init(&to, timeout, 0);
         while ((avs->avs_track_pid) != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_AVS,
                          (BSL_META_U(unit, "thread did not exit\n")));
                avs->avs_track_pid = SAL_THREAD_ERROR;
                rv = SOC_E_TIMEOUT;
                break;
            }

            sal_usleep(10000);
        }
    }
    if (NULL != avs->avs_track_notify) {
        sal_sem_destroy(avs->avs_track_notify);
        avs->avs_track_notify = NULL;
    }
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit, "thread stopped\n")));

    return (rv);
}
int
soc_avs_track_start(int unit, int interval)
{
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);
    sal_sem_t           sem;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    LOG_INFO(BSL_LS_SOC_AVS,
             (BSL_META_U(unit,
                         "avs_track start with interval=%d\n"), interval));

    /* Stop if already running */
    if (avs->avs_track_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_avs_track_stop(unit));
    }

    if (interval == 0) {
        return SOC_E_NONE;
    }
    if ((sem = avs->avs_track_notify) != NULL) {
        avs->avs_track_notify = NULL;    /* Stop others from waking sem */
        sal_sem_destroy(sem);           /* Then destroy it */
    }

    avs->avs_track_notify = sal_sem_create("avs track notify",
                                           sal_sem_BINARY, 0);

    sal_snprintf(avs->avs_track_name,
                 sizeof(avs->avs_track_name),
                 "bcmAVS.%d", unit);
    /* Start the thread */
    if (interval != 0) {
        avs->avs_track_interval = interval;

        avs->avs_track_pid =
            sal_thread_create(avs->avs_track_name,
                              SAL_THREAD_STKSZ,
                                            50,
                              soc_avs_thread, INT_TO_PTR(unit));

        if (avs->avs_track_pid == SAL_THREAD_ERROR) {
            avs->avs_track_interval = 0;
            LOG_ERROR(BSL_LS_SOC_AVS,
                      (BSL_META_U(unit, "thread create failed\n")));
            return (SOC_E_INTERNAL);
        }

        LOG_INFO(BSL_LS_SOC_AVS,
                 (BSL_META_U(unit, "Created AVS thread\n")));
    }
    return SOC_E_NONE;
}

int
soc_avs_info_dump(int unit)
{
    soc_avs_control_t *avs = SOC_AVS_CONTROL(unit);
    soc_avs_info_t *avs_info;
    int i, xbmp_index;

    if (!soc_avs_inited(unit)) {
        return SOC_E_INIT;
    }
    avs_info = SOC_AVS_INFO(unit);
    LOG_CLI((BSL_META_U(unit,
                        "Unit %d AVS Info Structure:\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "\tnum_centrals = %d num_remotes = %d\n"),
             avs_info->num_centrals, avs_info->num_remotes));
    xbmp_index = (avs_info->num_centrals + NUM_BITS_PER_XBMP - 1)
                 / NUM_BITS_PER_XBMP;
    for (i = 0; i < xbmp_index; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "\tcent_xbmp[%d] = 0x%08x\n"),
                 i, avs_info->cent_xbmp[i]));
    }
    xbmp_index = (avs_info->num_remotes + NUM_BITS_PER_XBMP - 1)
                 / NUM_BITS_PER_XBMP;
    for (i = 0; i < xbmp_index; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "\trmt_xbmp[%d] = 0x%08x\n"),
                 i, avs_info->rmt_xbmp[i]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\tvmin_avs = %d  vmax_avs = %d (in 0.1mV)\n"
                        "\tvmargin_low = %d vmargin_high = %d (in 0.1mV)\n"
                        "\tavs_flags = 0x%x \n"),
             avs_info->vmin_avs, avs_info->vmax_avs,
             avs_info->vmargin_low, avs_info->vmargin_high,
             avs_info->avs_flags));
    LOG_CLI((BSL_META_U(unit,
                        "\tdebug_margin_low = %d debug_margin_high = %d (in 0.1mV)\n"
                        "\tavs_thr_inc = %d avs_track_step = %d (in 0.1mV)\n"),
             avs->avs_debug_margin->vmargin_low, avs->avs_debug_margin->vmargin_high,
             avs->new_thr_inc, avs->track_step_size));
    return SOC_E_NONE;
}

#else /* INCLUDE_AVS */

typedef int _common_avs_c_not_empty; /* Make ISO compilers happy. */

#endif /* INCLUDE_AVS */
