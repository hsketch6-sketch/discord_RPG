FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git
RUN wget https://dpp.dev -O dpp.deb && dpkg -i dpp.deb
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp
CMD ["./bot"]
