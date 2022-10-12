import FWCore.ParameterSet.Config as cms

### muons ###

from PhysicsTools.PatAlgos.cleaningLayer1.muonCleaner_cfi import *

# tight muons
tightPatMuons = cleanPatMuons.clone()
tightPatMuons.src = cms.InputTag("slimmedMuons")
tightPatMuons.finalCut = cms.string("pt>=30 & abs(eta)<=2.5 & passed('CutBasedIdTight') & passed('PFIsoTight')")

# loose muons
loosePatMuons = cleanPatMuons.clone()
loosePatMuons.src = cms.InputTag("slimmedMuons")
loosePatMuons.finalCut = cms.string("pt>=10 & abs(eta)<=2.5 & passed('CutBasedIdLoose') & passed('PFIsoLoose')")

### electrons ###

from PhysicsTools.PatAlgos.cleaningLayer1.electronCleaner_cfi import *

# tight electrons
tightPatElectrons = cleanPatElectrons.clone()
tightPatElectrons.src = cms.InputTag("slimmedElectrons")
tightPatElectrons.finalCut = cms.string("pt>=40 & abs(eta)<=2.4 & electronID('cutBasedElectronID-Fall17-94X-V2-tight') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
tightPatElectrons.checkOverlaps.muons.src = cms.InputTag("loosePatMuons")

# loose electrons
loosePatElectrons = cleanPatElectrons.clone()
loosePatElectrons.src = cms.InputTag("slimmedElectrons")
loosePatElectrons.finalCut = cms.string("pt>=10 & abs(eta)<=2.5 & electronID('cutBasedElectronID-Fall17-94X-V2-loose') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
loosePatElectrons.checkOverlaps.muons.src = cms.InputTag("loosePatMuons")

### photons ###

from PhysicsTools.PatAlgos.cleaningLayer1.photonCleaner_cfi import *

# tight photons
tightPatPhotons = cleanPatPhotons.clone()
tightPatPhotons.src = cms.InputTag("slimmedPhotons")
tightPatPhotons.finalCut = cms.string("pt>=200 & abs(eta)<=2.4 & photonID('cutBasedPhotonID-Fall17-94X-V2-tight') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
tightPatPhotons.checkOverlaps.electrons.src = cms.InputTag("loosePatElectrons")
tightPatPhotons.checkOverlaps.electrons.requireNoOverlaps = cms.bool(True)

# loose photons
loosePatPhotons = cleanPatPhotons.clone()
loosePatPhotons.src = cms.InputTag("slimmedPhotons")
loosePatPhotons.finalCut = cms.string("pt>=20 & abs(eta)<=2.4 & photonID('cutBasedPhotonID-Fall17-94X-V2-loose') & (abs(superCluster.eta)<1.4442 | abs(superCluster.eta)>1.5660)")
loosePatPhotons.checkOverlaps.electrons.src = cms.InputTag("loosePatElectrons")
loosePatPhotons.checkOverlaps.electrons.requireNoOverlaps = cms.bool(True)
