/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __COMMON_LINUX_KRN_GMODULE_H__
#define __COMMON_LINUX_KRN_GMODULE_H__

#include <lkm.h>

typedef struct gmodule_s {
  
    const char* name;
    int         major;
    int		minor; 

    int (*init)(void);
    int (*cleanup)(void);
  
    int (*pprint)(void);
  
    int (*open)(void);
    int (*ioctl)(unsigned int cmd, unsigned long arg);
    int (*close)(void);
    int (*mmap) (struct file *filp, struct vm_area_struct *vma);

} gmodule_t;
  

/* The framework will ask for your module definition */
extern gmodule_t* gmodule_get(void);


/* Proc Filesystem information */
extern int pprintf(const char* fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
extern int gmodule_vpprintf(char** page, const char* fmt, va_list args)
    __attribute__ ((format (printf, 2, 0)));
extern int gmodule_pprintf(char** page, const char* fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

extern int gprintk(const char* fmt, ...)
    __attribute__ ((format (printf, 1, 2)));

extern int gdbg(const char* fmt, ...)
    __attribute__ ((format (printf, 1, 2)));
#define GDBG gdbg

#endif /* __COMMON_LINUX_KRN_GMODULE_H__ */
