cmake_minimum_required(VERSION 2.8.8)

project(pilight C)

set(PROJECT_VERSION 5.0)
set(PROJECT_NAME pilight)

include(${PROJECT_SOURCE_DIR}/CMakeConfig.txt)

set(MODULESPACK OFF)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)
set(CPACK_DEB_COMPONENT_INSTALL TRUE)

#Final compilation all platforms
#Removing debugging for final compilation
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_EXE_LINKER_FLAGS " -Wl,-rpath=/usr/local/lib/,-rpath=/usr/lib/,-rpath=/lib/")
set(CMAKE_SHARED_LINKER_FLAGS " -Wl,-rpath=/usr/local/lib/,-rpath=/usr/lib/,-rpath=/lib/")
set(CMAKE_MODULE_LINKER_FLAGS " -Wl,-rpath=/usr/local/lib/,-rpath=/usr/lib/,-rpath=/lib/")

set(CMAKE_ZLIB_LIBS_INIT)

find_library(CMAKE_ZLIB_LIBS_INIT 
			NAME z
            PATHS   /usr/lib
                    /usr/lib32
                    /usr/lib64
                    /usr/lib/i386-linux-gnu		
                    /usr/lib/x86_64-linux-gnu
                    /usr/local/lib
                    /usr/local/lib32
                    /usr/local/lib64
                    /usr/lib/arm-linux-gnueabi
                    /usr/lib/arm-linux-gnueabihf
			NO_DEFAULT_PATH)
			
message(STATUS "Looking for libz")
if(${CMAKE_ZLIB_LIBS_INIT} MATCHES "CMAKE_ZLIB_LIBS_INIT-NOTFOUND")
	message(FATAL_ERROR "Looking for libz - not found")
else()
	message(STATUS "Looking for libz - found (${CMAKE_ZLIB_LIBS_INIT})")
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "armv6" OR CPACK_DEBIAN_PACKAGE_ARCHITECTURE MATCHES "armhf")
	if(${USE_SOFT_FLOAT} MATCHES "OFF")
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
		set(CMAKE_C_FLAGS "-lz -Ofast -mfloat-abi=hard -mfpu=vfp -march=armv6 -ffast-math -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wformat=2 -g -Wall -Wconversion -Wunreachable-code -Wstrict-prototypes")
	else()
		set(CMAKE_C_FLAGS "-lz -Ofast -mfloat-abi=soft -msoft-float -mfpu=vfp -march=armv6 -ffast-math -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wformat=2 -g -Wall -Wconversion -Wunreachable-code -Wstrict-prototypes")
	endif()
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
	set(CMAKE_C_FLAGS "-Ofast -fPIC -march=native -mtune=native -ffast-math -Wconversion -Wunreachable-code -Wstrict-prototypes -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wformat=2 -g3 -Wall")
endif()
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "amd64")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
	set(CMAKE_C_FLAGS "-O2 -fPIC -march=native -mtune=native -ffast-math -Wformat=2 -g3 -Wformat-nonliteral")
endif()

# Compile final sources for Ubuntu 64bit
if(CPACK_DEBIAN_PACKAGE_ARCHITECTURE MATCHES "amd64")
	set(CMAKE_C_FLAGS "-m64 -fPIC")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
endif()
# Compile final sources for Ubuntu 32bit
if(CPACK_DEBIAN_PACKAGE_ARCHITECTURE MATCHES "i386")
	set(CMAKE_C_FLAGS "-m32")
	set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
endif()

# Start uninstaller generator
function(WRITE_UNINSTALL_TARGET_SCRIPT)
    # Create uninstall target template file, if it doesn't exist...
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in)
        set(__uninstall_filename ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake.in)
        # BEGIN actual write to file...
        file(WRITE ${__uninstall_filename} "\# - uninstall target template\n\#")
        file(APPEND ${__uninstall_filename} "
if (NOT EXISTS \"\@CMAKE_CURRENT_BINARY_DIR\@/install_manifest.txt\")
    message(FATAL_ERROR \"Cannot find install manifest: \\\"\@CMAKE_CURRENT_BINARY_DIR\@/install_manifest.txt\\\"\")
endif(NOT EXISTS \"\@CMAKE_CURRENT_BINARY_DIR\@/install_manifest.txt\")

file(READ \"\@CMAKE_CURRENT_BINARY_DIR\@/install_manifest.txt\" files)
string(REGEX REPLACE \"\\n\" \";\" files \"\${files}\")

foreach (file \${files})
    message(STATUS \"Uninstalling \\\"\$ENV{DESTDIR}\${file}\\\"\")
    if (EXISTS \"\$ENV{DESTDIR}\${file}\")
        execute_process(
            COMMAND \@CMAKE_COMMAND\@ -E remove \"\$ENV{DESTDIR}\${file}\"
            OUTPUT_VARIABLE rm_out
            RESULT_VARIABLE rm_retval
        )
        if(NOT \${rm_retval} EQUAL 0)
            message(FATAL_ERROR \"Problem when removing \\\"\$ENV{DESTDIR}\${file}\\\"\")
        endif (NOT \${rm_retval} EQUAL 0)
    else (EXISTS \"\$ENV{DESTDIR}\${file}\")
        message(STATUS \"File \\\"\$ENV{DESTDIR}\${file}\\\" does not exist.\")
    endif (EXISTS \"\$ENV{DESTDIR}\${file}\")
endforeach(file)

execute_process(COMMAND update-rc.d pilight remove)
execute_process(COMMAND ln -sf /usr/local/lib/pilight/libpilight.so.3 /usr/local/lib/libpilight.so)
execute_process(COMMAND ldconfig)

") # END of appending to file...
    endif()
endfunction()
# End of the uninstaller generator

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake_postinstall.cmake)
	set(__uninstall_filename ${CMAKE_CURRENT_BINARY_DIR}/cmake_postinstall.cmake)
	# BEGIN actual write to file...
	file(WRITE ${__uninstall_filename} "execute_process(COMMAND update-rc.d pilight defaults)")
endif()

execute_process(COMMAND git describe --always
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	RESULT_VARIABLE git_result
	OUTPUT_VARIABLE git_ver)

STRING(REGEX REPLACE "\n" "" git_ver "${git_ver}")
add_definitions(-DHASH="${git_ver}")

if(${MODULESPACK} MATCHES "ON")
	add_definitions(-DMODULE="1")
endif()

include_directories("${PROJECT_SOURCE_DIR}/libs/pilight/")
include_directories("${PROJECT_SOURCE_DIR}/libs/avrdude/")

file(GLOB protocol_headers
	"${PROJECT_SOURCE_DIR}/libs/protocols/*.h"
)

file(GLOB protocol
	"${PROJECT_SOURCE_DIR}/libs/protocols/*.c"
)

file(GLOB hardware_headers
	"${PROJECT_SOURCE_DIR}/libs/hardware/*.h"
)

file(GLOB hardware
	"${PROJECT_SOURCE_DIR}/libs/hardware/*.c"
)

file(GLOB pilight_headers
	"${PROJECT_SOURCE_DIR}/libs/pilight/*.h"
	"${PROJECT_SOURCE_DIR}/libs/avrdude/*.h")

file(GLOB pilight
	"${PROJECT_SOURCE_DIR}/libs/pilight/*.c"
	"${PROJECT_SOURCE_DIR}/libs/avrdude/*.c")
	
if(${PROTOCOL_ALECTO_WSD17} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/alecto_wsd17.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/alecto_wsd17.c")
endif()

if(${PROTOCOL_COGEX_SWITCH} MATCHES "OFF"
	AND ${PROTOCOL_KAKU_SWITCH_OLD} MATCHES "OFF"
	AND ${PROTOCOL_INTERTECHNO_OLD} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_switch_old.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_switch_old.c")
endif()

if(${PROTOCOL_KAKU_SCREEN_OLD} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_screen_old.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_screen_old.c")
endif()

if(${PROTOCOL_KAKU_DIMMER} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_dimmer.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_dimmer.c")
endif()

if(${PROTOCOL_KAKU_SCREEN} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_screen.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_screen.c")
endif()

if(${PROTOCOL_KAKU_CONTACT} MATCHES "OFF")
  list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_contact.h")
  list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_contact.c")
endif()

if(${PROTOCOL_COCO_SWITCH} MATCHES "OFF"
	AND ${PROTOCOL_DIO_SWITCH} MATCHES "OFF"
	AND ${PROTOCOL_NEXA_SWITCH} MATCHES "OFF"
	AND ${PROTOCOL_KAKU_SWITCH} MATCHES "OFF"
	AND ${PROTOCOL_INTERTECHNO_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_switch.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/arctech_switch.c")
endif()

if(${PROTOCOL_DS18B20} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/ds18b20.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/ds18b20.c")
endif()

if(${PROTOCOL_DS18S20} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/ds18s20.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/ds18s20.c")
endif()

if(${PROTOCOL_DHT22} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/dht22.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/dht22.c")
endif()

if(${PROTOCOL_ELRO_SWITCH} MATCHES "OFF" AND ${PROTOCOL_BRENNENSTUHL_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/elro_ad.h")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/elro_he.h")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/elro_hc.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/elro_ad.c")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/elro_hc.c")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/elro_he.c")
endif()

if(${PROTOCOL_GENERIC_DIMMER} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/generic_dimmer.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/generic_dimmer.c")
endif()

if(${PROTOCOL_GENERIC_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/generic_switch.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/generic_switch.c")
endif()

if(${PROTOCOL_GENERIC_SCREEN} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/generic_screen.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/generic_screen.c")
endif()

if(${PROTOCOL_GENERIC_WEATHER} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/generic_weather.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/generic_weather.c")
endif()

if(${PROTOCOL_GENERIC_WEBCAM} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/generic_webcam.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/generic_webcam.c")
endif()

if(${PROTOCOL_HOMEEASY_OLD} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/home_easy_old.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/home_easy_old.c")
endif()

if(${PROTOCOL_IMPULS} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/impuls.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/impuls.c")
endif()

if(${PROTOCOL_SELECTREMOTE} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/selectremote.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/selectremote.c")
endif()

if(${PROTOCOL_EHOME} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/ehome.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/ehome.c")
endif()

if(${PROTOCOL_RAW} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/raw.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/raw.c")
endif()

if(${PROTOCOL_RELAY} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/relay.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/relay.c")
endif()

if(${PROTOCOL_REV} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v1.h")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v2.h")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v3.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v1.c")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v2.c")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/rev_v3.c")
endif()

if(${PROTOCOL_CLARUS} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/clarus.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/clarus.c")
endif()

if(${PROTOCOL_RPI_TEMP} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/rpi_temp.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/rpi_temp.c")
endif()

if(${PROTOCOL_CONRAD_RSL_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/conrad_rsl_switch.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/conrad_rsl_switch.c")
endif()

if(${PROTOCOL_CONRAD_WEATHER} MATCHES "OFF"
   AND ${PROTOCOL_TFA} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/tfa.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/tfa.c")
endif()

if(${PROTOCOL_CONRAD_RSL_CONTACT} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/conrad_rsl_contact.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/conrad_rsl_contact.c")
endif()

if(${PROTOCOL_DHT11} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/dht11.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/dht11.c")
endif()

if(${PROTOCOL_DHT22} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/dht22.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/dht22.c")
endif()

if(${PROTOCOL_POLLIN_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/pollin.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/pollin.c")
endif()

if(${PROTOCOL_QUIGG_SWITCH} MATCHES "OFF")
    list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/quigg_switch.h")
    list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/quigg_switch.c")
endif()

if(${PROTOCOL_MUMBI_SWITCH} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/mumbi.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/mumbi.c")
endif()

if(${PROTOCOL_LM75} MATCHES "OFF"
   AND ${PROTOCOL_LM76} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/lm75.h")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/lm76.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/lm75.c")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/lm76.c")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocol/wiringPiI2C.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocol/wiringPiI2C.c")
endif()

if(${PROTOCOL_WUNDERGROUND} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/wunderground.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/wunderground.c")
endif()

if(${PROTOCOL_OPENWEATHERMAP} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/openweathermap.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/openweathermap.c")
endif()

if(${PROTOCOL_SILVERCREST} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/silvercrest.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/silvercrest.c")
endif()

if(${PROTOCOL_CLEVERWATTS} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/cleverwatts.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/cleverwatts.c")
endif()

if(${PROTOCOL_SUNRISESET} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/sunriseset.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/sunriseset.c")
endif()

if(${PROTOCOL_THREECHAN} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/threechan.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/threechan.c")
endif()

if(${PROTOCOL_TEKNIHALL} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/teknihall.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/teknihall.c")
endif()

if(${PROTOCOL_X10} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/x10.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/x10.c")
endif()

if(${PROTOCOL_PROGRAM} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/program.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/program.c")
endif()

if(${PROTOCOL_DATETIME} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/pdatetime.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/pdatetime.c")
endif()

if(${PROTOCOL_XBMC} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/xbmc.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/xbmc.c")
endif()

if(${PROTOCOL_LIRC} MATCHES "OFF")
	list(REMOVE_ITEM protocol_headers "${PROJECT_SOURCE_DIR}/libs/protocols/lirc.h")
	list(REMOVE_ITEM protocol "${PROJECT_SOURCE_DIR}/libs/protocols/lirc.c")
endif()

if(${HARDWARE_433_GPIO} MATCHES "OFF")
	list(REMOVE_ITEM hardware_headers "${PROJECT_SOURCE_DIR}/libs/hardware/433gpio.h")
	list(REMOVE_ITEM hardware "${PROJECT_SOURCE_DIR}/libs/hardware/433gpio.c")
endif()

if(${HARDWARE_433_LIRC} MATCHES "OFF")
	list(REMOVE_ITEM hardware_headers "${PROJECT_SOURCE_DIR}/libs/hardware/433module.h")
	list(REMOVE_ITEM hardware "${PROJECT_SOURCE_DIR}/libs/hardware/433module.c")
endif()

if(${HARDWARE_433_PILIGHT} MATCHES "OFF")
	list(REMOVE_ITEM hardware_headers "${PROJECT_SOURCE_DIR}/libs/hardware/433pilight.h")
	list(REMOVE_ITEM hardware "${PROJECT_SOURCE_DIR}/libs/hardware/433pilight.c")
endif()

if(${UPDATE} MATCHES "OFF")
	list(REMOVE_ITEM pilight_headers "${PROJECT_SOURCE_DIR}/libs/pilight/update.h")
	list(REMOVE_ITEM pilight "${PROJECT_SOURCE_DIR}/libs/pilight/update.c")
endif()

if(${WEBSERVER} MATCHES "OFF")
	list(REMOVE_ITEM pilight_headers "${PROJECT_SOURCE_DIR}/libs/pilight/webserver.h")
	list(REMOVE_ITEM pilight_headers "${PROJECT_SOURCE_DIR}/libs/pilight/mongoose.h")
	list(REMOVE_ITEM pilight "${PROJECT_SOURCE_DIR}/libs/pilight/webserver.c")
	list(REMOVE_ITEM pilight "${PROJECT_SOURCE_DIR}/libs/pilight/mongoose.c")
endif()

file(GLOB avrdude_sources "${PROJECT_SOURCE_DIR}/libs/avrdude/*.c")

if(${FIRMWARE} MATCHES "OFF")
	list(REMOVE_ITEM pilight "${PROJECT_SOURCE_DIR}/libs/pilight/firmware.c")
	list(REMOVE_ITEM pilight ${avrdude_sources})
	file(GLOB avrdude_headers "${PROJECT_SOURCE_DIR}/libs/avrdude/*.h")
	list(REMOVE_ITEM pilight ${avrdude_headers})
endif()

find_package(Threads REQUIRED)

if(${MODULESPACK} MATCHES "OFF")
	if(${UPDATE} MATCHES "ON")
		find_package(ZLIB REQUIRED)
	endif()

	foreach(header ${protocol_headers})
		string(REPLACE ${PROJECT_SOURCE_DIR}/libs "	#include \".." header1 ${header}) 
		string(REPLACE ".h" ".h\"!" header2 ${header1})
		list(APPEND header3 ${header2})
	endforeach(header)

	string(REPLACE "!" "\n" header4 ${header3})
	set(PROTOCOL_HEADERS ${header4})
	configure_file(libs/pilight/protocol_header.h.in libs/pilight/protocol_header.h)	

	execute_process(
	COMMAND grep -h "void .*Init(void)" ${protocol}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		RESULT_VARIABLE protocol_result
		OUTPUT_VARIABLE protocol_init)

	foreach(init ${protocol_init})
		string(REPLACE "void " "	" init1 ${init})
		string(REPLACE "(void) {" "()!" init2 ${init1})
		list(APPEND init3 ${init2})
	endforeach(init)

	string(REPLACE "!" ";" init4 ${init3})
	set(PROTOCOL_INIT ${init4})
	configure_file(libs/pilight/protocol_init.h.in libs/pilight/protocol_init.h)

	set(header3)
	foreach(header ${hardware_headers})
		string(REPLACE ${PROJECT_SOURCE_DIR}/libs "	#include \".." header1 ${header}) 
		string(REPLACE ".h" ".h\"!" header2 ${header1})
		list(APPEND header3 ${header2})
	endforeach(header)

	string(REPLACE "!" "\n" header4 ${header3})
	set(HARDWARE_HEADERS ${header4})
	configure_file(libs/pilight/hardware_header.h.in libs/pilight/hardware_header.h)	

	execute_process(
	COMMAND grep -h "void .*Init(void)" ${hardware}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		RESULT_VARIABLE protocol_result
		OUTPUT_VARIABLE protocol_init)

	set(init3)
	foreach(init ${protocol_init})
		string(REPLACE "void " "	" init1 ${init})
		string(REPLACE "(void) {" "()!" init2 ${init1})
		list(APPEND init3 ${init2})
	endforeach(init)

	string(REPLACE "!" ";" init4 ${init3})
	set(HARDWARE_INIT ${init4})
	configure_file(libs/pilight/hardware_init.h.in libs/pilight/hardware_init.h)	

	configure_file(pilight.h.in pilight.h)
endif()

if(${MODULESPACK} MATCHES "ON")
	foreach(file ${protocol})
		get_filename_component(name ${file} NAME_WE)
		add_library(${name}_module MODULE ${file})
		target_link_libraries(${name}_module ${CMAKE_THREAD_LIBS_INIT})
		target_link_libraries(${name}_module m)
		target_link_libraries(${name}_module ${CMAKE_DL_LIBS})
		set_target_properties(${name}_module PROPERTIES OUTPUT_NAME ${name})
		string(REPLACE "_" "-" debname ${name})
		install(FILES lib${name}.so DESTINATION lib/pilight/protocols RENAME ${name}.so COMPONENT pilight-protocol-${debname})
	endforeach()

	foreach(file ${hardware})
		get_filename_component(name ${file} NAME_WE)
		add_library(${name}_module MODULE ${file})
		target_link_libraries(${name}_module ${CMAKE_THREAD_LIBS_INIT})
		target_link_libraries(${name}_module m)
		target_link_libraries(${name}_module ${CMAKE_DL_LIBS})
		set_target_properties(${name}_module PROPERTIES OUTPUT_NAME ${name})
		string(REPLACE "_" "-" debname ${name})
		install(FILES lib${name}.so DESTINATION lib/pilight/hardware RENAME ${name}.so COMPONENT pilight-hardware-${debname})
	endforeach()	
endif()

if(${MODULESPACK} MATCHES "OFF")
	add_library(pilight OBJECT ${pilight} ${protocol} ${hardware})

	add_library(pilight_static STATIC $<TARGET_OBJECTS:pilight>)
	add_library(pilight_shared SHARED $<TARGET_OBJECTS:pilight>)

	target_link_libraries(pilight_shared ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(pilight_shared m)
	target_link_libraries(pilight_shared rt)
	target_link_libraries(pilight_shared ${CMAKE_DL_LIBS})
	target_link_libraries(pilight_static ${CMAKE_THREAD_LIBS_INIT})
	target_link_libraries(pilight_static m)
	target_link_libraries(pilight_static rt)
	target_link_libraries(pilight_static ${CMAKE_DL_LIBS})

	if(${UPDATE} MATCHES "ON")
		target_link_libraries(pilight_shared ${CMAKE_ZLIB_LIBS_INIT})
		target_link_libraries(pilight_static ${CMAKE_ZLIB_LIBS_INIT})
	endif()

	set_target_properties(pilight_shared pilight_static PROPERTIES OUTPUT_NAME pilight)

	set_source_files_properties(${PROJECT_SOURCE_DIR}/libs/pilight/wiringPi.c PROPERTIES COMPILE_FLAGS " -w ")
	set_source_files_properties(${PROJECT_SOURCE_DIR}/libs/pilight/wiringPiI2C.c PROPERTIES COMPILE_FLAGS " -w ")
	set_source_files_properties(${PROJECT_SOURCE_DIR}/libs/pilight/json.c PROPERTIES COMPILE_FLAGS " -w ")
	set_source_files_properties(${PROJECT_SOURCE_DIR}/libs/pilight/http_lib.c PROPERTIES COMPILE_FLAGS " -w ")
	set_source_files_properties(${PROJECT_SOURCE_DIR}/libs/pilight/mongoose.c PROPERTIES COMPILE_FLAGS " -w ")
	set_source_files_properties(${avrdude_sources} PROPERTIES COMPILE_FLAGS " -w ")

	if(EXISTS "/usr/local/lib/libpilight.so")
		install(CODE "execute_process(COMMAND rm /usr/local/lib/libpilight.so)")
	endif()

	install(FILES libpilight.so DESTINATION lib/pilight/ RENAME libpilight.so.${PROJECT_VERSION} COMPONENT pilight)
	install(FILES libpilight.a DESTINATION lib/pilight/ RENAME libpilight.a COMPONENT pilight)

	if(NOT EXISTS "/usr/local/lib/libpilight.so")
		install(CODE "execute_process(COMMAND ln -s \"/usr/local/lib/pilight/libpilight.so.${PROJECT_VERSION}\" \"/usr/local/lib/libpilight.so\")")
	endif()		
	
	if(EXISTS "/etc/pilight/settings.json")
		install(CODE "execute_process(COMMAND mv /etc/pilight/settings.json /etc/pilight/settings.json.bak)")
	endif()

	if(EXISTS "/etc/pilight/hardware.json")
		install(CODE "execute_process(COMMAND mv /etc/pilight/hardware.json /etc/pilight/hardware.json.bak)")
	endif()

	if(${WEBSERVER} MATCHES "ON")
		install(DIRECTORY web/ DESTINATION /usr/local/share/pilight COMPONENT webgui)
	endif()

	install(PROGRAMS init/pilight.initd DESTINATION /etc/init.d/ RENAME pilight)
	install(FILES settings.json-default DESTINATION /etc/pilight/ RENAME settings.json COMPONENT pilight)
	install(FILES tzdata.json DESTINATION /etc/pilight/ COMPONENT pilight)
	install(FILES hardware.json DESTINATION /etc/pilight/ COMPONENT pilight)

	add_executable(pilight-daemon daemon.c)
	target_link_libraries(pilight-daemon pilight_shared)
	target_link_libraries(pilight-daemon ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-daemon m)
	target_link_libraries(pilight-daemon ${CMAKE_THREAD_LIBS_INIT})

	if(${UPDATE} MATCHES "ON")
		target_link_libraries(pilight-daemon ${CMAKE_ZLIB_LIBS_INIT})
	endif()

	add_executable(pilight-raw raw.c)
	target_link_libraries(pilight-raw pilight_shared)
	target_link_libraries(pilight-raw ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-raw m)
	target_link_libraries(pilight-raw ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-send send.c)
	target_link_libraries(pilight-send pilight_shared)
	target_link_libraries(pilight-send ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-send m)
	target_link_libraries(pilight-send ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-control control.c)
	target_link_libraries(pilight-control pilight_shared)
	target_link_libraries(pilight-control ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-control m)
	target_link_libraries(pilight-control ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-learn learn.c)
	target_link_libraries(pilight-learn pilight_shared)
	target_link_libraries(pilight-learn ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-learn m)
	target_link_libraries(pilight-learn ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-receive receive.c)
	target_link_libraries(pilight-receive pilight_shared)
	target_link_libraries(pilight-receive ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-receive m)
	target_link_libraries(pilight-receive ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-debug debug.c)
	target_link_libraries(pilight-debug pilight_shared)
	target_link_libraries(pilight-debug ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-debug m)
	target_link_libraries(pilight-debug ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-uuid uuid.c)
	target_link_libraries(pilight-uuid pilight_shared)
	target_link_libraries(pilight-uuid ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-uuid m)
	target_link_libraries(pilight-uuid ${CMAKE_THREAD_LIBS_INIT})

	add_executable(pilight-flash flash.c)
	target_link_libraries(pilight-flash pilight_shared)
	target_link_libraries(pilight-flash ${CMAKE_DL_LIBS})
	target_link_libraries(pilight-flash m)
	target_link_libraries(pilight-flash ${CMAKE_THREAD_LIBS_INIT})

	if(EXISTS "/usr/local/sbin/pilight-send")
		install(CODE "execute_process(COMMAND rm /usr/local/sbin/pilight-send)")
	endif()

	if(EXISTS "/usr/local/sbin/pilight-receive")
		install(CODE "execute_process(COMMAND rm /usr/local/sbin/pilight-receive)")
	endif()

	if(EXISTS "/usr/local/sbin/pilight-control")
		install(CODE "execute_process(COMMAND rm /usr/local/sbin/pilight-control)")
	endif()

	if(EXISTS "/usr/local/sbin/pilight-uuid")
		install(CODE "execute_process(COMMAND rm /usr/local/sbin/pilight-uuid)")
	endif()

	install(PROGRAMS pilight-daemon DESTINATION sbin COMPONENT pilight)
	install(PROGRAMS pilight-raw DESTINATION sbin COMPONENT pilight)
	install(PROGRAMS pilight-send DESTINATION bin COMPONENT pilight)
	install(PROGRAMS pilight-control DESTINATION bin COMPONENT pilight)
	install(PROGRAMS pilight-learn DESTINATION sbin COMPONENT pilight)
	install(PROGRAMS pilight-receive DESTINATION bin COMPONENT pilight)
	install(PROGRAMS pilight-debug DESTINATION sbin COMPONENT pilight)
	install(PROGRAMS pilight-flash DESTINATION sbin COMPONENT pilight)
	install(PROGRAMS pilight-uuid DESTINATION bin COMPONENT pilight)
	install(CODE "execute_process(COMMAND update-rc.d pilight defaults)")
	install(CODE "execute_process(COMMAND ldconfig)")
endif()

WRITE_UNINSTALL_TARGET_SCRIPT()
configure_file("${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/")
set(CPACK_SOURCE_STRIP_FILES TRUE)
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PACKAGE_CONTACT "CurlyMo <info@pilight.org>")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME "pilight")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "zlib1g-dev")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_PACKAGE_DESCRIPTION "Control RF devices with the Raspberry Pi")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Control RF devices with the Raspberry Pi")

include(CPack)
