FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. 필수 도구 설치
RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git curl

# 2. 주소 세 토막 (P1 경로 수정 완료)
RUN P1="https://github.com/brainboxdotcc/" && \
    P2="DPP/releases/download/v10.0.35/" && \
    P3="libdpp-10.0.35-linux-x64.deb" && \
    wget ${P1}${P2}${P3} -O dpp.deb && \
    apt-get install -y ./dpp.deb && \
    rm dpp.deb

# 3. 코드 복사 및 컴파일
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp

# 4. 실행
CMD ["./bot"]
