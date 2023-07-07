# GPS-serial-transceiver-rpi
GPS serial transceiver for the raspberry pi


# How to run it
```bash
$ docker run --name gps_transceiver -it --rm --privileged --net host -v /tmp:/tmp:z -v /dev/gps_serial:/dev/gps_serial registry.digitalocean.com/vehicle-plus/tcu_gps-serial-receiver:v0
```

## build and push to container registry
```bash
$ docker buildx build --push \
--platform linux/amd64,linux/arm64 \
--tag registry.digitalocean.com/vehicle-plus/tcu_gps-serial-receiver:v0 .
```

# bird-view
![project view](./README_images/gradProjMap.png)

## check this medium blog:

[How to make serial device node name static](https://inegm.medium.com/persistent-names-for-usb-serial-devices-in-linux-dev-ttyusbx-dev-custom-name-fd49b5db9af1)

