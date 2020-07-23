/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        main.c
 * Purpose:     PE Mode utility code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bcm_xmod_pe_api.h"

extern int bcm_xmod_debug_cmd(int unit, int module, int cmd, void *txbuf, int txlen, int *rsp, void *rxbuf, int rxlen);
extern int bcm_reset_cpu(int unit, int module);
#ifdef PEMODE_BCM_BUILD
extern FILE * sal_fopen(char *file, char *mode);
extern int sal_fclose(FILE *fp);
extern int sal_fread(void *buf, int size, int num, FILE *fp);
extern int sal_feof(FILE *fp);
extern int sal_fsize(FILE *fp);
extern void *sal_alloc(unsigned int sz, char *s);
extern void sal_free(void *addr);
char *flname = "BCM8806x_pe"; 
#endif /* PEMODE_BCM_BUILD */
int bcm_xmod_db_test(int unit, int module);
int bcm_xmod_db_lb(int unit, int module, int lb_port, int lb, int en);
int bcm_xmod_db_stats(int unit, int module, int pe_port);
int bcm_xmod_db_insert_tag(int unit, int module, int pe_port);

void
display_usage(void) {
    printf( "Usage: pe_app <test>\n");
    printf( "  1 - FW download test\n");
    printf( "  2 - PE Init(mod[0/2], mode[1/2], 10g, ltup, tlpe0, ltpe1, ltpe2, ltpe3)\n");
    printf( "  3 - PE Port Status Get(mod[0/2])\n");
    printf( "  4 - PE Port Statistics Get(mod[0/2], clear)\n");
    printf( "  5 - PE FW Ver Get(mod[0/2])\n");
    printf( "  6 - PE Device Rev ID\n");
    printf( "  7 - PE Die Temp\n");
    printf( "  8 - XMOD Debug Test Cmd(mod[0/2])\n");
    printf( "  9 - XMOD Debug Loopback Cmd(mod[0/2], gport[0,4,5,6,7], lb, enable)\n");
    printf( " 10 - Resest CPUs\n");
    printf( " 11 - FW Print port stats(mod[0/2], gport[0,4,5,6,7])\n");
    printf( " 12 - FW Check insert tag(mod[0/2], port[0-3])\n");
    printf( " 13 - TSC Reg Access(port[0-3], sys[0/1], r/w[0/1], reg(32b), val(16b))\n");
    printf( " 14 - SBUS Access(port[0-3], reg(32b), cmd(32b), ring[0-2], val(32b))\n");
    printf( " 15 - AXI Reg Access(r/w[0/1], reg(32b), val(32b))\n");
    printf( " 16 - TOP Reg Access(r/w[0/1], reg(32b), val(32b))\n");
    printf( " 17 - Eyescan(port[0-3], sys[0/1])\n");
    printf( " 18 - DSC Dump(port[0-3], sys[0/1])\n");
    printf( " 19 - PRBS(port[0-3], sys[0/1], action[0-2], dir[0-2], poly[0-6], invert[0/1])\n");
    printf( "Parameters:\n");
    printf( "  mod - Module: 0-First Module(default); 2-Second Module\n");
    printf( "  mode - Mode: 1-1x4(50G up port, default); 2-1x2(25G up port)\n");
    printf( "  10g - Force up port link speed to 10G\n");
    printf( "  ltup - Set Link Training on up port\n");
    printf( "  ltpe(x) - Set Link Training on pe(x) port\n");
    printf( "  port - 0-port0; 1-port1; 2-port2; 3-port3\n");
    printf( "  gport - 0-LS port0; 4-SS port0; 5-SS port1; 6-SS port2; 7-SS port3\n");
    printf( "  sys - System: 0-Line Side(up port); 1-System Side(server port)\n");
    printf( "  r/w - 0-Read; 1-Write\n");
    printf( "  ring - 0-MIG0; 1-MIG1; 2-TOP\n");
    printf( "  action - 0-Set; 1-Get; 2-Clear\n");
    printf( "  dir - 0-RX/TX; 1-RX; 2-TX\n");
    printf( "  poly - 0-Poly7; 1-Poly9; 2-Poly11; 3-Poly15; 4-Poly23; 5-Poly31; 6-Poly58\n");
    printf( "  invert - 0-NoInvert; 1-Invert\n");
}


#ifdef INCLUDE_MAIN
int main() {
    pe_app(0,0,0,0);
    return 0;
}
#endif


int
pe_app(int testnum, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)
{
    xmod_pe_config_t    config;
    xmod_pe_status_t    status;
    xmod_pe_port_stats_t stats;
    xmod_pe_fw_ver_t    ver;
    int rc;
    int unit, module, rev_id, temp;
    int pe_port;
#ifdef PEMODE_BCM_BUILD
    FILE *fp;
#endif /* PEMODE_BCM_BUILD */
    void   *fl_data;
    int     fl_len;
    int     value;
    uint32  value32;
    uint32  data32[4];

    printf("pe_app() testnum: %d arg0 0x%x, arg1 0x%x, arg2 0x%x, arg3 0x%x, arg4 0x%x, arg5 0x%x, arg6 0x%x, arg7 0x%x\n",
        testnum, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    bcm_initialize();
    unit = 0;
    module = 0;

    switch (testnum) {
        case 1:
            printf("test %d\n", testnum);
            fl_data = NULL;
            fl_len = 0;
#ifdef PEMODE_BCM_BUILD
            if ((fp = sal_fopen(flname, "rb")) == NULL) {
                printf("%s() cannot read file: %s\n", __func__, flname);
                break;
            }
            fl_len = sal_fsize(fp);
            printf("%s() file %s length: %d\n", __func__, flname, fl_len);
            /* need to close/open to reset fp to beg file to read data*/
            sal_fclose(fp);
            if ((fp = sal_fopen(flname, "rb")) == NULL) {
                printf("%s() cannot read file: %s\n", __func__, flname);
                break;
            }
            fl_data = sal_alloc(fl_len, __FILE__);
            if (fl_data == NULL) {
                printf("%s() cannot alloc memory\n", __func__);
                sal_fclose(fp);
                break;
            }
            rc = sal_fread(fl_data, 1, fl_len, fp);
            sal_fclose(fp);
            if (rc != fl_len) {
                printf("%s() only read %d of %d bytes\n", __func__, rc, fl_len);
                break;
            }
            printf("%s() fl_data 0x%p fl_len %d\n", __func__, fl_data, fl_len);
#endif /* PEMODE_BCM_BUILD */
            rc = bcm_firmware_download(unit, module, fl_data, fl_len);
            if (rc!=0) {
                printf("%s() bcm_firmware_download() error %d\n", __func__, rc);
            }
#ifdef PEMODE_BCM_BUILD
              sal_free(fl_data);
#endif /* PEMODE_BCM_BUILD */
            break;
        case 2:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            if (arg1!=1 && arg1!=2) {
                arg1=XMOD_PE_MODE_4_PORTS;
            }
            module = arg0;
            config.mode = arg1;
            config.tag = XMOD_PE_TAG_BR;
            config.pfc = 1;
            config.line10g = arg2;
            config.tagid[0] = 0x11;
            config.tagid[1] = 0x22;
            config.tagid[2] = 0x33;
            config.tagid[3] = 0x44;
            config.lt_up_port = arg3;
            config.lt_pe_port[0] = arg4;
            config.lt_pe_port[1] = arg5;
            config.lt_pe_port[2] = arg6;
            config.lt_pe_port[3] = arg7;
            rc = bcm_xmod_pe_init(unit, module, &config);
            if (rc!=0) {
                printf("%s() bcm_xmod_pe_init() error %d\n", __func__, rc);
            }
            break;
        case 3:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            rc = bcm_xmod_pe_status_get(unit, module, &status);
            if (rc!=0) {
                printf("%s() bcm_xmod_pe_status_get() error %d\n", __func__, rc);
            }
            break;
        case 4:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            for (pe_port=0; pe_port<=4; pe_port++) {
                printf("%s() calling bcm_xmod_pe_port_stats_get(%d)\n", __func__, pe_port);
                rc = bcm_xmod_pe_port_stats_get(unit, module, pe_port, &stats, arg1);
                if (rc!=0) {
                    printf("%s() bcm_xmod_pe_port_stats_get() error %d\n", __func__, rc);
                }
            }
            break;
        case 5:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            rc = bcm_xmod_pe_fw_ver_get(unit, module, &ver);
            if (rc!=0) {
                printf("%s() bcm_xmod_pe_fw_ver_get() error %d\n", __func__, rc);
            }
            break;
        case 6:
            printf("test %d\n", testnum);
            rc = bcm_device_rev_id(unit, module, &rev_id);
            if (rc!=0) {
                printf("%s() bcm_device_rev_id() error %d\n", __func__, rc);
            }
            printf("%s() rev_id %d\n", __func__, rev_id);
            break;
        case 7:
            printf("test %d\n", testnum);
            rc = bcm_device_die_temp(unit, module, &temp);
            if (rc!=0) {
                printf("%s() bcm_device_die_temp() error %d\n", __func__, rc);
            }
            printf("%s() die temp %d\n", __func__, temp);
            break;
        case 8:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            bcm_xmod_db_test(unit, module);
            break;
        case 9:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            bcm_xmod_db_lb(unit, module, arg1, arg2, arg3);
            break;
        case 10:
            printf("test %d\n", testnum);
            bcm_reset_cpu(unit, module);
            break;
        case 11:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            bcm_xmod_db_stats(unit, module, arg1);
            break;
        case 12:
            printf("test %d\n", testnum);
            if (arg0!=0 && arg0!=2) {
                printf("%s() ERROR, invalid arg0 %d\n", __func__, arg0);
                break;
            }
            module = arg0;
            bcm_xmod_db_insert_tag(unit, module, arg1);
            break;
        case 13:
            printf("test %d\n", testnum);
            value=arg4;
            bcm_tsc_reg_access(unit, arg0, arg1, arg2, arg3, &value);
            break;
        case 14:
            printf("test %d\n", testnum);
            data32[0]=arg4;
            data32[1]=arg5;
            data32[2]=arg6;
            data32[3]=arg7;
            bcm_sbus_access(unit, arg0, arg1, arg2, arg3, data32);
            break;
        case 15:
            printf("test %d\n", testnum);
            value32=arg2;
            bcm_axi_reg_access(unit, module, arg0, arg1, &value32);
            break;
        case 16:
            printf("test %d\n", testnum);
            value32=arg2;
            bcm_top_reg_access(unit, module, arg0, arg1, &value32);
            break;
        case 17:
            printf("test %d\n", testnum);
            bcm_eyescan(unit, arg0, arg1);
            break;
        case 18:
            printf("test %d\n", testnum);
            bcm_dscdump(unit, arg0, arg1);
            break;
        case 19:
            printf("test %d\n", testnum);
            bcm_prbs(unit, arg0, arg1, arg2, arg3, arg4, arg5);
            break;
        default:
            display_usage();
            break;
    }
    return 0;
}


int
bcm_xmod_db_test(int unit, int module)
{
    int     idx;
    int     rsp;
    int     rc;
    uint8   txbuf[32];
    uint8   rxbuf[32];

    printf("%s() uint %d, module %d\n", __func__, unit, module);

    /* init buffers */
    for (idx=0; idx<32; idx++) {
        txbuf[idx]=0x20+idx;
        rxbuf[idx]=0;
    }

    rc = bcm_xmod_debug_cmd(unit, module, 0, (void*)txbuf, 32, &rsp, (void*)rxbuf, 32);
    if (rc) {
        printf("ERROR %d\n", rc);
    } else {
        printf("rsp (%d): RX DATA:", rsp);
        for (idx=0; idx<32; idx++) {
            printf("%02x ", rxbuf[idx]);
        }
        printf("\n");
    }

    return rc;    
}


int
bcm_xmod_db_lb(int unit, int module, int lb_port, int lb, int en)
{
    int     rsp;
    int     rc;
    uint8   txbuf[32];

    printf("%s() uint %d, module %d\n", __func__, unit, module);

    /* init buffers */
    txbuf[0]=lb_port;  /* port */
    txbuf[1]=lb;     /* LB - xmodLoopbackRemotePCS */
    txbuf[2]=en;     /* enable */

    rc = bcm_xmod_debug_cmd(unit, module, 1, (void*)txbuf, 3, &rsp, NULL, 0);
    if (rc) {
        printf("ERROR %d\n", rc);
    }

    return rc;    
}


int
bcm_xmod_db_stats(int unit, int module, int pe_port)
{
    int     rsp;
    int     rc;
    uint8   txbuf[32];

    printf("%s() uint %d, module %d\n", __func__, unit, module);

    /* init buffers */
    txbuf[0]=pe_port;   /* port */

    rc = bcm_xmod_debug_cmd(unit, module, 2, (void*)txbuf, 1, &rsp, NULL, 0);
    if (rc) {
        printf("ERROR %d\n", rc);
    }

    return rc;
}


int
bcm_xmod_db_insert_tag(int unit, int module, int pe_port)
{
    int     rsp;
    int     rc;
    uint8   txbuf[32];

    printf("%s() uint %d, module %d\n", __func__, unit, module);

    /* init buffers */
    txbuf[0]=pe_port;   /* port */

    rc = bcm_xmod_debug_cmd(unit, module, 3, (void*)txbuf, 1, &rsp, NULL, 0);
    if (rc) {
        printf("ERROR %d\n", rc);
    }

    return rc;
}

    
