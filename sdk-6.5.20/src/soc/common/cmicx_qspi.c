/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     CMICX QSPI driver
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/register.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/iproc.h>
#include <soc/cmicx_qspi.h>

#ifdef INCLUDE_CPU_I2C
 #include <soc/i2c.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>

#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
#include <sal/core/time.h>
#endif

/* Chip attributes */
#define SPBR_MIN                            8U
#define SPBR_MAX                            255U
#define NUM_TXRAM                           32
#define NUM_RXRAM                           32
#define NUM_CDRAM                           16

/*
 * Register fields
 */
#define MSPI_SPCR0_MSB_BITS_8               (0x00000020)
#define BSPI_RAF_CONTROL_START_MASK         (0x00000001)
#define BSPI_RAF_STATUS_SESSION_BUSY_MASK   (0x00000001)
#define BSPI_RAF_STATUS_FIFO_EMPTY_MASK     (0x00000002)
#define BSPI_BITS_PER_PHASE_ADDR_MARK       (0x00010000)
#define BSPI_BITS_PER_CYCLE_DATA_SHIFT      0
#define BSPI_BITS_PER_CYCLE_ADDR_SHIFT      16

/* SPI transfer flags */
#define SPI_RW_BEGIN 0x01
#define SPI_RW_END   0x02


/* Configurations */

#define QSPI_WAIT_TIMEOUT                   200U    /* msec */
#ifndef IPROC_BSPI_READ_DUMMY_CYCLES
#define IPROC_BSPI_READ_DUMMY_CYCLES 0x8
#endif
#ifndef IPROC_BSPI_READ_CMD
#define IPROC_BSPI_READ_CMD 0xb
#endif

#ifndef IPROC_BSPI_DATA_LANES
#define IPROC_BSPI_DATA_LANES 4
#endif

#ifndef IPROC_BSPI_ADDR_LANES
#define IPROC_BSPI_ADDR_LANES 4
#endif

/* 429 MHz */
#define IPROC_SYS_CLOCK        429
/* 15 Mhz */
#define IPROC_SCL_BAUD_RATE    15

#ifdef INCLUDE_CPU_I2C
#define IPROC_QSPI_READ(unit, reg, read_value_p) \
    iproc_qspi_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), read_value_p)
#define IPROC_QSPI_WRITE(unit, reg, value) \
    iproc_qspi_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), value)
#else
#define IPROC_QSPI_READ(unit, reg, rvp) \
        soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rvp)
#define IPROC_QSPI_WRITE(unit, reg, rv) \
        soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), rv)
#endif /* INCLUDE_CPU_I2C */

#define LMAX(a, b) (((a) > (b)) ? (a) : (b))
#define LMIN(a, b) (((a) < (b)) ? (a) : (b))


/* MSPI registers */
typedef struct qspi_mspi_hw_s {
    uint32 txram;
    uint32 rxram;
    uint32 cdram;
} qspi_mspi_hw_t;

/* State */
typedef enum cmicx_qspi_state_s {
    QSPI_STATE_DISABLED,
    QSPI_STATE_MSPI,
    QSPI_STATE_BSPI
} cmicx_qspi_state_t;

/* QSPI configuration data */
typedef struct cmicx_qspi_conf_s {
    /* Specified SPI parameters */
    uint32 max_hz;
    uint32 spi_mode;
    /* State */
    cmicx_qspi_state_t state;
    uint8 bspi_op;
    uint32 bspi_addr;
    int mspi_16bit;
    int mode_4byte;
    uint32 bspi_hw;
    qspi_mspi_hw_t mspi_hw[NUM_TXRAM];
} cmicx_qspi_conf_t;

typedef struct cmicx_qspi_s {
    cmicx_qspi_conf_t *conf;
    sal_mutex_t lock;
    uint8 access_method; /* method to access the device for QSPI flash memory access */
    uint8 i2c_bus;       /* I2C bus number for QSPI_ACCESS_METHOD_I2C */
    uint8 i2c_dev;       /* I2C slave device number for QSPI_ACCESS_METHOD_I2C */
} cmicx_qspi_t;


STATIC qspi_mspi_hw_t mspi_hw[NUM_TXRAM] = {
    {QSPI_MSPI_TXRAM00r, QSPI_MSPI_RXRAM00r, QSPI_MSPI_CDRAM00r},
    {QSPI_MSPI_TXRAM01r, QSPI_MSPI_RXRAM01r, QSPI_MSPI_CDRAM01r}, 
    {QSPI_MSPI_TXRAM02r, QSPI_MSPI_RXRAM02r, QSPI_MSPI_CDRAM02r},
    {QSPI_MSPI_TXRAM03r, QSPI_MSPI_RXRAM03r, QSPI_MSPI_CDRAM03r},
    {QSPI_MSPI_TXRAM04r, QSPI_MSPI_RXRAM04r, QSPI_MSPI_CDRAM04r},
    {QSPI_MSPI_TXRAM05r, QSPI_MSPI_RXRAM05r, QSPI_MSPI_CDRAM05r}, 
    {QSPI_MSPI_TXRAM06r, QSPI_MSPI_RXRAM06r, QSPI_MSPI_CDRAM06r},
    {QSPI_MSPI_TXRAM07r, QSPI_MSPI_RXRAM07r, QSPI_MSPI_CDRAM07r},
    {QSPI_MSPI_TXRAM08r, QSPI_MSPI_RXRAM08r, QSPI_MSPI_CDRAM08r},
    {QSPI_MSPI_TXRAM09r, QSPI_MSPI_RXRAM09r, QSPI_MSPI_CDRAM09r}, 
    {QSPI_MSPI_TXRAM10r, QSPI_MSPI_RXRAM10r, QSPI_MSPI_CDRAM10r},
    {QSPI_MSPI_TXRAM11r, QSPI_MSPI_RXRAM11r, QSPI_MSPI_CDRAM11r},
    {QSPI_MSPI_TXRAM12r, QSPI_MSPI_RXRAM12r, QSPI_MSPI_CDRAM12r},
    {QSPI_MSPI_TXRAM13r, QSPI_MSPI_RXRAM13r, QSPI_MSPI_CDRAM13r}, 
    {QSPI_MSPI_TXRAM14r, QSPI_MSPI_RXRAM14r, QSPI_MSPI_CDRAM14r},
    {QSPI_MSPI_TXRAM15r, QSPI_MSPI_RXRAM15r, QSPI_MSPI_CDRAM15r},
    {QSPI_MSPI_TXRAM16r, QSPI_MSPI_RXRAM16r, 0},
    {QSPI_MSPI_TXRAM17r, QSPI_MSPI_RXRAM17r, 0},
    {QSPI_MSPI_TXRAM18r, QSPI_MSPI_RXRAM18r, 0},
    {QSPI_MSPI_TXRAM19r, QSPI_MSPI_RXRAM19r, 0},
    {QSPI_MSPI_TXRAM20r, QSPI_MSPI_RXRAM20r, 0},
    {QSPI_MSPI_TXRAM21r, QSPI_MSPI_RXRAM21r, 0},
    {QSPI_MSPI_TXRAM22r, QSPI_MSPI_RXRAM22r, 0},
    {QSPI_MSPI_TXRAM23r, QSPI_MSPI_RXRAM23r, 0},
    {QSPI_MSPI_TXRAM24r, QSPI_MSPI_RXRAM24r, 0},
    {QSPI_MSPI_TXRAM25r, QSPI_MSPI_RXRAM25r, 0},
    {QSPI_MSPI_TXRAM26r, QSPI_MSPI_RXRAM26r, 0},
    {QSPI_MSPI_TXRAM27r, QSPI_MSPI_RXRAM27r, 0},
    {QSPI_MSPI_TXRAM28r, QSPI_MSPI_RXRAM28r, 0},
    {QSPI_MSPI_TXRAM29r, QSPI_MSPI_RXRAM29r, 0},
    {QSPI_MSPI_TXRAM30r, QSPI_MSPI_RXRAM30r, 0},
    {QSPI_MSPI_TXRAM31r, QSPI_MSPI_RXRAM31r, 0},
};

STATIC cmicx_qspi_t cmicx_qspi[SOC_MAX_NUM_DEVICES] = {{0}};

/* Set the current QSPI access method */
void
iproc_qspi_set_access_method(int unit, uint8 access_method, uint8 i2c_bus, uint8 i2c_dev)
{
    cmicx_qspi_t  *qspi = cmicx_qspi + unit;
    qspi->access_method = access_method;
    qspi->i2c_bus = i2c_bus;
    qspi->i2c_dev = i2c_dev;
}

#ifdef INCLUDE_CPU_I2C
/* Read a uint32 from the AXI address space using the current QSPI access method */
int
iproc_qspi_read(int unit, uint32 addr, uint32 *read_value)
{
    cmicx_qspi_t  *qspi = cmicx_qspi + unit;

    if (qspi->access_method == QSPI_ACCESS_METHOD_PCIE) {
        return soc_iproc_getreg(unit, addr, read_value);
    } else if (qspi->access_method == QSPI_ACCESS_METHOD_I2C) {
        return cpu_i2c_device_read(qspi->i2c_bus, qspi->i2c_dev, addr, read_value);
    } else if (qspi->access_method == QSPI_ACCESS_METHOD_I2C_BDE) {
        return soc_cm_i2c_device_read(unit, addr, read_value);
    }
    return SOC_E_INTERNAL;
}

/* Write a uint32 from the AXI address space using the current QSPI access method */
int
iproc_qspi_write(int unit, uint32 addr, uint32 value)
{
    cmicx_qspi_t  *qspi = cmicx_qspi + unit;

    if (qspi->access_method == QSPI_ACCESS_METHOD_PCIE) {
        return soc_iproc_setreg(unit, addr, value);
    } else if (qspi->access_method == QSPI_ACCESS_METHOD_I2C) {
        return cpu_i2c_device_write(qspi->i2c_bus, qspi->i2c_dev, addr, value);
    } else if (qspi->access_method == QSPI_ACCESS_METHOD_I2C_BDE) {
        return soc_cm_i2c_device_write(unit, addr, value);
    }
    return SOC_E_INTERNAL;
}
#endif /* INCLUDE_CPU_I2C */

int
cmicx_qspi_init(int unit,
              uint32 max_hz,
              uint32 mode)
{
    cmicx_qspi_conf_t *conf = NULL;
    cmicx_qspi_t  *qspi = &cmicx_qspi[unit];
    uint32 spbr;
    uint32 val;
    int rv = SOC_E_NONE;

    /* Check if already initialized */

    if (qspi->conf != NULL) {
        rv = SOC_E_INIT;
        goto error;
    }

    conf = sal_alloc(sizeof(*conf), "SOC_SPI");
    if (conf == NULL) {
        rv = SOC_E_MEMORY;
        goto error;
    }

    if ((qspi->lock =
        sal_mutex_create("qspi_lock")) == NULL) {
        rv = SOC_E_MEMORY;
        goto error;
    }
    conf->max_hz = max_hz;
    conf->spi_mode = mode;
    conf->state = QSPI_STATE_DISABLED;
    conf->mode_4byte = 0;
    sal_memcpy(conf->mspi_hw, mspi_hw, sizeof(mspi_hw));


    /* BSPI: clock configuration */
    IPROC_QSPI_READ(unit, CRU_CONTROLr, &val);
    val &= ~0x00000006;
    if (conf->max_hz >= 62500000) {
        val |= 0x00000006;
    } else if (conf->max_hz >= 50000000) {
        val |= 0x00000002;
    } else if (conf->max_hz >= 31250000) {
        val |= 0x00000004;
    }
    IPROC_QSPI_WRITE(unit, CRU_CONTROLr, val);

    /* BSPI: configure for dual/quad mode */
    if (IPROC_BSPI_DATA_LANES != 1 || 
        IPROC_BSPI_ADDR_LANES != 1) {
        /* Disable flex mode first */
        val = 0;
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_FLEX_MODE_ENABLEr, val);
        /* Data / Address lanes */
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_BITS_PER_CYCLEr, val);
        if (IPROC_BSPI_DATA_LANES == 4) {
            val |= 2 << BSPI_BITS_PER_CYCLE_DATA_SHIFT;
        } else {
            val |= ((IPROC_BSPI_DATA_LANES - 1) 
                    << BSPI_BITS_PER_CYCLE_DATA_SHIFT);
        }
        if (IPROC_BSPI_ADDR_LANES == 4) {
            val |= 2 << BSPI_BITS_PER_CYCLE_ADDR_SHIFT;
        } else {
            val |= ((IPROC_BSPI_ADDR_LANES - 1) 
                    << BSPI_BITS_PER_CYCLE_ADDR_SHIFT);
        }
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_BITS_PER_CYCLEr, val);
        /* Dummy cycles */
        IPROC_QSPI_READ(unit, QSPI_BSPI_REGISTERS_BITS_PER_PHASEr, &val);
        val &= ~0xFF;
        val |= IPROC_BSPI_READ_DUMMY_CYCLES;
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_BITS_PER_PHASEr, val);
        /* Command byte for BSPI */
        val = IPROC_BSPI_READ_CMD;
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_CMD_AND_MODE_BYTEr, val);
        /* Enable flex mode to take effect */
        val = 1;
        IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_FLEX_MODE_ENABLEr, val);
    }
    /* MSPI: Basic hardware initialization */
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR1_LSBr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR1_MSBr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_NEWQPr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_ENDQPr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR2r, 0);
    /*TBD: MSPI: SCK configuration */
    spbr = (IPROC_SYS_CLOCK) / (2 * IPROC_SCL_BAUD_RATE);
    spbr = LMAX(LMIN(spbr, SPBR_MAX), SPBR_MIN);
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR0_LSBr, spbr);

    /* MSPI: Mode configuration (8 bits by default) */
    conf->mspi_16bit = 0;
    val = 0x80 |                      /* Master */
         (8 << 2) |                  /* 8 bits per word */
         (conf->spi_mode & 3);        /* mode: CPOL / CPHA */

    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR0_MSBr, val);

    qspi->conf = conf;

    return rv;

error:
    if (conf) {
        sal_free(conf);
    }
    return SOC_E_NONE;
}

int 
cmicx_qspi_cleanup(int unit)
{
    cmicx_qspi_t  *qspi = &cmicx_qspi[unit];

    if (qspi->conf == NULL) {
        return SOC_E_PARAM;
    }
    if (qspi->lock) { 
        sal_mutex_destroy(qspi->lock);
        qspi->lock = 0;
    }
    sal_free(qspi->conf);
    qspi->conf = NULL;
    return SOC_E_NONE;
}

static void 
bspi_flush_prefetch_buffers(int unit)
{
    IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_B0_CTRLr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_B1_CTRLr, 0);
    IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_B0_CTRLr, 1);
    IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_B1_CTRLr, 1);
}

static int 
disable_mspi(int unit, cmicx_qspi_conf_t *conf)
{
    if (conf->state == QSPI_STATE_BSPI) {
        return SOC_E_NONE;
    }
    /* Disable write lock */
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_WRITE_LOCKr, 0);
    /* Flush prefetch buffers */
    bspi_flush_prefetch_buffers(unit);
    /* Switch to BSPI */
    IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_MAST_N_BOOT_CTRLr, 0);
    /* Update state */
    conf->state = QSPI_STATE_BSPI;

    return SOC_E_NONE;
}

static int 
enable_mspi(int unit, cmicx_qspi_conf_t *conf)
{
    soc_timeout_t to;
    uint32 val;

    if (conf->state == QSPI_STATE_MSPI) {
        return SOC_E_NONE;
    }
    /* Switch to MSPI if not yet */
    IPROC_QSPI_READ(unit, QSPI_BSPI_REGISTERS_MAST_N_BOOT_CTRLr, &val);

    if ((val & 0x01) == 0) {
        soc_timeout_init(&to, QSPI_WAIT_TIMEOUT * 1000, 1);
        while(1) {
            IPROC_QSPI_READ(unit, QSPI_BSPI_REGISTERS_BUSY_STATUSr, &val);
            if ((val & 0x01) == 0) {
                IPROC_QSPI_WRITE(unit, QSPI_BSPI_REGISTERS_MAST_N_BOOT_CTRLr, 1);
                sal_usleep(1);
                break;
            }
            if (soc_timeout_check(&to)) {
                break;
            }
        }
        IPROC_QSPI_READ(unit, QSPI_BSPI_REGISTERS_MAST_N_BOOT_CTRLr, &val);
        if (val != 0x01) {
            return SOC_E_FAIL;
        }
    }
    /* Update state */
    conf->state = QSPI_STATE_MSPI;

    return SOC_E_NONE;
}

int
cmicx_qspi_claim_bus(int unit)
{
    cmicx_qspi_t *qspi = &cmicx_qspi[unit];
    cmicx_qspi_conf_t *conf = qspi->conf;
    int rv = SOC_E_NONE;

    if (conf == NULL) {
        return SOC_E_PARAM;
    }
    /* Switch to MSPI by default */
    rv = enable_mspi(unit, conf);

    return rv;
}

int 
cmicx_qspi_release_bus(int unit)
{
    cmicx_qspi_t *qspi = &cmicx_qspi[unit];
    cmicx_qspi_conf_t *conf = qspi->conf;

    if (conf == NULL) {
        return SOC_E_PARAM;
    }
    /* Make sure no operation is in progress */
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR2r, 0);
    sal_usleep(1);
    /* Switch to BSPI */
    disable_mspi(unit, conf);
    /* Update state */
    conf->state = QSPI_STATE_DISABLED;

    return SOC_E_NONE;
}

static int
mspi_rw(int unit, cmicx_qspi_conf_t *conf,
          uint32 bytes, const uint8 *tx,
          uint8 *rx, int end)
{
    uint32 val;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#define PCIEFW_MEASURE_START diff = sal_time_usecs();
#define PCIEFW_MEASURE_END(testname) \
    diff = sal_time_usecs() - diff; \
    cli_out("    %s: %uus\n", #testname, diff);
#else
#define PCIEFW_MEASURE_START
#define PCIEFW_MEASURE_END(testname)
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    PCIEFW_MEASURE_START;
    IPROC_QSPI_READ(unit, QSPI_MSPI_SPCR0_MSBr, &val);
    /* Use 8-bit queue for odd-bytes transfer */
    val |= MSPI_SPCR0_MSB_BITS_8; 
    IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR0_MSBr, val);
    PCIEFW_MEASURE_END(mspi_rw_1_QSPI_MSPI_SPCR0_MSB_modify);
    conf->mspi_16bit = 0;

    while(bytes) {
        uint32 chunk;
        uint32 queues;
        uint32 i;
        soc_timeout_t to;

        /* Determine how many bytes to process this time */
        chunk = LMIN(bytes, NUM_CDRAM);
        queues = chunk;
        bytes -= chunk;
        /* Fill CDRAMs and TXRAMS */
        PCIEFW_MEASURE_START;
        for(i=0; i<chunk; i++) {
            IPROC_QSPI_WRITE(unit, conf->mspi_hw[i].cdram, 0x82);
            IPROC_QSPI_WRITE(unit, conf->mspi_hw[i << 1].txram,
                              tx ? tx[i] : 0xff);
        }
        PCIEFW_MEASURE_END(mspi_rw_2_write_chunk_loop);
        /* Setup queue pointers */
        PCIEFW_MEASURE_START;
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_NEWQPr, 0);
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_ENDQPr, (queues - 1));

        /* Deassert CS if requested and it's the last transfer */
        if (bytes == 0 && end ) {
            IPROC_QSPI_WRITE(unit, conf->mspi_hw[queues - 1].cdram, 0x02);
        }
        /* Kick off */
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_MSPI_STATUSr, 0);
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_SPCR2r, 0xc0);    /* cont | spe */
        PCIEFW_MEASURE_END(mspi_rw_3_after_chunk);
        /* Wait for completion */
        soc_timeout_init(&to, QSPI_WAIT_TIMEOUT * 1000, 1);
        PCIEFW_MEASURE_START;
        while(1) {
            IPROC_QSPI_READ(unit, QSPI_MSPI_MSPI_STATUSr, &val);
            if (val & 0x01) {
                break;
            }
            if (soc_timeout_check(&to)) {
                break;
            }
        }
        PCIEFW_MEASURE_END(mspi_rw_4_wait_for_QSPI_MSPI_MSPI_STATUS);
        if ((val & 0x01) == 0) {
            return -SOC_E_FAIL;
        }
        /* Read data out */
        if (rx) {
            PCIEFW_MEASURE_START;
            for(i=0; i<chunk; i++) {
                IPROC_QSPI_READ(unit,
                     conf->mspi_hw[(i << 1) + 1].rxram, &val);
                rx[i] = val & 0xff;
            }
            PCIEFW_MEASURE_END(mspi_rw_5_wait_for_read_chunk_loop);
        }
        /* Advance pointers */
        if (tx)
            tx += chunk;
        if (rx) 
            rx += chunk;
    }

    return SOC_E_NONE;
}

int
cmicx_qspi_rw(int unit,
              uint32 bytes,
              const void *dout,
              void *din,
              uint32 flags)
{
    cmicx_qspi_t *qspi = &cmicx_qspi[unit];
    cmicx_qspi_conf_t *conf = qspi->conf;
    uint8 *rx = din;
    const uint8 *tx = dout;
    int rv  = SOC_E_NONE;
#ifdef PCIEFW_QSPI_TEST_PERFORMANCE
    sal_usecs_t diff;
#endif /* PCIEFW_QSPI_TEST_PERFORMANCE */

    if (conf == NULL) {
        return SOC_E_PARAM;
    }
    /* MSPI: Enable write lock at the beginning */
    if (flags & SPI_RW_BEGIN) {
        /* Switch to MSPI if not yet */
        rv = enable_mspi(unit, conf);
        if (SOC_FAILURE(rv)) {
            return -rv;
        }
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_WRITE_LOCKr, 1);
    }
    /* MSPI: Transfer it */
    if (bytes) {
        if(tx || rx) {
            PCIEFW_MEASURE_START;
            rv = mspi_rw(unit, conf, bytes, tx, rx, (flags & SPI_RW_END));
            PCIEFW_MEASURE_END(cmicx_qspi_rw_mspi_rw);
        } else {
            rv = SOC_E_PARAM;
        }
    }
    /* MSPI: Disable write lock if it's done */
    if (flags & SPI_RW_END) {
        IPROC_QSPI_WRITE(unit, QSPI_MSPI_WRITE_LOCKr, 0);
    }

    return rv;
}

#endif /* BCM_CMICX_SUPPORT */
