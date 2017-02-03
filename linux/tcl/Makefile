#
# Copyright 2017, NICTA
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(NICTA_GPL)
#

# This Makefile generates a base rootfs for Tiny Core Linux, as well
# as a slightly modified build-rootfs script.

all: build-rootfs config/linux_build_config config/Module.symvers bzimage rootfs-bare.cpio

clean:
	rm -rf extr.*/
	rm -f build-rootfs rootfs-bare.cpio rootfs.cpio

distclean: clean
	rm -f *.tcz core.gz bzimage config/*

# Edit the TCZ extension list on the following line to change the software available in the image.
rootfs-bare.cpio: Makefile core.gz bash.tcz readline.tcz ncurses.tcz python.tcz screen.tcz elfutils.tcz
	fakeroot rm -rf extr.$@
	mkdir extr.$@
	( cd extr.$@ ; zcat ../$(filter %.gz, $^) | fakeroot cpio -idm )
# serial tty autologin
	fakeroot sed -i '/^# ttyS0$$/s#^..##' extr.$@/etc/securetty
	fakeroot sed -i '/^tty1:/{s#tty1#ttyS0#g}' extr.$@/etc/inittab
# set root password to 'root'
	fakeroot sed -i '/^root/s#\*#$$1$$vSHFxKn9$$rBp5NINud9umUKhE3/r9K.#' extr.$@/etc/shadow
# extensions
# put extensions into "autoinstall" directory
	mkdir -p extr.$@/tmp/builtin/optional
	cp -v $(filter %.tcz, $^) extr.$@/tmp/builtin
	printf '/tmp/builtin/%s\n' $(filter %.tcz, $^) > extr.$@/tmp/builtin/onboot.lst
# ...or extract them now, and run their installscripts on boot. larger image, faster boot.
# for x in $(basename $(filter %.tcz, $^)); do \
# 	echo "Installing $$x"; \
# 	( cd extr.$@ ; fakeroot unsquashfs -d . -f ../$$x.tcz ); \
# 	( cd extr.$@ ; if [ -f usr/local/tce.installed/$$x ]; then \
# 		fakeroot bash -c 'printf "/usr/local/tce.installed/%s\n" '$$x' >> opt/bootsync.sh'; \
# 	fi ) \
# done
# Optionally, compress with UPX. Isn't actually doing much good as far as I can tell...
	-find extr.$@/ -executable -type f | xargs file -i | grep application/x-executable | cut -d':' -f1 | xargs upx --best
	( cd extr.$@ ; find | fakeroot cpio -o -H newc ) > $@

LINUX_MAJOR=4
LINUX_MINOR=2.9
TCL_RELEASE=7.x

build-rootfs: Makefile ../build-rootfs
	cp ../build-rootfs build-rootfs.tmp
	sed -i '1asource tcl.cfg' build-rootfs.tmp
	sed -i '\#tee $$OUT/$$ROOTFS_TMP#{s#tee#tee -a#;s#/init$$#/opt/bootsync.sh#}' build-rootfs.tmp
	mv build-rootfs.tmp build-rootfs

%.tcz:
	wget 'http://tinycorelinux.net/${TCL_RELEASE}/x86/tcz/$@'

core.gz:
	wget 'http://tinycorelinux.net/${TCL_RELEASE}/x86/release/distribution_files/core.gz'

bzimage:
	wget -O bzimage 'http://tinycorelinux.net/${TCL_RELEASE}/x86/release/distribution_files/vmlinuz'

config:
	mkdir -p config

config/linux_build_config: config
	wget --no-use-server-timestamps -O config/linux_build_config 'http://tinycorelinux.net/${TCL_RELEASE}/x86/release/src/kernel/config-${LINUX_MAJOR}.${LINUX_MINOR}-tinycore'

config/Module.symvers: config
	wget --no-use-server-timestamps -O - 'http://tinycorelinux.net/${TCL_RELEASE}/x86/release/src/kernel/Module.symvers-${LINUX_MAJOR}.${LINUX_MINOR}-tinycore.gz' | gzip -dc > config/Module.symvers
