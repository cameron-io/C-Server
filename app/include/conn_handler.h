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
static void handle_client_cb(uv_stream_t *server, int status)
{
    req_data *r;

    // if status not zero there was an error
    if (status == -1)
        return;

    r = (req_data *)calloc(1, sizeof(*r));
    r->start = time(NULL);
    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    client->data = r;
    r->client = client;

    // initialize the new client
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        // start reading from stream
        uv_timer_t *timer;
        timer = (uv_timer_t *)malloc(sizeof(*timer));
        timer->data = r;
        r->timer = timer;

        uv_timer_init(loop, timer);
        uv_timer_set_repeat(timer, 1);
        uv_timer_start(timer, client_timeout_cb, 10000, 20000);
        uv_read_start((uv_stream_t *)client, alloc_buffer, read_cb);
    }
    else
    {
        // close client stream on error
        close_data(r);
    }
}

// Callback for the timer which signifies a timeout
static void client_timeout_cb(uv_timer_t *handle)
{
    req_data *r;
    r = (req_data *)handle->data;
    uv_timer_stop(handle);

    if (r->work_started)
        return;

    close_data(r);
}

// Callback for read function, called multiple times per request
static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    uv_tcp_t *client;
    req_data *r;
    char *tmp;
    client = (uv_tcp_t *)stream;
    r = (req_data *)stream->data;
    if (nread == -1 || nread == UV_EOF)
    {
        free(buf->base);
        uv_timer_stop(r->timer);
        close_data(r);
    }
    else
    {
        if (!r->text)
        {
            r->text = (char *)malloc(nread + 1);
            memcpy(r->text, buf->base, nread);
            r->text[nread] = '\0';
            r->text_len = nread;
        }
        else
        {
            tmp = (char *)realloc(r->text, r->text_len + nread + 1);
            memcpy(tmp + r->text_len, buf->base, nread);
            tmp[r->text_len + nread] = '\0';
            r->text = tmp;
            r->text_len += nread;
        }

        free(buf->base);

        if (!r->work_started && r->text_len &&
            (strstr(r->text, "\r\n") || strstr(r->text, "\n\n")))
        {
            r->work_req = (uv_work_t *)malloc(sizeof(*r->work_req));
            r->work_req->data = r;
            r->work_started = 1;

            uv_read_stop(stream);
            uv_queue_work(loop, r->work_req,
                          process_command_cb,
                          on_process_command_complete_cb);
        }
    }
}

#endif