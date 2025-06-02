cmake_minimum_required(VERSION 3.16)
project(CANtracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialBus)

# === Include directories ===
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/data)
include_directories(${CMAKE_SOURCE_DIR})

# === Link directories ===
link_directories(${CMAKE_SOURCE_DIR}/data)

qt6_add_resources(RESOURCES_ADDED resource.qrc)

# Set SKIP_AUTOGEN for lblf header files
set_source_files_properties(
    lblf_reader/lblf/blf_reader.hh
    lblf_reader/lblf/blf_structs.hh
    lblf_reader/lblf/utils.hh
    PROPERTIES SKIP_AUTOGEN ON
)

add_executable(CANtracing
    main.cpp
    src/Configuration.cpp
    src/MainWindow.cpp
    src/LoadingOverlay.cpp
    src/RunButton.cpp
    src/Msetup/MSetupWindow.cpp
    src/Sisetup/SimulationWindow.cpp
    src/Offline/OfflineModeWindow.cpp
    src/Trace/TraceWindow.cpp
    src/GraphicWindow.cpp
    src/ConvertWindow.cpp
    src/Msetup/OfflineMode/offlineMode.cpp
    src/Msetup/OnlineMode/onlineMode.cpp
    src/ChMap/HwMappingWindow.cpp
    src/ChMap/ChannelMappingDialog.cpp
    lblf_reader/BlfReader.cpp
    lblf_reader/lblf/blf_reader.cpp
    include/ChannelMappingDialog.h
    include/HwMappingWindow.h
    include/RunButton.h
    include/onlineMode.h
    include/offlineMode.h
    include/MainWindow.h
    include/MSetupWindow.h
    include/Configuration.h
    include/SimulationWindow.h
    include/OfflineModeWindow.h
    include/TraceWindow.h
    include/GraphicWindow.h
    include/ConvertWindow.h
    include/ClickableLabel.h
    include/LoadingOverlay.h
    lblf_reader/CanMessage.hpp
    lblf_reader/BlfReader.hpp
    lblf_reader/lblf/blf_reader.hh
    lblf_reader/lblf/blf_structs.hh
    lblf_reader/lblf/utils.hh
    app.rc
    ${RESOURCES_ADDED}
)

set(VXLAPI_LIB "${CMAKE_SOURCE_DIR}/data/libvxlapi64.a")

# === Link Qt and vxlapi ===
target_link_libraries(CANtracing PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialBus
    ${VXLAPI_LIB}  # <--- add this line to link vxlapi.lib
)

# === Copy folder data vào thư mục build ===
set(DATA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/data")
set(DATA_DEST_DIR "${CMAKE_BINARY_DIR}/data")

add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DATA_SOURCE_DIR}"
        "${DATA_DEST_DIR}"
    COMMENT "Copy data folder to build directory"
)

add_dependencies(CANtracing copy_data)

# === Copy DLL Qt cần thiết ===
set(Qt6_DLLS
    "${CMAKE_SOURCE_DIR}/Qt6/Qt6Core.dll"
    "${CMAKE_SOURCE_DIR}/Qt6/Qt6Gui.dll"
    "${CMAKE_SOURCE_DIR}/Qt6/Qt6Widgets.dll"
    "${CMAKE_SOURCE_DIR}/Qt6/libb2-1.dll"
)

add_custom_target(copy_qt_dlls ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt6_DLLS} ${CMAKE_BINARY_DIR}
)

foreach(dll ${Qt6_DLLS})
    add_custom_command(TARGET copy_qt_dlls POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dll}
        ${CMAKE_BINARY_DIR}
    )
endforeach()

add_dependencies(CANtracing copy_qt_dlls)

# === Copy plugin Qt platforms ===
set(Qt_Platform_Plugin "${CMAKE_SOURCE_DIR}/platforms/qwindows.dll")

add_custom_command(TARGET CANtracing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:CANtracing>/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${Qt_Platform_Plugin}"
            "$<TARGET_FILE_DIR:CANtracing>/platforms/"
)