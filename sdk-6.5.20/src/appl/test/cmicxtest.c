/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicxtest.c
 *
 * cmicx test - simple test of the cmicx sim from the sdk.
 */


/* includes */
/* #include "cmicx.h"*/
#include <appl/diag/test.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include "testlist.h"

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/phyctrl.h>

#if defined(PLISIM) && defined(BCM_CMICX_SUPPORT)

#include <../systems/sim/pcid/cmicx_sim.h>


/* defines */
#ifdef  PTRS_ARE_64BITS
#define CMICX_SIM_64_BIT   1
#else
#undef CMICX_SIM_64_BITS
#endif


/* schan command building function (from opcode, blockid/dport, etc.) */
static void _cmic_schan_command_build(schan_command_opcode opcode,int dport,
                      int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command) {

    *command = 0;
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_OPCODE_MSB,  CMIC_SCHAN_COMMAND_OPCODE_LSB,  (int)opcode);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_DPORT_MSB,   CMIC_SCHAN_COMMAND_DPORT_LSB,         dport);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_DLEN_MSB,    CMIC_SCHAN_COMMAND_DLEN_LSB,           dlen);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_ERR_BIT,                                             err);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_ECODE_MSB,   CMIC_SCHAN_COMMAND_ECODE_LSB,         ecode);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_DMA_BIT,                                             dma);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_BANK_MSB,    CMIC_SCHAN_COMMAND_BANK_LSB,           bank);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_NACK_BIT,                                           nack);

}


/* initialize the cmicx test */
int cmicx_test_init(int u, args_t *a, void **pa) {

  /*
    soc_pci_write(unit, addr, data);
    reg = soc_pci_read(unit, addr);
  */

    sal_printf("CMICx Sim Test Init'd\n");

    return 0;
}


/* run the cmicx test */
int cmicx_test_run(int u, args_t *a, void *pa) {

    /* variables */
    reg_t   schan_command = 0;
    reg_t   ctrl_data     = 0;
    reg_t   req_data      = 0;
    reg_t   status_data   = 0;
    reg_t   descmem[CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR];    /* memory for descriptors */
    reg_t   hostmem0[20];  /* memory for sbus acks */
    reg_t   hostmem1[20];  /* memory for sbus acks */
#define PACKET_DATA_BYTES 66
    const int     packet_data_bytes = PACKET_DATA_BYTES;
    unsigned char packet_data[] = { 0xBA,0xD0,0xBE,0xEF,0x00,0x00,0x00,0x00,   /* byte 8 here needs to be 0 to indicated the proper cos for the packet (0) in case of loopback. */
                                    0xBA,0xBE,0xFA,0xCE,0x01,0x23,0x45,0x67,
                                    0x89,0xAB,0xCD,0xEF,0xDE,0xAD,0xFE,0xED,
                                    0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                                    0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x00,
                                    0x11,0x00,0x00,0x11,0x11,0x00,0x00,0x11,
                                    0x00,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                    0xFF,0xFF,0x00,0x00,0x00,0x00 };
    unsigned char packet_data2[PACKET_DATA_BYTES + 4];
    int     unit          = 0;
    int     i;

    /* zero our host memory to avoid problems */
    memset(descmem,      0, sizeof(descmem));
    memset(hostmem0,     0, sizeof(hostmem0));
    memset(hostmem1,     0, sizeof(hostmem1));
    memset(packet_data2, 0, sizeof(packet_data2));

    sal_printf("CMICx Sim Test Running...\n");


    /*******************************************************/
    /*** schan testing *************************************/
    /*******************************************************/

    /* feed some schan commands to the cmicx to test the schan. */
    /* build a command opcode first. format: _cmic_schan_command_build
     * (schan_command_opcode opcode,int dport,int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command)
     */
    schan_command = 0;
    _cmic_schan_command_build( SCHAN_MEM_RD,1,0, false,0,false,0,false, &schan_command );
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_ADDR,   0x111111);    /* set all schan weights on the sbus to 1 */
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_ADDR, 0x111111);    /* set all sbusdma weights on the sbus to 1 */
    soc_pci_write( unit, CMIC_TOP_SBUS_TIMEOUT_REG_ADDR,                   0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_0_7_REG_ADDR,           0x70);   /* block id 1 is configured to sbus id 7 */
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_8_15_REG_ADDR,             0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_16_23_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_24_31_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_32_39_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_40_47_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_48_55_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_56_63_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_64_71_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_72_79_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_80_87_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_88_95_REG_ADDR,            0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_96_103_REG_ADDR,           0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_104_111_REG_ADDR,          0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_112_119_REG_ADDR,          0);
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_MAP_120_127_REG_ADDR,          0);
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE0_REG_ADDR, schan_command);
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE1_REG_ADDR,        0xABCD);
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_CTRL_REG_ADDR,               (reg_t)0x1);    /* send a MSG_START */
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_ACK_DATA_BEAT_COUNT_REG_ADDR,         0);
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_ERR_REG_ADDR,                         0);

    /* the simulation will be continually updated remotely. just read the schan control register to check the state of the schan access */
    ctrl_data = 0;
    do {
        ctrl_data = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CH0_CTRL_REG_ADDR);
    }
    while (REG_BIT_GET(ctrl_data,CMIC_SCHAN_CTRL_REG_MSG_DONE_BIT)==false);

    /* at this point, we have schan completion, so read the ack data and clear the msg_done bit */
    /* expected output:
     *
     * [SIM] schan ack command read:
     * 0x20080200
     * 0x0000BEEF
     */
    sal_printf("\nschan ack command read: ");
    schan_command = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE0_REG_ADDR);
    sal_printf("0x%08X\n",schan_command);
    schan_command = soc_pci_read(unit, CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE1_REG_ADDR);
    sal_printf("0x%08X\n\n",schan_command);
    soc_pci_write( unit, CMIC_COMMON_POOL_SCHAN_CH0_CTRL_REG_ADDR, (reg_t)0);


    /*******************************************************/
    /*** sbus dma register mode testing ********************/
    /*******************************************************/

    /* test the sbus dma with a simple case (register mode) */
    /* build the sbus command for the dma operation: _cmic_schan_command_build
     * (schan_command_opcode opcode,int dport,int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command)
     * and then build the request register with the desired settings. */
    schan_command = 0;
    req_data      = 0;
    _cmic_schan_command_build( SCHAN_MEM_RD,1,0, false,0,true,0,false, &schan_command );
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_DECR_BIT,                                                 false);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_REQ_SINGLE_BIT,                                           false);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT,                                           false);
    REG_FIELD_SET_TO( req_data, CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_MSB, CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_LSB,      2);   /* a shift of 2 means an increment of 4. */
    REG_FIELD_SET_TO( req_data, CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_MSB,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_LSB,     4);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_DMA_WR_TO_NULLSPACE_BIT,                                  false);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_WORDSWAP_IN_64BIT_SBUSDATA_BIT,                           false);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_MEMRD_ENDIANNESS_BIT,                                     false);
    REG_BIT_SET_TO(   req_data, CMIC_SBUSDMA_REQUEST_REG_MEMWR_ENDIANNESS_BIT,                                     false);
    REG_FIELD_SET_TO( req_data, CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_LSB,         0);
    REG_FIELD_SET_TO( req_data, CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_LSB,         1);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_TIMER_REG_ADDR,                                                   0);    /* disable "timer based mode" for the sbus dma */
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_ADDR,                                    0x111111);    /* set all sbusdma weights on the sbus to 1 */
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_REQUEST_REG_ADDR,                                          req_data);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_COUNT_REG_ADDR,                                                   2);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_OPCODE_REG_ADDR,                                      schan_command);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_SBUS_START_ADDRESS_REG_ADDR,                                 0xABCD);
#ifdef CMICX_SIM_64_BIT
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_LO_REG_ADDR,  (reg_t)( (uint64)hostmem0));
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_HI_REG_ADDR,  (reg_t)(((uint64)hostmem0)>>32));
#else
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_LO_REG_ADDR,  (reg_t)hostmem0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_HI_REG_ADDR,  (reg_t)0);
#endif
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_LO_REG_ADDR,                                 0x0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_HI_REG_ADDR,                                 0x0);
    soc_pci_write( unit, CMIC_CMC0_SHARED_CONFIG_REG_ADDR,                                                     0x0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_CONTROL_REG_ADDR,                                               0x1);   /* start the sbus dma transaction */

    /* update the simulation and check for sbus dma completion */
    status_data = 0;
    do {
        status_data = soc_pci_read( unit, CMIC_CMC0_SBUSDMA_CH0_STATUS_REG_ADDR );
    }
    while (REG_BIT_GET(status_data,CMIC_SBUSDMA_STATUS_REG_DONE_BIT)==false);   /* wait until we get a done confirmation */

    /* at this point, we have simple sbus dma completion, so check the result (do a mem dump) */
    /* expected output:
     *
     * hostmem0+0: 0x0000BEEF
     * hostmem0+4: 0x0000BEEF
     */
    soc_pci_write( unit, CMICX_SIM_CONTROL_REG_ADDR, 0x1 );
    for (i=0; i<2; i++) {
#ifdef CMICX_SIM_64_BIT
        sal_printf("0x%016lX: 0x%08X\n",(uint64)&(hostmem0[i]),hostmem0[i]);
#else
        sal_printf("0x%08X: 0x%08X\n",(unsigned)&(hostmem0[i]),hostmem0[i]);
#endif
    }
    sal_printf("\n");


    /*******************************************************/
    /*** sbus dma descriptor mode testing ******************/
    /*******************************************************/

    /* test the sbus dma with a more complicated case (descriptor mode) */
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_CONTROL_REG_ADDR,             0x0);   /* clear the start bit (and in fact all bits) from the previous transaction */
    REG_BIT_SET_TO(   descmem[0], CMIC_SBUSDMA_CONTROL_DESC_LAST_BIT,                    true);
    REG_BIT_SET_TO(   descmem[0], CMIC_SBUSDMA_CONTROL_DESC_SKIP_BIT,                   false);
    REG_BIT_SET_TO(   descmem[0], CMIC_SBUSDMA_CONTROL_DESC_JUMP_BIT,                   false);
    REG_BIT_SET_TO(   descmem[0], CMIC_SBUSDMA_CONTROL_DESC_APPEND_BIT,                 false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_DECR_BIT,                                                 false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_REQ_SINGLE_BIT,                                           false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT,                                           false);
    REG_FIELD_SET_TO( descmem[1], CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_MSB, CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_LSB,      2);   /* a shift of 2 means an increment of 4. */
    REG_FIELD_SET_TO( descmem[1], CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_MSB,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_LSB,     4);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_DMA_WR_TO_NULLSPACE_BIT,                                  false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_WORDSWAP_IN_64BIT_SBUSDATA_BIT,                           false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_MEMRD_ENDIANNESS_BIT,                                     false);
    REG_BIT_SET_TO(   descmem[1], CMIC_SBUSDMA_REQUEST_REG_MEMWR_ENDIANNESS_BIT,                                     false);
    REG_FIELD_SET_TO( descmem[1], CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_LSB,         0);
    REG_FIELD_SET_TO( descmem[1], CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_LSB,         1);
    descmem[2] = 2;                 /* count = 2 */
    _cmic_schan_command_build( SCHAN_MEM_RD,1,0, false,0,true,0,false, &descmem[3] );
    descmem[4] = 0xABCD;            /* sbus address */
#ifdef CMICX_SIM_64_BIT
    descmem[5] = (reg_t)( (uint64)hostmem1);        /* hostmem address lo */
    descmem[6] = (reg_t)(((uint64)hostmem1)>>32);   /* hostmem address hi */
#else
    descmem[5] = (reg_t)hostmem1;      /* hostmem address lo */
    descmem[6] = (reg_t)0;             /* hostmem address hi */
#endif
    descmem[7] = 0x0;               /* unused */
    soc_pci_write( unit, CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_ADDR,                                  0x111111);    /* set all sbusdma weights on the sbus to 1 */
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_REQUEST_REG_ADDR,                                               0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_COUNT_REG_ADDR,                                                 0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_OPCODE_REG_ADDR,                                                0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_SBUS_START_ADDRESS_REG_ADDR,                                  0x0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                            0x0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                            0x0);
#ifdef CMICX_SIM_64_BIT
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_LO_REG_ADDR,    (reg_t)( (uint64)descmem));        /* desc start adress is necessary to point to descriptor in host memory */
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_HI_REG_ADDR,    (reg_t)(((uint64)descmem)>>32));   /* desc start adress is necessary to point to descriptor in host memory */
#else
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_LO_REG_ADDR,    (reg_t)descmem);   /* desc start adress is necessary to point to descriptor in host memory */
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_HI_REG_ADDR,    (reg_t)0);         /* desc start adress is necessary to point to descriptor in host memory */
#endif
    soc_pci_write( unit, CMIC_CMC0_SHARED_CONFIG_REG_ADDR,                                                   0x0);
    soc_pci_write( unit, CMIC_CMC0_SBUSDMA_CH0_CONTROL_REG_ADDR,                                             0x5);   /* start the sbus dma transaction - descriptor mode */

    /* update the simulation and check for sbus dma completion */
    do {
        status_data = soc_pci_read( unit, CMIC_CMC0_SBUSDMA_CH0_STATUS_REG_ADDR );
    }
    while (REG_BIT_GET(status_data,CMIC_SBUSDMA_STATUS_REG_DONE_BIT)==false);   /* wait until we get a done confirmation */

    /* at this point, we have completion of the sbus dma operation, so print the memory out to see the results */
    /* expected output:
     *
     * hostmem1+0: 0x0000BEEF
     * hostmem1+4: 0x0000BEEF
     */
    soc_pci_write( unit, CMICX_SIM_CONTROL_REG_ADDR, 0x5 );
    for (i=0; i<2; i++) {
#ifdef CMICX_SIM_64_BIT
        sal_printf("0x%016lX: 0x%08X\n",(uint64)&(hostmem1[i]),hostmem1[i]);
#else
        sal_printf("0x%08X: 0x%08X\n",(unsigned)&(hostmem1[i]),hostmem1[i]);
#endif
    }
    sal_printf("\n");


    /*******************************************************/
    /*** test packet dma ***********************************/
    /*******************************************************/

    /* write to packet dma registers to start simple packet dma test case - transmit and then recieve a single packet in loop back. */
    /* the packet will be split in half and described with a continuous descriptor chain on both the rx and tx end. */
    ctrl_data = 0;
    REG_BIT_SET_TO(ctrl_data,CMIC_TOP_CONFIG_REG_ENABLE_PIO_WRITE_NON_POSTED_BIT,   false);
    REG_BIT_SET_TO(ctrl_data,CMIC_TOP_CONFIG_REG_IP_2_EP_LOOPBACK_ENABLE_BIT,        true);   /* enable hardware packet loopback. */
    soc_pci_write( unit, CMIC_TOP_CONFIG_REG_ADDR,                        ctrl_data);
    soc_pci_write( unit, CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_REG_ADDR,          0x0);
    soc_pci_write( unit, CMIC_CMC0_SHARED_IRQ_STAT0_REG_ADDR,                   0x0);
    soc_pci_write( unit, CMIC_CMC0_SHARED_IRQ_STAT_CLR0_REG_ADDR,               0x0);
    for (i=0; i<CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR*2; i++) {
        descmem[i] = 0;
    }
#ifdef CMICX_SIM_64_BIT
    descmem[0] = (reg_t)( (uint64)packet_data);        /* location of the start of the packet */
    descmem[1] = (reg_t)(((uint64)packet_data)>>32);
#else
    descmem[0] = (reg_t)packet_data;   /* location of the start of the packet */
    descmem[1] = (reg_t)0;
#endif
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB,CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB,                      32);   /* first descriptor points to 32 bytes of the packet. */
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT,                                                                true);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_GATHER_BIT,                                                               true);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT,                                                              false);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT,                                                           false);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT,                                                        false);
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB,CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB,                         0);
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB,CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB, 1);
#ifdef CMICX_SIM_64_BIT
    descmem[4] = (reg_t)( (uint64)(packet_data+32));        /* the rest of the packet will follow after the first half of the packet, but we still need to tell the dma engine where it is */
    descmem[5] = (reg_t)(((uint64)(packet_data+32))>>32);
#else
    descmem[4] = (reg_t)(packet_data+32);   /* the rest of the packet will follow after the first half of the packet, but we still need to tell the dma engine where it is */
    descmem[5] = (reg_t)0;
#endif
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB,CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB,  (packet_data_bytes-32));   /* the remainder of the packet bytes will be pointed to by this desc. */
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT,                                                               false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_GATHER_BIT,                                                              false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT,                                                              false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT,                                                           false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT,                                                        false);
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB,CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB,                         0);
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB,CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB, 0);
#ifdef CMICX_SIM_64_BIT
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR,   (reg_t)( (uint64)descmem));
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR,   (reg_t)(((uint64)descmem)>>32));
#else
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR,   (reg_t)descmem);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR,   (reg_t)0);
#endif
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_LO_REG_ADDR,       0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_HI_REG_ADDR,       0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0_REG_ADDR,           0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1_REG_ADDR,           0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_INTR_COAL_REG_ADDR,               0x0);
    ctrl_data = 0x0;
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DIRECTION_BIT,                 true);   /* tx transmission */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT,                    true);   /* start dma */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_ABORT_DMA_BIT,                false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_PKTDMA_ENDIANNESS_BIT,        false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DESC_ENDIANNESS_BIT,          false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DROP_RX_PKT_ON_CHAIN_END_BIT, false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_RLD_STS_UPD_DIS_BIT,          false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DESC_DONE_INTR_MODE_BIT,       true);   /* we expect interrupts on packet completion */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_ENABLE_CONTINUOUS_DMA_BIT,    false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_CONTIGUOUS_DESCRIPTORS_BIT,   false);
    REG_FIELD_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_LSB,      0);
    REG_FIELD_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_LSB,            0);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_ENDIANNESS_BIT,                                                           false);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_CTRL_REG_ADDR,               ctrl_data);    /* start the packet dma */

    /* check for packet dma completion on the tx */
    status_data = 0;
    do {
        status_data = soc_pci_read( unit, CMIC_CMC0_PKTDMA_CH0_STAT_REG_ADDR );
    }
    while (REG_BIT_GET(status_data,CMIC_PKTDMA_STAT_REG_CHAIN_DONE_BIT)==false);

    /* set up packet dma for rx of single packet over two descriptors. */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT,                        false);    /* clear the dma enable bit to set up for a new transfer. */
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_CTRL_REG_ADDR,               ctrl_data);
    for (i=0; i<CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR*2; i++) {
        descmem[i] = 0;
    }
#ifdef CMICX_SIM_64_BIT
    descmem[0] = (reg_t)( (uint64)packet_data2);   /* location of the start of the packet */
    descmem[1] = (reg_t)(((uint64)packet_data2)>>32);
#else
    descmem[0] = (reg_t)packet_data2;   /* location of the start of the packet */
    descmem[1] = (reg_t)0;
#endif
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB,CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB,                      32);   /* first descriptor accepts 32 bytes of the packet. */
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT,                                                                true);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_SCATTER_BIT,                                                              true);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT,                                                              false);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT,                                                           false);
    REG_BIT_SET_TO(  descmem[2],CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT,                                                        false);
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB,CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB,                         0);
    REG_FIELD_SET_TO(descmem[2],CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB,CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB, 1);
#ifdef CMICX_SIM_64_BIT
    descmem[4] = (reg_t)( (uint64)(packet_data2+32));   /* the rest of the packet will follow after the first half of the packet, but we still need to tell the dma engine where it is */
    descmem[5] = (reg_t)(((uint64)(packet_data2+32))>>32);
#else
    descmem[4] = (reg_t)(packet_data2+32);   /* the rest of the packet will follow after the first half of the packet, but we still need to tell the dma engine where it is */
    descmem[5] = (reg_t)0;
#endif
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB,CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB,  (packet_data_bytes-32));   /* the remainder of the packet bytes will be pointed to by this desc. */
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT,                                                               false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_SCATTER_BIT,                                                             false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT,                                                              false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT,                                                           false);
    REG_BIT_SET_TO(  descmem[6],CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT,                                                        false);
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB,CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB,                         0);
    REG_FIELD_SET_TO(descmem[6],CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB,CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB, 0);
#ifdef CMICX_SIM_64_BIT
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR, (reg_t)( (uint64)descmem));
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR, (reg_t)(((uint64)descmem)>>32));
#else
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR, (reg_t)descmem);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR, (reg_t)0);
#endif
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_LO_REG_ADDR,       0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_HI_REG_ADDR,       0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0_REG_ADDR,           0x1);   /* set up cos map to direct cos 0 to this channel. */
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1_REG_ADDR,           0x0);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_INTR_COAL_REG_ADDR,               0x0);
    ctrl_data = 0x0;
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DIRECTION_BIT,                false);   /* rx transmission */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT,                    true);   /* start dma */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_ABORT_DMA_BIT,                false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_PKTDMA_ENDIANNESS_BIT,        false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DESC_ENDIANNESS_BIT,          false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DROP_RX_PKT_ON_CHAIN_END_BIT, false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_RLD_STS_UPD_DIS_BIT,          false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_DESC_DONE_INTR_MODE_BIT,       true);   /* we expect interrupts on packet completion */
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_ENABLE_CONTINUOUS_DMA_BIT,    false);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_CONTIGUOUS_DESCRIPTORS_BIT,   false);
    REG_FIELD_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_LSB,      0);
    REG_FIELD_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_LSB,            0);
    REG_BIT_SET_TO(ctrl_data,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_ENDIANNESS_BIT,                                                           false);
    soc_pci_write( unit, CMIC_CMC0_PKTDMA_CH0_CTRL_REG_ADDR,               ctrl_data);    /* start the packet dma */

    /* check for packet dma completion on the rx */
    status_data = 0;
    do {
        status_data = soc_pci_read( unit, CMIC_CMC0_PKTDMA_CH0_STAT_REG_ADDR );
    }
    while (REG_BIT_GET(status_data,CMIC_PKTDMA_STAT_REG_CHAIN_DONE_BIT)==false);

    /* print the packet 2 memory. should reveal the original packet, which was read through the packet dma rx mechanism. */
    /* expected output:
     *
     * BA D0 BE EF 00 00 00 00
     * BA BE FA CE 01 23 45 67
     * 89 AB CD EF DE AD FE ED
     * 01 23 45 67 89 AB CD EF
     * 00 00 00 00 00 11 11 00
     * 11 00 00 11 11 00 00 11
     * 00 11 11 00 00 00 00 00
     * 00 00 00 00 00 00 00 00
     * FF FF
     */
    soc_pci_write( unit, CMICX_SIM_CONTROL_REG_ADDR, 0x1 );
    for (i=0; i<packet_data_bytes; i++) {
        sal_printf("%02X ",(unsigned)packet_data2[i]);
        if ((i%8)==7) {
            sal_printf("\n");
        }
    }
    sal_printf("\n\n");


    sal_printf("CMICx Sim Test Done.\n");
    return 0;
}


/* end the cmicx test */
int cmicx_test_done(int u, void *pa) {

    sal_printf("CMICx Sim Test Done.\n");

    return 0;
}

#endif /* PLISIM & BCM_CMICX_SUPPORT */
