//....................................................
//ProtoDUNE-DP PMT Calibration
//Waveform integration to obtain SPE spectra
//....................................................
// Ana Gallego Ros (ana.gallego@ciemat.es)
// First version: June 2019
//....................................................

void WF_integration(string label, string pmt_info_file, bool save_waveforms, bool all_the_events, const int number_events, const int nruns, bool top_fibers){
    
    cout << " " << endl;
    cout << "****************************************************" << endl;
    cout << " STEP 1/4: Waveform integration  " << endl;
    cout << "****************************************************" << endl;
    cout << " " << endl;

    //----------------------------
    // options
    //----------------------------
    int more_plots = true;
    bool debug = false;
    const int s_initial = 0; //first sample for pedestal calculation
    const int s_last = 30; //last sample for pedestal calculation (15 = 240 ns --> signal > 300 ns)
    
    //------------------------------------------------------
    // do not change! (in principle)
    //string sfilename = label+"/files.txt";
    string sfilename = label+"/files.txt";
    int xmin = -20e6;
    int xmax = 2e9;
    int nbins = 5000;
        
    int resistance = 50; //ohm (ADC)
    const double sampling = 16;//tTimeSample;
    int times = 0;
    int timess = 0;
    gStyle->SetOptStat(0);

    //------------------------------------------------------

    //--------------------------------
    //     PMT info file
    //--------------------------------
    ifstream inputFile(Form("%s.txt",pmt_info_file.c_str())); //<-- important! check
    cout << "Reading " << Form("%s.txt",pmt_info_file.c_str()) << endl;
    string line;
    int i=0;
    string SN[36];
    int info_ADC[36];
    string sn;
    int info_adc,hv1,hv2,hv3,hv4,led;
    
    while(getline(inputFile, line)) {
        istringstream fichero(line);
        if(pmt_info_file == "PMT_info_190724" || pmt_info_file == "PMT_info_190801"){
            fichero>>sn>>led>>info_adc>>hv1>>hv2>>hv3>>hv4;
        }
        else{
            fichero>>sn>>led>>info_adc;
        }
        SN[i] = sn;
        info_ADC[i] = info_adc;
        i++;
      //  cout<<sn<<" "<<info_adc<<endl;
    }
    
    //-----------------------------------
    // .txt for next step (join histograms)
    //-----------------------------------
    ofstream myfile;
    myfile.open(Form("%s/SPE/files_SPE.txt",label.c_str()));
    
    const int n = 7; //maximum number PMTs / LED
    int channel[n];

    //--------------------------------------------
    //             HISTOGRAMS
    //--------------------------------------------
    TH1D *h[n];
    char namee[100];
    char title[100];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_%i",i);
        sprintf(title,"title of h_%i",i);
        h[i] = new TH1D(namee,title,nbins,xmin,xmax); // <---- limits?
        h[i]->Reset("");
    }
    TH1D *hh[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"hh_%i",i);
        sprintf(title,"title of hh_%i",i);
        hh[i] = new TH1D(namee,title,nbins,xmin,xmax); // <---- limits?
        hh[i]->Reset("");
    }
    TH1D *h_30[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_30_%i",i);
        sprintf(title,"title of h_30_%i",i);
        h_30[i] = new TH1D(namee,title,nbins,0,5000); // <---- limits?
        h_30[i]->Reset("");
    }
    
    TH1D *h_rms[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_rms_%i",i);
        sprintf(title,"title of h_rms_%i",i);
        h_rms[i] = new TH1D(namee,title,50,0,3); // <---- limits?
        h_rms[i]->Reset("");
    }
    
    TH1D *h_peak[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_peak_%i",i);
        sprintf(title,"title of h_peak_%i",i);
        h_peak[i] = new TH1D(namee,title,126,0,126); // <---- Nsamples
       // h_peak[i] = new TH1D(namee,title,75,0,75); // <---- Nsamples
       // h_peak[i] = new TH1D(namee,title,63,0,63); // <---- Nsamples
        h_peak[i]->Reset("");
    }
    
    TH1D *h_peak_ns[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_peak_ns_%i",i);
        sprintf(title,"title of h_peak_ns_%i",i);
       //  h_peak_ns[i] = new TH1D(namee,title,63,0,1000); // <---- Nsamples x sampling
        //h_peak_ns[i] = new TH1D(namee,title,75,0,1200); // <---- Nsamples x sampling
      h_peak_ns[i] = new TH1D(namee,title,126,0,2000); // <---- Nsamples x sampling
        h_peak_ns[i]->Reset("");
    }
    
    TH1D *h_ped[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_ped_%i",i);
        sprintf(title,"title of h_ped_%i",i);
        h_ped[i] = new TH1D(namee,title,2000,2500,4500); // <---- limits?
        h_ped[i]->Reset("");
    }
    //---------------------------------------------
    
    //////////////////////////////////////////////
    //         input loop: ROOT files
    //////////////////////////////////////////////
    const char* cfilename = sfilename.c_str();
    ifstream ifile = cfilename;
    
    string line;
    string data;
    int LED=0;
    int count_hv=0;
    
    while(getline(ifile,line)){
        istringstream file(line);
        file >> data;
        cout << "Opening " << data << endl;
        
        string run = data.substr(9,4);// <--- run # (position might change!) // lxplus: (71,3), local:(10,3) (9,4)
        
        // LED: 1 2 3 4 5 6 1 2 3 4...
        LED++;
        cout<<"Run #"<<run<<", LED #"<<LED<<endl;
        
        string label2;
        label2 = Form("run%s_LED%i",run.c_str(),LED);
        myfile << Form("%s/SPE/%s_SPE.root",label.c_str(),label2.c_str()) << "\n";
        
        //..............................................
        //TFile *rFile = new TFile(Form("%s",data.c_str()), "READ"); //lxplus
        TFile *rFile = new TFile(Form("%s/%s",label.c_str(),data.c_str()), "READ"); //local
        
        TTree *rTree = (TTree*)rFile->Get("midas_data"); // <--- tree name
        const int NEntries = (int)rTree->GetEntries();
        cout << "Nevents: " << NEntries << std::endl;
        //..............................................
        
        int tnsamples;
        rTree->SetBranchAddress("nsamples",&tnsamples);
        //int tTimeSample;
        //rTree->SetBranchAddress("TimeSample",&tTimeSample);
        
        // Reset histograms:
        for (int i=0;i<n;i++) {
            h[i]->Reset("");
            hh[i]->Reset("");
            h_30[i]->Reset("");
            h_rms[i]->Reset("");
            h_peak[i]->Reset("");
            h_peak_ns[i]->Reset("");
            h_ped[i]->Reset("");
        }
        
        //------------------------------------------
        //       Number of PMTs
        //------------------------------------------
        if(LED==1 || LED==4){
            const int Npmts = 6;
        }
        else if(LED==2 || LED==5){
            const int Npmts = 5;
        }
        else if(LED==3 || LED==6){
            const int Npmts = 7;
        }
        //------------------------------------------

        //------------------------------------------
        
        if(LED==1){
            channel[0] = info_ADC[0];
            channel[1] = info_ADC[1];
            channel[2] = info_ADC[2];
            channel[3] = info_ADC[3];
            channel[4] = info_ADC[4];
            channel[5] = info_ADC[5];
        }// end LED 1
        
        if(LED==2){
            channel[0] = info_ADC[6];
            channel[1] = info_ADC[7];
            channel[2] = info_ADC[8];
            channel[3] = info_ADC[9];
            channel[4] = info_ADC[10];
        }// end LED 2
        
        if(LED==3){
            channel[0] = info_ADC[11];
            channel[1] = info_ADC[12];
            channel[2] = info_ADC[13];
            channel[3] = info_ADC[14];
            channel[4] = info_ADC[15];
            channel[5] = info_ADC[16];
            channel[6] = info_ADC[17];
        }// end LED 3
        
        if(LED==4){
            channel[0] = info_ADC[18];
            channel[1] = info_ADC[19];
            channel[2] = info_ADC[20];
            channel[3] = info_ADC[21];
            channel[4] = info_ADC[22];
            channel[5] = info_ADC[23];
        }// end LED 4
        
        if(LED==5){
            channel[0] = info_ADC[24];
            channel[1] = info_ADC[25];
            channel[2] = info_ADC[26];
            channel[3] = info_ADC[27];
            channel[4] = info_ADC[28];
        }// end LED 5
        
        if(LED==6){
            channel[0] = info_ADC[29];
            channel[1] = info_ADC[30];
            channel[2] = info_ADC[31];
            channel[3] = info_ADC[32];
            channel[4] = info_ADC[33];
            channel[5] = info_ADC[34];
            channel[6] = info_ADC[35];
        }// end LED 6
        
        //output root file:
        TList *l = new TList(); // <---- list of histograms
        string outfile = label+"/SPE/"+label2+"_SPE.root";  // <-- output histogram
        TFile *ofile = new TFile(outfile.c_str(),"RECREATE");

        // define & reset:
        double ped[n];
        double rms[n];
        double rms_cut[n];
        for(int k=0;k<n;k++){
            ped[k] = .0;
            rms[k] = .0;
            rms_cut[k] = .0;
        }
        
        // START: DEFINE RMS CUT! ---------------------------------------------------------
        for(int i=0; i<NEntries; i++){  // <---- Event loop (NEntries in general, change!)
            
            rTree->GetEntry(i);
            const int Nsamples = tnsamples;
            
           // cout<<Nsamples<<endl;
            
            short tadc_value_0[Nsamples];
            short tadc_value_1[Nsamples];
            short tadc_value_2[Nsamples];
            short tadc_value_3[Nsamples];
            short tadc_value_4[Nsamples];
            short tadc_value_5[Nsamples];
            short tadc_value_6[Nsamples];

            //select channels:
            if(LED==1){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[0]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[1]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[2]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[3]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[4]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[5]),&tadc_value_5);
                for(int ii = s_initial; ii < s_last; ii++){ //new range?
                    h_30[5]->Fill(tadc_value_5[ii]);
                }//end for 30 samples
            }//end if LED 1
            
            else if(LED==2){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[6]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[7]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[8]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[9]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[10]),&tadc_value_4);
            }//end if LED 2
            
            else if(LED==3){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[11]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[12]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[13]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[14]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[15]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[16]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[17]),&tadc_value_6);
                for(int ii = s_initial; ii < s_last; ii++){ //new range?
                    h_30[5]->Fill(tadc_value_5[ii]);
                    h_30[6]->Fill(tadc_value_6[ii]);
                }//end for 30 samples
            }//end if LED 3
            
            else if(LED==4){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[18]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[19]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[20]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[21]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[22]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[23]),&tadc_value_5);
                for(int ii = s_initial; ii < s_last; ii++){ //new range?
                    h_30[5]->Fill(tadc_value_5[ii]);
                }//end for 30 samples
            }//end if LED 4
            
            else if(LED==5){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[24]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[25]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[26]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[27]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[28]),&tadc_value_4);
            }//end if LED 5
            
            else if(LED==6){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[29]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[30]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[31]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[32]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[33]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[34]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[35]),&tadc_value_6);
                for(int ii = s_initial; ii < s_last; ii++){  //new range?
                    h_30[5]->Fill(tadc_value_5[ii]);
                    h_30[6]->Fill(tadc_value_6[ii]);
                }//end for 30 samples
            }//end if LED 6
            
            for(int ii = s_initial; ii < s_last; ii++){  //new range?
                h_30[0]->Fill(tadc_value_0[ii]);
                h_30[1]->Fill(tadc_value_1[ii]);
                h_30[2]->Fill(tadc_value_2[ii]);
                h_30[3]->Fill(tadc_value_3[ii]);
                h_30[4]->Fill(tadc_value_4[ii]);
            }//end for 30 samples

            for(int m=0;m<Npmts;m++){
                rms[m] = h_30[m]->GetRMS();
                h_30[m]->Reset(""); //<-- important!
               // cout<<m<<" "<<i<<endl;
                h_rms[m]->Fill(rms[m]);
            }
            
        }//end for i events

        // remove 12%
        for(int m=0;m<Npmts;m++){
            double xq[1];
            double yq[1];
            xq[0] = 0.88; //percentile
            h_rms[m]->GetQuantiles(1,yq,xq);
            rms_cut[m] = yq[0];
        }

        if(more_plots){ //rms
            TCanvas *cc11 = new TCanvas("cc11","");
            if(LED==3 || LED==6){
                cc11->Divide(4,2);
            }
            else{
                cc11->Divide(3,2);
            }
            for(int ij=0;ij<Npmts;ij++){
                cc11->cd(ij+1);
                h_rms[ij]->SetLineWidth(2);
                h_rms[ij]->SetLineColor(kBlue);
                h_rms[ij]->Draw("");
                h_rms[ij]->GetXaxis()->SetTitle("Pedestal RMS (ADC)");
                h_rms[ij]->SetTitle(Form("Run %s, LED %i, PMT %i (ADC ch# %i)",run.c_str(),LED,ij,channel[ij]));
            }

             //   cc11->Print(Form("%s/more_plots/ped_RMS_%s.pdf",label.c_str(),label2.c_str()),"pdf");

                if(timess==0){ //first
                    //cc11->Print(Form("%s/more_plots/ped_RMS_%s.pdf(",label.c_str(),label2.c_str()),"pdf");
                    cc11->Print(Form("%s/more_plots/ped_RMS.pdf(",label.c_str()),"pdf");
                }
                else if(timess==nruns-1){ //last
                    cc11->Print(Form("%s/more_plots/ped_RMS.pdf)",label.c_str()),"pdf");
                }
                else{ //rest
                    cc11->Print(Form("%s/more_plots/ped_RMS.pdf",label.c_str()),"pdf");
                }
        
            timess++;
            delete cc11;
            
        }//end if more plots rms
        // END: DEFINE RMS CUT! ---------------------------------------------------------
        
        // START: DEFINE INTREGRATION WINDOW! --------------------------------------------
        
        int min[n];
        int minimum[n];
        int bin[n];
        int peak_bin[n];
        int peak_bin_allrange[n];
        int max_allrange[n];
        double peak_x[n];
        double time0[n];
        double time1[n];

        for(int i=0; i<NEntries; i++){ // <---- Event loop (NEntries in general, change!)
            
            rTree->GetEntry(i);

            short tadc_value_0[Nsamples];
            short tadc_value_1[Nsamples];
            short tadc_value_2[Nsamples];
            short tadc_value_3[Nsamples];
            short tadc_value_4[Nsamples];
            short tadc_value_5[Nsamples];
            short tadc_value_6[Nsamples];
            
            if(LED==1){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[0]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[1]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[2]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[3]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[4]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[5]),&tadc_value_5);
                min[5] = tadc_value_5[0];
            }// end LED 1
            
            if(LED==2){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[6]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[7]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[8]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[9]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[10]),&tadc_value_4);
            }// end LED 2
            
            if(LED==3){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[11]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[12]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[13]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[14]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[15]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[16]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[17]),&tadc_value_6);
                min[5] = tadc_value_5[0];
                min[6] = tadc_value_6[0];
            }// end LED 3
            
            if(LED==4){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[18]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[19]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[20]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[21]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[22]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[23]),&tadc_value_5);
                min[5] = tadc_value_5[0];
            }// end LED 4
            
            if(LED==5){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[24]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[25]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[26]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[27]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[28]),&tadc_value_4);
            }// end LED 5
            
            if(LED==6){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[29]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[30]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[31]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[32]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[33]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[34]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[35]),&tadc_value_6);
                min[5] = tadc_value_5[0];
                min[6] = tadc_value_6[0];
            }// end LED 6
            
            min[0] = tadc_value_0[0];
            min[1] = tadc_value_1[0];
            min[2] = tadc_value_2[0];
            min[3] = tadc_value_3[0];
            min[4] = tadc_value_4[0];

            for(int ii = 0; ii < Nsamples; ii++){//Nsamples; ii++){ //find minimum value
                
                if(tadc_value_0[ii]<=min[0]){ // < or <= changes the position of peak
                    min[0] = tadc_value_0[ii];
                    minimum[0] = ii;
                }
                if(tadc_value_1[ii]<=min[1]){
                    min[1] = tadc_value_1[ii];
                    minimum[1] = ii;
                }
                if(tadc_value_2[ii]<=min[2]){
                    min[2] = tadc_value_2[ii];
                    minimum[2] = ii;
                }
                if(tadc_value_3[ii]<=min[3]){
                    min[3] = tadc_value_3[ii];
                    minimum[3] = ii;
                }
                if(tadc_value_4[ii]<=min[4]){
                    min[4] = tadc_value_4[ii];
                    minimum[4] = ii;
                }
                if(LED==1 || LED==4){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                }//end LEDs 1, 4
                if(LED==3 || LED==6){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                    if(tadc_value_6[ii]<=min[6]){
                        min[6] = tadc_value_6[ii];
                        minimum[6] = ii;
                    }
                }//end LED 3

            }// end for ii samples

            for(int m=0;m<Npmts;m++){
                //cout<<m<<" "<<minimum[m]<<" "<<min[m]<<endl;
                h_peak[m]->Fill(minimum[m]);
                h_peak_ns[m]->Fill(sampling*minimum[m]);
                //peak_bin[m] = h_peak[m]->GetMaximumBin();
                
               // h_peak[m]->GetXaxis()->SetRange(0,75); //NSamples
                peak_bin_allrange[m] = h_peak[m]->GetMaximumBin(); //returns maximum in range
                max_allrange[m] = h_peak[m]->GetBinContent(peak_bin_allrange[m]); //returns maximum in range
                
                //************************************************
               // h_peak[m]->GetXaxis()->SetRange(10,43); // check! 160 ns - 688 ns 63 samples
                //h_peak[m]->GetXaxis()->SetRange(10,31); // check! CROSSTALKS
                  h_peak[m]->GetXaxis()->SetRange(50,88); // check! 400 ns - 1400 ns 126 samples
                //************************************************
                peak_bin[m] = h_peak[m]->GetMaximumBin(); //returns maximum in range
                peak_x[m] = h_peak[m]->GetXaxis()->GetBinCenter(peak_bin[m]);
            }
            
        }// end for i events
        
        //**********************************
        // integration window for each PMT:
        //**********************************
        for(int m=0;m<Npmts;m++){
            time0[m]=(sampling*(peak_x[m]))-75.0; //ns
            time1[m]=(sampling*(peak_x[m]))+75.0; //ns
            if(debug) cout<<m<<" "<<peak_x[m]<<" "<<time0[m]<<" "<<time1[m]<<endl;
        }
        //cout<<peak_bin[5]<<" "<<peak_x[5]<<" "<<ceil(peak_x[5])<<endl;

        if(more_plots){
            TCanvas *c11 = new TCanvas("c11","");
            gStyle->SetOptStat(0);
            Double_t xl1=.2, yl1=0.8, xl2=xl1+.2, yl2=yl1+.07;
            if(LED==3 || LED==6){
                c11->Divide(4,2);
            }
            else{
                c11->Divide(3,2);
            }
            for(int ij=0;ij<Npmts;ij++){
                c11->cd(ij+1);
                h_peak_ns[ij]->SetLineWidth(2);
                h_peak_ns[ij]->SetLineColor(kBlue);
                h_peak_ns[ij]->Draw("");
                h_peak_ns[ij]->GetXaxis()->SetTitle("Time (ns) with minimum ADC");
                h_peak_ns[ij]->SetTitle(Form("Run %s, LED %i, PMT %i (ADC ch# %i)",run.c_str(),LED,ij,channel[ij]));

                TPaveText *pt = new TPaveText(xl1,yl1,xl2,yl2,"NDC");
                pt->SetTextFont(42);
                pt->SetTextSize(0.05);
                pt->SetTextColor(kRed);
                pt->SetFillColor(0);
                pt->SetBorderSize(0.0);
                pt->SetTextAlign(12);
                pt->AddText(Form("(%.0fns, %.0fns)",time0[ij],time1[ij]));
                pt->Draw("same");
                
                TLine *line_t0 = new TLine(time0[ij], 0, time0[ij], max_allrange[ij]);
                line_t0->SetLineColor(2);
                line_t0->Draw("same");
                
                TLine *line_t1 = new TLine(time1[ij], 0, time1[ij], max_allrange[ij]);
                line_t1->SetLineColor(2);
                line_t1->Draw("same");
            }
            TCanvas *c12 = new TCanvas("c12","");
            gStyle->SetOptStat(0);
            if(LED==3 || LED==6){
                c12->Divide(4,2);
            }
            else{
                c12->Divide(3,2);
            }
            for(int ij=0;ij<Npmts;ij++){
                c12->cd(ij+1);
                h_peak[ij]->SetLineWidth(2);
                h_peak[ij]->SetLineColor(kBlue);
                h_peak[ij]->Draw("");
                //h_peak[ij]->GetYaxis()->SetTitle("Number of events");
                h_peak[ij]->GetXaxis()->SetTitle("# sample with minimum ADC");
                h_peak[ij]->SetTitle(Form("Run %s, LED %i, PMT %i",run.c_str(),LED,ij));
            }
            
            //c12->Print(Form("%s/more_plots/sample_signal_%s.pdf",label.c_str(),label2.c_str()),"pdf");
            //c11->Print(Form("%s/more_plots/time_signal_%s.pdf",label.c_str(),label2.c_str()),"pdf");
            
            
            if(times==0){ //first
                //c11->Print(Form("%s/more_plots/time_signal_%s.pdf(",label.c_str(),label2.c_str()),"pdf");
                c11->Print(Form("%s/more_plots/time_signal.pdf(",label.c_str()),"pdf");
            }
            else if(times==nruns-1){ //last
                c11->Print(Form("%s/more_plots/time_signal.pdf)",label.c_str()),"pdf");
            }
            else{ //rest
                c11->Print(Form("%s/more_plots/time_signal.pdf",label.c_str()),"pdf");
            }

            delete c11;
            //delete c12;
            //cout<<"times "<<times<<endl;
            times++;
        }//end more plots
        // END: DEFINE INTREGRATION WINDOW! --------------------------------------------


        if(all_the_events){
            const int last = NEntries;// complete analysis
            cout << "Integrating " << last << " waveforms..." << endl;
        }
        else{
            const int last = number_events;//20000;//19993; //tests
            cout << "Integrating " << last << " waveforms..." << endl;
        }
        
        ///////////////////////////////////////////////////////////////
        for(int i=0; i<last; i++){ //NEntries;i++){   <---- Event loop (MAIN)
        ///////////////////////////////////////////////////////////////
            
            rTree->GetEntry(i);
            //const int Nsamples = tnsamples;
            
            if(debug){
                cout<<"WF #: "<<i<<endl;
                cout<< "Nsamples: "<<Nsamples<<endl;
                cout<< "Sampling (ns): "<<sampling<<endl;
                cout << " " << endl;
            }
            
            int tTimeStamp;
            //short tadc_value[7][Nsamples];
            short tadc_value_0[Nsamples];
            short tadc_value_1[Nsamples];
            short tadc_value_2[Nsamples];
            short tadc_value_3[Nsamples];
            short tadc_value_4[Nsamples];
            short tadc_value_5[Nsamples];
            short tadc_value_6[Nsamples];

            //Branches: (different for each LED)
            //rTree->SetBranchAddress("TimeStamp",&tTimeStamp);
            
            if(LED==1){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[0]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[1]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[2]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[3]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[4]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[5]),&tadc_value_5);
            }
            
            else if(LED==2){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[6]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[7]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[8]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[9]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[10]),&tadc_value_4);
            }
            
            else if(LED==3){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[11]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[12]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[13]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[14]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[15]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[16]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[17]),&tadc_value_6);
            }
            
            else if(LED==4){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[18]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[19]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[20]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[21]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[22]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[23]),&tadc_value_5);
            }//end if LED 4
            
            else if(LED==5){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[24]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[25]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[26]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[27]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[28]),&tadc_value_4);
            }//end if LED 5
            
            else if(LED==6){
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[29]),&tadc_value_0);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[30]),&tadc_value_1);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[31]),&tadc_value_2);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[32]),&tadc_value_3);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[33]),&tadc_value_4);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[34]),&tadc_value_5);
                rTree->SetBranchAddress(Form("adc_value_%i",info_ADC[35]),&tadc_value_6);
            }//end if LED 6
            
            // plot waveforms:
            int x[Nsamples] = {0};
            int y0[Nsamples] = {0};
            int y1[Nsamples] = {0};
            int y2[Nsamples] = {0};
            int y3[Nsamples] = {0};
            int y4[Nsamples] = {0};
            int y5[Nsamples] = {0};
            int y6[Nsamples] = {0};
            
            // define & reset:
            double charge[n];
            double ADC[n];
            double ped[n];
            double pedestal[n];
            for(int k=0;k<n;k++){
                charge[k] = .0;
                ADC[k] = .0;
                ped[k] = .0;
                rms[k] = .0;
                h_30[k]->Reset(""); //<-- important!
            }
     
            // CHECK RMS CUT:-------------------------
            for(int ii = s_initial; ii < s_last; ii++){
                h_30[0]->Fill(tadc_value_0[ii]);
                h_30[1]->Fill(tadc_value_1[ii]);
                h_30[2]->Fill(tadc_value_2[ii]);
                h_30[3]->Fill(tadc_value_3[ii]);
                h_30[4]->Fill(tadc_value_4[ii]);
            }//end for 30 samples
            
            if(LED==1 || LED==4){
                for(int ii = s_initial; ii < s_last; ii++){
                    h_30[5]->Fill(tadc_value_5[ii]);
                }//end for 30 samples
            }//end LED 1
            
            if(LED==3 || LED==6){
                for(int ii = s_initial; ii < s_last; ii++){
                    h_30[5]->Fill(tadc_value_5[ii]);
                    h_30[6]->Fill(tadc_value_6[ii]);
                }//end for 30 samples
            }//end LED 3
            
            // ---------------------------------------
            // OBTAIN RMS & PEDESTAL / CHANNEL:
            for(int m=0;m<Npmts;m++){
                rms[m] = h_30[m]->GetRMS();
                ped[m] = h_30[m]->GetMean();
            }
            // ---------------------------------------

            for(int ii = 0; ii < Nsamples; ii++){
                //cout<<Nsamples<<endl;
            
                x[ii] = ii*sampling; //ns
                
                //cout<<ii<<" "<<x[ii]<<" "<<time0[5]<<" "<<time1[5]<<endl;
                
                y0[ii] = tadc_value_0[ii];
                y1[ii] = tadc_value_1[ii];
                y2[ii] = tadc_value_2[ii];
                y3[ii] = tadc_value_3[ii];
                y4[ii] = tadc_value_4[ii];
                
                if(LED==1 || LED==3 || LED==4 || LED==6){
                    y5[ii] = tadc_value_5[ii];
                }
                
                if(LED==3 || LED==6){
                    y6[ii] = tadc_value_6[ii];
                }
                
                // put together in a loop?
                
                if(x[ii]>time0[0] && x[ii]<time1[0]){
                    ADC[0] = ADC[0] + (tadc_value_0[ii]-ped[0]);
                }
                if(x[ii]>time0[1] && x[ii]<time1[1]){
                    ADC[1] = ADC[1] + (tadc_value_1[ii]-ped[1]);
                }
                if(x[ii]>time0[2] && x[ii]<time1[2]){
                    ADC[2] = ADC[2] + (tadc_value_2[ii]-ped[2]);
                }
                
                if(x[ii]>time0[3] && x[ii]<time1[3]){
                    ADC[3] = ADC[3] + (tadc_value_3[ii]-ped[3]);
                    // cout<<i<<" "<<ii<<" "<<time0[3]<<" "<< time1[3] << " "<<x[ii]<<" "<<tadc_value_3[ii]<<" "<<ped[3]<<" "<<ADC[3]<<endl;
                }
                if(x[ii]>time0[4] && x[ii]<time1[4]){
                    ADC[4] = ADC[4] + (tadc_value_4[ii]-ped[4]);
                }
                if(LED==1 || LED==4){
                    if(x[ii]>time0[5] && x[ii]<time1[5]){
                        ADC[5] = ADC[5] + (tadc_value_5[ii]-ped[5]);
                    }
                }//end LEDs 1, 4
                if(LED==3 || LED==6){
                    if(x[ii]>time0[5] && x[ii]<time1[5]){
                        ADC[5] = ADC[5] + (tadc_value_5[ii]-ped[5]);
                    }
                    if(x[ii]>time0[6] && x[ii]<time1[6]){
                        ADC[6] = ADC[6] + (tadc_value_6[ii]-ped[6]);
                    }
                }//end LEDs 3, 6
                
            }//end loop ii samples
     
            // Fill SPE spectra:-------------------------------------------
 
            double Q[Npmts];
            for(int k=0;k<Npmts;k++){
                Q[k] = .0;
            }

            for(int k=0;k<Npmts;k++){
                //--------------------------------------------
                // RMS CUT & filling:
                //--------------------------------------------
                // cout<< i<<" "<<k<<" "<<rms[k]<<" "<<rms_cut[k]<<endl;
                if(rms[k]<rms_cut[k]){
                    charge[k] = ADC[k] * (2.0/4096.0) * sampling * (1e-9); // V * s
                    Q[k] = -(charge[k]/resistance)/(1.602e-19);
                    h[k]->Fill(Q[k]);
                }
            }
           // cout<< i<< " ch5" << " " << ADC[5] << " " << ped[5] << " " << Q[5] << endl;

            // -------------------------------------------------------------
            // start: plot and save waveforms
            // -------------------------------------------------------------
            if(save_waveforms){
                // Waveforms plots:
                TCanvas *c = new TCanvas("c","");
                c->SetCanvasSize(600,500);
                
                TGraph *gr0 = new TGraph(Nsamples,x,y0);
                TGraph *gr1 = new TGraph(Nsamples,x,y1);
                TGraph *gr2 = new TGraph(Nsamples,x,y2);
                TGraph *gr3 = new TGraph(Nsamples,x,y3);
                TGraph *gr4 = new TGraph(Nsamples,x,y4);
                
                if(LED==1 || LED==3 || LED==4 || LED==6){
                    TGraph *gr5 = new TGraph(Nsamples,x,y5);
                    gr5->GetXaxis()->SetTitleOffset(1.2);
                    gr5->GetYaxis()->SetTitleOffset(1.4);
                    gr5->GetXaxis()->SetTitleSize(0.04);
                    gr5->GetYaxis()->SetTitleSize(0.04);
                    gr5->GetXaxis()->SetLabelSize(0.04);
                    gr5->GetYaxis()->SetLabelSize(0.04);
                }//end LED 1
                
                if(LED==3 || LED==6){
                    TGraph *gr6 = new TGraph(Nsamples,x,y6);
                    gr6->GetXaxis()->SetTitleOffset(1.2);
                    gr6->GetYaxis()->SetTitleOffset(1.4);
                    gr6->GetXaxis()->SetTitleSize(0.04);
                    gr6->GetYaxis()->SetTitleSize(0.04);
                    gr6->GetXaxis()->SetLabelSize(0.04);
                    gr6->GetYaxis()->SetLabelSize(0.04);
                }//end LED 3

                //......................................
                gr0->GetXaxis()->SetTitleOffset(1.2);
                gr0->GetYaxis()->SetTitleOffset(1.4);
                gr0->GetXaxis()->SetTitleSize(0.04);
                gr0->GetYaxis()->SetTitleSize(0.04);
                gr0->GetXaxis()->SetLabelSize(0.04);
                gr0->GetYaxis()->SetLabelSize(0.04);
                //......................................
                gr2->GetXaxis()->SetTitleOffset(1.2);
                gr2->GetYaxis()->SetTitleOffset(1.4);
                gr2->GetXaxis()->SetTitleSize(0.04);
                gr2->GetYaxis()->SetTitleSize(0.04);
                gr2->GetXaxis()->SetLabelSize(0.04);
                gr2->GetYaxis()->SetLabelSize(0.04);
                //......................................
                gr1->GetXaxis()->SetTitleOffset(1.2);
                gr1->GetYaxis()->SetTitleOffset(1.4);
                gr1->GetXaxis()->SetTitleSize(0.04);
                gr1->GetYaxis()->SetTitleSize(0.04);
                gr1->GetXaxis()->SetLabelSize(0.04);
                gr1->GetYaxis()->SetLabelSize(0.04);
                //......................................
                gr3->GetXaxis()->SetTitleOffset(1.2);
                gr3->GetYaxis()->SetTitleOffset(1.4);
                gr3->GetXaxis()->SetTitleSize(0.04);
                gr3->GetYaxis()->SetTitleSize(0.04);
                gr3->GetXaxis()->SetLabelSize(0.04);
                gr3->GetYaxis()->SetLabelSize(0.04);
                //......................................
                gr4->GetXaxis()->SetTitleOffset(1.2);
                gr4->GetYaxis()->SetTitleOffset(1.4);
                gr4->GetXaxis()->SetTitleSize(0.04);
                gr4->GetYaxis()->SetTitleSize(0.04);
                gr4->GetXaxis()->SetLabelSize(0.04);
                gr4->GetYaxis()->SetLabelSize(0.04);
                //......................................

                gr0->SetLineColor(4);
                gr1->SetLineColor(4);
                gr2->SetLineColor(4);
                gr3->SetLineColor(4);
                gr4->SetLineColor(4);
                
                if(LED==1 || LED==3 || LED==4 || LED==6){
                    gr5->SetLineColor(4);
                    gr5->GetYaxis()->SetTitle("ADC");
                    gr5->GetXaxis()->SetTitle("time (ns)");
                }
                
                if(LED==3 || LED==6){
                    gr6->SetLineColor(4);
                    gr6->GetYaxis()->SetTitle("ADC");
                    gr6->GetXaxis()->SetTitle("time (ns)");
                }

                //gr1->SetMaximum(0.4);
                //gr1->SetMinimum(0.05);

                if(LED==1){
//                    SN[i] = sn;
//                    info_ADC[i] = info_adc;
                    //gr1->SetTitle(Form("Run #%s, FA0111, ADC ch #17 - WF #%i",run.c_str(),i));
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[0].c_str(),info_ADC[0],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[1].c_str(),info_ADC[1],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[2].c_str(),info_ADC[2],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[3].c_str(),info_ADC[3],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[4].c_str(),info_ADC[4],i));
                    gr5->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[5].c_str(),info_ADC[5],i));
                }//end LED 1
                
                if(LED==2){
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[6].c_str(),info_ADC[6],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[7].c_str(),info_ADC[7],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[8].c_str(),info_ADC[8],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[9].c_str(),info_ADC[9],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[10].c_str(),info_ADC[10],i));
                }//end LED 2
                
                if(LED==3){
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[11].c_str(),info_ADC[11],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[12].c_str(),info_ADC[12],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[13].c_str(),info_ADC[13],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[14].c_str(),info_ADC[14],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[15].c_str(),info_ADC[15],i));
                    gr5->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[16].c_str(),info_ADC[16],i));
                    gr6->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[17].c_str(),info_ADC[17],i));
                }//end LED 3
                
                if(LED==4){
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[18].c_str(),info_ADC[18],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[19].c_str(),info_ADC[19],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[20].c_str(),info_ADC[20],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[21].c_str(),info_ADC[21],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[22].c_str(),info_ADC[22],i));
                    gr5->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[23].c_str(),info_ADC[23],i));
                }//end LED 4
                
                if(LED==5){
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[24].c_str(),info_ADC[24],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[25].c_str(),info_ADC[25],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[26].c_str(),info_ADC[26],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[27].c_str(),info_ADC[27],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[28].c_str(),info_ADC[28],i));
                }//end LED 5
                
                if(LED==6){
                    gr0->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[29].c_str(),info_ADC[29],i));
                    gr1->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[30].c_str(),info_ADC[30],i));
                    gr2->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[31].c_str(),info_ADC[31],i));
                    gr3->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[32].c_str(),info_ADC[32],i));
                    gr4->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[33].c_str(),info_ADC[33],i));
                    gr5->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[34].c_str(),info_ADC[34],i));
                    gr6->SetTitle(Form("Run #%s, %s, ADC ch #%i - WF #%i",run.c_str(),SN[35].c_str(),info_ADC[35],i));
                }//end LED 6
                
                gr0->GetYaxis()->SetTitle("ADC");
                gr0->GetXaxis()->SetTitle("Time (ns)");
                gr1->GetYaxis()->SetTitle("ADC");
                gr1->GetXaxis()->SetTitle("Time (ns)");
                gr2->GetYaxis()->SetTitle("ADC");
                gr2->GetXaxis()->SetTitle("Time (ns)");
                gr3->GetYaxis()->SetTitle("ADC");
                gr3->GetXaxis()->SetTitle("Time (ns)");
                gr4->GetYaxis()->SetTitle("ADC");
                gr4->GetXaxis()->SetTitle("Time (ns)");

                //canvas:
                if(LED==3 || LED==6){
                    c->Divide(4,2);
                }
                else{
                    c->Divide(3,2);
                }
                
                c->cd(1);
                gr0->Draw("AL");
                c->cd(2);
                gr1->Draw("AL");
                c->cd(3);
                gr2->Draw("AL");
                c->cd(4);
                gr3->Draw("AL");
                c->cd(5);
                gr4->Draw("AL");
                
                if(LED==1|| LED==3 || LED==4 || LED==6){
                    c->cd(6);
                    gr5->Draw("AL");
                }
                if(LED==3 || LED==6){
                    c->cd(7);
                    gr6->Draw("AL");
                }
                
                c->Update();
                
                if(i==0){ //first
                    c->Print(Form("%s/WF/%s_WF.pdf(",label.c_str(),label2.c_str()),"pdf");
                }
                else if(i==last-1){ //last
                    c->Print(Form("%s/WF/%s_WF.pdf)",label.c_str(),label2.c_str()),"pdf");
                }
                else{ //rest
                    c->Print(Form("%s/WF/%s_WF.pdf",label.c_str(),label2.c_str()),"pdf");
                }
                //lets_pause();
                
                delete c;
            }// end if save_waveforms
            // -------------------------------------------------------------
            // end: plot and save waveforms
            // -------------------------------------------------------------
            
            if (i%1000==0) cout << i << " / " << NEntries << " waveforms analysed " << endl; //"\t"
            //cout<<i<<" "<<rms[0]<<" "<<rms_cut[0]<<" " <<counter_pmt0<<"/"<<last<<endl;
        }//end event loop i
 
        cout << "Saving..." << endl;
        if(save_waveforms){
            cout << "PMT waveforms: " << Form("%s_WF.pdf",label2.c_str()) << endl;
        }
    
        
        // SPE spectra:---------------------------------------
        //TCanvas *c1 = new TCanvas("c1","");
        //c1->SetCanvasSize(700,500);
        //gStyle->SetOptStat(0);
        
        if(LED==1){
            h[0]->SetTitle(Form("%s - ch #%i",SN[0].c_str(),info_ADC[0]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[1].c_str(),info_ADC[1]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[2].c_str(),info_ADC[2]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[3].c_str(),info_ADC[3]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[4].c_str(),info_ADC[4]));
            h[5]->SetTitle(Form("%s - ch #%i",SN[5].c_str(),info_ADC[5]));
        }
        
        if(LED==2){
            h[0]->SetTitle(Form("%s - ch #%i",SN[6].c_str(),info_ADC[6]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[7].c_str(),info_ADC[7]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[8].c_str(),info_ADC[8]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[9].c_str(),info_ADC[9]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[10].c_str(),info_ADC[10]));
        }
        
        if(LED==3){
            h[0]->SetTitle(Form("%s - ch #%i",SN[11].c_str(),info_ADC[11]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[12].c_str(),info_ADC[12]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[13].c_str(),info_ADC[13]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[14].c_str(),info_ADC[14]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[15].c_str(),info_ADC[15]));
            h[5]->SetTitle(Form("%s - ch #%i",SN[16].c_str(),info_ADC[16]));
            h[6]->SetTitle(Form("%s - ch #%i",SN[17].c_str(),info_ADC[17]));
        }
        
        if(LED==4){
            h[0]->SetTitle(Form("%s - ch #%i",SN[18].c_str(),info_ADC[18]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[19].c_str(),info_ADC[19]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[20].c_str(),info_ADC[20]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[21].c_str(),info_ADC[21]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[22].c_str(),info_ADC[22]));
            h[5]->SetTitle(Form("%s - ch #%i",SN[23].c_str(),info_ADC[23]));
        }
        
        if(LED==5){
            h[0]->SetTitle(Form("%s - ch #%i",SN[24].c_str(),info_ADC[24]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[25].c_str(),info_ADC[25]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[26].c_str(),info_ADC[26]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[27].c_str(),info_ADC[27]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[28].c_str(),info_ADC[28]));
        }
        
        if(LED==6){
            h[0]->SetTitle(Form("%s - ch #%i",SN[29].c_str(),info_ADC[29]));
            h[1]->SetTitle(Form("%s - ch #%i",SN[30].c_str(),info_ADC[30]));
            h[2]->SetTitle(Form("%s - ch #%i",SN[31].c_str(),info_ADC[31]));
            h[3]->SetTitle(Form("%s - ch #%i",SN[32].c_str(),info_ADC[32]));
            h[4]->SetTitle(Form("%s - ch #%i",SN[33].c_str(),info_ADC[33]));
            h[5]->SetTitle(Form("%s - ch #%i",SN[34].c_str(),info_ADC[34]));
            h[6]->SetTitle(Form("%s - ch #%i",SN[35].c_str(),info_ADC[35]));
        }
        
        for(int jj=0;jj<Npmts;jj++){
            //h[jj]->GetXaxis()->SetLimits(xmin,xmax);
            //h[jj]->Draw("HIST");
            h[jj]->GetYaxis()->SetTitle("Events");
            h[jj]->GetXaxis()->SetTitle("Charge (e-)");
            l->Add(h[jj]);
        }

        //c1->Update();
        
        l->Write();//"histlist", TObject::kSingleKey);
        ofile->ls();


        // ------------------------------------------------------

        if(LED==6){
            count_hv++;
            cout << "end of HV #0" << count_hv << endl;
            LED=0;
        }
        
        
        rFile->Close(); //root file
    } // end while input file (.root)
    
    myfile.close();
    //gSystem->Exit(0);
    
 
}
