# CMakeList.txt : CMake проект для Dungeons, весь процесс сборки и подключаемые внешние библиотеки пишем сюда. 
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Dungeons")

# --- Добавляем nlohmann json - работа с JSON-форматом ---
set(NLOHMANN_JSON_VERSION "v3.12.0")
include(FetchContent)
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG ${NLOHMANN_JSON_VERSION}
)
FetchContent_MakeAvailable(json)

# --- Основной исполняемый файл ---
add_executable(Dungeons
    "uid.h" "time.h" "Random.h" 
    "tui/Terminal.h" "tui/frame_building.h" 
    "EventLoop.h"
    "Dungeons.h"
    "Dungeons.cpp"
 "result.h"  "tui/tui_base.h" "tui/char_styling.h" "tui/tui_matrixes.h" "tui/StyleStack.h")

set_target_properties(Dungeons PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Dungeons PRIVATE nlohmann_json::nlohmann_json)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Dungeons PROPERTY CXX_STANDARD 20)
endif()

# --- GoogleTest ---
include(FetchContent)
set(GOOGLE_TEST_VERSION "v1.17.0")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ${GOOGLE_TEST_VERSION}
)
# Для Windows, чтобы не было конфликтов с MSVC runtime
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()  # Включаем CTest

# --- Добавляем тесты ---
add_executable(DungeonsTests
    "tests/time_test.cpp" 
    "tests/uid_test.cpp"
    "tests/test_main.cpp" 
 "tests/result_test.cpp" "tests/style_stack_test.cpp" "tests/char_styling_test.cpp")
target_link_libraries(DungeonsTests 
  GTest::gtest_main 
  nlohmann_json::nlohmann_json 
)

include(GoogleTest)
gtest_discover_tests(DungeonsTests)