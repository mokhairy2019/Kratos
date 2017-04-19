//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                   April 2017 $
//   Revision:            $Revision:                      0.0 $
//
//

#if !defined(KRATOS_HYPERELASTIC_MODEL_H_INCLUDED )
#define  KRATOS_HYPERELASTIC_MODEL_H_INCLUDED

// System includes
#include <string>
#include <iostream>

// External includes

// Project includes
#include "custom_models/elasticity_models/elasticity_model.hpp"

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
  class KRATOS_API(CONSTITUTIVE_MODELS_APPLICATION) HyperElasticModel : public ElasticityModel
  {
  protected:

    
    struct StrainInvariants
    {
      double I1;
      double I2;
      double I3;
      
      double J;
      double J_13;

    };
    
    struct HyperElasticFactors
    {
      double Alpha1;  //1st derivative I1
      double Alpha2;  //1st derivative I2
      double Alpha3;  //1st derivative I3
      double Alpha4;  //1st derivative J

      double Beta1;   //2nd derivative I1
      double Beta2;   //2nd derivative I2
      double Beta3;   //2nd derivative I3
      double Beta4;   //2nd derivative J
 
    };
   
    struct CauchyGreenData
    {
    public:
      
      StrainInvariants Invariants;

      MatrixType       CauchyGreenMatrix; //left(b) or right(C) cauchy green
      MatrixType       InverseCauchyGreenMatrix; //insverse right(C) cauchy green
      
    };

    
    struct HyperElasticModelData
    {
    private:

      Flags*               mpState;
      const ModelDataType* mpModelData;
      
    public:

      HyperElasticFactors    Factors;
      CauchyGreenData         Strain;

      //Set Data Pointers
      void SetState           (Flags& rState)                    {mpState = &rState;};
      void SetModelData       (const ModelDataType&  rModelData) {mpModelData = &rModelData;};
      
      //Get Data Pointers
      const ModelDataType&    GetModelData                () const {return *mpModelData;};
      const MaterialDataType& GetMaterialParameters       () const {return mpModelData->GetMaterialParameters();}; 

      //Get non const Data
      Flags& State                                        () {return *mpState;};

      //Get const Data
      const Flags&  GetState                              () const {return *mpState;};

      
    };
    
    
  public:
    
    ///@name Type Definitions
    ///@{
    typedef HyperElasticModelData        HyperElasticDataType;
    
    /// Pointer definition of HyperElasticModel
    KRATOS_CLASS_POINTER_DEFINITION( HyperElasticModel );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.    
    HyperElasticModel(); 

    /// Copy constructor.
    HyperElasticModel(HyperElasticModel const& rOther);

    /// Assignment operator.
    HyperElasticModel& operator=(HyperElasticModel const& rOther);

    /// Clone.
    virtual ElasticityModel::Pointer Clone() const;


    /// Destructor.
    virtual ~HyperElasticModel();


    ///@}
    ///@name Operators
    ///@{

    
    ///@}
    ///@name Operations
    ///@{
    
    
    /**
     * Calculate Strain Energy Density Functions
     */
    virtual void CalculateStrainEnergy(ModelDataType& rValues, double& rDensityFunction) override;
    
    
    /**
     * Calculate Stresses
     */
    virtual void CalculateStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override;

    virtual void CalculateIsochoricStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override;

    virtual void CalculateVolumetricStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override;


    /**
     * Calculate Constitutive Tensor
     */
    virtual void CalculateConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix) override; 
    
    virtual void CalculateIsochoricConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix) override; 

    virtual void CalculateVolumetricConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix) override; 

    
    /**
     * Calculate Stress and Constitutive Tensor
     */
    virtual void CalculateStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix) override;

    virtual void CalculateIsochoricStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix) override;

    virtual void CalculateVolumetricStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix) override;

    
    /**
     * Check
     */    
    virtual int Check(const Properties& rMaterialProperties, const ProcessInfo& rCurrentProcessInfo) override;
    
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
        buffer << "HyperElasticModel";
        return buffer.str();
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "HyperElasticModel";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const override
    {
      rOStream << "HyperElasticModel Data";
    }


    ///@}
    ///@name Friends
    ///@{


    ///@}

  protected:
    ///@name Protected static Member Variables
    ///@{

    const MatrixType msIdentityMatrix;

    ///@}
    ///@name Protected member Variables
    ///@{


    ///@}
    ///@name Protected Operators
    ///@{
    
   
    ///@}
    ///@name Protected Operations
    ///@{


    /**
     * Calculate Stresses
     */
    virtual void CalculateAndAddStressTensor(HyperElasticDataType& rVariables, MatrixType& rStressMatrix);

    virtual void CalculateAndAddIsochoricStressTensor(HyperElasticDataType& rVariables, MatrixType& rStressMatrix);

    virtual void CalculateAndAddVolumetricStressTensor(HyperElasticDataType& rVariables, MatrixType& rStressMatrix);

    /**
     * Calculate Constitutive Tensor
     */
    virtual void CalculateAndAddConstitutiveTensor(HyperElasticDataType& rVariables, Matrix& rConstitutiveMatrix);

    virtual void CalculateAndAddIsochoricConstitutiveTensor(HyperElasticDataType& rVariables, Matrix& rConstitutiveMatrix);

    virtual void CalculateAndAddVolumetricConstitutiveTensor(HyperElasticDataType& rVariables, Matrix& rConstitutiveMatrix);
    
    /**
     * Calculate Constitutive Components
     */    

    virtual double& AddConstitutiveComponent(HyperElasticDataType& rVariables, double &rCabcd,
					     const unsigned int& a, const unsigned int& b,
					     const unsigned int& c, const unsigned int& d);

    
    virtual double& AddIsochoricConstitutiveComponent(HyperElasticDataType& rVariables, double &rCabcd,
						      const unsigned int& a, const unsigned int& b,
						      const unsigned int& c, const unsigned int& d);
       

    virtual double& AddVolumetricConstitutiveComponent(HyperElasticDataType& rVariables, double &rCabcd,
						       const unsigned int& a, const unsigned int& b,
						       const unsigned int& c, const unsigned int& d);


    //************// Strain Data
    

    void CalculateStrainData(ModelDataType& rValues, HyperElasticDataType& rVariables);    
        
    void CalculateInvariants(HyperElasticDataType& rVariables);
        
    void CalculateScalingFactors(HyperElasticDataType& rVariables);

    void CalculateStrainInvariants(const MatrixType& rCauchyGreenMatrix, double& rI1, double& rI2, double& rI3);
    

    //************//W

    virtual void CalculateAndAddIsochoricStrainEnergy(HyperElasticDataType& rVariables, double& rIsochoricDensityFunction);
    
    virtual void CalculateAndAddVolumetricStrainEnergy(HyperElasticDataType& rVariables, double& rVolumetricDensityFunction);

    //************// dW
    
    virtual double& GetFunction1stI1Derivative(HyperElasticDataType& rVariables, double& rDerivative); //dW/dI1
 
    virtual double& GetFunction1stI2Derivative(HyperElasticDataType& rVariables, double& rDerivative); //dW/dI2
 
    virtual double& GetFunction1stI3Derivative(HyperElasticDataType& rVariables, double& rDerivative); //dW/dI3

    virtual double& GetVolumetricFunction1stJDerivative(HyperElasticDataType& rVariables, double& rDerivative); //dU/dJ


    
    virtual double& GetFunction2ndI1Derivative(HyperElasticDataType& rVariables, double& rDerivative); //ddW/dI1dI1

    virtual double& GetFunction2ndI2Derivative(HyperElasticDataType& rVariables, double& rDerivative); //ddW/dI2dI2

    virtual double& GetFunction2ndI3Derivative(HyperElasticDataType& rVariables, double& rDerivative); //ddW/dI3dI3
    
    virtual double& GetVolumetricFunction2ndJDerivative(HyperElasticDataType& rVariables, double& rDerivative); //ddU/dJdJ

    
    double& GetFourthOrderUnitTensor(double& rValue,
				     const double& a,
				     const double& b,
				     const double& c,
				     const double& d); //ddC/dCdC or ddb/dbdb


    //isochoric volumetric slit
      
    MatrixType& GetJLeftCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dJ/db
 
    MatrixType& GetIsochoricRightCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dC'/dC

    double& GetIsochoricRightCauchyGreenDerivative(const CauchyGreenData& rData,
						   double& rDerivative,
						   const double& a,
						   const double& b,
						   const double& c,
						   const double& d); //dC'/dC
   
    
    MatrixType& GetIsochoricLeftCauchyGreenDerivative(const CauchyGreenData& rData,
						      MatrixType& rDerivative); //db'/db

    double& GetIsochoricLeftCauchyGreenDerivative(const CauchyGreenData& rData,
						  double& rDerivative,
						  const double& a,
						  const double& b,
						  const double& c,
						  const double& d); //db'/db
    
    //************// right cauchy green: C
    
    MatrixType& GetI1RightCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dI1/dC

    MatrixType& GetI2RightCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dI2/dC

    MatrixType& GetI3RightCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dI3/dC 

    MatrixType& GetJRightCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dJ/dC



    double& GetInverseRightCauchyGreenDerivative(const CauchyGreenData& rData,
						 double& rDerivative,
						 const double& a,
						 const double& b,
						 const double& c,
						 const double& d); //dC^-1/dC

    
    //Invariants 1st derivatives by components
    double& GetI1RightCauchyGreen1stDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b); //dI1/dC


    double& GetI2RightCauchyGreen1stDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b); //dI2/dC
 

    double& GetI3RightCauchyGreen1stDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b); //dI3/dC



    double& GetJRightCauchyGreen1stDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b); ///dJ/dC
 

    //Invariants Square of the 1st derivatives by components
    double& GetI1RightCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						     double& rDerivative,
						     const double& a,
						     const double& b,
						     const double& c,
						     const double& d); //dI1/dC * dI2/dC
 

    double& GetI2RightCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						     double& rDerivative,
						     const double& a,
						     const double& b,
						     const double& c,
						     const double& d); //dI2/dC * dI3/dC
 

    double& GetI3RightCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						     double& rDerivative,
						     const double& a,
						     const double& b,
						     const double& c,
						     const double& d); //dI3/dC * dI3/dC
 
    
    double& GetJRightCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						    double& rDerivative,
						    const double& a,
						    const double& b,
						    const double& c,
						    const double& d); //dJ/dC * dJ/dC

    
    
    //Invariants 2nd derivatives by components
    double& GetI1RightCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b,
					       const double& c,
					       const double& d); //ddI1/dCdC
 

    double& GetI2RightCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b,
					       const double& c,
					       const double& d); //ddI2/dCdC


    double& GetI3RightCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					       double& rDerivative,
					       const double& a,
					       const double& b,
					       const double& c,
					       const double& d); //ddI3/dCdC


    double& GetJRightCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b,
					      const double& c,
					      const double& d); //ddJ/dCdC


    
    //************// left cauchy green : b
    
    MatrixType& GetI1LeftCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dI1/db

    MatrixType& GetI2LeftCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative);  //dI2/db

    MatrixType& GetI3LeftCauchyGreenDerivative(const CauchyGreenData& rData, MatrixType& rDerivative); //dI3/db


    //Invariants 1st derivatives by components
    double& GetI1LeftCauchyGreen1stDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b); //dI1/db
 

    double& GetI2LeftCauchyGreen1stDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b); //dI2/db


    double& GetI3LeftCauchyGreen1stDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b); //dI3/db

    
    double& GetJRightCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						    double& rDerivative,
						    const double& a,
						    const double& b); //dJ/db
  
    //Invariants Square of the 1st derivatives by components
    double& GetI1LeftCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						    double& rDerivative,
						    const double& a,
						    const double& b,
						    const double& c,
						    const double& d); //dI1/db * dI1/db
 

    double& GetI2LeftCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						    double& rDerivative,
						    const double& a,
						    const double& b,
						    const double& c,
						    const double& d); //dI2/db * dI2/db


    double& GetI3LeftCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						    double& rDerivative,
						    const double& a,
						    const double& b,
						    const double& c,
						    const double& d); //dI3/db * dI3/db


    
    double& GetJLeftCauchyGreenSquare1stDerivative(const CauchyGreenData& rData,
						   double& rDerivative,
						   const double& a,
						   const double& b,
						   const double& c,
						   const double& d); //dJ/db * dJ/db

    
    //Invariants 2nd derivatives by components
    double& GetI1LeftCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b,
					      const double& c,
					      const double& d); //ddI1/dbdb
 

    double& GetI2LeftCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b,
					      const double& c,
					      const double& d); //ddI2/dbdb


    double& GetI3LeftCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					      double& rDerivative,
					      const double& a,
					      const double& b,
					      const double& c,
					      const double& d); //ddI3/dbdb


    double& GetJLeftCauchyGreen2ndDerivative(const CauchyGreenData& rData,
					     double& rDerivative,
					     const double& a,
					     const double& b,
					     const double& c,
					     const double& d); //ddJ/dbdb
 
    
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
    ///@name Serialization
    ///@{
    friend class Serializer;


    virtual void save(Serializer& rSerializer) const override
    {
      KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, ElasticityModel )
    }

    virtual void load(Serializer& rSerializer) override
    {
      KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, ElasticityModel )      
    }

    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

  }; // Class HyperElasticModel

  ///@}

  ///@name Type Definitions
  ///@{


  ///@}
  ///@name Input and output
  ///@{


  ///@}

  ///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_HYPERELASTIC_MODEL_H_INCLUDED  defined 


