# 1. 환경 설정
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 2. 필수 도구 설치
RUN apt-get update && apt-get install -y \
    cmake g++ libssl-dev zlib1g-dev wget git \
    && rm -rf /var/lib/apt/lists/*

# 3. DPP 라이브러리 직접 설치 (최신 버전 링크로 수정)
RUN wget https://github.com -O dpp.deb \
    && dpkg -i dpp.deb \
    && rm dpp.deb

# 4. 코드 복사 및 빌드
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp

# 5. 실행
CMD ["./bot"]
