#include <iostream>
using namespace std;

#include "ATMFMConversionTask.hpp"

#include "GSpectra.h"
#include "GNetServerRoot.h"

ATMFMConversionTask::ATMFMConversionTask()
:LKConv("ATMFMConversionTask","")
{
}

bool ATMFMConversionTask::Init() 
{
    currfiles = 1;

    mode             = par -> GetParInt("RunMode");
    readtype         = par -> GetParInt("ReadType");
    numfiles         = par -> GetParInt("NumberofFiles");
    watcherPort      = par -> GetParInt("watcherPort");
    converterPort    = par -> GetParInt("ConverterPort");
    CoBoServerPort   = par -> GetParInt("CoBoServerPort");
    MutantServerPort = par -> GetParInt("MutantServerPort");
    BucketSize       = par -> GetParInt("BucketSize");
    energymethod     = par -> GetParInt("EnergyFindingMethod");
    readrw           = par -> GetParInt("ReadResponseWaveformFlag");
    RootConvert      = par -> GetParInt("RootConvertEnable");
    ScalerMode       = par -> GetParInt("ScalerMode");
    d2pMode          = par -> GetParInt("2pMode");
    updatefast       = par -> GetParInt("UpdateFast");
    DrawWaveform     = par -> GetParInt("DrawWaveformEnable");
    cleantrack       = par -> GetParInt("CleanTrackEnable");
    DrawTrack        = par -> GetParInt("DrawTrackEnable");
    SkipEvents       = par -> GetParInt("SkipEvents");
    firstEventNo     = par -> GetParInt("firstEventNo");
    IgnoreMM         = par -> GetParBool("IgnoreMicromegas");
    mfmfilename      = par -> GetParString("MFMFileName");
    watcherIP        = par -> GetParString("watcherIP");
    mapChanToMM      = par -> GetParString("ChanToMMMapFileName");
    mapChanToSi      = par -> GetParString("ChanToSiMapFileName");
    rwfilename       = par -> GetParString("ResponseWaveformFileName");
    supdatefast      = par -> GetParString("UpdateFast");

    outrtname = "TEvent";
    inoutrtname = "TEvent";

    if (SkipEvents==1)
        ;
    else if (SkipEvents==2)
        goodEventList = config["firstEventNo"].asString();
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
        convServer = new ATMFMFrameBuilder(converterPort);
        convServer->SetBucketSize(BucketSize);
        convServer->Init(mode,d2pMode);
        convServer->SetReadMode(mode);
        convServer->SetReadType(readtype);
        convServer->SetScaler(ScalerMode);
        convServer->Set2pMode(d2pMode);
        convServer->SetUpdateSpeed(updatefast);
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
    if(ScalerMode==0){
        outrfname = "/home/cens-alpha-00/MFMHistServer/online2.root";
        inoutrfname = "/home/cens-alpha-00/MFMHistServer/online2_et.root";
        //outrfname = "/data/grgroup/gr01/MFMHistServer/online/online.root";
        //inoutrfname = "/data/grgroup/gr01/MFMHistServer/online/online_et.root";
    }else{
        outrfname = "/home/cens-alpha-00/MFMHistServer/scaler.root";
        inoutrfname = "/home/cens-alpha-00/MFMHistServer/scaler_et.root";
        //outrfname = "/data/grgroup/gr01/MFMHistServer/online/scaler.root";
        //inoutrfname = "/data/grgroup/gr01/MFMHistServer/online/scaler_et.root";
    }

    lk_info <<"MODE: "<<mode<<"\t\tREAD TYPE:\t"<<readtype<<"\tROOTCONVERT:\t"<<RootConvert<<endl;

    if(mode==0 || mode==1)
    {
        lk_info <<"root file name="<< outrfname << ", root tree name=" << outrtname << endl;
        if(RootConvert==0) convServer->RootWOpenFile(outrfname, outrtname);
        while(keyinput=='r')
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
                lk_info << "MFM file name: " << infname << endl;
                ifstream fileMFM(infname.c_str(),std::ios::binary | std::ios::in);
                if(!fileMFM) {
                    lk_error << "Could not open input file!" << std::endl;
                    return false;
                }

                //XXX
                if(mode==1) {
                    if(RootConvert==0){
                        convServer->RootWInit();
                    }
                    if(RootConvert==1){
                        outrfname = infname+".root";
                        //outrfname.replace(5,9,"CRIBdisk");
                        //outrfname.replace(5,5,"disk01");
                        lk_info<<"root file name="<< outrfname << ", root tree name=" << outrtname << endl;
                        convServer->RootWOpenFile(outrfname, outrtname);
                        convServer->RootWInit();
                        convServer->SetRootConverter(RootConvert);
                    }
                }

                if(readtype==kReadType10){
                    size_buffer = 32;
                }else{
                    size_buffer = 512;
                }
                buffer = (char *) malloc (size_buffer);
                currit = 0;
                while(!fileMFM.eof()) {
                    fileMFM.read(buffer,size_buffer);
                    currit++;
                }
                maxit = currit;
                currit = 0;
                percent = maxit/10;
                fileMFM.close();

                lk_info << "maximum iteration number: " << maxit << endl;
                lk_info << "iteration number at 10 %: " << percent << endl;

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
                                convServer->addDataChunk(buffer,buffer+size_buffer);
                            }catch (const std::exception& e){
                                lx_cout << e.what() << endl;
                            }
                        }else if(fileMutant.gcount()>0) {
                            convServer->addDataChunk(buffer,buffer+fileMutant.gcount());
                        }
                    }
                    fileMutant.close();
                    lk_info << "Reading Mutant " << minfname << " done." << endl;
                }

                size_t const matrixSize = 4*68*512*sizeof(double);
                //size_t const matrixSize = 512;
                char *buffer = (char *) malloc (matrixSize);
                lk_info<<"READ BUFFERS, matrixSize was "<< 4*68*512*sizeof(double) <<endl;
                fileMFM.open(infname.c_str(),std::ios::binary | std::ios::in);

                //XXX
                while(!fileMFM.eof()) 
                {
                    //file2ead(buffer,size_buffer);
                    int filebuffer=0;
                    file2eekg(filebuffer, std::ios_base::cur);
                    file2ead(buffer,matrixSize);
                    filebuffer += matrixSize;
                    currit++;
                    if(!fileMFM.eof()) {
                        try {
                            //convServer->addDataChunk(buffer,buffer+size_buffer);
                            convServer->addDataChunk(buffer,buffer+matrixSize);
                        }catch (const std::exception& e){
                            lk_error << e.what() << endl;
                            return 0;
                        }
                        if(oflcnt>50){
                            oflcnt=0;
                        }
                        oflcnt++;
                        if(currit%percent==0) lk_info << Form("%d%% (%d/%d) Processed..",(100*currit/maxit),currit,maxit) << endl;
                    }else if(fileMFM.gcount()>0) {
                        try {
                            convServer->addDataChunk(buffer,buffer+fileMFM.gcount());
                        }catch (const std::exception& e){
                            lk_error << e.what() << endl;
                            return 0;
                        }
                        if(oflcnt>50){
                            oflcnt=0;
                        }
                        oflcnt++;
                        currit=maxit;
                        lk_info << Form("100% (%d/%d) Processed..",currit,maxit) << endl;
                        if(mode==1){
                            //if(RootConvert==1) convServer->RootWCloseFile();
                            convServer->RootWCloseFile();
                        }
                    }
                }

                fileMFM.close();
                if(mode==1){
                    if(readtype==kReadType14){
                        delete convServer;
                        return 0;
                    }else if(RootConvert==1 && (currit==maxit)){
                        if(numfiles==1 || currfiles>numfiles){
                            delete convServer;
                            return 0;
                        }
                    }else if(readtype==kReadType4 && (currit==maxit)){
                        delete convServer;
                        return 0;
                    }
                }
            }
            // XXX
        }

        delete convServer;
        return true;
    }

    return true;
}

void ATMFMConversionTask::Exec(Option_t*)
{
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
