cmake_minimum_required(VERSION 3.16)

project(ImageProcessingGUI VERSION 0.1 LANGUAGES CXX)

# Enable automatic Qt features
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt libraries
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Define project sources
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ImageIO.cpp
    IntensityTransformations.cpp
    imageprocessingbackend.cpp
    imageprocessingbackend.h
    ImageIO.h
    IntensityTransformations.h
    ImageFilter.cpp
    ImageFilter.h
)

# Define the executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ImageProcessingGUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ImageIO.cpp
        IntensityTransformations.cpp
    )
else()
    if(ANDROID)
        add_library(ImageProcessingGUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ImageProcessingGUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link the Qt Widgets library
target_link_libraries(ImageProcessingGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# macOS-specific properties (optional)
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ImageProcessingGUI)
endif()
set_target_properties(ImageProcessingGUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install targets
include(GNUInstallDirs)
install(TARGETS ImageProcessingGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ImageProcessingGUI)
endif()
