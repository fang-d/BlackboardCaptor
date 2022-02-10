cmake_minimum_required(VERSION 3.1)

project(BlackboardCaptor CXX)

include("3rdparty/DownloadOpenCV.cmake")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

link_directories("${OPENCV_DYNAMIC_LIBS}/${ANDROID_ABI}")

add_library(${PROJECT_NAME}
    SHARED
        "src/interface.cpp"
        "src/detection.cpp"
        "src/transformation.cpp"
)

target_compile_options(${PROJECT_NAME}
    PRIVATE -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "include"
)

target_include_directories(${PROJECT_NAME} SYSTEM
    PUBLIC ${OPENCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC opencv_java4
)
