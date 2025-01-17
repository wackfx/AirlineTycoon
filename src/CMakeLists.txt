project (AT)

add_library(common INTERFACE)

if(MSVC)
    add_compile_options("/MP")
else()
    target_compile_options(common INTERFACE "-Wall")
    target_compile_options(common INTERFACE "-Wextra")
    target_compile_options(common INTERFACE "-Wno-format-nonliteral")
    target_compile_options(common INTERFACE "-Wno-sign-compare")
    target_compile_options(common INTERFACE "-Wno-type-limits")

    target_compile_options(common INTERFACE "-Wcast-align")
    target_compile_options(common INTERFACE "-Wcast-qual")
    target_compile_options(common INTERFACE "-Wctor-dtor-privacy")
    target_compile_options(common INTERFACE "-Wdisabled-optimization")
    target_compile_options(common INTERFACE "-Wfloat-equal")
    target_compile_options(common INTERFACE "-Wformat=2")
    target_compile_options(common INTERFACE "-Winit-self")
    target_compile_options(common INTERFACE "-Winline")
    target_compile_options(common INTERFACE "-Wlogical-op")
    target_compile_options(common INTERFACE "-Wmissing-declarations")
    target_compile_options(common INTERFACE "-Wmissing-include-dirs")
    target_compile_options(common INTERFACE "-Wnoexcept")
    #target_compile_options(common INTERFACE "-Wold-style-cast")
    target_compile_options(common INTERFACE "-Woverloaded-virtual")
    target_compile_options(common INTERFACE "-Wredundant-decls")
    #target_compile_options(common INTERFACE "-Wshadow")
    #target_compile_options(common INTERFACE "-Wsign-conversion")
    target_compile_options(common INTERFACE "-Wsign-promo")
    target_compile_options(common INTERFACE "-Wstrict-null-sentinel")
    target_compile_options(common INTERFACE "-Wstrict-overflow=2")
    target_compile_options(common INTERFACE "-Wswitch-default")
    target_compile_options(common INTERFACE "-Wundef")
    target_compile_options(common INTERFACE "-Wunsafe-loop-optimizations")
endif()

add_subdirectory (TeakLibW)
add_subdirectory (SBLib)
add_subdirectory (SSE)

include_directories(${PROJECT_SOURCE_DIR})

# Add source to this project's executable.
add_executable (AT
    Buero.cpp
    City.cpp
    DataTabl.cpp
    Route.cpp
    Tips.cpp
    Abend.cpp
    AirportView.cpp
    ArabAir.cpp
    Askbrick.cpp
    AtNet.cpp
    Aufsicht.cpp
    Auftrag.cpp
    Bank.cpp
    Block.cpp
    Brick.cpp
    Checkup.cpp
    ColorFx.cpp
    Credits.cpp
    DeltaVid.cpp
    Designer.cpp
    Dialog.cpp
    DutyFree.cpp
    Editor.cpp
    Fracht.cpp
    GameFrame.cpp
    Global.cpp
    Globe.cpp
    HalldiskMenu.cpp
    HLine.cpp
    Init.cpp
    Insel.cpp
    Intro.cpp
    Kiosk.cpp
    Klacker.cpp
    Laptop.cpp
    Lastmin.cpp
    Mainfrm.cpp
    Makler.cpp
    Misc.cpp
    Museum.cpp
    Nasa.cpp
    NewGamePopup.cpp
    Options.cpp
    CVideo.cpp
    Outro.cpp
    Person.cpp
    Personal.cpp
    Planer.cpp
    Planetyp.cpp
    PlanProp.cpp
    Player.cpp
    PNet.cpp
    Reise.cpp
    Ricks.cpp
    RouteBox.cpp
    Rushmore.cpp
    Sabotage.cpp
    Sbbm.cpp
    Schedule.cpp
    Security.cpp
    Sim.cpp
    SmackPrs.cpp
    Sound.cpp
    Statistk.cpp
    StdAfx.cpp
    StdRaum.cpp
    Synthese.cpp
    Tafel.cpp
    Takeoff.cpp
    Talker.cpp
    TitlePopup.cpp
    WeltAll.cpp
    Werbung.cpp
    Werkstat.cpp
    World.cpp
    Res/Script2.rc
)

set_target_properties(AT PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../BUILD/"
)

set_target_properties(
    AT PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/../BUILD/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_features(AT PUBLIC cxx_std_17)
    target_compile_options(AT PUBLIC "/Zc:__cplusplus")
    target_compile_options(AT PUBLIC "/std:c++17")
    
    # Release configurations
    target_compile_definitions(AT PRIVATE "$<$<CONFIG:Release>:SENTRY;WIN32;_WINDOWS>")
    target_include_directories(AT PRIVATE "$<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/../libs/CrashRpt/install/include/>")
    target_link_libraries(AT PRIVATE "$<$<CONFIG:Release>:${PROJECT_SOURCE_DIR}/../libs/CrashRpt/install/lib/sentry.lib>")
    set_target_properties(AT PROPERTIES WIN32_EXECUTABLE "true")
    add_compile_options("$<$<CONFIG:Release>:/O2 /Ob2 /Ot /GF /Oi /Oy")
    add_link_options("$<$<CONFIG:Release>:/OPT:REF /OPT:ICF /LTCG")
endif()

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.16.0")
    target_precompile_headers(AT PUBLIC StdAfx.h)
endif()

target_link_libraries(AT PRIVATE
    common
    TeakLibW
    SSE
    SBLib
    libsmacker
    enet
    jansson
    SDL2::SDL2_ttf
    SDL2::SDL2_image
    SDL2::SDL2_mixer
    SDL2::SDL2
    SDL2::SDL2main
)

if (WIN32)
    target_link_libraries(AT PRIVATE ws2_32 Winmm)

    # Copy external deps into our target folder
    add_custom_command(
        TARGET AT PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/dependencies"
        "$<TARGET_FILE_DIR:AT>"
        COMMENT "Copying dependencies to $<TARGET_FILE_DIR:AT>"
    )
endif()

# Does our src folder contains a "gamefiles" folder ? Copy it.
if(EXISTS ${GAMEFILES_FOLDER} AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Cleanup unusued files
    add_custom_command(
        TARGET AT PRE_LINK
        COMMENT "Cleaning up gamefolder"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_MODULE_PATH}/cleanup_folder.cmake" "${GAMEFILES_FOLDER}"
    )
    add_custom_command(
        TARGET AT PRE_LINK
        COMMENT "Copying gamefiles to ${CMAKE_BUILD_TYPE} directory"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_MODULE_PATH}/copy_if_different.cmake" "${GAMEFILES_FOLDER}" "$<TARGET_FILE_DIR:AT>"
    )
endif()

# Copy patched assets into target
add_custom_command(
    TARGET AT PRE_LINK
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_MODULE_PATH}/copy_if_different.cmake" "${PATCH_FOLDER}" "$<TARGET_FILE_DIR:AT>/patch"
    COMMENT "Copying patches to ${CMAKE_BUILD_TYPE} directory"
)

# Compile patched assets with the freshly build executable
add_custom_command(
    TARGET AT POST_BUILD
    COMMAND $<TARGET_FILE:AT> /update-patched-files-only
    COMMENT "Compiling patched assets"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:AT>"
)

# Clean up patched assets from folder
add_custom_command(
    TARGET AT POST_BUILD
    COMMENT "Cleaning up"
    COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:AT>/debug.txt
    COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:AT>/.sentry-native
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_MODULE_PATH}/cleanup_folder.cmake" "$<TARGET_FILE_DIR:AT>/patch" ".txt"
)