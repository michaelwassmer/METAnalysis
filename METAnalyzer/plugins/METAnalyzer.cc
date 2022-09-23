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
//#include "DataFormats/PatCandidates/interface/Muon.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/VertexReco/interface/Vertex.h"

#include "TTree.h"
#include "TH1D.h"
#include "Math/Vector4D.h"

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
    edm::EDGetTokenT< GenEventInfoProduct >     EDMGenEventInfoToken;
    edm::EDGetTokenT< std::vector< reco::Vertex > > EDMPrimaryVertexToken; // Primary Vertices
    //edm::EDGetTokenT< std::vector< pat::Muon > > EDMLooseMuonToken; // Muon Collection

    // some useful information to keep
    const bool        isData;
    const std::string era;
    const double      sample_weight;

    // file service object to write output root file
    edm::Service< TFileService > fs;

    // outputs to write into the output root file
    TTree* tree;
    TH1D* sumw;
    TH1D* sume;

    // containers to hold singleton variables within an event
    std::map< std::string, std::unique_ptr< float > > single_float_vars;
    std::map< std::string, std::unique_ptr< int > >   single_int_vars;
    std::map< std::string, std::unique_ptr< long > >  single_long_vars;

    // containers to hold vector variables within an event
    std::map< std::string, std::unique_ptr< std::vector< float > > > vector_float_vars;
    std::map< std::string, std::unique_ptr< std::vector< int > > > vector_int_vars;
    std::map< std::string, std::unique_ptr< std::vector< ROOT::Math::XYZTVector > > > vector_tlorentz_vars;

    // ----------member functions ---------------------------

    // function to initialize a singleton variable
    void InitSingleVar(std::string name, std::string type);
    void InitVectorVar(std::string name, std::string type, size_t n_elements = 5);
    // functions to write singleton variables of different types into the containers defined above
    void FillSingleVar(std::string name, float value);
    void FillSingleVar(std::string name, double value);
    void FillSingleVar(std::string name, int value);
    void FillSingleVar(std::string name, long value);
    void FillVectorVar(std::string name, std::vector< float > vector);
    void FillVectorVar(std::string name, std::vector< int > vector);
    void FillVectorVar(std::string name, std::vector< ROOT::Math::XYZTVector > vector);
};

METAnalyzer::METAnalyzer(const edm::ParameterSet& iConfig) :
    EDMPFMETToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_pf"))},
    EDMPuppiMETToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_puppi"))},
    EDMPFMETOriginalToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_pf_original"))},
    EDMPuppiMETOriginalToken{consumes< std::vector< pat::MET > >(iConfig.getParameter< edm::InputTag >("met_puppi_original"))},
    EDMPrimaryVertexToken{consumes< std::vector< reco::Vertex > >(iConfig.getParameter< edm::InputTag >("primary_vertices"))},
    //EDMLooseMuonToken{consumes< std::vector< pat::Muon > >(iConfig.getParameter< edm::InputTag >("loose_muons"))},
    isData{iConfig.getParameter< bool >("isData")},
    era{iConfig.getParameter< std::string >("era")},
    sample_weight{iConfig.getParameter< double >("sample_weight")}

{
    // now do what ever initialization is needed

    // only read generator event info if we deal with simulation
    if(not isData){
        EDMGenEventInfoToken = consumes< GenEventInfoProduct >(iConfig.getParameter< edm::InputTag >("gen_event_info"));
    }

    // output tree to write to file
    tree = fs->make< TTree >("MET_tree", "MET_tree");

    // histogram to contain sum of all generator weights
    sumw = fs->make< TH1D >("sum_of_genweights", "sum_of_genweights", 1, 0, 2);
    sumw->Sumw2();
    // histogram to contain sum of all events
    sume = fs->make< TH1D >("sum_of_events", "sum_of_events", 1, 0, 2);

    // event identification information
    InitSingleVar("evt_run", "L");
    InitSingleVar("evt_lumi", "L");
    InitSingleVar("evt_id", "L");

    // necessary weights for sample normalization
    InitSingleVar("sample_weight", "F");
    InitSingleVar("generator_weight", "F");

    // raw pfmet quantities
    InitSingleVar("pt_pfmet_raw", "F");
    InitSingleVar("pt_pfmet_raw_jes_up", "F");
    InitSingleVar("pt_pfmet_raw_jes_down", "F");
    InitSingleVar("pt_pfmet_raw_jer_up", "F");
    InitSingleVar("pt_pfmet_raw_jer_down", "F");
    InitSingleVar("pt_pfmet_raw_jersmear_up", "F");
    InitSingleVar("pt_pfmet_raw_jersmear_down", "F");
    InitSingleVar("pt_pfmet_raw_uncen_up", "F");
    InitSingleVar("pt_pfmet_raw_uncen_down", "F");
    InitSingleVar("pt_pfmet_raw_ele_up", "F");
    InitSingleVar("pt_pfmet_raw_ele_down", "F");
    InitSingleVar("pt_pfmet_raw_muo_up", "F");
    InitSingleVar("pt_pfmet_raw_muo_down", "F");
    InitSingleVar("pt_pfmet_raw_tau_up", "F");
    InitSingleVar("pt_pfmet_raw_tau_down", "F");
    InitSingleVar("pt_pfmet_raw_pho_up", "F");
    InitSingleVar("pt_pfmet_raw_pho_down", "F");

    InitSingleVar("phi_pfmet_raw", "F");

    // type1 corrected pfmet quantities
    InitSingleVar("pt_pfmet_t1", "F");
    InitSingleVar("pt_pfmet_t1_jes_up", "F");
    InitSingleVar("pt_pfmet_t1_jes_down", "F");
    InitSingleVar("pt_pfmet_t1_jer_up", "F");
    InitSingleVar("pt_pfmet_t1_jer_down", "F");
    InitSingleVar("pt_pfmet_t1_jersmear_up", "F");
    InitSingleVar("pt_pfmet_t1_jersmear_down", "F");
    InitSingleVar("pt_pfmet_t1_uncen_up", "F");
    InitSingleVar("pt_pfmet_t1_uncen_down", "F");
    InitSingleVar("pt_pfmet_t1_ele_up", "F");
    InitSingleVar("pt_pfmet_t1_ele_down", "F");
    InitSingleVar("pt_pfmet_t1_muo_up", "F");
    InitSingleVar("pt_pfmet_t1_muo_down", "F");
    InitSingleVar("pt_pfmet_t1_tau_up", "F");
    InitSingleVar("pt_pfmet_t1_tau_down", "F");
    InitSingleVar("pt_pfmet_t1_pho_up", "F");
    InitSingleVar("pt_pfmet_t1_pho_down", "F");

    InitSingleVar("phi_pfmet_t1", "F");

    // type1 + smearing corrected pfmet quantities
    InitSingleVar("pt_pfmet_t1smear", "F");
    InitSingleVar("pt_pfmet_t1smear_jes_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jes_down", "F");
    InitSingleVar("pt_pfmet_t1smear_jer_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jer_down", "F");
    InitSingleVar("pt_pfmet_t1smear_jersmear_up", "F");
    InitSingleVar("pt_pfmet_t1smear_jersmear_down", "F");
    InitSingleVar("pt_pfmet_t1smear_uncen_up", "F");
    InitSingleVar("pt_pfmet_t1smear_uncen_down", "F");
    InitSingleVar("pt_pfmet_t1smear_ele_up", "F");
    InitSingleVar("pt_pfmet_t1smear_ele_down", "F");
    InitSingleVar("pt_pfmet_t1smear_muo_up", "F");
    InitSingleVar("pt_pfmet_t1smear_muo_down", "F");
    InitSingleVar("pt_pfmet_t1smear_tau_up", "F");
    InitSingleVar("pt_pfmet_t1smear_tau_down", "F");
    InitSingleVar("pt_pfmet_t1smear_pho_up", "F");
    InitSingleVar("pt_pfmet_t1smear_pho_down", "F");

    InitSingleVar("phi_pfmet_t1smear", "F");

    // type1 + xy corrected pfmet quantities
    InitSingleVar("pt_pfmet_t1xy", "F");

    InitSingleVar("phi_pfmet_t1xy", "F");

    // raw puppimet quantities
    InitSingleVar("pt_puppimet_raw", "F");
    InitSingleVar("pt_puppimet_raw_jes_up", "F");
    InitSingleVar("pt_puppimet_raw_jes_down", "F");
    InitSingleVar("pt_puppimet_raw_jer_up", "F");
    InitSingleVar("pt_puppimet_raw_jer_down", "F");
    InitSingleVar("pt_puppimet_raw_jersmear_up", "F");
    InitSingleVar("pt_puppimet_raw_jersmear_down", "F");
    InitSingleVar("pt_puppimet_raw_uncen_up", "F");
    InitSingleVar("pt_puppimet_raw_uncen_down", "F");
    InitSingleVar("pt_puppimet_raw_ele_up", "F");
    InitSingleVar("pt_puppimet_raw_ele_down", "F");
    InitSingleVar("pt_puppimet_raw_muo_up", "F");
    InitSingleVar("pt_puppimet_raw_muo_down", "F");
    InitSingleVar("pt_puppimet_raw_tau_up", "F");
    InitSingleVar("pt_puppimet_raw_tau_down", "F");
    InitSingleVar("pt_puppimet_raw_pho_up", "F");
    InitSingleVar("pt_puppimet_raw_pho_down", "F");

    InitSingleVar("phi_puppimet_raw", "F");

    // type1 corrected puppimet quantities
    InitSingleVar("pt_puppimet_t1", "F");
    InitSingleVar("pt_puppimet_t1_jes_up", "F");
    InitSingleVar("pt_puppimet_t1_jes_down", "F");
    InitSingleVar("pt_puppimet_t1_jer_up", "F");
    InitSingleVar("pt_puppimet_t1_jer_down", "F");
    InitSingleVar("pt_puppimet_t1_jersmear_up", "F");
    InitSingleVar("pt_puppimet_t1_jersmear_down", "F");
    InitSingleVar("pt_puppimet_t1_uncen_up", "F");
    InitSingleVar("pt_puppimet_t1_uncen_down", "F");
    InitSingleVar("pt_puppimet_t1_ele_up", "F");
    InitSingleVar("pt_puppimet_t1_ele_down", "F");
    InitSingleVar("pt_puppimet_t1_muo_up", "F");
    InitSingleVar("pt_puppimet_t1_muo_down", "F");
    InitSingleVar("pt_puppimet_t1_tau_up", "F");
    InitSingleVar("pt_puppimet_t1_tau_down", "F");
    InitSingleVar("pt_puppimet_t1_pho_up", "F");
    InitSingleVar("pt_puppimet_t1_pho_down", "F");

    InitSingleVar("phi_puppimet_t1", "F");

    // type1 + xy corrected puppimet quantities
    InitSingleVar("pt_puppimet_t1xy", "F");

    InitSingleVar("phi_puppimet_t1xy", "F");

    // type1 + smearing corrected puppimet quantities
    InitSingleVar("pt_puppimet_t1smear", "F");
    InitSingleVar("pt_puppimet_t1smear_jes_up", "F");
    InitSingleVar("pt_puppimet_t1smear_jes_down", "F");
    InitSingleVar("pt_puppimet_t1smear_jer_up", "F");
    InitSingleVar("pt_puppimet_t1smear_jer_down", "F");
    InitSingleVar("pt_puppimet_t1smear_jersmear_up", "F");
    InitSingleVar("pt_puppimet_t1smear_jersmear_down", "F");
    InitSingleVar("pt_puppimet_t1smear_uncen_up", "F");
    InitSingleVar("pt_puppimet_t1smear_uncen_down", "F");
    InitSingleVar("pt_puppimet_t1smear_ele_up", "F");
    InitSingleVar("pt_puppimet_t1smear_ele_down", "F");
    InitSingleVar("pt_puppimet_t1smear_muo_up", "F");
    InitSingleVar("pt_puppimet_t1smear_muo_down", "F");
    InitSingleVar("pt_puppimet_t1smear_tau_up", "F");
    InitSingleVar("pt_puppimet_t1smear_tau_down", "F");
    InitSingleVar("pt_puppimet_t1smear_pho_up", "F");
    InitSingleVar("pt_puppimet_t1smear_pho_down", "F");

    InitSingleVar("phi_puppimet_t1smear", "F");

    // generator met
    InitSingleVar("pt_genmet", "F");
    InitSingleVar("phi_genmet", "F");

    // number of primary vertices
    InitSingleVar("n_primary_vertices", "I");
    InitSingleVar("n_good_primary_vertices", "I");

    // loose muons
    //InitVectorVar("loose_muons_p4", "LorentzVector");

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

    // get pfmet pat::MET object
    edm::Handle< std::vector< pat::MET > > hPFMETs;
    iEvent.getByToken(EDMPFMETToken, hPFMETs);

    // get puppimet pat::MET object
    edm::Handle< std::vector< pat::MET > > hPuppiMETs;
    iEvent.getByToken(EDMPuppiMETToken, hPuppiMETs);

    // get primary vertex collection
    edm::Handle< std::vector< reco::Vertex > > hPVs;
    iEvent.getByToken(EDMPrimaryVertexToken, hPVs);

    // get muon collection
    //edm::Handle< std::vector< pat::Muon > > hLooseMuons;
    //iEvent.getByToken(EDMLooseMuonToken, hLooseMuons);

    // get generator event info object to retrieve generator weight
    edm::Handle< GenEventInfoProduct > hGenEventInfo;
    float generator_weight = 1.0;
    if(not isData){
        iEvent.getByToken(EDMGenEventInfoToken, hGenEventInfo);
        auto geneventinfo = *hGenEventInfo;
        generator_weight = geneventinfo.weight();
    }

    // retrieve pfmet pat::MET object
    auto pfmet        = hPFMETs->at(0);
    // retrieve puppimet pat::MET object
    auto puppimet        = hPuppiMETs->at(0);
    // retrieve genmet object
    auto genmet       = pfmet.genMET();

    // event identification information
    FillSingleVar("evt_run", long(iEvent.id().run()));
    FillSingleVar("evt_id", long(iEvent.id().event()));
    FillSingleVar("evt_lumi", long(iEvent.luminosityBlock()));

    // number of primary vertices
    int n_primary_vertices = hPVs->size();
    FillSingleVar("n_primary_vertices", n_primary_vertices);
    // number of "good" primary vertices
    int n_good_primary_vertices = 0;
    for (unsigned i = 0; i < hPVs->size(); i++) {
        if ((*hPVs)[i].ndof() > 4 && (fabs((*hPVs)[i].z()) <= 24.) && (fabs((*hPVs)[i].position().rho()) <= 2.0))
            n_good_primary_vertices += 1;
    }
    FillSingleVar("n_good_primary_vertices", n_good_primary_vertices);

    // necessary weights for sample normalization
    FillSingleVar("sample_weight", sample_weight);
    FillSingleVar("generator_weight", generator_weight);

    // raw pfmet quantities
    FillSingleVar("pt_pfmet_raw", pfmet.corPt(pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_jes_up", pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_jes_down", pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_jer_up", pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_jer_down", pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Raw));
    // FillSingleVar("pt_pfmet_raw_jersmear_up", pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Raw));
    // FillSingleVar("pt_pfmet_raw_jersmear_down", pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_uncen_up", pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_uncen_down", pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_ele_up", pfmet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_ele_down", pfmet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_muo_up", pfmet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_muo_down", pfmet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_tau_up", pfmet.shiftedPt(pat::MET::TauEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_tau_down", pfmet.shiftedPt(pat::MET::TauEnDown, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_pho_up", pfmet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Raw));
    FillSingleVar("pt_pfmet_raw_pho_down", pfmet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Raw));

    FillSingleVar("phi_pfmet_raw", pfmet.corPhi(pat::MET::Raw));

    // type1 corrected pfmet quantities
    FillSingleVar("pt_pfmet_t1", pfmet.corPt(pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_jes_up", pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_jes_down", pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_jer_up", pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_jer_down", pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1));
    // FillSingleVar("pt_pfmet_t1_jersmear_up", pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1));
    // FillSingleVar("pt_pfmet_t1_jersmear_down", pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_uncen_up", pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_uncen_down", pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_ele_up", pfmet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_ele_down", pfmet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_muo_up", pfmet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_muo_down", pfmet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_tau_up", pfmet.shiftedPt(pat::MET::TauEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_tau_down", pfmet.shiftedPt(pat::MET::TauEnDown, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_pho_up", pfmet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Type1));
    FillSingleVar("pt_pfmet_t1_pho_down", pfmet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Type1));

    FillSingleVar("phi_pfmet_t1", pfmet.corPhi(pat::MET::Type1));

    // type1 + xy corrected pfmet quantities
    FillSingleVar("pt_pfmet_t1xy", pfmet.corPt(pat::MET::Type1XY));

    FillSingleVar("phi_pfmet_t1xy", pfmet.corPhi(pat::MET::Type1XY));

    // type1 + smearing corrected pfmet quantities
    FillSingleVar("pt_pfmet_t1smear", pfmet.corPt(pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_jes_up", pfmet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_jes_down", pfmet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_jer_up", pfmet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_jer_down", pfmet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1Smear));
    // FillSingleVar("pt_pfmet_t1smear_jersmear_up", pfmet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1Smear));
    // FillSingleVar("pt_pfmet_t1smear_jersmear_down", pfmet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_uncen_up", pfmet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_uncen_down", pfmet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_ele_up", pfmet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_ele_down", pfmet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_muo_up", pfmet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_muo_down", pfmet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_tau_up", pfmet.shiftedPt(pat::MET::TauEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_tau_down", pfmet.shiftedPt(pat::MET::TauEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_pho_up", pfmet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_pfmet_t1smear_pho_down", pfmet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Type1Smear));

    FillSingleVar("phi_pfmet_t1smear", pfmet.corPhi(pat::MET::Type1Smear));

    // raw puppimet quantities
    FillSingleVar("pt_puppimet_raw", puppimet.corPt(pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_jes_up", puppimet.shiftedPt(pat::MET::JetEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_jes_down", puppimet.shiftedPt(pat::MET::JetEnDown, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_jer_up", puppimet.shiftedPt(pat::MET::JetResUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_jer_down", puppimet.shiftedPt(pat::MET::JetResDown, pat::MET::Raw));
    // FillSingleVar("pt_puppimet_raw_jersmear_up", puppimet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Raw));
    // FillSingleVar("pt_puppimet_raw_jersmear_down", puppimet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_uncen_up", puppimet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_uncen_down", puppimet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_ele_up", puppimet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_ele_down", puppimet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_muo_up", puppimet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_muo_down", puppimet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_tau_up", puppimet.shiftedPt(pat::MET::TauEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_tau_down", puppimet.shiftedPt(pat::MET::TauEnDown, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_pho_up", puppimet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Raw));
    FillSingleVar("pt_puppimet_raw_pho_down", puppimet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Raw));

    FillSingleVar("phi_puppimet_raw", puppimet.corPhi(pat::MET::Raw));

    // type1 corrected puppimet quantities
    FillSingleVar("pt_puppimet_t1", puppimet.corPt(pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_jes_up", puppimet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_jes_down", puppimet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_jer_up", puppimet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_jer_down", puppimet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1));
    // FillSingleVar("pt_puppimet_t1_jersmear_up", puppimet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1));
    // FillSingleVar("pt_puppimet_t1_jersmear_down", puppimet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_uncen_up", puppimet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_uncen_down", puppimet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_ele_up", puppimet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_ele_down", puppimet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_muo_up", puppimet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_muo_down", puppimet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_tau_up", puppimet.shiftedPt(pat::MET::TauEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_tau_down", puppimet.shiftedPt(pat::MET::TauEnDown, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_pho_up", puppimet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Type1));
    FillSingleVar("pt_puppimet_t1_pho_down", puppimet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Type1));

    FillSingleVar("phi_puppimet_t1", puppimet.corPhi(pat::MET::Type1));

    // type1 + xy corrected puppimet quantities
    FillSingleVar("pt_puppimet_t1xy", puppimet.corPt(pat::MET::Type1XY));

    FillSingleVar("phi_puppimet_t1xy", puppimet.corPhi(pat::MET::Type1XY));

    // type1 + smearing corrected puppimet quantities
    FillSingleVar("pt_puppimet_t1smear", puppimet.corPt(pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_jes_up", puppimet.shiftedPt(pat::MET::JetEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_jes_down", puppimet.shiftedPt(pat::MET::JetEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_jer_up", puppimet.shiftedPt(pat::MET::JetResUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_jer_down", puppimet.shiftedPt(pat::MET::JetResDown, pat::MET::Type1Smear));
    // FillSingleVar("pt_puppimet_t1smear_jersmear_up", puppimet.shiftedPt(pat::MET::JetResUpSmear,pat::MET::Type1Smear));
    // FillSingleVar("pt_puppimet_t1smear_jersmear_down", puppimet.shiftedPt(pat::MET::JetResDownSmear,pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_uncen_up", puppimet.shiftedPt(pat::MET::UnclusteredEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_uncen_down", puppimet.shiftedPt(pat::MET::UnclusteredEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_ele_up", puppimet.shiftedPt(pat::MET::ElectronEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_ele_down", puppimet.shiftedPt(pat::MET::ElectronEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_muo_up", puppimet.shiftedPt(pat::MET::MuonEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_muo_down", puppimet.shiftedPt(pat::MET::MuonEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_tau_up", puppimet.shiftedPt(pat::MET::TauEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_tau_down", puppimet.shiftedPt(pat::MET::TauEnDown, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_pho_up", puppimet.shiftedPt(pat::MET::PhotonEnUp, pat::MET::Type1Smear));
    FillSingleVar("pt_puppimet_t1smear_pho_down", puppimet.shiftedPt(pat::MET::PhotonEnDown, pat::MET::Type1Smear));

    FillSingleVar("phi_puppimet_t1smear", puppimet.corPhi(pat::MET::Type1Smear));

    // generator met
    if(not isData){
        FillSingleVar("pt_genmet", genmet->pt());
        FillSingleVar("phi_genmet", genmet->phi());
    }

    //std::vector< ROOT::Math::XYZTVector > loose_muons_p4;
    //for(auto muon : *hLooseMuons){loose_muons_p4.push_back(muon.p4());}
    //FillVectorVar("loose_muons_p4", loose_muons_p4);

    // fill output tree
    tree->Fill();
    // fill generator weight into histogram containing the sum of generator weights
    sumw->Fill(1.0, generator_weight);
    // increase number of processed events by filling 1 into histogram containing the sum of processed events
    sume->Fill(1);
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

// function to initialize singleton variables
void METAnalyzer::InitSingleVar(std::string name, std::string type)
{
    if (type == "F") {
        single_float_vars.insert({name, std::unique_ptr< float >(new float(-999.0))});
        tree->Branch(name.c_str(), single_float_vars[name].get(), (name + "/F").c_str());
    }
    else if (type == "I") {
        single_int_vars.insert({name, std::unique_ptr< int >(new int(-999))});
        tree->Branch(name.c_str(), single_int_vars[name].get(), (name + "/I").c_str());
    }
    else if (type == "L") {
        single_long_vars.insert({name, std::unique_ptr< long >(new long(-999))});
        tree->Branch(name.c_str(), single_long_vars[name].get(), (name + "/L").c_str());
    }
    else {
        std::cout << "currently only float (F), int (I), and long(L) types are supported at the moment" << std::endl;
        throw std::exception();
    }
}

// functions to fill singleton variables of different types
void METAnalyzer::FillSingleVar(std::string name, float value) { *single_float_vars[name] = value; }
// doubles are automatically converted to float since double precision is never needed for this use case
void METAnalyzer::FillSingleVar(std::string name, double value) { *single_float_vars[name] = static_cast<float>(value); }
void METAnalyzer::FillSingleVar(std::string name, int value) { *single_int_vars[name] = value; }
void METAnalyzer::FillSingleVar(std::string name, long value) { *single_long_vars[name] = value; }

// function to initialize vector variables
// reserve 100 elements for the std::vectors before creating the branches to make sure that the vectors are not reallocated (100 elements should be enough for this use case)
void METAnalyzer::InitVectorVar(std::string name, std::string type, size_t n_elements)
{
    if (type == "F") {
        vector_float_vars.insert({name, std::unique_ptr< std::vector< float > >(new std::vector< float >(n_elements, -999.0))});
        vector_float_vars[name]->reserve(100);
        tree->Branch(name.c_str(), vector_float_vars[name].get());
    }
    else if (type == "I") {
        vector_int_vars.insert({name, std::unique_ptr< std::vector < int > >(new std::vector< int >(n_elements, -999))});
        vector_int_vars[name]->reserve(100);
        tree->Branch(name.c_str(), vector_int_vars[name].get());
    }
    else if (type ==  "LorentzVector") {
        vector_tlorentz_vars.insert({name, std::unique_ptr< std::vector< ROOT::Math::XYZTVector > >(new std::vector< ROOT::Math::XYZTVector >(n_elements, {0., 0., 0., 0.}))});
        vector_tlorentz_vars[name]->reserve(100);
        tree->Branch(name.c_str(), vector_tlorentz_vars[name].get());
    }
    else {
        std::cout << "currently only vector of float (F), int (I), and LorentzVector types are supported at the moment" << std::endl;
        throw std::exception();
    }
}

void METAnalyzer::FillVectorVar(std::string name, std::vector< float > vector)
{
    vector_float_vars[name]->resize(vector.size());
    *vector_float_vars[name] = vector;
}
void METAnalyzer::FillVectorVar(std::string name, std::vector< int > vector)
{
    vector_int_vars[name]->resize(vector.size());
    *vector_int_vars[name] = vector;
}
void METAnalyzer::FillVectorVar(std::string name, std::vector< ROOT::Math::XYZTVector > vector)
{
    vector_tlorentz_vars[name]->resize(vector.size());
    *vector_tlorentz_vars[name] = vector;
}


// define this as a plug-in
DEFINE_FWK_MODULE(METAnalyzer);
