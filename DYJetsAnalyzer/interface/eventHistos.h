#ifndef eventHistos_h
#define eventHistos_h

#include <vector>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/SortedCollection.h"
#include "GENstudy/DYJetsAnalyzer/interface/eventInfo.h"

class TH1F;
class TH2F;
class TNtuple;

class eventHistos{
  public:
    eventHistos();

    void book(TFileDirectory histFolder, bool binIso=false);
    void FillHists(eventInfo, bool binIso=false, bool makeTuples = false);
  public:
    TH1F* m_eventWeight;
    TH1F* m_eventCount;
    TH1F* m_fourObjectInvariantMass;

    TH1F* m_leadLepPtHisto;
    TH1F* m_leadLepEtaHisto;
    TH1F* m_leadLepPhiHisto;

    TH1F* m_subleadLepPtHisto;
    TH1F* m_subleadLepEtaHisto;
    TH1F* m_subleadLepPhiHisto;

    TH1F* m_leadJetPtHisto;
    TH1F* m_leadJetEtaHisto;
    TH1F* m_leadJetPhiHisto;

    TH1F* m_subleadJetPtHisto;
    TH1F* m_subleadJetEtaHisto;
    TH1F* m_subleadJetPhiHisto;

    double cutProgress;

};
#endif
