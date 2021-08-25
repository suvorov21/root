/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * authors:                                                                  *
 *  Lydia Brenner (lbrenner@cern.ch), Carsten Burgard (cburgard@cern.ch)     *
 *  Katharina Ecker (kecker@cern.ch), Adam Kaluza      (akaluza@cern.ch)     *
 * Copyright (c) 2000-2007, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)
 *****************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
//
// RooLagrangianMorph
//
// The RooLagrangianMorphFunc is a type of RooAbsReal that allows to morph
// different input EFT samples to some arbitrary output EFT
// sample, as long as the desired set of output parameters lie
// within the realm spanned by the input samples. More
// specifically, it expects as an input a TFile (or TDirectory)
// with the following layout:
//
// TDirectory
//  |-sample1
//  | |-param_card    // TH1 EFT parameter values of sample1
//  | | histogram1    // TH1 of some physics distribution
//  | |-subfolder1    // a subfolder (optional)
//  | | |-histogram2  // TH1 of some physics distribution
//  | | |-....
//  |-sample2
//  | |-param_card     // TH1 of EFT parameter values of sample2
//  | | histogram1     // TH1 of some physics distribution
//  | |-subfolder1     // same folder structure as before
//  | | |-histogram2  // TH1 of some physics distribution
//  | | |-....
//  |-sampleN         
// The RooLagrangianMorphFunc operates on this structure, extracts data
// and meta-data and produces a morphing result as a RooRealSumFunc
// consisting of the input histograms with appropriate prefactors.
//
// The histograms to be morphed can be accessed via their paths in
// the respective sample, e.g. using
//    "histogram"
// or "subfolder1/histogram1"
// or "some/deep/path/to/some/subfolder/histname"
//
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROO_LAGRANGIAN_MORPH
#define ROO_LAGRANGIAN_MORPH

#include "Floats.h"
#include "RooAbsArg.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRatio.h"
#include "RooRealSumFunc.h"
#include "RooRealSumPdf.h"
#include "RooSetProxy.h"
#include "RooWrapperPdf.h"
#include "TMatrixD.h"

class RooWorkspace;
class RooParamHistFunc;
class RooProduct;
class RooRealVar;
class TPair;
class TFolder;
class RooLagrangianMorphFunc;

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class RooLagrangianMorphFunc : public RooAbsReal {

public:
  typedef std::map<const std::string, double> ParamSet;
  typedef std::map<const std::string, int> FlagSet;
  typedef std::map<const std::string, ParamSet> ParamMap;
  typedef std::map<const std::string, FlagSet> FlagMap;

  class Config {
  public:
    Config() {}
    Config(const RooAbsCollection &couplings);
    Config(const RooAbsCollection &prodCouplings,
           const RooAbsCollection &decCouplings);
    void setFileName(const char *filename);
    void setFolders(const RooArgList &folderlist);
    void setObservableName(const char *obsname);
    void setCouplings(const RooAbsCollection &couplings);
    void setCouplings(const RooAbsCollection &prodCouplings,
                      const RooAbsCollection &decCouplings);
    void allowNegativeYields(Bool_t allowNegativeYields);
    template <class T> void setVertices(const std::vector<T> &vertices);
    template <class T>
    void setDiagrams(const std::vector<std::vector<T>> &diagrams);
    template <class T>
    void setNonInterfering(const std::vector<T *> &nonInterfering);
    template <class T> void addDiagrams(const std::vector<T> &diagrams);

    std::string const& getFileName() const { return this->_fileName; }
    std::string const& getObservableName() const { return this->_obsName; }
    std::vector<std::vector<RooArgList *>> const& getDiagrams() const { return this->_configDiagrams; }
    RooArgList const& getCouplings() const { return this->_couplings; }
    RooArgList const& getProdCouplings() const { return this->_prodCouplings; }
    RooArgList const& getDecCouplings() const { return this->_decCouplings; }
    RooArgList const& getFolders() const { return this->_folderlist; }
    bool IsAllowNegativeYields() const { return this->_allowNegativeYields; }


    void append(ParamSet &set, const char *str, double val);
    //void append(ParamMap &map, const char *str, ParamSet &set);

    /* WIP
    void disableInterference(const std::vector<const char*>& nonInterfering) ;
    void disableInterferences(const std::vector<std::vector<const char*> >&
    nonInterfering) ;
    */

    RooRealVar *getParameter(const char *name) const;
    bool hasParameter(const char *name) const;

    void addFolders(const RooArgList &folders);

    ParamMap const& getParamCards() const { return this->_paramCards; };
    FlagMap const& getFlagValues() const { return this->_flagValues; };

    std::vector<std::string> const& getFolderNames() const { return _folderNames; };
    void printSamples() const;
    void printPhysics() const;
    /// Return the number of samples in this morphing function.
    int nSamples() const { return this->_folderNames.size(); }

    void readParameters(TDirectory *f);

  private:
    std::string _obsName;
    std::string _fileName;
    RooArgList _folderlist;
    std::vector<std::string> _folderNames;
    ParamMap _paramCards;
    FlagMap _flagValues;
    std::vector<RooArgList *> _vertices;
    RooArgList _couplings;
    RooArgList _prodCouplings;
    RooArgList _decCouplings;
    RooArgList _observables;
    RooArgList _binWidths;
    std::vector<std::vector<RooArgList *>> _configDiagrams;
    std::vector<RooArgList *> _nonInterfering;
    Bool_t _allowNegativeYields = true;
  };

  RooLagrangianMorphFunc();
  RooLagrangianMorphFunc(const char *name, const char *title,
                         const Config &config);
  RooLagrangianMorphFunc(const RooLagrangianMorphFunc &other,
                         const char *newName);

  virtual ~RooLagrangianMorphFunc();

  virtual std::list<Double_t> *binBoundaries(RooAbsRealLValue & /*obs*/,
                                             Double_t /*xlo*/,
                                             Double_t /*xhi*/) const override;
  virtual std::list<Double_t> *
  plotSamplingHint(RooAbsRealLValue & /*obs*/, Double_t /*xlo*/,
                   Double_t /*xhi*/) const override;
  virtual Bool_t isBinnedDistribution(const RooArgSet &obs) const override;
  virtual Double_t evaluate() const override;
  virtual TObject *clone(const char *newname) const override;
  virtual Double_t getValV(const RooArgSet *set = 0) const override;

  virtual Bool_t checkObservables(const RooArgSet *nset) const override;
  virtual Bool_t forceAnalyticalInt(const RooAbsArg &arg) const override;
  virtual Int_t
  getAnalyticalIntegralWN(RooArgSet &allVars, RooArgSet &numVars,
                          const RooArgSet *normSet,
                          const char *rangeName = 0) const override;
  virtual Double_t
  analyticalIntegralWN(Int_t code, const RooArgSet *normSet,
                       const char *rangeName = 0) const override;
  virtual void printMetaArgs(std::ostream &os) const override;
  virtual RooAbsArg::CacheMode canNodeBeCached() const override;
  virtual void setCacheAndTrackHints(RooArgSet &) override;

  void insert(RooWorkspace *ws);

  void setParameters(const char *foldername);
  void setParameters(TH1 *paramhist);
  void setParameter(const char *name, double value);
  void setFlag(const char *name, double value);
  void setParameters(const ParamSet &params);
  void setParameters(const RooArgList *list);
  double getParameterValue(const char *name) const;
  RooRealVar *getParameter(const char *name) const;
  RooRealVar *getFlag(const char *name) const;
  bool hasParameter(const char *paramname) const;
  bool isParameterUsed(const char *paramname) const;
  bool isParameterConstant(const char *paramname) const;
  void setParameterConstant(const char *paramname, bool constant) const;
  void setParameter(const char *name, double value, double min, double max);
  void setParameter(const char *name, double value, double min, double max,
                    double error);
  void randomizeParameters(double z);
  const RooArgSet *getParameterSet() const;
  ParamSet getMorphParameters(const char *foldername) const;
  ParamSet getMorphParameters() const;

  RooLagrangianMorphFunc *getLinear() const;

  int nParameters() const;
  int nPolynomials() const;

  bool isCouplingUsed(const char *couplname);
  const RooArgList *getCouplingSet() const;
  ParamSet getCouplings() const;

  // virtual Bool_t IsAllowNegativeYields() const { return
  // this->_config.IsAllowNegativeYields(); }
  TMatrixD getMatrix() const;
  TMatrixD getInvertedMatrix() const;
  double getCondition() const;

  RooRealVar *getObservable() const;
  RooRealVar *getBinWidth() const;

  void printEvaluation() const;
  void printCouplings() const;
  void printFlags() const;
  void printPhysics() const;

  RooProduct *getSumElement(const char *name) const;

  std::vector<std::string> getSamples() const;

  double expectedUncertainty() const;
  TH1 *createTH1(const std::string &name);
  TH1 *createTH1(const std::string &name, bool correlateErrors);

protected:
  class CacheElem;
  void init();
  void printAuthors() const;
  void setup(bool ownParams = true);
  bool _ownParameters = false;

  /* wip
  void disableInterference(const std::vector<const char*>& nonInterfering) ;
  void disableInterferences(const std::vector<std::vector<const char*> >&
  nonInterfering) ;
  */

  mutable RooObjCacheManager _cacheMgr; //! The cache manager

  void addFolders(const RooArgList &folders);

  bool hasCache() const;
  RooLagrangianMorphFunc::CacheElem *getCache(const RooArgSet *nset) const;
  void updateSampleWeights();

  RooRealVar *setupObservable(const char *obsname, TClass *mode,
                              TObject *inputExample);

public:
  /// length of floating point digits precision supported by implementation.
  static constexpr double implementedPrecision = SuperFloatPrecision::digits10;
  void importToWorkspace(RooWorkspace *ws, const RooAbsReal *object);
  void importToWorkspace(RooWorkspace *ws, RooAbsData *object);

  void writeMatrixToFile(const TMatrixD &matrix, const char *fname);
  void writeMatrixToStream(const TMatrixD &matrix, std::ostream &stream);
  TMatrixD readMatrixFromFile(const char *fname);
  TMatrixD readMatrixFromStream(std::istream &stream);

  // RooDataHist* makeDataHistogram(TH1* hist, RooRealVar* observable, const
  // char* histname = NULL) ; void setDataHistogram(TH1* hist, RooRealVar*
  // observable, RooDataHist* dh) ; void printDataHistogram(RooDataHist* hist,
  // RooRealVar* obs) ;

  int countSamples(std::vector<RooArgList *> &vertices);
  int countSamples(int nprod, int ndec, int nboth);

  TPair *makeCrosssectionContainer(double xs, double unc);
  std::map<std::string, std::string>
  createWeightStrings(const ParamMap &inputs,
                      const std::vector<std::vector<std::string>> &vertices);
  std::map<std::string, std::string>
  createWeightStrings(const ParamMap &inputs,
                      const std::vector<RooArgList *> &vertices,
                      RooArgList &couplings);
  std::map<std::string, std::string> createWeightStrings(
      const ParamMap &inputs, const std::vector<RooArgList *> &vertices,
      RooArgList &couplings, const FlagMap &flagValues, const RooArgList &flags,
      const std::vector<RooArgList *> &nonInterfering);
  RooArgSet createWeights(const ParamMap &inputs,
                          const std::vector<RooArgList *> &vertices,
                          RooArgList &couplings, const FlagMap &inputFlags,
                          const RooArgList &flags,
                          const std::vector<RooArgList *> &nonInterfering);
  RooArgSet createWeights(const ParamMap &inputs,
                          const std::vector<RooArgList *> &vertices,
                          RooArgList &couplings);

  bool updateCoefficients();
  bool useCoefficients(const TMatrixD &inverse);
  bool useCoefficients(const char *filename);
  bool writeCoefficients(const char *filename);

  int countContributingFormulas() const;
  RooAbsReal *getSampleWeight(const char *name);
  void printParameters(const char *samplename) const;
  void printParameters() const;
  void printSamples() const;
  void printSampleWeights() const;
  void printWeights() const;

  void setScale(double val);
  double getScale();

  int nSamples() const {return this->_config.getFolderNames().size(); }

  RooRealSumFunc *getFunc() const;
  std::unique_ptr<RooWrapperPdf> createPdf() const;

  RooAbsPdf::ExtendMode extendMode() const;
  Double_t expectedEvents(const RooArgSet *nset) const;
  Double_t expectedEvents(const RooArgSet &nset) const;
  Double_t expectedEvents() const;
  Bool_t selfNormalized() const { return true; }

  void readParameters(TDirectory *f) { _config.readParameters(f); }
  void collectInputs(TDirectory *f);

  static std::unique_ptr<RooRatio> makeRatio(const char *name,
                                             const char *title, RooArgList &nr,
                                             RooArgList &dr);

protected:
  double _scale = 1;
  std::map<std::string, int> _sampleMap;
  RooListProxy _physics;
  RooSetProxy _operators;
  RooListProxy _observables;
  RooListProxy _binWidths;
  RooListProxy _flags;
  Config _config;
  std::vector<std::vector<RooListProxy *>> _diagrams;
  mutable const RooArgSet *_curNormSet; //!
  std::vector<RooListProxy *> _nonInterfering;

  ClassDefOverride(RooLagrangianMorphFunc, 1)
};

#endif
