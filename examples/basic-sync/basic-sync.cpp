#include "../../src/missile.h"

using namespace std;

/* This follows the basic structure of a libusb script outlined at
	http://stackoverflow.com/questions/4813764/libusb-basic-example-wanted */
int main() {
	uint16_t vendor_id = 0x0a81;
	uint16_t product_id = 0x0701;
	libusb_device_handle* launcherHandle;

	libusb_init(NULL);
	libusb_set_debug ( 0, 3 );

	launcherHandle = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	if(launcherHandle == NULL) {
		cout << "No missile launcher found\n";
		return 1;
	}

	Missile *launcher = new Missile(launcherHandle);

	launcher -> async = false;
	launcher -> move(ML_DOWN, 1000);
	launcher -> move(ML_UP, 1000);
	launcher -> move(ML_LEFT, 1000);
	launcher -> move(ML_RIGHT, 1000);
	launcher -> fire();

	delete launcher;

	libusb_exit(NULL);
}

