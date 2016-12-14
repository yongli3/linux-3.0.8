
time make ARCH=arm CROSS_COMPILE="ccache arm-none-linux-gnueabi-" -j8 uImage V=1

time make ARCH=arm CROSS_COMPILE="ccache arm-none-linux-gnueabi-" INSTALL_MOD_STRIP=1 INSTALL_MOD_PATH=M modules_install

time make  ARCH=arm CROSS_COMPILE="ccache arm-none-linux-gnueabi-" -C ./ spi/ modules -j8 V=1


# kernel command as below
console=ttySAC2,115200 earlyprintk=ttySAC2,115200 root=/dev/mmcblk0p3 selinux=0 debug ip=dhcp

# gcc 4.9+ build okay, but boot fail
time make  ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabihf-" KCFLAGS="-march=armv5t" EXTRA_CFLAGS="-mno-unaligned-access" uImage  V=1
time ARCH=arm CROSS_COMPILE="ccache arm-linux-gnueabi-" make uImage  -j8 V=1
time make -j2 zImage V=1
time make -j2 modules V=1
make INSTALL_MOD_STRIP=1 INSTALL_MOD_PATH=M modules_install
#nfs 30008000 192.168.0.24://root/210/rootfs/4
#movi read kernel 30008000; movi read rootfs 30B00000 300000; bootm 30008000 30B00000

