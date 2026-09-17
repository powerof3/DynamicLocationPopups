#pragma once

namespace RE
{
	namespace HUDCRC32
	{
		static REL::Relocation<std::uint32_t*> LAST_LOCATION{ RELOCATION_ID(519577, 406124) };

#ifdef SKYRIM_SUPPORT_AE
		static REL::Relocation<std::uint32_t*> CURRENT_LOCATION{ REL::ID(406125) };
#elif !defined(SKYRIMVR)
		static REL::Relocation<std::uint32_t*> CURRENT_LOCATION{ REL::Offset(0x2F4C90C) }; // SSE
#else
		static REL::Relocation<std::uint32_t*> CURRENT_LOCATION{ REL::Offset(0x3011d34) }; // VR
#endif
	}
}
