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
   def __init__(self, mc, lumi):
      self.mcFiles=mc
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
      #Use # of muons passing tag for total weight because I know that there was no over/underflow
      eff = resultFile.Get("demo/"+name).Integral()/skimFile.Get("demo/allPassingEvents/eventWeight").Integral()
      return eff

   def stack(self, name, xtitle, log, tag, outDir,rebin=False):
      stackplot = ROOT.THStack("stack","")     
      leg = ROOT.TLegend(0.6,0.75,0.85,0.88)
      leg.SetBorderSize(0)
      leg.SetTextSize(0.035)
      leg.SetTextFont(42)
      leg2 = ROOT.TLegend(0.35,0.75,0.6,0.88)
      leg2.SetBorderSize(0)
      leg2.SetTextSize(0.035)
      leg2.SetTextFont(42)
      hists = []
      count = 0
      for mcFile in self.mcFiles:
         eff = self.calcScaleFactor(name,mcFile.skim,mcFile.result)
	 print "mcFile: ", mcFile
	 print "nevents: ", eff
         Nevents = mcFile.cx*1000*float(self.lumi)*eff
         hist=(mcFile.result.Get("demo/"+name).Clone())
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

      for hist in hists:
	 hist.Sumw2()
         stackplot.Add(hist)

      plot= ROOT.TCanvas("stackplot","stackplot",2)
      ROOT.gStyle.SetTextFont(43)
      pad1 = ROOT.TPad("pad1","pad1",0,0,1,1)
      pad2 = ROOT.TPad('pad2','pad2',0,0.0,1.0,0.3)
#      pad1.SetBottomMargin(0)
      pad2.SetTopMargin(0.)
      pad2.SetBottomMargin(0.35)

      pad1.SetFillStyle(4000)
      pad1.SetFrameFillStyle(1000)
      pad1.SetFrameFillColor(0)
      pad2.SetFillStyle(4000)
      pad2.SetFrameFillStyle(1000)
      pad2.SetFrameFillColor(0)
      pad1.Draw()
#      pad2.Draw()
      pad1.cd()
     
      stackplot.Draw("hist")

      print "stackplot.Integral(): ", stackplot.GetStack().Last().Integral()

#      dataHist.Draw("psame") 
      if(log):
         pad1.SetLogy()
         ROOT.gPad.SetLogy()
      maximum=stackplot.GetMaximum()
      minimum=min(stackplot.GetMinimum(),0)
      if log:
	 minimum=max(stackplot.GetMinimum(),100)
      if not log:
         minimum=0      
      
      if not log:
         stackplot.SetMaximum(maximum*1.2)
      else:
         stackplot.SetMaximum(maximum*6)
      stackplot.SetMinimum(minimum)
      stackplot.GetXaxis().SetTitleOffset(1)
      stackplot.GetXaxis().SetTitle(xtitle)
      #stackplot.GetXaxis().SetTitleSize(0.5)
      stackplot.GetXaxis().SetTickSize(0)
      stackplot.Draw("hist")
  
      allMC = stackplot.GetStack().Last().Clone()

      leg2.Draw()
      
      label = ROOT.TLatex(0.65,0.905,"%.2f fb^{-1} (13 TeV)"%float(self.lumi))
      label.SetNDC()
      label.SetTextFont(42)
      label.SetTextSize(0.033)
      label.Draw()
      
      stackplot.GetYaxis().SetTitle("Events")
      stackplot.GetYaxis().SetTitleSize(20)
      stackplot.GetYaxis().SetTitleFont(43)
      stackplot.GetYaxis().SetLabelFont(43)
      stackplot.GetYaxis().SetLabelSize(15)
      stackplot.GetYaxis().SetTitleOffset(1.15)
      
      lab_x0 = 0.10
      lab_y0 = 0.905
      tag1 = ROOT.TLatex(lab_x0,lab_y0,"CMS")
      tag1.SetNDC()
      tag1.SetTextFont(62)
      tag2 = ROOT.TLatex(lab_x0+0.08, lab_y0, "Internal")
      tag2.SetNDC()
      tag2.SetTextFont(52)
      tag1.SetTextSize(0.04)
      tag2.SetTextSize(0.03)
      tag1.Draw()
      tag2.Draw()

      plot.Modified()

      img = ROOT.TImage.Create()
      img.FromPad(plot)
      img.WriteImage(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.png")
      plot.Print(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.pdf")
      #plot.Print(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.C")
