message(STATUS "Creating release archive")
include(ExtraFiles)
get_extra_files(${SOURCE_DIR})

set(ARCHIVE_NAME "${CMAKE_CURRENT_BINARY_DIR}/gelly-installer.zip")
set(ARCHIVE_FILES "")
list(APPEND ARCHIVE_FILES "gelly_installer.exe")
foreach (file ${EXTRA_FILES})
  get_filename_component(file_name ${file} NAME)
  list(APPEND ARCHIVE_FILES "${file_name}")
endforeach ()

message(STATUS "Creating archive ${ARCHIVE_NAME} with files ${ARCHIVE_FILES}")
file(ARCHIVE_CREATE
        OUTPUT ${ARCHIVE_NAME}
        PATHS ${ARCHIVE_FILES}
        FORMAT zip
        VERBOSE
)