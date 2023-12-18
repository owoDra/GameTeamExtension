// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GTEXT_API DECLARE_LOG_CATEGORY_EXTERN(LogGTE, Log, All);

#if !UE_BUILD_SHIPPING

#define GTELOG(FormattedText, ...) UE_LOG(LogGTE, Log, FormattedText, __VA_ARGS__)

#define GTEENSURE(InExpression) ensure(InExpression)
#define GTEENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GTEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GTECHECK(InExpression) check(InExpression)
#define GTECHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GTELOG(FormattedText, ...)

#define GTEENSURE(InExpression) InExpression
#define GTEENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GTEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GTECHECK(InExpression) InExpression
#define GTECHECK_MSG(InExpression, InFormat, ...) InExpression

#endif