

void cosmics_hcalin_compare(bool log_scale=false)
{

  TString det = "HCALIN";
  TString gain = "RAW";
  TLegend * legend = new TLegend( 0.7, 0.4, 0.99, 0.6, "", "NDC" );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetTextSize( .08 );
  //legend->SetLineWidth(4);

  //April 2016 calibration runs
  /*char *old_files[] = {"/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/cosmic_data/jobs/Prototype_cosmics_1/HCALINRAW_Log_all_event.root",
    "/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/cosmic_data/jobs/Prototype_cosmics_2/HCALINRAW_Log_all_event.root",
    "/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/cosmic_data/jobs/Prototype_cosmics_3/HCALINRAW_Log_all_event.root",
    "/gpfs/mnt/gpfs02/sphenix/sim/sim01/sPHENIX/sunywrk/abhisek/cosmic_data/jobs/Prototype_cosmics_4/HCALINRAW_Log_all_event.root"
    };
  */

  //Self trigger --no tracking
  /*char *old_files[] = {
    "production/outputdir/HCALINRAW_cosmics_00003037-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003037-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003037-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003037-0000.root"
    };
  */

  //Self trigger -- with tracking
  char *old_files[] = {
    "production/HCALINRAW_all_event_0C.root",
    "production/HCALINRAW_all_event_1C.root",
    "production/HCALINRAW_all_event_2C.root",
    "production/HCALINRAW_all_event_3C.root"
  };


  char *files[] = {
    "production/outputdir/HCALINRAW_cosmics_00003087-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003086-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003071-0000.root",
    "production/outputdir/HCALINRAW_cosmics_00003069-0000.root"
  };


  char *file_tag[] = { "Proto2","Proto3"};
  //char *file_tag[] = { "Self Trigger","External Trigger"};
  TCanvas *c1 = new TCanvas(
                            det + gain + TString(log_scale ? "_Log" : ""),
                            det + gain + TString(log_scale ? "_Log" : ""),
                            750, 700);
  c1->Divide(4, 4, 0.0, 0.0);

  int idx = 1;
  TPad * p;
  double padxmin,padxmax,padymin,padymax;
  TText *text;
  double max = 0;
  bool swap = false;

  //double sim[] = {7.08378 7.57606 8.06622 8.54688}; //reverse phi
  double sim[] = {8.54688, 8.06622, 7.57606, 7.08378}; //phi

  for (int iphi = 4 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 4; ieta++)
        {
          for(int ifile=0; ifile<2; ifile++)
            {

              if(ifile==0) TFile *fin = TFile::Open( old_files[ieta] );
              else TFile *fin = TFile::Open( files[ieta] );
              if(!fin) continue;
              p = (TPad *) c1->cd(idx);
              if(ifile==1) idx++;
              if(ieta==0) p->SetLeftMargin(0.17);
              c1->Update();

              if (log_scale)
                {
                  p->SetLogy();
                }
              //p->SetGridx(0);
              //p->SetGridy(0);
              int towerid = iphi + 4*ieta;

              //if(ifile==1 && ieta==3 ) { ieta=0; swap=true; }
              TH1F * h = (TH1F*) fin->Get(Form("hEnergy_ieta%d_iphi%d", ieta, iphi)+ det);
              //if(ifile==1 && ieta==0 && swap==true) { ieta=3; swap=false; }
              h->SetFillColor(0);
              h->Rebin(10);
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
              h->SetLineColor(ifile+1);
              //h->GetXaxis()->SetLabelSize(0.1*h->GetXaxis()->GetLabelSize());
              //h->Fit("landau","R","",50,2000);
              //Ugly hack
              if(iphi==0 && ieta==0)
                {
                  h->GetYaxis()->SetTitleSize(0.07);
                  h->GetYaxis()->SetTitleOffset(1.18);
                  h->GetYaxis()->SetLabelSize(0.06);
                }
              if(iphi==3 && ieta==0) legend->AddEntry(h, file_tag[ifile], "L");
              if(ifile==0) max = h->GetMaximum();
              else h->Scale( max/h->GetMaximum() );
              (ifile==0)? h->Draw():h->Draw("Same");
              if(ifile==1) legend->Draw("Same");
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
              f3->SetLineColor(ifile+1);
              h->Fit(f3,"QR0");
              f3->SetLineWidth(2);
              f3->Draw("Same");
              //cout << "ieta " << ieta << " iphi " << iphi << " calib " << sim[iphi]/f3->GetParameter(1) << endl;
              cout << ieta << "    "  << iphi << "    " << sim[iphi]/f3->GetParameter(1) << endl;

              //cout << ieta << "    "  << iphi << "    " << f3->GetParameter(1) << endl;
              //cout << towerid << endl;
              //cout << f3->GetParameter(1) << endl;
            } // loop over files
        } // loop over eta
    } // loop over phi

  c1->Print("figures/RB_cosmics_hcalin_compare.png");
  c1->Print("figures/RB_cosmics_hcalin_compare.pdf");

}
