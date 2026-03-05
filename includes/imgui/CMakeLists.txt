add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(imgui PUBLIC
    opengl32
)
