#pragma once

#include <vector>
#include <string>

class DisplayInformation {
public:
	std::string name;
	std::string desc;
};


class DisplayProvider {
public:

	std::string name;
	std::string desc;

	std::vector<DisplayInformation> displayInformations;
};

class DisplayInformationManager {
public:
	std::vector<DisplayProvider> getDisplayProviders() { return displayProviders; }
	DisplayProvider getDisplayProvider(int index) { return displayProviders[index]; }
	void addDisplayProvers(DisplayProvider displayProvider) { displayProviders.push_back(displayProvider); }

	void setDisplayProviders(std::vector<DisplayProvider> value) { displayProviders = value; }
	void setDisplayInformations(int index, std::vector<DisplayInformation> value) { displayProviders[index].displayInformations = std::vector<DisplayInformation>(); }
	
private:
	std::vector<DisplayProvider> displayProviders;
};