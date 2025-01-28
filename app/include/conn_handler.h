#ifndef SERVER_CALLBACK_H
#define SERVER_CALLBACK_H

#include <uv.h>
#include <stdlib.h>
#include <time.h>
#include "req_cb.h"

static uv_loop_t *loop;

static void client_timeout_cb(uv_timer_t *handle);
static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

// Callback for handling the new connection
static void connection_cb(uv_stream_t *server, int status)
{
    req_data *data;

    // if status not zero there was an error
    if (status == -1)
        return;

    data = (req_data *)calloc(1, sizeof(*data));
    data->start = time(NULL);
    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    client->data = data;
    data->client = client;

    // initialize the new client
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        // start reading from stream
        uv_timer_t *timer;
        timer = (uv_timer_t *)malloc(sizeof(*timer));
        timer->data = data;
        data->timer = timer;

        uv_timer_init(loop, timer);
        uv_timer_set_repeat(timer, 1);
        uv_timer_start(timer, client_timeout_cb, 10000, 20000);
        uv_read_start((uv_stream_t *)client, alloc_buffer, read_cb);
    }
    else
    {
        // close client stream on error
        close_data(data);
    }
}

// Callback for the timer which signifies a timeout
static void client_timeout_cb(uv_timer_t *handle)
{
    req_data *data;
    data = (req_data *)handle->data;
    uv_timer_stop(handle);
    if (data->work_started)
        return;
    close_data(data);
}

// Callback for read function, called multiple times per request
static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    uv_tcp_t *client;
    req_data *data;
    char *tmp;
    client = (uv_tcp_t *)stream;
    data = (req_data *)stream->data;
    if (nread == -1 || nread == UV_EOF)
    {
        free(buf->base);
        uv_timer_stop(data->timer);
        close_data(data);
    }
    else
    {
        if (!data->text)
        {
            data->text = (char *)malloc(nread + 1);
            memcpy(data->text, buf->base, nread);
            data->text[nread] = '\0';
            data->text_len = nread;
        }
        else
        {
            tmp = (char *)realloc(data->text, data->text_len + nread + 1);
            memcpy(tmp + data->text_len, buf->base, nread);
            tmp[data->text_len + nread] = '\0';
            data->text = tmp;
            data->text_len += nread;
        }
        free(buf->base);
        if (!data->work_started && data->text_len &&
            (strstr(data->text, "\r\n") || strstr(data->text, "\n\n")))
        {
            data->work_req =
                (uv_work_t *)malloc(sizeof(*data->work_req));
            data->work_req->data = data;
            data->work_started = 1;
            uv_read_stop(stream);
            uv_queue_work(loop, data->work_req,
                          process_command_cb,
                          on_process_command_complete_cb);
        }
    }
}

#endif