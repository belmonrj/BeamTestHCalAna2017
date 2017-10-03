
void compare_full()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  // out_full_all_pionn.root
  // out_full_hcal_pionn.root
  // out_full_ohcal_pionn.root
  char* files[] = {"out_full_all_pionn.root", "out_full_hcal_pionn.root", "out_full_ohcal_pionn.root"};

  TH1F *res;
  TH1F *linearity;
  TF1 *res_fit;
  // --- legend for energy resolution
  TLegend * legend = new TLegend( 0.18, 0.6, 0.58, 0.9 );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetTextSize(0.05);
  legend->SetLineWidth(4);
  legend->SetFillStyle(0);
  // --- legend for linearity
  TLegend * legend2 = new TLegend( 0.18, 0.6, 0.48, 0.9 );
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);
  legend2->SetTextSize(0.05);
  legend2->SetFillStyle(0);

  TString name;
  char* tags[] = {"EMCAL+HCALIN+HCALOUT", "HCALIN+HCALOUT (EMCAL MIP)", "HCALOUT (EMCAL+HCALIN MIP)"};
  TCanvas *can = new TCanvas("can","");
  int colors[] = { 1,2,4};
  TF1 * f_calo_r = new TF1("f_calo_r", "sqrt(2*2+[0]*[0]+[1]*[1]/x)/100", 0.3, 35);
  f_calo_r->SetParLimits(0,0,30);
  f_calo_r->SetParLimits(1,0,200);
  bool is_prel = false;
  TText *prel_text;

  TCanvas *can3 = new TCanvas("can3","");
  TPad *pad1;
  float padxmin = 0;
  float padymin = 0.3;
  float padxmax = 1.0;
  float padymax = 1.0;
  char tmpname[512];
  sprintf(tmpname, "lin");
  pad1 = new TPad(tmpname, tmpname, padxmin, padymin, padxmax, padymax);
  pad1->SetBottomMargin(0);
  pad1->SetTopMargin(0.05);
  pad1->SetRightMargin(0.01);
  pad1->Draw();

  TPad *pad2;
  padxmin = 0;
  padymin = 0.;
  padxmax = 1.0;
  padymax = 0.3;
  pad2 = new TPad("ratio", "ratio", padxmin, padymin, padxmax, padymax);
  pad2->SetBottomMargin(0.3);
  pad2->SetTopMargin(0.0);
  pad2->SetRightMargin(0.01);
  pad2->Draw();



  for(int itype=0; itype<3; itype++)
    {
      TFile *fin = TFile::Open( files[itype] );
      res = (TH1F*) fin->Get("res");
      res->SetMarkerColor( colors[itype] );
      res->SetMarkerStyle(20+itype);
      res->SetMarkerSize(1.5);
      res->SetLineColor( colors[itype] );
      res->SetLineWidth(2);
      res->GetXaxis()->SetTitle("Input Energy (GeV)");
      res->GetYaxis()->SetTitle("Resolution (#sigma_{E}/#LTE#GT)");
      can->cd();
      (itype==0)? res->Draw():res->Draw("Same");
      //res_fit = (TF1*) fin->Get("f_calo_r");
      //delete res_fit;
      res->Fit(f_calo_r, "QRM", "", 0.3, 35);
      res_fit = (TF1*) res->GetFunction("f_calo_r");
      res_fit->SetRange(0.3,60);
      res_fit->SetLineColor(colors[itype]);
      res_fit->Draw("Same");
      legend->AddEntry( res, tags[itype], "P");
      legend2->AddEntry( res, tags[itype], "P");
      name  = Form("#DeltaE/E = 2%%(#deltap/p) #oplus %.1f%% #oplus %.1f%%/#sqrt{E}",
                   res_fit->GetParameter(0), res_fit->GetParameter(1));
      legend->AddEntry( res_fit->Clone(), name, "L");
      if(is_prel)
        {
          prel_text = new TText(0.5,0.3,"sPHENIX Preliminary" );
          prel_text->SetNDC();
          prel_text->Draw("Same");
        }

      pad1->cd();
      linearity = (TH1F*) fin->Get("linearity");
      linearity->SetMarkerColor( colors[itype] );
      linearity->SetMarkerStyle(20+itype);
      linearity->SetMarkerSize(1.5);
      linearity->GetXaxis()->SetLabelSize(0.06);
      linearity->GetYaxis()->SetLabelSize(0.06);
      linearity->GetXaxis()->SetTitle("Input Energy (GeV)");
      linearity->GetYaxis()->SetTitle("Measured Energy (GeV)");
      linearity->Scale(1./0.723);
      linearity->SetMaximum(65);
      linearity->SetAxisRange(0,65);
      (itype==0)? linearity->DrawClone("0"):linearity->DrawClone("Same,0");
      TF1 *fa = new TF1(Form("fa_%i",itype),"[0]*x",-0.1,65.);
      fa->SetLineColor( colors[itype] );
      fa->SetParameter(0,1.);
      fa->SetParLimits(0,0.6,1.8);
      linearity->Fit(fa, "QRM0", "goff", -0.1, 35.);
      fa->Draw("Same");
      name  = Form("E_{reco}=%.3f E_{truth}",fa->GetParameter(0));
      legend2->AddEntry(fa, name.Data(), "L");
      //if(is_prel) prel_text->Draw("Same");
      //if(itype==2) legend2->Draw("Same");

      //Ratio
      can3->cd();
      pad2->cd();
      linearity->Divide(fa);
      linearity->SetMaximum(1.3);
      linearity->SetMinimum(0.7);
      linearity->GetXaxis()->SetLabelSize(0.12);
      linearity->GetYaxis()->SetLabelSize(0.1);
      linearity->GetYaxis()->SetTitle("#frac{Measured Energy}{Input Energy}");
      linearity->GetXaxis()->SetTitleSize(0.13);
      linearity->GetYaxis()->SetTitleSize(0.1);
      linearity->GetYaxis()->SetTitleOffset(0.4);
      linearity->GetYaxis()->SetNdivisions(505, true);
      (itype==0)?linearity->DrawCopy():linearity->DrawCopy("Same");
      // --- draw straight line at unity for ratio part
      TLine *l = new TLine();
      l->DrawLine(0.0,1.0,linearity->GetNbinsX(),1.0);

    } // loop over types (pi+ and pi-)

  can->cd();
  legend->Draw();
  can->Print("figures/RB_full_resolution_comparison_mip.png");
  can->Print("figures/RB_full_resolution_comparison_mip.pdf");
  can3->cd();
  legend2->Draw();
  can3->Print("figures/RB_full_linearity_comparison_mip.png");
  can3->Print("figures/RB_full_linearity_comparison_mip.pdf");


}
