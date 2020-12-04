/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICd LED module
 */
#include <shared/bsl.h>
#include <sal/core/sync.h>

#include <soc/drv.h>
#include <soc/led.h>
#include <soc/cmic.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV4_SUPPORT)

/*******************************************************************************
 * Local definition.
 */
#define CMIC_LED_UC_MAX                5
#define CMIC_LED_PROGRAM_SIZE          256
#define CMIC_LED_DATA_SIZE             256
#define CMIC_MAX_PORTS_PER_LED_UC      64

/* CMICd locks. */
#define CMICD_LED_LOCK(unit, led_uc) \
        do { \
            sal_mutex_take(cmicd_led_ctrl[unit].lock[led_uc], sal_mutex_FOREVER); \
        } while(0)

#define CMICD_LED_UNLOCK(unit, led_uc) \
        do { \
            sal_mutex_give(cmicd_led_ctrl[unit].lock[led_uc]); \
        } while(0)

#define CMICD_LED_CHAIN_LOCK(unit) \
        do { \
            sal_mutex_take(cmicd_led_ctrl[unit].chain_lock, sal_mutex_FOREVER); \
        } while(0)

#define CMICD_LED_CHAIN_UNLOCK(unit) \
        do { \
            sal_mutex_give(cmicd_led_ctrl[unit].chain_lock); \
        } while(0)

/* CMICd LED register set. */
typedef struct {

    /* Control registers. */
    soc_reg_t ctrl;

    /* Status registers. */
    soc_reg_t status;

    /* LED firmware program. */
    soc_reg_t program;

    /* LED firmware data. */
    soc_reg_t data;

} cmicd_led_regs_t;

/* CMICd LED control data per unit. */
typedef struct
{
    /* Number of microcontrollers. */
    int num_of_uc;

    /* CMICd LED register set. */
    cmicd_led_regs_t *regs;

    /* CMICd LED chain hardware decriptor. */
    cmicd_led_chain_t chain;

    /* Mutex for the protection of LED chain updating. */
    sal_mutex_t chain_lock;

    /* Mutex for the protection of LED uC controlling. */
    sal_mutex_t lock[CMIC_LED_UC_MAX];

} cmicd_led_ctrl_t;

/*******************************************************************************
 * Local variables.
 */

/* CMICd LED control data per unit.*/
static cmicd_led_ctrl_t cmicd_led_ctrl[SOC_MAX_NUM_DEVICES] = { {0} };

/* CMICd control registers. */
static cmicd_led_regs_t cmicd_led_regs[] = {
    {CMIC_LEDUP0_CTRLr, CMIC_LEDUP0_STATUSr,
     CMIC_LEDUP0_PROGRAM_RAMr, CMIC_LEDUP0_DATA_RAMr},
    {CMIC_LEDUP1_CTRLr, CMIC_LEDUP1_STATUSr,
     CMIC_LEDUP1_PROGRAM_RAMr, CMIC_LEDUP1_DATA_RAMr},
    {CMIC_LEDUP2_CTRLr, CMIC_LEDUP2_STATUSr,
     CMIC_LEDUP2_PROGRAM_RAMr, CMIC_LEDUP2_DATA_RAMr},
    {CMIC_LEDUP3_CTRLr, CMIC_LEDUP3_STATUSr,
     CMIC_LEDUP3_PROGRAM_RAMr, CMIC_LEDUP3_DATA_RAMr},
    {CMIC_LEDUP4_CTRLr, CMIC_LEDUP4_STATUSr,
     CMIC_LEDUP4_PROGRAM_RAMr, CMIC_LEDUP4_DATA_RAMr},
};

/* CMIC LED uC remap registers. */
static const soc_reg_t
cmicd_remap_regs[CMIC_LED_UC_MAX][CMIC_MAX_PORTS_PER_LED_UC / 4] = {
{
    CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,   CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,  CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r,
    CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r,
},
{
    CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r,   CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r,  CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r, CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r, CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r,
    CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r, CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r,
},
{
    CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r,   CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r,  CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r, CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r, CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r, CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r, CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r,
    CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r, CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r,
},
{
    CMIC_LEDUP3_PORT_ORDER_REMAP_0_3r,   CMIC_LEDUP3_PORT_ORDER_REMAP_4_7r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_8_11r,  CMIC_LEDUP3_PORT_ORDER_REMAP_12_15r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_16_19r, CMIC_LEDUP3_PORT_ORDER_REMAP_20_23r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_24_27r, CMIC_LEDUP3_PORT_ORDER_REMAP_28_31r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_32_35r, CMIC_LEDUP3_PORT_ORDER_REMAP_36_39r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_40_43r, CMIC_LEDUP3_PORT_ORDER_REMAP_44_47r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_48_51r, CMIC_LEDUP3_PORT_ORDER_REMAP_52_55r,
    CMIC_LEDUP3_PORT_ORDER_REMAP_56_59r, CMIC_LEDUP3_PORT_ORDER_REMAP_60_63r,
},
{
    CMIC_LEDUP4_PORT_ORDER_REMAP_0_3r,   CMIC_LEDUP4_PORT_ORDER_REMAP_4_7r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_8_11r,  CMIC_LEDUP4_PORT_ORDER_REMAP_12_15r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_16_19r, CMIC_LEDUP4_PORT_ORDER_REMAP_20_23r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_24_27r, CMIC_LEDUP4_PORT_ORDER_REMAP_28_31r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_32_35r, CMIC_LEDUP4_PORT_ORDER_REMAP_36_39r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_40_43r, CMIC_LEDUP4_PORT_ORDER_REMAP_44_47r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_48_51r, CMIC_LEDUP4_PORT_ORDER_REMAP_52_55r,
    CMIC_LEDUP4_PORT_ORDER_REMAP_56_59r, CMIC_LEDUP4_PORT_ORDER_REMAP_60_63r,
}
};

static soc_field_t cmicd_remap_fields[] = {
    REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f,
    REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f,
    REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f,
    REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f,
    REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f,
    REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f,
    REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f,
    REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f,
    REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f,
    REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f,
    REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f,
    REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f,
    REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f,
    REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f,
    REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f,
    REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f,
};

/*
 * Function:
 *     soc_cmicd_led_uc_num_get
 * Purpose:
 *     Get number of LED microcontrollers.
 * Parameters:
 *     unit Unit number.
 *     led_uc_num Number of LED microcontrollers.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_uc_num_get(int unit, int *led_uc_num)
{

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc_num == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    /* Get nunmber of led uc. */
    *led_uc_num = cmicd_led_ctrl[unit].num_of_uc;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmicd_led_control_data_read
 * Purpose:
 *      Read data from led_control_data space.
 * Parameters:
 *      unit Unit number
 *      offset Offset within led_control_data.
 *      data Read result.
 *      len Read length in byte.
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicd_led_control_data_read(int unit, int led_uc,
                                int offset, uint8 *data, int len)
{
    int i;
    cmicd_led_regs_t *regs;
    uint32 addr, val32;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc < 0 ||
        led_uc >= cmicd_led_ctrl[unit].num_of_uc ||
        data == NULL ||
        offset < 0 ||
        len <= 0 ||
        (offset + len) > CMIC_LED_DATA_SIZE)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    CMICD_LED_LOCK(unit, led_uc);

    /* Read data from data RAM of led uc. */
    regs = &cmicd_led_ctrl[unit].regs[led_uc];
    addr = soc_reg_addr(unit, regs->data, REG_PORT_ANY, 0);
    addr += (offset * 4);

    for (i = 0; i < len; i++) {
         soc_pci_getreg(unit, addr, &val32);
         data[i] = val32 & 0xFF;
         addr += 4;
    }

    CMICD_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicd_led_control_data_write
 * Purpose:
 *     Write control data of LED firmware.
 * Parameters:
 *     unit Unit number
 *     offset Offset within led_control_data.
 *     data Write data.
 *     len Write length in byte.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_control_data_write(int unit, int led_uc,
                                 int offset, const uint8 *data, int len)
{
    cmicd_led_regs_t *regs;
    uint32 addr, data32;
    int i;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc < 0 ||
        led_uc >= cmicd_led_ctrl[unit].num_of_uc ||
        data == NULL ||
        offset < 0 ||
        len <= 0 ||
        (offset + len) > CMIC_LED_DATA_SIZE)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    CMICD_LED_LOCK(unit, led_uc);

    /* Write into Data RAM of led uc. */
    regs = &cmicd_led_ctrl[unit].regs[led_uc];
    addr = soc_reg_addr(unit, regs->data, REG_PORT_ANY, 0);
    addr += (offset * 4);

    for (i = 0; i < len; i ++) {
        data32 = data[i];
        soc_pci_write(unit, addr, data32);
        addr += 4;
    }

    CMICD_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;

}

/*
 * Function:
 *     soc_cmicd_led_start_set
 * Purpose:
 *     Start/stop LED firmware.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, request fw start and 0, request fw stop.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_fw_start_set(int unit, int led_uc, int start)
{
    cmicd_led_regs_t *regs;
    uint32 addr, data32;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc >= cmicd_led_ctrl[unit].num_of_uc) {
        return SOC_E_PARAM;
    }

    start = (start != 0);

    /* Get control register address of LED uc. */
    regs = &cmicd_led_ctrl[unit].regs[led_uc];
    addr = soc_reg_addr(unit, regs->ctrl, REG_PORT_ANY, 0);

    CMICD_LED_LOCK(unit, led_uc);

    /* Modify the enable bit of led uc. */
    soc_pci_getreg(unit, addr, &data32);
    soc_reg_field_set(unit, regs->ctrl, &data32, LEDUP_ENf, start);
    soc_pci_write(unit, addr, data32);

    CMICD_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicd_led_start_get
 * Purpose:
 *     Get if LED firmware is started or not.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, fw started and 0, fw stopped.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_fw_start_get(int unit, int led_uc, int *start)
{
    int rv = SOC_E_NONE;
    cmicd_led_regs_t *regs;
    uint32 addr, data32;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (start == NULL ||
        led_uc >= cmicd_led_ctrl[unit].num_of_uc) {
        return SOC_E_PARAM;
    }

    /* Get control register address of LED uc. */
    regs = &cmicd_led_ctrl[unit].regs[led_uc];
    addr = soc_reg_addr(unit, regs->ctrl, REG_PORT_ANY, 0);

    CMICD_LED_LOCK(unit, led_uc);

    /* Get the enable bit of led uc. */
    soc_pci_getreg(unit, addr, &data32);
    *start = soc_reg_field_get(unit, regs->ctrl, data32, LEDUP_ENf);

    CMICD_LED_UNLOCK(unit, led_uc);

    return rv;
}

/*
 * Function:
 *     soc_cmicd_led_fw_load
 * Purpose:
 *     Load LED firmware binary.
 * Parameters:
 *     unit Unit number
 *     led_uc LED microcontroller number.
 *     data Firmware binary content.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_fw_load(int unit, int led_uc, const uint8 *data, int len)
{
    cmicd_led_regs_t *regs;
    uint32 addr;
    int i;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (data == NULL ||
        len > CMIC_LED_PROGRAM_SIZE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    CMICD_LED_LOCK(unit, led_uc);

    regs = &cmicd_led_ctrl[unit].regs[led_uc];
    addr = soc_reg_addr(unit, regs->program, REG_PORT_ANY, 0);

    for (i = 0; i < len; i ++) {
        soc_pci_write(unit, addr, data[i]);
        addr += 4;
    }

    CMICD_LED_UNLOCK(unit, led_uc);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicd_led_port_to_uc_port_get
 * Purpose:
 *     Map physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_port_to_uc_port_get(int unit, int port, int *led_uc, int *led_uc_port)
{
    soc_reg_t reg;
    uint32 val32, addr;
    int uc, pos;
    cmicd_led_chain_t *chain;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc == NULL || led_uc_port == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    }

    CMICD_LED_CHAIN_LOCK(unit);

    /* Check if the port is already well defined. */
    chain = &cmicd_led_ctrl[unit].chain;
    if (!chain->valid[port]) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Port is not on any LED chain.\n")));
        CMICD_LED_CHAIN_UNLOCK(unit);
        return SOC_E_UNAVAIL;
    }

    /* Translate physical port to (led_uc, led_uc_port). */
    uc = cmicd_led_ctrl[unit].chain.uc[port];
    pos = cmicd_led_ctrl[unit].chain.pos[port];
    reg = cmicd_remap_regs[uc][pos / 4];
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    soc_pci_getreg(unit, addr, &val32);
    *led_uc_port = soc_reg_field_get(unit, reg, val32,
                                     cmicd_remap_fields[pos]);
    *led_uc = uc;

    CMICD_LED_CHAIN_UNLOCK(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicd_led_port_to_uc_port_set
 * Purpose:
 *     Configure the mapping from physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_port_to_uc_port_set(int unit, int port, int led_uc, int led_uc_port)
{
    uint32 val32, addr;
    soc_reg_t reg;
    int pos;
    cmicd_led_chain_t *chain;

    /* Check init state of LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "LED driver not initialized.\n")));
        return SOC_E_INIT;
    }

    /* Parameter sanity check. */
    if (led_uc_port < 0 ||
        led_uc_port >= CMIC_MAX_PORTS_PER_LED_UC) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    };

    CMICD_LED_CHAIN_LOCK(unit);

    chain = &cmicd_led_ctrl[unit].chain;

    /* Check if the port is already well defined. */
    if (!chain->valid[port]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Port is not on any LED chain.\n")));
        CMICD_LED_CHAIN_UNLOCK(unit);
        return SOC_E_UNAVAIL;
    }

    /* Ensure the port won't be assigned out of HW support. */
    if (led_uc != chain->uc[port]) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Port can not be assigned to this LED uC.\n")));
        CMICD_LED_CHAIN_UNLOCK(unit);
        return SOC_E_FAIL;
    };

    /* Assign physical port to (led_uc, led_uc_port). */
    pos = cmicd_led_ctrl[unit].chain.pos[port];
    reg = cmicd_remap_regs[led_uc][pos / 4];
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    soc_pci_getreg(unit, addr, &val32);
    soc_reg_field_set(unit, reg, &val32,
                      cmicd_remap_fields[pos], led_uc_port);
    soc_pci_write(unit, addr, val32);

    CMICD_LED_CHAIN_UNLOCK(unit);

    return SOC_E_NONE;
}

/* CMICd LED driver callback. */
static soc_led_driver_t cmicd_led_driver = {
    .soc_led_uc_num_get = soc_cmicd_led_uc_num_get,
    .soc_led_fw_load = soc_cmicd_led_fw_load,
    .soc_led_fw_start_get = soc_cmicd_led_fw_start_get,
    .soc_led_fw_start_set = soc_cmicd_led_fw_start_set,
    .soc_led_control_data_write = soc_cmicd_led_control_data_write,
    .soc_led_control_data_read = soc_cmicd_led_control_data_read,
    .soc_led_port_to_uc_port_get = soc_cmicd_led_port_to_uc_port_get,
    .soc_led_port_to_uc_port_set = soc_cmicd_led_port_to_uc_port_set,
};

/*
 * Function:
 *      soc_cmicd_led_deinit
 * Purpose:
 *      Cleanup CMICd LED driver.
 * Parameters:
 *      unit Unit number
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
soc_cmicd_led_deinit(int unit)
{
    int uc;

    /* Uninstall cmicd driver callback for BCM API. */
    soc_led_driver_attach(unit, NULL);

    cmicd_led_ctrl[unit].regs = NULL;
    cmicd_led_ctrl[unit].num_of_uc = 0;

    /* Free LED chain lock. */
    if (cmicd_led_ctrl[unit].chain_lock) {
        sal_mutex_destroy(cmicd_led_ctrl[unit].chain_lock);
    }
    cmicd_led_ctrl[unit].chain_lock = NULL;

    /* Free LED uC locks. */
    for (uc = 0; uc < CMIC_LED_UC_MAX; uc++) {
        if (cmicd_led_ctrl[unit].lock[uc]) {
            sal_mutex_destroy(cmicd_led_ctrl[unit].lock[uc]);
        }
        cmicd_led_ctrl[unit].lock[uc] = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmicd_led_init
 * Purpose:
 *      Init CMICd LED driver.
 * Parameters:
 *      unit Unit number
 * Returns:
 *      SOC_E_xxx
 */
STATIC int
soc_cmicd_led_init(int unit)
{
    int uc;
    int num_of_uc;

    /* Probe the number of LED uC. */
    for (num_of_uc = 0; num_of_uc < CMIC_LED_UC_MAX; num_of_uc ++) {
        if (!SOC_REG_IS_VALID(unit, cmicd_led_regs[num_of_uc].ctrl)) {
            break;
        }
    }

    if (num_of_uc == 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "Detected no LED uC.\n")));
        return SOC_E_INIT;
    }

    /* Create lock for CMICd LED chain. */
    cmicd_led_ctrl[unit].chain_lock = sal_mutex_create("CmicLedChainLock");
    if (cmicd_led_ctrl[unit].chain_lock == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "Out of memory.\n")));

        /* Deinit driver. */
        soc_cmicd_led_deinit(unit);
        return SOC_E_MEMORY;
    }

    /* Create locks for CMICd uCs. */
    for (uc = 0; uc < num_of_uc; uc++) {
         cmicd_led_ctrl[unit].lock[uc] = sal_mutex_create("CmicdLedLock");
         if (cmicd_led_ctrl[unit].lock[uc] == NULL) {
             LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Out of memory.\n")));
             /* Deinit driver. */
             soc_cmicd_led_deinit(unit);
             return SOC_E_MEMORY;
         }
    }

    cmicd_led_ctrl[unit].num_of_uc = num_of_uc;
    cmicd_led_ctrl[unit].regs = cmicd_led_regs;

    /* Install cmicd driver callback for BCM API. */
    soc_led_driver_attach(unit, &cmicd_led_driver);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmicd_led_chain_set
 * Purpose:
 *     Set device-specific CMICd LED chains.
 * Parameters:
 *     unit Unit number.
 *     chain A hardware descriptor for device-specific CMICd LED chains.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_cmicd_led_chain_set(int unit, cmicd_led_chain_t *chain)
{
    int uc;
    int pos;
    int port;

    if (chain == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Invalid parameter.\n")));
        return SOC_E_PARAM;
    };

    /* Initialize CMICd LED driver. */
    if (cmicd_led_ctrl[unit].regs == NULL) {
        SOC_IF_ERROR_RETURN(soc_cmicd_led_init(unit));
    }

    /* Check the chain data. */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
         if (chain->valid[port]) {
             uc = chain->uc[port];
             pos = chain->pos[port];
             if (uc < 0 || uc >= cmicd_led_ctrl[unit].num_of_uc ||
                 pos < 0 || pos >= CMIC_MAX_PORTS_PER_LED_UC) {
                 LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "LED chain data wrong (%d,%d,%d).\n"), port, uc, pos));
                 return SOC_E_FAIL;
             }
         }
    }

    /* Update/Init LED chain define. */
    CMICD_LED_CHAIN_LOCK(unit);
    sal_memcpy(&cmicd_led_ctrl[unit].chain, chain, sizeof(cmicd_led_chain_t));
    CMICD_LED_CHAIN_UNLOCK(unit);

    return SOC_E_NONE;
}

#endif /* BCM_CMICM_SUPPORT || BCM_CMICDV4_SUPPORT */
