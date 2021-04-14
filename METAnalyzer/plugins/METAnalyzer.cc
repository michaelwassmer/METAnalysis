// -*- C++ -*-
//
// Package:    METAnalysis/METAnalyzer
// Class:      METAnalyzer
//
/**\class METAnalyzer METAnalyzer.cc METAnalysis/METAnalyzer/plugins/METAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Michael Wassmer
//         Created:  Tue, 13 Apr 2021 13:44:39 GMT
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

#include "DataFormats/PatCandidates/interface/MET.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class METAnalyzer : public edm::one::EDAnalyzer< edm::one::SharedResources > {
   public:
    explicit METAnalyzer(const edm::ParameterSet&);
    ~METAnalyzer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    // ----------member data ---------------------------
    edm::EDGetTokenT< std::vector< pat::MET > > EDMPFMETToken;             // PF MET
    edm::EDGetTokenT< std::vector< pat::MET > > EDMPuppiMETToken;          // PUPPI MET
    edm::EDGetTokenT< std::vector< pat::MET > > EDMPFMETOriginalToken;     // PF MET
    edm::EDGetTokenT< std::vector< pat::MET > > EDMPuppiMETOriginalToken;  // PUPPI MET

    const bool        isData;
    const std::string era;

    edm::Service< TFileService > fs;

    TH1D* h_pfmet_pt;
    TH1D* h_pfmet_original_pt;
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
METAnalyzer::METAnalyzer(const edm::ParameterSet& iConfig) :
    EDMPFMETToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_pf"))},
    EDMPuppiMETToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_puppi"))},
    EDMPFMETOriginalToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_pf_original"))},
    EDMPuppiMETOriginalToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_puppi_original"))},
    isData{iConfig.getParameter< bool >("isData")},
    era{iConfig.getParameter< std::string >("era")}

{
    // now do what ever initialization is needed

    h_pfmet_pt          = fs->make< TH1D >("pfmet_pt", "PFMET p_{t}", 20, 0., 500.);
    h_pfmet_original_pt = fs->make< TH1D >("pfmet_original_pt", "PFMET p_{t,orig}", 20, 0., 500.);
}

METAnalyzer::~METAnalyzer()
{
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called for each event  ------------
void METAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    // get slimmedMETs (PFMET)
    edm::Handle< std::vector< pat::MET > > hPFMETs;
    iEvent.getByToken(EDMPFMETToken, hPFMETs);

    edm::Handle< std::vector< pat::MET > > hPFMETsOriginal;
    iEvent.getByToken(EDMPFMETOriginalToken, hPFMETsOriginal);

    auto pfmet_p4          = hPFMETs->at(0).corP4(pat::MET::Type1);
    auto pfmet_original_p4 = hPFMETsOriginal->at(0).corP4(pat::MET::Type1);

    h_pfmet_pt->Fill(pfmet_p4.pt());
    h_pfmet_original_pt->Fill(pfmet_original_p4.pt());
}

// ------------ method called once each job just before starting event loop  ------------
void METAnalyzer::beginJob() {}

// ------------ method called once each job just after ending the event loop  ------------
void METAnalyzer::endJob() {}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void METAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    // The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);

    // Specify that only 'tracks' is allowed
    // To use, remove the default given above and uncomment below
    // ParameterSetDescription desc;
    // desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
    // descriptions.addDefault(desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(METAnalyzer);
