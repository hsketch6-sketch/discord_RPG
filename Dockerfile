FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git

# 주소를 3조각으로 나눠서 합치는 방식 (절대 안 잘림)
RUN PART1="https://github.com" && \
    PART2="v10.0.35/libdpp-10.0.35-linux-x64.deb" && \
    FULL_URL="${PART1}${PART2}" && \
    wget $FULL_URL -O dpp.deb && \
    dpkg -i dpp.deb && \
    rm dpp.deb

WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp
CMD ["./bot"]
