//#define MODE_0234
//#define READTYPE_NOT_1

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
        convServer = new HistServer(converterPort);
        convServer->SetBucketSize(BucketSize);
        convServer->Init(mode,d2pMode);
        convServer->SetReadMode(mode);
        convServer->SetReadType(readtype);
        convServer->SetScaler(ScalerMode);
        convServer->Set2pMode(d2pMode);
        convServer->SetUpdateSpeed(updatefast);
    }
#ifdef MODE_0234
    else if(mode==0)
    {
        while(checkport(watcherPort))
        {
            lk_waring << "The Histogram Server Port No. " << watcherPort << " is being used. Checking " << (watcherPort+1) << "." << endl;
            watcherPort++;
        }
        lk_info << "The Histogram Server Port No. " << watcherPort << " is a good one. Please use this port number in vigru." << endl;
        convServer = new HistServer(watcherPort);
        convServer->SetBucketSize(BucketSize);
        convServer->Init(mode,d2pMode);
        convServer->SetReadMode(mode);
        convServer->SetReadType(readtype);
        convServer->SetScaler(ScalerMode);
        convServer->Set2pMode(d2pMode);
        convServer->SetHistMode();
        convServer->SetUpdateSpeed(updatefast);
        convServer->ReadMapChanToMM(mapChanToMM);
        convServer->ReadMapChanToSi(mapChanToSi);
        convServer->ReadMapChanToX6();
        convServer->SetIgnoreMM(IgnoreMM);
        convServer->SetDrawWaveform(DrawWaveform);
        convServer->SetCleanTrack(cleantrack);
        convServer->SetDrawTrack(DrawTrack);
        convServer->SetSkipEvents(SkipEvents);
        if(SkipEvents==1) convServer->SetfirstEventNo(firstEventNo);
        if(SkipEvents==2) convServer->ReadGoodEventList(goodEventList);
        convServer->SetEnergyMethod(energymethod);
        convServer->SetReadRF(readrw);
        if(readrw!=1){
            lk_error << "ReadResponseWaveformFlag should be set to 1." << endl;
            return false;
        }
    }
    else if(mode==2)
    {
        while(checkport(watcherPort)){
            lk_warning << "The Histogram Server Port No. " << watcherPort << " is being used. Checking " << (watcherPort+1) << "." << endl;
            watcherPort++;
        }
        lk_info << "The Histogram Server Port No. " << watcherPort << " is a good one. Please use this port number in vigru." << endl;
        histServer = new HistServer(watcherPort);
        histServer->SetBucketSize(BucketSize);
        histServer->Init(mode,d2pMode);
        histServer->SetReadMode(mode);
        histServer->SetReadType(readtype);
        histServer->SetScaler(ScalerMode);
        histServer->Set2pMode(d2pMode);
        histServer->SetHistMode();
        histServer->SetUpdateSpeed(updatefast);
        histServer->ReadMapChanToMM(mapChanToMM);
        histServer->ReadMapChanToSi(mapChanToSi);
        histServer->ReadMapChanToX6();
        histServer->SetIgnoreMM(IgnoreMM);
        histServer->SetDrawWaveform(DrawWaveform);
        histServer->SetCleanTrack(cleantrack);
        histServer->SetDrawTrack(DrawTrack);
        histServer->SetSkipEvents(SkipEvents);
        if(SkipEvents==1) histServer->SetfirstEventNo(firstEventNo);
        if(SkipEvents==2) histServer->ReadGoodEventList(goodEventList);
        histServer->SetEnergyMethod(energymethod);
        histServer->SetReadRF(readrw);
        if(readrw>0){
            histServer->ReadResponseWaveform(rwfilename);
        }else{
            histServer->SetResponseSample(0,295270,90,60,0,0,0,37,15,15,1); // Si (type,evtno,buckcut,buckwidth,cobo,asad,aget,chan,decrep,deciter,decboost)
            histServer->SetResponseSample(2,38509,100,220,1,1,0,16,20,20,1); // IC (type,evtno,buckcut,buckwidth,cobo,asad,aget,chan,decrep,deciter,decboost)
        }
    }
    else if(mode==3)
    {
        convServer = new HistServer(converterPort-10);
        convServer->SetBucketSize(BucketSize);
        convServer->Init(mode,d2pMode);
        convServer->SetReadMode(mode);
        convServer->SetReadType(readtype);
        convServer->SetScaler(ScalerMode);
        convServer->Set2pMode(d2pMode);
        convServer->SetUpdateSpeed(updatefast);
        convServer->SetEnergyMethod(energymethod);
        convServer->SetReadRF(readrw);
        if(readrw>0){
            convServer->ReadResponseWaveform(rwfilename);
        }else{
            convServer->SetResponseSample(0,295270,90,60,0,0,0,37,15,15,1); // Si (type,evtno,buckcut,buckwidth,cobo,asad,aget,chan,decrep,deciter,decboost)
            convServer->SetResponseSample(2,38509,100,220,1,1,0,16,20,20,1); // IC (type,evtno,buckcut,buckwidth,cobo,asad,aget,chan,decrep,deciter,decboost)
        }
    }
    else if(mode==4)
    {
        while(checkport(watcherPort)){
            lk_warning << "The Histogram Server Port No. " << watcherPort << " is being used. Checking " << (watcherPort+1) << "." << endl;
            watcherPort++;
        }
        lk_info << "The Histogram Server Port No. " << watcherPort << " is a good one. Please use this port number in vigru." << endl;
        histServer = new HistServer(watcherPort);
        histServer->SetBucketSize(BucketSize);
        histServer->Init(mode,d2pMode);
        histServer->SetReadMode(mode);
        histServer->SetHistMode();
        histServer->SetReadType(readtype);
        histServer->SetScaler(ScalerMode);
        histServer->Set2pMode(d2pMode);
        histServer->SetUpdateSpeed(updatefast);
        histServer->ReadMapChanToMM(mapChanToMM);
        histServer->ReadMapChanToSi(mapChanToSi);
        histServer->ReadMapChanToX6();
        histServer->SetIgnoreMM(IgnoreMM);
        histServer->SetDrawWaveform(DrawWaveform);
        histServer->SetCleanTrack(cleantrack);
        histServer->SetDrawTrack(DrawTrack);
        histServer->SetSkipEvents(SkipEvents);
        if(SkipEvents==1) histServer->SetfirstEventNo(firstEventNo);
        if(SkipEvents==2) histServer->ReadGoodEventList(goodEventList);
    }
#endif

    /**
     * ## Set readtype
     * - 0 : online
     * - 1 : offline with mfm file
     * - 3 : offline with list file
     * - 11: offline with exptname and run number
    */
#ifdef READTYPE_NOT_1
    if(readtype==kReadExpNameNo)
    {
        string delimiter = "/";
        size_t pos = mfmfilename.find(delimiter);
        if (pos==std::string::npos){
            lk_error << "You should input an experiment name and runfile name separated by '/' in mfmfilename." << endl;
            lk_error << "The runfile name should follow a format of 'run_#### (e.g. run_0102)." << endl;
            return false;
        }else{
            string expname = mfmfilename.substr(0,pos);
            string runno = mfmfilename.substr(pos+1,mfmfilename.length()-pos);
            pos = runno.find("_");
            if (pos==std::string::npos){
                lk_error << "The runfile name should follow a format of 'run_#### (e.g. run_0123)." << endl;
                return false;
            }
            string runpath="/mnt/CRIBdisk2/o14apf17/ganacq_manip/"+expname+"/acquisition/run/";
            //string runpath="/hdfs/data/"+expname+"/acquisition/run/";
            lk_info << runpath << " " << expname << " " << runno << endl;
            TSystemDirectory dir(runpath.c_str(),runpath.c_str());
            TList *files = dir.GetListOfFiles();
            files->Sort();
            if(files){
                TSystemFile *file;
                TString fname;
                TIter next(files);
                ofstream ofrunlist("temprunlist.txt",std::ofstream::out);
                while((file=(TSystemFile*)next())){
                    fname = file->GetName();
                    if(mode==0 || mode==1 || mode==2 || mode==3){
                        if(!file->IsDirectory() && fname.BeginsWith("run") && fname.Contains(runno.c_str(),TString::kIgnoreCase) && !fname.Contains(".root",TString::kIgnoreCase)){
                            ofrunlist << runpath << fname.Data() << endl;
                        }
                    }else if(mode==4){
                        if(!file->IsDirectory() && fname.BeginsWith("run") && fname.Contains(runno.c_str(),TString::kIgnoreCase) && fname.Contains("_et.root",TString::kIgnoreCase)){
                            fname = fname.ReplaceAll("_et.root","");
                            ofrunlist << runpath << fname.Data() << endl;
                        }
                    }
                }
                ofrunlist.close();
            }
            readtype=kReadList;
            mfmfilename="temprunlist.txt";
        }
    }
    if(readtype==kReadList || readtype==kReadType13)
    {
        infname=mfmfilename;
        lk_info << "MFM list file name: " << infname << endl;
        ifstream fileMFMList(infname.data());
        if(fileMFMList.fail()==true){
            lk_error<<"The MFM list file wasn't opened!"<<endl;
            return false;
        }
        while(fileMFMList.good()){
            fileMFMList >> slistinfname;
            listinfname[maxinfidx]=slistinfname;
            maxinfidx++;
        }
        fileMFMList.close();
        maxinfidx -= 2;
    }
#endif

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

    outrtname = "TEvent";
    inoutrtname = "TEvent";

    int sock; /* Socket descriptor */
    struct sockaddr_in servAddr; /* server address */
    int size_buffer;
    int size_to_recv;
    char *buffer;
    int received_data;
    int total_received_data;
    int coboflag=0;
    int onlcnt=0;
    int oflcnt=0;

    lk_info <<"MODE: "<<mode<<"\t\tREAD TYPE:\t"<<readtype<<"\tROOTCONVERT:\t"<<RootConvert<<endl;

    if(mode==0 || mode==1)
    {
        lk_info <<"root file name="<< outrfname << ", root tree name=" << outrtname << endl;
        if(RootConvert==0) convServer->RootWOpenFile(outrfname, outrtname);
        while(keyinput=='r')
        {
            // XXX >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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
#ifdef READTYPE_NOT_1
                else if(readtype==kReadType10)
                {
                    infname=mfmfilename;
                    size_t f = infname.find("/run/");
                    infname.replace(f, std::string("/run/").length(), "/run/mutant/");
                    f = infname.find("s.");
                    if (f!=std::string::npos){
                        infname.replace(f+1, (infname.length()-f), "");
                    }
                    cout << infname << endl;
                    readtype=kReadType14;
                }
#endif
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
#ifdef MODE_0234
                else if(mode==0){
                    convServer->RootWInit();
                }
#endif

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
            // XXX <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef READTYPE_NOT_1
            else if(readtype==kOnline){
                if(RootConvert==0 && onlcnt==0){
                    convServer->RootWInit();
                }
                sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (sock == 0) {
                    printf ("socket call failed\n");
                    return 1;
                }

                memset(&servAddr, 0, sizeof(servAddr));
                servAddr.sin_family = AF_INET;             /* Internet address family */
                servAddr.sin_addr.s_addr = inet_addr(watcherIP.c_str());   /* Server IP address */
                if(ScalerMode==0){
                    if(coboflag!=0 || (coboflag==0 && size_buffer==0)){
                        servAddr.sin_port = htons(MutantServerPort); /* Server port */
                        coboflag=0;
                    }else{
                        servAddr.sin_port = htons(CoBoServerPort); /* Server port */
                        coboflag++;
                    }
                }else{
                    servAddr.sin_port = htons(MutantServerPort); /* Server port */
                }
                if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
                    printf ("connect call failed\n");
                    return 2;
                }

                recv (sock, &size_buffer, sizeof (int), 0);
                buffer = (char *) malloc (size_buffer);
                total_received_data = 0;
                size_to_recv = size_buffer;


                while (total_received_data < size_buffer) {
                    received_data = recv (sock, buffer, size_to_recv, 0);
                    total_received_data += received_data;
                    if (received_data == -1) {
                        printf ("errno %d\n",errno);
                        perror (NULL);
                        return 3;
                    }

                    buffer += received_data;
                    size_to_recv -= received_data;

                }
                close (sock);

                if(size_buffer>4) convServer->addDataChunk(buffer-total_received_data,buffer);

                if(onlcnt>50){
                    cout << "Reading MFM online..." << endl;
                    onlcnt=0;
                }
                onlcnt++;

            }
            else if(readtype==kReadList || readtype==kReadType13){
                infname=listinfname[infidx];
                cout << "MFM file name: " << infname << endl;
                ifstream fileFromList(infname.c_str(),std::ios::binary | std::ios::in);
                if(!fileFromList) {
                    std::cout << "Could not open input file!" << std::endl;
                    return 0;
                }
                if(mode==0){
                    convServer->RootWInit();
                }else if(mode==1){
                    if(RootConvert==0){
                        convServer->RootWInit();
                    }
                    if(RootConvert==1){
                        outrfname = infname+".root";
                        if(atoi(&outrfname[13])>=1) outrfname.replace(13,1,"");
                        cout << "outrfname: " << outrfname << endl;
                        //outrfname.replace(5,5,"disk01");
                        convServer->RootWOpenFile(outrfname, outrtname);
                        convServer->RootWInit();
                        convServer->SetRootConverter(RootConvert);
                    }
                }
                buffer = (char *) malloc (size_buffer);
                size_buffer = 512;
                currit = 0;
                while(!fileFromList.eof()) {
                    fileFromList.read(buffer,size_buffer);
                    currit++;
                }
                maxit = currit;
                currit = 0;
                percent = maxit/10;
                fileFromList.close();

                if(d2pMode==1){
                    minfname=listinfname[infidx];
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
                                cout << e.what() << endl;
                            }
                        }else if(fileMutant.gcount()>0) {
                            convServer->addDataChunk(buffer,buffer+fileMutant.gcount());
                        }
                    }
                    fileMutant.close();
                    cout << "Reading Mutant " << minfname << " done." << endl;
                }

                ifstream file3(infname.c_str(),std::ios::binary | std::ios::in);
                while(!file3.eof()) {
                    file3.read(buffer,size_buffer);
                    currit++;
                    if(!file3.eof()) {
                        convServer->addDataChunk(buffer,buffer+size_buffer);
                        if(oflcnt>50){
                            oflcnt=0;
                        }
                        oflcnt++;
                        //if(currit%percent==0 && maxit!=0) cout << Form("Reading %d/%d file: %d%% (%d/%d) Processed..",(infidx+1),(maxinfidx+1),(100*currit/maxit),currit,maxit) << endl;
                    } else if(file3.gcount()>0) {
                        convServer->addDataChunk(buffer,buffer+file3.gcount());
                        if(oflcnt>50){
                            oflcnt=0;
                        }
                        oflcnt++;
                        currit=maxit;
                        cout << Form("Reading %d/%d file: 100% (%d/%d) Processed..",(infidx+1),(maxinfidx+1),currit,maxit) << endl;
                        if(mode==1){
                            //if(RootConvert==1) convServer->RootWCloseFile();
                            convServer->RootWCloseFile();
                        }
                    }
                }
                file3.close();
                if(readtype==kReadList && infidx==maxinfidx){
                    readtype=kReadType4;
                }else if(readtype==kReadType13 && infidx==maxinfidx){
                    readtype=kReadType14;
                }else{
                    infidx++;
                }
            }
            if(mode==1){
                if(readtype==kReadType14){
                    delete convServer;
                    return 0;
                }else if(readtype==kReadType4 && RootConvert==1){
                    delete convServer;
                    return 0;
                }
            }
#endif
        }

        delete convServer;
        return 0;
    }
#ifdef MODE_0234
    else if(mode==2){
        if(readtype==kOnline){
            inrfname = outrfname;
            inrtname = outrtname;
            tupdatehistogram=thread(updatehistogram);
        }else if(readtype==kReadMFM || readtype==kReadType2){
            if(RootConvert==0){
                inrfname = outrfname;
            }
            if(RootConvert==1){
                if(numfiles==1){
                    inrfname = mfmfilename+".root";
                    //inrfname.replace(5,5,"disk01");
                }else{
                    for(currfiles=0;currfiles<numfiles;currfiles++){
                        if(currfiles==0) inrfname=mfmfilename+".root";
                        else inrfname=mfmfilename+Form(".%d.root",currfiles);
                        //inrfname.replace(5,5,"disk01");
                        listinrfname[currfiles]=inrfname;
                    }
                    maxinrfidx=numfiles;
                    inrfname = "list";
                }
            }
            inrtname = outrtname;
            tupdatehistogram=thread(updatehistogram);
        }else if(readtype==kReadList || readtype==kReadType13){
            if(RootConvert==0){
                inrfname = outrfname;
            }
            if(RootConvert==1){
                inrfname = "list";
                for(infidx=0;infidx<(maxinfidx+1);infidx++){
                    //listinfname[infidx].replace(5,5,"disk01");
                    listinrfname[infidx]=listinfname[infidx]+".root";
                }
                maxinrfidx=maxinfidx+1;
            }
            inrtname = outrtname;
            tupdatehistogram=thread(updatehistogram);
        }

        while(keyinput=='r'){
        }
    }
    else if(mode==3){
        if(readtype==kOnline){
            inrfname = outrfname;
            inrtname = outrtname;
            tupdateenergytime=thread(updateenergytime);
        }else if(readtype==kReadMFM || readtype==kReadType2){
            if(RootConvert==0){
                inrfname = outrfname;
                inrtname = outrtname;
            }
            if(RootConvert==1){
                if(numfiles==1){
                    inrfname = mfmfilename+".root";
                    inoutrfname = mfmfilename+"_et.root";
                    //inrfname.replace(5,5,"disk01");
                }else{
                    for(currfiles=0;currfiles<numfiles;currfiles++){
                        if(currfiles==0) inrfname=mfmfilename+".root";
                        else inrfname=mfmfilename+Form(".%d.root",currfiles);
                        //inrfname.replace(5,5,"disk01");
                        listinrfname[currfiles]=inrfname;
                        if(currfiles==0) inoutrfname=mfmfilename+"_et.root";
                        else inoutrfname=mfmfilename+Form(".%d_et.root",currfiles);
                        //inrfname.replace(5,5,"disk01");
                        listinoutrfname[currfiles]=inoutrfname;
                    }
                    maxinrfidx=numfiles;
                    inrfname = "list";
                    inoutrfname = "list";
                }
            }
            inrtname = outrtname;
            tupdateenergytime=thread(updateenergytime);
        }else if(readtype==kReadList || readtype==kReadType13){
            if(RootConvert==0){
                inrfname = outrfname;
                inrtname = outrtname;
            }
            if(RootConvert==1){
                inrfname = "list";
                inoutrfname = "list";
                for(infidx=0;infidx<(maxinfidx+1);infidx++){
                    //listinfname[infidx].replace(5,5,"disk01");
                    listinrfname[infidx]=listinfname[infidx]+".root";
                    listinoutrfname[infidx]=listinfname[infidx]+"_et.root";
                }
                maxinrfidx=maxinfidx+1;
            }
            inrtname = outrtname;
            tupdateenergytime=thread(updateenergytime);
        }

        while(keyinput=='r'){
        }
        return 0;
    }
    else if(mode==4){
        if(readtype==kOnline){
            inrfname = inoutrfname;
            inrtname = inoutrtname;
            tupdatehistogram=thread(updatehistogram);
        }else if(readtype==kReadMFM || readtype==kReadType2){
            if(RootConvert==0){
                inrfname = inoutrfname;
            }
            if(RootConvert==1){
                if(numfiles==1){
                    inrfname = mfmfilename+"_et.root";
                    //inrfname.replace(5,5,"disk01");
                }else{
                    for(currfiles=0;currfiles<numfiles;currfiles++){
                        if(currfiles==0) inrfname=mfmfilename+".root";
                        else inrfname=mfmfilename+Form(".%d_et.root",currfiles);
                        //inrfname.replace(5,5,"disk01");
                        listinrfname[currfiles]=inrfname;
                    }
                    maxinrfidx=numfiles;
                    inrfname = "list";
                }
            }
            inrtname = outrtname;
            tupdatehistogram=thread(updatehistogram);
        }else if(readtype==kReadList || readtype==kReadType13){
            if(RootConvert==0){
                inrfname = inoutrfname;
                inrtname = inoutrtname;
            }
            if(RootConvert==1){
                inrfname = "list";
                for(infidx=0;infidx<(maxinfidx+1);infidx++){
                    //listinfname[infidx].replace(5,5,"disk01");
                    listinrfname[infidx]=listinfname[infidx]+"_et.root";
                }
                maxinrfidx=infidx+1;
            }
            inrtname = outrtname;
            tupdatehistogram=thread(updatehistogram);
        }

        while(keyinput=='r'){
        }
    }
#endif

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
