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
 * \file
 *
 * \brief picolog: lightweight logging for embedded systems
 *
 * A quick intro by example:
 *
 *     #include "picolog.h"
 *
 *     // To use picolog, you must define a function to process logging messages.
 *     // It can write the messages to a console, to a file, to an in-memory
 *     // buffer: the choice is yours.  And you get to choose the format of
 *     // the message.  This example prints to the console.  One caveat: msg
 *     // is a static string and will be over-written at the next call to PICOLOG.
 *     // You may print it or copy it, but saving a pointer to it will lead to
 *     // confusion and astonishment.
 *     //
 *     void my_console_logger(picolog_level_t level, const char *msg) {
 *         printf("%s [%s]: %s\n",
 *             get_timestamp(),
 *             picolog_level_name(level),
 *             msg);
 *     }
 *
 *     int main() {
 *         PICOLOG_INIT();
 *
 *         // log to the console messages that are WARNING or more severe.  You
 *         // can re-subscribe at any point to change the severity level.
 *         PICOLOG_SUBSCRIBE(my_console_logger, PICOLOG_WARNING);
 *
 *         // log to a file messages that are DEBUG or more severe
 *         PICOLOG_SUBSCRIBE(my_file_logger, PICOLOG_DEBUG);
 *
 *         int arg = 42;
 *         PICOLOG_INFO("Arg is %d", arg);  // logs to file but not console
 *     }
 */

#ifndef PICOLOG_H_
#define PICOLOG_H_

#ifdef __cplusplus
extern "C" {
    #endif

#include <stdio.h>

typedef enum {
  PICOLOG_TRACE_LEVEL=100,
  PICOLOG_DEBUG_LEVEL,
  PICOLOG_INFO_LEVEL,
  PICOLOG_WARNING_LEVEL,
  PICOLOG_ERROR_LEVEL,
  PICOLOG_CRITICAL_LEVEL,
  PICOLOG_ALWAYS_LEVEL
} picolog_level_t;

// The following macros enable or disable picolog.  If `PICOLOG_ENABLED` is
// defined at compile time, a macro such as `PICOLOG_INFO(...)` expands
// into `picolog_message(PICOLOG_INFO_LEVEL, ...)`.  If `PICOLOG_ENABLED` is not
// defined, then the same macro expands into `do {} while(0)` and will
// not generate any code at all.  
//
// There are two ways to enable picolog: you can uncomment the following
// line, or -- if it is commented out -- you can add -DPICOLOG_ENABLED to
// your compiler switches.
#define PICOLOG_ENABLED

#ifdef PICOLOG_ENABLED
  #define PICOLOG_INIT(a) picolog_init(a)
  #define PICOLOG_SUBSCRIBE(a, b) picolog_subscribe(a, b)
  #define PICOLOG_UNSUBSCRIBE(a) picolog_unsubscribe(a)
  #define PICOLOG_LEVEL_NAME(a) picolog_level_name(a)
  #define PICOLOG(...) picolog_message(__VA_ARGS__)
  #define PICOLOG_TRACE(...) picolog_message(PICOLOG_TRACE_LEVEL, __VA_ARGS__)
  #define PICOLOG_DEBUG(...) picolog_message(PICOLOG_DEBUG_LEVEL, __VA_ARGS__)
  #define PICOLOG_INFO(...) picolog_message(PICOLOG_INFO_LEVEL, __VA_ARGS__)
  #define PICOLOG_WARNING(...) picolog_message(PICOLOG_WARNING_LEVEL, __VA_ARGS__)
  #define PICOLOG_ERROR(...) picolog_message(PICOLOG_ERROR_LEVEL, __VA_ARGS__)
  #define PICOLOG_CRITICAL(...) picolog_message(PICOLOG_CRITICAL_LEVEL, __VA_ARGS__)
  #define PICOLOG_ALWAYS(...) picolog_message(PICOLOG_ALWAYS_LEVEL, __VA_ARGS__)
#else
  // picolog vanishes when disabled at compile time...
  #define PICOLOG_INIT(a) do {} while(0)
  #define PICOLOG_SUBSCRIBE(a, b) do {} while(0)
  #define PICOLOG_UNSUBSCRIBE(a) do {} while(0)
  #define PICOLOG_LEVEL_NAME(a) do {} while(0)
  #define PICOLOG(s, f, ...) do {} while(0)
  #define PICOLOG_TRACE(f, ...) do {} while(0)
  #define PICOLOG_DEBUG(f, ...) do {} while(0)
  #define PICOLOG_INFO(f, ...) do {} while(0)
  #define PICOLOG_WARNING(f, ...) do {} while(0)
  #define PICOLOG_ERROR(f, ...) do {} while(0)
  #define PICOLOG_CRITICAL(f, ...) do {} while(0)
  #define PICOLOG_ALWAYS(f, ...) do {} while(0)
#endif

typedef enum {
  PICOLOG_ERR_NONE = 0,
  PICOLOG_ERR_SUBSCRIBERS_EXCEEDED,
  PICOLOG_ERR_NOT_SUBSCRIBED,
} picolog_err_t;

// define the maximum number of concurrent subscribers
#ifndef PICOLOG_MAX_SUBSCRIBERS
#define PICOLOG_MAX_SUBSCRIBERS 6
#endif
// maximum length of formatted log message
#ifndef PICOLOG_MAX_MESSAGE_LENGTH
#define PICOLOG_MAX_MESSAGE_LENGTH 120
#endif
/**
 * @brief: prototype for picolog subscribers.
 */
typedef void (*picolog_function_t)(picolog_level_t severity, char *msg);

void picolog_init(picolog_level_t threshold);
picolog_err_t picolog_subscribe(picolog_function_t fn, picolog_level_t threshold);
picolog_err_t picolog_unsubscribe(picolog_function_t fn);
const char *picolog_level_name(picolog_level_t level);
void picolog_message(picolog_level_t severity, const char *fmt, ...);
void picolog_format(picolog_level_t severity, char *msg);

#ifdef __cplusplus
}
#endif

#endif /* PICOLOG_H_ */
