FROM ubuntu

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  libcurl4-openssl-dev

RUN mkdir /server
# RUN mkdir -p /server/build
# COPY . /server
#
# WORKDIR /server/build
#
# RUN cmake .. && make
#
# ENTRYPOINT ["./parsec-server"]
