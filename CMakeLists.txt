cmake_minimum_required(VERSION 3.10)

project(gmtk_game_jam_2021)

add_executable(gmtk_game code/main.c)

# Enable extra warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wno-variadic-macros -W -Wpedantic -Wextra -Wall -Wcast-align -Wcast-qual -Wstrict-aliasing=2 -Wframe-larger-than=32768 -Wno-strict-overflow -Wsync-nand -Wtrampolines -Wsign-compare -Werror=float-equal -Werror=missing-braces -Werror=init-self -Werror=logical-op -Werror=write-strings -Werror=address -Werror=array-bounds -Werror=char-subscripts -Werror=enum-compare -Werror=implicit-int -Werror=empty-body -Werror=main -Werror=aggressive-loop-optimizations -Werror=nonnull -Werror=parentheses -Werror=pointer-sign -Werror=return-type -Werror=sequence-point -Werror=uninitialized -Werror=volatile-register-var -Werror=ignored-qualifiers -Werror=missing-parameter-type -Werror=old-style-declaration -Wno-error=maybe-uninitialized -Wno-unused-function -Wodr -Wformat-signedness -Wsuggest-final-types -Wsuggest-final-methods -Wno-ignored-attributes -Wno-missing-field-initializers -Wshift-overflow=2 -Wduplicated-cond -Wduplicated-branches -Werror=restrict -Wdouble-promotion -Wformat=2")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -Wno-variadic-macros -W -Wpedantic -Wextra -Wall -Wcast-align -Wcast-qual -Wstrict-aliasing=2 -Wframe-larger-than=32768 -Wno-strict-overflow -Wsync-nand -Wtrampolines -Wsign-compare -Werror=float-equal -Werror=missing-braces -Werror=init-self -Werror=logical-op -Werror=write-strings -Werror=address -Werror=array-bounds -Werror=char-subscripts -Werror=enum-compare -Werror=implicit-int -Werror=empty-body -Werror=main -Werror=aggressive-loop-optimizations -Werror=nonnull -Werror=parentheses -Werror=pointer-sign -Werror=return-type -Werror=sequence-point -Werror=uninitialized -Werror=volatile-register-var -Werror=ignored-qualifiers -Werror=missing-parameter-type -Werror=old-style-declaration -Wno-error=maybe-uninitialized -Wno-unused-function -Wodr -Wformat-signedness -Wsuggest-final-types -Wsuggest-final-methods -Wno-ignored-attributes -Wno-missing-field-initializers -Wshift-overflow=2 -Wduplicated-cond -Wduplicated-branches -Werror=restrict -Wdouble-promotion -Wformat=2")

target_include_directories(gmtk_game PUBLIC include)
target_link_libraries(gmtk_game SDL2 SDL2_image)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
