//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                   April 2017 $
//   Revision:            $Revision:                      0.0 $
//
//

#if !defined(KRATOS_NON_LINEAR_ASSOCIATIVE_PLASTICITY_MODEL_H_INCLUDED )
#define  KRATOS_NON_LINEAR_ASSOCIATIVE_PLASTICITY_MODEL_H_INCLUDED


// System includes

// External includes

// Project includes
#include "custom_models/plasticity_models/plasticity_model.hpp"

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
  template<class TElasticityModel, class TYieldCriterion>
  class KRATOS_API(CONSTITUTIVE_MODELS_APPLICATION) NonLinearAssociativePlasticityModel : public PlasticityModel<TElasticityModel,TYieldCriterion>
  {
  public:
    
    ///@name Type Definitions
    ///@{

    //elasticity model
    typedef TElasticityModel                               ElasticityModelType;
    typedef typename  ElasticityModelType::Pointer      ElasticityModelPointer;

    //yield criterion
    typedef TYieldCriterion                                 YieldCriterionType;
    typedef typename YieldCriterionType::Pointer         YieldCriterionPointer;

    //base type
    typedef PlasticityModel<ElasticityModelType,YieldCriterionType>   BaseType;

    //common types
    typedef typename BaseType::Pointer                         BaseTypePointer;
    typedef typename BaseType::SizeType                               SizeType;
    typedef typename BaseType::VoigtIndexType                   VoigtIndexType;
    typedef typename BaseType::MatrixType                           MatrixType;
    typedef typename BaseType::ModelDataType                     ModelDataType;
    typedef typename BaseType::MaterialDataType               MaterialDataType;
    typedef typename BaseType::PlasticDataType                 PlasticDataType;
    typedef typename BaseType::InternalVariablesType     InternalVariablesType;

  protected:    

    struct ThermalVariables
    {
    public:
      double PlasticDissipation;
      double DeltaPlasticDissipation;

    private:

      friend class Serializer;
      
      void save(Serializer& rSerializer) const
      {
	rSerializer.save("PlasticDissipation",PlasticDissipation);
	rSerializer.save("DeltaPlasticDissipation",DeltaPlasticDissipation);
      };

      void load(Serializer& rSerializer)
      {
	rSerializer.load("PlasticDissipation",PlasticDissipation);
	rSerializer.load("DeltaPlasticDissipation",DeltaPlasticDissipation);
      };
      
    };


    struct PlasticFactors
    {      
      double Beta0;
      double Beta1;
      double Beta2;
      double Beta3;
      double Beta4;	   

      MatrixType  Normal;
      MatrixType  Dev_Normal;
    };

     	
  public:

    
    /// Pointer definition of NonLinearAssociativePlasticityModel
    KRATOS_CLASS_POINTER_DEFINITION( NonLinearAssociativePlasticityModel );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    NonLinearAssociativePlasticityModel();

    /// Constructor.
    NonLinearAssociativePlasticityModel(ElasticityModelPointer pElasticityModel, YieldCriterionPointer pYieldCriterion);
    
    /// Copy constructor.
    NonLinearAssociativePlasticityModel(NonLinearAssociativePlasticityModel const& rOther);

    /// Assignment operator.
    NonLinearAssociativePlasticityModel& operator=(NonLinearAssociativePlasticityModel const& rOther);

    /// Clone.
    virtual ElasticityModel::Pointer Clone() const override;
    
    /// Destructor.
    virtual ~NonLinearAssociativePlasticityModel();


    ///@}
    ///@name Operators
    ///@{

   
    ///@}
    ///@name Operations
    ///@{

    /**
     * Calculate Stresses
     */

    virtual void CalculateStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override;

    virtual void CalculateIsochoricStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix) override;
    
    /**
     * Calculate Constitutive Tensor
     */
    virtual void CalculateConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix) override; 
       
    /**
     * Calculate Stress and Constitutive Tensor
     */
    virtual void CalculateStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix) override;
    

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
      buffer << "NonLinearAssociativePlasticityModel" ;
      return buffer.str();
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const override
    {
      rOStream << "NonLinearAssociativePlasticityModel";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const override
    {
      rOStream << "NonLinearAssociativePlasticityModel Data";	    
    }

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
   
    // internal variables:
    InternalVariablesType  mInternal;
    InternalVariablesType  mPreviousInternal;

    ThermalVariables  mThermalVariables;
	
    ///@}
    ///@name Protected Operators
    ///@{

    
    ///@}
    ///@name Protected Operations
    ///@{

    /**
     * Calculate Stresses
     */
    virtual void CalculateAndAddIsochoricStressTensor(PlasticDataType& rVariables, MatrixType& rStressMatrix);

    /**
     * Calculate Constitutive Tensor
     */
    virtual void CalculateAndAddPlasticConstitutiveTensor(PlasticDataType& rVariables, Matrix& rConstitutiveMatrix);
    
    /**
     * Calculate Constitutive Components
     */    
    virtual double& AddPlasticConstitutiveComponent(PlasticDataType& rVariables,
						    PlasticFactors& rFactors, double& rCabcd,
						    const unsigned int& a, const unsigned int& b,
						    const unsigned int& c, const unsigned int& d);
    
    // calculate ratial return
    
    virtual bool CalculateRadialReturn        (PlasticDataType& rVariables, MatrixType& rStressMatrix);

    // implex protected methods
    virtual void CalculateImplexRadialReturn  (PlasticDataType& rVariables, MatrixType& rStressMatrix);


    // auxiliar methods

    virtual void InitializeVariables          (ModelDataType& rValues, PlasticDataType& rVariables);
     
    virtual void UpdateStressConfiguration    (PlasticDataType& rVariables, MatrixType& rStressMatrix);	 

    virtual void UpdateInternalVariables      (PlasticDataType& rVariables);
    
    virtual void CalculateScalingFactors      (PlasticDataType& rVariables, PlasticFactors& rScalingFactors);

    
    // energy calculation methods

    void CalculateThermalDissipation          (PlasticDataType& rVariables);

    // implex protected methods
    void CalculateImplexThermalDissipation    (PlasticDataType& rVariables);

    
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
      KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, BaseType )
      rSerializer.save("InternalVariables",mInternal);
      rSerializer.save("PreviousInternalVariables",mPreviousInternal);
      rSerializer.save("ThermalVariables",mThermalVariables);
    }

    virtual void load(Serializer& rSerializer) override
    {
      KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, BaseType )
      rSerializer.load("InternalVariables",mInternal);
      rSerializer.load("PreviousInternalVariables",mPreviousInternal);
      rSerializer.load("ThermalVariables",mThermalVariables);      
    }
    
    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

  }; // Class NonLinearAssociativePlasticityModel

  ///@}

  ///@name Type Definitions
  ///@{

  ///@}
  ///@name Input and output
  ///@{

  ///@}
  
  ///@} addtogroup block

  //*****************************DEFAULT CONSTRUCTOR************************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::NonLinearAssociativePlasticityModel()
    :BaseType()
  {
    KRATOS_TRY
	    

    KRATOS_CATCH(" ")    
  }
  
  //*******************************CONSTRUCTOR******************************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::NonLinearAssociativePlasticityModel(ElasticityModelPointer pElasticityModel, YieldCriterionPointer pYieldCriterion)
    :BaseType(pElasticityModel, pYieldCriterion)
  {
    KRATOS_TRY
	    

    KRATOS_CATCH(" ")    
  }

  //*******************************ASSIGMENT OPERATOR***********************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>& NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::operator=(NonLinearAssociativePlasticityModel const& rOther)
  {
    BaseType::operator=(rOther);
    return *this;
  }

  //*******************************COPY CONSTRUCTOR*************************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::NonLinearAssociativePlasticityModel(NonLinearAssociativePlasticityModel const& rOther)
    :BaseType(rOther)
    ,mInternal(rOther.mInternal)
    ,mPreviousInternal(rOther.mPreviousInternal)
    ,mThermalVariables(rOther.mThermalVariables)
  {

  }

  //********************************CLONE***********************************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  ElasticityModel::Pointer NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::Clone() const
  {
    return ( NonLinearAssociativePlasticityModel::Pointer(new NonLinearAssociativePlasticityModel(*this)) );
  }

  //********************************DESTRUCTOR******************************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::~NonLinearAssociativePlasticityModel()
  {
  }

  //*********************************INITIALIZE VARIABLES*******************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::InitializeVariables(ModelDataType& rValues, PlasticDataType& rVariables)
  {
    KRATOS_TRY

    //set model data pointer
    rVariables.SetModelData(rValues);

    rValues.State.Set(ConstitutiveModelData::PLASTIC_REGION,false);

    rValues.State.Set(ConstitutiveModelData::IMPLEX_ACTIVE,false);
    if( rValues.GetProcessInfo()[IMPLEX] == 1 )
      rValues.State.Set(ConstitutiveModelData::IMPLEX_ACTIVE,true);

    rVariables.SetState(rValues.State);
    
    // RateFactor
    rVariables.RateFactor = 0;

    // EquivalentPlasticStrain    
    rVariables.Internal = mInternal;

    // DeltaGamma / DeltaPlasticStrain (asociative plasticity)
    rVariables.DeltaInternal.Variables.clear();

    // Flow Rule local variables
    rVariables.TrialStateFunction = 0;
    rVariables.StressNorm = 0;
      
    KRATOS_CATCH(" ")
  }
	
  //*********************************CALCULATE STRESS***********************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix)
  {

    KRATOS_TRY

    // calculate volumetric stress
    MatrixType VolumetricStressMatrix;
    VolumetricStressMatrix.clear();
    this->mpElasticityModel->CalculateVolumetricStressTensor(rValues,VolumetricStressMatrix);

    // calculate isochoric stress
    this->CalculateIsochoricStressTensor(rValues,rStressMatrix);

    rValues.StressMatrix = rStressMatrix;  //store isochoric stress as StressMatrix
      
    rStressMatrix += VolumetricStressMatrix;
	
    
    KRATOS_CATCH(" ")
    
  }

  //****************************CALCULATE ISOCHORIC STRESS******************************
  //************************************************************************************
  
  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateIsochoricStressTensor(ModelDataType& rValues, MatrixType& rStressMatrix)
  {

    KRATOS_TRY
    
    PlasticDataType Variables;
    this->InitializeVariables(rValues,Variables);

    // calculate elastic isochoric stress
    this->mpElasticityModel->CalculateIsochoricStressTensor(rValues,rStressMatrix);

    // calculate plastic isochoric stress
    this->CalculateAndAddIsochoricStressTensor(Variables,rStressMatrix);


    if( rValues.State.Is(ConstitutiveModelData::UPDATE_INTERNAL_VARIABLES ) )
      this->UpdateInternalVariables(Variables);

    
    KRATOS_CATCH(" ")
    
  }

  //****************************CALCULATE ISOCHORIC STRESS******************************
  //************************************************************************************
  
  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateAndAddIsochoricStressTensor(PlasticDataType& rVariables, MatrixType& rStressMatrix)
  {

    KRATOS_TRY

    //1.-Isochoric stress norm   (rStressMatrix := Elastic Isochoric Stress Matrix)
    rVariables.StressNorm = ConstitutiveLawUtilities::CalculateStressNorm(rStressMatrix, rVariables.StressNorm);

    //2.-Check yield condition
    rVariables.TrialStateFunction = this->mpYieldCriterion->CalculateYieldCondition(rVariables, rVariables.TrialStateFunction);
    
    if( rVariables.State().Is(ConstitutiveModelData::IMPLEX_ACTIVE) ) 
      {
	//3.- Calculate the implex radial return
	this->CalculateImplexRadialReturn(rVariables,rStressMatrix);
      }
    else{

      if( rVariables.TrialStateFunction <= 0 )
	{
	  rVariables.State().Set(ConstitutiveModelData::PLASTIC_REGION,false);
	}
      else
	{

	  //3.- Calculate the radial return
	  bool converged = this->CalculateRadialReturn(rVariables,rStressMatrix);
	    
	  if(!converged)
	    std::cout<<" ConstitutiveLaw did not converge "<<std::endl;


	  //4.- Update back stress, plastic strain and stress
	  this->UpdateStressConfiguration(rVariables,rStressMatrix);
	    
	  //5.- Calculate thermal dissipation and delta thermal dissipation
	  this->CalculateThermalDissipation(rVariables);   

	  rVariables.State().Set(ConstitutiveModelData::PLASTIC_REGION,true);	    
	}

    }

    rVariables.State().Set(ConstitutiveModelData::COMPUTED_RETURN_MAPPING,true);
   
    KRATOS_CATCH(" ")
    
  }

  //***************************CALCULATE RADIAL RETURN MAPPING**************************
  //************************************************************************************
  
  // local newton procedure
  template<class TElasticityModel,class TYieldCriterion>
  bool NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateRadialReturn(PlasticDataType& rVariables, MatrixType& rStressMatrix)
  {
    KRATOS_TRY
      
    //Set convergence parameters
    unsigned int iter    = 0;
    double Tolerance     = 1e-5;
    double MaxIterations = 50;

    //start
    double DeltaDeltaGamma    = 0;
    double DeltaStateFunction = 0;
    double DeltaPlasticStrain = 0;
    
    double& EquivalentPlasticStrainOld  = mPreviousInternal.Variables[0];
    double& EquivalentPlasticStrain     = rVariables.Internal.Variables[0];
    double& DeltaGamma                  = rVariables.DeltaInternal.Variables[0];

    EquivalentPlasticStrain = 0;
    DeltaGamma = 0;

    double StateFunction                = rVariables.TrialStateFunction;
    
    while ( fabs(StateFunction)>=Tolerance && iter<=MaxIterations)
      {
	//Calculate Delta State Function:
	DeltaStateFunction = this->mpYieldCriterion->CalculateDeltaStateFunction( rVariables, DeltaStateFunction );

	//Calculate DeltaGamma:
	DeltaDeltaGamma  = StateFunction/DeltaStateFunction;
	DeltaGamma += DeltaDeltaGamma;
	       
	//Update Equivalent Plastic Strain:
	DeltaPlasticStrain      = sqrt(2.0/3.0) * DeltaGamma;
	EquivalentPlasticStrain = EquivalentPlasticStrainOld + DeltaPlasticStrain;
	       	
	//Calculate State Function:
	StateFunction = this->mpYieldCriterion->CalculateStateFunction( rVariables, StateFunction );

	iter++;
      }
	   

    if(iter>MaxIterations)
      return false;


    return true;

    KRATOS_CATCH(" ")
  }

  //***************************CALCULATE IMPLEX RETURN MAPPING**************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateImplexRadialReturn(PlasticDataType& rVariables, MatrixType& rStressMatrix)
  {
    KRATOS_TRY
      
    double& EquivalentPlasticStrainOld  = mPreviousInternal.Variables[0];
    double& EquivalentPlasticStrain     = rVariables.Internal.Variables[0];
    double& DeltaGamma                  = rVariables.DeltaInternal.Variables[0];
   
    //1.-Computation of the plastic Multiplier
    DeltaGamma = sqrt(3.0/2.0) * ( EquivalentPlasticStrain - EquivalentPlasticStrainOld );
	
    //2.- Update back stress, plastic strain and stress
    this->UpdateStressConfiguration(rVariables,rStressMatrix);

    //3.- Calculate thermal dissipation and delta thermal dissipation
    if( DeltaGamma > 0 ){
	  
      this->CalculateImplexThermalDissipation( rVariables );
      rVariables.State().Set(ConstitutiveModelData::PLASTIC_REGION,true);
      
    }
    else{
      
      mThermalVariables.PlasticDissipation = 0;
      mThermalVariables.DeltaPlasticDissipation = 0;
    } 
  
    KRATOS_CATCH(" ")
  }


  //***************************CALCULATE THERMAL DISSIPATION****************************
  //************************************************************************************


  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateThermalDissipation( PlasticDataType& rVariables )
  {
    KRATOS_TRY
      
    //1.- Thermal Dissipation:
 
    mThermalVariables.PlasticDissipation = this->mpYieldCriterion->CalculatePlasticDissipation( rVariables, mThermalVariables.PlasticDissipation );
  

    //std::cout<<" PlasticDissipation "<<mThermalVariables.PlasticDissipation<<std::endl;

    //2.- Thermal Dissipation Increment:

    mThermalVariables.DeltaPlasticDissipation = this->mpYieldCriterion->CalculateDeltaPlasticDissipation( rVariables, mThermalVariables.DeltaPlasticDissipation );
		    		    
    //std::cout<<" DeltaPlasticDissipation "<<mThermalVariables.DeltaPlasticDissipation<<std::endl;

    KRATOS_CATCH(" ")    
  }


  //***************************CALCULATE THERMAL DISSIPATION****************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateImplexThermalDissipation( PlasticDataType& rVariables )
  {
    KRATOS_TRY
       
    //1.- Thermal Dissipation:
	
    mThermalVariables.PlasticDissipation = this->mpYieldCriterion->CalculateImplexPlasticDissipation( rVariables, mThermalVariables.PlasticDissipation );
  
    //2.- Thermal Dissipation Increment:
      
    mThermalVariables.DeltaPlasticDissipation = this->mpYieldCriterion->CalculateImplexDeltaPlasticDissipation( rVariables, mThermalVariables.DeltaPlasticDissipation );

    KRATOS_CATCH(" ")  
  
  }

  //***************************UPDATE STRESS CONFIGURATION *****************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::UpdateStressConfiguration(PlasticDataType& rVariables, MatrixType& rStressMatrix)
  {
    KRATOS_TRY
          
    //Plastic Strain and Back Stress update
    if( rVariables.StressNorm > 0 ){

      double& DeltaGamma = rVariables.DeltaInternal.Variables[0];
      const MaterialDataType& rMaterial = rVariables.GetMaterialParameters();
             
      //Stress Update: 
      rStressMatrix -= ( rStressMatrix * ( ( 2.0 * rMaterial.GetLameMuBar() * DeltaGamma ) / rVariables.StressNorm ) );
	  
    }

    KRATOS_CATCH(" ")    
  }

  //***************************UPDATE INTERNAL VARIABLES********************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::UpdateInternalVariables(PlasticDataType& rVariables)
  {
    KRATOS_TRY
    
    double& EquivalentPlasticStrainOld  = mPreviousInternal.Variables[0];
    double& EquivalentPlasticStrain     = rVariables.Internal.Variables[0];
    double& DeltaGamma                  = rVariables.DeltaInternal.Variables[0];

    //update mechanical variables
    EquivalentPlasticStrainOld  = EquivalentPlasticStrain;
    EquivalentPlasticStrain    += sqrt(2.0/3.0) * DeltaGamma;
	
    //update thermal variables
    //mThermalVariables = rVariables.Thermal;
    
    KRATOS_CATCH(" ")    
  }


  //**************CALCULATE SCALING FACTORS FOR THE ELASTO PLASTIC MODULI***************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateScalingFactors(PlasticDataType& rVariables, PlasticFactors& rFactors)
  {
    KRATOS_TRY

    //0.-Get needed parameters
    const ModelDataType&    rModelData = rVariables.GetModelData();
    const MaterialDataType& rMaterial  = rVariables.GetMaterialParameters();

    double&             rDeltaGamma            = rVariables.DeltaInternal.Variables[0];
    const MatrixType&   rIsochoricStressMatrix = rModelData.GetStressMatrix(); //isochoric stress stored as StressMatrix
    
    //1.-Identity build
    MatrixType Identity = identity_matrix<double> (3);
    
    //2.-Auxiliar matrices
    rFactors.Normal      = rIsochoricStressMatrix * ( 1.0 / rVariables.StressNorm );

    MatrixType Norm_Normal      = prod( rFactors.Normal, trans(rFactors.Normal) );

    double Trace_Norm_Normal    = Norm_Normal( 0, 0 ) + Norm_Normal( 1, 1 ) + Norm_Normal( 2, 2 );

    rFactors.Dev_Normal  = Norm_Normal;
    
    rFactors.Dev_Normal -= (1.0/3.0) * Trace_Norm_Normal * Identity;

    //3.-Auxiliar constants    
    if( rVariables.State().Is(ConstitutiveModelData::IMPLEX_ACTIVE) ) 
      {
	
	rFactors.Beta0 = 0;
		
	rFactors.Beta1 = 2.0 * rMaterial.GetLameMuBar() * rDeltaGamma / rVariables.StressNorm;
		
	rFactors.Beta2 = (2.0/3.0) * rVariables.StressNorm * rDeltaGamma / ( rMaterial.GetLameMuBar() );
		
	rFactors.Beta3 = ( -rFactors.Beta1 + rFactors.Beta2 );
		
	rFactors.Beta4 = ( -rFactors.Beta1 ) * rVariables.StressNorm / ( rMaterial.GetLameMuBar() );

      }
    else
      {
    
	if( rVariables.State().Is(ConstitutiveModelData::PLASTIC_RATE_REGION) )
	  rVariables.RateFactor = 1;
	else if ( rVariables.State().IsNot(ConstitutiveModelData::PLASTIC_RATE_REGION) )
	  rVariables.RateFactor = 0;
	
	double DeltaHardening = this->mpYieldCriterion->GetHardeningLaw().CalculateDeltaHardening( rVariables, DeltaHardening );

	rFactors.Beta0 = 1.0 + DeltaHardening/(3.0 * rMaterial.GetLameMuBar());
		
	rFactors.Beta1 = 2.0 * rMaterial.GetLameMuBar() * rDeltaGamma / rVariables.StressNorm;
		
	rFactors.Beta2 = ( ( 1.0 - ( 1.0 / rFactors.Beta0 ) ) * (2.0/3.0) * rVariables.StressNorm * rDeltaGamma )/( rMaterial.GetLameMuBar() );
		
	rFactors.Beta3 = ( ( 1.0 / rFactors.Beta0 ) - rFactors.Beta1 + rFactors.Beta2 );
		
	rFactors.Beta4 = ( ( 1.0 / rFactors.Beta0 ) - rFactors.Beta1 ) * rVariables.StressNorm / ( rMaterial.GetLameMuBar() );

      }
	
    //std::cout<<"FACTORS:: Beta0 "<<rFactors.Beta0<<" Beta 1 "<<rFactors.Beta1<<" Beta2 "<<rFactors.Beta2<<" Beta 3 "<<rFactors.Beta3<<" Beta4 "<<rFactors.Beta4<<std::endl;

    KRATOS_CATCH(" ")    
	  
  }

  //***********************COMPUTE ALGORITHMIC CONSTITUTIVE MATRIX**********************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateConstitutiveTensor(ModelDataType& rValues, Matrix& rConstitutiveMatrix)
  {
    KRATOS_TRY

    //Initialize ConstitutiveMatrix
    rConstitutiveMatrix.clear();
      
    PlasticDataType Variables;
    this->InitializeVariables(rValues,Variables);

    // calculate isochoric stress (radial return is needed)

    MatrixType StressMatrix = rValues.StressMatrix;
    //1.-Elastic Isochoric Stress Matrix
    this->mpElasticityModel->CalculateIsochoricStressTensor(rValues,rValues.StressMatrix);

    //2.-Calculate and Add Plastic Isochoric Stress Matrix
    this->CalculateAndAddIsochoricStressTensor(Variables,rValues.StressMatrix);
    
    //Calculate Constitutive Matrix

    // calculate elastic constitutive tensor
    this->mpElasticityModel->CalculateConstitutiveTensor(rValues,rConstitutiveMatrix);
    
    // calculate plastic constitutive tensor
    this->CalculateAndAddPlasticConstitutiveTensor(Variables,rConstitutiveMatrix);

    rValues.StressMatrix = StressMatrix;
    
    KRATOS_CATCH(" ")
  }

  //************************************************************************************
  //************************************************************************************
  
  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateStressAndConstitutiveTensors(ModelDataType& rValues, MatrixType& rStressMatrix, Matrix& rConstitutiveMatrix)
  {
    KRATOS_TRY

    PlasticDataType Variables;
    this->InitializeVariables(rValues,Variables);

    //Calculate Stress Matrix

    // calculate volumetric stress
    MatrixType VolumetricStressMatrix;
    this->mpElasticityModel->CalculateVolumetricStressTensor(rValues,VolumetricStressMatrix);

    // calculate isochoric stress

    // calculate elastic isochoric stress
    this->mpElasticityModel->CalculateIsochoricStressTensor(rValues,rStressMatrix);
   
    // calculate plastic isochoric stress
    this->CalculateAndAddIsochoricStressTensor(Variables,rStressMatrix);

     rValues.StressMatrix = rStressMatrix;  //store isochoric stress as StressMatrix
   
    //Calculate Constitutive Matrix
    
    // calculate elastic constitutive tensor
    this->mpElasticityModel->CalculateConstitutiveTensor(rValues,rConstitutiveMatrix);
    
    // calculate plastic constitutive tensor
    this->CalculateAndAddPlasticConstitutiveTensor(Variables,rConstitutiveMatrix);

    
    rStressMatrix += VolumetricStressMatrix;
    
    
    KRATOS_CATCH(" ")
  }

  
  //************************************************************************************
  //************************************************************************************
  
  template<class TElasticityModel,class TYieldCriterion>
  void NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::CalculateAndAddPlasticConstitutiveTensor(PlasticDataType& rVariables, Matrix& rConstitutiveMatrix)
  {
    KRATOS_TRY
    
    //Compute radial return
    if( rVariables.State().IsNot(ConstitutiveModelData::COMPUTED_RETURN_MAPPING) )
      KRATOS_THROW_ERROR( std::logic_error, "ReturnMapping has to be computed to perform the calculation", "" )
    
    //Algorithmic moduli factors
    PlasticFactors Factors;    
    this->CalculateScalingFactors(rVariables,Factors);
    
    //Calculate HyperElastic ConstitutiveMatrix
    const ModelDataType&  rModelData        = rVariables.GetModelData();
    const SizeType&       rVoigtSize        = rModelData.GetVoigtSize();      
    const VoigtIndexType& rIndexVoigtTensor = rModelData.GetVoigtIndexTensor();
    
    for(SizeType i=0; i<rVoigtSize; i++)
      {
	for(SizeType j=0; j<rVoigtSize; j++)
	  {
	    
	    rConstitutiveMatrix(i,j) = this->AddPlasticConstitutiveComponent(rVariables,Factors,rConstitutiveMatrix(i,j),
									     rIndexVoigtTensor[i][0],rIndexVoigtTensor[i][1],
									     rIndexVoigtTensor[j][0],rIndexVoigtTensor[j][1]);
	  }
	
      }

    rVariables.State().Set(ConstitutiveModelData::COMPUTED_CONSTITUTIVE_MATRIX);
    
    KRATOS_CATCH(" ")
  }
  


  //********************CONSTITUTIVE MATRIX PLASTIC COMPONENT***************************
  //************************************************************************************

  template<class TElasticityModel,class TYieldCriterion>
  double& NonLinearAssociativePlasticityModel<TElasticityModel,TYieldCriterion>::AddPlasticConstitutiveComponent(PlasticDataType& rVariables,
													      PlasticFactors& rFactors, double& rCabcd,
													      const unsigned int& a, const unsigned int& b,
													      const unsigned int& c, const unsigned int& d)
  {
    KRATOS_TRY

    const ModelDataType&    rModelData       = rVariables.GetModelData();
    const MaterialDataType& rMaterial        = rVariables.GetMaterialParameters();

    const MatrixType& rIsochoricStressMatrix = rModelData.GetStressMatrix(); //isochoric stress stored as StressMatrix
    const MatrixType& rCauchyGreenMatrix     = rModelData.GetStrainMatrix();

    
    double Cabcd = (1.0/3.0) * ( rCauchyGreenMatrix(a,b) * rCauchyGreenMatrix(c,d) );
    
    Cabcd -= (0.5 * ( rCauchyGreenMatrix(a,c) * rCauchyGreenMatrix(b,d) + rCauchyGreenMatrix(a,d) * rCauchyGreenMatrix(b,c) ) );
    
    Cabcd *= 3.0 * rMaterial.GetLameMuBar();

    Cabcd += ( rCauchyGreenMatrix(c,d)* rIsochoricStressMatrix(a,b) + rIsochoricStressMatrix(c,d) * rCauchyGreenMatrix(a,b) );
 
    Cabcd *= (-2.0/3.0) * ( (-1) * rFactors.Beta1 );
    
    Cabcd -= rFactors.Beta3 * 2.0 * rMaterial.GetLameMuBar() * ( rFactors.Normal(a,b) * rFactors.Normal(c,d) );

    Cabcd -= rFactors.Beta4 * 2.0 * rMaterial.GetLameMuBar() * ( rFactors.Normal(a,b) * rFactors.Dev_Normal(c,d) );

    rCabcd += Cabcd;
    
    return rCabcd;     
    
    KRATOS_CATCH(" ")    
  }

  
}  // namespace Kratos.

#endif // KRATOS_NON_LINEAR_ASSOCIATIVE_PLASTICITY_MODEL_H_INCLUDED  defined 


