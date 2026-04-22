FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. 필수 도구 설치
RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git curl

# 2. DPP 설치 (주소 직접 안 쓰고 명령어로 최신파일 찾아오기)
RUN LATEST_URL=$(curl -s https://github.com | grep "browser_download_url.*-linux-x64.deb" | cut -d '"' -f 4) && \
    wget $LATEST_URL -O dpp.deb && \
    apt-get install -y ./dpp.deb && \
    rm dpp.deb

# 3. 코드 복사 및 컴파일
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp

# 4. 실행
CMD ["./bot"]
