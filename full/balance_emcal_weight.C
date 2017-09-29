void balance_emcal_weight()
{
  // --- list of energies
  double en[] = {2, 4, 6, 8, 12, 16, 24, 28};
  // --- list of files (note all are negative pions)
  char *files[] = { "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_2.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_4.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_6.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_8.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_12.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_16.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_24.root",
                    "/gpfs/mnt/gpfs04/sphenix/user/abhisek/shower/outputdir_full/Prototype_pionn_28.root"
  };

  // --- name for histogram
  TString name;
  // --- pointer for profile histogram for emcal energy asymmetry vs total energy
  TProfile *hprof;
  // --- canvas for drawing individual profiles...
  TCanvas *can = new TCanvas("can","can",600,600);
  can->Divide(3,3);
  // --- arrays for weights, min and max, etc
  float weight[] = {0., 0., 0., 0., 0., 0., 0., 0.};
  float weight_min[] = {1.0, 1.537, 1.195, 1.284, 1.306, 1.271, 1.268, 1.224};
  float weight_max[] = {1.0, 1.539, 1.197, 1.285, 1.307, 1.272, 1.269, 1.225};
  // --- loop over files/energies
  for(int ifile=0; ifile<8; ifile++)
    {
      // --- read in the data files
      TFile *fin = TFile::Open(files[ifile],"READ");
      // --- get the tree
      TTree *T = (TTree*) fin->Get("T");
      // --- set and alias for the analysis cut
      T->SetAlias("Cut", "good_h && sum_e>4" ); // good_h not defined?
      //T->SetAlias("Cut", Form("emcal_e<%f", 0.05*en[ifile]) );
      // --- draw the tree into an event list
      T->Draw(">>EventList", "Cut");
      // --- get a pointer to the newly created event list
      TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
      // --- show the number of events in the list relative to the total number of entries in the tree
      cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected" << endl;
      // --- set the tree event list to the new event list
      T->SetEventList( elist );
      // --- set the histogram name
      name = Form("hprofile%i", ifile);
      // --- create a new histogram with the above name
      hprof = new TProfile(name.Data(),name.Data(), 100, -1, 1);
      // --- change to the TPad unique to this file/energy
      can->cd(ifile+1);
      // --- loop over a set of weights for this file
      for(weight[ifile]=weight_min[ifile]; weight[ifile]<weight_max[ifile]; weight[ifile]+=0.001)
        {
          // --- print to screen the energy and weight for this file
          cout << "E " << en[ifile] << " weight " << weight[ifile] << endl;
          // --- set an alias for the hcal energy
          T->SetAlias("hcal_e", "2.0*hcalin_e+hcalout_e");
          // --- set an alias for the emcal energy
          //24 and 28 were half bias
          if(ifile==6 || ifile==7) T->SetAlias("cemc_e",Form("2*%f*emcal_e",weight[ifile]) );
          else T->SetAlias("cemc_e",Form("%f*emcal_e",weight[ifile]));
          // --- set an alias for the total energy
          T->SetAlias("tot_e", "hcal_e+cemc_e");
          // --- set an alias for the emcal energy asymmetry
          T->SetAlias("aemcal", "(cemc_e-hcal_e)/(cemc_e+hcal_e)");
          // --- draw a 2d histogram of total energy vs emcal asymmetry
          //T->Draw("tot_e");
          T->Draw("tot_e:aemcal","abs(aemcal)<1","colz");
          // --- draw a profile on top of that and also fill the data into the profile histogram
          T->Draw(Form("tot_e:aemcal>>%s",name.Data()),"", "Same"); //"prof,goff");
          // --- set some drawing options
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
          // --- update the canvas
          can->Update();
        } // loop over weights
    } // loop over files

} // end of macro
