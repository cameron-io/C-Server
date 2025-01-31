#pragma once

#include <uv.h>
#include <stdlib.h>

// Stores everything about a request
typedef struct
{
    time_t start;
    char *text;
    size_t text_len;
    char *response;
    int work_started;
    uv_tcp_t *client;
    uv_work_t *work_req;
    uv_write_t *write_req;
    uv_timer_t *timer;
} req_data;

// Allocate buffers as requested by UV
static void alloc_buffer(uv_handle_t *handle, size_t size,
                         uv_buf_t *buf)
{
    char *base;
    base = (char *)calloc(1, size);
    if (!base)
        *buf = uv_buf_init(NULL, 0);
    else
        *buf = uv_buf_init(base, size);
}

// Callback to free the handle
static void on_close_free(uv_handle_t *handle)
{
    free(handle);
}

// Callback for freeing up all resources allocated for request
static void close_data(req_data *data)
{
    if (!data)
        return;
    if (data->client)
        uv_close((uv_handle_t *)data->client,
                 on_close_free);
    if (data->work_req)
        free(data->work_req);
    if (data->write_req)
        free(data->write_req);
    if (data->timer)
        uv_close((uv_handle_t *)data->timer,
                 on_close_free);
    if (data->text)
        free(data->text);
    if (data->response)
        free(data->response);
    free(data);
}
