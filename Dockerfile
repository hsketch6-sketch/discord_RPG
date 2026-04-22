FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git

# 아래 줄이 잘리지 않게 한 줄로 길게 복사해야 합니다!
RUN wget https://github.com -O dpp.deb && dpkg -i dpp.deb && rm dpp.deb

WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp
CMD ["./bot"]
