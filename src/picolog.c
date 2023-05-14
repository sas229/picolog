/**
MIT License

Copyright (c) 2019 R. Dunbar Poor <rdpoor@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file picolog.c
 *
 * \brief picolog: lightweight logging for Raspberry Pi Pico (based on uLog)
 *
 * See picolog.h for sparse documentation.
 */

#include "picolog.h"

#ifdef PICOLOG_ENABLED  // whole file...

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


// =============================================================================
// types and definitions

typedef struct {
  picolog_function_t fn;
  picolog_level_t threshold;
} subscriber_t;

// =============================================================================
// local storage

static subscriber_t s_subscribers[PICOLOG_MAX_SUBSCRIBERS];
static char s_message[PICOLOG_MAX_MESSAGE_LENGTH];

// =============================================================================
// user-visible code

void picolog_init(picolog_level_t threshold) {
  printf("\x1b[2J");
  memset(s_subscribers, 0, sizeof(s_subscribers));
  picolog_subscribe(picolog_format, threshold);
}

// search the s_subscribers table to install or update fn
picolog_err_t picolog_subscribe(picolog_function_t fn, picolog_level_t threshold) {
  int available_slot = -1;
  int i;
  for (i=0; i<PICOLOG_MAX_SUBSCRIBERS; i++) {
    if (s_subscribers[i].fn == fn) {
      // already subscribed: update threshold and return immediately.
      s_subscribers[i].threshold = threshold;
      return PICOLOG_ERR_NONE;

    } else if (s_subscribers[i].fn == NULL) {
      // found a free slot
      available_slot = i;
    }
  }
  // fn is not yet a subscriber.  assign if possible.
  if (available_slot == -1) {
    return PICOLOG_ERR_SUBSCRIBERS_EXCEEDED;
  }
  s_subscribers[available_slot].fn = fn;
  s_subscribers[available_slot].threshold = threshold;
  return PICOLOG_ERR_NONE;
}

// search the s_subscribers table to remove
picolog_err_t picolog_unsubscribe(picolog_function_t fn) {
  int i;
  for (i=0; i<PICOLOG_MAX_SUBSCRIBERS; i++) {
    if (s_subscribers[i].fn == fn) {
      s_subscribers[i].fn = NULL;    // mark as empty
      return PICOLOG_ERR_NONE;
    }
  }
  return PICOLOG_ERR_NOT_SUBSCRIBED;
}

const char *picolog_level_name(picolog_level_t severity) {
  switch(severity) {
   case PICOLOG_TRACE_LEVEL: return "TRACE";
   case PICOLOG_DEBUG_LEVEL: return "DEBUG";
   case PICOLOG_INFO_LEVEL: return "INFO";
   case PICOLOG_WARNING_LEVEL: return "WARNING";
   case PICOLOG_ERROR_LEVEL: return "ERROR";
   case PICOLOG_CRITICAL_LEVEL: return "CRITICAL";
   case PICOLOG_ALWAYS_LEVEL: return "ALWAYS";
   default: return "UNKNOWN";
  }
}

void picolog_message(picolog_level_t severity, const char *fmt, ...) {
  va_list ap;
  int i;
  va_start(ap, fmt);
  vsnprintf(s_message, PICOLOG_MAX_MESSAGE_LENGTH, fmt, ap);
  va_end(ap);

  for (i=0; i<PICOLOG_MAX_SUBSCRIBERS; i++) {
    if (s_subscribers[i].fn != NULL) {
      if (severity >= s_subscribers[i].threshold) {
        s_subscribers[i].fn(severity, s_message);
      }
    }
  }
}

// =============================================================================
// private code

#define NORMAL  "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

void picolog_format(picolog_level_t severity, char *msg) {
	switch(severity) {   
    case PICOLOG_ALWAYS_LEVEL:
      printf("%s[%s] %s %s\n", BLUE, picolog_level_name(severity), msg, NORMAL);
      break;
    case PICOLOG_CRITICAL_LEVEL:
      printf("%s[%s] %s %s\n", MAGENTA, picolog_level_name(severity), msg, NORMAL);
      break;
    case PICOLOG_ERROR_LEVEL:
      printf("%s[%s] %s %s\n", RED, picolog_level_name(severity), msg, NORMAL);
      break;
    case PICOLOG_WARNING_LEVEL:
      printf("%s[%s] %s %s\n", YELLOW, picolog_level_name(severity), msg, NORMAL);
      break;
    case PICOLOG_INFO_LEVEL:
      printf("%s[%s] %s %s\n", GREEN, picolog_level_name(severity), msg, NORMAL);
      break;	
    case PICOLOG_DEBUG_LEVEL:
      printf("%s[%s] %s %s\n", WHITE, picolog_level_name(severity), msg, NORMAL);
      break;	
    case PICOLOG_TRACE_LEVEL:
      printf("%s[%s] %s %s\n", NORMAL, picolog_level_name(severity), msg, NORMAL);
      break;
	}
}

#endif  // #ifdef PICOLOG_ENABLED
