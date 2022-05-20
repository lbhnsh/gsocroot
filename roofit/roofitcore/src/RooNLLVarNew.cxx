/*
 * Project: RooFit
 * Authors:
 *   Jonas Rembser, CERN 2021
 *   Emmanouil Michalainas, CERN 2021
 *
 * Copyright (c) 2021, CERN
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted according to the terms
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)
 */

/**
\file RooNLLVarNew.cxx
\class RooNLLVarNew
\ingroup Roofitcore

This is a simple class designed to produce the nll values needed by the fitter.
In contrast to the `RooNLLVar` class, any logic except the bare minimum has been
transfered away to other classes, like the `RooFitDriver`. This class also calls
functions from `RooBatchCompute` library to provide faster computation times.
**/

#include <RooNLLVarNew.h>

#include <RooAddition.h>
#include <RooFormulaVar.h>
#include <RooNaNPacker.h>
#include <RooFit/Detail/Buffers.h>

#include <ROOT/StringUtils.hxx>

#include <Math/Util.h>
#include <TMath.h>

#include <numeric>
#include <stdexcept>
#include <vector>

using namespace ROOT::Experimental;

// Declare constexpr static members to make them available if odr-used in C++14.
constexpr const char *RooNLLVarNew::weightVarName;
constexpr const char *RooNLLVarNew::weightVarNameSumW2;

namespace {

std::unique_ptr<RooAbsReal> createRangeNormTerm(RooAbsPdf const &pdf, RooArgSet const &observables,
                                                std::string const &baseName, std::string const &rangeNames)
{

   RooArgSet observablesInPdf;
   pdf.getObservables(&observables, observablesInPdf);

   std::unique_ptr<RooAbsReal> integral{
      pdf.createIntegral(observablesInPdf, &observablesInPdf, pdf.getIntegratorConfig(), rangeNames.c_str())};
   auto out =
      std::make_unique<RooFormulaVar>((baseName + "_correctionTerm").c_str(), "(log(x[0]))", RooArgList(*integral));
   out->addOwnedComponents(std::move(integral));
   return out;
}

template <class Input>
double kahanSum(Input const &input)
{
   return ROOT::Math::KahanSum<double, 4u>::Accumulate(input.begin(), input.end()).Sum();
}

} // namespace

/** Contstruct a RooNLLVarNew

\param pdf The pdf for which the nll is computed for
\param observables The observabes of the pdf
\param isExtended Set to true if this is an extended fit
**/
RooNLLVarNew::RooNLLVarNew(const char *name, const char *title, RooAbsPdf &pdf, RooArgSet const &observables,
                           bool isExtended, std::string const &rangeName)
   : RooAbsReal(name, title), _pdf{"pdf", "pdf", this, pdf}, _observables{observables}, _isExtended{isExtended}
{
   if (!rangeName.empty()) {
      auto term = createRangeNormTerm(pdf, observables, pdf.GetName(), rangeName);
      _rangeNormTerm = std::make_unique<RooTemplateProxy<RooAbsReal>>("_rangeNormTerm", "_rangeNormTerm", this, *term);
      this->addOwnedComponents(std::move(term));
   }
}

RooNLLVarNew::RooNLLVarNew(const RooNLLVarNew &other, const char *name)
   : RooAbsReal(other, name), _pdf{"pdf", this, other._pdf}, _observables{other._observables}
{
   if (other._rangeNormTerm)
      _rangeNormTerm = std::make_unique<RooTemplateProxy<RooAbsReal>>("_rangeNormTerm", this, *other._rangeNormTerm);
}

/** Compute multiple negative logs of propabilities

\param dispatch A pointer to the RooBatchCompute library interface used for this computation
\param output An array of doubles where the computation results will be stored
\param nEvents The number of events to be processed
\param dataMap A map containing spans with the input data for the computation
**/
void RooNLLVarNew::computeBatch(cudaStream_t * /*stream*/, double *output, size_t /*nOut*/,
                                RooFit::Detail::DataMap const &dataMap) const
{
   std::size_t nEvents = dataMap.at(_pdf).size();
   auto probas = dataMap.at(_pdf);

   auto logProbasBuffer = ROOT::Experimental::Detail::makeCpuBuffer(nEvents);
   RooSpan<double> logProbas{logProbasBuffer->cpuWritePtr(), nEvents};
   (*_pdf).getLogProbabilities(probas, logProbas.data());

   auto &nameReg = RooNameReg::instance();
   auto weights = dataMap.at(nameReg.constPtr((_prefix + weightVarName).c_str()));
   auto weightsSumW2 = dataMap.at(nameReg.constPtr((_prefix + weightVarNameSumW2).c_str()));
   auto weightSpan = _weightSquared ? weightsSumW2 : weights;

   if ((_isExtended || _rangeNormTerm) && _sumWeight == 0.0) {
      _sumWeight = weights.size() == 1 ? weights[0] * nEvents : kahanSum(weights);
   }
   if ((_isExtended || _rangeNormTerm) && _weightSquared && _sumWeight2 == 0.0) {
      _sumWeight2 = weights.size() == 1 ? weightsSumW2[0] * nEvents : kahanSum(weightsSumW2);
   }
   if (_rangeNormTerm) {
      auto rangeNormTermSpan = dataMap.at(*_rangeNormTerm);
      if (rangeNormTermSpan.size() == 1) {
         _sumCorrectionTerm = (_weightSquared ? _sumWeight2 : _sumWeight) * rangeNormTermSpan[0];
      } else {
         if (weightSpan.size() == 1) {
            _sumCorrectionTerm = weightSpan[0] * kahanSum(rangeNormTermSpan);
         } else {
            // We don't need to use the library for now because the weights and
            // correction term integrals are always in the CPU map.
            _sumCorrectionTerm = 0.0;
            for (std::size_t i = 0; i < nEvents; ++i) {
               _sumCorrectionTerm += weightSpan[i] * rangeNormTermSpan[i];
            }
         }
      }
   }

   ROOT::Math::KahanSum<double> kahanProb;
   RooNaNPacker packedNaN(0.f);

   for (std::size_t i = 0; i < nEvents; ++i) {

      double eventWeight = weightSpan.size() > 1 ? weightSpan[i] : weightSpan[0];
      if (0. == eventWeight * eventWeight)
         continue;

      const double term = -eventWeight * logProbas[i];

      kahanProb.Add(term);
      packedNaN.accumulate(term);
   }

   if (packedNaN.getPayload() != 0.) {
      // Some events with evaluation errors. Return "badness" of errors.
      kahanProb = packedNaN.getNaNWithPayload();
   }

   if (_isExtended) {
      assert(_sumWeight != 0.0);
      kahanProb += _pdf->extendedTerm(_sumWeight, &_observables, _weightSquared ? _sumWeight2 : 0.0);
   }
   if (_rangeNormTerm) {
      kahanProb += _sumCorrectionTerm;
   }
   output[0] = kahanProb.Sum();
}

double RooNLLVarNew::evaluate() const
{
   return _value;
}

void RooNLLVarNew::getParametersHook(const RooArgSet * /*nset*/, RooArgSet *params, Bool_t /*stripDisconnected*/) const
{
   // strip away the observables and weights
   params->remove(_observables, true, true);
}

////////////////////////////////////////////////////////////////////////////////
/// Replaces all observables and the weight variable of this NLL with clones
/// that only differ by a prefix added to the names. Used for simultaneous fits.
/// \return A RooArgSet with the new observable args.
/// \param[in] prefix The prefix to add to the observables and weight names.
RooArgSet RooNLLVarNew::prefixObservableAndWeightNames(std::string const &prefix)
{
   _prefix = prefix;

   RooArgSet obsSet{_observables};
   RooArgSet obsClones;
   obsSet.snapshot(obsClones);
   for (RooAbsArg *arg : obsClones) {
      arg->setAttribute((std::string("ORIGNAME:") + arg->GetName()).c_str());
      arg->SetName((prefix + arg->GetName()).c_str());
   }
   recursiveRedirectServers(obsClones, false, true);

   RooArgSet newObservables{obsClones};

   setObservables(obsClones);
   addOwnedComponents(std::move(obsClones));

   return newObservables;
}

////////////////////////////////////////////////////////////////////////////////
/// Toggles the weight square correction.
void RooNLLVarNew::applyWeightSquared(bool flag)
{
   _weightSquared = flag;
}
