// Unknown must be the first entry so that it can be marked
// as `default` for switch statements
VENDOR(0x0000'FFFF, Unknown         )
VENDOR(0x0000'1002, AMD             )
VENDOR(0x0000'1010, ImgTec          )
VENDOR(0x0000'106B, Apple           )
VENDOR(0x0000'10DE, Nvidia          )
VENDOR(0x0000'13B5, ARM             )
VENDOR(0x0000'1414, Microsoft       )
VENDOR(0x0000'1AE0, Google          )
VENDOR(0x0000'5143, Qualcomm        )
VENDOR(0x0000'8086, Intel           )
VENDOR(0x0001'0001, VIV             )
VENDOR(0x0001'0002, VSI             )
VENDOR(0x0001'0003, Kazan           )
VENDOR(0x0001'0004, Codeplay        )
VENDOR(0x0001'0005, Mesa            )
VENDOR(0x0001'0006, Pocl            )
VENDOR(0x0001'0007, MobileEye       )

// VendorID seemingly given to partner-configured Qualcomm devices such as Windows on ARM laptops
VENDOR(0x4d4f'4351, QualcommPartner )