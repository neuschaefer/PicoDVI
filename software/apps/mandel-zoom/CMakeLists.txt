# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(mandel-zoom
	main.c
	mandel.c
)

target_compile_options(mandel-zoom PRIVATE -Wall)

target_compile_definitions(mandel-zoom PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	)

target_link_libraries(mandel-zoom
	pico_stdlib
	pico_multicore
	pico_util
	pico_float
	pico_double
	libdvi
)

# create map/bin/hex file etc.
pico_add_extra_outputs(mandel-zoom)
