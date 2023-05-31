FROM vsomeip_build:v0 as builder

COPY src_build src_build

RUN cd src_build; \
    mkdir build; \
    cd build; \
    cmake ..; \
    make

FROM alpine:3.17.2
COPY --from=builder /src_build/build src_build/build
COPY --from=builder /usr/lib/libstdc++.so.6 /usr/lib
COPY --from=builder /usr/lib/libgcc_s.so.1 /usr/lib