#include "HypreMatrix.h"
/* Author : havep at Wed Jul 18 14:46:45 2012
 * Generated by createNew
 */
#include <alien/kernels/hypre/data_structure/HypreVector.h>
#include <alien/kernels/hypre/HypreBackEnd.h>
#include <alien/kernels/hypre/data_structure/HypreInternal.h>
#include <alien/core/impl/MultiMatrixImpl.h>
#include <alien/data/ISpace.h>

#include <arccore/message_passing_mpi/MpiMessagePassingMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Alien {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

HypreMatrix::HypreMatrix(const MultiMatrixImpl* multi_impl)
: IMatrixImpl(multi_impl, AlgebraTraits<BackEnd::tag::hypre>::name())
, m_internal(nullptr)
, m_pm(nullptr)
{
  const auto& row_space = multi_impl->rowSpace();
  const auto& col_space = multi_impl->colSpace();
  if (row_space.size() != col_space.size())
    throw Arccore::FatalErrorException("Hypre matrix must be square");
  m_pm = multi_impl->distribution().parallelMng();
}

/*---------------------------------------------------------------------------*/

HypreMatrix::~HypreMatrix()
{
  delete m_internal;
}

/*---------------------------------------------------------------------------*/

bool
HypreMatrix::initMatrix(const int ilower, const int iupper, const int jlower,
    const int jupper, const Arccore::ConstArrayView<Arccore::Integer>& lineSizes)
{
  delete m_internal;
  auto* pm = dynamic_cast<Arccore::MessagePassing::Mpi::MpiMessagePassingMng*>(m_pm);
  if (*static_cast<const MPI_Comm*>(pm->getMPIComm()) != MPI_COMM_NULL)
    m_internal = new MatrixInternal(*static_cast<const MPI_Comm*>(pm->getMPIComm()));
  else
    m_internal = new MatrixInternal(MPI_COMM_WORLD);
  return m_internal->init(ilower, iupper, jlower, jupper, lineSizes);
}

/*---------------------------------------------------------------------------*/

bool
HypreMatrix::addMatrixValues(const int nrow, const int* rows, const int* ncols,
    const int* cols, const Arccore::Real* values)
{
  return m_internal->addMatrixValues(nrow, rows, ncols, cols, values);
}

/*---------------------------------------------------------------------------*/

bool
HypreMatrix::setMatrixValues(const int nrow, const int* rows, const int* ncols,
    const int* cols, const Arccore::Real* values)
{
  return m_internal->setMatrixValues(nrow, rows, ncols, cols, values);
}

/*---------------------------------------------------------------------------*/

bool
HypreMatrix::assemble()
{
  return m_internal->assemble();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Alien

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/