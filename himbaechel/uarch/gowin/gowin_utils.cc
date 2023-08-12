#include "log.h"
#include "nextpnr.h"
#include "util.h"

#define HIMBAECHEL_CONSTIDS "uarch/gowin/constids.inc"
#include "himbaechel_constids.h"
#include "himbaechel_helpers.h"

#include "gowin.h"
#include "gowin_utils.h"

NEXTPNR_NAMESPACE_BEGIN

// pin functions: GCLKT_4, SSPI_CS, READY etc
IdStringList GowinUtils::get_pin_funcs(BelId bel)
{
    IdStringList bel_name = ctx->getBelName(bel);

    const PadInfoPOD *pins = ctx->package_info->pads.get();
    size_t len = ctx->package_info->pads.ssize();
    for (size_t i = 0; i < len; i++) {
        const PadInfoPOD *pin = &pins[i];
        if (IdString(pin->tile) == bel_name[0] && IdString(pin->bel) == bel_name[1]) {
            return IdStringList::parse(ctx, IdString(pin->pad_function).str(ctx));
        }
    }
    return IdStringList();
}

bool GowinUtils::is_simple_io_bel(BelId bel)
{
    return chip_bel_info(ctx->chip_info, bel).flags & BelFlags::FLAG_SIMPLE_IO;
}

Loc GowinUtils::get_pair_iologic_bel(Loc loc)
{
    loc.z = BelZ::IOLOGICA_Z + (1 - (loc.z - BelZ::IOLOGICA_Z));
    return loc;
}

BelId GowinUtils::get_io_bel_from_iologic(BelId bel)
{
    Loc loc = ctx->getBelLocation(bel);
    loc.z = BelZ::IOBA_Z + loc.z - BelZ::IOLOGICA_Z;
    return ctx->getBelByLocation(loc);
}

bool GowinUtils::is_diff_io_supported(IdString type)
{
    const Extra_chip_data_POD *extra = reinterpret_cast<const Extra_chip_data_POD *>(ctx->chip_info->extra_data.get());
    for (auto &dtype : extra->diff_io_types) {
        if (IdString(dtype) == type) {
            return true;
        }
    }
    return false;
}

bool GowinUtils::have_bottom_io_cnds(void)
{
    const Extra_chip_data_POD *extra = reinterpret_cast<const Extra_chip_data_POD *>(ctx->chip_info->extra_data.get());
    return extra->bottom_io.conditions.size() != 0;
}

IdString GowinUtils::get_bottom_io_wire_a_net(int8_t condition)
{
    const Extra_chip_data_POD *extra = reinterpret_cast<const Extra_chip_data_POD *>(ctx->chip_info->extra_data.get());
    return IdString(extra->bottom_io.conditions[condition].wire_a_net);
}

IdString GowinUtils::get_bottom_io_wire_b_net(int8_t condition)
{
    const Extra_chip_data_POD *extra = reinterpret_cast<const Extra_chip_data_POD *>(ctx->chip_info->extra_data.get());
    return IdString(extra->bottom_io.conditions[condition].wire_b_net);
}
NEXTPNR_NAMESPACE_END