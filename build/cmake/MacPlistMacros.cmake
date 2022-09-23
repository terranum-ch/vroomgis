# Mac Plist Macros

FUNCTION(GET_VERSION_PLIST PLISTFILE OUTVAR)
    set(PVERSION "")
    IF (EXISTS ${PLISTFILE})
        file(READ "${PLISTFILE}" info_plist)
        STRING(REGEX REPLACE "\n" "" info_plist "${info_plist}")
        STRING(REGEX MATCH "<key>CFBundleShortVersionString</key>[ \t]*<string>([0-9a-z\\.]*)</string>" PLISTVERSION "${info_plist}")
        STRING(REGEX REPLACE "<key>CFBundleShortVersionString</key>[ \t]*<string>([0-9a-z\\.]*)</string>" "\\1" PVERSION "${PLISTVERSION}")
    ENDIF (EXISTS ${PLISTFILE})
    set(${OUTVAR} ${PVERSION} PARENT_SCOPE)
ENDFUNCTION(GET_VERSION_PLIST)
