
/**
 * @file EntryPoint.h
 * @brief Contains the entry point for the BeeEngine application.
 */

#pragma once

#include "Application.h"

namespace BeeEngine{
    /**
     * @class ApplicationArgs
     * @brief Represents the arguments passed to the application.
     */
    class ApplicationArgs
    {
    public:
        /**
         * @brief Gets the arguments passed to the application.
         * @return A constant reference to the vector of arguments.
         */
        const std::vector<UTF8String>& GetArguments() const
        {
            return m_Args;
        }

        /**
         * @brief Gets the number of arguments passed to the application.
         * @deprecated Use GetArguments() instead.
         * @return The number of arguments.
         */
        [[deprecated("Use GetArgs() instead")]]
        int GetArgc() const
        {
            return argc;
        }

        /**
         * @brief Gets the array of arguments passed to the application.
         * @deprecated Use GetArguments() instead.
         * @return The array of arguments.
         */
        [[deprecated("Use GetArgs() instead")]]
        char** GetArgv() const
        {
            return argv;
        }

        /**
         * @brief Constructs an ApplicationArgs object.
         * @param argc The number of arguments.
         * @param argv The array of arguments.
         */
        ApplicationArgs(int argc, char** argv)
        : argc(argc), argv(argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                m_Args.emplace_back(argv[i]);
                BeeExpects(IsValidString(m_Args.back()));
            }
        }
    private:
        int argc;
        char** argv;
        std::vector<UTF8String> m_Args;
    };
    /**
     * @brief Creates an instance of the application.
     *
     * This function is responsible for creating an instance of the user application using the provided arguments.
     * The user application must implement this function.
     *
     * @param args The arguments to be passed to the application.
     * @return A non-null pointer to the created application instance.
     */
    extern gsl::not_null<Application*> CreateApplication(const ApplicationArgs& args);
}

#ifdef __cplusplus
//extern "C" int main(int argc, char* argv[]);
#endif

