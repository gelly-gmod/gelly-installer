function(get_extra_files source_dir)
  set(EXTRA_FILES
          ${source_dir}/fonts/Poppins-Medium.ttf
          ${source_dir}/fonts/Poppins-Bold.ttf
          ${CURL_INCLUDE_DIRS}/../bin/libcurl-x64.dll
          ${CURL_INCLUDE_DIRS}/../bin/curl-ca-bundle.crt
          PARENT_SCOPE)
endfunction()
