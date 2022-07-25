﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* IRandomNumberGenerator.h                                    (C) 2000-2022 */
/*                                                                           */
/* Interface pour générateur de nombres aléatoires.                          */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifndef ARCANE_IRANDOMNUMBERGENERATOR_H
#define ARCANE_IRANDOMNUMBERGENERATOR_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/Array.h"
#include "arcane/utils/UtilsTypes.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/**
 * @brief Classe représentant une graine.
 * 
 * Une graine est définie comme étant un tableau de Bytes.
 * Sa taille est définie grâce à un des constructeurs ou grâce à la méthode
 * "resize()".
 * 
 * Une fois que la taille du tableau a été définie,
 *  - le seul moyen de la modifier est d'appeler "resize()",
 *  - si l'on appelle "setSeed()", la valeur sera rognée si sa taille est trop
 *    grande ou des "0x00" seront ajoutés à la fin du tableau si sa taille est 
 *    trop petite (attention aux entiers non signés négatifs).
 *  - si l'on appelle "seed()" avec un type de taille différente à la taille
 *    du tableau, on aura le même comportement que "setSeed()" (mais avec 
 *    le paramètre [OUT]).
 * 
 * L'utilisation normal d'une RNGSeed veut que la taille ne soit jamais modifiée
 * après coup. Pour être sûr de la taille de la seed lors de l'init, 
 * IRandomNumberGenerator possède une méthode "emptySeed()" permettant de
 * récupérer une RNGSeed vide de taille correcte pour l'implémentation. Un appel
 * à "seed_vide.setSeed(T)" produira donc une seed de taille correct, quelque
 * soit le type T de la valeur.
 * 
 * @deprecated Utiliser la classe RNGSeedHelper() à la place.
 */
class ARCANE_CORE_EXPORT RandomNumberGeneratorSeed
{
 public:
  /**
   * @brief Constructeur avec graine de type T et taille du tableau de Bytes.
   * 
   * - Si sizeof(T) > sizeOfSeed, alors la valeur sera rognée.
   * - Si sizeof(T) < sizeOfSeed et T=signed alors la valeur changera et
   *   deviendra positive.
   * - Si sizeof(T) = sizeOfSeed (ou sizeof(T) < sizeOfSeed et T=unsigned),
   *   parfait !
   * 
   * @tparam T Le type de graine (Int64 par ex).
   * @param value La valeur de la graine.
   * @param sizeOfSeed La taille de la graine.
   */
  template <class T>
  explicit RandomNumberGeneratorSeed(T value, Integer sizeOfSeed)
  {
    resize(sizeOfSeed);
    setSeed(value);
  }

  /**
   * @brief Constructeur copie.
   * 
   * @param seed La graine source.
   */
  RandomNumberGeneratorSeed(const RandomNumberGeneratorSeed& seed)
  {
    m_seed.copy(seed.constView());
  }

  /**
   * @brief Constructeur move.
   * 
   * @param seed La graine source.
   */
  RandomNumberGeneratorSeed(RandomNumberGeneratorSeed&& seed)
  {
    *this = std::move(seed);
  }

  /**
   * @brief Constructeur par défaut.
   * Nécessitera un appel à resize().
   */
  RandomNumberGeneratorSeed()
  : m_seed(0)
  {}

  virtual ~RandomNumberGeneratorSeed() {}

 public:
  /**
   * @brief Méthode permettant de définir une graine.
   * 
   * Attention, le tableau interne doit avoir une taille >= 1
   * avant l'appel à cette méthode.
   * 
   * @tparam T Le type de graine.
   * @param value La valeur de la graine.
   * @return true Si la graine a pu être construite.
   * @return false Si la graine n'a pas pu être construite.
   */
  template <class T>
  bool setSeed(T value)
  {
    if (m_seed.size() == 0) {
      return false;
    }
    memcpy(m_seed.data(), &value, std::min(m_seed.size(), (Integer)sizeof(T)));
    for (Integer i = sizeof(T); i < m_seed.size(); i++) {
      m_seed[i] = 0x00;
    }
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la valeur de la graine.
   * 
   * @tparam T Le type de la graine.
   * @param value [OUT] La valeur de la graine.
   * @param without_size_check Si le rognage de la valeur est autorisé.
   * @return true Si la graine a pu être placé.
   * @return false Si la graine n'a pas pu être placé ou si le tableau 
   *               a une taille nulle.
   */
  template <class T>
  bool seed(T& value, bool without_size_check = true) const
  {
    if (m_seed.size() == 0 || (!without_size_check && sizeof(T) != m_seed.size())) {
      return false;
    }
    value = 0;
    memcpy(&value, m_seed.data(), std::min(m_seed.size(), (Integer)sizeof(T)));
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la valeur de la graine.
   * 
   * @tparam T Le type de la graine.
   * @param value [OUT] La valeur de la graine.
   * @param without_size_check Si le rognage de la valeur est autorisé.
   * @return true Si la graine a pu être placé.
   * @return false Si la graine n'a pas pu être placé ou si le tableau 
   *               a une taille nulle.
   */
  template <class T>
  bool seed(T* value, bool without_size_check = true) const
  {
    if (m_seed.size() == 0 || (sizeof(T) != m_seed.size() && !without_size_check)) {
      return false;
    }
    *value = 0;
    memcpy(value, m_seed.data(), std::min(m_seed.size(), (Integer)sizeof(T)));
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la taille de la graine.
   * 
   * @return Integer La taille de la graine (en octet).
   */
  Integer sizeOfSeed() const
  {
    return m_seed.size();
  }

  /**
   * @brief Méthode permettant de récupérer une vue constante sur le tableau
   * de Byte interne.
   * 
   * @return ByteConstArrayView La vue.
   */
  ByteConstArrayView constView() const
  {
    return m_seed.constView();
  }

  /**
   * @brief Méthode permettant de récupérer une vue sur le tableau
   * de Byte interne.
   * 
   * @return ByteArrayView La vue.
   */
  ByteArrayView view()
  {
    return m_seed.view();
  }

  /**
   * @brief Méthode permettant de modifier la taille du tableau de Byte interne.
   * 
   * Si la taille voulu est plus petite que la taille d'origine, il y aura
   * une perte de donnée. A faire uniquement si c'est voulu.
   * Si la taille voulu est plus grande que la taille d'origine, il y aura
   * un ajout de 0x00 au début (attention aux entiers signés négatifs).
   * 
   * @param sizeOf La nouvelle taille.
   * @return true Si la taille a bien été modifiée.
   * @return false Si la taille n'a pas été modifiée.
   */
  bool resize(Integer sizeOf)
  {
    m_seed.resize(sizeOf, 0x00);
    return true;
  }

  /**
   * @brief Opérateur de copie.
   * 
   * @param seed La graine source.
   * @return RandomNumberGeneratorSeed& La graine destination.
   */
  RandomNumberGeneratorSeed& operator=(const RandomNumberGeneratorSeed& seed)
  {
    if (this == &seed) {
      return *this;
    }

    m_seed.copy(seed.constView());
    return *this;
  }

  /**
   * @brief Opérateur de copie depuis une valeur de graine.
   * 
   * Attention, le tableau interne doit avoir une taille >= 1
   * avant l'appel à cette méthode.
   * 
   * @tparam T Le type de la graine.
   * @param value La valeur de la graine.
   * @return RandomNumberGeneratorSeed& La graine destination.
   */
  template <class T>
  RandomNumberGeneratorSeed& operator=(T value)
  {
    setSeed(value);
    return *this;
  }

  /**
   * @brief Opérateur de comparaison.
   * 
   * @param seed La graine à comparer.
   * @return true Si les deux graines sont identiques.
   * @return false Si les deux graines ne sont pas identiques.
   */
  bool operator==(const RandomNumberGeneratorSeed& seed) const
  {
    ByteConstArrayView my_data = constView();
    ByteConstArrayView other_data = seed.constView();

    if (my_data != other_data) {
      return false;
    }
    return true;
  }

  /**
   * @brief Opérateur de comparaison.
   * 
   * @param seed La graine à comparer.
   * @return true Si les deux graines ne sont pas identiques.
   * @return false Si les deux graines sont identiques.
   */
  bool operator!=(const RandomNumberGeneratorSeed& seed) const
  {
    return !operator==(seed);
  }

 protected:
  ByteUniqueArray m_seed;
};


/**
 * @brief Classe permettant de manipuler facilement une graine.
 * 
 * Une graine est représentée par un tableau de Byte.
 * Cette classe utilise un ArrayView de ce tableau.
 * 
 * Cette classe permet de définir une valeur dans le tableau et
 * de récupérer cette valeur (autres autres choses).
 * 
 * Cette classe ne stocke pas le tableau mais uniquement
 * un ArrayView de ce tableau.
 * 
 */
class ARCANE_CORE_EXPORT RNGSeedHelper
{
 public:

  /**
   * @brief Constructeur de la classe.
   * 
   * @param av Un ArrayView de tableau représentant une graine.
   */
  RNGSeedHelper(ByteArrayView av)
  {
    m_seed = av;
  }

  /**
   * @brief Constructeur de classe.
   * 
   * L'utilisateur ne devrait pas à avoir à utiliser ce
   * constructeur car sa graine n'a pas de type connu.
   * 
   * @tparam T Un type de base.
   * @param var Un pointeur vers la graine 
   *            (attention, ne fait pas une copie de la valeur !)
   */
  template <class T>
  RNGSeedHelper(T* var)
  {
    m_seed = ByteArrayView(sizeof(T), (Byte*)var);
  }

  virtual ~RNGSeedHelper() = default;

 public:

  /**
   * @brief Méthode permettant de définir une valeur dans la graine.
   * 
   * @tparam T Le type de valeur.
   * @param value_in La futur valeur de la graine.
   * @return true Si la valeur a pu être attribuée.
   * @return false Si la valeur n'a pas pu être attribuée.
   */
  template <class T>
  bool setValue(T value_in)
  {
    if (m_seed.empty()) {
      return false;
    }
    memcpy(m_seed.data(), &value_in, std::min(m_seed.size(), (Integer)sizeof(T)));
    for (Integer i = sizeof(T); i < m_seed.size(); i++) {
      m_seed[i] = 0x00;
    }
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la valeur de la graine.
   * 
   * @tparam T Le type de la graine.
   * @param value_out [OUT] La valeur de la graine.
   * @param without_size_check Si le rognage de la valeur est autorisé.
   * @return true Si la valeur a pu être récupérée.
   * @return false Si la valeur n'a pas pu être récupérée ou si le tableau 
   *               a une taille nulle.
   */
  template <class T>
  bool value(T& value_out, bool without_size_check = true) const
  {
    if (m_seed.empty() || (!without_size_check && sizeof(T) != m_seed.size())) {
      return false;
    }
    value_out = 0;
    memcpy(&value_out, m_seed.data(), std::min(m_seed.size(), (Integer)sizeof(T)));
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la valeur de la graine.
   * 
   * @tparam T Le type de la graine.
   * @param value_out [OUT] La valeur de la graine.
   * @param without_size_check Si le rognage de la valeur est autorisé.
   * @return true Si la valeur a pu être récupérée.
   * @return false Si la valeur n'a pas pu être récupérée ou si le tableau 
   *               a une taille nulle.
   */
  template <class T>
  bool value(T* value_out, bool without_size_check = true) const
  {
    if (m_seed.empty() || (sizeof(T) != m_seed.size() && !without_size_check)) {
      return false;
    }
    *value_out = 0;
    memcpy(value_out, m_seed.data(), std::min(m_seed.size(), (Integer)sizeof(T)));
    return true;
  }

  /**
   * @brief Méthode permettant de récupérer la taille de la graine.
   * 
   * @return Integer La taille de la graine (en octet).
   */
  Integer sizeOfSeed() const
  {
    return m_seed.size();
  }

  /**
   * @brief Méthode permettant de récupérer une vue constante.
   * 
   * @return ByteConstArrayView La vue.
   */
  ByteConstArrayView constView() const
  {
    return m_seed.constView();
  }

  /**
   * @brief Méthode permettant de récupérer une vue.
   * 
   * @return ByteArrayView La vue.
   */
  ByteArrayView view()
  {
    return m_seed;
  }

  /**
   * @brief Opérateur de copie depuis une valeur de graine.
   * 
   * 
   * @tparam T Le type de la graine.
   * @param value La valeur de la graine.
   * @return RNGSeedHelper& La graine destination.
   */
  template <class T>
  RNGSeedHelper& operator=(T new_value)
  {
    setValue(new_value);
    return *this;
  }

  /**
   * @brief Méthode permettant de récupérer une copie du
   * tableau de Byte.
   * 
   * @return ByteUniqueArray La copie du tableau de Byte.
   */
  ByteUniqueArray copy()
  {
    return ByteUniqueArray(m_seed);
  }

 protected:
  ByteArrayView m_seed;
};

/**
 * @ingroup StandardService
 * @brief Interface pour un générateur de nombre aléatoire.
 */
class ARCANE_CORE_EXPORT IRandomNumberGenerator
{
 public:
  virtual ~IRandomNumberGenerator() = default;

 public:
  /**
   * @brief Méthode permettant d'initialiser le service.
   * 
   * Avec la graine en option (ou la graine par défaut si l'on est
   * en mode singleton).
   * 
   * @return true Si l'initialisation a bien eu lieu.
   * @return false Si l'initialisation n'a pas eu lieu.
   */
  virtual bool initSeed() = 0;

  /**
   * @brief Méthode permettant d'initialiser le service.
   * 
   * Si la graine n'a pas la bonne taille, false sera retourné.
   * 
   * @param seed La graine d'origine.
   * @return true Si l'initialisation a bien eu lieu.
   * @return false Si l'initialisation n'a pas eu lieu.
   * 
   * @deprecated Utiliser la méthode initSeed(ByteArrayView seed) à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method initSeed(ByteArrayView seed) instead")
  virtual bool initSeed(RandomNumberGeneratorSeed seed) = 0;

  /**
   * @brief Méthode permettant d'initialiser le service.
   * 
   * Si la graine n'a pas la bonne taille, false sera retourné.
   * 
   * @param seed La graine d'origine.
   * @return true Si l'initialisation a bien eu lieu.
   * @return false Si l'initialisation n'a pas eu lieu.
   */
  virtual bool initSeed(ByteArrayView seed) = 0;

  /**
   * @brief Méthode permettant de récupérer la graine actuelle.
   * 
   * @return RandomNumberGeneratorSeed La graine.
   * 
   * @deprecated Utiliser la méthode viewSeed() à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method viewSeed() instead")
  virtual RandomNumberGeneratorSeed seed() = 0;

  /**
   * @brief Méthode permettant de récupérer une vue constante sur la
   * graine actuelle.
   * 
   * @return ByteArrayView La graine.
   */
  virtual ByteConstArrayView viewSeed() = 0;

  /**
   * @brief Méthode permettant de récupérer une graine vide de bonne taille.
   * 
   * @return RandomNumberGeneratorSeed La graine vide.
   * 
   * @deprecated Utiliser la méthode emptySeedBUA() à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method emptySeedBUA() instead")
  virtual RandomNumberGeneratorSeed emptySeed() = 0;

  /**
   * @brief Méthode permettant de récupérer une graine vide de bonne taille.
   * 
   * @return ByteUniqueArray La graine vide.
   */
  virtual ByteUniqueArray emptySeedBUA() = 0;

  /**
   * @brief Méthode permettant de savoir la taille de seed nécessaire
   * pour l'implémentation.
   * 
   * @return Integer La taille de seed nécessaire (en octet).
   */
  virtual Integer neededSizeOfSeed() = 0;

  /**
   * @brief Méthode permettant de savoir si les sauts sont permis sur le
   * générateur de graines.
   * 
   * @return true Si oui.
   * @return false Si non.
   */
  virtual bool isLeapSeedSupported() = 0;

  /**
   * @brief Méthode permettant de générer une graine "enfant" à partir d'une
   * graine "parent".
   * 
   * @param leap Le saut à effectuer (0 = la graine n+1+0 / 1 = la graine n+1+1).
   * @return RandomNumberGeneratorSeed La nouvelle graine généré à partir de la graine en mémoire.
   * 
   * @deprecated Utiliser la méthode generateRandomSeedBUA(Integer leap = 0) à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method generateRandomSeedBUA(Integer leap = 0) instead")
  virtual RandomNumberGeneratorSeed generateRandomSeed(Integer leap = 0) = 0;

  /**
   * @brief Méthode permettant de générer une graine "enfant" à partir d'une
   * graine "parent".
   * 
   * @param leap Le saut à effectuer (0 = la graine n+1+0 / 1 = la graine n+1+1).
   * @return ByteUniqueArray La nouvelle graine généré à partir de la graine en mémoire.
   */
  virtual ByteUniqueArray generateRandomSeedBUA(Integer leap = 0) = 0;

  /**
   * @brief Méthode permettant de générer une graine "enfant" à partir d'une
   * graine "parent".
   * 
   * Cette méthode n'utilise pas la graine en mémoire mais la graine en paramètre.
   * Si la graine en paramètre n'a pas la bonne taille, une erreur sera émise.
   * 
   * @param parent_seed [IN/OUT] La graine "parent".
   * @param leap Le saut à effectuer (0 = la graine n+1+0 / 1 = la graine n+1+1).
   * @return RandomNumberGeneratorSeed La nouvelle graine généré à partir de la graine "parent".
   * 
   * @deprecated Utiliser la méthode generateRandomSeed(ByteArrayView parent_seed, Integer leap = 0) à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method generateRandomSeed(ByteArrayView parent_seed, Integer leap = 0) instead")
  virtual RandomNumberGeneratorSeed generateRandomSeed(RandomNumberGeneratorSeed* parent_seed, Integer leap = 0) = 0;
  
  /**
   * @brief Méthode permettant de générer une graine "enfant" à partir d'une
   * graine "parent".
   * 
   * Cette méthode n'utilise pas la graine en mémoire mais la graine en paramètre.
   * Si la graine en paramètre n'a pas la bonne taille, une erreur sera émise.
   * 
   * @param parent_seed [IN/OUT] La graine "parent".
   * @param leap Le saut à effectuer (0 = la graine n+1+0 / 1 = la graine n+1+1).
   * @return ByteUniqueArray La nouvelle graine généré à partir de la graine "parent".
   */
  virtual ByteUniqueArray generateRandomSeed(ByteArrayView parent_seed, Integer leap = 0) = 0;

  /**
   * @brief Méthode permettant de savoir si les sauts sont permis sur le
   * générateur de nombres.
   * 
   * @return true Si oui.
   * @return false Si non.
   */
  virtual bool isLeapNumberSupported() = 0;

  /**
   * @brief Méthode permettant de générer un nombre aléatoire avec
   * la graine en mémoire.
   * 
   * @param leap Le saut à effectuer (0 = le nombre n+1+0 / 1 = le nombre n+1+1).
   * @return Real Le nombre généré (entre 0 et 1).
   */
  virtual Real generateRandomNumber(Integer leap = 0) = 0;

  /**
   * @brief Méthode permettant de générer un nombre aléatoire avec
   * la graine transmise en paramètre.
   *    
   * Cette méthode n'utilise pas la graine en mémoire mais la graine en paramètre.
   * Si la graine en paramètre n'a pas la bonne taille, une erreur sera émise.
   * 
   * @param seed [IN/OUT] La graine.
   * @param leap Le saut à effectuer (0 = le nombre n+1+0 / 1 = le nombre n+1+1).
   * @return Real Le nombre généré (entre 0 et 1).
   * 
   * @deprecated Utiliser la méthode generateRandomNumber(ByteArrayView seed, Integer leap = 0) à la place.
   */
  ARCANE_DEPRECATED_REASON("Use method generateRandomNumber(ByteArrayView seed, Integer leap = 0) instead")
  virtual Real generateRandomNumber(RandomNumberGeneratorSeed* seed, Integer leap = 0) = 0;

  /**
   * @brief Méthode permettant de générer un nombre aléatoire avec
   * la graine transmise en paramètre.
   *    
   * Cette méthode n'utilise pas la graine en mémoire mais la graine en paramètre.
   * Si la graine en paramètre n'a pas la bonne taille, une erreur sera émise.
   * 
   * @param seed [IN/OUT] La graine.
   * @param leap Le saut à effectuer (0 = le nombre n+1+0 / 1 = le nombre n+1+1).
   * @return Real Le nombre généré (entre 0 et 1).
   */
  virtual Real generateRandomNumber(ByteArrayView seed, Integer leap = 0) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
