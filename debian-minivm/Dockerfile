FROM debian:bookworm-slim

RUN set -e -x \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get -y --no-install-recommends install \
       ifupdown isc-dhcp-client busybox \
       net-tools iproute2 iputils-ping telnet \
       traceroute procps nano vim-tiny \
    && rm -rf /var/lib/apt/lists/* \
    && for prog in acpid halt reboot poweroff \
                   depmod insmod lsmod modinfo modprobe rmmod; do \
           ln -s /bin/busybox "/sbin/$prog"; \
       done

COPY etc /etc/

ENTRYPOINT [ "/etc/init.sh" ]
