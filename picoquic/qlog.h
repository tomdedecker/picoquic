#ifndef QLOG_H
#define QLOG_H

#include "picoquic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define QLOG_MAX_EVENT_SIZE 128

typedef struct {
    char * qlog_filename;
    FILE * f_ptr;
    bool isEmpty;
    unsigned int event_count;
} qlog_t;

/** Initialises the qlog stream with the given filename, connectionID and starttime
 */
int qlog_init(picoquic_cnx_t * cnx);

/** Adds an event to the logfile
 *  Events should be structures as "category", "type", "trigger", {data}
 *  Timestamp is automatically prepended
 *  Result is an event of format ["time", "category", "type", "trigger", {data}]
 */
int qlog_add_event(const char * qlog_event);

/** Closes the logfile
 * 
 */
int qlog_close();

#endif // QLOG_H