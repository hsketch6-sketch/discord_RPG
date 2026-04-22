FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. 필수 도구 설치
RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git curl

# 2. DPP 설치 (주소를 변수로 쪼개서 절대 안 잘리게 함)
RUN URL_BASE="https://github.com" && \
    URL_FILE="v10.0.35/libdpp-10.0.35-linux-x64.deb" && \
    wget ${URL_BASE}/${URL_FILE} -O dpp.deb && \
    apt-get install -y ./dpp.deb && \
    rm dpp.deb

# 3. 코드 복사 및 컴파일
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp

# 4. 실행
CMD ["./bot"]
