#ifndef ItkToVtkImageExporter_h__
#define ItkToVtkImageExporter_h__

#include "vtkImageExport.h"
#include "vtkImageImport.h"
#include "itkVTKImageImport.h"
#include "itkVTKImageExport.h"
#include "vtkSmartPointer.h"


template <class ItkImageType>
class ItkToVtkImageExporter
{
public:
  ItkToVtkImageExporter(ItkImageType& itkImage)
  {
    m_itkImageExporter = itk::VTKImageExport<ItkImageType>::New();
    m_itkImageExporter->SetInput(&itkImage);
    
    //itk::VTKImageImport<ImageType>::Pointer itkImageImport = itk::VTKImageImport<ImageType>::New();
    m_vtkImageImporter = vtkSmartPointer<vtkImageImport>::New();
    ConnectPipelines(m_itkImageExporter, m_vtkImageImporter.GetPointer());
  }

  virtual ~ItkToVtkImageExporter()
  {
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

  vtkImageAlgorithm& vtkImage()
  {
    return *m_vtkImageImporter;
  }
private:
  vtkSmartPointer<vtkImageImport> m_vtkImageImporter;
  typename itk::VTKImageExport<ItkImageType>::Pointer m_itkImageExporter;
};

#endif //ItkToVtkImageExporter_h__
