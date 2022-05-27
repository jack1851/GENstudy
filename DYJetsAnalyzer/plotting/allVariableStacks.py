 #!/usr/bin/env python

import argparse
import os
import ROOT
import stackplot_tool as stack

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
DYJetsCx=1586 ### For the private RECO DY sample
DYJets10to50Cx=18610
WWCx=118.7

mcFiles = []
mcFiles.append(stack.mcFile(ROOT.TFile("hists/DYJets_m200.root","READ"),ROOT.TFile("hists/DYJets_m200.root","READ"), "DYJets",DYJetsCx,4))

info = stack.stackInfo(mcFiles, arg.lumi)
info.plotAll("allPassingEvents","Passing Events",outDir)


