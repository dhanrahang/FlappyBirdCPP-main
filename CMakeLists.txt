cmake_minimum_required(VERSION 3.20)
project(FlappyBird)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

add_executable(game game.cpp) 

if(EMSCRIPTEN)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL3)

    set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_BMP OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_GIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_JPG OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_LBM OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_PCX OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_PNG OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_PNM OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_QOI OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_SVG OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TGA OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_XCF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_XPM OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_XV OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL3_image)

    target_link_libraries(game PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

    target_link_options(game PRIVATE
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/img@/img"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/audio@/audio"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sASSERTIONS=1"
    )
    set_target_properties(game PROPERTIES SUFFIX ".html")
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL3 QUIET IMPORTED_TARGET sdl3)
        pkg_check_modules(SDL3_IMAGE QUIET IMPORTED_TARGET SDL3_image)
    endif()

    if(TARGET PkgConfig::SDL3 AND TARGET PkgConfig::SDL3_IMAGE)
        target_link_libraries(game PRIVATE PkgConfig::SDL3 PkgConfig::SDL3_IMAGE)
    else()
        find_package(SDL3 REQUIRED)
        find_package(SDL3_image REQUIRED)
        target_link_libraries(game PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
    endif()
endif()

target_include_directories(game PRIVATE include)

if(NOT EMSCRIPTEN)
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/img $<TARGET_FILE_DIR:game>/img
    )

    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/audio $<TARGET_FILE_DIR:game>/audio
    )
endif()
