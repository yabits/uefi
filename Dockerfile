FROM ubuntu:17.04

RUN apt update &&\
    apt install -y bison build-essential curl flex git gnat-5 \
                    libncurses5-dev m4 zlib1g-dev && \
    mkdir -p /root/src && \
    git clone --depth 1 -b 4.6 \
    https://github.com/coreboot/coreboot.git ~/src/coreboot && \
    make -C ~/src/coreboot crossgcc-i386 CPUS=$(nproc)
