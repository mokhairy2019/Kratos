//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics PfemFluidDynamics Application
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Alejandro Cornejo Velazquez
//                   Carlos Eulogio Flores
//

#if !defined(KRATOS_FIX_FREE_VELOCITY_ON_NODES_PROCESS)
#define KRATOS_FIX_FREE_VELOCITY_ON_NODES_PROCESS

#include "processes/process.h"
#include "pfem_fluid_dynamics_application_variables.h"
#include "includes/variables.h"

namespace Kratos {

typedef std::size_t SizeType;
typedef Node<3> NodeType;
typedef ModelPart::NodesContainerType::iterator NodeIteratorType;

class FixFreeVelocityOnNodesProcess : public Process 
{
 public:

  /// Pointer definition of FixFreeVelocityOnNodesProcess
  KRATOS_CLASS_POINTER_DEFINITION(FixFreeVelocityOnNodesProcess);

  typedef std::size_t IndexType;

  // Constructor
  FixFreeVelocityOnNodesProcess(ModelPart& rModelPart, const bool rFreeOrFix);

  // Destructor
  ~FixFreeVelocityOnNodesProcess() override = default;

  void operator()() { Execute(); }

  void Execute() override;

protected:

  // Member Variables
  ModelPart& mrModelPart;
  int mFreeOrFix;

};  // Class

}  // namespace Kratos
#endif /* KRATOS_FIX_FREE_VELOCITY_ON_NODES_PROCESS defined */
