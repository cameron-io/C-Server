#ifndef REQ_HANDLER_H
#define REQ_HANDLER_H

#include <uv.h>
#include <stdlib.h>
#include "req_data.h"
#include "req_router.h"

// Callback for when the TCP write is complete
static void on_write_end_cb(uv_write_t *req, int status)
{
    req_data *r;
    r = (req_data *)req->data;
    close_data(r);
}

// Callback for post completion of the work associated with the request
static void on_req_handle_complete_cb(uv_work_t *req, int status)
{
    req_data *r;
    r = (req_data *)req->data;
    uv_buf_t buf = uv_buf_init(r->response, strlen(r->response) + 1);
    r->write_req = (uv_write_t *)malloc(sizeof(*r->write_req));
    r->write_req->data = r;
    uv_timer_stop(r->timer);
    uv_write(r->write_req, (uv_stream_t *)r->client,
             &buf, 1, on_write_end_cb);
}

// Callback for doing the actual work.
static void req_handle_cb(uv_work_t *req)
{
    req_data *r;
    r = (req_data *)req->data;
    // Do the actual time-consuming request processing here
    r->response = strdup(route_request(r->text));
}

#endif