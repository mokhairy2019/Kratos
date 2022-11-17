//-------------------------------------------------------------
//         ___  __           ___ _      _    _
//  KRATOS| _ \/ _|___ _ __ | __| |_  _(_)__| |
//        |  _/  _/ -_) '  \| _|| | || | / _` |
//        |_| |_| \___|_|_|_|_| |_|\_,_|_\__,_|DYNAMICS
//
//  BSD License:    PfemFluidDynamicsApplication/license.txt
//
//  Main authors:   Alessandro Franci, Ignasi de Pouplana
//  Collaborators:  Timur Tomas
//
//-------------------------------------------------------------
//

// System includes
#include <iostream>

// External includes
#include <cmath>

// Project includes
#include "custom_constitutive/fluid_laws/herschel_bulkley_3D_law.h"
#include "includes/checks.h"
#include "includes/properties.h"
#include "pfem_fluid_dynamics_application_variables.h"

namespace Kratos
{

    //********************************CONSTRUCTOR*********************************
    //****************************************************************************

    HerschelBulkley3DLaw::HerschelBulkley3DLaw() : PfemFluidConstitutiveLaw() {}

    //******************************COPY CONSTRUCTOR******************************
    //****************************************************************************

    HerschelBulkley3DLaw::HerschelBulkley3DLaw(const HerschelBulkley3DLaw &rOther) : PfemFluidConstitutiveLaw(rOther) {}

    //***********************************CLONE************************************
    //****************************************************************************

    ConstitutiveLaw::Pointer HerschelBulkley3DLaw::Clone() const { return Kratos::make_shared<HerschelBulkley3DLaw>(*this); }

    //*********************************DESTRUCTOR*********************************
    //****************************************************************************

    HerschelBulkley3DLaw::~HerschelBulkley3DLaw() {}

    ConstitutiveLaw::SizeType HerschelBulkley3DLaw::WorkingSpaceDimension() { return 3; }

    ConstitutiveLaw::SizeType HerschelBulkley3DLaw::GetStrainSize() { return 6; }

    void HerschelBulkley3DLaw::CalculateMaterialResponseCauchy(Parameters &rValues)
    {

        Flags &r_options = rValues.GetOptions();

        const Properties &r_properties = rValues.GetMaterialProperties();

        Vector &r_strain_vector = rValues.GetStrainVector();
        Vector &r_stress_vector = rValues.GetStressVector();

        const double dynamic_viscosity = this->GetEffectiveMaterialParameter(rValues, DYNAMIC_VISCOSITY);
        const double yield_shear = this->GetEffectiveMaterialParameter(rValues, YIELD_SHEAR);
        const double adaptive_exponent = r_properties[ADAPTIVE_EXPONENT];
        double effective_dynamic_viscosity;
        const double flow_index = this->GetFlowIndex(rValues);

        const double equivalent_strain_rate =
            std::sqrt(2.0 * r_strain_vector[0] * r_strain_vector[0] + 2.0 * r_strain_vector[1] * r_strain_vector[1] +
                      2.0 * r_strain_vector[2] * r_strain_vector[2] + 4.0 * r_strain_vector[3] * r_strain_vector[3] +
                      4.0 * r_strain_vector[4] * r_strain_vector[4] + 4.0 * r_strain_vector[5] * r_strain_vector[5]);

        // Ensuring that the case of equivalent_strain_rate = 0 is not problematic
        const double tolerance = 1e-8;
        if (equivalent_strain_rate < tolerance)
        {
            effective_dynamic_viscosity = yield_shear * adaptive_exponent;
        }
        else
        {
            double regularization = 1.0 - std::exp(-adaptive_exponent * equivalent_strain_rate);
            effective_dynamic_viscosity = dynamic_viscosity * pow(equivalent_strain_rate, flow_index - 1) + regularization * yield_shear / equivalent_strain_rate;
        }

        const double strain_trace = r_strain_vector[0] + r_strain_vector[1] + r_strain_vector[2];

        r_stress_vector[0] = 2.0 * effective_dynamic_viscosity * (r_strain_vector[0] - strain_trace / 3.0);
        r_stress_vector[1] = 2.0 * effective_dynamic_viscosity * (r_strain_vector[1] - strain_trace / 3.0);
        r_stress_vector[2] = 2.0 * effective_dynamic_viscosity * (r_strain_vector[2] - strain_trace / 3.0);
        r_stress_vector[3] = 2.0 * effective_dynamic_viscosity * r_strain_vector[3];
        r_stress_vector[4] = 2.0 * effective_dynamic_viscosity * r_strain_vector[4];
        r_stress_vector[5] = 2.0 * effective_dynamic_viscosity * r_strain_vector[5];

        if (r_options.Is(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR))
        {
            this->EffectiveViscousConstitutiveMatrix3D(effective_dynamic_viscosity, rValues.GetConstitutiveMatrix());
        }
    }

    std::string HerschelBulkley3DLaw::Info() const { return "HerschelBulkley3DLaw"; }

    //******************CHECK CONSISTENCY IN THE CONSTITUTIVE LAW******************
    //*****************************************************************************

    int HerschelBulkley3DLaw::Check(const Properties &rMaterialProperties, const GeometryType &rElementGeometry,
                                    const ProcessInfo &rCurrentProcessInfo)
    {

        KRATOS_ERROR_IF(rMaterialProperties[DYNAMIC_VISCOSITY] <= 0.0)
            << "Incorrect or missing DYNAMIC_VISCOSITY provided in process info for HerschelBulkley3DLaw: "
            << rMaterialProperties[DYNAMIC_VISCOSITY] << std::endl;

        KRATOS_ERROR_IF(rMaterialProperties[YIELD_SHEAR] <= 0.0)
            << "Incorrect or missing YIELD_SHEAR provided in process info for HerschelBulkley3DLaw: "
            << rMaterialProperties[YIELD_SHEAR] << std::endl;

        KRATOS_ERROR_IF(rMaterialProperties[FLOW_INDEX] <= 0.0)
            << "Incorrect or missing FLOW_INDEX provided in process info for HerschelBulkley3DLaw: "
            << rMaterialProperties[FLOW_INDEX] << std::endl;

        KRATOS_ERROR_IF(rMaterialProperties[ADAPTIVE_EXPONENT] <= 0.0)
            << "Incorrect or missing ADAPTIVE_EXPONENT provided in process info for HerschelBulkley3DLaw: "
            << rMaterialProperties[ADAPTIVE_EXPONENT] << std::endl;

        KRATOS_ERROR_IF(rMaterialProperties[BULK_MODULUS] <= 0.0)
            << "Incorrect or missing BULK_MODULUS provided in process info for HerschelBulkley3DLaw: "
            << rMaterialProperties[BULK_MODULUS] << std::endl;

        return 0;
    }

    double HerschelBulkley3DLaw::GetApparentViscosity(ConstitutiveLaw::Parameters &rParameters) const
    {
        return rParameters.GetConstitutiveMatrix()(5, 5);
    }

    double HerschelBulkley3DLaw::GetEffectiveMaterialParameter(ConstitutiveLaw::Parameters &rParameters, const Variable<double> &rVariable) const
    {
        return rParameters.GetMaterialProperties()[rVariable];
    }

    double HerschelBulkley3DLaw::GetFlowIndex(ConstitutiveLaw::Parameters &rParameters) const
    {
        return rParameters.GetMaterialProperties()[FLOW_INDEX];
    }

    void HerschelBulkley3DLaw::save(Serializer &rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, PfemFluidConstitutiveLaw)
    }

    void HerschelBulkley3DLaw::load(Serializer &rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, PfemFluidConstitutiveLaw)
    }

} // Namespace Kratos
