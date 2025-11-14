cmake_minimum_required(VERSION 3.10)
project(hatch_generator VERSION 1.0.0 DESCRIPTION "Hatch generator for SLM slicer")

# Используем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаем исполняемый файл
add_executable(hatch_generator main.cpp)

# Для Windows может потребоваться
if(WIN32)
    target_compile_definitions(hatch_generator PRIVATE _USE_MATH_DEFINES)
endif()

# Опционально: настройки для отладки
set(CMAKE_BUILD_TYPE Debug)
target_compile_options(hatch_generator PRIVATE -Wall -Wextra)

# Опционально: установка в систему (если нужно)
# install(TARGETS hatch_generator DESTINATION bin)