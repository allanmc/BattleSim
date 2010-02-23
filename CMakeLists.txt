#this is a test

### Generic native Skirmish AI config
#
#message ("WEW ARE IN BattleSim, here: ${CMAKE_CURRENT_SOURCE_DIR}")
#FIND_LIBRARY(HUGIN_LIBRARY hugin-7.1-vc6 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/lib")
#SET(HUGIN_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/hugin-7.1-vc6.lib")
#LINK_LIBRARIES (brAIn )

INCLUDE_DIRECTORIES(lib)

set(mySourceDirRel         "src") # Common values are "" or "src"
set(additionalSources      ${CUtils_SRC})
set(additionalCompileFlags "")
set(additionalLibraries    ${Cpp_AIWRAPPER_TARGET})

include_directories(BEFORE ${Cpp_AIWRAPPER_GENERATED_SRC_DIR})

ConfigureNativeSkirmishAI(mySourceDirRel additionalSources additionalCompileFlags additionalLibraries)
