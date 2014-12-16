FROM ubuntu

RUN apt-get update
RUN apt-get -y install clang build-essential
ADD src /usr/src/popo/
WORKDIR /usr/src/popo
RUN make
CMD /usr/src/popo/popo

