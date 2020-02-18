//....................................................


void SPE_amplitude(string label, string pmt_info_file, bool save_waveforms, bool all_the_events, const int number_events, const int nruns, bool top_fibers){
    
    cout << " " << endl;
    cout << "****************************************************" << endl;
    cout << " SPE amplitude " << endl;
    cout << "****************************************************" << endl;
    cout << " " << endl;

    /////////////////
    int LED = 4;
    const double p0=0.7;
    ////////////////
    
    //----------------------------
    // options
    //----------------------------
    int more_plots = false;
    bool debug = false;
    const int s_initial = 0;
    const int s_last = 10;
    
    double old_yped;
    double yped;
    double ymax;
    double xmax;
    double old_ymax;
    int bin_max;
    int Nbins;
    double max_yped;
    double max_yspe;
    double max_binspe;
    double max_binped;
    double min_yped;
    double y;
    
    double mean_ped,error_mean_ped,mean_spe,error_mean_spe,sigma_ped,sigma_spe;
    double x1ped,x2ped,x1spe,x2spe,x1,x2,x2max;
    
    ofstream myfile;
    myfile.open(Form("%s/SPE_amplitude/results_LED%i.txt",label.c_str(),LED));
    
    //------------------------------------------------------
    // do not change! (in principle)
    //string sfilename = label+"/files.txt";
    string sfilename = label+"/file.txt";
    int xmin = -20e6;
    int xmaxx = 2e9;
    int nbins = 5000;
        
    int resistance = 50; //ohm (ADC)
    const double sampling = 16;//tTimeSample;
    int times = 0;
    int timess = 0;
   // gStyle->SetOptStat(0);

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
     //   cout<<sn<<" "<<info_adc<<endl;
    }
    
    
    const int n = 7; //maximum number PMTs / LED
    
    int start_histo = 0;
    int end_histo = 2000;
    int nbins = 500;
    //--------------------------------------------
    //             HISTOGRAMS
    //--------------------------------------------
    TH1D *h[8];
    char namee[100];
    char title[100];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h_%i",i);
        sprintf(title,"title of h_%i",i);
        h[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h[i]->Reset("");
    }
    
    TH1D *h1[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h1_%i",i);
        sprintf(title,"title of h_%i",i);
        h1[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h1[i]->Reset("");
    }
    
    TH1D *h2[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h2_%i",i);
        sprintf(title,"title of h_%i",i);
        h2[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h2[i]->Reset("");
    }
    
    TH1D *h3[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h3_%i",i);
        sprintf(title,"title of h_%i",i);
        h3[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h3[i]->Reset("");
    }
    
    TH1D *h4[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h4_%i",i);
        sprintf(title,"title of h_%i",i);
        h4[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h4[i]->Reset("");
    }
    
    TH1D *h5[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h5_%i",i);
        sprintf(title,"title of h_%i",i);
        h5[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h5[i]->Reset("");
    }
    
    TH1D *h6[8];
    for (int i=0;i<8;i++) {
        sprintf(namee,"h6_%i",i);
        sprintf(title,"title of h_%i",i);
        h6[i] = new TH1D(namee,title,nbins,start_histo,end_histo); // <---- limits?
        h6[i]->Reset("");
    }
    
    
    TH1D *hh[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"hh_%i",i);
        sprintf(title,"title of hh_%i",i);
        hh[i] = new TH1D(namee,title,nbins,xmin,xmaxx); // <---- limits?
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
        h_peak[i] = new TH1D(namee,title,76,0,75); // <---- Nsamples
        h_peak[i]->Reset("");
    }
    
    TH1D *h_peak_ns[n];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_peak_ns_%i",i);
        sprintf(title,"title of h_peak_ns_%i",i);
        h_peak_ns[i] = new TH1D(namee,title,76,0,1200); // <---- Nsamples x sampling
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
   // int LED=0;
    int count_hv=0;
    
    int total_pmts = 0;
    int this_hv;

    
    int hv = 0;
    TCanvas *cc = new TCanvas;
    cc->Divide(4,2);
    
    TCanvas *cc1 = new TCanvas;
    cc1->Divide(4,2);
    
    TCanvas *cc2 = new TCanvas;
    cc2->Divide(4,2);
    
    TCanvas *cc3 = new TCanvas;
    cc3->Divide(4,2);
    
    TCanvas *cc4 = new TCanvas;
    cc4->Divide(4,2);
    
    if(LED==1 || LED==4){
        TCanvas *cc5 = new TCanvas;
        cc5->Divide(4,2);
    }
   
    else if(LED==3 || LED==6){
        TCanvas *cc5 = new TCanvas;
        cc5->Divide(4,2);
        TCanvas *cc6 = new TCanvas;
        cc6->Divide(4,2);
    }

    
    while(getline(ifile,line)){
        istringstream file(line);
        file >> data;
        cout << "Opening " << data << endl;
        
        string run = data.substr(9,4);// <--- run # (position might change!) // lxplus: (71,3), local:(10,3) (9,4)
        
        // LED: 1 2 3 4 5 6 1 2 3 4...
        //LED++;
        
        cout<<"Run #"<<run<<", LED #"<<LED<<endl;
        
        string label2;
        label2 = Form("run%s_LED%i",run.c_str(),LED);
        
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
            //h[i]->Reset("");
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

        cout<<LED<<" "<<Npmts<<endl;

        // START:  --------------------------------------------
        
        // define:
        double ped[n];
        double rms[n];
        double rms_cut[n];
        double max_amp[n];
        
        int min[n];
        int minimum[n];
        int bin[n];
        int peak_bin[n];
        int peak_bin_allrange[n];
        int max_allrange[n];
        double peak_x[n];
        double time0[n];
        double time1[n];

        for(int i=0; i<NEntries; i++){ // <---- Event loop --> NEntries

            rTree->GetEntry(i);
            const int Nsamples = tnsamples;

            //reset:
            for(int k=0;k<n;k++){
                ped[k] = .0;
                rms[k] = .0;
                rms_cut[k] = .0;
                max_amp[k] = .0;
            }

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
                //cout<<"led2"<<endl;
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
            

            //PEDESTAL:
            for(int ii = s_initial; ii < s_last; ii++){
                h_30[0]->Fill(tadc_value_0[ii]);
                h_30[1]->Fill(tadc_value_1[ii]);
                h_30[2]->Fill(tadc_value_2[ii]);
                h_30[3]->Fill(tadc_value_3[ii]);
                h_30[4]->Fill(tadc_value_4[ii]);
            }//end for x samples
            
            if(LED==1 || LED==4){
                for(int ii = s_initial; ii < s_last; ii++){
                    h_30[5]->Fill(tadc_value_5[ii]);
                }//end for x samples
            }//end LEDs 1, 4
            
            if(LED==3 || LED==6){
                for(int ii = s_initial; ii < s_last; ii++){
                    h_30[5]->Fill(tadc_value_5[ii]);
                    h_30[6]->Fill(tadc_value_6[ii]);
                }//end for x samples
            }//end LEDs 3, 6
 
            
            // ---------------------------------------
            // OBTAIN RMS & PEDESTAL / CHANNEL / EVENT:
            for(int m=0;m<Npmts;m++){
                rms[m] = h_30[m]->GetRMS();
                ped[m] = h_30[m]->GetMean();
                h_30[m]->Reset("");
                //cout<<i<<" "<<m<<" "<<ped[m]<<" "<<rms[m]<<endl;
            }
            // ---------------------------------------
            //CHECK:
            // ---------------------------------------
            int new_s_initial = s_initial;
            int new_s_last = s_last;

            for(int m=0;m<Npmts;m++){

                if(rms[m]>3.0){
                    while(rms[m]>3.0 && new_s_last<60){

                       // cout<<i<<"; Ped RMS > 3.0! Recalculating..."<<endl;
                        
                        new_s_initial = new_s_initial + 10;
                        new_s_last = new_s_last + 10;

                        h_30[m]->Reset("");

                        for(int ii = new_s_initial; ii < new_s_last; ii++){
                            if(m==0) h_30[m]->Fill(tadc_value_0[ii]);
                            if(m==1) h_30[m]->Fill(tadc_value_1[ii]);
                            if(m==2) h_30[m]->Fill(tadc_value_2[ii]);
                            if(m==3) h_30[m]->Fill(tadc_value_3[ii]);
                            if(m==4) h_30[m]->Fill(tadc_value_4[ii]);

                            if(LED==1 || LED==4){
                                if(m==5) h_30[m]->Fill(tadc_value_5[ii]);
                            }//end LEDs 1, 4

                            if(LED==3 || LED==5){
                                if(m==5) h_30[m]->Fill(tadc_value_5[ii]);
                                if(m==6) h_30[m]->Fill(tadc_value_6[ii]);
                            }//end LEDs 3, 5

                        }//end for x samples

                        // ---------------------------------------
                        // OBTAIN RMS & PEDESTAL / CHANNEL / EVENT:

                        rms[m] = h_30[m]->GetRMS();
                        ped[m] = h_30[m]->GetMean();
                    }//end while
                }//End if rms>3
            }//end for npmts

            // ---------------------------------------

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
                if(LED==1){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                }//end LED 1
                if(LED==3){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                    if(tadc_value_6[ii]<=min[6]){
                        min[6] = tadc_value_6[ii];
                        minimum[6] = ii;
                    }
                }//end LED 3
                if(LED==4){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                }//end LED 4
                if(LED==6){
                    if(tadc_value_5[ii]<=min[5]){
                        min[5] = tadc_value_5[ii];
                        minimum[5] = ii;
                    }
                    if(tadc_value_6[ii]<=min[6]){
                        min[6] = tadc_value_6[ii];
                        minimum[6] = ii;
                    }
                }//end LED 6
            }// end for ii samples


            
            //Max. amplitude per event:
            for(int k=0;k<Npmts;k++){
                max_amp[k] = ped[k] - min[k];
               // cout<<"LED: "<<LED<<", PMT: "<<k<<" "<<", PED_RMS: "<<rms[k]<<", PED: "<<ped[k]<<" "<<", min: "<<min[k]<<" "<<", MAX. AMPLITUDE: "<<max_amp[k]<<endl;
                //lets_pause();
            }
            

            
          //  cout<<hv<<" "<<i<<endl;
            h[hv]->Fill(max_amp[0]);
            h1[hv]->Fill(max_amp[1]);
            h2[hv]->Fill(max_amp[2]);
            h3[hv]->Fill(max_amp[3]);
            h4[hv]->Fill(max_amp[4]);
            
            if(LED==1 || LED==4){
                h5[hv]->Fill(max_amp[5]);            }
            
            else if(LED==3 || LED==6){
                h6[hv]->Fill(max_amp[6]);
            }

            if(i%1000==0) cout << i << "/NEntries proccessed!"<<endl;
             
            
        }// end for i events

        
        this_hv = 1400 + (hv*50);
        //************************************************
//        if(hv<4){
//            h[hv]->GetXaxis()->SetRange(15,end_histo); // check!
//            h1[hv]->GetXaxis()->SetRange(15,end_histo); // check!
//            h2[hv]->GetXaxis()->SetRange(15,end_histo); // check!
//            h3[hv]->GetXaxis()->SetRange(15,end_histo); // check!
//            h4[hv]->GetXaxis()->SetRange(15,end_histo); // check!
//        }
//        else{
//            h[hv]->GetXaxis()->SetRange(40,end_histo); // check!
//            h1[hv]->GetXaxis()->SetRange(40,end_histo); // check!
//            h2[hv]->GetXaxis()->SetRange(40,end_histo); // check!
//            h3[hv]->GetXaxis()->SetRange(40,end_histo); // check!
//            h4[hv]->GetXaxis()->SetRange(40,end_histo); // check!
//        }
//        //************************************************
//

        
//        cout<<" "<<endl;
//        cout<<run<<" "<<SN[0]<<" "<<hv<<" "<<this_hv<<endl;
//        cout<<run<<" "<<SN[1]<<" "<<hv<<" "<<this_hv<<endl;
//        cout<<" "<<endl;

        int end_fit = end_histo;
        
        int this_pmt;
        
        if(LED==1){ //6
            this_pmt = 0;
        }
        else if(LED==2){ //5
            this_pmt = 6;
        }
        else if(LED==3){//7
            this_pmt = 11;
        }
        else if(LED==4){//6
            this_pmt = 18;
        }
        else if(LED==5){//5
            this_pmt = 23;
        }
        else if(LED==6){//7
            this_pmt = 30;
        }
        
        
        // pmt 0:
        cc->cd(hv+1);
        gPad->SetLogy();
        h[hv]->SetTitle(Form("%s, %i V",SN[this_pmt].c_str(),this_hv));
        h[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
        h[hv]->GetYaxis()->SetTitle("Entries");
        h[hv]->GetXaxis()->SetTitleOffset(0.95);
        h[hv]->GetYaxis()->SetTitleOffset(0.9);
        h[hv]->GetXaxis()->SetTitleSize(0.04);
        h[hv]->GetYaxis()->SetTitleSize(0.04);
        h[hv]->GetXaxis()->SetLabelSize(0.04);
        h[hv]->GetYaxis()->SetLabelSize(0.04);
        h[hv]->SetLineWidth(2);
        h[hv]->SetLineColor(1);
        h[hv]->Draw("");
        //h[hv]->Rebin(2);
        
        //--------------------------------------------
        // get maximum:
        //--------------------------------------------
        //--------------------------------------------
        double x1spe = 20.0;
        int xx = h[hv]->FindBin(x1spe);
        //--------------------------------------------
        //--------------------------------------------
        // Find maximum of SPE:
        //--------------------------------------------
        max_yspe = 0.0;
        for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
            y = h[hv]->GetBinContent(j);
            if(y>max_yspe){
                max_yspe = y;
                max_binspe = j;
            }
        }
        double max_xspe = h[hv]->GetBinCenter(max_binspe);
        double distance = max_xspe - x1spe;

        //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
        //--------------------------------------------
        double error_spe=11.;
        double p = p0;
        // mean_spe = max_xspe;//first
        while(error_spe>5 && p<20.0){ //important!
            
            x1spe = max_xspe - (p*distance);
            x2spe = max_xspe + (p*distance);

            //--------------------------------------------
            TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
            h[hv]->Fit("g2","QER");
            mean_spe = g2->GetParameter(1);
            error_mean_spe = g2->GetParError(1);
            
            error_spe = abs(100*error_mean_spe/mean_spe);
            sigma_spe = g2->GetParameter(2);
            //--------------------------------------------
            //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
            p = p + 0.5;;

        } //end while
        //--------------------------------------------
        myfile << 0 << " " << SN[this_pmt] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe) << " " << Form("%.0f",sigma_spe) <<"\n";
        //--------------------------------------------

        
        
        
        // pmt 1:
        cc1->cd(hv+1);
        gPad->SetLogy();
        h1[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+1].c_str(),this_hv));
        h1[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
        h1[hv]->GetYaxis()->SetTitle("Entries");
        h1[hv]->GetXaxis()->SetTitleOffset(0.95);
        h1[hv]->GetYaxis()->SetTitleOffset(0.9);
        h1[hv]->GetXaxis()->SetTitleSize(0.04);
        h1[hv]->GetYaxis()->SetTitleSize(0.04);
        h1[hv]->GetXaxis()->SetLabelSize(0.04);
        h1[hv]->GetYaxis()->SetLabelSize(0.04);
        h1[hv]->SetLineWidth(2);
        h1[hv]->SetLineColor(1);
        h1[hv]->Draw("");
        //h1[hv]->Rebin(2);
        //--------------------------------------------
        // get maximum:
        //--------------------------------------------
        //--------------------------------------------
        double x1spe = 20.0;
        int xx = h1[hv]->FindBin(x1spe);
        //--------------------------------------------
        //--------------------------------------------
        // Find maximum of SPE:
        //--------------------------------------------
        max_yspe = 0.0;
        for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
            y = h1[hv]->GetBinContent(j);
            if(y>max_yspe){
                max_yspe = y;
                max_binspe = j;
            }
        }
        double max_xspe = h1[hv]->GetBinCenter(max_binspe);
        double distance = max_xspe - x1spe;
        
        //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
        //--------------------------------------------
        double error_spe=11.;
        double p = p0;
        // mean_spe = max_xspe;//first
        while(error_spe>3 && p<20.0){ //important!
            
            x1spe = max_xspe - (p*distance);
            x2spe = max_xspe + (p*distance);
            
            
//            if(label=="runs_1006_1013"){
//                if(SN[this_pmt+1]=="FA0111" && this_hv>1451){
//                    x1spe = 10;
//                    x2spe = 40;
//                }
//                if(SN[this_pmt+1]=="FA0111" && this_hv>1551){
//                    x1spe = 20;
//                    x2spe = 60;
//                }
//                if(SN[this_pmt+1]=="FA0111" && this_hv>1651){
//                    x1spe = 30;
//                    x2spe = 100;
//                }
//                if(SN[this_pmt+1]=="FA0111" && this_hv>1701){
//                    x1spe = 40;
//                    x2spe = 140;
//                }
//
//
//
//
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1451){
//                    x1spe = 30;
//                    x2spe = 80;
//                }
//
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1501){
//                    x1spe = 40;
//                    x2spe = 120;
//                }
//
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1551){
//                    x1spe = 70;
//                    x2spe = 160;
//                }
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1601){
//                    x1spe = 100;
//                    x2spe = 220;
//                }
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1651){
//                    x1spe = 100;
//                    x2spe = 400;
//                }
//                if(SN[this_pmt+1]=="FA0147" && this_hv>1701){
//                    x1spe = 200;
//                    x2spe = 500;
//                }
//
//            }//end if 1006_1013
            
            //--------------------------------------------
            TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
            h1[hv]->Fit("g2","QER");
            mean_spe = g2->GetParameter(1);
            error_mean_spe = g2->GetParError(1);
            
            error_spe = abs(100*error_mean_spe/mean_spe);
            sigma_spe = g2->GetParameter(2);
            //--------------------------------------------
            //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
            p = p + 0.5;;
            
        } //end while
        //--------------------------------------------
        myfile << 1 << " " << SN[this_pmt+1] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
        //--------------------------------------------
        
        // pmt 2:
        cc2->cd(hv+1);
        gPad->SetLogy();
        h2[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+2].c_str(),this_hv));
        h2[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
        h2[hv]->GetYaxis()->SetTitle("Entries");
        h2[hv]->GetXaxis()->SetTitleOffset(0.95);
        h2[hv]->GetYaxis()->SetTitleOffset(0.9);
        h2[hv]->GetXaxis()->SetTitleSize(0.04);
        h2[hv]->GetYaxis()->SetTitleSize(0.04);
        h2[hv]->GetXaxis()->SetLabelSize(0.04);
        h2[hv]->GetYaxis()->SetLabelSize(0.04);
        h2[hv]->SetLineWidth(2);
        h2[hv]->SetLineColor(1);
        h2[hv]->Draw("");
        //h2[hv]->Rebin(2);
        //--------------------------------------------
        // get maximum:
        //--------------------------------------------
        //--------------------------------------------
        double x1spe = 20.0;
        int xx = h2[hv]->FindBin(x1spe);
        //--------------------------------------------
        //--------------------------------------------
        // Find maximum of SPE:
        //--------------------------------------------
        max_yspe = 0.0;
        for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
            y = h2[hv]->GetBinContent(j);
            if(y>max_yspe){
                max_yspe = y;
                max_binspe = j;
            }
        }
        double max_xspe = h2[hv]->GetBinCenter(max_binspe);
        double distance = max_xspe - x1spe;
        
        //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
        //--------------------------------------------
        double error_spe=11.;
        double p = p0;
        // mean_spe = max_xspe;//first
        while(error_spe>5 && p<20.0){ //important!
            
            x1spe = max_xspe - (p*distance);
            x2spe = max_xspe + (p*distance);
            
            //--------------------------------------------
            TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
            h2[hv]->Fit("g2","QER");
            mean_spe = g2->GetParameter(1);
            error_mean_spe = g2->GetParError(1);
            
            error_spe = abs(100*error_mean_spe/mean_spe);
            sigma_spe = g2->GetParameter(2);
            //--------------------------------------------
            //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
            p = p + 0.5;;
            
        } //end while
        //--------------------------------------------
        myfile << 2 << " " << SN[this_pmt+2] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
        //--------------------------------------------
        
        // pmt 3:
        cc3->cd(hv+1);
        gPad->SetLogy();
        h3[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+3].c_str(),this_hv));
        h3[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
        h3[hv]->GetYaxis()->SetTitle("Entries");
        h3[hv]->GetXaxis()->SetTitleOffset(0.95);
        h3[hv]->GetYaxis()->SetTitleOffset(0.9);
        h3[hv]->GetXaxis()->SetTitleSize(0.04);
        h3[hv]->GetYaxis()->SetTitleSize(0.04);
        h3[hv]->GetXaxis()->SetLabelSize(0.04);
        h3[hv]->GetYaxis()->SetLabelSize(0.04);
        h3[hv]->SetLineWidth(2);
        h3[hv]->SetLineColor(1);
        h3[hv]->Draw("");
        //h3[hv]->Rebin(2);
        //--------------------------------------------
        // get maximum:
        //--------------------------------------------
        //--------------------------------------------
        double x1spe = 20.0;
        int xx = h3[hv]->FindBin(x1spe);
        //--------------------------------------------
        //--------------------------------------------
        // Find maximum of SPE:
        //--------------------------------------------
        max_yspe = 0.0;
        for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
            y = h3[hv]->GetBinContent(j);
            if(y>max_yspe){
                max_yspe = y;
                max_binspe = j;
            }
        }
        double max_xspe = h3[hv]->GetBinCenter(max_binspe);
        double distance = max_xspe - x1spe;
        
        //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
        //--------------------------------------------
        double error_spe=11.;
        double p = p0;
        // mean_spe = max_xspe;//first
        while(error_spe>5 && p<20.0){ //important!
            
            x1spe = max_xspe - (p*distance);
            x2spe = max_xspe + (p*distance);
            
            //--------------------------------------------
            TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
            h3[hv]->Fit("g2","QER");
            mean_spe = g2->GetParameter(1);
            error_mean_spe = g2->GetParError(1);
            
            error_spe = abs(100*error_mean_spe/mean_spe);
            sigma_spe = g2->GetParameter(2);
            //--------------------------------------------
            //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
            p = p + 0.5;;
            
        } //end while
        //--------------------------------------------
        myfile << 3 << " " << SN[this_pmt+3] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
        //--------------------------------------------
        
        
        
        // pmt 4:
        cc4->cd(hv+1);
        gPad->SetLogy();
        h4[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+4].c_str(),this_hv));
        h4[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
        h4[hv]->GetYaxis()->SetTitle("Entries");
        h4[hv]->GetXaxis()->SetTitleOffset(0.95);
        h4[hv]->GetYaxis()->SetTitleOffset(0.9);
        h4[hv]->GetXaxis()->SetTitleSize(0.04);
        h4[hv]->GetYaxis()->SetTitleSize(0.04);
        h4[hv]->GetXaxis()->SetLabelSize(0.04);
        h4[hv]->GetYaxis()->SetLabelSize(0.04);
        h4[hv]->SetLineWidth(2);
        h4[hv]->SetLineColor(1);
        h4[hv]->Draw("");
        //h4[hv]->Rebin(2);
        //--------------------------------------------
        // get maximum:
        //--------------------------------------------
        //--------------------------------------------
        double x1spe = 20.0;
        int xx = h4[hv]->FindBin(x1spe);
        //--------------------------------------------
        //--------------------------------------------
        // Find maximum of SPE:
        //--------------------------------------------
        max_yspe = 0.0;
        for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
            y = h4[hv]->GetBinContent(j);
            if(y>max_yspe){
                max_yspe = y;
                max_binspe = j;
            }
        }
        double max_xspe = h4[hv]->GetBinCenter(max_binspe);
        double distance = max_xspe - x1spe;
        
        //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
        //--------------------------------------------
        double error_spe=11.;
        double p = p0;
        // mean_spe = max_xspe;//first
        while(error_spe>5 && p<20.0){ //important!
            
            x1spe = max_xspe - (p*distance);
            x2spe = max_xspe + (p*distance);
            
            //--------------------------------------------
            TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
            h4[hv]->Fit("g2","QER");
            mean_spe = g2->GetParameter(1);
            error_mean_spe = g2->GetParError(1);
            
            error_spe = abs(100*error_mean_spe/mean_spe);
            sigma_spe = g2->GetParameter(2);
            //--------------------------------------------
            //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
            p = p + 0.5;;
            
        } //end while
        //--------------------------------------------
        myfile << 4 << " " << SN[this_pmt+4] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
        //--------------------------------------------
        
        if(LED==1 || LED==3 || LED==4 || LED==6){
            // pmt 5:
            cc5->cd(hv+1);
            gPad->SetLogy();
            h5[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+5].c_str(),this_hv));
            h5[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
            h5[hv]->GetYaxis()->SetTitle("Entries");
            h5[hv]->GetXaxis()->SetTitleOffset(0.95);
            h5[hv]->GetYaxis()->SetTitleOffset(0.9);
            h5[hv]->GetXaxis()->SetTitleSize(0.04);
            h5[hv]->GetYaxis()->SetTitleSize(0.04);
            h5[hv]->GetXaxis()->SetLabelSize(0.04);
            h5[hv]->GetYaxis()->SetLabelSize(0.04);
            h5[hv]->SetLineWidth(2);
            h5[hv]->SetLineColor(1);
            h5[hv]->Draw("");
            //h5[hv]->Rebin(2);
            //--------------------------------------------
            // get maximum:
            //--------------------------------------------
            //--------------------------------------------
            double x1spe = 20.0;
            int xx = h5[hv]->FindBin(x1spe);
            //--------------------------------------------
            //--------------------------------------------
            // Find maximum of SPE:
            //--------------------------------------------
            max_yspe = 0.0;
            for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
                y = h5[hv]->GetBinContent(j);
                if(y>max_yspe){
                    max_yspe = y;
                    max_binspe = j;
                }
            }
            double max_xspe = h5[hv]->GetBinCenter(max_binspe);
            double distance = max_xspe - x1spe;
            
            //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
            //--------------------------------------------
            double error_spe=11.;
            double p = p0;
            // mean_spe = max_xspe;//first
            while(error_spe>5 && p<20.0){ //important!
                
                x1spe = max_xspe - (p*distance);
                x2spe = max_xspe + (p*distance);
                
                //--------------------------------------------
                TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
                h5[hv]->Fit("g2","QER");
                mean_spe = g2->GetParameter(1);
                error_mean_spe = g2->GetParError(1);
                
                error_spe = abs(100*error_mean_spe/mean_spe);
                sigma_spe = g2->GetParameter(2);
                //--------------------------------------------
                //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
                p = p + 0.5;
                
            } //end while
            //--------------------------------------------
            myfile << 5 << " " << SN[this_pmt+5] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
            //--------------------------------------------
            
        }//end if LED==1,4,3,6 (6 PMTs)
        
        
        if(LED==3 || LED==6){
            // pmt 6:
            cc6->cd(hv+1);
            gPad->SetLogy();
            h6[hv]->SetTitle(Form("%s, %i V",SN[this_pmt+6].c_str(),this_hv));
            h6[hv]->GetXaxis()->SetTitle("Max. amplitude (ADC)");
            h6[hv]->GetYaxis()->SetTitle("Entries");
            h6[hv]->GetXaxis()->SetTitleOffset(0.95);
            h6[hv]->GetYaxis()->SetTitleOffset(0.9);
            h6[hv]->GetXaxis()->SetTitleSize(0.04);
            h6[hv]->GetYaxis()->SetTitleSize(0.04);
            h6[hv]->GetXaxis()->SetLabelSize(0.04);
            h6[hv]->GetYaxis()->SetLabelSize(0.04);
            h[hv]->SetLineWidth(2);
            h6[hv]->SetLineColor(1);
            h6[hv]->Draw("");
            //h6[hv]->Rebin(2);
            //--------------------------------------------
            // get maximum:
            //--------------------------------------------
            //--------------------------------------------
            double x1spe = 20.0;
            int xx = h6[hv]->FindBin(x1spe);
            //--------------------------------------------
            //--------------------------------------------
            // Find maximum of SPE:
            //--------------------------------------------
            max_yspe = 0.0;
            for(int j=xx;j<nbins;j++){//starts in ... xx -> Nbins
                y = h6[hv]->GetBinContent(j);
                if(y>max_yspe){
                    max_yspe = y;
                    max_binspe = j;
                }
            }
            double max_xspe = h6[hv]->GetBinCenter(max_binspe);
            double distance = max_xspe - x1spe;
            
            //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
            //--------------------------------------------
            double error_spe=11.;
            double p = p0;
            // mean_spe = max_xspe;//first
            while(error_spe>5 && p<20.0){ //important!
                
                x1spe = max_xspe - (p*distance);
                x2spe = max_xspe + (p*distance);
                
                //--------------------------------------------
                TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
                h6[hv]->Fit("g2","QER");
                mean_spe = g2->GetParameter(1);
                error_mean_spe = g2->GetParError(1);
                
                error_spe = abs(100*error_mean_spe/mean_spe);
                sigma_spe = g2->GetParameter(2);
                //--------------------------------------------
                //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
                p = p + 0.5;
                
            } //end while
            //--------------------------------------------
            myfile << 6 << " " << SN[this_pmt+6] << " " << hv << " " << this_hv << " " << Form("%.0f",mean_spe) << " " << Form("%.0f",error_mean_spe)  << " " << Form("%.0f",sigma_spe) <<"\n";
            //--------------------------------------------
            
        }//end if LED==3,6 (7 PMTs)
        
        
        hv++;
        
        // cc->cd(i)->Modified();
        
        
        //   lets_pause();
        // system("PAUSE");
        
        total_pmts = total_pmts + Npmts;
        if(total_pmts==36) total_pmts = 0;

        

        // ------------------------------------------------------

//        if(LED==6){
//            count_hv++;
//            cout << "end of HV #0" << count_hv << endl;
//            LED=0;
//        }
        
        
       // cc->Update();


    } // end while input file (.root)
    
  //  cc->Update();
    //cc->Print("test.pdf");
    
    //rFile->Close(); //root file
    
    //gSystem->Exit(0);
    
    cc->Update();
    cc1->Update();
    cc2->Update();
    cc3->Update();
    cc4->Update();
    
    if(LED==1 || LED==4){
        cc5->Update();
    }
    
    else if(LED==3 || LED==6){
        cc6->Update();
    }
    
    

    
    myfile.close();
    
}
