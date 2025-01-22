FROM alpine
WORKDIR /var/lib/C-Server
RUN apk update && \
    apk upgrade && \
    apk add \
        clang \
        cmake \
        make \
        curl
ADD . .
RUN make compile
CMD make run APP=http_server