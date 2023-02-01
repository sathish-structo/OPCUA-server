







set(LIB_NAME "CPPTestServer")
file(GLOB SERVER_SRC "src/*.cpp")
#file(GLOB SERVER_HEADERS "*.h")

set(LIBRARY_FILES ${SERVER_SRC})

set(INCLUDE_DIRS
	"include"
	"${SDK_BASE}/Core/include"
	"${SDK_BASE}/api/include"
	)
	
include_directories(${INCLUDE_DIRS})
link_directories(${LINK_SDK_LIB_DIR})
#link_directories("/home/chanchal/OpcUaSDK/ReOPCUACppSDK/OpcUACppSDK/Source/lib/linux/x86_64/gcc9/release")

set(LIBRARY_LIBS "tb5cpp" "tb5ot" "tb5util" "tb5stack")


foreach(pLib ${LIBRARY_LIBS} )
	list(APPEND LIBRARIES "-l${pLib}")
endforeach(pLib)

## Set properties on files
set_source_files_properties(${LIBRARY_FILES} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pthread")
message(${LIBRARIES})

#add_library(tb5cpp STATIC IMPORTED)
#set_target_properties(tb5cpp PROPERTIES
#  IMPORTED_LOCATION "${LINK_SDK_LIB_DIR}/libtb5cpp.a")

#add_library(${LIB_NAME} ${LIBRARY_FILES})
add_executable(${LIB_NAME} ${LIBRARY_FILES})

## Link all external libraries
#set_property(TARGET somelib PROPERTY
#             IMPORTED_LOCATION ${LINK_SDK_LIB_DIR}/libtb5cpp.a ${LINK_SDK_LIB_DIR}/libtb5ot.a ${LINK_SDK_LIB_DIR}/libtb5util.a ${LINK_SDK_LIB_DIR}/libtb5stack.a)


find_library(LUA_LIB -ltb5cpp)
if(NOT LUA_LIB)
#  message(FATAL_ERROR "library not found")
endif()

#set(LIB_SDK tb5cpp tb5ot tb5util tb5stack)
#add_library(LIB_SDK STATIC IMPORTED)
#set_property(TARGET LIB_SDK PROPERTY
#             IMPORTED_LOCATION "${LINK_SDK_LIB_DIR}/")

#target_link_libraries(${LIB_NAME} ${LIBRARIES})
#target_link_libraries(${LIB_NAME} ${LIBRARY_LIBS})
#target_link_libraries(${LIB_NAME} abcd)
#target_link_libraries(${LIB_NAME} ${LINK_SDK_LIB_DIR}/libtb5cpp.a ${LINK_SDK_LIB_DIR}/libtb5ot.a ${LINK_SDK_LIB_DIR}/libtb5util.a ${LINK_SDK_LIB_DIR}/libtb5stack.a)
#find_library(LIB_FIND tb5cpp tb5ot tb5util tb5stack)
target_link_libraries(${LIB_NAME} tb5cpp tb5ot tb5util tb5stack xml2 ssl crypto pthread rt m dl)
#target_link_libraries(${LIB_NAME} somelib)

install(
	# Install the Libraries
	TARGETS
		${LIB_NAME}
		

	DESTINATION
		${INTSTALL_DIR}
		)

