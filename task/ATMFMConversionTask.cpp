#include <iostream>
using namespace std;

#include "ATMFMConversionTask.hpp"
#include "GETChannel.hpp"

#include "GSpectra.h"
#include "GNetServerRoot.h"

#include "LKRun.hpp"

//int ATMFMConversionTask::rsize_buffer=512;
//const int ATMFMConversionTask::maxfileno=2000;

ATMFMConversionTask::ATMFMConversionTask()
:LKTask("ATMFMConversionTask","")
{
}

bool ATMFMConversionTask::Init() 
{
    fChannelArray = new TClonesArray("GETChannel");
    fRun -> RegisterBranch("GETChannel", fChannelArray);

    currfiles = 1;

    int irt = fPar -> GetParInt("ReadType");
    if (irt==0 ) readtype = kOnline;
    if (irt==1 ) readtype = kReadMFM;
    if (irt==2 ) readtype = kReadType2;
    if (irt==3 ) readtype = kReadList;
    if (irt==4 ) readtype = kReadType4;
    if (irt==10) readtype = kReadType10;
    if (irt==11) readtype = kReadExpNameNo;
    if (irt==13) readtype = kReadType13;
    if (irt==14) readtype = kReadType14;

    mode             = fPar -> GetParInt("RunMode");
    numfiles         = fPar -> GetParInt("NumberofFiles");
    watcherPort      = fPar -> GetParInt("watcherPort");
    converterPort    = fPar -> GetParInt("ConverterPort");
    CoBoServerPort   = fPar -> GetParInt("CoBoServerPort");
    MutantServerPort = fPar -> GetParInt("MutantServerPort");
    BucketSize       = fPar -> GetParInt("BucketSize");
    energymethod     = fPar -> GetParInt("EnergyFindingMethod");
    readrw           = fPar -> GetParInt("ReadResponseWaveformFlag");
    RootConvert      = fPar -> GetParInt("RootConvertEnable");
    ScalerMode       = fPar -> GetParInt("ScalerMode");
    d2pMode          = fPar -> GetParInt("2pMode");
    updatefast       = fPar -> GetParInt("UpdateFast");
    DrawWaveform     = fPar -> GetParInt("DrawWaveformEnable");
    cleantrack       = fPar -> GetParInt("CleanTrackEnable");
    DrawTrack        = fPar -> GetParInt("DrawTrackEnable");
    if (fPar -> CheckPar("SkipEvents"))     SkipEvents       = fPar -> GetParInt("SkipEvents");
    if (fPar -> CheckPar("firstEventNo"))  firstEventNo     = fPar -> GetParInt("firstEventNo");
    IgnoreMM         = fPar -> GetParBool("IgnoreMicromegas");
    mfmfilename      = fPar -> GetParString("MFMFileName");
    //watcherIP        = fPar -> GetParString("watcherIP");
    mapChanToMM      = fPar -> GetParString("ChanToMMMapFileName");
    mapChanToSi      = fPar -> GetParString("ChanToSiMapFileName");
    rwfilename       = fPar -> GetParString("ResponseWaveformFileName");
    supdatefast      = fPar -> GetParString("UpdateFast");

    //outrtname = "TEvent";
    //inoutrtname = "TEvent";

    if (SkipEvents==1)
        ;
    else if (SkipEvents==2)
        goodEventList = fPar -> GetParString("firstEventNo");
    else 
        firstEventNo = 0;

    /**
     * ## Set mode
     * - 0 : read mfm and plot
     * - 1 : convert mfm to rootwf
     * - 2 : read rootwf
     * - 3 : convert rootwf to root et 
     * - 4 : reading root et
    */
    if(mode==1)
    {
        fFrameBuilder = new ATMFMFrameBuilder(converterPort);
        fFrameBuilder->SetBucketSize(BucketSize);
        fFrameBuilder->Init(mode,d2pMode);
        fFrameBuilder->SetReadMode(mode);
        fFrameBuilder->SetReadType(readtype);
        fFrameBuilder->SetScaler(ScalerMode);
        fFrameBuilder->Set2pMode(d2pMode);
        fFrameBuilder->SetUpdateSpeed(updatefast);

        fFrameBuilder->SetChannelArray(fChannelArray);
    }
    else
        return false;

    /**
     * ## Set readtype
     * - 0 : online
     * - 1 : offline with mfm file
     * - 3 : offline with list file
     * - 11: offline with exptname and run number
    */
    /*
    if(ScalerMode==0){
        outrfname = "/home/cens-alpha-00/ejungwoo/data/online2.root";
        inoutrfname = "/home/cens-alpha-00/ejungwoo/data/online2_et.root";
        //outrfname = "/data/grgroup/gr01/MFMHistServer/online/online.root";
        //inoutrfname = "/data/grgroup/gr01/MFMHistServer/online/online_et.root";
    }else{
        outrfname = "/home/cens-alpha-00/ejungwoo/data/scaler.root";
        inoutrfname = "/home/cens-alpha-00/ejungwoo/data/scaler_et.root";
        //outrfname = "/data/grgroup/gr01/MFMHistServer/online/scaler.root";
        //inoutrfname = "/data/grgroup/gr01/MFMHistServer/online/scaler_et.root";
    }
    */

    lk_info <<"MODE: "<<mode<<"\t\tREAD TYPE:\t"<<readtype<<endl;

    //lk_info <<"root file name="<< outrfname << ", root tree name=" << outrtname << endl;

    //while(keyinput=='r')
    {
        // XXX
        if(readtype==kReadMFM || readtype==kReadType2 || readtype==kReadType10)
        {
            if(readtype==kReadMFM || readtype==kReadType2){
                if(numfiles==1){
                    infname=mfmfilename;
                    if(readtype==kReadMFM) readtype=kReadType4;
                    if(readtype==kReadType2) readtype=kOnline;
                }else if(currfiles<numfiles){
                    if(currfiles==1)  infname=mfmfilename;
                    else  infname=mfmfilename+Form(".%d",(currfiles-1));
                    currfiles++;
                }else if(currfiles==numfiles){
                    infname=mfmfilename+Form(".%d",(currfiles-1));
                    currfiles++;
                    if(readtype==kReadMFM) readtype=kReadType4;
                    if(readtype==kReadType2) readtype=kOnline;
                }
            }
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            lk_info << "MFM file name: " << infname << endl;
            ifstream fileMFM(infname.c_str(),std::ios::binary | std::ios::in);
            if(!fileMFM) {
                lk_error << "Could not open input file!" << std::endl;
                return false;
            }
            size_buffer = 512; // XXX
            buffer = (char *) malloc (size_buffer);
            int countEvents = 0;
            while(!fileMFM.eof()) {
                fileMFM.read(buffer,size_buffer);
                countEvents++;
            }
            countEvents;
            fRun -> SetNumEvents(countEvents);
            fileMFM.close();
            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            /*
            if(d2pMode==1)
            {
                minfname=mfmfilename;
                size_t f = minfname.find("/run/");
                minfname.replace(f, std::string("/run/").length(), "/run/mutant/");
                f = minfname.find("s.");
                if (f!=std::string::npos){
                    minfname.replace(f+1, (minfname.length()-f), "");
                }
                ifstream fileMutant(minfname.c_str(),std::ios::binary | std::ios::in);
                while(!fileMutant.eof()) {
                    fileMutant.read(buffer,size_buffer);
                    if(!fileMutant.eof()) {
                        try {
                            fFrameBuilder->addDataChunk(buffer,buffer+size_buffer);
                        }catch (const std::exception& e){
                            lx_cout << e.what() << endl;
                        }
                    }else if(fileMutant.gcount()>0) {
                        fFrameBuilder->addDataChunk(buffer,buffer+fileMutant.gcount());
                    }
                }
                fileMutant.close();
                lk_info << "Reading Mutant " << minfname << " done." << endl;
            }
            */


            size_t const matrixSize = 4*68*512*sizeof(double);
            //size_t const matrixSize = 512;
            char *buffer = (char *) malloc (matrixSize);
            lk_info<<"READ BUFFERS, matrixSize was "<< 4*68*512*sizeof(double) <<endl;
            fFileStreamForEventLoop.open(infname.c_str(),std::ios::binary | std::ios::in);

            //fFileStreamForEventLoop.close();
            /*
            if(mode==1){
                if(readtype==kReadType14){
                    delete fFrameBuilder;
                    return true;
                }else if(RootConvert==1 && (currit==maxit)){
                    if(numfiles==1 || currfiles>numfiles){
                        delete fFrameBuilder;
                        return true;
                    }
                }else if(readtype==kReadType4 && (currit==maxit)){
                    delete fFrameBuilder;
                    return true;
                }
            }
            */
        }
        // XXX
    }

    return true;
}

void ATMFMConversionTask::Exec(Option_t*)
{
    if (fFileStreamForEventLoop.eof()) {
        lk_warning << "end of MFM file!" << endl;
        fRun -> SignalEndOfRun();
        return;
    }

    size_t const matrixSize = 4*68*512*sizeof(double);

    int filebuffer=0;
    fFileStreamForEventLoop.seekg(filebuffer, std::ios_base::cur);
    fFileStreamForEventLoop.read(buffer,matrixSize);
    filebuffer += matrixSize;

    if(!fFileStreamForEventLoop.eof()) {
        try {
            fFrameBuilder->addDataChunk(buffer,buffer+matrixSize);
        }catch (const std::exception& e){
            lk_error << "error occured from fFrameBuilder->addDataChunk(buffer,buffer+matrixSize)" << endl;
            lx_cout << e.what() << endl;
            fRun -> SignalEndOfRun();
            return;
        }
    }
    else if(fFileStreamForEventLoop.gcount()>0) {
        try {
            fFrameBuilder->addDataChunk(buffer,buffer+fFileStreamForEventLoop.gcount());
        }catch (const std::exception& e){
            lk_error << "error occured from LAST fFrameBuilder->addDataChunk(buffer,buffer+matrixSize)" << endl;
            lx_cout << e.what() << endl;
            return;
        }
    }
}

bool ATMFMConversionTask::EndOfRun()
{
    return true;
}

bool ATMFMConversionTask::checkport(int usPort)
{
    string shellCommand;
    string pcPort;
    //sprintf(shellCommand, "netstat -lntu | awk '{print $4}' | grep ':' | cut -d \":\" -f 2 | sort | uniq | grep %hu", usPort);
    shellCommand = "netstat -an | grep -i " + to_string(usPort);
    pcPort = ":"+to_string(usPort);

    string output = _executeShellCommand(shellCommand);
    lk_info << "shell-comment > output: " << shellCommand << " > " << output << endl;

    if(output.find(pcPort) == string::npos)
        return false;
    else
        return true;
}

string ATMFMConversionTask::_executeShellCommand(string command)
{
    char buffer[256];
    string result = "";
    const char * cmd = command.c_str();
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw runtime_error("popen() failed!");

    try {
        while (!feof(pipe))
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
