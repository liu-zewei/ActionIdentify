#ifndef _SYSCFG_H_
#define _SYSCFG_H_

#include <ctime>
#include <list>
#include <mutex>
#include <thread>
#include "json/json.h"
#include "HttpClient.h"

#define SYS_CFG_FILE "syscfg.json"
#define IMAGE_PATH "./image_path/"

enum MemberType
{
    MemberType_Teacher = 1,
    MemberType_Student
};

struct Member
{
    int         id;
    std::string name;
    std::string image;
    MemberType  type;
};

typedef std::list<Member*> MemberList;

time_t StringToDatetime(const std::string& str);
std::string DatetimeToString(time_t* pTime);

class SysCfg
{
public:
    static SysCfg* GetInstance();
	virtual ~SysCfg();
	
    void SetLoggingLevel(int level);
    int GetLoggingLevel();
	
    void SetCameraIP(const std::string& ip);
    std::string& GetCameraIP();
	
    void SetFrame(int frame);
    int GetFrame();
	
    void SetVideo(const std::string& video);
    std::string& GetVideo();
	
    void EnableDispaly();
    void DisenableDispaly();
    bool IsDisplay();

    void SetCloudPlatformHost(const std::string& host);
    std::string& GetCloudPlatformHost();

    void SetCloudPlatformPort(int port);
    int GetCloudPlatformPort();

    void SetCourseID(int id);
    int GetCourseID();

    void SetStartTime(long long time);
    long long GetStartTime();

    void SetEndTime(long long time);
    long long GetEndTime();

    MemberList& GetMemberList();

    std::string& GetMemberName(int stuId);

    void DownloadProc();

private:
	SysCfg();
    void RequestMemberList();
    void RequestCourse();
    void WriteCfgFile();
    void ReadCfgFile();
    void EmptyMemberList();
	
private:
	std::mutex mutex_;
    std::string cloudPlatformHost_;
    int cloudPlatformPort_;
    bool isDuringClass_;
    Json::Value cfgRoot_;
	int loggingLevel_;
	std::string cameraIP_;
	int frame_;
	std::string video_;
	bool isDisplay_;
    int courseId_;
    time_t startTime_;
    time_t endTime_;
    MemberList memberList_;
    HttpClient* pHttpClient_;
    std::thread *thread_;
};

#endif //_SYSCFG_H_
