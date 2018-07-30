#include "SysCfg.h"
#include "HTMLForm.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

time_t StringToDatetime(const std::string& str)
{
    char *pStr = (char*)str.data();
    tm tm_;
    int year, month, day, hour, minute, second;
    sscanf(pStr, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    time_t t_ = mktime(&tm_);
    return t_;
}

std::string DatetimeToString(time_t* pTime)
{
    char cTime[16];
    memset(cTime, 0, sizeof(cTime));

    tm* pTm = localtime(pTime);
    sprintf(cTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTm->tm_year + 1900,
            pTm->tm_mon +1,
            pTm->tm_mday,
            pTm->tm_hour,
            pTm->tm_min,
            pTm->tm_sec);
    return std::string(cTime);
}

SysCfg* SysCfg::GetInstance()
{
	static SysCfg* pIns = NULL;
	if ( NULL == pIns )
	{
		pIns = new SysCfg;
	}
	return pIns;
}

SysCfg::SysCfg()
{
    cloudPlatformPort_ = 80;
    isDuringClass_ = false;
    loggingLevel_ = 0;
    frame_ = 0;
    isDisplay_ = true;
    courseId_ = -1;
    startTime_ = 0;
    endTime_ = 0;
    pHttpClient_ = new HttpClient(cloudPlatformHost_, cloudPlatformPort_);
    thread_ = NULL;
}

SysCfg::~SysCfg()
{
    delete pHttpClient_;
    EmptyMemberList();
}

void SysCfg::SetLoggingLevel(int level)
{
	mutex_.lock();
	loggingLevel_ = level;
	mutex_.unlock();
}

int SysCfg::GetLoggingLevel()
{
	return loggingLevel_;
}
	
void SysCfg::SetCameraIP(const std::string& ip)
{
	mutex_.lock();
	cameraIP_ = ip;
	mutex_.unlock();
}

std::string& SysCfg::GetCameraIP()
{
	return cameraIP_;
}
	
void SysCfg::SetFrame(int frame)
{
	mutex_.lock();
	frame_ = frame;
	mutex_.unlock();
}

int SysCfg::GetFrame()
{
	return frame_;
}
	
void SysCfg::SetVideo(const std::string& video)
{
	mutex_.lock();
	video_ = video;
	mutex_.unlock();
}

std::string& SysCfg::GetVideo()
{
	return video_;
}

void SysCfg::EnableDispaly()
{
	mutex_.lock();
	isDisplay_ = true;
	mutex_.unlock();
}

void SysCfg::DisenableDispaly()
{
    mutex_.lock();
	isDisplay_ = false;
	mutex_.unlock();
}

bool SysCfg::IsDisplay()
{
	return isDisplay_;
}

void SysCfg::SetCloudPlatformHost(const std::string& host)
{
    mutex_.lock();
    cloudPlatformHost_ = host;
    mutex_.unlock();
}

std::string& SysCfg::GetCloudPlatformHost()
{
    return cloudPlatformHost_;
}

void SysCfg::SetCloudPlatformPort(int port)
{
    mutex_.lock();
    cloudPlatformPort_ = port;
    mutex_.unlock();
}

int SysCfg::GetCloudPlatformPort()
{
    return cloudPlatformPort_;
}

void SysCfg::SetCourseID(int id)
{
    mutex_.lock();
    courseId_ = id;
    mutex_.unlock();
}

int SysCfg::GetCourseID()
{
    return courseId_;
}

void SysCfg::SetStartTime(long long time)
{
    mutex_.lock();
    startTime_ = time;
    mutex_.unlock();
}

long long SysCfg::GetStartTime()
{
    return startTime_;
}

void SysCfg::SetEndTime(long long time)
{
    mutex_.lock();
    endTime_ = time;
    mutex_.unlock();
}

long long SysCfg::GetEndTime()
{
    return endTime_;
}

void SysCfg::RequestMemberList()
{
    //TODO
    std::string path = "";
    std::string rlt;
    HTMLForm form;
    form.Add("CourseID", courseId_);
    pHttpClient_->Post(path, rlt, form);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(rlt, root, false)){ return ; }
    if ( root["rlt"].asInt() == 0 ){
        EmptyMemberList();
        Json::Value data = root["data"];
        int count = root["data"].size();
        for ( int i = 0; i < count; ++i ){
            Member* pMember = new Member;
            pMember->id = data[i][0].asInt();
            pMember->name = data[i][1].asString();
            pMember->image = data[i][2].asString();
            pMember->type = (MemberType)(data[i][3].asInt());
            memberList_.push_back(pMember);
        }
    }

    thread_ = new std::thread(DownloadProc);
}

void SysCfg::RequestCourse()
{
    //获取机器标识码
    //TODO
    int machinId = 0;

    //请求课程
    //TODO
    //int HttpClient::Post(const std::string& path, std::string& rlt, const HTMLForm &form, const std::string& fields)
    std::string path = "";
    std::string rlt;
    HTMLForm form;
    form.Add("MachineId", machinId);
    pHttpClient_->Post(path, rlt, form);

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(rlt, root, false)){ return ; }
    if ( root["rlt"].asInt() == 0 ){
        Json::Value data = root["data"];
        courseId_ = data["CourseID"].asInt();
        startTime_ = StringToDatetime(data["StartTime"].asString());
        endTime_ = StringToDatetime(data["EndTime"].asString());
    }
}

MemberList &SysCfg::GetMemberList()
{
    return memberList_;
}

void SysCfg::WriteCfgFile()
{
    std::ofstream ofs;
    ofs.open(SYS_CFG_FILE, std::ofstream::out | std::ofstream::trunc);
    if (!ofs.is_open())
    {
        //qDebug() << "Open Config File Failed While Write";
        return ;
    }

    Json::FastWriter writer;
    cfgRoot_.clear();
    cfgRoot_["CloudPlatformHost"] = cloudPlatformHost_;
    cfgRoot_["CloudPlatformPort"] = cloudPlatformPort_;
    cfgRoot_["LoggingLevel"] = loggingLevel_;
    cfgRoot_["CameraIP"] = cameraIP_;
    cfgRoot_["Frame"] = frame_;
    cfgRoot_["Video"] = video_;
    cfgRoot_["IsDisplay"] = isDisplay_;
    cfgRoot_["CourseID"] = courseId_;
    cfgRoot_["StartTime"] = DatetimeToString(&startTime_);
    cfgRoot_["EndTime"] = DatetimeToString(&endTime_);

    std::string jsonStr = writer.write(cfgRoot_);
    ofs << jsonStr;
    ofs.close();
}

void SysCfg::DownloadProc()
{
    mutex_.lock();
    if (opendir(IMAGE_PATH) == NULL){
        mkdir(IMAGE_PATH, S_IRUSR|S_IWUSR|S_IXUSR|S_IRWXO);
    }
    MemberList::iterator it = memberList_.begin();
    for( ; it != memberList_.end(); ++it )
    {
        std::size_t found = *it->image.find_last_of("/\\");
        std::string fileName = IMAGE_PATH;
        fileName += *it->image.substr(found+1);
        pHttpClient_->Download(*it->image, fileName);
    }
    mutex_.unlock();
}

void SysCfg::ReadCfgFile()
{
    std::ifstream ifs;
    ifs.open(SYS_CFG_FILE, std::ifstream::in);
    if (!ifs.is_open())
    {
        //qDebug() << "Open Config File Failed While Read!";
        return ;
    }

    Json::Reader reader;
    if (!reader.parse(ifs, cfgRoot_, false))
    {
        //"Parse Config File Failed!"
        ifs.close();
        return ;
    }
    ifs.close();

    cloudPlatformHost_ = cfgRoot_["CloudPlatformHost"].asString();
    cloudPlatformPort_ = cfgRoot_["CloudPlatformPort"].asInt();
    loggingLevel_ = cfgRoot_["LoggingLevel"].asInt();
    cameraIP_ = cfgRoot_["CameraIP"].asString();
    frame_ = cfgRoot_["Frame"].asInt();
    video_ = cfgRoot_["Video"].asString();
    isDisplay_ = cfgRoot_["IsDisplay"].asBool();
    courseId_ = cfgRoot_["CourseID"].asInt();
    std::string startTime = cfgRoot_["StartTime"].asString();
    startTime_ = StringToDatetime(startTime);
    std::string endTime = cfgRoot_["EndTime"].asString();
    endTime_ = StringToDatetime(endTime);
}

void SysCfg::EmptyMemberList()
{
    MemberList::iterator it = memberList_.begin();
    for ( ; it != memberList_.end(); ++it ){
        delete *it;
    }
}
