cmake_minimum_required(VERSION 3.10)

project(gmtk_game_jam_2021)

add_executable(gmtk_game code/main.c)

# Enable extra warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wno-variadic-macros -W -Wpedantic -Wextra -Wall -Wcast-align -Wcast-qual -Wstrict-aliasing=2 -Wframe-larger-than=32768 -Wno-strict-overflow -Wsync-nand -Wtrampolines -Wsign-compare -Wfloat-equal -Wmissing-braces -Winit-self -Wlogical-op -Wwrite-strings -Waddress -Warray-bounds -Wchar-subscripts -Wenum-compare -Wimplicit-int -Wempty-body -Wmain -Waggressive-loop-optimizations -Wnonnull -Wparentheses -Wpointer-sign -Wreturn-type -Wsequence-point -Wuninitialized -Wvolatile-register-var -Wignored-qualifiers -Wmissing-parameter-type -Wold-style-declaration -Wno-error=maybe-uninitialized -Wno-unused-function -Wodr -Wformat-signedness -Wsuggest-final-types -Wsuggest-final-methods -Wno-ignored-attributes -Wno-missing-field-initializers -Wshift-overflow=2 -Wduplicated-cond -Wduplicated-branches -Wrestrict -Wdouble-promotion -Wformat=2")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -Wno-variadic-macros -W -Wpedantic -Wextra -Wall -Wcast-align -Wcast-qual -Wstrict-aliasing=2 -Wframe-larger-than=32768 -Wno-strict-overflow -Wsync-nand -Wtrampolines -Wsign-compare -Wfloat-equal -Wmissing-braces -Winit-self -Wlogical-op -Wwrite-strings -Waddress -Warray-bounds -Wchar-subscripts -Wenum-compare -Wimplicit-int -Wempty-body -Wmain -Waggressive-loop-optimizations -Wnonnull -Wparentheses -Wpointer-sign -Wreturn-type -Wsequence-point -Wuninitialized -Wvolatile-register-var -Wignored-qualifiers -Wmissing-parameter-type -Wold-style-declaration -Wno-error=maybe-uninitialized -Wno-unused-function -Wodr -Wformat-signedness -Wsuggest-final-types -Wsuggest-final-methods -Wno-ignored-attributes -Wno-missing-field-initializers -Wshift-overflow=2 -Wduplicated-cond -Wduplicated-branches -Wrestrict -Wdouble-promotion -Wformat=2")

target_include_directories(gmtk_game PUBLIC include)
target_link_libraries(gmtk_game m SDL2 SDL2_image SDL2_ttf)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
