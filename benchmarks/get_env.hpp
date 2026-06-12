// SPDX-FileCopyrightText: Steven Ward
// SPDX-License-Identifier: MPL-2.0

/// Wrapper for \c std::getenv
/**
* \file
* \author Steven Ward
*/

#pragma once

#include <cstdlib>
#include <optional>
#include <string>

/// Wrapper for \c std::getenv
/**
* Examples:
* \code{.cpp}
const char* name = "FOO";
if (const auto value = get_env(name))
    std::println("{} = {}", name, *value);
else
    std::println("{} is not set", name);
\endcode
*
* \code{.cpp}
std::println("{} = {}", name, get_env("SHELL").value_or("(none)"));
std::println("{} = {}", name, get_env("FOO").value_or("(none)"));
\endcode
*/
[[nodiscard]] std::optional<std::string>
get_env(const char* name) noexcept
{
    const char* const value = std::getenv(name);
    if (value == nullptr)
        return std::nullopt;
    return std::string{value};
}
