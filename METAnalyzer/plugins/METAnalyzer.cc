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
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

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
    edm::EDGetTokenT< std::vector< pat::MET > >      EDMPFMETToken;             // PF MET
    edm::EDGetTokenT< std::vector< pat::MET > >      EDMPuppiMETToken;          // PUPPI MET
    edm::EDGetTokenT< std::vector< pat::MET > >      EDMPFMETOriginalToken;     // PF MET
    edm::EDGetTokenT< std::vector< pat::MET > >      EDMPuppiMETOriginalToken;  // PUPPI MET
    edm::EDGetTokenT< GenEventInfoProduct >          EDMGenEventInfoToken;
    edm::EDGetTokenT< std::vector< reco::Vertex > >  EDMPrimaryVertexToken;  // Primary Vertices
    edm::EDGetTokenT< std::vector< pat::Muon > >     EDMLooseMuonToken;      // Muon Collection
    edm::EDGetTokenT< std::vector< pat::Electron > > EDMLooseElectronToken;  // Electron Collection
    edm::EDGetTokenT< std::vector< pat::Muon > >     EDMTightMuonToken;      // Muon Collection
    edm::EDGetTokenT< std::vector< pat::Electron > > EDMTightElectronToken;  // Electron Collection
    edm::EDGetTokenT< std::vector< pat::Jet > >      EDMJetToken;            // Jet Collection
    edm::EDGetTokenT< std::vector< bool > >          filterDecisionsToken;
    edm::EDGetTokenT< std::vector< std::string > >   filterNamesToken;

    // some useful information to keep
    const bool        isData;
    const std::string era;
    const double      sample_weight;
    const bool        write_triggers;

    // containers to bookkeep all desired MET variations and uncertainties
    const std::vector< std::string >                            met_types{"pfmet", "puppimet", "pfmetnomu", "puppimetnomu"};
    const std::map< std::string, pat::MET::METCorrectionLevel > met_corr_levels{
        {"raw", pat::MET::Raw}, {"t1", pat::MET::Type1}, {"t1smear", pat::MET::Type1Smear}, {"t1xy", pat::MET::Type1XY}};
    const std::map< std::string, pat::MET::METUncertainty > met_uncs{{"jes_up", pat::MET::JetEnUp},           {"jer_up", pat::MET::JetResUp},
                                                                     {"uncen_up", pat::MET::UnclusteredEnUp}, {"ele_up", pat::MET::ElectronEnUp},
                                                                     {"muo_up", pat::MET::MuonEnUp},          {"tau_up", pat::MET::TauEnUp},
                                                                     {"pho_up", pat::MET::PhotonEnUp},        {"jes_down", pat::MET::JetEnDown},
                                                                     {"jer_down", pat::MET::JetResDown},      {"uncen_down", pat::MET::UnclusteredEnDown},
                                                                     {"ele_down", pat::MET::ElectronEnDown},  {"muo_down", pat::MET::MuonEnDown},
                                                                     {"tau_down", pat::MET::TauEnDown},       {"pho_down", pat::MET::PhotonEnDown}};

    // file service object to write output root file
    edm::Service< TFileService > fs;

    // outputs to write into the output root file
    TTree* tree;
    TH1D*  sumw;
    TH1D*  sume;

    // containers to hold singleton variables within an event
    std::map< std::string, std::unique_ptr< float > > single_float_vars;
    std::map< std::string, std::unique_ptr< int > >   single_int_vars;
    std::map< std::string, std::unique_ptr< long > >  single_long_vars;
    std::map< std::string, std::unique_ptr< bool > >  single_bool_vars;

    // containers to hold vector variables within an event
    std::map< std::string, std::unique_ptr< std::vector< float > > >                  vector_float_vars;
    std::map< std::string, std::unique_ptr< std::vector< int > > >                    vector_int_vars;
    std::map< std::string, std::unique_ptr< std::vector< ROOT::Math::XYZTVector > > > vector_tlorentz_vars;

    // some useful strings
    const std::string separator = "_";
    const std::string pt        = "pt";
    const std::string phi       = "phi";

    // ----------member functions ---------------------------

    // function to initialize a singleton variable
    void InitSingleVar(std::string name, std::string type);
    void InitVectorVar(std::string name, std::string type, size_t n_elements = 5);
    // functions to write singleton variables of different types into the containers defined above
    void FillSingleVar(std::string name, float value);
    void FillSingleVar(std::string name, double value);
    void FillSingleVar(std::string name, int value);
    void FillSingleVar(std::string name, long value);
    void FillSingleVar(std::string name, bool value);
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
    EDMLooseMuonToken{consumes< std::vector< pat::Muon > >(iConfig.getParameter< edm::InputTag >("loose_muons"))},
    EDMLooseElectronToken{consumes< std::vector< pat::Electron > >(iConfig.getParameter< edm::InputTag >("loose_electrons"))},
    EDMTightMuonToken{consumes< std::vector< pat::Muon > >(iConfig.getParameter< edm::InputTag >("tight_muons"))},
    EDMTightElectronToken{consumes< std::vector< pat::Electron > >(iConfig.getParameter< edm::InputTag >("tight_electrons"))},
    EDMJetToken{consumes< std::vector< pat::Jet > >(iConfig.getParameter< edm::InputTag >("jets"))},
    isData{iConfig.getParameter< bool >("isData")},
    era{iConfig.getParameter< std::string >("era")},
    sample_weight{iConfig.getParameter< double >("sample_weight")},
    write_triggers{iConfig.getParameter< bool >("write_triggers")}

{
    // now do what ever initialization is needed

    // only read generator event info if we deal with simulation
    if (not isData) { EDMGenEventInfoToken = consumes< GenEventInfoProduct >(iConfig.getParameter< edm::InputTag >("gen_event_info")); }

    // only read trigger decisions if requested
    if (write_triggers) {
        filterNamesToken     = consumes< std::vector< std::string > >(iConfig.getParameter< edm::InputTag >("filterNames"));
        filterDecisionsToken = consumes< std::vector< bool > >(iConfig.getParameter< edm::InputTag >("filterDecisions"));
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

    // all kinds of MET
    for (const std::string& met_type : met_types) {
        for (const auto& met_corr_level : met_corr_levels) {
            InitSingleVar(pt + separator + met_type + separator + met_corr_level.first, "F");
            InitSingleVar(phi + separator + met_type + separator + met_corr_level.first, "F");
            for (const auto& met_unc : met_uncs) {
                InitSingleVar(pt + separator + met_type + separator + met_corr_level.first + separator + met_unc.first, "F");
            }
        }
    }

    // generator met
    InitSingleVar("pt_genmet", "F");
    InitSingleVar("phi_genmet", "F");

    // number of primary vertices
    InitSingleVar("n_primary_vertices", "I");
    InitSingleVar("n_good_primary_vertices", "I");

    // loose muons
    InitSingleVar("n_loose_muons", "I");
    InitVectorVar("p4_loose_muons", "LorentzVector");

    // loose electrons
    InitSingleVar("n_loose_electrons", "I");
    InitVectorVar("p4_loose_electrons", "LorentzVector");

    // tight muons
    InitSingleVar("n_tight_muons", "I");
    InitVectorVar("p4_tight_muons", "LorentzVector");

    // tight electrons
    InitSingleVar("n_tight_electrons", "I");
    InitVectorVar("p4_tight_electrons", "LorentzVector");

    // jets
    InitSingleVar("n_jets", "I");
    InitVectorVar("p4_jets", "LorentzVector");
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
    // get pfmet pat::MET object
    edm::Handle< std::vector< pat::MET > > hPFMETs;
    iEvent.getByToken(EDMPFMETToken, hPFMETs);

    // get puppimet pat::MET object
    edm::Handle< std::vector< pat::MET > > hPuppiMETs;
    iEvent.getByToken(EDMPuppiMETToken, hPuppiMETs);

    // get primary vertex collection
    edm::Handle< std::vector< reco::Vertex > > hPVs;
    iEvent.getByToken(EDMPrimaryVertexToken, hPVs);

    // get filter decisions (if desired)
    edm::Handle< std::vector< std::string > > hFilterNames;
    edm::Handle< std::vector< bool > >        hFilterDecisions;
    if (write_triggers) {
        iEvent.getByToken(filterNamesToken, hFilterNames);
        iEvent.getByToken(filterDecisionsToken, hFilterDecisions);
    }

    // get muon collection
    edm::Handle< std::vector< pat::Muon > > hLooseMuons;
    iEvent.getByToken(EDMLooseMuonToken, hLooseMuons);

    // get electron collection
    edm::Handle< std::vector< pat::Electron > > hLooseElectrons;
    iEvent.getByToken(EDMLooseElectronToken, hLooseElectrons);

    // get muon collection
    edm::Handle< std::vector< pat::Muon > > hTightMuons;
    iEvent.getByToken(EDMTightMuonToken, hTightMuons);

    // get electron collection
    edm::Handle< std::vector< pat::Electron > > hTightElectrons;
    iEvent.getByToken(EDMTightElectronToken, hTightElectrons);

    // get jet collection
    edm::Handle< std::vector< pat::Jet > > hJets;
    iEvent.getByToken(EDMJetToken, hJets);

    // get generator event info object to retrieve generator weight
    edm::Handle< GenEventInfoProduct > hGenEventInfo;
    float                              generator_weight = 1.0;
    if (not isData) {
        iEvent.getByToken(EDMGenEventInfoToken, hGenEventInfo);
        auto geneventinfo = *hGenEventInfo;
        generator_weight  = geneventinfo.weight();
    }

    // retrieve pfmet pat::MET object
    auto pfmet = hPFMETs->at(0);
    // retrieve puppimet pat::MET object
    auto puppimet = hPuppiMETs->at(0);
    // retrieve genmet object
    auto genmet = pfmet.genMET();

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
        if ((*hPVs)[i].ndof() > 4 && (fabs((*hPVs)[i].z()) <= 24.) && (fabs((*hPVs)[i].position().rho()) <= 2.0)) n_good_primary_vertices += 1;
    }
    FillSingleVar("n_good_primary_vertices", n_good_primary_vertices);

    // necessary weights for sample normalization
    FillSingleVar("sample_weight", sample_weight);
    FillSingleVar("generator_weight", generator_weight);

    // sum of all muon four vectors for metnomu calculation
    ROOT::Math::XYZTVector muons_p4_sum{0., 0., 0., 0.};
    for (const pat::Muon& muon : *hLooseMuons) muons_p4_sum += muon.p4();

    // all kinds of MET
    for (const std::string& met_type : met_types) {
        pat::MET met;
        if (met_type.find("pfmet") != std::string::npos)
            met = pfmet;
        else if (met_type.find("puppimet") != std::string::npos)
            met = puppimet;
        for (const auto& met_corr_level : met_corr_levels) {
            ROOT::Math::XYZTVector met_p4 = met.corP4(met_corr_level.second);
            if (met_type.find("nomu") != std::string::npos) { met_p4 += muons_p4_sum; }
            FillSingleVar(pt + separator + met_type + separator + met_corr_level.first, met_p4.pt());
            FillSingleVar(phi + separator + met_type + separator + met_corr_level.first, met_p4.phi());
            for (const auto& met_unc : met_uncs) {
                ROOT::Math::XYZTVector met_p4 = met.shiftedP4(met_unc.second, met_corr_level.second);
                if (met_type.find("nomu") != std::string::npos) { met_p4 += muons_p4_sum; }
                FillSingleVar(pt + separator + met_type + separator + met_corr_level.first + separator + met_unc.first, met_p4.pt());
            }
        }
    }

    // generator met
    if (not isData) {
        FillSingleVar("pt_genmet", genmet->pt());
        FillSingleVar("phi_genmet", genmet->phi());
    }

    // filter/trigger decisions
    if (write_triggers) {
        for (size_t i = 0; i < hFilterDecisions->size(); i++) {
            size_t      index       = hFilterNames->at(i).rfind("_v");
            std::string triggername = hFilterNames->at(i).substr(0, index);
            FillSingleVar(triggername, hFilterDecisions->at(i));
        }
    }

    // loose muons
    std::vector< ROOT::Math::XYZTVector > loose_muons_p4;
    for (const pat::Muon& muon : *hLooseMuons) { loose_muons_p4.push_back(muon.p4()); }
    FillVectorVar("p4_loose_muons", loose_muons_p4);
    int n_loose_muons = hLooseMuons->size();
    FillSingleVar("n_loose_muons", n_loose_muons);

    // loose electrons
    std::vector< ROOT::Math::XYZTVector > loose_electrons_p4;
    for (const pat::Electron& electron : *hLooseElectrons) { loose_electrons_p4.push_back(electron.p4()); }
    FillVectorVar("p4_loose_electrons", loose_electrons_p4);
    int n_loose_electrons = hLooseElectrons->size();
    FillSingleVar("n_loose_electrons", n_loose_electrons);

    // tight muons
    std::vector< ROOT::Math::XYZTVector > tight_muons_p4;
    for (const pat::Muon& muon : *hTightMuons) { tight_muons_p4.push_back(muon.p4()); }
    FillVectorVar("p4_tight_muons", tight_muons_p4);
    int n_tight_muons = hTightMuons->size();
    FillSingleVar("n_tight_muons", n_tight_muons);

    // tight electrons
    std::vector< ROOT::Math::XYZTVector > tight_electrons_p4;
    for (const pat::Electron& electron : *hTightElectrons) { tight_electrons_p4.push_back(electron.p4()); }
    FillVectorVar("p4_tight_electrons", tight_electrons_p4);
    int n_tight_electrons = hTightElectrons->size();
    FillSingleVar("n_tight_electrons", n_tight_electrons);

    // jets
    std::vector< ROOT::Math::XYZTVector > jets_p4;
    for (const pat::Jet& jet : *hJets) { jets_p4.push_back(jet.p4()); }
    FillVectorVar("p4_jets", jets_p4);
    int n_jets = hJets->size();
    FillSingleVar("n_jets", n_jets);

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
        if (single_float_vars.find(name) != single_float_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        single_float_vars.insert({name, std::unique_ptr< float >(new float(-999.0))});
        tree->Branch(name.c_str(), single_float_vars[name].get(), (name + "/F").c_str());
    }
    else if (type == "I") {
        if (single_int_vars.find(name) != single_int_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        single_int_vars.insert({name, std::unique_ptr< int >(new int(-999))});
        tree->Branch(name.c_str(), single_int_vars[name].get(), (name + "/I").c_str());
    }
    else if (type == "L") {
        if (single_long_vars.find(name) != single_long_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        single_long_vars.insert({name, std::unique_ptr< long >(new long(-999))});
        tree->Branch(name.c_str(), single_long_vars[name].get(), (name + "/L").c_str());
    }
    else if (type == "B") {
        if (single_bool_vars.find(name) != single_bool_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        single_bool_vars.insert({name, std::unique_ptr< bool >(new bool(false))});
        tree->Branch(name.c_str(), single_bool_vars[name].get(), (name + "/B").c_str());
    }
    else {
        std::cout << "currently only float (F), int (I), long(L), and bool (B) types are supported at the moment" << std::endl;
        throw std::exception();
    }
}

// functions to fill singleton variables of different types
void METAnalyzer::FillSingleVar(std::string name, float value)
{
    // if (single_float_vars.find(name) == single_float_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    *single_float_vars[name] = value;
}
// doubles are automatically converted to float since double precision is never needed for this use case
void METAnalyzer::FillSingleVar(std::string name, double value)
{
    // if (single_float_vars.find(name) == single_float_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    *single_float_vars[name] = static_cast< float >(value);
}
void METAnalyzer::FillSingleVar(std::string name, int value)
{
    // if (single_int_vars.find(name) == single_int_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    *single_int_vars[name] = value;
}
void METAnalyzer::FillSingleVar(std::string name, long value)
{
    // if (single_long_vars.find(name) == single_long_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    *single_long_vars[name] = value;
}
void METAnalyzer::FillSingleVar(std::string name, bool value)
{
    if (single_bool_vars.find(name) == single_bool_vars.end()) {
        std::cout << "boolean variable >>>" << name << "<<< not initialized, therefore initializing it for you" << std::endl;
        InitSingleVar(name, "B");
    }
    *single_bool_vars[name] = value;
}

// function to initialize vector variables
// reserve 100 elements for the std::vectors before creating the branches to make sure that the vectors are not reallocated (100 elements should be enough for
// this use case)
void METAnalyzer::InitVectorVar(std::string name, std::string type, size_t n_elements)
{
    if (type == "F") {
        if (vector_float_vars.find(name) != vector_float_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        vector_float_vars.insert({name, std::unique_ptr< std::vector< float > >(new std::vector< float >(n_elements, -999.0))});
        vector_float_vars[name]->reserve(100);
        tree->Branch(name.c_str(), vector_float_vars[name].get());
    }
    else if (type == "I") {
        if (vector_int_vars.find(name) != vector_int_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        vector_int_vars.insert({name, std::unique_ptr< std::vector< int > >(new std::vector< int >(n_elements, -999))});
        vector_int_vars[name]->reserve(100);
        tree->Branch(name.c_str(), vector_int_vars[name].get());
    }
    else if (type == "LorentzVector") {
        if (vector_tlorentz_vars.find(name) != vector_tlorentz_vars.end()) {
            std::cout << "variable >>>" << name << "<<< already initialized so cannot be initialized again" << std::endl;
            throw std::exception();
        }
        vector_tlorentz_vars.insert(
            {name, std::unique_ptr< std::vector< ROOT::Math::XYZTVector > >(new std::vector< ROOT::Math::XYZTVector >(n_elements, {0., 0., 0., 0.}))});
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
    // if (vector_float_vars.find(name) == vector_float_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    vector_float_vars[name]->resize(vector.size());
    *vector_float_vars[name] = vector;
}
void METAnalyzer::FillVectorVar(std::string name, std::vector< int > vector)
{
    // if (vector_int_vars.find(name) == vector_int_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    vector_int_vars[name]->resize(vector.size());
    *vector_int_vars[name] = vector;
}
void METAnalyzer::FillVectorVar(std::string name, std::vector< ROOT::Math::XYZTVector > vector)
{
    // if (vector_tlorentz_vars.find(name) == vector_tlorentz_vars.end()) {
    //    std::cout << "variable >>>" << name << "<<< not initialized so cannot yet be filled" << std::endl;
    //    throw std::exception();
    //}
    vector_tlorentz_vars[name]->resize(vector.size());
    *vector_tlorentz_vars[name] = vector;
}

// define this as a plug-in
DEFINE_FWK_MODULE(METAnalyzer);
