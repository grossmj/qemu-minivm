#!/bin/sh

# Running in QEMU MiniVM
if ! grep -q " /etc/hostname " /proc/mounts; then
	mkdir -p /lib/modules && mount /dev/sdb /lib/modules
	[ -f /dev/net/tun ] || modprobe tun
	modprobe openvswitch
	acpid
	hostname "$(cat /etc/hostname)"
	ip link set lo up
	sed -n 's/^ *\(eth[0-9]*\): .*/\1/p' /proc/net/dev | sort -V | \
		while read -r intf; do ip link set dev "$intf" up; done
	ifup -a -f
	# simple logrotate: rename *.log to *.log.1
	[ -d /var/log/openvswitch ] && \
	find /var/log/openvswitch -name '*.log' -type f -exec mv {} {}.1 \;
fi

/etc/openvswitch/init.sh
exec bash -i
