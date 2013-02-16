#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <time.h>
#include <climits>

#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)
    
const int VENDOR=0x4e7;
const int PRODUCT=0x0126;
//int lastX=-1,lastY=-1;
//const int noise = 100;
long lastSendTime = 0;
const int minInterval = 25000000; 
bool isPressed = false;



bool exitNow = false;
//int minX=4080, maxX=0, minY=4080, maxY=0;

int uinput_connect(wchar_t* wstr){
    int                    fd;
    struct uinput_user_dev uidev;
    char buf[256];
    //struct input_event     ev;
    //int                    dx, dy;
    //int                    i;

    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0)
        die("error: open");

    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
	die("ioctl: ");
    if (ioctl(fd,UI_SET_KEYBIT,BTN_TOUCH) < 0)
	die("ioctl: ");
    if (ioctl(fd,UI_SET_EVBIT,EV_ABS) < 0)
	die("ioctl: ");
    if (ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0)
	die("ioctl: ");
    if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
	die("ioctl: ");
    if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_X) < 0)
	die("ioctl: ");
    if (ioctl(fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) < 0)
	die("ioctl: ");

    wcstombs(buf,wstr,sizeof(buf));

    memset(&uidev, 0, sizeof(uidev));
    
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "%s", buf);
    
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = VENDOR;
    uidev.id.product = PRODUCT;
    uidev.id.version = 1;

    /*uidev.absmin[ABS_X] = 51;
    uidev.absmax[ABS_X] = 4080;
    uidev.absmin[ABS_Y] = 87;
    uidev.absmax[ABS_Y] = 4080;*/
    uidev.absmin[ABS_MT_POSITION_X] = 51;
    uidev.absmax[ABS_MT_POSITION_X] = 4080;
    uidev.absmin[ABS_MT_POSITION_Y] = 87;
    uidev.absmax[ABS_MT_POSITION_Y] = 4080;
    /*uidev.absmin[ABS_PRESSURE] = 10;
    uidev.absmax[ABS_PRESSURE] = 1000;*/

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");
    
    return fd;
}

void releaseButton(int fd, const unsigned char *data){
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = BTN_TOUCH;
    ev.value = 0;
    write(fd,&ev,sizeof(ev));
    
    ev.type = EV_ABS;
    ev.code = ABS_MT_TOUCH_MAJOR;
    ev.value = 0;
    write(fd,&ev,sizeof(ev));
    
    /*ev.type = EV_ABS;
    ev.code = ABS_PRESSURE;
    ev.value = 0;
    write(fd,&ev,sizeof(ev));*/
    
    /*memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = SYN_MT_REPORT;
    ev.value = 0;
    write(fd,&ev,sizeof(ev));*/
    
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd,&ev,sizeof(ev));
    
    isPressed = false;
    
    usleep(200000);
}

void pressButton(int fd, const unsigned char *data){
    struct input_event ev;
    
	struct timespec tv;
	
	clock_gettime(CLOCK_MONOTONIC,&tv);

	long currTime = tv.tv_nsec;
	
	if (currTime < lastSendTime){
	    lastSendTime = currTime;
	}
	
	if (currTime - lastSendTime < minInterval)
	    return;
	
	lastSendTime = currTime;

        int X = data[5] + 255 * data[6];
        int Y = data[10] + 255 * data[11];
        
        /*int Y = data[5] + 255 * data[6];
        int X = data[10] + 255 * data[11];*/
        
        //printf("X: %d LX: %d Y: %d LY: %d\n", X, lastX, Y, lastY);
        
        //X: 2079 LX: 2077 Y: 1715 LY: 1716
        /*if (
    	    (((X < lastX) && (X > lastX - noise)) ||
    	    ((X > lastX) && (X < lastX + noise))) &&
    	    (((Y < lastY) && (Y > lastY - noise)) ||
    	    ((Y > lastY) && (Y < lastY + noise)))
        ){
    	//    printf("SKIP\n");
    	    return;
        }*/
        
        /*lastX = X;
        lastY = Y;*/
        
        /*memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_ABS;
        ev.code = ABS_X;
        ev.value = X;
        write(fd,&ev,sizeof(ev));*/
        
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_ABS;
        ev.code = ABS_MT_POSITION_X;
        ev.value = X;
        write(fd,&ev,sizeof(ev));

        /*memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_ABS;
        ev.code = ABS_Y;
        ev.value = Y;
        write(fd,&ev,sizeof(ev));*/
        
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_ABS;
        ev.code = ABS_MT_POSITION_Y;
        ev.value = Y;
        write(fd,&ev,sizeof(ev));

        if (!isPressed){
    	    memset(&ev, 0, sizeof(struct input_event));
    	    ev.type = EV_ABS;
    	    ev.code = ABS_MT_TOUCH_MAJOR;
    	    ev.value = 1;
    	    write(fd,&ev,sizeof(ev));
    	    
    	    memset(&ev, 0, sizeof(struct input_event));
    	    ev.type = EV_KEY;
    	    ev.code = BTN_TOUCH;
    	    ev.value = 1;
    	    write(fd,&ev,sizeof(ev));
    	    isPressed = true;
        }
        
        /*memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_ABS;
        ev.code = ABS_PRESSURE;
        ev.value = 1;
        write(fd,&ev,sizeof(ev));*/

        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = SYN_MT_REPORT;
        ev.value = 0;
        write(fd,&ev,sizeof(ev));
        
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd,&ev,sizeof(ev));
}

void touch (int fd, const unsigned char *data, int size){
    switch (data[0]){
	case 13: pressButton(fd, data); break;
	case 5: releaseButton(fd, data); break;
	default: 
	    /*printf("size: %2d\n",size); 
	    for (int i=0; i<size; i++){
		printf("%02hhx ",data[i]);
	    }
	    printf("\n");*/
	break;
    }
    
    /*for (int j=0; j<size; j++){
	    printf("%02hhx ", data[j]);
    }
    printf("\n");*/
    
}

int run(){
	int res;
	unsigned char buf[128];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int fd;
	
	memset(buf,0x00,sizeof(buf));
	
	handle = hid_open(VENDOR, PRODUCT, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}
	
	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string\n");
	printf("Vendor: %ls\n", wstr);
	
	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string\n");
	printf("Product: %ls\n", wstr);
	
	fd = uinput_connect(wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	printf("Serial: %ls", wstr);
	printf("\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read indexed string 1\n");
	printf("Indexed: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
	
	res = 0;
	while (1) {
		res = hid_read(handle, buf, sizeof(buf));
		if (res>0){
		    touch(fd, buf, res);
		}else if (res<0){
		    break;
		}
		
		if (exitNow) break;
		
		usleep(10000);
	}
	
	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();
	
	return 0;
}

void handler(int signum) {
    exitNow = true;
}

int main(int argc, char* argv[])
{
    //TODO: check parameter value
    if (argc == 2){
        pid_t pid = fork();
        if (pid < 0) die("fork");
    
        if (pid > 0) exit(EXIT_SUCCESS);
    
        umask(0);
    
        pid_t sid = setsid();
    
        if (sid < 0) die("setsid");
    
	fclose(stdin);
        fclose(stdout);
        fclose(stderr);
    
        signal(SIGTERM, handler);
        signal(SIGHUP, SIG_IGN);
    }

    run();

    return 0;
}

