#ifndef ATMFMCONVERSIONTASK_HH
#define ATMFMCONVERSIONTASK_HH

#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for malloc */
#include <ncurses.h>    /* for getch() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <errno.h>
#include "TH1.h"
#include "TH1.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include <stdlib.h>
#include <unistd.h>
#include "mfm/FrameBuilder.h"
#include "json.h"
#include <sstream>
#include <fstream>
#include <TSystem.h>
#include <thread>

#include "LKTask.hpp"
#include "ATMFMFrameBuilder.hpp"

/*
 * AT-TPC MFM conversion class
 * Imported from MFMHistServer
 */
class ATMFMConversionTask : public LKTask
{
    public:
        ATMFMConversionTask();
        virtual ~ATMFMConversionTask() {};

        enum ReadType : int {
            kOnline = 0,
            kReadMFM = 1,
            kReadList = 3,
            kReadExpNameNo = 11,
            kReadType2 = 2,
            kReadType4 = 4,
            kReadType10 = 10,
            kReadType13 = 13,
            kReadType14 = 14,
        };

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    private:
        bool   checkport(int usPort);
        string _executeShellCommand(string command);

        int maxit = 0;
        int currit = 0;
        int percent = 0;
        int size_buffer;
        char *buffer;
        int oflcnt=0;

        // init parameters
        int mode = 1;
        ReadType readtype;
        int numfiles;
        int watcherPort;
        int converterPort;
        int CoBoServerPort;
        int MutantServerPort;
        int BucketSize;
        int energymethod;
        int readrw;
        int RootConvert;
        int ScalerMode;
        int d2pMode;
        int updatefast;
        int DrawWaveform;
        int cleantrack;
        int DrawTrack;
        int SkipEvents;
        int firstEventNo;
        bool IgnoreMM;
        TString mfmfilename;
        TString watcherIP;
        TString mapChanToMM;
        TString mapChanToSi;
        TString rwfilename;
        TString supdatefast;
        TString goodEventList;

        // global parameters in main.cc
        //static ATMFMFrameBuilder* histServer;
        ATMFMFrameBuilder* fFrameBuilder; // convServer
        //static int rsize_buffer;//=512;
        static string inrfname;
        static int maxinrfidx;
        //static const int maxfileno;//=2000;
        static string listinrfname[/*maxfileno*/2000];
        static string inrtname;
        //static string inoutrfname; // output root file name
        //static string listinoutrfname[/*maxfileno*/2000];
        static string maxinoutrfidx;
        //static string inoutrtname;
        //static char keyinput='r';
        static thread tgetkey;

        // parameter related to files
        int currfiles = 1;
        string infname;
        string minfname;
        string outrfname;
        //string outrtname;
        string slistinfname;
        string listinfname[/*maxfileno*/2000];
        int maxinfidx=0;
        int infidx=0;

        //TString infname;

        ifstream fFileStreamForEventLoop;

        TClonesArray *fChannelArray = nullptr;

    ClassDef(ATMFMConversionTask, 1)
};

#endif
