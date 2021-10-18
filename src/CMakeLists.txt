set(SRC main.cpp PDA.cpp PDA.h CFG.h CFG.cpp)
add_executable(MB ${SRC})
install(TARGETS MB DESTINATION ${PROJECT_SOURCE_DIR}/bin)

