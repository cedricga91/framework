﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* PDESRandomNumberGeneratorUnitTest.cc                        (C) 2000-2022 */
/*                                                                           */
/* Service du test du générateur de nombres (pseudo-)aléatoires avec         */
/* algorithme pseudo-DES.                                                    */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/BasicUnitTest.h"
#include "arcane/IRandomNumberGenerator.h"
#include "arcane/tests/ArcaneTestGlobal.h"

#include "arcane/tests/PDESRandomNumberGeneratorUnitTest_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcaneTest
{
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PDESRandomNumberGeneratorUnitTest
: public ArcanePDESRandomNumberGeneratorUnitTestObject
{

 public:
  explicit PDESRandomNumberGeneratorUnitTest(const ServiceBuildInfo& sb)
  : ArcanePDESRandomNumberGeneratorUnitTestObject(sb)
  {}

  ~PDESRandomNumberGeneratorUnitTest() {}

 public:
  void initializeTest() override;
  void executeTest() override;

 private:
  void testRNGS();
  void hardcodedValues();
  void hardcodedSeeds();
  void mcPi();
  void leepSeeds();
  void leepNumbers();

  const Int64 hardcoded_seed = 4294967297;
  const RealUniqueArray hardcoded_vals{
    0.376176703328938, 0.203853136647106, 0.543236669926571, 0.516308492941001,
    0.445606825584272, 0.267495363061149, 0.832417463039992, 0.11967593515044,
    0.0352465721550738, 0.47980882357161, 0.0590498646656005, 0.361190883945183,
    0.692373169790809, 0.13135186727069, 0.701084595503481, 0.127681029949293,
    0.68554413647629, 0.0479189166212371, 0.846545325322494, 0.0568973012174675,
    0.52948767502731, 0.643703472686373, 0.569386740263157, 0.218121968148538,
    0.124178313946883, 0.59092421508804, 0.754040863195856, 0.290482921536399,
    0.0259063446713852, 0.649482478542729, 0.974993708352869, 0.0926179281520404,
    0.42818301380599, 0.575909046455183, 0.754555668631645, 0.525146146386379,
    0.53141963481537, 0.527946936359135, 0.860741255437955, 0.886217272170865,
    0.905783767592049, 0.773827690453175, 0.305794745150351, 0.788491498083059,
    0.960830384287971, 0.0338097907892886, 0.697265511891603, 0.0826798047701767,
    0.628081739899015, 0.50181734599391, 0.317813147592064, 0.743015769359592,
    0.748691821619172, 0.532228241206159, 0.99605664609321, 0.943883901379664,
    0.241193767616583, 0.894332153409824, 0.910001233434135, 0.769234525373422,
    0.202208155912562, 0.969693744196632, 0.0252192755627009, 0.28420245041143,
    0.134602575329612, 0.205076039619702, 0.813649741374501, 0.86247787874832,
    0.777694601776845, 0.766327664529106, 0.766994941827056, 0.289581511724289,
    0.923309448547635, 0.730190307892636, 0.0842956745573359, 0.155329710551714,
    0.694651103975058, 0.128442001818378, 0.908152246836013, 0.811618350054331,
    0.967290693154098, 0.641808666541855, 0.315206037987095, 0.679576410923592,
    0.99499867074132, 0.476469897431545, 0.300122385551894, 0.794404575969142,
    0.891844643990751, 0.197779674554145, 0.71981752746071, 0.84580495146997,
    0.134986305649733, 0.195654556755629, 0.443416479071739, 0.0855064593969102,
    0.336126954501102, 0.364861651693621, 0.849602070089918, 0.5682620967981
  };

  const Int64UniqueArray hardcoded_seeds{
    6939235372800674851,  1774134854824149473,  8678271704849413744,  2073154441506600183,
    5781040954386991925,  595375303505718101,   2757007270532811093,  -7919398390197474384,
    5251826553399937363,  -929787228572470279,  -7350775191333437557, -1058683020420908109,
    -2333555138956985791, 1038855514968926216,  -7207527345388469756, -3612174759776103755,
    34577244946648886,    3634641805471522665,  -6793997795981352090, 6326502579982566989,
    9092809249989018759,  -9000712990899247918, 1318741534601918815,  6084720711858281674,
    -8996702654347830434, 366319653163069740,   -310084707188176345,  2473458535339371799,
    6828713473607066524,  -3338447327676362445, -4419534035235943979, 5654913766215234315,
    -2784460752587387254, -7596511144154531091, -4413726680771941043, 8165199158237120840,
    -226078867703889986,  7022280331164813716,  -3538075192685103361, -4675468775182080694,
    264882454441685421,   7784879787774616292,  -6131776782307617204, -7509720532565592406,
    -2632686252399234024, 8429956397232579866,  6394103135331009437,  -5159631701243187532,
    -1839888798523287722, -2363742498355327316, -2142729965057069240, 8983336831511704870,
    190136086719912610,   4895959487902858945,  -295619335846596531,  3796261332472513227,
    -9168827030150311576, 6923587413022196134,  3688818649974491271,  6509690066121862041,
    -7640083523641220306, -66933277645970014,   551334679430542616,   2248965245339175278,
    6502953631856376189,  -9154764402318392615, -275223871968729083,  7251497659329033201,
    -6133366772454452538, -6871681112815608999, 6160241844269452813,  6337008017013986228,
    -1789129430501518508, -778934091325645956,  -8501435885820998301, -2544253154225443905,
    -8984925953466456080, -4962810042853009759, -4148564218546094498, -5809355777438601950,
    -1975690508714788096, -8583493764522403284, -4629221690516185498, 1021469112989039281,
    1954754383115919588,  7649912313319838472,  -143214599990323944,  -1738160190502169608,
    -7363576043579166343, -6814585130773781277, -5887594600955786222, 3542085565446089874,
    -7038165219928972459, -8324160237523995663, -4998952219525874817, -9127785021258947660,
    -7876776378998795224, -6245871457033356668, 8656534886308383108,  2592989201197417484
  };
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_PDESRANDOMNUMBERGENERATORUNITTEST(PDESRandomNumberGeneratorUnitTest, PDESRandomNumberGeneratorUnitTest);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void PDESRandomNumberGeneratorUnitTest::
initializeTest()
{
  info() << "init test";
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void PDESRandomNumberGeneratorUnitTest::
executeTest()
{
  info() << "execute test";

  
  testRNGS();
  hardcodedValues();
  hardcodedSeeds();
  mcPi();
  leepSeeds();
  leepNumbers();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void PDESRandomNumberGeneratorUnitTest::
testRNGS()
{
  Integer aaa = 1234;
  RandomNumberGeneratorSeed test_a(aaa);

  Integer bbb;
  test_a.seed(bbb);

  if (aaa != bbb)
    ARCANE_FATAL("[testRNGS:0] Valeurs differentes.");

  RandomNumberGeneratorSeed test_b(bbb); //1234

  if (test_a != test_b)
    ARCANE_FATAL("[testRNGS:1] Valeurs differentes.");

  test_b = 1234;

  if (test_a != test_b)
    ARCANE_FATAL("[testRNGS:2] Valeurs differentes.");

  RandomNumberGeneratorSeed test_c = test_a;

  if (test_a != test_c)
    ARCANE_FATAL("[testRNGS:3] Valeurs differentes.");
}

void PDESRandomNumberGeneratorUnitTest::
hardcodedValues()
{
  IRandomNumberGenerator* rng = options()->getPDESRandomNumberGenerator();

  RandomNumberGeneratorSeed r_seed(hardcoded_seed);
  rng->initSeed(r_seed);
  RandomNumberGeneratorSeed initial_seed(hardcoded_seed);

  for (Integer i = 0; i < hardcoded_vals.size(); i++) {
    Real val1 = rng->generateRandomNumber();
    Real val2 = rng->generateRandomNumber(&initial_seed);

    if (val1 != val2) {
      info() << val1 << val2;
      ARCANE_FATAL("[hardcodedValues:0] Valeurs differentes.");
    }

    if ((Integer)(hardcoded_vals[i] * 1e9) != (Integer)(val1 * 1e9)) {
      info() << hardcoded_vals[i] << " " << val1;
      ARCANE_FATAL("[hardcodedValues:1] Valeurs differentes.");
    }
  }
}

void PDESRandomNumberGeneratorUnitTest::
hardcodedSeeds()
{
  IRandomNumberGenerator* rng = options()->getPDESRandomNumberGenerator();

  RandomNumberGeneratorSeed r_seed(hardcoded_seed);
  rng->initSeed(r_seed);
  RandomNumberGeneratorSeed initial_seed(hardcoded_seed);

  for (Integer i = 0; i < hardcoded_seeds.size(); i++) {
    RandomNumberGeneratorSeed val11 = rng->generateRandomSeed();
    RandomNumberGeneratorSeed val22 = rng->generateRandomSeed(&initial_seed);

    Int64 val1,val2;
    val11.seed(val1);
    val22.seed(val2);

    if (val1 != val2){
      info() << val1 << val2;
      ARCANE_FATAL("[hardcodedSeeds:0] Valeurs differentes.");
    }
    if (hardcoded_seeds[i] != val1){
      info() << hardcoded_seeds[i] << " " << val1;
      ARCANE_FATAL("[hardcodedSeeds:1] Valeurs differentes.");
    }
  }
}

void PDESRandomNumberGeneratorUnitTest::
mcPi()
{
  IRandomNumberGenerator* rng = options()->getPDESRandomNumberGenerator();
  rng->initSeed();

  const Integer nb_iter(10000);
  Real sum(0.);

  for (Integer i = 0; i < nb_iter; i++) {
    Real2 xy(rng->generateRandomNumber(), rng->generateRandomNumber());
    if (xy.squareNormL2() < 1)
      sum++;
  }
  Real estim = 4 * sum / nb_iter;
  info() << "Pi ~= " << estim;
  if (estim < 3.00 || estim > 3.50)
    ARCANE_FATAL("[mcPi:0] Pi.");
}

void PDESRandomNumberGeneratorUnitTest::
leepNumbers()
{
  IRandomNumberGenerator* rng = options()->getPDESRandomNumberGenerator();
  if(!rng->isLeapNumberSupported()) return;

  RandomNumberGeneratorSeed r_seed(hardcoded_seed);
  rng->initSeed(r_seed);

  for (Integer i = 2; i < hardcoded_vals.size(); i+=3) {
    Real val1 = rng->generateRandomNumber(2);

    if ((Integer)(hardcoded_vals[i] * 1e9) != (Integer)(val1 * 1e9)) {
      info() << hardcoded_vals[i] << " " << val1;
      ARCANE_FATAL("[leepNumbers:0] Valeurs differentes.");
    }
  }

  // On teste aussi les sauts négatifs.
  for (Integer i = hardcoded_vals.size() - 3; i >= 0; i--) {
    Real val1 = rng->generateRandomNumber(-2);

    if ((Integer)(hardcoded_vals[i] * 1e9) != (Integer)(val1 * 1e9)) {
      info() << hardcoded_vals[i] << " " << val1;
      ARCANE_FATAL("[leepNumbers:1] Valeurs differentes.");
    }
  }
}

void PDESRandomNumberGeneratorUnitTest::
leepSeeds()
{
  IRandomNumberGenerator* rng = options()->getPDESRandomNumberGenerator();
  if (!rng->isLeapSeedSupported())
    return;

  RandomNumberGeneratorSeed r_seed(hardcoded_seed);
  rng->initSeed(r_seed);

  for (Integer i = 2; i < hardcoded_seeds.size(); i += 3) {
    RandomNumberGeneratorSeed val11 = rng->generateRandomSeed(2);

    Int64 val1;
    val11.seed(val1);

    if (hardcoded_seeds[i] != val1) {
      info() << hardcoded_seeds[i] << " " << val1;
      ARCANE_FATAL("[leepSeeds:0] Valeurs differentes.");
    }
  }

  // On teste aussi les sauts négatifs.
  for (Integer i = hardcoded_seeds.size() - 3; i >= 0; i--) {
    RandomNumberGeneratorSeed val11 = rng->generateRandomSeed(-2);

    Int64 val1;
    val11.seed(val1);

    if (hardcoded_seeds[i] != val1) {
      info() << hardcoded_seeds[i] << " " << val1;
      ARCANE_FATAL("[leepSeeds:1] Valeurs differentes.");
    }
  }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
} // namespace ArcaneTest

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/