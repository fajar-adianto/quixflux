cmake_minimum_required(VERSION 3.16)

project(QuixFlux VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})

find_package(Qt6 6.2 COMPONENTS Core Quick Qml Gui REQUIRED)

qt_add_library(QuixFlux STATIC)
qt_add_qml_module(QuixFlux
    URI QuixFlux
    VERSION 1.0
    SOURCES
        qx_action_creator.h qx_action_creator.cpp
        qx_app_dispatcher.h qx_app_dispatcher.cpp
        qx_app_listener.h qx_app_listener.cpp
        qx_app_listener_group.h qx_app_listener_group.cpp
        qx_app_script.h qx_app_script.cpp
        qx_app_script_group.h qx_app_script_group.cpp
        qx_dispatcher.h qx_dispatcher.cpp
        qx_filter.h qx_filter.cpp
        qx_key_table.h qx_key_table.cpp
        qx_middleware.h qx_middleware.cpp
        qx_middleware_list.h qx_middleware_list.cpp
        qx_object.h qx_object.cpp
        qx_store.h qx_store.cpp
        private/quix_functions.h private/quix_functions.cpp
        private/qx_app_script_dispatcher_wrapper.h private/qx_app_script_dispatcher_wrapper.cpp
        private/qx_app_script_runnable.h private/qx_app_script_runnable.cpp
        private/qx_hook.h private/qx_hook.cpp
        private/qx_listener.h private/qx_listener.cpp
        private/qx_middlewares_hook.h private/qx_middlewares_hook.cpp
        private/qx_signal_proxy.h private/qx_signal_proxy.cpp
)

set_target_properties(QuixFlux PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER quixflux
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(QuixFlux
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QuixFlux
    PRIVATE Qt6::Quick Qt6::Qml Qt6::Core)

target_include_directories(QuixFlux PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(private)

# Example Project
# qt_add_executable(ExampleProject example/example.cpp)
# qt_add_qml_module(ExampleProject
#     URI ExampleProjectApp
#     VERSION 1.0
#     QML_FILES example/example.qml
# )

# target_link_libraries(ExampleProject PRIVATE Qt6::Quick QuixFluxplugin)
# target_compile_definitions(ExampleProject PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
