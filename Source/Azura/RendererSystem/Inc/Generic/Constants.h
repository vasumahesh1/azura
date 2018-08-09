#pragma once

namespace Azura
{
    using Version = std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>;

    enum
    {
        SemverMajor = 0,
        SemverMinor,
        SemverPatch
    };

    constexpr Version RENDER_SYSTEM_VERSION = std::make_tuple(0U, 0U, 1U);

    constexpr std::uint32_t RENDER_SYSTEM_MAJOR_SEMVER = std::get<SemverMajor>(RENDER_SYSTEM_VERSION);
    constexpr std::uint32_t RENDER_SYSTEM_MINOR_SEMVER = std::get<SemverMinor>(RENDER_SYSTEM_VERSION);
    constexpr std::uint32_t RENDER_SYSTEM_PATCH_SEMVER = std::get<SemverPatch>(RENDER_SYSTEM_VERSION);
}