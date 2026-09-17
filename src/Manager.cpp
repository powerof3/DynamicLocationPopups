#include "Manager.h"

void Manager::Register()
{
	RE::LocationDiscovery::GetEventSource()->AddEventSink(this);
}

void Manager::LoadSettings()
{
	constexpr auto path = L"Data/SKSE/Plugins/po3_DynamicLocationNamePopups.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, mode, "Settings", "iMode", ";Triggers location popup when:\n;0 - Entering any new location.\n;1 - Entering a location different from the current or last visited.");
	ini::get_value(ini, muteJingle, "Settings", "bMuteJingle", ";Mute discovery music jingle when entering a location.");

	(void)ini.SaveFile(path);
}

const char* Manager::GetLocationOnEntry(RE::MapMarkerData* a_mapMarkerData)
{
	if (!a_mapMarkerData) {
		return nullptr;
	}

	auto locationName = a_mapMarkerData->locationName.GetFullName();
	auto locationCRC = RE::BSCRC32<const char*>()(locationName);

	if (mode == 1) {
		if (locationCRC != lastCRC && locationCRC != currentCRC) {
			lastCRC = currentCRC;
			currentCRC = locationCRC;
			return locationName;
		}
	} else {
		if (locationCRC != currentCRC) {
			currentCRC = locationCRC;
			return locationName;
		}
	}

	return nullptr;
}

bool Manager::ShouldMuteJingle()
{
	if (showLocationPopUp) {
		showLocationPopUp = false;
		return muteJingle;
	}
	return false;
}

void Manager::SendLocationPopup(RE::MapMarkerData* a_mapMarkerData)
{
	if (auto locName = GetLocationOnEntry(a_mapMarkerData); !string::is_empty(locName)) {
		if (auto hudData = static_cast<RE::HUDData*>(RE::UIMessageQueue::GetSingleton()->CreateUIMessageData(RE::InterfaceStrings::GetSingleton()->hudData))) {
			showLocationPopUp = true;
			lastMarkerType = a_mapMarkerData->type.get();

			hudData->type = RE::HUD_MESSAGE_TYPE::kLocationDiscovery;
			hudData->typeData = a_mapMarkerData->type.underlying();
			hudData->text = locName;
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::InterfaceStrings::GetSingleton()->hudMenu, RE::UI_MESSAGE_TYPE::kUpdate, hudData);
			logger::info("Sending location popup: {}", locName);
		}
	}
}

RE::BSEventNotifyControl Manager::ProcessEvent(const RE::LocationDiscovery::Event* a_event, RE::BSTEventSource<RE::LocationDiscovery::Event>*)
{
	if (a_event) {
		GetLocationOnEntry(a_event->mapMarkerData);  // so we don't trigger two popups if the player discovers a location and then moves back into map marker range
	}

	return RE::BSEventNotifyControl::kContinue;
}
