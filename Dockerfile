FROM ubuntu:22.04 AS builder

# Устанавливаем зависимости
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем исходники
COPY . .

# Собираем
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# Runtime образ
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем бинарник
COPY --from=builder /app/build/CppWebApp /usr/local/bin/CppWebApp

# Пользователь
RUN useradd -m -s /bin/bash appuser
USER appuser

EXPOSE 3000

CMD ["CppWebApp"]
