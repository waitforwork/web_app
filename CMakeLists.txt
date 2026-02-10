cmake_minimum_required(VERSION 3.10)
project(CppWebApp VERSION 1.0.0 LANGUAGES CXX)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Исполняемый файл
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/HttpServer.cpp
)

# Тесты (опционально)
enable_testing()
add_executable(${PROJECT_NAME}_test tests/test_app.cpp src/HttpServer.cpp)
target_include_directories(${PROJECT_NAME}_test PRIVATE src)

add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

# Установка
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    COMPONENT applications
)

# Конфигурационный файл
install(FILES config.json DESTINATION /etc/cpp-webapp COMPONENT config)
