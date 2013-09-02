IF(WIN32)
		# copy DLL 
		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Release/libcurl.dll"
          DESTINATION "${PROJECT_BINARY_DIR}/Release"
          CONFIGURATIONS Release
        )
		
		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Release/libcurl.dll"
          DESTINATION "${PROJECT_BINARY_DIR}/RelWithDebInfo"
          CONFIGURATIONS RelWithDebInfo
        )
		
		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Debug/libcurld.dll"
          DESTINATION "${PROJECT_BINARY_DIR}/Debug"
          CONFIGURATIONS Debug
        )
		
	ENDIF(WIN32)