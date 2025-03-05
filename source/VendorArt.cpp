#include "VendorArt.hpp"

namespace VendorArt
{
using namespace std::string_view_literals;

#define VENDOR_ART_START(x) decltype(x) x = std::to_array<std::string_view>({

#define VENDOR_ART_LINE(x) x##sv,

#define VENDOR_ART_END(x)                                                      \
	});

#include "VendorArt.inc"

#undef VENDOR_ART_START
#undef VENDOR_ART_LINE
#undef VENDOR_ART_END
} // namespace VendorArt