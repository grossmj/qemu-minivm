# Docker base image for Netmiko, NAPALM, Pyntc, Terraform, and Ansible

FROM ubuntu:noble

RUN set -e -x \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
#
# qemu-mini-vm: install network configuration and system tools
#
    && apt-get -y --no-install-recommends install \
       ifupdown isc-dhcp-client busybox \
    && for prog in acpid halt reboot poweroff \
                   depmod insmod lsmod modinfo modprobe rmmod; do \
           ln -s /bin/busybox "/sbin/$prog"; \
       done \
#
# add HashiCorp repository
#
    && apt-get -y --no-install-recommends install \
       curl ca-certificates gpg lsb-release \
    && curl -s https://apt.releases.hashicorp.com/gpg | gpg --dearmor -o /usr/share/keyrings/hashicorp-archive-keyring.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/hashicorp-archive-keyring.gpg] https://apt.releases.hashicorp.com $(lsb_release -cs) main" > /etc/apt/sources.list.d/hashicorp.list \
    && apt-get update \
#
# install network automation tools
#
    && ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime \
    && apt-get -y --no-install-recommends install \
       net-tools iproute2 iputils-ping telnet traceroute \
       openssh-client sshpass less patch nano vim-tiny \
       python3 python3-pip python3-setuptools python3-wheel \
       ansible terraform \
    && rm -rf /var/lib/apt/lists/* \
    && pip3 install --break-system-packages --no-cache-dir \
       napalm pyntc netmiko \
#
# create ssh config
#
    && mkdir -p /root/.ssh \
    && chmod 700 /root/.ssh \
    && printf '\
HostKeyAlgorithms +ssh-rsa\n\
Ciphers +aes256-cbc,aes192-cbc,aes128-cbc,3des-cbc\n\
KexAlgorithms +diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1,diffie-hellman-group1-sha1\n' \
       >> /root/.ssh/config \
    && chmod 600 /root/.ssh/*

COPY etc /etc/

#VOLUME [ "/root", "/etc", "/usr" ]
VOLUME [ "/root" , "/etc"]

ENTRYPOINT [ "/etc/init.sh" ]
