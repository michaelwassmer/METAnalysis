from CRABClient.UserUtilities import config

config = config()

config.General.requestName = "THEREQUESTNAME"
config.General.workArea = "crab_ntuples"
config.General.transferLogs = False

config.JobType.pluginName = "Analysis"
config.JobType.psetName = "/nfs/dust/cms/user/mwassmer/RecoMET/rebase_tests/CMSSW_12_0_0/src/METAnalysis/METAnalyzer/test/AnalyzeMET_UL.py"
# config.JobType.outputFiles = "MET_ntuples.root"
# config.JobType.maxJobRuntimeMin = 2800
config.JobType.maxMemoryMB = 1500
config.JobType.pyCfgParams = [
    "outName=MET_ntuples",
    "isData=ISDATA",
    "maxEvents=999999999",
    "dataEra=DATAERA",
#    "globalTag=GLOBALTAG",
    "sampleWeight=SAMPLEWEIGHT"
]
config.JobType.sendPythonFolder = True
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = "THEINPUTDATASET"
config.Data.inputDBS = "global"
config.Data.splitting = "Automatic"
config.Data.unitsPerJob = 360
config.Data.publication = False
config.Data.publishDBS = "phys03"
config.Data.outputDatasetTag = "MET_Phi_Studies"
config.Data.outLFNDirBase = '/store/user/mwassmer/RecoMET/MET_Phi_Studies/DATAERA/'

config.User.voGroup = "dcms"

config.Site.storageSite = "T2_DE_DESY"
# config.Site.blacklist = ['T2_US_*']
# config.Site.whitelist = ['T2_DE_DESY']
