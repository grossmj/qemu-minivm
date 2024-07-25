# Kernel for the MiniVM project

Compiling our own kernel is a complex task, mainly
because of the 10000 parameters that need to be set.
So I decided to use one of the distribution kernels.
But this requires the use of an initial RAM disk.

As a base, I chose the kernel from the
[Debian Official Cloud Images](https://cloud.debian.org/images/cloud/).
They use the [linux-image-cloud-amd64](https://packages.debian.org/bookworm/linux-image-cloud-amd64) package.
We need to select the specific version, say 6.1.0-18, and download it.

To unpack this package (without using Debian tools), use this procedure:

```
mkdir debian
ar x --output debian linux-image-*.deb
cd debian
tar xf data.tar*
mv boot/vmlinuz* ..
mv lib ..
cd ..
rm -r debian
```

This gives us the kernel, next is the modules disk.
The command `./modules2qcow2 <kernel_version>` takes the modules
from `lib/modules/<kernel_version>` and packs them into
`modules-<kernel_version>.qcow2`.

Finally we have to create the initial RAM disk.

Gentoo Linux has a good introduction:
<https://wiki.gentoo.org/wiki/Custom_Initramfs>.

The `ramdisk-template` directory contains the `init` script and
the binaries, that are needed while executing the initial RAM disk.

For the vast majority of the programs an Alpine
[static busybox](https://pkgs.alpinelinux.org/package/edge/main/x86_64/busybox-static) is used.
Since Alpine does not provide a statically linked `e2fsck`,
[e2fsck-static](https://packages.debian.org/bookworm/e2fsck-static)
from Debian is used.
The `update-busybox` and `update-e2fsck` scripts update the binaries and
create/update the necessary links.

We don't want to put all the modules into the RAM disk.
So we need to create/modify the `modules.ramfs` configuration file,
which contains the files/directories of the modules we want to include.
The easiest way is to boot an image with this kernel and
use `lsmod` to see which modules are needed.
The location of the modules can be found by inspecting the file
`/lib/modules/<kernel_version>/modules.dep`.

The drivers are automatically detected during boot,
but some support modules are not.
We need to put these modules in the optional file `modules.autoload`:

Now `mkinitramfs <template_directory> <kernel_version>`
combines the template directory (`ramdisk-template`) and the module directory,
and uses the configuration files to build the `initramfs-<kernel_version>`.

Ready-to-use kernel files can be found in the releases section
of this repository.
