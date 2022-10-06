// System includes
#include <string>
#include <iostream>
#include <cmath>

// Project includes
#include "DEM_KDEM_with_damage_parallel_bond_Hertz_2D_CL.h"
#include "custom_elements/spheric_continuum_particle.h"

namespace Kratos {

    DEMContinuumConstitutiveLaw::Pointer DEM_KDEM_with_damage_parallel_bond_Hertz_2D::Clone() const {
        DEMContinuumConstitutiveLaw::Pointer p_clone(new DEM_KDEM_with_damage_parallel_bond_Hertz_2D(*this));
        return p_clone;
    }

    void DEM_KDEM_with_damage_parallel_bond_Hertz_2D::SetConstitutiveLawInProperties(Properties::Pointer pProp, bool verbose) {
        KRATOS_INFO("DEM") << "Assigning DEM_KDEM_with_damage_parallel_bond_Hertz_2D to Properties " << pProp->Id() << std::endl;
        pProp->SetValue(DEM_CONTINUUM_CONSTITUTIVE_LAW_POINTER, this->Clone());
        this->Check(pProp);
    }

    void DEM_KDEM_with_damage_parallel_bond_Hertz_2D::SetConstitutiveLawInPropertiesWithParameters(Properties::Pointer pProp, const Parameters& parameters, bool verbose) {
        KRATOS_INFO("DEM") << "Assigning DEM_KDEM_with_damage_parallel_bond_Hertz_2D to Properties " << pProp->Id() <<" with given parameters"<< std::endl;
        pProp->SetValue(DEM_CONTINUUM_CONSTITUTIVE_LAW_POINTER, this->Clone());
        TransferParametersToProperties(parameters, pProp);
        this->Check(pProp);
    }

    void DEM_KDEM_with_damage_parallel_bond_Hertz_2D::CalculateContactArea(double radius, double other_radius, double& calculation_area) {

        KRATOS_TRY
        double radius_sum = radius + other_radius;
        double equiv_radius = radius * other_radius / radius_sum; //double equiv_radius = 0.5 * radius_sum;
        calculation_area = 2.0 * equiv_radius * 1.0; // This last 1.0 is simply the unitary thickness of 2D, in meters.
        KRATOS_CATCH("")
    }

    void DEM_KDEM_with_damage_parallel_bond_Hertz_2D::CalculateElasticConstants(double& kn_el, double& kt_el, double initial_dist, double equiv_young,
                                             double equiv_poisson, double calculation_area, SphericContinuumParticle* element1, SphericContinuumParticle* element2, double indentation) {

        KRATOS_TRY

        //Get equivalent Young's Modulus
        const double my_young        = element1->GetYoung();
        const double other_young     = element2->GetYoung();
        const double my_poisson      = element1->GetPoisson();
        const double other_poisson   = element2->GetPoisson();
        const double equiv_young     = my_young * other_young / (other_young * (1.0 - my_poisson * my_poisson) + my_young * (1.0 - other_poisson * other_poisson));

        //Normal and Tangent elastic constants
        mUnbondedNormalElasticConstant = 0.25 * Globals::Pi * equiv_young; // Here length is 1.0m
        mUnbondedTangentialElasticConstant = mUnbondedNormalElasticConstant * (1.0 - equiv_poisson) / (1.0 - 0.5 * equiv_poisson);

        const double my_mass    = element1->GetMass();
        const double other_mass = element2->GetMass();
        const double equiv_mass = 1.0 / (1.0 / my_mass + 1.0 / other_mass);
        const double& equiv_gamma = (*mpProperties)[DAMPING_GAMMA];

        mUnbondedEquivViscoDampCoeffNormal     = 2.0 * equiv_gamma * sqrt(equiv_mass * mUnbondedNormalElasticConstant);
        mUnbondedEquivViscoDampCoeffTangential = 2.0 * equiv_gamma * sqrt(equiv_mass * mUnbondedTangentialElasticConstant);

        const double bonded_equiv_young = (*mpProperties)[BONDED_MATERIAL_YOUNG_MODULUS];
        const double bonded_equiv_shear = bonded_equiv_young / (2.0 * (1 + equiv_poisson));

        kn_el = bonded_equiv_young * calculation_area / initial_dist;
        kt_el = bonded_equiv_shear * calculation_area / initial_dist;

        KRATOS_CATCH("")
    }

    void DEM_KDEM_with_damage_parallel_bond_Hertz_2D::ComputeNormalUnbondedForce(double indentation) {

        KRATOS_TRY

        if (indentation > 0.0) {
            mUnbondedLocalElasticContactForce2 = mUnbondedNormalElasticConstant * indentation;
        } else {
            mUnbondedLocalElasticContactForce2 = 0.0;
        }
        KRATOS_CATCH("")
    }

} // namespace Kratos
