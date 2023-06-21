#include "GENstudy/DYJetsAnalyzer/interface/eventInfo.h"
#include <iostream>

eventInfo::eventInfo()
{
    eventWeight=1;
    fourObjectInvariantMass=-1;

    number_of_partons = 0;

    lepton1 = 0;
    lepton2 = 0;

    DJRValue0 = 10000.0;
    DJRValue1 = 10000.0;
    DJRValue2 = 10000.0;
    DJRValue3 = 10000.0;
    DJRValue4 = 10000.0;

    nMEPartons = -10000;

    lepton1Eta = -10000.0;
    lepton1Phi = -10000.0;
    lepton1Pt = -10000.0;

    lepton2Eta = -10000.0;
    lepton2Phi = -10000.0;
    lepton2Pt = -10000.0;

    leadJetEta = -10000.0;
    leadJetPhi = -10000.0;
    leadJetPt = -10000.0;

    subleadJetEta = -10000.0;
    subleadJetPhi = -10000.0;
    subleadJetPt = -10000.0;

    diJetMass = -10000.0; 
    diLepPt = -10000.0;
    diLepMass = -10000.0; 
    ptllOverMll = -10000.0; 
    leadJetZMass = -10000.0; 
    subleadJetZMass = -10000.0; 

    myGenJets.clear();

    for(int i = 0; i<6; i++) {
    passCutArray.push_back(true);
    }
}

