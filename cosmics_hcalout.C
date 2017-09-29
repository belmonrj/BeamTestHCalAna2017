#include<TF1.h>
#include "/direct/phenix+u/abhisek/tools/SetOKStyle.C"

void cosmics_hcalout(bool log_scale=false)
{
  SetOKStyle();
 OKStyle->SetOptStat(0);
 OKStyle->SetPadGridX(0);
 OKStyle->SetPadGridY(0);
 TString det = "HCALOUT";
 TString gain = "RAW";

  /*char *files[] = {
                  "../production/outputdir/HCALOUTRAW_cosmics_00003213-0000.root",
                  "../production/outputdir/HCALOUTRAW_cosmics_00003213-0000.root",
                  "../production/outputdir/HCALOUTRAW_cosmics_00003201-0000.root",
                  "../production/outputdir/HCALOUTRAW_cosmics_00003198-0000.root"
                 };
   */
  char *files[] = {
                  "../production/HCALOUT_3334_col0.root",
                  "../production/HCALOUT_3334_col1.root",
                  "../production/HCALOUT_3334_col2.root",
                  "../production/HCALOUT_3334_col3.root"
                  };
  
   TCanvas *c1 = new TCanvas(
      det + gain + TString(log_scale ? "_Log" : ""),
      det + gain + TString(log_scale ? "_Log" : ""),
      600, 600);
  c1->Divide(4, 4, 0., 0.01);
  int idx = 1;
  TPad * p;
  TText *text;

  //double sim[] = { 7.12943 6.97834 6.79614 6.69745 } //Reverse phi
  double sim[] = { 6.69745, 6.79614, 6.97834, 7.12943 };

  //double sim_width[] = {0.591123 0.553095 0.531708 0.45269}; //reverse order
  double sim_width[] = { 0.549016, 0.50869, 0.553028, 0.556054}; 

   for (int iphi = 4 - 1; iphi >= 0; iphi--)
    {
      for (int ieta = 0; ieta < 4; ieta++)
        {
         TFile *fin = TFile::Open( files[ieta] );
         if(!fin) continue;
          p = (TPad *) c1->cd(idx++);
          c1->Update();

          if (log_scale)
            {
              p->SetLogy();
            }
         int towerid = iphi + 4*ieta;
          //p->SetGridx(0);
          //p->SetGridy(0);
          TH1F * h = (TH1F*) fin->Get(Form("hEnergy_ieta%d_iphi%d", ieta, iphi)+ det);
          h->SetFillColor(0);
          h->SetFillColor(0);
          h->Rebin(5);
          h->SetAxisRange(50,1000);
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
          h->GetYaxis()->CenterTitle(true);
          if(iphi==0 && ieta==0)
          {
           h->GetYaxis()->SetTitleSize(0.07);
           h->GetYaxis()->SetTitleOffset(1.18);
           h->GetYaxis()->SetLabelSize(0.06);
          }
          h->Draw();
          text = new TText(0.5,0.8,Form("Tower %i", towerid));
          text->SetNDC();
          text->SetTextSize(2.2*text->GetTextSize());
          if(iphi==0) text->SetTextSize(0.9*text->GetTextSize());
          text->Draw("Same");

          double min = 100; //h->GetMean()+0.3*h->GetRMS();
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
          f3->Draw("Same");

          //cout << "ieta " << ieta << " iphi " << iphi << " calib " << sim[iphi]/f3->GetParameter(1) << endl;
          //cout << ieta << "    "  << iphi << "    " << sim[iphi]/f3->GetParameter(1) << endl;

          //cout << f3->GetParameter(1)/sim[iphi] << ", " ;
          //f3->Print();
          cout << f3->GetParameter(2) << ", " ;
         }
    }
 

}
