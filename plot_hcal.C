

void plot_hcal(bool log_scale=true)
{
  TString det = "HCALOUT";
  TString gain = "RAW";
  TLegend * legend = new TLegend( 0.7, 0.4, 0.99, 0.6, "", "NDC" );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetTextSize( .08 );
  //legend->SetLineWidth(4);

  char* files[] = {
    "HCALOUTRAW_Log_all_event.root",
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
  double max = 0;
  bool swap = false;

  for (int iphi = 4 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 4; ieta++)
        {

          TFile *fin = TFile::Open( files[0] );
          if(!fin) continue;
          p = (TPad *) c1->cd(idx);
          idx++;
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
          //h->Rebin(10);
          //h->SetAxisRange(60,2000);
          h->SetAxisRange(10,2000);
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
          h->SetLineColor(1);
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
          legend->Draw("Same");
          if(iphi==0 && ieta==2) text = new TText(0.65,0.8,Form("Tower %i", towerid));
          else if(iphi==1 && ieta==2) text = new TText(0.6,0.8,Form("Tower %i", towerid));
          else text = new TText(0.5,0.8,Form("Tower %i", towerid));
          text->SetNDC();
          text->SetTextSize(2.2*text->GetTextSize());
          if(iphi==0) text->SetTextSize(0.9*text->GetTextSize());
          text->Draw("Same");
        } // loop over eta
    } // loop over phi

  c1->Print("figure_hcalonly_adcdistributiontowers.png");

}
