from CRABClient.UserUtilities import config

config = config()

config.General.requestName = "THEREQUESTNAME"
config.General.workArea = "crab_ntuples"
config.General.transferLogs = True

config.JobType.pluginName = "Analysis"
config.JobType.psetName = "/nfs/dust/cms/user/mwassmer/RecoMET/CMSSW_10_2_18/src/METAnalysis/METAnalyzer/test/AnalyzeMET.py"
# config.JobType.outputFiles = "MET_ntuples.root"
# config.JobType.maxJobRuntimeMin = 2800
config.JobType.maxMemoryMB = 1500
config.JobType.pyCfgParams = [
    "outName=MET_ntuples",
    "isData=ISDATA",
    "maxEvents=999999999",
    "dataEra=DATAERA",
    "globalTag=GLOBALTAG",
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
config.Data.outputDatasetTag = "MET_analysis"
config.Data.outLFNDirBase = '/store/user/mwassmer/MET_analysis/'

config.User.voGroup = "dcms"

config.Site.storageSite = "T2_DE_DESY"
# config.Site.blacklist = ['T2_US_*']
# config.Site.whitelist = ['T2_DE_DESY']
