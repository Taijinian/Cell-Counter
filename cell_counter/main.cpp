#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "ItkToVtkImageExporter.h"
#include "itkAdaptiveOtsuThresholdImageFilter.h"

#include "vtkSmartPointer.h"
#include "QVTKWidget.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "itkTIFFImageIO.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"


//typedef itk::RGBPixel<unsigned char> PixelType;
typedef unsigned char PixelType;
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

void displayImage(vtkImageAlgorithm& image,
                  QVTKWidget&        renderWidget)
{
#if 0
  vtkImageViewer2* image_view = vtkImageViewer2::New();
  image_view->SetInputConnection(image.GetOutputPort());

  renderWidget.SetRenderWindow(image_view->GetRenderWindow());
  image_view->SetupInteractor(renderWidget.GetRenderWindow()->GetInteractor());

  image_view->SetColorLevel(138.5);
  image_view->SetColorWindow(233);
#else
  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
  imageActor->SetInput(image.GetOutput());
  vtkSmartPointer<vtkRenderer> imageRenderer = vtkSmartPointer<vtkRenderer>::New();
  imageRenderer->SetBackground(0.1, 0.1, 0.2);
  imageRenderer->AddActor(imageActor);
  renderWidget.GetRenderWindow()->AddRenderer(imageRenderer);
#endif
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
  itk::ImageFileReader<ImageType>::Pointer itkImage = readImage("c:\\projects\\cell_counter\\data\\mjc-20100424-0011.tif");
  
  //convert itk image to vtk image
  ItkToVtkImageExporter<ImageType> itkToVtkImageExporter(*itkImage->GetOutput());

  //display original image
  displayImage(itkToVtkImageExporter.vtkImage(), vtkWidget);

  //
  ImageType::SpacingType spacing = itkImage->GetOutput()->GetSpacing();
  ImageType::SizeType size = itkImage->GetOutput()->GetLargestPossibleRegion().GetSize();

  const unsigned int radius = 20;
  ImageType::SizeType m_radius;
  for( unsigned int i = 0; i < ImageDimension; i++ )
    m_radius[i] = static_cast<ImageType::SizeType::SizeValueType>( radius/spacing[i] );

  ImageType::Pointer output;
  itk::AdaptiveOtsuThresholdImageFilter<ImageType, ImageType>::Pointer thresholdFilter = 
                        itk::AdaptiveOtsuThresholdImageFilter<ImageType, ImageType>::New();
  thresholdFilter->SetInput(itkImage->GetOutput());
  thresholdFilter->SetInsideValue(255);
  //unsigned char outsideValue[]= {0,0,0};
  //thresholdFilter->SetOutsideValue(itk::RGBPixel<unsigned char>(outsideValue));
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->SetNumberOfHistogramBins(256);
  thresholdFilter->SetNumberOfControlPoints(5000);
  thresholdFilter->SetNumberOfLevels(3);
  thresholdFilter->SetNumberOfSamples(50);
  thresholdFilter->SetRadius(m_radius);
  thresholdFilter->Update();
  //convert itk image to vtk image
  ItkToVtkImageExporter<ImageType> thresholdItkToVtkImageExporter(*thresholdFilter->GetOutput());
  //display threshold image
  displayImage(thresholdItkToVtkImageExporter.vtkImage(), vtkWidget);

  //
  mainWindow.show();

  app.exec();

  return 0;
}

