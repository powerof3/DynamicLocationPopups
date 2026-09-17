#pragma once

class Manager :
	public REX ::TSingleton<Manager>,
	public RE::BSTEventSink<RE::LocationDiscovery::Event>
{
public:
	void Register();
	void LoadSettings();
	void ResetState();

	bool ShouldMuteJingle();
	void SendLocationPopup(RE::MapMarkerData* a_mapMarkerData);

private:
	const char* GetLocationOnEntry(RE::MapMarkerData* a_mapMarkerData);

	virtual RE::BSEventNotifyControl ProcessEvent(const RE::LocationDiscovery::Event* a_event, RE::BSTEventSource<RE::LocationDiscovery::Event>* a_eventSource) override;

	// members
	static constexpr auto path = R"(Data\SKSE\Plugins\po3_DynamicLocationNamePopups.ini)"sv;

	REX::TIniSetting<std::uint32_t> mode{ "Settings", "iMode", 1 };
	REX::TIniSetting<bool>          muteJingle{ "Settings", "bMuteJingle", false };

	std::uint32_t   currentCRC{ 0 };
	std::uint32_t   lastCRC{ 0 };
	RE::MARKER_TYPE lastMarkerType{ RE::MARKER_TYPE::kNone };
	bool            showLocationPopUp{ false };
};
