/*! \file bcmlu_mmap.c
 *
 * When running 32-bit applications on a 64-bit platform, we need to
 * use a special version of mmap in order to map physical addresses
 * larger than 32 bits.
 *
 * Depending on the system capabilities we use either mamp64 or mmap2.
 *
 * Note that while mmap64 supports the full 64-bit physical address
 * space, mmap2 only supports 44-bit physical addresses.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include <bsl/bsl.h>

#include <bcmlu/bcmlu_mmap.h>

/*! Alloc override of the default system page size for mmap2. */
#ifndef BCMLU_MMAP2_PAGESIZE
#define BCMLU_MMAP2_PAGESIZE    sysconf(_SC_PAGESIZE)
#endif

#ifndef BCMLU_USE_MMAP2
/*!
 * Several 64-bit platforms define SYS_mmap2, but not __NR_mmap2, so
 * we check for __NR_mmap2 instead.
 */
#if (UINTPTR_MAX == 0xffffffff) && defined(__NR_mmap2)
#define BCMLU_USE_MMAP2         1
#else
#define BCMLU_USE_MMAP2         0
#endif
#endif /* BCMLU_USE_MMAP2 */

#ifndef BCMLU_USE_MMAP64
/*!
 * mmap64 is the preferred method of mapping physical addresses from a
 * 32-bit application.
 */
#if (UINTPTR_MAX == 0xffffffff) && defined(__USE_LARGEFILE64)
#define BCMLU_USE_MMAP64        1
#else
#define BCMLU_USE_MMAP64        0
#endif
#endif /* BCMLU_USE_MMAP64 */

const char *
bcmlu_mmap_type_str(void)
{
#if (BCMLU_USE_MMAP64 == 1)
    return "mmap64";
#elif (BCMLU_USE_MMAP2 == 1)
    return "mmap2";
#else
    return "mmap";
#endif
}

void *
bcmlu_mmap(int fd, uint64_t offset, size_t size)
{
    void *map;

#if (BCMLU_USE_MMAP64 == 1)
    map = mmap64(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_SHARED, fd, offset);
#elif (BCMLU_USE_MMAP2 == 1)
    long pgsz = BCMLU_MMAP2_PAGESIZE;
    off_t mmap_offset = offset / pgsz;
    if ((uint64_t)mmap_offset != (offset / pgsz)) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Physical address 0x%"PRIx64" "
                            "out-out-range for mmap2\n"),
                   offset));
        return NULL;
    }
    map = (void *)syscall(__NR_mmap2,
                          NULL, size, PROT_READ | PROT_WRITE,
                          MAP_SHARED, fd, mmap_offset);
#else
    off_t mmap_offset = offset;
    if ((uint64_t)mmap_offset != offset) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Physical address 0x%"PRIx64" "
                            "out-out-range for mmap\n"),
                   offset));
        return NULL;
    }
    map = mmap(NULL, size, PROT_READ | PROT_WRITE,
               MAP_SHARED, fd, mmap_offset);
#endif
    if (map == MAP_FAILED) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Failed to map 0x%08"PRIx64"/0x%zx via %s: %s\n"),
                   offset, size, bcmlu_mmap_type_str(), strerror(errno)));
        map = NULL;
    }
    return map;
}

int
bcmlu_munmap(void *addr, size_t size)
{
    int rv;

    rv = munmap(addr, size);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SYS_PCI,
                  (BSL_META("Failed to unmap %p/0x%zx via %s: %s\n"),
                   addr, size, bcmlu_mmap_type_str(), strerror(errno)));
    }
    return rv;
}
