set(TARGET_NAME big)

set(PUBLIC_HEADER_FILES
  include/big.hpp)

set(SOURCE_FILES ${PUBLIC_HEADER_FILES} tmp.cpp)

# Define the library
add_library(${TARGET_NAME} STATIC
  ${SOURCE_FILES})

# Allow includes from include/
target_include_directories(${TARGET_NAME}
  PUBLIC include/)

set_target_properties(${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER_FILES}")

install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)
