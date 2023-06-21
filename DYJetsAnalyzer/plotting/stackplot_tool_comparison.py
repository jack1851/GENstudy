 #!/usr/bin/env python

import ROOT
import copy
import math
import array

class mcFile:
   def __init__(self, skimfile, resultsfile, name, cross, fillcolor):
      self.skim=skimfile
      self.result=resultsfile
      self.label=name
      self.cx=cross
      self.color=fillcolor

class stackInfo:
   def __init__(self, mc, lumi, data, xsec):
      self.mcFiles=mc
      self.mcFile2=data
      self.mcFile2_xsec = xsec
      print "lumi: ", lumi
      self.lumi=lumi
   def plotAll(self,basedir,tag,outDir):
      #Histogram name, X-axis label, Log Scale, Title, Output Directory, Rebin
      self.stack(basedir+"/fourObjectInvariantMass","m_{lljj} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/leadLepPtHisto","Lead lepton p_{T} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/subleadLepPtHisto","Sublead lepton p_{T} (GeV)",True,tag,outDir, True) 
      self.stack(basedir+"/leadJetPtHisto","Lead jet p_{T} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/subleadJetPtHisto","Sublead jet p_{T} (GeV)",True,tag,outDir, True)      
      self.stack(basedir+"/diJetMass","Dijet Mass m_{jj} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/diLepPtHisto","Dilepton  p_{T} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/ptllOverMllHisto","p_{T}_{ll}/m_{ll}",True,tag,outDir, True)
      self.stack(basedir+"/leadJetZMass","m_{jz} (GeV)",True,tag,outDir, True)
      self.stack(basedir+"/subleadJetZMass","m_{jz} (GeV)",True,tag,outDir, True)


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
      if("fourObjectInvariantMass" in name):
        binBoundaries = [0,50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,900,1000,1250,1500, 2000, 3000]
      elif ("leadLepPtHisto" in name):
        binBoundaries = [0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 250, 300, 400, 500, 1000]
      elif ("subleadLepPtHisto" in name):
        binBoundaries = [0, 20,30,40,50, 60,70, 80,90, 100,120,140,160,180,200, 400, 1000]
      elif ("leadJetPtHisto" in name):
        binBoundaries = [30,40,50,60,70,80,100,120,140,160,180,200,250,300,400,600, 800,1000]
      elif ("subleadJetPtHisto" in name):
        binBoundaries = [30,40,50,60,70,80,100,120,140,160,180,200,250,300, 400,600,800, 1000]
      elif ("diJetMass" in name):
        binBoundaries = [0, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 450, 500, 550, 600, 700, 800, 900, 1000, 1250,1500, 2000, 3000]
      elif ("diLepPtHisto" in name):
        binBoundaries = [0, 25, 50, 100, 150, 200, 250, 400,600,2000]
      elif ("ptllOverMllHisto" in name):
        binBoundaries = [0, 1, 2, 3, 4, 5, 6, 7, 8, 10]
      elif ("leadJetZMass" in name):
        binBoundaries = [0, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 450, 500, 550, 600, 700, 800, 900, 1000, 1500, 2000, 2500, 3000]
      elif ("subleadJetZMass" in name):
        binBoundaries = [0, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 450, 500, 550, 600, 700, 800, 900, 1000, 1500, 2000, 2500, 3000]

      binBoundariesArray = array.array('d', binBoundaries)
      for mcFile in self.mcFiles:
         eff = self.calcScaleFactor(name,mcFile.skim,mcFile.result)
	#print "nevents: ", eff
	#print "mcFile xsec: ", mcFile.cx
         Nevents = mcFile.cx*1000*float(self.lumi)*eff
         hist=(mcFile.result.Get("demo/"+name).Clone())
         #hist.Add(mcFile.result.Get("demo/"+nextname))
         if(hist.Integral()>0):
           #hist.Scale(Nevents/hist.Integral())
           print "50 < mll < 200 integral:", hist.Integral()
           hist.Scale(1.0/hist.Integral())
           print "50 < mll < 200 normalized integral:", 1.0/hist.Integral()

         hist.SetFillColor(mcFile.color)
         hist.SetLineWidth(0)
         if(rebin):
            #hist.Rebin(4)
           hist_Rebin = hist.Rebin(len(binBoundariesArray) - 1,"hnew",binBoundariesArray)

         for i in range(1,hist_Rebin.GetNbinsX()+1):
	   binWidth = binBoundaries[i] - binBoundaries[i-1]
           hist_Rebin.SetBinContent(i,hist_Rebin.GetBinContent(i))
           hist_Rebin.SetBinError(i,hist_Rebin.GetBinError(i))
         # hist_Rebin.SetBinContent(i,hist_Rebin.GetBinContent(i)/binWidth)
         # hist_Rebin.SetBinError(i,hist_Rebin.GetBinError(i)/binWidth)   

         print "50 < mll < 200 rebinned integral:", hist_Rebin.Integral()
         hist_Rebin.SetDirectory(0)
         #hist.SetDirectory(0)
         hists.append(hist_Rebin)
         #hists.append(hist)

         #if count < 2:
           #leg2.AddEntry(hist, mcFile.label,"f")
       	 #else:
           #leg.AddEntry(hist, mcFile.label,"f")
	 #count += 1

         if count < 2:
           leg2.AddEntry(hist_Rebin, mcFile.label,"f")
         else:
           leg.AddEntry(hist_Rebin, mcFile.label,"f")
         count += 1

      dataHist = (self.mcFile2.Get("demo/"+name).Clone())
      eff = self.calcScaleFactor(name,self.mcFile2,self.mcFile2)
      #print "nevents: ", eff
      #print "mcFile xsec:: ", mcFile.cx
      Nevents = self.mcFile2_xsec*1000*float(self.lumi)*eff
      if(dataHist.Integral()>0):
        # print "mll > 200 integral:", dataHist.Integral()
         dataHist.Scale(1.0/dataHist.Integral())
        # print "mll > 200 normalzied integral:", 1.0/dataHist.Integral()
        # dataHist.Scale(Nevents/dataHist.Integral())
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
      dataHist.SetLineWidth(1)
      dataHist.SetLineColor(1)
      dataHist.SetMarkerStyle(21)
      dataHist.SetMarkerSize(0.5)
      if(rebin):
         #dataHist.Rebin(4)
         dataHist_Rebin = dataHist.Rebin(len(binBoundariesArray)-1,"hnew",binBoundariesArray)
      
      for i in range(1,dataHist_Rebin.GetNbinsX()+1):
         binWidth = binBoundaries[i]-binBoundaries[i-1]
         dataHist_Rebin.SetBinContent(i,dataHist_Rebin.GetBinContent(i))
         dataHist_Rebin.SetBinError(i,dataHist_Rebin.GetBinError(i))
         #dataHist_Rebin.SetBinContent(i,dataHist_Rebin.GetBinContent(i)/binWidth)
         #dataHist_Rebin.SetBinError(i,dataHist_Rebin.GetBinError(i)/binWidth)
        #print(dataHist_Rebin.GetBinError(i))

      print "mll > 200 rebinned integral: ", dataHist_Rebin.Integral()
      dataHist_Rebin.SetStats(0)
      stackplot.Draw("hist")
     #print "dataHist.Integral(): ", dataHist_Rebin.Integral()
      dataHist_Rebin.Draw("pesame") 
      if(log):
         pad1.SetLogy()
         ROOT.gPad.SetLogy()
      if self.mcFile2 is not None:
         maximum=max(stackplot.GetMaximum(),dataHist_Rebin.GetMaximum())
         minimum=min(stackplot.GetMinimum(),dataHist_Rebin.GetMinimum(0))
      else:
         maximum=stackplot.GetMaximum()
         minimum=stackplot.GetMinimum()
      if(minimum==0 and self.mcFile2 is not None):
         minimum=dataHist_Rebin.GetMinimum(0)
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
      if self.mcFile2 is not None:
         dataHist_Rebin.Draw("psame") 
  
      newDataHist = copy.deepcopy(dataHist_Rebin)
      allMC = stackplot.GetStack().Last().Clone()

      herr = allMC.Clone('herr')
      theErrorGraph = ROOT.TGraphErrors(herr)
      theErrorGraph.SetFillColor(ROOT.kGray+2)
      theErrorGraph.SetFillStyle(3002)
      herr.SetFillColor(ROOT.kGray+2)
      herr.SetFillStyle(3002)
      herr.SetMarkerColor(1111)

      theErrorGraph.Draw('SAME2')

      leg.AddEntry(dataHist, "m_{ll} > 200 (ptl = 80 GeV)", "p") 
      leg.Draw()
      leg2.Draw()
      
      label = ROOT.TLatex(0.73,0.905,"%.2f fb^{-1} (13 TeV)"%float(self.lumi))
      label.SetNDC()
      label.SetTextFont(42)
      label.SetTextSize(0.033)
      label.Draw()
      
      stackplot.GetYaxis().SetTitle("A.U./X GeV")
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
   #   ratio = self.getRatio(allMC,newDataHist)
      herr3= self.TOTerror(allMC,ratio);
  #   herr3= self.TOTerror(newDataHist,ratio);
      toterree = ROOT.TGraphErrors(herr3)

      ratio.SetStats(0)
      ratio.GetYaxis().SetRangeUser(-.5,2.5)
#      ratio.GetYaxis().SetRangeUser(0.25,2.5)
      ratio.GetYaxis().SetNdivisions(504)
      ratio.GetYaxis().SetTitle("Ratio")
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
      line.SetLineColor(ROOT.kRed)
      line.SetLineStyle(2)
      line.Draw()
      tKsChi = ROOT.TLatex()
      tKsChi.SetNDC()
      tKsChi.SetTextFont(42)
      tKsChi.SetTextSize(0.09)

      ratio.Draw("PE")
     # ratioFit = ROOT.TF1("myFit","[0]",500,3000) 
     # ratio.Fit("myFit")
     # para0 = ratioFit.GetParameter(0) 
     # chi2 = ratioFit.GetChisquare()
     # dof = ratioFit.GetNDF()

     # ratioFit1 = ROOT.TF1("myFit1","[0]+[1]*x",500,3000)
     # ratioFit1.SetLineColor(3)
     # ratio.Fit("myFit1","0")
     # para0_1 = ratioFit1.GetParameter(0)
     # para0_2 = ratioFit1.GetParameter(1)
     # chi2_1 = ratioFit1.GetChisquare()
     # dof_1 = ratioFit1.GetNDF()

     # ratioFit.Draw("same")
     # ratioFit1.Draw("same")

     # fitInfo = ROOT.TLatex()
     # fitInfo.SetTextAlign(12)
     # fitInfo.SetTextSize(13)     
     # fitInfo.DrawLatexNDC(0.35,0.9,"Ratio = %0.2f"%(para0) )
     # fitInfo.DrawLatexNDC(0.35,0.8,"#chi^{2}/NDF = %0.2f"%(chi2/dof) )

     # fitInfo.DrawLatexNDC(0.55,0.9,"Ratio = %0.2f %0.2fm_{lljj} (MeV)"%(para0_1,para0_2*1000) )
     # fitInfo.DrawLatexNDC(0.55,0.8,"#chi^{2}/NDF = %0.2f"%(chi2_1/dof_1) )

      toterree.SetFillColor(ROOT.kGray+2)
      toterree.SetLineColor(ROOT.kGray+2)
      toterree.SetFillStyle(3002)
#      toterree.Draw("2 same")
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
#     plot.Print(outDir+"/"+name.split("/")[0]+name.split("/")[1]+"_stack.png")
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
           #hist.Scale(Nevents/hist.Integral())
            hist.Scale(1/hist.Integral())
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
#      dataHist=self.mcFile2.Get("demo/allEvents/cutProgress").Clone()
#      for n in range(1,len(cutList)+2):
#         if(dataHist.GetBinContent(n)>0):
#            outfile.write(cutList[n-2]+","+str(dataHist.GetBinContent(n))+"\n")
    
      outfile.close()
