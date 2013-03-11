First you need libusb1.0 On Debian this is the libusb-1.0-0-dev package:

        apt-get install libusb-1.0-0-dev

Now compile:

        git submodule update --init
        make

To use the rocket-launcher as non-root user:

        cp 40-rocketlauncher.rules /etc/udev/rules.d
        service udev restart

Now re-plug the missile launcher and run one of the binaries.
