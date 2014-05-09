# Libraries
set(UTPTAM_LIBS
	${GSTREAMER_LIBRARIES}
	gstapp-0.10
	GVars3
	cvd
	ARgsub
	AR 
	GLU 
	GL
	lapack
	blas
	opencv_core
	opencv_imgproc
	opencv_highgui
	opencv_ml
	opencv_video
	opencv_features2d
	opencv_calib3d
	opencv_objdetect
	opencv_contrib
	opencv_legacy
	opencv_flann
)

#
# UTPTAM
#

set(AGAST_SRC
    agast/nonMaximumSuppression.cc
    agast/agast5_8_nms.cc
    agast/agast5_8.cc
    agast/agast7_12d_nms.cc
    agast/agast7_12d.cc
    agast/agast7_12s_nms.cc
    agast/agast7_12s.cc
    agast/oast9_16_nms.cc
    agast/oast9_16.cc
)

set(UTPTAM_SRC
	${AGAST_SRC}
	ARToolkit.cc
	ATANCamera.cc
	Bundle.cc
	FeatureGrid.cc
	FrameGrabber.cc
	Frontend.cc
	FrontendRenderer.cc
	GLWindow2.cc
	GLWindowMenu.cc
	HomographyInit.cc
	InitialTracker.cc
	KeyFrame.cc
	main.cc
	Map.cc
	MapLockManager.cc
	MapMaker.cc
	MapPoint.cc
	MapSerializer.cc
	MapViewer.cc
	MD5.cc
	MD5Wrapper.cc
	MiniPatch.cc
	PatchFinder.cc
	PerformanceMonitor.cc
	TargetController.cc
	Rectangle.cc
	Relocaliser.cc
	ScaleMarkerTracker.cc
	ShiTomasi.cc
	SmallBlurryImage.cc
	StereoPlaneFinder.cc
	System.cc
	Swarm.cc
	Timing.cc
	tinyxml.cc
	tinyxmlerror.cc
	tinyxmlparser.cc
	Tracker.cc
	Utils.cc
	MathUtils.cc
	MKProtocol.cc
	MKConnection.cc
	MikroKopter.cc
	rs232.c
	VideoSource.cc
	${VIDEO_SOURCE_SRC}
)

add_executable(UTPTAM ${UTPTAM_SRC})
target_link_libraries(UTPTAM ${UTPTAM_LIBS})
target_link_libraries(UTPTAM flycapture)

