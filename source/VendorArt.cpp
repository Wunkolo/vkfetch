#include "VendorArt.hpp"

namespace VendorArt
{
#define VENDOR_ART_START(x) decltype(x) x = std::to_array<const char*>({

#define VENDOR_ART_LINE(x) x,

#define VENDOR_ART_END(x)                                                      \
	});

#include "VendorArt.inc"

#undef VENDOR_ART_START
#undef VENDOR_ART_LINE
#undef VENDOR_ART_END
} // namespace VendorArt