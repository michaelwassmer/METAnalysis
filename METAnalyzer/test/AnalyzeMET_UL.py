import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing


options = VarParsing("analysis")
options.register(
    "isData",
    False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "is it data or MC?",
)
options.register(
    "skipEvents",
    0,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    "Number of events to skip",
)
options.register(
    "globalTag",
    "102X_upgrade2018_realistic_v21",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "global tag",
)
options.register(
    "dataEra",
    "2018",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "the era of the data taking period or mc campaign, e.g. '2016B' or '2017'",
)
options.register(
    "outName",
    "MET_ntuples",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "name and path of the output files (without extension)",
)
# this option is just there to make the config work with the shell script workflow, it does nothing
options.register(
    "sampleWeight",
    1.0,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.float,
    "not doing anything",
)
options.parseArguments()

print(options.sampleWeight)

if options.maxEvents == -1:  # maxEvents is set in VarParsing class by default to -1
    options.maxEvents = 1001  # reset for testing

if options.isData:
    options.globalTag = "106X_dataRun2_v35"
elif not options.isData:
    if "2016preVFP" in options.dataEra:
        options.globalTag = "106X_mcRun2_asymptotic_preVFP_v11"
    elif "2016postVFP" in options.dataEra:
        options.globalTag = "106X_mcRun2_asymptotic_v17"
    elif "2017" in options.dataEra:
        options.globalTag = "106X_mc2017_realistic_v8"
    elif "2018" in options.dataEra:
        options.globalTag = "106X_upgrade2018_realistic_v15_L1v1"
    else:
        raise Exception(
            "dataEra "
            + options.dataEra
            + " not supported for this config: USE dataEra=2016/2017"
        )
else:
    raise Exception("Problem with isData option! This should never happen!")

if not options.inputFiles:
    if not options.isData:
        if "2018" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL18MiniAOD/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_upgrade2018_realistic_v11_L1v1-v2/00000/71F1E2CE-7274-8F48-909C-3AF1C874A792.root"
            ]
        elif "2017" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17MiniAOD/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v2/00000/B5B3F393-9668-654A-B338-9DFFAD451B00.root"
            ]
        elif "2016postVFP" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL16MiniAOD/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v13-v2/10001/165C7F49-9F40-A442-B61E-562F4C739B85.root"
            ]
        elif "2016preVFP" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL16MiniAODAPV/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_mcRun2_asymptotic_preVFP_v8-v2/10001/68625BA9-DC73-E443-8C70-C35D2A71A2E3.root"
            ]
        else:
            raise Exception(
                "dataEra "
                + options.dataEra
                + " not supported for this config: USE dataEra=2016/2017/2018"
            )
    else:
        if "2018" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2018B/MET/MINIAOD/12Nov2019_UL2018-v3/110000/DF6D6B8D-40A6-9D4D-9118-3D58895920B5.root"
            ]
        elif "2017" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2017B/MET/MINIAOD/09Aug2019_UL2017_rsb-v1/50000/7BE4D67B-7654-A741-9035-86B87C53242B.root"
            ]
        elif "2016preVFP" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2016D/MET/MINIAOD/21Feb2020_UL2016_HIPM-v1/40000/7E0E2AA5-9359-EA4A-A5C9-FB982CE2575F.root"
            ]
        elif "2016postVFP" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2016H/MET/MINIAOD/21Feb2020_UL2016-v2/230000/369789BB-378D-9747-9CFC-09392D1B8C85.root"
            ]
        else:
            raise Exception(
                "dataEra "
                + options.dataEra
                + " not supported for this config: USE dataEra=2016/2017/2018"
            )

# checks for correct values and consistency
if "data" in options.globalTag.lower() and not options.isData:
    print ("\n\nConfig ERROR: GT contains seems to be for data but isData==False\n\n")
    sys.exit()
if "mc" in options.globalTag.lower() and options.isData:
    print ("\n\nConfig ERROR: GT contains seems to be for MC but isData==True\n\n")
    sys.exit()
if not options.inputFiles:
    print ("\n\nConfig ERROR: no inputFiles specified\n\n")
    sys.exit()

process = cms.Process("METAnalysis")

#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#    ignoreTotal = cms.untracked.int32(0)
#)

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.inputFiles),
    skipEvents=cms.untracked.uint32(int(options.skipEvents)),
)
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

# messages
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = options.globalTag
process.load("CondCore.CondDB.CondDB_cfi")

####################### MET stuff #############################

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import (
    runMetCorAndUncFromMiniAOD,
)
from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD

makePuppiesFromMiniAOD(process, False)

runMetCorAndUncFromMiniAOD(
    process,
    isData=options.isData,
    fixEE2017=True if "2017" in options.dataEra else False,
    fixEE2017Params={
        "userawPt": True,
        "ptThreshold": 50.0,
        "minEtaThreshold": 2.65,
        "maxEtaThreshold": 3.139,
    },
)

runMetCorAndUncFromMiniAOD(
    process,
    isData=options.isData,
    metType="Puppi",
    postfix="Puppi",
    jetFlavor="AK4PFPuppi",
)

process.puppiNoLep.useExistingWeights = True
process.puppi.useExistingWeights = True

####################### MET filters ##########################
#process.load("MonoTop.METFilter.METFilter_cfi")
#if options.isData:
#    process.METFilter.filterData = cms.InputTag("TriggerResults","","RECO")
#    process.METFilter.filterNames += ["Flag_eeBadScFilter"]
#print process.METFilter.filterNames

####################### configure process #############################

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outName+".root") )

process.load("METAnalysis.METAnalyzer.METAnalyzer_cfi")
process.METAnalyzer.isData = cms.bool(options.isData)
process.METAnalyzer.era = cms.string(options.dataEra)
process.METAnalyzer.sample_weight = cms.double(options.sampleWeight)
process.METAnalyzer.met_pf = cms.InputTag("slimmedMETs","",process.name_())
process.METAnalyzer.met_puppi = cms.InputTag("slimmedMETsPuppi","",process.name_())

if "2016" in options.dataEra:
    if options.isData:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "DQM")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "DQM")
    else:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "PAT")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "PAT")
elif "2017" in options.dataEra:
    if options.isData:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "PAT")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "PAT")
    else:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "PAT")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "PAT")
elif "2018" in options.dataEra:
    if options.isData:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "RECO")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "RECO")
    else:
        process.METAnalyzer.met_pf_original = cms.InputTag("slimmedMETs", "", "PAT")
        process.METAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "PAT")
else:
    raise Exception(
        "dataEra "
        + options.dataEra
        + " not supported: USE dataEra=2016/2017/2018"
    )

###############################################################

process.content = cms.EDAnalyzer("EventContentAnalyzer")

# met sequence to recalculate MET if running in scheduled mode
#process.met = cms.Path(process.fullPatMetSequence)

# write the events which pass the skimming selection and only keep the specified file content
process.final = cms.EndPath(process.METAnalyzer)

# associate the patAlgosToolsTask to the Endpath for unscheduled mode
process.final.associate(process.patAlgosToolsTask)
#print(process.dumpPython())
