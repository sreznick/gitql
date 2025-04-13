CMAKE_MINIMUM_REQUIRED(VERSION 3.25)
project(gitql)

LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

SET (CMAKE_CXX_STANDARD 23)

set(ANTLR4CPP_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.13.2-complete.jar)
include( ExternalAntlr4Cpp )

include_directories( ${ANTLR4CPP_INCLUDE_DIRS} )
link_directories( ${ANTLR4CPP_LIBS} )
message(STATUS "Found antlr4cpp libs: ${ANTLR4CPP_LIBS} and includes: ${ANTLR4CPP_INCLUDE_DIRS} ")

antlr4cpp_process_grammar(gitql gitql
  ${CMAKE_CURRENT_SOURCE_DIR}/TLexer.g4
  ${CMAKE_CURRENT_SOURCE_DIR}/TParser.g4)
include_directories(${antlr4cpp_include_dirs_gitql})

add_executable(gitql main.cpp ${antlr4cpp_src_files_gitql}  ${antlr4cpp_generation_gitql})
add_dependencies(gitql antlr4cpp antlr4cpp_generation_gitql)
target_link_libraries(gitql antlr4-runtime)

