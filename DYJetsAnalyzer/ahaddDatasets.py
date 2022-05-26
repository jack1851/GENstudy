import sys
import os
import subprocess
from shutil import copyfile

if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "=========="
    print "This program takes collections of histograms, ROOT file by ROOT file and \ncombines each set together into one ROOT file"
    print "In order, please specify:"
    print "Text file contain list of datasets:"
    print "Directory where the datasets are stored:"
    print "Destination directory to put combined files in:"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python ahadddatasets.py TTToSemiLeptonic /eos/cms/store/user/aevans/undefined_2018Jun15_111107 /afs/cern.ch/work/a/aevans/public/thesis/datasetStacks/"
    print ""
    exit(0)
if len(sys.argv) != 4:
    print "inputs not understood, try --help/-h"
    exit(1)
#proceed with input parsing

datasetsList = sys.argv[1]
datasetsROOToutputDir = sys.argv[2]
datasetsROOTdestination = sys.argv[3]



datasets = []
datasets.append(datasetsList)
print datasets
#run over datasets

for dataset in datasets:
    ahaddOut = datasetsROOTdestination+"/"+dataset+".root"
    removePrevious = "rm "+ahaddOut
    print removePrevious
    subprocess.call(removePrevious, shell=True)
    ahaddCommand = "./ahadd.py "+ahaddOut+" "+datasetsROOToutputDir+"/*.root"+" --jobs 1"
    #ahaddCommand = "hadd "+ahaddOut+" "+datasetsROOToutputDir+"/*.root"
    print ahaddCommand
    subprocess.call(ahaddCommand, shell=True)   
print "CHECKING FOR EXT DATASETS"
for line in os.listdir(datasetsROOToutputDir):
    if "ext" in line:
        print line
