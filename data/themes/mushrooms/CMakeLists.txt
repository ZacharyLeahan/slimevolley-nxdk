SET(THEME mushrooms)
IF(DATA_INSTALL_DIR)
	SET(DESTINATION ${DATA_INSTALL_DIR}/themes/${THEME})
ELSE(DATA_INSTALL_DIR)
	SET(DESTINATION ${DATA_DIR}/themes/${THEME})
ENDIF(DATA_INSTALL_DIR)

INSTALL(FILES
	police.ttf

	slimeIAG.png
	slimeIAD.png

	slimeG.png
	slimeD.png

	oeil.png

	menu.png
	jeu.png

	fleche.png
	balle.png
	
	theme.txt
	
	s_slime.wav
	s_mur.wav
	s_jg_m.wav
	s_filet.wav

	DESTINATION ${DESTINATION}
)
