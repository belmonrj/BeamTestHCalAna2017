void balance_emcal_weight()
{
  double en[] = {2, 4, 6, 8, 12, 16, 24, 28};
 char *files[] = { "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_2.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_4.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_6.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_8.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_12.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_16.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_24.root",
                   "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_28.root"
                  };

 TString name;
 TProfile *hprof;
 TCanvas *can = new TCanvas("can","can",600,600);
 can->Divide(3,3);
 float weight[] = {0., 0., 0., 0., 0., 0., 0., 0.};
 float weight_min[] = {1.0, 1.537, 1.195, 1.284, 1.306, 1.271, 1.268, 1.224};
 float weight_max[] = {1.0, 1.539, 1.197, 1.285, 1.307, 1.272, 1.269, 1.225};
 for(int ifile=0; ifile<8; ifile++)
 {
 TFile *fin = TFile::Open(files[ifile],"READ");
 TTree *T = (TTree*) fin->Get("T");
 T->SetAlias("Cut", "good_h && sum_e>4" );
 //T->SetAlias("Cut", Form("emcal_e<%f", 0.05*en[ifile]) );
 T->Draw(">>EventList", "Cut");
 TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected" << endl;
  T->SetEventList( elist );
  name = Form("hprofile%i", ifile);
  hprof = new TProfile(name.Data(),name.Data(), 100, -1, 1);
  can->cd(ifile+1);
  for(weight[ifile]=weight_min[ifile]; weight[ifile]<weight_max[ifile]; weight[ifile]+=0.001)
  {
  cout << "E " << en[ifile] << " weight " << weight[ifile] << endl;
  T->SetAlias("hcal_e", "2.0*hcalin_e+hcalout_e");
  //24 and 28 were half bias
  if(ifile==6 || ifile==7) T->SetAlias("cemc_e",Form("2*%f*emcal_e",weight[ifile]) );
  else T->SetAlias("cemc_e",Form("%f*emcal_e",weight[ifile])); 
  T->SetAlias("tot_e", "hcal_e+cemc_e");
  T->SetAlias("aemcal", "(cemc_e-hcal_e)/(cemc_e+hcal_e)");

  //T->Draw("tot_e");
  T->Draw("tot_e:aemcal","abs(aemcal)<1","colz");
  T->Draw(Form("tot_e:aemcal>>%s",name.Data()),"", "Same"); //"prof,goff");
  hprof->SetMarkerStyle(20);
  hprof->SetMarkerSize(0.7);
  /*hprof->SetMaximum(20);
  if(hprof->GetEntries()<10) continue;
  hprof->Fit("pol1","","goff",-0.8,0.8);
  TF1 *pol1 = (TF1*) hprof->GetFunction("pol1");
  if(!pol1) continue;
  hprof->SetMaximum( 2*pol1->GetParameter(0) );
  hprof->SetMinimum( 0.5*pol1->GetParameter(1) );
  //hprof->Draw("Same");
  //pol1->Draw("Same");*/
  can->Update();
 }
 }

}
