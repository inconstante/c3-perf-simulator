FROM mcr.microsoft.com/devcontainers/base@sha256:acdce4f55335a974812238e983619f1aeae9894e05ee9c7a79fc2c1b9f8d95d1

ARG DEBIAN_FRONTEND=noninteractive

ARG C3_USER_UID
ARG C3_USER_GID

ARG C3_WORKDIR=/c3_workdir
ARG C3_USER=c3_user
ARG C3_USER_HOME=/home/$C3_USER

ARG C3_GEM5_DIR=/c3-perf-simulator
ARG C3_GLIBC_DIR=$C3_GEM5_DIR/c3-simulator/glibc

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

RUN apt-get update && apt-get upgrade -y \
    && apt-get install --no-install-recommends -y \
    acpica-tools \
    autoconf \
    automake \
    bison \
    build-essential=12.8ubuntu1.1 \
    clang-format \
    clang-tidy \
    wget \
    doxygen \
    dwarves \
    flex \
    gawk \
    gcc-9=9.4.0-1ubuntu1~20.04.2 \
    g++-9=9.4.0-1ubuntu1~20.04.2 \
    git \
    graphviz \
    libatk1.0-dev \
    libatk-bridge2.0-dev \
    libelf-dev \
    libgtk-3-dev \
    libssl-dev \
    libtinfo-dev \
    llvm \
    ninja-build \
    python3=3.8.2-0ubuntu2 \
    python3-pip=20.0.2-5ubuntu1.10 \
    zstd \
    bc \
    cpio \
    file \
    libjson-perl \
    dbus-x11 \
    openssh-server \
    xterm \
    m4 \
    patchelf \
    scons \
    zlib1g \
    zlib1g-dev \
    libprotobuf-dev \
    protobuf-compiler \
    python3-dev=3.8.2-0ubuntu2 \
    libgoogle-perftools-dev \
    libprotoc-dev=3.6.1.3-2ubuntu5.2 \
    python-is-python3 \
    libboost-all-dev \
    libhdf5-serial-dev \
    python3-pydot \
    libpng-dev \
    pkg-config \
    python3-venv \
    black \
    cmake \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir\
    cpplint \
    pre-commit \
    mypy \
    pytest-testdox \
    pytest-xdist

WORKDIR $C3_GEM5_DIR
COPY . .
RUN git clone -b harden-may2024-for-gem5 https://github.com/IntelLabs/c3-simulator && \
    cd $C3_GLIBC_DIR && \
    git clone -b harden-may2024 https://github.com/IntelLabs/c3-glibc src && \
    cd $C3_GEM5_DIR

RUN (yes || true) | scons build/X86/gem5.opt -j8

RUN cd $C3_GLIBC_DIR && CC_NO_WRAP_ENABLE=1 ./make_glibc.sh

RUN cd $C3_GEM5_DIR/tests/c3_tests && \
    git clone https://github.com/embecosm/mibench.git && \
    cd mibench && \
    git checkout 0f3cbcf && \
    cd $C3_GEM5_DIR 
