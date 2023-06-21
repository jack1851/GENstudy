 #!/usr/bin/env python

import argparse
import os
import ROOT
import stackplot_tool_comparison as stack

parser = argparse.ArgumentParser(description="Create stack plots for the mu+X skim")
parser.add_argument("--o", "--outputdir"  , dest="outDir"    , help="output directory"          , default=os.getcwd()+"/plots/")
parser.add_argument("--l", "--lumi", dest="lumi", help="luminosity", default=0)
parser.add_argument("--t", "--title", dest="label", help="Data label", default="Mu+X Data Skim")
arg = parser.parse_args()

outDir = arg.outDir
#Check for trailing slash on ouput dir and delete
if arg.outDir.split("/")[-1] == "": outDir = arg.outDir[:-1]
if not os.path.isdir(outDir):
     print "Output directory " + outDir + " does not exist!"
     quit()

#Cross Sections in pb
WJetsCx=61527.6
TTToSemiLeptonicCx=365.34
TTTo2L2NuCx=88.29
DYJetsCx_1=3.421 ### For the private RECO DY sample
#DYJetsCx_xqcut20=1586*0.6625093123055389 ### For the private RECO DY sample
DYJetsCx_2=3.091
DYJets10to50Cx=18610
WWCx=118.7

mcFiles = []
mcFiles.append(stack.mcFile(ROOT.TFile("hists/m200/xqcut10/lepton_cuts/DYJets_m200_20ptl.root","READ"),ROOT.TFile("hists/m200/xqcut10/lepton_cuts/DYJets_m200_20ptl.root","READ"), " m_{ll} > 200 (ptl = 20 GeV)",DYJetsCx_1,4))

#info = stack.stackInfo(mcFiles, arg.lumi)
info = stack.stackInfo(mcFiles, arg.lumi, ROOT.TFile("hists/m200/xqcut10/lepton_cuts/DYJets_m200_80ptl.root","READ"),DYJetsCx_2)

info.plotAll("allPassingEvents","Passing Events",outDir)
#info.plotAll("allPassingEvents_dileptonM200to300","Passing Events",outDir)
#info.plotAll("allPassingEvents_dileptonM300to400","Passing Events",outDir)
#info.plotAll("allPassingEvents_dileptonM400","Passing Events",outDir)


