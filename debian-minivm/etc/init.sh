#!/bin/sh

# Running in QEMU MiniVM
if ! grep -q " /etc/hostname " /proc/mounts; then
	# Uncomment if the module image is configured as a second drive
	# mkdir -p /lib/modules && mount /dev/sdb /lib/modules
	acpid
	hostname "$(cat /etc/hostname)"
	ip link set lo up
	ifup -a -f
fi

exec bash -i -l
