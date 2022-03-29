import FWCore.ParameterSet.Config as cms

####################### Muon stuff #############################

from PhysicsTools.PatAlgos.cleaningLayer1.muonCleaner_cfi import *
tightPatMuons = cleanPatMuons.clone()
tightPatMuons.src=cms.InputTag("slimmedMuons")
tightPatMuons.finalCut=cms.string("pt>=30 & abs(eta)<=2.4 & passed('CutBasedIdTight') & passed('PFIsoTight')") 

loosePatMuons = cleanPatMuons.clone()
loosePatMuons.src=cms.InputTag("slimmedMuons")
loosePatMuons.finalCut=cms.string("pt>=30 & abs(eta)<=2.4 & passed('CutBasedIdLoose') & passed('PFIsoLoose')")

countTightPatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("tightPatMuons")
)

countLoosePatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("loosePatMuons")
)
