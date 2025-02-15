function(get_extra_files source_dir)
  set(EXTRA_FILES
          ${source_dir}/resources/Poppins-Medium.ttf
          ${source_dir}/resources/Poppins-Bold.ttf
          ${source_dir}/resources/gelly-logo.png
          ${CURL_INCLUDE_DIRS}/../bin/libcurl-x64.dll
          ${CURL_INCLUDE_DIRS}/../bin/curl-ca-bundle.crt
          PARENT_SCOPE)
endfunction()
