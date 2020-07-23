/*! \file bcma_ha_mem.c
 *
 * HA memory driver based on POSIX shm API..
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmha/bcmha_mem.h>
#include <bcmlt/bcmlt.h>

#include <bcma/ha/bcma_ha.h>

#define BSL_LOG_MODULE BSL_LS_APPL_HA

/*******************************************************************************
 * Local definitions
 */

/*! Shared file default location. */
#ifndef BCMA_HA_SHARED_FILE_DIR
#define BCMA_HA_SHARED_FILE_DIR         "/dev/shm"
#endif

/*! Shared file name before prefixes and postfixes. */
#ifndef BCMA_HA_SHARED_FILENAME
#define BCMA_HA_SHARED_FILENAME         "sdk_ha"
#endif

/* Shared file control */
typedef struct ha_ctrl_s {
    char *fname;
    int fd;
    uint32_t mapped_size;
} ha_ctrl_t;

/*******************************************************************************
 * Local data
 */

/* Shared file control for all units + generic instance */
static ha_ctrl_t ha_ctrl[BCMDRD_CONFIG_MAX_UNITS + 1];

/*******************************************************************************
 * Private functions
 */

/*
 * Callback to allocate new HA memory pool.
 *
 * This is done by increasing the shared memory file by the requested
 * size (syspage-aligned), and mapping the new file section into the
 * logical memory space.
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
 * Callback for freeing HA memory pool.
 *
 * This is done by unmapping the associated shared file section from
 * the logical memory space.
 */
static void
ha_pool_free(void *ctx, void *mmap_ptr, uint32_t len)
{
    munmap(mmap_ptr, len);
}

/*
 * This function retrieves the username of the current user and
 * appends it to the provided print buffer.
 */
static void
append_uname(shr_pb_t *pb)
{
    struct passwd pwd;
    struct passwd *result;
    char *pwbuf;
    size_t pwbufsz;
    int rv;

    pwbufsz = 2 * 1024;
    pwbuf = sal_alloc(pwbufsz, "bcmaHaUsername");
    while (1) {
        if (pwbuf == NULL) {
            break;
        }
        rv = getpwuid_r(getuid(), &pwd, pwbuf, pwbufsz, &result);
        if (result) {
            /* Append current username */
            shr_pb_printf(pb, "%s", result->pw_name);
            sal_free(pwbuf);
            return;
        }
        if (rv == ERANGE) {
            pwbufsz <<= 1;
            sal_free(pwbuf);
            pwbuf = malloc(pwbufsz);
        } else if (rv < 0) {
            sal_free(pwbuf);
            break;
        }
    }

    /* Unable to retrieve current username */
    shr_pb_printf(pb, "nouser");
}

/*
 * This function constructs the filename for a shared file.
 *
 * For a cold boot it will create a unique name that does not already
 * exist. For a warm boot it will create the generic name (based on
 * the username).
 *
 * The filename will be of the format:
 *
 *   /<username>_sdk_ha_<unit>_<instance>
 *
 * If the unit number is negative, then "<unit>" will be replaced by
 * "gen" (for generic).
 *
 * If the instance is negative, then "_<instance>" will be omitted.
 */
static int
build_filename(int unit, ha_ctrl_t *hc, bool warm, int instance)
{
    int rv = SHR_E_NONE;
    shr_pb_t *pb;
    struct stat f_info;
    const char *fname;
    int pb_dir_len;

    pb = shr_pb_create();

    shr_pb_printf(pb, "%s", BCMA_HA_SHARED_FILE_DIR);

    /* Save length of base directory */
    pb_dir_len = sal_strlen(shr_pb_str(pb));

    shr_pb_printf(pb, "/");
    append_uname(pb);

    shr_pb_printf(pb, "_%s", BCMA_HA_SHARED_FILENAME);

    if (unit < 0) {
        shr_pb_printf(pb, "_gen");
    } else {
        shr_pb_printf(pb, "_%d", unit);
    }

    if (instance >= 0) {
        shr_pb_printf(pb, "_x%x", instance);
    }

    /* Do not overwrite existing instance on cold boot */
    if (!warm && instance >= 0) {
        if (stat(shr_pb_str(pb), &f_info) == 0) {
            /* Filename exists */
            rv = SHR_E_PARAM;
        } else if (errno != ENOENT) {
            rv = SHR_E_INTERNAL;
        }
    }

    if (SHR_SUCCESS(rv)) {
        /* Save filename without the base directory */
        if (hc->fname) {
            sal_free(hc->fname);
        }
        fname = shr_pb_str(pb);
        hc->fname = sal_strdup(&fname[pb_dir_len]);
    }
    shr_pb_destroy(pb);
    return rv;
}

static int
open_ha_file(char *fname, int size, bool warm,
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
                      (BSL_META("Error opening file %s\n"), fname));
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

/*******************************************************************************
 * Public functions
 */

int
bcma_ha_mem_init(int unit, int size, bool warm, int inst)
{
    ha_ctrl_t *hc;
    int rv;
    int fh = -1;
    void *mmap_ptr = NULL;
    long file_size = 0;

    if (unit == BCMHA_GEN_UNIT) {
        hc = &ha_ctrl[BCMDRD_CONFIG_MAX_UNITS];
    } else if (BCMDRD_UNIT_VALID(unit)) {
        hc = &ha_ctrl[unit];
    } else {
        return SHR_E_UNIT;
    }
    if (hc->fd > 0) {
        return SHR_E_INIT;
    }
    hc->mapped_size = 0;

    rv = build_filename(unit, hc, warm, inst);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = open_ha_file(hc->fname, size, warm, &fh, &file_size,
                      &mmap_ptr, &hc->mapped_size);
    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to open HA file %s\n"),
                   hc->fname));
        sal_free(hc->fname);
        hc->fname = NULL;
        return rv;
    }
    hc->fd = fh;

    return bcmha_mem_init(unit, ha_pool_alloc, ha_pool_free, hc,
                          mmap_ptr, (int)file_size);
}

int
bcma_ha_mem_cleanup(int unit, bool warm_next)
{
    ha_ctrl_t *hc;
    int rv;

    if (unit == BCMHA_GEN_UNIT) {
        hc = &ha_ctrl[BCMDRD_CONFIG_MAX_UNITS];
    } else if (BCMDRD_UNIT_VALID(unit)) {
        hc = &ha_ctrl[unit];
    } else {
        return SHR_E_UNIT;
    }

    rv = bcmha_mem_cleanup(unit);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    if (hc->fd > 0) {
        close(hc->fd);
        hc->fd = 0;
    }
    if (hc->fname) {
        if (!warm_next) {
            shm_unlink(hc->fname);
        }
        sal_free(hc->fname);
        hc->fname = NULL;
    }

    return SHR_E_NONE;
}

int
bcma_ha_mem_name_get(int unit, int buf_size, char *buf)
{
    ha_ctrl_t *hc;

    if (unit < 0) {
        hc = &ha_ctrl[BCMDRD_CONFIG_MAX_UNITS];
    } else if (BCMDRD_UNIT_VALID(unit)) {
        hc = &ha_ctrl[unit];
    } else {
        return SHR_E_UNIT;
    }

    if (buf == NULL) {
        return SHR_E_PARAM;
    }

    if (hc->fname == NULL) {
        return SHR_E_UNAVAIL;
    }

    sal_snprintf(buf, buf_size, "%s%s", BCMA_HA_SHARED_FILE_DIR, hc->fname);

    return SHR_E_NONE;
}
