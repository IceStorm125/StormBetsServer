# ---------------------------------------------------------
# 1) Build stage
# ---------------------------------------------------------
FROM debian:trixie AS builder

RUN apt-get update && apt-get install -y \
    g++ cmake make \
    python3 python3-pip python3-venv python3-setuptools python3-wheel \
    git curl \
    libcurl4-openssl-dev \
    build-essential \
    libssl-dev libboost-system-dev zlib1g-dev \
    qtbase5-dev libqt5sql5-mysql \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && pip3 install --break-system-packages conan


WORKDIR /app
COPY conanfile.* ./

RUN conan profile detect --force
RUN conan install . -s compiler.cppstd=23 --build=missing

COPY tgbot-cpp ./tgbot-cpp/
COPY CMakeLists.txt ./
COPY src/ ./src/
COPY include/ ./include/
COPY tests/ ./tests/
COPY wait-for-it.sh ./

RUN chmod +x /app/wait-for-it.sh
RUN mkdir -p build && cd build && cmake .. && make -j4

# ---------------------------------------------------------
# 2) Test stage
# ---------------------------------------------------------
FROM builder AS test

WORKDIR /app/build

CMD ["ctest", "--output-on-failure"]

# ---------------------------------------------------------
# 3) Runtime stage
# ---------------------------------------------------------
FROM debian:trixie AS runtime

RUN apt-get update && apt-get install -y \
    libssl3 \
    libboost-system-dev \
    zlib1g \
    libcurl4 \
    libqt5core5a \
    libqt5network5 \
    libqt5sql5-mysql \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем собранный бинарник и скрипт wait-for-it.sh
COPY --from=builder /app/build/StormBetsServer .
COPY --from=builder /app/wait-for-it.sh .

RUN chmod +x /app/wait-for-it.sh

CMD ["./StormBetsServer"]
