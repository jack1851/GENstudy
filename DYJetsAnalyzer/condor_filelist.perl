#!/usr/bin/perl

use Getopt::Long;
use File::Basename;

#------------------------
#$prodSpace=$ENV{"HOME"}."/work";
#$prodSpace="/data/cmszfs1/user/".$ENV{"USER"};
$prodSpace="/data/cmszfs1/user/".$ENV{"USER"}."/WR/DY_GEN_STUDY/WR_ANALYZE";
$batch=10;
$startPoint=0;
$nosubmit='';
$use_xrootd=''; # '' is false in perl


$rt=$ENV{"LOCALRT"};
$arch=$ENV{"SCRAM_ARCH"};

$jobBase="default";

GetOptions(
    "batch=i" => \$batch,
    "start=i" => \$startPoint,
    "nosubmit" => \$nosubmit,
    "prodspace=s" => \$prodSpace,
    "jobname=s" => \$jobBase,
    "xrootd" => \$use_xrootd,
    "nice" => \$nice_user,
    "resubmit" => \$resub
);

$executable="$prodSpace/$jobBase/cfg/$jobBase";
print "$executable\n";
open(OUTP,">$executable");
print OUTP "#!/bin/sh\n";
print OUTP "source /local/grid/cmssoft/cms/cmsset_default.sh\n";
print OUTP "pwd\n";
print OUTP "hostname\n";
print OUTP "export SINGULARITY_BIND=\"/local/,/export/scratch/,/home/\"\n";
print OUTP "cmssw-cc7 --command-to-run ".$ENV{"HOME"}."/batch_cmsRun \$@";
close(OUTP);
chmod 0755,$executable;
print "$#ARGV\n";
$nargs = $#ARGV;

#if ($#ARGV!=3 && $#ARGV!=6) {
#    print "Usage: [BASE CONFIG] [NAME OF FILE CONTAINING LIST OF FILENAMES] [isMC=True/False] [runRandomTrack=True/False] [runLocally=True/False] [isSig=True/False] [hasDpho=True/False]\n\n";
#    print "    --batch (number of files per jobs) (default $batch)\n";
#    print "    --start (output file number for first job) (default $startPoint)\n";
#    print "    --jobname (name of the job) (default based on base config)\n";
#    print "    --prodSpace (production space) (default $prodSpace)\n";
#    print "    --nosubmit (don't actually submit, just make files)\n";
#    print "    --xrootd (use xrootd for file access)\n";
#    print "    --nice (set nice_user=true)\n";
#    exit(1);
#}

$basecfg=shift @ARGV;
$filelist=shift @ARGV;
$cmsRunArguments=shift @ARGV;
for(my $i = 0; $i <= $nargs-3; $i++)
{  
   $nextArg=shift @ARGV;
   $cmsRunArguments=$cmsRunArguments." ".$nextArg;    
}
print "cmsRun Arguments: $cmsRunArguments\n";

if ($jobBase eq "default") {
    my $stub3=$basecfg;
    $stub3=~s|.*/||g;
    $stub3=~s|_cfg.py||;
    $stub3=~s|[.]py||;
    $jobBase=$stub3;
}


if (length($rt)<2) {
    print "You must run \"cmsenv\" in the right release area\n";
    print "before running this script!\n";
   exit(1);
}

if ($use_xrootd) {
    # Try to find the user's proxy file
    open(VOMSY,"voms-proxy-info|");
    while (<VOMSY>) {
        if (/path\s+:\s+(\S+)/) {
            $voms_proxy=$1;
        }
    }
    close(VOMSY);
}
#------------------------

print "Setting up a job based on $basecfg into $jobBase using $filelist\n";
if ($nosubmit) {
    print "  Will not actually submit this job\n";
}

$cfg=$basecfg;

#system("mkdir -p $prodSpace/$jobBase");
#mkdir("$prodSpace/$jobBase/cfg");
#mkdir("$prodSpace/$jobBase/log");

system("mkdir -p -m755 $prodSpace/$jobBase");
system("mkdir -p -m755 $prodSpace/$jobBase/cfg");
system("mkdir -p -m755 $prodSpace/$jobBase/log");

$linearn=0;

srand(); # make sure rand is ready to go
if ($nosubmit) {
    open(SUBMIT,">condor_submit.txt");
} else {
    open(SUBMIT,"|condor_submit");
}
print(SUBMIT "Executable = $executable\n");
print(SUBMIT "Arguments = \"$cmsRunArguments\"\n");
print(SUBMIT "Universe = vanilla\n");
print(SUBMIT "initialdir = $prodSpace/$jobBase\n");
print(SUBMIT "Output = $prodSpace/$jobBase/log/output.\$(Process)\n");
print(SUBMIT "Error = $prodSpace/$jobBase/log/error.\$(Process)\n");
print(SUBMIT "request_memory = 1G\n");
print(SUBMIT "should_transfer_files = YES\n");
if ($use_xrootd) {
    # If the proxy file exists and is a normal file, we use it
    if (-f $voms_proxy) {
        print("Found voms proxy: $voms_proxy\n");
        print(SUBMIT "should_transfer_files = YES\n");
        print(SUBMIT "transfer_input_files = $voms_proxy\n");
        print(SUBMIT "X509UserProxy = $voms_proxy\n");
    }
    # Invalid file
    else {
        print("No voms proxy found! Please run `voms-proxy-init` and confirm that the file exists at /tmp/x509*\n");
        exit(1);
    }
}
if ($nice_user) {
    print(SUBMIT "nice_user = True\n");
}

open(FLIST,$filelist);
while (<FLIST>) {
    chomp;
    push @flist,$_;
}
close(FLIST);

$i=0;
$ii=$startPoint-1;

while ($i<=$#flist) {
    $ii++;

    @jobf=();
    for ($j=0; $j<$batch && $i<=$#flist; $j++) {
        if ($flist[$i]=~/file:/){
           $flist[$i]=~s|file:||;
        }
        push @jobf,$flist[$i];
        $i++;
    }

    $jobCfg=specializeCfg($cfg,$ii,@jobf);

    $stub=$jobCfg;
    $stub=~s|.*/([^/]+)_cfg.py$|$1|;
    $log="$prodSpace/$jobBase/log/$stub.log";
    $elog="$prodSpace/$jobBase/log/$stub.err";
    $sleep=(($ii*2) % 60)+2;  # Never sleep more than a ~minute, but always sleep at least 2
    print(SUBMIT "Arguments = $arch $rt $prodSpace/$jobBase $jobCfg $log $elog $fname $sleep $cmsRunArguments\n");
    print(SUBMIT "transfer_input_files = ");
    for ($jobNum=0; $jobNum<=$#jobf-1; $jobNum++) {
    	print(SUBMIT "$jobf[$jobNum],");
    }
    print(SUBMIT "$jobf[$#jobf]\n");
    print(SUBMIT "Queue\n");
}

close(SUBMIT);


sub specializeCfg($$@) {
    my ($inp, $index, @files)=@_;


    $stub2=$jobBase;
    $stub2.=sprintf("_%03d",$index);

    $mycfg="$prodSpace/$jobBase/cfg/".$stub2."_cfg.py";
    print "   $inp $index --> $stub2 ($mycfg) \n";
    #print "$inp $text\n";
    open(INP,$inp);
    open(OUTP,">$mycfg");
    $sector=0;
    $had2=0;
    $had3=0;
    while(<INP>) {
        if (/TFileService/) {
            $sector=2;
            $had2=1;
        }
        if (/PoolOutputModule/) {
            $sector=3;
            $had3=1;
        }
        if (/[.]Source/) {
            $sector=1;
        }
        if (/rivetAnalyzer[.]OutputFile/) {
            $sector=4;
        }
        # TFile Service Block
        if ($sector==2 && /^[^\#]*fileName\s*=/) {
            if ($had3==1) {
                $fname="$prodSpace/$jobBase/".$stub2."-hist.root";
            } else {
                $fname=$stub2.".root";
            }
            unlink($fname);
            print OUTP "       fileName = cms.string(\"$fname\"),\n";
            # PoolOutputModule Block
        } elsif ($sector==3 && /^[^\#]*fileName\s*=/) {
            if ($had2==1) {
                $fname="$prodSpace/$jobBase/".$stub2."-pool.root";
            } else {
                $fname="$prodSpace/$jobBase/".$stub2.".root";
            }
            unlink($fname);
            print OUTP "       fileName = cms.untracked.string(\"$fname\"),\n";
            # *Source Block (PoolSource, etc.)
        } elsif ($sector==4 && /^[^\#]*rivetAnalyzer[.]OutputFile\s*=/) {          
                $fname="$prodSpace/$jobBase/".$stub2.".yoda";
            unlink($fname);
            print OUTP "process.rivetAnalyzer.OutputFile = cms.string(\"$fname\")\n";
            # PoolOutputModule Block
        } elsif ($sector==1 && /^[^\#]*fileNames\s*=/) {
            print OUTP "    fileNames=cms.untracked.vstring(\n";
            for ($qq=0; $qq<=$#files; $qq++) {
                $storefile=$files[$qq];
                if ($storefile=~/store/) {
                    if ($use_xrootd) {
                        $storefile=~s|.*/store|root://cmsxrootd.fnal.gov//store|;
                    } else {
                        $storefile=~s|.*/store|/store|;
                    }
                } else {
                    my($filename, $dirs, $suffix) = fileparse($storefile);
                    $storefile="file:".$filename;
                }

                print OUTP "         '".$storefile."'";
                print OUTP "," if ($qq!=$#files);
                print OUTP "\n";
            }
            print OUTP "     )\n";
        } else {
            print OUTP;
        }

        $depth++ if (/\{/ && $sector!=0);
        if (/\}/ && $sector!=0) {
            $depth--;
            $sector=0 if ($depth==0);
        }
#   printf("%d %d %s",$sector,$depth,$_);

    }
    close(OUTP);
    close(INP);
    return $mycfg;
}
