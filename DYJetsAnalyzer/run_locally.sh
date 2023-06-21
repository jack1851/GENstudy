source /cvmfs/cms.cern.ch/cmsset_default.sh
export SINGULARITY_BIND=/home/,/local/cms/user/,/export/scratch/
cmssw-cc7 --ignore-mount /cvmfs/grid.cern.ch/etc/grid-security,/cvmfs/grid.cern.ch/etc/grid-security/vomses --command-to-run cmsenv --command-to-run scram b --command-to-run cmsRun python/Config_DYJetsAnalyzer.py runLocally=True
