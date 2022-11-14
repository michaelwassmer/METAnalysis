import FWCore.ParameterSet.Config as cms

tightJetId = cms.EDProducer("PatJetIDValueMapProducer",
              filterParams=cms.PSet(
                version = cms.string('RUN2ULCHS'),
                quality = cms.string('TIGHT'),
              ),
                          src = cms.InputTag("slimmedJets")
)
tightJetIdLepVeto = cms.EDProducer("PatJetIDValueMapProducer",
              filterParams=cms.PSet(
                version = cms.string('RUN2ULCHS'),
                quality = cms.string('TIGHTLEPVETO'),
              ),
                          src = cms.InputTag("slimmedJets")
)
updatedJetsWithUserData = cms.EDProducer("PATJetUserDataEmbedder",
     src = cms.InputTag("slimmedJets"),
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
jets.add(tightJetId)
jets.add(tightJetIdLepVeto)
jets.add(updatedJetsWithUserData)
jets.add(finalJets)
