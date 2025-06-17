cmake_minimum_required(VERSION 3.16)
project(WordLadder VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC (Meta Object Compiler)
set(CMAKE_AUTOMOC ON)

# Add executable
add_executable(WordLadder
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Graph.h
    WordLadderEngine.h
    GameStats.h
)

# Link Qt libraries
target_link_libraries(WordLadder Qt6::Core Qt6::Widgets)

# Set executable properties
set_target_properties(WordLadder PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Copy dictionary file to build directory (if exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/dictionary.txt")
    configure_file("${CMAKE_SOURCE_DIR}/dictionary.txt"
                   "${CMAKE_BINARY_DIR}/dictionary.txt" COPYONLY)
endif()

# Install target
install(TARGETS WordLadder
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
