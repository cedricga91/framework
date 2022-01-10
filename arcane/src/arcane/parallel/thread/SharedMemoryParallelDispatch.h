﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* SharedMemoryParallelDispatch.h                              (C) 2000-2020 */
/*                                                                           */
/* Implémentation des messages en mémoire partagée.                          */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_PARALLEL_THREAD_SHAREDMEMORYPARALLELDISPATCH_H
#define ARCANE_PARALLEL_THREAD_SHAREDMEMORYPARALLELDISPATCH_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/TraceAccessor.h"
#include "arcane/utils/Array.h"
#include "arcane/utils/IThreadBarrier.h"
#include "arcane/utils/HPReal.h"
#include "arcane/utils/DataTypeContainer.h"

#include "arcane/IParallelDispatch.h"
#include "arcane/ISerializer.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::MessagePassing
{

class SharedMemoryParallelMng;
class ISharedMemoryMessageQueue;
template<class Type>
class SharedMemoryParallelDispatch;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename DataType>
class ThreadDispatcherContainerTraits
{
 public:
  typedef UniqueArray<SharedMemoryParallelDispatch<DataType>*> InstanceType;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class SharedMemoryAllDispatcher
: public ArcaneDataTypeContainer<ThreadDispatcherContainerTraits>
{
 public:
  //! Informations nécessaires pour créer un sous-parallelMng().
  class CreateSubParallelMngInfo
  {
   public:
    Ref<IParallelMngContainer> m_builder;
  };
 public:
  void resize(Integer n)
  {
    m_char.resize(n);
    m_signed_char.resize(n);
    m_unsigned_char.resize(n);
    m_short.resize(n);
    m_unsigned_short.resize(n);
    m_int.resize(n);
    m_unsigned_int.resize(n);
    m_long.resize(n);
    m_unsigned_long.resize(n);
    m_long_long.resize(n);
    m_unsigned_long_long.resize(n);
    m_float.resize(n);
    m_double.resize(n);
    m_long_double.resize(n);
    m_apreal.resize(n);
    m_real2.resize(n);
    m_real3.resize(n);
    m_real2x2.resize(n);
    m_real3x3.resize(n);
    m_hpreal.resize(n);
  }
 public:
  CreateSubParallelMngInfo m_create_sub_parallel_mng_info;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface des messages pour le type \a Type
 */
template<class Type>
class SharedMemoryParallelDispatch
: public TraceAccessor
, public IParallelDispatchT<Type>
, public ITypeDispatcher<Type>
{
  class AllToAllVariableInfo
  {
   public:
    Span<const Type> send_buf;
    Int32ConstArrayView send_count;
    Int32ConstArrayView send_index;
    Span<Type> recv_buf;
    Int32ConstArrayView recv_count;
    Int32ConstArrayView recv_index;
  };

  class ReduceInfo
  {
   public:
    Span<Type> reduce_buf;
    Type reduce_value;
    int m_index;
  };

 private:
  class MinMaxSumInfo
  {
   public:
    Int32 m_min_rank;
    Int32 m_max_rank;
    Type m_min_value;
    Type m_max_value;
    Type m_sum_value;
  };
  
 public:
  using Request = Parallel::Request;
  using PointToPointMessageInfo = Parallel::PointToPointMessageInfo;
  using MessageRank = Parallel::MessageRank;
  using MessageTag = Parallel::MessageTag;
  typedef Parallel::eReduceType eReduceType;
 public:
  SharedMemoryParallelDispatch(ITraceMng* tm,SharedMemoryParallelMng* parallel_mng,
                               ISharedMemoryMessageQueue* message_queue,
                               ArrayView< SharedMemoryParallelDispatch<Type>* > all_dispatchs
                          );
  ~SharedMemoryParallelDispatch() override;
  void finalize() override;
 public:
  //@{ // From MessagePassing
  void broadcast(Span<Type> send_buf,Int32 sub_domain) override;
  void allGather(Span<const Type> send_buf,Span<Type> recv_buf) override;
  void allGatherVariable(Span<const Type> send_buf,Array<Type>& recv_buf) override;
  void gather(Span<const Type> send_buf,Span<Type> recv_buf,Int32 rank) override;
  void gatherVariable(Span<const Type> send_buf,Array<Type>& recv_buf,Int32 rank) override;
  void scatterVariable(Span<const Type> send_buf,Span<Type> recv_buf,Int32 root) override;
  void allReduce(eReduceType op,Span<Type> send_buf) override;
  void allToAll(Span<const Type> send_buf,Span<Type> recv_buf,Int32 count) override;
  void allToAllVariable(Span<const Type> send_buf,ConstArrayView<Int32> send_count,
                        ConstArrayView<Int32> send_index,Span<Type> recv_buf,
                        ConstArrayView<Int32> recv_count,ConstArrayView<Int32> recv_index) override;
  Request send(Span<const Type> send_buffer,Int32 proc,bool is_blocked) override;
  Request send(Span<const Type> send_buffer,const PointToPointMessageInfo& message) override;
  Request receive(Span<Type> recv_buffer,Int32 rank,bool is_blocked) override;
  Request receive(Span<Type> recv_buffer,const PointToPointMessageInfo& message) override;
  Request nonBlockingAllReduce(eReduceType op,Span<Type> send_buf); //!< A supprimer
  Request nonBlockingAllReduce(eReduceType op,Span<const Type> send_buf,Span<Type> recv_buf);
  Request nonBlockingAllGather(Span<const Type> send_buf, Span<Type> recv_buf);
  Request nonBlockingBroadcast(Span<Type> send_buf, Int32 rank);
  Request nonBlockingGather(Span<const Type> send_buf, Span<Type> recv_buf, Int32 rank);
  Request nonBlockingAllToAll(Span<const Type> send_buf, Span<Type> recv_buf, Int32 count);
  Request nonBlockingAllToAllVariable(Span<const Type> send_buf, ConstArrayView<Int32> send_count,
                                      ConstArrayView<Int32> send_index, Span<Type> recv_buf,
                                      ConstArrayView<Int32> recv_count, ConstArrayView<Int32> recv_index);
  //@}

  void broadcast(ArrayView<Type> send_buf,Integer sub_domain) override
  { this->broadcast(Span<Type>(send_buf),sub_domain); }
  void allGather(ConstArrayView<Type> send_buf,ArrayView<Type> recv_buf) override
  { this->allGather(Span<const Type>(send_buf),Span<Type>(recv_buf)); }
  void allGatherVariable(ConstArrayView<Type> send_buf,Array<Type>& recv_buf) override
  { this->allGatherVariable(Span<const Type>(send_buf),recv_buf); }
  void gather(ConstArrayView<Type> send_buf,ArrayView<Type> recv_buf,Integer rank) override
  { this->gather(Span<const Type>(send_buf),Span<Type>(recv_buf),rank); }
  void gatherVariable(ConstArrayView<Type> send_buf,Array<Type>& recv_buf,Integer rank) override
  { this->gatherVariable(Span<const Type>(send_buf),recv_buf,rank); }
  void scatterVariable(ConstArrayView<Type> send_buf,ArrayView<Type> recv_buf,Integer root) override
  { this->scatterVariable(Span<const Type>(send_buf),Span<Type>(recv_buf),root); }
  void allToAll(ConstArrayView<Type> send_buf,ArrayView<Type> recv_buf,Integer count) override
  { this->allToAll(Span<const Type>(send_buf),Span<Type>(recv_buf),count); }
  void allToAllVariable(ConstArrayView<Type> send_buf,Int32ConstArrayView send_count,
                        Int32ConstArrayView send_index,ArrayView<Type> recv_buf,
                        Int32ConstArrayView recv_count,Int32ConstArrayView recv_index) override
  { this->allToAllVariable(Span<const Type>(send_buf),send_count,send_index,
                           Span<Type>(recv_buf),recv_count,recv_index); }
  Request send(ConstArrayView<Type> send_buffer,Integer proc,bool is_blocked) override
  { return this->send(Span<const Type>(send_buffer),proc,is_blocked); }
  Request recv(ArrayView<Type> recv_buffer,Int32 rank,bool is_blocked) override
  { return this->receive(Span<Type>(recv_buffer),rank,is_blocked); }
  void allReduce(eReduceType op,ArrayView<Type> send_buf) override
  { return this->allReduce(op,Span<Type>(send_buf)); }

  void send(ConstArrayView<Type> send_buffer,Integer proc) override;
  void recv(ArrayView<Type> recv_buffer,Integer proc) override;
  void sendRecv(ConstArrayView<Type> send_buffer,ArrayView<Type> recv_buffer,Integer proc) override;
  Type allReduce(eReduceType op,Type send_buf) override;
  Type scan(eReduceType op,Type send_buf) override;
  void scan(eReduceType op,ArrayView<Type> send_buf) override;
  void computeMinMaxSum(Type val,Type& min_val,Type& max_val,Type& sum_val,
                                Int32& min_rank,
                                Int32& max_rank) override;
  void computeMinMaxSum(ConstArrayView<Type> values,
                        ArrayView<Type> min_values,
                        ArrayView<Type> max_values,
                        ArrayView<Type> sum_values,
                        ArrayView<Int32> min_ranks,
                        ArrayView<Int32> max_ranks) override;
  ITypeDispatcher<Type>* toArccoreDispatcher() override { return this; }

  void waitAll();

 private:
  SharedMemoryParallelMng* m_parallel_mng;
  Int32 m_rank;
  Int32 m_nb_rank;
 public:
  ArrayView< SharedMemoryParallelDispatch<Type>* > m_all_dispatchs;
 private:
  Span<Type> m_broadcast_view;
  Span<const Type> m_const_view;
  Span<Type> m_recv_view;
  Span<const Type> m_send_view;
  AllToAllVariableInfo m_alltoallv_infos;
 public:
  ReduceInfo m_reduce_infos;
 private:
  ISharedMemoryMessageQueue* m_message_queue;
  void _collectiveBarrier();
 private:
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane::MessagePassing

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

