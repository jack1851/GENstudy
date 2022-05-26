#ifndef eventInfo_h
#define eventInfo_h

#include <vector>
#include <map>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1F.h"

class eventInfo{
  public:
    eventInfo();
    double eventWeight;
    double fourObjectInvariantMass;

    double lepton1Eta;
    double lepton1Phi;
    double lepton1Pt;

    double lepton2Eta;
    double lepton2Phi;
    double lepton2Pt;

    double leadJetEta;
    double leadJetPhi;
    double leadJetPt;

    double subleadJetEta;
    double subleadJetPhi;
    double subleadJetPt;
};
#endif
