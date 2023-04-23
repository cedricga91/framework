﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* IParallelMngInternal.h                                      (C) 2000-2023 */
/*                                                                           */
/* Partie interne à Arcane de IParallelMng.                                  */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_CORE_INTERNAL_IPARALLELMNGINTERNAL_H
#define ARCANE_CORE_INTERNAL_IPARALLELMNGINTERNAL_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ArcaneTypes.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Partie interne de IParallelMng.
 */
class ARCANE_CORE_EXPORT IParallelMngInternal
{
 public:

  virtual ~IParallelMngInternal() = default;

 public:

  //! Runner par défaut. Peut être nul
  virtual Runner* defaultRunner() const = 0;

  //! File par défaut pour les messages. Peut être nul
  virtual RunQueue* defaultQueue() const = 0;

  /*!
   * \brief Indique si l'implémentation gère les accélérateurs.
   *
   * Si c'est le cas on peut utiliser directement la mémoire de l'accélérateur
   * dans les appels MPI ce qui permet d'éviter d'éventuelles recopies.
   */
  virtual bool isAcceleratorAware() const = 0;

 public:

  virtual void setDefaultRunner(Runner* runner) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
