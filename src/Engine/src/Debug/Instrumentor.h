//
// Created by alexl on 20.05.2023.
//

#pragma once

//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in
//     profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use things like __FUNCSIG__ for the profile
// name.
//
#include "Core/Casts.h"
#include "version"

#ifdef BEE_ENABLE_PROFILING
#ifdef __cpp_lib_source_location
#include "source_location"
#else
#include "source_location.h"
#endif
#define BEE_DEBUG_START_PROFILING_SESSION(name, filename)                                                              \
    ::BeeEngine::Debug::Instrumentor::Get().BeginSession(name, filename)
#define BEE_DEBUG_END_PROFILING_SESSION() ::BeeEngine::Debug::Instrumentor::Get().EndSession()
#define BEE_PROFILE_SCOPE(name) ::BeeEngine::Debug::InstrumentationTimer timer##__LINE__(name)
#define BEE_PROFILE_FUNCTION() BEE_PROFILE_SCOPE(std::source_location::current().function_name())
#else
#define BEE_DEBUG_START_PROFILING_SESSION(name, filename)
#define BEE_DEBUG_END_PROFILING_SESSION()
#define BEE_PROFILE_SCOPE(name)
#define BEE_PROFILE_FUNCTION()
#endif

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>

#include <thread>

namespace BeeEngine
{
    namespace Debug
    {
        struct ProfileResult
        {
            std::string Name;
            long long Start, End;
            uint32_t ThreadID;
        };

        struct InstrumentationSession
        {
            std::string Name;
        };

        class Instrumentor
        {
        private:
            InstrumentationSession* m_CurrentSession;
            std::ofstream m_OutputStream;
            int m_ProfileCount;

        public:
            Instrumentor() : m_CurrentSession(nullptr), m_ProfileCount(0) {}
            inline bool IsSessionActive() { return m_CurrentSession != nullptr; }
            void BeginSession(const std::string& name, const std::string& filepath = "results.json")
            {
#if defined(LINUX)
                m_OutputStream.open("/tmp/beeengine/" + filepath);
#else
                m_OutputStream.open(filepath);
#endif                
                WriteHeader();
                m_CurrentSession = new InstrumentationSession{name};
            }

            void EndSession()
            {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
                m_ProfileCount = 0;
            }

            void WriteProfile(const ProfileResult& result)
            {
                if (m_ProfileCount++ > 0)
                    m_OutputStream << ",";

                std::string name = result.Name;
                std::replace(name.begin(), name.end(), '"', '\'');

                m_OutputStream << "{";
                m_OutputStream << "\"cat\":\"function\",";
                m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
                m_OutputStream << "\"name\":\"" << name << "\",";
                m_OutputStream << "\"ph\":\"X\",";
                m_OutputStream << "\"pid\":0,";
                m_OutputStream << "\"tid\":" << result.ThreadID << ",";
                m_OutputStream << "\"ts\":" << result.Start;
                m_OutputStream << "}";

                m_OutputStream.flush();
            }

            void WriteHeader()
            {
                m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
                m_OutputStream.flush();
            }

            void WriteFooter()
            {
                m_OutputStream << "]}";
                m_OutputStream.flush();
            }

            static Instrumentor& Get()
            {
                static Instrumentor instance;
                return instance;
            }
        };

        class InstrumentationTimer
        {
        public:
            InstrumentationTimer(const char* name) : m_Name(name), m_Stopped(false)
            {
                m_StartTimepoint = std::chrono::high_resolution_clock::now();
            }

            ~InstrumentationTimer()
            {
                if (!m_Stopped)
                    Stop();
            }

            void Stop()
            {
                if (!Instrumentor::Get().IsSessionActive())
                    return;
                auto endTimepoint = std::chrono::high_resolution_clock::now();

                auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint)
                                 .time_since_epoch()
                                 .count();
                auto end =
                    std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

                uint32_t threadID =
                    narrow_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
                Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

                m_Stopped = true;
            }

        private:
            const char* m_Name;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
            bool m_Stopped;
        };
    } // namespace Debug
} // namespace BeeEngine
