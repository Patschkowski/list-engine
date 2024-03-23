cmake_minimum_required(VERSION 3.28)

project(LIST)

find_package(Doxygen)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(list WIN32
  "src/helpers.h"
  "src/helper_botActions.c"
  "src/helper_botParser.c"
  "src/helper_draw.c"
  "src/helper_mapParser.c"
  "src/helper_parser.c"
  "src/helper_savegame.c"
  "src/helper_tokenizer.c"
  "src/init_game.c"
  "src/init_sdl.c"
  "src/lists.c"
  "src/main.c"
  "src/shared.h"
  "src/state_loading.c"
  "src/state_menu.c"
  "src/state_playing.c"
  "src/state_saving.c")
target_compile_features(list PRIVATE c_std_90)
target_link_libraries(list PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf)

if(DOXYGEN_FOUND)
  set(DOXYGEN_EXCLUDE_PATTERNS */build_msvc/* */.vs/*)
  set(OUTPUT_LANGUAGE German)
  doxygen_add_docs(doxygen ${CMAKE_CURRENT_SOURCE_DIR} ALL)
endif()