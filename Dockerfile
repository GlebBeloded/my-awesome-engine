FROM debian:latest
RUN apt --assume-yes update && apt --assume-yes upgrade && apt --assume-yes install python3 \
wget g++ make clang clang-tools clang-format cppcheck && \
mkdir ~/temp                                          &&\
cd ~/temp &&\
wget https://cmake.org/files/v3.15/cmake-3.15.4.tar.gz &&\
tar -xzvf cmake-3.15.4.tar.gz &&\
cd cmake-3.15.4/ &&\
./bootstrap &&\
make -j$(nproc) &&\
make install