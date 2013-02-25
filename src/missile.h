#include <libusb-1.0/libusb.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

/* These constants are from the excellent breakdown of protocol at
	http://matthias.vallentin.net/blog/2007/04/writing-a-linux-kernel-driver-for-an-unknown-usb-device/
	and http://www.amctrl.com/rocketlauncher.html
   The diagonals did not work for me, and up/down had to be switched */

#define ML_DOWN         0x01
#define ML_UP           0x02
#define ML_LEFT         0x04
#define ML_RIGHT        0x08
#define ML_FIRE         0x10
#define ML_STOP         0x20

/* Pseudo-states which are emulated by quick flicks of direction */
#define ML_UP_LEFT      (ML_UP | ML_LEFT)
#define ML_DOWN_LEFT    (ML_DOWN | ML_LEFT)
#define ML_UP_RIGHT     (ML_UP | ML_RIGHT)
#define ML_DOWN_RIGHT   (ML_DOWN | ML_RIGHT)

#define ML_FIRE_TIME	4500		/* Milliseconds for shoot cycle */
#define ML_MOVE_TIME	 100		/* Milliseconds per movement */

class Missile {
	public:
		Missile(libusb_device_handle* launcherHandle);
		~Missile();
		bool async;
		bool check_state();
		void move(unsigned char direction, long duration);
		void move(unsigned char direction);
		void fire();
		void stop();
		bool observeLimits;

	private:
		void wait();
		void set_state(unsigned char cmd);
		void set_state(unsigned char cmd, long expiry);

		bool state_expires;
		timespec state_expiry;

		unsigned char state; /* State of this class */
		unsigned char ml_state; /* Current state of the device */
		libusb_device_handle* handle;
		void ml_set_state(unsigned char cmd); /* Set state of device itself */
};


