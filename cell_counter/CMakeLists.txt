CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Cell_Counter)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )

FIND_PACKAGE(VTK REQUIRED)
IF( VTK_FOUND )
  INCLUDE (${USE_VTK_FILE})
ENDIF( VTK_FOUND )

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

set (MOC_HEADERS
)
qt4_wrap_cpp (MOC_OUTFILES ${MOC_HEADERS})

set (UI_FILES
)
qt4_wrap_ui (UIC_OUTFILES ${UI_FILES})

#IF(Qt4_FOUND)
#  INCLUDE($(QT_USE_FILE))
#
#  set(QT_UI_SRCS )
#  QT4_WRAP_UI(QTUI_H_SRCS ${QT_UI_SRCS})
#
#  set(QT_MOC_HEADERS )
#  QT4_WRAP_CPP(QT_MOC_SRCS ${QT_MOC_HEADERS})
#ELSE()
#  MESSAGE(FATAL_ERROR "didn't find Qt4")
#ENDIF(Qt4_FOUND)

SET(executable_target cell_counter)

SET (SRC
	 main.cpp)
	 
INCLUDE_DIRECTORIES(
	    bspline_scattered_data_approximation/Source
	    AdaptiveOtsuThresholdImageFilter/code
	)
	
ADD_EXECUTABLE( ${executable_target} 
		${SRC}
		${MOC_OUTFILES}
		${UIC_OUTFILES}
		)

TARGET_LINK_LIBRARIES( ${executable_target}
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkWidgets vtkRendering vtkGraphics vtkHybrid 
    vtkImaging vtkIO vtkFiltering vtkCommon
    ${QT_LIBRARIES}
    QVTK)

