#
# Copyright 2016, Data 61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

CURRENT_DIR := $(dir $(abspath $(lastword ${MAKEFILE_LIST})))
LDFLAGS += -L$(CURRENT_DIR)out/static_lib

# Provides a default install target to avoid breaking legacy
# applications, now that such a target is required by build-rootfs
install:
	find -\( -name '.git' -prune -\) -or -type f -executable -exec cp -v {} ${SBIN_PATH} \;
# Find all executable files which are not in a .git directory and copy them into $OUT/$ROOTFS_TMP/usr/sbin
