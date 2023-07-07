cmake_minimum_required(VERSION 3.12)
project(ImGuizmo)

CPMAddPackage(
        NAME ImGuizmo
        GITHUB_REPOSITORY CedricGuillemet/ImGuizmo
        GIT_TAG 822be7b44c37dbe98d328739ebe0d5a1ea87ecfc
        DOWNLOAD_ONLY YES
)

# Установка пути до директории с исходными файлами ImGuizmo
set(IMGUIZMO_SOURCE_DIR ${ImGuizmo_SOURCE_DIR})

# Добавление всех исходных файлов в проект
file(GLOB IMGUIZMO_SOURCES ${IMGUIZMO_SOURCE_DIR}/*.cpp)

# Добавление всех заголовочных файлов в проект
file(GLOB IMGUIZMO_HEADERS ${IMGUIZMO_SOURCE_DIR}/*.h)

add_library(ImGuizmo STATIC ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})

target_include_directories(ImGuizmo
        PUBLIC ${IMGUIZMO_SOURCE_DIR}
        )
target_link_libraries(ImGuizmo PRIVATE ImGui)