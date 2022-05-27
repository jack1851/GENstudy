# GENstudy

Setup up repo by running the following:

```
1. cmsrel CMSSW_10_6_18
2. cd CMSSW_10_6_18/src/
3. cmsenv
4. git clone https://github.com/Michael-Krohn/GENstudy.git
5. scram b
6. cd GENstudy/DYJetsAnalyzer/
```

First, create a txt file that contains the file paths for every GEN root file, one per line. Also, begin each file path with 'file:/hdfs/...'. For an example, see datafiles/DYJets_m200.txt


## Run locally
```
cmsRun python/Config_DYJetsAnalyzer.py runLocally=True
```

## Run on condor

```
./condor_filelist.perl python/Config_DYJetsAnalyzer.py datafiles/DYJets_m200.txt --batch 1 --jobname Config_DYJetsAnalyzer_DYJets_m200
```

Once jobs have finished running, combine them into a single root file:

```
python ahaddDatasets.py <Name of dataset> <Path to root files output from condor> <output directory>
```

Plot the hists from the output root file:
```
python plotting/allVariableStacks.py  --o plots/ --l 59.74
```
You can modify which plot are plotted by editing plotting/stackplot_tool.py
