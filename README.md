# WR-Project

## General Linux 

### Logging in
```
ssh -Y jack1851@spartha.spa.umn.edu
ssh -Y zebra01
```
### Copying a file from ssh to local

scp jack1851@zebra02.spa.umn.edu:/path/to/file/on/zebra/file_on_zebra.txt /home/jack1851/WR_local/files

scp /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/plots/m200/dsc1

### Copying a folder from ssh to local

scp -r /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/plots/m200/dsc1 /home/jack1851/WR_local/plots

## Sample Generation

The following is an example of how to create a complete background sample from start to finish. We will be doing a large run (over 1 million events). The parameters of interest are: requiring at least one jet to be produced, a minimum invariant dilepton mass of $50< m_{ll}<200 \text{ GeV}$, and a parton seperation of $\texttt{xqcut = 10}$.

## Gridpack Creation

A gridpack is a tarball (compressed) file containing all the information to run a certain process in Powheg or MadGraph. The base directory for gridpack generation is

```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/genproductions/bin/MadGraph5_aMCatNLO
```

#### Edit Input Parameters

Producing a gridpack requires two main files that act as templates: a run card and a processing card. These cards are located at:
```
cd input_cards/DYJets_50m200_InputCards
vim DYJets_50m200_0ptl_proc_card.dat
vim DYJets_50m200_0ptl_run_card.dat
```
edit them accordingly.

#### Submit the gridpack script

```
./submit_condor_gridpack_generation.sh DYJets_50m200_0ptl input_cards/DYJets_50m200_InputCards/
```

where the format of the command is: 

```
./submit_condor_gridpack_generation.sh gridpackname carddir
```

Note that the gridpack name needs to match the name of the input cards above. Once the gridpack has finished being generated, there should be three more files in the MadGraph5_aMCatNLO/ directory:

```
DYJets_50m200_0ptl/
DYJets_50m200_0ptl.log
DYJets_50m200_0ptl_slc7_amd64_gcc700_CMSSW_10_6_19_tarball.tar.xz
```

#### Copy the tarbell to the Gridpack directory

The LHE generation script takes the newly generated tarbell as an input file, so we need to copy it to the gridpack directory 
```
cp DYJets_50m200_0ptl_slc7_amd64_gcc700_CMSSW_10_6_19_tarball.tar.xz DYJets_50m200_dsc4_1to3j/DYJets_50m200_dsc4_1to3j_gridpack/work/gridpack/
```
#### Move Gridpack outputs to the Samples directory

Move these Gridpack outputs to the Samples directory. In this case:

```
/data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/genproductions/bin/MadGraph5_aMCatNLO/samples/50m200/
```

## LHE File Creation

The Les Houches Event file format (LHE) is an agreement between Monte Carlo event generators and theorists to define Matrix Element level event listings in a common language.

#### Edit the submit description file

Open the submit description file:

```
vim /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/genproductions/bin/MadGraph5_aMCatNLO/submit_LHE_generation.sh  
```

In that file, edit the $\texttt{InitialDir}$, $\texttt{transfer input files}$, and $\texttt{Arguments}$ options. The third argument is the number of events in each file $(\texttt{200000})$, and $\texttt{Queue 10}$ means we are creating 10 LHE files. We expect 2 million events (10 files), but some may fail. 

#### Submit LHE jobs to Condor

To submit the LHE creation jobs to condor, run from the MadGraph5_aMCatNLO/ directory:
```
condor_submit submit_LHE_generation.sh
```

This will take about 48 hours. You can check the status of the jobs with

```
condor_q -submitter jack1851 
```

After the jobs have finished, you will see the $\texttt{.lhe}$ files in the gridpack directory. Note the sizes of the files - for some reason there is an error causing some $\texttt{.lhe}$ files to fail, which is indicated by the smaller file size. You can see the exact message in the $\texttt{error\}$ directory. In this case, we have 5 successful files, for a total of 1,000,000 events.

![lhegrab](https://user-images.githubusercontent.com/123029512/214107116-55a131bf-6414-4c9f-91ce-002447fc787a.png)

#### Make new LHE directories and give permissions

First give the $\texttt{.lhe}$ files permissions with $\texttt{chmod 755 \*.lhe}$. Then create new directories with the following commands:

```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/50m200/xqcut10/
mkdir -m755 -p DYJets_50m200_0ptl/LHE
chmod 755 DYJets_50m200_0ptl/
```

#### Move the LHE files to the new directory

We then move the successful LHE files from the gridpack to this new directory. For example:
```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/genproductions/bin/MadGraph5_aMCatNLO/samples/50m200/DYJets_50m200_0ptl/DYJets_50m200_0ptl_gridpack/work/gridpack
mv cmsgrid_final_30181.lhe* /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/50m200/DYJets_50m200_0ptl/LHE
```
Check that the files successfully transferred and the permissions are correct:
```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/50m200/xqcut10/DYJets_50m200_0ptl/LHE 
```

## GEN (.root) File Creation

Go to the correct directory:

```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/MCsampleProduction/FullSimulation/RunIISummer20UL16/GEN__CMSSW_10_6_22/src
```

#### Make .csv file

Make a $\texttt{.csv}$ file with the following info. Make sure to input the total number of events (1,000,000) and the number of jobs/batches (400). This means there are $1000000/400 = 2500$ events per batch. This is from DYJets_50m200_0ptl_GEN.csv:

```
DYJets_50m200_0ptl_TuneCP5_13TeV,Hadronizer_TuneCP5_13TeV_MLM_5f_max4j_qCut19_LHE_pythia8_cff.py,2000000,200,/data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/50m200/xqcut10/DYJets_50m200_0ptl/LHE/cmsgrid_final_390920.lhe
```
#### Make and edit run_condor.py

Run the following command:
```
python2.7 config_GEN_condor.py DYJets_50m200_0ptl_GEN.csv --inputLHE
```
You will see the following list of instructions:

![runcondor](https://user-images.githubusercontent.com/123029512/214172277-efa3a2c5-f8a7-4fea-8348-e2eef66ee362.png)

Some notes:
To load the CMS environment:
```
cmsenv
scram b
```
As well as adding the $\texttt{skipEvents}$ line in run_condor.py, also add additional arguments to $\texttt{fileNames}$ with paths to the additional $\texttt{.lhe}$ files. 

#### Submit the jobs to Condor

When we reload the cms environment, we need to give permissions on some lib files:
```
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/MCsampleProduction/FullSimulation/RunIISummer20UL16/GEN__CMSSW_10_6_22/lib/slc7_amd64_gcc700/.poisonededmplugincache
```

Run this command to submit the GEN jobs to condor. It may say an executable file DNE: just resubmit the jobs again:
```
source submit_condor_DYJets_50m200_0ptl_GEN.sh
```
The jobs will fail due to a permission error. We need to give the cfg files permissions:
```
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/DYJets_50m200_dsc4_1to3j_TuneCP5_13TeV/cfg/*.py
condor_rm jack1851
```

Then resubmit the jobs. Each job takes 2-3 hours to complete.
```
source submit_condor_DYJets_50m200_dsc4_1to3j_GEN.sh
```

#### Move the output root files and clean up directories

Go back to the main directory and move the new folders and files:
```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/MCsampleProduction/FullSimulation/RunIISummer20UL16/GEN__CMSSW_10_6_22/src/
mv DYJets_50m200_dsc4_1to3j_GEN/ samples/50m200
mv DYJets_50m200_dsc4_1to3j_GEN.csv samples/50m200
mv submit_condor_DYJets_50m200_dsc4_1to3j_GEN.sh samples/50m200
```
The output root files are located here:
```
cd /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/MCsampleProduction/FullSimulation/RunIISummer20UL16/GEN__CMSSW_10_6_22/src/samples/m200/DYJets_50m200_dsc4_1to3j_GEN/DYJets_50m200_dsc4_1to3j_TuneCP5_13TeV
```
Move them to  the directory we created when we moved the LHE file:
```
mv *.root /data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/m200/DYJets_50m200_dsc4_1to3j
```

## GEN Study (ANALYZE)

#### Load the CMS environment
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src
cmsenv
scram b
```

#### Create a .txt file with all of the root files
Go to the correct directory:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/datafiles
```
Create a txt file that contains the file paths for every GEN root file, one per line. For example in DYJets_m200_dsc2_1to3j.txt
```
file:/data/cmszfs1/user/jack1851/WR/DY_PRODUCTION/WR_DY_GEN/DYJets_50m200_dsc2_1to3j/DYJets_50m200_dsc4_1to3j_TuneCP5_13TeV_000.root
```
#### Run locally
If you change the analyzer and want to test it, you can run one file locally and check the output, before submitting all of the jobs to condor.

Go to the correct directory:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer
```
Go into container and load the environment:
```
cmssw-cc7 --ignore-mount /cvmfs/grid.cern.ch/etc/grid-security,/cvmfs/grid.cern.ch/etc/grid-security/vomses
cmsenv
scram b
```
Submit the command:
```
cmsRun python/Config_DYJetsAnalyzer.py runLocally=true
```
The file will be called GEN_Histos.root.

#### Run on Condor

Go to the correct directory:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer
```
Check permissions in the folder 
```
ls -la /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/lib/slc7_amd64_gcc700
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/lib/slc7_amd64_gcc700/.edmplugincache
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/lib/slc7_amd64_gcc700/libGENstudyDYJetsAnalyzer.so
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/lib/slc7_amd64_gcc700/GENstudyDYJetsAnalyzerAuto.edmplugin
```
Submit the script.
```
./condor_filelist.perl python/Config_DYJetsAnalyzer.py datafiles/50m200/xqcut10/DYJets_50m200_0ptl_xqcut10_qcut19.txt --batch 1 --jobname Config_DYJetsAnalyzer_DYJets_50m200_20ptl
```
If it is the first time submitting jobs to condor, then they will fail because the executable file hasn't been created. Just resubmit the jobs again. You will see that they all submit, however, we need to give permissions to the python config files. To do this:
```
chmod 755 /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/WR_ANALYZE/Config_DYJetsAnalyzer_DYJets_50m200_20ptl/cfg/*.py
```
Then remove the jobs that you just submitted
```
condor_rm jack1851
```
Then resubmit condor_filelist. Check the root config files were produced here:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/WR_ANALYZE/Config_DYJetsAnalyzer_DYJets_50m200_20ptl
```

#### Ahadd the Config.root files 
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer
```
Go into container and load the environment:
```
cmssw-cc7 --ignore-mount /cvmfs/grid.cern.ch/etc/grid-security,/cvmfs/grid.cern.ch/etc/grid-security/vomses
cmsenv
scram b
```
Run the following to combine the foot files into a single root file:
```
python2.7 ahaddDatasets.py DYJets_50m200_20ptl /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/WR_ANALYZE/Config_DYJetsAnalyzer_DYJets_50m200_20ptl/ /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/hists/50m200/xqcut10/lepton_cuts/
```
Where the format of the command is 
```
python2.7 ahaddDatasets.py <Name of dataset> <Path to root files output from condor> <output directory>
```
Check the root file was produced:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/hists/50m200
```

## Plotting

#### Plotting in root (TBrowser).
First make sure you are logged into the machines via $\texttt{ssh -Y}$, then go to the correct directory.
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/hists
```

Go into container and load the environment:
```
cmssw-cc7 --ignore-mount /cvmfs/grid.cern.ch/etc/grid-security,/cvmfs/grid.cern.ch/etc/grid-security/vomses
cmsenv
scram b
```
Then to open root and browse plots, type:
```
root
new TBrowser()
```
Then to open root, type:
```
root
new TBrowser()
```
Then choose the appropriate directory.

#### Making a comparison plot from the script
Go to the plotting directory:
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/plotting
```
For comparison plotting, we use two files: allVariableStacks_comparison.py and stackplot_tool_comparison.py. 

First, edit allVariableStacks_comparison.py and input the names of the two files you want to compare. You can also edit the output directory.
```
vim allVariableStacks_comparison.py
```
Then, open stackplot_tool_comparison.py. Edit the $\texttt{plotAll}$ method, so that the appropriate plot is being made. You can also edit the bin sizes etc here.
```
vim stackplot_tool_comparison.py
```
Then check the directory 
```
cd /data/cmszfs1/user/jack1851/WR/DY_GEN_STUDY/CMSSW_10_6_18/src/GENstudy/DYJetsAnalyzer/plots
```
and check that there is a directory for the plot you want to make. If not, make one. Then go into container and load the environment:
```
cmssw-cc7 --ignore-mount /cvmfs/grid.cern.ch/etc/grid-security,/cvmfs/grid.cern.ch/etc/grid-security/vomses
cmsenv
scram b
```
And submit the plotting script
```
python plotting/allVariableStacks_comparison.py --o plots/2023/allPassingEvents/50m200/numberOfJets --l 59.74
```
