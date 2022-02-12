//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics ThermalDEM Application
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Rafael Rangel (rrangel@cimne.upc.edu)
//

#if !defined(NUMERICAL_INTEGRATION_METHOD_ADAPTIVE_SIMPSON_H_INCLUDED)
#define NUMERICAL_INTEGRATION_METHOD_ADAPTIVE_SIMPSON_H_INCLUDED

// System includes
#include <limits>

// External includes
#include "includes/define.h"
#include "includes/model_part.h"

// Project includes
#include "numerical_integration_method.h"

namespace Kratos
{
  class KRATOS_API(THERMAL_DEM_APPLICATION) AdaptiveSimpsonQuadrature : public NumericalIntegrationMethod
  {
    public:

      // Pointer definition of AdaptiveSimpsonQuadrature
      KRATOS_CLASS_POINTER_DEFINITION(AdaptiveSimpsonQuadrature);

      // Constructor
      AdaptiveSimpsonQuadrature();

      // Destructor
      virtual ~AdaptiveSimpsonQuadrature();

      // Clone
      NumericalIntegrationMethod* CloneRaw() const override {
        NumericalIntegrationMethod* cloned_utl(new AdaptiveSimpsonQuadrature(*this));
        return cloned_utl;
      }

      NumericalIntegrationMethod::Pointer CloneShared() const override {
        NumericalIntegrationMethod::Pointer cloned_utl(new AdaptiveSimpsonQuadrature(*this));
        return cloned_utl;
      }

      // Public derived methods
      void   SetNumericalIntegrationMethodInProperties (Properties::Pointer pProp, bool verbose = true) const override;
      double SolveIntegral                             (void) override;
      
    protected:

      // Protected methods
      double RecursiveIntegration (double a, double b, double fa, double fb, double fc);

    private:

      // Assignment operator
      AdaptiveSimpsonQuadrature& operator=(AdaptiveSimpsonQuadrature const& rOther) {
        return *this;
      }

      // Copy constructor
      AdaptiveSimpsonQuadrature(AdaptiveSimpsonQuadrature const& rOther) {
        *this = rOther;
      }

  }; // Class AdaptiveSimpsonQuadrature
} // namespace Kratos

#endif // NUMERICAL_INTEGRATION_METHOD_ADAPTIVE_SIMPSON_H_INCLUDED
