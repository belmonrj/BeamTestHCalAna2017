
void compare_signal_sim()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  //char* files[] = {"out_hcal_pionn.root", "proto3_sim/Full/out_pi-.root"};
  char* files[] = {"out_hcal_pionp.root", "proto3_sim/Full/out_pi+.root"};

  TH1F *signal_data;
  TH1F *signal_sim;
  TLegend * legend = new TLegend( 0.65, 0.6, 0.88, 0.75, "", "NDC" );
  legend->SetFillColor(0);
  legend->SetBorderSize(0);
  legend->SetTextSize( .09 );
  legend->SetLineWidth(5);

  TLegend * legend2 = new TLegend( 0.2, 0.6, 0.5, 0.8, "", "NDC" );
  legend2->SetFillColor(0);
  legend2->SetBorderSize(0);
  legend2->SetTextSize( .03 );

  TString name;
  // data is 32 GeV.. simulation is 28 GeV...
  //double en[] = {2, 4, 6, 8, 12, 16, 24}; //, 28}; // pionn
  double en[] = {2, 4, 6, 8, 12, 16, 20}; //, 28}; // pionp
  //double en[] = {2, 4, 6, 8, 12, 16, 24, 32};

  char* tags[] = {"DATA", "SIM"};

  TCanvas *can = new TCanvas("can","can",800,500);
  can->Divide(4,2,0.001,0.001);
  int colors[] = {1, 30};
  double new_bins[1000];
  double data_max;
  TH1F *data_copy;

  for(int ien=0; ien<8; ien++)
    {
      name = Form("hsignal_%i", (int)en[ien] );
      TVirtualPad *p = can->cd(ien+1);
      p->SetTopMargin(0.01);
      p->SetLeftMargin(0.17);
      p->SetRightMargin(0.01);
      data_max = 0;

      for(int itype=0; itype<2; itype++)
        {
          TFile *fin = TFile::Open( files[itype], "READ" );
          if ( !fin ) {cout<<"null file"<<endl; continue;}
          signal_data = (TH1F*) fin->Get(name.Data());
          if ( !signal_data ) {cout<<"null histo "<<name.Data()<<" from file "<<files[itype]<<endl; continue;}
          signal_data->SetMarkerStyle(20);
          signal_data->SetMarkerSize(0.8);
          signal_data->SetLineColor(colors[itype]);
          signal_data->SetMarkerColor(colors[itype]);
          signal_data->GetYaxis()->SetTitle("Counts/bin");
          signal_data->GetXaxis()->SetTitle("Reco Energy");
          signal_data->GetXaxis()->SetLabelSize(0.05);
          signal_data->GetYaxis()->SetLabelSize(0.05);
          signal_data->GetXaxis()->CenterTitle(true);
          signal_data->GetYaxis()->CenterTitle(true);
          signal_data->GetYaxis()->SetTitleOffset(1.1);
          signal_data->GetYaxis()->SetTitleSize(0.07);
          signal_data->GetXaxis()->SetTitleSize(0.05);
          signal_data->SetStats(0);
          signal_data->Rebin(4);
          signal_data->SetAxisRange( 0, signal_data->GetMean()+4*signal_data->GetRMS());
          double min_range = signal_data->GetMean()-1.6*signal_data->GetRMS();
          double max_range = signal_data->GetMean()+1.6*signal_data->GetRMS();
          signal_data->Fit("gaus","R","same",min_range,max_range);
          TF1 *fit = (TF1*) signal_data->GetFunction("gaus");
          if(!fit) continue;
          double scale = fit->GetParameter(1)/en[ien];
          cout << scale << endl;
          int nbins = signal_data->GetXaxis()->GetNbins();
          for(int i=0; i <= nbins; i++) new_bins[i] = signal_data->GetBinLowEdge(i+1)/scale;
          signal_data->SetBins(nbins, new_bins);
          signal_data->SetAxisRange(0,signal_data->GetMean()+6*signal_data->GetRMS());
          //delete fit;
          double min_range = signal_data->GetMean()-1.6*signal_data->GetRMS();
          double max_range = signal_data->GetMean()+1.6*signal_data->GetRMS();
          signal_data->Fit("gaus","R","same",min_range,max_range);
          TF1 *fit = (TF1*) signal_data->GetFunction("gaus");
          fit->SetLineColor( colors[itype] );
          delete fit;
          if(itype==0) data_max = signal_data->GetMaximum();
          else signal_data->Scale( data_max/signal_data->GetMaximum() );

          if(itype==1)
            {
              signal_data->SetMarkerStyle(0);
              signal_data->SetFillColor(30);
              signal_data->SetFillStyle(1001);
              signal_data->SetFillColorAlpha(30,0.35);
              signal_data->SetLineColor(30);
            }
          if(itype==0)
            {
              data_copy = signal_data;
              signal_data->Draw();
            }
          else
            {
              signal_data->Draw("h,Same");
              data_copy->Draw("Sames");
            }
          TText *txt = new TText(0.7,0.8,Form("%2.0f GeV",en[ien]) );
          txt->SetNDC();
          txt->SetTextSize(1.8*txt->GetTextSize());
          txt->Draw("Same");

          if(ien==0)
            {
              if(itype==0) legend->AddEntry(signal_data, tags[itype], "P");
              else legend->AddEntry(signal_data, tags[itype], "F");
            }
        } // loop over types (sim and real)

      legend->DrawClone();
    } // loop over energies

  //p = can->cd(9);
  //p->SetLeftMargin(0);
  //p->SetRightMargin(0);

  // can->Print("figures/RB_hcalonly_sim_pionn_comparison.png");
  // can->Print("figures/RB_hcalonly_sim_pionn_comparison.pdf");
  can->Print("figures/RB_hcalonly_sim_pionp_comparison.png");
  can->Print("figures/RB_hcalonly_sim_pionp_comparison.pdf");

}
