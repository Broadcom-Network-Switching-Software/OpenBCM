/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bcm_xmod_pe_api.c
 */

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "bcm_xmod_api.h"
#include "bcm_phy.h"

extern int
bcm_xmod_command(int unit, int module, int mode_opcode,  uint32 *arg, int arg_size, uint32 *result, int result_size);
extern uint32
swap_uint32( uint32 val );

#define XMOD_DEV_DEBUG_CMD                          0x1a
#define XMOD_DEV_DEBUG_CMD_IN_LEN   (XMOD_BUFFER_MAX_LEN)
#define XMOD_DEV_DEBUG_CMD_OUT_LEN  (XMOD_BUFFER_MAX_LEN)
/*
 * Function:
 *      bcm_xmod_debug_cmd
 * Purpose:
 *      send a debug command to fw via xmod
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0)
 *      cmd - debug command
 *      txbuf - transmit buffer (arguements passed to fw) must be in LE
 *      txlen - tx buffer length
 *      rsp - command response
 *      rxbuf - recieve buffer (arguements from fw) will be in LE
 *      rxlen - rx buffer length
 * Returns:     
 *      SOC_E_NONE
 */
int bcm_xmod_debug_cmd(int unit, int module, int cmd, void *txbuf, int txlen, int *rsp, void *rxbuf, int rxlen)
{
    uint32 xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint32 xmodrxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8 *buftxptr, *bufrxptr;
    int xmodtxlen, xmodrxlen;
    uint32 x_cmd, x_response;
    int rv;

    /* setting up xmod xmod_dev_debug_cmd:                             */
    /* int xmod_dev_debug_cmd(IN uint32 cmd, OUT uint32 response); */

    /* check for tx lengths */
    if (txlen+sizeof(x_cmd) > XMOD_DEV_DEBUG_CMD_IN_LEN) {
        printf("ERROR, TX length (%d) too long > %d\n", txlen+sizeof(x_cmd), XMOD_DEV_DEBUG_CMD_IN_LEN);
        return SOC_E_PARAM;
    }
    /* check for rx lengths */
    if (rxlen+sizeof(x_response) > XMOD_DEV_DEBUG_CMD_OUT_LEN) {
        printf("ERROR, RX length (%d) too long > %d\n", rxlen+sizeof(x_response), XMOD_DEV_DEBUG_CMD_OUT_LEN);
        return SOC_E_PARAM;
    }
        
    x_cmd = swap_uint32(cmd);
    
    /* write args to xmod buffer */
    buftxptr = (uint8 *)xmodtxbuff;
    WRITE_XMOD_ARG_BUFF(buftxptr, &x_cmd, sizeof(x_cmd));
    xmodtxlen = sizeof(x_cmd);
    if ((txbuf!=NULL) && txlen) {
        WRITE_XMOD_ARG_BUFF(buftxptr, txbuf, txlen);
        xmodtxlen += txlen;
    }
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* get rx len */
    xmodrxlen = sizeof(x_response);
    if ((rxbuf!=NULL) && rxlen) {
        xmodrxlen += rxlen;
    }
    xmodrxlen = GET_XMOD_BUF_WORD_LEN(xmodrxlen);
    
    /* call xmod */
    rv = bcm_xmod_command(unit, module, XMOD_CMD_MODE_CORE(XMOD_DEV_DEBUG_CMD), xmodtxbuff, xmodtxlen, xmodrxbuff, xmodrxlen);

    /* retrieve the argument from the xmod rx buffer */
    bufrxptr = (uint8 *)xmodrxbuff;
    READ_XMOD_ARG_BUFF(bufrxptr, &x_response, sizeof(x_response));
    if ((rxbuf!=NULL) && rxlen) {
        READ_XMOD_ARG_BUFF(bufrxptr, rxbuf, rxlen);
    }
    *rsp = swap_uint32(x_response);

    return rv;
}

/*
 * Function:
 *      bcm_reg_read16
 * Purpose:
 *      reads the register specified by addr
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      addr - register address
 *      value - returned reg value
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_reg_read16(int unit, int module, uint16 addr, uint16 *value)
{
    phy_ctrl_t *pc;
	uint16	val;
    int rc = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, module);

    rc = READ_PHY_REG(unit, pc, addr, &val);
    if (rc == SOC_E_NONE) {
        *value = val;
    }

	return rc;
}


/*
 * Function:
 *      bcm_reg_write16
 * Purpose:
 *      writes value to the register specified by addr
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      addr - register address
 *      value - to be written
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_reg_write16(int unit, int module, uint16 addr, uint16 value)
{
    phy_ctrl_t *pc;
    int rc = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, module);

    rc = WRITE_PHY_REG(unit, pc, addr, value);
    
	return rc;
}



/*
 * Function:
 *      bcm_reg_read32
 * Purpose:
 *      reads the register specified by addr
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      addr - register address
 *      value - returned reg value
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_reg_read32(int unit, int module, uint32 addr, uint32 *value)
{
    phy_ctrl_t *pc;
	uint32	val;
    int rc = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, module);

    rc = _bcm_toplvl_sbus_read(unit, pc, addr, &val, 1);
    if (rc == SOC_E_NONE) {
        *value = val;
    }

	return rc;
}


/*
 * Function:
 *      bcm_reg_write32
 * Purpose:
 *      reads the register specified by addr
 * Parameters:
 *      unit - BCM unit number (0).
 *      module - module number. (0,1)
 *      addr - register address
 *      value - value to be written
 * Returns:     
 *      SOC_E_NONE
 */
int
bcm_reg_write32(int unit, int module, uint32 addr, uint32 value)
{
    phy_ctrl_t *pc;
    int rc = SOC_E_NONE;

    pc = EXT_PHY_SW_STATE(unit, module);

    rc = _bcm_toplvl_sbus_write(unit, pc, addr, &value, 1);
    
	return rc;
}




