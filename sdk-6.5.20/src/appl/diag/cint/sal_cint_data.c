/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * sal_cint_data.c
 *
 * Hand-coded support for a few SAL core routines. 
 */

typedef int sal_core_cint_data_not_empty; /* Make ISO compilers happy. */
#include <sdk_config.h>

#if defined(INCLUDE_LIB_CINT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cint_logger.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/core/thread.h>
#include <sal/appl/config.h>

#include <sal/appl/sal.h>



/*
 * Explicit wrappers as these may be macros instead of functions
 */
static void* __memset(void* p, int c, int sz)
{
    return sal_memset(p, c, sz); 
}
static void* __memcpy(void* dst, void* src, int sz)
{
    return sal_memcpy(dst, src, sz); 
}
static int __memcmp(void* dst, void* src, int sz)
{
    return sal_memcmp(dst, src, sz);
}
static int __strcmp(char* p, char* c)
{
    return sal_strcmp(p, c);
}
static int __strncmp(char* p, char* c, int sz)
{
    return sal_strncmp(p, c, sz);
}
static char* __strcpy(char* dst, char* src)
{
    return sal_strcpy(dst, src);
}
static char* __strncpy(char* dst, char* src, int sz)
{
    return sal_strncpy(dst, src, sz);
}
static char* __strncpy_s(char* dst, char* src, int sz)
{
        return sal_strncpy_s(dst, src, sz);
}
static char* __strstr(char* p, char* c)
{
    return sal_strstr(p, c);
}
static int __strlen(char* c)
{
    return sal_strlen(c);
}
static int __strnlen(char* c, int sz)
{
        return sal_strnlen(c, sz);
}
static int __rand(void)
{
    return sal_rand();
}
static int __srand(int p)
{
    sal_srand(p);
    return p;
}
static double __sal_time(void)
{
    return sal_time();
}
static unsigned int __sal_time_usecs(void)
{
    return sal_time_usecs();
}
static void __sleep(int sec)
{
    sal_sleep(sec);
}
static void __usleep(unsigned int usec)
{
    sal_usleep(usec);
}
static void __udelay(unsigned int usec)
{
    sal_udelay(usec);
}

void *cint_sal_alloc(unsigned int size, char *name)
{
    return sal_alloc(size,  name);
}
void cint_sal_free(void *ptr)
{
    sal_free(ptr);
}

CINT_FWRAPPER_CREATE_RP2(void*, void, 1, 0,
                         cint_sal_alloc,
                         int,int,size,0,0,
                         void*,void,name,1,0);

CINT_FWRAPPER_CREATE_VP1(cint_sal_free,
                         void*,void,p,1,0);

CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         __memset,
                         void*,void,p,1,0,
                         int,int,c,0,0,
                         int,int,sz,0,0); 

CINT_FWRAPPER_CREATE_RP3(void*,void,1,0,
                         __memcpy,
                         void*,void,dst,1,0,
                         void*,void,src,1,0,
                         int,int,sz,0,0);
CINT_FWRAPPER_CREATE_RP3(int,int,0,0,
                         __memcmp,
                         void*,void,dst,1,0,
                         void*,void,src,1,0,
                         int,int,sz,0,0);
CINT_FWRAPPER_CREATE_RP2(int,int,1,0,
                         __strcmp,
                         char*,char,p,1,0,
                         char*,char,c,1,0);
CINT_FWRAPPER_CREATE_RP3(int,int,1,0,
                         __strncmp,
                         char*,char,p,1,0,
                         char*,char,c,1,0,
                         int,int,sz,0,0);

CINT_FWRAPPER_CREATE_RP2(char*,char,1,0,
                         __strcpy,
                         char*,char,dst,1,0,
                         char*,char,src,1,0);
CINT_FWRAPPER_CREATE_RP3(char*,char,1,0,
                         __strncpy,
                         char*,char,dst,1,0,
                         char*,char,src,1,0,
                         int,int,sz,0,0);
CINT_FWRAPPER_CREATE_RP3(char*,char,1,0,
                         __strncpy_s,
                         char*,char,dst,1,0,
                         char*,char,src,1,0,
                         int,int,sz,0,0);
CINT_FWRAPPER_CREATE_RP2(char*,char,1,0,
                         __strstr,
                         char*,char,p,1,0,
                         char*,char,c,1,0);
CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         __strlen,
                         char*,char,c,1,0);
CINT_FWRAPPER_CREATE_RP2(int,int,0,0,
                         __strnlen,
                         char*,char,c,1,0,
                         int,int,sz,0,0);
CINT_FWRAPPER_CREATE_RP0(int,int,0,0,
                         __rand);
CINT_FWRAPPER_CREATE_RP0(int,int,0,0,
                         __sal_time);
CINT_FWRAPPER_CREATE_RP0(unsigned int,unsigned int,0,0,
                         __sal_time_usecs);
CINT_FWRAPPER_CREATE_RP1(int,int,0,0,
                         __srand,
                         int,int,p,0,0);
CINT_FWRAPPER_CREATE_VP1(__sleep,
                         int,int,sec,0,0);
CINT_FWRAPPER_CREATE_VP1(__usleep,
                         unsigned int,unsigned int,usec,0,0);
CINT_FWRAPPER_CREATE_VP1(__udelay,
                         unsigned int,unsigned int,usec,0,0);
CINT_FWRAPPER_CREATE_RP2(int,int,1,0,
                         sal_config_set,
                         char*,char,name,1,0,
                         char*,char,value,1,0);  

/* cmicx diag additions */

CINT_FWRAPPER_CREATE_RP1(char*,char,1,0,
                         sal_config_get,
                         char*,char,name,1,0);

/* end cmicx diag additions */


/*
 * COMPILER_64 macros
 */
static void __COMPILER_64_TO_32_LO(int* dst, uint64 src) { COMPILER_64_TO_32_LO(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_TO_32_LO, int*,int,dst,1,0,uint64,uint64,src,0,0); 

static void __COMPILER_64_TO_32_HI(int* dst, uint64 src) { COMPILER_64_TO_32_HI(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_TO_32_HI, int*,int,dst,1,0,uint64,uint64,src,0,0); 

static int __COMPILER_64_HI(uint64* src) { return COMPILER_64_HI(*src); }
CINT_FWRAPPER_CREATE_RP1(int,int,0,0,__COMPILER_64_HI,uint64*,uint64,src,1,0); 

static unsigned int __COMPILER_64_LO(uint64* src) { return COMPILER_64_LO(*src); }
CINT_FWRAPPER_CREATE_RP1(unsigned int,unsigned int,0,0,__COMPILER_64_LO,uint64*,uint64,src,1,0); 

static void __COMPILER_64_ZERO(uint64* dst) { COMPILER_64_ZERO(*dst); }
CINT_FWRAPPER_CREATE_VP1(__COMPILER_64_ZERO, uint64*, uint64, dst, 1, 0); 

static int __COMPILER_64_IS_ZERO(uint64* src) { return COMPILER_64_IS_ZERO(*src); }
CINT_FWRAPPER_CREATE_RP1(int,int,0,0,__COMPILER_64_IS_ZERO,uint64*,uint64,src,1,0); 

static void __COMPILER_64_SET(uint64* dst, uint32 hi, uint32 lo) { COMPILER_64_SET(*dst, hi, lo); }
CINT_FWRAPPER_CREATE_VP3(__COMPILER_64_SET, 
                         uint64*, uint64, dst, 1, 0, 
                         uint32, uint32, hi, 0, 0, 
                         uint32, uint32, lo, 0, 0); 

static void __COMPILER_64_UDIV_64(uint64* dst, uint64 src) { COMPILER_64_UDIV_64(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_UDIV_64, uint64*,uint64,dst,1,0,uint64,uint64,src,0,0);

static void __COMPILER_64_UDIV_32(uint64* dst, uint32 src) { COMPILER_64_UDIV_32(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_UDIV_32, uint64*,uint64,dst,1,0,uint32,uint32,src,0,0);

static void __COMPILER_64_UMUL_32(uint64* dst, uint32 src) { COMPILER_64_UMUL_32(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_UMUL_32, uint64*,uint64,dst,1,0,uint32,uint32,src,0,0);

static void __COMPILER_64_ADD_64(uint64* dst, uint64 src) { COMPILER_64_ADD_64(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_ADD_64, uint64*,uint64,dst,1,0,uint64,uint64,src,0,0);

static void __COMPILER_64_ADD_32(uint64* dst, uint32 src) { COMPILER_64_ADD_32(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_ADD_32, uint64*,uint64,dst,1,0,uint32,uint32,src,0,0);

static void __COMPILER_64_SUB_64(uint64* dst, uint64 src) { COMPILER_64_SUB_64(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_SUB_64, uint64*,uint64,dst,1,0,uint64,uint64,src,0,0);

static void __COMPILER_64_SUB_32(uint64* dst, uint32 src) { COMPILER_64_SUB_32(*dst, src); }
CINT_FWRAPPER_CREATE_VP2(__COMPILER_64_SUB_32, uint64*,uint64,dst,1,0,uint32,uint32,src,0,0);

static cint_function_t __cint_sal_functions[] = 
    {
        CINT_FWRAPPER_NENTRY("sal_alloc", cint_sal_alloc),
        CINT_FWRAPPER_NENTRY("sal_free", cint_sal_free),
        CINT_FWRAPPER_NENTRY("sal_memset", __memset), 
        CINT_FWRAPPER_NENTRY("sal_memcpy", __memcpy),
        CINT_FWRAPPER_NENTRY("sal_memcmp", __memcmp),
        CINT_FWRAPPER_NENTRY("sal_strcpy", __strcpy),
        CINT_FWRAPPER_NENTRY("sal_strncpy", __strncpy),
        CINT_FWRAPPER_NENTRY("sal_strncpy_s", __strncpy_s),
        CINT_FWRAPPER_NENTRY("sal_strcmp", __strcmp),
        CINT_FWRAPPER_NENTRY("sal_strncmp", __strncmp),
        CINT_FWRAPPER_NENTRY("sal_strstr", __strstr),
        CINT_FWRAPPER_NENTRY("sal_strlen", __strlen),
        CINT_FWRAPPER_NENTRY("sal_strnlen", __strnlen),
        CINT_FWRAPPER_NENTRY("sal_rand", __rand),
        CINT_FWRAPPER_NENTRY("sal_time", __sal_time),
        CINT_FWRAPPER_NENTRY("sal_time_usecs", __sal_time_usecs),
        CINT_FWRAPPER_NENTRY("sal_srand", __srand),
        CINT_FWRAPPER_NENTRY("sal_sleep", __sleep),
        CINT_FWRAPPER_NENTRY("sal_usleep", __usleep),
        CINT_FWRAPPER_NENTRY("sal_udelay", __udelay),
        CINT_FWRAPPER_NENTRY("COMPILER_64_TO_32_LO", __COMPILER_64_TO_32_LO), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_TO_32_HI", __COMPILER_64_TO_32_HI), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_HI", __COMPILER_64_HI), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_LO", __COMPILER_64_LO), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_ZERO", __COMPILER_64_ZERO), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_IS_ZERO", __COMPILER_64_IS_ZERO), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_SET", __COMPILER_64_SET), 
        CINT_FWRAPPER_NENTRY("COMPILER_64_UDIV_64", __COMPILER_64_UDIV_64),
        CINT_FWRAPPER_NENTRY("COMPILER_64_UDIV_32", __COMPILER_64_UDIV_32),
        CINT_FWRAPPER_NENTRY("COMPILER_64_UMUL_32", __COMPILER_64_UMUL_32),
        CINT_FWRAPPER_NENTRY("COMPILER_64_ADD_32", __COMPILER_64_ADD_32),
        CINT_FWRAPPER_NENTRY("COMPILER_64_ADD_64", __COMPILER_64_ADD_64),
        CINT_FWRAPPER_NENTRY("COMPILER_64_SUB_32", __COMPILER_64_SUB_32),
        CINT_FWRAPPER_NENTRY("COMPILER_64_SUB_64", __COMPILER_64_SUB_64),
        CINT_FWRAPPER_ENTRY(sal_config_set),

        /* cmicx diag additions */

        CINT_FWRAPPER_ENTRY(sal_config_get),

        /* end cmicx diag additions */

        CINT_ENTRY_LAST
    }; 


/* Typedefs section */
static cint_parameter_desc_t __cint_typedefs[] =
{
    {"uint8", "sal_mac_addr_t" , 0 , 6},
    { NULL } 
};

cint_data_t sal_cint_data = 
    {
        NULL,
        __cint_sal_functions,
        NULL, 
        NULL, 
        __cint_typedefs,
        NULL, 
        NULL,
        NULL, 
        NULL
    }; 

/*
 * This is the print function used by CINT. 
 *
 * The purpose of this function is to set the SAL main thread to avoid background thread prefixes during output. 
 */

int cint_printk(const char* fmt, ...)
{
    int rv; 
    va_list args;
    sal_thread_t t; 
#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
    va_list args2;
    int ototal, obuf_size = 1024;
    char obuf[obuf_size], *obuf2;
    cint_logger_thread_data_t *tdata = cint_logger_thread_specific();
    unsigned int bsl_ls;
#endif

    t = sal_thread_main_get(); 
    sal_thread_main_set(sal_thread_self()); 

    va_start(args, fmt);

#if CINT_CONFIG_INCLUDE_CINT_LOGGER == 1
    if ((*tdata).logger_is_active) {

        bsl_ls = BSL_LAYER_ENC(cint_logger_cfg.logLayer) | BSL_SOURCE_ENC(cint_logger_cfg.logSource);

        va_copy(args2, args);

        ototal = vsnprintf(obuf, obuf_size, fmt, args2);

        if (ototal >= obuf_size) {

            obuf_size = ototal + 1;

            obuf2 = sal_alloc(obuf_size, "cint_printk output buffer - 2nd attempt");

            ototal = vsnprintf(obuf2, obuf_size, fmt, args);

            if (ototal >= obuf_size) {
               /* assert(); -- This shouldn't happen */
            }

            rv = bsl_printf("<c=%u>%s", bsl_ls | BSL_VERBOSE, obuf2);

            sal_free(obuf2);

        } else {

            rv = bsl_printf("<c=%u>%s", bsl_ls | BSL_VERBOSE, obuf);

        }

        va_end(args2);

    } else
#endif
    {
        rv = sal_vprintf(fmt, args); 
    }

    va_end(args); 

    sal_thread_main_set(t); 
    return rv; 
}

#endif /* INCLUDE_LIB_CINT */

    

