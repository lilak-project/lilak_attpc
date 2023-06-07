#ifndef ATCONVERSIONTASK_HH
#define ATCONVERSIONTASK_HH

#include "LKTask.h"

class ATConversionTask : public LKTask
{
    public:
        ATConversionTask();
        virtual ~ATConversionTask() {};

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        TClonesArray *fChannelArray = nullptr;

        TString fInputFileName = "~/data/texat/run_0824.dat.19-03-23_23h42m36s.38.root";
        TFile* fInputFile;
        TTree* fInputTree;

        Int_t   mmMul;
        Int_t   mmHit;
        Int_t   mmEventIdx;
        Int_t   mmFrameNo[1030];   //[mmMul]
        Int_t   mmDecayNo[1030];   //[mmMul]
        Int_t   mmCobo[1030];   //[mmMul]
        Int_t   mmAsad[1030];   //[mmMul]
        Int_t   mmAget[1030];   //[mmMul]
        Int_t   mmChan[1030];   //[mmMul]
        Float_t mmTime[1030];   //[mmMul]
        Float_t mmEnergy[1030];   //[mmMul]
        Int_t   mmWaveformX[1030][512];   //[mmMul][time]
        Int_t   mmWaveformY[1030][512];   //[mmMul][time]

        /// 0 = Left Strip 
        /// 1 = Right Strip 
        /// 2 = Left Chain 
        /// 3 = Right Chain 
        /// 4 = Low Center 
        /// 5 = High Center 
        /// 6 = Forward Si 
        /// 7 = Forward CsI 
        /// 8 = MMJr 
        /// 10 = CENS X6 
        /// 11 = CENS CSI 
        /// 100 = External inputs 
        /// initialized by -1
        Int_t type[3][4][4][68];
        /// 0 for left 
        /// 1 for right 
        /// 2 for center(front) 
        /// 10 for bottom-left(X6) 
        /// 11 for bottom-right(X6) 
        /// nothing for CsI(CENS) 
        /// initialized by -1
        Int_t DetLoc[3][4][4][68];

        const Int_t mmnum = 1024; // # of all channels
        const Int_t sinum = 45; // quadrant*9
        const Int_t X6num = 600; // 20chan*30det
        const Int_t CsInum = 64; // 1chan*64det
        const Int_t mmjrnum = 68; // should be changed but...

        Int_t mmasad[mmnum];
        Int_t mmaget[mmnum];
        Int_t mmdchan[mmnum];
        Int_t mmx[mmnum];
        Int_t mmy[mmnum];
        Int_t mmpx[4][4][64];
        Int_t mmpy[4][4][64]; // [mmAsad][mmAget][dchan]

        Int_t siasad[sinum];
        Int_t siaget[sinum];
        Int_t sichan[sinum];
        Int_t six[sinum];
        Int_t siy[sinum];
        Int_t sipos[sinum]; // x: 0-4 | y: 0-1 | pos: 1,2,3,4 (in circle)
        Int_t sipx[4][4][68];
        Int_t sipy[4][4][68];
        Int_t sistrip[4][4][68];
        Int_t sidet[4][4][68]; // px: 0-4 | py: 0-1 | strip: 1,2,3,4 (in circle) | sidet: 0-9

        Int_t fcsidet[68]; // 0-9

        Int_t X6asad[X6num];
        Int_t X6aget[X6num];
        Int_t X6chan[X6num];
        Int_t X6flag[X6num];
        Int_t X6detnum[X6num];
        Int_t X6pos[X6num]; // flag: 0(junc) & 1(ohm) && 10(bottom junc) & 11(bottom ohm) | detnum: 0-32(max) | pos: 1-16(channel from the official doc.)
        Int_t X6det[4][4][68];
        Int_t X6strip[4][4][68];
        Int_t X6ud[4][4][68]; // det: 1? for LS, 2? for RS, 10? for LB, 20? for RB | strip: 1-8(junc) & 1-4(ohm) | ud: 0(pin side) & 1

        Int_t CsIasad[CsInum];
        Int_t CsIaget[CsInum];
        Int_t CsIchan[CsInum];
        Int_t CsICTnum[CsInum];
        Int_t CsIpinflag[CsInum];
        Int_t CsItoX6det[CsInum]; //should be changed
        Int_t CsICT[4][4][68];
        Int_t CsIpin[4][4][68];
        Int_t CsIX6det[4][4][68]; // CT: 1-64 | pin: 1 for X6 pin side | X6det: matching X6 num


    ClassDef(ATConversionTask, 1)
};

#endif


/*
// ver. CRIB

//==================================================================================
//string inrfname = "./rootdata/run_0595.dat.15-03-23_04h53m18s.1.root"; //before find gate
//string inrfname = "./rootdata/run_0659.dat.16-03-23_01h53m21s.6.root"; //He:CO2
string inrfname = " ~/data/texat/run_0824.dat.19-03-23_23h42m36s.38.root"; //He:CO2
//string inrfname = "./rootdata/run_0893.dat.21-03-23_16h43m35s.1.root"; //CO2
//string inrfname = "./rootdata/run_0977.dat.24-03-23_02h55m04s.root";//14N with gate 0us/4us
//==================================================================================
Int_t TimeBin = 40; // 25 MHz readout -> 40 ns | 20 MHz readout -> 50 ns
Double_t DriftV = 0.738594; // IHM(0821): 0.738594cm/us
Int_t roughtiming = 250;

Int_t MaxBuck = 512;
Double_t CenterX = 3.42; // [mm]
Double_t CenterY = 1.67; // [mm]
Double_t ChainX = 1.67; // [mm]
Double_t StripY = 1.67; // [mm]
Double_t Stripspace = 1.75; // [mm]

const Int_t mmnum = 1024; // # of all channels
const Int_t sinum = 45; // quadrant*9
const Int_t X6num = 600; // 20chan*30det
const Int_t CsInum = 64; // 1chan*64det
const Int_t mmjrnum = 68; // should be changed but...
//==================================================================================
TFile* fInputFile = new TFile(inrfname.c_str(), "read");
TTree* fInputTree = (TTree*) fInputFile -> Get("TEvent");

Int_t type[3][4][4][68]; // 0 = Left Strip | 1 = Right Strip | 2 = Left Chain | 3 = Right Chain | 4 = Low Center | 5 = High Center | 6 = Forward Si | 7 = Forward CsI | 8 = MMJr | 10 = CENS X6 | 11 = CENS CSI | 100 = External inputs | initialized by -1
Int_t DetLoc[3][4][4][68]; // 0 for left | 1 for right | 2 for center(front) | 10 for bottom-left(X6) | 11 for bottom-right(X6) | nothing for CsI(CENS) | initialized by -1

Int_t mmasad[mmnum], mmaget[mmnum], mmdchan[mmnum];
Int_t mmx[mmnum], mmy[mmnum];
Int_t mmpx[4][4][64], mmpy[4][4][64]; // [mmAsad][mmAget][dchan]

Int_t siasad[sinum], siaget[sinum], sichan[sinum];
Int_t six[sinum], siy[sinum], sipos[sinum]; // x: 0-4 | y: 0-1 | pos: 1,2,3,4 (in circle)
Int_t sipx[4][4][68],sipy[4][4][68],sistrip[4][4][68],sidet[4][4][68]; // px: 0-4 | py: 0-1 | strip: 1,2,3,4 (in circle) | sidet: 0-9
//I think better to use 0-9, 0-4 for quadrants and add 100 for DSSD

Int_t fcsidet[68]; // 0-9

Int_t X6asad[X6num], X6aget[X6num], X6chan[X6num];
Int_t X6flag[X6num], X6detnum[X6num], X6pos[X6num]; // flag: 0(junc) & 1(ohm) && 10(bottom junc) & 11(bottom ohm) | detnum: 0-32(max) | pos: 1-16(channel from the official doc.)
Int_t X6det[4][4][68],X6strip[4][4][68],X6ud[4][4][68]; // det: 1? for LS, 2? for RS, 10? for LB, 20? for RB | strip: 1-8(junc) & 1-4(ohm) | ud: 0(pin side) & 1

Int_t CsIasad[CsInum], CsIaget[CsInum], CsIchan[CsInum];
Int_t CsICTnum[CsInum], CsIpinflag[CsInum], CsItoX6det[CsInum]; //should be changed
Int_t CsICT[4][4][68],CsIpin[4][4][68],CsIX6det[4][4][68]; // CT: 1-64 | pin: 1 for X6 pin side | X6det: matching X6 num

Double_t siJpar0[68], siJpar1[68]; // mmChan[hit]
Double_t siOpar0[68], siOpar1[68]; // mmChan[hit]
Double_t fcsipar0[68], fcsipar1[68]; // mmChan[hit]
Double_t X6Jpar0[32][8], X6Jpar1[32][8]; // [X6det][X6strip]
Double_t X6Opar0[32][4], X6Opar1[32][4]; // [X6det][X6strip]


Int_t Entries;
Int_t line;
Int_t chan;
Int_t dchan;
Int_t Buckbegin;

TH2D* HLWaveFormbyEvent;
TH2D* base;
TH2D* target;
TH2D* temp;
Int_t noisymm[1030]; // 0: empty | 1: noisy | 2: fine
TH2D* orgWF[1030];
TH2D* norWF[1030];
Int_t GoodSiEntry[9999]; // [entry] // 42 for bad Si Entry | front Si Chan for good Si Entry
Int_t SiBLR[9999]; // [entry] // 0 for left | 1 for right | 2 for center | 9 for bad
Int_t mmBase[1030]; // [mmMul]
Int_t ref_Asad, ref_Aget, ref_Chan, ref_Base;
Int_t Ref[512] = {0};
Int_t Stripval, Striphit;
Int_t mmNor[1030][512]; // [mmMul][buck]
//==================================================================================
Int_t           mmMul;
Int_t           mmHit;
Int_t           mmEventIdx;
Int_t           mmFrameNo[1030];   //[mmMul]
Int_t           mmDecayNo[1030];   //[mmMul]
Int_t           mmCobo[1030];   //[mmMul]
Int_t           mmAsad[1030];   //[mmMul]
Int_t           mmAget[1030];   //[mmMul]
Int_t           mmChan[1030];   //[mmMul]
Float_t         mmTime[1030];   //[mmMul]
Float_t         mmEnergy[1030];   //[mmMul]
Int_t           mmWaveformX[1030][512];   //[mmMul][time]
Int_t           mmWaveformY[1030][512];   //[mmMul][time]
//==================================================================================
void SetBranch();
void SetDetType();
void ana_DrawWFbyEntry(Int_t evtbegin, Int_t evtend);
void ana_DrawWFbyEntryLR(Int_t evt);
void test_DrawWFbyEntryLR2(Int_t evt);
void test_BeamSeparate(Int_t evt);
void test_BeamTiming(Int_t evt);
void test_BeamTiming_dtplot();
void test_DrawTrack(Int_t evt);
void test_DrawTrack_Hough(Int_t evt);
void test_DrawTrack_realHough(Int_t evt);
void test_DrawTrack_extrapolation();
void ana_SiHitPattern();
void find_mmBase(Int_t evt, Int_t hit);
void test_BeamEnergy(Int_t evt);
void test_DrawMMS(Int_t evt);
void ana_DrawOneWFbyEntry(Int_t evtidx, Int_t coboid, Int_t asadid, Int_t agetid, Int_t chanid);
void test_nor(Int_t evt);
void test_FindNoisy(Int_t evt);
void ana_FindEventbyId(Int_t evtbegin, Int_t evtend, Int_t coboid, Int_t asadid, Int_t agetid, Int_t chanid, Int_t ymin, Int_t ymax);
void test_LeftorRight(Int_t evtbegin, Int_t evtend);
void LeftorRight_new(Int_t evtbegin, Int_t evtend); //find events with proper Si hits
void test_BaseLine(Int_t evt);
void test_norLR(Int_t evt);
void test_norLR_foruse(Int_t evt);
void test_norLR_forbetter(Int_t evt);
void test_DrawMMSLR(Int_t evt);
void test_simpledEE(Int_t evtbegin, Int_t evtend);
void test_findwindow_org(Int_t evt);
void test_findwindow_nor(Int_t evt);
void TAC_scint_X6();
void totE_strip_frontsi();
void PID_forward_CsIvsSi(Int_t calflag);
void PID_forward_SivsMM();
void PID_side_CsIvsX6(Int_t calflag);
void PID_side_X6vsMM();
void FindL0();
//==================================================================================
*/
