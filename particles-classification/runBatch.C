void runBatch(int start_range=0, int stop_range=950, const char* outfilename="AnalysisResults.root", int Nevents=20000000) {
  TStopwatch timer;
  timer.Start();

  //Setting up required packages
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libSTEERBase.so");
  gSystem->Load("libESD.so");
  gSystem->Load("libAOD.so");
  gSystem->Load("libANALYSIS.so");
  gSystem->Load("libANALYSISalice.so");
  gSystem->Load("libEVGEN.so");
  gSystem->Load("libpythia6_4_25.so");
  gROOT->ProcessLine(".include $ALICE_ROOT/include");

  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");

  //add files to analysis
  TChain *chain = new TChain("aodTree");
  char pname[2000];
  for( int i = start_range; i < stop_range; ++i) {
      sprintf(pname, "AOD/Ali%d.root", i);
      chain->Add(pname);
    }

  // Make the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("TestManager");

  AliAODInputHandler* aodH = new AliAODInputHandler;
  mgr->SetInputEventHandler(aodH);

  //AddTaskPIDResponse - get labels from Monte Carlo data
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
  Bool_t isMC=kTRUE; // kTRUE in case of Monte Carlo
  AddTaskPIDResponse(isMC,kTRUE);

  //Efficiency task - extracting data from tracks
  gROOT->LoadMacro("AliAnalysisTaskExampleMCAOD.cxx+g");
  AliAnalysisTaskExampleMCAOD *myTask = new AliAnalysisTaskExampleMCAOD("MyTask");
  myTask->SelectCollisionCandidates(AliVEvent::kMB);
  if( !myTask )
	  exit(-1);
  mgr->AddTask(myTask);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput2 = mgr->CreateContainer("MyTree", TList::Class(),AliAnalysisManager::kOutputContainer,outfilename);

  mgr->ConnectInput(myTask,0,cinput);
  mgr->ConnectOutput(myTask,1,coutput2);

  if ( !mgr->InitAnalysis() )
	  return;
  mgr->PrintStatus();
  mgr->StartAnalysis("local",chain,Nevents);

  timer.Stop();
  timer.Print();
  TFile *ifile = new TFile(outfilename);

  TList *lists;
  (TList *) ifile->GetObject("MyTree", lists);

  TFile *ofile = new TFile(outfilename, "RECREATE");

  TIter next(lists);
  TObject *obj;
  while (obj = next())
    {
      ofile->cd();
      obj->Write();
    }
  delete ofile;
}
