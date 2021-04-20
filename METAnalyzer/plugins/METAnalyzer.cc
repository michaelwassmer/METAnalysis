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
    const double      sample_weight;

    edm::Service< TFileService > fs;

    TTree* tree;

    std::map< std::string, std::unique_ptr< float > > single_float_vars;
    std::map< std::string, std::unique_ptr< int > >   single_int_vars;
    std::map< std::string, std::unique_ptr< long > >  single_long_vars;

    // ----------member functions ---------------------------
    void InitSingleVar(std::string name, std::string type);
    void FillSingleVar(std::string name, float value);
    void FillSingleVar(std::string name, int value);
    void FillSingleVar(std::string name, long value);
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
    era{iConfig.getParameter< std::string >("era")},
    sample_weight{iConfig.getParameter< double >("sample_weight")}

{
    // now do what ever initialization is needed
    tree = fs->make< TTree >("MET_tree", "MET_tree");

    InitSingleVar("evt_run", "L");
    InitSingleVar("evt_lumi", "L");
    InitSingleVar("evt_id", "L");
    
    InitSingleVar("sample_weight", "F");

    InitSingleVar("pt_pfmet_raw", "F");
    InitSingleVar("pt_pfmet_raw_jes_up", "F");
    InitSingleVar("pt_pfmet_raw_jes_down", "F");
    InitSingleVar("pt_pfmet_raw_jer_up", "F");
    InitSingleVar("pt_pfmet_raw_jer_down", "F");
    InitSingleVar("pt_pfmet_raw_jersmear_up", "F");
    InitSingleVar("pt_pfmet_raw_jersmear_down", "F");
    InitSingleVar("pt_pfmet_raw_uncen_up", "F");
    InitSingleVar("pt_pfmet_raw_uncen_down", "F");

    InitSingleVar("pt_pfmet_t1", "F");
    InitSingleVar("pt_pfmet_t1_jes_up", "F");
    InitSingleVar("pt_pfmet_t1_jes_down", "F");
    InitSingleVar("pt_pfmet_t1_jer_up", "F");
    InitSingleVar("pt_pfmet_t1_jer_down", "F");
    InitSingleVar("pt_pfmet_t1_jersmear_up", "F");
    InitSingleVar("pt_pfmet_t1_jersmear_down", "F");
    InitSingleVar("pt_pfmet_t1_uncen_up", "F");
    InitSingleVar("pt_pfmet_t1_uncen_down", "F");

    InitSingleVar("pt_pfmet_t1smear", "F");
    InitSingleVar("pt_pfmet_t1smear_jes_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jes_down", "F");
    InitSingleVar("pt_pfmet_t1smear_jer_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jer_down", "F");
    InitSingleVar("pt_pfmet_t1smear_jersmear_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jersmear_down", "F");
    InitSingleVar("pt_pfmet_t1smear_uncen_up", "F");
    InitSingleVar("pt_pfmet_t1smear_uncen_down", "F");

    InitSingleVar("pt_genmet", "F");
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

    auto pfmet  = hPFMETs->at(0);
    auto genmet = pfmet.genMET();

    FillSingleVar("evt_run", long(iEvent.id().run()));
    FillSingleVar("evt_id", long(iEvent.id().event()));
    FillSingleVar("evt_lumi", long(iEvent.luminosityBlock()));
    
    FillSingleVar("sample_weight", float(sample_weight));

    FillSingleVar("pt_pfmet_raw", float(pfmet.corPt(pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_jes_up", float(pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_jes_down", float(pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_jer_up", float(pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_jer_down", float(pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Raw)));
    // FillSingleVar("pt_pfmet_raw_jersmear_up", float(pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Raw)));
    // FillSingleVar("pt_pfmet_raw_jersmear_down", float(pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_uncen_up", float(pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Raw)));
    FillSingleVar("pt_pfmet_raw_uncen_down", float(pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Raw)));

    FillSingleVar("pt_pfmet_t1", float(pfmet.corPt(pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_jes_up", float(pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_jes_down", float(pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_jer_up", float(pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_jer_down", float(pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1)));
    // FillSingleVar("pt_pfmet_t1_jersmear_up", float(pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1)));
    // FillSingleVar("pt_pfmet_t1_jersmear_down", float(pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_uncen_up", float(pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1)));
    FillSingleVar("pt_pfmet_t1_uncen_down", float(pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1)));

    FillSingleVar("pt_pfmet_t1smear", float(pfmet.corPt(pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_jes_up", float(pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_jes_down", float(pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_jer_up", float(pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_jer_down", float(pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1Smear)));
    // FillSingleVar("pt_pfmet_t1smear_jersmear_up", float(pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1Smear)));
    // FillSingleVar("pt_pfmet_t1smear_jersmear_down", float(pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_uncen_up", float(pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1Smear)));
    FillSingleVar("pt_pfmet_t1smear_uncen_down", float(pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1Smear)));

    FillSingleVar("pt_genmet", float(genmet->pt()));

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

void METAnalyzer::InitSingleVar(std::string name, std::string type)
{
    if (type == "F") {
        single_float_vars.insert({name, std::unique_ptr< float >(new float(-999.0))});
        tree->Branch(name.c_str(), single_float_vars[name].get(), (name + "/F").c_str());
    }
    else if (type == "I") {
        single_int_vars.insert({name, std::unique_ptr< int >(new int(-999.0))});
        tree->Branch(name.c_str(), single_int_vars[name].get(), (name + "/I").c_str());
    }
    else if (type == "L") {
        single_long_vars.insert({name, std::unique_ptr< long >(new long(-999.0))});
        tree->Branch(name.c_str(), single_long_vars[name].get(), (name + "/L").c_str());
    }
    else {
        std::cout << "currently only float (F), int (I), and long(L) types are supported at the moment" << std::endl;
        throw std::exception();
    }
}
void METAnalyzer::FillSingleVar(std::string name, float value) { *single_float_vars[name] = value; }
void METAnalyzer::FillSingleVar(std::string name, int value) { *single_int_vars[name] = value; }
void METAnalyzer::FillSingleVar(std::string name, long value) { *single_long_vars[name] = value; }

// define this as a plug-in
DEFINE_FWK_MODULE(METAnalyzer);
