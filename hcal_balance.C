

void hcal_balance()
{

  double en[] = {2, 4, 6, 8, 12, 16, 20, 30, 40, 50, 60};
  int nfiles = 11;
  char *files[] = {
    "outputdir_set2/Prototype_pionp_2.root",
    "outputdir_set2/Prototype_pionp_4.root",
    "outputdir_set2/Prototype_pionp_6.root",
    "outputdir_set2/Prototype_pionp_8.root",
    "outputdir_set2/Prototype_pionp_12.root",
    "outputdir_set2/Prototype_pionp_16.root",
    "outputdir_set2/Prototype_pionp_20.root",
    "outputdir_set2/Prototype_pionp_30.root",
    "outputdir_set2/Prototype_pionp_40.root",
    "outputdir_set2/Prototype_pionp_50.root",
    "outputdir_set2/Prototype_pionp_60.root"
  };


  TFile *fout = TFile::Open("out.root","RECREATE");
  TCanvas *can = new TCanvas("can","can",1000,600);
  can->Divide(5,2);
  TString name;
  TH1F *hsignal;
  TProfile *hprof;
  TH1F *res = new TH1F("res","res",280,0,70);
  res->SetMarkerStyle(20); res->SetMarkerSize(1.2);
  res->GetYaxis()->SetTitle("Resolution");
  res->GetXaxis()->SetTitle("Beam Energy");

  TH1F *linearity = new TH1F("linearity","linearity",280,0,70);
  linearity->SetMarkerStyle(20);
  linearity->SetMarkerSize(1.5);
  linearity->GetYaxis()->SetTitle("Reconstructed Energy");
  linearity->GetXaxis()->SetTitle("Beam Energy");
  linearity->SetMarkerColor(2);

  float emcal_weight[] = {0.5, 0.499, 0.674, 0.702, 0.711, 0.686, 0.692, 0.668};


  for(int ifile=0; ifile<nfiles; ifile++)
    {
      cout << "Reading " << files[ifile] << endl;
      TFile *fin = TFile::Open(files[ifile],"READ");
      if(!fin) cout << "file not found " << files[ifile] << endl;
      //name = Form("hsignal_%i", (int) en[ifile]);
      //hsignal = new TH1F(name.Data(),name.Data(),700,0,70);
      //hsignal = new TH1F(name.Data(),name.Data(),400,0,80);
      //hsignal->Sumw2();
      //hsignal->SetMarkerSize(1.2);
      //hsignal->GetXaxis()->SetTitle("Beam Energy");
      TTree *T = (TTree*) fin->Get("T");
      if(!T) continue;
      T->SetAlias("hcalin_e_new", "1.*hcalin_e" );
      T->SetAlias("hcal_e", "hcalin_e_new+hcalout_e");
      T->SetAlias("ahcal", "(hcalin_e_new-hcalout_e)/(hcalin_e_new+hcalout_e)");
      T->SetAlias("aemcal", "(cemc_e-hcal_e)/(cemc_e+hcal_e)");
      (can->cd(ifile+1))->SetLogz();
      string mycut = "abs(ahcal)<1.2 && good_h";
      T->Draw("hcal_e:ahcal",mycut.c_str(),"colz");
      T->SetAlias("Good_h","abs(C2_sum)<15");

      name = Form("hprofile%i", ifile);
      hprof = new TProfile(name.Data(),name.Data(), 100, -0.8, 0.8);
      T->Draw(Form("hcal_e:ahcal>>%s",name.Data()),mycut.c_str(),"Same");
      hprof->SetMarkerStyle(20);
      hprof->SetMarkerSize(0.7);
      hprof->SetMaximum(20);
      if(hprof->GetEntries()<10) continue;
      hprof->Fit("pol1","QR","goff",-0.7,0.7);
      TF1 *pol1 = (TF1*) hprof->GetFunction("pol1");
      if(!pol1) continue;
      pol1->SetLineColor(2);
      hprof->SetMaximum( 2*pol1->GetParameter(0) );
      hprof->SetMinimum( 0.5*pol1->GetParameter(1) );

      TText *txt = new TText(0.6,0.7,Form("%2.0f GeV",en[ifile]) );
      txt->SetNDC();
      txt->SetTextSize(2*txt->GetTextSize());
      txt->Draw("Same");
      can->Update();
    }

  can->Print("figures/RB_hcalonly_hcalbalance.png");
  can->Print("figures/RB_hcalonly_hcalbalance.pdf");

}
