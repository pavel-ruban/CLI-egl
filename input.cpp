#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <linux/input.h>
#include <pcre.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <math.h>
#include <sys/select.h>
#include <sys/types.h>
#include "input.h"

#define KEYBOARD 0
#define MOUSE 1
#define OVECCOUNT 30

extern float xAngle, xRadius, yAngle, yRadius, zAngle, zRadius, xCustom, yCustom;

/**
 * Parse line starting with H symbol for mouse/keyboard input driver path.
 */
char* parseInputHandler(int type, char* subject) {
  static pcre *rem = 0, *rek = 0;
  int utf8, erroffset, ovector[OVECCOUNT], rc, i, subject_length = strlen(subject);
  const char* pattern;
  const char *error;
  static char path[50];
  pcre *re;

  if (!rem)
    rem = pcre_compile("event([0-9]+) mouse0", 0, &error, &erroffset, NULL);

  if (!rek)
    rek = pcre_compile("sysrq kbd event([0-9]+)(?!.*?js)", 0, &error, &erroffset, NULL);

  if (rek == NULL || rem == NULL)
    goto reerror;

  switch (type) {
    case MOUSE:
      re = rem;
      break;
    
    case KEYBOARD:
      re = rek;
      break;
    
  }

  rc = pcre_exec(
    re,                   /* the compiled pattern */
    NULL,                 /* no extra data ‐ we didn’t study the pattern */
    subject,              /* the subject string */
    subject_length,       /* the length of the subject */
    0,                    /* start at offset 0 in the subject */
    0,                    /* default options */
    ovector,              /* output vector for substring information */
    OVECCOUNT
  ); 

  if (rc > 0) {
    int  subject_length = ovector[2 * 1 + 1] - ovector[2 * 1];
    char* substring_start = subject + ovector[2 * 1];
    
    strcpy(path, "/dev/input/event");

    for (i = 0; i <= subject_length; i++) {
      path[16 + i] = *(substring_start + i);  
    }

    path[16 + subject_length] = '\0';

    printf("found path is %s\n", path);
    return path; 
  }

  return 0;

  reerror:

  /* Compilation failed: print the error message and exit */
  printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
  return 0;
}

/**
 * Get dynamic keyboard & mouse paths if available.
 * Open driver paths & returns they file descriptors.
 *
 * If driver isn't available fd in corresponding struct index will be -1.
 */
inputFds initInput() {
  size_t bytes = 0;
  int job = 1, fd, subjLen, i;
  char ch, subject[100];
  char* path;

  inputFds inputFds;
  inputFds.mouse = 0;
  inputFds.kbd = 0;

  // Open system active input devices definition file.
  fd = open("/proc/bus/input/devices", O_RDWR);

  while (job) {
    bytes = read(fd, &ch, 1);
    if (bytes <= 0) goto readError;

    // Parse only lines consist first char = 'H'.
    if (ch == 'H') {
      subjLen = 0;
      subject[0] = ch;

      // Read lines starts with H & store it to subject char array.
      while (subject[subjLen] != '\n') {
        subjLen++;
        bytes = read(fd, subject + subjLen, 1);
        if (bytes <= 0) goto readError;
      }
      *(subject + subjLen + 1) = '\0';

      // Echo debug data.
      printf("%s", subject);

      //  Parse input handler if it wasn't init yet & open file descriptor for coresponding event driver.
      if (!inputFds.mouse) {
        if (path = parseInputHandler(MOUSE, subject)) {
          inputFds.mouse = open(path, O_RDONLY);
        }
      }
      if (!inputFds.kbd) {
        if (path = parseInputHandler(KEYBOARD, subject)) {
          inputFds.kbd = open(path, O_RDONLY);
        }
      }
    }
    // If line doesn't start with H, skip parsing & move stream offset until new line start. 
    else {
      while (ch != '\n') {
        bytes = read(fd, &ch, 1);
        if (bytes <= 0) goto readError;
      }
    }

    // If all devices have been init skip further processing.
    if (inputFds.kbd > 0 && inputFds.mouse > 0) {
      break;
    }
  }

  return inputFds; 

  readError:

  if (bytes < 0) {
    printf("read error during parsing input devices");
    exit(1);
  }
  else {
    job = 0;
  }

  return inputFds; 
}

/**
 * Process incoming events.
 */
void handleEvent(int fd, CONTEXT_TYPE *context) {
  struct input_event ev;
  size_t bytes;
  int ins = sizeof(struct input_event);
  bytes = read(fd,  &ev, ins);

  printf("event: type %i | code %i | value %i\n", ev.type, ev.code, ev.value);

  switch (ev.type) {
    case 1:
      switch (ev.code) {
        case 1:
          if (ev.value == 1) {
            context->flags.run = false;
          }
          break;

        case 16:
          if (ev.value == 1) {
            xAngle += 1.0;
          }
          else if (ev.value == 2) {
            xAngle += 1.0;
          }
          break;

        case 17:
          if (ev.value == 1) {
            xAngle -= 1.0;
          }
          else if (ev.value == 2) {
            xAngle -= 1.0;
          }
          break;

        case 18:
          if (ev.value == 1) {
            xRadius += 0.1;
          }
          else if (ev.value == 2) {
            xRadius += 0.1;
          }
          break;

        case 19:
          if (ev.value == 1) {
            xRadius -= 0.1;
          }
          else if (ev.value == 2) {
            xRadius -= 0.1;
          }
          break;

        case 30:
          if (ev.value == 1) {
            yAngle += 1.0;
          }
          else if (ev.value == 2) {
            yAngle += 1.0;
          }
          break;

        case 31:
          if (ev.value == 1) {
            if (context->flags.debug) {
              context->flags.debug = false;
            }
            else {
              context->flags.debug = true;
            }
            yAngle -= 1.0;
          }
          else if (ev.value == 2) {
            yAngle -= 1.0;
          }
          break;

        case 32:
          if (ev.value == 1) {
            yRadius += 0.1;
          }
          else if (ev.value == 2) {
            yRadius += 0.1;
          }
          break;

        case 33:
          if (ev.value == 1) {
            yRadius -= 0.1;
          }
          else if (ev.value == 2) {
            yRadius -= 0.1;
          }
          break;

        case 44:
          if (ev.value == 1) {
            zAngle += 1.0;
          }
          else if (ev.value == 2) {
            zAngle += 1.0;
          }
          break;

        case 45:
          if (ev.value == 1) {
            zAngle -= 1.0;
          }
          else if (ev.value == 2) {
            zAngle -= 1.0;
          }
          break;

        case 46:
          if (ev.value == 1) {
            zRadius += 0.1;
          }
          else if (ev.value == 2) {
            zRadius += 0.1;
          }
          break;

        case 47:
          if (ev.value == 1) {
            zRadius -= 0.1;
          }
          else if (ev.value == 2) {
            zRadius -= 0.1;
          }
          break;

        case 272:
          if (ev.value == 1) {
            context->vars.pressed = true;

            context->vars.mouseClickedX = context->vars.mouseX;
            context->vars.mouseClickedY = context->vars.mouseY;
          }
          else if (ev.value == 0) {
            context->vars.pressed = false;
          }
          break;


      }
      break;

    case 2:
      switch (ev.code) {
        case 0:
          context->vars.mouseX += 0.001 * ev.value;
          if (context->vars.mouseX > 1.0)
            context->vars.mouseX = 1.0;
          if (context->vars.mouseX < -1.0)
            context->vars.mouseX = -1.0;
          break;

        case 1:
          context->vars.mouseY += 0.001 * -1 * ev.value;
          if (context->vars.mouseY > 1.0)
            context->vars.mouseY = 1.0;
          if (context->vars.mouseY < -1.0)
            context->vars.mouseY = -1.0;
          break;
      }
      break;
  }
}
