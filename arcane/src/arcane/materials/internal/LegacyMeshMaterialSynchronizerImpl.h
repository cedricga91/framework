﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2024 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* LegacyMeshMaterialSynchronizerImpl.h                                  (C) 2000-2024 */
/*                                                                           */
/* Synchronisation de la liste des matériaux/milieux des entités.            */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_MATERIALS_INTERNAL_MESHMATERIALSYNCHRONIZERIMPL_H
#define ARCANE_MATERIALS_INTERNAL_MESHMATERIALSYNCHRONIZERIMPL_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/TraceAccessor.h"
#include "arcane/utils/ArrayView.h"

#include "arcane/VariableTypedef.h"

#include "arcane/materials/MaterialsGlobal.h"
#include "arcane/materials/MatItem.h"

#include "arcane/materials/internal/IMeshMaterialSynchronizerImpl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Materials
{
class MeshMaterialModifierImpl;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Stratégie de synchronisation de la liste des matériaux/milieux des entités.
 *
 * Cette classe permet de syncrhoniser entre les sous-domaines la liste
 * des matériaux/milieux auxquelles une maille appartient.
 *
 * Les mailles fantômes de ce sous-domaine vont récupérer des mailles propres
 * leur liste des matériaux/milieux. Ces mailles fantômes vont ensuite éventuellement
 * être ajoutés ou retirer des matériaux et milieux actuels pour être en cohérence
 * avec cette liste issue des mailles propres.
 */
class LegacyMeshMaterialSynchronizerImpl
: public TraceAccessor
, public IMeshMaterialSynchronizerImpl
{
 public:

  explicit LegacyMeshMaterialSynchronizerImpl(IMeshMaterialMng* material_mng);
  ~LegacyMeshMaterialSynchronizerImpl();

 public:

  /*!
   * \brief Synchronisation de la liste des matériaux/milieux des entités.
   *
   * Cette opération est collective.
   *
   * Retourne \a true si des mailles ont été ajoutées ou supprimées d'un matériau
   * ou d'un milieu lors de cette opération pour ce sous-domaine.
   */
  bool synchronizeMaterialsInCells();
  void checkMaterialsInCells(Integer max_print);

 private:

  IMeshMaterialMng* m_material_mng;

  inline static void _setBit(ByteArrayView bytes, Integer position);
  inline static bool _hasBit(ByteConstArrayView bytes, Integer position);
  void _fillPresence(AllEnvCell all_env_cell, ByteArrayView presence);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane::Materials

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif