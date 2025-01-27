#ifndef REQ_CALLBACK_H
#define REQ_CALLBACK_H

#include <uv.h>
#include <stdlib.h>
#include "req_data.hh"
#include "req_handler.hh"

/*
 *  Callback for when the TCP write is complete
 */
static void on_write_end_cb(uv_write_t *req, int status)
{
    struct client_request_data *data;
    data = (client_request_data *)req->data;
    close_data(data);
}
/*
 * Callback for post completion of the work associated with the
 * request
 */
static void on_process_command_complete_cb(uv_work_t *req, int status)
{
    struct client_request_data *data;
    data = (client_request_data *)req->data;
    uv_buf_t buf = uv_buf_init(data->response, strlen(data->response) + 1);
    data->write_req = (uv_write_t *)malloc(sizeof(*data->write_req));
    data->write_req->data = data;
    uv_timer_stop(data->timer);
    uv_write(data->write_req, (uv_stream_t *)data->client,
             &buf, 1, on_write_end_cb);
}
/*
 * Callback for doing the actual work.
 */
static void process_command_cb(uv_work_t *req)
{
    struct client_request_data *data;
    char *x;
    data = (client_request_data *)req->data;
    // Do the actual time-consuming request processing here
    data->response = strdup(handle_request(data->text));
}

#endif