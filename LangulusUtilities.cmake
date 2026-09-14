include(FetchContent)

# Utility for fetching langulus libraries using FetchContent                  
function(fetch_langulus_module NAME GIT_TAG TAG)
	if (LANGULUS)
		message(FATAL_ERROR "[LANGULUS] You can't fetch Langulus${NAME}, because this build \
		indicates LANGULUS is being build along your project. The library you're \
		trying to fetch should already be available locally.")
	endif()

    if(NOT DEFINED LANGULUS_EXTERNAL_DIRECTORY)
        set(LANGULUS_EXTERNAL_DIRECTORY "${CMAKE_SOURCE_DIR}/external" CACHE PATH
            "Place where external dependencies will be downloaded")
        message(STATUS "[LANGULUS] LANGULUS_EXTERNAL_DIRECTORY not defined, using default: ${LANGULUS_EXTERNAL_DIRECTORY}")
    endif()

   # Completely avoid downloading or updating anything, once the appropriate  
   # folder exists                                                            
   string(TOUPPER Langulus${NAME} UPPERCASE_NAME)
   if (EXISTS "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src")
      set(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src" CACHE INTERNAL "" FORCE)
      message(STATUS "[LANGULUS] Reusing the cached external library Langulus${NAME}")
      message(STATUS "[LANGULUS] (delete ${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src manually if you want to redownload)")
   else()
      unset(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} CACHE)
      message(STATUS "[LANGULUS] Freshly downloading external library Langulus${NAME} from https://github.com/Langulus/${NAME}.git")
   endif()

   FetchContent_Declare(
      Langulus${NAME}
      GIT_REPOSITORY  https://github.com/Langulus/${NAME}.git
      GIT_TAG         ${TAG}
      SOURCE_DIR      "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src"
      SUBBUILD_DIR    "${CMAKE_BINARY_DIR}/external/Langulus${NAME}-subbuild"
      ${ARGN}
   )
   FetchContent_MakeAvailable(Langulus${NAME})
endfunction()

# Utility for fetching external libraries using FetchContent                  
function(fetch_external_module NAME GIT_REPOSITORY REPO GIT_TAG TAG)
    if(NOT DEFINED LANGULUS_EXTERNAL_DIRECTORY)
        set(LANGULUS_EXTERNAL_DIRECTORY "${CMAKE_SOURCE_DIR}/external" CACHE PATH
            "Place where external dependencies will be downloaded")
        message(STATUS "[LANGULUS] LANGULUS_EXTERNAL_DIRECTORY not defined, using default: ${LANGULUS_EXTERNAL_DIRECTORY}")
    endif()

   # Completely avoid downloading or updating anything, once the appropriate  
   # folder exists                                                            
   string(TOUPPER ${NAME} UPPERCASE_NAME)
   if (EXISTS "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src")
      set(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src" CACHE INTERNAL "" FORCE)
      message(STATUS "[LANGULUS] Reusing the cached external library ${NAME}")
      message(STATUS "[LANGULUS] (delete ${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src manually if you want to redownload)")
   else()
      unset(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} CACHE)
      message(STATUS "[LANGULUS] Freshly downloading external library ${NAME} from ${REPO} ...")
   endif()

   FetchContent_Declare(
      ${NAME}
      GIT_REPOSITORY  ${REPO}
      GIT_TAG         ${TAG}
      SOURCE_DIR      "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src"
      SUBBUILD_DIR    "${CMAKE_BINARY_DIR}/external/${NAME}-subbuild"
      ${ARGN}
      EXCLUDE_FROM_ALL
   )
   FetchContent_MakeAvailable(${NAME})

   string(TOLOWER ${NAME} LOWERCASE_NAME)
   set(${NAME}_SOURCE_DIR "${${LOWERCASE_NAME}_SOURCE_DIR}" CACHE INTERNAL "${NAME} source directory")
   set(${NAME}_BINARY_DIR "${${LOWERCASE_NAME}_BINARY_DIR}" CACHE INTERNAL "${NAME} binary directory")
endfunction()

# Create a library dependent on build configuration                           
function(add_langulus_library NAME)
   set(multiValueArgs SOURCES LIBRARIES DEPENDENCIES EMSCRIPTEN_COMPILE_FLAGS EMSCRIPTEN_LINK_FLAGS)
   cmake_parse_arguments(PARSE_ARGV 0 arg "" "" "${multiValueArgs}")

   if (EMSCRIPTEN AND LANGULUS_OPTION_SHARED_LIBRARIES)
      # When building for emscripten, we "fake" a shared library by           
      # creating an executable with exports and no entry point                
      # This "fake" shared library has to be linked in a specific way         
      # only from wasm MAIN_MODULEs, so that both LangulusCore                
      # interface is inherited, and the shared library is loaded in           
      # at startup. Here's an example:                                        
      #	target_link_libraries(LangulusLoggerTest PRIVATE                     
      #		$<TARGET_FILE:LangulusLogger> LangulusLogger                      
      #		^ 							  ^                                          
      #		+- Links the *.wasm file      +- Inherits interface               
      # https://github.com/emscripten-core/emscripten/issues/17804            
      add_executable(${NAME} ${arg_SOURCES})
      set_target_properties(${NAME} PROPERTIES
         ENABLE_EXPORTS ON
         COMPILE_FLAGS  "-sSIDE_MODULE --no-entry -fPIC ${arg_EMSCRIPTEN_COMPILE_FLAGS}"
         LINK_FLAGS     "-sSIDE_MODULE -sWASM=1 --no-entry -fPIC ${arg_EMSCRIPTEN_LINK_FLAGS}"
         SUFFIX		   ".wasm"
      )

      # When building for emscripten, our shared libraries are "fake"         
      # and have to be linked in a specific way from wasm MAIN_MODULEs        
      # - once by using the *.wasm file, and once by using the shared         
      # library target.                                                       
      # Any *.so files on the other hand must be packed in a *.data           
      # file by using --preload-file with all the required mods, like         
      # so: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LangulusMod*.so                 
      foreach(ITEM ${arg_LIBRARIES})
         get_target_property(target_type ${ITEM} TYPE)
         if (target_type STREQUAL "SHARED_LIBRARY")
            target_link_libraries(${NAME} PRIVATE $<TARGET_FILE:${ITEM}> ${ITEM})
         else()
            target_link_libraries(${NAME} PRIVATE ${ITEM})
         endif ()
      endforeach()
   else()
      add_library(${NAME} ${LANGULUS_LIBRARY_TYPE} ${arg_SOURCES})
      target_link_libraries(${NAME} PRIVATE ${arg_LIBRARIES})
   endif()
      
   foreach(ITEM ${arg_DEPENDENCIES})
      add_dependencies(${NAME} ${ITEM})
   endforeach()
endfunction()

# Create a shared module library												
function(add_langulus_mod NAME)
   if (EMSCRIPTEN)
      # When building for emscripten, we "fake" a shared library by	
      # creating an executable with exports and no entry point		
      # This "fake" shared library has to be linked in a specific way	
      # only from wasm MAIN_MODULEs, so that both LangulusCore		
      # interface is inherited, and the shared library is loaded in	
      # at startup (mainly used in mod tests). You don't have to link 
      # a mod at all, if you plan on dlopen-ing it at runtime. 		
      # All Langulus symbols will be dangling until you do that,		
      # hence the additional -sERROR_ON_UNDEFINED_SYMBOLS=0			
      add_executable(${NAME} ${ARGN})
      set_property(TARGET ${NAME} APPEND PROPERTY
         COMPILE_FLAGS "-sSIDE_MODULE --no-entry -fPIC")
      set_property(TARGET ${NAME} APPEND PROPERTY
         LINK_FLAGS "-sSIDE_MODULE -sWASM=1 --no-entry -fPIC -sERROR_ON_UNDEFINED_SYMBOLS=0")
      set_target_properties(${NAME} PROPERTIES
         ENABLE_EXPORTS ON
         SUFFIX ".so"
      )
   else()
      add_library(${NAME} SHARED ${ARGN})
   endif()

   # Lack of $<TARGET_FILE:Langulus> is INTENTIONAL					
   # - we wouldn't want a mod to load any base framework symbols		
   target_link_libraries(${NAME} PRIVATE Langulus)
endfunction()

# Create an executable															
function(add_langulus_app NAME)
   set(multiValueArgs SOURCES LIBRARIES DEPENDENCIES EMSCRIPTEN_COMPILE_FLAGS EMSCRIPTEN_LINK_FLAGS)
   cmake_parse_arguments(PARSE_ARGV 0 arg "" "" "${multiValueArgs}")
   add_executable(${NAME} ${arg_SOURCES})

   if (EMSCRIPTEN)
      # Pack all dependencies into a *.data file                              
      foreach(ITEM ${arg_DEPENDENCIES})
         string(APPEND arg_EMSCRIPTEN_LINK_FLAGS
            " --preload-file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ITEM}.so@${ITEM}.so")
      endforeach()

      # attention: -fwasm-exception are currently supported in several
      # major web browsers, but may not be supported in all			
      # WebAssembly engines yet. Also -sASYNCIFY isn't compatible!	
      set_target_properties(${NAME} PROPERTIES
         COMPILE_FLAGS  "-sMAIN_MODULE -fwasm-exceptions ${arg_EMSCRIPTEN_COMPILE_FLAGS}"
         LINK_FLAGS     "-sMAIN_MODULE -sWASM=1 --emrun -sALLOW_MEMORY_GROWTH -fwasm-exceptions ${arg_EMSCRIPTEN_LINK_FLAGS}"
         SUFFIX         ".html"
      )

      # When building for emscripten, our shared libraries are "fake"         
      # and have to be linked in a specific way from wasm MAIN_MODULEs        
      # - once by using the *.wasm file, and once by using the shared         
      # library target.                                                       
      # Any *.so files on the other hand must be packed in a *.data           
      # file by using --preload-file with all the required mods, like         
      # so: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LangulusMod*.so                 
      foreach(ITEM ${arg_LIBRARIES})
         get_target_property(target_type ${ITEM} TYPE)
         if (target_type STREQUAL "SHARED_LIBRARY")
            target_link_libraries(${NAME} PRIVATE $<TARGET_FILE:${ITEM}> ${ITEM})
         else()
            target_link_libraries(${NAME} PRIVATE ${ITEM})
         endif ()
      endforeach()
   else()
      target_link_libraries(${NAME} PRIVATE ${arg_LIBRARIES})
   endif()

   foreach(ITEM ${arg_DEPENDENCIES})
      add_dependencies(${NAME} ${ITEM})
   endforeach()
endfunction()

# Create a test executable if tests are enabled                               
function(add_langulus_test NAME)
   # Don't fetch langulus libraries if we're building the framework	
   if (NOT LANGULUS)
      fetch_langulus_module(Testing GIT_TAG main)
   endif()

   add_langulus_app(${NAME} ${ARGN})
   target_link_libraries(${NAME} PRIVATE LangulusTesting)
   add_test(
      NAME              ${NAME}
      COMMAND           ${NAME}
      WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
   )
   message(STATUS "[LANGULUS] Test added: ${NAME}")
endfunction()
