/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	fileio.c
 * Purpose:	File I/O
 */

#include <sys/types.h>
#include <sys/stat.h>		/* mkdir */
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>

#include <sal/appl/io.h>
#include <sal/core/alloc.h>

#if defined(__STRICT_ANSI__)
#define NO_ENV_SUPPORT
#endif

int
sal_homedir_set(char *dir)
{
#ifndef NO_ENV_SUPPORT
    char *buf = NULL;
    if (dir != NULL) {
        if (dir[0] != '/') {
            return -1;
        }
        
        if ((buf = malloc(5 + strlen(dir) + 1)) == NULL) {
            return -1;
        }
        /* coverity[secure_coding] */
        sprintf(buf, "HOME=%s", dir);
        if (putenv(buf)) {
            free(buf);
            return -1;
        }
        
    }
#endif    
    return 0; 
    
}

char *
sal_homedir_get(char *buf, size_t size)
{
#ifndef NO_ENV_SUPPORT
    char	*s;

    if ((s = getenv("HOME")) != NULL) {
	strncpy(buf, s, size);
	buf[size - 2] = 0;
    } else {
        /* coverity[secure_coding] */
	strcpy(buf, "/");
    }

    if (buf[strlen(buf) - 1] != '/') {
        /* coverity[secure_coding] */
	strcat(buf, "/");
    }
#endif
    return buf;
}

/*
 * Modify umask for folder creation
 */
int sal_umask(int mask)
{
    return umask(mask);
}

/*
 * Get current working directory.
 *
 * NOTE: this version of cwd always includes the trailing slash so
 * a filename should be directly appended.
 */

char *sal_getcwd(char *buf, size_t size)
{
    if (getcwd(buf, size - 1) == NULL)
	return NULL;

    if (buf[strlen(buf) - 1] != '/')
        /* coverity[secure_coding] */
	strcat(buf, "/");

    return buf;
}

int sal_ls(char *f, char *flags)
{
    char cmd[256];
    /* coverity[secure_coding] */ 
    sprintf(cmd, "ls %s %s\n", flags ? flags : "", f);
    return(system(cmd));
}

int sal_cd(char *f)
{
#ifdef NO_ENV_SUPPORT
    return chdir("/");
#endif
    int rv;
    char *s = NULL;
    if (f == NULL) {
        f = getenv("HOME");
        if (f == NULL) {
            f = "/";
        } else {
            s = strdup(f);
            f = s;
        }
    }
    /* coverity[tainted_string] */
    rv = chdir(f);
    if (s) {
        free(s);
    }
    return rv;
}

FILE *
sal_fopen(char *file, char *mode)
/*
 * Function: 	sal_fopen
 * Purpose:	"fopen" a file.
 * Parameters:	name - name of file to open
 *		mode - file mode.
 * Returns:	NULL or FILE * pointer.
 */
{
    return fopen(file, mode);
}

int
sal_fclose(FILE *fp)
/*
 * Function: 	sal_fclose
 * Purpose:	Close a file opened with sal_fopen
 * Parameters:	fp - FILE pointer.
 * Returns:	non-zero on error
 */
{
    return fclose(fp);
}

int
sal_fread(void *buf, int size, int num, FILE *fp)
/*
 * Function: 	sal_fread
 * Purpose:	read() a file.
 * Parameters:	name - name of file to open
 *		mode - file mode.
 * Returns:	number of bytes read
 */
{
    return fread(buf, size, num, fp);
}

int
sal_feof(FILE *fp)
/*
 * Function: 	sal_feof
 * Purpose:	Return TRUE if EOF of a file is reached.
 * Parameters:	FILE * pointer.
 * Returns:	TRUE or FALSE
 */
{
    return feof(fp);
}

int
sal_ferror(FILE *fp)
/*
 * Function: 	sal_ferror
 * Purpose:	Return TRUE if an error condition for a file pointer is set.
 * Parameters:	FILE * pointer.
 * Returns:	TRUE or FALSE
 */
{
    return ferror(fp);
}

int
sal_fsize(FILE *fp)
/*
 * Function: 	sal_fsize
 * Purpose:	Return the size of a file if possible
 * Parameters:	FILE * pointer.
 * Returns:	File size or -1 in case of failure
 */
{
    int rv;
    
    if (0 != fseek(fp, 0, SEEK_END)) {
        return -1;
    } 
    rv = (int)ftell(fp);
    if (0 != fseek(fp, 0, SEEK_SET)) {
        return -1;
    }
    return rv < 0 ? -1 : rv;
}

int
sal_remove(char *f)
{
    return(unlink(f));
}

int
sal_rename(char *f_old, char *f_new)
{
    return rename(f_old, f_new);
}
char *
sal_dirname(char *path, char *dest)
{
    /**
     * we use here extra variable cause dirname/basename 
     * might change the input string,we use it staticly because 
     * we need it to be persistent outside the subroutine 
     */
    char p[SAL_NAME_MAX];
    sal_strncpy(p, path,sizeof(p)-1);
    sal_strcpy(dest,dirname(p));
    return dest;
}

char *
sal_basename(char *path, char *dest)
{
    /**
     * we use here extra variable cause dirname/basename 
     * might change the input string,we use it staticly because 
     * we need it to be persistent outside the subroutine 
     */
    char p[SAL_NAME_MAX];
    sal_strncpy(p, path,sizeof(p)-1);
    sal_strcpy(dest,basename(p));
    return dest;
} 

int
sal_mkdir(char *path)
{
    return mkdir(path, 0777);
}

int
sal_rmdir(char *path)
{
    return rmdir(path);
}

SAL_DIR *
sal_opendir(char *dirName)
{
    return (SAL_DIR *) opendir(dirName);
}

int
sal_closedir(SAL_DIR *dirp)
{
    return closedir((DIR *) dirp);
}

struct sal_dirent *
sal_readdir(SAL_DIR *dirp)
{
    static struct sal_dirent dir;
    struct dirent *d;

    if ((d = readdir((DIR *) dirp)) == NULL) {
	return NULL;
    }

    strncpy(dir.d_name, d->d_name, sizeof (dir.d_name));
    dir.d_name[sizeof (dir.d_name) - 1] = 0;

    return &dir;
}

void
sal_rewinddir(SAL_DIR *dirp)
{
    rewinddir((DIR *) dirp);
}
