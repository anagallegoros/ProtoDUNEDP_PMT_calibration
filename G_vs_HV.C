//....................................................
// ProtoDUNE-DP PMT Calibration
// Gain curves: gain vs high voltaje (exponential fit)
//....................................................
// Macro created by Ana Gallego Ros (ana.gallego@ciemat.es)
// First version: June 2019
//....................................................

void G_vs_HV(string label, const int nhvs) {
    
    gROOT->Reset();
    
    cout << " " << endl;
    cout << "****************************************************" << endl;
    cout << " STEP 4/4: Gain vs HV curves " << endl;
    cout << "****************************************************" << endl;
    cout << " " << endl;
    
    // Do not change! (in principle)
    const int npmts = 36;
    const int n = npmts * nhvs; //number of histograms (npmt x nhv)

    //--------------------------------------------
    //               fit results
    //--------------------------------------------
    ofstream myfile;
    myfile.open(Form("%s/RESULTS/final_fit_results_%s.txt",label.c_str(),label.c_str()));
//    ofstream myfile2;
//    myfile2.open(Form("%s/RESULTS/results_HV_1e7.txt",label.c_str()));
    
    double HV1[nhvs];
    double G1[nhvs];
    double e01[nhvs];
    double eG1[nhvs];
    
    int PMT[n];
    int IGNORE[n];
    double HV[n];
    double G[n];
    double eG[n];
    double e0[n];
    double sigmaG[n];
    int ADC[n];
    int LED[n];
    string SN[n];


    for(int k=0;k<n;k++){
        PMT[k] = 99;
        ADC[k] = 99;
        LED[k] = 99;
        HV[k] = .0;
        G[k] = .0;
        eG[k] = .0;
        sigmaG[k] = .0;
        IGNORE[k] = 99;
        e0[k] = .0;
    }

    //--------------------------------
    //       input file
    //--------------------------------
    ifstream inputFile(Form("%s/RESULTS/gain_results_%s.txt",label.c_str(),label.c_str())); //<-- change!
    string line;
    int i=0;
    int pmt, ign, adc, led;
    double hv, g, eg, sigmag;
    string sn;
    
   while(getline(inputFile, line)) {
       if (line[0] == '#')
          continue;
       istringstream fichero(line);
       fichero>>pmt>>sn>>adc>>led>>hv>>g>>eg>>sigmag>>ign;
       IGNORE[i] = ign;
       PMT[i] = pmt;
       SN[i] = sn;
       ADC[i] = adc;
       LED[i] = led;
       G[i] = g;
       eG[i] = eg;
       sigmaG[i] = sigmag;
       
       //cout<<sn<<" "<<g<<endl;
       
       if(IGNORE[i]==1) HV[i] = 0;
       else HV[i] = hv;
       
       i++;
   }
    
    //***************************
    //       Loop npmts
    //***************************

    for(int pm=0;pm<npmts;pm++){
        
        for(int ihv=0;ihv<nhvs;ihv++){
            int index = pm + (ihv*npmts);
            HV1[ihv] = HV[index];
            G1[ihv] = G[index];
            e01[ihv] = .0;
            eG1[ihv] = eG[index];
        }//end for hvs
        
        //--------------------------------
        //        canvas / graph
        //--------------------------------
        TCanvas *c1 = new TCanvas("c1","");
        double w = 700;
        double hh = 500;
        c1->SetCanvasSize(w,hh);
        c1->SetLogx();
        c1->SetLogy();
        //c1->SetGridx();
        c1->SetGridy();
        
        TGraphErrors *gr1 = new TGraphErrors(nhvs,HV1,G1,e01,eG1);
        
        TGaxis::SetMaxDigits(3);
        //......................................
        gr1->GetXaxis()->SetTitleOffset(1);
        gr1->GetYaxis()->SetTitleOffset(1);
        //gr1->GetXaxis()->SetTitleFont(132);
        //gr1->GetYaxis()->SetTitleFont(132);
        gr1->GetXaxis()->SetTitleSize(0.045);
        gr1->GetYaxis()->SetTitleSize(0.045);
        //......................................
        //gr1->GetXaxis()->SetLabelFont(132);
        //gr1->GetYaxis()->SetLabelFont(132);
        gr1->GetXaxis()->SetLabelSize(0.045);
        gr1->GetYaxis()->SetLabelSize(0.045);
        //......................................
        gr1->SetMarkerStyle(20);
        gr1->SetMarkerSize(1);
        gr1->SetLineWidth(1);
        gr1->SetLineStyle(1);
        gr1->SetMarkerColor(4);
        gr1->SetLineColor(4);
        
        double first_HV = HV1[0];
        double last_HV = HV1[nhvs-1];
        
        double first_G = 0.5*G1[0];
        double last_G = 2*G1[nhvs-1];
        
        
        if(first_HV<1){
            if(HV1[1]>1){
                first_HV = HV1[1];
                first_G = 0.5*G1[1];
            }
            else if(HV1[2]>1){
                first_HV = HV1[2];
                first_G = 0.5*G1[2];
            }
            else if(HV1[3]>1){
                first_HV = HV1[3];
                first_G = 0.5*G1[3];
            }
            else if(HV1[4]>1){
                first_HV = HV1[4];
                first_G = 0.5*G1[4];
            }
            else if(HV1[5]>1){
                first_HV = HV1[5];
                first_G = 0.5*G1[5];
            }
            else if(HV1[6]>1){
                first_HV = HV1[6];
                first_G = 0.5*G1[6];
            }
            else if(HV1[7]>1){
                first_HV = HV1[7];
                first_G = 0.5*G1[7];
            }
        }
            
            
            if(last_HV<1){
                if(HV1[nhvs-2]>1){
                    last_HV = HV1[nhvs-2];
                    last_G = 2*G1[nhvs-2];
                }
                else if(HV1[nhvs-3]>1){
                    last_HV = HV1[nhvs-3];
                    last_G = 2*G1[nhvs-3];
                }
                else if(HV1[nhvs-4]>1){
                    last_HV = HV1[nhvs-4];
                    last_G = 2*G1[nhvs-4];
                }
                else if(HV1[nhvs-5]>1){
                    last_HV = HV1[nhvs-5];
                    last_G = 2*G1[nhvs-5];
                }
                else if(HV1[nhvs-6]>1){
                    last_HV = HV1[nhvs-6];
                    last_G = 2*G1[nhvs-6];
                }

            }
            
        
        
            gr1->GetXaxis()->SetLimits(first_HV-50,last_HV+50);
        
        
        double xlow = first_HV - 100; //100 -   //<-- important!
        double xup = last_HV + 100; //100 +    //<-- important!
        
        gr1->SetMaximum(last_G);//4e10
        gr1->SetMinimum(first_G);
        
        gr1->SetTitle("");
        gr1->GetXaxis()->SetTitle("High voltage (V)");
        gr1->GetYaxis()->SetTitle("Gain");

        gr1->Draw("AP");
        c1->Update();
        
        //--------------------------------------------------------------------------
        //        Lines needed to have the value in Log Scale (x axis). START
        //--------------------------------------------------------------------------
        int    nbinsx = int(xup - xlow);
        int    step_normal     = 100;
        int    factor_large    = 1;
        //  int    factor_large    = ;
        double  fraction_normal = 0.015;
        double  fraction_large  = 0.035;
        double uxmin = c1->GetUxmin();
        double uxmax = c1->GetUxmax();
        double uymin = c1->GetUymin();
        double uymax = c1->GetUymax();
        //    // x-axis ticks
        // //----------------------------------------------------------------------------
        int ixlow      = int(xlow);
        int ixup       = int(xup);
        int step_large = factor_large * step_normal;
        gr1->GetXaxis()->SetNdivisions(0);
        double ymin = (c1->GetLogy()) ? pow(10, uymin) : uymin;
        double ymax = (c1->GetLogy()) ? pow(10, uymax) : uymax;
        double bottom_tick_normal = fraction_normal * (uymax - uymin);
        double bottom_tick_large  = fraction_large  * (uymax - uymin);
        double top_tick_normal = fraction_normal * (uymax - uymin);
        double top_tick_large  = fraction_large  * (uymax - uymin);
        if (c1->GetLogy()) {
            bottom_tick_normal = pow(10, uymin + bottom_tick_normal) -
            pow(10, uymin);
            bottom_tick_large  = pow(10, uymin + bottom_tick_large)  -
            pow(10, uymin);
            TLine tick;
            //   for (int i=xlow; i<=xup; i+=100) { //salen rayas fuera
            for (int i=xlow+100; i<=xup-100; i+=100) {
                double xx = i;
                tick.DrawLine(xx, ymin, xx, ymin + ((i-ixlow+50)%step_large == 0 ?
                                                    bottom_tick_large : bottom_tick_normal));
            }
            // x-axis labels
            //--------------------------------------------------------------------------
            double ylatex = ymin - bottom_tick_normal;
            if ((ixup-ixlow)%step_large >= step_normal) {
                TLatex* latex = new TLatex(xup, ylatex, Form("%.0f", xup));
                latex->SetTextAlign(   23);
                latex->SetTextFont (   42);
                latex->SetTextSize (0.045);
                latex->Draw("same");
            }
            for (int i=ixlow+100; i<ixup; i+=step_large) {
                double xx = i;
                TLatex* latex = new TLatex(xx, ylatex, Form("%.0f", xx));
                latex->SetTextAlign(   23);
                latex->SetTextFont (   42);
                latex->SetTextSize (0.045);
                latex->Draw("same");
            }
            }
        //--------------------------------------------------------------------------
        //        Lines needed to have the value in Log Scale (x axis). END
        //--------------------------------------------------------------------------

        //--------------------------------
        //          FIT
        //--------------------------------
        TF1 *myfit = new TF1("myfit","pow(10,[0])*pow(x,[1])", first_HV, last_HV);  //<-- RANGE!
        myfit->SetParName(0,"A");
        myfit->SetParName(1,"B");
        myfit->SetLineStyle(2);
        myfit->SetLineWidth(2);
        myfit->SetLineColor(4);
        gr1->Fit("myfit","REMQ"); //REM
        gStyle->SetOptFit(0);
        
        double A = myfit->GetParameter(0);
        double errorA = myfit->GetParError(0);
        double B = myfit->GetParameter(1);
        double errorB = myfit->GetParError(1);
        //->GetChisquare();
        
        //--------------------------------------------------
        //          HV for nominal gain
        //--------------------------------------------------
        //G=10^A*V^B
        double gain=1.e7; //<-- nominal gain
        double vv=pow(gain/(pow(10,A)),1/B);

        
        //--------------------------
        //       fit result
        //--------------------------
        double xl1=0.12, yl1=0.78, xl2=xl1+0.6, yl2=yl1+0.1;
        leg = new TLegend(xl1,yl1,xl2,yl2);
        leg->SetBorderSize(0.0);
        leg->SetTextSize(0.035);
        // 36 first lines. ok
        leg->SetHeader(Form("%s (ch#%i, LED#%i) - FIT: G=10^{%.3f #pm %.3f}HV^{%.3f #pm %.3f}",SN[pm].c_str(),ADC[pm],LED[pm],A,errorA,B,errorB));
        leg->Draw();
        
        //--------------------------
        //       Save in txt
        //--------------------------
        myfile << pm << " " << SN[pm].c_str() << " " << ADC[pm] << " " << LED[pm] << " " << Form("%.3f",A) << " " << Form("%.3f",errorA) << " " << Form("%.3f",B) << " " << Form("%.3f",errorB) << " " << Form("%.0f",vv) << "\n";
        
//        myfile <<  Form("%.3f",A) << " " << Form("%.3f",B) << " " ;
//        myfile2 <<  Form("%.0f",vv) << " " ;
        
        //--------------------------
        //       Save in PDF
        //--------------------------
        c1->Update();
       // lets_pause();
        
        if(pm == 0){ //first
            c1->Print(Form("%s/RESULTS/G_vs_HV_curves_%s.pdf(",label.c_str(),label.c_str(),"pdf"));
            delete c1;
        }
        else if(pm == npmts-1){ //last
            c1->Print(Form("%s/RESULTS/G_vs_HV_curves_%s.pdf)",label.c_str(),label.c_str(),"pdf"));
            delete c1;
        }
        else{ //rest
            c1->Print(Form("%s/RESULTS/G_vs_HV_curves_%s.pdf",label.c_str(),label.c_str(),"pdf"));
            delete c1;
        }
        
    }// end for pm

    myfile.close();
    //myfile2.close();

}
