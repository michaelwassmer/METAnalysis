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
    "weight",
    1.0,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.float,
    "not doing anything",
)
options.parseArguments()

if options.maxEvents is -1:  # maxEvents is set in VarParsing class by default to -1
    options.maxEvents = 1001  # reset for testing

if options.isData:
    if "2016" in options.dataEra:
        options.globalTag = "102X_dataRun2_v13"
    elif "2017" in options.dataEra:
        options.globalTag = "102X_dataRun2_v13"
    elif "2018" in options.dataEra:
        options.globalTag = "102X_dataRun2_v13"
    else:
        raise Exception(
            "dataEra "
            + options.dataEra
            + " not supported for this config: USE dataEra=2016/2017"
        )
elif not options.isData:
    if "2016" in options.dataEra:
        options.globalTag = "102X_mcRun2_asymptotic_v8"
    elif "2017" in options.dataEra:
        options.globalTag = "102X_mc2017_realistic_v8"
    elif "2018" in options.dataEra:
        options.globalTag = "102X_upgrade2018_realistic_v21"
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
                "root://xrootd-cms.infn.it///store/mc/RunIIAutumn18MiniAOD/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/40000/E26C264E-6FAE-BA4B-B729-1FDD04B0B2AC.root"
            ]
        elif "2017" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIIFall17MiniAODv2/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v2/100000/44A5063A-27D3-E811-BE88-0CC47AFB7DDC.root"
            ]
        elif "2016" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/mc/RunIISummer16MiniAODv3/TTToSemiLeptonic_TuneCP5_PSweights_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3-v1/270000/68E65DC7-2C79-E911-A393-F01FAFE5CBE2.root"
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
                "root://xrootd-cms.infn.it///store/data/Run2018B/MET/MINIAOD/17Sep2018-v1/100000/84F4D3C4-7275-834A-ADDF-E34194D17EB3.root"
            ]
        elif "2017" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2017E/MET/MINIAOD/31Mar2018-v1/80000/5E616617-8C38-E811-B665-0025905B859E.root"
            ]
        elif "2016" in options.dataEra:
            options.inputFiles = [
                "root://xrootd-cms.infn.it///store/data/Run2016G/MET/MINIAOD/17Jul2018-v1/50000/C64557A4-228C-E811-9543-0090FAA587C4.root"
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
#from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD

#makePuppiesFromMiniAOD(process, False)

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

#runMetCorAndUncFromMiniAOD(
#    process,
#    isData=options.isData,
#    metType="Puppi",
#    postfix="Puppi",
#    jetFlavor="AK4PFPuppi",
#)

#process.puppiNoLep.useExistingWeights = True
#process.puppi.useExistingWeights = True

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
process.METAnalyzer.met_pf = cms.InputTag("slimmedMETs","",process.name_())
process.METAnalyzer.met_puppi = cms.InputTag("slimmedMETsPuppi")

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
        process.MMETAnalyzer.met_puppi_original = cms.InputTag("slimmedMETsPuppi", "", "PAT")
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

# met sequence to recalculate MET
process.met = cms.Path(process.fullPatMetSequence)

# write the events which pass the skimming selection and only keep the specified file content
process.final = cms.EndPath(process.METAnalyzer)
#print(process.dumpPython())
