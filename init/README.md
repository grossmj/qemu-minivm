# mininit - init program for QEMU MiniVMs

This application is a basic init program for QEMU MiniVMs.
It mounts important filesystems, runs `/etc/init.sh` and
then waits for it to finish.

The mininit binary is statically linked:  
`gcc -Wall -O2 -s -static -o mininit mininit.c`
