#include "../../src/missile.h"
#include <ncurses.h>
#include <cstdlib>
#include <signal.h>
#include <mutex>
#include <thread>

using namespace std;
Missile *launcher;
mutex launcher_mutex;
bool finished;

/**
 * Catch Ctrl+C
 */
void sigint_handler(int s) {
	launcher_mutex.lock();
	launcher -> stop();
	finished = true;
	launcher_mutex.unlock();

	endwin();
	exit(1); 
}

/**
 * Wait for keyboard input
 */
void followKeyboard(Missile *newLauncher) {
	launcher = newLauncher;
	
	/* Get characters */
	int ch;
	do {
		ch = getch();
		launcher_mutex.lock();
		switch(ch) {
			case KEY_UP:
			case 'w':
				launcher -> move(ML_UP);
				break;
			case KEY_DOWN:
			case 's':
				launcher -> move(ML_DOWN);
				break;
			case KEY_LEFT:
			case 'a':
				launcher -> move(ML_LEFT);
				break;
			case KEY_RIGHT:
			case 'd':
				launcher -> move(ML_RIGHT);
				break;
			case 10: // Return
				launcher -> stop();
				break;
			case 32: // Space
				launcher -> fire();
				break;
			case 27: // Esc
			case 'q':
				launcher -> stop();
				finished = true;
		}
		launcher_mutex.unlock();
	} while(!finished);
}

/**
 * USB code and main loop
 */
int main() {
	uint16_t vendor_id = 0x0a81;
	uint16_t product_id = 0x0701;
	libusb_device_handle* launcherHandle;

	/* Open rocket launcher */
	libusb_init(NULL);
	libusb_set_debug ( 0, 3 );
	launcherHandle = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	if(launcherHandle == NULL) {
		cout << "No missile launcher found\n";
		return 1;
	}

	/* Set up curses */
	signal(SIGINT, sigint_handler);
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak(); // Unbuffered input
	printw("Use UP, DOWN, LEFT, RIGHT to operate the launcher, SPACE to fire,\nRETURN to quit.\n");

	/* Initialise Missile object and get input in another thread */
	Missile *launcher = new Missile(launcherHandle);
	thread t1(followKeyboard, launcher);

	while(!finished) {
		launcher_mutex.lock();
		launcher -> check_state();
		launcher_mutex.unlock();
		usleep(100000);
	}

	t1.join();
	
	delete launcher;
	libusb_exit(NULL);
	endwin();
	return 0;
}

