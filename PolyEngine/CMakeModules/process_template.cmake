if (NOT WIN32)
	configure_file("${INPUT_FILE}" "${OUTPUT_FILE}" @ONLY)
endif()
