FROM gcc:latest
COPY . /Battleship
WORKDIR /Battleship/
RUN apt-get update
RUN apt-get -qq install make -y
RUN DEBIAN_FRONTEND=noninteractive apt-get -qq install libgtk-3-dev -y
RUN make cliente
CMD ["./cliente1","192.168.1.51","12000"]