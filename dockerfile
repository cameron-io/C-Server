FROM alpine
WORKDIR /var/lib/C-Server
RUN apk update && \
    apk upgrade && \
    apk add --no-cache \
        clang \
        cmake \
        make \
        curl
ADD . .
CMD make run APP=http_server