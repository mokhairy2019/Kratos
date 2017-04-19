//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                   April 2017 $
//   Revision:            $Revision:                      0.0 $
//
//

#if !defined(KRATOS_LINEAR_ELASTIC_AXISYMMETRIC_2D_LAW_H_INCLUDED)
#define  KRATOS_LINEAR_ELASTIC_AXISYMMETRIC_2D_LAW_H_INCLUDED

// System includes

// External includes 

// Project includes
#include "custom_laws/linear_elastic_laws/linear_elastic_3D_law.hpp"

namespace Kratos
{
  ///@addtogroup ConstitutiveModelsApplication
  ///@{

  ///@name Kratos Globals
  ///@{ 
  
  ///@} 
  ///@name Type Definitions
  ///@{ 
  
  ///@} 
  ///@name  Enum's
  ///@{
      
  ///@}
  ///@name  Functions 
  ///@{
      
  ///@}
  ///@name Kratos Classes
  ///@{
  
  /// Short class definition.
  /** Detail class definition.
  */
  class KRATOS_API(CONSTITUTIVE_MODELS_APPLICATION) LinearElasticAxisymmetric2DLaw : public LinearElastic3DLaw
    {
    public:
      ///@name Type Definitions
      ///@{
      
      /// Pointer definition of LinearElasticAxisymmetric2DLaw
      KRATOS_CLASS_POINTER_DEFINITION(LinearElasticAxisymmetric2DLaw);
  
      ///@}
      ///@name Life Cycle 
      ///@{ 
      
      /// Default constructor.
      LinearElasticAxisymmetric2DLaw() : LinearElastic3DLaw() {}

      /// Copy constructor.
      LinearElasticAxisymmetric2DLaw(const LinearElasticAxisymmetric2DLaw& rOther) : LinearElastic3DLaw(rOther) {}

      /// Clone.
      ConstitutiveLaw::Pointer Clone() const override
      {
	return (LinearElasticAxisymmetric2DLaw::Pointer(new LinearElasticAxisymmetric2DLaw(*this)));
      }
      
      /// Destructor.
      virtual ~LinearElasticAxisymmetric2DLaw() {}
      

      ///@}
      ///@name Operators 
      ///@{

      /// Law Dimension
      SizeType WorkingSpaceDimension() override { return 2; }

      /// Law Voigt Strain Size
      SizeType GetStrainSize() override { return 4; }

      /// Law Features
      void GetLawFeatures(Features& rFeatures) override
      {
	KRATOS_TRY
	  
    	//Set the type of law
	rFeatures.mOptions.Set( AXISYMMETRIC_LAW );
	rFeatures.mOptions.Set( INFINITESIMAL_STRAINS );
	rFeatures.mOptions.Set( ISOTROPIC );

	//Set strain measure required by the consitutive law
	rFeatures.mStrainMeasures.push_back(StrainMeasure_Infinitesimal);
	rFeatures.mStrainMeasures.push_back(StrainMeasure_Deformation_Gradient);

	//Set the strain size
	rFeatures.mStrainSize = GetStrainSize();

	//Set the spacedimension
	rFeatures.mSpaceDimension = WorkingSpaceDimension();
	
	KRATOS_CATCH(" ")
      }
      
      ///@}
      ///@name Operations
      ///@{
      
      
      ///@}
      ///@name Access
      ///@{ 
      
      
      ///@}
      ///@name Inquiry
      ///@{
      
      
      ///@}      
      ///@name Input and output
      ///@{

      /// Turn back information as a string.
      virtual std::string Info() const override
      {
	std::stringstream buffer;
        buffer << "LinearElasticAxisymmetric2DLaw" ;
        return buffer.str();
      }
      
      /// Print information about this object.
      virtual void PrintInfo(std::ostream& rOStream) const override {rOStream << "LinearElasticAxisymmetric2DLaw";}

      /// Print object's data.
      virtual void PrintData(std::ostream& rOStream) const override {}
      
            
      ///@}      
      ///@name Friends
      ///@{
      
            
      ///@}
      
    protected:
      ///@name Protected static Member Variables 
      ///@{ 
        
        
      ///@} 
      ///@name Protected member Variables 
      ///@{ 
        
        
      ///@} 
      ///@name Protected Operators
      ///@{ 


      /**
       * calculates the linear elastic constitutive matrix in terms of Young's modulus and
       * Poisson ratio
       * @param E the Young's modulus
       * @param NU the Poisson ratio
       * @return the linear elastic constitutive matrix
       */
      void CalculateLinearElasticMatrix(Matrix& rConstitutiveMatrix,
					const Properties& rMaterialProperties) override
      {
	KRATOS_TRY

	// Lame constants
	const double& rYoungModulus          = rMaterialProperties[YOUNG_MODULUS];
	const double& rPoissonCoefficient    = rMaterialProperties[POISSON_RATIO];
	  
	rConstitutiveMatrix ( 0 , 0 ) = (rYoungModulus*(1.0-rPoissonCoefficient)/((1.0+rPoissonCoefficient)*(1.0-2*rPoissonCoefficient)));
	rConstitutiveMatrix ( 1 , 1 ) = rConstitutiveMatrix ( 0 , 0 );
	rConstitutiveMatrix ( 2 , 2 ) = rConstitutiveMatrix ( 0 , 0 );
	
	rConstitutiveMatrix ( 3 , 3 ) = rConstitutiveMatrix ( 0 , 0 )*(1-2*rPoissonCoefficient)/(2*(1.0-rPoissonCoefficient));
	
	rConstitutiveMatrix ( 0 , 1 ) = rConstitutiveMatrix ( 0 , 0 )*rPoissonCoefficient/(1.0-rPoissonCoefficient);
	rConstitutiveMatrix ( 1 , 0 ) = rConstitutiveMatrix ( 0 , 1 );
	
	rConstitutiveMatrix ( 0 , 2 ) = rConstitutiveMatrix ( 0 , 1 );
	
	rConstitutiveMatrix ( 2 , 0 ) = rConstitutiveMatrix ( 0 , 1 );
	
	rConstitutiveMatrix ( 1 , 2 ) = rConstitutiveMatrix ( 0 , 1 );
	
	rConstitutiveMatrix ( 2 , 1 ) = rConstitutiveMatrix ( 0 , 1 );
    
	KRATOS_CATCH(" ")
      }    
      
        
      ///@} 
      ///@name Protected Operations
      ///@{ 
        
        
      ///@} 
      ///@name Protected  Access 
      ///@{ 
        
        
      ///@}      
      ///@name Protected Inquiry 
      ///@{ 
        
        
      ///@}    
      ///@name Protected LifeCycle 
      ///@{ 
      
            
      ///@}
      
    private:
      ///@name Static Member Variables 
      ///@{ 
        
        
      ///@} 
      ///@name Member Variables 
      ///@{ 
        
        
      ///@} 
      ///@name Private Operators
      ///@{ 
        
        
      ///@} 
      ///@name Private Operations
      ///@{ 
        
        
      ///@} 
      ///@name Private  Access 
      ///@{ 
        
        
      ///@}    
      ///@name Private Inquiry 
      ///@{ 
        

      ///@}
      ///@name Serialization
      ///@{
      friend class Serializer;

      virtual void save(Serializer& rSerializer) const override
      {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, LinearElastic3DLaw )
      }
      
      virtual void load(Serializer& rSerializer) override
      {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, LinearElastic3DLaw )
      }

      
      ///@}    
      ///@name Un accessible methods 
      ///@{ 
      
      /// Assignment operator.
      LinearElasticAxisymmetric2DLaw& operator=(LinearElasticAxisymmetric2DLaw const& rOther){ return *this; }

        
      ///@}    
        
    }; // Class LinearElasticAxisymmetric2DLaw 

  ///@} 
  
  ///@name Type Definitions       
  ///@{ 
  
  
  ///@} 
  ///@name Input and output 
  ///@{ 
        

  ///@}

  ///@} addtogroup block
  
}  // namespace Kratos.

#endif // KRATOS_LINEAR_ELASTIC_AXISYMMETRIC_2D_LAW_H_INCLUDED  defined
