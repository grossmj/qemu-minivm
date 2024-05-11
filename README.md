# QEMU VMs based on Docker Images

While I like the simplicity and performance of Docker VMs,
they have some drawbacks when used within GNS3:

* Persistence - Only the persistent directories will be saved.
  Additionally installed software will be lost after restart.
* Host kernel - The Docker VM can only use the features
  available in the host kernel.
* Security - The isolation between the Docker VM and the host
  is not as good as with QEMU VMs.

Looking for a way to make QEMU VMs lighter and easier to setup,
I found this article:  
[Execute Docker Containers as QEMU MicroVMs](https://mergeboard.com/blog/2-qemu-microvm-docker/)

Sad enough the use of QEMU MicroVMs is not possible in GNS3.
GNS3 doesn't offer the `virtio-blk-device` and `virtio-net-device` devices.
Instead of extending GNS3 I used the concept with normal QEMU VMs.
Booting is not as fast as with MicroVMs, but 1-2 seconds boot time
is perfectly acceptable.

The MiniVM is split into several parts, the kernel components and
the root disk image.

## Kernel

The kernel consists of the kernel image, an initial RAM disk and
an optional module disk image.
The kernel can be used for all of the appliances -
it only needs to be built once.

QEMU will directly boot the kernel, without the use of a boot loader.
In GNS3 this is configured in the advanced tab of the appliance template.

![Template/Advanced](pictures/template_advanced.png)

For more information see the [kernel](kernel) directory.


## Root Image

The root image is a converted Docker image, so first we need to create that.

The Docker context directory is a fairly standard one,
for an example see the `alpine-minivm` directory.
Only the `etc` subdirectory contains additional files
normally provided by Docker but not by QEMU.
Additionally it needs `etc/acpi/PWRF/00000080`, which is run on ACPI poweroff.

Besides these `etc` files the QEMU VM needs an `init` program.
I include a modified version of the init from the MicroVM article.

Now we need to build the Docker VM and do some initial tests.
Then the Docker image needs to be converted to QCOW2 and the
init program added, this is done by the `docker2qcow2` script.


## Recovery

If something goes wrong with your VM and you can't access it
in the normal way, it's good to have fallback methods.
As with "normal" Linux VMs, you can add special parameters
to the kernel command line:

* `single` - This will open a shell in the initial RAM disk,
  just before the root disk is mounted.
  Exiting this recovery shell resumes the normal boot sequence.
* `init=/bin/sh` - This runs a shell after the initial RAM disk
  has been run, so that the root disk is mounted.
  This is useful if the `/etc/init.sh` script contains a fatal error.

The recovery shell can't be stopped in the normal way.
Instead, a forced shutdown must be performed
by running `sync` then `poweroff -f`.


## GNS3 Integration

By importing the *.gns3a appliances you can use them in GNS3.
As noted in the usage instruction, for a clean shutdown I recommend to
set the parameter "On close" to "Send the shutdown signal (ACPI)"
in the node properties and/or in the template.
