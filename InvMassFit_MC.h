// InvMassFit_MC.h
// David Grund, Feb 27, 2022
// To perform fit of the invariant mass distribution of MC data

// Main functions
void InvMassFit_MC_DoFit(Int_t opt);
// Support functions
void InvMassFit_MC_DrawCorrMatrix(TCanvas *cCorrMat, RooFitResult* fResFit);
void InvMassFit_MC_SetCanvas(TCanvas *c, Bool_t bLogScale);
void InvMassFit_MC_PrepareData();

void InvMassFit_MC_main(Int_t optMain){

    if(optMain == 0){
        InvMassFit_MC_PrepareData();
        // inc
        InvMassFit_MC_DoFit(0);
        // coh
        InvMassFit_MC_DoFit(1);
        // all
        InvMassFit_MC_DoFit(2);
        // allbins
        InvMassFit_MC_DoFit(3);
    }

    if(optMain == 1){
        // in bins
        InvMassFit_MC_DoFit(4);
        InvMassFit_MC_DoFit(5);
        InvMassFit_MC_DoFit(6);
        InvMassFit_MC_DoFit(7);
        if(nPtBins == 5) InvMassFit_MC_DoFit(8);
    }

    Printf("Done.");

    return;
}

void InvMassFit_MC_DoFit(Int_t opt){
    // Fit the invariant mass distribution using Double-sided CB function
    // Peak corresponding to psi(2s) excluded

    // Cuts:
    char fStrReduce[120];
    Double_t fPtCut     = -999;
    Double_t fPtCutLow  = -999;
    Double_t fPtCutUpp  = -999;
    Double_t fYCut      = 0.80;
    Double_t fMCutLow   = 2.90;
    Double_t fMCutUpp   = 3.30;

    switch(opt){
        case 0: // 'inc': incoherent-enriched sample
            fPtCut = 0.20;
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fM>%f && fM<%f",fYCut,fPtCut,fMCutLow,fMCutUpp);
            break;
        case 1: // 'coh': coherent-enriched sample
            fPtCut = 0.20;
            sprintf(fStrReduce,"abs(fY)<%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCut,fMCutLow,fMCutUpp);
            break;
        case 2: // 'all': total sample (pT < 2.0 GeV/c)
            fPtCut = 2.00;
            sprintf(fStrReduce,"abs(fY)<%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCut,fMCutLow,fMCutUpp);
            break;
        case 3: // 'allbins': sample with pT from 0.2 to 1 GeV/c 
            fPtCutLow = 0.20;
            fPtCutUpp = 1.00;
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
        case 4: // pT bin 1
            fPtCutLow = ptBoundaries[0];
            fPtCutUpp = ptBoundaries[1];
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
        case 5: // pT bin 2
            fPtCutLow = ptBoundaries[1];
            fPtCutUpp = ptBoundaries[2];
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
        case 6: // pT bin 3
            fPtCutLow = ptBoundaries[2];
            fPtCutUpp = ptBoundaries[3];
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
        case 7: // pT bin 4
            fPtCutLow = ptBoundaries[3];
            fPtCutUpp = ptBoundaries[4];
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
        case 8: // pT bin 5
            fPtCutLow = ptBoundaries[4];
            fPtCutUpp = ptBoundaries[5];
            sprintf(fStrReduce,"abs(fY)<%f && fPt>%f && fPt<%f && fM>%f && fM<%f",fYCut,fPtCutLow,fPtCutUpp,fMCutLow,fMCutUpp);
            break;
    }

    // Binning:
    Int_t nBins = 100; // so that each bin between 2.90 and 3.30 GeV is 4 MeV wide
    RooBinning binM(nBins,fMCutLow,fMCutUpp);
    Double_t BinSizeDouble = (fMCutUpp - fMCutLow) * 1000 / nBins; // in MeV
    BinSizeDouble = BinSizeDouble + 0.5;
    // https://stackoverflow.com/questions/9695329/c-how-to-round-a-double-to-an-int
    Int_t BinSize = (Int_t)BinSizeDouble;

    Printf("\n");
    Printf("*** Bin size (double): %.3f ***", BinSizeDouble);
    Printf("*** Bin size (int): %i ***\n", BinSize);

    // Roofit variables
    RooRealVar fM("fM","fM",fMCutLow,fMCutUpp);
    RooRealVar fPt("fPt","fPt",0,10.);
    RooRealVar fY("fY","fY",-0.8,0.8);

    //fM.setBinning(binM);

    // Get the data trees
    TFile *f_in = new TFile("Trees/" + str_subfolder + "InvMassFit_MC/InvMassFit_MC.root"); 
    TTree *t_in = NULL;
    if(opt == 0 || opt == 3 || opt == 4 || opt == 5 || opt == 6 || opt == 7 || opt == 8){
        f_in->GetObject("tIncEnrSample",t_in);
    } else if(opt == 1){
        f_in->GetObject("tCohEnrSample",t_in);
    } else if(opt == 2){
        f_in->GetObject("tMixedSample",t_in);
    }

    RooDataSet *fDataIn = new RooDataSet("fDataIn", "fDataIn", RooArgSet(fM,fY,fPt), Import(*t_in));
    RooAbsData* fDataSet = fDataIn->reduce(fStrReduce);

    // Print the number of entries in the dataset
    Int_t nEvents = fDataSet->numEntries();
    Printf("*** Number of events in the dataset: %i ***\n", nEvents);

    // Roofit variables
    RooRealVar norm_L("norm_L","N_{L}(J/#psi)",nEvents,0,1e06);
    RooRealVar norm_R("norm_R","N_{R}(J/#psi)",nEvents,0,1e06);
    RooRealVar N("N","N(J/#psi)",nEvents,0,1e06);

    RooRealVar mean_L("m","m_{J/#psi}",3.097,3.0,3.2);
    RooRealVar sigma_L("sig","#sigma_{J/#psi}",0.0186,0.01,0.2);
    RooRealVar alpha_L("#alpha_{L}","alpha_{L}",1.,0.0,20.0);
    RooRealVar n_L("n_{L}","n_{L}",1.,0,30);

    RooGenericPdf mean_R("mean_R","m_{J/#psi}","m",RooArgSet(mean_L));
    RooGenericPdf sigma_R("sigma_R","#sigma_{J/#psi}","sig",RooArgSet(sigma_L));
    RooRealVar alpha_R("#alpha_{R}","alpha_{R}",-1.,-20.0,0.0); 
    RooRealVar n_R("n_{R}","n_{R}",8.,0,30);

    RooCBShape CB_left("CB_left","CB_left",fM,mean_L,sigma_L,alpha_L,n_L);
    RooCBShape CB_right("CB_right","CB_right",fM,mean_R,sigma_R,alpha_R,n_R);
    RooRealVar frac("frac","fraction of CBs",0.5);
    RooAddPdf DoubleSidedCB("DoubleSidedCB","DoubleSidedCB",RooArgList(CB_left,CB_right),RooArgList(frac));

    // Create model
    RooExtendPdf DSCBExtended("DSCBExtended","Extended DSCB",DoubleSidedCB,N);
    // Perform fit
    RooFitResult* fResFit = DSCBExtended.fitTo(*fDataSet,Extended(kTRUE),Range(fMCutLow,fMCutUpp),Save());

    // ##########################################################
    // Plot the results
    // Draw Correlation Matrix
    TCanvas *cCorrMat = new TCanvas("cCorrMat","cCorrMat",700,600);
    InvMassFit_MC_DrawCorrMatrix(cCorrMat, fResFit);

    // Draw histogram and fit
    TCanvas *cHist = new TCanvas("cHist","cHist",800,600);
    InvMassFit_MC_SetCanvas(cHist,kFALSE);

    RooPlot* frameM = fM.frame(Title("Mass fit")); 
    fDataSet->plotOn(frameM,Name("data"),Binning(binM),MarkerStyle(20),MarkerSize(1.));
    DoubleSidedCB.plotOn(frameM,Name("DoubleSidedCB"),LineColor(215),LineWidth(3),LineStyle(kDashed));
    // Y axis
    frameM->GetYaxis()->SetTitleSize(0.045);
    frameM->GetYaxis()->SetLabelSize(0.045);
    frameM->GetYaxis()->SetLabelOffset(0.01);
    frameM->GetYaxis()->SetTitle(Form("Counts per %i MeV/#it{c}^{2}", BinSize));
    frameM->GetYaxis()->SetTitleOffset(1);
    frameM->GetYaxis()->SetMaxDigits(3);
    // X axis
    frameM->GetXaxis()->SetTitleSize(0.045);
    frameM->GetXaxis()->SetLabelSize(0.045);
    frameM->GetXaxis()->SetLabelOffset(0.01);
    frameM->GetXaxis()->SetTitle("#it{m}_{#mu#mu} (GeV/#it{c}^{2})");
    frameM->GetXaxis()->SetTitleOffset(1.1);
    frameM->Draw();

    // Get chi2 
    Double_t chi2 = frameM->chiSquare("CB_ext","data",fResFit->floatParsFinal().getSize());

    TLegend *leg = new TLegend(0.655,0.5,0.945,0.935);
    leg->AddEntry((TObject*)0,Form("#it{N} = %.f #pm %.f", N.getVal(), N.getError()),"");
    leg->AddEntry((TObject*)0,Form("#mu = %.4f GeV/#it{c}^{2}", mean_L.getVal()),""); // mean_L.getError()
    leg->AddEntry((TObject*)0,Form("#sigma = %.4f GeV/#it{c}^{2}", sigma_L.getVal()),""); // sigma_L.getError()
    leg->AddEntry((TObject*)0,Form("#alpha_{L} = %.3f #pm %.3f", alpha_L.getVal(), alpha_L.getError()),"");
    leg->AddEntry((TObject*)0,Form("#alpha_{R} = %.3f #pm %.3f", (-1)*(alpha_R.getVal()), alpha_R.getError()),"");
    leg->AddEntry((TObject*)0,Form("#it{n}_{L} = %.2f #pm %.2f", n_L.getVal(), n_L.getError()),"");
    leg->AddEntry((TObject*)0,Form("#it{n}_{R} = %.2f #pm %.2f", n_R.getVal(), n_R.getError()),"");
    leg->SetTextSize(0.042);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    TLegend *leg2 = new TLegend(0.10,0.7,0.3,0.935);
    leg2->AddEntry((TObject*)0,Form("ALICE Simulation"),"");
    leg2->AddEntry((TObject*)0,Form("Pb#minusPb #sqrt{#it{s}_{NN}} = 5.02 TeV"),"");
    leg2->AddEntry((TObject*)0,Form("MC rec: J/#psi #rightarrow #mu^{+}#mu^{-}"),"");
    // Print the pT cut
    if(opt == 0){
        leg2->AddEntry((TObject*)0,Form("#it{p}_{T} > %.2f GeV/#it{c}", fPtCut),"");
    } else if(opt == 1 || opt == 2){
        leg2->AddEntry((TObject*)0,Form("#it{p}_{T} < %.2f GeV/#it{c}", fPtCut),"");
    } else if(opt == 3 || opt == 4 || opt == 5 || opt == 6 || opt == 7 || opt == 8){
        leg2->AddEntry((TObject*)0,Form("#it{p}_{T} #in (%.2f,%.2f) GeV/#it{c}", fPtCutLow,fPtCutUpp),"");
    }
    leg2->SetTextSize(0.042);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->Draw();

    // Draw Histogram with log scale
    TCanvas *cHistLog = new TCanvas("cHistLog","cHistLog",800,600);
    InvMassFit_MC_SetCanvas(cHistLog,kTRUE);
    frameM->Draw();
    leg->Draw();
    leg2->Draw();

    // Prepare path
    gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/");
    TString str = "Results/" + str_subfolder + "InvMassFit_MC/";

    switch(opt){
        case 0:
            gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/inc/");
            str = str + "inc/inc";
            break;
        case 1:
            gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/coh/");
            str = str + "coh/coh";
            break;
        case 2:
            gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/all/");
            str = str + "all/all";
            break;
        case 3:
            gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/allbins/");
            str = str + "allbins/allbins";
            break;
        case 4:
            if(nPtBins == 4) str = str + "4bins/bin1"; 
            if(nPtBins == 5) str = str + "5bins/bin1"; 
            break;
        case 5:
            if(nPtBins == 4) str = str + "4bins/bin2"; 
            if(nPtBins == 5) str = str + "5bins/bin2"; 
            break;
        case 6:
            if(nPtBins == 4) str = str + "4bins/bin3"; 
            if(nPtBins == 5) str = str + "5bins/bin3"; 
            break;
        case 7:
            if(nPtBins == 4) str = str + "4bins/bin4"; 
            if(nPtBins == 5) str = str + "5bins/bin4"; 
            break;
        case 8:
            if(nPtBins == 5) str = str + "5bins/bin5"; 
            break;
    }
    if(opt > 3 && nPtBins == 4) gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/4bins/");
    if(opt > 3 && nPtBins == 5) gSystem->Exec("mkdir -p Results/" + str_subfolder + "InvMassFit_MC/5bins/");
    // Print the plots
    cHist->Print((str + ".pdf").Data());
    cHist->Print((str + ".png").Data());
    cHistLog->Print((str + "_log.pdf").Data());
    cHistLog->Print((str + "_log.png").Data());
    cCorrMat->Print((str + "_cm.pdf").Data());
    cCorrMat->Print((str + "_cm.png").Data());  
    
    // Print the values of alpha and n to txt output files
    ofstream outfile((str + ".txt").Data());
    outfile << "alpha_L \t" << alpha_L.getVal() << "\t" << alpha_L.getError() << "\n";
    outfile << "alpha_R \t" << alpha_R.getVal() << "\t" << alpha_R.getError() << "\n";
    outfile << "n_L \t" << n_L.getVal() << "\t" << n_L.getError() << "\n";
    outfile << "n_R \t" << n_R.getVal() << "\t" << n_R.getError() << "\n";
    outfile.close();
    Printf("*** Results printed to %s.***", (str + ".txt").Data());

    return;
}

void InvMassFit_MC_DrawCorrMatrix(TCanvas *cCorrMat, RooFitResult* fResFit){

    cCorrMat->SetTopMargin(0.05);
    cCorrMat->SetRightMargin(0.12);
    cCorrMat->SetLeftMargin(0.12);

    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gStyle->SetPaintTextFormat("4.2f");

    TH2* hCorr = fResFit->correlationHist();

    hCorr->GetXaxis()->SetBinLabel(7,"#sigma");
    hCorr->GetYaxis()->SetBinLabel(7,"#sigma");

    hCorr->SetMarkerSize(2.0);
    hCorr->GetXaxis()->SetLabelSize(0.08); // 0.049
    hCorr->GetYaxis()->SetLabelSize(0.08);
    hCorr->Draw("colz,text");

    return;
}

void InvMassFit_MC_SetCanvas(TCanvas *c, Bool_t bLogScale){

    if(bLogScale == kTRUE) c->SetLogy();
    c->SetTopMargin(0.055);
    c->SetBottomMargin(0.12);
    c->SetRightMargin(0.03);
    c->SetLeftMargin(0.11);

    return;
}

void InvMassFit_MC_PrepareData(){

    gSystem->Exec("mkdir -p Trees/" + str_subfolder + "InvMassFit_MC/");
    TString name = "Trees/" + str_subfolder + "InvMassFit_MC/InvMassFit_MC.root";

    // kCohJpsiToMu
    TFile *f_in_coh = TFile::Open((str_in_MC_fldr_rec + "AnalysisResults_MC_kCohJpsiToMu.root").Data(), "read");
    if(f_in_coh) Printf("Input data loaded.");

    TTree *t_in_coh = dynamic_cast<TTree*> (f_in_coh->Get(str_in_MC_tree_rec.Data()));
    if(t_in_coh) Printf("Input tree loaded.");

    // kIncohJpsiToMu
    TFile *f_in_inc = TFile::Open((str_in_MC_fldr_rec + "AnalysisResults_MC_kIncohJpsiToMu.root").Data(), "read");
    if(f_in_inc) Printf("Input data loaded.");

    TTree *t_in_inc = dynamic_cast<TTree*> (f_in_inc->Get(str_in_MC_tree_rec.Data()));
    if(t_in_inc) Printf("Input tree loaded.");
    
    // Create new MC trees with applied cuts
    TFile f_out(name.Data(),"RECREATE");

    TTree *tIncEnrSample = new TTree("tIncEnrSample", "tIncEnrSample");
    tIncEnrSample->Branch("fPt", &fPt, "fPt/D");
    tIncEnrSample->Branch("fM", &fM, "fM/D");
    tIncEnrSample->Branch("fY", &fY, "fY/D");

    TTree *tCohEnrSample = new TTree("tCohEnrSample", "tCohEnrSample");
    tCohEnrSample->Branch("fPt", &fPt, "fPt/D");
    tCohEnrSample->Branch("fM", &fM, "fM/D");
    tCohEnrSample->Branch("fY", &fY, "fY/D");

    TTree *tMixedSample = new TTree("tMixedSample", "tMixedSample");
    tMixedSample->Branch("fPt", &fPt, "fPt/D");
    tMixedSample->Branch("fM", &fM, "fM/D");
    tMixedSample->Branch("fY", &fY, "fY/D");

    ConnectTreeVariablesMCRec(t_in_coh);

    Printf("%lli entries found in the tree.", t_in_coh->GetEntries());
    Int_t nEntriesAnalysed = 0;

    for(Int_t iEntry = 0; iEntry < t_in_coh->GetEntries(); iEntry++){
        t_in_coh->GetEntry(iEntry);
        // no inv mass cut, pT cut: all
        if(EventPassedMCRec(0, 2)){
            tCohEnrSample->Fill();
            tMixedSample->Fill();
        } 

        if((iEntry+1) % 100000 == 0){
            nEntriesAnalysed += 100000;
            Printf("%i entries analysed.", nEntriesAnalysed);
        }
    }

    ConnectTreeVariablesMCRec(t_in_inc);

    Printf("%lli entries found in the tree.", t_in_inc->GetEntries());
    nEntriesAnalysed = 0;

    for(Int_t iEntry = 0; iEntry < t_in_inc->GetEntries(); iEntry++){
        t_in_inc->GetEntry(iEntry);
        // no inv mass cut, pT cut: all
        if(EventPassedMCRec(0, 2)){
            tIncEnrSample->Fill();
            tMixedSample->Fill();
        } 

        if((iEntry+1) % 100000 == 0){
            nEntriesAnalysed += 100000;
            Printf("%i entries analysed.", nEntriesAnalysed);
        }
    }

    f_out.Write("",TObject::kWriteDelete);

    return;
}