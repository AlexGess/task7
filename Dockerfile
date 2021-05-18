FROM debian

RUN apt-get update && apt-get upgrade -y \
    && apt-get install \
        gcc \
        g++ \
        libboost-all-dev \
        mingw-w64 \
        mingw-w64-tools \
        gcc-mingw-w64 \
        gcc-mingw-w64-i686 \
        g++-mingw-w64 \
        g++-mingw-w64-i686 \
        wine \
        gdb -y


COPY src /tasks/
WORKDIR /tasks

