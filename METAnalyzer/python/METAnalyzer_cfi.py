import FWCore.ParameterSet.Config as cms

METAnalyzer = cms.EDAnalyzer(
    "METAnalyzer",
    era=cms.string("NA"),
    isData=cms.bool(False),
    sample_weight=cms.double(1.0),
    met_pf=cms.InputTag("slimmedMETs"),
    met_puppi=cms.InputTag("slimmedMETsPuppi"),
    met_pf_original=cms.InputTag("slimmedMETs"),
    met_puppi_original=cms.InputTag("slimmedMETsPuppi"),
    gen_event_info=cms.InputTag("generator")
)
