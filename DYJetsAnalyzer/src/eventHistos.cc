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
#include "TMath.h"
#include <iostream>

eventHistos::eventHistos()
{
   cutProgress = -0.5;
}

void eventHistos::book(TFileDirectory histFolder, bool binIso){
  m_eventWeight = histFolder.make<TH1F>("eventWeight",";Weight; Events",200, -100, 100);
  m_eventCount = histFolder.make<TH1F>("eventCount", "; ;Events", 1, 0, 1);
  m_passCutHisto = histFolder.make<TH1F>("passCutHisto", "; ;Events", 6, -0.5, 5.5);
  m_number_of_partons = histFolder.make<TH1F>("number_of_partons", "; ;Events", 5, -0.5, 4.5);

  m_djr0to1_allpartons = histFolder.make<TH1F>("djr0to1_allpartons", "; ;Events", 50, -0.5, 3);
  m_djr0to1_0partons = histFolder.make<TH1F>("djr0to1_0partons", "; ;Events", 50, -0.5, 3);
  m_djr0to1_1partons = histFolder.make<TH1F>("djr0to1_1partons", "; ;Events", 50, -0.5, 3);
  m_djr0to1_2partons = histFolder.make<TH1F>("djr0to1_2partons", "; ;Events", 50, -0.5, 3);
  m_djr0to1_3partons = histFolder.make<TH1F>("djr0to1_3partons", "; ;Events", 50, -0.5, 3);
  m_djr0to1_4partons = histFolder.make<TH1F>("djr0to1_4partons", "; ;Events", 50, -0.5, 3);

  m_djr1to2_allpartons = histFolder.make<TH1F>("djr1to2_allpartons", "; ;Events", 50, -0.5, 3);
  m_djr1to2_0partons = histFolder.make<TH1F>("djr1to2_0partons", "; ;Events", 50, -0.5, 3);
  m_djr1to2_1partons = histFolder.make<TH1F>("djr1to2_1partons", "; ;Events", 50, -0.5, 3);
  m_djr1to2_2partons = histFolder.make<TH1F>("djr1to2_2partons", "; ;Events", 50, -0.5, 3);
  m_djr1to2_3partons = histFolder.make<TH1F>("djr1to2_3partons", "; ;Events", 50, -0.5, 3);
  m_djr1to2_4partons = histFolder.make<TH1F>("djr1to2_4partons", "; ;Events", 50, -0.5, 3);

  m_fourObjectInvariantMass = histFolder.make<TH1F>("fourObjectInvariantMass", "; m_{lljj} (GeV);Events", 600, 0, 3000);

  m_diJetMass = histFolder.make<TH1F>("diJetMass", "; m_{jj} (Gev);Events", 600, 0, 3000);
  m_diLepPtHisto = histFolder.make<TH1F>("diLepPtHisto" , ";Dilepton p_{T} (GeV);Events" , 400 , 0 , 2000);
  m_diLepMassHisto = histFolder.make<TH1F>("diLepMassHisto" , ";Dilepton mass m_{ll} (GeV);Events" , 400 , 0, 2000);
  m_ptllOverMllHisto = histFolder.make<TH1F>("ptllOverMllHisto" , "; p_{T_{ll}}/m_{ll} (GeV);Events" , 100 , 0 , 10);
  m_leadJetZMass = histFolder.make<TH1F>("leadJetZMass", "; m_{jz} (Gev);Events", 600, 0, 3000);
  m_subleadJetZMass = histFolder.make<TH1F>("subleadJetZMass", "; m_{jz} (Gev);Events", 600, 0, 3000);

  m_leadLepPtHisto = histFolder.make<TH1F>("leadLepPtHisto" , ";Lead lepton p_{T} (GeV);Events" , 200 , 0 , 1000);
  m_subleadLepPtHisto = histFolder.make<TH1F>("subleadLepPtHisto" , "; Sublead lepton p_{T} (GeV);Events" , 200 , 0 , 1000 );
  m_leadLepEtaHisto = histFolder.make<TH1F>("leadLepEtaHisto" , ";Lead lepton #eta;Events" , 100 , -3 , 3 );
  m_subleadLepEtaHisto = histFolder.make<TH1F>("subleadLepEtaHisto" , "; Sublead lepton #eta;Events" , 100 , -3 , 3 );
  m_leadLepPhiHisto = histFolder.make<TH1F>("leadLepPhiHisto" , ";Lead lepton #phi;Events" , 100 , -4 , 4 );
  m_subleadLepPhiHisto = histFolder.make<TH1F>("subleadLepPhiHisto" , "; Sublead lepton #phi;Events" , 100 , -4 , 4 );

  m_leadJetPtHisto = histFolder.make<TH1F>("leadJetPtHisto" , ";Lead jet p_{T} (GeV);Events" , 200 , 0 , 1000 );
  m_leadJetEtaHisto = histFolder.make<TH1F>("leadJetEtaHisto" , ";Lead jet #eta;Events" , 100 , -3 , 3 );
  m_leadJetPhiHisto = histFolder.make<TH1F>("leadJetPhiHisto" , ";Lead jet #phi;Events" , 100 , -4 , 4 );
  
  m_subleadJetPtHisto = histFolder.make<TH1F>("subleadJetPtHisto" , ";Sublead jet p_{T} (GeV);Events" , 200 , 0 , 1000 );
  m_subleadJetEtaHisto = histFolder.make<TH1F>("subleadJetEtaHisto" , ";Sublead jet #eta;Events" , 100 , -3 , 3 );
  m_subleadJetPhiHisto = histFolder.make<TH1F>("subleadJetPhiHisto" , ";Sublead jet #phi;Events" , 100 , -4 , 4 );
}

void eventHistos::FillHists(eventInfo info, bool binIso, bool makeTuples)
{

     m_eventCount->Fill(0.5,info.eventWeight);

     m_fourObjectInvariantMass->Fill(info.fourObjectInvariantMass, info.eventWeight);

     m_djr0to1_allpartons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
     m_djr1to2_allpartons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);

     if(info.nMEPartons == 0){
          m_djr0to1_0partons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
          m_djr1to2_0partons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);     
     }
     else if(info.nMEPartons == 1){
	  m_djr0to1_1partons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
	  m_djr1to2_1partons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);
     }
     else if(info.nMEPartons == 2){
          m_djr0to1_2partons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
	  m_djr1to2_2partons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);
     }
     else if(info.nMEPartons == 3){
	  m_djr0to1_3partons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
	  m_djr1to2_3partons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);
     } 
     else if(info.nMEPartons == 4){
	  m_djr0to1_4partons->Fill(TMath::Log10(info.DJRValue0), info.eventWeight);
	  m_djr1to2_4partons->Fill(TMath::Log10(info.DJRValue1), info.eventWeight);
     }  

     m_diJetMass->Fill(info.diJetMass, info.eventWeight);
     m_diLepPtHisto->Fill(info.diLepPt, info.eventWeight);
     m_diLepMassHisto->Fill(info.diLepMass, info.eventWeight);
     m_ptllOverMllHisto->Fill(info.ptllOverMll, info.eventWeight);
     m_leadJetZMass->Fill(info.leadJetZMass, info.eventWeight);
     m_subleadJetZMass->Fill(info.subleadJetZMass, info.eventWeight);



     m_number_of_partons->Fill(info.number_of_partons, info.eventWeight);

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

void eventHistos::fillCutFlow(eventInfo info)
{
    for(int i = 0; i < int(info.passCutArray.size()); i++) {
        if(info.passCutArray[i]==true) m_passCutHisto->Fill(i); 
    }
}
