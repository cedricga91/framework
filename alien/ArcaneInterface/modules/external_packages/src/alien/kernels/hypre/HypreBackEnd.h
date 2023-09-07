// -*- C++ -*-
#ifndef ALIEN_KERNELS_HYPRE_HYPREBACKEND_H
#define ALIEN_KERNELS_HYPRE_HYPREBACKEND_H
/* Author : havep at Fri Jul 20 17:03:16 2012
 * Generated by createNew
 */

#include <alien/core/backend/BackEnd.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore::MessagePassing {
class IMessagePassingMng;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IOptionsHypreSolver;

namespace Alien {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class MultiVectorImpl;
class HypreMatrix;
class HypreVector;
class Space;
template <class Matrix, class Vector> class IInternalLinearAlgebra;
template <class Matrix, class Vector> class IInternalLinearSolver;

extern IInternalLinearAlgebra<HypreMatrix, HypreVector>*
HypreInternalLinearAlgebraFactory();

extern IInternalLinearSolver<HypreMatrix, HypreVector>* HypreInternalLinearSolverFactory(
    Arccore::MessagePassing::IMessagePassingMng* p_mng, IOptionsHypreSolver* options);

/*---------------------------------------------------------------------------*/

namespace BackEnd {
  namespace tag {
    struct hypre
    {
    };
  } // namespace tag
}

template <> struct AlgebraTraits<BackEnd::tag::hypre>
{
  typedef HypreMatrix matrix_type;
  typedef HypreVector vector_type;
  typedef IOptionsHypreSolver options_type;
  typedef IInternalLinearAlgebra<matrix_type, vector_type> algebra_type;
  typedef IInternalLinearSolver<matrix_type, vector_type> solver_type;
  static algebra_type* algebra_factory(
      Arccore::MessagePassing::IMessagePassingMng* p_mng = nullptr)
  {
    return HypreInternalLinearAlgebraFactory();
  }
  static solver_type* solver_factory(
      Arccore::MessagePassing::IMessagePassingMng* p_mng, options_type* options)
  {
    return HypreInternalLinearSolverFactory(p_mng, options);
  }
  static BackEndId name() { return "hypre"; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Alien

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ALIEN_KERNELS_HYPRE_HYPREBACKEND_H */