set (FOO_SRCS
	 MainInterface.cpp

)
# Generate the executable application for the current poject
ADD_EXECUTABLE (MainInterface 
				${FOO_SRCS}
#				${FOO_MOC_H}
)

# Add the link-libraries for the current poject
TARGET_LINK_LIBRARIES (MainInterface 
					   ${QT_LIBRARIES}
					   ${OpenCV_LIBS}
					   vacReceiver
)
SET_TARGET_PROPERTIES(MainInterface PROPERTIES FOLDER "Apps")
# The installation path for the application
INSTALL_TARGETS (/bin MainInterface )

 