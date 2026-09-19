#pragma once
#include "../stream/io.h"
#include "Environment.h"

namespace CommonLib {
    namespace system {

        typedef std::shared_ptr<class IProcess> IProcessPtr;
        class IProcess
        {
        public:
            IProcess(){}
            virtual ~IProcess(){}

            virtual int32_t GetPid() const= 0;
            virtual std::string GetFullImageName() const = 0;
            virtual bool Wait(int32_t timeoutSec) = 0;
            virtual int32_t GetExitCode()  const = 0;
            virtual bool IsRunning() const = 0;
            virtual  void SetPriority(int level) = 0;
            virtual  void Terminate() = 0;

            virtual io::TWriteReadPtr GetStdio() const = 0;
            virtual io::TWriteReadPtr GetStderr() const = 0;

            static IProcessPtr StartProcess(const std::string & filePath, const std::string& args, const std::vector<CEnvValue> &envs);


        };


    }
}