# if you dont access use this 
# FROM vsomeip_build:v0 as builder
FROM registry.digitalocean.com/vehicle-plus/tcu_builder_packs:v8 as builder


RUN apk update && apk add --no-cache --virtual .second_build_dependency \
    binutils cmake curl gcc g++ git libtool make tar build-base linux-headers

COPY src_build src_build

RUN cd src_build; \
    rm -rf build; \
    mkdir build; \
    cd build; \
    cmake ..; \
    make

FROM alpine:3.17.2

# ENV INITSYSTEM on
# RUN echo 'SUBSYSTEM=="tty", ATTRS{idVendor}=="1546", ATTRS{idProduct}=="01a8", SYMLINK+="gps_serial"' > /etc/udev/rules.d/99-usb-serial.rules 

COPY --from=builder /src_build/build /src_build/build
COPY --from=builder /usr/local/lib/libvsomeip3.so.3 /usr/local/lib
COPY --from=builder /usr/local/lib/libvsomeip3-cfg.so.3 /usr/local/lib
COPY --from=builder /usr/local/lib/libvsomeip3-sd.so.3 /usr/local/lib
COPY --from=builder /usr/lib/libstdc++.so.6 /usr/lib
COPY --from=builder /usr/lib/libboost_thread.so.1.63.0 /usr/lib
COPY --from=builder /usr/lib/libboost_system.so.1.63.0 /usr/lib
COPY --from=builder /usr/lib/libgcc_s.so.1 /usr/lib
COPY --from=builder /usr/lib/libboost_filesystem.so.1.63.0 /usr/lib


WORKDIR /src_build/build
ENTRYPOINT [ "./GPS_serial_transciever" ]

# COPY vsomeip.json /etc/vsomeip