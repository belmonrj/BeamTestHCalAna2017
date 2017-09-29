#include "/phenix/u/abhisek/tools/SetOKStyle.C"

void Resolution_generic()
{
  // --- probably want to remove and rely on my defaults in rootlogon
  SetOKStyle();
  OKStyle->SetPadGridX(0);
  OKStyle->SetPadGridY(0);
  OKStyle->SetOptTitle(0);
  OKStyle->SetOptStat(0);
  OKStyle->SetPadRightMargin(0.01);
  // --- energy values
  double en[] = {2, 4, 6, 8, 12, 16, 24, 28};
  // --- files to read in corresponding to above energies in order
  // --- note that these are all negative pions; probably want to look at positive pions as well
  char *files[] = { "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_2.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_4.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_6.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_8.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_12.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_16.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_24.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_28.root"
  };

  // --- output file for writing histograms obtained from above
  TFile *fout = TFile::Open("out.root","RECREATE");
  // --- number of files/energies, used in loop below
  int nfiles = 8;
  // --- canvas for drawing 1d distributions
  TCanvas *can = new TCanvas("can","can",600,600);
  can->Divide(3,3);
  // --- histogram name for 1d distirbution
  TString name;
  // --- string for making analysis cuts
  TString cutname;
  // --- pointer for 1d histograms created in the loop (should maybe initialize to NULL?)
  TH1F *hsignal;
  // --- boolean flags for selecting shower origin
  bool hcalout_only = false; // shower begins in outer hcal
  bool hcal_only = false; // shower begins in inner or outer hcal
  bool full = true; // shower begins anywhere in calorimeter
  // --- histogram ranges
  double min_range = 0.;
  double max_range = 40.;
  // --- histogram to store resolutions
  // --- drawn in this macro (but not printed to file) and also written to file
  TH1F *res = new TH1F("res","res",100,0,40);
  res->SetMarkerStyle(20); res->SetMarkerSize(1.2);
  res->GetYaxis()->SetTitle("Resolution");
  res->GetXaxis()->SetTitle("Beam Energy");
  // --- histogram to store linearity
  // --- drawn in this macro (but not printed to file) and also written to file
  TH1F *linearity = new TH1F("linearity","linearity",100,0,35);
  linearity->SetMarkerStyle(20);
  linearity->SetMarkerSize(1.5);
  linearity->GetYaxis()->SetTitle("Reconstructed Energy");
  linearity->GetXaxis()->SetTitle("Beam Energy");
  linearity->SetMarkerColor(2);
  // --- need to understand what the emcal weights are for... some kind of calibration i guess?
  //float emcal_weight[] = {0.5, 0.499, 0.674, 0.702, 0.711, 0.686, 0.692, 0.668};
  //float emcal_weight[] = {1.0, 1.0, 1.16, 1.4, 1.5, 1.5, 1.54, 1.46};
  //float emcal_weight[] = {1.0, 1.0, 1.16, 1.4, 1.5, 1.5, 1.38, 1.32};
  float emcal_weight[] = {1.0, 1.537, 1.195, 1.284, 1.306, 1.271, 1.268, 1.224};

  // --- loop over files/energies
  for(int ifile=0; ifile<nfiles; ifile++)
    {
      // --- open file for specific energy to get data
      TFile *fin = TFile::Open(files[ifile],"READ");
      // --- set the histogram name
      name = Form("hsignal_%i", (int) en[ifile]);
      // --- create the 1d distribution histo for this energy
      //hsignal = new TH1F(name.Data(),name.Data(),100,0,50);
      hsignal = new TH1F(name.Data(),name.Data(),500,0,50);
      hsignal->Sumw2();
      hsignal->SetMarkerSize(1.2);
      hsignal->GetXaxis()->SetTitle("Beam Energy");
      // --- get the tree from the file
      TTree *T = (TTree*) fin->Get("T");
      // --- set some aliases for the analysis
      // --- note the factor of two because 24 and 28 were half bias
      if ( ifile==6 || ifile==7 ) T->SetAlias("cemc_e",Form("2*%f*emcal_e",emcal_weight[ifile]) );
      else T->SetAlias("cemc_e",Form("%f*emcal_e",emcal_weight[ifile]));
      // --- the rest of the aliases
      T->SetAlias("hcalin_e_new", "1.7*hcalin_e" ); // why does the inner hcal need to be scaled by 1.7?
      T->SetAlias("hcal_e", "hcalin_e_new+hcalout_e"); // combined inner+outer hcal
      T->SetAlias("tot_e", "hcal_e+cemc_e"); // combined energy in the calorimeter system
      T->SetAlias("ahcal", "(hcalin_e_new-hcalout_e)/(hcalin_e_new+hcalout_e)"); // inner-outer hcal energy asymmetry
      T->SetAlias("aemcal", "(cemc_e-hcal_e)/(cemc_e+hcal_e)"); // emcal-hcal energy assymmetry
      // --- now set the analysis cuts (need to add this info to the analysis note)
      if ( hcalout_only ) // showers in outer hcal
        cutname = Form("cemc_e<%f && hcalin_e_new<%f  && good_h", 0.05*en[ifile], 0.05*en[ifile]);
      else if ( hcal_only ) // showers in inner or outer hcal
        {
          if ( en[ifile]<6.1 ) cutname = Form("cemc_e<%f && good_h", 0.12*en[ifile]);
          else cutname = Form("cemc_e<%f && good_h", 0.05*en[ifile]);
        }
      else if ( full ) // showers anywhere in calo system
        {
          //if(en[ifile]>2 && en[ifile]<6.1) cutname = "good_h && aemcal>-1 && aemcal<0.5";
          //else
          cutname = "good_h && aemcal>-1 && aemcal<1.";
          if ( ifile==6 || ifile==7 ) cutname = "valid_hodo_v && valid_hodo_h && trigger_veto_pass && C2_sum<400 && aemcal<0.8";
        }
      // --- draw from the tree and fill it into the histogram
      //T->Draw(Form("%f*cemc_e+hcalin_e_new+hcalout_e>>%s",2*emcal_weight[ifile],name.Data()),cutname, "goff");
      T->Draw(Form("tot_e>>%s",name.Data()), cutname, "goff");
      can->cd(ifile+1);
      hsignal->SetAxisRange(0,hsignal->GetMean()+4*hsignal->GetRMS());
      hsignal->Draw();
      TText *txt = new TText(0.6,0.7,Form("%2.0f GeV",en[ifile]) );
      txt->SetNDC();
      txt->SetTextSize(2*txt->GetTextSize());
      txt->Draw("Same");
      //double min_range = hsignal->GetMean()-0.8*hsignal->GetRMS();
      //double max_range = hsignal->GetMean()+1.2*hsignal->GetRMS();
      //if(min_range<4) min_range = 3; //2.5;
      // --- adjust the ranges based on where the shower originates
      // --- not so important since the figures aren't currently saved anyway...
      if ( hcalout_only )
        {
          min_range = hsignal->GetMean()-1.3*hsignal->GetRMS();
          max_range = hsignal->GetMean()+1.5*hsignal->GetRMS();
        }
      else if ( hcal_only )
        {
          min_range = hsignal->GetMean()-1.6*hsignal->GetRMS();
          if ( min_range<2 ) min_range = 2.2;
          max_range = hsignal->GetMean()+1.7*hsignal->GetRMS();
        }
      else if ( full )
        {
          min_range = hsignal->GetMean()-1.4*hsignal->GetRMS();
          max_range = hsignal->GetMean()+1.4*hsignal->GetRMS();
        }
      // --- fit the 1d with a gaussian to get the parameters
      // --- physics question: why not just get the variance straight from the histo?
      hsignal->Fit("gaus","","",min_range,max_range);
      TF1 *gaus = (TF1*) hsignal->GetFunction("gaus");
      can->Update();
      fout->cd();
      hsignal->Write();
      //cout << gaus->GetParameter(2)/gaus->GetParameter(1) << endl;
      // --- leave the loop for certain energies
      // --- i guess there's some problem with the data?
      if ( hcalout_only && en[ifile]<7 ) continue;
      else if ( hcal_only && en[ifile]<6 ) continue;
      else if ( full && en[ifile]<4 ) continue;
      // --- set the resolutions
      res->SetBinContent(res->FindBin(en[ifile]), gaus->GetParameter(2)/gaus->GetParameter(1));
      res->SetBinError( res->FindBin(en[ifile]), gaus->GetParError(2)/gaus->GetParameter(1));
      // --- set the linearity
      linearity->SetBinContent(linearity->FindBin(en[ifile]), gaus->GetParameter(1));
      linearity->SetBinError(linearity->FindBin(en[ifile]), gaus->GetParError(1));
    }
  can->Print("figure_multipanel_energyreso.png");
  // --- setup for more plots, not printed to file
  TLegend * legend = new TLegend( 0.2, 0.7, 0.8, 0.88, "", "NDC" );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetTextSize( .05 );
  // --- resolution plot starts here
  TCanvas *can2 = new TCanvas("can2","can2",800,600);
  res->SetMaximum(1.2);
  res->Draw("P");
  TF1 * f_calo_r = new TF1("f_calo_r", "sqrt(2*2+[0]*[0]+[1]*[1]/x)/100", 0.3, 35);
  f_calo_r->SetParLimits(0,0,30);
  f_calo_r->SetParLimits(1,0,200);
  res->Fit(f_calo_r, "RM", "", 0.3, 35);
  //legend->AddEntry(res,"T-1044, HCAL cosmic calib.","P");
  //f_calo_r->SetLineColor(1);
  //f_calo_r->DrawCopy("same");
  name  = Form("#DeltaE/E = 2%% #oplus %.1f%% #oplus %.1f%%/#sqrt{E}",
               f_calo_r->GetParameter(0), f_calo_r->GetParameter(1));
  legend->AddEntry( f_calo_r,  name.Data(), "L" );
  legend->Draw();
  can2->Update();
  can2->Print("figure_energyreso.png");
  // --- linearity plot starts here
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  linearity->Draw();
  linearity->Fit("pol1");
  c2->Print("figure_energylinearity.png");
  // --- now write all the histograms to file
  fout->cd();
  res->Write();
  f_calo_r->Write();
  linearity->Write();
}
