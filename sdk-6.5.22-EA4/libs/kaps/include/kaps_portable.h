

#ifndef __KAPS_PORTABLE_H
#define __KAPS_PORTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif



#if defined(KAPS_BIG_ENDIAN) && defined(KAPS_LITTLE_ENDIAN)
#error cannot define both big and little endian
#endif



#ifdef KAPS_TRACE
#define KAPS_TRACE_IN(format, ...) \
    do {\
        kaps_printf("KAPS_TRACE:> %s " format, __FUNCTION__, __VA_ARGS__); \
        fflush(stdout);\
    } while (0)

#define KAPS_TRACE_OUT(format, ...) \
        do {\
            kaps_printf("KAPS_TRACE:< %s " format, __FUNCTION__, __VA_ARGS__); \
            fflush(stdout);\
        } while (0)

#define KAPS_TRACE_PRINT(format, ...) \
    do {\
        kaps_printf(format, __VA_ARGS__); \
        fflush(stdout);\
    } while (0)

#else
#define KAPS_TRACE_IN(format, ...)
#define KAPS_TRACE_OUT(format, ...)
#define KAPS_TRACE_PRINT(format, ...)
#endif





#if defined(__i386__) || defined(__alpha__)\
      || defined(__ia64) || defined(__ia64__)\
      || defined(__amd64) || defined(__amd64__)\
      || defined(__x86_64) || defined(__x86_64__)\
      || defined(__bfin__) || (!defined(__AARCH64EB__) && (defined(__arm)\
      || defined(__arm__) || defined(aarch64_) || defined(__aarch64__))) || defined(KAPS_LITTLE_ENDIAN)
#define __EO_33(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, AA, BB, CC, DD, EE, FF, GG) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V; W; X; Y; Z; AA; BB; CC; DD; EE; FF; GG
#define __EO_32(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, AA, BB, CC, DD, EE, FF) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V; W; X; Y; Z; AA; BB; CC; DD; EE; FF
#define __EO_25(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V; W; X; Y
#define __EO_24(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V; W; X
#define __EO_23(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V; W
#define __EO_22(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U; V
#define __EO_21(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T; U
#define __EO_20(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S; T
#define __EO_19(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R; S
#define __EO_18(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q; R
#define __EO_17(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P; Q
#define __EO_16(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) A; B; C; D; E; F; G; H; I; J; K; L; M; N; O; P
#define __EO_14(A, B, C, D, E, F, G, H, I, J, K, L, M, N) A; B; C; D; E; F; G; H; I; J; K; L; M; N
#define __EO_13(A, B, C, D, E, F, G, H, I, J, K, L, M) A; B; C; D; E; F; G; H; I; J; K; L; M
#define __EO_12(A, B, C, D, E, F, G, H, I, J, K, L) A; B; C; D; E; F; G; H; I; J; K; L
#define __EO_11(A, B, C, D, E, F, G, H, I, J, K) A; B; C; D; E; F; G; H; I; J; K
#define __EO_10(A, B, C, D, E, F, G, H, I, J) A; B; C; D; E; F; G; H; I; J
#define __EO_9(A, B, C, D, E, F, G, H, I) A; B; C; D; E; F; G; H; I
#define __EO_8(A, B, C, D, E, F, G, H) A; B; C; D; E; F; G; H
#define __EO_7(A, B, C, D, E, F, G) A; B; C; D; E; F; G
#define __EO_6(A, B, C, D, E, F) A; B; C; D; E; F
#define __EO_5(A, B, C, D, E) A; B; C; D; E
#define __EO_4(A, B, C, D) A; B; C; D
#define __EO_3(A, B, C) A; B; C
#define __EO_2(A, B) A; B

#define __KAPS_BYTESWAP_32(x) (x)
#define __KAPS_BYTESWAP_64(x) (x)
#define __KAPS_INPLACE_SWAP_32(x, n)
#define __KAPS_SWAP_4BYTES(x, n)
#define __KAPS_BYTESWAP_TWO_32B(x) (x)

#ifndef KAPS_LITTLE_ENDIAN
#define KAPS_LITTLE_ENDIAN
#endif


#elif defined(__sparc) || defined(__sparc__)\
    || defined(__powerpc__) || defined(__ppc__)\
    || defined(__hpux) || defined(__hppa)\
    || defined(__s390__) || defined(__mips)\
    || defined(__mips__) || defined(__AARCH64EB__) || defined(KAPS_BIG_ENDIAN)
#define __EO_33(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, AA, BB, CC, DD, EE, FF, GG) GG; FF; EE; DD; CC; BB; AA; Z; Y; X; W; V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_32(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, AA, BB, CC, DD, EE, FF) FF; EE; DD; CC; BB; AA; Z; Y; X; W; V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_25(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y) Y; X; W; V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_24(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X) X; W; V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_23(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W) W; V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_22(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V) V; U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_21(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U) U; T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_20(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T) T; S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_19(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S) S; R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_18(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R) R; Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_17(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q) Q; P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_16(A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P) P; O; N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_14(A, B, C, D, E, F, G, H, I, J, K, L, M, N) N; M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_13(A, B, C, D, E, F, G, H, I, J, K, L, M) M; L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_12(A, B, C, D, E, F, G, H, I, J, K, L) L; K; J; I; H; G; F; E; D; C; B; A
#define __EO_11(A, B, C, D, E, F, G, H, I, J, K) K; J; I; H; G; F; E; D; C; B; A
#define __EO_10(A, B, C, D, E, F, G, H, I, J) J; I; H; G; F; E; D; C; B; A
#define __EO_9(A, B, C, D, E, F, G, H, I) I; H; G; F; E; D; C; B; A
#define __EO_8(A, B, C, D, E, F, G, H) H; G; F; E; D; C; B; A
#define __EO_7(A, B, C, D, E, F, G) G; F; E; D; C; B; A
#define __EO_6(A, B, C, D, E, F) F; E; D; C; B; A
#define __EO_5(A, B, C, D, E) E; D; C; B; A
#define __EO_4(A, B, C, D) D; C; B; A
#define __EO_3(A, B, C) C; B; A
#define __EO_2(A, B) B; A

#ifndef KAPS_BIG_ENDIAN
#define KAPS_BIG_ENDIAN
#endif


#define __KAPS_BYTESWAP_64(x)\
   ((((x) & 0xFF00000000000000ull) >> 56) |   \
    (((x) & 0x00FF000000000000ull) >> 40) |   \
    (((x) & 0x0000FF0000000000ull) >> 24) |   \
    (((x) & 0x000000FF00000000ull) >> 8)  |   \
    (((x) & 0x00000000FF000000ull) << 8)  |   \
    (((x) & 0x0000000000FF0000ull) << 24) |   \
    (((x) & 0x000000000000FF00ull) << 40) |   \
    (((x) & 0x00000000000000FFull) << 56));


#define __KAPS_BYTESWAP_TWO_32B(x)\
   ((((x) & 0xFF00000000000000ull) >> 24) |   \
    (((x) & 0x00FF000000000000ull) >> 8)  |   \
    (((x) & 0x0000FF0000000000ull) << 8)  |   \
    (((x) & 0x000000FF00000000ull) << 24) |   \
    (((x) & 0x00000000FF000000ull) >> 24) |   \
    (((x) & 0x0000000000FF0000ull) >> 8)  |   \
    (((x) & 0x000000000000FF00ull) << 8)  |   \
    (((x) & 0x00000000000000FFull) << 24));



#define __KAPS_BYTESWAP_32(x)\
   ((((x) & 0xFF000000ul) >> 24) |   \
    (((x) & 0x00FF0000ul) >> 8)  |   \
    (((x) & 0x0000FF00ul) << 8)  |   \
    (((x) & 0x000000FFul) << 24));


#define __KAPS_INPLACE_SWAP_32(x, n)\
    (((uint32_t *) (&x))[n]) = __KAPS_BYTESWAP_32((((uint32_t *) (&x))[n]))


#define __KAPS_SWAP_4BYTES(x, n)\
    {uint8_t tmp; tmp = x[n]; x[n] = x[n+3]; x[n+3] = tmp; tmp = x[n+1]; x[n+1] = x[n+2]; x[n+2] = tmp;}


#endif


struct kaps_timeval {
    long tv_sec;                
    long tv_ns;                 
};

typedef long kaps_time_t;  


struct kaps_tm {
    int32_t sec;   
    int32_t min;   
    int32_t hour;  
    int32_t mday;  
    int32_t month; 
    int32_t year;  
    int32_t wday;  
    int32_t yday;  
    int32_t isdst; 
};


struct kaps_mutex {
    char mutex_opaque_type; 
};


int32_t kaps_assert_detail(const char *msg, const char *file, int32_t line);


int32_t kaps_assert_detail_or_error(const char *msg, uint32_t return_error, uint32_t error_code, const char *file, int32_t line);


int32_t kaps_identity(const int32_t b);

#define KAPS_CAST_PTR_TO_U32(x) ((uint32_t) (uintptr_t) x) 
#define KAPS_CAST_U32_TO_PTR(x)    (void*) ((uintptr_t) x) 


#define kaps_assert(cond, msg) ((void) ((cond) || kaps_assert_detail((msg), __FILE__, __LINE__))) 
#define kaps_sassert(cond)     ((void) ((cond) || kaps_assert_detail((" "), __FILE__, __LINE__))) 
#define kaps_assert_(b, m)     (kaps_identity(b) ? 0 : kaps_assert_detail(m, __FILE__, __LINE__)), 

#define KAPS_ASSERT_OR_ERROR(cond, msg, return_error, error_code)                                           \
    do {                                                                                                   \
        if (!(cond)) {                                                                                     \
            KAPS_STRY(kaps_assert_detail_or_error((msg), (return_error), (error_code), __FILE__, __LINE__)); \
        }                                                                                                  \
    } while (0) 



#if 1
#define  KAPS_TX_ERROR_CHECK(dev, status)                                                                        \
         (void) dev;                                                                                             \
         (void) status;
#else
#define  KAPS_TX_ERROR_CHECK(dev, status)                                                                        \
    if (dev->main_bc_device)                                                                                    \
        dev = dev->main_bc_device;                                                                              \
    if (dev->main_dev)                                                                                          \
        dev = dev->main_dev;                                                                                    \
    if (dev->fatal_transport_error || dev->check_fatal_dbg_0 || dev->check_fatal_dbg_1)   {                     \
        kaps_printf("\n ____________________________________________________________________________________\n");\
        kaps_printf("\n Transport error !!!  (check0: %d error: %d check1: %d)",                                 \
            dev->check_fatal_dbg_0, dev->fatal_transport_error, dev->check_fatal_dbg_1);                        \
        if (status)                                                                                             \
            kaps_printf("\n Status   :  Reason Code: %d  [ %s ]  ", status, kaps_get_status_string(status));      \
        kaps_printf("\n File     : %s  \n Function : %s  \n LineNo   : %u  ", __FILE__, __FUNCTION__, __LINE__); \
        kaps_printf("\n ____________________________________________________________________________________\n");\
    }
#endif

#ifdef WIN32
#define kaps_concat_identity(a)  a
#define kaps_concat_2(a, b)      kaps_concat_identity(a) kaps_concat_identity(b)
#define kaps_concat_3(a, b, c)   kaps_concat_identity(a) kaps_concat_identity(b) kaps_concat_identity(c)
#else
#define kaps_concat_2(a, b)      a##b              
#define kaps_concat_3(a, b, c)   a##b##c           
#endif


#define kaps_stdout   stdout


#define kaps_stderr   stderr


#define kaps_fopen    fopen


#define kaps_eof      (EOF)



void *kaps_memset(void *str, int val, uint32_t size);


void *kaps_memcpy(void *dest, const void *src, uint32_t size);


int kaps_memcmp(const void *s1, const void *s2, uint32_t size);


void *kaps_memmove(void *dstp, const void *srcp, uint32_t numOfBytes);


void *kaps_sysmalloc(uint32_t size);


void *kaps_syscalloc(uint32_t nmemblk, uint32_t size);


void kaps_sysfree(void *ptr);


void kaps_abort(void);


int kaps_sprintf(char *buff, const char *fmt, ...);


int kaps_vsnprintf(char *buff, uint32_t buffsize, const char *fmt, va_list ap);


int kaps_snprintf(char *buff, uint32_t buffsize, const char *fmt, ...);


int kaps_printf(const char *fmt, ...);


int kaps_fputs(const char *str, FILE *fp);


int kaps_vprintf(const char *fmt, va_list arg);


int kaps_fprintf(FILE * fp, const char *fmt, ...);


int kaps_fscanf(FILE * fp, const char *fmt, ...);


int kaps_fseek(FILE * fp, int32_t offset, int whence);



void kaps_enable_error_box(int32_t enable);


int kaps_gettimeofday(struct kaps_timeval *tv);


int kaps_gettimerval(struct kaps_timeval * tv);


kaps_time_t kaps_time(kaps_time_t *currtime);


struct kaps_tm *kaps_localtime(kaps_time_t *currtime, struct kaps_tm *timeinfo);


char *kaps_getenv(const char *name);


void kaps_srandom(uint32_t seed);


int kaps_random(void);


int kaps_random_r(uint32_t *seedp);




FILE *kaps_open_file(const char *fname);



char *kaps_strsep(char **stringp, const char *delim);



int kaps_fclose(FILE * fp);



void kaps_usleep(uint32_t usecs);




void *kaps_mmap(void *addr, uint32_t length, int prot, int flags,
               int fd, uint64_t offset);



int kaps_munmap(void *addr, uint32_t length);



int kaps_open(const char *pathname, int flags, uint32_t mode);


int kaps_vfprintf(FILE * fp, const char *fmt, va_list ap);


int kaps_mutex_init(struct kaps_mutex **mutex);


int kaps_mutex_destroy(struct kaps_mutex *mutex);


int kaps_mutex_lock(struct kaps_mutex *mutex);


int kaps_mutex_trylock(struct kaps_mutex *mutex);


int kaps_mutex_unlock(struct kaps_mutex *mutex);



#ifdef KAPS_UNDEF_FUN
extern int rand_r(unsigned *);
extern unsigned int usleep(unsigned int usecs);
extern int ioctl(int, int, ...);
#endif




#ifdef __cplusplus
}
#endif
#endif                          
