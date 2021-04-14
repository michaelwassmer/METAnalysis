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

#include "TTree.h"

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

    // different kinds of MET
    TH1D* h_pt_pfmet_t1;
    TH1D* h_pt_pfmet_t1smear;

    // ratios of different METs
    TH1D* h_ratio_pt_pfmet_t1smear_div_pfmet_t1;

    TTree* tree;

    std::map< std::string, std::unique_ptr< float > > single_vars;
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

    h_pt_pfmet_t1      = fs->make< TH1D >("pt_pfmet_t1", "PFMET T1;p_{T};arbitrary units", 20, 0., 500.);
    h_pt_pfmet_t1smear = fs->make< TH1D >("pt_pfmet_t1smear", "PFMET T1Smear;p_{T};arbitrary units", 20, 0., 500.);

    h_ratio_pt_pfmet_t1smear_div_pfmet_t1 =
        fs->make< TH1D >("pt_pfmet_t1smear_div_pfmet_t1", "PFMET T1Smear/T1;p_{T,T1Smear}/p_{T,T1};arbitrary units", 40, 0., 2.);

    tree = fs->make< TTree >("MET_tree", "MET_tree");

    single_vars.insert({"pt_pfmet_t1", std::unique_ptr< float >(new float(-999.0))});
    single_vars.insert({"pt_pfmet_t1smear", std::unique_ptr< float >(new float(-999.0))});

    tree->Branch("pt_pfmet_t1", single_vars["pt_pfmet_t1"].get(), "pt_pfmet_t1/F");
    tree->Branch("pt_pfmet_t1smear", single_vars["pt_pfmet_t1smear"].get(), "pt_pfmet_t1smear/F");
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

    auto pfmet_t1_p4      = hPFMETs->at(0).corP4(pat::MET::Type1);
    auto pfmet_t1smear_p4 = hPFMETs->at(0).corP4(pat::MET::Type1Smear);

    h_pt_pfmet_t1->Fill(pfmet_t1_p4.pt());
    h_pt_pfmet_t1smear->Fill(pfmet_t1smear_p4.pt());

    h_ratio_pt_pfmet_t1smear_div_pfmet_t1->Fill(pfmet_t1smear_p4.pt() / pfmet_t1_p4.pt());

    *single_vars["pt_pfmet_t1"]      = pfmet_t1_p4.pt();
    *single_vars["pt_pfmet_t1smear"] = pfmet_t1smear_p4.pt();

    tree->Fill();
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
