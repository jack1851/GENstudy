import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
import os
#INPUT PARSING SECTION
options = VarParsing ('analysis')

options.register( 'runLocally',
                                  False,
                                  VarParsing.multiplicity.singleton,
                                  VarParsing.varType.bool,
                                  "True if running locally")

options.parseArguments()

process = cms.Process("Analysis")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#setup global tag
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from Configuration.AlCa.autoCond import autoCond

process.GlobalTag.globaltag = cms.string('106X_upgrade2018_realistic_v15_L1v1')

if(options.runLocally):
    import FWCore.Utilities.FileUtils as FileUtils
    process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
    mylist = FileUtils.loadListFromFile('datafiles/DYJets_m50to150.txt')
    readFiles = cms.untracked.vstring( *mylist)

process.source = cms.Source ("PoolSource",
	 fileNames = readFiles
#	  fileNames = cms.untracked.vstring (options.inputFiles),
)

process.demo = cms.EDAnalyzer('DYJetsAnalyzer',
	genParticles = cms.InputTag("genParticles"),
        genJets = cms.InputTag("ak4GenJets"),
	genInfo = cms.InputTag("generator")
)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("GEN_Histos.root"),
                                   closeFileFast = cms.untracked.bool(False)
)


process.p = cms.Path(process.demo)

