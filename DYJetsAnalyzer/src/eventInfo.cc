#include "GENstudy/DYJetsAnalyzer/interface/eventInfo.h"
#include <iostream>

eventInfo::eventInfo()
{
    eventWeight=1;
    fourObjectInvariantMass=-1;

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
}

