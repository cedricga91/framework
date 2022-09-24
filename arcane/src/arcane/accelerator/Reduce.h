﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* Reduce.h                                                    (C) 2000-2022 */
/*                                                                           */
/* Gestion des réductions pour les accélérateurs.                            */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_ACCELERATOR_REDUCE_H
#define ARCANE_ACCELERATOR_REDUCE_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/accelerator/core/IReduceMemoryImpl.h"
#include "arcane/accelerator/AcceleratorGlobal.h"

#include <limits.h>
#include <float.h>
#include <atomic>
#include <iostream>

#if defined(__HIP__)
#include <hip/hip_runtime.h>
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::Accelerator
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arccore;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace impl
{

template<typename DataType>
class ReduceIdentity;
template<>
// TODO: utiliser numeric_limits.
class ReduceIdentity<double>
{
 public:
  ARCCORE_HOST_DEVICE static constexpr double sumValue() { return 0.0; }
  ARCCORE_HOST_DEVICE static constexpr double minValue() { return DBL_MAX; }
  ARCCORE_HOST_DEVICE static constexpr double maxValue() { return -DBL_MAX; }
};
template<>
class ReduceIdentity<Int32>
{
 public:
  ARCCORE_HOST_DEVICE static constexpr Int32 sumValue() { return 0; }
  ARCCORE_HOST_DEVICE static constexpr Int32 minValue() { return INT32_MAX; }
  ARCCORE_HOST_DEVICE static constexpr Int32 maxValue() { return -INT32_MAX; }
};
template<>
class ReduceIdentity<Int64>
{
 public:
  ARCCORE_HOST_DEVICE static constexpr Int64 sumValue() { return 0; }
  ARCCORE_HOST_DEVICE static constexpr Int64 minValue() { return INT64_MAX; }
  ARCCORE_HOST_DEVICE static constexpr Int64 maxValue() { return -INT64_MAX; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// L'implémentation utilisée est définie dans 'CudaReduceImpl.h'

template<typename DataType>
class ReduceAtomicSum;

template<>
class ReduceAtomicSum<double>
{
 public:
  static double apply(std::atomic<double>* ptr,double v)
  {
    double old = ptr->load(std::memory_order_consume);
    double wanted = old + v;
    while (!ptr->compare_exchange_weak(old,wanted,std::memory_order_release,std::memory_order_consume))
      wanted = old + v;
    return wanted;
  }
};
template<>
class ReduceAtomicSum<Int64>
{
 public:
  static Int64 apply(std::atomic<Int64>* ptr,Int64 v)
  {
    Int64 x = std::atomic_fetch_add(ptr,v);
    return x+v;
  }
};
template<>
class ReduceAtomicSum<Int32>
{
 public:
  static Int32 apply(std::atomic<Int32>* ptr,Int32 v)
  {
    Int32 x = std::atomic_fetch_add(ptr,v);
    return x+v;
  }
};

template<typename DataType>
class ReduceFunctorSum
{
 public:
  static ARCCORE_DEVICE
  DataType applyDevice(DataType* ptr,DataType v,DataType identity)
  {
    _applyDevice(ptr,v,identity);
    return *ptr;
  }
  static DataType apply(std::atomic<DataType>* ptr,DataType v)
  {
    return ReduceAtomicSum<DataType>::apply(ptr,v);
  }
 public:
  ARCCORE_HOST_DEVICE static constexpr
  DataType identity() { return impl::ReduceIdentity<DataType>::sumValue(); }
 private:
  static ARCCORE_DEVICE
  void _applyDevice(DataType* ptr,DataType v,DataType identity);
};

template<typename DataType>
class ReduceFunctorMax
{
 public:
  static ARCCORE_DEVICE
  DataType applyDevice(DataType* ptr,DataType v,DataType identity)
  {
    _applyDevice(ptr,v,identity);
    return *ptr;
  }
  static DataType apply(std::atomic<DataType>* ptr,DataType v)
  {
    DataType prev_value = *ptr;
    while(prev_value < v && !ptr->compare_exchange_weak(prev_value, v)) {}
    return *ptr;
  }
 public:
  ARCCORE_HOST_DEVICE static constexpr
  DataType identity() { return impl::ReduceIdentity<DataType>::maxValue(); }
 private:
  static ARCCORE_DEVICE
  void _applyDevice(DataType* ptr,DataType v,DataType identity);
};

template<typename DataType>
class ReduceFunctorMin
{
 public:
  static ARCCORE_DEVICE
  DataType applyDevice(DataType* ptr,DataType v,DataType identity)
  {
    _applyDevice(ptr,v,identity);
    return *ptr;
  }
  static DataType apply(std::atomic<DataType>* ptr,DataType v)
  {
    DataType prev_value = *ptr;
    while(prev_value > v && !ptr->compare_exchange_weak(prev_value, v)) {}
    return *ptr;
  }
 public:
  ARCCORE_HOST_DEVICE static constexpr
  DataType identity() { return impl::ReduceIdentity<DataType>::minValue(); }
 private:
  static ARCCORE_DEVICE
  void _applyDevice(DataType* ptr,DataType v,DataType identity);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace impl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*!
 * \brief Opérateur de réduction
 *
 * Cette classe permet de gérer une réduction sur accélérateur ou en
 * multi-thread.
 *
 * La réduction finale a lieu lors de l'appel à reduce(). Il ne faut donc
 * faire cet appel qu'une seule fois et dans une partie collective. Cet appel
 * n'est valide que sur les instance créées avec un constructeur vide. Ces dernières
 * ne peuvent être créées que sur l'hôte.
 *
 * \warning Le constructeur de recopie ne doit pas être appelé explicitement.
 * L'instance de départ doit rester valide tant qu'il existe des copies ou
 * des références dans le noyau de calcul.
 *
 * NOTE sur l'implémentation
 *
 * Sur GPU, les réductions sont effectuées dans le destructeur de la classe
 * La valeur 'm_managed_memory_value' sert à conserver ces valeurs.
 * Sur l'hôte, on utilise un 'std::atomic' pour conserver la valeur commune
 * entre les threads. Cette valeur est référencée par 'm_parent_value' et n'est
 * valide que sur l'hôte.
 */
template<typename DataType,typename ReduceFunctor>
class Reducer
{
 public:
  // TODO: obliger à construire une instance avec un runtime associé qui se chargera d'allouer
  // 'm_managed_memory_value' si besoin.
  Reducer(RunCommand& command)
  : m_managed_memory_value(&m_local_value), m_command(&command)
  {
    //std::cout << String::format("Reduce main host this={0}\n",this); std::cout.flush();
    m_is_master_instance = true;
    m_identity = ReduceFunctor::identity();
    m_local_value = m_identity;
    m_atomic_value = m_identity;
    m_parent_value = &m_atomic_value;
    //printf("Create null host parent_value=%p this=%p\n",(void*)m_parent_value,(void*)this);
    m_memory_impl = impl::internalGetOrCreateReduceMemoryImpl(&command);
    if (m_memory_impl){
      m_managed_memory_value = impl::allocateReduceMemory<DataType>(m_memory_impl);
      *m_managed_memory_value = m_identity;
    }
  }
  ARCCORE_HOST_DEVICE Reducer(const Reducer& rhs)
  : m_managed_memory_value(rhs.m_managed_memory_value), m_local_value(rhs.m_local_value), m_identity(rhs.m_identity)
  {
#ifdef ARCCORE_DEVICE_CODE
    //int threadId = threadIdx.x + blockDim.x * threadIdx.y + (blockDim.x * blockDim.y) * threadIdx.z;
    //if ((threadId%16)==0)
    //  printf("Create ref device Id=%d parent=%p\n",threadId,&rhs);
#else
    m_parent_value = rhs.m_parent_value;
    m_local_value = rhs.m_identity;
    //std::cout << String::format("Reduce copy host  this={0} parent_value={1} rhs={2}\n",this,(void*)m_parent_value,&rhs); std::cout.flush();
    //if (!rhs.m_is_master_instance)
    //ARCANE_FATAL("Only copy from master instance is allowed");
    //printf("Create ref host parent_value=%p this=%p rhs=%p\n",(void*)m_parent_value,(void*)this,(void*)&rhs);
#endif
  }
  ARCCORE_HOST_DEVICE Reducer(Reducer&& rhs) = delete;
#if 0
  : m_managed_memory_value(rhs.m_managed_memory_value), m_local_value(rhs.m_local_value), m_identity(rhs.m_identity)
  {
#ifdef ARCCORE_DEVICE_CODE
    int threadId = threadIdx.x + blockDim.x * threadIdx.y + (blockDim.x * blockDim.y) * threadIdx.z;
    printf("Create && device Id=%d\n",threadId);
#else
    //printf("Create && host = %p\n",((void*)this));
    //#if TEST_PARENT
    m_parent_value = rhs.m_parent_value;
    rhs.m_local_value = rhs.m_identity;
    //#endif
#endif
  }
#endif

  Reducer& operator=(const Reducer& rhs) = delete;

  ARCCORE_HOST_DEVICE ~Reducer()
  {
#ifdef ARCCORE_DEVICE_CODE
    //int threadId = threadIdx.x + blockDim.x * threadIdx.y + (blockDim.x * blockDim.y) * threadIdx.z;
    //if ((threadId%16)==0)
    //printf("Destroy device Id=%d\n",threadId);
    ReduceFunctor::applyDevice(m_managed_memory_value,m_local_value,m_identity);
#else
    //      printf("Destroy host parent_value=%p this=%p\n",(void*)m_parent_value,(void*)this);
    // Code hôte
    //std::cout << String::format("Reduce destructor this={0} parent_value={1} v={2}\n",this,(void*)m_parent_value,m_local_value);
    //std::cout.flush();
    if (!m_is_master_instance)
      ReduceFunctor::apply(m_parent_value,m_local_value);

    //printf("Destroy host %p %p\n",m_managed_memory_value,this);
    if (m_memory_impl)
      m_memory_impl->release();
#endif
  }
 public:
  ARCCORE_HOST_DEVICE void setValue(DataType v)
  {
    m_local_value = v;
  }
  ARCCORE_HOST_DEVICE DataType localValue() const
  {
    return m_local_value;
  }
  // Effectue la réduction et récupère la valeur. ATTENTION: ne faire qu'une seule fois.
  DataType reduce()
  {
    // TODO: faire un fatal si 'm_is_master_instance' vaut 'false'.
    // Effectue la réduction sur m_managed_memory_value
    // One ne peut pas le faire avant car comme en CUDA elle est gérée
    // par la mémoire unifiée, il n'est pas possible d'y accéder tant que
    // le CPU est en train de le faire.
    if (m_parent_value){
      //std::cout << String::format("Reduce host has parent this={0} local_value={1} parent_value={2}\n",
      //                            this,m_local_value,*m_parent_value);
      //std::cout.flush();
      ReduceFunctor::apply(m_parent_value,*m_managed_memory_value);
      *m_managed_memory_value = *m_parent_value;
    }
    else{
      //std::cout << String::format("Reduce host no parent this={0} local_value={1} managed={2}\n",
      //                            this,m_local_value,*m_managed_memory_value);
      //std::cout.flush();
    }
    return *m_managed_memory_value;
  }
 protected:
  impl::IReduceMemoryImpl* m_memory_impl = nullptr;
  DataType* m_managed_memory_value;
 private:
  RunCommand* m_command;
 protected:
  mutable DataType m_local_value;
  std::atomic<DataType>* m_parent_value = nullptr;
  mutable std::atomic<DataType> m_atomic_value;
 private:
  DataType m_identity;
 private:
  bool m_is_allocated = false;
  bool m_is_master_instance = false;
};

template<typename DataType>
class ReducerSum
: public Reducer<DataType,impl::ReduceFunctorSum<DataType>>
{
  using BaseClass = Reducer<DataType,impl::ReduceFunctorSum<DataType>>;
  using BaseClass::m_local_value;
 public:
  ReducerSum(RunCommand& command) : BaseClass(command){}
 public:
  ARCCORE_HOST_DEVICE DataType add(DataType v) const
  {
    m_local_value += v;
    return m_local_value;
  }
};

template<typename DataType>
class ReducerMax
: public Reducer<DataType,impl::ReduceFunctorMax<DataType>>
{
  using BaseClass = Reducer<DataType,impl::ReduceFunctorMax<DataType>>;
  using BaseClass::m_local_value;
 public:
  ReducerMax(RunCommand& command) : BaseClass(command){}
 public:
  ARCCORE_HOST_DEVICE DataType max(DataType v) const
  {
    m_local_value = v>m_local_value ? v : m_local_value;
    return m_local_value;
  }
};

template<typename DataType>
class ReducerMin
: public Reducer<DataType,impl::ReduceFunctorMin<DataType>>
{
  using BaseClass = Reducer<DataType,impl::ReduceFunctorMin<DataType>>;
  using BaseClass::m_local_value;
 public:
  ReducerMin(RunCommand& command) : BaseClass(command){}
 public:
  ARCCORE_HOST_DEVICE DataType min(DataType v) const
  {
    m_local_value = v<m_local_value ? v : m_local_value;
    return m_local_value;
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane::Accelerator

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// Cette macro est définie si on souhaite rendre inline l'implémentation.
// Dans l'idéal il ne faut pas que ce soit le cas (ce qui permettrait de
// changer l'implémentation sans tout recompiler) mais cela ne semble pas
// bien fonctionner pour l'instant.

#define ARCANE_INLINE_REDUCE_IMPL

#ifdef ARCANE_INLINE_REDUCE_IMPL

#  ifndef ARCANE_INLINE_REDUCE
#    define ARCANE_INLINE_REDUCE inline
#  endif

#if defined(__CUDACC__) || defined(__HIP__)
#  include "arcane/accelerator/CommonCudaHipReduceImpl.h"
#else

#endif

#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
