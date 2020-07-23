/*! \file ha.c
 *
 * Sample HA management.
 *
 * In HSDK, SDKLT lib and BCM API Emulator(BAE) of each inst have separate
 * share files. Thus HA lib has to maintain instances with the number of
 * (BCMDRD_CONFIG_MAX_UNITS * 2).
 * SDKLT lib uses HA instance range 0 ~ (BCMDRD_CONFIG_MAX_UNITS-1), and the
 * instance id is identical with the DRD device number.
 * BAE uses HA instances start from BCMDRD_CONFIG_MAX_UNITS, and the instance
 * id is DRD device number with the offset of BCMDRD_CONFIG_MAX_UNITS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <appl/diag/ltsw/ha.h>

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmha/bcmha_mem.h>
#include <bcmdrd_config.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/* Name convention of the shared file: <user-name>_sdk_ha_<inst-id>. */
#define HA_SHARED_FILE_NAME "sdk_ha"

/* Name convention of the shared generic file: <user-name>_sdk_ha_gen. */
#define HA_SHARED_GEN_FILE_NAME "sdk_ha_gen"

/*!
 * \brief Invalid HA file instance number
 *
 * Instance number can be assigned to each HA file when running at multiple
 * instance environment (mostly used by QA).
 */
#define HA_INVALID_INSTANCE 0xFFFFFFFF

/*! Shared file default location. */
#ifndef HA_SHARED_FILE_DIR
#define HA_SHARED_FILE_DIR         "/dev/shm"
#endif

#define HA_MAX_UNITS (BCMDRD_CONFIG_MAX_UNITS * 2)

/* Maximum name length for shared file */
#define MAX_FILE_NAME_LEN   256

/* Shared file control */
typedef struct ha_ctrl_s {
    char *fname;
    int fd;
    uint32_t mapped_size;
} ha_ctrl_t;

/* Shared file control for all units + units + generic instance */
static ha_ctrl_t ha_ctrl[HA_MAX_UNITS + 1];

/*******************************************************************************
 * Private functions
 */
static void *
ha_pool_alloc(void *ctx,
              uint32_t size,
              uint32_t *actual_size)
{
    ha_ctrl_t *hc = (ha_ctrl_t *)ctx;
    void *mmap_ptr;
    uint32_t addition_mem_size;
    int pgsize = sysconf(_SC_PAGESIZE);

    /* Sanity check */
    if (hc == NULL) {
        return NULL;
    }

    /* Set the size to be on a page boundary */
    addition_mem_size = pgsize * ((size + pgsize - 1) / pgsize);

    /* Extend the shared file size */
    if (ftruncate(hc->fd, hc->mapped_size + addition_mem_size) != 0) {
        return NULL;
    }

    /* Create another memory map section */
    if ((mmap_ptr = mmap(NULL,
                         addition_mem_size,
                         PROT_WRITE | PROT_READ,
                         MAP_SHARED, hc->fd, hc->mapped_size)) == MAP_FAILED) {
        return NULL;
    }
    hc->mapped_size += addition_mem_size;

    *actual_size = addition_mem_size;

    return mmap_ptr;
}

/*
 * This function retrieves the user name of the current user and place it into
 * the provided buffer.
 */
static int insert_uname(char *buf, size_t buf_length, size_t *used)
{
    struct passwd pwd;
    struct passwd *result;
    char *loc_buf;
    size_t bufsize;
    int rv;
    int ret = SHR_E_MEMORY;

    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if ((long)bufsize == -1) {   /* Value was indeterminate */
        bufsize = 16384;        /* Should be more than enough */
    }
    loc_buf = malloc(bufsize);
    if (loc_buf) {
        rv = getpwuid_r(getuid(), &pwd, loc_buf, bufsize, &result);
        if (!rv && result) {
            *used = strlen(result->pw_name);
            if (*used < buf_length - 1) {
                strncpy(buf, result->pw_name, *used + 1);
                ret = SHR_E_NONE;
            }
        } else {
            ret = SHR_E_INTERNAL;
        }
        free(loc_buf);
    }
    return ret;
}

/*
 * This function construct the file name for the share file. For cold boot
 * it will create unique name that is not already exist. For warm boot it
 * will create the generic name (based on the user name).
 * The file name will be of the format:
 * /<user-name>_sdk_ha_<unit> or
 * /<user-name>_sdk_ha_<unit>_<instance>
 */
static int
build_file_name(int unit, bool warm, uint32_t instance)
{
    ha_ctrl_t *hc = &ha_ctrl[unit];
    int j;
    struct stat f_info;
    char *fname;
    int len;
    char path[MAX_FILE_NAME_LEN];
    size_t used;
    char *fname_var;

    if (!hc->fname) {
        if ((hc->fname = malloc(MAX_FILE_NAME_LEN)) == NULL) {
            return SHR_E_MEMORY;
        }
    }
    fname = hc->fname;
    memset(fname, 0, MAX_FILE_NAME_LEN);
    fname[0] = '/';
    j = insert_uname(&fname[1], MAX_FILE_NAME_LEN-1, &used);
    if (j != SHR_E_NONE) {
        return j;
    }
    used++;     /* For the first character */
    fname_var = &fname[used];

    if (instance == HA_INVALID_INSTANCE) {
        len = snprintf(fname_var, MAX_FILE_NAME_LEN - used, "_%s_%d",
                       HA_SHARED_FILE_NAME, unit);
    } else {
        len = snprintf(fname_var, MAX_FILE_NAME_LEN - used, "_%s_%d_x%x",
                       HA_SHARED_FILE_NAME, unit, instance);
    }
    if (len >= MAX_FILE_NAME_LEN-1) {
        return SHR_E_INTERNAL;
    }

    /* Don't care about name uniqueness for warm boot */
    if (warm) {
        return SHR_E_NONE;
    }

    j = 0;
    /* Generate new file names until finding that no file has this name. */
    do {
        snprintf(path, sizeof(path), "%s%s", HA_SHARED_FILE_DIR, fname);
        if (stat(path, &f_info) != 0) {
            if (errno == ENOENT) {
                return SHR_E_NONE;
            } else {
                return SHR_E_INTERNAL;
            }
        }
        /*
         * For cold boot it is expected that the instance given is unique.
         * If we are here it means that the file already exit so it is not
         * unique. Return error.
         */
        if (instance != HA_INVALID_INSTANCE) {
            return SHR_E_PARAM;
        }
        len = snprintf(fname_var, MAX_FILE_NAME_LEN - used, "_%s%d_%d",
                       HA_SHARED_FILE_NAME, j++, unit);
        if (len + used >= MAX_FILE_NAME_LEN-1) {
            return SHR_E_INTERNAL;
        }
    } while (1);
}

/*
 * Generate file name of the form:
 * /<user-name>_sdk_ha_gen or
 * /<user-name>_sdk_ha_gen_<instance>
 */
static int
build_gen_file_name(bool warm, uint32_t instance)
{
    ha_ctrl_t *hc = &ha_ctrl[HA_MAX_UNITS];
    int j;
    struct stat f_info;
    char *fname;
    int len;
    char path[MAX_FILE_NAME_LEN + sizeof(HA_SHARED_FILE_DIR)];
    size_t used;
    char *fname_var;

    if (!hc->fname) {
        if ((hc->fname = malloc(MAX_FILE_NAME_LEN)) == NULL) {
            return SHR_E_MEMORY;
        }
    }
    fname = hc->fname;
    memset(fname, 0, MAX_FILE_NAME_LEN);
    fname[0] = '/';

    j = insert_uname(&fname[1], sizeof(path)-1, &used);
    if (j != SHR_E_NONE) {
        return j;
    }
    used++; /* For the first character */
    fname_var = &fname[used];

    if (instance == HA_INVALID_INSTANCE) {
        len = snprintf(fname_var, MAX_FILE_NAME_LEN,
                       "_%s", HA_SHARED_GEN_FILE_NAME);
    } else {
        len = snprintf(fname_var, MAX_FILE_NAME_LEN, "_%s_x%x",
                       HA_SHARED_GEN_FILE_NAME, instance);
    }
    if (len + used >= MAX_FILE_NAME_LEN - 1) {
        return SHR_E_INTERNAL;
    }

    /* Don't care about name uniqueness for warm boot */
    if (warm) {
        return SHR_E_NONE;
    }

    j = 0;
    /* Generate new file names until finding that no file has this name. */
    do {
        snprintf(path, sizeof(path), "%s%s", HA_SHARED_FILE_DIR, fname);
        if (stat(path, &f_info) != 0) {
            if (errno == ENOENT) {
                return SHR_E_NONE;
            } else {
                return SHR_E_INTERNAL;
            }
        }
        /*
         * For cold boot it is expected that the instance given is unique.
         * If we are here it means that the file already exit so it is not
         * unique. Return error.
         */
        if (instance != HA_INVALID_INSTANCE) {
            return SHR_E_PARAM;
        }
        len = snprintf(fname_var, MAX_FILE_NAME_LEN - used, "_%s%d",
                       HA_SHARED_GEN_FILE_NAME, j++);
        if (len + used >= MAX_FILE_NAME_LEN - 1) {
            return SHR_E_INTERNAL;
        }
    } while (1);
}

/*
 * This function being called for cold boot. In this scenario, the shared
 * directory should be emptied out from all HA files associated with the
 * current user. This function therefore scans the shared directory and
 * deletes every file with prefix name matches the HA user name file.
*/
static void
clear_old_files(int unit)
{
    ha_ctrl_t *hc = &ha_ctrl[unit];
    size_t len;
    DIR *dp;
    struct dirent *ep;
    int rv;
    char *fname;

    rv = build_file_name(unit, true, HA_INVALID_INSTANCE);
    if (rv != SHR_E_NONE) {
        return;
    }
    fname = hc->fname;
    fname++;    /* This first character is '/' */
    len = strlen(fname);

    /* Search the shared file directory for files with the same prefix */
    dp = opendir(HA_SHARED_FILE_DIR);
    while ((ep = readdir (dp)) != NULL) {
        if (strlen(ep->d_name) < len) {
            continue;
        }

        if (strncmp(ep->d_name, fname, len) == 0) {
            /* Don't remove files that has special ID */
            if (strlen(ep->d_name) > len + 1 && ep->d_name[len] == '_' &&
                ep->d_name[len+1] == 'x') {
                continue;
            }
            shm_unlink(ep->d_name);
        } else if (strncmp(ep->d_name, fname, len - 3) == 0) {
            if (isalpha(ep->d_name[len-3]) && ep->d_name[len-2] == '-' &&
                ep->d_name[len-1] == '0' + unit) {
                shm_unlink(ep->d_name);
            }
        }
    }
    closedir(dp);
}

static void
clear_old_gen_files(void)
{
    ha_ctrl_t *hc = &ha_ctrl[HA_MAX_UNITS];
    size_t len;
    DIR *dp;
    struct dirent *ep;
    int rv;
    char *fname;

    rv = build_gen_file_name(true, HA_INVALID_INSTANCE);
    if (rv != SHR_E_NONE) {
        return;
    }

    fname = &hc->fname[1];
    len = strlen(fname);

    /* Search the shared file directory for files with the same prefix */
    dp = opendir(HA_SHARED_FILE_DIR);
    while ((ep = readdir(dp)) != NULL) {
        if (strncmp(ep->d_name, fname, len) == 0) {
            /* Don't remove files that has special ID */
            if (strlen(ep->d_name) > len + 1 && ep->d_name[len] == '_' &&
                ep->d_name[len+1] == 'x') {
                continue;
            }
            shm_unlink(ep->d_name);
        }
    }
    closedir(dp);
}


static int open_ha_file(char *fname, int size, bool warm,
                        int *fh, long *file_size,
                        void **mmap_ptr, uint32_t *mapped_size)
{
    long sys_page_size = sysconf(_SC_PAGESIZE);
    struct stat stat_buf;

    *file_size = sys_page_size *((size + sys_page_size - 1)/ sys_page_size);
    if (warm) {
        /* Try to open the existing shared file */
        *fh = shm_open(fname, O_RDWR,
                       (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));

        if (*fh < 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(0,
                  "\n Error opening file %s"), fname));
        }
    } else {
        /* Existing file can't be opened */
        *fh = -1;
    }
    if (*fh < 0) {
        /* Existing file is not there, create new file */
        *fh = shm_open(fname, (int)(O_CREAT | O_RDWR),
                      (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
        if (*fh < 0) {
            return SHR_E_FAIL;
        }
        /* When file created its size set to 0. Adjust it to the desired size */
        if (ftruncate(*fh, *file_size) != 0) {
            close(*fh);
            return SHR_E_FAIL;
        }
        warm = false;
    } else {   /* File opened successfully without the create flag */
        if (fstat(*fh, &stat_buf) == 0) { /* Get the current file size */
            if (stat_buf.st_size < (off_t)*file_size) {
                /* Increase the file size if too small */
                if (ftruncate(*fh, *file_size) != 0) {
                    close(*fh);
                    return SHR_E_FAIL;
                }
            } else {
                /* Update the blk_len according to the file size */
                *file_size = stat_buf.st_size;
            }
        }
    }

    if (!warm) {
        *mmap_ptr = NULL;
        return SHR_E_NONE;
    }

    /* Memory map the file */
    if ((*mmap_ptr = mmap(NULL, *file_size,
                          PROT_WRITE | PROT_READ,
                          MAP_SHARED, *fh, 0)) == MAP_FAILED) {
        close(*fh);
        return SHR_E_FAIL;
    }
    *mapped_size = *file_size;

    return SHR_E_NONE;
}

static void
ha_pool_free(void *ctx, void *mmap_ptr, uint32_t len)
{
    munmap(mmap_ptr, len);
}

static int
ha_unit_open_ext(int unit,
                 int size,
                 bool enabled,
                 bool warm,
                 uint32_t instance)
{
    ha_ctrl_t *hc;
    int fh = -1;
    void *mmap_ptr = NULL;
    int rv;
    long file_size = 0;

    if ((unit < 0) || (unit >= HA_MAX_UNITS)) {
        return SHR_E_UNIT;
    }
    hc = &ha_ctrl[unit];
    if (hc->fd > 0) {
        return SHR_E_UNIT;
    }
    hc->mapped_size = 0;

    if (!enabled) {
        /* Use normal heap memory */
        /* Open the SDKLT HA files using bcmha */
        if (unit < BCMDRD_CONFIG_MAX_UNITS) {
            return bcmha_mem_init(unit, NULL, NULL, NULL, NULL, 0);
        } else {
            return shr_ha_unit_init(unit, NULL, DEFAULT_HA_FILE_SIZE,
                                    NULL, NULL, NULL);
        }
    }

    if (!warm && instance == HA_INVALID_INSTANCE) {
        clear_old_files(unit);
    }
    rv = build_file_name(unit, warm, instance);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = open_ha_file(hc->fname, size, warm, &fh, &file_size,
                      &mmap_ptr, &hc->mapped_size);
    if (rv != SHR_E_NONE) {
        free(hc->fname);
        hc->fname = NULL;
        return rv;
    }
    hc->fd = fh;

    /* Open the SDKLT HA files using bcmha */
    if (unit < BCMDRD_CONFIG_MAX_UNITS) {
        return bcmha_mem_init(unit,
                              ha_pool_alloc, ha_pool_free,
                              hc, mmap_ptr, (int)file_size);
    } else {
        return shr_ha_unit_init(unit,
                                mmap_ptr, (int)file_size,
                                ha_pool_alloc, ha_pool_free,
                                hc);
    }
}

static int
ha_unit_open(int unit, int size, bool enabled, bool warm)
{
    return ha_unit_open_ext(unit, size, enabled, warm, HA_INVALID_INSTANCE);
}

static int
ha_unit_close(int unit, bool warm_exit)
{
    ha_ctrl_t *hc;
    int rv;

    if ((unit < 0) || (unit >= HA_MAX_UNITS)) {
        return SHR_E_UNIT;
    }
    hc = &ha_ctrl[unit];

    rv = shr_ha_unit_close(unit);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    if (hc->fd > 0) {
        close(hc->fd);
        hc->fd = 0;
    }
    if (hc->fname) {
        if (!warm_exit) {
            shm_unlink(hc->fname);
        }
        free(hc->fname);
        hc->fname = NULL;
    }

    return SHR_E_NONE;
}


static int
ha_gen_open_ext(int size, bool enabled, bool warm, uint32_t instance)
{
    ha_ctrl_t *hc = &ha_ctrl[HA_MAX_UNITS];
    int fh = -1;
    void *mmap_ptr = NULL;
    int rv;
    long file_size = 0;

    hc->mapped_size = 0;

    if (!enabled) {
        /* Use normal heap memory */
        return bcmha_mem_init(BCMHA_GEN_UNIT, NULL, NULL, NULL, NULL, 0);
    }

    if (!warm && instance == HA_INVALID_INSTANCE) {
        clear_old_gen_files();
    }
    rv = build_gen_file_name(warm, instance);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = open_ha_file(hc->fname, size, warm, &fh, &file_size,
                      &mmap_ptr, &hc->mapped_size);
    if (rv != SHR_E_NONE) {
        LOG_WARN(BSL_LS_APPL_HA,
                 (BSL_META("Failed to open HA file %s\n"),
                  hc->fname));

        free(hc->fname);
        hc->fname = NULL;
        return rv;
    }
    hc->fd = fh;

    return bcmha_mem_init(BCMHA_GEN_UNIT,
                          ha_pool_alloc, ha_pool_free,
                          hc, mmap_ptr, (int)file_size);
}

static int
ha_gen_open(int size, bool enabled, bool warm)
{
    return ha_gen_open_ext(size, enabled, warm, HA_INVALID_INSTANCE);
}

static int
ha_gen_close(bool warm_exit)
{
    ha_ctrl_t *hc = &ha_ctrl[HA_MAX_UNITS];
    int rv;

    rv = shr_ha_gen_close();
    if (rv != SHR_E_NONE) {
        return rv;
    }

    if (hc->fd > 0) {
        close(hc->fd);
        hc->fd = 0;
    }
    if (hc->fname) {
        if (!warm_exit) {
            shm_unlink(hc->fname);
        }
        free(hc->fname);
        hc->fname = NULL;
    }

    return SHR_E_NONE;
}

static int
ha_file_name_get(int unit, char *buf, int buf_size)
{
    ha_ctrl_t *hc;

    if ((unit < 0) || (unit >= HA_MAX_UNITS)) {
        return SHR_E_UNIT;
    }
    hc = &ha_ctrl[unit];

    if (buf == NULL) {
        return SHR_E_PARAM;
    }

    if (hc->fname == NULL) {
        return SHR_E_UNAVAIL;
    }

    sal_snprintf(buf, buf_size, "%s%s", HA_SHARED_FILE_DIR, hc->fname);

    return SHR_E_NONE;
}

static int
ha_gen_file_name_get(char *buf, int buf_size)
{
    ha_ctrl_t *hc = &ha_ctrl[HA_MAX_UNITS];

    if (buf == NULL) {
        return SHR_E_PARAM;
    }

    if (hc->fname == NULL) {
        return SHR_E_UNAVAIL;
    }

    sal_snprintf(buf, buf_size, "%s%s",
                 HA_SHARED_FILE_DIR, hc->fname);

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
appl_ltsw_ha_unit_open(int unit, int size, int enabled, int warm)
{
    int rv;
    rv = ha_unit_open(unit, size, enabled, warm);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to initialize HA instance for DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    rv = ha_unit_open((unit + BCMDRD_CONFIG_MAX_UNITS), size, enabled, warm);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to initialize HA instance for BAE of DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}

int
appl_ltsw_ha_unit_close(int unit, int enabled, int warm_exit)
{
    int rv;

    if (!enabled) {
        return SHR_E_NONE;
    }

    rv = ha_unit_close(unit, warm_exit);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to close HA instance for DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    rv = ha_unit_close((unit + BCMDRD_CONFIG_MAX_UNITS), warm_exit);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to close HA instance for BAE of DRD dev %d - %s\n",
                unit, shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}

int
appl_ltsw_ha_gen_open(int size, int enabled, int warm)
{
    int rv;
    rv = ha_gen_open(size, enabled, warm);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to initialize HA shared gen files - %s\n",
                shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}

int
appl_ltsw_ha_gen_close(int enabled, int warm_exit)
{
    int rv;

    if (!enabled) {
        return SHR_E_NONE;
    }

    rv = ha_gen_close(warm_exit);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to close HA shared gen files - %s\n",
                shr_errmsg(rv));
        return rv;
    }

    return SHR_E_NONE;
}

int
appl_ltsw_ha_file_name_get(int inst, char *buf, int buf_size)
{
    return ha_file_name_get(inst, buf, buf_size);
}

int
appl_ltsw_ha_gen_file_name_get(char *buf, int buf_size)
{
    return ha_gen_file_name_get(buf, buf_size);
}

