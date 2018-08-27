/*---------------------------------------------------------------------------*/
/* IMessagePassingMng.h                                        (C) 2000-2018 */
/*                                                                           */
/* Interface du gestionnaire des �changes de messages.                       */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_MESSAGEPASSING_IMESSAGEPASSINGMNG_H
#define ARCCORE_MESSAGEPASSING_IMESSAGEPASSINGMNG_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/messagepassing/MessagePassingGlobal.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{
namespace MessagePassing
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface du gestionnaire des �changes de messages.
 */
class ARCCORE_MESSAGEPASSING_EXPORT IMessagePassingMng
{
 public:

  virtual ~IMessagePassingMng(){}

 public:

  //! Rang de cette instance dans le communicateur
  virtual Int32 commRank() const =0;

  //! Nombre d'instance dans le communicateur
  virtual Int32 commSize() const =0;

 public:

  virtual IDispatchers* dispatchers() =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace MessagePassing
} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
