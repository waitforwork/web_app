#!/bin/bash

set -e  # Выход при ошибке

echo "Building C++ Web Application..."

# Создаем директорию сборки
mkdir -p build
cd build

# Конфигурируем CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Собираем
make -j$(nproc)

echo "Build completed successfully!"
echo "Executable: $(pwd)/CppWebApp"

# Запускаем тесты
echo "Running tests..."
./CppWebApp_test

cd ..
