cmake_minimum_required(VERSION 4.0)
project(Comp8051Project)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILES
    main.cpp
    TextureManager.cpp
    map.cpp
    Game.cpp
    Vector2D.cpp
    Collision.cpp
    CollisionSystem.cpp
    tinyxml2.cpp
    AssetManager.cpp
    Scene.cpp
    EventResponseSystem.cpp
    MainMenuSystem.cpp
    RotationSystem.cpp
    CleanupSystem.cpp
    #add all new source files here
)

if (WIN32)
    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_WIN32")
    set(SDL3_INCLUDE_DIR "${SDL3_SDK_ROOT}/include")
    set(SDL3_LIB_DIR "${SDL3_SDK_ROOT}/lib")
    set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
    set(ECS_DIR "${SRC_DIR}/ecs")
    set(ECS_SYS_DIR "${ECS_DIR}/system")
    set(UTIL_DIR "${SRC_DIR}/utils")
    set(VENDOR_DIR "${SRC_DIR}/vendor")
    set(MANAGER_DIR "${SRC_DIR}/manager")
    set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
    set(ANIM_DIR "${ASSETS_DIR}/animations")
    set(EVENT_DIR "${ECS_DIR}/event")

    file(GLOB SDL3_DLLS "${SDL3_SDK_ROOT}/bin/*.dll")
    file(COPY ${SDL3_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

endif ()

if (APPLE)
    set(SDL3_SDK_ROOT "${CMAKE_SOURCE_DIR}/SDL3_SDK_APPLE")
    set(SDL3_INCLUDE_DIR "${SDL3_SDK_ROOT}/include")
    set(SDL3_LIB_DIR "${SDL3_SDK_ROOT}/lib")
endif ()

include_directories(
        ${SDL3_INCLUDE_DIR}
        ${SRC_DIR}
        ${ECS_DIR}
        ${ECS_SYS_DIR}
        ${UTIL_DIR}
        ${VENDOR_DIR}
        ${MANAGER_DIR}
        ${ASSETS_DIR}
        ${ANIM_DIR}
        ${EVENT_DIR}
        #add all new directories here
)
link_directories(
        ${SDL3_LIB_DIR}
)

add_executable(Comp8051Project ${SRC_FILES})

target_link_libraries(Comp8051Project
        PRIVATE
        SDL3
        SDL3_image
        SDL3_ttf
        SDL3_mixer
)

if (APPLE)
    set_target_properties(Comp8051Project PROPERTIES
            BUILD_RPATH "${SDL3_LIB_DIR}"
    )

    target_compile_options(Comp8051Project PRIVATE -Wno-narrowing)
endif ()