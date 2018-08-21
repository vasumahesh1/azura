#pragma once

#include <tuple>
#include "Types.h"

namespace Azura
{
    using Version = std::tuple<U32, U32, U32>;

    enum
    {
        SemverMajor = 0,
        SemverMinor,
        SemverPatch
    };

    const Version RENDER_SYSTEM_VERSION = std::make_tuple(0U, 0U, 1U);

    const U32 RENDER_SYSTEM_MAJOR_SEMVER = std::get<SemverMajor>(RENDER_SYSTEM_VERSION);
    const U32 RENDER_SYSTEM_MINOR_SEMVER = std::get<SemverMinor>(RENDER_SYSTEM_VERSION);
    const U32 RENDER_SYSTEM_PATCH_SEMVER = std::get<SemverPatch>(RENDER_SYSTEM_VERSION);
} // namespace Azura