IF(WIN32)
		# copy DLL 
#		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Release/libcurl.dll"
#          DESTINATION "${PROJECT_BINARY_DIR}/Release"
#          CONFIGURATIONS Release
#        )
#		
#		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Release/libcurl.dll"
#          DESTINATION "${PROJECT_BINARY_DIR}/RelWithDebInfo"
#          CONFIGURATIONS RelWithDebInfo
#        )
#		
#		install(FILES "${CURL_INCLUDE_DIR}/../lib/DLL-Debug/libcurld.dll"
#          DESTINATION "${PROJECT_BINARY_DIR}/Debug"
#          CONFIGURATIONS Debug
#        )

	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CURL_INCLUDE_DIR}/../lib/DLL-Debug/libcurld.dll"
		"${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
	
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CURL_INCLUDE_DIR}/../lib/DLL-Release/libcurl.dll"
		"${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")
		
	ENDIF(WIN32)
