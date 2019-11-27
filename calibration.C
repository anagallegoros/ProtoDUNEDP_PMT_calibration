//....................................................
// ProtoDUNE-DP PMT Calibration
//....................................................
// Ana Gallego Ros (ana.gallego@ciemat.es)
// First version: June 2019
//....................................................

//#include <fstream>
//#include "TString.h"
//#include "TFile.h"

#include "WF_integration.C"
#include "join_histograms.C"
#include "fit_2gaussians.C"
#include "G_vs_HV.C"
#include "G_vs_HV_several.C"
//#include "fit_2gaussians_amplitude.C"
#include "fit_2gaussians_topfibers.C"
#include "WF_integration_topfibers.C"
//#include "WF_integration_crosstalks.C"
#include "SPE_amplitude.C"

void lets_pause (){
    TTimer * timer = new TTimer("gSystem->ProcessEvents();", 50, kFALSE);
    timer->TurnOn();
    timer->Reset();
    std::cout << "q/Q to quit, other to continuee: ";
    char kkey;
    std::cin.get(kkey);
    if( kkey == 'q' || kkey == 'Q')             gSystem->Exit(0);
    timer->TurnOff();
    delete timer;
}

void calibration(){
    
    // inputs:----------------------------------
    string label = "runs_2021_2028";//"runs_1006_1013";
    const int nhvs = 8;
    
    // 950-956: 7 HVs
    // 1006-1013: 8 HVs
    // 1111-1117: 7 HVs
    // runs_642_661_top
    // runs_1293_1292_top
    // runs_1296_1295_top
    // runs_1307_1300_top
    // runs_1284_1291
    // runs_1432_1439_top
    //runs_1443_1450_top
    // runs_1637_1644
    //"runs_1367_1373";
    
    bool save_waveforms = false;
    bool all_the_events = true;
    int number_events = 10;
    
    bool Gain_vs_HV = true;
    bool Gain_Stability = false;
    bool top_fibers = false;
    
    bool info_190724 = false; // #run < 637
    bool info_190801 = false; // 637 < #run < ?
    bool info_190813 = true; // ? < #run < ?
    //------------------------------------------

    cout << " -------------------------------------------------" << endl;
    cout << "|                                                 |" << endl;
    cout << "|      ProtoDUNE Dual Phase PMT Calibration       |" << endl;
    cout << "|                                                 |" << endl;
    cout << " -------------------------------------------------" << endl;
    cout << " " << endl;
    cout << " > Calibration: " << label << endl;

    if(info_190724) string pmt_info_file = "PMT_info_190724";
    else if(info_190801) string pmt_info_file = "PMT_info_190801";
    else if(info_190813) string pmt_info_file = "PMT_info_190813";
    
    if(Gain_vs_HV){
        const int nruns = 6 * nhvs; //complete calibration (6 LEDs x n HVs)
    }
    else if(Gain_Stability || top_fibers){
        const int nruns = 6; //6 LEDs x 1 HVs
    }

    //------------------------------------------------------
    // 3-4 teps!
    //------------------------------------------------------
    
    // 1)
    WF_integration(label,pmt_info_file,save_waveforms,all_the_events,number_events,nruns,top_fibers);
    //WF_integration_topfibers(label,pmt_info_file,save_waveforms,all_the_events,number_events,nruns,top_fibers);
    
    // 2)
    // join_histograms(label);
    
    // 3)
    //fit_2gaussians(label,pmt_info_file,nhvs);
    //fit_2gaussians_topfibers(label,pmt_info_file,nhvs);

    //4)
     //if(Gain_vs_HV) G_vs_HV(label,nhvs);
    
    // Other studies:
    //SPE_amplitude(label,pmt_info_file,save_waveforms,all_the_events,number_events,nruns,top_fibers);
    //------------------------------------------------------
    
    cout << " " << endl;
    cout << " ------------------------------------------------------" << endl;
    cout << "   End of the PMT calibration analysis (" << label << ")!" << endl;
    cout << " ------------------------------------------------------" << endl;
    cout << " ** Outputs: **" << endl;
    cout << " WF/runxxxx_LEDx_WF.pdf" << endl;
    cout << " RESULTS/fits_PMT_ordering.pdf" << endl;
    cout << " RESULTS/fits_HV_ordering.pdf" << endl;
    cout << " RESULTS/G_vs_HV_curves.pdf" << endl;
    cout << " RESULTS/gain_results.txt" << endl;
    cout << " RESULTS/final_fit_results.txt" << endl;
    cout << " " << endl;

    //gSystem->Exit(0);
}

