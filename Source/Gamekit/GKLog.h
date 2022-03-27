#pragma once

#include "Logging/LogMacros.h"

// extracted from the UE log macro in Engine\Source\Runtime\Core\Public\Logging\LogMacros.h
// UE_LOG macro generates the function below as a lambda that gets executed
#define GENERATE_LOG_FUN(Verbosity)\
    template<typename Category, typename Format, typename... Args>\
    void Log_##Verbosity(const char *    Filename,                                                                   \
                int             Line,                                                                                \
                const Category &LCategoryName,                                                                       \
                const Format &  LFormat,                                                                             \
                const Args &...UE_LOG_Args)\
    { \
	    TRACE_LOG_MESSAGE(LCategoryName, Verbosity, LFormat, UE_LOG_Args...) \
	    UE_LOG_EXPAND_IS_FATAL(Verbosity, \
		    { \
			    FMsg::Logf_Internal(UE_LOG_SOURCE_FILE(Filename), Line, LCategoryName.GetCategoryName(), ELogVerbosity::Verbosity, LFormat, UE_LOG_Args...); \
			    _DebugBreakAndPromptForRemote(); \
			    FDebug::ProcessFatalError(); \
		    }, \
		    { \
			    FMsg::Logf_Internal(UE_LOG_SOURCE_FILE(Filename), Line, LCategoryName.GetCategoryName(), ELogVerbosity::Verbosity, LFormat, UE_LOG_Args...); \
		    } \
	    ) \
    }; 


GENERATE_LOG_FUN(Fatal)
GENERATE_LOG_FUN(Error)
GENERATE_LOG_FUN(Warning)
GENERATE_LOG_FUN(Display)
GENERATE_LOG_FUN(Log)
GENERATE_LOG_FUN(Verbose)
GENERATE_LOG_FUN(VeryVerbose)

#define GAMEKIT_LOG(Category, Verbosity, Format, ...) Log_##Verbosity(__FILE__, __LINE__, Category, Format, __VA_ARGS__)

#define GK_WARNING(Format, ...) GAMEKIT_LOG(LogGamekit, Warning, Format, __VA_ARGS__)
#define GK_DISPLAY(Format, ...) GAMEKIT_LOG(LogGamekit, Display, Format, __VA_ARGS__)
#define GK_LOG(Format, ...)     GAMEKIT_LOG(LogGamekit, Log, Format, __VA_ARGS__)
