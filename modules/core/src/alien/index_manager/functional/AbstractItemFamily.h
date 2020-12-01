/*
 * Copyright 2020 IFPEN-CEA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <unordered_map>
#include <alien/index_manager/IAbstractFamily.h>
#include "alien/index_manager/IIndexManager.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Alien {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* This module relies on the fact that local_id of the item i is also i.
 */
class ALIEN_EXPORT AbstractItemFamily : public IAbstractFamily
{
 public:
  AbstractItemFamily(const AbstractItemFamily& family);

  /*! Build a family for locally known unique ids.
     *
     * @param uniqueIds Array of locally known uniqueIds.
     * @param owners Array of item owners
     * @param parallel_mng Parallel Manager.
     */
  AbstractItemFamily(const ConstArrayView<Int64> uniqueIds,
                     const ConstArrayView<Integer> owners,
                     IMessagePassingMng* parallel_mng,
                     ITraceMng* trace_mng=nullptr) ;

  /*! Build a family for locally known unique ids.
   *
   * @param uniqueIds Array of locally known uniqueIds.
   * @param ghost_uniqueIds Array of ghost known uniqueIds.
   * @param ghost_owners Array of ghost item owners
   * @param parallel_mng Parallel Manager.
   */
  AbstractItemFamily(const ConstArrayView<Int64>   uniqueIds,
                     const ConstArrayView<Int64>   ghost_uniqueIds,
                     const ConstArrayView<Integer> ghost_owners,
                     IMessagePassingMng* parallel_mng,
                     ITraceMng* trace_mng=nullptr);

  /*! Build a family for locally owned unique ids.
   *
   * @param uniqueIds Array of locally owned uniqueIds.
   * @param parallel_mng Parallel Manager.
   */

  /* FIXME: Comments
   * Local items then others ?
   * Owners array is built in parallel ?
   */
  AbstractItemFamily(const ConstArrayView<Int64> uniqueIds,
                     IMessagePassingMng* parallel_mng,
                     ITraceMng* trace_mng=nullptr);

  virtual ~AbstractItemFamily() {}

 public:
  IAbstractFamily* clone() const { return new AbstractItemFamily(*this); }

 public:
  Int32 maxLocalId() const { return m_unique_ids.size(); }

  void uniqueIdToLocalId(ArrayView<Int32> localIds,
                         ConstArrayView<Int64> uniqueIds) const;

  IAbstractFamily::Item item(Int32 localId) const;

  SafeConstArrayView<Integer> owners(ConstArrayView<Int32> localIds) const;

  SafeConstArrayView<Int64> uids(ConstArrayView<Int32> localIds) const;

  SafeConstArrayView<Int32> allLocalIds() const;

 private:
  IMessagePassingMng* m_parallel_mng = nullptr;
  ITraceMng*          m_trace_mng    = nullptr ;
  UniqueArray<Int64> m_unique_ids;
  UniqueArray<Integer> m_owners;
  std::unordered_map<Int64,Integer> m_uid2lid ;
};

class ALIEN_EXPORT AbstractFamily : public IIndexManager::IAbstractFamily
{
 public:
  AbstractFamily(const AbstractFamily& family);

  /*! Build a family for locally known unique ids.
     *
     * @param uniqueIds Array of locally known uniqueIds.
     * @param owners Array of item owners
     * @param parallel_mng Parallel Manager.
     */
  AbstractFamily(const ConstArrayView<Int64> uniqueIds,
                     const ConstArrayView<Integer> owners,
                     IMessagePassingMng* parallel_mng,
                     ITraceMng* trace_mng=nullptr) ;

  /*! Build a family for locally known unique ids.
   *
   * @param uniqueIds Array of locally known uniqueIds.
   * @param ghost_uniqueIds Array of ghost known uniqueIds.
   * @param ghost_owners Array of ghost item owners
   * @param parallel_mng Parallel Manager.
   */
  AbstractFamily(const ConstArrayView<Int64>   uniqueIds,
                     const ConstArrayView<Int64>   ghost_uniqueIds,
                     const ConstArrayView<Integer> ghost_owners,
                     IMessagePassingMng* parallel_mng,
                     ITraceMng* trace_mng=nullptr);

  /*! Build a family for locally owned unique ids.
   *
   * @param uniqueIds Array of locally owned uniqueIds.
   * @param parallel_mng Parallel Manager.
   */

  /* FIXME: Comments
   * Local items then others ?
   * Owners array is built in parallel ?
   */
  AbstractFamily(const ConstArrayView<Int64> uniqueIds,
                 IMessagePassingMng* parallel_mng,
                 ITraceMng* trace_mng=nullptr);

  virtual ~AbstractFamily() {}

 public:
  IAbstractFamily* clone() const { return new AbstractFamily(*this); }

 public:
  Int32 maxLocalId() const { return m_unique_ids.size(); }

  void uniqueIdToLocalId(ArrayView<Int32> localIds,
                         ConstArrayView<Int64> uniqueIds) const;

  IAbstractFamily::Item item(Int32 localId) const;

  Arccore::SharedArray<Arccore::Integer>  owners(ConstArrayView<Int32> localIds) const;

  Arccore::SharedArray<Arccore::Int64>  uids(ConstArrayView<Int32> localIds) const;

  Arccore::SharedArray<Arccore::Int32> allLocalIds() const;

 private:
  IMessagePassingMng* m_parallel_mng = nullptr;
  ITraceMng*          m_trace_mng    = nullptr ;
  UniqueArray<Int64> m_unique_ids;
  UniqueArray<Integer> m_owners;
  std::unordered_map<Int64,Integer> m_uid2lid ;
};
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Alien

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
