#Change PROJECT_NAME to the name of your project
PROJECT(BSplineScatteredDataTest)

#include some macros from another file...
INCLUDE(${CMAKE_SOURCE_DIR}/IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
    ITKIO
   )

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project
SET(CurrentExe
    "itkBSplineScatteredDataPointSetToImageFilterTest"
    )
ADD_EXECUTABLE(${CurrentExe} 
               ${CurrentExe})
TARGET_LINK_LIBRARIES(${CurrentExe}
                      ${Libraries})

ADD_TEST(Run1 ${CurrentExe} ${BSplineScatteredDataTest_SOURCE_DIR}/brain.nii ${BSplineScatteredDataTest_BINARY_DIR}/brain_out.nii)
ADD_TEST(CompareImage1 ImageCompare ${BSplineScatteredDataTest_SOURCE_DIR}/brain_smooth.nii
${BSplineScatteredDataTest_BINARY_DIR}/brain_out.nii)


