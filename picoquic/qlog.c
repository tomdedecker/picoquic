#include "qlog.h"
#include "picoquic_internal.h"
#include <stdint.h>

static qlog_t * qlog = NULL;

/** Initialises the qlog stream with the given connectionID and starttime
 */
int qlog_init(picoquic_cnx_t * cnx) {
    // Throw error if log is not NULL, don't overwrite existing instance
    if (qlog != NULL) {
        return -1;
    }
    
    // Connection has to exist
    if (cnx == NULL) {
        return -2;
    }
    
    // Only log client side connections
    if (!cnx->client_mode) {
        return -3;
    }
    
    uint64_t starttime = cnx->start_time;
    
    picoquic_connection_id_t connectionId = cnx->initial_cnxid;
    
    char connId[connectionId.id_len * sizeof(uint8_t) + 1];
    
    for (int i = 0; i < connectionId.id_len; ++i) {
        sprintf(connId+(i*sizeof(uint8_t)), "%x", connectionId.id[i]);
    }
    connId[connectionId.id_len * sizeof(uint8_t)] = '0';
    
    // Allocate memory for the log
    qlog = (qlog_t*) malloc(sizeof(qlog_t));
    
    // Create a logfile with the connection ID as name in write mode
    char filename[128];
    sprintf(filename, "%s.json", connId);
    qlog->f_ptr = fopen(filename, "w");
    
    // No events have been written yet
    qlog->isEmpty = true;
    
    // Copy filename
    qlog->qlog_filename = (char*) malloc(strlen(filename) + 1);
    strcpy(qlog->qlog_filename, filename);
    
    // Write first part of the log format
    fprintf(qlog->f_ptr, 
    "{\"connectionid\": \"0x%s\", \"starttime\": %lu,\n \"fields\":\n\t[\"time\",\"category\",\"type\",\"trigger\",\"data\"],\n \"events\": [\n", 
    connId, starttime);
    
    // Initialise eventcount to 0
    qlog->event_count = 0;

    return 0;
}

/** Adds an event to the logfile
 *  Events should be structures as "category", "type", "trigger", {data}
 *  Timestamp is automatically prepended
 *  Result is an event of format ["time", "category", "type", "trigger", {data}]
 */
int qlog_add_event(const char * qlog_event) {
    if (qlog_event == NULL) {
        return -1;
    }
    if (qlog->isEmpty == false) {
        fputs(",\n", qlog->f_ptr); 
    } else {
        qlog->isEmpty = false;
    }
    ++qlog->event_count;
    fputs("\t", qlog->f_ptr);
    fprintf(qlog->f_ptr, "[%u,", qlog->event_count);
    fputs(qlog_event, qlog->f_ptr);
    fputs("]", qlog->f_ptr);
    
    return 0;
}

/** Closes the logfile 
 */
int qlog_close() {
    // Can't close non-existing connection
    if (qlog == NULL) {
        return -1;
    }
    
    if (qlog->isEmpty == false) {
        fputs("\n", qlog->f_ptr);
    }
    fputs("]}", qlog->f_ptr);
    
    // Clean up memory
    fclose(qlog->f_ptr);
    free(qlog->qlog_filename);
   
    return 0;
}