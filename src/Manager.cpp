#include "Manager.h"

void Manager::Register()
{
	RE::LocationDiscovery::GetEventSource()->AddEventSink(this);
}

void Manager::LoadSettings()
{
	const auto store = REX::FIniSettingStore::GetSingleton();
	store->Init(path.data(), "");

	store->Load();
	store->Save();
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
	if (auto locName = GetLocationOnEntry(a_mapMarkerData); !REX::STR::IS_EMPTY(locName)) {
		if (auto hudData = static_cast<RE::HUDData*>(RE::UIMessageQueue::GetSingleton()->CreateUIMessageData(RE::InterfaceStrings::GetSingleton()->hudData))) {
			showLocationPopUp = true;
			lastMarkerType = a_mapMarkerData->type.get();

			hudData->type = RE::HUD_MESSAGE_TYPE::kLocationDiscovery;
			hudData->typeData = a_mapMarkerData->type.underlying();
			hudData->text = locName;
			RE::UIMessageQueue::GetSingleton()->AddMessage(RE::InterfaceStrings::GetSingleton()->hudMenu, RE::UI_MESSAGE_TYPE::kUpdate, hudData);
			REX::INFO("Sending location popup: {}", locName);
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
