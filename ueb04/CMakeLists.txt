# Minimum CMake Version
cmake_minimum_required (VERSION 3.3)

# Project Name
project(ueb04)

set(RuntimeOutputDir ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RuntimeOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${RuntimeOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RuntimeOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${RuntimeOutputDir})

# Compiler Flags
if(MSVC)
	# Setzten des Warnunglevels auf (Wall) unter Windows
	# behandeln der Warnungen als Fehler (WX)
	if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
	else()
		set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
	endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	if(APPLE) 
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long")
	else()
	  	# Setztes 
	  	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -Werror")
	endif()
endif()

# Optimierung 
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Versionsnummer
set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)

# Check ob OpenGL installiert ist
find_package(OpenGL REQUIRED)

#UNIX SYSTEM
if(UNIX)
	# Check ob glut installiert
	find_package(GLUT REQUIRED)

	# setzten der Include Directories

	include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif() 

#WINDOWS SYSTEM
if(WIN32)
    if(MSVC)
		# Setzten der Windows vorkompilierten Bibliotheken
		if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")	#x32
    			link_directories(${CMAKE_SOURCE_DIR}/../win/libs/)
		endif()
		add_compile_options(/wd4204)
    endif()

        include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../win/include)

	# nicht ZERO_CHECK bauen
	set(CMAKE_SUPPRESS_REGENERATION true)
	# cmake Folder ALL_BUILD in Filter/Unterordner
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	if(MSVC)
		# ${PROJECT_NAME} als Startup-Projekt setzten
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	endif()
endif()

# hinzufuegen aller files im src Folder
file(GLOB src_files
	"src/*.h"
	"src/*.c"
)

# erstellen des Targets ${PROJECT_NAME} 
add_executable(${PROJECT_NAME} ${src_files})

#Visual Studio
# erstellen der filter fuer die content-files
foreach(source IN LISTS content)
    get_filename_component(source_path "${source}" PATH)
    file(RELATIVE_PATH pathR "${CMAKE_CURRENT_SOURCE_DIR}" "${source_path}")
    string(REPLACE "/" "\\" source_path_ide "${pathR}")
    source_group("${source_path_ide}" FILES "${source}")
endforeach()

# linken der Libraries
if(WIN32)
        target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} glut32 freeglut_static freeglut)
elseif(APPLE) #apple
	target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} m ${GLUT_LIBRARY})
else()
        target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_gl_LIBRARY} m glut GLU)
endif()

# C Standard
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

#copy dll file
#WINDOWS SYSTEM
if(WIN32)
file(COPY ${CMAKE_SOURCE_DIR}/../win/libs/glut32.dll DESTINATION ${RuntimeOutputDir})
endif()
