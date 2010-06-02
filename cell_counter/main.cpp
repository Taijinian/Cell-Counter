#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "vtkSmartPointer.h"
#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "vtkTIFFReader.h"
#include "QVTKWidget.h"
#include "itkTIFFImageIO.h"
#include "itkRGBPixel.h"

#include "vtkImageExport.h"
#include "vtkImageImport.h"
#include "itkVTKImageImport.h"

#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

typedef itk::RGBPixel<unsigned int> PixelType;
const unsigned int ImageDimension = 2;
typedef itk::Image< PixelType, ImageDimension > ImageType;

template <typename VTK_Exporter, typename ITK_Importer>
void ConnectVTK2ITKPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

void addMenu(QMainWindow& mainWindow)
{
    QMenu* fileMenu = mainWindow.menuBar()->addMenu("&File");
    //fileMenu.addAction("&Open Images", fileOpener, SLOT(openImages()));
}

itk::ImageFileReader<ImageType>::Pointer readImage(std::string const& fileName)
{
  itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
  reader->SetFileName(fileName.c_str());
  //TODO: assume TIFF for now. Can change to arbitrary file extensions later.
  itk::TIFFImageIO::Pointer tiffImageIO = itk::TIFFImageIO::New();
  reader->SetImageIO(tiffImageIO);

  reader->Update();
  return reader;
}

vtkImageAlgorithm* convertITKImageToVTK(ImageType& itkImage)
{
  itk::VTKImageExport<ImageType>::Pointer itkImageExport = itk::VTKImageExport<ImageType>::New();
  itkImageExport->SetInput(&itkImage);
  
  //itk::VTKImageImport<ImageType>::Pointer itkImageImport = itk::VTKImageImport<ImageType>::New();
  vtkImageImport* vtkImageImport = vtkImageImport::New();
  ConnectPipelines(itkImageExport, vtkImageImport);

  return vtkImageImport;
}

void displayImage(vtkImageAlgorithm& image,
                  QVTKWidget&        vtkRenderWidget)
{
  vtkImageViewer* image_view = vtkImageViewer::New();
  image_view->SetInputConnection(image.GetOutputPort());

  vtkRenderWidget.SetRenderWindow(image_view->GetRenderWindow());
  image_view->SetupInteractor(vtkRenderWidget.GetRenderWindow()->GetInteractor());

  image_view->SetColorLevel(138.5);
  image_view->SetColorWindow(233);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  //create render window
  QMainWindow mainWindow;
  mainWindow.resize(1024, 768);
  addMenu(mainWindow);

  QVTKWidget vtkWidget;
  mainWindow.setCentralWidget(&vtkWidget);

  //read image
  itk::ImageFileReader<ImageType>::Pointer itkImage = readImage("c:\\projects\\mjc-20100424-0011.tif");
  
  //convert itk image to vtk image
  vtkImageAlgorithm* vtkImage = convertITKImageToVTK(*itkImage->GetOutput());

  //display image
  displayImage(*vtkImage, vtkWidget);
  
  mainWindow.show();

  app.exec();

  return 0;
}

