/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * soc_cint_data.c
 *
 * Hand-coded support for a few SAL core routines.
 */
typedef int soc_core_cint_data_not_empty; /* Make ISO compilers happy. */
#include <sdk_config.h>

#if defined(INCLUDE_LIB_CINT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <soc/property.h>
#include <soc/drv.h>
#include <soc/intr.h>
#include <soc/i2c.h>
#include <soc/cmic.h>
#include <soc/sbusdma.h>
#include <shared/util.h>
#include <shared/bitop.h>
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#endif

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/DRC/drc_combo28_init.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>
#include <soc/dpp/DRC/drc_combo28.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/init/init_mem.h>
#endif
/* cmicx diag additions */

static cint_parameter_desc_t __cint_parameters__soc_sbd_dm_cb_f[] =
{
    {
        "void",
        "r",
        0,
        0
    },
    {
        "int",
        "unit",
        0,
        0
    },
    {
        "int",
        "status",
        0,
        0
    },
    {
        "sbusdma_desc_handle_t",
        "handle",
        0,
        0
    },
    {
        "void",
        "data",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_parameters__isr_chain_done_t[] =
{
    {
        "void",
        "r",
        0,
        0
    },
    {
        "int",
        "i",
        0,
        0
    },
    {
        "dv_t*",
        "dv",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_parameters__isr_reload_done_t[] =
{
    {
        "void",
        "r",
        0,
        0
    },
    {
        "int",
        "i",
        0,
        0
    },
    {
        "dv_t*",
        "dv",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_parameters__isr_desc_done_t[] =
{
    {
        "void",
        "r",
        0,
        0
    },
    {
        "int",
        "i",
        0,
        0
    },
    {
        "dv_t*",
        "dv",
        1,
        0
    },
    {
        "dcb_t*",
        "dcb",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static void
__cint_fpointer__soc_sbd_dm_cb_f(int unit,
                                 int status,
                                 sbusdma_desc_handle_t handle,
                                 void *data);

static void
__cint_fpointer__isr_chain_done_t(int i, dv_t *dv);

static void
__cint_fpointer__isr_reload_done_t(int i, dv_t *dv);

static void
__cint_fpointer__isr_desc_done_t(int i,dv_t *dv,dcb_t * dcb);

/* end cmicx diag additions */


static cint_function_pointer_t __cint_soc_function_pointers[] =
{

    /* cmicx diag additions */

    {
        "soc_sbd_dm_cb_f",
        (cint_fpointer_t) __cint_fpointer__soc_sbd_dm_cb_f,
        __cint_parameters__soc_sbd_dm_cb_f
    },
    {
        "isr_chain_done_t",
        (cint_fpointer_t) __cint_fpointer__isr_chain_done_t,
        __cint_parameters__isr_chain_done_t
    },
    {
        "isr_reload_done_t",
        (cint_fpointer_t) __cint_fpointer__isr_reload_done_t,
        __cint_parameters__isr_reload_done_t
    },
    {
        "isr_desc_done_t",
        (cint_fpointer_t) __cint_fpointer__isr_desc_done_t,
        __cint_parameters__isr_desc_done_t
    },

    /* end cmicx diag additions */

    CINT_ENTRY_LAST
};

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_init, \
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP2(char*,char,1,0,
                         soc_property_get_str,
                         int,int,unit,0,0,
                         char*,char,name,1,0);

CINT_FWRAPPER_CREATE_RP3(void*, void, 1, 0,
                         soc_cm_salloc,
                         int,int,unit,0,0,
                         int,int,size,0,0,
                         void*,void,name,1,0);

CINT_FWRAPPER_CREATE_VP2(soc_cm_sfree,
                         int,int,unit,0,0,
                         void*,void,ptr,1,0);

CINT_FWRAPPER_CREATE_RP3(uint32,uint32,0,0,
                         soc_property_get,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         uint32,uint32,def,0,0);
CINT_FWRAPPER_CREATE_RP3(pbmp_t,pbmp_t,0,0,
                         soc_property_get_pbmp,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         int,int,defneg,0,0);

CINT_FWRAPPER_CREATE_RP3(pbmp_t,pbmp_t,0,0,
                         soc_property_get_pbmp_default,
                         int,int,unit,0,0,
                         char*,char,name,1,0,
                         pbmp_t,pbmp_t,def,0,0);

CINT_FWRAPPER_CREATE_RP3(char*,char,1,0,
                         soc_property_port_get_str,
                         int,int,unit,0,0,
                         int,int,port,0,0,
                         char*,char,name,1,0);

CINT_FWRAPPER_CREATE_RP4(uint32,uint32,0,0,
                         soc_property_port_get,
                         int,int,unit,0,0,
                         int,int,port,0,0,
                         char*,char,name,1,0,
                         uint32,uint32,def,0,0);

CINT_FWRAPPER_CREATE_RP5(uint32,uint32,0,0,
                         soc_property_suffix_num_get,
                         int,int,unit,0,0,
                         int,int,tc,0,0,
                         char*,char,name,1,0,
                         char*,char,suffix,1,0,
                         uint32,uint32,def,0,0);

CINT_FWRAPPER_CREATE_RP4(uint32,uint32,0,0,
                         soc_property_cos_get,
                         int,int,unit,0,0,
                         soc_cos_t,soc_cos_t,cos,0,0,
                         char*,char,name,1,0,
                         uint32,uint32,def,0,0);

#if defined(BCM_ESW_SUPPORT)
/* additions for cmicx diagnostic tests */

/* for schan and sbus tests */
CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_mem_read_range,
                         int,int,unit,0,0,
                         int,int,mem,0,0,   /* actually soc_mem_t */
                         int,int,copyno,0,0,
                         int,int,index_min,0,0,
                         int,int,index_max,0,0,
                         void *,void,buffer,1,0);

CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_mem_write_range,
                         int,int,unit,0,0,
                         int,int,mem,0,0,   /* actually soc_mem_t */
                         int,int,copyno,0,0,
                         int,int,index_min,0,0,
                         int,int,index_max,0,0,
                         void *,void,buffer,1,0);

CINT_FWRAPPER_CREATE_RP6(unsigned int,unsigned int,0,0,   /* actually uint32 */
                         soc_mem_addr_get,
                         int,int,unit,0,0,
                         int,int,mem,0,0,   /* actually soc_mem_t */
                         int,int,array_index,0,0,   /* actually unsigned */
                         int,int,block,0,0,   /* actually soc_block_t */
                         int,int,index,0,0,
                         uint8 *,uint8,acc_type,1,0);

#if defined(BCM_ESW_SUPPORT) || \
    defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#ifdef BCM_SBUSDMA_SUPPORT

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_sbusdma_desc_create,
                         int,int,unit,0,0,
                         soc_sbusdma_desc_ctrl_t *,soc_sbusdma_desc_ctrl_t,ctrl,1,0,
                         soc_sbusdma_desc_cfg_t *,soc_sbusdma_desc_cfg_t,cfg,1,0,
                         int,int, alloc_memory,0,0,
                         sbusdma_desc_handle_t *,sbusdma_desc_handle_t,desc_handle,1,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_sbusdma_desc_run,
                         int,int,unit,0,0,
                         sbusdma_desc_handle_t,sbusdma_desc_handle_t,desc_handle,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_sbusdma_desc_delete,
                         int,int,unit,0,0,
                         sbusdma_desc_handle_t,sbusdma_desc_handle_t,handle,0,0);
#endif
#endif
CINT_FWRAPPER_CREATE_VP9(soc_schan_header_cmd_set,
                         int,int,unit,0,0,
                         void*,void,header,1,0,   /* should actually be schan_header_t* */
                         int,int,opcode,0,0,
                         int,int,dst_blk,0,0,
                         int,int,src_blk,0,0,
                         int,int,acc_type,0,0,
                         int,int,data_byte_len,0,0,
                         int,int,dma,0,0,
                         uint32,uint32,bank_ignore_mask,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_schan_op,
                         int,int,unit,0,0,
                         void*,void,msg,1,0,   /* should actually be schan_msg_t* */
                         int,int,dwc_write,0,0,
                         int,int,dwc_read,0,0,
                         uint32,uint32,flags,0,0);

CINT_FWRAPPER_CREATE_VP8(soc_schan_header_status_get,
                         int,int,unit,0,0,
                         void*,void,header,1,0,   /* should actually be schan_header_t* */
                         int *,int,opcode,1,0,
                         int *,int,src_blk,1,0,
                         int *,int,data_byte_len,1,0,
                         int *,int,err,1,0,
                         int *,int,ecode,1,0,
                         int *,int,nack,1,0);

CINT_FWRAPPER_CREATE_VP3(soc_schan_dump,
                         int,int,unit,0,0,
                         schan_msg_t *,schan_msg_t,msg,1,0,
                         int,int,dwc,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_attach,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_detach,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_mem_clear,
                         int,int,unit,0,0,
                         soc_mem_t,soc_mem_t,mem,0,0,
                         int,int,copyno,0,0,
                         int,int,force_all,0,0);

int MACRO_soc_mem_entry_words(int unit,int mem) {
    return soc_mem_entry_words(unit,mem);
}
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         MACRO_soc_mem_entry_words,
                         int,int,unit,0,0,
                         int,int,mem,0,0);

int MACRO_SOC_MEM_BLOCK_ANY(int unit,int mem) {
    return SOC_MEM_BLOCK_ANY(unit,mem);
}
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         MACRO_SOC_MEM_BLOCK_ANY,
                         int,int,unit,0,0,
                         int,int,mem,0,0);

/* for packet dma tests */
#ifdef SOC_PCI_DEBUG
CINT_FWRAPPER_CREATE_RP2(uint32,uint32,0,0,
                         soc_pci_read,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_pci_write,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,data,0,0);
#else
uint32 MACRO_soc_pci_read(int unit,uint32 addr) {
    return soc_pci_read(unit,addr);
}
CINT_FWRAPPER_CREATE_RP2(uint32,uint32,0,0,
                         MACRO_soc_pci_read,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0);

int MACRO_soc_pci_write(int unit,uint32 addr,uint32 data) {
    return soc_pci_write(unit,addr,data);
}
CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         MACRO_soc_pci_write,
                         int,int,unit,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,data,0,0);
#endif

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_mem_read,
                         int,int,unit,0,0,
                         soc_mem_t,soc_mem_t,mem,0,0,
                         int,int,copyno,0,0,
                         int,int,index,0,0,
                         void*,void,entry_data,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_mem_write,
                         int,int,unit,0,0,
                         soc_mem_t,soc_mem_t,mem,0,0,
                         int,int,copyno,0,0,
                         int,int,index,0,0,
                         void*,void,entry_data,1,0);

CINT_FWRAPPER_CREATE_VP5(soc_mem_field32_set,
                         int,int,unit,0,0,
                         soc_mem_t,soc_mem_t,mem,0,0,
                         void*,void,entbuf,1,0,
                         soc_field_t,soc_field_t,field,0,0,
                         uint32,uint32,value,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_mem_scan_stop,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_reg_fields32_modify,
                         int,int,unit,0,0,
                         soc_reg_t,soc_reg_t,reg,0,0,
                         soc_port_t,soc_port_t,port,0,0,
                         int,int,field_count,0,0,
                         soc_field_t*,soc_field_t,fields,1,0,
                         uint32*,uint32,values,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_reg_field32_modify,
                         int,int,unit,0,0,
                         soc_reg_t,soc_reg_t,reg,0,0,
                         soc_port_t,soc_port_t,port,0,0,
                         soc_field_t,soc_field_t,field,0,0,
                         uint32,uint32,value,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_reg_get,
                         int,int,unit,0,0,
                         soc_reg_t,soc_reg_t,reg,0,0,
                         int,int,port,0,0,
                         int,int,index,0,0,
                         uint64*,uint64,data,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_reg_set,
                         int,int,unit,0,0,
                         soc_reg_t,soc_reg_t,reg,0,0,
                         int,int,port,0,0,
                         int,int,index,0,0,
                         uint64,uint64,data,0,0);

#if defined(SAL_HAS_ATOMIC_ALLOC)
CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         soc_at_alloc,
                         int,int,dev,0,0,
                         int,int,size,0,0,
                         char*,char,name,1,0);    /* should actually be const char * */
CINT_FWRAPPER_CREATE_VP2(soc_at_free,
                         int,int,dev,0,0,
                         void*,void,ptr,1,0);
#else
void *MACRO_soc_at_alloc(int dev,int size,char *name) {
    return soc_at_alloc(dev,size,name);
}
CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         MACRO_soc_at_alloc,
                         int,int,dev,0,0,
                         int,int,size,0,0,
                         char*,char,name,1,0);    /* should actually be const char * */
void MACRO_soc_at_free(int dev,void *ptr) {
    soc_at_free(dev,ptr);
}
CINT_FWRAPPER_CREATE_VP2(MACRO_soc_at_free,
                         int,int,dev,0,0,
                         void*,void,ptr,1,0);
#endif

CINT_FWRAPPER_CREATE_VP1(soc_linkscan_pause,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_counter_stop,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_reset_init,
                         int,int,unit,0,0);
#ifdef BCM_CMIC_SUPPORT
CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_dma_init,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dma_attach,
                         int,int,unit,0,0,
                         int,int,reset,0,0);

CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         soc_dma_abort,
                         int,int,unit,0,0);

CINT_FWRAPPER_CREATE_RP8(int,int,0,0,
                         soc_dma_desc_add,
                         dv_t*,dv_t,dv,1,0,
                         sal_vaddr_t,sal_vaddr_t,addr,0,0,
                         uint16,uint16,cnt,0,0,
                         pbmp_t,pbmp_t,pbmp,0,0,
                         pbmp_t,pbmp_t,ubmp,0,0,
                         pbmp_t,pbmp_t,l3pbm,0,0,
                         uint32,uint32,flags,0,0,
                         uint32*,uint32,hgh,1,0);

CINT_FWRAPPER_CREATE_VP1(soc_dma_desc_end_packet,
                         dv_t*,dv_t,dv,1,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dma_rld_desc_add,
                         dv_t*,dv_t,dv,1,0,
                         sal_vaddr_t,sal_vaddr_t,addr,0,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_dma_chan_config,
                         int,int,unit,0,0,
                         dma_chan_t,dma_chan_t,chan,0,0,
                         dvt_t,dvt_t,type,0,0,
                         uint32,uint32,flags,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dma_abort_dv,
                         int,int,unit,0,0,
                         dv_t*,dv_t,dv,0,0);

CINT_FWRAPPER_CREATE_VP2(soc_dma_dv_reset,
                         dvt_t,dvt_t,dvt,0,0,
                         dv_t*,dv_t,dv,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dma_start,
                         int,int,unit,0,0,
                         dma_chan_t,dma_chan_t,chan,0,0,
                         dv_t*,dv_t,dv,1,0);

CINT_FWRAPPER_CREATE_VP3(soc_dma_dump_dv,
                         int,int,unit,0,0,
                         char*,char,pfx,1,0,
                         dv_t*,dv_t,dv,1,0);

CINT_FWRAPPER_CREATE_RP3(dv_t*,dv_t,1,0,
                         soc_dma_dv_alloc,
                         int,int,unit,0,0,
                         dvt_t,dvt_t,dvt,0,0,
                         int,int,cnt,0,0);

CINT_FWRAPPER_CREATE_VP2(soc_dma_dv_free,
                         int,int,unit,0,0,
                         dv_t*,dv_t,dv,1,0);
#endif

int MACRO_soc_mem_index_max(int unit,unsigned mem) {
    return soc_mem_index_max(unit,mem);
}
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         MACRO_soc_mem_index_max,
                         int,int,unit,0,0,
                         unsigned,unsigned,mem,0,0);

/* end additions for cmicx diagnostic tests */

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                          soc_link_fwd_set, \
                          int,int,unit,0,0,
                          pbmp_t,pbmp_t,fwd,0,0);

CINT_FWRAPPER_CREATE_VP2(soc_link_fwd_get, \
                         int,int,unit,0,0,
                         pbmp_t*,pbmp_t,fwd,1,0);


#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
#if defined(INCLUDE_I2C)


CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_i2c_read_byte,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8*,uint8,value,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_i2c_write_byte,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,value,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_i2c_read_word,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint16*,uint16,value,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_i2c_write_word,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint16,uint16,value,0,0);


CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_i2c_read_byte_data,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint8*,uint8,value,1,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_i2c_write_byte_data,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint8,uint8,value,0,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_i2c_read_word_data,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint16*,uint16,value,1,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_i2c_write_word_data,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint16,uint16,value,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_i2c_block_read,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint8*,uint8,n,1,0,
                         uint8*,uint8,d,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_i2c_block_write,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint8,uint8,n,0,0,
                         uint8*,uint8,d,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_i2c_multi_write,
                         int,int,unit,0,0,
                         uint8,uint8,saddr,0,0,
                         uint8,uint8,com,0,0,
                         uint8,uint8,n,0,0,
                         uint8*,uint8,d,1,0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_i2c_probe, \
                         int,int,unit,0,0);
#endif
CINT_FWRAPPER_CREATE_RP1(uint16, uint16, 0, 0,
                         _shr_swap16, \
                         uint16,uint16,val,0,0);
#endif

#ifdef BCM_DNX_SUPPORT
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         dnx_test_dynamic_memory_access, \
                         int,int,unit,0,0);
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_reg_bits,
                         int,int,unit,0,0,
                         int,int,reg,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_reg_field_length,
                         int,int,unit,0,0,
                         int,int,reg,0,0,
                         int,int,field,0,0);
CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_active_interrupts_get,
                         int,int,unit,0,0,
                         int,int,flags,0,0,
                         int,int,max_interrupts_size,0,0,
                         soc_interrupt_cause_t*,soc_interrupt_cause_t,interrupts,1,0,
                         int*,int,total_interrupts,1,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_nof_interrupts,
                         int,int,unit,0,0,
                         int*,int,nof_interrupts,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_interrupt_info_get,
                         int,int,unit,0,0,
                         int,int,interrupt_id,0,0,
                         soc_interrupt_db_t*,soc_interrupt_db_t,inter,1,0);

CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_get_interrupt_id,
                         int,int,unit,0,0,
                         int,int,reg,0,0,
                         int,int,reg_index,0,0,
                         int,int,field,0,0,
                         int,int,bit_in_field,0,0,
                         int*,int,interrupt_id,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_get_interrupt_id_specific,
                         int,int,unit,0,0,
                         int,int,reg_adress,0,0,
                         int,int,reg_block,0,0,
                         int,int,field_bit,0,0,
                         int*,int,interrupt_id,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_interrupt_is_supported,
                         int,int,unit,0,0,
                         int,int,block_instance,0,0,
                         int,int,inter_id,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_interrupt_has_func_arr,
                         int,int,unit,0,0,
                         int,int,interrupt_id,0,0);

CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_interrupt_event_cb_none,
                         int,int,unit,0,0,
                         soc_switch_event_t,soc_switch_event_t,event,0,0,
                         uint32,uint32,arg1,0,0,
                         uint32,uint32,arg2,0,0,
                         uint32,uint32,arg3,0,0,
                         void*,void,data,0,0);

#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)

CINT_FWRAPPER_CREATE_RP3(uint32,uint32,0,0,
                         arad_kbp_cpu_lookup_reply,
                         uint32,uint32,core,0,0,
                         int,int,unit,0,0,
                         uint32*,uint32,data,1,0);

CINT_FWRAPPER_CREATE_RP5(uint32,uint32,0,0,
                         arad_kbp_cpu_record_send,
                         int,int,unit,0,0,
                         uint32,uint32,opcode,0,0,
                         uint32*,uint32,msb_data,1,0,
                         uint32*,uint32,lsb_data,1,0,
                         int,int,lsb_enable,0,0);
CINT_FWRAPPER_CREATE_RP7(uint32,uint32,0,0,
                         aradplus_kbp_cpu_record_send,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         uint32,uint32,opcode,0,0,
                         uint32*,uint32,msb_data,1,0,
                         uint32*,uint32,lsb_data,1,0,
                         int,int,lsb_enable,0,0,
                         soc_reg_above_64_val_t,soc_reg_above_64_val_t,read_data,0,0);

CINT_FWRAPPER_CREATE_RP5(uint32,uint32,0,0,
                         arad_kbp_lut_write,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         uint8,uint8,addr,0,0,
                         arad_kbp_lut_data_t*,arad_kbp_lut_data_t,lut_data,1,0,
                         uint32*,uint32,lut_data_row,1,0);

CINT_FWRAPPER_CREATE_RP5(uint32,uint32,0,0,
                         arad_kbp_lut_read,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         uint8,uint8,addr,0,0,
                         arad_kbp_lut_data_t*,arad_kbp_lut_data_t,lut_data,1,0,
                         uint32*,uint32,lut_data_row,1,0);

CINT_FWRAPPER_CREATE_RP3(uint32,uint32,0,0,
                         arad_kbp_rop_write,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         arad_kbp_rop_write_t*,arad_kbp_rop_write_t,wr_data,1,0);

CINT_FWRAPPER_CREATE_RP3(uint32,uint32,0,0,
                         arad_kbp_rop_read,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         arad_kbp_rop_read_t*,arad_kbp_rop_read_t,rd_data,1,0);

CINT_FWRAPPER_CREATE_RP4(uint32,uint32,0,0,
                         arad_kbp_rop_ad_write,
                         int,int,unit,0,0,
                         uint32,uint32,core,0,0,
                         uint8*,uint8,wr_data,1,0,
                         uint16,uint16,length,0,0);

#endif /* #if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) */

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_port_phy_pll_os_set,
                         int,int,unit,0,0,
                         int,int,port,0,0,
                         uint32,uint32,vco_freq,0,0,
                         uint32,uint32,oversample_mode,0,0,
                         uint32,uint32,pll_divider,0,0);

#if defined(BCM_PETRA_SUPPORT)
/* 
 * Dram user access access functions
 */
CINT_FWRAPPER_CREATE_RP4(uint32,uint32,0,0,
                         arad_dram_logical2physical_addr_mapping,
                         int,int,unit,0,0,
                         int,int,buf_num,0,0,
                         int,int,index,0,0,
                         uint32*,uint32,phy_addr,1,0);

CINT_FWRAPPER_CREATE_RP6(int,int,0,0,
                         soc_arad_user_buffer_dram_access,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         uint32,uint32,access_type,0,0,
                         uint8*,uint8,buf,1,0,
                         int,int,offset,0,0,
                         int,int,nbytes,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_arad_user_buffer_dram_read,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         uint8*,uint8,buf,1,0,
                         int,int,offset,0,0,
                         int,int,nbytes,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_arad_user_buffer_dram_write,
                         int,int,unit,0,0,
                         uint32,uint32,flags,0,0,
                         uint8*,uint8,buf,1,0,
                         int,int,offset,0,0,
                         int,int,nbytes,0,0);
                         
/*
 * DRC init functions
 */

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_dprc_out_of_reset,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_drc_clam_shell_cfg,
                         int,int,dram_ndx,0,0,
                         int,int,init,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_drc_soft_init,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         int,int,init,0,0);
/*
 * Shmoo library functions 
 */
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_combo28_shmoo_phy_cfg_pll,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_combo28_shmoo_phy_init,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0);

/*
 * CallBack Functions 
 */
CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_phy_reg_read,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         uint32,uint32,addr,0,0,
                         uint32*,uint32,data,1,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_phy_reg_write,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,data,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_phy_reg_modify,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         uint32,uint32,addr,0,0,
                         uint32,uint32,data,0,0,
                         uint32,uint32,mask,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_wait_dram_init_done,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_load_mrs,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         uint32,uint32,mrs_num,0,0,
                         uint32,uint32,mrs_opcode,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_enable_adt,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         int,int,action,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         int,int,action,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_pll_set,
                         int,int,unit,0,0,
                         int,int,dram_ndx,0,0,
                         CONST combo28_drc_pll_t*,combo28_drc_pll_t,pll_info,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint32,uint32,command_parity_lattency,0,0,
                         int,int,use_continious_gddr5_dqs,0,0,
                         int,int,enable,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_mpr_en,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_vendor_info_get,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         combo28_vendor_info_t*,combo28_vendor_info_t,info,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int, int, use_continious_gddr5_dqs,0 ,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_dram_init,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int, int, phase,0 ,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_modify_mrs,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint32, uint32, mr_ndx, 0 ,0,
                         uint32, uint32, data, 0 ,0,
                         uint32, uint32, mask, 0 ,0);

CINT_FWRAPPER_CREATE_RP8(int,int,0,0,
                         soc_dpp_drc_combo28_dram_cpu_command,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint32, uint32, ras_n, 0 ,0,
                         uint32, uint32, cas_n, 0 ,0,
                         uint32, uint32, we_n, 0 ,0,
                         uint32, uint32, act_n, 0 ,0,
                         uint32, uint32, bank, 0 ,0,
                         uint32, uint32, address, 0 ,0);
CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_bist_conf_set,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         combo28_bist_info_t,combo28_bist_info_t,info,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         combo28_bist_err_cnt_t*,combo28_bist_err_cnt_t,info,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         combo28_gddr5_bist_info_t,combo28_gddr5_bist_info_t,info,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         combo28_gddr5_bist_err_cnt_t*,combo28_gddr5_bist_err_cnt_t,info,1,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int*,int, pairs_were_swapped,1,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_precharge_all,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0);

CINT_FWRAPPER_CREATE_RP7(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_write_mpr,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint32, uint32, intial_calib_mpr_addr, 0 ,0,
                         uint32, uint32, mpr_mode, 0 ,0,
                         uint32, uint32, mpr_page, 0 ,0,
                         uint32, uint32, mrs_readout, 0 ,0,
                         int, int, enable_mpr, 0 ,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_mpr_load,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint8*, uint8, mpr_pattern, 1 ,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_enable_wr_crc,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);
                         
CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_enable_rd_crc,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);
                         
CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_enable_wr_dbi,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);

CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_enable_rd_dbi,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0);

CINT_FWRAPPER_CREATE_RP5(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_enable_refresh,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         int,int,enable,0,0,
                         uint32,uint32,new_trefi,0,0,
                         uint32*,uint32,curr_refi,1,0);

CINT_FWRAPPER_CREATE_RP4(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_force_dqs,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0,
                         uint32,uint32,force_dqs_val,0,0,
                         uint32,uint32,force_dqs_oeb,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_soft_reset_drc_without_dram,
                         int,int,unit,0,0,
                         int,int,drc_ndx,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_dram_info_access,
                         int,int,unit,0,0,
                         combo28_shmoo_dram_info_t**,combo28_shmoo_dram_info_t,shmoo_info,0,0);

CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         soc_dpp_drc_combo28_shmoo_vendor_info_access,
                         int,int,unit,0,0,
                         combo28_vendor_info_t**,combo28_vendor_info_t,vendor_info,0,0);

#endif /* BCM_PETRA_SUPPORT */

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_DFE_SUPPORT)|| defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT) */


/* cmicx diags additions */

static void
__cint_fpointer__soc_sbd_dm_cb_f(int unit,
                                 int status,
                                 sbusdma_desc_handle_t handle,
                                 void *data)
{
    cint_interpreter_callback(__cint_soc_function_pointers+0,
                              4,
                              0,
                              &unit,
                              &status,
                              &handle,
                              &data);
}

static void
__cint_fpointer__isr_chain_done_t(int i, dv_t *dv)
{
    cint_interpreter_callback(__cint_soc_function_pointers+1,
                              2,
                              0,
                              &i,
                              &dv);
}

static void
__cint_fpointer__isr_reload_done_t(int i, dv_t *dv)
{
    cint_interpreter_callback(__cint_soc_function_pointers+2,
                              2,
                              0,
                              &i,
                              &dv);
}

static void
__cint_fpointer__isr_desc_done_t(int i, dv_t *dv,dcb_t * dcb)
{
    cint_interpreter_callback(__cint_soc_function_pointers+3,
                              3,
                              0,
                              &i,
                              &dv,
                              &dcb);
}

/* end cmicx diag additions */

/* additions for cmicx diagnostic tests */

/* soc_sbusdma_desc_ctrl_t */
static cint_parameter_desc_t __cint_struct_members__soc_sbusdma_desc_ctrl_t[] =
{

    {"uint32","flags",0,0},
    {"char","name",0,16},
    {"uint32","cfg_count",0,0},
    {"void","hw_desc",1,0},
    {"void","buff",1,0},
    {"soc_sbd_dm_cb_f","cb",0,0},
    {"void","data",1,0},
    CINT_ENTRY_LAST

};
static void*
__cint_maddr__soc_sbusdma_desc_ctrl_t(void* p, int mnum, cint_struct_type_t* parent)
{
    soc_sbusdma_desc_ctrl_t* s = (soc_sbusdma_desc_ctrl_t*) p;

    switch(mnum) {
    case 0: return &(s->flags);
    case 1: return &(s->name);
    case 2: return &(s->cfg_count);
    case 3: return &(s->hw_desc);
    case 4: return &(s->buff);
    case 5: return &(s->cb);
    case 6: return &(s->data);
    default: return NULL;
    }
}

/* soc_sbusdma_desc_cfg_t */
static cint_parameter_desc_t __cint_struct_members__soc_sbusdma_desc_cfg_t[] =
{

    {"int","acc_type",0,0,},
    {"uint32","blk",0,0},
    {"uint32","addr",0,0},
    {"uint32","width",0,0},
    {"uint32","count",0,0},
    {"uint32","addr_shift",0,0},
    {"void","buff",1,0},
    CINT_ENTRY_LAST

};
static void*
__cint_maddr__soc_sbusdma_desc_cfg_t(void* p, int mnum, cint_struct_type_t* parent)
{
    soc_sbusdma_desc_cfg_t* s = (soc_sbusdma_desc_cfg_t*) p;

    switch(mnum) {
    case 0: return &(s->acc_type);
    case 1: return &(s->blk);
    case 2: return &(s->addr);
    case 3: return &(s->width);
    case 4: return &(s->count);
    case 5: return &(s->addr_shift);
    case 6: return &(s->buff);
    default: return NULL;
    }
}

/* dv_t */
static cint_parameter_desc_t __cint_struct_members__dv_t[] =
{

    {"int","dv_unit",0,0,},
    {"int","dv_cnt",0,0},
    {"int","dv_vcnt",0,0},
    {"int","dv_flags",0,0},
    {"void*","dv_dcb",1,0},
    CINT_ENTRY_LAST

};
static void*
__cint_maddr__dv_t(void* p, int mnum, cint_struct_type_t* parent)
{
    dv_t* s = (dv_t*) p;

    switch(mnum) {
    case 0: return &(s->dv_unit);
    case 1: return &(s->dv_cnt);
    case 2: return &(s->dv_vcnt);
    case 3: return &(s->dv_flags);
    case 4: return &(s->dv_dcb);
    default: return NULL;
    }
}

/* end additions for cmicx diagnostic tests */



#define TEST_TCAM

#ifdef BCM_PETRA_SUPPORT
#ifdef TEST_TCAM

#include <soc/dpp/ARAD/arad_tbl_access.h>


#include <bcm_int/dpp/error.h>


static uint32
arad_pp_ihb_tcam_tbl_read_unsafe_lcl(
                                 SOC_SAND_IN   int                              unit,
                                 SOC_SAND_IN   uint32                           bank_ndx,
                                 SOC_SAND_IN   uint32                           nof_entries_in_line,
                                 SOC_SAND_IN   uint32                           entry_offset,
                                 SOC_SAND_OUT  ARAD_PP_IHB_TCAM_BANK_TBL_DATA    *data
                                 )
{
    uint32
        soc_sand_rc;

    soc_sand_rc = arad_pp_ihb_tcam_tbl_read_unsafe(unit, bank_ndx, nof_entries_in_line, entry_offset, data);
    if(SOC_SAND_FAILURE(soc_sand_rc)){
        return -1;
    }
    return 0;
}


static uint32
arad_pp_ihb_tcam_tbl_write_unsafe_lcl(
                                  SOC_SAND_IN   int                              unit,
                                  SOC_SAND_IN   uint32                           bank_ndx,
                                  SOC_SAND_IN   uint32                           nof_entries_in_line,
                                  SOC_SAND_IN   uint32                            entry_offset,
                                  SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA     *data
                                  )
{
    uint32
        soc_sand_rc;

    soc_sand_rc = arad_pp_ihb_tcam_tbl_write_unsafe(unit, bank_ndx, nof_entries_in_line, entry_offset, data);
    if(SOC_SAND_FAILURE(soc_sand_rc)){
        return -1;
    }
    return 0;
}

static uint32
arad_pp_ihb_tcam_tbl_compare_unsafe_lcl(
                                    SOC_SAND_IN   int                                unit,
                                    SOC_SAND_IN   uint32                             bank_ndx,
                                    SOC_SAND_IN   uint32                             nof_entries_in_line,
                                    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA     *compare_data,
                                    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_COMPARE_DATA      *found_data
                                    )
{
    uint32
        soc_sand_rc;

    soc_sand_rc = arad_pp_ihb_tcam_tbl_compare_unsafe(unit, bank_ndx, nof_entries_in_line, compare_data, found_data);
    if(SOC_SAND_FAILURE(soc_sand_rc)){
        return -1;
    }
    return 0;
}


CINT_FWRAPPER_CREATE_RP5(uint32, uint32, 0, 0,
                         arad_pp_ihb_tcam_tbl_read_unsafe_lcl,
                         uint32, uint32, unit, 0, 0,
                         uint32, uint32, bank_ndx, 0, 0,
                         uint32, uint32, nof_entries_in_line, 0, 0,
                         uint32, uint32, entry_offset, 0, 0,
                         ARAD_PP_IHB_TCAM_BANK_TBL_DATA*, ARAD_PP_IHB_TCAM_BANK_TBL_DATA, data, 1, 0);

CINT_FWRAPPER_CREATE_RP5(uint32, uint32, 0, 0,
                         arad_pp_ihb_tcam_tbl_write_unsafe_lcl,
                         uint32, uint32, unit, 0, 0,
                         uint32, uint32, bank_ndx, 0, 0,
                         uint32, uint32, nof_entries_in_line, 0, 0,
                         uint32, uint32, entry_offset, 0, 0,
                         ARAD_PP_IHB_TCAM_BANK_TBL_DATA*, ARAD_PP_IHB_TCAM_BANK_TBL_DATA, data, 1, 0);

CINT_FWRAPPER_CREATE_RP5(uint32, uint32, 0, 0,
                         arad_pp_ihb_tcam_tbl_compare_unsafe_lcl,
                         uint32, uint32, unit, 0, 0,
                         uint32, uint32, bank_ndx, 0, 0,
                         uint32, uint32, nof_entries_in_line, 0, 0,
                         ARAD_PP_IHB_TCAM_BANK_TBL_DATA*, ARAD_PP_IHB_TCAM_BANK_TBL_DATA, compare_data, 1, 0,
                         ARAD_PP_IHB_TCAM_COMPARE_DATA*, ARAD_PP_IHB_TCAM_COMPARE_DATA, found_data, 1, 0);

#endif
#endif /* BCM_PETRA_SUPPORT */

/*
 * bitop macros
 */
static void __SHR_BITSET(uint32* data, int n) { SHR_BITSET(data, n); }
CINT_FWRAPPER_CREATE_VP2(__SHR_BITSET, uint32*,int,data,1,0,int,int,n,0,0); 

static void __SHR_BITCLR(uint32* data, int n) { SHR_BITCLR(data, n); }
CINT_FWRAPPER_CREATE_VP2(__SHR_BITCLR, uint32*,int,data,1,0,int,int,n,0,0); 

static int __SHR_BITGET(uint32* data, int n) {return SHR_BITGET(data, n); }
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0, __SHR_BITGET, uint32*,int,data,1,0,int,int,n,0,0); 


static cint_function_t __cint_soc_functions[] =
    {
        CINT_FWRAPPER_ENTRY(soc_init),
        CINT_FWRAPPER_ENTRY(soc_property_get_str),
        CINT_FWRAPPER_ENTRY(soc_property_get),
        CINT_FWRAPPER_ENTRY(soc_property_get_pbmp),
        CINT_FWRAPPER_ENTRY(soc_property_get_pbmp_default),
        CINT_FWRAPPER_ENTRY(soc_property_port_get_str),
        CINT_FWRAPPER_ENTRY(soc_property_port_get),
        CINT_FWRAPPER_ENTRY(soc_property_suffix_num_get),
        CINT_FWRAPPER_ENTRY(soc_property_cos_get),
        CINT_FWRAPPER_ENTRY(soc_cm_salloc),
        CINT_FWRAPPER_ENTRY(soc_cm_sfree),
#if defined(BCM_ESW_SUPPORT)
        CINT_FWRAPPER_ENTRY(soc_link_fwd_set),
        CINT_FWRAPPER_ENTRY(soc_link_fwd_get),

        /* additions for cmicx diagnostic tests */

        /* for schan and sbus tests */
        CINT_FWRAPPER_ENTRY(soc_mem_read_range),
        CINT_FWRAPPER_ENTRY(soc_mem_write_range),
        CINT_FWRAPPER_ENTRY(soc_mem_addr_get),
#if defined(BCM_ESW_SUPPORT) || \
            defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#ifdef BCM_SBUSDMA_SUPPORT
        CINT_FWRAPPER_ENTRY(soc_sbusdma_desc_create),
        CINT_FWRAPPER_ENTRY(soc_sbusdma_desc_run),
        CINT_FWRAPPER_ENTRY(soc_sbusdma_desc_delete),
#endif
#endif
        CINT_FWRAPPER_ENTRY(soc_schan_header_cmd_set),
        CINT_FWRAPPER_ENTRY(soc_schan_op),
        CINT_FWRAPPER_ENTRY(soc_schan_header_status_get),
        CINT_FWRAPPER_ENTRY(soc_schan_dump),
        CINT_FWRAPPER_ENTRY(soc_attach),
        CINT_FWRAPPER_ENTRY(soc_detach),
        CINT_FWRAPPER_ENTRY(soc_mem_clear),
        CINT_FWRAPPER_ENTRY(MACRO_soc_mem_entry_words),
        CINT_FWRAPPER_ENTRY(MACRO_SOC_MEM_BLOCK_ANY),

        /* for packet dma tests */
#ifdef SOC_PCI_DEBUG
        CINT_FWRAPPER_ENTRY(soc_pci_read),
        CINT_FWRAPPER_ENTRY(soc_pci_write),
#else
        CINT_FWRAPPER_ENTRY(MACRO_soc_pci_read),
        CINT_FWRAPPER_ENTRY(MACRO_soc_pci_write),
#endif
        CINT_FWRAPPER_ENTRY(soc_mem_read),
        CINT_FWRAPPER_ENTRY(soc_mem_write),
        CINT_FWRAPPER_ENTRY(soc_mem_field32_set),
        CINT_FWRAPPER_ENTRY(soc_mem_scan_stop),
        CINT_FWRAPPER_ENTRY(soc_reg_fields32_modify),
        CINT_FWRAPPER_ENTRY(soc_reg_field32_modify),
        CINT_FWRAPPER_ENTRY(soc_reg_get),
        CINT_FWRAPPER_ENTRY(soc_reg_set),
#if defined(SAL_HAS_ATOMIC_ALLOC)
        CINT_FWRAPPER_ENTRY(soc_at_alloc),
        CINT_FWRAPPER_ENTRY(soc_at_free),
#else
        CINT_FWRAPPER_ENTRY(MACRO_soc_at_alloc),
        CINT_FWRAPPER_ENTRY(MACRO_soc_at_free),
#endif
        CINT_FWRAPPER_ENTRY(soc_linkscan_pause),
        CINT_FWRAPPER_ENTRY(soc_counter_stop),
        CINT_FWRAPPER_ENTRY(soc_reset_init),
#ifdef BCM_CMIC_SUPPORT
        CINT_FWRAPPER_ENTRY(soc_dma_init),
        CINT_FWRAPPER_ENTRY(soc_dma_attach),
        CINT_FWRAPPER_ENTRY(soc_dma_abort),
        CINT_FWRAPPER_ENTRY(soc_dma_desc_add),
        CINT_FWRAPPER_ENTRY(soc_dma_desc_end_packet),
        CINT_FWRAPPER_ENTRY(soc_dma_rld_desc_add),
        CINT_FWRAPPER_ENTRY(soc_dma_chan_config),
        CINT_FWRAPPER_ENTRY(soc_dma_abort_dv),
        CINT_FWRAPPER_ENTRY(soc_dma_dv_reset),
        CINT_FWRAPPER_ENTRY(soc_dma_start),
        CINT_FWRAPPER_ENTRY(soc_dma_dump_dv),
        CINT_FWRAPPER_ENTRY(soc_dma_dv_alloc),
        CINT_FWRAPPER_ENTRY(soc_dma_dv_free),
#endif
        CINT_FWRAPPER_ENTRY(MACRO_soc_mem_index_max),

        /* end additions for cmicx diagnostic tests */
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
#if defined(INCLUDE_I2C)

        CINT_FWRAPPER_ENTRY(soc_i2c_read_byte),
        CINT_FWRAPPER_ENTRY(soc_i2c_write_byte),
        CINT_FWRAPPER_ENTRY(soc_i2c_read_byte_data),
        CINT_FWRAPPER_ENTRY(soc_i2c_write_byte_data),
        CINT_FWRAPPER_ENTRY(soc_i2c_read_word),
        CINT_FWRAPPER_ENTRY(soc_i2c_write_word),
        CINT_FWRAPPER_ENTRY(soc_i2c_read_word_data),
        CINT_FWRAPPER_ENTRY(soc_i2c_write_word_data),
        CINT_FWRAPPER_ENTRY(soc_i2c_block_read),
        CINT_FWRAPPER_ENTRY(soc_i2c_block_write),
        CINT_FWRAPPER_ENTRY(soc_i2c_multi_write),
        CINT_FWRAPPER_ENTRY(soc_i2c_probe),
#endif
        CINT_FWRAPPER_ENTRY(_shr_swap16),
#endif
#ifdef BCM_DNX_SUPPORT
        CINT_FWRAPPER_ENTRY(dnx_test_dynamic_memory_access),
#endif
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
        CINT_FWRAPPER_ENTRY(soc_reg_bits),
        CINT_FWRAPPER_ENTRY(soc_reg_field_length),
        CINT_FWRAPPER_ENTRY(soc_nof_interrupts),
        CINT_FWRAPPER_ENTRY(soc_active_interrupts_get),
        CINT_FWRAPPER_ENTRY(soc_interrupt_info_get),
        CINT_FWRAPPER_ENTRY(soc_get_interrupt_id),
        CINT_FWRAPPER_ENTRY(soc_get_interrupt_id_specific),
        CINT_FWRAPPER_ENTRY(soc_interrupt_is_supported),
        CINT_FWRAPPER_ENTRY(soc_interrupt_has_func_arr),
        CINT_FWRAPPER_ENTRY(soc_interrupt_event_cb_none),
        CINT_FWRAPPER_ENTRY(soc_port_phy_pll_os_set),
#ifdef BCM_PETRA_SUPPORT
        CINT_FWRAPPER_ENTRY(arad_dram_logical2physical_addr_mapping),
        CINT_FWRAPPER_ENTRY(soc_arad_user_buffer_dram_access),
        CINT_FWRAPPER_ENTRY(soc_arad_user_buffer_dram_read),
        CINT_FWRAPPER_ENTRY(soc_arad_user_buffer_dram_write),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_dprc_out_of_reset),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_drc_clam_shell_cfg),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_drc_soft_init),
        CINT_FWRAPPER_ENTRY(soc_combo28_shmoo_phy_cfg_pll),
        CINT_FWRAPPER_ENTRY(soc_combo28_shmoo_phy_init),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_phy_reg_read),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_phy_reg_write),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_phy_reg_modify),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_wait_dram_init_done),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_load_mrs),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_enable_adt),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_pll_set),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_mpr_en),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_vendor_info_get),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_dram_init),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_modify_mrs),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_dram_cpu_command),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_vendor_info_get),  
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_bist_conf_set),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt), 
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_precharge_all),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_write_mpr),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_mpr_load),  
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_enable_wr_crc),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_enable_rd_crc),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_enable_wr_dbi),          
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_enable_rd_dbi),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_enable_refresh),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_force_dqs),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_soft_reset_drc_without_dram),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_dram_info_access),
        CINT_FWRAPPER_ENTRY(soc_dpp_drc_combo28_shmoo_vendor_info_access),

#endif
#endif
#ifdef BCM_PETRA_SUPPORT
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        CINT_FWRAPPER_ENTRY(arad_kbp_cpu_lookup_reply),
        CINT_FWRAPPER_ENTRY(arad_kbp_cpu_record_send),
        CINT_FWRAPPER_ENTRY(aradplus_kbp_cpu_record_send),
        CINT_FWRAPPER_ENTRY(arad_kbp_lut_write),
        CINT_FWRAPPER_ENTRY(arad_kbp_lut_read),
        CINT_FWRAPPER_ENTRY(arad_kbp_rop_write),
        CINT_FWRAPPER_ENTRY(arad_kbp_rop_read),
        CINT_FWRAPPER_ENTRY(arad_kbp_rop_ad_write),
#endif
#ifdef TEST_TCAM
        CINT_FWRAPPER_ENTRY(arad_pp_ihb_tcam_tbl_read_unsafe_lcl),
        CINT_FWRAPPER_ENTRY(arad_pp_ihb_tcam_tbl_write_unsafe_lcl),
        CINT_FWRAPPER_ENTRY(arad_pp_ihb_tcam_tbl_compare_unsafe_lcl),
#endif
#endif /* BCM_PETRA_SUPPORT */
        CINT_FWRAPPER_NENTRY("SHR_BITSET", __SHR_BITSET),
        CINT_FWRAPPER_NENTRY("SHR_BITGET", __SHR_BITGET),
        CINT_FWRAPPER_NENTRY("SHR_BITCLR", __SHR_BITCLR),

        CINT_ENTRY_LAST

    };

#ifdef BCM_PETRA_SUPPORT
#ifdef TEST_TCAM
static cint_parameter_desc_t __cint_struct_members__ARAD_PP_IHB_TCAM_BANK_TBL_DATA[] =
{
    {
        "uint32",
        "mask",
        0,
        ARAD_PP_IHB_TCAM_DATA_WIDTH
    },
    {
        "uint32",
         "value",
         0,
         ARAD_PP_IHB_TCAM_DATA_WIDTH
    },
    {
        "uint32",
        "valid",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__ARAD_PP_IHB_TCAM_BANK_TBL_DATA(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    ARAD_PP_IHB_TCAM_BANK_TBL_DATA* s = (ARAD_PP_IHB_TCAM_BANK_TBL_DATA*) p;

    switch(mnum)
    {
    case 0: rv = &(s->mask); break;
    case 1: rv = &(s->value); break;
    case 2: rv = &(s->valid); break;
    default: rv = NULL; break;
    }

    return rv;
}

static cint_parameter_desc_t __cint_struct_members__ARAD_PP_IHB_TCAM_COMPARE_DATA[] =
{
    {
        "uint32",
        "found",
        0,
        0
    },
    {
        "uint32",
        "address",
        0,
        0
    },
    {NULL}
};

static void*
__cint_maddr__ARAD_PP_IHB_TCAM_COMPARE_DATA(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    ARAD_PP_IHB_TCAM_COMPARE_DATA* s = (ARAD_PP_IHB_TCAM_COMPARE_DATA*) p;

    switch(mnum)
    {
    case 0: rv = &(s->found); break;
    case 1: rv = &(s->address); break;
    default: rv = NULL; break;
    }

    return rv;
}
static cint_parameter_desc_t __cint_struct_members__combo28_drc_pll_t[] =
{
    {
        "uint32",
        "iso_in",
        0,
        0
    },
    {
        "uint32",
        "cp",
        0,
        0
    },
    {
        "uint32",
        "cp1",
        0,
        0
    },
    {
        "uint32",
        "cz",
        0,
        0
    },
    {
        "uint32",
        "icp",
        0,
        0
    },
    {
        "uint32",
        "rp",
        0,
        0
    },    
    {
        "uint32",
        "rz",
        0,
        0
    },
    {
        "uint32",
        "ldo_ctrl",
        0,
        0
    },
    {
        "uint32",
        "msc_ctrl",
        0,
        0
    },
    {
        "uint32",
        "ndiv_frac",
        0,
        0
    },
    {
        "uint32",
        "ndiv_int",
        0,
        0
    },
    {
        "uint32",
        "pdiv",
        0,
        0
    },
    {
        "uint32",
        "ssc_limit",
        0,
        0
    },
    {
        "uint32",
        "ssc_mode",
        0,
        0
    },
    {
        "uint32",
        "ssc_step",
        0,
        0
    },
    {
        "uint32",
        "vco_gain",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__combo28_vendor_info_t[] =
{
    {
        "uint32",
        "dram_density",
        0,
        0
    },
    {
        "uint32",
        "fifo_depth",
        0,
        0
    },
    {
        "uint32",
        "revision_id",
        0,
        0
    },
    {
        "uint32",
        "manufacture_id",
        0,
        0
    },
    { NULL }
};


static cint_parameter_desc_t __cint_struct_members__combo28_bist_info_t[] =
{
    {
        "uint32",
        "write_weight",
        0,
        0
    },
    {
        "uint32",
        "read_weight",
        0,
        0
    },
    {
        "uint32",
        "bist_timer_us",
        0,
        0
    },
    {
        "uint32",
        "bist_num_actions",
        0,
        0
    },
    {
        "uint32",
        "bist_start_address",
        0,
        0
    },
    {
        "uint32",
        "bist_end_address",
        0,
        0
    },
    {
        "int",
        "mpr_mode",
        0,
        0
    },
    { NULL }
};



static cint_parameter_desc_t __cint_struct_members__combo28_gddr5_bist_info_t[] =
{
    {
        "uint32",
        "fifo_depth",
        0,
        0
    },    
    {
        "int",
        "bist_mode",
        0,
        0
    },
    {
        "uint32*",
        "data_pattern",
        1,
        0
    },  
    {
        "uint8*",
        "dbi_pattern",
        1,
        0
    }, 
    {
        "uint8*",
        "edd_pattern",
        1,
        0
    }, 
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__combo28_bist_err_cnt_t[] =
{
    {
        "uint32",
        "uint32 bist_err_occur",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_full_err_cnt",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_single_err_cnt",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_global_err_cnt",
        0,
        0
    },
    { NULL }
};


static cint_parameter_desc_t __cint_struct_members__combo28_gddr5_bist_err_cnt_t[] =
{
    {
        "uint32",
        "uint32 bist_data_err_occur",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_dbi_err_occur",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_edc_err_occur",
        0,
        0
    },
    {
        "uint32",
        "uint32 bist_adt_err_occur",
        0,
        0
    },
    { NULL }
};


static void*
__cint_maddr__combo28_drc_pll_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_drc_pll_t* s = (combo28_drc_pll_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->iso_in); break;
    case 1: rv = &(s->cp); break;
    case 2: rv = &(s->cp1); break;
    case 3: rv = &(s->cz); break;
    case 4: rv = &(s->icp); break;
    case 5: rv = &(s->rp); break;
    case 6: rv = &(s->rz); break;
    case 7: rv = &(s->ldo_ctrl); break;
    case 8: rv = &(s->msc_ctrl); break;
    case 9: rv = &(s->ndiv_frac); break;
    case 10: rv = &(s->ndiv_int); break;
    case 11: rv = &(s->pdiv); break;
    case 12: rv = &(s->ssc_limit); break;
    case 13: rv = &(s->ssc_mode); break;
    case 14: rv = &(s->ssc_step); break;
    case 15: rv = &(s->vco_gain); break;
    default: rv = NULL; break;
    }

    return rv;
}
static void*
__cint_maddr__combo28_vendor_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_vendor_info_t* s = (combo28_vendor_info_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->dram_density); break;
    case 1: rv = &(s->fifo_depth); break;
    case 2: rv = &(s->revision_id); break;
    case 3: rv = &(s->manufacture_id); break;
    default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__combo28_bist_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_bist_info_t* s = (combo28_bist_info_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->write_weight); break;
    case 1: rv = &(s->read_weight); break;
    case 2: rv = &(s->bist_timer_us); break;
    case 3: rv = &(s->bist_num_actions); break;
    case 4: rv = &(s->bist_start_address); break;
    case 5: rv = &(s->bist_end_address); break;
    case 6: rv = &(s->mpr_mode); break;
    default: rv = NULL; break;
    }

    return rv;
}


static void*
__cint_maddr__combo28_gddr5_bist_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_gddr5_bist_info_t* s = (combo28_gddr5_bist_info_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->fifo_depth); break;
    case 1: rv = &(s->bist_mode); break;
    case 2: rv = &(s->data_pattern); break;
    case 3: rv = &(s->dbi_pattern); break;
    case 4: rv = &(s->edc_pattern); break;
    default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__combo28_bist_err_cnt_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_bist_err_cnt_t* s = (combo28_bist_err_cnt_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->bist_err_occur); break;
    case 1: rv = &(s->bist_full_err_cnt); break;
    case 2: rv = &(s->bist_single_err_cnt); break;
    case 3: rv = &(s->bist_global_err_cnt); break;
    default: rv = NULL; break;
    }

    return rv;
}


static void*
__cint_maddr__combo28_gddr5_bist_err_cnt_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    combo28_gddr5_bist_err_cnt_t* s = (combo28_gddr5_bist_err_cnt_t*) p;

    switch(mnum)
    {
    case 0: rv = &(s->bist_data_err_occur); break;
    case 1: rv = &(s->bist_dbi_err_occur); break;
    case 2: rv = &(s->bist_edc_err_occur); break;
    case 3: rv = &(s->bist_adt_err_occur); break;
    default: rv = NULL; break;
    }

    return rv;
}


#endif
#endif /* BCM_PETRA_SUPPORT */
static cint_parameter_desc_t __cint_struct_members__soc_interrupt_cause_t[] =
{
    {
        "int",
        "id",
        0,
        0
    },
    {
        "int",
        "index",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__soc_interrupt_db_t[] =
{
#if !defined(SOC_NO_NAMES)
    {
        "char",
        "name",
        1,
        0
    },
#endif
    {
        "int",
        "id",
        0,
        0
    },
    {
        "int",
        "reg",
        0,
        0
    },
    {
        "int",
        "reg_index",
        0,
        0
    },
    {
        "int",
        "field",
        0,
        0
    },
    {
        "int",
        "mask_reg",
        0,
        0
    },
    {
        "int",
        "mask_reg_index",
        0,
        0
    },
    {
        "int",
        "mask_field",
        0,
        0
    },
    {
        "int",
        "vector_id",
        0,
        0
    },
    {
        "soc_interrupt_tree_t",
        "vector_info",
        1,
        0
    },
    {
        "soc_block_type_t",
        "block_type",
        0,
        0
    },
    {
        "clear_func",
        "interrupt_clear",
        0,
        0
    },
    {
        "void",
        "interrupt_clear_param1",
        1,
        0
    },
    {
        "void",
        "interrupt_clear_param2",
        1,
        0
    },
    {
        "uint32",
        "statistics_count",
        1,
        0
    },
    {
        "uint32",
        "storm_detection_occurrences",
        1,
        0
    },
    {
        "uint32",
        "storm_detection_start_time",
        1,
        0
    },
    {
        "uint32",
        "storm_nominal_ct",
        1,
        0
    },
    {
        "int",
        "bit_in_field",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__soc_interrupt_cause_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_interrupt_cause_t* s = (soc_interrupt_cause_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->id); break;
        case 1: rv = &(s->index); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
__cint_maddr__soc_interrupt_db_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    soc_interrupt_db_t* s = (soc_interrupt_db_t*) p;

    switch(mnum)
    {
#if !defined(SOC_NO_NAMES)
        case 0: rv = &(s->name); break;
        case 1: rv = &(s->id); break;
        case 2: rv = &(s->reg); break;
        case 3: rv = &(s->reg_index); break;
        case 4: rv = &(s->field); break;
        case 5: rv = &(s->mask_reg); break;
        case 6: rv = &(s->mask_reg_index); break;
        case 7: rv = &(s->mask_field); break;
        case 8: rv = &(s->vector_id); break;
        case 9: rv = &(s->vector_info); break;
        case 10: rv = &(s->block_type); break;
        case 11: rv = &(s->interrupt_clear); break;
        case 12: rv = &(s->interrupt_clear_param1); break;
        case 13: rv = &(s->interrupt_clear_param2); break;
        case 14: rv = &(s->statistics_count); break;
        case 15: rv = &(s->storm_detection_occurrences); break;
        case 16: rv = &(s->storm_detection_start_time); break;
        case 17: rv = &(s->storm_nominal_count); break;
        case 18: rv = &(s->bit_in_field); break;
#else
        case 0: rv = &(s->id); break;
        case 1: rv = &(s->reg); break;
        case 2: rv = &(s->reg_index); break;
        case 3: rv = &(s->field); break;
        case 4: rv = &(s->mask_reg); break;
        case 5: rv = &(s->mask_reg_index); break;
        case 6: rv = &(s->mask_field); break;
        case 7: rv = &(s->vector_id); break;
        case 8: rv = &(s->vector_info); break;
        case 9: rv = &(s->block_type); break;
        case 10: rv = &(s->interrupt_clear); break;
        case 11: rv = &(s->interrupt_clear_param1); break;
        case 12: rv = &(s->interrupt_clear_param2); break;
        case 13: rv = &(s->statistics_count); break;
        case 14: rv = &(s->storm_detection_occurrences); break;
        case 15: rv = &(s->storm_detection_start_time); break;
        case 16: rv = &(s->storm_nominal_count); break;
        case 17: rv = &(s->bit_in_field); break;
#endif
        default: rv = NULL; break;
    }

    return rv;
}

#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
static cint_parameter_desc_t __cint_struct_members__arad_kbp_lut_data_t[] =
{
    {
        "uint32",
        "rec_size",
        0,
        0
    },
    {
        "uint32",
        "rec_type",
        0,
        0
    },
    {
        "uint32",
        "rec_is_valid",
        0,
        0
    },
    {
        "uint32",
        "mode",
        0,
        0
    },
    {
        "uint32",
        "key_config",
        0,
        0
    },
    {
        "uint32",
        "lut_key_data",
        0,
        0
    },
    {
        "uint32",
        "instr",
        0,
        0
    },
    {
        "uint32",
        "key_w_cpd_gt_80",
        0,
        0
    },
    {
        "uint32",
        "copy_data_cfg",
        0,
        0
    },
    {
        "uint32",
        "result_idx_ad_cfg[0]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[0]",
        0,
        0
    },
   {
        "uint32",
        "result_idx_ad_cfg[1]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[1]",
        0,
        0
    },
   {
        "uint32",
        "result_idx_ad_cfg[2]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[2]",
        0,
        0
    },
 {
        "uint32",
        "result_idx_ad_cfg[3]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[3]",
        0,
        0
    },
   {
        "uint32",
        "result_idx_ad_cfg[4]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[4]",
        0,
        0
    },
 {
        "uint32",
        "result_idx_ad_cfg[5]",
        0,
        0
    },
    {
        "uint32",
        "result_idx_or_ad[5]",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__arad_kbp_lut_data_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    arad_kbp_lut_data_t* s = (arad_kbp_lut_data_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->rec_size); break;
        case 1: rv = &(s->rec_type); break;
        case 2: rv = &(s->rec_is_valid); break;
        case 3: rv = &(s->mode); break;
        case 4: rv = &(s->key_config); break;
        case 5: rv = &(s->lut_key_data); break;
        case 6: rv = &(s->instr); break;
        case 7: rv = &(s->key_w_cpd_gt_80); break;
        case 8: rv = &(s->copy_data_cfg); break;
        case 9: rv = &(s->result_idx_ad_cfg[0]); break;
        case 10: rv = &(s->result_idx_or_ad[0]); break;
        case 11: rv = &(s->result_idx_ad_cfg[1]); break;
        case 12: rv = &(s->result_idx_or_ad[1]); break;
        case 13: rv = &(s->result_idx_ad_cfg[2]); break;
        case 14: rv = &(s->result_idx_or_ad[2]); break;
        case 15: rv = &(s->result_idx_ad_cfg[3]); break;
        case 16: rv = &(s->result_idx_or_ad[3]); break;
        case 17: rv = &(s->result_idx_ad_cfg[4]); break;
        case 18: rv = &(s->result_idx_or_ad[4]); break;
        case 19: rv = &(s->result_idx_ad_cfg[5]); break;
        case 20: rv = &(s->result_idx_or_ad[5]); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_enum_map_t __cint_enum_map__NlmAradWriteMode[] =
{
    { "NLM_ARAD_WRITE_MODE_DATABASE_DM", NLM_ARAD_WRITE_MODE_DATABASE_DM },
    { "NLM_ARAD_WRITE_MODE_DATABASE_XY", NLM_ARAD_WRITE_MODE_DATABASE_XY },
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__arad_kbp_rop_write_t[] =
{
    {
        "uint8",
        "addr",
        0,
        NLMDEV_REG_ADDR_LEN_IN_BYTES
    },
    {
        "uint8",
        "data",
        0,
        NLM_DATA_WIDTH_BYTES
    },
    {
        "uint8",
        "mask",
        0,
        NLM_DATA_WIDTH_BYTES
    },
    {
        "uint8",
        "addr_short",
        0,
        NLMDEV_REG_ADDR_LEN_IN_BYTES
    },
    {
        "uint8",
        "vBit",
        0,
        0
    },
    {
        "NlmAradWriteMode",
        "writeMode",
        0,
        0
    },
    { NULL }
};

static void*
__cint_maddr__arad_kbp_rop_write_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    arad_kbp_rop_write_t* s = (arad_kbp_rop_write_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->addr); break;
        case 1: rv = &(s->data); break;
        case 2: rv = &(s->mask); break;
        case 3: rv = &(s->addr_short); break;
        case 4: rv = &(s->vBit); break;
        case 5: rv = &(s->writeMode); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_enum_map_t __cint_enum_map__NlmAradReadMode[] =
{
    { "NLM_ARAD_READ_MODE_DATA_X", NLM_ARAD_READ_MODE_DATA_X },
    { "NLM_ARAD_READ_MODE_DATA_Y", NLM_ARAD_READ_MODE_DATA_Y },
    { NULL }
};

static cint_parameter_desc_t __cint_struct_members__arad_kbp_rop_read_t[] =
{
    {
        "uint8",
        "addr",
        0,
        NLMDEV_REG_ADDR_LEN_IN_BYTES
    },
    {
        "uint8",
        "vBit",
        0,
        0
    },
    {
        "NlmAradReadMode",
        "dataType",
        0,
        0
    },
    {
        "uint8",
        "data",
        0,
        NLM_DATA_WIDTH_BYTES + 1
    },
    { NULL }
};



static void*
__cint_maddr__arad_kbp_rop_read_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    arad_kbp_rop_read_t* s = (arad_kbp_rop_read_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->addr); break;
        case 1: rv = &(s->vBit); break;
        case 2: rv = &(s->dataType); break;
        case 3: rv = &(s->data); break;
        default: rv = NULL; break;
    }

    return rv;
}
#endif /* #if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) */
static cint_struct_type_t __cint_soc_structures[] =
{

    /* additions for cmicx diagnostic tests */

    {
    "soc_sbusdma_desc_ctrl_t",
    sizeof(soc_sbusdma_desc_ctrl_t),
    __cint_struct_members__soc_sbusdma_desc_ctrl_t,
    __cint_maddr__soc_sbusdma_desc_ctrl_t
    },
    {
    "soc_sbusdma_desc_cfg_t",
    sizeof(soc_sbusdma_desc_cfg_t),
    __cint_struct_members__soc_sbusdma_desc_cfg_t,
    __cint_maddr__soc_sbusdma_desc_cfg_t
    },
    {
    "dv_t",
    sizeof(dv_t),
    __cint_struct_members__dv_t,
    __cint_maddr__dv_t
    },

    /* end additions for cmicx diagnostic tests */

    {
    "soc_interrupt_cause_t",
    sizeof(soc_interrupt_cause_t),
    __cint_struct_members__soc_interrupt_cause_t,
    __cint_maddr__soc_interrupt_cause_t
    },
      {
    "soc_interrupt_db_t",
    sizeof(soc_interrupt_db_t),
    __cint_struct_members__soc_interrupt_db_t,
    __cint_maddr__soc_interrupt_db_t
    },
#ifdef BCM_PETRA_SUPPORT
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    {
    "arad_kbp_lut_data_t",
    sizeof(arad_kbp_lut_data_t),
    __cint_struct_members__arad_kbp_lut_data_t,
    __cint_maddr__arad_kbp_lut_data_t
    },
    {
    "arad_kbp_rop_write_t",
    sizeof(arad_kbp_rop_write_t),
    __cint_struct_members__arad_kbp_rop_write_t,
    __cint_maddr__arad_kbp_rop_write_t
    },
    {
    "arad_kbp_rop_read_t",
    sizeof(arad_kbp_rop_read_t),
    __cint_struct_members__arad_kbp_rop_read_t,
    __cint_maddr__arad_kbp_rop_read_t
    },
#endif
#ifdef TEST_TCAM
    {
        "ARAD_PP_IHB_TCAM_BANK_TBL_DATA",
            sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA),
            __cint_struct_members__ARAD_PP_IHB_TCAM_BANK_TBL_DATA,
            __cint_maddr__ARAD_PP_IHB_TCAM_BANK_TBL_DATA
    },
    {
        "ARAD_PP_IHB_TCAM_COMPARE_DATA",
        sizeof(ARAD_PP_IHB_TCAM_COMPARE_DATA),
        __cint_struct_members__ARAD_PP_IHB_TCAM_COMPARE_DATA,
        __cint_maddr__ARAD_PP_IHB_TCAM_COMPARE_DATA
    },
#endif
    {
        "combo28_drc_pll_t",
        sizeof(combo28_drc_pll_t),
        __cint_struct_members__combo28_drc_pll_t,
        __cint_maddr__combo28_drc_pll_t
    },
    {
        "combo28_vendor_info_t",
        sizeof(combo28_vendor_info_t),
        __cint_struct_members__combo28_vendor_info_t,
        __cint_maddr__combo28_vendor_info_t
    },
    {
        "combo28_bist_info_t",
        sizeof(combo28_bist_info_t),
        __cint_struct_members__combo28_bist_info_t,
        __cint_maddr__combo28_bist_info_t
    },
    {
        "combo28_gddr5_bist_info_t",
        sizeof(combo28_gddr5_bist_info_t),
        __cint_struct_members__combo28_gddr5_bist_info_t,
        __cint_maddr__combo28_gddr5_bist_info_t
    },
    {
        "combo28_bist_err_cnt_t",
        sizeof(combo28_bist_err_cnt_t),
        __cint_struct_members__combo28_bist_err_cnt_t,
        __cint_maddr__combo28_bist_err_cnt_t
    },   
    {
        "combo28_gddr5_bist_err_cnt_t",
        sizeof(combo28_gddr5_bist_err_cnt_t),
        __cint_struct_members__combo28_gddr5_bist_err_cnt_t,
        __cint_maddr__combo28_gddr5_bist_err_cnt_t
    },   
#endif /* BCM_PETRA_SUPPORT */
    { NULL }
};

static cint_enum_type_t __cint_soc_enums[] =
{
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    { "NlmAradWriteMode", __cint_enum_map__NlmAradWriteMode },
    { "NlmAradReadMode", __cint_enum_map__NlmAradReadMode },
#endif
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_soc_typedefs[] =
{

    /* additions for cmicx diagnostic tests */

    { "int", "soc_mem_t", 0, 0 },
    { "int", "soc_block_t", 0, 0 },
    { "int", "sbusdma_desc_handle_t", 0, 0 },

    /* end additions for cmicx diagnostic tests */

    {
        "int",
        "soc_port_t",
        0,
        0
    },
    {NULL}
};

static cint_constants_t __cint_soc_constants[] =
{
   CINT_ENTRY_LAST
};

cint_data_t soc_cint_data =
    {
        NULL,
        __cint_soc_functions,
        __cint_soc_structures,
        __cint_soc_enums,
        __cint_soc_typedefs,
        __cint_soc_constants,
        __cint_soc_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */



