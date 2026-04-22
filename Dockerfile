FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# 1. 필수 도구 설치
RUN apt-get update && apt-get install -y cmake g++ libssl-dev zlib1g-dev wget git curl

# 2. DPP 설치 (의존성 문제까지 한 번에 해결하는 마법의 명령어 추가)
RUN wget https://github.com -O dpp.deb && \
    apt-get install -y ./dpp.deb && \
    rm dpp.deb

# 3. 코드 복사 및 컴파일
WORKDIR /app
COPY . .
RUN g++ -std=c++17 main.cpp -o bot -ldpp

# 4. 실행
CMD ["./bot"]
