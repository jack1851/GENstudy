 #!/usr/bin/env python

import ROOT
import copy
import math

class mcFile:
   def __init__(self, skimfile, resultsfile, name, cross, fillcolor):
      self.skim=skimfile
      self.result=resultsfile
      self.label=name
      self.cx=cross
      self.color=fillcolor

class stackInfo:
   def __init__(self, mc, lumi,data=None):
      self.mcFiles=mc
      self.dataFile=data
      print "lumi: ", lumi
      self.lumi=lumi
   def plotAll(self,basedir,tag,outDir):
      #Histogram name, X-axis label, Log Scale, Title, Output Directory, Rebin
      self.stack(basedir+"/fourObjectInvariantMass","m_{lljj} (GeV)",True,tag,outDir, True)
 

   def getRatio(self, hist, reference):
        ratio = hist.Clone("%s_ratio"%hist.GetName())
        ratio.SetDirectory(0)
        ratio.SetLineColor(hist.GetLineColor())
        for xbin in xrange(1,reference.GetNbinsX()+1):
                ref = reference.GetBinContent(xbin)
                val = hist.GetBinContent(xbin)

                refE = reference.GetBinError(xbin)
                valE = hist.GetBinError(xbin)

                try:
                        ratio.SetBinContent(xbin, val/ref)
                        ratio.SetBinError(xbin, math.sqrt( (val*refE/(ref**2))**2 + (valE/ref)**2 ))
                except ZeroDivisionError:
                        #ratio.SetBinContent(xbin, 1.0)
                        ratio.SetBinError(xbin, 0.0)

        return ratio

   def TOTerror(self, hmc, ratio ):
      hmc.Sumw2()
      den1 = hmc.Clone ("den1");
      den2 = hmc.Clone ("den2");

      nvar = hmc.GetNbinsX();

      x1 = []
      y1 = []
      exl1 = []
      eyl1= []
      exh1= []
      eyh1= []

      for km in range(1,nvar+1):
        delta = hmc.GetBinError(km)
        den1.SetBinError(km,0)

      # ratio from variation and nominal
      ratiop = hmc.Clone("ratiop");
      ratiom = hmc.Clone("ratiom");

      ratiop.Divide(den1);
      ratiom.Divide(den1);
      #den1.Divide(ratiop)
      #den2.Divide(ratiom)

      return ratiop;


   def calcScaleFactor(self,name,skimFile,resultFile):
      eff = resultFile.Get("demo/"+name).Integral()/skimFile.Get("demo/allPassingEvents/eventWeight").Integral()
      return eff

   def stack(self, name, xtitle, log, tag, outDir,rebin=False):
      stackplot = ROOT.THStack("stack","")     
      leg = ROOT.TLegend(0.6,0.75,0.85,0.88)
      leg.SetBorderSize(0)
      leg.SetTextSize(0.035)
      leg.SetTextFont(42)
      leg2 = ROOT.TLegend(0.15,0.75,0.4,0.88)
      leg2.SetBorderSize(0)
      leg2.SetTextSize(0.035)
      leg2.SetTextFont(42)
      hists = []
      count = 0
      for mcFile in self.mcFiles:
         eff = self.calcScaleFactor(name,mcFile.skim,mcFile.result)
	 print "nevents: ", eff
	 print "mcFile xsec: ", mcFile.cx
         Nevents = mcFile.cx*1000*float(self.lumi)*eff
         hist=(mcFile.result.Get("demo/"+name).Clone())
         #hist.Add(mcFile.result.Get("demo/"+nextname))
         if(hist.Integral()>0):
            hist.Scale(Nevents/hist.Integral())
         hist.SetFillColor(mcFile.color)
         hist.SetLineWidth(0)
         if(rebin):
            hist.Rebin(4)
         hist.SetDirectory(0)
         hists.append(hist)
	 if count < 2:
           leg2.AddEntry(hist, mcFile.label,"f")
	 else:
           leg.AddEntry(hist, mcFile.label,"f")
	 count += 1

      dataHist = self.dataFile.Get("demo/"+name).Clone()
      for hist in hists:
	 print "MC hist integral(): ", hist.Integral()
	 hist.Sumw2()
         #hist.Scale(dataHist.Integral()/hist.Integral())
         stackplot.Add(hist)

      plot= ROOT.TCanvas("stackplot","stackplot",2)
      ROOT.gStyle.SetTextFont(43)
      pad1 = ROOT.TPad("pad1","pad1",0,0.3,1,1)
      pad2 = ROOT.TPad('pad2','pad2',0,0.0,1.0,0.3)
      pad1.SetBottomMargin(0)
      pad2.SetTopMargin(0.)
      pad2.SetBottomMargin(0.35)

      pad1.SetFillStyle(4000)
      pad1.SetFrameFillStyle(1000)
      pad1.SetFrameFillColor(0)
      pad2.SetFillStyle(4000)
      pad2.SetFrameFillStyle(1000)
      pad2.SetFrameFillColor(0)
      pad1.Draw()
      pad2.Draw()
      pad1.cd()
     
      dataHist.Sumw2()
      dataHist.SetLineWidth(0)
      dataHist.SetLineColor(1)
      dataHist.SetMarkerStyle(21)
      dataHist.SetMarkerSize(0.5)
      if(rebin):
         dataHist.Rebin(4)
       
      dataHist.SetStats(0)
      stackplot.Draw("hist")
      print "dataHist.Integral(): ", dataHist.Integral()
      dataHist.Draw("psame") 
      if(log):
         pad1.SetLogy()
         ROOT.gPad.SetLogy()
      if self.dataFile is not None:
         maximum=max(stackplot.GetMaximum(),dataHist.GetMaximum())
         minimum=min(stackplot.GetMinimum(),dataHist.GetMinimum(0))
      else:
         maximum=stackplot.GetMaximum()
         minimum=stackplot.GetMinimum()
      if(minimum==0 and self.dataFile is not None):
         minimum=dataHist.GetMinimum(0)
      if not log:
         minimum=0      
      
      if not log:
         stackplot.SetMaximum(maximum*1.2)
      else:
         stackplot.SetMaximum(maximum*6)
      stackplot.SetMinimum(minimum*0.2)
      stackplot.GetXaxis().SetTitleOffset(2)
      stackplot.GetXaxis().SetTitle("")
      stackplot.GetXaxis().SetTickSize(0)
      stackplot.Draw("hist")
      if self.dataFile is not None:
         dataHist.Draw("psame") 
  
      newDataHist = copy.deepcopy(dataHist)
      allMC = stackplot.GetStack().Last().Clone()

      herr = allMC.Clone('herr')
      theErrorGraph = ROOT.TGraphErrors(herr)
      theErrorGraph.SetFillColor(ROOT.kGray+2)
      theErrorGraph.SetFillStyle(3002)
      herr.SetFillColor(ROOT.kGray+2)
      herr.SetFillStyle(3002)
      herr.SetMarkerColor(1111)

      theErrorGraph.Draw('SAME2')

      leg.AddEntry(dataHist, "Data", "p") 
      leg.Draw()
      leg2.Draw()
      
      label = ROOT.TLatex(0.73,0.905,"%.2f fb^{-1} (13 TeV)"%float(self.lumi))
      label.SetNDC()
      label.SetTextFont(42)
      label.SetTextSize(0.033)
      label.Draw()
      
      stackplot.GetYaxis().SetTitle("Events")
      stackplot.GetYaxis().SetTitleSize(20)
      stackplot.GetYaxis().SetTitleFont(43)
      stackplot.GetYaxis().SetTitleOffset(1.2)
      stackplot.GetYaxis().SetLabelFont(43)
      stackplot.GetYaxis().SetLabelSize(15)
#      stackplot.GetXaxis().SetLabelFont(43)
#      stackplot.GetXaxis().SetLabelSize(15)
#      stackplot.GetXaxis().SetTickSize(0.07)
      stackplot.GetXaxis().SetTitleOffset(2)
      stackplot.GetXaxis().SetTitle("")
      stackplot.GetXaxis().SetTickSize(0)
#      xlabel = ROOT.TPaveText(0.2,0.0,0.99,0.06,"brNDC")
#      xlabel.SetTextSize(20)
#      xlabel.AddText(xtitle)
#      xlabel.SetFillColor(0)
#      xlabel.SetBorderSize(0)
#      xlabel.Draw()
      
      lab_x0 = 0.10
      lab_y0 = 0.905
      tag1 = ROOT.TLatex(lab_x0,lab_y0,"CMS")
      tag1.SetNDC()
      tag1.SetTextFont(62)
      tag2 = ROOT.TLatex(lab_x0+0.06, lab_y0, "Internal")
      tag2.SetNDC()
      tag2.SetTextFont(52)
      tag1.SetTextSize(0.04)
      tag2.SetTextSize(0.03)
      tag1.Draw()
      tag2.Draw()

      plot.Modified()

      pad2.cd()

      ratio = self.getRatio(newDataHist,allMC)
      herr3= self.TOTerror(allMC,ratio);
      toterree = ROOT.TGraphErrors(herr3)

      ratio.SetStats(0)
      ratio.GetYaxis().SetRangeUser(0.25,2.5)
      ratio.GetYaxis().SetNdivisions(504)
      ratio.GetYaxis().SetTitle("Data/Simulation")
      ratio.GetXaxis().SetTitle(xtitle)

      ratio.GetXaxis().SetTitleSize(0.14)
      ratio.GetXaxis().SetTitleOffset(1.0)
      ratio.GetXaxis().SetLabelOffset(0.005)
      ratio.GetYaxis().SetTitleOffset(0.4)
      ratio.GetYaxis().SetLabelSize(0.12)
      ratio.GetYaxis().SetTitleSize(0.11)
      ratio.GetXaxis().SetLabelSize(0.11)

      ratio.SetLineStyle(1)
      ratio.SetLineWidth(2)

      line = ROOT.TLine(ratio.GetXaxis().GetXmin(), 1.0,
                      ratio.GetXaxis().GetXmax(), 1.0)
      line.SetLineColor(ROOT.kGray)
      line.SetLineStyle(2)
      line.Draw()
      tKsChi = ROOT.TLatex()
      tKsChi.SetNDC()
      tKsChi.SetTextFont(42)
      tKsChi.SetTextSize(0.09)

      ratio.Draw("PE")

      toterree.SetFillColor(ROOT.kGray+2)
      toterree.SetLineColor(ROOT.kGray+2)
      toterree.SetFillStyle(3002)
#        toterree.Draw("2 same")
      line.Draw("same")

      leg4 = ROOT.TLegend(0.7,0.89,0.5,0.8)
      leg4.SetFillStyle(0)
      leg4.SetBorderSize(0)
      leg4.SetTextSize(0.05)
      leg4.SetTextFont(42)
      leg4.AddEntry(toterree,"MC uncert. (stat.)","fl")
#      leg4.Draw()

      
      img = ROOT.TImage.Create()
      img.FromPad(plot)
      img.WriteImage(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.png")
      plot.Print(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.pdf")
      #plot.Print(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.C")

   def cutFlowText(self, name, outDir):
      outfile = open(outDir+"/CutProgress.txt","w")
      cutList = [ 'All Events',
                  'Tight ID muon',
                  'Tight PF Iso muon',
                  'Muon pt > 26 GeV and |eta|<2.',
                  'Muon is global',
                  'Track With >20 GeV pt >0.2 dR from muon',
                  'Track |eta|>1.45 and <2.4',
                  'Track is High Purity',
                  'Track Belongs to PV',
                  'Track Longitudinal Impact Parameter <0.05',
                  'Track Transverse Impact Parameter <0.005',
                  'Tag and Probe Opposite Charge',
                  'Tag and Probe valid fitted vertex',
                  'Fitted vtx reduced chi^2<3.0',
                  'Tag Probe Invariant Mass >50  <150 GeV',
                  'Probe track ecal iso > 30 GeV or tk iso>0.1',
                  'No other probes with dR>0.3 pair with track']
     
      hists = [] 
      for mcFile in self.mcFiles:
         eff = self.calcScaleFactor(name,mcFile.skim,mcFile.result)
         #neff = self.calcScaleFactor(nextname,mcFile.skim,mcFile.result)
         Nevents = mcFile.cx*1000*float(self.lumi)*eff
         #Nevents = Nevents+mcFile.cx*1000*float(self.lumi)*neff
         #NtotalEvents=NtotalEvents+Nevents
         hist=(mcFile.result.Get("demo/"+name).Clone())
         #hist.Add(mcFile.result.Get("demo/"+nextname))
         if(hist.Integral()>0):
            hist.Scale(Nevents/hist.Integral())
         hists.append(hist)

      stackplot = hists[0].Clone()    
      for hist in hists:
         if(hist!=hists[0]):
            stackplot.Add(hist)

      outfile.write("Cut Name,WW,TTTo2L2Nu,TTToSemiLeptonic,WJets,DYJets,DYJets_10to50\n")
      for n in range(2,len(cutList)+2):
         outfile.write(cutList[n-2])
         for hist in hists:
            if(hist.GetBinContent(n)>0):
               outfile.write(","+str(hist.GetBinContent(n)))
            else:
               outfile.write(",0")
         outfile.write("\n")

#      outfile.write("Data:\n")
#      dataHist=self.dataFile.Get("demo/allEvents/cutProgress").Clone()
#      for n in range(1,len(cutList)+2):
#         if(dataHist.GetBinContent(n)>0):
#            outfile.write(cutList[n-2]+","+str(dataHist.GetBinContent(n))+"\n")
    
      outfile.close()
