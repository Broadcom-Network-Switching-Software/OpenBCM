# $Id: Makefile,v 1.2 2004/02/24 20:52:40 dtalayco Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Include standard Makefile configuration
#

include ${SDK}/make/Make.config
include ${SDK}/make/Make.depend

all::

install:: ${install_files}

${install_files}::
	${INSTALL} -d ${install_headers}/$(dir $@)
	${INSTALL} -m0644 -c $@ ${install_headers}/$(dir $@)

clean::

distclean::
	$(RM) ${install_headers}
