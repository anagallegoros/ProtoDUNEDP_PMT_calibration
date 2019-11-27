//....................................................
//ProtoDUNE-DP PMT Calibration
//Two gaussians fit: 36 spectra / HV
//....................................................
//Ana Gallego (ana.gallego@ciemat.es)
//First version: June 2019
//....................................................

#include <fstream>
#include "TString.h"
#include "TFile.h"

void fit_2gaussians(string label, string pmt_info_file, const int nhvs){

   // gROOT->Reset();
    
    cout << " " << endl;
    cout << "****************************************************" << endl;
    cout << " STEP 3/4: Two gaussians fit " << endl;
    cout << "****************************************************" << endl;
    cout << " " << endl;
    
    // Do not change! (in principle)
    const int npmts = 36;
    const int n = npmts * nhvs; //number of histograms (npmt x nhv)
    int real_hv;
   // double real_hv;
    
    gStyle->SetOptStat(0);
    
    //--------------------------------------------
    //                FILES
    //--------------------------------------------
    ofstream myfile;
    myfile.open(Form("%s/RESULTS/gain_results_%s.txt",label.c_str(),label.c_str()));

    //--------------------------------
    //     PMT info file
    //--------------------------------
    ifstream inputFile(Form("%s.txt",pmt_info_file.c_str())); //<-- important! check
    cout << "Reading " << Form("%s.txt",pmt_info_file.c_str()) << endl;
    string line;
    int i=0;
    string SN[npmts];
    string sn;
    int ADC[npmts],HV1[npmts],HV2[npmts],HV3[npmts],HV4[npmts],LED[npmts];
    int led,adc,hv1,hv2,hv3,hv4;
    
    while(getline(inputFile, line)) {
        istringstream fichero(line);
        fichero>>sn>>led>>adc;//>>hv1>>hv2>>hv3>>hv4;
        SN[i] = sn;
//        HV1[i] = hv1;
//        HV2[i] = hv2;
//        HV3[i] = hv3;
//        HV4[i] = hv4;
        ADC[i] = adc;
        LED[i] = led;
        i++;
    }
    
    //--------------------------------------------
    //       LIMITS OF FITS & CANVAS
    //--------------------------------------------
    double x1ped,x2ped,x1spe,x2spe,x1,x2,x2max;
    double all_x2[n];
    // limits x axis:
    x1=-20e6;
    x2=200e6;
    x1ped=x1;
    double a[n],b[n],ccc[n],d[n],e[n],ff[n]; //limits to repeat the fits

    //--------------------------------------------
    //             HISTOGRAMS
    //--------------------------------------------
    TH1D *h[n]; //144
    char namee[100];
    char title[100];
    for (int i=0;i<n;i++) {
        sprintf(namee,"h_%i",i);
        sprintf(title,"title of h_%i",i);
        h[i] = new TH1D(namee,title,5000,x1,x2);
    }

    //--------------------------------------------
    //              RESULTS & FITS
    //--------------------------------------------
    double mean_ped,error_mean_ped,mean_spe,error_mean_spe,sigma_ped,sigma_spe;
    double G1[n], errorG1[n], sigmaG1[n];
    int IGNORE[n], SAT[n];

    for(int i=0;i<n;i++){
        G1[i]=.0;
        errorG1[i]=.0;
        SAT[i]=0;
        sigmaG1[i]=.0;
        IGNORE[i]=0;
    }
    
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
    int array_pmts[n];

    //******************************************
    //     First loop --> nhvs high voltages
    //******************************************
    int total1 = -1;
    int hv = 0;
    for(int ihv=0;ihv<nhvs;ihv++){ // 4 HVs
        hv++;
        cout<<"Starting HV #0" <<hv<<"..."<<endl;
        
        TFile *f = new TFile(Form("%s/FIT/HV_0%i.root",label.c_str(),hv),"READ"); // read histograms
        
        string outfile = Form("%s/FIT/HV_0%i_fits.root",label.c_str(),hv);  // <-- output histogram
        TFile *ofile = new TFile(outfile.c_str(),"RECREATE");
        
        //******************************************
        //     Second loop --> npmts PMTs
        //******************************************
        for(int i=0;i<npmts;i++){ // 36 histograms
        
            total1++;
            const int total = total1;
            
            string name = Form("h_%i",i);
            TH1D *h[total] = (TH1D*)f->Get(name.c_str());
            h[total]->SetName(name.c_str());
            
            array_pmts[total] = i;

            h[total]->Rebin(5);
            
            if(label=="runs_950_956"){
                if(SN[i]=="FC0005") h[total]->Rebin(2);
                if(SN[i]=="FA0147") h[total]->Rebin(2);
            }

            
            if(i==0){ //create a new canvas (4 histograms)
                //     if(i%4 == 0){ //create a new canvas (4 histograms)
                
                TCanvas *c = new TCanvas;
                double w = 1000;
                double hh = 1000;
                c->SetCanvasSize(w,hh);
                
                c->Divide(8,8);
                int jj = 0;
            }
            jj++;
            
            if(SN[i]=="FA0146") c->cd(2);
            if(SN[i]=="FA0136") c->cd(4);
            if(SN[i]=="FA0147") c->cd(5);
            if(SN[i]=="FA0113") c->cd(7);
            
            if(SN[i]=="FA0121") c->cd(9);
            if(SN[i]=="FA0115") c->cd(11);
            if(SN[i]=="FA0150") c->cd(14);
            if(SN[i]=="FA0156") c->cd(16);
            
            if(SN[i]=="FA0119") c->cd(18);
            if(SN[i]=="FA0106") c->cd(20);
            if(SN[i]=="FA0111") c->cd(21);
            if(SN[i]=="FA0133") c->cd(23);
            
            if(SN[i]=="FA0114") c->cd(25);
            if(SN[i]=="FA0148") c->cd(26);
            if(SN[i]=="FA0110") c->cd(27);
            if(SN[i]=="FA0132") c->cd(28);
            if(SN[i]=="FA0139") c->cd(29);
            if(SN[i]=="FA0107") c->cd(30);
            if(SN[i]=="FA0151") c->cd(31);
            if(SN[i]=="FA0137") c->cd(32);
            
            if(SN[i]=="FA0135") c->cd(33);
            if(SN[i]=="FA0124") c->cd(35);
            if(SN[i]=="FA0130") c->cd(36);
            if(SN[i]=="FA0104") c->cd(37);
            if(SN[i]=="FA0153") c->cd(38);
            if(SN[i]=="FA0134") c->cd(40);
            
            if(SN[i]=="FC0005") c->cd(42);
            if(SN[i]=="FA0155") c->cd(44);
            if(SN[i]=="FA0129") c->cd(45);
            if(SN[i]=="FA0112") c->cd(47);
            
            if(SN[i]=="FC0004") c->cd(49);
            if(SN[i]=="FA0149") c->cd(51);
            if(SN[i]=="FA0116") c->cd(54);
            if(SN[i]=="FA0157") c->cd(56);
            
            if(SN[i]=="FA0122") c->cd(60);
            if(SN[i]=="FA0105") c->cd(61);
            
            //c->cd(jj);
            gPad->SetLogy();
            gStyle->SetOptTitle(0);

                //--------------------------------------------
                h[total]->GetXaxis()->SetTitle("Q (|e|)");
                h[total]->GetYaxis()->SetTitle("Entries");
                h[total]->GetXaxis()->SetTitleOffset(0.95);
                h[total]->GetYaxis()->SetTitleOffset(0.9);
                h[total]->GetXaxis()->SetTitleSize(0.04);
                h[total]->GetYaxis()->SetTitleSize(0.04);
                h[total]->GetXaxis()->SetLabelSize(0.04);
                h[total]->GetYaxis()->SetLabelSize(0.04);
                Nbins = h[total]->GetXaxis()->GetNbins(); // number of bins

            //............................................
            //--------------------------------------------
            //          FIT (2 GAUSSIANS). START
            //--------------------------------------------
            //............................................
            
            //--------------------------------------------
            // Find maximum of histogram (max pedestal):
            //--------------------------------------------
            max_yped=0.0;
            for(int j=0;j<Nbins;j++){ // 0 -> Nbins
                y = h[total]->GetBinContent(j);
                if(y>max_yped){
                    max_yped=y;
                    max_binped=j;
                }
            }
            //--------------------------------------------
            // Find x2ped for fit:
            //--------------------------------------------
            //min_yped=1e10;
            for(int j=max_binped+5;j<Nbins;j++){ //starts in... max_binped+5 -> Nbins
                min_yped = h[total]->GetBinContent(j);
                if(h[total]->GetBinContent(j+1)>min_yped && h[total]->GetBinContent(j+2)>min_yped && h[total]->GetBinContent(j+3)>min_yped){ // when it starts to increase: break
                    x2ped = h[total]->GetBinCenter(j);
                    break;
                }
                min_yped = y;
            }
        
            if(label=="runs_950_956"){
                if(hv==7){
                    if(SN[i]=="FC0005"){
                        x1ped = -100e6;
                        x2ped = 100e6;
                    }
                }
            }
            
            a[total] = x1ped;
            b[total] = x2ped;
            //--------------------------------------------
            //   Pedestal fit
            //--------------------------------------------
            TF1 *g1 = new TF1("g1","gaus",x1ped,x2ped);
            h[total]->Fit("g1","QER");
            mean_ped = g1->GetParameter(1);
            error_mean_ped = g1->GetParError(1);
            sigma_ped = g1->GetParameter(2);
            
            //--------------------------------------------
            x1spe = x2ped + sigma_ped;//2*x2ped;//x2ped+7.5*sigma_ped;//1.8*x2ped;
            
            //x2ped+7.5*sigma_ped; //<-- change?
            int xx = h[total]->FindBin(x1spe);
            //--------------------------------------------
            
            //--------------------------------------------
            // Find maximum of SPE:
            //--------------------------------------------
            max_yspe = 0.0;
            for(int j=xx;j<Nbins;j++){//starts in ... xx -> Nbins
                y = h[total]->GetBinContent(j);
                if(y>max_yspe){
                    max_yspe = y;
                    max_binspe = j;
                }
            }
            
            double max_xspe = h[total]->GetBinCenter(max_binspe);
            double distance = max_xspe - x1spe;
            
            //x2spe = max_xspe + distance;
            //cout<<max_xspe<<" "<<distance<<" "<<x2spe<<endl;
            //--------------------------------------------
            
            double error_spe=11.;
            int p=1;
            // mean_spe = max_xspe;//first

            while(error_spe>5 && p<50){ //important!
                
                x1spe = max_xspe - (p*distance); // try different factors...?
                x2spe = max_xspe + (p*distance);

                //*******************
                // PARTICULAR CASES:
                //*******************
                
                // *** old ones copied in txt ***
                
                 if(label=="runs_2021_2028"){
                     if(SN[i]=="FA0129" && hv==3){
                         x1spe = 20e6;
                         x2spe = 120e6;
                     }
                     if(SN[i]=="FA0129" && hv==4){
                         x1spe = 50e6;
                         x2spe = 180e6;
                     }

                     if(SN[i]=="FA0121" && hv==5){
                         x1spe = 10e6;
                         x2spe = 400e6;
                     }
                     if(SN[i]=="FA0121" && hv==6){
                         x1spe = 20e6;
                         x2spe = 80e6;
                     }
                     if(SN[i]=="FA0136" && hv==7){
                         x1spe = 200e6;
                         x2spe = 640e6;
                     }
                     if(SN[i]=="FA0149" && hv==4){
                         x1spe = 50e6;
                         x2spe = 150e6;
                     }
                     if(SN[i]=="FA0149" && hv==5){
                         x1spe = 210e6;
                         x2spe = 50e6;
                     }
                     if(SN[i]=="FA0114" && hv==3){
                         x1spe = 50e6;
                         x2spe = 300e6;
                     }
                     if(SN[i]=="FA0114" && hv==4){
                         x1spe = 50e6;
                         x2spe = 410e6;
                     }
                     if(SN[i]=="FA0135" && hv==2){
                         x1spe = 8e6;
                         x2spe = 30e6;
                     }
                     if(SN[i]=="FA0110" && hv==4){
                         x1spe = 100e6;
                         x2spe = 500e6;
                     }
                     if(SN[i]=="FA0110" && hv==5){
                         x1spe = 100e6;
                         x2spe = 500e6;
                     }

                     if(SN[i]=="FA0130" && hv==4){
                         x1spe = 50e6;
                         x2spe = 180e6;
                     }
                     if(SN[i]=="FA0130" && hv==5){
                         x1spe = 100e6;
                         x2spe = 250e6;
                     }
                     if(SN[i]=="FA0130" && hv==6){
                         x1spe = 120e6;
                         x2spe = 400e6;
                     }
                     if(SN[i]=="FA0130" && hv==7){
                         x1spe = 400e6;
                         x2spe = 900e6;
                     }

                     
                     
                 }//end label 2021_2028
                
                ccc[total] = x1spe;
                d[total] = x2spe;
                //--------------------------------------------
                TF1 *g2 = new TF1("g2","gaus",x1spe,x2spe);
                h[total]->Fit("g2","QER");
                mean_spe = g2->GetParameter(1);
                error_mean_spe = g2->GetParError(1);
                
                error_spe = abs(100*error_mean_spe/mean_spe);
                sigma_spe = g2->GetParameter(2);
                //--------------------------------------------
                //cout << i << " " << hv << " " << error_spe <<" "<< p <<endl;
                p++;
                
                //*******************
                // BREAKS SPE FIT
                //*******************
            
                // *** old ones copied in txt ***

            } //end while

            // cout<<x1ped<<" "<<x2ped<<" "<<x1spe<<" "<<x2spe<<endl;
            
            //--------------------------------------------
            // Find the end of histogram:
            //--------------------------------------------
            double end;
            int ee = h[total]->FindBin(x2spe);
            for(int j=ee;j<Nbins;j++){ //starts in... ee -> Nbins
                end = h[total]->GetBinContent(j);
                // cout<<end<<endl;
                if(end<1 && h[total]->GetBinContent(j+1)<1 && h[total]->GetBinContent(j+2)<1 && h[total]->GetBinContent(j+3)<1 && h[total]->GetBinContent(j+4)<1 && h[total]->GetBinContent(j+5)<1 && h[total]->GetBinContent(j+6)<1 && h[total]->GetBinContent(j+7)<1 && h[total]->GetBinContent(j+8)<1 && h[total]->GetBinContent(j+9)<1 && h[total]->GetBinContent(j+10)<1 && h[total]->GetBinContent(j+11)<1){
                    // cout<<"end "<<end<<endl;
                    int limit=j;
                    break;
                }
            }
            //############################################
            // (end histo) limits x axis --> PARTICULAR CASES:
            //############################################
            x2 = h[total]->GetBinCenter(limit);
            
            // *** old ones copied in txt ***

            all_x2[total] = x2;
            
            //--------------------------------------------
            //  cout<<"x2 "<<x2<<endl;

            //############################################
            // IGNORE PMTs
            //############################################
            
            // *** old ones copied in txt ***

            if(label=="runs_2021_2028"){

                if(SN[i]=="FA0147" && hv>4) IGNORE[total] = 1;
                if(SN[i]=="FA0111" && hv<3) IGNORE[total] = 1;
                if(SN[i]=="FA0111" && hv==6) IGNORE[total] = 1;
                if(SN[i]=="FA0111" && hv==7) IGNORE[total] = 1;
                if(SN[i]=="FA0150" && hv>7) IGNORE[total] = 1;
                if(SN[i]=="FA0150" && hv==4) IGNORE[total] = 1;
                if(SN[i]=="FA0113" && hv>6) IGNORE[total] = 1;
                if(SN[i]=="FA0113" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FA0133" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0129" && hv>4) IGNORE[total] = 1;
                if(SN[i]=="FA0116" && hv<4) IGNORE[total] = 1;
                if(SN[i]=="FA0105" && hv<3) IGNORE[total] = 1;
                if(SN[i]=="FA0105" && hv>6) IGNORE[total] = 1;
                if(SN[i]=="FA0112" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0139" && hv==1) IGNORE[total] = 1;
                if(SN[i]=="FA0139" && hv>5) IGNORE[total] = 1;
                if(SN[i]=="FA0153" && hv>4) IGNORE[total] = 1;
                if(SN[i]=="FA0151" && hv>6) IGNORE[total] = 1;
                if(SN[i]=="FA0151" && hv==4) IGNORE[total] = 1;
                if(SN[i]=="FA0137" && hv<5) IGNORE[total] = 1;
                if(SN[i]=="FA0134" && hv==7) IGNORE[total] = 1;
                //if(SN[i]=="FA0146" && hv==) IGNORE[total] = 1;
                if(SN[i]=="FA0121" && hv<5) IGNORE[total] = 1;
                if(SN[i]=="FA0121" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0119" && hv<4) IGNORE[total] = 1;
                if(SN[i]=="FA0119" && hv==6) IGNORE[total] = 1;
                if(SN[i]=="FA0115" && hv<3) IGNORE[total] = 1;
                if(SN[i]=="FA0115" && hv==6) IGNORE[total] = 1;
                if(SN[i]=="FA0115" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0136" && hv<3) IGNORE[total] = 1;
                if(SN[i]=="FA0136" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FA0136" && hv==6) IGNORE[total] = 1;
                if(SN[i]=="FA0136" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0106" && hv<3) IGNORE[total] = 1;
                if(SN[i]=="FA0106" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FC0005" && hv>4) IGNORE[total] = 1;
                if(SN[i]=="FC0004" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FC0004" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0155" && hv==4) IGNORE[total] = 1;
                if(SN[i]=="FA0155" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FA0155" && hv==6) IGNORE[total] = 1;
                if(SN[i]=="FA0149" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0122" && hv==4) IGNORE[total] = 1;
                if(SN[i]=="FA0122" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FA0122" && hv==8) IGNORE[total] = 1;
                if(SN[i]=="FA0114" && hv>5) IGNORE[total] = 1;
                if(SN[i]=="FA0148" && hv==3) IGNORE[total] = 1;
                if(SN[i]=="FA0148" && hv==5) IGNORE[total] = 1;
                if(SN[i]=="FA0135" && hv<2) IGNORE[total] = 1;
                if(SN[i]=="FA0135" && hv>3) IGNORE[total] = 1;
                if(SN[i]=="FA0110" && hv>5) IGNORE[total] = 1;
                if(SN[i]=="FA0124" && hv<7) IGNORE[total] = 1;
                if(SN[i]=="FA0132" && hv>5) IGNORE[total] = 1;
                if(SN[i]=="FA0132" && hv==3) IGNORE[total] = 1;
                if(SN[i]=="FA0130" && hv<4) IGNORE[total] = 1;
                if(SN[i]=="FA0130" && hv==7) IGNORE[total] = 1;
                
            }//end if label 2021_2028
            
            //############################################
            
            //............................................
            //--------------------------------------------
            //          FIT (2 GAUSSIANS). END!
            //--------------------------------------------
            //............................................
            
            
            //--------------------------------------------
            //              Draw + legend
            //--------------------------------------------

            h[total]->SetLineWidth(2);
           // h[total]->SetLineColor(1);
            h[total]->Draw();
            
            //TPB PMTs:
            if(SN[i]=="FA0114") h[total]->SetLineColor(4);
            else if(SN[i]=="FA0110") h[total]->SetLineColor(4);
            else if(SN[i]=="FA0132") h[total]->SetLineColor(4);
            else if(SN[i]=="FA0139") h[total]->SetLineColor(4);
            else if(SN[i]=="FA0107") h[total]->SetLineColor(4);
            else if(SN[i]=="FA0137") h[total]->SetLineColor(4);
            else h[total]->SetLineColor(1);
            
            // Old runs:
            /*
            if(hv==1) real_hv = HV1[i];
            else if(hv==2) real_hv = HV2[i];
            else if(hv==3) real_hv = HV3[i];
            else if(hv==4) real_hv = HV4[i];
            if(nhvs>4){
                if(hv==5) real_hv = HV3[i];
                else if(hv==6) real_hv = HV4[i];
            }
             */
            
//            if(hv==1) real_hv = 1100;
//            else if(hv==2) real_hv = 1175;
//            else if(hv==3) real_hv = 1250;
//            else if(hv==4) real_hv = 1325;
//            if(nhvs>4){
//                if(hv==5) real_hv = 1400;
//                else if(hv==6) real_hv = 1475;
//            }

            
            
            if(hv==1) real_hv = 1400;
            else if(hv==2) real_hv = 1450;
            else if(hv==3) real_hv = 1500;
            else if(hv==4) real_hv = 1550;
            else if(hv==5) real_hv = 1600;
            else if(hv==6) real_hv = 1650;
            else if(hv==7) real_hv = 1700;
            else if(hv==8) real_hv = 1750;

            //TOP FIBERS:

            /*
            if(hv==1) real_hv = 15;
            else if(hv==2) real_hv = 16;
            else if(hv==3) real_hv = 17;
            else if(hv==4) real_hv = 18;
            else if(hv==5) real_hv = 19.5;
            */
            
          //  cout<<i<<" "<<SN[i]<<" "<<real_hv<<" "<<ADC[i]<<" "<<LED[i]<<endl;
            
            if(nhvs==1){
                string nameee = Form("%s, ADC ch #%i, LED #%i",SN[i].c_str(),ADC[i],LED[i]);
            }

            else{
               // string nameee = Form("%s, ADC ch #%i, LED @ %.1f V",SN[i].c_str(),ADC[i],real_hv);
                string nameee = Form("%s @ %i V, ADC ch #%i, LED #%i",SN[i].c_str(),real_hv,ADC[i],LED[i]);
            }
            
            legg = new TLegend(0.1,0.9,0.9,1.0);
            legg->SetBorderSize(0.0);
            legg->SetTextSize(0.045);
            legg->SetHeader(nameee.c_str());
            legg->Draw();
            
            if(IGNORE[total]==0){
                //pedestal fit:
                g1->SetLineColor(kRed);
                g1->Draw("SAME");
                //SPE fit:
                g2->SetLineColor(kGreen);
                g2->Draw("SAME");
            }
            
            //g1->Print("all");
            //g2->Print("all");

            //Save! <--------------------------------------
            h[total]->Write(name.c_str());
            //---------------------------------------------

            //--------------------------------------------
            //                 GAIN
            //--------------------------------------------
            G1[total] = abs(mean_spe)-abs(mean_ped);
            errorG1[total] = sqrt((error_mean_spe*error_mean_spe)+(error_mean_ped*error_mean_ped));
            sigmaG1[total] = sqrt((sigma_spe*sigma_spe)+(sigma_ped*sigma_ped));
            // cout << G1[total] << " " << errorG1[total] << " " << sigmaG1[total] << endl;
            
            // x axis:
            e[total] = x1;
            ff[total] = x2;
            
            h[total]->SetAxisRange(x1, x2, "X");
            
            // Gain result from the fit:
            double gain = G1[total];
            
            if(IGNORE[total]==0){
                Double_t xl1=.6, yl1=0.8, xl2=xl1+.2, yl2=yl1+.07;
                TPaveText *pt = new TPaveText(xl1,yl1,xl2,yl2,"NDC");
                pt->SetTextFont(42);
                pt->SetTextSize(0.05);
                pt->SetTextColor(kRed);
                pt->SetFillColor(0);
                pt->SetBorderSize(0.0);
                pt->SetTextAlign(12);
                pt->AddText(Form("Gain = %.2e",gain));
                pt->Draw();
            }

            //--------------------------------------------
            //          Save PDF (Same HV together)
            //--------------------------------------------
            c->Update();
        
            if(total == 35){ //first
                c->Print(Form("%s/RESULTS/fits_HV_ordering_%s.pdf(",label.c_str(),label.c_str(),"pdf"));
                delete c;
            }
            else if(total == n-1){ //last
                c->Print(Form("%s/RESULTS/fits_HV_ordering_%s.pdf)",label.c_str(),label.c_str(),"pdf"));
                delete c;
            }
            else if((total+1)%36 == 0){ //rest
                c->Print(Form("%s/RESULTS/fits_HV_ordering_%s.pdf",label.c_str(),label.c_str(),"pdf"));
                delete c;
            }

            

            //--------------------------------------------
            //       Save results, same HV together
            //--------------------------------------------
            myfile << i << " " << SN[i] << " " << ADC[i] << " " << LED[i] << " " << real_hv << " " << G1[total] << " " << errorG1[total] << " " << sigmaG1[total] <<" " << IGNORE[total] << "\n";
            
         //  cout << "PMT: " << i << "; " << hv << "V; G: " << G1[total] << "; errorG: " << errorG1[total] << "; errorG: " << sigmaG1[total] <<"; SAT? " << SAT[total] << endl;
            
        }// end for i 36 pmts
        
        ofile->Close();
        f->Close();

    }//end for files hv

    myfile.close();

    //change?
    TFile *data1 = new TFile(Form("%s/FIT/HV_01_fits.root",label.c_str()),"READ");
    TFile *data2 = new TFile(Form("%s/FIT/HV_02_fits.root",label.c_str()),"READ");
    TFile *data3 = new TFile(Form("%s/FIT/HV_03_fits.root",label.c_str()),"READ");
    TFile *data4 = new TFile(Form("%s/FIT/HV_04_fits.root",label.c_str()),"READ");
    TFile *data5 = new TFile(Form("%s/FIT/HV_05_fits.root",label.c_str()),"READ");
    TFile *data6 = new TFile(Form("%s/FIT/HV_06_fits.root",label.c_str()),"READ");
    TFile *data7 = new TFile(Form("%s/FIT/HV_07_fits.root",label.c_str()),"READ");
    TFile *data8 = new TFile(Form("%s/FIT/HV_08_fits.root",label.c_str()),"READ");  

    
    //--------------------------------------------
    //       Save results: same PMT, all HVs
    //--------------------------------------------
    int tot=0;
    for(int l=0;l<npmts;l++){// pmts
        TCanvas *cc = new TCanvas;
        if(nhvs==1){
            //cc->Divide(1,1);
        }
        else if(nhvs<5){
            cc->Divide(2,2);
        }
        else if(nhvs<7){
            cc->Divide(3,2);
        }
        
        else if(nhvs<9){ // 7 HVs
            //cout<<"ok"<<endl;
            cc->Divide(4,2);
        }

        
        int jj = 0;
        int this_hv;
    
        
        for(int iout=0;iout<nhvs;iout++){ // hvs
            jj++;
            tot++;
            this_hv = iout+1;
            
            int ij = l + (iout*npmts);

           // TFile *data = new TFile(Form("%s/FIT/HV_0%i_fits.root",label.c_str(),this_hv),"READ"); // histograms

            if(nhvs==1){
                //cc->Divide(1,1);
            }
            else{
                cc->cd(jj);
            }
            
            gPad->SetLogy();
            
            string index = Form("h_%i",l);
         
            if(this_hv==1){
                TH1D* hist = (TH1D*)data1->Get(index.c_str());
                real_hv = 1400;}
            else if(this_hv==2){
                TH1D* hist = (TH1D*)data2->Get(index.c_str());
                real_hv = 1450;}
            else if(this_hv==3){
                TH1D* hist = (TH1D*)data3->Get(index.c_str());
                real_hv = 1500;}
            else if(this_hv==4){
                TH1D* hist = (TH1D*)data4->Get(index.c_str());
                real_hv = 1550;}
            else if(this_hv==5){
                TH1D* hist = (TH1D*)data5->Get(index.c_str());
                real_hv = 1600;}
            else if(this_hv==6){
                TH1D* hist = (TH1D*)data6->Get(index.c_str());
                real_hv = 1650;}
            else if(this_hv==7){
                TH1D* hist = (TH1D*)data7->Get(index.c_str());
                real_hv = 1700;}
            else if(this_hv==8){
                TH1D* hist = (TH1D*)data8->Get(index.c_str());
                real_hv = 1750;}
            

            //TH1D* hist = (TH1D*)data->Get(index.c_str());
            
            
            //--------------------------------------------
            // X axis:
            //--------------------------------------------
            hist->SetAxisRange(e[ij],ff[ij], "X");
            hist->Draw("HIST");
            hist->SetLineWidth(2);
            hist->SetLineColor(1);
            
            if(IGNORE[ij]==0){
                
                //--------------------------------------------
                // Pedestal fit:
                //--------------------------------------------
                TF1 *g1 = new TF1("g1","gaus",a[ij],b[ij]);
                hist->Fit("g1","QER");
                mean_ped = g1->GetParameter(1);
                error_mean_ped = g1->GetParError(1);
                sigma_ped = g1->GetParameter(2);
                
                //--------------------------------------------
                // SPE fit:
                //--------------------------------------------
                TF1 *g2 = new TF1("g2","gaus",ccc[ij],d[ij]);
                hist->Fit("g2","QER");
                mean_spe = g2->GetParameter(1);
                error_mean_spe = g2->GetParError(1);
                //--------------------------------------------

                //pedestal fit:
                g1->SetLineColor(kRed);
                g1->Draw("SAME");
                //SPE fit:
                g2->SetLineColor(kGreen);
                g2->Draw("SAME");
            }
            
            // real HVs:
            /*
            if(this_hv==1) real_hv = HV1[l];
            else if(this_hv==2) real_hv = HV2[l];
            else if(this_hv==3) real_hv = HV3[l];
            else real_hv = HV4[l];
             */
            /*
            if(this_hv==1) real_hv = 1100;
            else if(this_hv==2) real_hv = 1175;
            else if(this_hv==3) real_hv = 1250;
            else if(this_hv==4) real_hv = 1325;
            else if(this_hv==5) real_hv = 1400;
            else real_hv = 1475;
             */

            string name3;
            
           // cout<<SN[l]<<" "<<real_hv<<" "<<ADC[l]<<" "<<LED[l]<<endl;
            
            if(nhvs==1) name3 = Form("%s, ADC ch #%i, LED #%i",SN[l].c_str(),ADC[l],LED[l]);
            else name3 = Form("%s @ %i V, ADC ch #%i, LED #%i",SN[l].c_str(),real_hv,ADC[l],LED[l]);
            //else name3 = Form("%s, ADC ch #%i, LED @ %.1f V",SN[l].c_str(),ADC[l],real_hv);
            //cout<<nhvs<<" "<<this_hv<<" "<<l<<" "<<name3<<endl;
            
            leg = new TLegend(0.1,0.9,0.9,1.0);
            leg->SetBorderSize(0.0);
            leg->SetTextSize(0.045);
            leg->SetHeader(name3.c_str());
            leg->Draw();

            // Gain result from the fit:

            double gain = G1[ij];

            if(IGNORE[ij]==0){
                if(nhvs<5){
                    Double_t xl1=.6, yl1=0.8, xl2=xl1+.2, yl2=yl1+.07;
                }
                else if(nhvs<7){
                    Double_t xl1=.5, yl1=0.8, xl2=xl1+.2, yl2=yl1+.07;
                }
                else{
                    Double_t xl1=.4, yl1=0.8, xl2=xl1+.2, yl2=yl1+.07;
                }
                TPaveText *pt = new TPaveText(xl1,yl1,xl2,yl2,"NDC");
                pt->SetTextFont(42);
                pt->SetTextSize(0.05);
                pt->SetTextColor(kRed);
                pt->SetFillColor(0);
                pt->SetBorderSize(0.0);
                pt->SetTextAlign(12);
                pt->AddText(Form("Gain = %.2e",gain));
                pt->Draw();
            }

            
        }//end for hvs

        cc->Update();
        if(l == 0){ //first
            cc->Print(Form("%s/RESULTS/fits_PMT_ordering_%s.pdf(",label.c_str(),label.c_str(),"pdf"));
        }
        else if(l == npmts-1){ //last
            cc->Print(Form("%s/RESULTS/fits_PMT_ordering_%s.pdf)",label.c_str(),label.c_str(),"pdf"));
        }
        else{ //rest
            cc->Print(Form("%s/RESULTS/fits_PMT_ordering_%s.pdf",label.c_str(),label.c_str(),"pdf"));
            
        }

        delete cc;
        
    }//end for l pmts
    
    //change?
    data1->Close();
    data2->Close();
    data3->Close();
    data4->Close();
    data5->Close();
    data6->Close();
    data7->Close();
    
    
    //----------------------
    //    end save pdf/PMT
    //----------------------

    //gSystem->Exit(0);

}
