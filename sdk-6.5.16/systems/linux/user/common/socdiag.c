/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * socdiag: low-level diagnostics shell for Orion (SOC) driver.
 */

#include <unistd.h>
#include <stdlib.h>

#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/pci.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <shared/shr_bprof.h>

#include <appl/diag/system.h>

#include <linux-bde.h>

#if defined(MEMLOG_SUPPORT) && defined(__GNUC__)
#include <sal/core/memlog.h>
#include <sys/stat.h>
#include <fcntl.h>
int memlog_fd = 0;
char memlog_buf[MEM_LOG_BUF_SIZE];
sal_mutex_t memlog_lock = NULL;
#endif

#ifdef LINUX_PLI_COMBO_BDE
#include <bde/pli/plibde.h>
#endif

ibde_t *bde;

/* The bus properties are (currently) the only system specific
 * settings required. 
 * These must be defined beforehand 
 */

#ifndef SYS_BE_PIO
#error "SYS_BE_PIO must be defined for the target platform"
#endif
#ifndef SYS_BE_PACKET
#error "SYS_BE_PACKET must be defined for the target platform"
#endif
#ifndef SYS_BE_OTHER
#error "SYS_BE_OTHER must be defined for the target platform"
#endif

#if !defined(SYS_BE_PIO) || !defined(SYS_BE_PACKET) || !defined(SYS_BE_OTHER)
#error "platform bus properties not defined."
#endif

/* Function defined in linux-user-bde.c */
extern int
bde_icid_get(int d, uint8 *data, int len);

static soc_chip_info_vectors_t chip_info_vect = {
    bde_icid_get
};

static void
chip_info_vect_config(void)
{
    soc_chip_info_vect_config(&chip_info_vect);
}

#ifdef INCLUDE_KNET

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <soc/knet.h>
#include <uk-proxy-kcom.h>
#include <bcm-knet-kcom.h>

/* Function defined in linux-user-bde.c */
extern int
bde_irq_mask_set(int unit, uint32 addr, uint32 mask);
extern int
bde_hw_unit_get(int unit, int inverse);

static soc_knet_vectors_t knet_vect_uk_proxy = {
    {
        uk_proxy_kcom_open,
        uk_proxy_kcom_close,
        uk_proxy_kcom_msg_send,
        uk_proxy_kcom_msg_recv
    },
    bde_irq_mask_set,
    bde_hw_unit_get
};

static soc_knet_vectors_t knet_vect_bcm_knet = {
    {
        bcm_knet_kcom_open,
        bcm_knet_kcom_close,
        bcm_knet_kcom_msg_send,
        bcm_knet_kcom_msg_recv
    },
    bde_irq_mask_set,
    bde_hw_unit_get
};

static void
knet_kcom_config(void)
{
    soc_knet_vectors_t *knet_vect;
    char *kcom_name;
    int procfd;
    char procbuf[128];

    /* Direct IOCTL by default */
    knet_vect = &knet_vect_bcm_knet;
    kcom_name = "bcm-knet";

    if ((procfd = open("/proc/linux-uk-proxy", O_RDONLY)) >= 0) {
        if ((read(procfd, procbuf, sizeof(procbuf))) > 0 &&
            strstr(procbuf, "KCOM_KNET : ACTIVE") != NULL) {
            /* Proxy loaded and active */
            knet_vect = &knet_vect_uk_proxy;
            kcom_name = "uk-proxy";
        }
        close(procfd);
    }

    soc_knet_config(knet_vect);
    var_set("kcom", kcom_name, 0, 0);
}

#endif /* INCLUDE_KNET */


#ifdef BCM_INSTANCE_SUPPORT
static int
_instance_config(const char *inst)
{
    const char *ptr;
#ifndef NO_SAL_APPL
    char *estr;
#endif
    unsigned int dev_mask, dma_size;

    if (inst == NULL) {
#ifndef NO_SAL_APPL
        estr = "./bcm.user -i <dev_mask>[:dma_size_in_mb] \n";
        sal_console_write(estr, sal_strlen(estr) + 1);
#endif
        return -1;
    }
    dev_mask = strtol(inst, NULL, 0);
    if ((ptr = strchr(inst,':')) == NULL) {
        dma_size = 4;
    } else {
        ptr++;
        dma_size = strtol(ptr, NULL, 0);
    }

    if (dma_size < 4) {
#ifndef NO_SAL_APPL
        estr = "dmasize must be > 4M and a power of 2 (4M, 8M etc.)\n";
        sal_console_write(estr, sal_strlen(estr) + 1);
#endif
        return -1;
    } else {
        if ( (dma_size >> 2) & ((dma_size >> 2 )-1)) {
#ifndef NO_SAL_APPL
            estr = "dmasize must be a power of 2 (4M, 8M etc.)\n";
            sal_console_write(estr, sal_strlen(estr) + 1);
#endif
            return -1;
        }
    }
    
    return linux_bde_instance_attach(dev_mask, dma_size);
}
#endif

#ifdef LINUX_PLI_COMBO_BDE
static int sim_path = 0;

int
bcm_sim_path_get(void)
{
    return sim_path;
}

int intr_int_context(void) 
{
    extern int pli_intr_int_context() __attribute__((weak)); 
    extern int linux_intr_int_context() __attribute__((weak)); 

    if (sim_path) {
        return pli_intr_int_context();
    }
    return linux_intr_int_context();
}
#endif

int
bde_create(void)
{
    linux_bde_bus_t bus;
    bus.be_pio = SYS_BE_PIO;
    bus.be_packet = SYS_BE_PACKET;
    bus.be_other = SYS_BE_OTHER;

#ifdef LINUX_PLI_COMBO_BDE
    sim_path = sal_ctoi(getenv("BCM_SIM_PATH"), 0);
    if (sim_path) {
        return plibde_create(&bde);
    }
#endif

    return linux_bde_create(&bus, &bde);

}

/*
 * Main loop.
 */
int main(int argc, char *argv[])
{
    int i, len;
    char *envstr;
    char *config_file, *config_temp;
#ifdef BCM_INSTANCE_SUPPORT
    const char *inst = NULL;
#endif

#ifdef BCM_BPROF_STATS
    shr_bprof_stats_time_init();
#endif

    if ((envstr = getenv("BCM_CONFIG_FILE")) != NULL) {
        config_file = envstr;
        len = sal_strlen(config_file);
        if ((config_temp = sal_alloc(len+5, NULL)) != NULL) {
            sal_strcpy(config_temp, config_file);
            sal_strcpy(&config_temp[len], ".tmp");
#ifndef NO_SAL_APPL
            sal_config_file_set(config_file, config_temp);
#endif
            sal_free(config_temp);
        }
    }

#ifdef BCM_INSTANCE_SUPPORT
    for (i = 1; i < argc; i++) {
         if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--instance")) {
            inst = argv[i+1];            
            /*
             * specify the dev_mask and its dma_size (optional,default:4MB)
             * bcm.user -i 0x1[:8]
             */
            if (_instance_config(inst) < 0){
#ifndef NO_SAL_APPL
                char *estr = "config error!\n";
                sal_console_write(estr, sal_strlen(estr) + 1);
#endif
                exit(1);
            }
        }
    }
#endif

    if (sal_core_init() < 0
#ifndef NO_SAL_APPL
        || sal_appl_init() < 0
#endif
        ) {
        /*
         * If SAL initialization fails then printf will most
         * likely assert or fail. Try direct console access
         * instead to get the error message out.
         */
#ifndef NO_SAL_APPL
        char *estr = "SAL Initialization failed\r\n";
        sal_console_write(estr, sal_strlen(estr) + 1);
#endif
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--reload")) {
            sal_boot_flags_set(sal_boot_flags_get() | BOOT_F_RELOAD);
        }
    }

#ifdef MEMLOG_SUPPORT
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-memlog") && argv[i+1] != NULL) {
            memlog_fd = creat(argv[i+1], S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        }
    }
    if (memlog_fd > 0) {
        memlog_lock = sal_mutex_create("memlog lock");
    }
#endif
#ifdef INCLUDE_KNET
    knet_kcom_config();
#endif
    chip_info_vect_config();

    diag_shell();

    linux_bde_destroy(bde);
#ifdef MEMLOG_SUPPORT
    if (memlog_lock) {
        sal_mutex_destroy(memlog_lock);
    }
#endif
    return 0;
}

#ifndef NO_SAL_APPL
/*
 * This function is required for the pci_print_all function defined in
 * sal/appl/pci_common.c. The pci_print_all function is a legacy
 * function used only by the CLI 'show pci' command.
 */

#include <sal/appl/pci.h>

uint32 pci_config_getw(pci_dev_t *dev, uint32 addr)
{
#ifdef LINUX_PLI_COMBO_BDE
    extern uint32 pli_pci_config_getw(pci_dev_t *dev, uint32 addr);
    if (sim_path) {
        return pli_pci_config_getw(dev, addr);
    }
#endif
    /* We only support our own devices */
    if (dev->busNo != 0) {
        return 0xffffffff;
    }

    if (dev->devNo >= bde->num_devices(BDE_SWITCH_DEVICES)) {
        return 0xffffffff;
    }

    return bde->pci_conf_read(dev->devNo, addr);
}
#endif /* NO_SAL_APPL */
