import FWCore.ParameterSet.Config as cms

countTightPatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("tightPatMuons")
)

countLoosePatMuons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("loosePatMuons")
)

countTightPatElectrons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("tightPatElectrons")
)

countLoosePatElectrons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(2),
    src = cms.InputTag("loosePatElectrons")
)

countTightPatPhotons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(0),
    src = cms.InputTag("tightPatPhotons")
)

countLoosePatPhotons = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(0),
    maxNumber = cms.uint32(0),
    src = cms.InputTag("loosePatPhotons")
)

countJets = cms.EDFilter("PATCandViewCountFilter",
    minNumber = cms.uint32(1),
    maxNumber = cms.uint32(999),
    src = cms.InputTag("finalJets")
)
#import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
#HLTFilter = hlt.hltHighLevel.clone(
#    HLTPaths = ['HLT_IsoMu2*','HLT_PFMET*'],
#    throw = True
#)
