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

      // ----------member data ---------------------------
      edm::EDGetToken m_genParticleToken;
      edm::EDGetToken m_genJetsToken;
      edm::EDGetToken m_genEventInfoToken;

      eventHistos myHistograms;

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
  m_genEventInfoToken (consumes<GenEventInfoProduct> (iConfig.getParameter<edm::InputTag>("genInfo")))
{
   //now do what ever initialization is needed
  usesResource("TFileService");
  edm::Service<TFileService> fs;

  myHistograms.book(fs->mkdir("allEvents"),false);

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
   std::cout << "STARTING NEW EVENT" <<std::endl;
   using namespace edm;

   eventInfo myEvent;

   edm::Handle<GenEventInfoProduct> genEventInfo;
   iEvent.getByToken(m_genEventInfoToken, genEventInfo);

   myEvent.eventWeight = genEventInfo->weight()/fabs(genEventInfo->weight());


   edm::Handle<std::vector<reco::GenParticle>> genParticles;
   iEvent.getByToken(m_genParticleToken, genParticles);

   const reco::GenParticle* lepton1   = 0;
   const reco::GenParticle* lepton2      = 0;

   for (std::vector<reco::GenParticle>::const_iterator iParticle = genParticles->begin(); iParticle != genParticles->end(); iParticle++) {
       if( ! iParticle->isHardProcess() ) continue;  //ONLY HARD PROCESS AND NOT INCOMING
       if( iParticle->status() == 21 ) continue;
       std::cout << "STATUS: " << iParticle->status() << " PDGID: " << iParticle->pdgId() << " MOTHER: " << iParticle->mother()->pdgId() << std::endl;
       if( abs( iParticle->mother()->pdgId() ) ==23) {//CAME FROM A Z
         if( abs( iParticle->pdgId() ) == 13 || abs( iParticle->pdgId() ) == 11 ){ //HERE'S A LEPtON
	   if(lepton1 == 0){
             lepton1 = &(*iParticle);
	   }
	   if(lepton1 != 0){
	     lepton2 = &(*iParticle);
	   }
	 }
       }
   }

   if(lepton1 == 0 || lepton2 == 0) return;
   //if(lepton1->pt()<60) return;
   //if(lepton2->pt()<53) return;
   if(lepton1->pt()<27) return;
   if(lepton2->pt()<27) return;
   if(fabs(lepton1->eta())>2.4) return;
   if(fabs(lepton2->eta())>2.4) return;
   std::cout << "Gen lepton1: " << lepton1->p4() << ", "<< lepton1->pt() << std::endl;
   std::cout << "Gen lepton2: " << lepton2->p4() << ", "<< lepton2->pt() << std::endl;


  edm::Handle<std::vector<reco::GenJet>> genJets;
  iEvent.getByToken(m_genJetsToken, genJets);

  std::vector<const reco::GenJet*> myGenJets;

  for (std::vector<reco::GenJet>::const_iterator iJet = genJets->begin(); iJet != genJets->end(); iJet++) {
    //std::cout << "GEN JET pt: " << iJet->pt() << ", eta: " << iJet->eta() << std::endl;
    if (iJet->pt()<30.0) continue;
    if (fabs(iJet->eta()) > 2.4) continue;
      myGenJets.push_back(&(*iJet));
  }

  if(myGenJets.size() < 2) return;
  std::cout << "Gen Jet1: " << myGenJets[0]->p4() << ", "<< myGenJets[0]->pt() << std::endl;
  std::cout << "Gen Jet2: " << myGenJets[1]->p4() << ", "<< myGenJets[1]->pt() << std::endl;

  std::cout << "lepton1 eta: " << lepton1->eta() << std::endl;

  myEvent.fourObjectInvariantMass = (lepton1->p4() + lepton2->p4() + myGenJets[0]->p4() + myGenJets[1]->p4()).mass();

  myEvent.lepton1Eta = lepton1->eta();
  myEvent.lepton1Phi = lepton1->phi();
  myEvent.lepton1Pt = lepton1->pt();

  myEvent.lepton2Eta = lepton2->eta();
  myEvent.lepton2Phi = lepton2->phi();
  myEvent.lepton2Pt = lepton2->pt();

  myEvent.leadJetEta = myGenJets[0]->eta();
  myEvent.leadJetPhi = myGenJets[0]->phi();
  myEvent.leadJetPt = myGenJets[0]->pt();

  myEvent.subleadJetEta = myGenJets[1]->eta();
  myEvent.subleadJetPhi = myGenJets[1]->phi();
  myEvent.subleadJetPt = myGenJets[1]->pt();

  myHistograms.FillHists(myEvent);
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

//define this as a plug-in
DEFINE_FWK_MODULE(DYJetsAnalyzer);
