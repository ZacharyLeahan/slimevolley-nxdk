IF(DATA_INSTALL_DIR)
	SET(DESTINATION ${DATA_INSTALL_DIR}/slimes)
ELSE(DATA_INSTALL_DIR)
	SET(DESTINATION ${DATA_DIR}/slimes)
ENDIF(DATA_INSTALL_DIR)


INSTALL(FILES
	slimeJD1.png
	slimeJD2.png
	slimeJD3.png
	slimeJD4.png
	slimeJD5.png
	slimeJD6.png
	slimeJD7.png
	
	slimeJG1.png
	slimeJG2.png
	slimeJG3.png
	slimeJG4.png
	slimeJG5.png
	slimeJG6.png
	slimeJG7.png
	
	DESTINATION ${DESTINATION}
)
