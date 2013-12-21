This code provides a userspace USB driver for linux users with a DreamCheeky missile launcher.

The two example programs included are:
- basic-sync -- A minimal demonstration of how to interact with the "missile" class. This moves and fires the launcher according to a hardcoded set of commands.
- joystick-ctl -- Allows you to use a joystick to move and fire the launcher.

Supported launchers
-------------------
I wrote this specifically for the missile launcher shown in [this review](http://www.techwarelabs.com/reviews/misc/usb-missile-launcher/). The USB vendor ID is 0x0701 and product ID is 0x0660.

Modifying this code for the later launchers would be quite simple, so lodge an issue with specific details if you would like support to be added for your launcher.

Howto
-----

First you need libusb1.0 and ncurses. On Debian the packes are:

        apt-get install libusb-1.0-0-dev libncurses-dev

Now compile:

        git submodule update --init
        make

To use the rocket-launcher as non-root user:

        cp 40-rocketlauncher.rules /etc/udev/rules.d
        service udev restart

Now re-plug the missile launcher and run one of the binaries.
