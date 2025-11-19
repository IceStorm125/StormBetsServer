# ---------------------------------------------------------
# 1) Build stage
# ---------------------------------------------------------
FROM debian:bookworm AS builder

# Установка build-зависимостей
RUN apt-get update && apt-get install -y \
    g++ cmake make \
    python3 python3-pip python3-venv python3-setuptools python3-wheel \
    git curl \
    libcurl4-openssl-dev \
    build-essential \
    libssl-dev libboost-system-dev zlib1g-dev \
    qtbase5-dev libqt5sql5-mysql \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Conan
RUN pip3 install --break-system-packages conan

WORKDIR /app

# Копируем только Conan-манифесты сначала — для кэширования зависимостей
COPY conanfile.* ./

RUN conan profile detect --force
RUN conan install . --build=missing

# Копируем оставшиеся файлы
COPY tgbot-cpp ./tgbot-cpp/
COPY CMakeLists.txt ./
COPY src/ ./src/
COPY include/ ./include/
COPY tests/ ./tests/
COPY wait-for-it.sh ./

# Сборка проекта
RUN mkdir -p build && cd build && cmake .. && make -j4

# ---------------------------------------------------------
# 2) Runtime stage (минимальный образ)
# ---------------------------------------------------------
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libssl3 libboost-system1.74.0 zlib1g libcurl4 \
    libqt5core5a libqt5network5 libqt5sql5-mysql \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем собранный бинарник и скрипт wait-for-it.sh
COPY --from=builder /app/build/StormBetsServer .
COPY --from=builder /app/wait-for-it.sh .

CMD ["./StormBetsServer"]
