project(libssc)

set(RAM_PROJECT "ram-operations")
set(RAM_LIB ram)

add_library(${RAM_LIB} STATIC ${RAM_PROJECT}/${RAM_PROJECT}.c)

set(libssc ${RAM_LIB})
