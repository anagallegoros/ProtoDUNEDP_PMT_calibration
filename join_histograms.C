//....................................................
//ProtoDUNE-DP PMT Calibration
//Include 36 histograms in the same file / HV
//....................................................
// Ana Gallego Ros (ana.gallego@ciemat.es)
// First version: June 2019
//....................................................

void join_histograms(string label){
    
    //gROOT->Reset();
    
    cout << " " << endl;
    cout << "****************************************************************" << endl;
    cout << " STEP 2/4: Join the SPE spectra: file with 36 histograms / HV  " << endl;
    cout << "****************************************************************" << endl;
    cout << " " << endl;

    // Do not change! (in principle)
    bool debug = false;
    const int n = 36;
    int xmin = 1e21;
    int xmax = 3e21;
    int nbins = 20;
    int counter = 0;
    int LED = 0;
    int counter_HV = 1;

    //--------------------------------------------
    //read file txt created by previous macro
    //--------------------------------------------
    string sfilename = label+"/SPE/files_SPE.txt";
    const char* cfilename = sfilename.c_str();
    ifstream ifile = cfilename;

    //--------------------------------------------
    //             HISTOGRAMS
    //--------------------------------------------
    TH1D* h[n];
    char namee[100];
    char title[100];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_%i",i);
        sprintf(title,"title of h_%i",i);
        h[i] = new TH1D(namee,title,nbins,xmin,xmax);
    }
    //--------------------------------------------

    //////////////////////////////////////////////
    //         input loop: ROOT files
    //////////////////////////////////////////////

    string line;
    string spe;
    while(getline(ifile,line)){ // 6 times / HV
        //..............................................
        istringstream file(line);
        file >> spe;
        cout << "Opening " << spe << endl;
        TFile *f = new TFile(spe.c_str(),"READ");
        //..............................................

        counter++;
        if(LED > 5){
            LED = 0;
            counter_HV++;
        }
        LED++;
        //cout<< counter << " " << counter_HV << " " << LED<< endl;

        if(LED == 1){// 6 PMTs
            TH1D *h[0] = (TH1D*)f->Get("h_0");
            TH1D *h[1] = (TH1D*)f->Get("h_1");
            TH1D *h[2] = (TH1D*)f->Get("h_2");
            TH1D *h[3] = (TH1D*)f->Get("h_3");
            TH1D *h[4] = (TH1D*)f->Get("h_4");
            TH1D *h[5] = (TH1D*)f->Get("h_5");
            //h[0]->SetName("h_0");
        }
        
        else if(LED == 2){// 5 PMTs
            TH1D *h[6] = (TH1D*)f->Get("h_0");
            TH1D *h[7] = (TH1D*)f->Get("h_1");
            TH1D *h[8] = (TH1D*)f->Get("h_2");
            TH1D *h[9] = (TH1D*)f->Get("h_3");
            TH1D *h[10] = (TH1D*)f->Get("h_4");
        }
        
        else if(LED == 3){// 7 PMTs
            TH1D *h[11] = (TH1D*)f->Get("h_0");
            TH1D *h[12] = (TH1D*)f->Get("h_1");
            TH1D *h[13] = (TH1D*)f->Get("h_2");
            TH1D *h[14] = (TH1D*)f->Get("h_3");
            TH1D *h[15] = (TH1D*)f->Get("h_4");
            TH1D *h[16] = (TH1D*)f->Get("h_5");
            TH1D *h[17] = (TH1D*)f->Get("h_6");
        }

        if(LED == 4){// 6 PMTs
            TH1D *h[18] = (TH1D*)f->Get("h_0");
            TH1D *h[19] = (TH1D*)f->Get("h_1");
            TH1D *h[20] = (TH1D*)f->Get("h_2");
            TH1D *h[21] = (TH1D*)f->Get("h_3");
            TH1D *h[22] = (TH1D*)f->Get("h_4");
            TH1D *h[23] = (TH1D*)f->Get("h_5");
        }
        
        else if(LED == 5){// 5 PMTs
            TH1D *h[24] = (TH1D*)f->Get("h_0");
            TH1D *h[25] = (TH1D*)f->Get("h_1");
            TH1D *h[26] = (TH1D*)f->Get("h_2");
            TH1D *h[27] = (TH1D*)f->Get("h_3");
            TH1D *h[28] = (TH1D*)f->Get("h_4");
        }
        
        else if(LED == 6){// 7 PMTs
            TH1D *h[29] = (TH1D*)f->Get("h_0");
            TH1D *h[30] = (TH1D*)f->Get("h_1");
            TH1D *h[31] = (TH1D*)f->Get("h_2");
            TH1D *h[32] = (TH1D*)f->Get("h_3");
            TH1D *h[33] = (TH1D*)f->Get("h_4");
            TH1D *h[34] = (TH1D*)f->Get("h_5");
            TH1D *h[35] = (TH1D*)f->Get("h_6");
        }

        if(counter == 6){ // 6 files
            //--------------------------------------------
            //output file:
            //--------------------------------------------
            string outfile = Form("%s/FIT/HV_0%i.root",label.c_str(),counter_HV);  // <--- output histogram
            cout << "Saving " << Form("%s/FIT/HV_0%i.root",label.c_str(),counter_HV) << endl;
            cout << " " << endl;
            TFile *ofile = new TFile(outfile.c_str(),"RECREATE");
            for(int j=0;j<n;j++){
                string name = Form("h_%i",j);
                h[j]->Write(name.c_str());
                h[j]->Reset("");
            }
            counter = 0;
            ofile->Close();
        }
        
    } // end while input file

    //gSystem->Exit(0);

    
}
