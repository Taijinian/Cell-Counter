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


const unsigned int ImageDimension = 2;
typedef itk::RGBPixel<unsigned char> OriginalPixelType;
typedef itk::Image< OriginalPixelType, ImageDimension > OriginalImageType;

typedef unsigned char BinaryPixelType;
typedef itk::Image< BinaryPixelType, ImageDimension > BinaryImageType;

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

template <typename ImageType>
typename itk::ImageFileReader<ImageType>::Pointer readImage(std::string const& fileName)
{
  itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
  reader->SetFileName(fileName.c_str());
  //TODO: assume TIFF for now. Can change to arbitrary file extensions later.
  itk::TIFFImageIO::Pointer tiffImageIO = itk::TIFFImageIO::New();
  reader->SetImageIO(tiffImageIO);

  reader->Update();
  return reader;
}


void displayImage(vtkImageAlgorithm&           image,
                  vtkSmartPointer<vtkRenderer> imageRenderer,
                  double                       opacity = 1.0)
{

  vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
  imageActor->SetOpacity(opacity);
  imageActor->SetInput(image.GetOutput());
  static double zCoord = 0;
  imageActor->SetPosition(1, 1, zCoord);
  zCoord += 20;
  imageRenderer->AddActor(imageActor);
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
  vtkSmartPointer<vtkRenderer> imageRenderer = vtkSmartPointer<vtkRenderer>::New();
  vtkWidget.GetRenderWindow()->AddRenderer(imageRenderer);
  imageRenderer->SetBackground(0.1, 0.1, 0.2);

  //read image
  itk::ImageFileReader<OriginalImageType>::Pointer originalItkImage = 
        readImage<OriginalImageType>("c:\\projects\\cell_counter\\data\\mjc-20100424-0011.tif");
  
  //convert itk image to vtk image
  ItkToVtkImageExporter<OriginalImageType> originalImageItkToVtkExporter(*originalItkImage->GetOutput());

  //display original image
  displayImage(originalImageItkToVtkExporter.vtkImage(), imageRenderer);

  //create a binary threshold image
  OriginalImageType::SpacingType spacing = originalItkImage->GetOutput()->GetSpacing();
  OriginalImageType::SizeType size = originalItkImage->GetOutput()->GetLargestPossibleRegion().GetSize();

  const unsigned int radius = 20;
  OriginalImageType::SizeType m_radius;
  for( unsigned int i = 0; i < ImageDimension; i++ )
    m_radius[i] = static_cast<OriginalImageType::SizeType::SizeValueType>( radius/spacing[i] );

  OriginalImageType::Pointer output;
  itk::AdaptiveOtsuThresholdImageFilter<OriginalImageType, BinaryImageType>::Pointer thresholdFilter = 
                        itk::AdaptiveOtsuThresholdImageFilter<OriginalImageType, BinaryImageType>::New();
  thresholdFilter->SetInput(originalItkImage->GetOutput());
  thresholdFilter->SetInsideValue(255);
  //unsigned char outsideValue[]= {0,0,0};
  //thresholdFilter->SetOutsideValue(itk::RGBPixel<unsigned char>(outsideValue));
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->SetNumberOfHistogramBins(256);
  thresholdFilter->SetNumberOfControlPoints(10);
  thresholdFilter->SetNumberOfLevels(3);
  thresholdFilter->SetNumberOfSamples(50);
  thresholdFilter->SetRadius(m_radius);
  thresholdFilter->Update();
  //convert itk image to vtk image
  ItkToVtkImageExporter<BinaryImageType> thresholdItkToVtkImageExporter(*thresholdFilter->GetOutput());
  //display threshold image
  displayImage(thresholdItkToVtkImageExporter.vtkImage(), imageRenderer, 0.3);

  //
  mainWindow.show();

  app.exec();

  return 0;
}

