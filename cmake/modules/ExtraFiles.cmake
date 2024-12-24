function(get_extra_files source_dir)
  set(EXTRA_FILES
          ${source_dir}/vendor/imgui/misc/fonts/Roboto-Medium.ttf
          ${CURL_INCLUDE_DIRS}/../bin/libcurl-x64.dll
          ${CURL_INCLUDE_DIRS}/../bin/curl-ca-bundle.crt
          PARENT_SCOPE)
endfunction()
