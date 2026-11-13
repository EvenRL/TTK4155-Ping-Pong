# Local project include path for the reorganized Node 1 layout.
if(NOT CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER NAMES avr-gcc REQUIRED)
endif()

enable_language(C)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../include")

target_include_directories(Node_1_default_image_KCA4caSU
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
)
