include(FetchContent)

function(fetch_external_module NAME GIT_REPOSITORY REPO GIT_TAG TAG)
    if(NOT DEFINED LANGULUS_EXTERNAL_DIRECTORY)
        set(LANGULUS_EXTERNAL_DIRECTORY "${CMAKE_SOURCE_DIR}/external" CACHE PATH
            "Place where external dependencies will be downloaded and configured")
        message(WARNING "LANGULUS_EXTERNAL_DIRECTORY not defined, using default: ${LANGULUS_EXTERNAL_DIRECTORY}")
    endif()

    message(STATUS "Fetching external library ${NAME}...")
    FetchContent_Declare(
        ${NAME}
        GIT_REPOSITORY  ${REPO}
        GIT_TAG         ${TAG}
        SOURCE_DIR      "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src"
        SUBBUILD_DIR    "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-subbuild"
        ${ARGN}
    )
    FetchContent_MakeAvailable(${NAME})
	
	string(TOLOWER ${NAME} LOWERCASE_NAME)
    set(${NAME}_SOURCE_DIR "${${LOWERCASE_NAME}_SOURCE_DIR}" CACHE INTERNAL "")
    set(${NAME}_BINARY_DIR "${${LOWERCASE_NAME}_BINARY_DIR}" CACHE INTERNAL "")
endfunction()