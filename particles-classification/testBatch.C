#include <vector>
#include <iostream>
#include <stdexcept>

//analysis task to test
class AliAnalysisTaskExampleMCAOD;

//testing classes for track criteria validation

//base class for tests
class AliTestTrack {
public:
	//test given analysis task function and check if results match
	virtual bool testCurrent(AliAnalysisTaskExampleMCAOD* task, bool result);
};

//class for testing tracks outliers
class AliValidTrack : public AliTestTrack {
public:
	AliValidTrack(double E, double P, bool c) :
		Eta(E), Pt(P), cov(c) {}

	//test whether tracks are chosen correctly based on their Eta,
	//momentum and covariance matrix presence
	virtual bool testCurrent(AliAnalysisTaskExampleMCAOD* task, bool result)
	{
		return task->isTrackValid(Eta, Pt, cov) == result;
	}
private:
	double Eta;
	double Pt;
	bool cov;
};

//class for testing traditional proton classification
class AliProtonTrack : public AliTestTrack {
public:
	AliProtonTrack(double p, double TPC, double TOF, double b) :
		Pt(p), nSigmaTPC(TPC), nSigmaTOF(TOF), tbeta(b) {}

	bool testCurrent(AliAnalysisTaskExampleMCAOD* task, bool result)
	{
		return task->IsProtonNSigma(Pt, nSigmaTPC, nSigmaTOF, tbeta) == result;
	}
private:
	double Pt;
	double nSigmaTPC;
	double nSigmaTOF;
	double tbeta;
};

//class for testing traditional pion classification
class AliPionTrack : public AliTestTrack {
public:
	AliPionTrack(double p, double TPC, double TOF, double b) :
		Pt(p), nSigmaTPC(TPC), nSigmaTOF(TOF), tbeta(b) {}

	bool testCurrent(AliAnalysisTaskExampleMCAOD* task, bool result)
	{
		return task->IsPionNSigma(Pt, nSigmaTPC, nSigmaTOF, tbeta) == result;
	}
private:
	double Pt;
	double nSigmaTPC;
	double nSigmaTOF;
	double tbeta;
};

//class for testing traditional kaon classification
class AliKaonTrack : public AliTestTrack {
public:
	AliKaonTrack(double p, double TPC, double TOF, double b) :
		Pt(p), nSigmaTPC(TPC), nSigmaTOF(TOF), tbeta(b) {}

	bool testCurrent(AliAnalysisTaskExampleMCAOD* task, bool result)
	{
		return task->IsKaonNSigma(Pt, nSigmaTPC, nSigmaTOF, tbeta) == result;
	}
private:
	double Pt;
	double nSigmaTPC;
	double nSigmaTOF;
	double tbeta;
};

//test whether tracks incorrectly measured are correctly dropped
bool testTrackValid(AliAnalysisTaskExampleMCAOD *myTask)
{
	const int test_number = 6;
 	AliValidTrack *tracks[test_number];
	std::vector<int> track_validation;
 	tracks[0] = new AliValidTrack(0.5, 1, true);
	track_validation.push_back( 1 );
 	tracks[1] = new AliValidTrack(0.9, 1, true);
	track_validation.push_back( 0 );
 	tracks[2] = new AliValidTrack(-1.0, 1, true);
	track_validation.push_back( 0 );
 	tracks[3] = new AliValidTrack(0.5, 25, true);
	track_validation.push_back( 0 );
 	tracks[4] = new AliValidTrack(0.5, 0.1, true);
	track_validation.push_back( 0 );
 	tracks[5] = new AliValidTrack(0.5, 1, false);
	track_validation.push_back( 0 );

	bool success = true;
	for( int i = 0; i < track_validation.size(); ++i )
	{
		if( !tracks[i]->testCurrent(myTask, track_validation[i]) ) {
      		::Error("Test failed", "Test failed on track number %d ", i);
			success = false;
			break;
		}
	}

	//clean up after tests
	for( int i = 0; i < test_number ; ++i )
		delete tracks[i];

	return success;
}

//test whether protons are correctly classified via traditional method
bool testProtonValid(AliAnalysisTaskExampleMCAOD *myTask)
{
	const int test_number = 4;
 	AliProtonTrack *tracks[test_number];
	std::vector<int> track_validation;
 	tracks[0] = new AliProtonTrack(0.4, 1, 1, 0);
	track_validation.push_back( 1 );
 	tracks[1] = new AliProtonTrack(1.0, 0.5, 0.5, 0);
	track_validation.push_back( 1 );
 	tracks[2] = new AliProtonTrack(0.4, 5, 5, 0);
	track_validation.push_back( 0 );
 	tracks[3] = new AliProtonTrack(0.6, 5, 5, 0);
	track_validation.push_back( 0 );

	bool success = true;
	for( int i = 0; i < track_validation.size(); ++i )
	{
		if( !tracks[i]->testCurrent(myTask, track_validation[i]) ) {
      		::Error("Test failed", "Test failed on track number %d ", i);
			success = false;
			break;
		}
	}

	//clean up after tests
	for( int i = 0; i < test_number ; ++i )
		delete tracks[i];

	return success;
}

//test whether pions are correctly classified via traditional method
bool testPionValid(AliAnalysisTaskExampleMCAOD *myTask)
{
	const int test_number = 4;
 	AliPionTrack *tracks[test_number];
	std::vector<int> track_validation;
 	tracks[0] = new AliPionTrack(0.4, 1, 1, 0);
	track_validation.push_back( 1 );
 	tracks[1] = new AliPionTrack(1.0, 0.5, 0.5, 0);
	track_validation.push_back( 1 );
 	tracks[2] = new AliPionTrack(0.4, 5, 5, 0);
	track_validation.push_back( 0 );
 	tracks[3] = new AliPionTrack(0.6, 5, 5, 0);
	track_validation.push_back( 0 );

	bool success = true;
	for( int i = 0; i < track_validation.size(); ++i )
	{
		if( !tracks[i]->testCurrent(myTask, track_validation[i]) ) {
      		::Error("Test failed", "Test failed on track number %d ", i);
			success = false;
			break;
		}
	}

	//clean up after tests
	for( int i = 0; i < test_number ; ++i )
		delete tracks[i];

	return success;
}

//test whether kaons are correctly classified via traditional method
bool testKaonValid(AliAnalysisTaskExampleMCAOD *myTask)
{
	const int test_number = 4;
 	AliKaonTrack *tracks[test_number];
	std::vector<int> track_validation;
 	tracks[0] = new AliKaonTrack(0.4, 1, 1, 0);
	track_validation.push_back( 1 );
 	tracks[1] = new AliKaonTrack(1.0, 0.5, 0.5, 0);
	track_validation.push_back( 1 );
 	tracks[2] = new AliKaonTrack(0.4, 5, 5, 0);
	track_validation.push_back( 0 );
 	tracks[3] = new AliKaonTrack(0.6, 5, 5, 0);
	track_validation.push_back( 0 );

	bool success = true;
	for( int i = 0; i < track_validation.size(); ++i )
	{
		if( !tracks[i]->testCurrent(myTask, track_validation[i]) ) {
      		::Error("Test failed", "Test failed on track number %d ", i);
			success = false;
			break;
		}
	}

	//clean up after tests
	for( int i = 0; i < test_number ; ++i )
		delete tracks[i];

	return success;
}

//test all major functions of analysis task used to prepare data particles
int testBatch(){
	TStopwatch timer;
	timer.Start();

	//load all required libraries
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
	gROOT->LoadMacro("AliAnalysisTaskExampleMCAOD.cxx+g");

	//run all test functions
	AliAnalysisTaskExampleMCAOD *myTask = new AliAnalysisTaskExampleMCAOD("MyTask");
	bool track_result = testTrackValid(myTask);
	bool pion_result = testPionValid(myTask);
	bool proton_result = testProtonValid(myTask);
	bool kaon_result = testKaonValid(myTask);
	timer.Print();
	if( track_result && pion_result && proton_result && kaon_result ) {
		::Info("Test succesful", "All tests have passed");
		return 0;
	}

	return 1;
}
