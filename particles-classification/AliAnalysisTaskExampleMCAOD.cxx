#include <iostream>
#include <fstream>
#include <cmath>
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TList.h"
#include "TObjArray.h"
#include "TString.h"
#include "TParticle.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliCentrality.h"

#include "AliESDEvent.h"
#include "AliESDtrackCuts.h"
#include "AliESDInputHandler.h"

#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliInputEventHandler.h"

#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODHandler.h"
#include "AliAODInputHandler.h"
#include "AliAODMCParticle.h"
#include "AliPIDResponse.h"

#include "AliAODpidUtil.h"
#include "AliAODHeader.h"

#include "AliAnalysisTaskExampleMCAOD.h"



ClassImp(AliAnalysisTaskExampleMCAOD)

//position of primary vertex
double fV1[3];


AliAnalysisTaskExampleMCAOD::AliAnalysisTaskExampleMCAOD() :
	AliAnalysisTaskSE(), fHistoList(0),  fpidResponse(0), fAODpidUtil(0)
{
	DefineOutput(1, TList::Class());
}

AliAnalysisTaskExampleMCAOD::AliAnalysisTaskExampleMCAOD(const Char_t *partName) :
	AliAnalysisTaskSE(partName), fHistoList(0),  fpidResponse(0), fAODpidUtil(0)
{
	DefineOutput(1, TList::Class());
}


AliAnalysisTaskExampleMCAOD::~AliAnalysisTaskExampleMCAOD()
{
	if (AliAnalysisManager::GetAnalysisManager()->GetAnalysisType() != AliAnalysisManager::kProofAnalysis)
		delete fHistoList;
}

void AliAnalysisTaskExampleMCAOD::UserCreateOutputObjects()
{
	//output objects
	fHistoList = new TList();
	fHistoList->SetOwner(kTRUE);

	//ttree holding all attributes of observations for classification
	treeOutput = new TTree("t3", "OutputTree");
	treeOutput->Branch("TPCNcls", &TPCNcls, "TPCNcls/D");
	treeOutput->Branch("TPCsignal", &TPCsignal, "TPCsignal/D");
	treeOutput->Branch("ped", &ped, "ped/D");
	treeOutput->Branch("Pt", &Pt, "Pt/D");
	treeOutput->Branch("Px", &Px, "Px/D");
	treeOutput->Branch("Py", &Py, "Py/D");
	treeOutput->Branch("Pz", &Pz, "Pz/D");
	treeOutput->Branch("nSigmaTOFPi", &nSigmaTOFPi, "nSigmaTOFPi/D");
	treeOutput->Branch("nSigmaTOFK", &nSigmaTOFK, "nSigmaTOFK/D");
	treeOutput->Branch("nSigmaTOFP", &nSigmaTOFP, "nSigmaTOFP/D");
	treeOutput->Branch("nSigmaTOFe", &nSigmaTOFe, "nSigmaTOFe/D");
	treeOutput->Branch("nSigmaTPCPi", &nSigmaTPCPi, "nSigmaTPCPi/D");
	treeOutput->Branch("nSigmaTPCK", &nSigmaTPCK, "nSigmaTPCK/D");
	treeOutput->Branch("nSigmaTPCP", &nSigmaTPCP, "nSigmaTPCP/D");
	treeOutput->Branch("nSigmaTPCe", &nSigmaTPCe, "nSigmaTPCe/D");
	treeOutput->Branch("tbeta", &tbeta, "tbeta/D");
	treeOutput->Branch("cov0", &cov0, "cov0/D");
	treeOutput->Branch("cov1", &cov1, "cov1/D");
	treeOutput->Branch("cov2",&cov2, "cov2/D");
	treeOutput->Branch("cov3", &cov3, "cov3/D");
	treeOutput->Branch("cov4", &cov4, "cov4/D");
	treeOutput->Branch("cov5", &cov5, "cov5/D");
	treeOutput->Branch("cov6", &cov6, "cov6/D");
	treeOutput->Branch("cov7", &cov7, "cov7/D");
	treeOutput->Branch("cov8", &cov8, "cov8/D");
	treeOutput->Branch("cov9", &cov9, "cov9/D");
	treeOutput->Branch("cov10", &cov10, "cov10/D");
	treeOutput->Branch("cov11", &cov11, "cov11/D");
	treeOutput->Branch("cov12", &cov12, "cov12/D");
	treeOutput->Branch("cov13", &cov13, "cov13/D");
	treeOutput->Branch("cov14", &cov14, "cov14/D");
	treeOutput->Branch("cov15", &cov15, "cov15/D");
	treeOutput->Branch("cov16", &cov16, "cov16/D");
	treeOutput->Branch("cov17", &cov17, "cov17/D");
	treeOutput->Branch("cov18", &cov18, "cov18/D");
	treeOutput->Branch("cov19", &cov19, "cov19/D");
	treeOutput->Branch("cov20", &cov20, "cov20/D");
	treeOutput->Branch("PDGCode", &PDGCode, "PDGCode/I");
	treeOutput->Branch("isPion", &isPion, "IsPion/I");
	treeOutput->Branch("isKaon", &isKaon, "IsKaon/I");
	treeOutput->Branch("isProton", &isProton, "IsProton/I");

	fHistoList->Add(treeOutput);

	//********** PID ****************
	AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
	AliInputEventHandler* inputHandler = (AliInputEventHandler*) (man->GetInputEventHandler());
	fpidResponse = inputHandler->GetPIDResponse();


	PostData(1, fHistoList);
}

//traditional method for pion classification
bool AliAnalysisTaskExampleMCAOD::IsPionNSigma(float mom,
		float nsigmaTPCPi, float nsigmaTOFPi, float TOFtime)
{
	if (mom > 0.5) {
		if (TMath::Hypot( nsigmaTOFPi, nsigmaTPCPi ) < 2)
			return true;
	}
	else {
		if (TMath::Abs(nsigmaTPCPi) < 2)
			return true;
	}

	return false;
}

//traditional method for kaon classification
bool AliAnalysisTaskExampleMCAOD::IsKaonNSigma(float mom,
		float nsigmaTPCK, float nsigmaTOFK, float TOFtime)
{
	if (mom > 0.5) {
		if (TMath::Hypot( nsigmaTOFK, nsigmaTPCK ) < 2)
			return true;
	}
	else {
		if (TMath::Abs(nsigmaTPCK) < 2)
			return true;
	}

	return false;
}

//traditional method for proton classification
bool AliAnalysisTaskExampleMCAOD::IsProtonNSigma(float mom,
		float nsigmaTPCP, float nsigmaTOFP, float TOFtime)
{
	if (mom > 0.5) {
		if (TMath::Hypot( nsigmaTOFP, nsigmaTPCP ) < 2)
			return true;
	}
	else {
		if (TMath::Abs(nsigmaTPCP) < 2)
			return true;
	}

	return false;
}

//function checking whether track was measured correctly
bool AliAnalysisTaskExampleMCAOD::isTrackValid(double eta, double pt, bool covxyz)
{
	//typical cuts (selection criteria)
	if(eta < -0.8 || eta > 0.8) //pseudorapidity range of TPC
		return false;

	if (pt < 0.2 || pt > 20) //transverse momentum
		return false;

	if ( !covxyz )
		return false;

	return true;
}

//load info about track to designated data structures
//which are connected to TTree branches for further filling
void AliAnalysisTaskExampleMCAOD::loadTrackInfo(AliAODTrack* track)
{
	float tPt = track->Pt();
	float tTofSig = track->GetTOFsignal(); //TOF signal
	double pidTime[5]; track->GetIntegratedTimes(pidTime); //TOF times
	tbeta = tTofSig-pidTime[2];

	double covxyz[21];
	track->GetCovarianceXYZPxPyPz(covxyz);

	//Number of sigmas (nsigma) - traditional - method of selecting tracks
	isPion = (IsPionNSigma(tPt,nSigmaTPCPi, nSigmaTOFPi, tbeta));
	isKaon = (IsKaonNSigma(tPt,nSigmaTPCK, nSigmaTOFK, tbeta));
	isProton = (IsProtonNSigma(tPt,nSigmaTPCP, nSigmaTOFP, tbeta));


 	TPCNcls = track->GetTPCNcls();
	TPCsignal = track->GetTPCsignal();
	//tracks momentum
	double track_ped = sqrt( track->Px() * track->Px() + track->Py() * track->Py() + track->Pz() * track->Pz() );
	ped = track_ped;
	Pt = track->Pt();
	Px = track->Px();
	Py = track->Py();
	Pz = track->Pz();

	//fill the covariance matrix
	cov0 = covxyz[0];
	cov1 = covxyz[1];
	cov2 = covxyz[2];
	cov3 = covxyz[3];
	cov4 = covxyz[4];
	cov5 = covxyz[5];
	cov6 = covxyz[6];
	cov7 = covxyz[7];
	cov8 = covxyz[8];
	cov9 = covxyz[9];
	cov10 = covxyz[10];
	cov11 = covxyz[11];
	cov12 = covxyz[12];
	cov13 = covxyz[13];
	cov14 = covxyz[14];
	cov15 = covxyz[15];
	cov16 = covxyz[16];
	cov17 = covxyz[17];
	cov18 = covxyz[18];
	cov19 = covxyz[19];
	cov20 = covxyz[20];
}

//function iterating over every available track to get its info
//and save it into TTree
void AliAnalysisTaskExampleMCAOD::UserExec(Option_t *)
{
	//prepare environment
	AliAODInputHandler *aodH = dynamic_cast<AliAODInputHandler *>(
			AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler());
	AliAODEvent *fAOD = aodH->GetEvent();
	fAODpidUtil = aodH->GetAODpidUtil();

	//get event
	AliAODEvent* aodEvent = dynamic_cast<AliAODEvent*>(InputEvent());
	if (!aodEvent) return;
	AliAODHeader *fAODheader = (AliAODHeader*)aodEvent->GetHeader();
	Double_t mult = fAODheader->GetRefMultiplicity();
	if(mult<0) return;

	// EVENT SELECTION ********************
	const AliAODVertex* vertex =(AliAODVertex*) aodEvent->GetPrimaryVertex();
	vertex->GetPosition(fV1); //position of primary vertex
	if (!vertex || vertex->GetNContributors()<=0) return;

	AliAnalysisUtils *anaUtil=new AliAnalysisUtils();

	Bool_t fMVPlp = kFALSE;
	Bool_t fisPileUp = kTRUE;
	Int_t fMinPlpContribMV = 0;
	Int_t fMinPlpContribSPD = 3;

	if(fMVPlp)
		anaUtil->SetUseMVPlpSelection(kTRUE);
	else
		anaUtil->SetUseMVPlpSelection(kFALSE);

	if(fMinPlpContribMV)
		anaUtil->SetMinPlpContribMV(fMinPlpContribMV);

	if(fMinPlpContribSPD)
		anaUtil->SetMinPlpContribSPD(fMinPlpContribSPD);

	if(fisPileUp)
		if(anaUtil->IsPileUpEvent(aodEvent))
			return;

	delete anaUtil;

	Float_t zvtx = vertex->GetZ();
	if (TMath::Abs(zvtx) > 10)
		return;

 	//getting MC particle array
	TClonesArray *arrayMC = dynamic_cast<TClonesArray*>(
			aodEvent->FindListObject(AliAODMCParticle::StdBranchName()));

	//loop over AOD reconstructed tracks
	for (Int_t iTracks = 0; iTracks < aodEvent->GetNumberOfTracks(); iTracks++) {
		//get track
		if(!arrayMC)
			continue;

		AliAODTrack *track = (AliAODTrack*)aodEvent->GetTrack(iTracks);
		if (!track)
			continue;

		UInt_t filterBit = 96;
		if(!track->TestFilterBit(filterBit))
			continue;

		//check if particle was correctly measured
		double cov[21];
		if( !isTrackValid( track->Eta(), track->Pt(), track->GetCovarianceXYZPxPyPz(cov) ) )
			continue;

		//check particle nsigma status
		nSigmaTOFPi = -9999;
		nSigmaTOFK = -9999;
		nSigmaTOFP = -9999;
		nSigmaTOFe = -9999;
		int statusTOFPi =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTOF,track,AliPID::kPion, nSigmaTOFPi);
		int statusTOFK =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTOF,track,AliPID::kKaon, nSigmaTOFK);
		int statusTOFP =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTOF,track,AliPID::kProton, nSigmaTOFP);
		int statusTOFe =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTOF,track,AliPID::kElectron, nSigmaTOFe);
		if((statusTOFPi != AliPIDResponse::kDetPidOk)
				|| (statusTOFK != AliPIDResponse::kDetPidOk)
				|| (statusTOFP != AliPIDResponse::kDetPidOk)
				|| (statusTOFe != AliPIDResponse::kDetPidOk))
			continue;

		nSigmaTPCPi = -9999;
		nSigmaTPCK = -9999;
		nSigmaTPCP = -9999;
		nSigmaTPCe = -9999;
		int statusTPCPi =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTPC,track,AliPID::kPion, nSigmaTPCPi);
		int statusTPCK =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTPC,track,AliPID::kKaon, nSigmaTPCK);
		int statusTPCP =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTPC,track,AliPID::kProton, nSigmaTPCP);
		int statusTPCe =
			fpidResponse->NumberOfSigmas(AliPIDResponse::kTPC,track,AliPID::kElectron, nSigmaTPCe);
		if((statusTPCPi != AliPIDResponse::kDetPidOk)
				|| (statusTPCK != AliPIDResponse::kDetPidOk)
				|| (statusTPCP != AliPIDResponse::kDetPidOk)
				|| (statusTPCe != AliPIDResponse::kDetPidOk))
			continue;

		//prepare TTree variables to hold current track data
		loadTrackInfo(track);

		//get particle code
		Int_t label = TMath::Abs(track->GetLabel());
		AliAODMCParticle *MCtrk = (AliAODMCParticle*)arrayMC->At(label);
		int PDGcode = MCtrk->GetPdgCode();
		PDGCode = TMath::Abs(PDGcode);

		//save all attributes into TTree
		treeOutput->Fill();
	}
}


