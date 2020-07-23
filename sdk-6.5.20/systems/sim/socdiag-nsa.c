/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef NO_SAL_APPL

/******************************************************************************
 *
 * About NO_SAL_APPL mode
 *
 * This configuration is built with NO_SAL_APPL defined. 
 *
 * In this build configuration, the Application SAL is not required to be compiled
 * or ported to the target operating system/environment. 
 *
 * There are 4 functions you must provide to hook the diag shell up to your
 * environment. These 4 functions are documented below. 
 * They are also implemented for the simulation target. 
 *
 * You should provide your own implementation of these functions
 * to import the diag shell into your environment. 
 */

/*
 * Function:
 *      sal_portable_vprintf
 * Purpose:
 *      Printf function used by the diag shell. 
 * Parameters:
 *      fmt -- format string
 *      vargs -- va_list
 *
 * Notes:
 *      Required for output from the shell
 *
 *      A version compatible with unix is provided here for
 *      the simulator build. Provide your own in your environment. 
 */
#include <shared/bsl.h>

#include <stdarg.h>     /* for va_list */
#include <stdio.h>      /* for vprintf */
#include <sal/appl/io.h> /* for prototype for these functions */

int 
sal_portable_vprintf(const char* fmt, va_list vargs)
{
    return vprintf(fmt, vargs); 
}



/*
 * Function:
 *      sal_portable_readline
 * Purpose:
 *      Retrieve an input line for the diag shell. 
 * Parameters:
 *      prompt -- prompt to be printed to the user at the input line
 *      buf    -- receive buffer for the line
 *      bufsize -- maximum size of the receive buffer
 * Returns:
 *      buf
 *
 * Notes:
 *      Required for input to the diag shell. 
 *      If you are only calling sh_process_command() and not 
 *      the diag shell input loop, this function will never get called
 *      and you can just return NULL.
 */

extern char* 
sal_portable_readline(char *prompt, char *buf, int bufsize)
{
    if(prompt) {
        printf("%s", prompt); 
    }
    if(buf && bufsize > 0) {
        buf[0] = 0; 
        fgets(buf, bufsize, stdin); 
        if(buf[strlen(buf)-1] == '\n') {
            buf[strlen(buf)-1] = 0;
        }
    }
    return buf; 
}


/******************************************************************************
 *
 * END OF PORTABILITY FUNCTIONS FOR BCM_PORTABLE_DIAG_MODE
 */




/******************************************************************************
 *
 * The rest of this file is concerned with providing an actual system main
 * application using the diag shell in portability mode. 
 * 
 * These are specific to the simulator platform and are not part of the 
 * portability functions required and described above. 
 */

#include <unistd.h>
#include <appl/diag/system.h> /* diag_shell */
#include <sal/appl/pci.h>     /* pci_device_iter */
#include <bde/pli/plibde.h>   /* bde_create */

ibde_t *bde;

int
bde_create(void)
{	
    return plibde_create(&bde);
}

/*
 * Main loop.
 */
int main(int argc, char *argv[])
{
    if (sal_core_init()) {
	LOG_CLI((BSL_META("SAL Core Initialization failed\n")));
	exit(1);
    }

    LOG_CLI((BSL_META("Broadcom Command Monitor: (NO_SAL_APPL build)\n"
                      "Copyright (c) 1998-2010 Broadcom Corp.\n")));
    LOG_CLI((BSL_META("Version %s built %s\n"), _build_release, _build_date));

    diag_shell();

    return 0;
}


/*
 * This function needed by the plibde implementation only
 */

#ifndef PCI_MAX_DEV
#define PCI_MAX_DEV 32
#endif

#ifndef PCI_MAX_BUS
#define PCI_MAX_BUS 1
#endif

int
pci_device_iter(int (*rtn)(pci_dev_t *dev,
			   uint16 pciVenID,
			   uint16 pciDevID,
			   uint8 pciRevID))
{
    uint32		venID, devID, revID;
    int			rv = 0;
    pci_dev_t		dev;
    
    dev.funcNo = 0;

    for (dev.busNo = 0;
	 dev.busNo < PCI_MAX_BUS && rv == 0;
	 dev.busNo++) {

	for (dev.devNo = 0;
	     dev.devNo < PCI_MAX_DEV && rv == 0;
	     dev.devNo++) {

	    if (dev.devNo == 12) {
		continue;
	    }

            /* NOTE -- pci_config_getw exported by plibde (not appl sal)  in this case */
	    venID = pci_config_getw(&dev, PCI_CONF_VENDOR_ID) & 0xffff;

	    if (venID == 0xffff) {
		continue;
	    }

	    devID = pci_config_getw(&dev, PCI_CONF_VENDOR_ID) >> 16;
	    revID = pci_config_getw(&dev, PCI_CONF_REVISION_ID) & 0xff;

	    rv = (*rtn)(&dev, venID, devID, revID);
	}
    }

    return rv;
}


#endif /* NO_SAL_APPL */
