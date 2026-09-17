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

void Manager::ResetState()
{
	currentCRC = 0;
	lastCRC = 0;
}

const char* Manager::GetLocationOnEntry(RE::MapMarkerData* a_mapMarkerData)
{
	if (!a_mapMarkerData) {
		return nullptr;
	}

	const auto locationName = a_mapMarkerData->locationName.GetFullName();
	if (REX::STR::IS_EMPTY(locationName)) {
		return nullptr;
	}

	const auto locationCRC = RE::BSCRC32<std::string_view>()(locationName);

	if (locationCRC == currentCRC) {
		return nullptr;
	}

	if (mode == 1 && locationCRC == lastCRC) {
		std::swap(currentCRC, lastCRC); 
		return nullptr;
	}

	lastCRC = currentCRC;
	currentCRC = locationCRC;
	return locationName;
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
