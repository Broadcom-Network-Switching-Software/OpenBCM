/*! \file bcm56780_a0_macsec_tdm.c
 *
 * File containing macsec tdm for bcm56780_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/trident4/dv/ngsdk_interface/flexport/bcm56780_a0_macsec_tdm.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/chip/bcm56780_a0_sec.h>
#include <bcmsec/bcmsec_utils_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_FLEXPORT

typedef struct {
    /*! Physical to logical port mapping. */
    int port_p2l_mapping[MAX_NUM_PPORT];
    /*! Number of lanes in the port macro of logical port. */
    int port_num_lanes[MAX_NUM_LPORT];
} macsec_drv_info_t;


static void bcm56780_a0_get_port_mode(int unit , int pm_num, int *u0_port_mode , int *u1_port_mode, macsec_drv_info_t *macsec_drv_info);
static int tdm_gen(int unit ,int bh0_l,int bh0_h,int bh1_l,int bh1_h, int inst);

static void bcm56780_a0_mac_drv_info_populate(int unit, macsec_drv_info_t *macsec_drv_info)
{
    bcmsec_dev_info_t *dev_info;
    bcmsec_pport_info_t *pport_info, *pinfo;
    bcmsec_lport_info_t *lport_info, *linfo;
    int i;

    bcmsec_dev_info_get(unit, &dev_info);
    bcmsec_pport_info_get(unit, &pport_info);
    bcmsec_lport_info_get(unit, &lport_info);
    for (i=0 ; i < dev_info->num_pport; i++ ){
        pinfo = &pport_info[i];
        macsec_drv_info->port_p2l_mapping[i] =  pinfo->port_p2l_mapping;
    }
    for (i=0 ; i < dev_info->num_lport; i++ ){
        linfo = &lport_info[i];
        macsec_drv_info->port_num_lanes[i] =  linfo->port_num_lanes;
    }
}


/*! @fn void bcm56780_a0_mac_sec_tdm_gen(int unit)
 *  @param unit Device number
 *  @brief API will program all the tdm tables of 6 macsec instances.
 */
static int
bcm56780_a0_mac_sec_tdm_gen(int unit,macsec_drv_info_t *macsec_drv_info)
{
    int ioerr = 0;
    TOP_MACSEC_CTRL0r_t result;
    uint32_t active ;
   // int mac_sec_num ;
    int ix;
    int u0_port_mode0;
    int u0_port_mode1;
    int u1_port_mode0;
    int u1_port_mode1;

    uint32_t macsec_active[12] = { 0, 0, 0, 0, 0, 0 ,
                                   0, 0, 0, 0, 0, 0
                                 };

    uint32_t macsec_ch_ports[12] = { 0 , 0 , 4, 5, 0 , 0 ,
                                     0 , 0 , 15, 14, 0 ,0
                                   };

/* reg fields of TOP_MACSEC_CTRL0r_t
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM0_OR_PM6f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM10_OR_PM16f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM11_OR_PM17f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM12_OR_PM18f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM13_OR_PM19f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM1_OR_PM7f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM2_OR_PM8f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC_SRC_PM3_OR_PM9f_GET,
 * TOP_MACSEC_CTRL0r_MACSEC0_ACTIVEf_GET,
 * TOP_MACSEC_CTRL0r_MACSEC1_ACTIVEf_GET,
 * TOP_MACSEC_CTRL0r_MACSEC2_ACTIVEf_GET,
 * TOP_MACSEC_CTRL0r_MACSEC3_ACTIVEf_GET,
 * TOP_MACSEC_CTRL0r_MACSEC4_ACTIVEf_GET,
 * TOP_MACSEC_CTRL0r_MACSEC5_ACTIVEf_GET,
 */


/* will derive the pm for macsec configured through registers.
 */
/* TOP_MACSEC_CTRL0r_t result;*/
    SHR_FUNC_ENTER(unit);
    ioerr += READ_TOP_MACSEC_CTRL0r(unit, &result);
/*  active = TOP_MACSEC_CTRL0r_GET(result); */
/*  active = active & 0xffff; */
    active = TOP_MACSEC_CTRL0r_MACSEC0_ACTIVEf_GET(result);
    macsec_active[0] = active & 0x1;
    macsec_active[1] = (active >> 1) & 0x1 ;
    active = TOP_MACSEC_CTRL0r_MACSEC1_ACTIVEf_GET(result);
    macsec_active[2] = active & 0x1;
    macsec_active[3] = (active >> 1) & 0x1 ;
    active = TOP_MACSEC_CTRL0r_MACSEC2_ACTIVEf_GET(result);
    macsec_active[4] = active & 0x1;
    macsec_active[5] = (active >> 1) & 0x1 ;
    active = TOP_MACSEC_CTRL0r_MACSEC3_ACTIVEf_GET(result);
    macsec_active[6] = active & 0x1;
    macsec_active[7] = (active >> 1) & 0x1 ;
    active = TOP_MACSEC_CTRL0r_MACSEC4_ACTIVEf_GET(result);
    macsec_active[8] = active & 0x1;
    macsec_active[9] = (active >> 1) & 0x1 ;
    active = TOP_MACSEC_CTRL0r_MACSEC5_ACTIVEf_GET(result);
    macsec_active[10] = active & 0x1;
    macsec_active[11] = (active >> 1) & 0x1 ;

    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM0_OR_PM6f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[0] = 0x0;
    else
        macsec_ch_ports[0] = 0x6;

    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM1_OR_PM7f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[1] = 0x1;
    else
        macsec_ch_ports[1] = 0x7;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM2_OR_PM8f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[4] = 0x2;
    else
        macsec_ch_ports[4] = 0x8;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM3_OR_PM9f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[5] = 0x3;
    else
        macsec_ch_ports[5] = 0x9;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM10_OR_PM16f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[7] = 0xa;
    else
        macsec_ch_ports[7] = 0x10;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM11_OR_PM17f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[6] = 0xb;
    else
        macsec_ch_ports[6] = 0x11;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM12_OR_PM18f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[11] = 0xc;
    else
        macsec_ch_ports[11] = 0x12;
    active = TOP_MACSEC_CTRL0r_MACSEC_SRC_PM13_OR_PM19f_GET(result);
    if (active & 0x1)
        macsec_ch_ports[10] = 0xd;
    else
        macsec_ch_ports[10] = 0x13;

  /* for (ix = 0; ix < 12 ; ix++) {  12 macsec channels
  * macsec_active[ix] = active & 0x1;
  * active = active>>1;
  * }
  */


 /*  for (ix = 0; ix < 4 ; ix++) {  4 channel sel between port_if0 or port_if1
 *    sel = ch_prt_num & 0x1;
 *    ch_prt_num = ch_prt_num >> 1;
 *    if (sel)
 *        macsec_ch_ports[ix] = ix;
 *    else
 *        macsec_ch_ports[ix] = (ix+6);
 *  }
 *
 *  ch_prt_num = ch_prt_num >> 2 ;   2 idle bits in the register
 *
 *  for (ix = 10; ix < 14 ; ix++) {  4 channel sel between port_if0 or port_if1
 *    sel = ch_prt_num & 0x1;
 *    ch_prt_num = ch_prt_num >> 1;
 *    if (sel)
 *        macsec_ch_ports[ix-4] = ix;
 *    else
 *        macsec_ch_ports[ix-4] = (ix+6);
 *  }
 */
  /*  port mode derivation for both the chanels */

    for (ix = 0; ix < 6 ; ix++) { /* 12 macsec channels 6 instances */
     /*   if (ix == 1) {
     *       mac_sec_num = 2;
     *   } else if (ix == 2) {
     *       mac_sec_num = 1;
     *   }  else if (ix == 4) {
     *       mac_sec_num = 5;
     *   } else if (ix == 5) {
     *       mac_sec_num = 4;
     *   } else {
     *       mac_sec_num = ix;
     *   }
     */

         if (macsec_active[(ix*2)] || macsec_active[(ix*2)+1]) { /* if both channels not enabled skip programming */
             bcm56780_a0_get_port_mode(unit , macsec_ch_ports[(ix*2)], &u0_port_mode0 , &u0_port_mode1,macsec_drv_info);
             bcm56780_a0_get_port_mode(unit , macsec_ch_ports[(ix*2)+1], &u1_port_mode0 , &u1_port_mode1, macsec_drv_info);
             SHR_IF_ERR_EXIT
                (tdm_gen(unit,u0_port_mode0 ,u0_port_mode1 ,u1_port_mode0 ,u1_port_mode1 , ix));
         }
   }
exit:
    SHR_FUNC_EXIT();
}

static int
tdm_gen(int unit ,int bh0_l,int bh0_h,int bh1_l,int bh1_h, int inst)
{
    static uint32_t tdm_arr[33];
    uint32_t tdm_arr_0[18],tdm_arr_1[18];
    int i,j,k,l;
//    int ioerr = 0;
    bcmsec_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t     ltmbuf[2] = {0};
    uint32_t     ltmbuff = 0;
    uint32_t wrap_ptr[1] = {32};
    uint64_t flags = 0;

//    MACSEC_TDM_WRAP_PTRr_t wrap_ptr;

    SHR_FUNC_ENTER(unit);
    for (i=0;i<18;i++){
         tdm_arr_0[i] = 0;
         tdm_arr_1[i] = 0;
    }
    if (bh0_l == 12){  /* single_400g */
        for (i=0;i<8;i++){
             tdm_arr_0[i] = 0;
        }
    }
    else {
    switch (bh0_l) {
       case 4 :        /* single */
           tdm_arr_0[0] = 0;
           tdm_arr_0[2] = 0;
           tdm_arr_0[4] = 0;
           tdm_arr_0[6] = 0;
           break;

       case 3 :      /* dual */
           tdm_arr_0[0] = 0;
           tdm_arr_0[2] = 2;
           tdm_arr_0[4] = 0;
           tdm_arr_0[6] = 2;
           break;

       case 2 :     /* tri2 */
           tdm_arr_0[0] = 0;
           tdm_arr_0[2] = 2;
           tdm_arr_0[4] = 0;
           tdm_arr_0[6] = 3;
           break;

       case 1 :      /* tri1 */
           tdm_arr_0[0] = 0;
           tdm_arr_0[2] = 2;
           tdm_arr_0[4] = 1;
           tdm_arr_0[6] = 2;
           break;

       case 0 :       /* quad */
           tdm_arr_0[0] = 0;
           tdm_arr_0[2] = 2;
           tdm_arr_0[4] = 1;
           tdm_arr_0[6] = 3;
           break;

       default :
              LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Wrong bh0_port_l value %0d\n"),bh0_l ));
     }
    switch (bh0_h){
    case 4 :      /* single */
           tdm_arr_0[1] = 4;
           tdm_arr_0[3] = 4;
           tdm_arr_0[5] = 4;
           tdm_arr_0[7] = 4;
           break;

    case 3 :       /* dual */
           tdm_arr_0[1] = 4;
           tdm_arr_0[3] = 6;
           tdm_arr_0[5] = 4;
           tdm_arr_0[7] = 6;
           break;

    case 2 :      /* tri2 */
           tdm_arr_0[1] = 4;
           tdm_arr_0[3] = 6;
           tdm_arr_0[5] = 4;
           tdm_arr_0[7] = 7;
           break;

    case 1 :       /* tri1 */
           tdm_arr_0[1] = 4;
           tdm_arr_0[3] = 6;
           tdm_arr_0[5] = 5;
           tdm_arr_0[7] = 6;
           break;

    case 0 :       /* quad */
           tdm_arr_0[1] = 4;
           tdm_arr_0[3] = 6;
           tdm_arr_0[5] = 5;
           tdm_arr_0[7] = 7;
           break;

    default :;
              LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Wrong bh0_port_h value %0d\n"),bh0_h ));
    }
   } /* else */

   if (bh1_l == 12){   /* single_400g */
       for (i=0;i<8;i++){
           tdm_arr_1[i] = 8;
       }
   }
   else {
    switch (bh1_l) {
       case 4 :     /* single */
              tdm_arr_1[0] = 8;
              tdm_arr_1[2] = 8;
              tdm_arr_1[4] = 8;
              tdm_arr_1[6] = 8;
              break;

       case 3 :     /* dual */
              tdm_arr_1[0] = 8;
              tdm_arr_1[2] = 10;
              tdm_arr_1[4] = 8;
              tdm_arr_1[6] = 10;
              break;

       case 2 :     /* tri2 */
           tdm_arr_1[0] = 8;
           tdm_arr_1[2] = 10;
           tdm_arr_1[4] = 8;
           tdm_arr_1[6] = 11;
           break;

       case 1 :       /* tri1 */
           tdm_arr_1[0] = 8;
           tdm_arr_1[2] = 10;
           tdm_arr_1[4] = 9;
           tdm_arr_1[6] = 10;
           break;

       case 0 :       /* quad */
           tdm_arr_1[0] = 8;
           tdm_arr_1[2] = 10;
           tdm_arr_1[4] = 9;
           tdm_arr_1[6] = 11;
           break;

       default :
              LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Wrong bh0_port_l value %0d\n"),bh1_l ));
     }
     switch (bh1_h){
       case 4 :         /* single */
           tdm_arr_1[1] = 12;
           tdm_arr_1[3] = 12;
           tdm_arr_1[5] = 12;
           tdm_arr_1[7] = 12;
           break;

       case 3 :        /* dual */
           tdm_arr_1[1] = 12;
           tdm_arr_1[3] = 14;
           tdm_arr_1[5] = 12;
           tdm_arr_1[7] = 14;
           break;

       case 2 :        /* tri2 */
           tdm_arr_1[1] = 12;
           tdm_arr_1[3] = 14;
           tdm_arr_1[5] = 12;
           tdm_arr_1[7] = 15;
           break;

       case 1 :        /* tri1 */
           tdm_arr_1[1] = 12;
           tdm_arr_1[3] = 14;
           tdm_arr_1[5] = 13;
           tdm_arr_1[7] = 14;
           break;

       case 0 :      /* quad  */
           tdm_arr_1[1] = 12;
           tdm_arr_1[3] = 14;
           tdm_arr_1[5] = 13;
           tdm_arr_1[7] = 15;
           break;

       default :
              LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Wrong bh0_port_l value %0d\n"),bh1_h));
     }
    } /* else */

    j = 0;
    for (k=0;k<8;k++){  /* arrange tdm with one tdm of BH followed by another BH  */
         tdm_arr[j]   = tdm_arr_0[k];
         tdm_arr[j+1] = tdm_arr_1[k];
         j = j+2;
    }
    for (l=0;l<16;l++){
         tdm_arr[l+16] = tdm_arr[l];
    }
    tdm_arr[32] = 17; /* sbus slot */
   /* calendar is single array of 34 locations */
  /* write consolidated array */

 /*  if (td4_pm_is_macsec_valid(unit, inst) != 0) {
 *            printf("inst %d invalid or not mapped\n", inst);
 *            goto exit;
 *        }

 *       rv = td4_pm_to_macsec_port(unit, inst, 0, &macsec_port);
 *        if (rv != SHR_E_NONE) {
 *            printf("inst %d to macsec port error\n", inst);
 *            goto exit;
 *        }
 */
 /*	ioerr += READ_MACSEC_TDM_WRAP_PTRr(unit,&wrap_ptr,inst);
 *	MACSEC_TDM_WRAP_PTRr_SET(wrap_ptr,32);
 *        ioerr += WRITE_MACSEC_TDM_WRAP_PTRr(unit,wrap_ptr,inst);
 */
         BCMSEC_PT_DYN_INFO(pt_info, 0, inst, flags);
         SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, MACSEC_TDM_WRAP_PTRr, TDM_WRAP_PTRf,
                              &ltmbuff, wrap_ptr));
         SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_write(unit, MACSEC_TDM_WRAP_PTRr, ltid,
                                     &pt_info, &ltmbuff));




    for (l=0;l<34;l++){
         BCMSEC_PT_DYN_INFO(pt_info, l, inst, flags);
         SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, MACSEC_TDM_CALENDARm, PORTIDf,
                              ltmbuf, &tdm_arr[l]));
         SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_write(unit, MACSEC_TDM_CALENDARm, ltid,
                                     &pt_info, ltmbuf));
   }

exit:
    SHR_FUNC_EXIT();
}

/* portmode generation based on per pm */

static void
bcm56780_a0_get_port_mode(int unit , int pm_num, int *u0_port_mode , int *u1_port_mode, macsec_drv_info_t *macsec_drv_info)
{
/* bcmsec_drv_info_t *drv_info; */
    int num_lanes[8];
    int phy_port_base, subport;
    int lport;
    int phy_port;

 /*  SHR_FUNC_ENTER(unit); */
 /*  bcmsec_drv_info_get(unit, &drv_info); */
    phy_port_base = 1 + (pm_num * 8);

    for (phy_port = phy_port_base; phy_port < (phy_port_base + 8 ); phy_port++)
    {
        subport = (phy_port - 1) % 8 ;
        lport = macsec_drv_info->port_p2l_mapping[phy_port];
        num_lanes[subport] = macsec_drv_info->port_num_lanes[lport];
    }

    *u0_port_mode = 0;
    *u1_port_mode = 0;

    if (num_lanes[0] == 8)
    {
       *u0_port_mode = *u0_port_mode | 0xC;
       *u1_port_mode = *u1_port_mode | 0x8;
    }
    if (num_lanes[0] == 4) {
       *u0_port_mode = *u0_port_mode | 0x4;
    }
    if (num_lanes[0] == 2) {
       *u0_port_mode = *u0_port_mode | 0x2;
    }
    if (num_lanes[2] == 2) {
       *u0_port_mode = *u0_port_mode | 0x1;
    }

    if (num_lanes[4] == 4) {
       *u1_port_mode = *u1_port_mode | 0x4;
    }
    if (num_lanes[4] == 2) {
      *u1_port_mode = *u1_port_mode | 0x2;
    }
    if (num_lanes[6] == 2) {
       *u1_port_mode = *u1_port_mode | 0x1;
   }
}

int
bcm56780_a0_sec_tdm_init(int unit)
{
    macsec_drv_info_t *macsec_drv_info = NULL;
    size_t size;

    SHR_FUNC_ENTER(unit);
    size = sizeof(macsec_drv_info_t);
    SHR_ALLOC(macsec_drv_info , size, "bcmsecTdmDrvInfo");
    SHR_NULL_CHECK(macsec_drv_info, SHR_E_MEMORY);
    sal_memset(macsec_drv_info, 0, size);

    bcm56780_a0_mac_drv_info_populate(unit,macsec_drv_info);
    SHR_IF_ERR_EXIT
        (bcm56780_a0_mac_sec_tdm_gen(unit,macsec_drv_info));

exit:
    if (macsec_drv_info) {
        SHR_FREE(macsec_drv_info);
    }
    SHR_FUNC_EXIT();
}
