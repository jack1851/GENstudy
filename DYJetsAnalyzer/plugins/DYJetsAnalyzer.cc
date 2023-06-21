// -*- C++ -*-
//
// Package:    GENstudy/DYJetsAnalyzer
// Class:      DYJetsAnalyzer
//
/**\class DYJetsAnalyzer DYJetsAnalyzer.cc GENstudy/DYJetsAnalyzer/plugins/DYJetsAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Michael Krohn
//         Created:  Wed, 11 May 2022 14:27:19 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "GENstudy/DYJetsAnalyzer/interface/eventHistos.h"
#include "GENstudy/DYJetsAnalyzer/interface/eventInfo.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.



class DYJetsAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit DYJetsAnalyzer(const edm::ParameterSet&);
      ~DYJetsAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      double dR2(double eta1, double eta2, double phi1, double phi2);
      double dPhi(double phi1, double phi2);
      double GetDYReshape(double mwr);
      void leptonSelection(eventInfo& myEvent); 
      void genJetSelection(std::vector<const reco::GenJet*> myGenJets, edm::Handle<std::vector<reco::GenJet>> genJets, eventInfo& myEvent);
      void genParticleSelection(edm::Handle<std::vector<reco::GenParticle>> genParticles, eventInfo& myEvent);
      void fillLHEInfo(edm::Handle<LHEEventProduct> LHEInfo, eventInfo& myEvent);

      // ----------member data ---------------------------
      edm::EDGetToken m_genParticleToken;
      edm::EDGetToken m_genJetsToken;
      edm::EDGetToken m_genEventInfoToken;
      edm::EDGetToken m_LHEEventProductToken;

      eventHistos myHistograms;
      bool verbose;
      eventHistos myHistograms_dilepton50M200;
      eventHistos myHistograms_dileptonM200;
      eventHistos myHistograms_dileptonM200to300;
      eventHistos myHistograms_dileptonM300to400;
      eventHistos myHistograms_dileptonM400;
      eventHistos myHistograms_mlljj_le_2tev;
      eventHistos myHistograms_mlljj_gr_2tev;
      TH1D * hist_DYReshape_Resolved_ratio_AllCh;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DYJetsAnalyzer::DYJetsAnalyzer(const edm::ParameterSet& iConfig)
 :
  m_genParticleToken(consumes<std::vector<reco::GenParticle>> (iConfig.getParameter<edm::InputTag>("genParticles"))),
  m_genJetsToken(consumes<std::vector<reco::GenJet>> (iConfig.getParameter<edm::InputTag>("genJets"))),
  m_genEventInfoToken (consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>("genInfo"))),
  m_LHEEventProductToken (consumes<LHEEventProduct> (iConfig.getParameter<edm::InputTag>("LHEInfo")))
{
   //now do what ever initialization is needed
  usesResource("TFileService");
  edm::Service<TFileService> fs;

  myHistograms.book(fs->mkdir("allPassingEvents"),false);
  myHistograms_dilepton50M200.book(fs->mkdir("allPassingEvents_dilepton50M200"),false);
  myHistograms_dileptonM200.book(fs->mkdir("allPassingEvents_dileptonM200"),false);
  myHistograms_dileptonM200to300.book(fs->mkdir("allPassingEvents_dileptonM200to300"),false);
  myHistograms_dileptonM300to400.book(fs->mkdir("allPassingEvents_dileptonM300to400"),false);
  myHistograms_dileptonM400.book(fs->mkdir("allPassingEvents_dileptonM400"),false);
  myHistograms_mlljj_le_2tev.book(fs->mkdir("allPassingEvents_mlljj_le_2tev"),false);
  myHistograms_mlljj_gr_2tev.book(fs->mkdir("allPassingEvents_mlljj_gr_2tev"),false);
  verbose = false;
  TFile *file_DYReshape = new TFile("/data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/plugins/DYReshapeJetPt_2018.root");
  hist_DYReshape_Resolved_ratio_AllCh = (TH1D *)file_DYReshape->Get("Resolved_ratio_AllCh");
}


DYJetsAnalyzer::~DYJetsAnalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
DYJetsAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
//   std::cout << "STARTING NEW EVENT" <<std::endl;
   if (verbose) std::cout << "STARTING NEW EVENT" <<std::endl;
   using namespace edm;

   eventInfo myEvent;

   edm::Handle<GenEventInfoProduct> genEventInfo;
   iEvent.getByToken(m_genEventInfoToken, genEventInfo);

   myEvent.eventWeight = genEventInfo->weight()/fabs(genEventInfo->weight());
   myHistograms.m_eventWeight->Fill(myEvent.eventWeight);
   
   myEvent.nMEPartons = genEventInfo->nMEPartons();

//   std::cout << "The size of the DJRValues vector is: " << genEventInfo->DJRValues().size() << std::endl;

  if(genEventInfo->DJRValues().size()>0){
     for(size_t i = 0; i < genEventInfo->DJRValues().size(); i++) {
         if(i == 0){
		 myEvent.DJRValue0 = genEventInfo->DJRValues()[0];}
	 else if(i == 1){
                 myEvent.DJRValue1 = genEventInfo->DJRValues()[1];}
	 else if(i == 2){
		 myEvent.DJRValue2 = genEventInfo->DJRValues()[2];}
	 else if(i == 3){
	         myEvent.DJRValue3 = genEventInfo->DJRValues()[3];}
	 else if(i == 4){
		 myEvent.DJRValue4 = genEventInfo->DJRValues()[4];}
     }
  }

//if(genEventInfo->DJRValues().size()>0){
//   std::cout << "DJR 0: " <<  myEvent.DJRValue0 << std::endl;
//   std::cout << "DJR 1: " <<  myEvent.DJRValue1 << std::endl;
//   std::cout << "DJR 2: " <<  myEvent.DJRValue2 << std::endl;
//   std::cout << "DJR 3: " <<  myEvent.DJRValue3 << std::endl;
//   std::cout << "DJR 4: " <<  myEvent.DJRValue4 << std::endl;
//}
   edm::Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

  // const reco::GenParticle* lepton1   = 0;
  //const reco::GenParticle* lepton2      = 0;

//   std::cout << "Calling genParicleSelection: " << std::endl;

   genParticleSelection(genParticles, myEvent);

   if(myEvent.lepton1 == 0 || myEvent.lepton2 == 0) {
     myEvent.passCutArray[0] = false; 
     return;
   }
   
//   std::cout << "Calling leptonSelection: " << std::endl;

   leptonSelection(myEvent);
 
   edm::Handle<std::vector<reco::GenJet>> genJets;
   iEvent.getByToken(m_genJetsToken, genJets);

   //std::vector<const reco::GenJet*> myGenJets;

   genJetSelection(myEvent.myGenJets,genJets, myEvent);

  if(verbose) std::cout << "Gen lepton1: " << myEvent.lepton1->p4() << ", "<< myEvent.lepton1->pt() << std::endl;
  if(verbose) std::cout << "Gen lepton2: " << myEvent.lepton2->p4() << ", "<< myEvent.lepton2->pt() << std::endl;

  if(myEvent.myGenJets.size() < 2) {
     myEvent.passCutArray[5] = false;
     myHistograms.fillCutFlow(myEvent);
     return;
  }
  
//  myEvent.eventWeight = myEvent.eventWeight*GetDYReshape(myEvent.myGenJets[0]->pt());
//  myHistograms.m_eventWeight->Fill(myEvent.eventWeight);

  myHistograms.fillCutFlow(myEvent); 

  //Checking Lepton Selections
  for(int i = 0; i < int(myEvent.passCutArray.size()); i++){
       if (myEvent.passCutArray[i] == false) return;
  }
 
//  std::cout << "Gen Jet1: " << myEvent.myGenJets[0]->p4() << ", "<< myEvent.myGenJets[0]->pt() << std::endl;
//  std::cout << "Gen Jet2: " << myEvent.myGenJets[1]->p4() << ", "<< myEvent.myGenJets[1]->pt() << std::endl;
  
  if (myEvent.myGenJets[0]->pt() < myEvent.myGenJets[1]->pt()) std::cout << "Warning: Sublead jet pt greater than lead jet pt" << std::endl; //CHECKS LEAD VS. SUBLEAD JET PT
  if (verbose) std::cout << "lepton1 eta: " << myEvent.lepton1->eta() << std::endl;

  myEvent.fourObjectInvariantMass = (myEvent.lepton1->p4() + myEvent.lepton2->p4() + myEvent.myGenJets[0]->p4() + myEvent.myGenJets[1]->p4()).mass();

  myEvent.diJetMass = (myEvent.myGenJets[0]->p4() + myEvent.myGenJets[1]->p4()).mass();
  myEvent.diLepPt = (myEvent.lepton1->p4() + myEvent.lepton2->p4()).pt();
  myEvent.diLepMass = (myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass();
  myEvent.ptllOverMll = (myEvent.lepton1->p4() + myEvent.lepton2->p4()).pt()/(myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass();
  myEvent.leadJetZMass = (myEvent.lepton1->p4() + myEvent.lepton2->p4() + myEvent.myGenJets[0]->p4()).mass();
  myEvent.subleadJetZMass = (myEvent.lepton1->p4() + myEvent.lepton2->p4() + myEvent.myGenJets[1]->p4()).mass();

  myEvent.lepton1Eta = myEvent.lepton1->eta();
  myEvent.lepton1Phi = myEvent.lepton1->phi();
  myEvent.lepton1Pt = myEvent.lepton1->pt();

  myEvent.lepton2Eta = myEvent.lepton2->eta();
  myEvent.lepton2Phi = myEvent.lepton2->phi();
  myEvent.lepton2Pt = myEvent.lepton2->pt();

  myEvent.leadJetEta = myEvent.myGenJets[0]->eta();
  myEvent.leadJetPhi = myEvent.myGenJets[0]->phi();
  myEvent.leadJetPt = myEvent.myGenJets[0]->pt();

  myEvent.subleadJetEta = myEvent.myGenJets[1]->eta();
  myEvent.subleadJetPhi = myEvent.myGenJets[1]->phi();
  myEvent.subleadJetPt = myEvent.myGenJets[1]->pt();

  std::cout << "BEGIN EVENT:" << std::endl;
  std::cout << "LEPTON KINEMATICS" << std::endl;
  std::cout << "Leading Lepton 4-momentum vector: " << myEvent.lepton1->p4() << std::endl;
  std::cout << "Subleading Lepton 4-momentum vector: " << myEvent.lepton2->p4() << std::endl;
  std::cout << "Dilepton 4-momentum vector: " << myEvent.lepton1->p4() + myEvent.lepton2->p4() << std::endl;
  std::cout << "Dilepton Mass: m_{ll} = " << myEvent.diLepMass << " GeV" << std::endl;
  std::cout << "Dilepton p_T: pT_{ll} = " << myEvent.diLepPt << " GeV" << std::endl;
  std::cout << "pt_{ll}/m_{ll} = " << myEvent.ptllOverMll << std::endl;
  std::cout << "Leading Lepton Pt  = " << myEvent.lepton1Pt << " GeV" << std::endl;
  std::cout << "Subleading Lepton Pt  = " << myEvent.lepton2Pt << " GeV" <<  std::endl;
  std::cout << "Leading Lepton Eta  = " << myEvent.lepton1Eta << std::endl;
  std::cout << "Subleading Lepton Eta  = " << myEvent.lepton2Eta << std::endl;
  std::cout << "Leading Lepton Phi  = " << myEvent.lepton1Phi << std::endl;
  std::cout << "Subleading Lepton Phi  = " << myEvent.lepton2Phi << std::endl;

  std::cout << " " << std::endl;

  std::cout << "JET KINEMATICS" << std::endl;
  std::cout << "Leading Jet 4-vector: " << myEvent.myGenJets[0]->p4() << std::endl;
  std::cout << "Subleading Jet 4-vector: " << myEvent.myGenJets[1]->p4() << std::endl;
  std::cout << "Z boson 4-momentum vector: " << myEvent.lepton1->p4() + myEvent.lepton2->p4() << std::endl;
  std::cout << "DiJet Mass: m_{jj} = " << myEvent.diJetMass << " GeV" << std::endl;
  std::cout << "Leading Jet Pt : " << myEvent.leadJetPt << " GeV" << std::endl;
  std::cout << "Subleading Jet Pt: " << myEvent.subleadJetPt << " GeV" << std::endl;
  std::cout << "DiJet Pt: " << (myEvent.myGenJets[0]->p4() + myEvent.myGenJets[1]->p4()).pt() << " GeV" << std::endl;
  std::cout << "Four Object Invariant Mass: m_{lljj} = " << myEvent.fourObjectInvariantMass << " GeV" << std::endl;
  std::cout << "Lead Jet + Z Mass = " << myEvent.leadJetZMass << " GeV" << std::endl;
  std::cout << "Sublead Jet + Z Mass = " << myEvent.subleadJetZMass << " GeV" << std::endl;
  std::cout << "Leading Jet Eta  = " << myEvent.leadJetEta << std::endl;
  std::cout << "Subleading Jet Eta  = " << myEvent.subleadJetEta << std::endl;
  std::cout << "Leading Jet Phi  = " << myEvent.leadJetPhi << std::endl;
  std::cout << "Subleading Jet Phi  = " << myEvent.subleadJetPhi << std::endl;
  std::cout << "END EVENT " << std::endl;
  std::cout << " " << std::endl;

  edm::Handle<LHEEventProduct> LHE_Info;
  iEvent.getByToken(m_LHEEventProductToken, LHE_Info);
  if(!LHE_Info.isValid()) {
   std::cout << "LHE INFO NOT FOUND" << std::endl;
  return;
  }

  fillLHEInfo(LHE_Info, myEvent);

  myHistograms.FillHists(myEvent);

  if((myEvent.lepton1->p4() + myEvent.lepton2->p4() + myEvent.myGenJets[0]->p4() + myEvent.myGenJets[1]->p4()).mass() < 2000){
    myHistograms_mlljj_le_2tev.FillHists(myEvent);
  }else if((myEvent.lepton1->p4() + myEvent.lepton2->p4() + myEvent.myGenJets[0]->p4() + myEvent.myGenJets[1]->p4()).mass() > 2000){
    myHistograms_mlljj_gr_2tev.FillHists(myEvent);
  }

  if((myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() > 50 && (myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() < 200){
    myHistograms_dilepton50M200.FillHists(myEvent);
  }else if((myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() > 200){
    myHistograms_dileptonM200.FillHists(myEvent);
  }


  if((myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() > 200 && (myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() < 300){
    myHistograms_dileptonM200to300.FillHists(myEvent);
  }else if((myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() > 300 && (myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() < 400){
    myHistograms_dileptonM300to400.FillHists(myEvent);
  }else if((myEvent.lepton1->p4() + myEvent.lepton2->p4()).mass() > 400){
    myHistograms_dileptonM400.FillHists(myEvent);
  }

  }

// ------------ method called once each job just before starting event loop  ------------
void
DYJetsAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
DYJetsAnalyzer::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DYJetsAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//HELPERS
double DYJetsAnalyzer::dR2(double eta1, double eta2, double phi1, double phi2) {
    double deta = eta1 - eta2;
    double dphi = dPhi(phi1, phi2);
    return deta*deta + dphi*dphi;
 }
double DYJetsAnalyzer::dPhi(double phi1, double phi2) {
    double raw_dphi = phi1 - phi2;
    if (fabs(raw_dphi) < ROOT::Math::Pi()) return raw_dphi;
    double region = std::round(raw_dphi / (2.*ROOT::Math::Pi()));
    return raw_dphi - 2.*ROOT::Math::Pi()*region;
}

//double DYJetsAnalyzer::GetDYReshape(double mwr){
//   mwr is the jet pT
//     if(mwr>=2000.) mwr=2000.;
//
//     int this_bin = -1;
//     this_bin = hist_DYReshape_Resolved_ratio_AllCh->FindBin(mwr);
//     return hist_DYReshape_Resolved_ratio_AllCh->GetBinContent(this_bin);
//}

void DYJetsAnalyzer::leptonSelection(eventInfo& my_Event) { 
   if(my_Event.lepton1->pt()<my_Event.lepton2->pt()) std::swap(my_Event.lepton1,my_Event.lepton2); //CHECKS LEAD VS. SUBLEAD LEPTON PT
   if(my_Event.lepton1->pt()<80) my_Event.passCutArray[1]=false;
   if(my_Event.lepton2->pt()<80) my_Event.passCutArray[2]=false;
   if(fabs(my_Event.lepton1->eta())>2.4) my_Event.passCutArray[3]=false;
   if(fabs(my_Event.lepton2->eta())>2.4) my_Event.passCutArray[4]=false;   
}

void DYJetsAnalyzer::genJetSelection(std::vector<const reco::GenJet*> my_Gen_Jets, edm::Handle<std::vector<reco::GenJet>> gen_Jets, eventInfo& my_Event) {
      for (std::vector<reco::GenJet>::const_iterator iJet = gen_Jets->begin(); iJet != gen_Jets->end(); iJet++) {
   // std::cout << "GEN JET pt: " << iJet->pt() << ", eta: " << iJet->eta() << std::endl;
      if (iJet->pt()<30.0) continue;
      if (fabs(iJet->eta()) > 2.4) continue;
      if (sqrt(dR2(my_Event.lepton1->eta(), iJet->eta(), my_Event.lepton1->phi(), iJet->phi())) < 0.4) continue;
      if (sqrt(dR2(my_Event.lepton2->eta(), iJet->eta(), my_Event.lepton2->phi(), iJet->phi())) < 0.4) continue;
      my_Event.myGenJets.push_back(&(*iJet));
  }
}

void DYJetsAnalyzer::genParticleSelection(edm::Handle<std::vector<reco::GenParticle>> gen_Particles, eventInfo& my_Event) {
   bool verbose = false; 
   for (std::vector<reco::GenParticle>::const_iterator iParticle = gen_Particles->begin(); iParticle != gen_Particles->end(); iParticle++) {
       if( ! iParticle->isHardProcess() ) continue;  //ONLY HARD PROCESS AND NOT INCOMING
       if( iParticle->status() == 21 ) continue;
       if (verbose) std::cout << "STATUS: " << iParticle->status() << " PDGID: " << iParticle->pdgId() << " MOTHER: " << iParticle->mother()->pdgId() << std::endl;
         if( abs( iParticle->pdgId() ) == 13 || abs( iParticle->pdgId() ) == 11 ){ //HERE'S A LEPtON
           if(my_Event.lepton1 == 0){
             my_Event.lepton1 = &(*iParticle);
           }
           if(my_Event.lepton1 != 0){
             my_Event.lepton2 = &(*iParticle);
           }
         }
   }
}

void DYJetsAnalyzer::fillLHEInfo(edm::Handle<LHEEventProduct> LHEInfo, eventInfo& my_Event)
{
 //==== LHE object
 const lhef::HEPEUP& lheEvent = LHEInfo->hepeup();
 std::vector<lhef::HEPEUP::FiveVector> lheParticles = lheEvent.PUP;
 int count = 0;
 for(size_t idxParticle = 0; idxParticle<lheParticles.size(); ++idxParticle){
   if (lheEvent.ISTUP[idxParticle] == 1){
     if (lheEvent.IDUP[idxParticle] == 1 or lheEvent.IDUP[idxParticle] == -1 or lheEvent.IDUP[idxParticle] == 2 or lheEvent.IDUP[idxParticle] == -2 or lheEvent.IDUP[idxParticle] == 3 or lheEvent.IDUP[idxParticle] == -3 or lheEvent.IDUP[idxParticle] == 4 or lheEvent.IDUP[idxParticle] == -4 or lheEvent.IDUP[idxParticle] == 5 or lheEvent.IDUP[idxParticle] == -5 or lheEvent.IDUP[idxParticle] == 21 or lheEvent.IDUP[idxParticle] == -21){ 
  //std::cout << "Particle ID: " << lheEvent.IDUP[idxParticle] << std::endl;
    count = count + 1;
    }
  }
    
  } 
 my_Event.number_of_partons = count;
 }

//define this as a plug-in
DEFINE_FWK_MODULE(DYJetsAnalyzer);
