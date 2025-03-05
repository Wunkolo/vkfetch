#pragma once

#include <array>
#include <string_view>

namespace VendorArt
{

#define VENDOR_ART_START(x) extern std::array<std::string_view, (0

#define VENDOR_ART_LINE(x) +1

#define VENDOR_ART_END(x) )> x;

#include "VendorArt.inc"

#undef VENDOR_ART_START
#undef VENDOR_ART_LINE
#undef VENDOR_ART_END

} // namespace VendorArt