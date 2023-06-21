#ifndef eventInfo_h
#define eventInfo_h

#include <vector>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1F.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"

class eventInfo{
  public:
    eventInfo();
    double eventWeight;
    std::vector<bool> passCutArray;
    int number_of_partons;
    double fourObjectInvariantMass;

    std::vector<const reco::GenJet*> myGenJets;

    const reco::GenParticle* lepton1;
    const reco::GenParticle* lepton2;

    double lepton1Eta;
    double lepton1Phi;
    double lepton1Pt;

    double DJRValue0;
    double DJRValue1;
    double DJRValue2;
    double DJRValue3;
    double DJRValue4;

    int nMEPartons;
    
    double lepton2Eta;
    double lepton2Phi;
    double lepton2Pt;

    double leadJetEta;
    double leadJetPhi;
    double leadJetPt;

    double subleadJetEta;
    double subleadJetPhi;
    double subleadJetPt;

    double diJetMass;
    double diLepPt;
    double diLepMass;
    double ptllOverMll;
    double leadJetZMass;
    double subleadJetZMass;
};
#endif
