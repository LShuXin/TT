cmake_minimum_required(VERSION 2.6)
PROJECT(pushserver)

AUX_SOURCE_DIRECTORY(./ SRC_LIST1)
AUX_SOURCE_DIRECTORY(./socket SRC_LIST2)
AUX_SOURCE_DIRECTORY(../base/pb/protocol SRC_LIST3)


SET(EXECUTABLE_OUTPUT_PATH  ./)
if(APPLE)
    SET(PB_LIB_DIR ../base/pb/lib/mac)
elseif(UNIX)
    SET(PB_LIB_DIR ../base/pb/lib/linux)
elseif(MSVC)
    SET(PB_LIB_DIR ../lib)
    SET(EXECUTABLE_OUTPUT_PATH  ../bin)
endif()

if(MSVC)
    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
    -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
    -DAC_HAS_DEBUG -DLINUX_DAEMON -D_HAS_STD_BYTE=0 /GL /Zc:auto /W1 /std:c++17)
else()
    ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
    -DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
    -DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++17 )
endif()

INCLUDE_DIRECTORIES(./ ../base/pb/protocol ../base/pb ../base/slog)
LINK_DIRECTORIES(./ ${PB_LIB_DIR} ../base/slog/lib)

ADD_EXECUTABLE(push_server ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})
if(MSVC)
    message("PROJECT: ${CMAKE_GENERATOR}")
    # having 'debug' listed twice, is that ok?
    TARGET_LINK_LIBRARIES(push_server slog libssl debug libprotobuf-lited optimized libprotobuf-lite debug pthreadVC3d optimized pthreadVC3)
else()
    TARGET_LINK_LIBRARIES(push_server protobuf-lite pthread ssl slog)
endif()