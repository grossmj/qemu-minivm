FROM alpine

RUN set -e -x -o pipefail \
    && apk upgrade \
    && apk add nano \
    && rm -rf /var/cache/apk/*

COPY etc /etc/

ENTRYPOINT [ "/etc/init.sh" ]
