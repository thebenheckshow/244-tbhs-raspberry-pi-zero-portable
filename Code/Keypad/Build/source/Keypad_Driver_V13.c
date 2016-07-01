
/*
 * This is a keypad driver for a Raspberry Pi Zero gpio keyboard.
 *
 */

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include <sys/types.h>

// from version 6
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <iostream>

#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/mman.h>

#define die(str, args...) do { \
perror(str); \
exit(EXIT_FAILURE); \
} while(0)

#define col0  22
#define col1  10
#define col2  9
#define col3  11
#define col4  5
#define col5  6
#define col6  13
#define col7  19
#define col8  26
#define col9  7
#define col10 12
#define col11 16
#define col12 20
#define col13 21

#define row0 27
#define row1 17
#define row2 4
#define row3 23
#define row4 24

//#define TRUE 1
//#define FALSE 0

#define GND -1
// end from version 6


unsigned char abc[] = {
  //     ~, 1,   2, 3,     4, 5,     6, 7,   8,    9,    0,    -,     =, BS,
  //   TAB, Q,   W, E,     R, T,     Y, U,   I,    O,    P,    [,     ],  \,
  //   CAP, A,   S, D,     F, G,     H, J,   K,    L,    ;,    ', ENTER,   ,
  // SHIFT, Z,   X, C,     V, B,     N, M,   ,,    .,    /,   UP, SHIFT,   ,
  //  CTRL,  , ALT,  , SPACE,  , SPACE,  , ALT, CTRL, LEFT, DOWN, RIGHT,   ,
  KEY_GRAVE,       KEY_1,           KEY_2,       KEY_3,         KEY_4,     KEY_5,            KEY_6,          KEY_7,              KEY_8,          KEY_9,             KEY_0,        KEY_MINUS,          KEY_EQUAL,  KEY_BACKSPACE,
  KEY_TAB,         KEY_Q,           KEY_W,       KEY_E,         KEY_R,     KEY_T,            KEY_Y,          KEY_U,              KEY_I,          KEY_O,             KEY_P,    KEY_LEFTBRACE,     KEY_RIGHTBRACE,  KEY_BACKSLASH,
  KEY_CAPSLOCK,    KEY_A,           KEY_S,       KEY_D,         KEY_F,     KEY_G,            KEY_H,          KEY_J,              KEY_K,          KEY_L,     KEY_SEMICOLON,   KEY_APOSTROPHE,          0,      KEY_ENTER,
  KEY_LEFTSHIFT,   KEY_Z,           KEY_X,       KEY_C,         KEY_V,     KEY_B,            KEY_N,          KEY_M,          KEY_COMMA,        KEY_DOT,         KEY_SLASH,           KEY_UP,     0,              KEY_RIGHTSHIFT,
  KEY_LEFTCTRL,        0,     KEY_LEFTALT,           0,     KEY_SPACE,         0,        KEY_SPACE,              0,       KEY_RIGHTALT,  KEY_RIGHTCTRL,          KEY_LEFT,         KEY_DOWN,          KEY_RIGHT,        KEY_ESC};

void pin_init(){
   wiringPiSetupGpio ();

   pinMode (col0,  OUTPUT);
   pinMode (col1,  OUTPUT);
   pinMode (col2,  OUTPUT);
   pinMode (col3,  OUTPUT);
   pinMode (col4,  OUTPUT);
   pinMode (col5,  OUTPUT);
   pinMode (col6,  OUTPUT);
   pinMode (col7,  OUTPUT);
   pinMode (col8,  OUTPUT);
   pinMode (col9,  OUTPUT);
   pinMode (col10, OUTPUT);
   pinMode (col11, OUTPUT);
   pinMode (col12, OUTPUT);
   pinMode (col13, OUTPUT);

   digitalWrite (col0,   HIGH);
   digitalWrite (col1,   HIGH);
   digitalWrite (col2,   HIGH);
   digitalWrite (col3,   HIGH);
   digitalWrite (col4,   HIGH);
   digitalWrite (col5,   HIGH);
   digitalWrite (col6,   HIGH);
   digitalWrite (col7,   HIGH);
   digitalWrite (col8,   HIGH);
   digitalWrite (col9,   HIGH);
   digitalWrite (col10,  HIGH);
   digitalWrite (col11,  HIGH);
   digitalWrite (col12,  HIGH);
   digitalWrite (col13,  HIGH);

   pinMode (row0, INPUT);
   pinMode (row1, INPUT);
   pinMode (row2, INPUT);
   pinMode (row3, INPUT);
   pinMode (row4, INPUT);

   pullUpDnControl (row0, PUD_UP);
   pullUpDnControl (row1, PUD_UP);
   pullUpDnControl (row2, PUD_UP);
   pullUpDnControl (row3, PUD_UP);
   pullUpDnControl (row4, PUD_UP);
}

unsigned int get_key(){

   unsigned int keyPadNumber[5][14] = {
      {0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13},
      {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
      {28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41},
      {42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55},
      {56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69}
   };

   unsigned int keyPadBool[5][14] = {
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'}
   };

   int column[14] = {col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12, col13};
   int row[5] = {row0, row1, row2, row3, row4};

   int columnCount = 0;

   int rowTest = 0;
   int rowCount = 0;

   int key = -1;
   int keyPress = 0;

//
   while ((keyPress == 0 and rowTest == 0)) // While no key pressed
   {
      for ( columnCount = 0; columnCount <= 13; columnCount++  )
      {
         delayMicroseconds(1);

         digitalWrite (column[columnCount], LOW);
         for ( rowCount = 0; rowCount < 5; rowCount++ )
         {
            //keyPress = digitalRead (row[rowCount]);

            if(digitalRead (row[rowCount]) == 0)
            {
               rowTest = 1;
               key = keyPadNumber[rowCount][columnCount];
               keyPadBool[rowCount][columnCount] = 1;

               // test output
              // std::cout << "\nROW: " << rowCount << "\tCOLUMN: " << columnCount << "\tKEY: " << key << "\n";

            }
            else
            {
               keyPadBool[rowCount][columnCount] = 0;
            }
         }
         digitalWrite (column[columnCount], HIGH);
      }
   }
   delay(200);

   int r;
   int c;
   for (r=0; r < 5; r++) {
      for (c=0; c < 14; c++) {
         if (keyPadBool[r][c] == 1){
            return (key);
         }
      }
   }
   return (-1);
}

int send_event(int fd, __u16 type, __u16 code, __s32 value)
{
    struct input_event event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;

    if (write(fd, &event, sizeof(event)) != sizeof(event)) {
        fprintf(stderr, "Error on send_event");
        return -1;
    }

    return 0;
}

int main(void) {
  pid_t pid;

  pid = getpid();

  if (pid > 0){
    //printf("\nPID Parent= %d\n", pid);
    //return 0;
  }

  if (pid == 0){
    //printf("\nPID child = %d\n", pid);
    return 0;
  }
  /* uinput init */
  int i,fd;
  struct uinput_user_dev device;
  memset(&device, 0, sizeof device);

  fd=open("/dev/uinput",O_WRONLY);
  strcpy(device.name,"test keyboard");

  device.id.bustype=BUS_USB;
  device.id.vendor=1;
  device.id.product=1;
  device.id.version=1;

  if (write(fd,&device,sizeof(device)) != sizeof(device))
  {
    fprintf(stderr, "error setup\n");
  }

  if (ioctl(fd,UI_SET_EVBIT,EV_KEY) < 0)
    fprintf(stderr, "error evbit key\n");

  for (i=0;i<70;i++)
    if (ioctl(fd,UI_SET_KEYBIT, abc[i]) < 0)
      fprintf(stderr, "error evbit key\n");

    if (ioctl(fd,UI_SET_EVBIT,EV_REL) < 0)
      fprintf(stderr, "error evbit rel\n");

    for(i=REL_X;i<REL_MAX;i++)
      if (ioctl(fd,UI_SET_RELBIT,i) < 0)
        fprintf(stderr, "error setrelbit %d\n", i);

      if (ioctl(fd,UI_DEV_CREATE) < 0)
      {
        fprintf(stderr, "error create\n");
      }

      sleep(2);
    /* end uinput init */

    /* init event0 */
    int fdkey;
    fdkey = open("/dev/input/event0", O_RDONLY);
    // struct input_event evkey;

    int flags = fcntl(fd, F_GETFL, 0);
    //int err;
    fcntl(fdkey, F_SETFL, flags | O_NONBLOCK);
    /* end init event0 */

    pin_init();

    //int j, m=0,k;
    int returnKeyPress = 0;

    while(1) {
      /* check for key */
      //              k=0;
      //              err=-1;
      //              while ((err==-1) && (k<500)) {
      //                      err = read(fdkey, &evkey, sizeof(struct input_event));
      //                      k++;
      //                      usleep(1000);
      //              }
      //
      //              if (err!=-1) {
      //                      if ((evkey.type==1) && (evkey.value==0)) {
      //
      //                              if ((m==27) || (m==28)) break;
      //                              send_event(fd, EV_KEY, abc[m], 1);
      //                              send_event(fd, EV_SYN, SYN_REPORT, 0);
      //                              send_event(fd, EV_KEY, abc[m], 0);
      //                              send_event(fd, EV_SYN, SYN_REPORT, 0);
      //                      } else {
      //                              if (m==0)
      //                                      m=27;
      //                              else
      //                                      m--;
      //                      }
      //                      //printf("%i",m);
      //              }
      returnKeyPress = get_key();
      //
      if ( returnKeyPress != -1 ){
        send_event(fd, EV_KEY, abc[returnKeyPress], 1);
        send_event(fd, EV_SYN, SYN_REPORT, 0);
        send_event(fd, EV_KEY, abc[returnKeyPress], 0);
        send_event(fd, EV_SYN, SYN_REPORT, 0);
      }

    }

    close(fd);

     return 0;
}