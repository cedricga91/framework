﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* NumMatrix.h                                                 (C) 2000-2022 */
/*                                                                           */
/* Matrice carrée de taille fixe de types numériques.                        */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_UTILS_NUMMATRIX_H
#define ARCANE_UTILS_NUMMATRIX_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/NumVector.h"
#include "arcane/utils/Real2x2.h"
#include "arcane/utils/Real3x3.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \internal
 * \brief Petite matrice carrée de taille fixe de N données numériques.
 *
 * \note Actuellement uniquement implémenté pour 2 ou 3 valeurs et pour le
 * type Real.
 *
 * \warning API en cours de définition. Ne pas utiliser en dehors de Arcane
 *
 * Il est possible d'accéder à chaque composante du vecteur par 'operator[]'
 * ou 'operator()' ou par les méthodes vx(), vy(), vz() si la dimension est
 * suffisante (par exemple vz() est uniquement accessible si la Size>=3.
 */
template <typename T, int Size>
class ARCANE_UTILS_EXPORT NumMatrix
{
  static_assert(Size == 2 || Size == 3, "Valid values for Size are 2 or 3");

 public:

  using VectorType = NumVector<T, Size>;
  using ThatClass = NumMatrix<T, Size>;
  using DataType = T;

 public:

  //! Construit la matrice avec tous les coefficiants nuls.
  NumMatrix() = default;

  //! Construit la matrice avec les lignes (ax,ay,az)
  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  constexpr ARCCORE_HOST_DEVICE NumMatrix(const VectorType& ax, const VectorType& ay, const VectorType& az)
  {
    m_values[0] = ax;
    m_values[1] = ay;
    m_values[2] = az;
  }

  //! Construit la matrice avec les lignes (ax,ay)
  template <int S = Size, typename = std::enable_if_t<S == 2, void>>
  constexpr ARCCORE_HOST_DEVICE NumMatrix(const VectorType& ax, const VectorType& ay)
  {
    m_values[0] = ax;
    m_values[1] = ay;
  }

  //! Construit l'instance avec le triplet (v,v,v).
  constexpr ARCCORE_HOST_DEVICE explicit NumMatrix(T v)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] = v;
  }

  template <int S = Size, typename = std::enable_if_t<S == 2, void>>
  explicit constexpr ARCCORE_HOST_DEVICE NumMatrix(Real2x2 v)
  : NumMatrix(VectorType(v.x), VectorType(v.y))
  {}

  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  explicit constexpr ARCCORE_HOST_DEVICE NumMatrix(Real3x3 v)
  : NumMatrix(VectorType(v.x), VectorType(v.y), VectorType(v.z))
  {}

  //! Affecte à l'instance le triplet (v,v,v).
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator=(T v)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] = v;
    return (*this);
  }

  template <int S = Size, typename = std::enable_if_t<S == 2, void>>
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator=(const Real2x2& v)
  {
    *this = ThatClass(v);
    return (*this);
  }

  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator=(const Real3x3& v)
  {
    *this = ThatClass(v);
    return (*this);
  }

  template <int S = Size, typename = std::enable_if_t<S == 2, void>>
  operator Real2x2() const { return Real2x2(m_values[0], m_values[1]); }

  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  operator Real3x3() const { return Real3x3(m_values[0], m_values[1], m_values[2]); }

 public:

  //! Construit la matrice nulle
  constexpr ARCCORE_HOST_DEVICE static ThatClass zero() { return ThatClass(); }

  //! Construit la matrice identité
  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  constexpr ARCCORE_HOST_DEVICE static ThatClass identity()
  {
    return ThatClass(VectorType(1.0, 0.0, 0.0), VectorType(0.0, 1.0, 0.0), VectorType(0.0, 0.0, 1.0));
  }

  //! Construit la matrice ((ax,bx,cx),(ay,by,cy),(az,bz,cz)).
  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  constexpr ARCCORE_HOST_DEVICE static ThatClass fromColumns(T ax, T ay, T az, T bx, T by, T bz, T cx, T cy, T cz)
  {
    return ThatClass(VectorType(ax, bx, cx), VectorType(ay, by, cy), VectorType(az, bz, cz));
  }

  //! Construit la matrice ((ax,bx,cx),(ay,by,cy),(az,bz,cz)).
  template <int S = Size, typename = std::enable_if_t<S == 3, void>>
  constexpr ARCCORE_HOST_DEVICE static ThatClass fromLines(T ax, T bx, T cx, T ay, T by, T cy, T az, T bz, T cz)
  {
    return ThatClass(VectorType(ax, bx, cx), VectorType(ay, by, cy), VectorType(az, bz, cz));
  }

 public:

  /*!
   * \brief Compare la matrice avec la matrice nulle.
   *
   * La matrice est nulle si et seulement si chacune de ses composantes
   * est inférieure à un espilon donné. La valeur de l'epsilon utilisée est celle
   * de float_info<value_type>::nearlyEpsilon():
   * \f[A=0 \Leftrightarrow |A.x|<\epsilon,|A.y|<\epsilon,|A.z|<\epsilon \f]
   *
   * \retval true si la matrice est égale à la matrice nulle,
   * \retval false sinon.
   */
  constexpr ARCCORE_HOST_DEVICE bool isNearlyZero() const
  {
    bool is_nearly_zero = true;
    for (int i = 0; i < Size; ++i)
      is_nearly_zero = is_nearly_zero && math::isNearlyZero(m_values[i]);
    return is_nearly_zero;
  }

  //! Ajoute \a b au triplet.
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator+=(const ThatClass& b)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] += b.m_values[i];
    return (*this);
  }
  //! Soustrait \a b au triplet
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator-=(const ThatClass& b)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] -= b.m_values[i];
    return (*this);
  }
  //! Multiple chaque composante de la matrice par le réel \a b
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator*=(T b)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] *= b;
    return (*this);
  }
  //! Divise chaque composante de la matrice par le réel \a b
  constexpr ARCCORE_HOST_DEVICE ThatClass& operator/=(T b)
  {
    for (int i = 0; i < Size; ++i)
      m_values[i] *= b;
    return (*this);
  }
  //! Créé un triplet qui vaut ce triplet ajouté à \a b
  friend constexpr ARCCORE_HOST_DEVICE ThatClass operator+(const ThatClass& a, const ThatClass& b)
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = a.m_values[i] + b.m_values[i];
    return v;
  }
  //! Créé un triplet qui vaut \a b soustrait de ce triplet
  friend constexpr ARCCORE_HOST_DEVICE ThatClass operator-(const ThatClass& a, const ThatClass& b)
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = a.m_values[i] - b.m_values[i];
    return v;
  }
  //! Créé un tenseur opposé au tenseur actuel
  constexpr ARCCORE_HOST_DEVICE ThatClass operator-() const
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = -m_values[i];
    return v;
  }

  //! Multiplication par un scalaire.
  friend constexpr ARCCORE_HOST_DEVICE ThatClass operator*(DataType a, const ThatClass& mat)
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = a * mat.m_values[i];
    return v;
  }
  //! Multiplication par un scalaire.
  friend constexpr ARCCORE_HOST_DEVICE ThatClass operator*(const ThatClass& mat, DataType b)
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = mat.m_values[i] * b;
    return v;
  }
  //! Division par un scalaire.
  friend constexpr ARCCORE_HOST_DEVICE ThatClass operator/(const ThatClass& mat, DataType b)
  {
    ThatClass v;
    for (int i = 0; i < Size; ++i)
      v.m_values[i] = mat.m_values[i] / b;
    return v;
  }

  /*!
   * \brief Compare composant pas composante l'instance courante à \a b.
   *
   * \retval true si this.x==b.x et this.y==b.y et this.z==b.z.
   * \retval false sinon.
   */
  friend constexpr ARCCORE_HOST_DEVICE bool operator==(const ThatClass& a, const ThatClass& b)
  {
    for (int i = 0; i < Size; ++i)
      if (a.m_values[i] != b.m_values[i])
        return false;
    return true;
  }

  /*!
   * \brief Compare deux triplets.
   * Pour la notion d'égalité, voir operator==()
   * \retval true si les deux triplets sont différents,
   * \retval false sinon.
   */
  friend constexpr ARCCORE_HOST_DEVICE bool operator!=(const ThatClass& a, const ThatClass& b)
  {
    return !operator==(b);
  }

 public:

  constexpr ARCCORE_HOST_DEVICE VectorType& operator()(Int32 i)
  {
    ARCCORE_CHECK_AT(i, Size);
    return m_values[i];
  }
  constexpr ARCCORE_HOST_DEVICE VectorType operator()(Int32 i) const
  {
    ARCCORE_CHECK_AT(i, Size);
    return m_values[i];
  }
  constexpr ARCCORE_HOST_DEVICE VectorType& operator[](Int32 i)
  {
    ARCCORE_CHECK_AT(i, Size);
    return m_values[i];
  }
  constexpr ARCCORE_HOST_DEVICE VectorType operator[](Int32 i) const
  {
    ARCCORE_CHECK_AT(i, Size);
    return m_values[i];
  }
  constexpr ARCCORE_HOST_DEVICE T& operator()(Int32 i, Int32 j)
  {
    ARCCORE_CHECK_AT(i, Size);
    ARCCORE_CHECK_AT(j, Size);
    return m_values[i](j);
  }
  constexpr ARCCORE_HOST_DEVICE T operator()(Int32 i, Int32 j) const
  {
    ARCCORE_CHECK_AT(i, Size);
    ARCCORE_CHECK_AT(j, Size);
    return m_values[i](j);
  }

 public:

  template <int S = Size, typename = std::enable_if_t<S >= 1, void>>
  VectorType& vx()
  {
    return m_values[0];
  }
  template <int S = Size, typename = std::enable_if_t<S >= 1, void>>
  VectorType vx() const
  {
    return m_values[0];
  }

  template <int S = Size, typename = std::enable_if_t<S >= 2, void>>
  VectorType& vy()
  {
    return m_values[1];
  }
  template <int S = Size, typename = std::enable_if_t<S >= 2, void>>
  VectorType vy() const
  {
    return m_values[1];
  }

  template <int S = Size, typename = std::enable_if_t<S >= 3, void>>
  VectorType& vz()
  {
    return m_values[2];
  }
  template <int S = Size, typename = std::enable_if_t<S >= 3, void>>
  VectorType vz() const
  {
    return m_values[2];
  }

 private:

  VectorType m_values[Size] = {};

 private:

  /*!
   * \brief Compare les valeurs de \a a et \a b avec le comparateur TypeEqualT
   * \retval true si \a a et \a b sont égaux,
   * \retval false sinon.
   */
  constexpr ARCCORE_HOST_DEVICE static bool _eq(T a, T b)
  {
    return TypeEqualT<T>::isEqual(a, b);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif