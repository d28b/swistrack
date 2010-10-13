cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#rosbuild_invoke_rospack(swistrack)
#set(wxWidgets_USE_LIBS base core )

find_package(wxWidgets REQUIRED)

if(NOT wxWidgets_FOUND)
message(FATAL_ERROR "wxWidgets not found!  Aborting...")
endif(NOT wxWidgets_FOUND)

include(${wxWidgets_USE_FILE})
include_directories(${wxWidgets_INCLUDE_DIRS})
include_directories(3rdParty/libtsai/include 3rdParty/camshift_wrapper 3rdParty/cvblobslib_v6p1 Code/gui Code/core)
rosbuild_find_ros_package(wxswig)
set(WXSWIG_EXECUTABLE ${wxswig_PACKAGE_PATH}/bin/swig)
# Deal with CMake 2.4 vs. 2.6
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.4)   # CMake 2.6
  set(SWIG_OUTPUT_CPP_FILE ${PROJECT_SOURCE_DIR}/src/rxtools/rosout_panel_swig_generated.cpp)
  set(SWIG_COMMAND ${WXSWIG_EXECUTABLE} ${SWIG_FLAGS} -o ${SWIG_OUTPUT_CPP_FILE} -outdir ../lib
-module ${PROJECT_NAME} ${SWIG_INTERFACE_FILE})
else("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.4)    # CMake 2.4
  set(SWIG_OUTPUT_CPP_FILE src/rxtools/rosout_panel_swig_generated.cpp)
  set(SWIG_COMMAND ${WXSWIG_EXECUTABLE} ${SWIG_FLAGS} -o ../${SWIG_OUTPUT_CPP_FILE} -outdir ../lib
-module ${PROJECT_NAME} ${SWIG_INTERFACE_FILE})
endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.4)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

rosbuild_add_library(libtsai 3rdParty/libtsai/tsaisrc/ecalmain.c 3rdParty/libtsai/tsaisrc/cal_eval.c 3rdParty/libtsai/tsaisrc/cal_main.c 3rdParty/libtsai/tsaisrc/cal_tran.c 3rdParty/libtsai/tsaisrc/cal_util.c 3rdParty/libtsai/mathsrc/matrix.c 3rdParty/libtsai/mathsrc/minpack/dpmpar.c 3rdParty/libtsai/mathsrc/minpack/enorm.c 3rdParty/libtsai/mathsrc/minpack/fdjac2.c 3rdParty/libtsai/mathsrc/minpack/lmdif.c 3rdParty/libtsai/mathsrc/minpack/lmpar.c 3rdParty/libtsai/mathsrc/minpack/qrfac.c 3rdParty/libtsai/mathsrc/minpack/qrsolv.c)
rosbuild_add_library(camshift_wrapper 3rdParty/camshift_wrapper/camshift_wrapper.c 3rdParty/camshift_wrapper/capture.c 3rdParty/camshift_wrapper/facedet.c 3rdParty/camshift_wrapper/TrackFaces.c)
rosbuild_add_library(cvblobslib_v6p1 3rdParty/cvblobslib_v6p1/Blob.cpp 3rdParty/cvblobslib_v6p1/BlobExtraction.cpp 3rdParty/cvblobslib_v6p1/BlobResult.cpp)
rosbuild_add_library(gui Code/gui/AboutDialog.cpp Code/gui/ConfigurationParameterDropdownList.cpp Code/gui/Application.cpp Code/gui/ConfigurationParameterFactory.cpp Code/gui/CanvasAnnotation.cpp Code/gui/ConfigurationParameterGigECameraList.cpp Code/gui/Canvas.cpp Code/gui/ConfigurationParameterInputFile.cpp Code/gui/CanvasPanel.cpp Code/gui/ConfigurationParameterInteger.cpp Code/gui/CanvasTitle.cpp Code/gui/ConfigurationParameterOutputFile.cpp Code/gui/ComponentListPanel.cpp Code/gui/ConfigurationParameterPointDouble.cpp Code/gui/ComponentsDialog.cpp Code/gui/ConfigurationParameterPointInteger.cpp Code/gui/ComponentTreeItem.cpp Code/gui/ConfigurationParameterString.cpp Code/gui/ConfigurationPanel.cpp Code/gui/ConfigurationParameterVideoFormat.cpp Code/gui/ConfigurationParameterAngle.cpp Code/gui/ErrorListDialog.cpp Code/gui/ConfigurationParameterButton.cpp Code/gui/NMEALogFile.cpp Code/gui/ConfigurationParameterCheckBox.cpp Code/gui/SwisTrack.cpp Code/gui/ConfigurationParameterColor.cpp Code/gui/TCPServerConnection.cpp Code/gui/ConfigurationParameter.cpp Code/gui/TCPServer.cpp Code/gui/ConfigurationParameterDate.cpp Code/gui/TCPServerDialog.cpp Code/gui/ConfigurationParameterDouble.cpp Code/gui/TimelinePanel.cpp )
rosbuild_add_executable(SwisTrack Code/core/CommunicationCommandHandler.cpp Code/core/CommunicationInterface.cpp Code/core/CommunicationMessage.cpp Code/core/CommunicationNMEAInterface.cpp Code/core/ComponentAdaptiveBackgroundSubtractionColor.cpp Code/core/ComponentAdaptiveBackgroundSubtractionGray.cpp Code/core/ComponentAdaptiveBackgroundSubtractionMedian.cpp Code/core/ComponentAdaptiveBackgroundSubtractionMode.cpp Code/core/ComponentAdaptiveThreshold.cpp Code/core/ComponentBackgroundSubtractionCheungKamath.cpp Code/core/ComponentBackgroundSubtractionColor.cpp Code/core/ComponentBackgroundSubtractionGray.cpp Code/core/ComponentBinaryDilation.cpp Code/core/ComponentBinaryErosion.cpp Code/core/ComponentBinaryMask.cpp Code/core/ComponentBinarySmooth.cpp Code/core/ComponentBlobDetectionCircularHough.cpp Code/core/ComponentBlobDetectionMinMax.cpp Code/core/ComponentBlobDetectionRedGreen.cpp Code/core/ComponentBlobDetectionTwoColors.cpp Code/core/ComponentBlobSelection.cpp Code/core/ComponentCalibrationFileTSAI.cpp Code/core/ComponentCalibrationLinear.cpp Code/core/ComponentCalibrationOpenCV.cpp Code/core/ComponentCalibrationTSAI.cpp Code/core/ComponentCamShiftTracking.cpp Code/core/ComponentCannyEdgeDetection.cpp Code/core/ComponentCategory.cpp Code/core/ComponentChannelArithmetic.cpp Code/core/ComponentClassifierTracker.cpp Code/core/ComponentColorBlur.cpp Code/core/ComponentColorHistogramParticles.cpp Code/core/ComponentColorMask.cpp Code/core/ComponentColorSwapper.cpp Code/core/ComponentConvertBayerToColor.cpp Code/core/ComponentConvertToColor.cpp Code/core/ComponentConvertToGray.cpp Code/core/Component.cpp Code/core/ComponentDoubleThresholdColorIndependent.cpp Code/core/ComponentDynamicNearestNeighborTracking.cpp Code/core/ComponentEditor.cpp Code/core/ComponentFilterParticles.cpp Code/core/ComponentFloorDetection.cpp Code/core/ComponentGrayMask.cpp Code/core/ComponentHSVBackgroundSubtractionColor.cpp Code/core/ComponentIDReaderRing.cpp Code/core/ComponentInputCamera1394.cpp Code/core/ComponentInputCameraGigE.cpp Code/core/ComponentInputCameraProselicaGigE.cpp Code/core/ComponentInputCameraUSB.cpp Code/core/ComponentInputFileAVI.cpp Code/core/ComponentInputFileImage.cpp Code/core/ComponentInvertBinary.cpp Code/core/ComponentKalmanFilterTrack.cpp Code/core/ComponentMinCostFlowTracking.cpp Code/core/ComponentMinFlowTracking.cpp Code/core/ComponentMorphology.cpp Code/core/ComponentMotionTemplateParticleDetector.cpp Code/core/ComponentMoveBinaryToColor.cpp Code/core/ComponentMoveColorToBinary.cpp Code/core/ComponentMoveColorToGray.cpp Code/core/ComponentMoveGrayToColor.cpp Code/core/ComponentNearestNeighborTracking.cpp Code/core/ComponentOutputDataAssociationTraining.cpp Code/core/ComponentOutputFileAVI.cpp Code/core/ComponentOutputFile.cpp Code/core/ComponentOutputFileM4V.cpp Code/core/ComponentOutputFramesImages.cpp Code/core/ComponentOutputImageOverlayColor.cpp Code/core/ComponentOutputImageStatisticsBinary.cpp Code/core/ComponentOutputImageStatisticsColor.cpp Code/core/ComponentOutputImageStatisticsGray.cpp Code/core/ComponentOutputMarkFrameManual.cpp Code/core/ComponentOutputParticles.cpp Code/core/ComponentOutputSQLite.cpp Code/core/ComponentRosTest.cpp Code/core/ComponentSimulationParticles.cpp Code/core/ComponentSobelDifferentiation.cpp Code/core/ComponentSpecificColorSubtraction.cpp Code/core/ComponentThresholdColorCommon.cpp Code/core/ComponentThresholdColorIndependent.cpp Code/core/ComponentThresholdGray.cpp Code/core/ComponentTracking.cpp Code/core/ComponentTrackSmoothing.cpp Code/core/ComponentTriggerCounter.cpp Code/core/ComponentTrigger.cpp Code/core/ComponentTriggerTimer.cpp Code/core/ConfigurationConversion.cpp Code/core/ConfigurationReaderXML.cpp Code/core/ConfigurationWriterXML.cpp Code/core/ConfigurationXML.cpp Code/core/DataAssociationClassifier.cpp Code/core/DataStructure.cpp Code/core/DataStructureImage.cpp Code/core/DataStructureInput.cpp Code/core/DataStructureParticles.cpp Code/core/DataStructureTracks.cpp Code/core/Display.cpp Code/core/DisplayEditor.cpp Code/core/DisplayRenderer.cpp Code/core/DisplaySubscriberInterface.cpp Code/core/ErrorList.cpp Code/core/ErrorListItem.cpp Code/core/ImageConversion.cpp Code/core/MinCostFlow.cpp Code/core/NMEALog.cpp Code/core/ObjectList.cpp Code/core/Particle.cpp Code/core/Random.cpp Code/core/RandomExponential.cpp Code/core/RandomNormal.cpp Code/core/RandomPoisson.cpp Code/core/SimulationParticles.cpp Code/core/StatusItem.cpp Code/core/SwisTrackCore.cpp Code/core/SwisTrackCoreEditor.cpp Code/core/SwisTrackCoreEventRecorder.cpp Code/core/SwisTrackCoreInterface.cpp Code/core/SwisTrackCoreTrigger.cpp Code/core/Track.cpp Code/core/Utility.cpp Code/core/WriteAvi.cpp)
target_link_libraries(SwisTrack ${wxWidgets_LIBRARIES} libtsai camshift_wrapper cvblobslib_v6p1 gui)
