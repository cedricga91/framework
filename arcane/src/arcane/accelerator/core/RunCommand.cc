﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* RunCommand.cc                                               (C) 2000-2022 */
/*                                                                           */
/* Gestion d'une commande sur accélérateur.                                  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/accelerator/core/RunCommand.h"

#include "arcane/utils/PlatformUtils.h"
#include "arcane/utils/FatalErrorException.h"
#include "arcane/utils/IMemoryAllocator.h"
#include "arcane/utils/CheckedConvert.h"
#include "arcane/utils/NumArray.h"

#include "arcane/accelerator/core/RunQueueImpl.h"
#include "arcane/accelerator/core/RunQueue.h"
#include "arcane/accelerator/core/IReduceMemoryImpl.h"
#include "arcane/accelerator/core/Runner.h"
#include "arcane/accelerator/core/Memory.h"
#include "arcane/accelerator/core/IRunQueueStream.h"

#include <set>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \file RunCommandLoop.h
 *
 * \brief Types et macros pour gérer les boucles sur les accélérateurs
 */

/*!
 * \file RunCommandEnumerate.h
 *
 * \brief Types et macros pour gérer les énumérations des entités sur les accélérateurs
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Accelerator::impl
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ReduceMemoryImpl
: public IReduceMemoryImpl
{
 public:

  ReduceMemoryImpl(RunCommandImpl* p)
  : m_command(p) //, m_grid_buffer(eMemoryRessource::Device)
  {
    // TODO: prendre en compte la politique d'exécution pour savoir
    // comment allouer.
    m_allocator = platform::getAcceleratorHostMemoryAllocator();
    if (!m_allocator)
      ARCANE_FATAL("No HostMemory allocator available for accelerator");
    _allocateMemoryForReduceData(128);
    m_grid_memory_info.m_grid_device_count = reinterpret_cast<unsigned int*>(m_allocator->allocate(sizeof(int)));
    (*m_grid_memory_info.m_grid_device_count) = 0;
  }
  ~ReduceMemoryImpl()
  {
    m_allocator->deallocate(m_managed_memory);
    m_allocator->deallocate(m_grid_memory_info.m_grid_device_count);
  }

 public:

  void* allocateReduceDataMemory(SmallSpan<const std::byte> identity_span) override;
  void setGridSizeAndAllocate(Int32 grid_size) override
  {
    m_grid_size = grid_size;
    _setReducePolicy();
    _allocateGridDataMemory();
  }
  Int32 gridSize() const { return m_grid_size; }

  GridMemoryInfo gridMemoryInfo() override
  {
    return m_grid_memory_info;
  }
  void release() override;

 private:

  RunCommandImpl* m_command;
  IMemoryAllocator* m_allocator;

  //! Pointeur vers la mémoire unifiée contenant la donnée réduite
  std::byte* m_managed_memory = nullptr;

  //! Taille allouée pour \a m_managed_memory
  Int64 m_size = 0;

  //! Taille courante de la grille (nombre de blocs)
  Int32 m_grid_size = 0;

  //! Taille de la donnée actuelle
  Int64 m_data_type_size = 0;

  GridMemoryInfo m_grid_memory_info;

  NumArray<Byte,1> m_grid_buffer;

 private:

  void _allocateGridDataMemory()
  {
    // TODO: pouvoir utiliser un padding pour éviter que les lignes de cache
    // entre les blocs se chevauchent
    Int32 total_size = CheckedConvert::toInt32(m_data_type_size * m_grid_size);
    if (total_size > m_grid_memory_info.m_grid_memory_size) {
      m_grid_buffer.resize(total_size);
      m_grid_memory_info.m_grid_memory_value_as_bytes = reinterpret_cast<std::byte*>(m_grid_buffer.to1DSpan().data());
      m_grid_memory_info.m_grid_memory_size = total_size;
      //std::cout << "RESIZE GRID t=" << total_size << "\n";
    }
  }
  void _setReducePolicy();
  void _allocateMemoryForReduceData(Int32 new_size)
  {
    m_managed_memory = reinterpret_cast<std::byte*>(m_allocator->allocate(new_size));
    m_size = new_size;
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Implémentation d'une commande pour accélérateur.
 * \warning API en cours de définition.
 */
class RunCommandImpl
{
  friend class RunCommand;

 public:

  RunCommandImpl(RunQueueImpl* queue);
  ~RunCommandImpl();
  RunCommandImpl(const RunCommandImpl&) = delete;
  RunCommandImpl& operator=(const RunCommandImpl&) = delete;

 public:

  static RunCommandImpl* create(RunQueueImpl* r);

 public:

  void release();
  const TraceInfo& traceInfo() const { return m_trace_info; }
  const String& kernelName() const { return m_kernel_name; }

 public:

  void reset();
  impl::IReduceMemoryImpl* getOrCreateReduceMemoryImpl()
  {
    ReduceMemoryImpl* p = _getOrCreateReduceMemoryImpl();
    if (p) {
      m_active_reduce_memory_list.insert(p);
    }
    return p;
  }

  void releaseReduceMemoryImpl(ReduceMemoryImpl* p)
  {
    auto x = m_active_reduce_memory_list.find(p);
    if (x == m_active_reduce_memory_list.end())
      ARCANE_FATAL("ReduceMemoryImpl in not in active list");
    m_active_reduce_memory_list.erase(x);
    m_reduce_memory_pool.push(p);
  }

  IRunQueueStream* internalStream() const { return m_queue->_internalStream(); }

 private:

  ReduceMemoryImpl* _getOrCreateReduceMemoryImpl()
  {
    // Pas besoin d'allouer de la mémoire spécifique si on n'est pas
    // sur un accélérateur
    if (!impl::isAcceleratorPolicy(m_queue->executionPolicy()))
      return nullptr;

    auto& pool = m_reduce_memory_pool;

    if (!pool.empty()) {
      ReduceMemoryImpl* p = pool.top();
      pool.pop();
      return p;
    }
    return new ReduceMemoryImpl(this);
  }

 public:

  RunQueueImpl* m_queue;
  TraceInfo m_trace_info;
  String m_kernel_name;
  Int32 m_nb_thread_per_block = 0;

  // NOTE: cette pile gère la mémoire associé à un seul runtime
  // Si on souhaite un jour supporté plusieurs runtimes il faudra une pile
  // par runtime. On peut éventuellement limiter cela si on est sur
  // qu'une commande est associée à un seul type (au sens runtime) de RunQueue.
  std::stack<ReduceMemoryImpl*> m_reduce_memory_pool;

  //! Liste des réductions actives
  std::set<ReduceMemoryImpl*> m_active_reduce_memory_list;

 private:

  void _freePools();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommandImpl::
RunCommandImpl(RunQueueImpl* queue)
: m_queue(queue)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommandImpl::
~RunCommandImpl()
{
  _freePools();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommandImpl::
_freePools()
{
  while (!m_reduce_memory_pool.empty()) {
    delete m_reduce_memory_pool.top();
    m_reduce_memory_pool.pop();
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommandImpl::
release()
{
  m_queue->_internalFreeRunCommandImpl(this);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommandImpl* RunCommandImpl::
create(RunQueueImpl* r)
{
  return r->_internalCreateOrGetRunCommandImpl();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommandImpl::
reset()
{
  m_kernel_name = String();
  m_trace_info = TraceInfo();
  m_nb_thread_per_block = 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void ReduceMemoryImpl::
release()
{
  m_command->releaseReduceMemoryImpl(this);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void ReduceMemoryImpl::
_setReducePolicy()
{
  m_grid_memory_info.m_reduce_policy = m_command->m_queue->runner()->deviceReducePolicy();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* ReduceMemoryImpl::
allocateReduceDataMemory(SmallSpan<const std::byte> identity_span)
{
  Int32 data_type_size = identity_span.size();
  m_data_type_size = data_type_size;
  if (data_type_size > m_size)
    _allocateMemoryForReduceData(data_type_size);
  MemoryCopyArgs copy_args(m_managed_memory,identity_span.data(),data_type_size);
  m_command->internalStream()->copyMemory(copy_args.addAsync());
  // TODO: utiliser un 'memcpy' ou 'prefetch' asynchrone sur la file associée à la commande
  return m_managed_memory;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane::Accelerator::impl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Accelerator
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommand::
RunCommand(RunQueue& run_queue)
: m_run_queue(run_queue)
, m_p(run_queue._getCommandImpl())
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommand::
~RunCommand()
{
  m_p->release();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommand::
_resetInfos()
{
  m_p->reset();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const TraceInfo& RunCommand::
traceInfo() const
{
  return m_p->traceInfo();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const String& RunCommand::
kernelName() const
{
  return m_p->kernelName();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32 RunCommand::
nbThreadPerBlock() const
{
  return m_p->m_nb_thread_per_block;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommand& RunCommand::
addTraceInfo(const TraceInfo& ti)
{
  m_p->m_trace_info = ti;
  return *this;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommand& RunCommand::
addKernelName(const String& v)
{
  m_p->m_kernel_name = v;
  return *this;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

RunCommand& RunCommand::
addNbThreadPerBlock(Int32 v)
{
  if (v<0)
    v = 0;
  if (v>0 && v<32)
    v = 32;
  m_p->m_nb_thread_per_block = v;
  return *this;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" ARCANE_ACCELERATOR_CORE_EXPORT
RunCommand& operator<<(RunCommand& command,const TraceInfo& trace_info)
{
  return command.addTraceInfo(trace_info);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

impl::RunCommandImpl* RunCommand::
_internalCreateImpl(impl::RunQueueImpl* queue)
{
  return new impl::RunCommandImpl(queue);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommand::
_internalDestroyImpl(impl::RunCommandImpl* p)
{
  delete p;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void RunCommand::
_allocateReduceMemory(Int32 nb_grid)
{
  auto& mem_list = m_p->m_active_reduce_memory_list;
  if (!mem_list.empty()) {
    for (auto& x : mem_list)
      x->setGridSizeAndAllocate(nb_grid);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace impl
{
extern "C++" IReduceMemoryImpl*
internalGetOrCreateReduceMemoryImpl(RunCommand* command)
{
  return command->m_p->getOrCreateReduceMemoryImpl();
}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
