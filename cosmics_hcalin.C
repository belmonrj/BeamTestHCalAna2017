

void cosmics_hcalin(bool log_scale=false)
{

  TString det = "HCALIN";
  TString gain = "RAW";

  char *files[] = {
    "production/outputdir/HCALINRAW_cosmics_00003087-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003086-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003071-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003069-0000.root"
  };

  TCanvas *c1 = new TCanvas(
                            det + gain + TString(log_scale ? "_Log" : ""),
                            det + gain + TString(log_scale ? "_Log" : ""),
                            750, 700);
  c1->Divide(4, 4, 0.0, 0.0);
  int idx = 1;
  TPad * p;
  double padxmin,padxmax,padymin,padymax;
  TText *text;

  //double sim[] = {7.11907 7.63993 8.18212 8.62478}; //reverse phi
  double sim[] = {8.62478, 8.18212, 7.63993, 7.11907}; //phi

  //double sim_width[] = {0.497448 0.565498 0.606628 0.63112}; //reverse order
  double sim_width[] = {0.63112, 0.606628, 0.565498, 0.497448 };

  for (int iphi = 4 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 4; ieta++)
        {
          TFile *fin = TFile::Open( files[ieta] );
          if(!fin) continue;
          p = (TPad *) c1->cd(idx++);
          if(ieta==0) p->SetLeftMargin(0.17);
          c1->Update();

          if (log_scale)
            {
              p->SetLogy();
            }
          //p->SetGridx(0);
          //p->SetGridy(0);
          int towerid = iphi + 4*ieta;
          TH1F * h = (TH1F*) fin->Get(Form("hEnergy_ieta%d_iphi%d", ieta, iphi)+ det);
          h->SetFillColor(0);
          h->Rebin(5);
          h->SetAxisRange(60,2000);
          h->SetLineWidth(2);
          h->GetXaxis()->SetLabelSize(.07);
          h->GetXaxis()->CenterTitle(true);
          h->GetXaxis()->SetTitle("Tower ADC");
          h->GetXaxis()->SetTitleOffset(0.7);
          h->GetYaxis()->SetLabelSize(0.07);
          h->GetYaxis()->SetTitleSize(0.08);
          h->GetYaxis()->SetTitleOffset(1.1);
          h->GetXaxis()->SetNdivisions(505, true);
          h->GetYaxis()->SetTitle("Counts/bin");
          //h->GetYaxis()->SetNdivisions(505, true);
          h->GetYaxis()->CenterTitle(true);
          //h->GetXaxis()->SetLabelSize(0.1*h->GetXaxis()->GetLabelSize());
          //h->Fit("landau","R","",50,2000);
          //Ugly hack
          if(iphi==0 && ieta==0)
            {
              h->GetYaxis()->SetTitleSize(0.07);
              h->GetYaxis()->SetTitleOffset(1.18);
              h->GetYaxis()->SetLabelSize(0.06);
            }
          h->Draw();
          if(iphi==0 && ieta==2) text = new TText(0.65,0.8,Form("Tower %i", towerid));
          else if(iphi==1 && ieta==2) text = new TText(0.6,0.8,Form("Tower %i", towerid));
          else text = new TText(0.5,0.8,Form("Tower %i", towerid));
          text->SetNDC();
          text->SetTextSize(2.2*text->GetTextSize());
          if(iphi==0) text->SetTextSize(0.9*text->GetTextSize());
          text->Draw("Same");
          double min = 50;
          TF1 *f1 = new TF1(Form("fit1_ieta%d_iphi%d", ieta, iphi)+ det, "landau(0)",min,2000);
          h->Fit(f1,"QR0");
          //f1->Draw("Same");

          TF1 *f2 = new TF1(Form("fit2_ieta%d_iphi%d", ieta, iphi)+ det, "expo(0)",min,2000);
          h->Fit(f2,"QR0");
          //f2->Draw("Same");

          TF1 *f3 = new TF1(Form("fit3_ieta%d_iphi%d", ieta, iphi)+ det,"expo(3)+landau(0)",min,2000);
          f3->SetParameters( f1->GetParameter(0), f1->GetParameter(1), f1->GetParameter(2), f2->GetParameter(0), f2->GetParameter(1) );
          f3->SetLineColor(2);
          h->Fit(f3,"QR0");
          f3->SetLineWidth(2);
          f3->Draw("Same");
          //cout << "ieta " << ieta << " iphi " << iphi << " calib " << f3->GetParameter(1) << endl;
          //cout << ieta << "    "  << iphi << "    " << sim[iphi]/f3->GetParameter(1) << endl;

          //cout << ieta << "    "  << iphi << "    " << f3->GetParameter(1)/sim[iphi] << endl;
          //cout << towerid << endl;
          //cout << f3->GetParameter(1)/sim[iphi] << ", ";
          //f3->Print();
          cout << f3->GetParameter(2)/sim_width[iphi] << endl;
        } // loop over eta
    } // loop over phi

  c1->Print("figure_cosmics_hcalin.png");

}
