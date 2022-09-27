import FWCore.ParameterSet.Config as cms

####################### Muon stuff #############################

from PhysicsTools.PatAlgos.cleaningLayer1.muonCleaner_cfi import *

# tight muons
tightPatMuons = cleanPatMuons.clone()
tightPatMuons.src = cms.InputTag("slimmedMuons")
tightPatMuons.finalCut = cms.string("pt>=30 & abs(eta)<=2.4 & passed('CutBasedIdTight') & passed('PFIsoTight')") 

# loose muons
loosePatMuons = cleanPatMuons.clone()
loosePatMuons.src = cms.InputTag("slimmedMuons")
loosePatMuons.finalCut = cms.string("pt>=30 & abs(eta)<=2.4 & passed('CutBasedIdLoose') & passed('PFIsoLoose')")

# crude singlemu and dimuon selection with tight muons
countTightPatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("tightPatMuons")
)

# crude singlemu and dimuon selection with loose muons
countLoosePatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("loosePatMuons")
)

from PhysicsTools.PatAlgos.cleaningLayer1.electronCleaner_cfi import *

# tight electrons
tightPatElectrons = cleanPatElectrons.clone()
tightPatElectrons.src = cms.InputTag("slimmedElectrons")
tightPatElectrons.finalCut = cms.string("pt>=40 & abs(eta)<=2.4 & electronID('cutBasedElectronID-Fall17-94X-V2-tight') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
tightPatElectrons.checkOverlaps.muons.src = cms.InputTag("loosePatMuons")

# loose electrons
loosePatElectrons = cleanPatElectrons.clone()
loosePatElectrons.src = cms.InputTag("slimmedElectrons")
loosePatElectrons.finalCut = cms.string("pt>=10 & abs(eta)<=2.4 & electronID('cutBasedElectronID-Fall17-94X-V2-loose') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
loosePatElectrons.checkOverlaps.muons.src = cms.InputTag("loosePatMuons")

countTightPatElectrons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(0),
    src = cms.InputTag("tightPatElectrons")
)

countLoosePatElectrons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(0),
    src = cms.InputTag("loosePatElectrons")
)

import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
HLTFilter = hlt.hltHighLevel.clone(
    HLTPaths = ['HLT_IsoMu2*','HLT_PFMET*'],
    throw = True
)
