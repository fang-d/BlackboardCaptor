cmake_minimum_required(VERSION 3.5)

set(OPENCV_VERSION   4.5.5)
set(OPENCV_SDK_HASH  404e5a04b2f01833a208857eb192c88f33fdfc66f827ac2a993e4581849b2d08)

if (NOT DEFINED GITHUB_PROXY)
    set(GITHUB_PROXY github.com)
endif ()
set(OPENCV_HASH_TYPE SHA256)
set(OPENCV_SDK_NAME  opencv-${OPENCV_VERSION}-android-sdk)
set(OPENCV_SDK_FILE  ${CMAKE_SOURCE_DIR}/3rdparty/opencv-${OPENCV_VERSION}-android-sdk.zip)
set(OPENCV_SDK_URL   https://${GITHUB_PROXY}/opencv/opencv/releases/download/${OPENCV_VERSION}/opencv-${OPENCV_VERSION}-android-sdk.zip)
set(OPENCV_SDK_DIR   ${CMAKE_SOURCE_DIR}/3rdparty/OpenCV-android-sdk)
message("https://${GITHUB_PROXY}/opencv/opencv/releases/download/${OPENCV_VERSION}/opencv-${OPENCV_VERSION}-android-sdk.zip")

function (download_file URL FILE_PATH HASH_TYPE HASH_VALUE)
    if (EXISTS ${FILE_PATH})
        file(${HASH_TYPE} ${FILE_PATH} ACTUAL_HASH)
        if (NOT ("${HASH_VALUE}" STREQUAL "${ACTUAL_HASH}"))
            file(REMOVE ${FILE_PATH})
        endif ()
    endif ()
    if (NOT EXISTS ${FILE_PATH})
        file(DOWNLOAD ${URL} ${FILE_PATH} EXPECTED_HASH ${HASH_TYPE}=${HASH_VALUE})
    endif ()
endfunction (download_file)

if(NOT EXISTS ${OPENCV_SDK_DIR})
    download_file(${OPENCV_SDK_URL} ${OPENCV_SDK_FILE} ${OPENCV_HASH_TYPE} ${OPENCV_SDK_HASH})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf ${OPENCV_SDK_FILE} WORKING_DIRECTORY ${OPENCV_SDK_DIR}/..)
endif()

set(OPENCV_INCLUDE_DIRS ${OPENCV_SDK_DIR}/sdk/native/jni/include)
set(OPENCV_DYNAMIC_LIBS ${OPENCV_SDK_DIR}/sdk/native/libs)