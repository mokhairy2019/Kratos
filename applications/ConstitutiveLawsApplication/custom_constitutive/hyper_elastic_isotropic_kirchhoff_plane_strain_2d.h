// KRATOS ___                _   _ _         _   _             __                       _
//       / __\___  _ __  ___| |_(_) |_ _   _| |_(_)_   _____  / /  __ ___      _____   /_\  _ __  _ __
//      / /  / _ \| '_ \/ __| __| | __| | | | __| \ \ / / _ \/ /  / _` \ \ /\ / / __| //_\\| '_ \| '_  |
//     / /__| (_) | | | \__ \ |_| | |_| |_| | |_| |\ V /  __/ /__| (_| |\ V  V /\__ \/  _  \ |_) | |_) |
//     \____/\___/|_| |_|___/\__|_|\__|\__,_|\__|_| \_/ \___\____/\__,_| \_/\_/ |___/\_/ \_/ .__/| .__/
//                                                                                         |_|   |_|
//
//  License:         BSD License
//                   license: structural_mechanics_application/license.txt
//
//  Main authors:    Malik Ali Dawi
//                   Ruben Zorrilla
//

#if !defined (KRATOS_HYPER_ELASTIC_ISOTROPIC_KIRCHHOFF_PLANE_STRAIN_2D_LAW_H_INCLUDED)
#define  KRATOS_HYPER_ELASTIC_ISOTROPIC_KIRCHHOFF_PLANE_STRAIN_2D_LAW_H_INCLUDED

// System includes

// External includes

// Project includes
#include "custom_constitutive/hyper_elastic_isotropic_kirchhoff_3d.h"

namespace Kratos
{
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
/**
 * @class HyperElasticIsotropicKirchhoffPlaneStrain2D
 * @ingroup StructuralMechanicsApplication
 * @brief This law defines an hyperelastic material according to the Saint-Venant–Kirchhoff formulation for 2D-plane strain cases
 * @details The simplest hyperelastic material model is the Saint Venant–Kirchhoff model which is just an extension of the linear elastic material model to the nonlinear regime.
 * More info https://en.wikipedia.org/wiki/Hyperelastic_material#Saint_Venant%E2%80%93Kirchhoff_model
 * @author Malik Ali Dawi
 * @author Ruben Zorrilla
 */
class KRATOS_API(CONSTITUTIVE_LAWS_APPLICATION) HyperElasticIsotropicKirchhoffPlaneStrain2D
    : public HyperElasticIsotropicKirchhoff3D
{
public:

    ///@name Type Definitions
    ///@{

    /// The definition of the process info
    typedef ProcessInfo               ProcessInfoType;

    /// The definition of the CL base  class
    typedef ConstitutiveLaw                CLBaseType;

    /// The definition of the base class
    typedef HyperElasticIsotropicKirchhoff3D BaseType;

    /// The definition of the size type
    typedef std::size_t                      SizeType;

    /// The definition of the index type
    typedef std::size_t                      IndexType;

    /// Static definition of the dimension
    static constexpr SizeType Dimension = 2;

    /// Static definition of the VoigtSize
    static constexpr SizeType VoigtSize = 3;

    /// Pointer definition of HyperElasticIsotropicKirchhoffPlaneStrain2D
    KRATOS_CLASS_POINTER_DEFINITION( HyperElasticIsotropicKirchhoffPlaneStrain2D );

    ///@name Lyfe Cycle
    ///@{

    /**
     * @brief Default constructor.
     */
    HyperElasticIsotropicKirchhoffPlaneStrain2D();

    /**
     * @brief Copy constructor.
     */
    HyperElasticIsotropicKirchhoffPlaneStrain2D (const HyperElasticIsotropicKirchhoffPlaneStrain2D& rOther);

    /**
     * @brief Destructor.
     */
    ~HyperElasticIsotropicKirchhoffPlaneStrain2D() override;

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief Clone operator
     */
    ConstitutiveLaw::Pointer Clone() const override;

    /**
     * @brief This function is designed to be called once to check compatibility with element
     * @param rFeatures The Features of the law
     */
    void GetLawFeatures(Features& rFeatures) override;

    /**
     * @brief Dimension of the law:
     */
    SizeType WorkingSpaceDimension() override
    {
        return Dimension;
    };

    /**
     * @brief Voigt tensor size:
     */
    SizeType GetStrainSize() override
    {
        return VoigtSize;
    };

protected:

    ///@name Protected static Member Variables
    ///@{

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
     * @brief It calculates the constitutive matrix C (PK2)
     * @param rConstitutiveMatrix The constitutive matrix
     * @param YoungModulus The Young modulus
     * @param PoissonCoefficient The Poisson coefficient
     */
    void CalculateConstitutiveMatrixPK2(
        Matrix& rConstitutiveMatrix,
        const double YoungModulus,
        const double PoissonCoefficient
        ) override;

    /**
     * @brief It calculates the strain vector
     * @param rValues The Internalvalues of the law
     * @param rStrainVector The strain vector in Voigt notation
     */
    void CalculateGreenLagrangianStrain(
        ConstitutiveLaw::Parameters& rValues,
        Vector& rStrainVector
        ) override;

    /**
     * @brief Calculates the Almansi strains
     * @param rValues The Internalvalues of the law
     * @param rStrainVector The strain vector in Voigt notation
     */
    void CalculateAlmansiStrain(
        ConstitutiveLaw::Parameters& rValues,
        Vector& rStrainVector
        ) override;

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

    ///@}
    ///@name Private  Access
    ///@{
    ///@}

    ///@}
    ///@name Serialization
    ///@{
    friend class Serializer;

    void save(Serializer& rSerializer) const override
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, BaseType )
    }

    void load(Serializer& rSerializer) override
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, BaseType)
    }


}; // Class HyperElasticIsotropicKirchhoffPlaneStrain2D
}  // namespace Kratos.
#endif // KRATOS_HYPER_ELASTIC_ISOTROPIC_KIRCHHOFF_PLANE_STRAIN_2D_LAW_H_INCLUDED  defined
