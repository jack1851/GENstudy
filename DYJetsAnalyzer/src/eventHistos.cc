#include "GENstudy/DYJetsAnalyzer/interface/eventHistos.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "TH2F.h"
#include "TNtuple.h"
#include <iostream>

eventHistos::eventHistos()
{
   cutProgress = -0.5;
}

void eventHistos::book(TFileDirectory histFolder, bool binIso){
  m_eventWeight = histFolder.make<TH1F>("eventWeight",";Weight; Events",200, -100, 100);
  m_eventCount = histFolder.make<TH1F>("eventCount", "; ;Events", 1, 0, 1);

  m_fourObjectInvariantMass = histFolder.make<TH1F>("fourObjectInvariantMass", "; m_{lljj} (Gev);Events", 560, 200, 3000);

  m_leadLepPtHisto = histFolder.make<TH1F>("leadLepPtHisto" , ";Lead lepton p_{T} (GeV);Events" , 100 , 0 , 1000 );
  m_subleadLepPtHisto = histFolder.make<TH1F>("subleadLepPtHisto" , "; Sublead lepton p_{T} (GeV);Events" , 100 , 0 , 1000 );
  m_leadLepEtaHisto = histFolder.make<TH1F>("leadLepEtaHisto" , ";Lead lepton #eta;Events" , 100 , -3 , 3 );
  m_subleadLepEtaHisto = histFolder.make<TH1F>("subleadLepEtaHisto" , "; Sublead lepton #eta;Events" , 100 , -3 , 3 );
  m_leadLepPhiHisto = histFolder.make<TH1F>("leadLepPhiHisto" , ";Lead lepton #phi;Events" , 100 , -4 , 4 );
  m_subleadLepPhiHisto = histFolder.make<TH1F>("subleadLepPhiHisto" , "; Sublead lepton #phi;Events" , 100 , -4 , 4 );

  m_leadJetPtHisto = histFolder.make<TH1F>("leadJetPtHisto" , ";Lead jet p_{T} (GeV);Events" , 100 , 0 , 1000 );
  m_leadJetEtaHisto = histFolder.make<TH1F>("leadJetEtaHisto" , ";Lead jet #eta;Events" , 100 , -3 , 3 );
  m_leadJetPhiHisto = histFolder.make<TH1F>("leadJetPhiHisto" , ";Lead jet #phi;Events" , 100 , -4 , 4 );
  
  m_subleadJetPtHisto = histFolder.make<TH1F>("subleadJetPtHisto" , ";Sublead jet p_{T} (GeV);Events" , 100 , 0 , 1000 );
  m_subleadJetEtaHisto = histFolder.make<TH1F>("subleadJetEtaHisto" , ";Sublead jet #eta;Events" , 100 , -3 , 3 );
  m_subleadJetPhiHisto = histFolder.make<TH1F>("subleadJetPhiHisto" , ";Sublead jet #phi;Events" , 100 , -4 , 4 );
}

void eventHistos::FillHists(eventInfo info, bool binIso, bool makeTuples)
{
     m_eventWeight->Fill(info.eventWeight);
     m_eventCount->Fill(0.5,info.eventWeight);

     m_fourObjectInvariantMass->Fill(info.fourObjectInvariantMass, info.eventWeight);

     m_leadLepPtHisto->Fill(info.lepton1Pt, info.eventWeight);
     m_leadLepEtaHisto->Fill(info.lepton1Eta, info.eventWeight);
     m_leadLepPhiHisto->Fill(info.lepton1Phi, info.eventWeight);

     m_subleadLepPtHisto->Fill(info.lepton2Pt, info.eventWeight);
     m_subleadLepEtaHisto->Fill(info.lepton2Eta, info.eventWeight);
     m_subleadLepPhiHisto->Fill(info.lepton2Phi, info.eventWeight);

     m_leadJetPtHisto->Fill(info.leadJetPt, info.eventWeight);
     m_leadJetEtaHisto->Fill(info.leadJetEta, info.eventWeight);
     m_leadJetPhiHisto->Fill(info.leadJetPhi, info.eventWeight);

     m_subleadJetPtHisto->Fill(info.subleadJetPt, info.eventWeight);
     m_subleadJetEtaHisto->Fill(info.subleadJetEta, info.eventWeight);
     m_subleadJetPhiHisto->Fill(info.subleadJetPhi, info.eventWeight);
}


