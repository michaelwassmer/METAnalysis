import FWCore.ParameterSet.Config as cms

from  PhysicsTools.PatAlgos.recoLayer0.jetCorrFactors_cfi import *

jetCorrFactors = patJetCorrFactors.clone(src='slimmedJets',
    levels = cms.vstring('L1FastJet',
        'L2Relative',
        'L3Absolute',
    'L2L3Residual'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

from  PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cfi import *

updatedJets = updatedPatJets.clone(
    addBTagInfo=False,
    jetSource='slimmedJets',
    jetCorrFactorsSource=cms.VInputTag(cms.InputTag("jetCorrFactors") ),
)

tightJetId = cms.EDProducer("PatJetIDValueMapProducer",
              filterParams=cms.PSet(
                version = cms.string('RUN2ULCHS'),
                quality = cms.string('TIGHT'),
              ),
                          src = cms.InputTag("updatedJets")
)

tightJetIdLepVeto = cms.EDProducer("PatJetIDValueMapProducer",
              filterParams=cms.PSet(
                version = cms.string('RUN2ULCHS'),
                quality = cms.string('TIGHTLEPVETO'),
              ),
                          src = cms.InputTag("updatedJets")
)

updatedJetsWithUserData = cms.EDProducer("PATJetUserDataEmbedder",
     src = cms.InputTag("updatedJets"),
     userInts = cms.PSet(
        tightId = cms.InputTag("tightJetId"),
        tightIdLepVeto = cms.InputTag("tightJetIdLepVeto")
     )
)

finalJets = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("updatedJetsWithUserData"),
    cut = cms.string("pt > 50 & abs(eta)<=2.4 & userInt('tightIdLepVeto')>0")
)

jets = cms.Task()
jets.add(jetCorrFactors)
jets.add(updatedJets)
jets.add(tightJetId)
jets.add(tightJetIdLepVeto)
jets.add(updatedJetsWithUserData)
jets.add(finalJets)
