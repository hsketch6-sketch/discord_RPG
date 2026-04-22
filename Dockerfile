FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git

# 주소를 쪼개서 넣었으니 절대 안 잘립니다.
RUN URL="https://github.com" && \
    wget $URL -O dpp.deb && \
    dpkg -i dpp.deb && \
    rm dpp.deb

WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp
CMD ["./bot"]
